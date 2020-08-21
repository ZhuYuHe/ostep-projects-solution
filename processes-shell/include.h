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

#define MAX_PID_NUM 20
#define ERROR_MSSG "An error has occurred\n"
#define ERROR fprint(stderr, ERROR_MSSG);
//#define MAX_LINE_LEN 200

// data used in whole shell process
//typedef struct ShellContext {
//    // paths set by user
//    char *paths;
//    // cwd max len set by os
//    char cwd[PATH_MAX];
//}
//
//// data used in one line
//typedef struct LineContext {
//    struct ShellContext *shell_context; 
//    ProcessContext prcesss[MAX_PID_NUM];
//    char line[MAX_LINE_LEN];
//} LineContext;
//
//// data used in a process
//typedef struct ProcessContext {
//    int pid; 
//    bool is_redir;
//    int redir;
//    char *redir_file;
//    char *cmd;
//} LineContext;
