/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointUtilities/ND.h"
#include "include/FloatingPointTrie/ND.h"
#include "lib/include/logger.h"
#include <bitset>
#include <cstdint>
#include <iostream>

size_t getFPHTypeSize(FPH_dtype type) {
  switch (type) {
  case FPH_dtype::Double:
    return sizeof(double);
  case FPH_dtype::Float:
    return sizeof(float);
  case FPH_dtype::Int32:
    return sizeof(int32_t);
  case FPH_dtype::Int64:
    return sizeof(int64_t);
  default:
    return 0; // throw an exception here
  }
}

int getNumDims2D(unsigned int combinedTLE) {

  // Extract individual TLEs from the combinedTLE
  unsigned int tle1 = (combinedTLE >> 3) & 0x7; // Extract bits 3-5 for tle1
  unsigned int tle2 = combinedTLE & 0x7;        // Extract bits 0-2 for tle2

  // Check special conditions
  bool isTle1Special = (tle1 == 0 || tle1 == 1 || tle1 == 4 || tle1 == 7);
  bool isTle2Special = (tle2 == 0 || tle2 == 1 || tle2 == 4 || tle2 == 7);

  // Compute ndims based on the special conditions
  int ndims = (~((isTle1Special << 1) | isTle2Special)) & 0x3;

  return ndims;
}


float int32_to_float(int32_t value) {
  float result;

#ifdef x86_ARCH
  __asm__(
      "cvtsi2ss %1, %0" // Convert int32 to float32
      : "=x"(result)    // Output: xmm register holding the result
      : "r"(value) // Input: general-purpose register holding the int32 value
  );
#elif ARM_ARCH
  __asm__(
      "scvtf %s0, %w1" // Convert int32 to float32
      : "=w"(result)   // Output: scalar floating-point register
      : "r"(value) // Input: general-purpose register holding the int32 value
  );
#endif

  return result;
}

float int64_to_float(int64_t value) {
  float result;

#ifdef x86_ARCH
  __asm__(
      "cvtsi2ss %1, %0" // Convert int64 to float32
      : "=x"(result)    // Output: xmm register holding the result
      : "r"(value) // Input: general-purpose register holding the int64 value
  );
#elif ARM_ARCH
  __asm__(
      "scvtf %s0, %w1" // Convert int32 to float32
      : "=w"(result)   // Output: scalar floating-point register
      : "r"(value) // Input: general-purpose register holding the int32 value
  );
#endif

  return result;
}

TLE setTLEComponents_32(uint32_t fpNumber) {
  TLE tle;
  uint32_t expMask = 0xFF << 23;
  uint32_t signBit = (fpNumber >> 31) & 1;
  uint32_t exponent = (fpNumber >> 23) & 0xFF;
  // mask everything else apart from magnitude and compare
  uint32_t mantissa = fpNumber & 0x7FFFFF;

  // Early return for Zero check
  if (!(fpNumber & ~(1U << 31))) {
    tle.TLE = 4;
    return tle;
  }

  // Early return for Inf or Nan
  if ((fpNumber & expMask) == expMask) {
    tle.TLE = mantissa ? 0 : (signBit ? 7 : 1);
    return tle;
  }

  // Handle normal numbers
  bool isExponentLessThan127 = exponent < 127;
  int index = (signBit << 1) | isExponentLessThan127;
  static const int results[] = {2, 3, 6, 5};
  tle.TLE = results[index];
  return tle;
}

TLE setTLEComponents(uint64_t fpNumber) {
  TLE tle;
  uint64_t expMask = 0x7FFULL << 52;
  uint64_t signBit = (fpNumber >> 63) & 1;
  uint64_t exponent = (fpNumber >> 52) & 0x7FF;
  // mask everything else apart from magnitude and compare
  uint64_t mantissa = fpNumber & 0xFFFFFFFFFFFFF;

  // Early return for Zero check
  if (!(fpNumber & ~(1ULL << 63))) {
    tle.TLE = 4;
    return tle;
  }

  // Early return for Inf or Nan
  if ((fpNumber & expMask) == expMask) {
    tle.TLE = mantissa ? 0 : (signBit ? 7 : 1);
    return tle;
  }

  // Handle normal numbers
  bool isExponentLessThan1023 = exponent < 1023;
  int index = (signBit << 1) | isExponentLessThan1023;
  static const int results[] = {2, 3, 6, 5};
  tle.TLE = results[index];
  return tle;
}


unsigned int createInternal8Bit_32(uint32_t fpNumber, bool default_mode) {
  uint32_t exponent = (fpNumber >> 23) & 0xFF;
  bool placed_signedExponentBit = (exponent < 127);

  uint32_t adjustedExponent =
      placed_signedExponentBit ? (127 - exponent) : (exponent - 127);
  if (default_mode && placed_signedExponentBit) {
    adjustedExponent = ((adjustedExponent - 1) & 0xFF);
  }

  uint32_t maskedNumber = (fpNumber & 0x007FFFFF) | (adjustedExponent << 23);

  uint32_t firstOnePos;
#ifdef x86_ARCH
  __asm__("bsrl %1, %0" : "=r"(firstOnePos) : "r"(maskedNumber) : "cc");
#elif ARM_ARCH
  __asm__("clz %0, %1" : "=r"(firstOnePos) : "r"(maskedNumber) : "cc");
  firstOnePos = 31 - firstOnePos;
#endif

  int zeroCount = std::min(31 - static_cast<int>(firstOnePos) - 2, 7);

  uint64_t precisionBits = ((maskedNumber >> (firstOnePos - 5)) & 0x1F);

  return (zeroCount << 5) | precisionBits;
}

unsigned int createInternal8Bit(uint64_t fpNumber, bool default_mode) {
  uint64_t exponent = (fpNumber >> 52) & 0x7FF;
  bool placed_signedExponentBit = ((exponent <= 1023) ? 1 : 0);

  uint64_t adjustedExponent =
      placed_signedExponentBit ? (1023 - exponent) : (exponent - 1023);
  if (default_mode && placed_signedExponentBit) {
    adjustedExponent = ((adjustedExponent - 1) & 0x7FF);
  }

  // Update fpNumber with the adjusted exponent
  uint64_t maskedNumber =
      (fpNumber & 0x000FFFFFFFFFFFFF) | (adjustedExponent << 52);

  // Calculate the position of the first significant bit using assembly or
  // intrinsic
  uint64_t firstOnePos;
#ifdef x86_ARCH
  __asm__("bsrq %1, %0" : "=r"(firstOnePos) : "r"(maskedNumber) : "cc");
#elif ARM_ARCH
  __asm__("clz %0, %1" : "=r"(firstOnePos) : "r"(maskedNumber) : "cc");
  firstOnePos = 63 - firstOnePos;
#endif
  int zeroCount =
      std::min(std::max(63 - static_cast<int>(firstOnePos) - 2, 0), 7);

  uint64_t precisionBits = ((maskedNumber >> (firstOnePos - 5)) & 0x1F);

  return (zeroCount << 5) | precisionBits;
}
