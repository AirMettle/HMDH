/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#ifndef FLOATINGPOINTHISTOGRAM_UTILS_H
#define FLOATINGPOINTHISTOGRAM_UTILS_H

#include <iostream>
#include <vector>
#include <fstream>
#include <openssl/evp.h>
#include <openssl/types.h>

std::vector<double> readBinaryFile(const std::string &filename);
std::string hashBuffer(const std::vector<char> &buffer);
std::string readValueFromJson(const std::string &filename,
                              const std::string &key);



#endif // FLOATINGPOINTHISTOGRAM_UTILS_H
