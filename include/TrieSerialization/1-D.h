/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION_H
#define FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION_H

#include "BooleanArray.h"
#include "FloatingPointTrie/1-D.h"

[[nodiscard]] std::pair<std::unique_ptr<TrieNode_1DxF>, trie_header>
processBuffer1DxF(const std::vector<char> &buffer);


[[nodiscard]] std::vector<char> serializeCounts(const uint32_t counts[],
                                                size_t len);
[[nodiscard]] std::vector<uint32_t>
deserializeCounts(const std::vector<char> &buffer, size_t &offset, size_t len);
[[nodiscard]] std::vector<char>
serializeCompactBooleanArray(const BooleanArray &array);
[[nodiscard]] std::vector<uint64_t>
deserializeCompactBooleanArray(const std::vector<char> &buffer, size_t &offset,
                               std::size_t len);

void serializeTrieNode_1DxF_compact(const TrieNode_1DxF *node,
                                    std::vector<char> &buffer);
void serializeTrieNode_1DxF_ND(const TrieNode_1DxF *node,
                               std::vector<char> &buffer);
std::unique_ptr<TrieNode_1DxF>
deserializeTrieNode_1DxF_compact(const std::vector<char> &buffer, size_t &offset);

void serializeTrieHeader(const trie_header &header, std::vector<char> &buffer);
trie_header deserializeTrieHeader(const std::vector<char> &buffer,
                                  size_t &offset);

[[nodiscard]] bool verifyEndOfFileMarker(const std::vector<char> &buffer,
                                         size_t &offset);

// Compact serialization functions end

#endif // FLOATINGPOINTHISTOGRAM_TRIESERIALIZATION_H