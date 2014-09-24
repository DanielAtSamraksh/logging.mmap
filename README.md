logging.mmap
============

C++ module for logging.

This allows easy, elegant logging of binary formats. 

To run the tests, type `make`. 

Example use:

   
     // example of how to use logfile and logger
     
     #include "log.h"
     
     // struct record_t {
     class record_t {
      public:
       int i, j;
     };
     
     
     int main(int argc, char **argv) {
     
       if (argc != 2) {
         printf("usage: %s testfile-name\n", argv[0]);
         return -1;
       }
       string fname = argv[1];
     
     
       // create a record
       record_t r;
       r.i = 1; r.j = 2;
     
       // write a buffer to the plain logfile.
       logFile_t plainLogger ( fname + ".plain" ); // alternative declaration
       logFile_t plainLogger;
       plainLogger.init(fname + ".plain");
       plainLogger.write(&r, sizeof(r));
     
       // write a record to the fancy logfile.
       // logger_t < class record_t > fancyLogger(fname + ".fancy"); // alternative declaration
       logger_t < class record_t > fancyLogger;
       fancyLogger.init(fname + ".fancy");
       fancyLogger(r);
     
     }

  
