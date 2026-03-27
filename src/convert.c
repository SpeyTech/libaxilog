/**
 * @file convert.c
 * @brief DVM Q16.16 Conversion Primitives — Implementation
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
 * @traceability SRS-005-SHALL-005, SRS-005-SHALL-006, SRS-005-SHALL-007,
 *               SRS-005-SHALL-008, SRS-005-SHALL-033, SRS-005-SHALL-036,
 *               SRS-005-SHALL-037, SRS-005-SHALL-038, SRS-005-SHALL-039,
 *               SRS-005-SHALL-068
 */

#include "axilog/convert.h"

/* ========================================================================
 * Integer to Q16.16 Conversion
 * ======================================================================== */

/**
 * SRS-005-SHALL-005: Integer to Q16.16 conversion
 * SRS-005-SHALL-033: Pure, reentrant, no global state
 * SRS-005-SHALL-036: O(1) bounded time
 * SRS-005-SHALL-038: No dynamic allocation
 */
q16_16_t ax_int_to_q16(int16_t n, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible for int16_t input */

    /* Multiplication by Q16_ONE, not shift, to avoid signed shift issues */
    /* SRS-005-SHALL-068: No signed right-shift */
    return (int32_t)n * Q16_ONE;
}

/**
 * SRS-005-SHALL-005: Integer to Q16.16 conversion
 * SRS-005-SHALL-031: Saturation fault signalling
 */
q16_16_t ax_int32_to_q16(int32_t n, ct_fault_flags_t *faults)
{
    int64_t result = (int64_t)n * Q16_ONE;

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
 * Q16.16 to Integer Conversion
 * ======================================================================== */

/**
 * SRS-005-SHALL-006: Q16.16 to integer with truncation toward zero
 * SRS-005-SHALL-033: Pure, reentrant, no global state
 * SRS-005-SHALL-068: Uses division, not right-shift
 */
int32_t ax_q16_to_int(q16_16_t x, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible */

    /* C99 division truncates toward zero */
    return x / Q16_ONE;
}

/**
 * SRS-005-SHALL-007: Q16.16 to integer with round-to-nearest-even
 * SRS-005-SHALL-033: Pure, reentrant, no global state
 * SRS-005-SHALL-068: Uses division, not right-shift
 *
 * Algorithm:
 *   1. Compute integer part via division (truncation toward zero)
 *   2. Compute fractional remainder
 *   3. If frac > 0.5: round away from zero
 *   4. If frac < 0.5: round toward zero (truncate)
 *   5. If frac == 0.5: round to even (banker's rounding)
 */
int32_t ax_q16_to_int_rne(q16_16_t x, ct_fault_flags_t *faults)
{
    (void)faults;  /* No fault possible */

    /* Integer part via division (truncates toward zero) */
    int32_t base = x / Q16_ONE;

    /* Fractional remainder: x - base * Q16_ONE */
    int32_t frac = x - (base * Q16_ONE);

    /* Handle negative fractional part (make positive for comparison) */
    if (frac < 0) {
        frac = -frac;
    }

    if (frac > Q16_HALF) {
        /* Round away from zero */
        return (x >= 0) ? base + 1 : base - 1;
    } else if (frac < Q16_HALF) {
        /* Round toward zero (already truncated) */
        return base;
    } else {
        /* Tie: round to even */
        if (base & 1) {
            /* base is odd, round away from zero to make even */
            return (x >= 0) ? base + 1 : base - 1;
        }
        /* base is even, keep it */
        return base;
    }
}

/* ========================================================================
 * Fractional Extraction
 * ======================================================================== */

/**
 * Extract fractional part as Q16.16 value in [0, Q16_ONE).
 */
q16_16_t ax_q16_frac(q16_16_t x, ct_fault_flags_t *faults)
{
    (void)faults;

    /* Integer part via division */
    int32_t int_part = x / Q16_ONE;

    /* Fractional part */
    int32_t frac = x - (int_part * Q16_ONE);

    /* Ensure positive result */
    if (frac < 0) {
        frac += Q16_ONE;
    }

    return frac;
}
