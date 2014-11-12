#ifndef MKPATH_H_
#define MKPATH_H_

int mkpath(const char *path);
int mkpathto(const char *path);
// int mkpath(const char *path, mode_t mode);

#endif // MKPATH_H_
#ifndef MK_PATH_CPP
#define MK_PATH_CPP

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int do_mkpath(const char *path, bool to) {
  // printf("making %s\n", path);
  char *pathcopy = strdup(path);
  if (!pathcopy) {
    printf ("strdup error\n");
    return 1;
  }
  else {
    // printf("successfully copied %s\n", pathcopy);
  }
  int retval = 0;
  int len = strlen(path);
  // printf("len is %d\n", len);
  char *last = pathcopy;
  
// build the path component-wise, starting at the beginning, zeroing the last segment.
  do {
    // printf("in while loop, pathcopy = %ld, last = %ld\n", (long) pathcopy, (long) last); 
    // last = strchrnul(last+1, '/');
    last = strchr(last+1, '/');
    if (last) *last = '\0';
    else if (to) { // stop here
      break;
    }
    else last = pathcopy + len;
    // printf("working on %s\n", pathcopy);

    struct stat st;
    if (stat(pathcopy, &st) == 0) { // path exists
      printf("path %s exists\n", pathcopy);
      if (! S_ISDIR(st.st_mode)) {
	printf ("mkpath: error: %s exists and is not a directory!\n", pathcopy);
	retval = 1;
	break;
      }
    }
    else if (mkdir(pathcopy, 0777)) {
      printf ("mkpath: error: cannot create %s!\n", pathcopy); 
      retval = 1;
      break;
    }
    *last = '/'; // continue
    // printf("Changing back\n");
  } while (last < pathcopy + len - 1);

  free (pathcopy);
  return retval;
}      

int mkpath(const char *path) { return do_mkpath(path, false); }
int mkpathto(const char *path) { return do_mkpath(path, true); }

#ifdef TEST_MKPATH


int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s directory_name\n", argv[0]);
    exit (1);
  }
  mkpath(argv[1]);
  return 0;
}

#endif /* TEST */
#endif //  MK_PATH_CPP
#ifndef LOGFILE_H_
#define LOGFILE_H_

#include <sys/types.h>
#include <sys/mman.h>
#include <string>
#include <stdio.h>


using std::string;

/// logFile_t ( "path/to/logfile" ):  create a logfile
/// write ( void *buf, unsigned bytes ): write the buffer to the logfile

class logFile_t { 
 public:

  /// Construct without a filename. The filename must be set before use.
  logFile_t ();

  /// Initialize the filename.
  void init ( string filename );

  /// Construct without a filename. Object is ready to use.
  logFile_t ( string filename );

  /// write buffer to the logfile, no need to open first, return
  /// number of bytes written or negative on error.
  int write ( const void* msg, unsigned len );

  ~logFile_t ();

  /// Manually unmaps, syncs and closes, leaving the log in a clean state.
  int close ();

  /// Manually flush. Make sure the log is written to disk.
  int flush ();

 private:

  string filename;              /// name of the logfile

  int fd;

  off_t bytes_written_so_far;	/** total bytes written so far */
  char* mmap_start_addr;	/** address of mmaped region */
  off_t mmap_size;		/** the size of the mmapped region */
  off_t mmap_offset;		/** offset in the file where mmapped
				    region begins */

  int mmap ( off_t len );
  int munmap ();
  int open ();

};

#endif /* LOGFILE_H_ */
#ifndef LOGFILE_CPP
#define LOGFILE_CPP

#include <cstdlib>    /* exit */
#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <cstring>    /* memcpy */
#include <stdio.h>    /* perror, printf, sprintf */

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
// int logFile_t::write(const char* msg, unsigned len) {
int logFile_t::write(const void* msg, unsigned len) {
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

/// Return 0 on success. This allows multiple error codes. There are many ways to screw up.
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

  if ( pa_len &&
       lseek (this->fd, pa_offset + pa_len - 1, SEEK_SET) == -1) {
    printf("logFile_t::mmap: lseek error");
    return -1;
  }
  if (::write (fd, "", 1) != 1) {
    printf("logFile_t::mmap: write error");
    return -1;
  }
  this->mmap_start_addr = (char*) ::mmap (
    0, pa_len,
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

#ifdef TEST_LOGFILE

#include <iostream>
using std::getline;
using std::cout;
using std::cin;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s directory_name < inputfile \n", argv[0]);
    exit (1);
  }

  logFile_t logfile ( argv[1] );

  for ( string line; getline( cin, line ); ) {
    line += "\n"; // newline gets stripped with getline
    logfile.write ( line.c_str(), line.size() );
  }
  
  return 0;
}


#endif // TEST_LOGFILE

#endif // LOGFILE_CPP

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <stdio.h>    /* perror, printf, sprintf */
#include <cstdlib>    /* exit */


using std::string;

class logger_base_t {
 public:
  logger_base_t () {};
  logger_base_t ( string path ) { init (path); };
  void init ( string path ) {
    this->log.init(path); 
    this->logfile_name = path;
  };
  void check () {
    if ( 0 == this->logfile_name.size() ) {
      printf ( "logger_base_t::write: Error!"
	       "No logfile. "
	       "Set it in the constructor or with init.\n" );
      exit ( 1 );
    }
  };

 protected:
  string logfile_name;
  logFile_t log;
};

/// Logs a struct or a class. Every call to log expects a struct.
template < class S >
struct struct_logger_t : logger_base_t {
  struct_logger_t (){};
  struct_logger_t ( string path ){
    this->init( path );
  };
  int operator() ( const S *s ) {
    return this->operator() ( *s );
  };
  int operator() ( const S &s ) {
    this->check();
    return this->log.write ( &s, sizeof (s) );
  };
};

/// Logs a string.
struct logger_t : logger_base_t {
  logger_t (){};
  logger_t ( string path ){
    this->init( path );
  };
  int operator() ( const string s ) {
    return this->log.write ( s.c_str(), s.size() );
  };
};

/// Logs a struct, but also logs the struct representation, stored in the struct's repr method.
template < class S >
struct struct_logger_repr_t : logger_base_t {
  struct_logger_repr_t (){};
  struct_logger_repr_t ( string path ){
    this->init( path );
  };
  void init ( string path ) {
    logger_t repr ( path + ".repr" );
    repr ( S::repr() ); // class must have a repr method
    this->log.init(path); 
    this->logfile_name = path;
  };
  int operator() ( const S *s ) {
    return this->operator() ( *s );
  };
  int operator() ( const S &s ) {
    this->check();
    return this->log.write ( &s, sizeof (s) );
  };
};

#endif /* LOGGER_H_ */
