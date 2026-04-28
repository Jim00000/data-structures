// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file alloc.h
 */
#pragma once

#include <stddef.h>

typedef void *(*alloc_api_fn_t) (size_t size);
typedef void (*dealloc_api_fn_t) (void *ptr);

void *ds_alloc (size_t size);

void ds_dealloc (void *ptr);

void ds_bind_alloc (alloc_api_fn_t alloc);

void ds_bind_dealloc (dealloc_api_fn_t dealloc);