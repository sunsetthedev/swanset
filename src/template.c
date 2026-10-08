#include "template.h"
#include "manager.h"
#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Template **templates;

void initTemplates() {
  // */ Variables
  const char *homedir = getenv("HOME");
  char basedir[256];
  char tempdir[256];
  int i = 0;
  snprintf(basedir, 256, "%s/Swanset", homedir);
  snprintf(tempdir, 256, "%s/templates", basedir);

  struct dirent *entrydir;
  DIR *dir = opendir(tempdir);
  templates = malloc(sizeof(Template *) * MAX_TEMPLATE_COUNT);
  while ((entrydir = readdir(dir)) != NULL) {

    char dirs[256];
    if (checkstr(entrydir->d_name, ".") || checkstr(entrydir->d_name, "..")) {
      continue;
    } else {
      if (entrydir->d_type == DT_DIR) {
        snprintf(dirs, 256, "%s/%s", tempdir, entrydir->d_name);
        templates[i] = malloc(sizeof(Template));
        templates[i]->tempName = strdup(entrydir->d_name);
        templates[i]->dirName = strdup(dirs);
        i++;
      }
    }
  }

  closedir(dir);
  free(entrydir);
}

void freeallTemplates() {
  for (int i = 0; i < MAX_TEMPLATE_COUNT; i++) {
    if (templates[i] != NULL) {
      free((void *)templates[i]->tempName);
      free((void *)templates[i]->dirName);
      free(templates[i]);
    }
  }
  free(templates);
}

Template *getTemplate(const char *tempName) {
  Template *tmp = malloc(sizeof(Template));
  for (int i = 0; i < MAX_TEMPLATE_COUNT; i++) {
    if (strcmp(tempName, templates[i]->tempName) == 0) {
      tmp = templates[i];
      if (tmp == NULL) {
        fprintf(stderr, "Template is NULL!!! Return with error....\n");
        exit(1);
      }
      break;
    } else {
      fprintf(stderr, "No template found!!! Return with error....\n");
      exit(1);
    }
  }
  return tmp;
}
