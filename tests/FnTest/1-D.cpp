/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include "include/FloatingPointTrie/1-D.h"
#include <gtest/gtest.h>
#include "tests/utils/utils.h"



const std::string dim1_file_path = "../../tests/TestData/dim1_vx.bin";
const std::string result_file_path =
    "../../tests/FnTest_Results/1D_output.json";

// Read the data from the binary file
const std::vector<double> dim1_data = readBinaryFile(dim1_file_path);

TEST(Generate_Apollo16, Generate_Apollo16_fntest) {
  // Check that data is non-empty and has the expected structure
  ASSERT_FALSE(dim1_data.empty()) << "Data from binary file is empty.";

  FPHArray array0 =
      buildFPHArray(dim1_data.data(), static_cast<int>(dim1_data.size()));

  std::vector<char> result = generate_1DxF(array0);

  std::string result_hash = hashBuffer(result);

  std::string expected_hash = readValueFromJson(result_file_path, "1DxF");

  EXPECT_EQ(result_hash, expected_hash);
}

