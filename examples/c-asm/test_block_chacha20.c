// Tests for chacha20_block assembly function
// Test vector from RFC 8439, Section 2.3.2

#include <stdint.h>

extern void chacha20_block(uint32_t *output_block, const uint32_t *initial_state);

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

// Test cases for chacha20_block

/*
 * RFC 8439 2.3.2 — ChaCha20 block function test vector
 *
 * Key (8 words):
 *   00010203 04050607 08090a0b 0c0d0e0f
 *   10111213 14151617 18191a1b 1c1d1e1f
 * Counter: 0x00000001
 * Nonce (3 words): 00000009 0000004a 00000000
 *
 * Initial state layout (16 words):
 *   state[0..3]   = constants "expa" "nd 3" "2-by" "te k"
 *   state[4..11]  = key
 *   state[12]     = counter
 *   state[13..15] = nonce
 */
static void test_rfc8439_block(void) {
    print_string("Test 1: RFC 8439 2.3.2 ChaCha20 block vector\n");

    static const uint32_t initial_state[16] = {
        /* constants */
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
        /* key */
        0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
        0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
        /* counter */
        0x00000001,
        /* nonce */
        0x09000000, 0x4a000000, 0x00000000
    };

    /* Expected output (RFC 8439 2.3.2) */
    static const uint32_t expected[16] = {
        0xe4e7f110, 0x15593bd1, 0x1fdd0f50, 0xc47120a3,
        0xc7f4d1c7, 0x0368c033, 0x9aaa2204, 0x4e6cd4c3,
        0x466482d2, 0x09aa9f07, 0x05d7c214, 0xa2028bd9,
        0xd19c12b5, 0xb94e16de, 0xe883d0cb, 0x4e3c50a2
    };

    print_string("  inputs:   (initial state)\n");
    for (int i = 0; i < 16; i += 4) {
        print_string("    ");
        print_hex(initial_state[i]);     print_string(" ");
        print_hex(initial_state[i + 1]); print_string(" ");
        print_hex(initial_state[i + 2]); print_string(" ");
        print_hex(initial_state[i + 3]); print_string("\n");
    }

    print_string("  expected: (output block)\n");
    for (int i = 0; i < 16; i += 4) {
        print_string("    ");
        print_hex(expected[i]);     print_string(" ");
        print_hex(expected[i + 1]); print_string(" ");
        print_hex(expected[i + 2]); print_string(" ");
        print_hex(expected[i + 3]); print_string("\n");
    }

    uint32_t output[16];
    chacha20_block(output, initial_state);

    check("output[0]  == 0xe4e7f110", output[0],  0xe4e7f110);
    check("output[1]  == 0x15593bd1", output[1],  0x15593bd1);
    check("output[2]  == 0x1fdd0f50", output[2],  0x1fdd0f50);
    check("output[3]  == 0xc47120a3", output[3],  0xc47120a3);
    check("output[4]  == 0xc7f4d1c7", output[4],  0xc7f4d1c7);
    check("output[5]  == 0x0368c033", output[5],  0x0368c033);
    check("output[6]  == 0x9aaa2204", output[6],  0x9aaa2204);
    check("output[7]  == 0x4e6cd4c3", output[7],  0x4e6cd4c3);
    check("output[8]  == 0x466482d2", output[8],  0x466482d2);
    check("output[9]  == 0x09aa9f07", output[9],  0x09aa9f07);
    check("output[10] == 0x05d7c214", output[10], 0x05d7c214);
    check("output[11] == 0xa2028bd9", output[11], 0xa2028bd9);
    check("output[12] == 0xd19c12b5", output[12], 0xd19c12b5);
    check("output[13] == 0xb94e16de", output[13], 0xb94e16de);
    check("output[14] == 0xe883d0cb", output[14], 0xe883d0cb);
    check("output[15] == 0x4e3c50a2", output[15], 0x4e3c50a2);
}


void main(void) {
    print_string("------ chacha20_block tests ------\n");

    test_rfc8439_block();

    print_string("------------------------------------\n");
    print_string("Results: ");

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

    // Signal QEMU to exit via SiFive test device (virt machine: 0x100000)
    // Writing 0x5555 = pass, 0x3333 = fail
    volatile uint32_t *qemu_exit = (volatile uint32_t *)0x100000;
    *qemu_exit = (tests_passed == tests_run) ? 0x5555 : 0x3333;

    while (1) {
        __asm__ volatile("nop");
    }
}
