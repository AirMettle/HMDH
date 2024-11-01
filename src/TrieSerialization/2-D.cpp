/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include "include/FloatingPointTrie/2-D.h"
#include "include/FloatingPointUtilities/ND.h"
#include "include/TrieSerialization/1-D.h"
#include "include/TrieSerialization/2-D.h"
#include "lib/include/logger.h"


std::unique_ptr<TrieNode_1DxF> deserialize2D_88_l0(std::vector<char> &buffer,
                                                   size_t &offset, int level);
std::unique_ptr<TrieNode_1DxF_Level1>
deserialize2D_88_l1(std::vector<char> &buffer, size_t &offset,
                    int level [[maybe_unused]]);


void serialize2DxF(const TLETrieNode_2DxF *node, std::vector<char> &buffer) {
  BooleanArray compact_array = BooleanArray(BINS_64 / 64);
  for (size_t i = 0; i < BINS_64; i++) {
    if (node->populated[i]) {
      compact_array.set(i, true);
    }
  }

  // Store compact array to buffer
  auto compact_array_buffer = serializeCompactBooleanArray(compact_array);
  buffer.insert(
      buffer.end(), compact_array_buffer.begin(), compact_array_buffer.end());

  // Store count for buckets with populated bit set using minBits
  auto counts_buffer = serializeCounts(node->TLEcounts, BINS_64);
  buffer.insert(buffer.end(), counts_buffer.begin(), counts_buffer.end());

  for (size_t i = 0; i < BINS_64; i++) {
    if (node->populated[i] && node->nodes[i]) {
        serializeTrieNode_1DxF_ND(node->nodes[i].get(), buffer);
    }
  }

  // add end of file marker
  int32_t endOfFileMarker = -1;
  auto marker_bytes = reinterpret_cast<const char *>(&endOfFileMarker);
  buffer.insert(
      buffer.end(), marker_bytes, marker_bytes + sizeof(endOfFileMarker));
}

std::pair<std::unique_ptr<TLETrieNode_2DxF>, trie_header>
processBuffer2DxF(const std::vector<char> &buffer) {

  size_t offset = 0;
  trie_header t_header_deserialized = deserializeTrieHeader(buffer, offset);

  std::unique_ptr<TLETrieNode_2DxF> node = nullptr;

  if (offset < buffer.size()) {
    node = deserialize2DxF(buffer, offset);
  } else {
    LOG_ERROR("Deserialization failed. Offset is out of bounds.");
  }

  return std::make_pair(std::move(node), t_header_deserialized);
}

std::unique_ptr<TLETrieNode_2DxF> deserialize2DxF(std::vector<char> buffer,
                                                  size_t &offset) {
  auto node = std::make_unique<TLETrieNode_2DxF>();

  std::vector<uint64_t> compact_arr_values =
      deserializeCompactBooleanArray(buffer, offset, BINS_64 / 64);
  BooleanArray compact_array = BooleanArray(compact_arr_values);

  for (int i = 0; i < BINS_64; i++) {
    node->populated[i] = compact_array.get(i);
  }

  auto counts = deserializeCounts(buffer, offset, node->populated.count());
  auto count_idx = 0;
  for (int i = 0; i < BINS_64; i++) {
    if (node->populated[i]) {
      node->TLEcounts[i] = counts[count_idx];
      count_idx++;
    }
  }

  for (int i = 0; i < BINS_64; i++) {
    if (node->populated[i]) {
      int nDims = getNumDims2D(i);
      switch (nDims) {
      case 0:
        continue;
      case 1:
      case 2:
        node->nodes[i] = deserialize2D_88_l0(buffer, offset, 1);
        break;
      case 3:
        node->nodes[i] = deserialize2D_88_l0(buffer, offset, 2);
        break;
      default:
        break;
      }
    }
  }

  if (!verifyEndOfFileMarker(buffer, offset)) {
    LOG_ERROR("End of file marker not found");
    std::cout << "End of file marker not found" << std::endl;
    return node;
  }

  return node;
}

std::unique_ptr<TrieNode_1DxF> deserialize2D_88_l0(std::vector<char> &buffer,
                                                   size_t &offset, int level) {
  auto node = std::make_unique<TrieNode_1DxF>();
  // deserialize compact BooleanArray
  std::vector<uint64_t> compact_arr_values =
      deserializeCompactBooleanArray(buffer, offset, BINS_256 / 64);
  BooleanArray compact_array = BooleanArray(compact_arr_values);
  // Convert compact BooleanArray to populated bitset
  for (size_t i = 0; i < BINS_256; i++) {
    node->populated[i] = compact_array.get(i);
  }

  // Deserialize count for buckets with populated bit set
  auto counts = deserializeCounts(buffer, offset, node->populated.count());
  auto count_idx = 0;
  for (size_t i = 0; i < BINS_256; i++) {
    if (node->populated[i]) {
      node->count_level0[i] = counts[count_idx];
      count_idx++;
    }
  }

  if (level == 1) {
    return node;
  }

  // Recursively deserialize child nodes
  for (size_t i = 0; i < BINS_256; i++) {
    if (node->populated[i]) {
      node->nodes[i] = deserialize2D_88_l1(buffer, offset, level);
    }
  }

  return node;
}

std::unique_ptr<TrieNode_1DxF_Level1>
deserialize2D_88_l1(std::vector<char> &buffer, size_t &offset,
                    int level [[maybe_unused]]) {
  auto node = std::make_unique<TrieNode_1DxF_Level1>();
  // deserialize compact BooleanArray
  std::vector<uint64_t> compact_arr_values =
      deserializeCompactBooleanArray(buffer, offset, BINS_256 / 64);
  BooleanArray compact_array = BooleanArray(compact_arr_values);

  // Deserialize count for buckets with populated bit set
  auto counts = deserializeCounts(buffer, offset, compact_array.count());
  auto count_idx = 0;
  for (size_t i = 0; i < BINS_256; i++) {
    if (compact_array.get(i)) {
      node->count[i] = counts[count_idx];
      count_idx++;
    }
  }

  return node;
}
