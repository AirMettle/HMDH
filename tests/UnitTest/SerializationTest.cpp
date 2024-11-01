/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include "include/FloatingPointTrie/2-D.h"
#include "include/TrieSerialization/1-D.h"
#include "include/TrieSerialization/2-D.h"

#include "gtest/gtest.h"
#include "tests/utils/utils.h"

const std::string dim1_file_path = "../../tests/TestData/dim1_vx.bin";
const std::string dim2_file_path = "../../tests/TestData/dim2_vy.bin";

const std::vector<double> dim1_data = readBinaryFile(dim1_file_path);
const std::vector<double> dim2_data = readBinaryFile(dim2_file_path);

const FPHArray array0 =
    buildFPHArray(dim1_data.data(), static_cast<int>(dim1_data.size()));
const FPHArray array1 =
    buildFPHArray(dim2_data.data(), static_cast<int>(dim2_data.size()));
const bool default_mode = true;


class SerializationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // The SetUp method is currently empty because there is no special
    // initialization required for this specific test suite.
  }


  void TearDown() override {
    // The TearDown method is empty because there are no resources to clean up
    // after the tests in this suite.
  }
};


TEST_F(SerializationTest, VerifiesSerializationAndDeserialization) {
  uint32_t counts[5] = {255, 65535, 16777215, 12343, 12345};
  std::vector<char> buffer = serializeCounts(counts, 5);
  size_t offset = 0;
  std::vector<uint32_t> deserializedCounts =
      deserializeCounts(buffer, offset, 5);
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ(counts[i], deserializedCounts[i]);
  }
}

bool compareNodes2Dl1(const std::unique_ptr<TrieNode_1DxF_Level1> &node1,
                      const std::unique_ptr<TrieNode_1DxF_Level1> &node2) {
  for (size_t i = 0; i < BINS_256; i++) {
    if (node1->count[i] != node2->count[i]) {
      std::cout << "compareNodes2Dl1: Mismatch at index " << i
                << ": node1->count[" << i << "] = " << node1->count[i]
                << ", node2->count[" << i << "] = " << node2->count[i]
                << std::endl;
      return false;
    }
  }
  return true;
}

bool compareNodes2Dl0(const std::unique_ptr<TrieNode_1DxF> &node1,
                      const std::unique_ptr<TrieNode_1DxF> &node2) {
  for (size_t i = 0; i < BINS_256; i++) {

    if (node1->populated[i] != node2->populated[i]) {
      std::cout << "compareNodes2Dl0: Mismatch at index " << i
                << ": node1->populated[" << i << "] = " << node1->populated[i]
                << ", node2->populated[" << i << "] = " << node2->populated[i]
                << std::endl;
      return false;
    }
    if (node1->count_level0[i] != node2->count_level0[i]) {
      std::cout << "compareNodes2Dl0: Mismatch at index " << i
                << ": node1->count_level0[" << i
                << "] = " << node1->count_level0[i] << ", node2->count_level0["
                << i << "] = " << node2->count_level0[i] << std::endl;
      return false;
    }
    if (node1->nodes[i] != nullptr && node2->nodes[i] != nullptr) {
      if (!compareNodes2Dl1(node1->nodes[i], node2->nodes[i])) {
        std::cout << "compareNodes2Dl0: Mismatch in child nodes at index " << i
                  << std::endl;
        return false;
      }
    } else if (node1->nodes[i] != node2->nodes[i]) {
      std::cout << "compareNodes2Dl0: One of the nodes is null at index " << i
                << ": node1->nodes[" << i
                << "] = " << (node1->nodes[i] ? "not null" : "null")
                << ", node2->nodes[" << i
                << "] = " << (node2->nodes[i] ? "not null" : "null")
                << std::endl;
      return false;
    }
  }
  return true;
}


TEST_F(SerializationTest, VerifiesDeserializationConstructsCorrectTrie_2x8) {
  uint64_t curr_trie_size = 0;
  std::unique_ptr<TLETrieNode_2DxF> root =
      execCreateAndInsert_2D(array0, array1, curr_trie_size, true);
  std::vector<char> buffer = execSerialize_2D(root.get(), curr_trie_size, true);

  auto result = processBuffer2DxF(buffer);

  std::unique_ptr<TLETrieNode_2DxF> deserializedRoot = std::move(result.first);

  if (deserializedRoot == nullptr) {
    std::cout << "Deserialized root is null" << std::endl;
  }

  for (size_t i = 0; i < BINS_64; i++) {
    EXPECT_EQ(root->populated[i], deserializedRoot->populated[i]);
    EXPECT_EQ(root->TLEcounts[i], deserializedRoot->TLEcounts[i]);

    if (root->nodes[i] != nullptr && deserializedRoot->nodes[i] != nullptr) {
      EXPECT_EQ(
          compareNodes2Dl0(root->nodes[i], deserializedRoot->nodes[i]), true);
    }
  }
}

