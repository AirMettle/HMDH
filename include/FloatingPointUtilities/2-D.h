/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef FLOATINGPOINTUTILITIES_2D_H
#define FLOATINGPOINTUTILITIES_2D_H

#include "FloatingPointTrie/1-D.h"
#include "FloatingPointTrie/2-D.h"


unsigned int combine_chunks_2DxF(unsigned int x, unsigned int y);
void insertintoTLETrie_2DxF(TLETrieNode_2DxF *root, unsigned int combined,
                            unsigned int combinedTLE, int ndims,
                            uint64_t &curr_trie_size);


#endif // FLOATINGPOINTUTILITIES_2D_H