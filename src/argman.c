#include "argman.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *joinArgs(int ac, char **ag, int firstArg) {
  size_t length = 0;
  for (int i = firstArg; i < ac; i++) {
    size_t argLength = strlen(ag[i]);
    size_t separatorLength = i > firstArg ? 1 : 0;
    if (length > SIZE_MAX - separatorLength - 1 ||
        argLength > SIZE_MAX - length - separatorLength - 1) {
      errormessage("Command arguments are too long");
      exit(EXIT_FAILURE);
    }
    length += argLength + separatorLength;
  }

  char *joined = malloc(length + 1);
  if (!joined) {
    errormessage("Memory allocation failed for command arguments");
    exit(EXIT_FAILURE);
  }

  char *next = joined;
  for (int i = firstArg; i < ac; i++) {
    if (i > firstArg)
      *next++ = ' ';
    size_t argLength = strlen(ag[i]);
    memcpy(next, ag[i], argLength);
    next += argLength;
  }
  *next = '\0';
  return joined;
}

static void copyExtraArgs(Command *cmd, int ac, char **ag, int firstArg) {
  cmd->extraArgCount = ac - firstArg;
  cmd->extraArray =
      calloc((size_t)cmd->extraArgCount + 1, sizeof(*cmd->extraArray));
  if (!cmd->extraArray) {
    errormessage("Memory allocation failed for extra arguments");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < cmd->extraArgCount; i++) {
    cmd->extraArray[i] = strdup(ag[firstArg + i]);
    if (!cmd->extraArray[i]) {
      errormessage("Memory allocation failed for command argument");
      exit(EXIT_FAILURE);
    }
  }
}

// Get form the program argument and store it and send into [MANAGER]
Command *getCmd(int ac, char **ag) {
  Command *cmd = calloc(1, sizeof(Command));
  if (!cmd)
    return NULL;

  /* default values */
  cmd->atype = NONE;
  cmd->cdata = NULL;
  cmd->extraArray = NULL;
  cmd->extraArgCount = 0;

  if (ac < 2) {
    return cmd;
  }

  /* command name is the first argument after program name */
  char *cmdtext = ag[1];

  /* commands that take no extra argument (ac == 2) */
  if (ac == 2) {
    if (checkstr(cmdtext, "init")) {
      cmd->atype = INIT;
    } else if (checkstr(cmdtext, "version")) {
      cmd->atype = VERSION;
    } else if (checkstr(cmdtext, "build")) {
      cmd->atype = BUILD;
    } else if (checkstr(cmdtext, "help")) {
      cmd->atype = HELP;
    } else if (checkstr(cmdtext, "list")) {
      cmd->atype = LIST;
    } else if (checkstr(cmdtext, "show")) {
      cmd->atype = SHOW;
    } else if (checkstr(cmdtext, "complete")) {
      cmd->atype = COMPLETE;
    } else if (checkstr(cmdtext, "delete")) {
      cmd->atype = DELETE;
    } else if (checkstr(cmdtext, "run")) {
      cmd->atype = RUN;
    }
    return cmd;
  }
  if (ac >= 3) {
    if (checkstr(cmdtext, "run")) {
      cmd->cdata = joinArgs(ac, ag, 2);
      cmd->atype = RUN;
    } else if (checkstr(cmdtext, "template")) {
      cmd->cdata = joinArgs(ac, ag, 2);
      cmd->atype = TEMPLATE;
    }
  }

  /* commands that require at least one argument (ac >= 3) */
  if (ac == 3) {
    if (checkstr(cmdtext, "create")) {
      /* second argument is at ag[2] */
      cmd->cdata = strdup(ag[2]);
      cmd->atype = CREATE;
    } else if (checkstr(cmdtext, "add")) {
      /* second argument is at ag[2] */
      cmd->cdata = strdup(ag[2]);
      cmd->atype = ADD;
    }
  }
  if (cmd->atype == RUN || cmd->atype == TEMPLATE) {
    copyExtraArgs(cmd, ac, ag, 2);
  }
  // Extra Argument needed Commands

  return cmd;
}

// DEBUG:
// Just use for debugging
void displayType(ARGTYPE at) {
  switch (at) {
  case NONE:
    puts("NONE");
    break;
  case INIT:
    puts("INIT");
    break;
  case VERSION:
    puts("VERSION");
    break;
  case BUILD:
    puts("BUILD");
    break;
  case HELP:
    puts("HELP");
    break;
  case LIST:
    puts("LIST");
    break;
  case SHOW:
    puts("SHOW");
    break;
  case COMPLETE:
    puts("COMPLETE");
    break;
  case DELETE:
    puts("DELETE");
    break;
  case RUN:
    puts("RUN");
    break;
  case CREATE:
    puts("CREATE");
    break;
  case ADD:
    puts("ADD");
    break;
  case TEMPLATE:
    puts("TEMPLATE");
    break;
  default:
    printf("UNKNOWN ARGTYPE (%d)\n", at);
    break;
  }
}
