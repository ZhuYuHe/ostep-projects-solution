#include "string_utils.h"
#include "include.h"
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
