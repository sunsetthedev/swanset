#include "argman.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get form the program argument and store it and send into [MANAGER]
Command *getCmd(int ac, char **ag) {
  Command *cmd = malloc(sizeof(Command));
  if (!cmd)
    return NULL;

  /* default values */
  cmd->atype = NONE;
  cmd->cdata = NULL;

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
  if (ac == 2 || ac >= 3) {
    if (checkstr(cmdtext, "run")) {
      /* run may accept further args; store the first arg */
      cmd->cdata = strdup(ag[2]);
      strcat(cmd->cdata, " ");
      for (int i = 3; i < ac; i++) {
        strcat(cmd->cdata, ag[i]);
        if (!(i == ac - 1)) {
          strcat(cmd->cdata, " ");
        }
      }
      cmd->atype = RUN;
    }
  }

  /* commands that require at least one argument (ac >= 3) */
  if (ac == 3) {
    if (checkstr(cmdtext, "create")) {
      /* second argument is at ag[2] */
      cmd->cdata = strdup(ag[2]);
      cmd->atype = CREATE;
    }
    else if (checkstr(cmdtext, "add")) {
      /* second argument is at ag[2] */
      cmd->cdata = strdup(ag[2]);
      cmd->atype = ADD;
    }
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
  default:
    printf("UNKNOWN ARGTYPE (%d)\n", at);
    break;
  }
}
