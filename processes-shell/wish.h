#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char *EXIT = "exit";
static const char *delim = " ";
char error_message[30] = "An error has occurred\n";

//typedef struct ErrMessage {
    //char *cmd_not_fd;
    //char *too_much_arg;
    //char *common_err;
//} ErrMessage;

//static const struct ErrMessage message = {"wish: command not found: %s\n", "wish: too much arguments%s\n", "wish: encounter an error\n"};

bool str_equal(char *s1, char *s2) {
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
            if (!first) {
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
