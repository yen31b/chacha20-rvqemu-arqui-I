// Tests for chacha20_quarter_round assembly function
// Test vectors from RFC 8439, Section 2.1.1

#include <stdint.h>

// Assembly function declaration
extern void chacha20_quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

static void print_char(char c) {
    volatile char *uart = (volatile char *)0x10000000;
    *uart = c;
}

static void print_string(const char *str) {
    while (*str) {
        print_char(*str++);
    }
}

static void print_hex(uint32_t val) {
    const char hex[] = "0123456789abcdef";
    print_string("0x");
    for (int i = 7; i >= 0; i--) {
        print_char(hex[(val >> (i * 4)) & 0xF]);
    }
}

// --- Test framework ---

static int tests_run    = 0;
static int tests_passed = 0;

static void check(const char *label, uint32_t got, uint32_t expected) {
    tests_run++;
    if (got == expected) {
        tests_passed++;
        print_string("  [PASS] ");
        print_string(label);
        print_string("\n");
    } else {
        print_string("  [FAIL] ");
        print_string(label);
        print_string("\n         got=");
        print_hex(got);
        print_string("  expected=");
        print_hex(expected);
        print_string("\n");
    }
}

// Test cases:

/*
 * Test 1 Verify arithmetic works as expected
 * RFC 8439 2.1.1. Test Vector for the ChaCha Quarter Round
 *
 * Input:  a=0x11111111  b=0x01020304  c=0x9b8d6f43  d=0x01234567
 * Output: a=0xea2a92f4  b=0xcb1cf8ce  c=0x4581472e  d=0x5881c4bb
 */
static void test_rfc8439_basic(void) {
    print_string("Test 1: RFC 8439 2.1.1 basic vector\n");

    uint32_t a = 0x11111111;
    uint32_t b = 0x01020304;
    uint32_t c = 0x9b8d6f43;
    uint32_t d = 0x01234567;

    print_string("  inputs:   a="); print_hex(a);
    print_string(" b=");            print_hex(b);
    print_string(" c=");            print_hex(c);
    print_string(" d=");            print_hex(d);
    print_string("\n");
    print_string("  expected: a=0xea2a92f4 b=0xcb1cf8ce c=0x4581472e d=0x5881c4bb\n");
    print_string(" Results:\n");
    print_string(" \n");

    chacha20_quarter_round(&a, &b, &c, &d);

    check("a == 0xea2a92f4", a, 0xea2a92f4);
    check("b == 0xcb1cf8ce", b, 0xcb1cf8ce);
    check("c == 0x4581472e", c, 0x4581472e);
    check("d == 0x5881c4bb", d, 0x5881c4bb);
}

/*
 * Test 2: RFC 8439 2.2.1 Test Vector for the Quarter Round on the ChaCha State
 *
 * Full input state:
 *   879531e0  c5ecf37d  516461b1  c9a62f8a
 *   44c20ef3  3390af7f  d9fc690b  2a5f714c
 *   53372767  b00a5631  974c541a  359e9963
 *   5c971061  3d631689  2098d9d6  91dbd320
 *
 * Positions used: a=state[2], b=state[7], c=state[8], d=state[13]
 *   Input:  a=0x516461b1  b=0x2a5f714c  c=0x53372767  d=0x3d631689
 *   Output: a=0xbdb886dc  b=0xcfacafd2  c=0xe46bea80  d=0xccc07c79
 */
static void test_rfc8439_state(void) {
    print_string("Test 2: RFC 8439 2.2.1 QUARTERROUND(2,7,8,13) on ChaCha state\n");

    uint32_t a = 0x516461b1;  /* state[2]  */
    uint32_t b = 0x2a5f714c;  /* state[7]  */
    uint32_t c = 0x53372767;  /* state[8]  */
    uint32_t d = 0x3d631689;  /* state[13] */

    print_string("  inputs:   a="); print_hex(a);
    print_string(" b=");            print_hex(b);
    print_string(" c=");            print_hex(c);
    print_string(" d=");            print_hex(d);
    print_string("\n");
    print_string("  expected: a=0xbdb886dc b=0xcfacafd2 c=0xe46bea80 d=0xccc07c79\n");
    print_string(" Results:\n");
    print_string(" \n");

    chacha20_quarter_round(&a, &b, &c, &d);

    check("a == 0xbdb886dc", a, 0xbdb886dc);
    check("b == 0xcfacafd2", b, 0xcfacafd2);
    check("c == 0xe46bea80", c, 0xe46bea80);
    check("d == 0xccc07c79", d, 0xccc07c79);
}

void main(void) {
    print_string("------- TESTS for chacha20_quarter_round -------\n");

    test_rfc8439_basic();
    test_rfc8439_state();

    print_string("------------------------------------\n");
    print_string("Results: ");

    // Print passed count
    char passed_digit = '0' + (char)tests_passed;
    print_char(passed_digit);
    print_string("/");
    char total_digit = '0' + (char)tests_run;
    print_char(total_digit);
    print_string(" tests passed\n");

    if (tests_passed == tests_run) {
        print_string("ALL TESTS PASSED\n");
    } else {
        print_string("SOME TESTS FAILED\n");
    }

    while (1) {
        __asm__ volatile("nop");
    }
}
