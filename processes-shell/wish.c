#include "wish.h"
#include "string_utils.h"

int inactive_mode() {
    ShellContext shell_context;
    shell_context.paths = "/bin";
    getcwd(shell_context.cwd, PATH_MAX);
    while (true) {
        fprintf(stdout, "wish> ");
        LineContext line_context;
        line_context.shell_context = &shell_context;
        char *line = NULL;
        size_t n = 0;
        size_t nread = 0; 
        if ((nread = getline(&line, &n, stdin)) <= 1) {
            continue;
        }
        if ((line_context.line = line_process(line, line_context.line)) == NULL) {
            continue;
        }
        exec_line(&line_context);
        free(line);
    }
    return 0;
}

int batch_mode(char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        print_message(stderr, error_message, "");
        return 1;
    }
    char *line = NULL;
    size_t n = 0;
    size_t nread = 0;
    while ((nread = getline(&line, &n, fp)) != -1) {
        char *new_line = NULL;
        new_line = line_process(line, new_line);
        if (empty(new_line)) {
            continue;
        } if (exec_line(new_line) != 0) {
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
