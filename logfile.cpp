#ifndef LOGFILE_CPP
#define LOGFILE_CPP

#include "logfile.h"
#include <cstdlib>    /* exit */
#include <fcntl.h>    /* O_RDWR */
#include <unistd.h>   /* open(), creat(), ftruncate, fsync */
#include <cstring>    /* memcpy */
#include <stdio.h>    /* perror, printf, sprintf */
#include "mkpath.h"



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

