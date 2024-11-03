#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sxhkd.h"
#include "control.h"

int get_ctrl_path(char* target, pid_t pid, size_t nbytes) {
    DIR *ctrl_dir;
    snprintf(target, nbytes, "/tmp/sxhkd-%d", getuid());
    if ((ctrl_dir = opendir(target)) == NULL) {
        if (errno == ENOENT) {
            if (mkdir(target, S_IRWXU | S_IRWXG) < 0)
                goto abort;
        }
        else goto abort;
    }
    else closedir(ctrl_dir);
    nbytes -= strlen(target);
    return snprintf(target, nbytes, "/tmp/sxhkd-%d/sxhkd_%d", getuid(), pid);
abort:
    perror("get_ctrl_path");
    exit(errno);
}

void send_message(pid_t pid, char *message) {
    char ctrl_path[MAXLEN];
    int ctrl_fifo;
    kill(pid, SIGUSR1);
    get_ctrl_path(ctrl_path, pid, sizeof ctrl_path);
    if ((ctrl_fifo = open(ctrl_path, O_WRONLY)) < 0)
        goto abort;
    write(ctrl_fifo, message, MAXLEN);
    close(ctrl_fifo);
    return;
abort:
    perror("send_message");
    exit(errno);
}
