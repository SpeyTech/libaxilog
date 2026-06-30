/**
 * @file arith.c
 * @brief DVM Q16.16 Arithmetic Primitives — Implementation
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * MEMORY: Zero Dynamic Allocation
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * All arithmetic uses int64_t widening for overflow detection.
 * No floating-point. No signed right-shift. Truncation toward zero.
 *
 * @traceability SRS-005-SHALL-010, SRS-005-SHALL-011, SRS-005-SHALL-012,
 *               SRS-005-SHALL-013, SRS-005-SHALL-014, SRS-005-SHALL-015,
 *               SRS-005-SHALL-016, SRS-005-SHALL-017, SRS-005-SHALL-018,
 *               SRS-005-SHALL-031, SRS-005-SHALL-033, SRS-005-SHALL-036,
 *               SRS-005-SHALL-037, SRS-005-SHALL-038, SRS-005-SHALL-039,
 *               SRS-005-SHALL-067, SRS-005-SHALL-068, SRS-005-SHALL-069,
 *               SRS-005-SHALL-070
 */

#include "axilog/arith.h"

/* ========================================================================
 * Saturated Arithmetic
 * ======================================================================== */

/**
 * SRS-005-SHALL-010: Saturated addition
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 * SRS-005-SHALL-036: O(1) bounded time
 */
q16_16_t ax_add_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    int64_t sum = (int64_t)a + (int64_t)b;

    if (sum > INT32_MAX) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    if (sum < INT32_MIN) {
        faults->underflow = 1;
        return INT32_MIN;
    }
    return (int32_t)sum;
}

/**
 * SRS-005-SHALL-011: Saturated subtraction
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 * SRS-005-SHALL-036: O(1) bounded time
 */
q16_16_t ax_sub_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    int64_t diff = (int64_t)a - (int64_t)b;

    if (diff > INT32_MAX) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    if (diff < INT32_MIN) {
        faults->underflow = 1;
        return INT32_MIN;
    }
    return (int32_t)diff;
}

/**
 * SRS-005-SHALL-012: Saturated multiplication
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 * SRS-005-SHALL-036: O(1) bounded time
 * SRS-005-SHALL-067: Truncation toward zero (C99 division)
 * SRS-005-SHALL-068: No signed right-shift — uses division
 */
q16_16_t ax_mul_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    int64_t prod = (int64_t)a * (int64_t)b;

    /* Division by Q16_ONE with truncation toward zero (C99) */
    /* SRS-005-SHALL-067: Mandates truncation */
    /* SRS-005-SHALL-068: Division not shift */
    int64_t result = prod / Q16_ONE;

    if (result > INT32_MAX) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    if (result < INT32_MIN) {
        faults->underflow = 1;
        return INT32_MIN;
    }
    return (int32_t)result;
}

/**
 * SRS-005-SHALL-013: Saturated division
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 * SRS-005-SHALL-036: O(1) bounded time
 * SRS-005-SHALL-069: INT32_MIN / -1 → overflow
 */
q16_16_t ax_div_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    /* Division by zero check */
    if (b == 0) {
        faults->div_zero = 1;
        return 0;
    }

    /* Widen and scale */
    int64_t num = (int64_t)a * Q16_ONE;
    int64_t result = num / (int64_t)b;

    /* SRS-005-SHALL-069: Overflow cases including INT32_MIN / -1 */
    if (result > INT32_MAX) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    if (result < INT32_MIN) {
        faults->underflow = 1;
        return INT32_MIN;
    }
    return (int32_t)result;
}

/* ========================================================================
 * Unary Operations
 * ======================================================================== */

/**
 * SRS-005-SHALL-014: Negation with saturation
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 *
 * INT32_MIN has no positive counterpart in two's-complement.
 * -INT32_MIN would overflow, so we saturate to INT32_MAX.
 */
q16_16_t ax_neg_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    if (x == INT32_MIN) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    return -x;
}

/**
 * SRS-005-SHALL-015: Absolute value with saturation
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-033: Pure function
 */
q16_16_t ax_abs_q16(q16_16_t x, ct_fault_flags_t *faults)
{
    if (x == INT32_MIN) {
        faults->overflow = 1;
        return INT32_MAX;
    }
    return (x < 0) ? -x : x;
}

/* ========================================================================
 * Min / Max / Clamp
 * ======================================================================== */

/**
 * SRS-005-SHALL-016: Minimum
 * SRS-005-SHALL-033: Pure function
 */
q16_16_t ax_min_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible */
    return (a < b) ? a : b;
}

/**
 * SRS-005-SHALL-017: Maximum
 * SRS-005-SHALL-033: Pure function
 */
q16_16_t ax_max_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible */
    return (a > b) ? a : b;
}

/**
 * SRS-005-SHALL-018: Clamp
 * SRS-005-SHALL-033: Pure function
 * SRS-005-SHALL-070: Invalid bounds behaviour
 *
 * If lo > hi, sets domain fault and returns lo.
 */
q16_16_t ax_clamp_q16(q16_16_t x, q16_16_t lo, q16_16_t hi,
                       ct_fault_flags_t *faults)
{
    /* SRS-005-SHALL-070: Invalid bounds */
    if (lo > hi) {
        faults->domain = 1;
        return lo;
    }

    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}
