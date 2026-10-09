#include "util.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "termicolor.h"

bool checkstr(const char *c1, const char *c2) { return strcmp(c1, c2) == 0; }

// TODO: Need Optimizatin
int createDIR(const char *dname) {
  printf("[CREATE] : ..Creating Directory.. {%s}\n", dname);
  if (mkdir(dname, 0777) == 0) {
    puts("Finished");
  } else {
    fprintf(stderr, "Failed to create %s\n", dname);
    return 1;
  }
  return 0;
}
int createFILE(const char *bdir, char *fname, const char *data) {
  char fullpath[256];
  snprintf(fullpath, sizeof(fullpath), "%s/%s", bdir, fname);

 // printf("[CREATE] : ..Creating FILE.. {%s}\n", fullpath);
  FILE *file = fopen(fullpath, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to create FILE {%s}\n", fullpath);
    return 1;
  }
  fprintf(file, "%s", data);

  fclose(file);

  return 0;
}
char *getHomeDIR() { return getenv("HOME"); }
char *getBaseDIR() { return strcat(getHomeDIR(), "/Swanset"); }
char *getTemplateDIR() { return strcat(getBaseDIR(), "/templates"); }
// LOG SYSTEM
// v0.1.0 finished : 2026-07-25 23:49
Log *initLog(FILE *fpath) {
  Log *l = malloc(sizeof(Log));
  l->line = 0;
  l->currentdata = "";
  l->filepath = fpath;
  return l;
}

bool isDIRExist(const char *basedir, const char *dirname) {
  struct dirent *entrydata;
  DIR *dir = opendir(basedir);
  if (!dir) {
    fprintf(stderr, "Can't check the basedir in isDIRExist");
    exit(1);
  }
  while ((entrydata = readdir(dir)) != NULL) {
    if (checkstr(dirname, entrydata->d_name)) {
      return true;
    }
  }

  return false;
}
int copy_file(const char *src, const char *dst) {
  FILE *source = fopen(src, "rb");
  if (!source)
    return -1;

  FILE *dest = fopen(dst, "wb");
  if (!dest) {
    fclose(source);
    return -1;
  }

  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  // Read chunks of data and write them to the new file
  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source)) > 0) {
    fwrite(buffer, 1, bytes_read, dest);
  }

  fclose(source);
  fclose(dest);
  return 0; // Success
}
void copy_folder(const char *src_dir, const char *dst_dir) {
  struct dirent *entry;
  struct stat entry_stat;
  char src_path[1024];
  char dst_path[1024];

  // Create destination folder
  mkdir(dst_dir, 0777);
  DIR *dir = opendir(src_dir);
  if (!dir)
    return;

  while ((entry = readdir(dir)) != NULL) {
    // Skip hidden navigation directories
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    // Build full paths: src_dir/filename and dst_dir/filename
    snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, entry->d_name);

    // Get file/folder information
    if (stat(src_path, &entry_stat) == 0) {
      if (S_ISDIR(entry_stat.st_mode)) {
        // If it's a directory, recurse into it
        copy_folder(src_path, dst_path);
      } else if (S_ISREG(entry_stat.st_mode)) {
        // If it's a regular file, copy it
        copy_file(src_path, dst_path);
      }
    }
  }
  closedir(dir);
}
// Checking file is exist in base directory
bool isFILEExist(const char *basedir, const char *filename)
{
  struct dirent *entrydata;
  DIR *dir = opendir(basedir);
  if (!dir) {
    fprintf(stderr, "Can't check the basedir in isFILEExist");
    exit(1);
  }
  while ((entrydata = readdir(dir)) != NULL) {
    if (checkstr(filename, entrydata->d_name)) {
      return true;
    }
  }

  return false;
}

// Checking if dirpath directory is empty or not
bool isEmptyDIR(const char *dirpath) {
  struct dirent *entry;
  DIR *dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "Failed to open directory: %s\n", dirpath);
    return false; // Consider non-existent directories as not empty
  }

  while ((entry = readdir(dir)) != NULL) {
    // Skip hidden navigation directories
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      closedir(dir);
      return false; // Found a file or subdirectory, so it's not empty
    }
  }

  closedir(dir);
  return true; // No files or subdirectories found, so it's empty
}

char *toLowerCase(const char *text) {
  char *str = malloc(strlen(text) + 1);
  if (str == NULL) {
    fprintf(stderr, "Failed to allocate memory for toLowerCase\n");
    return NULL;
  }
  strcpy(str, text);
  for (int i = 0; str[i]; i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str[i] = str[i] + 32;
    }
  }
  return str;
}

char *getcurrentDIRName() {
  char cwd[PATH_MAX];
  char *dir_name;
  char *result;

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd");
    exit(1);
  }

  dir_name = strrchr(cwd, '/');
  if (dir_name != NULL && *(dir_name + 1) != '\0') {
    result = malloc(strlen(dir_name + 1) + 1);
    if (result == NULL) {
      fprintf(stderr, "Failed to allocate memory for current directory name\n");
      exit(1);
    }
    strcpy(result, dir_name + 1);
  } else {
    result = malloc(strlen(cwd) + 1);
    if (result == NULL) {
      fprintf(stderr, "Failed to allocate memory for current directory name\n");
      exit(1);
    }
    strcpy(result, cwd);
  }
  return result;
}

TerminalColor get_terminal_color(const char *color_name) {
  for (size_t i = 0; i < sizeof(terminal_colors) / sizeof(TerminalColor); ++i) {
    if (strcmp(terminal_colors[i].color_name, color_name) == 0) {
      return terminal_colors[i];
    }
  }
  // Return a default color (white) if not found
  TerminalColor default_color = {"\x1b[37m", "White"};
  return default_color;
}

void printwcolor(const char *color, const char *message) {
  TerminalColor term_color = get_terminal_color(color);
  printf("%s%s%s", term_color.color_code, message, CLR_RESET);
}

void errormessage(const char *message) {
  printwcolor("Bright Red", "[ERROR] : ");
  printwcolor("Bright Red", message);
  printf("\n");
}
void infoMessage(const char *message) {
  printwcolor("Bright Blue", "[INFO] : ");
  printwcolor("Bright Blue", message);
  printf("\n");
}
void successMessage(const char *message) {
  printwcolor("Bright Green", "[SUCCESS] : ");
  printwcolor("Bright Green", message);
  printf("\n");
}
void warningMessage(const char *message) {
  printwcolor("Bright Yellow", "[WARNING] : ");
  printwcolor("Bright Yellow", message);
  printf("\n");
}
void debugMessage(const char *message) {
  printwcolor("Bright Magenta", "[DEBUG] : ");
  printwcolor("Bright Magenta", message);
  printf("\n");
}
void actionMessage(const char *message) {
  printwcolor("Bright Cyan", "[ACTION] : ");   
  printwcolor("Bright Cyan", message);
  printf("\n");
}
