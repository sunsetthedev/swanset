#include "manager.h"
#include "argman.h"
#include "swanconf.h"
#include "template.h"
#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Helper functions for core functionalities
char *getCFiles(const char *dirpath) {
  struct dirent *entry;
  DIR *dir = opendir(dirpath);
  if (!dir) {
    errormessage("Failed to open directory for listing .c files.");
    return NULL;
  }

  char *c_files = malloc(1024);
  c_files[0] = '\0';

  while ((entry = readdir(dir)) != NULL) {
    // Skip hidden navigation directories
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      // Check if the file has a .c extension
      const char *dot = strrchr(entry->d_name, '.');
      if (dot && strcmp(dot, ".c") == 0) {
        strcat(c_files, "src/");
        strcat(c_files, entry->d_name);
        strcat(c_files, " ");
      }
    }
  }

  closedir(dir);
  return c_files; // Return the list of .c files
}

void writeConfFile(const char *projname) {
  const char *username = getenv("USER");
  if (isFILEExist(".", "project.conf")) {
    errormessage("There is project.conf file exist in this directory");
    actionMessage("Overwriting the existing project.conf file...");
  }
  FILE *file = fopen("project.conf", "w");

  fprintf(file,
          "[project]\n"
          "name=\"%s\"\n"
          "version=\"0.1.0\"\n"
          "author=\"%s\"\n\n"
          "[build]\n"
          "compiler=\"gcc\"\n"
          "cflags=\"-Wall -std=c99\"\n"
          "cdir = \"src/*.c\"\n"
          "output = \"build/%s\"\n",
          projname, username, projname);

  fclose(file);
}

void buildProject(Template *temp) {
  if (temp == NULL) {
    fprintf(stderr, "Template is NULL\n");
    exit(1);
  }
  if (temp->dirName == NULL) {
    fprintf(stderr, "Template dirName is NULL\n");
    exit(1);
  }
  if (!isEmptyDIR(".")) {
    errormessage(
        "Current directory is not empty. Please run in an empty directory.");
    exit(1);
  }
  infoMessage("Creating Project...");
  const char *projname = toLowerCase(getcurrentDIRName());
  puts(getcurrentDIRName());
  writeConfFile(projname);
  copy_folder(temp->dirName, ".");
  successMessage("Project Created Successfully");
}

// Name functions are the working functions. These are like creating project,
// list projects and core of the Project

// [CORE] : Function for core usage
// Init the project in current location

// [CORE] : Display the LOGO of the program and the usage of the program
void DisplayLOGO() {
  /* Use explicit ANSI escape codes to avoid relying on external macros */
  printf("\x1b[36m");
  printf("\n\n  /$$$$$$                                                        "
         "  /$$    \n");
  printf(" /$$__  $$                                                        | "
         "$$    \n");
  printf("| $$  \\__/ /$$  /$$  /$$  /$$$$$$  /$$$$$$$   /$$$$$$$  /$$$$$$  "
         "/$$$$$$  \n");
  printf("|  $$$$$$ | $$ | $$ | $$ |____  $$| $$__  $$ /$$_____/ /$$__  $$|_  "
         "$$_/ \n");
  printf(" \\____  $$| $$ | $$ | $$  /$$$$$$$| $$  \\ $$|  $$$$$$ | $$$$$$$$  "
         "| $$   \n");
  printf(" /$$  \\ $$| $$ | $$ | $$ /$$__  $$| $$  | $$ \\____  $$| $$_____/  "
         "| $$ /$$\n");
  printf("|  $$$$$$/|  $$$$$/$$$$/|  $$$$$$$| $$  | $$ /$$$$$$$/|  $$$$$$$  |  "
         "$$$$/\n");
  printf(" \\______/  \\_____/\\___/  \\_______/|__/  |__/|_______/  "
         "\\_______/   \\___/  \n\n\n");
  printf("\x1b[0m");
  printf("Swanset (v:0.1.0)\n");
  printf("A simple project manager for C\n");
  printf("Usage: swanset [command] [options]\n");
  printf("Commands:\n");
  printf("  init        Initialize a new project\n");
  printf("  version     Show the version of the program\n");
  printf("  build       Build the project\n");
  printf("  run         Run the project\n");
  printf("  create      Create a new project from a template\n");
  printf("  list        List all projects\n");
  printf("  show        Show details of a project\n");
  printf("  complete    Mark a project as complete\n");
  printf("  delete      Delete a project\n");
}

// [CORE] : INIT Function for init the project in current location
void Init() {
  infoMessage("Initializing Project...");
  infoMessage("Checking for templates...");
  initTemplates();
  successMessage("Templated Loaded");
  Template *currenttemp = getTemplate("hello-world");
  if (currenttemp == NULL) {
    errormessage("Template not found. Please check the templates directory.");
    exit(1);
  }
  buildProject(currenttemp);
  freeallTemplates();
}

