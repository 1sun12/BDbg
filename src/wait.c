#include "wait.h"

/* ===== Private / Internal ===== */

/**
 *  @title wait_pid_h
 *  @brief [PRIVATE] Raw waitpid system call. Not exposed on the struct.
 *  Called internally by wait_wait() only.
 *  @param self The wait api object
 *  @return pid_t from waitpid (child pid on success, 0 for WNOHANG with no change, -1 on error)
 */
static pid_t wait_pid_h(wait_t *self) {
  return waitpid(self->cpid, &self->status, self->option);
}

/* ===== Help ===== */

void wait_help_me(void) {
  char help[] = 
    "\n\n===== Wait API =====\n\n"
    "# Description\n"
    "---\n"
    "Wait is an api that abstracts the complexity of the waitpid sys call from <sys/wait.h>.\n"
    "It wraps waitpid, decodes the bit-packed status integer, and returns a clean enumeration\n"
    "telling you what happened to the child process.\n"
    "\n# Variables\n"
    "---\n"
    "NAME \t\tTYPE \t\t\tDESC\n\n"
    "cpid \t\tpid_t \t\t\tProcess id of the child being waited on.\n"
    "status \t\tint \t\t\tBit packed code returned from waitpid (raw, use state instead).\n"
    "option \t\tint \t\t\tThe selected option passed to waitpid (3rd argument).\n"
    "options_e \twait_op_e \t\tEnumeration used in setting option.\n"
    "state \t\twait_state_e \t\tDecoded child state after wait_wait() returns.\n"
    "stop_sig \tint \t\t\tSignal that stopped the child (valid when state == WAIT_STOPPED).\n"
    "exit_code \tint \t\t\tExit code from the child (valid when state == WAIT_EXITED).\n"
    "term_sig \tint \t\t\tSignal that killed the child (valid when state == WAIT_SIGNALED).\n"
    "\n# Enumerations\n"
    "---\n"
    "NAME \twait_op_e (waitpid options, count from 0)\n"
    "COUNT \tOPTION \t\t\tDESC\n\n"
    "0 \tWAIT_BLOCK \t\tBlocks until child changes state.\n"
    "1 \tWAIT_NO_HANG \t\tDo not block, return 0 if nothing happened yet.\n"
    "2 \tWAIT_UNTRACED \t\tReport stopped children, not just terminated (used with ptrace).\n"
    "3 \tWAIT_CONTINUED \t\tReport children resumed by SIGCONT.\n"
    "\n"
    "NAME \twait_state_e (child state after wait, count from 0)\n"
    "COUNT \tSTATE \t\t\tDESC\n\n"
    "0 \tWAIT_STOPPED \t\tChild was stopped by a signal. Check stop_sig.\n"
    "1 \tWAIT_EXITED \t\tChild exited normally. Check exit_code.\n"
    "2 \tWAIT_SIGNALED \t\tChild was killed by a signal. Check term_sig.\n"
    "3 \tWAIT_UNKNOWN \t\tCould not determine child state.\n"
    "\n# Methods\n"
    "---\n"
    "NAME \twait_help_me\n"
    " \tCASE \ttemp->help_me();\n"
    " \tTYPE \tvoid wait_help_me(void)\n"
    " \tDESC \tPrints this man page.\n"
    "\n"
    "NAME \twait_create\n"
    " \tCASE \twait_t *temp = wait_create(cpid, WAIT_BLOCK);\n"
    " \tTYPE \twait_t *wait_create(pid_t cpid, int option)\n"
    " \tDESC \tCreates a new wait api. Pass a wait_op_e value for option.\n"
    "\n"
    "NAME \twait_destroy\n"
    " \tCASE \ttemp->destroy(&temp);\n"
    " \tTYPE \tvoid wait_destroy(wait_t **trash_ptr)\n"
    " \tDESC \tFree the wait api from memory. Warning: temp is gone after this call.\n"
    "\n"
    "NAME \twait_wait\n"
    " \tCASE \twait_state_e state = temp->wait(temp);\n"
    " \tTYPE \twait_state_e wait_wait(wait_t *self)\n"
    " \tDESC \tCalls waitpid, decodes the status, populates state/stop_sig/exit_code/term_sig.\n"
    " \t     \tReturns the wait_state_e so you can switch on it immediately.\n"
    "\n"
    "NAME \twait_set_cpid / wait_get_cpid\n"
    " \tCASE \ttemp->set_cpid(temp, new_pid); pid_t p = temp->get_cpid(temp);\n"
    " \tDESC \tSet or get the child process id.\n"
    "\n"
    "NAME \twait_set_op / wait_get_op\n"
    " \tCASE \ttemp->set_op(temp, WAIT_UNTRACED); int o = temp->get_op(temp);\n"
    " \tDESC \tSet or get the waitpid option. Pass a wait_op_e enum value to set_op.\n"
    "\n# Examples\n"
    "---\n"
    "#include <unistd.h>\n"
    "#include \"wait.h\"\n"
    "\n"
    "pid_t cpid = fork();\n"
    "if (cpid == 0) {\n"
    "  /* child: exec target program */\n"
    "} else {\n"
    "  /* parent: create wait api, block until child changes state */\n"
    "  wait_t *w = wait_create(cpid, WAIT_BLOCK);\n"
    "  wait_state_e state = w->wait(w);\n"
    "\n"
    "  switch (state) {\n"
    "    case WAIT_STOPPED:\n"
    "      printf(\"Child stopped by signal: %%d\\n\", w->stop_sig);\n"
    "      break;\n"
    "    case WAIT_EXITED:\n"
    "      printf(\"Child exited with code: %%d\\n\", w->exit_code);\n"
    "      break;\n"
    "    case WAIT_SIGNALED:\n"
    "      printf(\"Child killed by signal: %%d\\n\", w->term_sig);\n"
    "      break;\n"
    "    default:\n"
    "      printf(\"Unknown state\\n\");\n"
    "      break;\n"
    "  }\n"
    "\n"
    "  w->destroy(&w);\n"
    "}\n"
    "\n\n===== Wait API =====\n\n";

  printf("%s", help);
}

