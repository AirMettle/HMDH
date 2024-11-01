/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/TrieSerialization/1-D.h"
#include "lib/include/logger.h"

void serializeTrieNode_16_Level1(const TrieNode_1DxF_Level1 *node,
                                 std::vector<char> &buffer);
[[nodiscard]] std::unique_ptr<TrieNode_1DxF_Level1>
deserializeTrieNode_16_Level1(const std::vector<char> &buffer, size_t &offset);

void serializeTrieNode_16_Level1_compact(const TrieNode_1DxF_Level1 *node,
                                         std::vector<char> &buffer);
std::unique_ptr<TrieNode_1DxF_Level1>
deserializeTrieNode_16_Level1_compact(const std::vector<char> &buffer,
                                      size_t &offset);

// Helper functions to compute minimum Bits used for count
// serialization/deserialization
int minimumBits(int maxNumber) {
  if (maxNumber == 0)
    return 1;

  int bits = 0;
  while (maxNumber > 0) {
    maxNumber >>= 1;
    bits++;
  }
  return bits;
}

std::vector<char> serializeCounts(const uint32_t counts[], size_t len) {
  std::vector<char> buffer;
  int maxCount = *std::max_element(counts, counts + len);
  int minBits = minimumBits(maxCount);
  buffer.push_back(
      static_cast<char>(minBits)); // Store minBits as a single byte

  int bitBuffer = 0; // Temporary buffer for bits
  int bitCount = 0;  // Counter for bits in bitBuffer

  for (size_t i = 0; i < len; i++) {
    if (counts[i] > 0) {
      bitBuffer |=
          counts[i]
          << bitCount;     // Shift count left by bitCount and OR into bitBuffer
      bitCount += minBits; // Increment bitCount

      while (bitCount >= 8) { // While bitBuffer has at least one byte
        buffer.push_back(
            static_cast<char>(bitBuffer & 0xFF)); // Push low 8 bits to buffer
        bitBuffer >>= 8;                          // Remove the stored bits
        bitCount -= 8;                            // Decrement bit count by 8
      }
    }
  }

  if (bitCount > 0) { // Handle remaining bits
    buffer.push_back(static_cast<char>(bitBuffer & 0xFF));
  }

  return buffer;
}

std::vector<uint32_t> deserializeCounts(const std::vector<char> &buffer,
                                        size_t &offset, size_t len) {
  if (offset >= buffer.size()) {
    LOG_ERROR("Attempt to read beyond buffer size at initial offset check.");
    return {};
  }

  int minBits = static_cast<unsigned char>(
      buffer[offset++]); // Read the number of bits used per count

  std::vector<uint32_t> counts(len, 0);
  [[maybe_unused]] int currentBit =
      0;                  // Current bit position in the entire stream
  uint32_t bitBuffer = 0; // Buffer to accumulate bits
  int bitsInBuffer = 0;   // Number of valid bits currently in bitBuffer

  for (size_t i = 0; i < len; i++) {
    uint32_t currentCount = 0;

    // Ensure we have enough bits in bitBuffer
    while (bitsInBuffer < minBits) {
      if (offset >= buffer.size()) {
        LOG_ERROR("Buffer underflow while trying to read new byte at index {}",
                  i);
        return {};
      }
      bitBuffer |= (static_cast<unsigned char>(buffer[offset++])
                    << bitsInBuffer); // Shift new byte into bitBuffer
      bitsInBuffer += 8;              // We've added 8 new bits to the buffer
    }

    // Extract the count from bitBuffer
    currentCount =
        bitBuffer & ((1 << minBits) - 1); // Mask out the bits we need
    bitBuffer >>= minBits; // Remove the bits we've just processed
    bitsInBuffer -=
        minBits; // Update the count of valid bits remaining in bitBuffer

    counts[i] = currentCount;
  }

  return counts;
}