void Version() { infoMessage("Sunset (v0.1.0)"); }

// Build Function for building binary of Project Done in 17/8/2026
void Build() {
  if (!isFILEExist(".", "project.conf")) {
    errormessage("There is no project.conf file in this directory.");
    exit(1);
  }
  const char *config = "project.conf";
  if (!parse_config(config)) {
    errormessage("Config file is missing or corrupted.");
    exit(1);
  }
  const char *compiler = get_config("build.compiler");
  if (compiler == NULL) {
    warningMessage("No compiler found");
  }
  const char *flags = get_config("build.cflags");
  const char *cdir = get_config("build.cdir");
  const char *output = get_config("build.output");
  /* command buffer must be writable (not const) */
  char cmd[256];
  if (checkstr(cdir, "src/*.c")) {
    char *cfiles = getCFiles("src");
    if (cfiles == NULL) {
      errormessage("No .c files found in the specified directory.");
      exit(1);
    }
    snprintf(cmd, sizeof(cmd), "%s %s %s -o %s", compiler, cfiles, flags,
             output);
    debugMessage("Build Info:");
    debugMessage(cmd);
    free(cfiles); // Free the allocated memory for cfiles
  } else {
    snprintf(cmd, sizeof(cmd), "%s %s %s -o %s", compiler, cdir, flags, output);
    debugMessage("Build Info:");
    debugMessage(cmd);
  }
  infoMessage("Building the project....");
  if (!system(cmd)) {
    successMessage("Your project have been builded.");
    infoMessage("See in build/ or run with \"swanset run\"");
  } else {
    errormessage("Building have been terminated. Got some error");
  }
}

// Run function to run binary file from build function for the project
void Run() {

  if (!isFILEExist(".", "project.conf")) {
    errormessage("There is no project.conf file in this directory.");
    exit(1);
  }
  if (!parse_config("project.conf")) {
    errormessage("Project Config file is corrupted or missing");
    exit(1);
  }
  const char *binfile = get_config("build.output");
  actionMessage("Running the project...\n");

  if (!system(binfile)) {
    printf("\n");
    successMessage("Run successfully.");
  } else {
    errormessage("Running is stop by ERROR");
    exit(1);
  }
}

void Add(const char *name) {

  // Check is this a project directory
  if (!isFILEExist(".", "project.conf")) {
    errormessage("There is no project.conf file in this directory.");
    exit(1);
  }
  // Check again to have the src folder
  if (!isDIRExist(".", "src")) {
    errormessage("There is no include folder!!!");
    exit(1); // Later add to create folder by asking user(me) if he(me) want
  }
  // Check again to have the include folder
  if (!isDIRExist(".", "include")) {
    errormessage("There is no include folder!!!");
    exit(1); // Later add to create folder by asking user(me) if he(me) want
  }
  char headername[1024];
  snprintf(headername, 1024, "%s_H", toUpperCase(name));
  debugMessage(headername);

  // Do the jobs done functions
  char cfile[1024];
  snprintf(cfile, 1024, "%s.c", name);
  char hfile[1024];
  snprintf(hfile, 1024, "%s.h", name);
  debugMessage(cfile);
  debugMessage(hfile);

  char hdata[1024];
  snprintf(hdata, 1024, "#ifndef %s\n#define %s\n\n#endif // !%s", headername,
           headername, headername);
  debugMessage(hdata);
  char cdata[1024] = "#include \"../include/";
  strcat(cdata, hfile);
  strcat(cdata, "\"\n\n");
  debugMessage(cdata);

  actionMessage("Creating files....");
  createFILE("src", cfile, cdata);
  createFILE("include", hfile, hdata);
  infoMessage("files are added");
  successMessage("Successfully");
}

// Exec functions
void execute(Command *cmd, int ac, char *ag[]) {
  displayType(cmd->atype);
  debugMessage(cmd->cdata);
  if (cmd == NULL) {
    infoMessage("Command is NULL\n");
    exit(1);
  }
  switch (cmd->atype) {
  case INIT: // Done
    Init();
    break;
  case VERSION: // Not Quite Done
    Version();
    break;
  case BUILD: // Do in 15/8/2026 need to done in 17 UPDATE : Done
    Build();
    break;
  case RUN: // Do in 18/8/2026 need to done in 20 UPDATE : Kinda done in 26
    Run();
    break;
  case NONE: // Done
    DisplayLOGO();
    break;
  case ADD:
    Add(cmd->cdata);
    break;
  default:
    errormessage("Unknown command. Please check the usage.");
    break;
  }
}
