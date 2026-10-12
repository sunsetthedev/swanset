#include "template.h"
#include "manager.h"
#include "util.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim(char *str) {
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return str;
  char *end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
  return str;
}

Template **templates;

void initTemplates() {
  // */ Variables
  const char *homedir = getenv("HOME");
  char tempdir[256];

  int i = 0;
  snprintf(tempdir, 256, "%s/.local/share/Swanset/templates", homedir);

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

// Append string to dynamic data buffer
void append_data(FileConfig *file, const char *line) {
  size_t line_len = strlen(line);
  size_t separator_len = file->data_len > 0 ? 1 : 0;
  if (line_len > SIZE_MAX - file->data_len - separator_len - 1) {
    fprintf(stderr, "Template file data is too large\n");
    exit(EXIT_FAILURE);
  }

  char *data =
      realloc(file->data, file->data_len + separator_len + line_len + 1);
  if (!data) {
    perror("Failed to allocate template file data");
    exit(EXIT_FAILURE);
  }
  file->data = data;
  if (separator_len)
    file->data[file->data_len++] = '\n';
  memcpy(file->data + file->data_len, line, line_len);
  file->data_len += line_len;
  file->data[file->data_len] = '\0';
}

// Find a global variable by key
const char *get_var(TemplateConfig *cfg, const char *key) {
  for (int i = 0; i < cfg->var_count; i++) {
    if (strcmp(cfg->vars[i].key, key) == 0) {
      return cfg->vars[i].value;
    }
  }
  return NULL;
}

// Check if folder exists in the folders list
int folder_exists(TemplateConfig *cfg, const char *folder) {
  for (int i = 0; i < cfg->folder_count; i++) {
    if (strcmp(cfg->folders[i], folder) == 0)
      return 1;
  }
  return 0;
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

// Outer Function
void parse_template(const char *filename, TemplateConfig *cfg) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening config file");
    exit(1);
  }

  char line[MAX_LINE_LEN];
  FileConfig *current_file = NULL;
  int reading_data = 0;

  while (fgets(line, sizeof(line), file)) {
    // Remove newline
    line[strcspn(line, "\r\n")] = 0;
    char *trimmed = trim(line);

    // Skip empty lines and comments
    if (strlen(trimmed) == 0 || strncmp(trimmed, "//", 2) == 0) {
      if (reading_data)
        append_data(current_file, trimmed);
      continue;
    }

    // STATE: Reading multi-line data
    if (reading_data) {
      if (strcmp(trimmed, "]") == 0) {
        reading_data = 0;
        current_file = NULL;
      } else {
        append_data(current_file, trimmed);
      }
      continue;
    }

    // STATE: File Header (#filename.c)
    if (trimmed[0] == '#') {
      if (cfg->file_count >= MAX_FILES) {
        fprintf(stderr, "Error: Too many files!\n");
        exit(1);
      }
      current_file = &cfg->files[cfg->file_count++];
      memset(current_file, 0, sizeof(FileConfig));
      strncpy(current_file->name, trimmed + 1, sizeof(current_file->name) - 1);
      strcpy(current_file->mode, "644"); // Default permissions
      continue;
    }

    // STATE: Key-Value Parsing
    char *eq_pos = strchr(trimmed, '=');
    if (eq_pos) {
      *eq_pos = '\0';
      char *key = trim(trimmed);
      char *val = trim(eq_pos + 1);

      // Remove quotes from values if present
      if (val[0] == '"' && val[strlen(val) - 1] == '"') {
        val[strlen(val) - 1] = '\0';
        val++;
      }

      if (current_file) {
        // File-specific attributes
        if (strcmp(key, "under") == 0) {
          strncpy(current_file->under, val, sizeof(current_file->under) - 1);
        } else if (strcmp(key, "mode") == 0) {
          strncpy(current_file->mode, val, sizeof(current_file->mode) - 1);
        } else if (strcmp(key, "data") == 0) {
          if (val[0] == '[') {
            reading_data = 1;
            // Check if data starts on the same line: data=[hello
            if (strlen(val) > 1) {
              append_data(current_file, val + 1);
            }
          }
        }
      } else {
        // Global variables
        if (strcmp(key, "folders") == 0) {
          // Parse space-separated folders
          char *token = strtok(val, " ");
          while (token != NULL && cfg->folder_count < MAX_FOLDERS) {
            strncpy(cfg->folders[cfg->folder_count++], token, 63);
            token = strtok(NULL, " ");
          }
        } else {
          if (cfg->var_count < MAX_VARS) {
            strncpy(cfg->vars[cfg->var_count].key, key, 63);
            strncpy(cfg->vars[cfg->var_count].value, val, 255);
            cfg->var_count++;
          }
        }
      }
    }
  }
  fclose(file);
}

