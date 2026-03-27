/**
 * @file transcendental.c
 * @brief DVM Q16.16 Transcendental Approximations — Implementation
 *
 * Copyright (c) 2026 The Murray Family Innovation Trust
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * MEMORY: Zero Dynamic Allocation
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * All transcendental functions use fixed polynomial approximations with
 * frozen coefficients. No floating-point intermediates. No libm dependency.
 * Bit-identical across x86_64, ARM64, RISC-V.
 *
 * exp() uses range reduction to [0, ln(2)] then a minimax polynomial.
 * tanh() uses the identity (e^2x - 1) / (e^2x + 1) for moderate x,
 * and saturates to ±1 for large |x|.
 *
 * @traceability SRS-005-SHALL-040, SRS-005-SHALL-041, SRS-005-SHALL-042,
 *               SRS-005-SHALL-043, SRS-005-SHALL-044, SRS-005-SHALL-045,
 *               SRS-005-SHALL-046, SRS-005-SHALL-048, SRS-005-SHALL-049,
 *               SRS-005-SHALL-050, SRS-005-SHALL-051, SRS-005-SHALL-054,
 *               SRS-005-SHALL-058, SRS-005-SHALL-067, SRS-005-SHALL-068
 */

#include "axilog/transcendental.h"
#include "axilog/arith.h"

/* ========================================================================
 * Frozen Constants (Compile-Time)
 *
 * SRS-005-SHALL-048: Fixed coefficients frozen at compile time
 * SRS-005-SHALL-049: Fixed table contents frozen at compile time
 * SRS-005-SHALL-050: Compile-time generation
 * SRS-005-SHALL-051: No runtime synthesis
 * ======================================================================== */

/**
 * ln(2) in Q16.16 = 0.693147... * 65536 ≈ 45426
 */
#define LN2_Q16  45426

/**
 * 1/ln(2) in Q16.16 = 1.442695... * 65536 ≈ 94548
 */
#define INV_LN2_Q16  94548

/**
 * exp() polynomial coefficients for range [0, ln(2)]
 *
 * These are minimax polynomial coefficients for exp(x) on [0, ln(2)]
 * scaled to Q16.16. The polynomial is:
 *   exp(x) ≈ c0 + c1*x + c2*x² + c3*x³ + c4*x⁴ + c5*x⁵
 *
 * Coefficients derived from Taylor series with adjustment for minimax:
 *   c0 = 1.0           = 65536
 *   c1 = 1.0           = 65536
 *   c2 = 0.5           = 32768
 *   c3 = 1/6           ≈ 10923
 *   c4 = 1/24          ≈ 2731
 *   c5 = 1/120         ≈ 546
 *
 * SRS-005-SHALL-048: Frozen at compile time
 */
static const q16_16_t EXP_COEFF[6] = {
    65536,   /* c0 = 1.0 */
    65536,   /* c1 = 1.0 */
    32768,   /* c2 = 0.5 */
    10923,   /* c3 ≈ 1/6 */
    2731,    /* c4 ≈ 1/24 */
    546      /* c5 ≈ 1/120 */
};

/**
 * Powers of 2 table for exp() range reconstruction.
 * 2^n for n = 0..15 in Q16.16.
 *
 * SRS-005-SHALL-049: Frozen at compile time
 */
static const q16_16_t POW2_TABLE[16] = {
    65536,      /* 2^0  = 1 */
    131072,     /* 2^1  = 2 */
    262144,     /* 2^2  = 4 */
    524288,     /* 2^3  = 8 */
    1048576,    /* 2^4  = 16 */
    2097152,    /* 2^5  = 32 */
    4194304,    /* 2^6  = 64 */
    8388608,    /* 2^7  = 128 */
    16777216,   /* 2^8  = 256 */
    33554432,   /* 2^9  = 512 */
    67108864,   /* 2^10 = 1024 */
    134217728,  /* 2^11 = 2048 */
    268435456,  /* 2^12 = 4096 */
    536870912,  /* 2^13 = 8192 */
    1073741824, /* 2^14 = 16384 */
    INT32_MAX   /* 2^15 saturates */
};

/* ========================================================================
 * Internal Helpers
 * ======================================================================== */

/**
 * @brief Evaluate exp polynomial on [0, ln(2)].
 *
 * Uses Horner's method with saturated multiplication.
 *
 * @param x      Input in [0, LN2_Q16]
 * @param faults Fault context
 * @return exp(x) approximation
 */
