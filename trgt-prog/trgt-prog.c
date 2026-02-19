/* this is the target program the debugger will be targetting, just as a proof of concept first before targetting other more advanced programs */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main () {
  int test_int = 0;
  uint8_t test_uint8 = 97;
  char *hello_world = "Hello World!";

  printf("\n\n=== Target Program ===\n\n");
  printf("test_int\t%d\n", test_int);
  printf("test_uint8\t%d\n, test_uint8");

  sleep(1);
  printf("%s\n", hello_world);

  return EXIT_SUCCESS;
}
