/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointTrie/1-D.h"
#include "include/FeatureFlags.h"
#include "include/FloatingPointTrie/ND.h"
#include "include/FloatingPointUtilities/1-D.h"
#include "include/FloatingPointUtilities/ND.h"
#include "include/TrieOperation/1-D.h"
#include "include/TrieSerialization/1-D.h"
#include "lib/include/logger.h"
#include "include/ExportUtilities/1-D.h"



std::vector<char> generate_1DxF(const FPHArray &array, bool default_mode) {
  SpecialCounts specialCounts;

  uint64_t curr_trie_size = 0;
  std::unique_ptr<TrieNode_1DxF> root = execCreateAndInsert_1DxF(
          specialCounts, curr_trie_size, array, default_mode);

  std::vector<char> buffer = execSerialization_1DxF(
          root, specialCounts, curr_trie_size, default_mode);

  return buffer;
}

std::unique_ptr<TrieNode_1DxF>
execCreateAndInsert_1DxF(SpecialCounts &specialCounts,
                         uint64_t &curr_trie_size, const FPHArray &array,
                         bool default_mode) {
  std::unique_ptr<TrieNode_1DxF> root = CreateParentNode_1DxF();
  curr_trie_size = sizeof(TrieNode_1DxF);

  switch (array.type) {
  case FPH_dtype::Double: {
    const double *values = static_cast<const double *>(array.values);
    for (int i = 0; i < array.length; ++i) {
      uint64_t fpNumber;
      std::memcpy(&fpNumber, &values[i], sizeof(values[i]));
      if (!isSpecialCase(fpNumber, specialCounts)) {
        createAndInsertFP16(root.get(), fpNumber, curr_trie_size, default_mode);
        if (enable_threshold_1D && curr_trie_size > threshold_1D) {
          LOG_ERROR("Trie size exceeded threshold limit of {}.", threshold_1D);
          LOG_ERROR(
              "Insertion process stopped at index {} of the input dataset.", i);
          LOG_ERROR("Last failed value: {}", values[i]);
          break;
        }
      }
    }
  } break;
  case FPH_dtype::Float: {
    const float *values = static_cast<const float *>(array.values);
    for (int i = 0; i < array.length; ++i) {
      uint32_t fpNumber;
      std::memcpy(&fpNumber, &values[i], sizeof(values[i]));
      if (!isSpecialCase_32(fpNumber, specialCounts)) {
        createAndInsertFP16_32(
            root.get(), fpNumber, curr_trie_size, default_mode);
        if (enable_threshold_1D && curr_trie_size > threshold_1D) {
          LOG_ERROR("Trie size exceeded threshold limit of {}.", threshold_1D);
          LOG_ERROR(
              "Insertion process stopped at index {} of the input dataset.", i);
          LOG_ERROR("Last failed value: {}", values[i]);
          break;
        }
      }
    }
  } break;
  case FPH_dtype::Int32: {
    const int32_t *values = static_cast<const int32_t *>(array.values);
    for (int i = 0; i < array.length; ++i) {
      float floatValue = int32_to_float(values[i]);
      uint32_t fpNumber;
      std::memcpy(&fpNumber, &floatValue, sizeof(floatValue));
      if (!isSpecialCase_32(fpNumber, specialCounts)) {
        createAndInsertFP16_32(
            root.get(), fpNumber, curr_trie_size, default_mode);
        if (enable_threshold_1D && curr_trie_size > threshold_1D) {
          LOG_ERROR("Trie size exceeded threshold limit of {}.", threshold_1D);
          LOG_ERROR(
              "Insertion process stopped at index {} of the input dataset.", i);
          LOG_ERROR("Last failed value: {}", values[i]);
          break;
        }
      }
    }
  } break;
  case FPH_dtype::Int64: {
    const int64_t *values = static_cast<const int64_t *>(array.values);
    for (int i = 0; i < array.length; ++i) {
      float floatValue = int64_to_float(values[i]);
      uint32_t fpNumber;
      std::memcpy(&fpNumber, &floatValue, sizeof(floatValue));
      if (!isSpecialCase_32(fpNumber, specialCounts)) {
        createAndInsertFP16_32(
            root.get(), fpNumber, curr_trie_size, default_mode);
        if (enable_threshold_1D && curr_trie_size > threshold_1D) {
          LOG_ERROR("Trie size exceeded threshold limit of {}.", threshold_1D);
          LOG_ERROR(
              "Insertion process stopped at index {} of the input dataset.", i);
          LOG_ERROR("Last failed value: {}", values[i]);
          break;
        }
      }
    }
  } break;
  default:
    LOG_ERROR("Unknown data type");
    break;
  }
  exportTrieToCSV(root.get(), "1DxF.csv");
  return root;
}

std::vector<char>
execSerialization_1DxF(const std::unique_ptr<TrieNode_1DxF> &root,
                       const SpecialCounts &specialCounts,
                       uint64_t trieSize, bool default_mode) {
  std::vector<char> buffer;
  buffer.reserve(sizeof(trie_header) + trieSize);
  trie_header t_header;
  strcpy(t_header.type_code, "HMDH");
  t_header.version = 0;
  t_header.m_width = 4;
  t_header.precision_bits = 10;
  t_header.node_width = 8;
  strcpy(t_header.type, "1-D");
  strcpy(t_header.config, "116");
  t_header.mode = default_mode ? 1 : 0;
  t_header.pos_inf_count = specialCounts.posInfCount;
  t_header.neg_inf_count = specialCounts.negInfCount;
  t_header.pos_zero_count = specialCounts.posZeroCount;
  t_header.neg_zero_count = specialCounts.negZeroCount;
  t_header.nan_count = specialCounts.nanCount;
  t_header.trie_root_ref = sizeof(trie_header);
  serializeTrieHeader(t_header, buffer);
    serializeTrieNode_1DxF_compact(root.get(), buffer);

  return buffer;
}
