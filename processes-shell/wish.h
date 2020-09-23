// how-to-create-a-pipe :  https://tldp.org/LDP/lpg/node11.html
// include "include.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <limits.h>
#include <fcntl.h>


//typedef struct Environment {
//    /* Paths set by user */
//    char *paths;
//    char cwd[PATH_MAX];
//    bool path_set_by_user;
//} Environment;

//typedef struct Launch {
//    struct Environment *environment;
//    char *command;
//    char *args;
//    char *redirection;
//} Launch;


//typedef struct ErrMessage {
    //char *cmd_not_fd;
    //char *too_much_arg;
    //char *common_err;
//} ErrMessage;

//static const struct ErrMessage message = {"wish: command not found: %s\n", "wish: too much arguments%s\n", "wish: encounter an error\n"};
//
//
//
void print_message(FILE *out, char *msg, char *pattern) {
    fprintf(out, msg, pattern);
}
bool is_blank_char(char c) {
    if (c == ' ' || c == '\t' || c == '\0')  {
        return true;
    }
    return false;
}

bool empty(char *s) {
    if (s == NULL || strlen(s) == 0) {
        return true;
    }
    return false;
}
void clean(char *line) {
    if (line == NULL) {
        return;
    }
    // case : line - _ls -la /home_
    bool first_non_space = false;
    int cur = 0;
    int pre = 0;
    while (true) {
        if (line[cur] == '\0') {
            break;
        }
        if (is_blank_char(line[cur])) {
            if (first_non_space && !is_blank_char(line[cur-1])) {
                line[pre] = line[cur];
                pre++;
            } 
        } else {
            first_non_space = true;
            line[pre] = line[cur];
            ++pre;
        }
        ++cur;
    }
    line[pre] = '\0';
    while(is_blank_char(line[--pre])) {
        line[pre] = '\0';
    }
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


size_t parse_line(char *line, char ***my_args) {
    size_t len = 0;
    while (line) {
        char *sep = strsep(&line, delim);
        (*my_args)[len-1] = strdup(sep);
    }
    return len;
}

int exec_line(LineContext *line_context) {
    int pid_num = 0;
    char *total_line = line_context->line;
    while (!empty(total_line)) {
        char *line = strsep(&total_line, "&");
        ProcessContext process_context;
        process_context.is_redir = false;
        if (strchr(line, '>') != NULL) {
            process_context.is_redir = true;
        }
        if (empty(line)) {
            return 0;
        }
        char *total_cmd = strsep(&line, ">");
        // line - redirction
        // total_cmd - command
        // totla_line - next cmd
        clean(line);
        clean(total_cmd);
        process_context.redir_file = line;
        process_context.cmd = total_cmd;
        if (process_context.is_redir && (empty(line) || strchr(line, ' ') != NULL)) {
            ERROR;
            continue;
        }
        char **my_args = NULL;
        char *argv[MAX_ARGUMENTS];
        size_t arg_len = parse_line(total_cmd, &argv);
        if (arg_len == 0) {
            return 0;
        }
        process_context.argv = argv;
        if (arg_len == 0) {
            return 0;
        }
        if (str_equal(argv[0], "exit")) {
            if (arg_len > 1) {
                ERROR;
            } else {
                exit(0);
            }
        } else if (str_equal(argv[0], "cd")){
            if (arg_len == 1 || arg_len > 2) {
                ERROR;
            } else {
                if (chdir(argv[1]) != 0) {
                    ERROR;
                }
            }
        } else if (str_equal(my_args[0], "path")) {
            if (arg_len == 1) {
                line_context->shell_context->paths = NULL;
                continue;
            }
            line_context->shell_context->paths = realloc(path_list, sizeof(char *) * (path_len + arg_len - 1));
            char *cmd = line_context->shell_context->cwd;
            size_t cwd_len = strlen(cwd);
            cwd[cwd_len] = '/';
            for (int i = 1; i < arg_len; ++i) {
                size_t len = strlen(argv[i]);
                if (argv[i][0] != '/') {
                    if (argv[i][len-1] != '/') {
                        argv[i] = realloc(argv[i], sizeof(char) * (cwd_len + len + 3));
                        argv[i] = strcat(cwd, argv[i]);
                        argv[i][cwd_len+len+1] = '/';
                    } else {
                        argv[i] = realloc(argv[i], sizeof(char) * (cwd_len + len + 2));
                        argv[i] = strcat(cwd, argv[i]);
                    }
                }
                line_context->shell_context->paths[path_len + i-1] = strdup(my_args[i]);
            }
            path_len += arg_len-1;
        } else if (search_cmd(&cmd)){
            int rc = fork();
            if (rc < 0) {
                print_message(stderr, error_message, "");
                exit(1);
            } else if (rc == 0) {
                // son process
                // printf("I am son, my pid is %d\n", (int) getpid());
                if (line != NULL) {
                    int rfd = open(line, O_RDWR | O_CREAT | O_TRUNC, 0600);
                    dup2(rfd, STDOUT_FILENO);
                }
                if (-1 == execv(cmd, my_args)) {
                    print_message(stderr, error_message, "");
                }
            } else {
                pids[pid_num++] = rc;
            }
        } else {
            print_message(stderr, error_message, "");
        }
    }
    for (int i = 0; i < pid_num; ++i) {
        waitpid(pids[i], 0, 0);
    }
    return 0;
}
