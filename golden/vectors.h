/**
 * @file vectors.h
 * @brief Golden Reference Vectors for L1 Primitives
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * These vectors are frozen at compile time and provide bit-identical
 * reference values for cross-platform verification.
 *
 * @traceability SRS-005-SHALL-055: Golden vector provision
 */

#ifndef AXILOG_GOLDEN_VECTORS_H
#define AXILOG_GOLDEN_VECTORS_H

#include "axilog/types.h"

/* ========================================================================
 * Addition Vectors
 * SRS-005-SHALL-055: Golden vectors for boundary values
 * ======================================================================== */

typedef struct {
    q16_16_t a;
    q16_16_t b;
    q16_16_t expected;
    uint8_t expect_overflow;
    uint8_t expect_underflow;
} add_vector_t;

static const add_vector_t ADD_VECTORS[] = {
    /* Normal cases */
    { 65536,      65536,      131072,     0, 0 },  /* 1.0 + 1.0 = 2.0 */
    { 0,          0,          0,          0, 0 },  /* 0 + 0 = 0 */
    { -65536,     65536,      0,          0, 0 },  /* -1.0 + 1.0 = 0 */
    { 32768,      32768,      65536,      0, 0 },  /* 0.5 + 0.5 = 1.0 */

    /* Overflow */
    { INT32_MAX,  1,          INT32_MAX,  1, 0 },  /* MAX + 1 → overflow */
    { INT32_MAX,  INT32_MAX,  INT32_MAX,  1, 0 },  /* MAX + MAX → overflow */

    /* Underflow */
    { INT32_MIN,  -1,         INT32_MIN,  0, 1 },  /* MIN + (-1) → underflow */
    { INT32_MIN,  INT32_MIN,  INT32_MIN,  0, 1 },  /* MIN + MIN → underflow */

    /* Boundary values */
    { INT32_MAX,  0,          INT32_MAX,  0, 0 },  /* MAX + 0 = MAX */
    { INT32_MIN,  0,          INT32_MIN,  0, 0 },  /* MIN + 0 = MIN */
    { 1,          -1,         0,          0, 0 },  /* EPS + (-EPS) = 0 */
};

#define ADD_VECTOR_COUNT (sizeof(ADD_VECTORS) / sizeof(ADD_VECTORS[0]))

/* ========================================================================
 * Subtraction Vectors
 * ======================================================================== */

typedef struct {
    q16_16_t a;
    q16_16_t b;
    q16_16_t expected;
    uint8_t expect_overflow;
    uint8_t expect_underflow;
} sub_vector_t;

static const sub_vector_t SUB_VECTORS[] = {
    /* Normal cases */
    { 131072,     65536,      65536,      0, 0 },  /* 2.0 - 1.0 = 1.0 */
    { 65536,      65536,      0,          0, 0 },  /* 1.0 - 1.0 = 0 */
    { 0,          65536,      -65536,     0, 0 },  /* 0 - 1.0 = -1.0 */

    /* Overflow */
    { INT32_MAX,  -1,         INT32_MAX,  1, 0 },  /* MAX - (-1) → overflow */
    { INT32_MAX,  INT32_MIN,  INT32_MAX,  1, 0 },  /* MAX - MIN → overflow */

    /* Underflow */
    { INT32_MIN,  1,          INT32_MIN,  0, 1 },  /* MIN - 1 → underflow */
    { INT32_MIN,  INT32_MAX,  INT32_MIN,  0, 1 },  /* MIN - MAX → underflow */
};

#define SUB_VECTOR_COUNT (sizeof(SUB_VECTORS) / sizeof(SUB_VECTORS[0]))

/* ========================================================================
 * Multiplication Vectors
 * SRS-005-SHALL-067: Truncation toward zero
 * ======================================================================== */

typedef struct {
    q16_16_t a;
    q16_16_t b;
    q16_16_t expected;
    uint8_t expect_overflow;
    uint8_t expect_underflow;
} mul_vector_t;

