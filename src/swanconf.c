#include "swanconf.h"

ConfigItem config_db[MAX_CONFIG_ITEMS];
int config_count = 0;

static char *trim(char *str) {
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

void add_config(const char *section, const char *key, const char *value) {
  if (config_count >= MAX_CONFIG_ITEMS)
    return;

  ConfigItem *item = &config_db[config_count++];
  if (strlen(section) > 0) {
    snprintf(item->key, sizeof(item->key), "%s.%s", section, key);
  } else {
    strncpy(item->key, key, sizeof(item->key) - 1);
  }
  strncpy(item->value, value, sizeof(item->value) - 1);
}

// Helper to retrieve a value
const char *get_config(const char *full_key) {
  for (int i = 0; i < config_count; i++) {
    if (strcmp(config_db[i].key, full_key) == 0) {
      return config_db[i].value;
    }
  }
  return NULL;
}

// Main parsing function
bool parse_config(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file)
    return false;

  char line[MAX_LINE_LENGTH];
  char current_section[64] = "";

  while (fgets(line, sizeof(line), file)) {
    char *trimmed = trim(line);

    // Skip empty lines and comments
    if (trimmed[0] == '\0' || trimmed[0] == '#')
      continue;

    // Check for Section [name]
    if (trimmed[0] == '[') {
      char *end_bracket = strchr(trimmed, ']');
      if (end_bracket) {
        *end_bracket = '\0';
        strncpy(current_section, trimmed + 1, sizeof(current_section) - 1);
      }
      continue;
    }

    // Check for Key-Value pair
    char *equals_sign = strchr(trimmed, '=');
    if (equals_sign) {
      *equals_sign = '\0';
      char *key = trim(trimmed);
      char *value = trim(equals_sign + 1);

      // Strip quotes from strings if present
      if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        value[strlen(value) - 1] = '\0';
        value++;
      }

      add_config(current_section, key, value);
    }
  }

  fclose(file);
  return true;
}
