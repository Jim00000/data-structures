// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll_unittest_impl.hpp
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>

extern "C"
{
  void TestClearAPI_destroy (void *data);

  void *TestCreateAPI_malloc_return_null (size_t size);

  void TestForeachAPI_visitor (void *data, void *context);
}

size_t TestClearAPI_get_data_counter ();
size_t TestClearAPI_get_data_sum ();

std::tuple<bool, size_t> TestForeachAPI_get_statistic ();