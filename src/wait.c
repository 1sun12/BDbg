
#include "wait.h"

wait_t *wait_create() {

  /* allocate memory on the heap */
  wait_t *create = NULL;
  create = (wait_t *)malloc(sizeof(wait_t));

  /* init variables */
  create->test = 5;

  /* return new wait */
  return create;
}
