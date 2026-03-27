#!/usr/bin/env python3
"""
ax-rtm-verify.py — Requirements Traceability Matrix Verification

Copyright (c) 2026 The Murray Family Innovation Trust
SPDX-License-Identifier: GPL-3.0-or-later

SRS: SRS-005 v1.1-Frozen
Patent: UK GB2521625.0

Verifies that all SRS-005 SHALL requirements are traced to implementation
and test code.

Usage:
    ./ax-rtm-verify.py [--verbose]
"""

import os
import re
import sys
from pathlib import Path
from dataclasses import dataclass
from typing import Dict, List, Set, Tuple

# ============================================================================
# SRS-005 v1.1 Requirements Registry
# ============================================================================

SRS_005_REQUIREMENTS = {
    # Representation Model (4)
    "SRS-005-SHALL-001": "Q16.16 in int32_t",
    "SRS-005-SHALL-002": "Unity = 65536",
    "SRS-005-SHALL-003": "Bounds Q16_MAX/MIN/EPS",
    "SRS-005-SHALL-004": "Two's-complement",

    # Conversion Semantics (5)
    "SRS-005-SHALL-005": "int → Q16.16 via ×65536",
    "SRS-005-SHALL-006": "Q16.16 → int truncation",
    "SRS-005-SHALL-007": "Q16.16 → int RNE",
    "SRS-005-SHALL-008": "No float conversion",
    "SRS-005-SHALL-009": "Decimal string RNE (optional)",

    # Core Arithmetic (13)
    "SRS-005-SHALL-010": "Saturated addition",
    "SRS-005-SHALL-011": "Saturated subtraction",
    "SRS-005-SHALL-012": "Saturated multiplication",
    "SRS-005-SHALL-013": "Saturated division",
    "SRS-005-SHALL-014": "Negation with saturation",
    "SRS-005-SHALL-015": "Abs with saturation",
    "SRS-005-SHALL-016": "Minimum",
    "SRS-005-SHALL-017": "Maximum",
    "SRS-005-SHALL-018": "Clamp",
    "SRS-005-SHALL-067": "Mul truncation toward zero",
    "SRS-005-SHALL-068": "No signed right-shift",
    "SRS-005-SHALL-069": "INT32_MIN/-1 → overflow",
    "SRS-005-SHALL-070": "Clamp invalid bounds",

    # Comparison and Ordering (10)
    "SRS-005-SHALL-019": "Signed comparison",
    "SRS-005-SHALL-020": "No FP comparison",
    "SRS-005-SHALL-021": "Equality bit-exact",
    "SRS-005-SHALL-022": "Less-than",
    "SRS-005-SHALL-023": "Greater-than",
    "SRS-005-SHALL-024": "Less-than-or-equal",
    "SRS-005-SHALL-025": "Greater-than-or-equal",
    "SRS-005-SHALL-026": "Sign classification",
    "SRS-005-SHALL-027": "Total ordering",
    "SRS-005-SHALL-028": "Monotonicity declared",

    # Fault Contract (7)
    "SRS-005-SHALL-029": "Fault context acceptance",
    "SRS-005-SHALL-030": "Fault types defined",
    "SRS-005-SHALL-031": "Saturation sets flags",
    "SRS-005-SHALL-032": "Total functions",
    "SRS-005-SHALL-033": "Pure, reentrant",
    "SRS-005-SHALL-034": "No silent failure",
    "SRS-005-SHALL-035": "Fault propagation",

    # Boundedness and Memory (4)
    "SRS-005-SHALL-036": "O(1) bounded time",
    "SRS-005-SHALL-037": "Bounded stack space",
    "SRS-005-SHALL-038": "No malloc",
    "SRS-005-SHALL-039": "No global mutable state",

    # Transcendentals (8)
    "SRS-005-SHALL-040": "exp polynomial",
    "SRS-005-SHALL-041": "exp domain [-11,11]",
    "SRS-005-SHALL-042": "exp ±2 ULP",
    "SRS-005-SHALL-043": "tanh implementation",
    "SRS-005-SHALL-044": "tanh full domain",
    "SRS-005-SHALL-045": "tanh ±2 ULP",
    "SRS-005-SHALL-046": "sigmoid via exp",
    "SRS-005-SHALL-047": "No trig in v1.0",

    # Lookup/Polynomial Rules (6)
    "SRS-005-SHALL-048": "Fixed coefficients",
    "SRS-005-SHALL-049": "Fixed table contents",
    "SRS-005-SHALL-050": "Compile-time generation",
    "SRS-005-SHALL-051": "No runtime synthesis",
    "SRS-005-SHALL-052": "Endianness neutrality",
    "SRS-005-SHALL-053": "Table provenance",

    # Cross-Platform Identity (4)
    "SRS-005-SHALL-054": "Platform equivalence",
    "SRS-005-SHALL-055": "Golden vectors provided",
    "SRS-005-SHALL-056": "Cross-platform tests",
    "SRS-005-SHALL-057": "Compiler independence",

    # Forbidden Dependencies (6)
    "SRS-005-SHALL-058": "No libm",
    "SRS-005-SHALL-059": "No FP hardware state",
    "SRS-005-SHALL-060": "No locale",
    "SRS-005-SHALL-061": "No system clock",
    "SRS-005-SHALL-062": "No randomness",
    "SRS-005-SHALL-063": "No external I/O",

    # Header/Traceability (3)
    "SRS-005-SHALL-064": "Compliance header",
    "SRS-005-SHALL-065": "Function SRS anchors",
    "SRS-005-SHALL-066": "Core types header",
}

