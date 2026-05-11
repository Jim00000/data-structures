// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll.h
 * @brief Singly linked list API.
 *
 * This header file contains declarations for a singly linked list, including creation,
 * destruction, search, insertion, deletion, and traversal operations. The list supports
 * generic data types.
 *
 * @note This is an opaque pointer-based container. Ownership of stored data is user-managed.
 * @note Iterators are invalidated by structural modifications.
 * @note Iterator invalidation policy: Any structural modification to the list (insertion, removal,
 * clear, sort, etc.) invalidates all existing iterators. This container uses a versioning
 * mechanism to detect invalid iterators (fail-fast behavior).
 *
 * @warning This container is not thread-safe.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "ds/status.h"

struct SllNode;

/**
 * @brief Opaque singly linked list container type.
 */
typedef struct Sll sll_t;

/**
 * @brief Modification version counter for the list.
 *
 * This counter is incremented on every structural modification of the list, such as insertion,
 * deletion, or sorting.
 *
 * It is used to detect iterator invalidation and ensure safe traversal.
 */
typedef size_t iter_version_t;

/**
 * @brief Iterator type for singly linked list.
 *
 * This iterator represents a position within a singly linked list.
 *
 * The iterator becomes invalid if the list is modified after the iterator is created (e.g.,
 * insertion, deletion, or sorting operations).
 */
typedef struct SllIter
{
  struct SllNode *node;            ///< Pointer to current list node.
  const sll_t *src;                ///< Pointer to the original list.
  iter_version_t version_snapshot; ///< Snapshot of list version for invalidation detection.
  bool is_before_begin;
} sll_iter_t;

/**
 * @brief Function used to destroy an element's stored data.
 *
 * @param data Pointer to the element's stored data.
 *
 * @note This function is responsible for releasing any resources
 *       associated with the data pointer.
 * @note If NULL, no destruction is performed on the data.
 */
typedef void (*sll_destroy_fn_t) (void *data);

/**
 * @brief Visitor function applied to each element during traversal.
 *
 * @param data Pointer to the element's stored data.
 * @param context User-provided context passed through the traversal API.
 */
typedef void (*sll_visit_fn_t) (void *data, void *context);

/**
 * @brief Predicate function used to test whether an element matches a condition.
 *
 * @param data Pointer to the element's stored data.
 * @param context User-provided context passed through the API.
 *
 * @return true if the element matches the condition, false otherwise.
 */
typedef bool (*sll_match_fn_t) (void *data, void *context);

/**
 * @brief Comparison function used to order two elements.
 *
 * @param lhs Pointer to the first element's stored data.
 * @param rhs Pointer to the second element's stored data.
 *
 * @return A positive value if lhs is greater than rhs;
 *         a negative value if lhs is less than rhs;
 *         zero if lhs and rhs are equal.
 *
 * @note The function operates on opaque pointers to user-managed data.
 */
typedef int (*sll_compare_fn_t) (const void *lhs, const void *rhs);

/**
 * @brief Create an empty singly linked list.
 *
 * @return Pointer to an empty singly linked list, or NULL on allocation failure.
 *
 * @note The caller owns the returned object and must free it using sll_free().
 *
 * @par Complexity
 * O(1)
 */
sll_t *sll_create (void);

/**
 * @brief Remove all elements from the list and optionally destroy their associated data.
 *
 * @param sll Pointer to the singly linked list (must not be NULL).
 * @param destroyer Optional function used to destroy the data of each element.
 *
 * @note If destroyer is not NULL, it will be called on each element's data before the node is
 *       deallocated.
 * @note If destroyer is NULL, element data pointers are left untouched.
 * @note After this call, the list is empty.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n)
 */
void sll_clear (sll_t *sll, sll_destroy_fn_t destroyer);

/**
 * @brief Destroy the list and release its internal resources.
 *
 * @param sll Pointer to the singly linked list (must not be NULL).
 * @param destroyer Optional function used to destroy the data of each element.
 *
 * @note If destroyer is not NULL, it will be called on each element's data.
 * @note If destroyer is NULL, element data pointers are left untouched.
 *
 * @warning Passing NULL results in undefined behavior.
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n)
 */
void sll_free (sll_t *sll, sll_destroy_fn_t destroyer);

