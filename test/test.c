#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

int main(void) {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }

    char *dir_name = strrchr(cwd, '/');
    if (dir_name != NULL && *(dir_name + 1) != '\0') {
        printf("%s\n", dir_name + 1);
    } else {
        printf("%s\n", cwd);
    }

    return 0;
}
