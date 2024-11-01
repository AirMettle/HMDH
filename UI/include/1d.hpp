#ifndef FPH_DESERIALIZER_1D_HPP
#define FPH_DESERIALIZER_1D_HPP

#include "nlohmann/json.hpp"

// Function to deserialize the TLE and convert it to a flattened JSON object
nlohmann::json toJsonFlattendTLE1DxF(const std::vector<char> &buffer,
                                     size_t endOfHeaderOffset);
nlohmann::json drill1DxF(const std::vector<char> &buffer,
                         size_t endOfHeaderOffset, int x);

#endif // FPH_DESERIALIZER_1D_HPP
