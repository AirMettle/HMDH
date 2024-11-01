/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "include/FloatingPointTrie/2-D.h"
#include "include/TrieOperation/2-D.h"

std::unique_ptr<TLETrieNode_2DxF> CreateParentNode_TLE2DxF() {
  auto parentNode = std::make_unique<TLETrieNode_2DxF>();
  parentNode->populated.reset();

  return parentNode;
}