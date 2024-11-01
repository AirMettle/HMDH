/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include "include/BooleanArray.h"
#include "include/FloatingPointTrie/1-D.h"
#include "include/FloatingPointUtilities/1-D.h"
#include "include/TrieOperation/1-D.h"
#include "include/TrieSerialization/1-D.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <iostream>

// THIS TEST CASE IS FOR TESTING APOLLO16 INTERNAL FILING

class CreateAndInsertFPTest_Apollo16 : public ::testing::Test {
protected:
  double orignalNumber;
  std::unique_ptr<TrieNode_1DxF> root = CreateParentNode_1DxF();
  uint64_t curr_trie_size = 0;
  bool default_mode = true;

  void setManualExponent(double number, uint64_t manualExponent) {
    uint64_t fpTemp;
    std::memcpy(&fpTemp, &number, sizeof(number));

    // Clear the existing exponent bits
    fpTemp &= ~(0x7FFULL << 52);

    // Set the manual exponent
    fpTemp |= (manualExponent << 52);

    // Update the original number with the new exponent
    std::memcpy(&orignalNumber, &fpTemp, sizeof(fpTemp));
  }
};

TEST_F(CreateAndInsertFPTest_Apollo16, NormalOperation_TestCase1) {

  double originalNumber = 1.113;
  uint64_t fpNumber;
  std::memcpy(&fpNumber, &originalNumber, sizeof(originalNumber));

  unsigned int expectedIndex8 =
      0b00110111; // Refer FpInternalManualTest.cpp Test Case 1
  unsigned int expectedLast8 = 0b00111011;

  createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);

  ASSERT_NE(root->nodes[expectedIndex8], nullptr);
  EXPECT_EQ(root->populated[expectedIndex8], true);
  EXPECT_EQ(root->nodes[expectedIndex8]->count[expectedLast8], 1);
}

TEST_F(CreateAndInsertFPTest_Apollo16, NormalOperation_TestCase2) {

  double originalNumber = 85.125;
  uint64_t fpNumber;
  std::memcpy(&fpNumber, &originalNumber, sizeof(originalNumber));

  unsigned int expectedIndex8 =
      0b00011110; // Refer FpInternalManualTest.cpp Test Case 2
  unsigned int expectedIndex5 = 0b01010100;

  createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);

  ASSERT_NE(root->nodes[expectedIndex8], nullptr);
  EXPECT_EQ(root->populated[expectedIndex8], true);
  EXPECT_EQ(root->nodes[expectedIndex8]->count[expectedIndex5], 1);
}

TEST_F(CreateAndInsertFPTest_Apollo16, NegativeExponent_TestCase1) {

  double originalNumber = 85.125;
  uint64_t fpNumber;
  std::memcpy(&fpNumber, &originalNumber, sizeof(originalNumber));
  fpNumber = 0b0011111111100000000000000000000000000000000000000000000000000000;

  unsigned int expectedIndex8 =
      0b01111100; // Refer FpInternalManualTest.cpp Test Case 2
  unsigned int expectedIndex5 = 0b00000000;

  createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);

  ASSERT_NE(root->nodes[expectedIndex8], nullptr);
  EXPECT_EQ(root->populated[expectedIndex8], true);
  EXPECT_EQ(root->nodes[expectedIndex8]->count[expectedIndex5], 1);
}

TEST_F(CreateAndInsertFPTest_Apollo16, NegativeExponent_TestCase2) {

  uint64_t fpNumber;

  fpNumber = 0b0010000111000000000000000000000000000000000000000000000000000000;

  unsigned int expectedIndex8 =
      0b01000111; // Refer FpInternalManualTest.cpp Test Case 2
  unsigned int expectedIndex5 = 0b10001000;

  createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);

  ASSERT_NE(root->nodes[expectedIndex8], nullptr);
  EXPECT_EQ(root->populated[expectedIndex8], true);
  EXPECT_EQ(root->nodes[expectedIndex8]->count[expectedIndex5], 1);
}

TEST_F(CreateAndInsertFPTest_Apollo16, NegativeExponent_TestCase3) {

  double originalNumber = 1.0;
  uint64_t fpNumber;

  setManualExponent(originalNumber, 1); // Orignal Number - 2.22507e-308
  std::memcpy(&fpNumber, &orignalNumber, sizeof(orignalNumber));

  default_mode = true;

  unsigned int expectedIndex8 =
      0b01000011; // Refer FpInternalManualTest.cpp Test Case 2
  unsigned int expectedIndex5 = 0b11111010;

  createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);

  ASSERT_NE(root->nodes[expectedIndex8], nullptr);
  EXPECT_EQ(root->populated[expectedIndex8], true);
  EXPECT_EQ(root->nodes[expectedIndex8]->count[expectedIndex5], 1);
}
