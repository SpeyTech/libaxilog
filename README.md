# libaxilog

**DVM Q16.16 Arithmetic Substrate for Axioma**

[![License: AGPL-3.0](https://img.shields.io/badge/License-AGPL--3.0-blue.svg)](LICENSE)
[![SRS: SRS-005](https://img.shields.io/badge/SRS-005%20v1.1-green.svg)](docs/SRS-005.md)
[![Determinism: D1](https://img.shields.io/badge/Determinism-D1%20Strict-purple.svg)]()

---

## Overview

libaxilog is the **L1 substrate** of the Axioma safety-critical AI framework. It provides mathematically closed, bit-identical fixed-point arithmetic primitives that all higher layers (L2–L7) depend on.

**Key Properties:**

- **Q16.16 Fixed-Point**: 16 bits integer, 16 bits fractional (1/65536 ≈ 0.0000153 precision)
- **Deterministic**: Bit-identical across x86_64, ARM64, RISC-V
- **Total Functions**: Every operation defined for all valid inputs
- **No Floating-Point**: Zero FP hardware dependency
- **No Dynamic Allocation**: Static memory only
- **Saturating Arithmetic**: Overflow/underflow with fault flags

---

## Specification Conformance

libaxilog implements **SRS-005 v1.1-Frozen** — 70 SHALL requirements covering:

| Category | Requirements |
|----------|-------------|
| Representation Model | SHALL-001–004 |
| Conversion Semantics | SHALL-005–009 |
| Core Arithmetic | SHALL-010–018, 067–070 |
| Comparison & Ordering | SHALL-019–028 |
| Fault Contract | SHALL-029–035 |
| Boundedness & Memory | SHALL-036–039 |
| Transcendentals | SHALL-040–047 |
| Lookup/Polynomial Rules | SHALL-048–053 |
| Cross-Platform Identity | SHALL-054–057 |
| Forbidden Dependencies | SHALL-058–063 |
| Header/Traceability | SHALL-064–066 |

See [CONFORMANCE.md](CONFORMANCE.md) for the full Requirements Traceability Matrix.

---

## Quick Start

```bash
# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Test
ctest --output-on-failure

# Install
sudo make install
```

---

## API Overview

### Types (`axilog/types.h`)

```c
typedef int32_t q16_16_t;       // Q16.16 fixed-point

#define Q16_ONE   65536         // 1.0
#define Q16_HALF  32768         // 0.5
#define Q16_MAX   INT32_MAX     // Maximum value
#define Q16_MIN   INT32_MIN     // Minimum value
#define Q16_EPS   1             // Smallest positive

typedef struct {
    uint8_t overflow;           // Result > Q16_MAX
    uint8_t underflow;          // Result < Q16_MIN
    uint8_t div_zero;           // Division by zero
    uint8_t domain;             // Input outside valid domain
    // ... reserved fields for L3–L5 integration
} ct_fault_flags_t;
```

### Arithmetic (`axilog/arith.h`)

```c
// Saturating arithmetic with fault propagation
q16_16_t ax_add_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);
q16_16_t ax_sub_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);
q16_16_t ax_mul_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);
q16_16_t ax_div_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);

// Unary
q16_16_t ax_neg_q16(q16_16_t x, ct_fault_flags_t *faults);
q16_16_t ax_abs_q16(q16_16_t x, ct_fault_flags_t *faults);

// Min/Max/Clamp
q16_16_t ax_min_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);
q16_16_t ax_max_q16(q16_16_t a, q16_16_t b, ct_fault_flags_t *faults);
q16_16_t ax_clamp_q16(q16_16_t x, q16_16_t lo, q16_16_t hi, ct_fault_flags_t *faults);
```

### Conversion (`axilog/convert.h`)

```c
q16_16_t ax_int_to_q16(int16_t n, ct_fault_flags_t *faults);
int32_t ax_q16_to_int(q16_16_t x, ct_fault_flags_t *faults);      // Truncate
int32_t ax_q16_to_int_rne(q16_16_t x, ct_fault_flags_t *faults);  // Round-to-nearest-even
```

### Comparison (`axilog/compare.h`)

```c
bool ax_eq_q16(q16_16_t a, q16_16_t b);
bool ax_lt_q16(q16_16_t a, q16_16_t b);
bool ax_gt_q16(q16_16_t a, q16_16_t b);
bool ax_le_q16(q16_16_t a, q16_16_t b);
bool ax_ge_q16(q16_16_t a, q16_16_t b);
ax_sign_t ax_sign_q16(q16_16_t x);
```

### Transcendentals (`axilog/transcendental.h`)

```c
// exp(x) — domain: [-11.0, 11.0]
q16_16_t ax_exp_q16(q16_16_t x, ct_fault_flags_t *faults);

// tanh(x) — full domain, output: [-1.0, 1.0]
q16_16_t ax_tanh_q16(q16_16_t x, ct_fault_flags_t *faults);

// sigmoid(x) — full domain, output: [0, 1.0]
q16_16_t ax_sigmoid_q16(q16_16_t x, ct_fault_flags_t *faults);

// ReLU activations
q16_16_t ax_relu_q16(q16_16_t x, ct_fault_flags_t *faults);
q16_16_t ax_leaky_relu_q16(q16_16_t x, q16_16_t alpha, ct_fault_flags_t *faults);
```

---

## Example Usage

```c
#include <axilog/types.h>
#include <axilog/arith.h>
#include <axilog/transcendental.h>

int main(void)
{
    ct_fault_flags_t faults;
    ct_fault_clear(&faults);

    // Fixed-point arithmetic: 2.5 * 1.5 = 3.75
    q16_16_t a = 163840;  // 2.5 in Q16.16
    q16_16_t b = 98304;   // 1.5 in Q16.16
    q16_16_t result = ax_mul_q16(a, b, &faults);
    // result = 245760 (3.75 in Q16.16)

    // Sigmoid activation
    q16_16_t x = 65536;   // 1.0
    q16_16_t sig = ax_sigmoid_q16(x, &faults);
    // sig ≈ 47915 (0.731 in Q16.16)

    // Check for faults
    if (ct_fault_any(&faults)) {
        // Handle overflow/underflow/domain error
    }

    return 0;
}
```

---

## Layer Integration

libaxilog is the foundation of the Axioma stack:

```
L7  axioma-governance   Proof-carrying policies
L6  axioma-audit        Cryptographic audit ledger
L5  axioma-agent        Behavioural FSM
L4  axioma-policy       Policy evaluation, operational envelope
L3  axioma-oracle       Oracle Boundary Gateway
L2  certifiable-*       Deterministic ML ecosystem
L1  libaxilog      ←    DVM substrate (this library)
```

The `ct_fault_flags_t` structure is the **canonical source** for fault propagation across all layers. Reserved fields (encoding, schema, ordering, size, protocol) are set by higher layers but defined here for type consistency.

---

## License

AGPL-3.0-or-later

Copyright © 2026 Spey Systems LTD

---

## Related Projects

- [certifiable-inference](https://github.com/SpeyTech/certifiable-inference) — Deterministic neural network inference
- [axioma-oracle](https://github.com/SpeyTech/axioma-oracle) — L3 Oracle Boundary Gateway
- [fixed-point-fundamentals](https://github.com/SpeyTech/fixed-point-fundamentals) — Educational course

---

*SpeyTech · Scottish Highlands · March 2026*
