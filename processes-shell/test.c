#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char dest[100];
    strcpy(dest, "first");
    char *src = " second";
    strcat(dest, src);
    printf("dest: %s\n", dest);
    return 0;
}
