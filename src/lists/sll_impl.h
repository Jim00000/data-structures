// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll_impl.h
 * @brief Private implementation details for the singly linked list.
 */
#include "ds/lists/sll.h"

typedef struct SllNode sll_node_t;

struct SllNode
{
  void *data;
  sll_node_t *next;
};

struct Sll
{
  sll_node_t *head;
  sll_node_t *tail;
  size_t size;
  iter_version_t version;
};