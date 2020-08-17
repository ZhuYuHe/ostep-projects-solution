#include "wish.h"

int inactive_mode() {
    while (true) {
        print_message(stdout, "wish> ", "");
        char *line = NULL;
        size_t n = 0;
        size_t nread = 0; 
        if ((nread = getline(&line, &n, stdin)) <= 1) {
            continue;
        }
        char *new_line = NULL;
        if ((new_line = line_process(line, new_line)) == NULL) {
            continue;
        }
        exec_line(new_line);
        free(line);
        free(new_line);
    }
    return 0;
}

int batch_mode(char *file) {
    FILE *fp = fopen(file, "r");
    char *line = NULL;
    size_t n = 0;
    size_t nread = 0;
    while ((nread = getline(&line, &n, fp)) != -1) {
        char *new_line = NULL;
        if ((new_line = line_process(line, new_line)) == NULL) {
            continue;
        }
        if (exec_line(new_line) != 0) {
            free(new_line);
            return 1;
        }
        free(new_line);
    }
    free(line);
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    init_path();
    if (argc == 1) {
        return inactive_mode();
    } else if (argc == 2) {
        return batch_mode(argv[1]);
    } else {
        print_message(stderr, error_message, "");
        return 1;
    }
}
