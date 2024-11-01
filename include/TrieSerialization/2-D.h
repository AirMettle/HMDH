/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION2D_H
#define FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION2D_H


#include "FloatingPointTrie/1-D.h"
#include "FloatingPointTrie/2-D.h"



void serialize2DxF(const TLETrieNode_2DxF *node, std::vector<char> &buffer);

std::unique_ptr<TLETrieNode_2DxF> deserialize2DxF(std::vector<char> buffer,
                                                  size_t &offset);

[[nodiscard]] std::pair<std::unique_ptr<TLETrieNode_2DxF>, trie_header>
processBuffer2DxF(const std::vector<char> &buffer);

#endif // FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION_H