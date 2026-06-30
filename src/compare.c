/**
 * @file compare.c
 * @brief DVM Q16.16 Comparison and Ordering Primitives — Implementation
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * MEMORY: Zero Dynamic Allocation
 * SRS: SRS-005
 * Patent: UK GB2521625.0
 *
 * All comparisons use signed integer semantics.
 * No floating-point. Bit-identical across platforms.
 *
 * @traceability SRS-005-SHALL-019, SRS-005-SHALL-020, SRS-005-SHALL-021,
 *               SRS-005-SHALL-022, SRS-005-SHALL-023, SRS-005-SHALL-024,
 *               SRS-005-SHALL-025, SRS-005-SHALL-026, SRS-005-SHALL-027,
 *               SRS-005-SHALL-033, SRS-005-SHALL-036
 */

#include "axilog/compare.h"

/* ========================================================================
 * Equality
 * ======================================================================== */

/**
 * SRS-005-SHALL-021: Equality as exact bit comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-020: No floating-point conversion
 * SRS-005-SHALL-033: Pure function
 */
bool ax_eq_q16(q16_16_t a, q16_16_t b)
{
    return a == b;
}

/**
 * Inequality (complement of equality).
 */
bool ax_ne_q16(q16_16_t a, q16_16_t b)
{
    return a != b;
}

/* ========================================================================
 * Relational Comparisons
 * ======================================================================== */

/**
 * SRS-005-SHALL-022: Less-than using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 * SRS-005-SHALL-033: Pure function
 */
bool ax_lt_q16(q16_16_t a, q16_16_t b)
{
    return a < b;
}

/**
 * SRS-005-SHALL-023: Greater-than using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 * SRS-005-SHALL-033: Pure function
 */
bool ax_gt_q16(q16_16_t a, q16_16_t b)
{
    return a > b;
}

/**
 * SRS-005-SHALL-024: Less-than-or-equal using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 * SRS-005-SHALL-033: Pure function
 */
bool ax_le_q16(q16_16_t a, q16_16_t b)
{
    return a <= b;
}

/**
 * SRS-005-SHALL-025: Greater-than-or-equal using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 * SRS-005-SHALL-033: Pure function
 */
bool ax_ge_q16(q16_16_t a, q16_16_t b)
{
    return a >= b;
}

/* ========================================================================
 * Sign Classification
 * ======================================================================== */

/**
 * SRS-005-SHALL-026: Deterministic sign classification
 * SRS-005-SHALL-033: Pure function
 */
ax_sign_t ax_sign_q16(q16_16_t x)
{
    if (x < 0) return AX_SIGN_NEGATIVE;
    if (x > 0) return AX_SIGN_POSITIVE;
    return AX_SIGN_ZERO;
}

/* ========================================================================
 * Three-Way Comparison
 * ======================================================================== */

/**
 * SRS-005-SHALL-027: Total ordering guarantee
 * SRS-005-SHALL-033: Pure function
 *
 * Returns -1 if a < b, 0 if a == b, 1 if a > b.
 */
int ax_cmp_q16(q16_16_t a, q16_16_t b)
{
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

/* ========================================================================
 * Predicates
 * ======================================================================== */

/**
 * Check if value is zero.
 */
bool ax_is_zero_q16(q16_16_t x)
{
    return x == 0;
}

/**
 * Check if value is positive.
 */
bool ax_is_positive_q16(q16_16_t x)
{
    return x > 0;
}

/**
 * Check if value is negative.
 */
bool ax_is_negative_q16(q16_16_t x)
{
    return x < 0;
}
