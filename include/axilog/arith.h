/**
 * @file arith.h
 * @brief DVM Q16.16 Arithmetic Primitives
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
 * All arithmetic uses int64_t widening for overflow detection,
 * with saturation to [Q16_MIN, Q16_MAX] and fault flag propagation.
 *
 * @traceability SRS-005-SHALL-010, SRS-005-SHALL-011, SRS-005-SHALL-012,
 *               SRS-005-SHALL-013, SRS-005-SHALL-014, SRS-005-SHALL-015,
 *               SRS-005-SHALL-016, SRS-005-SHALL-017, SRS-005-SHALL-018,
 *               SRS-005-SHALL-029, SRS-005-SHALL-031, SRS-005-SHALL-054,
 *               SRS-005-SHALL-064, SRS-005-SHALL-065, SRS-005-SHALL-067,
 *               SRS-005-SHALL-068, SRS-005-SHALL-069, SRS-005-SHALL-070
 */

#ifndef AXILOG_ARITH_H
#define AXILOG_ARITH_H

#include "axilog/types.h"

/* ========================================================================
 * Saturated Arithmetic
 * ======================================================================== */

/**
 * @brief Saturated Q16.16 addition.
 *
 * Computes a + b with int64_t widening, saturating to [Q16_MIN, Q16_MAX].
 *
 * SRS-005-SHALL-010: Saturated addition
 * SRS-005-SHALL-028: Monotonicity classification (MONOTONE_INCREASING)
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Monotonicity: MONOTONE_INCREASING (fixed b)
 *
 * @param a      First operand
 * @param b      Second operand
 * @param faults Fault context (sets overflow or underflow)
 * @return a + b, saturated
 */
q16_16_t ax_add_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/**
 * @brief Saturated Q16.16 subtraction.
 *
 * Computes a - b with int64_t widening, saturating to [Q16_MIN, Q16_MAX].
 *
 * SRS-005-SHALL-011: Saturated subtraction
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Monotonicity: MONOTONE_INCREASING (fixed b)
 *
 * @param a      First operand
 * @param b      Second operand
 * @param faults Fault context (sets overflow or underflow)
 * @return a - b, saturated
 */
q16_16_t ax_sub_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/**
 * @brief Saturated Q16.16 multiplication.
 *
 * Computes (a × b) / 65536 with int64_t widening.
 * Uses truncation toward zero (C99 division semantics).
 *
 * SRS-005-SHALL-012: Saturated multiplication
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-054: Cross-platform identity
 * SRS-005-SHALL-067: Truncation toward zero
 *
 * Monotonicity: MONOTONE_INCREASING (b > 0), MONOTONE_DECREASING (b < 0)
 *
 * @param a      First operand
 * @param b      Second operand
 * @param faults Fault context (sets overflow or underflow)
 * @return (a × b) / 65536, saturated, truncated toward zero
 */
q16_16_t ax_mul_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/**
 * @brief Saturated Q16.16 division.
 *
 * Computes (a × 65536) / b with int64_t widening.
 * Sets div_zero fault if b == 0, returning 0.
 *
 * SRS-005-SHALL-013: Saturated division
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 * SRS-005-SHALL-054: Cross-platform identity
 * SRS-005-SHALL-069: Division overflow (INT32_MIN / -1 → overflow)
 *
 * @param a      Dividend
 * @param b      Divisor (must not be 0)
 * @param faults Fault context (sets div_zero, overflow, or underflow)
 * @return (a × 65536) / b, saturated
 */
q16_16_t ax_div_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/* ========================================================================
 * Unary Operations
 * ======================================================================== */

/**
 * @brief Saturated Q16.16 negation.
 *
 * Returns -x, with saturation for INT32_MIN (which has no positive
 * counterpart in two's-complement).
 *
 * SRS-005-SHALL-014: Negation with saturation
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 *
 * Monotonicity: MONOTONE_DECREASING
 *
 * @param x      Operand
 * @param faults Fault context (sets overflow if x == INT32_MIN)
 * @return -x, saturated
 */
q16_16_t ax_neg_q16(q16_16_t x, ct_fault_flags_t *faults);

/**
 * @brief Saturated Q16.16 absolute value.
 *
 * Returns |x|, with saturation for INT32_MIN.
 *
 * SRS-005-SHALL-015: Absolute value with saturation
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 *
 * Monotonicity: NON_MONOTONIC
 *
 * @param x      Operand
 * @param faults Fault context (sets overflow if x == INT32_MIN)
 * @return |x|, saturated
 */
q16_16_t ax_abs_q16(q16_16_t x, ct_fault_flags_t *faults);

/* ========================================================================
 * Min / Max / Clamp
 * ======================================================================== */

/**
 * @brief Q16.16 minimum.
 *
 * Returns the lesser of a and b.
 *
 * SRS-005-SHALL-016: Minimum
 * SRS-005-SHALL-029: Fault context acceptance
 *
 * @param a      First operand
 * @param b      Second operand
 * @param faults Fault context (unused, no fault possible)
 * @return min(a, b)
 */
q16_16_t ax_min_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/**
 * @brief Q16.16 maximum.
 *
 * Returns the greater of a and b.
 *
 * SRS-005-SHALL-017: Maximum
 * SRS-005-SHALL-029: Fault context acceptance
 *
 * @param a      First operand
 * @param b      Second operand
 * @param faults Fault context (unused, no fault possible)
 * @return max(a, b)
 */
q16_16_t ax_max_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

/**
 * @brief Q16.16 clamp.
 *
 * Constrains x to [lo, hi]. If lo > hi, sets domain fault and returns lo.
 *
 * SRS-005-SHALL-018: Clamp
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-070: Invalid bounds behaviour
 *
 * @param x      Value to clamp
 * @param lo     Lower bound
 * @param hi     Upper bound
 * @param faults Fault context (sets domain if lo > hi)
 * @return x clamped to [lo, hi]
 */
q16_16_t ax_clamp_q16(q16_16_t x, q16_16_t lo, q16_16_t hi,
                       ct_fault_flags_t *faults);

#endif /* AXILOG_ARITH_H */
