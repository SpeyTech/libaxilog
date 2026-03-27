/**
 * @file test_convert.c
 * @brief Unit Tests for DVM Q16.16 Conversion Primitives
 *
 * Copyright (c) 2026 The Murray Family Innovation Trust
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DVEC: v1.3
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * @traceability SRS-005-SHALL-005, SRS-005-SHALL-006, SRS-005-SHALL-007,
 *               SRS-005-SHALL-068
 */

#include <stdio.h>
#include <stdlib.h>
#include "axilog/types.h"
#include "axilog/convert.h"
#include "../golden/vectors.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        printf("  [TEST] %s... ", #name); \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASS\n"); \
    } while(0)

#define FAIL(msg) \
    do { \
        printf("FAIL: %s\n", msg); \
    } while(0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            FAIL(msg); \
            return; \
        } \
    } while(0)

/* ========================================================================
 * Integer to Q16.16 Tests
 * SRS-005-SHALL-005: Integer to Q16.16 conversion
 * ======================================================================== */

static void test_int_to_q16_positive(void)
{
    TEST(int_to_q16_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_int_to_q16(1, &faults);
    ASSERT(result == 65536, "1 should convert to 65536");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_int_to_q16_negative(void)
{
    TEST(int_to_q16_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_int_to_q16(-1, &faults);
    ASSERT(result == -65536, "-1 should convert to -65536");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_int_to_q16_zero(void)
{
    TEST(int_to_q16_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_int_to_q16(0, &faults);
    ASSERT(result == 0, "0 should convert to 0");
    PASS();
}

static void test_int_to_q16_max(void)
{
    TEST(int_to_q16_max);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* int16_t max = 32767 → Q16.16 = 32767 * 65536 = 2147418112 */
    q16_16_t result = ax_int_to_q16(32767, &faults);
    ASSERT(result == 2147418112, "32767 should convert correctly");
    ASSERT(!ct_fault_any(&faults), "No fault for int16_t max");
    PASS();
}

static void test_int32_to_q16_overflow(void)
{
    TEST(int32_to_q16_overflow);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 32768 * 65536 > INT32_MAX */
    q16_16_t result = ax_int32_to_q16(32768, &faults);
    ASSERT(result == INT32_MAX, "32768 should overflow");
    ASSERT(faults.overflow == 1, "Overflow flag should be set");
    PASS();
}

/* ========================================================================
 * Q16.16 to Integer Tests
 * SRS-005-SHALL-006: Truncation toward zero
 * SRS-005-SHALL-068: No signed right-shift
 * ======================================================================== */

static void test_q16_to_int_exact(void)
{
    TEST(q16_to_int_exact);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    int32_t result = ax_q16_to_int(65536, &faults);
    ASSERT(result == 1, "65536 (1.0) should convert to 1");
    PASS();
}

static void test_q16_to_int_truncate_positive(void)
{
    TEST(q16_to_int_truncate_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 1.75 = 114688 → should truncate to 1 */
    int32_t result = ax_q16_to_int(114688, &faults);
    ASSERT(result == 1, "1.75 should truncate to 1");
    PASS();
}

static void test_q16_to_int_truncate_negative(void)
{
    TEST(q16_to_int_truncate_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* -1.75 = -114688 → should truncate to -1 (toward zero) */
    int32_t result = ax_q16_to_int(-114688, &faults);
    ASSERT(result == -1, "-1.75 should truncate to -1");
    PASS();
}

/* ========================================================================
 * Round-to-Nearest-Even Tests
 * SRS-005-SHALL-007: RNE rounding
 * ======================================================================== */

static void test_rne_half_to_even(void)
{
    TEST(rne_half_to_even);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 1.5 = 98304 → should round to 2 (1 is odd) */
    int32_t result = ax_q16_to_int_rne(98304, &faults);
    ASSERT(result == 2, "1.5 should round to 2 (banker's rounding)");
    PASS();
}

static void test_rne_half_stays_even(void)
{
    TEST(rne_half_stays_even);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 2.5 = 163840 → should round to 2 (2 is even) */
    int32_t result = ax_q16_to_int_rne(163840, &faults);
    ASSERT(result == 2, "2.5 should round to 2 (banker's rounding)");
    PASS();
}

static void test_rne_below_half(void)
{
    TEST(rne_below_half);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 1.25 = 81920 → should round to 1 */
    int32_t result = ax_q16_to_int_rne(81920, &faults);
    ASSERT(result == 1, "1.25 should round to 1");
    PASS();
}

static void test_rne_above_half(void)
{
    TEST(rne_above_half);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 1.75 = 114688 → should round to 2 */
    int32_t result = ax_q16_to_int_rne(114688, &faults);
    ASSERT(result == 2, "1.75 should round to 2");
    PASS();
}

static void test_rne_negative(void)
{
    TEST(rne_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* -1.5 = -98304 → should round to -2 (-1 is odd) */
    int32_t result = ax_q16_to_int_rne(-98304, &faults);
    ASSERT(result == -2, "-1.5 should round to -2 (banker's rounding)");
    PASS();
}

static void test_rne_golden_vectors(void)
{
    TEST(rne_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < CONVERT_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);

        int32_t trunc = ax_q16_to_int(CONVERT_VECTORS[i].x, &faults);
        int32_t rne = ax_q16_to_int_rne(CONVERT_VECTORS[i].x, &faults);

        if (trunc != CONVERT_VECTORS[i].expected_trunc) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: trunc expected %d, got %d",
                     i, CONVERT_VECTORS[i].expected_trunc, trunc);
            FAIL(msg);
            return;
        }
        if (rne != CONVERT_VECTORS[i].expected_rne) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: RNE expected %d, got %d",
                     i, CONVERT_VECTORS[i].expected_rne, rne);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Fractional Extraction Tests
 * ======================================================================== */

static void test_frac_positive(void)
{
    TEST(frac_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 1.25 = 81920 → frac = 0.25 = 16384 */
    q16_16_t result = ax_q16_frac(81920, &faults);
    ASSERT(result == 16384, "frac(1.25) should be 0.25");
    PASS();
}

static void test_frac_negative(void)
{
    TEST(frac_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* -1.25 = -81920 → frac should be positive: 0.75 = 49152 */
    q16_16_t result = ax_q16_frac(-81920, &faults);
    ASSERT(result == 49152, "frac(-1.25) should be 0.75");
    PASS();
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void)
{
    printf("======================================================================\n");
    printf("libaxilog — Conversion Tests\n");
    printf("SRS-005 v1.1 Verification\n");
    printf("======================================================================\n\n");

    /* Integer to Q16.16 */
    printf("[SUITE] Integer to Q16.16 (SRS-005-SHALL-005)\n");
    test_int_to_q16_positive();
    test_int_to_q16_negative();
    test_int_to_q16_zero();
    test_int_to_q16_max();
    test_int32_to_q16_overflow();

    /* Q16.16 to Integer */
    printf("\n[SUITE] Q16.16 to Integer (SRS-005-SHALL-006, 068)\n");
    test_q16_to_int_exact();
    test_q16_to_int_truncate_positive();
    test_q16_to_int_truncate_negative();

    /* Round-to-Nearest-Even */
    printf("\n[SUITE] Round-to-Nearest-Even (SRS-005-SHALL-007)\n");
    test_rne_half_to_even();
    test_rne_half_stays_even();
    test_rne_below_half();
    test_rne_above_half();
    test_rne_negative();
    test_rne_golden_vectors();

    /* Fractional Extraction */
    printf("\n[SUITE] Fractional Extraction\n");
    test_frac_positive();
    test_frac_negative();

    /* Summary */
    printf("\n======================================================================\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("======================================================================\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
