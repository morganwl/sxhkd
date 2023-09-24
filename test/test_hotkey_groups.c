/* Test toggling of hotkey groups */

#include <stdbool.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "types.h"

void test_find_hotkey(void)
{
    xcb_keysym_t keysym = 0x38;                 /* keysym for a */
    xcb_button_t button = 0;                    /* no button */
    hotkey_t *hotkey;

    hotkey = make_hotkey(make_chain(), "cd");
    add_hotkey(hotkey);

    TEST_ASSERT_NOT_NULL(find_hotkey(keysym, button, 0,
                XCB_KEY_RELEASE, false));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_find_hotkey);
    return UNITY_END();
}
