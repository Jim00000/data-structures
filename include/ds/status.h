// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file status.h
 * @brief Definition of return status codes for data structure operations.
 */
#pragma once

/**
 * @enum DS_STATUS
 * @brief Enumeration of status codes.
 *
 * Used to indicate the success or specific failure reason of an operation.
 */
typedef enum DS_STATUS
{
  DS_STS_OK = 0,        /**< Operation completed successfully */
  DS_STS_ALLOC_FAIL,    /**< Memory allocation failed (e.g., malloc returned NULL) */
  DS_STS_INVALID_PARAM, /**< The provided parameter is invalid */
  DS_STS_EMPTY,         /**< The operation cannot be performed because the container is empty */
  DS_STS_UNKNOWN_ERROR, /**< Unidentified error */
} ds_sts_t;