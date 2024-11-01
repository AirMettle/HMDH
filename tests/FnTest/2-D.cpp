/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include <gtest/gtest.h>
#include "FloatingPointTrie/2-D.h"
#include "tests/utils/utils.h"

const std::string dim1_file_path = "../../tests/TestData/dim1_vx.bin";
const std::string dim2_file_path = "../../tests/TestData/dim2_vy.bin";
const std::string result_file_path =
    "../../tests/FnTest_Results/2D_output.json";

// Read the data from the binary file
const std::vector<double> dim1_data = readBinaryFile(dim1_file_path);
// Read the data from the binary file
const std::vector<double> dim2_data = readBinaryFile(dim2_file_path);

TEST(Generate_2d_88, Generate_2d_88_fntest) {
  // Check that data is non-empty and has the expected structure
  ASSERT_FALSE(dim1_data.empty()) << "Data from dim1.bin file is empty.";
  ASSERT_FALSE(dim2_data.empty()) << "Data from dim2.bin file is empty.";

  FPHArray array0 =
      buildFPHArray(dim1_data.data(), static_cast<int>(dim1_data.size()));
  FPHArray array1 =
      buildFPHArray(dim2_data.data(), static_cast<int>(dim2_data.size()));

  std::vector<char> result = generate_2DxF(array0, array1);

  std::string result_hash = hashBuffer(result);

  std::string expected_hash = readValueFromJson(result_file_path, "2DxF");

  EXPECT_EQ(result_hash, expected_hash);
}

