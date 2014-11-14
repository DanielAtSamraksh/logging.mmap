#ifndef MK_PATH_CPP
#define MK_PATH_CPP

#include "mkpath.h"

#ifdef TEST_MKPATH



int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s directory_name\n", argv[0]);
    exit (1);
  }
  mkpath(argv[1]);
  return 0;
}

#endif /* TEST */
#endif //  MK_PATH_CPP
