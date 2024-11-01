/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#ifndef BOOLEAN_ARRAY_H
#define BOOLEAN_ARRAY_H

#include <bitset>
#include <iostream>
#include <memory>
#include <vector>

/**
 * Class for managing a dynamic array of boolean values compactly.
 */
class BooleanArray {
public:
  /**
   * Constructs a BooleanArray of specified size.
   * @param size Initial size of the BooleanArray.
   */
  explicit BooleanArray(int size);

  /**
   * Constructs a BooleanArray from an existing array of uint64_t.
   * @param array Initial array values.
   */
  explicit BooleanArray(std::vector<uint64_t> array);

  /**
   * Sets the boolean value at a specified index.
   * @param index Index at which to set the value.
   * @param value Boolean value to set.
   */
  void set(int index, bool value);

  /**
   * Gets the boolean value at a specified index.
   * @param index Index from which to get the boolean value.
   * @return Boolean value at the index.
   */
  [[nodiscard]] bool get(int index) const;

  /**
   * Retrieves the underlying uint64_t array representing the boolean array.
   * @return The vector of uint64_t.
   */
  [[nodiscard]] std::vector<uint64_t> get_array() const;

  /**
   * Counts the number of true values in the BooleanArray.
   * @return Number of true values.
   */
  [[nodiscard]] uint64_t count() const;

  /**
   * Prints the contents of the BooleanArray to standard output.
   */
  void print() const;

private:
  std::vector<uint64_t> array_;
  uint64_t count_;
};

#endif // BOOLEAN_ARRAY_H
