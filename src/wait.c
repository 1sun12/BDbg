#include "wait.h"

void wait_help_me() {
  char help[] = 
    "\n\n===== Wait =====\n\n"
    "# Description\n"
    "---\n"
    "Wait is an api that abstracts some of the complexity of the wait & waitpid sys call from <sys/wait.h>.\n"
    "\n# Variables\n"
    "---\n"
    "NAME \t\tTYPE \t\tDESC\n\n"
    "pid \t\tpid_t \t\tProcess id of the child (singular) being waited on.\n"
    "status \t\tint \t\tBit packed code that is returned from a call to waitpid.\n"
    "option \t\tint \t\tThe selected option passed to waitpid, its third and last parameter. See    ...\n"
    "enumeration wait_op_e for options counting from 0.\n"
    "\n# Enumerations\n"
    "---\n"
    "NAME \twait_op_e\n"
    "COUNT \t\tOPTION \t\tDESC\n\n"
    "0 \t\tWAIT_BLOCK \t\tBlocks until child changes state.\n"
    "1 \t\tWAIT_NO_HANG \t\tDo not block, return 0 immediate if nothing has happened yet.\n"
    "2 \t\tWAIT_UNTRACED \t\tReport stopped children, not just terminated ones (used with ptrace).\n"
    "3 \t\tWAIT_CONTINUED \t\tReport children resumed by SIGCONT.\n"
    "\n# Methods\n"
    "---\n"
    "NAME \twait_help_me\n"
    " \t\tCASE \ttemp->help_me();\n"
    " \t\tTYPE \tvoid wait_help_me(void)\n"
    " \t\tDESC \tCall as is to get this man page.\n"
    "\n"
    "NAME \twait_create\n"
    " \t\tCASE \twait_t *temp = wait_create(cpid, option);\n"
    " \t\tTYPE \twait_t *wait_create(pid_t cpid, int option)\n"
    " \t\tDESC \tCreates a new wait api.\n"
    "\n"
    "NAME \twait_destroy\n"
    " \t\tCASE \ttemp->destroy(temp);\n"
    " \t\tTYPE \tvoid wait_destroy(wait_t **trash_ptr)\n"
    " \t\tDESC \tFree the wait api from memory. Warning, temp is now gone and no longer can be used.\n"
    "\n# Examples\n"
    "---\n"
    "#include <unistd.h>\n"
    "#include <sys/wait.h>\n"
    "\n/* fork program, create api, call wait */\n"
    "pid_t cpid = fork();\n"
    "wait_t *temp = wait_create(cpid, 0);\n"
    "temp->wait();\n"
    "\n\n===== Wait =====\n\n";

  printf("%s", help);
}

wait_t *wait_create(pid_t cpid, int option) {
  /* create a new wait api */
  wait_t *temp = NULL;

  /* [!] allocate memory */
  temp = (wait_t *)malloc(sizeof(wait_t));

  /* initialize variables */
  temp->cpid = cpid;
  temp->option = option;

  /* wire up psuedo-methods */
  temp->help_me = wait_help_me;
  temp->destroy = wait_destroy;
  temp->wait = wait_wait;
  temp->set_cpid = wait_set_cpid;
  temp->get_cpid = wait_get_cpid;
  temp->set_op = wait_set_op;
  temp->get_op = wait_get_op;


  /* return the new wait api */
  return temp;
}

void wait_destroy(wait_t **trash_ptr) {
  wait_t *trash = *trash_ptr;   /* needed for the 3rd step :] */
  free(trash);                  /* trash the memory block where wait api is stored */
  *trash_ptr = NULL;            /* clean up the pass-by-reference pointer */
}

void wait_wait(wait_t *self) {
  /* call waitpid */
  
}

void wait_set_cpid(wait_t *self, pid_t x) {
  self->cpid = x;
};

pid_t wait_get_cpid(wait_t *self) {
  return self->cpid;
}

void wait_set_op(wait_t *self, int x) {
  self->options_e = x;
  switch (self->options_e) {
    case WAIT_BLOCK:
      self->option = 0;
      break;
    case WAIT_NO_HANG:
      self->option = WNOHANG;
      break;
    case WAIT_UNTRACED:
      self->option = WUNTRACED;
      break;
    case WAIT_CONTINUED:
      self->option = WCONTINUED;
      break;
    default:
      self->option = 0;
      break;
  }
}

int wait_get_op(wait_t *self) {
  return self->option;
}
