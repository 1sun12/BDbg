
#include <stdio.h>
#include <stdlib.h>

/**
 * test_ptrace()
 * @brief playing with ptrace, see `man 2 ptrace`
 */
void test_ptrace() {
  // code
}

int main () {
  printf("Hello world!\n");

  int *ptr = NULL;

  int number = 5;

  ptr = &number;

  printf("Number: %d\n", number);
  printf("Address: 0x%x\n", ptr);

  return EXIT_SUCCESS;
}