static const mul_vector_t MUL_VECTORS[] = {
    /* Normal cases */
    { 65536,      65536,      65536,      0, 0 },  /* 1.0 * 1.0 = 1.0 */
    { 131072,     131072,     262144,     0, 0 },  /* 2.0 * 2.0 = 4.0 */
    { 32768,      32768,      16384,      0, 0 },  /* 0.5 * 0.5 = 0.25 */
    { -65536,     65536,      -65536,     0, 0 },  /* -1.0 * 1.0 = -1.0 */
    { -65536,     -65536,     65536,      0, 0 },  /* -1.0 * -1.0 = 1.0 */

    /* Zero */
    { 0,          65536,      0,          0, 0 },  /* 0 * 1.0 = 0 */
    { 65536,      0,          0,          0, 0 },  /* 1.0 * 0 = 0 */

    /* Overflow */
    { INT32_MAX,  131072,     INT32_MAX,  1, 0 },  /* MAX * 2 → overflow */
    { 1048576,    1048576,    16777216,   0, 0 },  /* 16.0 * 16.0 = 256.0 (no overflow) */

    /* Unity cases */
    { 1,          65536,      1,          0, 0 },  /* EPS * 1.0 = EPS */
    { 65536,      1,          1,          0, 0 },  /* 1.0 * EPS = EPS */
};

#define MUL_VECTOR_COUNT (sizeof(MUL_VECTORS) / sizeof(MUL_VECTORS[0]))

/* ========================================================================
 * Division Vectors
 * SRS-005-SHALL-069: INT32_MIN / -1 → overflow
 * ======================================================================== */

typedef struct {
    q16_16_t a;
    q16_16_t b;
    q16_16_t expected;
    uint8_t expect_overflow;
    uint8_t expect_underflow;
    uint8_t expect_div_zero;
} div_vector_t;

static const div_vector_t DIV_VECTORS[] = {
    /* Normal cases */
    { 65536,      65536,      65536,      0, 0, 0 },  /* 1.0 / 1.0 = 1.0 */
    { 131072,     65536,      131072,     0, 0, 0 },  /* 2.0 / 1.0 = 2.0 */
    { 65536,      131072,     32768,      0, 0, 0 },  /* 1.0 / 2.0 = 0.5 */
    { -65536,     65536,      -65536,     0, 0, 0 },  /* -1.0 / 1.0 = -1.0 */

    /* Division by zero */
    { 65536,      0,          0,          0, 0, 1 },  /* 1.0 / 0 → div_zero */
    { 0,          0,          0,          0, 0, 1 },  /* 0 / 0 → div_zero */

    /* Overflow cases */
    { INT32_MAX,  1,          INT32_MAX,  1, 0, 0 },  /* MAX / EPS → overflow */

    /* Zero numerator */
    { 0,          65536,      0,          0, 0, 0 },  /* 0 / 1.0 = 0 */
};

#define DIV_VECTOR_COUNT (sizeof(DIV_VECTORS) / sizeof(DIV_VECTORS[0]))

/* ========================================================================
 * Exponential Vectors
 * SRS-005-SHALL-042: ±2 ULP accuracy
 * ======================================================================== */

typedef struct {
    q16_16_t x;
    q16_16_t expected;  /* Approximate, ±2 ULP */
    uint8_t expect_domain;
} exp_vector_t;

static const exp_vector_t EXP_VECTORS[] = {
    /* exp(0) = 1.0 exactly */
    { 0,          65536,      0 },

    /* exp(1) ≈ 2.71828... → 178145 in Q16.16 */
    { 65536,      178145,     0 },

    /* exp(-1) ≈ 0.36788... → 24109 in Q16.16 */
    { -65536,     24109,      0 },

    /* exp(2) ≈ 7.389... → 484249 in Q16.16 */
    { 131072,     484249,     0 },

    /* exp(-2) ≈ 0.1353... → 8869 in Q16.16 */
    { -131072,    8869,       0 },

    /* exp(0.5) ≈ 1.6487... → 108076 in Q16.16 */
    { 32768,      108076,     0 },

    /* Domain exceeded */
    { 786432,     INT32_MAX,  1 },  /* exp(12) → domain error */
    { -786432,    0,          1 },  /* exp(-12) → domain error */
};

#define EXP_VECTOR_COUNT (sizeof(EXP_VECTORS) / sizeof(EXP_VECTORS[0]))