// Validation & Interpolation

void validate_and_process(TemplateConfig *cfg) {
  // 1. Validate Folders
  for (int i = 0; i < cfg->file_count; i++) {
    FileConfig *f = &cfg->files[i];
    if (strlen(f->under) > 0 && !folder_exists(cfg, f->under)) {
      fprintf(stderr,
              "❌ ERROR: File '%s' specifies under='%s', but '%s' is not in "
              "the folders list!\n",
              f->name, f->under, f->under);
      exit(1);
    }
  }

  // 2. Variable Interpolation in Data
  for (int i = 0; i < cfg->file_count; i++) {
    FileConfig *f = &cfg->files[i];
    if (!f->data)
      continue;

    // Simple interpolation: find ${var} and replace
    // Note: For a production engine, you'd write to a new buffer to avoid
    // realloc issues. Here we do a simple in-place conceptual replacement for
    // demonstration.
    char *start = f->data;
    while ((start = strstr(start, "${")) != NULL) {
      char *end = strchr(start, '}');
      if (end) {
        char var_name[64] = {0};
        strncpy(var_name, start + 2, end - start - 2);

        const char *replacement = get_var(cfg, var_name);
        if (replacement) {
          // Calculate sizes
          size_t var_block_len = (end - start) + 1;
          size_t rep_len = strlen(replacement);
          size_t tail_len = strlen(end + 1);

          // Reallocate if needed
          if (rep_len > var_block_len) {
            size_t diff = rep_len - var_block_len;
            f->data = realloc(f->data, f->data_len + diff + 1);
            start = strstr(f->data, var_name) - 2; // Recalculate pointer
          }

          // Shift memory and insert
          char *dest = start + rep_len;
          memmove(dest, end + 1, tail_len + 1);
          memcpy(start, replacement, rep_len);

          f->data_len = f->data_len - var_block_len + rep_len;
          start += rep_len;
        } else {
          start = end + 1; // Skip if var not found
        }
      } else {
        break;
      }
    }
  }
}

void buildTemplate(const char *filename) {
  TemplateConfig tempconf = {0};
  debugMessage("Parse Started");
  parse_template(filename, &tempconf);
  debugMessage("Got it");

  const char *tempname = get_var(&tempconf, "name");
  if (!tempname || tempname[0] == '\0') {
    errormessage("Template configuration must define a non-empty name");
    exit(EXIT_FAILURE);
  }

  printf("🌐 Global Variables:\n");

  for (int i = 0; i < tempconf.var_count; i++) {
    printf("   %s = \"%s\"\n", tempconf.vars[i].key, tempconf.vars[i].value);
  }

  actionMessage("Creating Template....");
  if (createDIR(tempname) != 0) {
    for (int i = 0; i < tempconf.file_count; i++)
      free(tempconf.files[i].data);
    exit(EXIT_FAILURE);
  }
  successMessage("Successfully ");

  for (int i = 0; i < tempconf.file_count; i++)
    free(tempconf.files[i].data);
}
