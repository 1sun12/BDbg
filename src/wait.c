#include "wait.h"

void wait_help_me() {
  char help[] = 
    "\n\n===== Wait =====\n\n"
    "# Description\n"
    "---\n"
    "Wait is an api that abstracts some of the complexity of the wait & waitpid sys call from <unistd.h>.\n"
    "\n# Variables\n"
    "---\n"
    "NAME \t\tTYPE \t\tDESC\n\n"
    "pid \t\tpid_t \t\tProcess id of the child (singular) being waited on.\n"
    "status \t\tint \t\tBit packed code that is returned from a call to waitpid.\n"
    "option \t\tint \t\tThe selected option passed to waitpid, its third and last parameter.\n"
    "\n# Methods\n"
    "---\n"
    "NAME \twait_help_me\n"
    " \t\tCASE \ttemp->help_me();\n"
    " \t\tTYPE \tvoid wait_help_me(void)\n"
    " \t\tDESC \tCall as is to get this man page.\n"
    "\n"
    "NAME \twait_create\n"
    " \t\tCASE \twait_t *temp = wait_create();\n"
    " \t\tTYPE \twait_t *wait_create(void)\n"
    " \t\tDESC \tCreates a new wait api.\n"
    "\n"
    "NAME \twait_destroy\n"
    " \t\tCASE \ttemp->destroy(temp);\n"
    " \t\tTYPE \tvoid wait_destroy(wait_t **trash_ptr)\n"
    " \t\tDESC \tFree the wait api from memory. Warning, temp is now gone and no longer can be used.\n";

  printf("%s", help);
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
