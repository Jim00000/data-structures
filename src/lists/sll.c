// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll.c
 * @brief Singly linked list implementation.
 * This module implements a generic singly linked list container with
 * explicit ownership semantics and fail-fast iterator support.
 *
 * The design prioritizes:
 * - O(1) insertion/removal at head and tail (when applicable)
 * - O(n) traversal operations
 * - In-place structural modifications without additional allocations
 *
 * Iterator invalidation policy:
 * Any structural modification to the list (insertion, removal, reverse,
 * sort, clear) invalidates all existing iterators. A versioning mechanism
 * is used to detect invalid iterators in debug and iterator-aware APIs.
 *
 * Ownership model:
 * The list stores opaque pointers to user-managed data. It never copies
 * or frees user data unless explicitly instructed via destroy callbacks.
 *
 * Thread safety:
 * This container is not thread-safe and requires external synchronization
 * for concurrent access.
 */

#include <assert.h>
#include <stddef.h>

#include "ds/alloc.h"
#include "ds/lists/sll.h"
#include "ds/status.h"

// internal implementation details
#include "sll_impl.h"

/**
 * @brief Allocate and initialize a new list node.
 *
 * @param data Pointer to user-managed data stored in the node. The pointer is stored as-is.
 * @return Pointer to the newly created node, or NULL if allocation fails.
 *
 * @internal
 * @note The node does not take ownership of the data pointer.
 *
 * @par Complexity
 * O(1)
 */
static sll_node_t *
sll_node_alloc (void *data)
{
  sll_node_t *new_node = ds_alloc (sizeof (sll_node_t));

  if (!new_node)
    {
      return NULL;
    }

  new_node->data = data;
  new_node->next = NULL;

  return new_node;
}

/**
 * @brief Destroy a list node and optionally destroy its associated data.
 *
 * @param node Pointer to the list node (must not be NULL).
 * @param destroyer Optional function used to destroy the data of the element.
 *
 * @internal
 * @note If destroyer is not NULL, it will be called on node->data before deallocation.
 * @note If destroyer is NULL, node->data is left untouched.
 *
 * @par Complexity
 * O(1)
 */
static void
sll_destroy_node (sll_node_t *node, sll_destroy_fn_t destroyer)
{
  assert (node);

  if (destroyer)
    {
      destroyer (node->data);
    }

  ds_dealloc (node);
}

#ifndef NDEBUG
/**
 * @brief Check whether the given iterator belongs to the list.
 *
 * @param sll Pointer to the singly linked list.
 * @param iterator Iterator to be checked for membership in the list.
 * @return true if the iterator belongs to the list, false otherwise.
 *
 * @internal
 * @note Passing NULL as iterator returns false.
 *
 * @par Complexity
 * O(n)
 */
static bool
sll_contains (const sll_t *sll, const sll_iter_t *iterator)
{
  assert (sll && iterator);

  if (iterator->node == NULL)
    {
      return sll == iterator->src;
    }

  for (sll_iter_t curr_iter = sll_begin (sll); !sll_iter_is_end (&curr_iter);
       curr_iter = sll_next (curr_iter))
    {
      if (curr_iter.node == iterator->node)
        {
          return true;
        }
    }

  return false;
}
#endif

/**
 * @brief Asserts the internal invariants of the list.
 *
 * @param sll Pointer to the singly linked list.
 *
 * @internal
 * @note This function is intended for debug builds only and may be compiled out in release builds.
 *
 * @par Complexity
 * O(n) in debug builds.
 */
static void
sll_assert_invariant ([[maybe_unused]] const sll_t *sll)
{
  assert (sll);

  /* Empty list invariant */
  assert (sll->size == 0 ? (sll->head == NULL && sll->tail == NULL) : true);

  /* Tail must be terminal */
  assert (sll->tail == NULL || sll->tail->next == NULL);

#ifndef NDEBUG
  /* Size correctness */
  size_t count = 0;
  for (sll_node_t *node = sll->head; node != NULL; node = node->next)
    {
      count++;
    }
  assert (count == sll->size);
#endif
}

/**
 * @brief Split the list into two parts after the first n nodes.
 *
 * @param head Pointer to the head node of the list.
 * @param n Number of nodes to keep in the first list. The remaining nodes form a second list.
 *
 * @internal
 *
 * @par Complexity
 * O(n)
 */
static sll_node_t *
split (sll_node_t *head, size_t n)
{
  if (!head)
    {
      return NULL;
    }

  while ((--n) && head->next)
    {
      head = head->next;
    }

  sll_node_t *other = head->next;
  head->next = NULL;

  return other;
}

