#ifndef LOGFILE_H_
#define LOGFILE_H_

#include <sys/types.h>
#include <sys/mman.h>
#include <string>
#include <stdio.h>

#define PATH_SIZE 500

using std::string;


class logFile_t { 
 public:

  /// Construct without a filename. The filename must be set before use.
  logFile_t ();

  /// Initialize the filename.
  void init ( string filename );

  /// Construct without a filename. Object is ready to use.
  logFile_t ( string filename );

  /// write buffer to the logfile, no need to open first
  /* int write ( const char* msg, unsigned len ); */
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
