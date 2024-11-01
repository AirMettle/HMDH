/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "2d.hpp"

#include "FloatingPointTrie/1-D.h"
#include "FloatingPointTrie/2-D.h"
#include "TrieSerialization/2-D.h"
#include <cstddef>
#include <vector>


nlohmann::json toJsonFlattendTLE2x8(const std::vector<char> &buffer,
                                    size_t endOfHeaderOffset) {
  // Deserialize the trie from the buffer
  size_t offset = endOfHeaderOffset;
  std::unique_ptr<TLETrieNode_2DxF> node = deserialize2DxF(buffer, offset);

  int y_axis = 8;
  int x_axis = 8;
  std::vector<std::vector<uint32_t>> flattend_counts(
      y_axis, std::vector<uint32_t>(x_axis, 0));

  for (size_t i = 0; i < BINS_64; i++) {
    int y_idx = i & 0x7;
    int x_idx = (i >> 3) & 0x7;

    flattend_counts[y_idx][x_idx] += node->TLEcounts[i];
  }

  nlohmann::json j;

  for (int y = 0; y < flattend_counts.size(); y++) {
    std::string yKey = "y" + std::to_string(y);

    nlohmann::json xArray = nlohmann::json::array();
    for (int x = 0; x < flattend_counts[y].size(); x++) {

      xArray.push_back(static_cast<double>(flattend_counts[y][x]));
    }

    // Add the yArray to the JSON object with the zKey
    j[yKey] = xArray;
  }

  return j;
}

nlohmann::json drill2D2x8(const std::vector<char> &buffer,
                          size_t endOfHeaderOffset, int x, int y) {

  // Deserialize the trie from the buffer
  size_t offset = endOfHeaderOffset;
  std::unique_ptr<TLETrieNode_2DxF> node = deserialize2DxF(buffer, offset);

  nlohmann::json drillArray = nlohmann::json::array();
  int combinedTLE = (x << 3) | y;

  uint32_t nDims = getNumDims2D(combinedTLE);
  auto l0 = node->nodes[combinedTLE].get();
  if (l0 == nullptr) {
    std::cout << "Node is null" << std::endl;
    nlohmann::json drillNode;
    drillNode["M_X"] = std::bitset<4>(0).to_string();
    drillNode["M_Y"] = std::bitset<4>(0).to_string();
    drillNode["P_X"] = std::bitset<4>(0).to_string();
    drillNode["P_Y"] = std::bitset<4>(0).to_string();
    drillNode["Count"] = 0;
    drillNode["TLE_X"] = x;
    drillNode["TLE_Y"] = y;
    drillArray.push_back(drillNode);

    return drillArray;
  }

  // 1 dim is non-special
  if (nDims == 1 or nDims == 2) {
    for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
      // Looping through a specific TrieNode_1DxF bucket
      if (!l0->populated.test(i)) {
        continue;
      }
      nlohmann::json drillNode;
      int msb8bits = i & 0xFF;
      int mBits = (msb8bits >> 4) & 0xF;
      int pBits = msb8bits & 0xF;
      switch (nDims) {
      case 1:
        drillNode["M_X"] = std::bitset<4>(0).to_string();
        drillNode["M_Y"] = std::bitset<4>(mBits).to_string();
        drillNode["P_X"] = std::bitset<4>(0).to_string();
        drillNode["P_Y"] = std::bitset<4>(pBits).to_string();
        drillNode["Count"] = l0->count_level0[i];
        drillNode["TLE_X"] = x;
        drillNode["TLE_Y"] = y;
        drillArray.push_back(drillNode);
        break;
      case 2:
        drillNode["M_X"] = std::bitset<4>(mBits).to_string();
        drillNode["M_Y"] = std::bitset<4>(0).to_string();
        drillNode["P_X"] = std::bitset<4>(pBits).to_string();
        drillNode["P_Y"] = std::bitset<4>(0).to_string();
        drillNode["Count"] = l0->count_level0[i];
        drillNode["TLE_X"] = x;
        drillNode["TLE_Y"] = y;
        drillArray.push_back(drillNode);
      }
    }
    return drillArray;
  }

  // 2 dims are non-special
  if (nDims == 3) {
    for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
      // Looping through a specific Node3D_888_l0 bucket
      if (!l0->populated.test(i) and l0->nodes[i] == nullptr) {
        continue;
      }
      int msb8bits = i & 0xFF;
      auto l1 = l0->nodes[i].get();
      for (size_t j = 0; j < LEVEL1_BUCKET_SIZE_1DxF; j++) {
        if (l1->count[j] == 0) {
          continue;
        }
        int lsb8bits = j & 0xFF;
        int combinedBits = (msb8bits << 8) | lsb8bits;
        int m1_bits = (((combinedBits >> 15) & 1) << 2)
                      | (((combinedBits >> 14) & 1) << 1)
                      | (((combinedBits >> 13) & 1) << 0);
        int m2_bits = (((combinedBits >> 12) & 1) << 2)
                      | (((combinedBits >> 11) & 1) << 1)
                      | (((combinedBits >> 10) & 1) << 0);
        int p1_bits = (((combinedBits >> 9) & 1) << 4)
                      | (((combinedBits >> 8) & 1) << 3)
                      | (((combinedBits >> 7) & 1) << 2)
                      | (((combinedBits >> 6) & 1) << 1)
                      | (((combinedBits >> 5) & 1) << 0);
        int p2_bits = (((combinedBits >> 4) & 1) << 4)
                      | (((combinedBits >> 3) & 1) << 3)
                      | (((combinedBits >> 2) & 1) << 2)
                      | (((combinedBits >> 1) & 1) << 1)
                      | (((combinedBits >> 0) & 1) << 0);

         // Constructing M_X, M_Y, P_X, and P_Y based on requirements
          int M_X_bits = (m1_bits << 1) | ((p1_bits >> 4) & 1);  // 3 bits of m1 + first bit of p1
          int M_Y_bits = (m2_bits << 1) | ((p2_bits >> 4) & 1);  // 3 bits of m2 + first bit of p2
          int P_X_bits = p1_bits & 0x0F;  // Last 4 bits of p1 (exclude the first bit)
          int P_Y_bits = p2_bits & 0x0F;  // Last 4 bits of p2 (exclude the first bit)

          nlohmann::json drillNode;
          drillNode["M_X"] = std::bitset<4>(M_X_bits).to_string();  // 3 bits of m1 + 1 bit of p1
          drillNode["M_Y"] = std::bitset<4>(M_Y_bits).to_string();  // 3 bits of m2 + 1 bit of p2
          drillNode["P_X"] = std::bitset<4>(P_X_bits).to_string();  // Last 4 bits of p1
          drillNode["P_Y"] = std::bitset<4>(P_Y_bits).to_string();  // Last 4 bits of p2
          drillNode["Count"] = l0->nodes[i]->count[j];
          drillNode["Count_Level1"] = l0->count_level0[i];
          drillNode["TLE_X"] = x;
          drillNode["TLE_Y"] = y;
        drillArray.push_back(drillNode);
      }
    }
    return drillArray;
  }
  return drillArray;
}