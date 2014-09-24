#include "logfile.h"
#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <cstring>    /* memcpy */
#include <stdio.h>    /* perror, printf, sprintf */
#include "mkpath.h"

// #define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) do { perror(msg); } while (0)


logFile_t::logFile_t () {
  this->init("");
}
logFile_t::logFile_t ( string fname ) {
  this->init(fname);
}
void logFile_t::init ( string fname ) {
  this->filename = fname;
  this->fd = -1;
  this->mmap_start_addr = 0;
  this->mmap_size = 0;
  this->mmap_offset = 0;
  this->bytes_written_so_far = 0;
} 

logFile_t::~logFile_t () {
  this->close();
}  

/// Return 0 on success, -1 on failure.
int logFile_t::open ( ) {
  if ( this->filename.size() == 0 ) {
    printf("logFile_t::open: no filename\n");
    return -1;
  }
  if ( this->fd >= 0 ) {
    printf("logFile_t::open: file %s is already open.\n", 
	   this->filename.c_str());
    return 0;
  }
  if ( mkpathto ( this->filename.c_str() ) ) {
    printf("logFile_t::open: error in mkpathto.\n");
    return -1;
  }
  this->fd = ::open( this->filename.c_str() ,  
		     O_RDWR | O_CREAT, 0666);
  if (this->fd < 0) {
    printf("logFile_t::open: error opening %s.\n", this->filename.c_str() );
    return -1;
  }
  return 0;
}

int logFile_t::flush () {
  if ( this->fd >= 0 && ::fsync ( this->fd )) {
    printf("logFile_t::close: error fsyncing file %s.\n", 
	   this->filename.c_str() );
    handle_error("fsync");
    return -1;
  }
  return 0;
}

/// also munmaps.
int logFile_t:: close () {
  if ( this->munmap() ) {
    printf( "logFile_t::close: munmap error.\n" );
    return -1;
  }
  
  if ( this->fd < 0 ) {
    // printf("logFile_t::close: file %s is already closed.\n", 
    // 	   filename.c_str());
    return 0;
  }

  //  printf("truncating\n");
  if ( ::ftruncate ( this->fd, this->bytes_written_so_far )) {
    printf("logFile_t::close: error truncating file %s to %ld.\n", 
	   this->filename.c_str(), this->bytes_written_so_far );
    return -1;
  }

  // I am not sure if this is needed, but it doesn't hurt.
  this->flush();

  if ( ::close ( this->fd ) ) {
    printf("logFile_t::close: error closing file %s after %ld bytes written.\n", 
	   filename.c_str(), this->bytes_written_so_far );
    handle_error("close");
    return -1;
  }
  this->fd = -1;
  return 0;
}
    
/// Return number of bytes written or negative on error.
/// Automatically opens and mmaps if needed.
int logFile_t::write(const char* msg, unsigned len) {
  if ( this->mmap(len) ) { 
    printf("logFile_t::write: (line no %d) mmap error.\n", __LINE__);
    return -1;
  }
  char *start = this->mmap_start_addr + this->bytes_written_so_far - this->mmap_offset; 
  if (! memcpy(start, msg, len)) {
    printf("logFile_t::write: memcpy error.\n");
    return -1;
  }
  this->bytes_written_so_far += len;
  return 0;
};

/// Return 0 on success.
int logFile_t::munmap() {
  if ( this->mmap_size >= 0 
       && ::munmap ( this->mmap_start_addr, this->mmap_size )) {
      printf( "logFile_t::munmap: Error munmapping\n");
      printf( "logFile_t::munmap: Bytes written so far = %ld \n",
	      this->bytes_written_so_far );
      handle_error("munmap");
      return -1;
  }
  return 0;
}

/// Return 0 on success.
/// Automatically open, close if needed. 
int logFile_t::mmap(off_t len) {
  if (this->bytes_written_so_far 
      + len < this->mmap_offset + this->mmap_size ) { 
    // there is enough room
    return 0;
  }
  // printf( "logFile_t::mmap(%ld): mmapping after writing %ld bytes\n", 
  // 	  len, this->bytes_written_so_far );

  if ( this->mmap_start_addr ) { // need to close file and munmap
    if ( this->close() ) {
      printf("logFile_t::mmap: close error");
      handle_error("mmap");
      return -1;
    }
  }

  if ( this->open() ) {
    printf("logFile_t::mmap: open error");
    handle_error("open");
    return -1;
  }
  
  off_t page_size = sysconf(_SC_PAGE_SIZE);
  // po_ means page-aligned
  off_t pa_offset = this->bytes_written_so_far & ~(page_size - 1);
  off_t extra_length = this->bytes_written_so_far - pa_offset;
  off_t pa_len = (((extra_length + len) / page_size) + 1) * page_size;

  if ( pa_len && lseek (this->fd, pa_offset + pa_len - 1, SEEK_SET) == -1) {
    printf("logFile_t::mmap: lseek error");
    return -1;
  }
  if (::write (fd, "", 1) != 1) {
    printf("logFile_t::mmap: write error");
    return -1;
  }
  this->mmap_start_addr = (char*) ::mmap (0, pa_len,
					  PROT_READ | PROT_WRITE, MAP_SHARED, 
					  fd, pa_offset);
  if ( this->mmap_start_addr == (caddr_t) -1 ) { 
    printf("logFile_t::mmap: mmap error");
    return 1;
  }
  this->mmap_offset = pa_offset;
  this->mmap_size = pa_len;
  return 0;
}


#ifdef TEST_LOGGING

/* Test this by running: 
 > g++ -DTEST_LOGGING -I${TUS} -I. mkpath.cpp logging.cpp && ./a.out

*/

int main(int argc, char** argv){
  if (argc != 2) {
    printf("usage: %s testfile-name\n", argv[0]);
    return -1;
  }
  string fname = argv[1];

  int n = 1000000;
  char buff[100];
  logFile_t log(fname+".mmap"); 
  int fd = open( (fname+".write").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
  for (int i = 0; i< n; i++) {
    sprintf(buff, "%d\n", i);
    if (log.write ( buff, strlen(buff) )){
      return 1;
    }
    write ( fd, buff, strlen(buff) );
  }
  log.close();
  close(fd);
}

#endif
