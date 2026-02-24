
#include "wait.h"

wait_t *wait_create() {

  /* allocate memory on the heap */
  wait_t *create = NULL;
  create = (wait_t *)malloc(sizeof(wait_t));

  /* wire up function pointers */
  create->destroy = &wait_destroy;

  /* return new wait */
  return create;
}

void wait_destroy(wait_t **self) {
  /* create a temp storing the address inside of self */
  wait_t *trash = *self; 

  /* free this memory */
  free(trash);

  /* don't forget to NULL the pointer */
  trash = NULL;
}
