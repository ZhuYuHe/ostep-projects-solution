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
#include "include.h"

//#define ERROR_MSSG "An error has occurred\n"
//#define ERROR fprintf(stderr, ERROR_MSSG);

int main(int argc, char *argv[]) {
    char *test = malloc(sizeof(char) * 30);
    strcpy(test, "hello");
    strcpy(test, "hehe");
    char *tmp = test;
    printf("%s\n", test);
    free(test);
    printf("%s\n", tmp);
    return 0;
}