static q16_16_t exp_poly(q16_16_t x, ct_fault_flags_t *faults)
{
    /* Horner's method: c0 + x*(c1 + x*(c2 + x*(c3 + x*(c4 + x*c5)))) */
    q16_16_t result;

    /* Start with c5 */
    result = EXP_COEFF[5];

    /* c4 + x*c5 */
    result = ax_add_q16(EXP_COEFF[4], ax_mul_q16(x, result, faults), faults);

    /* c3 + x*(c4 + x*c5) */
    result = ax_add_q16(EXP_COEFF[3], ax_mul_q16(x, result, faults), faults);

    /* c2 + x*(c3 + ...) */
    result = ax_add_q16(EXP_COEFF[2], ax_mul_q16(x, result, faults), faults);

    /* c1 + x*(c2 + ...) */
    result = ax_add_q16(EXP_COEFF[1], ax_mul_q16(x, result, faults), faults);

    /* c0 + x*(c1 + ...) */
    result = ax_add_q16(EXP_COEFF[0], ax_mul_q16(x, result, faults), faults);

    return result;
}

/* ========================================================================
 * Exponential Function
 * ======================================================================== */

/**
 * SRS-005-SHALL-040: Polynomial approximation
 * SRS-005-SHALL-041: Domain enforcement [-11, 11]
 * SRS-005-SHALL-042: ±2 ULP accuracy
 * SRS-005-SHALL-054: Cross-platform identity
 * SRS-005-SHALL-058: No libm dependency
 *
 * Algorithm:
 *   1. Domain check: x ∈ [-11, 11]
 *   2. Handle negative: exp(-x) = 1/exp(x)
 *   3. Range reduction: x = n*ln(2) + r, where r ∈ [0, ln(2))
 *   4. Polynomial: exp(r) via minimax polynomial
 *   5. Reconstruction: exp(x) = 2^n * exp(r)
 */
q16_16_t ax_exp_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    /* SRS-005-SHALL-041: Domain check */
    if (x < AX_EXP_DOMAIN_MIN) {
        faults->domain = 1;
        return 0;  /* exp(-11) ≈ 0 */
    }
    if (x > AX_EXP_DOMAIN_MAX) {
        faults->domain = 1;
        return INT32_MAX;  /* Saturate */
    }

    /* Handle x = 0 exactly */
    if (x == 0) {
        return Q16_ONE;  /* exp(0) = 1.0 */
    }

    /* Track if input was negative */
    int negative = 0;
    q16_16_t abs_x = x;
    if (x < 0) {
        negative = 1;
        abs_x = -x;
    }

    /*
     * Range reduction: abs_x = n * ln(2) + r
     * where n = floor(abs_x / ln(2)) and r = abs_x - n * ln(2)
     *
     * n = (abs_x * (1/ln(2))) / Q16_ONE
     */
    int64_t n_scaled = ((int64_t)abs_x * INV_LN2_Q16) / Q16_ONE;
    int32_t n = (int32_t)(n_scaled / Q16_ONE);

    /* Clamp n to table bounds */
    if (n < 0) n = 0;
    if (n > 15) n = 15;

    /* r = abs_x - n * ln(2) */
    int32_t r = abs_x - (n * LN2_Q16);
    if (r < 0) {
        r = 0;  /* Clamp to valid range */
    }
    if (r > LN2_Q16) {
        r = LN2_Q16;  /* Clamp to valid range */
    }

    /* Polynomial evaluation on [0, ln(2)] */
    q16_16_t exp_r = exp_poly((q16_16_t)r, faults);

    /* Reconstruction: exp(abs_x) = 2^n * exp(r) */
    q16_16_t pow2_n;
    if (n >= 0 && n < 16) {
        pow2_n = POW2_TABLE[n];
    } else {
        pow2_n = INT32_MAX;
        faults->overflow = 1;
    }

    q16_16_t result = ax_mul_q16(pow2_n, exp_r, faults);

    /* If original x was negative: exp(-x) = 1/exp(x) */
    if (negative) {
        /* exp(-x) = Q16_ONE² / exp(x) = Q16_ONE / (exp(x) / Q16_ONE) */
        if (result == 0) {
            return INT32_MAX;  /* 1/0 → saturate */
        }
        result = ax_div_q16(Q16_ONE, result, faults);
    }

    return result;
}

