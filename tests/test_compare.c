/**
 * @file test_compare.c
 * @brief Unit Tests for DVM Q16.16 Comparison Primitives
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * DVEC: v1.3
 * SRS: SRS-005
 *
 * @traceability SRS-005-SHALL-019, SRS-005-SHALL-020, SRS-005-SHALL-021,
 *               SRS-005-SHALL-022, SRS-005-SHALL-023, SRS-005-SHALL-024,
 *               SRS-005-SHALL-025, SRS-005-SHALL-026, SRS-005-SHALL-027
 */

#include <stdio.h>
#include <stdlib.h>
#include "axilog/types.h"
#include "axilog/compare.h"

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
 * Equality Tests
 * SRS-005-SHALL-021: Equality as exact bit comparison
 * ======================================================================== */

static void test_eq_same(void)
{
    TEST(eq_same);
    ASSERT(ax_eq_q16(65536, 65536) == true, "1.0 == 1.0");
    ASSERT(ax_eq_q16(0, 0) == true, "0 == 0");
    ASSERT(ax_eq_q16(-65536, -65536) == true, "-1.0 == -1.0");
    PASS();
}

static void test_eq_different(void)
{
    TEST(eq_different);
    ASSERT(ax_eq_q16(65536, 65537) == false, "1.0 != 1.0+eps");
    ASSERT(ax_eq_q16(65536, -65536) == false, "1.0 != -1.0");
    PASS();
}

static void test_ne(void)
{
    TEST(ne);
    ASSERT(ax_ne_q16(65536, 65537) == true, "1.0 != 1.0+eps");
    ASSERT(ax_ne_q16(65536, 65536) == false, "1.0 == 1.0");
    PASS();
}

/* ========================================================================
 * Relational Tests
 * SRS-005-SHALL-022, 023, 024, 025: Relational comparisons
 * SRS-005-SHALL-019: Signed comparison
 * ======================================================================== */

static void test_lt(void)
{
    TEST(lt);
    ASSERT(ax_lt_q16(0, 65536) == true, "0 < 1.0");
    ASSERT(ax_lt_q16(-65536, 0) == true, "-1.0 < 0");
    ASSERT(ax_lt_q16(65536, 65536) == false, "1.0 not < 1.0");
    ASSERT(ax_lt_q16(65536, 0) == false, "1.0 not < 0");
    PASS();
}

static void test_gt(void)
{
    TEST(gt);
    ASSERT(ax_gt_q16(65536, 0) == true, "1.0 > 0");
    ASSERT(ax_gt_q16(0, -65536) == true, "0 > -1.0");
    ASSERT(ax_gt_q16(65536, 65536) == false, "1.0 not > 1.0");
    ASSERT(ax_gt_q16(0, 65536) == false, "0 not > 1.0");
    PASS();
}

static void test_le(void)
{
    TEST(le);
    ASSERT(ax_le_q16(0, 65536) == true, "0 <= 1.0");
    ASSERT(ax_le_q16(65536, 65536) == true, "1.0 <= 1.0");
    ASSERT(ax_le_q16(65536, 0) == false, "1.0 not <= 0");
    PASS();
}

static void test_ge(void)
{
    TEST(ge);
    ASSERT(ax_ge_q16(65536, 0) == true, "1.0 >= 0");
    ASSERT(ax_ge_q16(65536, 65536) == true, "1.0 >= 1.0");
    ASSERT(ax_ge_q16(0, 65536) == false, "0 not >= 1.0");
    PASS();
}

/* ========================================================================
 * Sign Classification Tests
 * SRS-005-SHALL-026: Deterministic sign classification
 * ======================================================================== */

static void test_sign_positive(void)
{
    TEST(sign_positive);
    ASSERT(ax_sign_q16(65536) == AX_SIGN_POSITIVE, "1.0 is positive");
    ASSERT(ax_sign_q16(1) == AX_SIGN_POSITIVE, "eps is positive");
    ASSERT(ax_sign_q16(INT32_MAX) == AX_SIGN_POSITIVE, "MAX is positive");
    PASS();
}

static void test_sign_negative(void)
{
    TEST(sign_negative);
    ASSERT(ax_sign_q16(-65536) == AX_SIGN_NEGATIVE, "-1.0 is negative");
    ASSERT(ax_sign_q16(-1) == AX_SIGN_NEGATIVE, "-eps is negative");
    ASSERT(ax_sign_q16(INT32_MIN) == AX_SIGN_NEGATIVE, "MIN is negative");
    PASS();
}

