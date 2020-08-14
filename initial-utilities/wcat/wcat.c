#include <stdio.h>
#define BUFFER_SIZE 1000

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            return 1;
        }
        char* buffer = new char[BUFFER_SIZE];
        buffer = fgets(buffer, BUFFER_SIZE, fp);
        while (buffer != NULL) {
            printf("%s", buffer);
            buffer = fgets(buffer, BUFFER_SIZE, fp);
        }
        fclose(fp);
    }
    return 0;
}
