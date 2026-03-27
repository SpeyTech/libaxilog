# ============================================================================
# libaxilog — DVM Q16.16 Arithmetic Substrate
# ============================================================================
#
# Copyright (c) 2026 The Murray Family Innovation Trust
# SPDX-License-Identifier: GPL-3.0-or-later
#
# DVEC: v1.3
# DETERMINISM: D1 — Strict Deterministic
# SRS: SRS-005
# Patent: UK GB2521625.0
#
# Build:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   make
#   ctest --output-on-failure
#
# ============================================================================

cmake_minimum_required(VERSION 3.16)

project(libaxilog
    VERSION 0.1.0
    DESCRIPTION "DVM Q16.16 Arithmetic Substrate for Axioma"
    LANGUAGES C
)

# ============================================================================
# C Standard
# ============================================================================

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ============================================================================
# Build Type Default
# ============================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ============================================================================
# D1 Hardened Compiler Flags
#
# SRS-005-SHALL-036: O(1) bounded time (WCET)
# SRS-005-SHALL-037: Bounded stack space
# SRS-005-SHALL-038: No dynamic allocation
# SRS-005-SHALL-039: No global mutable state
# SRS-005-SHALL-058: No libm
# SRS-005-SHALL-068: No signed right-shift (enforced via static analysis)
# ============================================================================

set(AXILOG_HARDENED_FLAGS
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wcast-qual
    -Wcast-align
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wmissing-declarations
    -Wredundant-decls
    -Wshadow
    -Wundef
    -Wunused
    -Wformat=2
    -Wformat-security
    -Wstack-usage=1024
    -fstack-protector-strong
    -fno-common
    -fvisibility=hidden
)

# Additional flags for Debug builds (sanitizers)
set(AXILOG_DEBUG_FLAGS
    -fsanitize=address
    -fsanitize=undefined
    -fno-omit-frame-pointer
)

# ============================================================================
# Library Target
# ============================================================================

add_library(axilog STATIC
    src/arith.c
    src/compare.c
    src/convert.c
    src/transcendental.c
)

target_include_directories(axilog
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/golden
)

target_compile_options(axilog PRIVATE ${AXILOG_HARDENED_FLAGS})

# Debug sanitizers
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(axilog PRIVATE ${AXILOG_DEBUG_FLAGS})
    target_link_options(axilog PRIVATE ${AXILOG_DEBUG_FLAGS})
endif()

# ============================================================================
# Forbidden Pattern Checks (Static Analysis)
#
# SRS-005-SHALL-008: No floating-point
# SRS-005-SHALL-038: No malloc
# SRS-005-SHALL-058: No libm
# SRS-005-SHALL-061: No system clock
# SRS-005-SHALL-062: No randomness
# SRS-005-SHALL-068: No signed right-shift
# ============================================================================

# Custom target for forbidden pattern scan
add_custom_target(forbidden-scan
    COMMAND echo "=== Forbidden Pattern Scan ==="
    COMMAND echo "Checking for float/double..."
    COMMAND ! grep -rn "\\bfloat\\b\\|\\bdouble\\b" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || true
    COMMAND echo "Checking for malloc/free..."
    COMMAND ! grep -rn "\\bmalloc\\b\\|\\bcalloc\\b\\|\\brealloc\\b\\|\\bfree\\b" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || true
    COMMAND echo "Checking for math.h..."
    COMMAND ! grep -rn "#include.*math\\.h" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || true
    COMMAND echo "Checking for time functions..."
    COMMAND ! grep -rn "\\btime\\b\\|\\bclock\\b\\|\\bgettimeofday\\b" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || true
    COMMAND echo "Checking for random functions..."
    COMMAND ! grep -rn "\\brand\\b\\|\\bsrand\\b\\|\\brandom\\b" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || true
    COMMAND echo "Checking for signed right-shift (>>)..."
    COMMAND ! grep -rn ">>" ${CMAKE_CURRENT_SOURCE_DIR}/src/ || echo "No signed right-shift found"
    COMMAND echo "=== Scan Complete ==="
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Scanning for forbidden patterns (SRS-005)"
)

# ============================================================================
# Test Targets
# ============================================================================

enable_testing()

# Test executable helper macro
macro(axilog_add_test name)
    add_executable(${name} tests/${name}.c)
    target_link_libraries(${name} PRIVATE axilog)
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/golden
    )
    target_compile_options(${name} PRIVATE ${AXILOG_HARDENED_FLAGS})

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${name} PRIVATE ${AXILOG_DEBUG_FLAGS})
        target_link_options(${name} PRIVATE ${AXILOG_DEBUG_FLAGS})
    endif()

    add_test(NAME ${name} COMMAND ${name})
endmacro()

axilog_add_test(test_arith)
axilog_add_test(test_compare)
axilog_add_test(test_convert)
axilog_add_test(test_transcendental)

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

install(TARGETS axilog
    EXPORT axilogTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/axilog
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT axilogTargets
    FILE axilogTargets.cmake
    NAMESPACE axilog::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/axilog
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "libaxilog ${PROJECT_VERSION}")
message(STATUS "========================================")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler:        ${CMAKE_C_COMPILER}")
message(STATUS "Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")
