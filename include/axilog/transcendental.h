/**
 * @file transcendental.h
 * @brief DVM Q16.16 Transcendental Approximations
 *
 * Copyright (c) 2026 Spey Systems LTD
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * DVEC: v1.3
 * DETERMINISM: D1 — Strict Deterministic
 * MEMORY: Zero Dynamic Allocation
 * SRS: SRS-005
 *
 * All transcendental functions use fixed polynomial approximations
 * with frozen coefficients. No floating-point intermediates.
 * Bit-identical across x86_64, ARM64, RISC-V.
 *
 * @traceability SRS-005-SHALL-040, SRS-005-SHALL-041, SRS-005-SHALL-042,
 *               SRS-005-SHALL-043, SRS-005-SHALL-044, SRS-005-SHALL-045,
 *               SRS-005-SHALL-046, SRS-005-SHALL-048, SRS-005-SHALL-049,
 *               SRS-005-SHALL-050, SRS-005-SHALL-051, SRS-005-SHALL-054,
 *               SRS-005-SHALL-064, SRS-005-SHALL-065
 */

#ifndef AXILOG_TRANSCENDENTAL_H
#define AXILOG_TRANSCENDENTAL_H

#include "axilog/types.h"

/* ========================================================================
 * Domain Constants
 * ======================================================================== */

/**
 * @brief exp() valid input domain bounds.
 *
 * exp(11) ≈ 59874 fits in Q16.16 integer part
 * exp(12) ≈ 162755 overflows
 *
 * Domain: [-11.0, 11.0] in Q16.16 = [-720896, 720896]
 *
 * SRS-005-SHALL-041: Exponential domain
 */
#define AX_EXP_DOMAIN_MIN  (-720896)  /* -11.0 in Q16.16 */
#define AX_EXP_DOMAIN_MAX  ( 720896)  /*  11.0 in Q16.16 */

/* ========================================================================
 * Exponential Function
 * ======================================================================== */

/**
 * @brief Q16.16 exponential function.
 *
 * Computes e^x using a fixed polynomial approximation with frozen
 * coefficients. Bit-identical across platforms.
 *
 * Domain: [-11.0, 11.0] (Q16.16: [-720896, 720896])
 * Range:  [0, ~59874] (Q16.16 representation)
 *
 * Inputs outside domain set faults->domain and return saturated value.
 *
 * SRS-005-SHALL-040: Polynomial approximation
 * SRS-005-SHALL-041: Domain enforcement
 * SRS-005-SHALL-042: ±2 ULP accuracy
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Monotonicity: MONOTONE_INCREASING
 *
 * @param x      Input value in Q16.16
 * @param faults Fault context (sets domain if out of range)
 * @return e^x in Q16.16, saturated if domain exceeded
 */
q16_16_t ax_exp_q16(q16_16_t x, ct_fault_flags_t *faults);

/* ========================================================================
 * Hyperbolic Tangent
 * ======================================================================== */

/**
 * @brief Q16.16 hyperbolic tangent function.
 *
 * Computes tanh(x) using the identity:
 *   tanh(x) = (exp(2x) - 1) / (exp(2x) + 1)
 *
 * For large |x|, saturates smoothly to ±1.0.
 *
 * Domain: All Q16.16 values (saturates gracefully)
 * Range:  [-1.0, 1.0] (Q16.16: [-65536, 65536])
 *
 * SRS-005-SHALL-043: Hyperbolic tangent implementation
 * SRS-005-SHALL-044: Full domain acceptance
 * SRS-005-SHALL-045: ±2 ULP accuracy
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Monotonicity: MONOTONE_INCREASING
 *
 * @param x      Input value in Q16.16
 * @param faults Fault context (unused, no fault possible)
 * @return tanh(x) in Q16.16
 */
q16_16_t ax_tanh_q16(q16_16_t x, ct_fault_flags_t *faults);

/* ========================================================================
 * Sigmoid Function
 * ======================================================================== */

/**
 * @brief Q16.16 sigmoid (logistic) function.
 *
 * Computes sigmoid(x) = 1 / (1 + exp(-x))
 *
 * Implemented using ax_exp_q16 internally.
 *
 * Domain: All Q16.16 values (saturates gracefully)
 * Range:  [0, 1.0] (Q16.16: [0, 65536])
 *
 * SRS-005-SHALL-046: Sigmoid using ax_exp_q16
 * SRS-005-SHALL-029: Fault context acceptance
 * SRS-005-SHALL-054: Cross-platform identity
 *
 * Monotonicity: MONOTONE_INCREASING
 *
 * @param x      Input value in Q16.16
 * @param faults Fault context (unused, no fault possible)
 * @return sigmoid(x) in Q16.16
 */
q16_16_t ax_sigmoid_q16(q16_16_t x, ct_fault_flags_t *faults);

/* ========================================================================
 * ReLU Activation
 * ======================================================================== */

/**
 * @brief Q16.16 ReLU activation.
 *
 * Computes max(0, x).
 *
 * Domain: All Q16.16 values
 * Range:  [0, Q16_MAX]
 *
 * @param x      Input value in Q16.16
 * @param faults Fault context (unused, no fault possible)
 * @return max(0, x)
 */
q16_16_t ax_relu_q16(q16_16_t x, ct_fault_flags_t *faults);

/**
 * @brief Q16.16 Leaky ReLU activation.
 *
 * Computes x if x > 0, else alpha * x.
 *
 * @param x      Input value in Q16.16
 * @param alpha  Leak coefficient in Q16.16 (typically 0.01 = 655)
 * @param faults Fault context (sets overflow/underflow if multiplication saturates)
 * @return Leaky ReLU result
 */
q16_16_t ax_leaky_relu_q16(q16_16_t x, q16_16_t alpha, ct_fault_flags_t *faults);

#endif /* AXILOG_TRANSCENDENTAL_H */