/**
 * @brief Retrieve the first element of the list.
 *
 * @param sll Pointer to the singly linked list.
 * @param result Output parameter used to store the data of the first element.
 * @return true if the list is non-empty and the element was retrieved successfully,
 *         false if the list is empty.
 *
 * @note The returned data is not copied; it is the pointer stored in the node.
 *
 * @warning Behavior is undefined if any pointer argument is NULL.
 *
 * @par Complexity
 * O(1)
 */
bool sll_front (const sll_t *sll, void **result);

/**
 * @brief Retrieve the last element of the list.
 *
 * @param sll Pointer to the singly linked list.
 * @param result Output parameter used to store the data of the last element.
 * @return true if the list is non-empty and the element was retrieved successfully,
 *         false if the list is empty.
 *
 * @note The returned data is not copied; it is the pointer stored in the node.
 *
 * @warning Behavior is undefined if any pointer argument is NULL.
 *
 * @par Complexity
 * O(1)
 */
bool sll_back (const sll_t *sll, void **result);

sll_iter_t sll_before_begin (const sll_t *sll);

/**
 * @brief Return an iterator to the first element of the list.
 *
 * @param sll Pointer to the singly linked list.
 * @return Iterator to the first element, or an end iterator if the list is empty.
 *
 * @note An iterator with node == NULL represents the end iterator. End iterators are not
 *       dereferenceable.
 * @note The iterator becomes invalid if the list is structurally modified. Use
 *       sll_iter_is_consistent() to detect invalidation (fail-fast behavior).
 * @note Store a snapshot of the list version for fail-fast detection.
 *
 * @par Complexity
 * O(1)
 */
sll_iter_t sll_begin (const sll_t *sll);

sll_iter_t sll_end (const sll_t *sll);

/**
 * @brief Check whether the iterator is valid with respect to its source list.
 *
 * The iterator is considered consistent if its version snapshot matches the current version of the
 * source list.
 *
 * @param iterator Iterator to be checked (must not be NULL).
 * @return true if the iterator is still consistent with its source list.
 *
 * @note This function is used for fail-fast iterator validation.
 *
 * @par Complexity
 * O(1)
 */
bool sll_iter_is_consistent (const sll_iter_t *iterator);

/**
 * @brief Move the iterator to the next element.
 *
 * @param iterator Iterator to the list node.
 * @return Iterator advanced to the next element. If the iterator is at the end, an end iterator is
 *         returned.
 *
 * @note The iterator becomes invalid if the list has been structurally modified.
 *
 * @warning The iterator must be valid. Behavior is undefined otherwise.
 *
 * @par Complexity
 * O(1)
 */
sll_iter_t sll_next (sll_iter_t iterator);

/**
 * @brief Get the data from the current iterator.
 *
 * @param iterator Iterator to the list node (must not be NULL and must be valid).
 * @return Pointer to user-provided data stored in the element.
 *
 * @note This function assumes the iterator is valid and belongs to the list. Behavior is undefined
 *       if the iterator is invalid or does not belong to the container.
 *
 * @warning Behavior is undefined if the iterator is invalid. Use sll_iter_is_consistent() to check
 *          validity.
 *
 * @par Complexity
 * O(1)
 */
void *sll_iter_data (const sll_iter_t *iterator);

/**
 * @brief Check whether the iterator reaches the list end.
 *
 * @param iterator Iterator to the list node.
 * @return true if the iterator is at the end; false otherwise.
 *
 * @note This function only checks structural end (node == NULL) and does not validate iterator
 * consistency.
 *
 * @par Complexity
 * O(1)
 */
bool sll_iter_is_end (const sll_iter_t *iterator);

/**
 * @brief Find the first element that matches a predicate.
 *
 * @param sll Pointer to the singly linked list.
 * @param condition Predicate function used to test each element.
 * @param context User-provided context passed to the predicate.
 *
 * @return Iterator to the first matching element, or an end iterator if no match is found.
 *
 * @par Complexity
 * O(n)
 */
sll_iter_t sll_find_if (const sll_t *sll, sll_match_fn_t condition, void *context);

/* Insertion */
/**
 * @brief Insert a new element at the front of the list.
 *
 * @param sll Pointer to the singly linked list.
 * @param data Pointer to user-managed data stored in the element. The list stores the pointer
 *             without copying it.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_ALLOC_FAIL Memory allocation failed.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(1)
 */
ds_sts_t sll_push_front (sll_t *sll, void *data);

/**
 * @brief Insert a new element at the back of the list.
 *
 * @param sll Pointer to the singly linked list.
 * @param data Pointer to user-managed data stored in the element. The list stores the pointer
 *             without copying it.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_ALLOC_FAIL Memory allocation failed.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(1)
 */
