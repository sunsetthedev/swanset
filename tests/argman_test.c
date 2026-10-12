#include "argman.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool checkstr(const char *left, const char *right) {
  return strcmp(left, right) == 0;
}

void errormessage(const char *message) { fprintf(stderr, "%s\n", message); }

void debugMessage(const char *message) { (void)message; }

static void freeCommand(Command *cmd) {
  if (!cmd)
    return;
  for (int i = 0; i < cmd->extraArgCount; i++)
    free(cmd->extraArray[i]);
  free(cmd->extraArray);
  free(cmd->cdata);
  free(cmd);
}

int main(void) {
  char *templateArgs[] = {"swanset", "template", "add",
                          "test/hello world.tempconf"};
  Command *cmd = getCmd(4, templateArgs);
  assert(cmd != NULL);
  assert(cmd->atype == TEMPLATE);
  assert(strcmp(cmd->cdata, "add test/hello world.tempconf") == 0);
  assert(cmd->extraArgCount == 2);
  assert(strcmp(cmd->extraArray[0], "add") == 0);
  assert(strcmp(cmd->extraArray[1], "test/hello world.tempconf") == 0);
  freeCommand(cmd);

  char *runArgs[] = {"swanset", "run", "build/app", "argument with spaces"};
  cmd = getCmd(4, runArgs);
  assert(cmd != NULL);
  assert(cmd->atype == RUN);
  assert(strcmp(cmd->cdata, "build/app argument with spaces") == 0);
  assert(cmd->extraArgCount == 2);
  assert(strcmp(cmd->extraArray[1], "argument with spaces") == 0);
  freeCommand(cmd);

  return 0;
}
