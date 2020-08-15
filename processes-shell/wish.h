#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static const char *EXIT = "exit";
static const char *CD = "cd";
static const char *PATH = "path";
static const char *delim = " ";

static char **path_list = NULL;

char error_message[30] = "An error has occurred\n";

//typedef struct ErrMessage {
    //char *cmd_not_fd;
    //char *too_much_arg;
    //char *common_err;
//} ErrMessage;

//static const struct ErrMessage message = {"wish: command not found: %s\n", "wish: too much arguments%s\n", "wish: encounter an error\n"};

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

void alloc_mem_for_string_arr(char ***p, size_t arr_len, size_t str_len) {
    if ((*p = realloc(*p, sizeof(char *) * arr_len)) == NULL) {
        print_message(stderr, error_message, "");
        exit(1);
    }
    if (((*p)[arr_len-1] = (char *)malloc(sizeof(char) * str_len)) == NULL) {
        print_message(stderr, error_message, "");
        exit(1);
    }
}

size_t parse_line(char *line, char ***my_args) {
    *my_args = malloc(sizeof(char*));
    size_t len = 0;
    while (line) {
        char *sep = strsep(&line, delim);
        alloc_mem_for_string_arr(my_args, ++len, strlen(sep));
        strcpy((*my_args)[len-1], sep);
    }
    return len;
}

int exec_line(char *line) {
    char **my_args = NULL;
    size_t arg_len = parse_line(line, &my_args);
    printf("exec line: %s\n", line);
    printf("arg_len: %ld\n", arg_len);
    if (my_args == NULL) {
        printf("my_args is NULL\n");
        return 0;
    }
    if (arg_len == 0) {
        return 0;
    }
    if (str_equal(my_args[0], EXIT)) {
        printf("in str_equal\n");
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
        path_list = malloc(sizeof(char*));
        path_list = realloc(path_list, sizeof(char *) * (arg_len - 1));
        for (int i = 1; i < arg_len; ++i) {
            path_list[i-1] = malloc(sizeof(char) * strlen(my_args[i]));
            strcpy(path_list[i-1], my_args[i]);
        }
    } else {
        int rc = fork();
        if (rc < 0) {
            print_message(stderr, error_message, "");
            exit(1);
        } else if (rc == 0) {
            // son process
            printf("I am son, my pid is %d\n", (int) getpid());
            if (-1 == execv(my_args[0], my_args)) {
                print_message(stderr, error_message, "");
            }
        } else {
            // father process
            wait(NULL);
            printf("I am father of (pid: %d), my pid is %d\n", rc, (int) getpid());
        }
    }
    return 0;
}
