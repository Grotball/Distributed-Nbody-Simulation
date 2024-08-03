#pragma once

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__)
#include <immintrin.h>
#endif

#if defined(__AVX__)
#define USE_SIMD
constexpr int MaxPackedFloats = 8;
#else
constexpr int MaxPackedFloats = 1;
#endif
 