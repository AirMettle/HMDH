/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FLOATINGPOINTHISTOGRAM_EXPORTUTILITIES_H
#define FLOATINGPOINTHISTOGRAM_EXPORTUTILITIES_H

#include "FloatingPointTrie/1-D.h"

/**
 * Writes data from a buffer to a file in binary mode.
 *
 * @param buffer Reference to a vector of characters to be written to the file.
 * @param filename Name (and optional path) of the file. If only a name is
 * provided, the file is created in the current working directory.
 */
void Write(const std::vector<char> &buffer, const std::string &filename);

void exportTrieToCSV (TrieNode_1DxF *node,const std::string &filename);


#endif