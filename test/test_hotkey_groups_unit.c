/* Test toggling of hotkey groups */

#include <stdbool.h>
#include <stdlib.h>

#include "unity.h"

#include "types.h"
#include "sxhkd.h"

void setUp(void) {
    setup();
}

void tearDown(void) {
    cleanup();
}

/**
 * Creates a hotkey and then asserts that it can be found.
 */
void test_find_hotkey(void)
{
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_find_hotkey);
    return UNITY_END();
}
