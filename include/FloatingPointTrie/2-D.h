/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FPH_FLOATINGPOINTTRIE2D_H
#define FPH_FLOATINGPOINTTRIE2D_H

#include <cstdint>
#include <memory>

#include "FloatingPointTrie/1-D.h"
#include "TrieOperation/1-D.h"
#include "FloatingPointUtilities/ND.h"
#include "FloatingPointTrie/ND.h"

/**
* Definitions for constants for the TLE Trie 2-D.

* LEVEL_TLE : Number of bits to encode the TLE
* LEVEL_TLE_BUCKET_SIZE : Number of buckets in the TLE level (2^10 = 1024)
*/

struct TLETrieNode_2DxF {
  std::bitset<BINS_64> populated;
  uint32_t TLEcounts[BINS_64];
  std::unique_ptr<TrieNode_1DxF> nodes[BINS_64];
};


/**
* Function to generate 2-D trie from the input values

* @param values1 : Array of input values for the first dimension
* @param values2 : Array of input values for the second dimension
* @param values_len1 : Length of the values1 array
* @param values_len2 : Length of the values2 array
* @param default_mode : Flag to enable the default mode
* @return Trie in serialized buffer of characters.
*/

[[nodiscard]] std::unique_ptr<TLETrieNode_2DxF>
execCreateAndInsert_2D(const FPHArray &array1, const FPHArray &array2,
                       uint64_t &curr_trie_size, bool default_mode = true);
[[nodiscard]] std::vector<char> execSerialize_2D(TLETrieNode_2DxF *root,
                                                 uint64_t &curr_trie_size,
                                                 bool default_mode);
[[nodiscard]] std::vector<char> generate_2DxF(const FPHArray &array1,
                                               const FPHArray &array2,
                                               bool default_mode = true);

#endif // FPH_FLOATINGPOINTTRIE2D_H