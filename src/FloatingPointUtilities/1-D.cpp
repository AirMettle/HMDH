/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointUtilities/1-D.h"
#include "include/FloatingPointTrie/1-D.h"
#include "lib/include/logger.h"

void createAndInsertFP16(TrieNode_1DxF *node, uint64_t fpNumber,
                         uint64_t &curr_trie_size, bool default_mode) {
  // Extract and calculate necessary bits directly from the floating-point
  // number
  bool placed_sign_bit = ((fpNumber >> 63) & 1);

  uint64_t exponent = (fpNumber >> 52) & 0x7FF;
  bool placed_signedExponentBit = ((exponent < 1023) ? 1 : 0);

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

  int zeroCount = std::min(63 - static_cast<int>(firstOnePos) - 2, 15);

  uint64_t precisionBits = ((maskedNumber >> (firstOnePos - 10)) & 0x3FF);

  // Trie Insertion Logic
  unsigned int index8 = placed_sign_bit << 7 | placed_signedExponentBit << 6
                        | (zeroCount << 2) | ((precisionBits >> 8) & 0x3);
  unsigned int index8_level2 = precisionBits & 0xFF;

  if (!node->populated.test(index8)) {
    node->populated.set(index8);
    node->nodes[index8] = std::make_unique<TrieNode_1DxF_Level1>();
    curr_trie_size += sizeof(TrieNode_1DxF_Level1);
  }

  if (node->nodes[index8]) {
    node->count_level0[index8]++;
    node->nodes[index8]->count[index8_level2]++;
  }
}

void createAndInsertFP16_32(TrieNode_1DxF *node, uint32_t fpNumber,
                            uint64_t &curr_trie_size, bool default_mode) {
  bool placed_sign_bit = ((fpNumber >> 31) & 1);
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

  int zeroCount = std::min(31 - static_cast<int>(firstOnePos) - 2, 15);

  uint64_t precisionBits = ((maskedNumber >> (firstOnePos - 10)) & 0x3FF);

  // Trie Insertion Logic
  unsigned int index8 = placed_sign_bit << 7 | placed_signedExponentBit << 6
                        | (zeroCount << 2) | ((precisionBits >> 8) & 0x3);
  unsigned int index8_level2 = precisionBits & 0xFF;

  if (!node->populated.test(index8)) {
    node->populated.set(index8);
    node->nodes[index8] = std::make_unique<TrieNode_1DxF_Level1>();
    curr_trie_size += sizeof(TrieNode_1DxF_Level1);
  }

  if (node->nodes[index8]) {
    node->count_level0[index8]++;
    node->nodes[index8]->count[index8_level2]++;
  }
}


bool isSpecialCase(uint64_t fpNumber, SpecialCounts &counts) {
  uint64_t isInfOrNan = (fpNumber & (0x7FFULL << 52)) == (0x7FFULL << 52);

  uint64_t isZero = !(fpNumber & ~(1ULL << 63));

  if (!(isInfOrNan || isZero))
    return false;
  uint64_t signBit = ((fpNumber >> 63) & 1);
  if (isInfOrNan) {
    uint64_t isNaN = (fpNumber & 0xFFFFFFFFFFFFF);

    if (isNaN) {
      counts.nanCount++;
    } else {
      counts.posInfCount += !signBit;
      counts.negInfCount += signBit;
    }
  } else {
    counts.posZeroCount += !signBit;
    counts.negZeroCount += signBit;
  }

  return true;
}

bool isSpecialCase_32(uint32_t fpNumber, SpecialCounts &counts) {
  uint32_t isInfOrNan = (fpNumber & (0xFFU << 23)) == (0xFFU << 23);
  uint32_t isZero = !(fpNumber & ~(1U << 31));

  if (!(isInfOrNan || isZero)) {
    return false;
  }
  uint32_t signBit = (fpNumber >> 31) & 1U;

  if (isInfOrNan) {
    uint32_t isNaN = (fpNumber & 0x7FFFFF);

    if (isNaN) {
      counts.nanCount++;
    } else {
      counts.posInfCount += !signBit;
      counts.negInfCount += signBit;
    }
  } else {
    counts.posZeroCount += !signBit;
    counts.negZeroCount += signBit;
  }

  return true;
}
