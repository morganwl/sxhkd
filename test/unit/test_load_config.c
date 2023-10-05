/**
 * Tests changes to the configuration loading mechanism.
 */

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "unity.h"

#include "sxhkd.h"
#include "parse.h"

void setUp(void) {
    setup();
}

void tearDown(void) {
    cleanup();
}

void load_config_from_string(char *cfg) {
    /* load_config(filename) opens a file descriptor for filename, or
     * uses stdin if the filename is "-". In order to pass our own
     * string, we pipe the string into standard input.
     */
    int pid;
    int p[2];
    if (pipe(p) == -1)
        exit(errno);
    dup2(p[0], STDIN_FILENO);
    if( (pid = fork()) == 0) {
        dup2(p[1], STDOUT_FILENO);
        printf("%s", cfg);
        exit(EXIT_SUCCESS);
    }
    close(p[1]);
    load_config("-");
}

/**
 * Passing a configuration file of '-' should load configuration from
 * standard input.
 */
void test_load_from_stdin(void) {
    char cfg[] = "{a}\n  echo a\n";
    hotkey_t *hotkey;
    load_config_from_string(cfg);
    hotkey = find_hotkey(0x61, XCB_NONE, 0, XCB_KEY_PRESS, false);
    TEST_ASSERT_NOT_NULL(hotkey);
    TEST_ASSERT_EQUAL_STRING("echo a", hotkey->command);
}

/**
 * A hotkey followed by "[%s]" should be parsed correctly.
 */
void test_process_group(void) {
    char chain[] = "{a} [A]";
    char command[] = "echo a";
    hotkey_t *hotkey;
    process_hotkey(chain, command);
    hotkey = find_hotkey(0x61, XCB_NONE, 0, XCB_KEY_PRESS, false);
    TEST_ASSERT_NOT_NULL(hotkey);
    TEST_ASSERT_EQUAL_STRING("echo a", hotkey->command);
}

/**
 * extract_group(group, hk_string) should copy the trailing string
 * formatted [%s] into group and truncate hk_string appropriately. If no
 * trailing [%s] is found, an empty group should be returned.
 *
 * Should return true on success and false on failure.
 */
bool extract_group(char *group, char *hk_string);
void test_extract_group(void) {
    char hk_strings[4][32] = {
        "{a} [A]",
        "ctrl+b [A]",
        "ctrl+b; {a} [foo]",
        "ctrl+b",
    };
    char expected_grp[4][4] = { "A", "A", "foo", ""};
    char expected_hk[4][32] = {
        "{a}",
        "ctrl+b",
        "ctrl+b; {a}",
        "ctrl+b",
    };
    char group[4] = {'\0'};
    size_t i;
    for (i = 0; i < 4; i++) {
        TEST_ASSERT(extract_group(group, hk_strings[i]));
        TEST_ASSERT_EQUAL_STRING(expected_grp[i], group);
        TEST_ASSERT_EQUAL_STRING(expected_hk[i], hk_strings[i]);
    }
}

/**
 * add_hotkey_group should add hk to the group identified by grp_key,
 * creating a the group if necessary.
 */
void add_hotkey_group(hotkey_t *hk, char *grp_key);
void test_add_hotkey_group(void) {
    hotkey_t *hk = make_hotkey(NULL, "");
    TEST_ASSERT_NULL(groups_head);
    add_hotkey_group(hk, "foo");
    TEST_ASSERT_NOT_NULL(groups_head);
    TEST_ASSERT_EQUAL_STRING("foo", groups_head->key);
}

/**
 * A hotkey followed by "[%s]" should be assigned to group %s.
 */
void test_load_group(void) {
    char cfg[] = "{a} [A]\n"\
                  "  echo a\n\n"\
                  "{b} [A]"\
                  "\n  echo b\n\n"\
                  "{c} [B]\n"\
                  "  echo c\n\n"\
                  "{d}\n"\
                  "  echo d\n";
    group_t *group;
    load_config_from_string(cfg);
    for(group = find_group("A")->group; group != NULL; group = group->next)
        TEST_ASSERT_NOT_NULL(strstr("echo a echo b", group->hotkey->command));
    for(group = find_group("B")->group; group != NULL; group = group->next)
        TEST_ASSERT_NOT_NULL(strstr("echo c", group->hotkey->command));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_load_from_stdin);
    RUN_TEST(test_extract_group);
    RUN_TEST(test_add_hotkey_group);
    RUN_TEST(test_process_group);
    return UNITY_END();
}
