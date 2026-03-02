#include "wait.h"

void wait_help_me() {
  printf("Nothing here yet :]\n");
}

wait_t *wait_create() {
  /* create a new wait api */
  wait_t *temp = NULL;

  /* [!] allocate memory */
  temp = (wait_t *)malloc(sizeof(wait_t));

  /* wire up psuedo-methods */
  temp->help_me = wait_help_me;
  temp->destroy = wait_destroy;

  /* return the new wait api */
  return temp;
}

void wait_destroy(wait_t **trash_ptr) {
  wait_t *trash = *trash_ptr;   /* needed for the 3rd step :] */
  free(trash);                  /* trash the memory block where wait api is stored */
  *trash_ptr = NULL;            /* clean up the pass-by-reference pointer */
}