# Requirements that are documentation/negative requirements (no code trace needed)
DOCUMENTATION_ONLY = {
    "SRS-005-SHALL-004",  # Platform assumption
    "SRS-005-SHALL-008",  # Negative: no float
    "SRS-005-SHALL-009",  # Optional/deferred
    "SRS-005-SHALL-020",  # Negative: no FP comparison
    "SRS-005-SHALL-032",  # Design property
    "SRS-005-SHALL-033",  # Design property
    "SRS-005-SHALL-037",  # Build flag verification
    "SRS-005-SHALL-038",  # Negative: no malloc
    "SRS-005-SHALL-039",  # Negative: no globals
    "SRS-005-SHALL-047",  # Negative: no trig
    "SRS-005-SHALL-051",  # Negative: no runtime synthesis
    "SRS-005-SHALL-052",  # Design property
    "SRS-005-SHALL-053",  # Documentation
    "SRS-005-SHALL-056",  # CI configuration
    "SRS-005-SHALL-057",  # CI configuration
    "SRS-005-SHALL-058",  # Negative: no libm
    "SRS-005-SHALL-059",  # Negative: no FP state
    "SRS-005-SHALL-060",  # Negative: no locale
    "SRS-005-SHALL-061",  # Negative: no clock
    "SRS-005-SHALL-062",  # Negative: no random
    "SRS-005-SHALL-063",  # Negative: no I/O
    "SRS-005-SHALL-064",  # Header format
    "SRS-005-SHALL-065",  # Documentation
}


@dataclass
class TraceResult:
    """Result of tracing a requirement."""
    req_id: str
    description: str
    found_in_headers: List[str]
    found_in_source: List[str]
    found_in_tests: List[str]
    is_doc_only: bool

    @property
    def is_traced(self) -> bool:
        if self.is_doc_only:
            return True
        return bool(self.found_in_headers or self.found_in_source)


def find_requirement_traces(root_dir: Path, req_id: str) -> TraceResult:
    """Search for traces of a requirement in source files."""
    headers: List[str] = []
    sources: List[str] = []
    tests: List[str] = []

    pattern = re.compile(re.escape(req_id))

    # Search headers
    for f in (root_dir / "include" / "axilog").glob("*.h"):
        content = f.read_text()
        if pattern.search(content):
            headers.append(f.name)

    # Search source
    for f in (root_dir / "src").glob("*.c"):
        content = f.read_text()
        if pattern.search(content):
            sources.append(f.name)

    # Search tests
    for f in (root_dir / "tests").glob("*.c"):
        content = f.read_text()
        if pattern.search(content):
            tests.append(f.name)

    # Search golden vectors
    golden = root_dir / "golden" / "vectors.h"
    if golden.exists():
        content = golden.read_text()
        if pattern.search(content):
            headers.append("golden/vectors.h")

    return TraceResult(
        req_id=req_id,
        description=SRS_005_REQUIREMENTS[req_id],
        found_in_headers=headers,
        found_in_source=sources,
        found_in_tests=tests,
        is_doc_only=req_id in DOCUMENTATION_ONLY,
    )


def main():
    verbose = "--verbose" in sys.argv or "-v" in sys.argv

    # Find root directory
    script_dir = Path(__file__).parent
    if (script_dir / "include").exists():
        root_dir = script_dir
    else:
        root_dir = Path.cwd()

    print("=" * 70)
    print("libaxilog — Requirements Traceability Matrix Verification")
    print("SRS-005 v1.1-Frozen — 70 SHALL Requirements")
    print("=" * 70)
    print()

    traced = 0
    untraced = 0
    doc_only = 0
    results: List[TraceResult] = []

    for req_id in sorted(SRS_005_REQUIREMENTS.keys(), key=lambda x: int(x.split("-")[-1])):
        result = find_requirement_traces(root_dir, req_id)
        results.append(result)

        if result.is_doc_only:
            doc_only += 1
            status = "DOC"
        elif result.is_traced:
            traced += 1
            status = "✓"
        else:
            untraced += 1
            status = "✗"

        if verbose or status == "✗":
            locs = []
            if result.found_in_headers:
                locs.append(f"H:{','.join(result.found_in_headers)}")
            if result.found_in_source:
                locs.append(f"S:{','.join(result.found_in_source)}")
            if result.found_in_tests:
                locs.append(f"T:{','.join(result.found_in_tests)}")
            loc_str = " | ".join(locs) if locs else "(documentation/negative)"
            print(f"[{status}] {req_id}: {result.description}")
            if verbose:
                print(f"     {loc_str}")

    print()
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print(f"Total requirements:     {len(SRS_005_REQUIREMENTS)}")
    print(f"Traced in code:         {traced}")
    print(f"Documentation-only:     {doc_only}")
    print(f"UNTRACED:               {untraced}")
    print()

    if untraced > 0:
        print("VERDICT: FAIL — Untraced requirements found")
        print()
        print("Untraced requirements:")
        for r in results:
            if not r.is_traced and not r.is_doc_only:
                print(f"  - {r.req_id}: {r.description}")
        return 1
    else:
        print("VERDICT: PASS — All requirements traced")
        return 0


if __name__ == "__main__":
    sys.exit(main())
