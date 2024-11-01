/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/ExportUtilities/1-D.h"
#include <fstream>

void Write(const std::vector<char> &buffer, const std::string &filename) {
  std::ofstream file(filename, std::ios::binary);
  if (file.is_open()) {
    file.write(buffer.data(), buffer.size());
    file.close();
  } else {
    std::cerr << "Unable to open file for writing." << std::endl;
  }
}

void exportTrieToCSV(TrieNode_1DxF *node, const std::string &filename) {
    if (!node) {
        std::cout << "Node is nullptr. Cannot export." << std::endl;
        return;
    }

    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write headers
    outFile << "Sign,Exponent,X-offset,Y-offset\n";


    for (int i = 0; i < 256; ++i) {
        if (node->populated.test(i)) {

            std::bitset<8> bits(i);


            // Extracting bits
            int sign = bits[7]; // Assuming most significant bit (MSB) first
            int exponent = bits[6];
            int x_offset =
                    (i) & 0x3F; // Masking to get the next 6 bits after the first two

            // Writing to the primary file
            outFile << sign << "," << exponent << "," << x_offset << ","
                    << node->count_level0[i] << "\n";

        }
    }

    outFile.close();
    std::cout << "Data exported successfully to " << filename << std::endl;
}
