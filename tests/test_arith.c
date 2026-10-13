/**
 * @file test_arith.c
 * @brief Unit Tests for DVM Q16.16 Arithmetic Primitives
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * DVEC: v1.3
 * SRS: SRS-005
 *
 * @traceability SRS-005-SHALL-010, SRS-005-SHALL-011, SRS-005-SHALL-012,
 *               SRS-005-SHALL-013, SRS-005-SHALL-014, SRS-005-SHALL-015,
 *               SRS-005-SHALL-016, SRS-005-SHALL-017, SRS-005-SHALL-018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "axilog/types.h"
#include "axilog/arith.h"
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
 * Addition Tests
 * SRS-005-SHALL-010: Saturated addition
 * ======================================================================== */

static void test_add_normal(void)
{
    TEST(add_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_add_q16(65536, 65536, &faults);
    ASSERT(result == 131072, "1.0 + 1.0 should equal 2.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_add_overflow(void)
{
    TEST(add_overflow);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_add_q16(INT32_MAX, 1, &faults);
    ASSERT(result == INT32_MAX, "Overflow should saturate to MAX");
    ASSERT(faults.overflow == 1, "Overflow flag should be set");
    PASS();
}

static void test_add_underflow(void)
{
    TEST(add_underflow);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_add_q16(INT32_MIN, -1, &faults);
    ASSERT(result == INT32_MIN, "Underflow should saturate to MIN");
    ASSERT(faults.underflow == 1, "Underflow flag should be set");
    PASS();
}

static void test_add_golden_vectors(void)
{
    TEST(add_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < ADD_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_add_q16(ADD_VECTORS[i].a, ADD_VECTORS[i].b, &faults);

        if (result != ADD_VECTORS[i].expected) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected %d, got %d",
                     i, ADD_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
        if (faults.overflow != ADD_VECTORS[i].expect_overflow) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: overflow mismatch", i);
            FAIL(msg);
            return;
        }
        if (faults.underflow != ADD_VECTORS[i].expect_underflow) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: underflow mismatch", i);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Subtraction Tests
 * SRS-005-SHALL-011: Saturated subtraction
 * ======================================================================== */

static void test_sub_normal(void)
{
    TEST(sub_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_sub_q16(131072, 65536, &faults);
    ASSERT(result == 65536, "2.0 - 1.0 should equal 1.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_sub_golden_vectors(void)
{
    TEST(sub_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < SUB_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_sub_q16(SUB_VECTORS[i].a, SUB_VECTORS[i].b, &faults);

        if (result != SUB_VECTORS[i].expected) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected %d, got %d",
                     i, SUB_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Multiplication Tests
 * SRS-005-SHALL-012: Saturated multiplication
 * SRS-005-SHALL-067: Truncation toward zero
 * ======================================================================== */

static void test_mul_normal(void)
{
    TEST(mul_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_mul_q16(65536, 65536, &faults);
    ASSERT(result == 65536, "1.0 * 1.0 should equal 1.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_mul_half(void)
{
    TEST(mul_half);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* 0.5 * 0.5 = 0.25 */
    q16_16_t result = ax_mul_q16(32768, 32768, &faults);
    ASSERT(result == 16384, "0.5 * 0.5 should equal 0.25");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_mul_overflow(void)
{
    TEST(mul_overflow);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_mul_q16(INT32_MAX, 131072, &faults);
    ASSERT(result == INT32_MAX, "Overflow should saturate to MAX");
    ASSERT(faults.overflow == 1, "Overflow flag should be set");
    PASS();
}

static void test_mul_golden_vectors(void)
{
    TEST(mul_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < MUL_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_mul_q16(MUL_VECTORS[i].a, MUL_VECTORS[i].b, &faults);

        if (result != MUL_VECTORS[i].expected) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected %d, got %d",
                     i, MUL_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Division Tests
 * SRS-005-SHALL-013: Saturated division
 * SRS-005-SHALL-069: INT32_MIN / -1 → overflow
 * ======================================================================== */

static void test_div_normal(void)
{
    TEST(div_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_div_q16(65536, 65536, &faults);
    ASSERT(result == 65536, "1.0 / 1.0 should equal 1.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_div_by_zero(void)
{
    TEST(div_by_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_div_q16(65536, 0, &faults);
    ASSERT(result == 0, "Division by zero should return 0");
    ASSERT(faults.div_zero == 1, "div_zero flag should be set");
    PASS();
}

static void test_div_golden_vectors(void)
{
    TEST(div_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < DIV_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_div_q16(DIV_VECTORS[i].a, DIV_VECTORS[i].b, &faults);

        if (result != DIV_VECTORS[i].expected) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected %d, got %d",
                     i, DIV_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
        if (faults.div_zero != DIV_VECTORS[i].expect_div_zero) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: div_zero mismatch", i);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Negation Tests
 * SRS-005-SHALL-014: Negation with saturation
 * ======================================================================== */

static void test_neg_normal(void)
{
    TEST(neg_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_neg_q16(65536, &faults);
    ASSERT(result == -65536, "-1.0 expected");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_neg_min(void)
{
    TEST(neg_min);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* -INT32_MIN overflows */
    q16_16_t result = ax_neg_q16(INT32_MIN, &faults);
    ASSERT(result == INT32_MAX, "Negation of MIN should saturate to MAX");
    ASSERT(faults.overflow == 1, "Overflow flag should be set");
    PASS();
}

/* ========================================================================
 * Absolute Value Tests
 * SRS-005-SHALL-015: Absolute value with saturation
 * ======================================================================== */

static void test_abs_positive(void)
{
    TEST(abs_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_abs_q16(65536, &faults);
    ASSERT(result == 65536, "|1.0| = 1.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_abs_negative(void)
{
    TEST(abs_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_abs_q16(-65536, &faults);
    ASSERT(result == 65536, "|-1.0| = 1.0");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_abs_min(void)
{
    TEST(abs_min);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_abs_q16(INT32_MIN, &faults);
    ASSERT(result == INT32_MAX, "|MIN| should saturate to MAX");
    ASSERT(faults.overflow == 1, "Overflow flag should be set");
    PASS();
}

/* ========================================================================
 * Min / Max / Clamp Tests
 * SRS-005-SHALL-016, 017, 018
 * ======================================================================== */

static void test_min(void)
{
    TEST(min);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    ASSERT(ax_min_q16(65536, 131072, &faults) == 65536, "min(1, 2) = 1");
    ASSERT(ax_min_q16(-65536, 65536, &faults) == -65536, "min(-1, 1) = -1");
    PASS();
}

static void test_max(void)
{
    TEST(max);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    ASSERT(ax_max_q16(65536, 131072, &faults) == 131072, "max(1, 2) = 2");
    ASSERT(ax_max_q16(-65536, 65536, &faults) == 65536, "max(-1, 1) = 1");
    PASS();
}

static void test_clamp_normal(void)
{
    TEST(clamp_normal);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    ASSERT(ax_clamp_q16(65536, 0, 131072, &faults) == 65536, "1 in [0,2] = 1");
    ASSERT(ax_clamp_q16(-65536, 0, 131072, &faults) == 0, "-1 clamped to 0");
    ASSERT(ax_clamp_q16(262144, 0, 131072, &faults) == 131072, "4 clamped to 2");
    PASS();
}

static void test_clamp_invalid_bounds(void)
{
    TEST(clamp_invalid_bounds);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* SRS-005-SHALL-070: Invalid bounds behaviour */
    q16_16_t result = ax_clamp_q16(65536, 131072, 0, &faults);  /* lo > hi */
    ASSERT(result == 131072, "Invalid bounds returns lo");
    ASSERT(faults.domain == 1, "Domain fault should be set");
    PASS();
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void)
{
    printf("======================================================================\n");
    printf("libaxilog — Arithmetic Tests\n");
    printf("SRS-005 v1.1 Verification\n");
    printf("======================================================================\n\n");

    /* Addition */
    printf("[SUITE] Addition (SRS-005-SHALL-010)\n");
    test_add_normal();
    test_add_overflow();
    test_add_underflow();
    test_add_golden_vectors();

    /* Subtraction */
    printf("\n[SUITE] Subtraction (SRS-005-SHALL-011)\n");
    test_sub_normal();
    test_sub_golden_vectors();

    /* Multiplication */
    printf("\n[SUITE] Multiplication (SRS-005-SHALL-012, 067)\n");
    test_mul_normal();
    test_mul_half();
    test_mul_overflow();
    test_mul_golden_vectors();

    /* Division */
    printf("\n[SUITE] Division (SRS-005-SHALL-013, 069)\n");
    test_div_normal();
    test_div_by_zero();
    test_div_golden_vectors();

    /* Negation */
    printf("\n[SUITE] Negation (SRS-005-SHALL-014)\n");
    test_neg_normal();
    test_neg_min();

    /* Absolute Value */
    printf("\n[SUITE] Absolute Value (SRS-005-SHALL-015)\n");
    test_abs_positive();
    test_abs_negative();
    test_abs_min();

    /* Min / Max / Clamp */
    printf("\n[SUITE] Min/Max/Clamp (SRS-005-SHALL-016, 017, 018, 070)\n");
    test_min();
    test_max();
    test_clamp_normal();
    test_clamp_invalid_bounds();

    /* Summary */
    printf("\n======================================================================\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("======================================================================\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
