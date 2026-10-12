#ifndef TEMPLATE_H

#define MAX_TEMPLATE_COUNT 10

#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_VARS 64
#define MAX_FILES 64
#define MAX_FOLDERS 32
#define MAX_LINE_LEN 1024


typedef struct {
  const char *dirName;
  const char *tempName;
} Template;

// ==========================================
// For Building Templates from tempconf files
// ==========================================
typedef struct {
  char key[64];
  char value[256];
} GlobalVar;

typedef struct {
  char name[64];
  char under[64];
  char mode[10];
  char *data; // Dynamically allocated for multi-line
  size_t data_len;
} FileConfig;

typedef struct {
  GlobalVar vars[MAX_VARS];
  int var_count;

  char folders[MAX_FOLDERS][64];
  int folder_count;

  FileConfig files[MAX_FILES];
  int file_count;
} TemplateConfig;

void initTemplates();
Template *getTemplate(const char *tempName);
void freeallTemplates();

// Building templates 

void buildTemplate(const char *filename);


#endif // !TEMPLATE_H