/**
 * @brief Merge two sorted lists into a single sorted list.
 *
 * @param left First sorted list to be merged.
 * @param right Second sorted list to be merged.
 * @param out_tail Pointer to store the tail node of the merged list.
 * @param cmp Comparison function used to determine element ordering. It must return a negative
 *            value if lhs < rhs, zero if equal, and a positive value otherwise.
 *
 * @internal
 * @note This function reuses existing nodes and does not allocate or free memory.
 *
 * @par Complexity
 * O(n)
 */
static sll_node_t *
merge (sll_node_t *left, sll_node_t *right, sll_node_t **out_tail, sll_compare_fn_t cmp)
{
  assert (left || right);
  assert (out_tail);
  assert (cmp);

  sll_node_t dummy_head = { 0 };
  sll_node_t *curr = &dummy_head;

  while (left && right)
    {
      if (cmp (left->data, right->data) <= 0)
        {
          curr->next = left;
          left = left->next;
        }
      else
        {
          curr->next = right;
          right = right->next;
        }
      curr = curr->next;
    }

  /* link the left parts */
  curr->next = left ? left : right;

  /* move to the tail */
  while (curr->next)
    {
      curr = curr->next;
    }

  /* record the tail */
  *out_tail = curr;

  /* return the head of the merged list */
  return dummy_head.next;
}

sll_t *
sll_create (void)
{
  sll_t *sll = ds_alloc (sizeof (sll_t));

  if (!sll)
    {
      return NULL;
    }

  sll->head = NULL;
  sll->tail = NULL;
  sll->size = 0;
  sll->version = 0;

  return sll;
}

void
sll_clear (sll_t *sll, sll_destroy_fn_t destroyer)
{
  assert (sll);

  sll_node_t *curr = sll->head;

  while (curr)
    {
      sll_node_t *next = curr->next;
      sll_destroy_node (curr, destroyer);
      curr = next;
    }

  /* Reset list state */
  sll->head = NULL;
  sll->tail = NULL;
  sll->size = 0;
  sll->version++;

  sll_assert_invariant (sll);
}

void
sll_free (sll_t *sll, sll_destroy_fn_t destroyer)
{
  assert (sll);

  /* Clear all elements in the list */
  sll_clear (sll, destroyer);

  /* Deallocate the list structure */
  ds_dealloc (sll);
}

bool
sll_front (const sll_t *sll, void **result)
{
  assert (sll != NULL && "sll expected to be non-null");
  assert (result != NULL && "result expected to be non-null");

  if (!sll->head)
    {
      return false;
    }

  *result = sll->head->data;

  return true;
}

bool
sll_back (const sll_t *sll, void **result)
{
  assert (sll != NULL && "sll expected to be non-null");
  assert (result != NULL && "data expected to be non-null");

  if (!sll->tail)
    {
      return false;
    }

  *result = sll->tail->data;

  return true;
}

sll_iter_t
sll_begin (const sll_t *sll)
{
  assert (sll);
  return (sll_iter_t){ .node = sll->head, .src = sll, .version_snapshot = sll->version };
}

sll_iter_t
sll_end (const sll_t *sll)
{
  assert (sll);
  return (sll_iter_t){ .node = NULL, .src = sll, .version_snapshot = sll->version };
}

bool
sll_iter_is_consistent (const sll_iter_t *iterator)
{
  assert (iterator && iterator->src);
  return (bool)(iterator->src->version == iterator->version_snapshot);
}

sll_iter_t
sll_next (sll_iter_t iterator)
{
  if (iterator.node != NULL)
    {
      iterator.node = iterator.node->next;
    }

  return iterator;
}

void *
sll_iter_data (const sll_iter_t *iterator)
{
  assert (iterator);
  assert (iterator->node);
  assert (sll_iter_is_consistent (iterator));

  return iterator->node->data;
}

bool
sll_iter_is_end (const sll_iter_t *iterator)
{
  assert (iterator);

  return (bool)(iterator->node == NULL);
}

sll_iter_t
sll_find_if (const sll_t *sll, sll_match_fn_t condition, void *context)
{
  assert (sll);
  assert (condition);

  sll_iter_t iter = sll_begin (sll);
  while (!sll_iter_is_end (&iter))
    {
      if (condition (sll_iter_data (&iter), context))
        {
          return iter;
        }
      iter = sll_next (iter);
    }

  return (sll_iter_t){ .node = NULL, .src = sll, .version_snapshot = sll->version };
}

