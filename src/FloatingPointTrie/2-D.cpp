/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointTrie/2-D.h"
#include "FloatingPointTrie/ND.h"
#include "include/FeatureFlags.h"
#include "include/FloatingPointUtilities/2-D.h"
#include "include/FloatingPointUtilities/ND.h"
#include "include/TrieOperation/2-D.h"
#include "include/TrieSerialization/1-D.h"
#include "include/TrieSerialization/2-D.h"
#include "lib/include/logger.h"
#include <memory>

#ifdef BENCHMARK_MODE
#include "include/Timer.h"
#endif

std::vector<char> generate_2DxF(const FPHArray &array1, const FPHArray &array2,
                                 bool default_mode) {
  uint64_t curr_trie_size = 0;
  std::unique_ptr<TLETrieNode_2DxF> root =
      execCreateAndInsert_2D(array1, array2, curr_trie_size, default_mode);

  std::vector<char> buffer =
      execSerialize_2D(root.get(), curr_trie_size, default_mode);

  return buffer;
}

std::unique_ptr<TLETrieNode_2DxF> execCreateAndInsert_2D(const FPHArray &array1,
                                                         const FPHArray &array2,
                                                         uint64_t &curr_trie_size,
                                                         bool default_mode) {
  if (array1.length != array2.length) {
    LOG_ERROR("Length of Dimension-1 and Dimension-2 is not equal");
  }

  std::unique_ptr<TLETrieNode_2DxF> root = CreateParentNode_TLE2DxF();
  curr_trie_size += sizeof(TLETrieNode_2DxF);

  unsigned int internalFPHNumber1 = 0;
  unsigned int internalFPHNumber2 = 0;

  for (int i = 0; i < array1.length; ++i) {
    TLE tle1, tle2;

    switch (array1.type) {
    case FPH_dtype::Double: {
      uint64_t fpNumber1_64;
      std::memcpy(&fpNumber1_64, &static_cast<const double *>(array1.values)[i],
                  sizeof(double));
      tle1 = setTLEComponents(fpNumber1_64);
      internalFPHNumber1 = createInternal8Bit(fpNumber1_64, default_mode);
      break;
    }
    case FPH_dtype::Float: {
      uint32_t fpNumber1_32;
      std::memcpy(&fpNumber1_32, &static_cast<const float *>(array1.values)[i],
                  sizeof(float));
      tle1 = setTLEComponents_32(fpNumber1_32);
      internalFPHNumber1 = createInternal8Bit_32(fpNumber1_32, default_mode);
      break;
    }
    case FPH_dtype::Int32: {
      float floatValue =
          int32_to_float(static_cast<const int32_t *>(array1.values)[i]);
      uint32_t fpNumber1_32;
      std::memcpy(&fpNumber1_32, &floatValue, sizeof(floatValue));
      tle1 = setTLEComponents_32(fpNumber1_32);
      internalFPHNumber1 = createInternal8Bit_32(fpNumber1_32, default_mode);
      break;
    }
    case FPH_dtype::Int64: {
      float floatValue =
          int64_to_float(static_cast<const int64_t *>(array1.values)[i]);
      uint32_t fpNumber1_32;
      std::memcpy(&fpNumber1_32, &floatValue, sizeof(floatValue));
      tle1 = setTLEComponents_32(fpNumber1_32);
      internalFPHNumber1 = createInternal8Bit_32(fpNumber1_32, default_mode);
      break;
    }
    default:
      LOG_ERROR("Unknown data type for Dimension 1");
      break;
    }

    switch (array2.type) {
    case FPH_dtype::Double: {
      uint64_t fpNumber2_64;
      std::memcpy(&fpNumber2_64, &static_cast<const double *>(array2.values)[i],
                  sizeof(double));
      tle2 = setTLEComponents(fpNumber2_64);
      internalFPHNumber2 = createInternal8Bit(fpNumber2_64, default_mode);
      break;
    }
    case FPH_dtype::Float: {
      uint32_t fpNumber2_32;
      std::memcpy(&fpNumber2_32, &static_cast<const float *>(array2.values)[i],
                  sizeof(float));
      tle2 = setTLEComponents_32(fpNumber2_32);
      internalFPHNumber2 = createInternal8Bit_32(fpNumber2_32, default_mode);
      break;
    }
    case FPH_dtype::Int32: {
      float floatValue =
          int32_to_float(static_cast<const int32_t *>(array2.values)[i]);
      uint32_t fpNumber2_32;
      std::memcpy(&fpNumber2_32, &floatValue, sizeof(floatValue));
      tle2 = setTLEComponents_32(fpNumber2_32);
      internalFPHNumber2 = createInternal8Bit_32(fpNumber2_32, default_mode);
      break;
    }
    case FPH_dtype::Int64: {
      float floatValue =
          int64_to_float(static_cast<const int64_t *>(array2.values)[i]);
      uint32_t fpNumber2_32;
      std::memcpy(&fpNumber2_32, &floatValue, sizeof(floatValue));
      tle2 = setTLEComponents_32(fpNumber2_32);
      internalFPHNumber2 = createInternal8Bit_32(fpNumber2_32, default_mode);
      break;
    }
    default:
      LOG_ERROR("Unknown data type for Dimension 2");
      break;
    }
    unsigned int combinedTLE = (tle1.TLE << 3) | tle2.TLE;

    // Check special conditions and set ndims accordingly
    unsigned int isTle1Special =
        (tle1.TLE == 0 || tle1.TLE == 1 || tle1.TLE == 4 || tle1.TLE == 7);
    unsigned int isTle2Special =
        (tle2.TLE == 0 || tle2.TLE == 1 || tle2.TLE == 4 || tle2.TLE == 7);
    unsigned int ndims = (~((isTle1Special << 1) | isTle2Special)) & 0x3;

    unsigned int combined = 0;

    switch (ndims) {
    case 0:
      combined = 0; // No need to compute internal numbers
      break;
    case 1:
      combined = internalFPHNumber2;
      break;
    case 2:
      combined = internalFPHNumber1;
      break;
    case 3:
      combined = combine_chunks_2DxF(internalFPHNumber1, internalFPHNumber2);
      break;
    default:
      LOG_ERROR("Invalid value for ndims");
      break;
    }
      insertintoTLETrie_2DxF(
              root.get(), combined, combinedTLE, ndims, curr_trie_size);
    if (enable_threshold_2D && curr_trie_size > threshold_2D) {
      LOG_ERROR("Trie size exceeded threshold limit of {}.", threshold_2D);
      LOG_ERROR(
          "Insertion process stopped at index {} of the input dataset.", i);
      LOG_ERROR("Last filed values -> @ 0: {} @ 1: {}",
                static_cast<const double *>(array1.values)[i],
                static_cast<const double *>(array2.values)[i]);
      break;
    }
  }

  return root;
}

std::vector<char> execSerialize_2D(TLETrieNode_2DxF *root,
                                   uint64_t &curr_trie_size,
                                   bool default_mode) {
  std::vector<char> buffer;

  buffer.reserve(sizeof(trie_header) + curr_trie_size);
  trie_header t_header;
  strcpy(t_header.type_code, "HMDH");
  t_header.version = 0;
  t_header.m_width = 3;
  t_header.precision_bits = 5;
  t_header.node_width = 8;
  strcpy(t_header.type, "2-D");
  strcpy(t_header.config, "288");
  t_header.mode = default_mode ? 1 : 0;
  // Add the logic to handle the special counts from the TLE
  t_header.trie_root_ref = sizeof(trie_header);
  serializeTrieHeader(t_header, buffer);
  // serialization logic goes here
    serialize2DxF(root, buffer);

  return buffer;
}

