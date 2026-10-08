

/*
  swan init
  swan create "Cyberdeck Project"
  swan list
  swan show 1
  swan complete 1
  swan delete 1
*/

#ifndef ARGMAN_H
#define ARGMAN_H

typedef enum {
  INIT,
  VERSION,
  BUILD,
  RUN,
  CREATE,
  LIST,
  SHOW,
  COMPLETE,
  DELETE,
  HELP,
  NONE

} ARGTYPE;

typedef struct {
  ARGTYPE atype;
  char *cdata;
} Command;

Command *getCmd(int ac, char **ag);

// DEBUG:
void displayType(ARGTYPE at);

#endif // !ARGMAN_H
