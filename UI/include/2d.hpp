#ifndef FPH_DESERIALIZER_2D_HPP
#define FPH_DESERIALIZER_2D_HPP

#include "nlohmann/json.hpp"

// Function to deserialize the TLE and convert it to a flattened JSON object
nlohmann::json toJsonFlattendTLE2x8(const std::vector<char> &buffer,
                                    size_t endOfHeaderOffset);
nlohmann::json drill2D2x8(const std::vector<char> &buffer,
                          size_t endOfHeaderOffset, int x, int y);


#endif // FPH_DESERIALIZER_2D_HPP
