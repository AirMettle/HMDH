/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include <gtest/gtest.h>
#include "include/FloatingPointTrie/1-D.h"
#include "include/TrieSerialization/1-D.h"

TEST(HeaderDeserialization, Header2D) {

  std::vector<char> buffer;
  bool default_mode = true;
  size_t offset = 0;

  buffer.reserve(sizeof(trie_header));
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

  trie_header t_header_deserialized = deserializeTrieHeader(buffer, offset);
  EXPECT_EQ(strcmp(t_header.type_code, t_header_deserialized.type_code), 0);
  EXPECT_EQ(t_header.version, t_header_deserialized.version);
  EXPECT_EQ(t_header.m_width, t_header_deserialized.m_width);
  EXPECT_EQ(t_header.precision_bits, t_header_deserialized.precision_bits);
  EXPECT_EQ(t_header.node_width, t_header_deserialized.node_width);
  EXPECT_EQ(strcmp(t_header.type, t_header_deserialized.type), 0);
  EXPECT_EQ(strcmp(t_header.config, t_header_deserialized.config), 0);
  EXPECT_EQ(t_header.mode, t_header_deserialized.mode);
  EXPECT_EQ(t_header.trie_root_ref, t_header_deserialized.trie_root_ref);
}

TEST(HeaderDeserialization, Header1D) {

  std::vector<char> buffer;
  bool default_mode = true;
  size_t offset = 0;

  buffer.reserve(sizeof(trie_header));
  trie_header t_header;

  strcpy(t_header.type_code, "HMDH");
  t_header.version = 0;
  t_header.m_width = 5;
  t_header.precision_bits = 13;
  t_header.node_width = 8;
  strcpy(t_header.type, "1-D");
  strcpy(t_header.config, "116");
  t_header.mode = default_mode ? 1 : 0;
  t_header.trie_root_ref = sizeof(trie_header);
  serializeTrieHeader(t_header, buffer);

  trie_header t_header_deserialized = deserializeTrieHeader(buffer, offset);

  std::cout << "Offset: " << offset << std::endl;

  EXPECT_EQ(strcmp(t_header.type_code, t_header_deserialized.type_code), 0);
  EXPECT_EQ(t_header.version, t_header_deserialized.version);
  EXPECT_EQ(t_header.m_width, t_header_deserialized.m_width);
  EXPECT_EQ(t_header.precision_bits, t_header_deserialized.precision_bits);
  EXPECT_EQ(t_header.node_width, t_header_deserialized.node_width);
  EXPECT_EQ(strcmp(t_header.type, t_header_deserialized.type), 0);
  EXPECT_EQ(strcmp(t_header.config, t_header_deserialized.config), 0);
  EXPECT_EQ(t_header.mode, t_header_deserialized.mode);
  EXPECT_EQ(t_header.trie_root_ref, t_header_deserialized.trie_root_ref);
}