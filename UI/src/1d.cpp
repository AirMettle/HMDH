/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "1d.hpp"

#include "FloatingPointTrie/1-D.h"
#include "TrieSerialization/1-D.h"
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

nlohmann::json toJsonFlattendTLE1DxF(const std::vector<char> &buffer,
                                     size_t endOfHeaderOffset) {
  // Deserialize the trie from the buffer
  size_t offset = endOfHeaderOffset;
  std::unique_ptr<TrieNode_1DxF> node =
      deserializeTrieNode_1DxF_compact(buffer, offset);

  std::map<uint32_t, uint32_t> top_level;
  std::map<uint32_t, std::map<uint32_t, uint32_t>> top_level1;

  size_t top_level_count = 0;
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    // get first 4 bits
    uint32_t msb_4 = (i >> 4) & 0xf;
    uint32_t lsb_4 = i & 0xf;
    if (top_level.find(msb_4) != top_level.end()) {
      top_level[msb_4] += node->count_level0[i];
      std::map<uint32_t, uint32_t> inner_map = top_level1[msb_4];
      inner_map[lsb_4] = node->count_level0[i];
      top_level1[msb_4] = inner_map;
    } else {
      top_level[msb_4] = node->count_level0[i];
      std::map<uint32_t, uint32_t> inner_map;
      inner_map[lsb_4] = node->count_level0[i];
      top_level1[msb_4] = inner_map;
    }
    top_level_count += node->count_level0[i];
  }

  // Using a range-based for loop (C++11 and above)
  size_t dict_sum = 0;
  nlohmann::json payload_0;
  for (const auto &pair : top_level) {
    payload_0[std::to_string(pair.first)] = pair.second;
    dict_sum += pair.second;
  }

  nlohmann::json payload_1;
  size_t inner_sum = 0;
  for (const auto &pair : top_level1) {
    nlohmann::json inner_payload;
    std::map<uint32_t, uint32_t> inner_map = pair.second;
    for (const auto &inner_pair : inner_map) {
      inner_payload[std::to_string(inner_pair.first)] = inner_pair.second;
      inner_sum += inner_pair.second;
    }
    payload_1[std::to_string(pair.first)] = inner_payload;
  }

  if (top_level_count != dict_sum and top_level_count != inner_sum) {
    std::cout << "THIS SHOULD NOT PRINT (DRILL)" << std::endl;
  }
  std::cout << "These sums should be the same. "
              << "top_level_count: " << top_level_count
              << " dict_sum: " << dict_sum << " inner_sum: " << inner_sum
              << std::endl;

  nlohmann::json payload_top_level = nlohmann::json::array();
  payload_top_level.push_back(payload_0);
  payload_top_level.push_back(payload_1);

  std::cout << payload_top_level.dump(4)
            << std::endl; // Pretty-print with an indentation of 4 spaces

  return payload_top_level;
}

nlohmann::json drill1DxF(const std::vector<char> &buffer,
                         size_t endOfHeaderOffset, int top_8_bits) {

  // Deserialize the trie from the buffer
  size_t offset = endOfHeaderOffset;
  std::unique_ptr<TrieNode_1DxF> l0_node =
      deserializeTrieNode_1DxF_compact(buffer, offset);

  std::cout << "L0 count: " << l0_node->count_level0[top_8_bits] << std::endl;

  auto l1_node = l0_node->nodes[top_8_bits].get();

  std::map<uint32_t, uint32_t> top_level;
  std::map<uint32_t, std::map<uint32_t, uint32_t>> top_level1;

  size_t top_level_count = 0;
  for (size_t i = 0; i < LEVEL1_BUCKET_SIZE_1DxF; i++) {
    // get first 4 bits
    uint32_t msb_4 = (i >> 4) & 0xf;
    uint32_t lsb_4 = i & 0xf;
    if (top_level.find(msb_4) != top_level.end()) {
      top_level[msb_4] += l1_node->count[i];
      std::map<uint32_t, uint32_t> inner_map = top_level1[msb_4];
      inner_map[lsb_4] = l1_node->count[i];
      top_level1[msb_4] = inner_map;
    } else {
      top_level[msb_4] = l1_node->count[i];
      std::map<uint32_t, uint32_t> inner_map;
      inner_map[lsb_4] = l1_node->count[i];
      top_level1[msb_4] = inner_map;
    }
    top_level_count += l1_node->count[i];
  }

  // Using a range-based for loop (C++11 and above)
  size_t dict_sum = 0;
  nlohmann::json payload_0;
  for (const auto &pair : top_level) {
    payload_0[std::to_string(pair.first)] = pair.second;
    dict_sum += pair.second;
  }

  nlohmann::json payload_1;
  size_t inner_sum = 0;
  for (const auto &pair : top_level1) {
    nlohmann::json inner_payload;
    std::map<uint32_t, uint32_t> inner_map = pair.second;
    for (const auto &inner_pair : inner_map) {
      inner_payload[std::to_string(inner_pair.first)] = inner_pair.second;
      inner_sum += inner_pair.second;
    }
    payload_1[std::to_string(pair.first)] = inner_payload;
  }

  if (top_level_count != dict_sum and top_level_count != inner_sum) {
    std::cout << "THIS SHOULD NOT PRINT (DRILL)" << std::endl;
  }
  std::cout << "These sums should be the same. "
              << "top_level_count: " << top_level_count
              << " dict_sum: " << dict_sum << " inner_sum: " << inner_sum
              << std::endl;

  nlohmann::json payload_top_level = nlohmann::json::array();
  payload_top_level.push_back(payload_0);
  payload_top_level.push_back(payload_1);

  std::cout << payload_top_level.dump(4)
            << std::endl; // Pretty-print with an indentation of 4 spaces

  return payload_top_level;
}