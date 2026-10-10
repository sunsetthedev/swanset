#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==========================================
// Data Structures
// ==========================================

#define MAX_VARS 64
#define MAX_FILES 64
#define MAX_FOLDERS 32
#define MAX_LINE_LEN 1024

typedef struct {
    char key[64];
    char value[256];
} GlobalVar;

typedef struct {
    char name[64];
    char under[64];
    char mode[10];
    char *data;       // Dynamically allocated for multi-line
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

// ==========================================
// Utility Functions
// ==========================================

// Trim leading and trailing whitespace
char* trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// Append string to dynamic data buffer
void append_data(FileConfig *file, const char *line) {
    size_t line_len = strlen(line);
    file->data = realloc(file->data, file->data_len + line_len + 2); // +2 for \n and \0
    if (file->data_len > 0) {
        strcat(file->data, "\n");
        file->data_len++;
    }
    strcat(file->data, line);
    file->data_len += line_len;
}

// Find a global variable by key
const char* get_var(TemplateConfig *cfg, const char *key) {
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
        if (strcmp(cfg->folders[i], folder) == 0) return 1;
    }
    return 0;
}

// ==========================================
// Parser Logic
// ==========================================

void parse_config(const char *filename, TemplateConfig *cfg) {
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
            if (reading_data) append_data(current_file, trimmed);
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
                fprintf(stderr, "Error: Too many files!\n"); exit(1);
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
            if (val[0] == '"' && val[strlen(val)-1] == '"') {
                val[strlen(val)-1] = '\0';
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

// ==========================================
// Validation & Interpolation (The "Cool" Stuff)
// ==========================================

void validate_and_process(TemplateConfig *cfg) {
    // 1. Validate Folders
    for (int i = 0; i < cfg->file_count; i++) {
        FileConfig *f = &cfg->files[i];
        if (strlen(f->under) > 0 && !folder_exists(cfg, f->under)) {
            fprintf(stderr, "❌ ERROR: File '%s' specifies under='%s', but '%s' is not in the folders list!\n", 
                    f->name, f->under, f->under);
            exit(1);
        }
    }

    // 2. Variable Interpolation in Data
    for (int i = 0; i < cfg->file_count; i++) {
        FileConfig *f = &cfg->files[i];
        if (!f->data) continue;

        // Simple interpolation: find ${var} and replace
        // Note: For a production engine, you'd write to a new buffer to avoid realloc issues.
        // Here we do a simple in-place conceptual replacement for demonstration.
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

// ==========================================
// Output / Debugging
// ==========================================

void print_config(TemplateConfig *cfg) {
    printf("📦 Template Configuration Parsed Successfully!\n");
    printf("------------------------------------------------\n");
    
    printf("🌐 Global Variables:\n");
    for (int i = 0; i < cfg->var_count; i++) {
        printf("   %s = \"%s\"\n", cfg->vars[i].key, cfg->vars[i].value);
    }
    
    printf("\n📁 Allowed Folders:\n   ");
    for (int i = 0; i < cfg->folder_count; i++) {
        printf("[%s] ", cfg->folders[i]);
    }
    printf("\n\n");

    printf("📄 Files to Generate:\n");
    for (int i = 0; i < cfg->file_count; i++) {
        FileConfig *f = &cfg->files[i];
        printf("   🔹 %s\n", f->name);
        if (strlen(f->under) > 0) printf("      📂 under: %s\n", f->under);
        printf("      🔒 mode: %s\n", f->mode);
        if (f->data) {
            printf("      📝 data:\n");
            // Indent data for readability
            char *line_start = f->data;
            while (*line_start) {
                char *nl = strchr(line_start, '\n');
                if (nl) *nl = '\0';
                printf("         | %s\n", line_start);
                if (nl) { *nl = '\n'; line_start = nl + 1; }
                else break;
            }
        }
        printf("\n");
    }
}

// ==========================================
// Main Execution
// ==========================================

int main() {
    TemplateConfig config;
    memset(&config, 0, sizeof(TemplateConfig));

    // Parse the file
    parse_config("template.conf", &config);
    
    // Validate folders and interpolate variables
    validate_and_process(&config);
    
    // Print results
    print_config(&config);

    // Cleanup dynamic memory
    for (int i = 0; i < config.file_count; i++) {
        if (config.files[i].data) free(config.files[i].data);
    }

    return 0;
}
