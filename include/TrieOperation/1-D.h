/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FPH_TRIEOPERATION_H
#define FPH_TRIEOPERATION_H

#include "FloatingPointTrie/1-D.h"

/**
 * This function is used to Create root node of the Trie for Apollo16
 * Configuration.
 *
 * @return The root node of the Trie.
 */
std::unique_ptr<TrieNode_1DxF> CreateParentNode_1DxF();


#endif //FPH_TRIEOPERATION_H
