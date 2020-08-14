#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }
    int cnt = 0;
    char c = '0';
    for (int k = 1; k < argc; ++k) {
        FILE *fp = fopen(argv[k], "r");
        while (0 != fread(&cnt, sizeof(int), 1, fp)) {
            if (0 == fread(&c, sizeof(char), 1, fp)) {
                return 1;
            }
            for (int i = 0; i < cnt; ++i) {
                printf("%c", c);
            }
        }
        fclose(fp);
    }
    return 0;
}
