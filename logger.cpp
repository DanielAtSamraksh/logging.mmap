
#include "logger.h"

#ifdef TEST_LOGGER

#include <iostream>
using std::getline;
using std::cout;
using std::cin;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s directory_name < inputfile \n", argv[0]);
    exit (1);
  }

  logger_t logger ( argv[1] );

  for ( string line; getline( cin, line ); ) {
    line += "\n"; // newline gets stripped with getline
    logger ( line ); 
  }
  
  return 0;
}


#endif // TEST_LOGGER


