/* Acceptance test for toggling of hotkey groups */

#define _POSIX_SOURCE
#define _GNU_SOURCE

#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "unity.h"

#ifndef BUILD_PATH
#define BUILD_PATH "./"
#endif

int sxhkd_p;
int cfg_fifo[2];
int stt_fifo[2];

void exec_sxhkd(void) {
    dup2(cfg_fifo[1], STDIN_FILENO);
    dup2(stt_fifo[0], STDOUT_FILENO);
    execl(BUILD_PATH "sxhkd", "sxhkd", "-c", "-", NULL);
}

void setUp(void) {
    char cfg[] = "{a} [A]\n"\
                  "  echo 'Caught a'\n\n"\
                  "{b} [A]\n"\
                  "  echo 'Caught b'n\n"\
                  "{c} [B]\n"\
                  "  echo 'Caught c'n\n"\
                  "{d}\n"\
                  "  echo 'Caught d'\n";

    if((pipe(cfg_fifo)) || (pipe(stt_fifo)))
        exit(errno);
    if((sxhkd_p = fork()) == 0)
        exec_sxhkd();
    if(sxhkd_p == -1)
        exit(errno);
    write(cfg_fifo[0], cfg, sizeof(cfg));
}

void tearDown(void) {
    /*
     * Kill child sxhkd.
     * Wait.
     */
    kill(sxhkd_p, SIGHUP);
    wait(&sxhkd_p);
}

/**
 * Asserts that all hotkey groups load from configuration and are
 * enabled at start.
 */
void test_toggle_group_load(void) {
    char buffer[32] = {0};
    system("xdotool key a");
    read(stt_fifo[1], buffer, 32);
    TEST_ASSERT_EQUAL_STRING("Caught a\n", buffer);
    system("xdotool key b");
    read(stt_fifo[1], buffer, 32);
    TEST_ASSERT_EQUAL_STRING("Caught b\n", buffer);
    system("xdotool key c");
    read(stt_fifo[1], buffer, 32);
    TEST_ASSERT_EQUAL_STRING("Caught a\n", buffer);
    system("xdotool key d");
    read(stt_fifo[1], buffer, 32);
    TEST_ASSERT_EQUAL_STRING("Caught a\n", buffer);
}

/**
 * Asserts that the first toggle signal to group A disables group A and
 * only group A.
 */
void test_toggle_group_disable_A(void) {
}

/**
 * Asserts that the first toggle signal to group A disables group A and
 * only group A.
 */
void test_toggle_group_disable_B(void) {
}

/**
 * Asserts that two toggle signals to group A returns to initial state.
 */
void test_toggle_group_enable_A(void) {
}

/**
 * Asserts that two toggle signals to group A returns to initial state.
 */
void test_toggle_group_enable_B(void) {
}

/**
 * Asserts that toggle signals behave appropriately after repeated
 * enabling and disabling.
 */
void test_toggle_group_loop(void) {
}

void test_toggle_group_func(void) {
    /*
     * - Start with sxhkd with four hotkeys configured. Two in group "a",
     *   one in group "b" and one in no group.
     * - Each hotkey should send a unique message via pipe to test
     *   process.
     * - Confirm that all hotkeys behave as expected.
     * - Send a toggle signal to sxhkd.
     * - Confirm that all hotkeys behave as expected.
     * - Send a toggle signal to sxhkd.
     * - Confirm that all hotkeys behave as expected.
     */
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_toggle_group_load);
    return UNITY_END();
}
