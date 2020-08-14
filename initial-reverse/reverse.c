#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    FILE *inp = stdin;
    FILE *out = stdout;
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n"); 
        return 1;
    }
    if (argc == 3) {
        // fopen , return FILE*
        inp = fopen(argv[1], "r");
        if (inp == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            return 1;
        }
        out = fopen(argv[2], "w");
        if (out == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            return 1;
        }
        // struct stat : file describation struct
        struct stat *struct_inp = malloc(sizeof(struct stat));
        struct stat *struct_out = malloc(sizeof(struct stat));
        // fileno : FILE* to fd(int)
        // fstat : fd to struct stat
        fstat(fileno(inp), struct_inp);  
        fstat(fileno(out), struct_out);  
        // if two files are same, there inode number is same
        if (struct_inp->st_ino == struct_out->st_ino) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            free(struct_inp);
            free(struct_out);
            return 1;
        }
        free(struct_inp);
        free(struct_out);
    } else if (argc == 2) {
        inp = fopen(argv[1], "r");
        if (inp == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    }
    // char ** pointer to a string, can represent a string array
    char **str_arr = malloc(sizeof(char*)); 
    // char *   a string
    char *line = NULL;
    size_t n = 0;
    int len = 0;
    // nread: char numbers that getline read (not n!!!)
    size_t nread;
    while ((nread = getline(&line, &n, inp)) != -1) {
        // realloc returns a [new] pointer to a bigger(if size is bigger) memory,
        // so str_arr must be assigned to the new pointer
        if ((str_arr = realloc(str_arr, sizeof(char *) * (++len))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            return 1;
        }
        // alloc memory for new string
        str_arr[len-1] = (char *)malloc(sizeof(char) * (nread));
        // copy string to new memory
        strcpy(str_arr[len-1], line);
    }
    for (int i = len - 1; i >= 0; --i) {
        fprintf(out, "%s", str_arr[i]);
        free(str_arr[i]);
    }
    free(str_arr);
    free(line);
}
