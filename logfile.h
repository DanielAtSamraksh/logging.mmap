#ifndef LOGFILE_H_
#define LOGFILE_H_

#include <cstdlib>    /* exit */
#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <cstring>    /* memcpy */
#include <stdio.h>    /* perror, printf, sprintf */
#include "mkpath.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <string>


// #define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) do { perror(msg); } while (0)

using std::string;

/// logFile_t ( "path/to/logfile" ):  create a logfile
/// write ( void *buf, unsigned bytes ): write the buffer to the logfile

class logFile_t { 
 public:

  /// Construct without a filename. The filename must be set before use.
  logFile_t () {
    this->init("");
  };

  /// Initialize the filename.
  void init ( string fname ) {
    this->filename = fname;
    this->fd = -1;
    this->mmap_start_addr = 0;
    this->mmap_size = 0;
    this->mmap_offset = 0;
    this->bytes_written_so_far = 0;
    this->page_size = sysconf(_SC_PAGE_SIZE);

  };

  /// Construct without a filename. Object is ready to use.
  logFile_t ( string fname ) {
    this->init(fname);
  };

  /// write buffer to the logfile, no need to open first, return
  /// number of bytes written or negative on error.
  int write ( const void* msg, unsigned len ) {
    // Return number of bytes written or negative on error.
    // Automatically opens and mmaps if needed.
    if ( this->mmap(len) ) { 
      printf("logFile_t::write: (line no %d) mmap error.\n", __LINE__);
      return -1;
    }
    char *start = this->mmap_start_addr + this->bytes_written_so_far - this->mmap_offset; 
    if (! ::memcpy(start, msg, len)) {
      printf("logFile_t::write: memcpy error.\n");
      return -1;
    }
    this->bytes_written_so_far += len;
    return 0;
  };

  ~logFile_t ()  {
    this->close();
  };

  /// Manually unmaps, syncs and closes, leaving the log in a clean state.
  int close ()  {
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
  };

  /// Manually flush. Make sure the log is written to disk.
  int flush () {
    if ( this->fd >= 0 && ::fsync ( this->fd )) {
      printf("logFile_t::close: error fsyncing file %s.\n", 
	     this->filename.c_str() );
      handle_error("fsync");
      return -1;
    }
    return 0;
  };

 private:

  string filename;              /// name of the logfile

  int fd;

  /** all sizes in bytes. */
  off_t bytes_written_so_far;	/** total bytes written so far */
  char* mmap_start_addr;	/** address of mmaped region */
  off_t mmap_size;		/** the size of the mmapped region */
  off_t mmap_offset;		/** offset in the file where mmapped
				    region begins */
  off_t page_size;              /** page size */ 

  int mmap ( off_t len )  {
    if (this->bytes_written_so_far + len
	< this->mmap_offset + this->mmap_size ) { 
      return 0;      // there is enough room
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
  
    off_t start_page = this->bytes_written_so_far / this->page_size;
    off_t stop_page = ( this->bytes_written_so_far + len - 1 ) / this->page_size + 1;
    if ( start_page >= stop_page ) { printf( "Page calculation is wrong." ); exit (1); }
    this->mmap_size = ( stop_page - start_page ) * this->page_size;
    this->mmap_offset = start_page * this->page_size;
    if ( lseek (this->fd, stop_page * this->page_size - 1, SEEK_SET) < 0 ) {
      printf("logFile_t::mmap: lseek error");
      return -1;
    }
    if (::write (fd, "", 1) != 1) {
      printf("logFile_t::mmap: write error at end of file");
      return -1;
    }
    this->mmap_start_addr = (char*) ::mmap (
      0, this->mmap_size,
      PROT_READ | PROT_WRITE, MAP_SHARED, 
      fd, this->mmap_offset);
    if ( this->mmap_start_addr == (caddr_t) -1 ) { 
      printf("logFile_t::mmap: mmap error");
      return 1;
    }
    return 0;
  };

  int munmap () {
    if ( this->mmap_size >= 0 
	 && ::munmap ( this->mmap_start_addr, this->mmap_size )) {
      printf( "logFile_t::munmap: Error munmapping\n");
      printf( "logFile_t::munmap: Bytes written so far = %ld \n",
	      this->bytes_written_so_far );
      handle_error("munmap");
      return -1;
    }
    return 0;
  };
  
  int open () {
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
  };

};

#endif /* LOGFILE_H_ */
