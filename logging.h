#ifndef LOGGING_H_
#define LOGGING_H_

#include <sys/types.h>
#include <sys/mman.h>
#include <string>
#include <map>
#include <stdio.h>

#define PATH_SIZE 500

using std::string;
using std::map;


class logFile_t { 
 public:

  /// Construct without a filename. The filename must be set before use.
  logFile_t ();

  /// Initialize the filename.
  void init ( string filename );

  /// Construct without a filename. Object is ready to use.
  logFile_t ( string filename );

  /// write buffer to the logfile, no need to open first
  int write ( const char* msg, unsigned len );

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

typedef map< string , logFile_t > logFiles_t;

class log_t {
 public:
  log_t ();
  log_t ( unsigned nodes,
	  const char*  motionModel,
	  unsigned seconds,
	  double metersPerSecond, // should be average link life
	  double nodesPerMeterSquared
	  );
  ~log_t ();
  int open ( const char* name, unsigned bytes ); 
  int write ( const char* name, const char* buf, unsigned bytes );
  int close ( const char* name);
  int close ( string name);
 private: 
  char path[PATH_SIZE];
  char charBuffer[PATH_SIZE];
  logFiles_t logFiles; 
};

#endif /* LOGGING_H_ */
