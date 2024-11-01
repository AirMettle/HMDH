/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FPH_FLOATINGPOINTUTILITIES_H
#define FPH_FLOATINGPOINTUTILITIES_H

#include "FloatingPointTrie/1-D.h"

/**
 * This function is used to check if a number is special or not
 *
 * @param fpNumber The number to check.
 * @param counts The counts of special cases.
 * @return True if the number is special, false otherwise.
 */
bool isSpecialCase(uint64_t fpNumber, SpecialCounts &counts);
bool isSpecialCase_32(uint32_t fpNumber, SpecialCounts &counts);

/**
 * This function is used to file and insert a floating-point number into a trie.
 * (Apollo16 Configuration)
 *
 * @param node The root node of the Trie.
 * @param fpNumber The floating-point number to file to internal rep and insert.
 * @param default_mode The default mode to use.
 */
void createAndInsertFP16(TrieNode_1DxF *node, uint64_t fpNumber,
                         uint64_t &curr_trie_size, bool default_mode);
void createAndInsertFP16_32(TrieNode_1DxF *node, uint32_t fpNumber,
                            uint64_t &curr_trie_size, bool default_mode);
/**
 * This function is used to file and insert a floating-point number into a
 * trie.(Roaring20 Configuration)
 *
 * @param node The root node of the Trie.
 * @param fpNumber The floating-point number to file to internal rep and insert.
 * @param default_mode The default mode to use.
 */

// ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

#endif // FPH_FLOATINGPOINTUTILITIES_H
