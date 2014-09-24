#include "logger.h"

/* Test this by running: 
 > g++ -DTEST_LOGGER -I. mkpath.cpp logfile.cpp logger.cpp && ./a.out

*/

struct my_string {
  // int i;
  char s[10];
};

struct my_ints {
  int i, j;
};

int main(int argc, char** argv){
 
  if (argc != 2) {
    printf("usage: %s testfile-name\n", argv[0]);
    return -1;
  }
  
  string fname = argv[1];

  my_string s;
  my_ints ii;

  int n = 10;

  logger_t <my_string> logstring(fname+".string.mmap"); 
  int fdstring = open( (fname+".string.write").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);

  logger_t <my_ints> logints(fname+".ints.mmap"); 
  int fdints = open( (fname+".ints.write").c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);

  for (int i = 0; i< n; i++) {
    snprintf((char*) &(s.s), 10, "%8d\n", i);
    if ( logstring ( s ) ) {
      return 1;
    }
    // write ( fd, (const void*) &s, sizeof(s) ); 
    write ( fdstring, &s, sizeof(s) );

    ii.i = ii.j = i;
    if ( logints ( ii ) ) {
      return 1;
    }
    // write ( fd, (const void*) &s, sizeof(s) ); 
    write ( fdints, &ii, sizeof(ii) );

    
  }
  close(fdstring);
  close(fdints);
}

