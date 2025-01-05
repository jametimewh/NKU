#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#define PATH_MAX 100
#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100

// Function to trim spaces and tabs from a string
char *trim(char *str) {
    char *end;

    // Trim leading spaces and tabs
    while (*str == ' ' || *str == '\t') str++;

    if (*str == 0) return str; // All spaces or empty string

    // Trim trailing spaces and tabs
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;

    *(end + 1) = '\0';
    return str;
}

// Function to change directory
void change_directory(char *path) {
    if (path == NULL || strcmp(path, "") == 0) {
        struct passwd *pw = getpwuid(getuid());
        const char *home = pw->pw_dir;
        if (chdir(home) != 0) {
            perror("cd");
        }
    } else {
        if (chdir(path) != 0) {
            perror("cd");
        }
    }
}

// Function to execute a single command
void execute_command(char *command) {
    char *args[MAX_ARG_SIZE];
    char *token;
    int argc = 0;

    // Tokenize the command by spaces and tabs
    token = strtok(command, " \t");
    while (token != NULL && argc < MAX_ARG_SIZE - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;

    if (args[0] == NULL) return; // Empty command

    if (strcmp(args[0], "cd") == 0) {
        change_directory(args[1]);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("mini-shell");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork");
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char cwd[PATH_MAX];
    char *username = getpwuid(getuid())->pw_name;

    while (1) {
        // Display prompt
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            continue;
        }
        printf("%s@%s$ ", username, cwd);
        fflush(stdout);

        // Read input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Trim and ignore empty input
        char *trimmed_input = trim(input);
        if (strlen(trimmed_input) == 0) continue;

        // Split commands by ";"
        char *input_copy = strdup(trimmed_input);
        char *saveptr;
        char *command = strtok_r(input_copy, ";", &saveptr);
        while (command != NULL) {
            char *single_command = strdup(trim(command));
            execute_command(single_command);
            free(single_command);
            command = strtok_r(NULL, ";", &saveptr);
        }
        free(input_copy);
    }

    return 0;
}
