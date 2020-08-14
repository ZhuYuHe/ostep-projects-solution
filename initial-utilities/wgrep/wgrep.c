#include <stdio.h>
#define BUFFER_SIZE 1000

bool match(char* buffer, char* pattern) {
    if (*pattern == '\0') {
        return true;
    }
    if (*buffer == '\0') {
        return false;
    }
    if (*buffer != *pattern) {
        return false;
    }
    return match(++buffer, ++pattern);
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }
    FILE *fp;
    char *pattern = argv[1];
    if (argc == 3) {
        char *file = argv[2];
        fp = fopen(file, "r");
    } else {
        fp = stdin;
    }
    if (fp == NULL) {
        printf("wgrep: cannot open file\n");
        return 1;
    }
    char *buffer = new char[BUFFER_SIZE];
    buffer = fgets(buffer, BUFFER_SIZE, fp);
    while (buffer != NULL) {
        char* tmp = buffer;
        bool flag = false;
        while (*tmp != '\0') {
            if (match(tmp, pattern)) {
                flag = true;
            }
            tmp++;
        }
        if (flag) {
            printf("%s", buffer);
        }
        buffer = fgets(buffer, BUFFER_SIZE, fp);
    }
    return 0;
}
