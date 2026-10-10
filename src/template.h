#ifndef TEMPLATE_H

#define MAX_TEMPLATE_COUNT 10

#include "util.h"
#include <dirent.h>
#include <stdio.h>
typedef struct {
  const char *dirName;
  const char *tempName;
} Template;




void initTemplates();
Template *getTemplate(const char *tempName);
void freeallTemplates();
#endif // !TEMPLATE_H
