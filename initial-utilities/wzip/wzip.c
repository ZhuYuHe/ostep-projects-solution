#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    int pre = -1;
    int c = 0;
    int cnt = 1;
    for (int i = 1; i < argc; ++i) {
        char *file = argv[i];
        FILE *fp = fopen(file, "r");
        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            return 1;
        }
        while ((c=fgetc(fp)) != EOF) {
            if (pre != -1) {
                if (pre == c) {
                    cnt ++;
                } else {
                    //printf("cnt: %d\n", cnt);
                    //printf("pre: %c\n", pre);
                    fwrite(&cnt, sizeof(int), 1, stdout);
                    fputc(pre, stdout);
                    cnt = 1;
                }
            }
            pre = c;
        }
        fclose(fp);
    }
    if (cnt > 0) {
        fwrite(&cnt, sizeof(int), 1, stdout);
        fputc(pre, stdout);
    }
    return 0;
}