std::vector<char> serializeCompactBooleanArray(const BooleanArray &array) {
  std::vector<char> buffer;
  for (size_t i = 0; i < array.get_array().size(); i++) {
    auto compact_value = array.get_array()[i];
    auto compact_value_bytes = reinterpret_cast<const char *>(&compact_value);
    buffer.insert(buffer.end(), compact_value_bytes,
                  compact_value_bytes + sizeof(uint64_t));
  }
  return buffer;
}

std::vector<uint64_t>
deserializeCompactBooleanArray(const std::vector<char> &buffer, size_t &offset,
                               std::size_t len) {
  std::vector<uint64_t> compact_arr_values = std::vector<uint64_t>();

  for (std::size_t i = 0; i < len; i++) {
    if (offset + sizeof(uint64_t) <= buffer.size()) {
      uint64_t compact_value;
      std::memcpy(&compact_value, &buffer[offset], sizeof(uint64_t));
      offset += sizeof(uint64_t);
      compact_arr_values.push_back(compact_value);
    } else {
      LOG_ERROR("Buffer underflow when attempting to deserialize compact "
                "BooleanArray.");
      return std::vector<uint64_t>();
    }
  }
  if (compact_arr_values.size() != len) {
    throw std::runtime_error("Error: Compact array values size mismatch");
    LOG_ERROR("Compact array values size mismatch");
  }
  return compact_arr_values;
}

bool verifyEndOfFileMarker(const std::vector<char> &buffer, size_t &offset) {
  if (offset + sizeof(int32_t) > buffer.size()) {
    LOG_ERROR("Insufficient data for end marker.");
    return false;
  }

  int32_t endOfFileMarker;
  std::memcpy(&endOfFileMarker, &buffer[offset], sizeof(int32_t));
  if (endOfFileMarker == -1) {
    offset += sizeof(int32_t);
    return true;
  } else {
    LOG_ERROR("NODE DESERIALIZATION ERROR : End marker not found at expected "
              "position.");
    return false;
  }
}

void serializeTrieHeader(const trie_header &header, std::vector<char> &buffer) {
  // Append type_code (5 bytes)
  buffer.insert(buffer.end(), header.type_code, header.type_code + 5);

  // Append version (4 bytes)
  buffer.insert(buffer.end(), reinterpret_cast<const char *>(&header.version),
                reinterpret_cast<const char *>(&header.version) +
                    sizeof(header.version));

  // Append m_width (4 bytes)
  buffer.insert(buffer.end(), reinterpret_cast<const char *>(&header.m_width),
                reinterpret_cast<const char *>(&header.m_width) +
                    sizeof(header.m_width));

  // Append precision_bits (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.precision_bits),
                reinterpret_cast<const char *>(&header.precision_bits) +
                    sizeof(header.precision_bits));

  // Append node_width (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.node_width),
                reinterpret_cast<const char *>(&header.node_width) +
                    sizeof(header.node_width));

  // Append type (4 bytes)
  buffer.insert(buffer.end(), header.type, header.type + 4);

  // Append Config (4 bytes)
  buffer.insert(buffer.end(), header.config, header.config + 4);

  // Append mode (4 bytes)
  buffer.insert(buffer.end(), reinterpret_cast<const char *>(&header.mode),
                reinterpret_cast<const char *>(&header.mode) +
                    sizeof(header.mode));

  // Append pos_inf_count (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.pos_inf_count),
                reinterpret_cast<const char *>(&header.pos_inf_count) +
                    sizeof(header.pos_inf_count));

  // Append neg_inf_count (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.neg_inf_count),
                reinterpret_cast<const char *>(&header.neg_inf_count) +
                    sizeof(header.neg_inf_count));

  // Append pos_zero_count (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.pos_zero_count),
                reinterpret_cast<const char *>(&header.pos_zero_count) +
                    sizeof(header.pos_zero_count));

  // Append neg_zero_count (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.neg_zero_count),
                reinterpret_cast<const char *>(&header.neg_zero_count) +
                    sizeof(header.neg_zero_count));

  // Append nan_count (4 bytes)
  buffer.insert(buffer.end(), reinterpret_cast<const char *>(&header.nan_count),
                reinterpret_cast<const char *>(&header.nan_count) +
                    sizeof(header.nan_count));

  // Append trie_root_ref (4 bytes)
  buffer.insert(buffer.end(),
                reinterpret_cast<const char *>(&header.trie_root_ref),
                reinterpret_cast<const char *>(&header.trie_root_ref) +
                    sizeof(header.trie_root_ref));
}

