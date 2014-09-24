#ifndef LOGGER_H_
#define LOGGER_H_

#include "logfile.h"

using std::string;

template < class S >
class logger_t {
 public:
  logger_t ();
  logger_t ( string _path );
  void init ( string _path );
  int operator() ( const S* s ); 
 private:
  logFile_t log;
};

#endif /* LOGGER_H_ */
