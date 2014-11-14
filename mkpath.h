#ifndef MKPATH_H_
#define MKPATH_H_

#include <sys/stat.h> 
#include <string.h> // strdup, strlen, strcat
#include <stdlib.h> // malloc
#include <unistd.h>
#include <stdio.h>

/// create directories to the final /. Return 0 for success.
inline int mkpathto ( const char *path ) {
  // copy the path so that we can modify it.
  char *p = strdup ( path );
  if ( ! p ) {
    printf ("strdup error\n");
    return 1;
  }
  int return_value = 0;
  // Move to the end of the path creating directories as needed. 
  for ( char *slash = strchr ( p, '/' ); slash != NULL; slash = strchr ( slash + 1, '/' )) {
    *slash = '\0'; // temporarily end the string at the slash
    struct stat st;
    if ( 0 == stat ( p, &st ) ) {
      if (! S_ISDIR( st.st_mode )) {
	printf ( "mkpath error: %s is not a dir.\n", p );
	return_value = 1;
	break;
      }
    }
    else if ( mkdir ( p, 0777 )) {
      printf ( "mkpath error: cannot create dir %s.\n", p );
      return_value = 1;
      break;
    }
    *slash = '/'; // replace the slash
  }
  free ( p );
  return return_value;
};

/// Create directories up to the last element, even if it doesn't end in a slash.
inline int mkpath ( const char *path ) {
  int len = strlen ( path );
  if ( path[len-1] == '/' ) {
    return mkpathto ( path );
  }
  char *p = (char*) malloc ( strlen ( path ) + 2 );
  strcpy ( p, path );
  strcat ( p, "/");
  int return_value = mkpathto ( p );
  free ( p );
  return return_value;
}
  
#endif // MKPATH_H_
