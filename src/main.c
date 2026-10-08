#include "argman.h"
#include "manager.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  Command *cmd = getCmd(argc, argv);
  execute(cmd,argc,argv);
  return EXIT_SUCCESS;
}