trie_header deserializeTrieHeader(const std::vector<char> &buffer,
                                  size_t &offset) {
  trie_header header;

  // Ensure there's enough data in the buffer
  // 57 is the maximum size of the header , it will never be less than this
  // or more than 57 under any circumstances
  if (buffer.size() < 57) {
    throw std::runtime_error(
        "Buffer too small to contain a complete trie_header");
    LOG_ERROR("Buffer too small to contain a complete trie_header");
  }

  // Read type_code (5 bytes)
  std::memcpy(header.type_code, buffer.data() + offset, 5);
  offset += 5;

  // Read version (4 bytes)
  std::memcpy(&header.version, buffer.data() + offset, sizeof(header.version));
  offset += sizeof(header.version);

  // Read m_width (4 bytes)
  std::memcpy(&header.m_width, buffer.data() + offset, sizeof(header.m_width));
  offset += sizeof(header.m_width);

  // Read precision_bits (4 bytes)
  std::memcpy(&header.precision_bits, buffer.data() + offset,
              sizeof(header.precision_bits));
  offset += sizeof(header.precision_bits);

  // Read node_width (4 bytes)
  std::memcpy(&header.node_width, buffer.data() + offset,
              sizeof(header.node_width));
  offset += sizeof(header.node_width);

  // Read type (4 bytes)
  std::memcpy(header.type, buffer.data() + offset, 4);
  offset += 4;

  // Read Config (4 bytes)
  std::memcpy(header.config, buffer.data() + offset, 4);
  offset += 4;

  // Read mode (4 bytes)
  std::memcpy(&header.mode, buffer.data() + offset, sizeof(header.mode));
  offset += sizeof(header.mode);

  // Read pos_inf_count (4 bytes)
  std::memcpy(&header.pos_inf_count, buffer.data() + offset,
              sizeof(header.pos_inf_count));
  offset += sizeof(header.pos_inf_count);

  // Read neg_inf_count (4 bytes)
  std::memcpy(&header.neg_inf_count, buffer.data() + offset,
              sizeof(header.neg_inf_count));
  offset += sizeof(header.neg_inf_count);

  // Read pos_zero_count (4 bytes)
  std::memcpy(&header.pos_zero_count, buffer.data() + offset,
              sizeof(header.pos_zero_count));
  offset += sizeof(header.pos_zero_count);

  // Read neg_zero_count (4 bytes)
  std::memcpy(&header.neg_zero_count, buffer.data() + offset,
              sizeof(header.neg_zero_count));
  offset += sizeof(header.neg_zero_count);

  // Read nan_count (4 bytes)
  std::memcpy(&header.nan_count, buffer.data() + offset,
              sizeof(header.nan_count));
  offset += sizeof(header.nan_count);

  // Read trie_root_ref (4 bytes)
  std::memcpy(&header.trie_root_ref, buffer.data() + offset,
              sizeof(header.trie_root_ref));
  offset += sizeof(header.trie_root_ref);

  return header;
}

// apollo 16
void serializeTrieNode_1DxF_compact(const TrieNode_1DxF *node,
                                    std::vector<char> &buffer) {
  // Convert populated bitset to compact BooleanArray
  BooleanArray compact_array = BooleanArray(LEVEL0_BUCKET_SIZE_1DxF / 64);
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i]) {
      compact_array.set(i, true);
    }
  }

  // Store compact array to buffer
  auto compact_array_buffer = serializeCompactBooleanArray(compact_array);
  buffer.insert(buffer.end(), compact_array_buffer.begin(),
                compact_array_buffer.end());

  // Store counts
  auto counts_buffer =
      serializeCounts(node->count_level0, LEVEL0_BUCKET_SIZE_1DxF);
  buffer.insert(buffer.end(), counts_buffer.begin(), counts_buffer.end());

  // Recursively serialize child nodes
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i] && node->nodes[i]) {
      serializeTrieNode_16_Level1_compact(node->nodes[i].get(), buffer);
    }
  }

  // Add end of file marker
  int32_t endOfFileMarker = -1;
  auto marker_bytes = reinterpret_cast<const char *>(&endOfFileMarker);
  buffer.insert(buffer.end(), marker_bytes,
                marker_bytes + sizeof(endOfFileMarker));
}

