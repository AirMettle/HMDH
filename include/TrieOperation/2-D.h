/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef TRIEOPERATION_2D_H
#define TRIEOPERATION_2D_H

#include "FloatingPointTrie/1-D.h"
#include "FloatingPointTrie/2-D.h"

/**
 * This function is used to Create root node of the Trie for 2-D
 * Configuration.
 *
 * @return The root node of the Trie.
 */
std::unique_ptr<TLETrieNode_2DxF> CreateParentNode_TLE2DxF();

#endif // TRIEOPERATION_2D_H