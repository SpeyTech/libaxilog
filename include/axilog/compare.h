/**
 * @file compare.h
 * @brief DVM Q16.16 Comparison and Ordering Primitives
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * MEMORY: Zero Dynamic Allocation
 * SRS: SRS-005
 *
 * All comparisons are signed, total, and bit-identical across platforms.
 * No floating-point conversion or host math libraries.
 *
 * @traceability SRS-005-SHALL-019, SRS-005-SHALL-020, SRS-005-SHALL-021,
 *               SRS-005-SHALL-022, SRS-005-SHALL-023, SRS-005-SHALL-024,
 *               SRS-005-SHALL-025, SRS-005-SHALL-026, SRS-005-SHALL-027,
 *               SRS-005-SHALL-064, SRS-005-SHALL-065
 */

#ifndef AXILOG_COMPARE_H
#define AXILOG_COMPARE_H

#include "axilog/types.h"

/* ========================================================================
 * Equality
 * ======================================================================== */

/**
 * @brief Q16.16 equality (exact bit comparison).
 *
 * SRS-005-SHALL-021: Equality as exact bit comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-020: No floating-point conversion
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a == b
 */
bool ax_eq_q16(q16_16_t a, q16_16_t b);

/**
 * @brief Q16.16 inequality.
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a != b
 */
bool ax_ne_q16(q16_16_t a, q16_16_t b);

/* ========================================================================
 * Relational Comparisons
 * ======================================================================== */

/**
 * @brief Q16.16 less-than.
 *
 * SRS-005-SHALL-022: Less-than using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a < b
 */
bool ax_lt_q16(q16_16_t a, q16_16_t b);

/**
 * @brief Q16.16 greater-than.
 *
 * SRS-005-SHALL-023: Greater-than using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a > b
 */
bool ax_gt_q16(q16_16_t a, q16_16_t b);

/**
 * @brief Q16.16 less-than-or-equal.
 *
 * SRS-005-SHALL-024: Less-than-or-equal using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a <= b
 */
bool ax_le_q16(q16_16_t a, q16_16_t b);

/**
 * @brief Q16.16 greater-than-or-equal.
 *
 * SRS-005-SHALL-025: Greater-than-or-equal using signed integer comparison
 * SRS-005-SHALL-019: Signed comparison
 * SRS-005-SHALL-027: Total ordering guarantee
 *
 * @param a First operand
 * @param b Second operand
 * @return true if a >= b
 */
bool ax_ge_q16(q16_16_t a, q16_16_t b);

/* ========================================================================
 * Sign Classification
 * ======================================================================== */

/**
 * @brief Q16.16 sign classification.
 *
 * SRS-005-SHALL-026: Deterministic sign classification
 *
 * @param x Value to classify
 * @return AX_SIGN_NEGATIVE, AX_SIGN_ZERO, or AX_SIGN_POSITIVE
 */
ax_sign_t ax_sign_q16(q16_16_t x);

/* ========================================================================
 * Three-Way Comparison
 * ======================================================================== */

/**
 * @brief Q16.16 three-way comparison.
 *
 * Returns -1 if a < b, 0 if a == b, 1 if a > b.
 *
 * SRS-005-SHALL-027: Total ordering guarantee
 *
 * @param a First operand
 * @param b Second operand
 * @return -1, 0, or 1
 */
int ax_cmp_q16(q16_16_t a, q16_16_t b);

/* ========================================================================
 * Predicates
 * ======================================================================== */

/**
 * @brief Check if Q16.16 value is zero.
 *
 * @param x Value to check
 * @return true if x == 0
 */
bool ax_is_zero_q16(q16_16_t x);

/**
 * @brief Check if Q16.16 value is positive.
 *
 * @param x Value to check
 * @return true if x > 0
 */
bool ax_is_positive_q16(q16_16_t x);

/**
 * @brief Check if Q16.16 value is negative.
 *
 * @param x Value to check
 * @return true if x < 0
 */
bool ax_is_negative_q16(q16_16_t x);

#endif /* AXILOG_COMPARE_H */