void serializeTrieNode_1DxF_ND(const TrieNode_1DxF *node,
                               std::vector<char> &buffer) {
  // Convert populated bitset to compact BooleanArray
  BooleanArray compact_array = BooleanArray(LEVEL0_BUCKET_SIZE_1DxF / 64);
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i]) {
      compact_array.set(i, true);
    }
  }

  // Store compact array to buffer
  auto compact_array_buffer = serializeCompactBooleanArray(compact_array);
  buffer.insert(buffer.end(), compact_array_buffer.begin(),
                compact_array_buffer.end());

  // Store counts
  auto counts_buffer =
      serializeCounts(node->count_level0, LEVEL0_BUCKET_SIZE_1DxF);
  buffer.insert(buffer.end(), counts_buffer.begin(), counts_buffer.end());

  // Recursively serialize child nodes
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i] && node->nodes[i]) {
      serializeTrieNode_16_Level1_compact(node->nodes[i].get(), buffer);
    }
  }
}

void serializeTrieNode_16_Level1_compact(const TrieNode_1DxF_Level1 *node,
                                         std::vector<char> &buffer) {
  // Build compact boolean array to represent populated bitset
  BooleanArray compact_array = BooleanArray(LEVEL1_BUCKET_SIZE_1DxF / 64);
  for (size_t i = 0; i < LEVEL1_BUCKET_SIZE_1DxF; i++) {
    if (node->count[i] > 0) {
      compact_array.set(i, true);
    }
  }
  // Store compact array to buffer
  auto compact_array_buffer = serializeCompactBooleanArray(compact_array);
  buffer.insert(buffer.end(), compact_array_buffer.begin(),
                compact_array_buffer.end());

  // Store counts for buckets with count > 0
  auto counts_buffer = serializeCounts(node->count, LEVEL1_BUCKET_SIZE_1DxF);
  buffer.insert(buffer.end(), counts_buffer.begin(), counts_buffer.end());
}

std::unique_ptr<TrieNode_1DxF>
deserializeTrieNode_1DxF_compact(const std::vector<char> &buffer,
                                 size_t &offset) {
  auto node = std::make_unique<TrieNode_1DxF>();

  // deserialize compact BooleanArray
  std::vector<uint64_t> compact_arr_values = deserializeCompactBooleanArray(
          buffer, offset, LEVEL0_BUCKET_SIZE_1DxF / 64);
  BooleanArray compact_array = BooleanArray(compact_arr_values);

  // Convert compact BooleanArray to populated bitset
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    node->populated[i] = compact_array.get(i);
  }

  // Deserialize count for buckets with populated bit set
  auto counts = deserializeCounts(buffer, offset, node->populated.count());
  auto count_idx = 0;
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i]) {
      node->count_level0[i] = counts[count_idx];
      count_idx++;
    }
  }

  // Recursively deserialize child nodes
  for (size_t i = 0; i < LEVEL0_BUCKET_SIZE_1DxF; i++) {
    if (node->populated[i]) {
      if (offset < buffer.size()) {
        node->nodes[i] = deserializeTrieNode_16_Level1_compact(buffer, offset);
        if (!node->nodes[i]) {
          LOG_ERROR("Deserialization of child node failed at index {}", i);
          return nullptr;
        }
      } else {
        LOG_ERROR("Buffer underflow when attempting to deserialize children.");
        return nullptr;
      }
    }
  }

  if (!verifyEndOfFileMarker(buffer, offset)) {
    return nullptr;
  }

  return node;
}

