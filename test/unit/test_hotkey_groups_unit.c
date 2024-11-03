/* Test toggling of hotkey groups */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include "unity.h"

#include "types.h"
#include "sxhkd.h"
#include "control.h"

void setUp(void) {
    setup();
}

void tearDown(void) {
    cleanup();
}

/**
 * Creates a hotkey and then asserts that it can be found.
 */
void test_find_hotkey(void) {
    xcb_keysym_t keysym = 0x38;                 /* keysym for a */
    xcb_button_t button = 0;                    /* no button */
    chain_t *chain = make_chain();
    hotkey_t *hotkey;

    add_chord(chain, make_chord(keysym, button, 0, XCB_KEY_PRESS, false, false));

    hotkey = make_hotkey(chain, "cd");
    add_hotkey(hotkey);

    TEST_ASSERT_NOT_NULL(find_hotkey(keysym, button, 0,
                XCB_KEY_PRESS, false));
}

/**
 * validate pid
 * parse message command (but not command options)
 * write message struct to control pipe
 * send USR1 signal to process
 */
void test_send_message(void) {
    /*
     * open fifo
     * mask USR1
     * send_message(...)
     * assert USR1 pending
     * assert fifo
     */
    pid_t pid;
    char ctrl_path[MAXLEN];
    sigset_t set;
    pid = getpid();
    get_ctrl_path(ctrl_path, pid, sizeof ctrl_path);
    if ((mkfifo(ctrl_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0)
        exit(errno);
    sigemptyset(&set);
    if ((sigaddset(&set, SIGUSR1) < 0))
        exit(errno);
    sigprocmask(SIG_BLOCK, &set, NULL);
    if (fork() == 0) {
        send_message(pid, "toggle group");
        exit(EXIT_SUCCESS);
    }
    struct timespec timeout = {1, 50};
    TEST_ASSERT_EQUAL(SIGUSR1,
            sigtimedwait(&set, NULL, &timeout));
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    {
        int ctrl_fifo;
        fd_set descriptors;
        struct timeval timeout = {1, 50};
        char ctrl_string[MAXLEN] = {'\0'};
        int i = 0;
        if ((ctrl_fifo = open(ctrl_path, O_RDONLY)) == -1)
            exit(errno);
        do {
            FD_ZERO(&descriptors);
            FD_SET(ctrl_fifo, &descriptors);
            TEST_ASSERT_EQUAL(1, select(ctrl_fifo + 1, &descriptors, NULL, NULL, &timeout));
            read(ctrl_fifo, ctrl_string + i, 1);
        } while(ctrl_string[i++] != '\0');
        ctrl_string[i] = '\0';
        close(ctrl_fifo);
        remove(ctrl_path);
        TEST_ASSERT_EQUAL_STRING(CTRL_PREFIX CTRL_TOGGLE "group", ctrl_string);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_find_hotkey);
    RUN_TEST(test_send_message);
    return UNITY_END();
}
