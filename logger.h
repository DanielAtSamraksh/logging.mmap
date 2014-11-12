#ifndef LOGGER_H_
#define LOGGER_H_

#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <stdio.h>    /* perror, printf, sprintf */
#include <cstdlib>    /* exit */

#include "logfile.h"

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
