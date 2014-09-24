#include <iostream> /* cout */
#include <stdio.h>  /* printf, sprintf */
#include <string.h> /* strlen */

using std::cout;

struct C {
  void method () {
    cout << "__func__ " <<  __func__ << "\n";
    cout << "__FUNCTION__ " <<  __FUNCTION__ << "\n";
    cout << "__PRETTY_FUNCTION__ " <<  __PRETTY_FUNCTION__ << "\n";
  }

};

int main(int argc, char** argv) {
  C obj;
  obj.method();

  char buf[100];
  for ( int i= 0; i < 20000; i+= i+1 ) {
    sprintf(buf, "%10d\n", i);
    printf("%s size: %lu\n", buf, strlen(buf));
  }

}
