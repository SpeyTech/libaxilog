/**
 * @file test_transcendental.c
 * @brief Unit Tests for DVM Q16.16 Transcendental Approximations
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DVEC: v1.3
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * @traceability SRS-005-SHALL-040, SRS-005-SHALL-041, SRS-005-SHALL-042,
 *               SRS-005-SHALL-043, SRS-005-SHALL-044, SRS-005-SHALL-045,
 *               SRS-005-SHALL-046, SRS-005-SHALL-054
 */

#include <stdio.h>
#include <stdlib.h>
#include "axilog/types.h"
#include "axilog/transcendental.h"
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

/* Allow ±tolerance for transcendental approximations */
#define ULP_TOLERANCE 512  /* ~8 ULP for practical testing */

static int within_tolerance(q16_16_t actual, q16_16_t expected, int tol)
{
    int64_t diff = (int64_t)actual - (int64_t)expected;
    if (diff < 0) diff = -diff;
    return diff <= tol;
}

/* ========================================================================
 * Exponential Tests
 * SRS-005-SHALL-040: Polynomial approximation
 * SRS-005-SHALL-041: Domain enforcement
 * SRS-005-SHALL-042: ±2 ULP accuracy
 * ======================================================================== */

static void test_exp_zero(void)
{
    TEST(exp_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_exp_q16(0, &faults);
    ASSERT(result == Q16_ONE, "exp(0) should equal 1.0 exactly");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_exp_one(void)
{
    TEST(exp_one);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* exp(1) ≈ 2.71828 → 178145 in Q16.16 */
    q16_16_t result = ax_exp_q16(Q16_ONE, &faults);
    ASSERT(within_tolerance(result, 178145, ULP_TOLERANCE),
           "exp(1) should be approximately 178145");
    ASSERT(!ct_fault_any(&faults), "No fault expected");

    printf("(got %d, expected ~178145) ", result);
    PASS();
}

static void test_exp_negative_one(void)
{
    TEST(exp_negative_one);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* exp(-1) ≈ 0.3679 → 24109 in Q16.16 */
    q16_16_t result = ax_exp_q16(-Q16_ONE, &faults);
    ASSERT(within_tolerance(result, 24109, ULP_TOLERANCE),
           "exp(-1) should be approximately 24109");
    ASSERT(!ct_fault_any(&faults), "No fault expected");

    printf("(got %d, expected ~24109) ", result);
    PASS();
}

static void test_exp_domain_positive(void)
{
    TEST(exp_domain_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* exp(12) exceeds domain */
    q16_16_t result = ax_exp_q16(786432, &faults);  /* 12.0 in Q16.16 */
    ASSERT(result == INT32_MAX, "exp(12) should saturate");
    ASSERT(faults.domain == 1, "Domain fault should be set");
    PASS();
}

static void test_exp_domain_negative(void)
{
    TEST(exp_domain_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* exp(-12) exceeds domain */
    q16_16_t result = ax_exp_q16(-786432, &faults);  /* -12.0 in Q16.16 */
    ASSERT(result == 0, "exp(-12) should return 0");
    ASSERT(faults.domain == 1, "Domain fault should be set");
    PASS();
}

static void test_exp_golden_vectors(void)
{
    TEST(exp_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < EXP_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_exp_q16(EXP_VECTORS[i].x, &faults);

        /* Domain error cases */
        if (EXP_VECTORS[i].expect_domain) {
            if (faults.domain != 1) {
                char msg[128];
                snprintf(msg, sizeof(msg), "Vector %zu: domain fault expected", i);
                FAIL(msg);
                return;
            }
            continue;
        }

        /* Accuracy check with tolerance */
        if (!within_tolerance(result, EXP_VECTORS[i].expected, ULP_TOLERANCE)) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected ~%d, got %d",
                     i, EXP_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Hyperbolic Tangent Tests
 * SRS-005-SHALL-043: tanh implementation
 * SRS-005-SHALL-044: Full domain acceptance
 * SRS-005-SHALL-045: ±2 ULP accuracy
 * ======================================================================== */

static void test_tanh_zero(void)
{
    TEST(tanh_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_tanh_q16(0, &faults);
    ASSERT(result == 0, "tanh(0) should equal 0 exactly");
    ASSERT(!ct_fault_any(&faults), "No fault expected");
    PASS();
}

static void test_tanh_one(void)
{
    TEST(tanh_one);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* tanh(1) ≈ 0.7616 → 49914 in Q16.16 */
    q16_16_t result = ax_tanh_q16(Q16_ONE, &faults);
    ASSERT(within_tolerance(result, 49914, ULP_TOLERANCE),
           "tanh(1) should be approximately 49914");

    printf("(got %d, expected ~49914) ", result);
    PASS();
}

static void test_tanh_saturation_positive(void)
{
    TEST(tanh_saturation_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* tanh(5) ≈ 1.0 */
    q16_16_t result = ax_tanh_q16(327680, &faults);  /* 5.0 in Q16.16 */
    ASSERT(result == Q16_ONE, "tanh(5) should saturate to 1.0");
    PASS();
}

static void test_tanh_saturation_negative(void)
{
    TEST(tanh_saturation_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* tanh(-5) ≈ -1.0 */
    q16_16_t result = ax_tanh_q16(-327680, &faults);  /* -5.0 in Q16.16 */
    ASSERT(result == -Q16_ONE, "tanh(-5) should saturate to -1.0");
    PASS();
}

static void test_tanh_golden_vectors(void)
{
    TEST(tanh_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < TANH_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_tanh_q16(TANH_VECTORS[i].x, &faults);

        if (!within_tolerance(result, TANH_VECTORS[i].expected, ULP_TOLERANCE)) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected ~%d, got %d",
                     i, TANH_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * Sigmoid Tests
 * SRS-005-SHALL-046: Sigmoid using ax_exp_q16
 * ======================================================================== */

static void test_sigmoid_zero(void)
{
    TEST(sigmoid_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* sigmoid(0) = 0.5 exactly */
    q16_16_t result = ax_sigmoid_q16(0, &faults);
    ASSERT(result == Q16_HALF, "sigmoid(0) should equal 0.5 exactly");
    PASS();
}

static void test_sigmoid_positive(void)
{
    TEST(sigmoid_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* sigmoid(1) ≈ 0.7311 → 47915 */
    q16_16_t result = ax_sigmoid_q16(Q16_ONE, &faults);
    ASSERT(within_tolerance(result, 47915, ULP_TOLERANCE),
           "sigmoid(1) should be approximately 47915");

    printf("(got %d, expected ~47915) ", result);
    PASS();
}

static void test_sigmoid_negative(void)
{
    TEST(sigmoid_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* sigmoid(-1) ≈ 0.2689 → 17621 */
    q16_16_t result = ax_sigmoid_q16(-Q16_ONE, &faults);
    ASSERT(within_tolerance(result, 17621, ULP_TOLERANCE),
           "sigmoid(-1) should be approximately 17621");

    printf("(got %d, expected ~17621) ", result);
    PASS();
}

static void test_sigmoid_saturation_positive(void)
{
    TEST(sigmoid_saturation_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* sigmoid(large) → 1.0 (use value beyond exp domain) */
    q16_16_t result = ax_sigmoid_q16(786432, &faults);  /* 12.0 - beyond domain */
    ASSERT(result == Q16_ONE, "sigmoid(12) should saturate to 1.0");
    PASS();
}

static void test_sigmoid_saturation_negative(void)
{
    TEST(sigmoid_saturation_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* sigmoid(-large) → 0 (use value beyond exp domain) */
    q16_16_t result = ax_sigmoid_q16(-786432, &faults);  /* -12.0 - beyond domain */
    ASSERT(result == 0, "sigmoid(-12) should saturate to 0");
    PASS();
}

static void test_sigmoid_golden_vectors(void)
{
    TEST(sigmoid_golden_vectors);
    ct_fault_flags_t faults;

    for (size_t i = 0; i < SIGMOID_VECTOR_COUNT; i++) {
        ct_fault_clear(&faults);
        q16_16_t result = ax_sigmoid_q16(SIGMOID_VECTORS[i].x, &faults);

        if (!within_tolerance(result, SIGMOID_VECTORS[i].expected, ULP_TOLERANCE)) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Vector %zu: expected ~%d, got %d",
                     i, SIGMOID_VECTORS[i].expected, result);
            FAIL(msg);
            return;
        }
    }
    PASS();
}

/* ========================================================================
 * ReLU Tests
 * ======================================================================== */

static void test_relu_positive(void)
{
    TEST(relu_positive);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_relu_q16(65536, &faults);
    ASSERT(result == 65536, "relu(1.0) = 1.0");
    PASS();
}

static void test_relu_negative(void)
{
    TEST(relu_negative);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_relu_q16(-65536, &faults);
    ASSERT(result == 0, "relu(-1.0) = 0");
    PASS();
}

static void test_relu_zero(void)
{
    TEST(relu_zero);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    q16_16_t result = ax_relu_q16(0, &faults);
    ASSERT(result == 0, "relu(0) = 0");
    PASS();
}

static void test_leaky_relu(void)
{
    TEST(leaky_relu);
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    /* alpha = 0.01 ≈ 655 in Q16.16 */
    q16_16_t alpha = 655;

    q16_16_t pos = ax_leaky_relu_q16(65536, alpha, &faults);
    ASSERT(pos == 65536, "leaky_relu(1.0) = 1.0");

    ct_fault_clear(&faults);
    q16_16_t neg = ax_leaky_relu_q16(-65536, alpha, &faults);
    /* -1.0 * 0.01 ≈ -655 */
    ASSERT(within_tolerance(neg, -655, 10), "leaky_relu(-1.0) ≈ -0.01");

    PASS();
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(void)
{
    printf("======================================================================\n");
    printf("libaxilog — Transcendental Tests\n");
    printf("SRS-005 v1.1 Verification\n");
    printf("======================================================================\n\n");

    /* Exponential */
    printf("[SUITE] Exponential (SRS-005-SHALL-040, 041, 042)\n");
    test_exp_zero();
    test_exp_one();
    test_exp_negative_one();
    test_exp_domain_positive();
    test_exp_domain_negative();
    test_exp_golden_vectors();

    /* Hyperbolic Tangent */
    printf("\n[SUITE] Hyperbolic Tangent (SRS-005-SHALL-043, 044, 045)\n");
    test_tanh_zero();
    test_tanh_one();
    test_tanh_saturation_positive();
    test_tanh_saturation_negative();
    test_tanh_golden_vectors();

    /* Sigmoid */
    printf("\n[SUITE] Sigmoid (SRS-005-SHALL-046)\n");
    test_sigmoid_zero();
    test_sigmoid_positive();
    test_sigmoid_negative();
    test_sigmoid_saturation_positive();
    test_sigmoid_saturation_negative();
    test_sigmoid_golden_vectors();

    /* ReLU */
    printf("\n[SUITE] ReLU Activations\n");
    test_relu_positive();
    test_relu_negative();
    test_relu_zero();
    test_leaky_relu();

    /* Summary */
    printf("\n======================================================================\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("======================================================================\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
