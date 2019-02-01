#ifndef TESTS_H
#define TESTS_H

#include "types.h"
#ifndef ASM
// test launcher
void launch_tests();
void keyboard_test(char c);
void rtc_test();
void test_term_read_write(uint8_t buf_size);

#endif
#endif /* TESTS_H */
