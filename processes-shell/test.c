#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_blank_char(char c) {
    if (c == ' ' || c == '\t')  {
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
    while(is_blank_char(line[--pre])) {
        line[pre] = '\0';
    }
}

int main(int argc, char *argv[]) {
    char *t = NULL;
    printf("%ld\n", strlen(t));
    char *test = strdup("ls -la /home>t &   ls /home  ");
    char *sep = strsep(&test, "&");
    printf("before clean test : %s len: %ld\n", test, strlen(test));
    clean(test);
    printf("after clean test : %s len: %ld\n", test, strlen(test));
    return 0;
}
