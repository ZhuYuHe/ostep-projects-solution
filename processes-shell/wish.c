#include "wish.h"

size_t parse_line(char *line, char **my_args) {
    my_args = malloc(sizeof(char*));
    size_t len = 0;
    while (line) {
        char *sep = strsep(&line, delim);
        if ((my_args = realloc(my_args, sizeof(char *) * (++len))) == NULL) {
            print_message(stderr, error_message, "");
            exit(1);
        }
        if ((my_args[len-1] = (char *)malloc(sizeof(char) * strlen(sep))) == NULL) {
            print_message(stderr, error_message, "");
            exit(1);
        }
        strcpy(my_args[len-1], sep);
    }
    return len;
}

int exec_line(char *line) {
    char **my_args = NULL;
    size_t arg_len = parse_line(line, my_args);
    if (arg_len == 0) {
        return 0;
    }
    if (str_equal(my_args[0], EXIT)) {
        if (arg_len > 1) {
            print_message(stderr, error_message, "");
        } else {
            exit(0);
        }
    } else {
        print_message(stderr, error_message, "");
    }
    return 0;
}

int inactive_mode() {
    char *line = NULL;
    while (true) {
        print_message(stdout, "wish> ", "");
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
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return inactive_mode();
    } else if (argc == 2) {
        return batch_mode(argv[1]);
    } else {
        print_message(stderr, error_message, "");
        return 1;
    }
}
