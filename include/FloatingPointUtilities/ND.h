/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FPH_NDUTILITIES_H
#define FPH_NDUTILITIES_H

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <memory>
#include <bitset>

#include "FloatingPointTrie/ND.h"

// BEGIN SECTION === Constants ===

#define BITS_6 6
#define BITS_8 8
#define BITS_10 10

#define BINS_64 (1 << BITS_6)
#define BINS_256 (1 << BITS_8)
#define BINS_1024 (1 << BITS_10)

// END SECTION === Constants ===

/**
** @brief Individual Top Level Encoding (TLE) bits

** @param TLE 3 bits (0-7) to encode the top level of the 3D trie
** @see options are :
** 1. NaN = 0
** 2. +ve inf = 1
** 3. +ve +veExp = 2
** 4. +ve -veExp = 3
** 5. 0 = 4
** 6. -ve -veExp = 5
** 7. -ve +veExp = 6
** 8. -ve Inf = 7
*/
struct TLE {
  uint8_t TLE : 3;
};

TLE setTLEComponents(uint64_t fpNumber);
TLE setTLEComponents_32(uint32_t fpNumber);
float int32_to_float(int32_t value);
float int64_to_float(int64_t value);
size_t getFPHTypeSize(FPH_dtype type);
int getNumDims2D(unsigned int combinedTLE);
unsigned int createInternal8Bit_32(uint32_t fpNumber, bool default_mode);
unsigned int createInternal8Bit(uint64_t fpNumber, bool default_mode);


#endif // FPH_NDUTILITIES_H