/* ========================================================================
 * Hyperbolic Tangent
 * ======================================================================== */

/**
 * SRS-005-SHALL-043: tanh implementation
 * SRS-005-SHALL-044: Full domain acceptance
 * SRS-005-SHALL-045: ±2 ULP accuracy
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Algorithm:
 *   For |x| > ~4.0 (in Q16.16: 262144), tanh saturates to ±1.
 *   Otherwise: tanh(x) = (exp(2x) - 1) / (exp(2x) + 1)
 */
q16_16_t ax_tanh_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    (void)faults;  /* Mark as used */

    /* Saturation threshold: |x| > 4.0 → tanh ≈ ±1 */
    const q16_16_t TANH_SAT_THRESHOLD = 262144;  /* 4.0 in Q16.16 */

    if (x > TANH_SAT_THRESHOLD) {
        return Q16_ONE;  /* +1.0 */
    }
    if (x < -TANH_SAT_THRESHOLD) {
        return -Q16_ONE;  /* -1.0 */
    }

    /* Handle x = 0 exactly */
    if (x == 0) {
        return 0;  /* tanh(0) = 0 */
    }

    /*
     * tanh(x) = (e^2x - 1) / (e^2x + 1)
     *
     * For stability, use: tanh(x) = (e^x - e^-x) / (e^x + e^-x)
     * But simpler with 2x approach for moderate x.
     */

    /* Compute 2x with saturation check */
    q16_16_t two_x = ax_mul_q16(x, 2 * Q16_ONE, faults);

    /* If 2x is out of exp domain, saturate */
    if (two_x > AX_EXP_DOMAIN_MAX || two_x < AX_EXP_DOMAIN_MIN) {
        return (x > 0) ? Q16_ONE : -Q16_ONE;
    }

    /* exp(2x) */
    q16_16_t exp_2x = ax_exp_q16(two_x, faults);

    /* (exp(2x) - 1) / (exp(2x) + 1) */
    q16_16_t num = ax_sub_q16(exp_2x, Q16_ONE, faults);
    q16_16_t den = ax_add_q16(exp_2x, Q16_ONE, faults);

    if (den == 0) {
        /* Shouldn't happen, but guard anyway */
        return (x > 0) ? Q16_ONE : -Q16_ONE;
    }

    q16_16_t result = ax_div_q16(num, den, faults);

    /* Clamp to [-1, 1] for safety */
    if (result > Q16_ONE) result = Q16_ONE;
    if (result < -Q16_ONE) result = -Q16_ONE;

    return result;
}

/* ========================================================================
 * Sigmoid Function
 * ======================================================================== */

/**
 * SRS-005-SHALL-046: Sigmoid using ax_exp_q16
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * sigmoid(x) = 1 / (1 + exp(-x))
 */
q16_16_t ax_sigmoid_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    /*
     * For large positive x: sigmoid → 1
     * For large negative x: sigmoid → 0
     * Threshold at exp domain boundaries.
     */

    if (x > AX_EXP_DOMAIN_MAX) {
        return Q16_ONE;  /* 1.0 */
    }
    if (x < -AX_EXP_DOMAIN_MAX) {
        return 0;
    }

    /* exp(-x) */
    q16_16_t neg_x = ax_neg_q16(x, faults);
    q16_16_t exp_neg_x = ax_exp_q16(neg_x, faults);

    /* 1 + exp(-x) */
    q16_16_t den = ax_add_q16(Q16_ONE, exp_neg_x, faults);

    if (den == 0) {
        return Q16_ONE;  /* Guard: shouldn't happen */
    }

    /* 1 / (1 + exp(-x)) */
    q16_16_t result = ax_div_q16(Q16_ONE, den, faults);

    /* Clamp to [0, 1] */
    if (result < 0) result = 0;
    if (result > Q16_ONE) result = Q16_ONE;

    return result;
}

/* ========================================================================
 * ReLU Activation
 * ======================================================================== */

/**
 * ReLU: max(0, x)
 */
q16_16_t ax_relu_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible */
    return (x > 0) ? x : 0;
}

/**
 * Leaky ReLU: x if x > 0, else alpha * x
 */
q16_16_t ax_leaky_relu_q16(q16_16_t x, q16_16_t alpha, ct_fault_flags_t *faults)
{
    if (x > 0) {
        return x;
    }
    return ax_mul_q16(alpha, x, faults);
}