ds_sts_t sll_push_back (sll_t *sll, void *data);

/**
 * @brief Insert a new element after the specified iterator position.
 *
 * @param sll Pointer to the singly linked list.
 * @param pos Iterator that must belong to the list (otherwise behavior is undefined).
 * @param data Pointer to user-managed data stored in the element.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_ALLOC_FAIL Memory allocation failed.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(1). May perform O(n) validation in debug builds.
 */
ds_sts_t sll_insert_after (sll_t *sll, const sll_iter_t *pos, void *data);

/**
 * @brief Remove the first element from the list and return its data.
 *
 * @param sll Pointer to the singly linked list.
 * @param result Output parameter used to store the data of the removed element. Valid only if the
 *               function returns DS_STS_OK.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_EMPTY The list is empty.
 *
 * @note The returned data pointer is not freed by this function. Ownership remains with the
 *       caller.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(1)
 */
ds_sts_t sll_pop_front (sll_t *sll, void **result);

/**
 * @brief Remove the last element from the list and return its data.
 *
 * @param sll Pointer to the singly linked list.
 * @param result Output parameter used to store the data of the removed element. Only valid if the
 *               function returns DS_STS_OK.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_EMPTY The list is empty.
 *
 * @note The returned data pointer is not freed by this function. Ownership of the data is
 *       transferred to the caller.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n), due to traversal required to locate the element preceding the tail.
 */
ds_sts_t sll_pop_back (sll_t *sll, void **result);

/**
 * @brief Remove the element after the given position.
 *
 * @param sll Pointer to the singly linked list.
 * @param pos Iterator that must belong to the list and must not be the tail element.
 * @param result Output parameter used to store the data of the removed element.
 *               The caller takes ownership of the returned data.
 *
 * @return Status of the operation.
 * @retval DS_STS_OK Success.
 * @retval DS_STS_INVALID_PARAM Invalid input pointer.
 * @retval DS_STS_EMPTY No element exists after the given position.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(1)
 */
ds_sts_t sll_remove_after (sll_t *sll, const sll_iter_t *pos, void **result);

/**
 * @brief Remove all elements that satisfy the given condition.
 *
 * @param sll Pointer to the singly linked list.
 * @param condition Predicate function used to match elements.
 * @param context User-provided context passed to the predicate.
 * @param destroyer Optional function to destroy the data of removed elements.
 *
 * @return Number of elements removed.
 *
 * @note If destroyer is not NULL, it will be called on each removed element's data.
 *       Otherwise, the data pointers are left untouched.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n)
 */
size_t sll_remove_if (sll_t *sll, sll_match_fn_t condition, void *context,
                      sll_destroy_fn_t destroyer);

/* Operations */

/**
 * @brief Reverse the order of the elements in the list.
 *
 * @param sll Pointer to the singly linked list (must not be NULL).
 *
 * @note The operation is performed in-place without allocating additional nodes.
 * @note The head and tail of the list are swapped.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n)
 */
void sll_reverse (sll_t *sll);

/**
 * @brief Sort the elements in the list using the merge sort algorithm.
 *
 * @param sll  Pointer to the singly linked list (must not be NULL).
 * @param comp Comparison function used to order elements. It must return a negative value if the
 *             first argument is less than the second, zero if they are equal, and a positive value
 *             otherwise.
 *
 * @note The sort is performed in-place without allocating additional nodes.
 * @note The sort is stable.
 *
 * @warning All iterators are invalidated.
 *
 * @par Complexity
 * O(n log n)
 */
void sll_sort (sll_t *sll, sll_compare_fn_t cmp);

ds_sts_t sll_splice_after (sll_t *dst, sll_iter_t pos, sll_t *src, sll_iter_t first,
                           sll_iter_t last);

/**
 * @brief Get the number of elements in the list.
 *
 * @param sll Pointer to the singly linked list (must not be NULL).
 *
 * @return Number of elements in the list.
 *
 * @par Complexity
 * O(1)
 */
size_t sll_size (const sll_t *sll);

/**
 * @brief Check whether the list is empty.
 *
 * @param sll Pointer to the singly linked list (must not be NULL).
 *
 * @return true if the list is empty, false otherwise.
 *
 * @par Complexity
 * O(1)
 */
bool sll_is_empty (const sll_t *sll);