/* ===== Constructor & Destructor ===== */

wait_t *wait_create(pid_t cpid, int option) {
  /* create a new wait api */
  wait_t *temp = NULL;

  /* [!] allocate memory and zero it out */
  temp = (wait_t *)calloc(1, sizeof(wait_t));
  if (temp == NULL) {
    perror("[ERROR]:wait_create");
    return NULL;
  }

  /* initialize variables */
  temp->cpid = cpid;
  temp->status = 0;
  temp->state = WAIT_UNKNOWN;
  temp->stop_sig = 0;
  temp->exit_code = 0;
  temp->term_sig = 0;

  /* set the option via set_op so the enum and raw value stay in sync */
  wait_set_op(temp, option);

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

/* ===== Methods ===== */

wait_state_e wait_wait(wait_t *self) {
  /* call the private raw waitpid wrapper */
  pid_t result = wait_pid_h(self);

  /* waitpid returned an error */
  if (result < 0) {
    perror("[ERROR]:wait_wait");
    self->state = WAIT_UNKNOWN;
    return self->state;
  }

  /* WNOHANG: nothing happened yet, child still running */
  if (result == 0) {
    self->state = WAIT_UNKNOWN;
    return self->state;
  }

  /* decode the bit-packed status into our clean enum + fields */
  if (WIFSTOPPED(self->status)) {
    self->state = WAIT_STOPPED;
    self->stop_sig = WSTOPSIG(self->status);
  } 
  else if (WIFEXITED(self->status)) {
    self->state = WAIT_EXITED;
    self->exit_code = WEXITSTATUS(self->status);
  } 
  else if (WIFSIGNALED(self->status)) {
    self->state = WAIT_SIGNALED;
    self->term_sig = WTERMSIG(self->status);
  } 
  else {
    self->state = WAIT_UNKNOWN;
  }

  return self->state;
}

void wait_set_cpid(wait_t *self, pid_t x) {
  self->cpid = x;
}

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