std::unique_ptr<TrieNode_1DxF_Level1>
deserializeTrieNode_16_Level1_compact(const std::vector<char> &buffer,
                                      size_t &offset) {
  auto node = std::make_unique<TrieNode_1DxF_Level1>();

  // Deserialize compact BooleanArray
  std::vector<uint64_t> compact_arr_values = deserializeCompactBooleanArray(
          buffer, offset, LEVEL1_BUCKET_SIZE_1DxF / 64);
  BooleanArray compact_array = BooleanArray(compact_arr_values);
  // Deserialize counts for buckets with count > 0
  auto counts = deserializeCounts(buffer, offset, compact_array.count());
  auto count_idx = 0;
  for (size_t i = 0; i < LEVEL1_BUCKET_SIZE_1DxF; i++) {
    if (compact_array.get(i)) {
      node->count[i] = counts[count_idx];
      count_idx++;
    }
  }

  return node;
}

trie_header deserializeTrieHeader(const std::vector<char> &buffer) {
  trie_header t_header;
  if (buffer.size() >= sizeof(trie_header)) {
    memcpy(&t_header, buffer.data(), sizeof(trie_header));

    //        // Print statements for each field
    //        std::cout << "Type Code: " << t_header.type_code << std::endl;
    //        std::cout << "Version: " << t_header.version << std::endl;
    //        std::cout << "M Width: " << t_header.m_width << std::endl;
    //        std::cout << "Precision Bits: " << t_header.precision_bits <<
    //        std::endl; std::cout << "Node Width: " << t_header.node_width <<
    //        std::endl; std::cout << "Type: " << t_header.type << std::endl;
    //        std::cout << "Mode: " << t_header.mode << std::endl;
    //        std::cout << "Positive Infinity Count: " << t_header.pos_inf_count
    //        << std::endl; std::cout << "Negative Infinity Count: " <<
    //        t_header.neg_inf_count << std::endl; std::cout << "Positive Zero
    //        Count: " << t_header.pos_zero_count << std::endl; std::cout <<
    //        "Negative Zero Count: " << t_header.neg_zero_count << std::endl;
    //        std::cout << "NaN Count: " << t_header.nan_count << std::endl;
    //        std::cout << "Positive-Positive Count: " << t_header.pos_pos <<
    //        std::endl; std::cout << "Positive-Negative Count: " <<
    //        t_header.pos_neg << std::endl; std::cout << "Negative-Positive
    //        Count: " << t_header.neg_pos << std::endl; std::cout <<
    //        "Negative-Negative Count: " << t_header.neg_neg << std::endl;
    //        std::cout << "Trie Root Reference: " << t_header.trie_root_ref <<
    //        std::endl;
  } else {
    std::cout << "Buffer is too small to contain a valid trie header."
              << std::endl;

    // Handle the error (e.g., set default values or throw an exception)
  }

  return t_header;
}

std::pair<std::unique_ptr<TrieNode_1DxF>, trie_header>
processBuffer1DxF(const std::vector<char> &buffer) {
  trie_header header;

  // Deserialize the header
  if (buffer.size() >= sizeof(trie_header)) {
    header = deserializeTrieHeader(buffer);
  } else {
    LOG_ERROR("Buffer is too small to contain a valid trie header.");
    // Handle the error or set default values for the header
  }

  // Start deserialization of TrieNode after the trie header
  size_t index = sizeof(trie_header);
  std::unique_ptr<TrieNode_1DxF> root = nullptr;

  if (index < buffer.size()) {
    // root = deserializeTrieNode_16(buffer, index);
    root = deserializeTrieNode_1DxF_compact(buffer, index);

    if (root.get() == nullptr) {
      LOG_ERROR("Deserialization resulted in a null root node.");
    } else {
      // std::cout << "Root node is valid." << std::endl;
    }
  } else {
    LOG_ERROR("Insufficient buffer size for trie data.");
  }

  return std::make_pair(std::move(root), header);
}
