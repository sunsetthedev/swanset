#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>
#define BUFFER_SIZE 4096
// TODO: Make functionable Log System Later : 2026-07-23 20:20
typedef struct {
  int line;
  char *currentdata;
  // char **datas;
  FILE *filepath;
} Log;

// BASIC UTILS
bool checkstr(const char *c1, const char *c2);
char *getcurrentDIRName();
char *toLowerCase(const char *text);
char *toUpperCase(const char *text);

// LOG SYSTEM
Log *initLog(FILE *fpath);
void writeLog(Log *l, char *data);
void deletelog(Log *l);
char *getLatestLog(Log *l);

// Running the program


// DIR Manipulation
int createFILE(const char *bdir, char *fname, const char *data);
int createDIR(const char *dname);
char *getTemplateDIR();
char *getBaseDIR();
bool isDIRExist(const char *basedir, const char *dirname);
bool isFILEExist(const char *basedir, const char *filename);
bool isEmptyDIR(const char *dirpath);
int copy_file(const char *src, const char *dst);
void copy_folder(const char *src_dir, const char *dst_dir);

// Terminal Color print Functions
void printwcolor(const char *color, const char *message);
void errormessage(const char *message);
void infoMessage(const char *message);
void successMessage(const char *message);
void warningMessage(const char *message);
void debugMessage(const char *message);
void actionMessage(const char *message);
#endif // !UTIL_H
