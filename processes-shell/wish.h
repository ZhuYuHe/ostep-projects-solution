#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <limits.h>

static const char *EXIT = "exit";
static const char *CD = "cd";
static const char *PATH = "path";
static const char *delim = " ";
static char cwd[PATH_MAX];
//static char *redirection = "";
//static char *redir_file_name = "";

static int path_len = 0;
static char **path_list = NULL;

//typdef struct consts {
//    char *EXIT;
//    char *CD;
//    char *delim;
//} consts;
//
//typedef struct Environment {
//    /* Paths set by user */
//    char *paths;
//    char cwd[PATH_MAX];
//    bool path_set_by_user;
//} Environment;
//
//typedef struct Launch {
//    struct Environment *environment;
//    char *command;
//    char *args;
//    char *redirection;
//} Launch;

char error_message[30] = "An error has occurred\n";

//typedef struct ErrMessage {
    //char *cmd_not_fd;
    //char *too_much_arg;
    //char *common_err;
//} ErrMessage;

//static const struct ErrMessage message = {"wish: command not found: %s\n", "wish: too much arguments%s\n", "wish: encounter an error\n"};
//

void init_path() {
    path_list = malloc(sizeof(char*));
    path_list[0] = strdup("/bin/");
    path_len = 1;
    getcwd(cwd, PATH_MAX);
}

bool str_equal(char *s1, const char *s2) {
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2) {
            return false;
        }
        s1++;
        s2++;
    }
    if (*s1 == '\0' && *s2 == '\0') {
        return true;
    } else {
        return false;
    }
}

void print_message(FILE *out, char *msg, char *pattern) {
    fprintf(out, msg, pattern);
}

void *line_process(char *line, char *res) {
    if (res == NULL) {
        if ((res = (char *)malloc(sizeof(char) * strlen(line))) == NULL) {
            print_message(stderr, error_message, "");
            return res;
        }
    } else {
        if ((res = realloc(res, sizeof(char) * strlen(line))) == NULL) {
            print_message(stderr, error_message, "");
            return res;
        }
    }
    int len = 0;
    bool first = true;
    while (*line != '\0') {
        if (*line == '\n') {
            ++line;
            continue;
        }
        if (*line == ' ' || *line == '\t') {
            if (!first || len == 0) {
                ++line;
                continue;
            }
            first = false;
            *line = ' ';
        } else {
            first = true;
        }
        res[len++] = *line;
        ++line;
    }
    res[len] = '\0';
    while (res[--len] == ' ') {
        res[len] = '\0';
    }
    return res;
}


size_t parse_line(char *line, char ***my_args) {
    *my_args = malloc(sizeof(char*));
    size_t len = 0;
    while (line) {
        char *sep = strsep(&line, delim);
        if ((*my_args = realloc(*my_args, sizeof(char *) * (++len))) == NULL) {
            print_message(stderr, error_message, "");
            exit(1);
        }
        // strdup is better than realloc memory then strcpy
        (*my_args)[len-1] = strdup(sep);
    }
    return len;
}

bool search_cmd(char **cmd) {
    if (path_list == NULL) {
        return false;
    }
    char **tmp = path_list;
    int pl = path_len;
    size_t cmd_len = strlen(*cmd);
    while (--pl >= 0) {
        char *path_bin = strdup(*(tmp++));
        size_t pb_len = strlen(path_bin);
        path_bin = realloc(path_bin, sizeof(char)*(pb_len + cmd_len + 1));
        path_bin = strcat(path_bin, *cmd);
        if (access(path_bin, X_OK) == 0) {
            *cmd = strdup(path_bin);
            return true;
        }
    }
    return false;
}

int exec_line(char *line) {
    char **my_args = NULL;
    size_t arg_len = parse_line(line, &my_args);
    //printf("exec line: %s\n", line);
    //printf("arg_len: %ld\n", arg_len);
    if (my_args == NULL) {
        //printf("my_args is NULL\n");
        return 0;
    }
    my_args = realloc(my_args, sizeof(char*) * (arg_len+1));
    my_args[arg_len] = NULL;
    char *cmd = strdup(my_args[0]);
    if (arg_len == 0) {
        return 0;
    }
    if (str_equal(my_args[0], EXIT)) {
        if (arg_len > 1) {
            print_message(stderr, error_message, "");
        } else {
            exit(0);
        }
    } else if (str_equal(my_args[0], CD)){
        if (arg_len == 1 || arg_len > 2) {
            print_message(stderr, error_message, "");
        } else {
            if (chdir(my_args[1]) != 0) {
                print_message(stderr, error_message, "");
            }
        }
    } else if (str_equal(my_args[0], PATH)) {
        path_list = realloc(path_list, sizeof(char *) * (arg_len - 1));
        getcwd(cwd, PATH_MAX);
        size_t cwd_len = strlen(cwd);
        cwd[cwd_len] = '/';
        for (int i = 1; i < arg_len; ++i) {
            size_t len = strlen(my_args[i]);
            if (my_args[i][0] != '/') {
                if (my_args[i][len-1] != '/') {
                    my_args[i] = realloc(my_args[i], sizeof(char) * (cwd_len + len + 3));
                    my_args[i] = strcat(cwd, my_args[i]);
                    my_args[i][cwd_len+len+1] = '/';
                } else {
                    my_args[i] = realloc(my_args[i], sizeof(char) * (cwd_len + len + 2));
                    my_args[i] = strcat(cwd, my_args[i]);
                }
            }
            path_list[i-1] = strdup(my_args[i]);
        }
        path_len = arg_len-1;
    } else if (search_cmd(&cmd)){
        int rc = fork();
        if (rc < 0) {
            print_message(stderr, error_message, "");
            exit(1);
        } else if (rc == 0) {
            // son process
            //printf("I am son, my pid is %d\n", (int) getpid());
            if (-1 == execv(cmd, my_args)) {
                print_message(stderr, error_message, "");
            }
        } else {
            // father process
            wait(NULL);
            //printf("I am father of (pid: %d), my pid is %d\n", rc, (int) getpid());
        }
    } else {
        print_message(stderr, error_message, "");
    }
    return 0;
}
