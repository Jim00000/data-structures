// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file main_test.cpp
 */
#include <gtest/gtest.h>

int
main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}