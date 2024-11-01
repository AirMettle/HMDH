/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FPH_FLOATINGPOINTTRIE_H
#define FPH_FLOATINGPOINTTRIE_H

#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "FloatingPointTrie/ND.h"
#include "FloatingPointUtilities/ND.h"

/**
 * Structure defining the header for a trie.
 *
 * @param type_code Identifies the file or data structure type; expected to be
 * "HierFPHG".
 * @param version Version of the trie.
 * @param m_width Width of the magnitude in int.
 * @param precision_bits Number of bits used for precision.
 * @param node_width Width of top level node in the trie.
 * @param type Dimension of the trie. (1-D or 2-D)
 * @param mode Checks if the trie is in default mode or not.
 * @param pos_inf_count Count of positive infinity values stored.
 * @param neg_inf_count Count of negative infinity values stored.
 * @param pos_zero_count Count of positive zero values stored.
 * @param neg_zero_count Count of negative zero values stored.
 * @param nan_count Count of NaN (Not a Number) values stored.
 * @param trie_root_ref Reference or pointer to the root node of the trie.
 */
struct trie_header {
  char type_code[5];
  int version = 0;
  int m_width;
  int precision_bits;
  int node_width;
  char type[4];
  char config[4];
  int mode;
  int pos_inf_count = 0;
  int neg_inf_count = 0;
  int pos_zero_count = 0;
  int neg_zero_count = 0;
  int nan_count = 0;
  int trie_root_ref;
};

/**
 * Structure defining the Special Counts from the array.
 *
 * @param posInfCount Count of positive infinity values stored.
 * @param negInfCount Count of negative infinity values stored.
 * @param posZeroCount Count of positive zero values stored.
 * @param negZeroCount Count of negative zero values stored.
 * @param nanCount Count of NaN (Not a Number) values stored.
 */

struct SpecialCounts {
  int posInfCount = 0;
  int negInfCount = 0;
  int posZeroCount = 0;
  int negZeroCount = 0;
  int nanCount = 0;
  int posneg = 0;
  int pospos = 0;
  int negpos = 0;
  int negneg = 0;
};

/**
 * Definitions of constants for 1DxF Config for the Trie
 *
 * LEVEL0_BITS: Width of the top level node in the trie.(No of Bits used for
 * indexing) LEVEL1_BITS: Width of the second level node in the trie.
 *
 * LEVEL0_BUCKET_SIZE: The size of buckets at Level 0. (Total will be 256
 * buckets as 2^8) LEVEL1_BUCKET_SIZE: The size of buckets at Level 1. (Total
 * will be 256 buckets as 2^8)
 */
#define LEVEL0_BITS_1DxF 8
#define LEVEL1_BITS_1DxF 8
#define LEVEL0_BUCKET_SIZE_1DxF (1 << LEVEL0_BITS_1DxF)
#define LEVEL1_BUCKET_SIZE_1DxF (1 << LEVEL1_BITS_1DxF)

struct TrieNode_1DxF_Level1; // Forward Declaration

/**
 * Structure defining the Trie Node for 1DxF at Level 0.
 *
 * @param populated Bitset to check if the bucket is populated.
 * @param nodes Array of unique pointers to the next level nodes.
 * @param count_level0 Array to store the count of values in the bucket.
 */
struct TrieNode_1DxF {
  std::bitset<LEVEL0_BUCKET_SIZE_1DxF> populated;
  std::unique_ptr<TrieNode_1DxF_Level1> nodes[LEVEL0_BUCKET_SIZE_1DxF];
  uint32_t count_level0[LEVEL0_BUCKET_SIZE_1DxF] = {0};
};

/**
 * Structure defining the Trie Node for 1DxF at Level 1.
 *
 * @param count Array to store the count of values in the bucket.
 */
struct TrieNode_1DxF_Level1 {
  uint32_t count[LEVEL1_BUCKET_SIZE_1DxF] = {0}; // 4 * 32 = 128
};

/**
 * Function to generate the 1DxF Trie.
 *
 * @param values Array of floating point values.
 * @param values_len Length of the values array.
 * @param default_mode Flag to check if filing is suppose to be in default mode or not
 * @return Trie in the form of a serialized buffer of characters.
 */
[[nodiscard]] std::vector<char> generate_1DxF(const FPHArray &array, bool default_mode = true);
[[nodiscard]] std::unique_ptr<TrieNode_1DxF> execCreateAndInsert_1DxF(SpecialCounts &specialCounts, uint64_t &curr_trie_size, const FPHArray &array, bool default_mode);
[[nodiscard]] std::vector<char> execSerialization_1DxF(const std::unique_ptr<TrieNode_1DxF> &root, const SpecialCounts &specialCounts, uint64_t trieSize, bool default_mode);

// ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

#endif // FPH_FLOATINGPOINTTRIE_H