static void test_sign_zero(void)
{
    TEST(sign_zero);
    ASSERT(ax_sign_q16(0) == AX_SIGN_ZERO, "0 is zero");
    PASS();
}

/* ========================================================================
 * Three-Way Comparison Tests
 * SRS-005-SHALL-027: Total ordering guarantee
 * ======================================================================== */

static void test_cmp(void)
{
    TEST(cmp);
    ASSERT(ax_cmp_q16(0, 65536) == -1, "0 < 1.0 → -1");
    ASSERT(ax_cmp_q16(65536, 65536) == 0, "1.0 == 1.0 → 0");
    ASSERT(ax_cmp_q16(65536, 0) == 1, "1.0 > 0 → 1");
    PASS();
}

/* ========================================================================
 * Predicate Tests
 * ======================================================================== */

static void test_is_zero(void)
{
    TEST(is_zero);
    ASSERT(ax_is_zero_q16(0) == true, "0 is zero");
    ASSERT(ax_is_zero_q16(1) == false, "eps is not zero");
    ASSERT(ax_is_zero_q16(-1) == false, "-eps is not zero");
    PASS();
}

static void test_is_positive(void)
{
    TEST(is_positive);
    ASSERT(ax_is_positive_q16(65536) == true, "1.0 is positive");
    ASSERT(ax_is_positive_q16(0) == false, "0 is not positive");
    ASSERT(ax_is_positive_q16(-65536) == false, "-1.0 is not positive");
    PASS();
}

static void test_is_negative(void)
{
    TEST(is_negative);
    ASSERT(ax_is_negative_q16(-65536) == true, "-1.0 is negative");
    ASSERT(ax_is_negative_q16(0) == false, "0 is not negative");
    ASSERT(ax_is_negative_q16(65536) == false, "1.0 is not negative");
    PASS();
}

/* ========================================================================
 * Total Ordering Tests
 * SRS-005-SHALL-027: Total ordering over all 2^32 values
 * ======================================================================== */

static void test_total_ordering_boundaries(void)
{
    TEST(total_ordering_boundaries);

    /* Verify signed ordering at boundaries */
    ASSERT(ax_lt_q16(INT32_MIN, INT32_MAX) == true, "MIN < MAX");
    ASSERT(ax_lt_q16(INT32_MIN, 0) == true, "MIN < 0");
    ASSERT(ax_lt_q16(0, INT32_MAX) == true, "0 < MAX");
    ASSERT(ax_lt_q16(-1, 0) == true, "-1 < 0");
    ASSERT(ax_lt_q16(0, 1) == true, "0 < 1");

    /* Verify transitivity */
    ASSERT(ax_lt_q16(INT32_MIN, -1) == true, "MIN < -1");
    ASSERT(ax_lt_q16(-1, 1) == true, "-1 < 1");
    /* Therefore MIN < 1 (transitivity) */
    ASSERT(ax_lt_q16(INT32_MIN, 1) == true, "MIN < 1 (transitive)");

    PASS();
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void)
{
    printf("======================================================================\n");
    printf("libaxilog — Comparison Tests\n");
    printf("SRS-005 v1.1 Verification\n");
    printf("======================================================================\n\n");

    /* Equality */
    printf("[SUITE] Equality (SRS-005-SHALL-021)\n");
    test_eq_same();
    test_eq_different();
    test_ne();

    /* Relational */
    printf("\n[SUITE] Relational (SRS-005-SHALL-022, 023, 024, 025)\n");
    test_lt();
    test_gt();
    test_le();
    test_ge();

    /* Sign Classification */
    printf("\n[SUITE] Sign Classification (SRS-005-SHALL-026)\n");
    test_sign_positive();
    test_sign_negative();
    test_sign_zero();

    /* Three-Way Comparison */
    printf("\n[SUITE] Three-Way Comparison (SRS-005-SHALL-027)\n");
    test_cmp();

    /* Predicates */
    printf("\n[SUITE] Predicates\n");
    test_is_zero();
    test_is_positive();
    test_is_negative();

    /* Total Ordering */
    printf("\n[SUITE] Total Ordering (SRS-005-SHALL-027)\n");
    test_total_ordering_boundaries();

    /* Summary */
    printf("\n======================================================================\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("======================================================================\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
