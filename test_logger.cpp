#include "logger.h"


/// logger logs structures. Define the structures here.

struct my_string_t {
  // int i;
  char s[10];
};

struct my_ints_t {
  int i, j;
};

struct my_repr_t {
  int i;
  char s[10];
  static string repr() {
    return "struct my_repr_t {\n"
      "  int i;\n"
      "  char s[10];\n";
  };
};


int main(int argc, char** argv){
 
  if (argc != 2) {
    printf("usage: %s testfile-name\n", argv[0]);
    return -1;
  }
  
  string fname = argv[1];


  int n = 10;

  // log in parallel: to logstring via the logger class being tested,
  // and to fdstring using regular writes.
  struct_logger_t <my_string_t> logstring( fname+".string.mmap" ); 
  int fdstring = open( ( fname+".string.write" ).c_str(),
		       O_RDWR | O_CREAT | O_TRUNC, 0666);
  my_string_t s;

  for (int i = 0; i< n; i++) {

    snprintf((char*) &(s.s), 10, "%8d\n", i);
    // log via logger class
    if ( logstring ( s ) ) {
      return 1;
    }
    // log via write
    // write ( fd, (const void*) &s, sizeof(s) ); 
    write ( fdstring, &s, sizeof(s) );
  }
  close(fdstring);

  
  // log in parallel: to logints via logger class and to fdints via writes.
  struct_logger_t <my_ints_t> logints(fname+".ints.mmap"); 
  int fdints = open( (fname+".ints.write").c_str(),
		     O_RDWR | O_CREAT | O_TRUNC, 0666);
  my_ints_t ii;
  for (int i = 0; i< n; i++) {
    ii.i = ii.j = i;
    // log via logger class
    if ( logints ( ii ) ) {
      return 1;
    }
    // log via write
    // write ( fd, (const void*) &s, sizeof(s) ); 
    write ( fdints, &ii, sizeof(ii) );
  }
  close(fdints);


  // log in parallel: to logrepr via logger class and to fdrepr via writes
  struct_logger_repr_t <my_repr_t> logrepr ( fname+".repr.mmap" );
  int fdrepr = open ( (fname+".repr.write").c_str(),
		      O_RDWR | O_CREAT | O_TRUNC, 0666);
  my_repr_t repr;
  
  for (int i = 0; i< n; i++) {
    repr.i = i;
    snprintf((char*) &(repr.s), 10, "%8d\n", i);
    // log via logger class
    if ( logrepr ( repr ) ) {
      return 1;
    }
    // log via write
    // write ( fd, (const void*) &s, sizeof(s) ); 
    write ( fdrepr, &repr, sizeof(repr) );
  }
  close(fdrepr);

}

