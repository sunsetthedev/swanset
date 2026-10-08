#ifndef SWANCONF_H
#define SWANCONF_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
#define MAX_CONFIG_ITEMS 256

typedef struct {
  char key[128];
  char value[256];
} ConfigItem;
#define CONFIG_DB [MAX_CONFIG_ITEMS];
// extern ConfigItem config_db[MAX_CONFIG_ITEMS];
// extern int config_count;

// char *trim(char *str);
void add_config(const char *section, const char *key, const char *value);
const char *get_config(const char *full_key);
bool parse_config(const char *filename);
#endif // !SWANCONF_H
