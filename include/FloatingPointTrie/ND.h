/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FPH_FLOATINGPOINTNDTRIE_ND_H
#define FPH_FLOATINGPOINTNDTRIE_ND_H

#include <bitset>
#include <memory>
#include <vector>

enum class FPH_dtype {
  Double,
  Float,
  Int32,
  Int64

};

struct FPHArray {
  const void *values{};
  int length{};
  FPH_dtype type;

  FPHArray() = default;
  FPHArray(const void *vals, int len, FPH_dtype t)
      : values(vals), length(len), type(t) {}
};

inline FPHArray buildFPHArray(const double *values, int length) {
  return {values, length, FPH_dtype::Double};
}

inline FPHArray buildFPHArray(const float *values, int length) {
  return {values, length, FPH_dtype::Float};
}

inline FPHArray buildFPHArray(const int32_t *values, int length) {
  return {values, length, FPH_dtype::Int32};
}

inline FPHArray buildFPHArray(const int64_t *values, int length) {
  return {values, length, FPH_dtype::Int64};
}

#endif // FPH_FLOATINGPOINTNDTRIE_ND_H