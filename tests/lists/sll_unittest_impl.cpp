// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll_unittest_impl.cpp
 */
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>
#include <tuple>

#include "sll_unittest_impl.hpp"

namespace
{
size_t TestClearAPI_data_counter = 0;
intptr_t TestClearAPI_data_sum = 0;

size_t TestForeachAPI_data_index = 0;
std::array<intptr_t, 5> TestForeachAPI_expected_data = { 1, 2, 3, 4, 5 };
}

void
TestClearAPI_destroy (void *data)
{
  TestClearAPI_data_sum += reinterpret_cast<intptr_t> (data);
  TestClearAPI_data_counter++;
}

size_t
TestClearAPI_get_data_counter ()
{
  return TestClearAPI_data_counter;
}

size_t
TestClearAPI_get_data_sum ()
{
  return TestClearAPI_data_sum;
}

void *
TestCreateAPI_malloc_return_null (size_t)
{
  return NULL;
}

void
TestForeachAPI_visitor (void *data, void *context)
{
  (void)context;
  const intptr_t int_data = reinterpret_cast<intptr_t> (data);
  TestForeachAPI_expected_data.at (TestForeachAPI_data_index) -= int_data;
  TestForeachAPI_data_index++;
}

std::tuple<bool, size_t>
TestForeachAPI_get_statistic ()
{
  bool allzero = std::all_of (TestForeachAPI_expected_data.begin (),
                              TestForeachAPI_expected_data.end (), [] (auto i) { return i == 0; });
  return std::make_tuple (allzero,                  // whether the data is correct. true = correct.
                          TestForeachAPI_data_index // visited count
  );
}