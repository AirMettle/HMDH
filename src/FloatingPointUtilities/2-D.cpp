/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointUtilities/2-D.h"
#include "include/FloatingPointTrie/1-D.h"
#include "lib/include/logger.h"
#include <cstdint>
#include <memory>


unsigned int combine_chunks_2DxF(unsigned int x, unsigned int y) {
  if (x > 0xFF || y > 0xFF) {
    LOG_ERROR("Error: Input values exceed 8 bits.");
    return 0;
  }

  // Extract first 3 bits from both x and y
  unsigned int first_3_bits_x = (x >> 5) & 0x7;
  unsigned int first_3_bits_y = (y >> 5) & 0x7;

  // Extract last 5 bits from both x and y
  unsigned int last_5_bits_x = x & 0x1F;
  unsigned int last_5_bits_y = y & 0x1F;

  // Combine into a 16-bit number
  unsigned int result = 0;

  // Place first 3 bits of x in bits 15-13 of the result
  result |= (first_3_bits_x << 13);

  // Place first 3 bits of y in bits 12-10 of the result
  result |= (first_3_bits_y << 10);

  // Place last 5 bits of x in bits 9-5 of the result
  result |= (last_5_bits_x << 5);

  // Place last 5 bits of y in bits 4-0 of the result
  result |= last_5_bits_y;

  return result;
}

void insertintoTLETrie_2DxF(TLETrieNode_2DxF *root, unsigned int combined,
                            unsigned int combinedTLE, int ndims,
                            uint64_t &curr_trie_size) {
  if (!root) {
    LOG_ERROR("Root is null in insertintoTLETrie_2DxF.");
    return;
  }

  if (!root->populated.test(combinedTLE)) {
    root->populated.set(combinedTLE);
  }

  root->TLEcounts[combinedTLE]++;

  if (ndims == 0) {
    return;
  }

  if (ndims == 1 || ndims == 2) {
    if (!root->nodes[combinedTLE]) {
      root->nodes[combinedTLE] = std::make_unique<TrieNode_1DxF>();
      curr_trie_size += sizeof(TrieNode_1DxF);
    }
    if (!root->nodes[combinedTLE]->populated.test(combined)) {
      root->nodes[combinedTLE]->populated.set(combined);
    }
    root->nodes[combinedTLE]->count_level0[combined]++;
  }

  if (ndims == 3) {
    unsigned int first8 = combined >> 8 & 0xFF;
    unsigned int last8 = combined & 0xFF;

    if (!root->nodes[combinedTLE]) {
      root->nodes[combinedTLE] = std::make_unique<TrieNode_1DxF>();
      curr_trie_size += sizeof(TrieNode_1DxF);
    }

    if (!root->nodes[combinedTLE]->populated.test(first8)) {
      root->nodes[combinedTLE]->populated.set(first8);
      root->nodes[combinedTLE]->nodes[first8] =
          std::make_unique<TrieNode_1DxF_Level1>();
      curr_trie_size += sizeof(TrieNode_1DxF_Level1);
    }

    root->nodes[combinedTLE]->count_level0[first8]++;
    root->nodes[combinedTLE]->nodes[first8]->count[last8]++;
  }
}