ds_sts_t
sll_push_front (sll_t *sll, void *data)
{
  if (!sll)
    {
      return DS_STS_INVALID_PARAM;
    }

  sll_node_t *new_node = sll_node_alloc (data);

  // Check memory allocation
  if (!new_node)
    {
      return DS_STS_ALLOC_FAIL;
    }

  new_node->next = sll->head;

  /* Check tail */
  if (sll_is_empty (sll))
    {
      assert (sll->head == NULL);
      assert (sll->size == 0);
      sll->tail = new_node;
    }

  sll->head = new_node; // Assign the new node to the head node of this singly linked list
  sll->size++;          // Increase the counter size
  sll->version++;       // Bump version

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

ds_sts_t
sll_push_back (sll_t *sll, void *data)
{
  if (!sll)
    {
      return DS_STS_INVALID_PARAM;
    }

  sll_node_t *new_node = sll_node_alloc (data);

  // Check memory allocation
  if (!new_node)
    {
      return DS_STS_ALLOC_FAIL;
    }

  /* if tail element is empty */
  if (!sll->tail)
    {
      assert (sll->head == NULL);
      assert (sll->size == 0);
      sll->tail = new_node;
      sll->head = new_node;
    }
  else
    {
      assert (sll->head != NULL);
      assert (sll->size != 0);
      sll->tail->next = new_node;
      sll->tail = new_node;
    }

  sll->size++;    // Increase the counter size
  sll->version++; // Bump version

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

ds_sts_t
sll_insert_after (sll_t *sll, const sll_iter_t *pos, void *data)
{
  if (!sll || !pos || !pos->node)
    {
      return DS_STS_INVALID_PARAM;
    }

  /* Ensure the iterator belongs to this list */
  assert (sll_contains (sll, pos) && "Iterator does not belong to the list");

  // Make a new sll node
  sll_node_t *new_node = sll_node_alloc (data);

  // Check memory allocation
  if (!new_node)
    {
      return DS_STS_ALLOC_FAIL;
    }

  /* Insert the new node */
  new_node->next = pos->node->next;
  pos->node->next = new_node;
  sll->size++;
  sll->version++;

  /* update tail if needed */
  if (pos->node == sll->tail)
    {
      sll->tail = new_node;
    }

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

ds_sts_t
sll_pop_front (sll_t *sll, void **result)
{
  if (!sll || !result)
    {
      return DS_STS_INVALID_PARAM;
    }

  /* Return if empty */
  if (!sll_front (sll, result))
    {
      return DS_STS_EMPTY;
    }

  /* Remove the head element from the list */
  sll_node_t *node = sll->head;
  sll->head = sll->head->next;
  sll->size--;
  sll->version++;

  /* Check if the list becomes empty. */
  if (sll->head == NULL)
    {
      sll->tail = NULL;
    }

  /* Deallocate the removed node */
  ds_dealloc (node);

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

ds_sts_t
sll_pop_back (sll_t *sll, void **result)
{
  if (!sll || !result)
    {
      return DS_STS_INVALID_PARAM;
    }

  /* Return if empty */
  if (!sll_back (sll, result))
    {
      return DS_STS_EMPTY;
    }

  sll_node_t *prev = NULL;
  sll_node_t *curr = sll->head;

  while (curr != sll->tail)
    {
      prev = curr;
      curr = curr->next;
    }

  if (prev == NULL)
    {
      /* Single element */
      sll->head = NULL;
      sll->tail = NULL;
    }
  else
    {
      prev->next = NULL;
      sll->tail = prev;
    }

  sll->size--;
  sll->version++;

  /* Deallocate the removed node */
  ds_dealloc (curr);

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

ds_sts_t
sll_remove_after (sll_t *sll, const sll_iter_t *pos, void **result)
{
  if (!sll || !pos || !pos->node || !result)
    {
      return DS_STS_INVALID_PARAM;
    }

  /* Ensure the iterator belongs to this list */
  assert (sll_contains (sll, pos) && "Iterator does not belong to the list");

  /* No element after tail */
  if (pos->node == sll->tail)
    {
      return DS_STS_EMPTY;
    }

  /* Get the removed node */
  sll_node_t *node = pos->node->next;
  assert (node != NULL);

  /* Relink */
  pos->node->next = node->next;

  /* Update tail if needed */
  if (node == sll->tail)
    {
      sll->tail = pos->node;
    }

  /* Return data */
  *result = node->data;

  /* Update size */
  sll->size--;

  /* Bump version */
  sll->version++;

  /* Free node */
  ds_dealloc (node);

  sll_assert_invariant (sll);

  return DS_STS_OK;
}

size_t
sll_remove_if (sll_t *sll, sll_match_fn_t condition, void *context, sll_destroy_fn_t destroyer)
{
  assert (sll != NULL);
  assert (condition != NULL);

  size_t removed = 0;
  sll_node_t *curr = sll->head;
  sll_node_t *prev = NULL;

  while (curr)
    {
      if (condition (curr->data, context))
        {
          sll_node_t *next = curr->next;

          if (prev == NULL)
            {
              /* remove head */
              sll->head = next;
            }
          else
            {
              prev->next = next;
            }

          /* check tail */
          if (curr == sll->tail)
            {
              sll->tail = prev;
            }

          if (destroyer)
            {
              destroyer (curr->data);
            }

          ds_dealloc (curr);
          sll->size--;
          removed++;

          curr = next;
        }
      else
        {
          prev = curr;
          curr = curr->next;
        }
    }

  sll->version++;

  sll_assert_invariant (sll);

  return removed;
}

void
sll_reverse (sll_t *sll)
{
  assert (sll != NULL);

  if (!sll->head || !sll->head->next)
    {
      return;
    }

  sll_node_t *curr = sll->head;
  sll_node_t *prev = NULL;

  while (curr)
    {
      sll_node_t *next = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next;
    }

  /* prev should be the original tail */
  assert (prev == sll->tail);

  /* Swap head & tail */
  sll->tail = sll->head;
  sll->head = prev;

  sll->version++;

  sll_assert_invariant (sll);
}

void
sll_sort (sll_t *sll, sll_compare_fn_t cmp)
{
  assert (sll != NULL);
  assert (cmp != NULL);

  if (!sll->head || !sll->head->next)
    {
      return;
    }

  size_t step = 1;

  while (step < sll->size)
    {
      sll_node_t *curr = sll->head;
      sll_node_t *new_head = NULL;
      sll_node_t *new_tail = NULL;

      while (curr)
        {
          sll_node_t *left = curr;
          sll_node_t *right = split (left, step);
          curr = split (right, step);
          sll_node_t *merged_tail = NULL;
          sll_node_t *merged_head = merge (left, right, &merged_tail, cmp);

          if (!new_head)
            {
              new_head = merged_head;
              new_tail = merged_tail;
            }
          else
            {
              assert (new_tail);
              assert (new_tail->next == NULL);
              new_tail->next = merged_head;
              new_tail = merged_tail;
            }
        }

      sll->head = new_head;
      sll->tail = new_tail;
      step *= 2;
    }

  sll->version++;

  sll_assert_invariant (sll);
}

typedef struct
{
  sll_node_t *head;
  sll_node_t *tail;
  size_t size;
} sll_range_t;

static sll_range_t
detach_range (sll_t *src, sll_iter_t *first, sll_iter_t *last)
{
  assert (src);
  assert (first);
  assert (sll_contains (src, first));
  assert (sll_contains (src, last));

  /* empty range */
  if (first->node->next == last->node)
    {
      return (sll_range_t){ 0 };
    }

  sll_range_t range = { .head = first->node->next, .size = 0 };

  if (last->node == NULL)
    {
      range.tail = src->tail;
      first->node->next = NULL;
      src->tail = first->node;
    }
  else
    {
      range.tail = last->node;
      first->node->next = last->node->next;
    }

  assert (range.tail);
  range.tail->next = NULL;

  /* compute size */
  for (sll_node_t *curr = range.head; curr && (curr != last->node); curr = curr->next)
    {
      range.size++;
    }

  assert (range.size > 0);
  src->size -= range.size;
  src->version++;

  return range;
}

static void
attach_after (sll_t *dst, sll_iter_t *pos, sll_range_t *range)
{
  assert (dst);
  assert (pos);
  assert (range);
  assert (pos->src == dst);
  assert (sll_contains (dst, pos));

  /* empty range */
  if (range->size == 0)
    {
      return;
    }

  assert (range->head);
  assert (range->tail);
  assert (range->tail->next == NULL);

  if (pos->node)
    {
      range->tail->next = pos->node->next;
      pos->node->next = range->head;

      if (pos->node == dst->tail)
        {
          dst->tail = range->tail;
        }
    }
  else
    {
      /* append to tail */

      if (dst->tail)
        {
          dst->tail->next = range->head;
          dst->tail = range->tail;
        }
      else
        {
          /* empty dst */
          dst->head = range->head;
          dst->tail = range->tail;
        }
    }

  dst->size += range->size;
  dst->version++;
}

ds_sts_t
sll_splice_after (sll_t *dst, sll_iter_t *pos, sll_t *src, sll_iter_t *first, sll_iter_t *last)
{
  assert (dst);
  assert (pos);
  assert (src);
  assert (first);
  assert (last);

  if (!dst || !pos || !src || !first || !last || (first->node == last->node))
    {
      return DS_STS_INVALID_PARAM;
    }

  assert (pos->src == dst);
  assert (first->src == src);
  assert (last->src == src);
  assert (sll_iter_is_consistent (pos));
  assert (sll_iter_is_consistent (first));
  assert (sll_iter_is_consistent (last));
  assert (sll_contains (src, first));

  sll_range_t range = detach_range (src, first, last);
  attach_after (dst, pos, &range);

  return DS_STS_OK;
}

size_t
sll_size (const sll_t *sll)
{
  assert ((sll != NULL) && "sll expected to be non-null");
  return sll->size;
}

bool
sll_is_empty (const sll_t *sll)
{
  return (bool)(sll_size (sll) == 0);
}