/* ========================================================================
 * Hyperbolic Tangent Vectors
 * SRS-005-SHALL-045: ±2 ULP accuracy
 * ======================================================================== */

typedef struct {
    q16_16_t x;
    q16_16_t expected;  /* Approximate, ±2 ULP */
} tanh_vector_t;

static const tanh_vector_t TANH_VECTORS[] = {
    /* tanh(0) = 0 exactly */
    { 0,          0 },

    /* tanh(1) ≈ 0.7616... → 49914 in Q16.16 */
    { 65536,      49914 },

    /* tanh(-1) ≈ -0.7616... → -49914 in Q16.16 */
    { -65536,     -49914 },

    /* tanh(2) ≈ 0.9640... → 63178 in Q16.16 */
    { 131072,     63178 },

    /* tanh(4) ≈ 0.9993... ≈ 1.0 → 65536 (saturated) */
    { 262144,     65536 },

    /* tanh(-4) ≈ -0.9993... ≈ -1.0 → -65536 (saturated) */
    { -262144,    -65536 },

    /* tanh(0.5) ≈ 0.4621... → 30286 in Q16.16 */
    { 32768,      30286 },
};

#define TANH_VECTOR_COUNT (sizeof(TANH_VECTORS) / sizeof(TANH_VECTORS[0]))

/* ========================================================================
 * Sigmoid Vectors
 * ======================================================================== */

typedef struct {
    q16_16_t x;
    q16_16_t expected;  /* Approximate */
} sigmoid_vector_t;

static const sigmoid_vector_t SIGMOID_VECTORS[] = {
    /* sigmoid(0) = 0.5 exactly */
    { 0,          32768 },

    /* sigmoid(1) ≈ 0.7311... → 47915 in Q16.16 */
    { 65536,      47915 },

    /* sigmoid(-1) ≈ 0.2689... → 17621 in Q16.16 */
    { -65536,     17621 },

    /* sigmoid(4) ≈ 0.9820... → 64356 */
    { 262144,     64356 },

    /* sigmoid(-4) ≈ 0.0180... → 1180 */
    { -262144,    1180 },

    /* Large positive → 1.0 */
    { 720896,     65536 },

    /* Large negative → 0 */
    { -720896,    0 },
};

#define SIGMOID_VECTOR_COUNT (sizeof(SIGMOID_VECTORS) / sizeof(SIGMOID_VECTORS[0]))

/* ========================================================================
 * Conversion Vectors
 * SRS-005-SHALL-007: RNE rounding
 * ======================================================================== */

typedef struct {
    q16_16_t x;
    int32_t expected_trunc;  /* Truncation toward zero */
    int32_t expected_rne;    /* Round to nearest even */
} convert_vector_t;

static const convert_vector_t CONVERT_VECTORS[] = {
    /* Exact integers */
    { 65536,      1,      1 },      /* 1.0 → 1 */
    { 131072,     2,      2 },      /* 2.0 → 2 */
    { -65536,     -1,     -1 },     /* -1.0 → -1 */

    /*
     * 1.25 = 81920 (65536 + 16384)  → trunc=1, rne=1 (< 0.5)
     * 1.5  = 98304 (65536 + 32768)  → trunc=1, rne=2 (tie, 1 is odd, round to 2)
     * 1.75 = 114688                 → trunc=1, rne=2 (> 0.5)
     * 2.5  = 163840                 → trunc=2, rne=2 (tie, 2 is even, keep 2)
     */
    { 81920,      1,      1 },      /* 1.25 → 1 (both) */
    { 98304,      1,      2 },      /* 1.5 → trunc=1, rne=2 (tie, round to even) */
    { 114688,     1,      2 },      /* 1.75 → trunc=1, rne=2 */
    { 163840,     2,      2 },      /* 2.5 → trunc=2, rne=2 (tie, 2 is even) */

    /* Negative */
    { -98304,     -1,     -2 },     /* -1.5 → trunc=-1, rne=-2 */
    { -163840,    -2,     -2 },     /* -2.5 → trunc=-2, rne=-2 */
};

#define CONVERT_VECTOR_COUNT (sizeof(CONVERT_VECTORS) / sizeof(CONVERT_VECTORS[0]))

#endif /* AXILOG_GOLDEN_VECTORS_H */
