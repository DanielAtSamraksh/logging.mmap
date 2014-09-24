#ifndef LOGGER_H_
#define LOGGER_H_

#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <stdio.h>    /* perror, printf, sprintf */

#include "logfile.h"

using std::string;

template < class S >
class logger_t {
 public:
  logger_t () {};
  logger_t ( string path ) { init (path); };
  void init ( string path ) {
    this->log.init(path); 
    this->logfile_name = path;
  };
  int operator() ( const S &s ) {
    if ( this->logfile_name.size() ) {
      return this->log.write( &s, sizeof(s) );
      // we want size of s, not S, in case the size of the object is variable.
    }
    return 0; // not initialized, fail silently.
  };

 private:
  string logfile_name;
  logFile_t log;
};

#endif /* LOGGER_H_ */
