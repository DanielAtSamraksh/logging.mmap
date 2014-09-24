#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <stdio.h>    /* perror, printf, sprintf */
#include <string.h>   /* strlen */
#include "logfile.h"

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

