#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <stdio.h>    /* perror, printf, sprintf */

#include "logfile.h"
#include "logger.h"

template < class S > logger_t<S>::logger_t () {}
template < class S > logger_t<S>::logger_t ( string p ) { init (p); }
template < class S > void logger_t<S>::init ( string p ) { this->log.init(p); }
template < class S > int logger_t<S>::operator() ( const S* s ) { 
  return this->log.write( (const char *) s, sizeof(s) );
}

#ifdef TEST_LOGGER

/* Test this by running: 
 > g++ -DTEST_LOGGER -I. mkpath.cpp logfile.cpp logger.cpp && ./a.out

*/

struct my_struct {
  char s[100];
};

int main(int argc, char** argv){
 
  if (argc != 2) {
    printf("usage: %s testfile-name\n", argv[0]);
    return -1;
  }
  
  string fname = argv[1];

  my_struct s;
  int n = 1000000;
  logger_t <my_struct> log(fname+".mmap"); 
  int fd = open( (fname+".write").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
  for (int i = 0; i< n; i++) {
    sprintf((char*) &(s.s), "%-99d\n", i);
    if ( log ( &s ) ) {
      return 1;
    }
    write ( fd, (const void*) &s, sizeof(s) );
  }
  close(fd);
}

#endif
