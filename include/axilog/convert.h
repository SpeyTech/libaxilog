/**
 * @file convert.h
 * @brief DVM Q16.16 Conversion Primitives
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
 *               SRS-005-SHALL-008, SRS-005-SHALL-009, SRS-005-SHALL-064,
 *               SRS-005-SHALL-065
 */

#ifndef AXILOG_CONVERT_H
#define AXILOG_CONVERT_H

#include "axilog/types.h"

/* ========================================================================
 * Integer to Q16.16 Conversion
 * ======================================================================== */

/**
 * @brief Convert int16_t to Q16.16.
 *
 * Uses multiplication by 65536 to avoid shift on signed values.
 * Input constrained to int16_t to guarantee no overflow.
 *
 * SRS-005-SHALL-005: Integer to Q16.16 conversion
 * SRS-005-SHALL-029: Fault context acceptance
 *
 * @param n      Integer value in range [-32768, 32767]
 * @param faults Fault context (unused, no fault possible)
 * @return Q16.16 representation of n
 */
q16_16_t ax_int_to_q16(int16_t n, ct_fault_flags_t *faults);

/**
 * @brief Convert int32_t to Q16.16 with saturation.
 *
 * For values outside int16_t range, saturates with fault flag.
 *
 * SRS-005-SHALL-005: Integer to Q16.16 conversion
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-031: Saturation fault signalling
 *
 * @param n      Integer value
 * @param faults Fault context
 * @return Q16.16 representation, saturated if necessary
 */
q16_16_t ax_int32_to_q16(int32_t n, ct_fault_flags_t *faults);

/* ========================================================================
 * Q16.16 to Integer Conversion
 * ======================================================================== */

/**
 * @brief Convert Q16.16 to integer with truncation toward zero.
 *
 * Uses division by 65536 (C99 truncation toward zero).
 *
 * SRS-005-SHALL-006: Q16.16 to integer conversion
 * SRS-005-SHALL-029: Fault context acceptance
 *
 * @param x      Q16.16 value
 * @param faults Fault context (unused, no fault possible)
 * @return Integer part of x, truncated toward zero
 */
int32_t ax_q16_to_int(q16_16_t x, ct_fault_flags_t *faults);

/**
 * @brief Convert Q16.16 to integer with round-to-nearest-even.
 *
 * Uses division (not right-shift) to avoid implementation-defined
 * behaviour for negative values.
 *
 * SRS-005-SHALL-007: Q16.16 to integer with RNE rounding
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-068: No signed right-shift
 *
 * @param x      Q16.16 value
 * @param faults Fault context (unused, no fault possible)
 * @return Integer part of x, rounded to nearest even
 */
int32_t ax_q16_to_int_rne(q16_16_t x, ct_fault_flags_t *faults);

/* ========================================================================
 * Fractional Extraction
 * ======================================================================== */

/**
 * @brief Extract fractional part of Q16.16 value.
 *
 * Returns the fractional portion as a Q16.16 value in [0, 1).
 * For negative values, returns the positive fractional remainder.
 *
 * @param x      Q16.16 value
 * @param faults Fault context (unused)
 * @return Fractional part in range [0, Q16_ONE)
 */
q16_16_t ax_q16_frac(q16_16_t x, ct_fault_flags_t *faults);

#endif /* AXILOG_CONVERT_H */
