/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/
#include "include/TrieOperation/1-D.h"
#include "include/FloatingPointTrie/1-D.h"

std::unique_ptr<TrieNode_1DxF> CreateParentNode_1DxF() {
  auto parentNode = std::make_unique<TrieNode_1DxF>();
  parentNode->populated.reset();

  return parentNode;
}
