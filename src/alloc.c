// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file alloc.c
 */
#include <assert.h>
#include <stdlib.h>

#include "ds/alloc.h"

/* default memory allocation uses malloc of glic */
static alloc_api_fn_t alloc_delegator = malloc;

/* default memory deallocation uses free of glic */
static dealloc_api_fn_t dealloc_delegator = free;

void *
ds_alloc (size_t size)
{
  assert (alloc_delegator != NULL);

  if (alloc_delegator)
    {
      return alloc_delegator (size);
    }

  return NULL;
}

void
ds_dealloc (void *ptr)
{
  assert (dealloc_delegator != NULL);

  if (dealloc_delegator)
    {
      dealloc_delegator (ptr);
    }
}

void
ds_bind_alloc (alloc_api_fn_t alloc)
{
  alloc_delegator = alloc;
}

void
ds_bind_dealloc (dealloc_api_fn_t dealloc)
{
  dealloc_delegator = dealloc;
}