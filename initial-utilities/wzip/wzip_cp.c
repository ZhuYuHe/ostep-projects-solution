#include <stdio.h>
#include <stdlib.h>

FILE* open_file(char* filename) {
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("wzip: cannot open file\n");
    exit(1);
  }

  return fp;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }

  FILE* fp;
  int c = 0;
  int last = -1;
  int counter = 0;
  for (int i = 1; i < argc; i++) {
    fp = open_file(argv[i]);

    while ((c = fgetc(fp)) != EOF) {
      if (last == -1) {
        last = c;
        counter++;
      } else if (c != last) {
        fwrite(&counter, sizeof(int), 1, stdout);
        fputc(last, stdout);
        counter = 1;
      } else {
        counter++;
      }

      last = c;
    }

    fclose(fp);
  }

  if (counter > 0) {
    fwrite(&counter, sizeof(int), 1, stdout);
    fputc(last, stdout);
  }

  return 0;
}
