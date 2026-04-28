// SPDX-FileCopyrightText: 2026 Jim00000
//
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file sll_unittest.cpp
 */
#include <gtest/gtest.h>

/* Helper */
#include "sll_unittest_impl.hpp"

/* C code under test */
extern "C"
{
#include "ds/alloc.h"
#include "ds/lists/sll.h"
}

class Singly_Linked_List_Test : public ::testing::Test
{
protected:
  Singly_Linked_List_Test ()
  {
    this->sll = sll_create ();
    this->destroyer = nullptr;
  }

  ~Singly_Linked_List_Test () override { sll_free (this->sll, this->destroyer); }

  void
  SetUp () override
  {
  }

  void
  TearDown () override
  {
    sll_clear (this->sll, this->destroyer);
    this->destroyer = nullptr;
  }

  sll_t *sll;
  sll_destroy_fn_t destroyer;
};

TEST_F (Singly_Linked_List_Test, TestListEmptyAPI)
{
  ASSERT_TRUE (sll_is_empty (sll));
  sll_push_front (sll, (void *)3);
  ASSERT_FALSE (sll_is_empty (sll));
  sll_push_front (sll, (void *)2);
  ASSERT_FALSE (sll_is_empty (sll));
  sll_push_front (sll, (void *)1);
  ASSERT_FALSE (sll_is_empty (sll));
}

TEST_F (Singly_Linked_List_Test, TestListSizeAPI)
{
  ASSERT_EQ (sll_size (sll), 0);
  for (intptr_t i = 0; i < 64; i++)
    {
      sll_push_front (sll, (void *)i);
      ASSERT_EQ (sll_size (sll), i + 1);
    }
  sll_clear (sll, nullptr);
  ASSERT_EQ (sll_size (sll), 0);
}

TEST_F (Singly_Linked_List_Test, TestCreateAPI)
{
  sll_t *local_sll = sll_create ();
  ASSERT_NE (local_sll, nullptr);
  sll_free (local_sll, nullptr);

  // Simulate heap allocation failure
  ds_bind_alloc (TestCreateAPI_malloc_return_null);
  local_sll = sll_create ();
  ASSERT_EQ (local_sll, nullptr);

  // Set default malloc api for other
  ds_bind_alloc (malloc);
}

TEST_F (Singly_Linked_List_Test, TestClearAPI)
{
  sll_push_front (sll, (void *)3);
  sll_push_front (sll, (void *)2);
  sll_push_front (sll, (void *)1);
  ASSERT_EQ (sll_size (sll), 3);
  sll_clear (sll, TestClearAPI_destroy);
  ASSERT_EQ (sll_size (sll), 0);
  ASSERT_TRUE (sll_is_empty (sll));
  ASSERT_EQ (TestClearAPI_get_data_counter (), 3);
  ASSERT_EQ (TestClearAPI_get_data_sum (), 6);
}

TEST_F (Singly_Linked_List_Test, TestFreeAPI)
{
  sll_t *local_sll = sll_create ();
  ASSERT_NE (local_sll, nullptr);
  sll_push_front (sll, (void *)3);
  sll_push_front (sll, (void *)2);
  sll_push_front (sll, (void *)1);
  sll_free (local_sll, nullptr);
}

TEST_F (Singly_Linked_List_Test, TestFrontAPI)
{
  void *read_result;
  ASSERT_DEATH (sll_front (nullptr, nullptr), ".*");
  ASSERT_DEATH (sll_front (sll, nullptr), ".*");
  ASSERT_EQ (sll_front (sll, &read_result), false);
  ASSERT_EQ (sll_push_front (sll, (void *)5), DS_STS_OK);
  ASSERT_EQ (sll_front (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)5);
  ASSERT_EQ (sll_push_front (sll, (void *)4), DS_STS_OK);
  ASSERT_EQ (sll_front (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)4);
  ASSERT_EQ (sll_push_back (sll, (void *)6), DS_STS_OK);
  ASSERT_EQ (sll_front (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)4);
}

TEST_F (Singly_Linked_List_Test, TestBackAPI)
{
  void *read_result;
  ASSERT_DEATH (sll_back (nullptr, nullptr), ".*");
  ASSERT_DEATH (sll_back (sll, nullptr), ".*");
  ASSERT_EQ (sll_back (sll, &read_result), false);
  ASSERT_EQ (sll_push_front (sll, (void *)5), DS_STS_OK);
  ASSERT_EQ (sll_back (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)5);
  ASSERT_EQ (sll_push_front (sll, (void *)4), DS_STS_OK);
  ASSERT_EQ (sll_back (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)5);
  ASSERT_EQ (sll_push_back (sll, (void *)6), DS_STS_OK);
  ASSERT_EQ (sll_back (sll, &read_result), true);
  ASSERT_EQ (read_result, (void *)6);
}

TEST_F (Singly_Linked_List_Test, TestIterBegin)
{
  sll_iter_t it;
  ASSERT_DEATH (sll_begin (nullptr), ".*");
  sll_push_front (sll, (void *)3);
  it = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&it), (void *)3);
  sll_push_front (sll, (void *)2);
  it = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&it), (void *)2);
  sll_push_front (sll, (void *)1);
  it = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&it), (void *)1);
}

TEST_F (Singly_Linked_List_Test, TestIterEnd)
{
  sll_iter_t it = sll_begin (sll);
  ASSERT_EQ (sll_iter_is_end (&it), true);
}

TEST_F (Singly_Linked_List_Test, TestIterNext)
{
  sll_iter_t it;
  sll_push_front (sll, (void *)3);
  sll_push_front (sll, (void *)2);
  sll_push_front (sll, (void *)1);
  it = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&it), (void *)1);
  it = sll_next (it);
  ASSERT_EQ (sll_iter_data (&it), (void *)2);
  it = sll_next (it);
  ASSERT_EQ (sll_iter_data (&it), (void *)3);
  it = sll_next (it);
}

TEST_F (Singly_Linked_List_Test, TestInvalidateIterEnd)
{
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);

  sll_iter_t it = sll_begin (sll);

  ASSERT_TRUE (sll_iter_is_consistent (&it));

  /* This operation will invalidate any iterators created before this */
  sll_push_back (sll, (void *)1);

  ASSERT_FALSE (sll_iter_is_consistent (&it));
}

TEST_F (Singly_Linked_List_Test, TestPushFrontAPI)
{
  sll_push_front (sll, (void *)3);
  sll_push_front (sll, (void *)2);
  sll_push_front (sll, (void *)1);
  ASSERT_FALSE (sll_is_empty (sll));
  ASSERT_EQ (sll_size (sll), 3);
}

TEST_F (Singly_Linked_List_Test, TestPushBackAPI)
{
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  ASSERT_FALSE (sll_is_empty (sll));
  ASSERT_EQ (sll_size (sll), 3);
}

TEST_F (Singly_Linked_List_Test, TestInsertAfterAPI)
{
  sll_iter_t iter;
  ASSERT_EQ (sll_insert_after (nullptr, nullptr, NULL), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_insert_after (sll, nullptr, NULL), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_push_back (sll, (void *)1), DS_STS_OK);
  iter = sll_begin (sll);
  ASSERT_EQ (sll_insert_after (sll, &iter, (void *)3), DS_STS_OK);
  sll_match_fn_t cond = [] (void *data, void *) { return (intptr_t)data == (intptr_t)3; };
  iter = sll_find_if (sll, cond, nullptr);
  ASSERT_EQ (sll_insert_after (sll, &iter, (void *)5), DS_STS_OK);

  iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)3);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)5);

  ASSERT_EQ (sll_size (sll), 3);
}

TEST_F (Singly_Linked_List_Test, TestFindAPI)
{
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)5);

  sll_match_fn_t cond = [] (void *data, void *) { return (intptr_t)data == (intptr_t)5; };
  sll_iter_t it = sll_find_if (sll, cond, nullptr);
  ASSERT_EQ (sll_iter_data (&it), (void *)5);
  cond = [] (void *data, void *) { return (intptr_t)data == (intptr_t)8; };
  it = sll_find_if (sll, cond, nullptr);
  ASSERT_EQ (it.node, nullptr);

  /* invalid parameters */
  ASSERT_DEATH (sll_find_if (sll, nullptr, nullptr),
                ".*"); // expected to be catached in assert(...)
  ASSERT_DEATH (sll_find_if (nullptr, nullptr, nullptr),
                ".*"); // expected to be catached in assert(...)
}

TEST_F (Singly_Linked_List_Test, TestPopFrontAPI)
{
  ASSERT_EQ (sll_pop_front (nullptr, nullptr), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_pop_front (sll, nullptr), DS_STS_INVALID_PARAM);

  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)5);

  void *result;
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)1);
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)2);
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)3);
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)4);
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)5);
  ASSERT_EQ (sll_pop_front (sll, &result), DS_STS_EMPTY);
  ASSERT_EQ (sll_size (sll), 0);
}

TEST_F (Singly_Linked_List_Test, TestPopBackAPI)
{
  ASSERT_EQ (sll_pop_back (nullptr, nullptr), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_pop_back (sll, nullptr), DS_STS_INVALID_PARAM);

  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)5);

  void *result;
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)5);
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)4);
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)3);
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)2);
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)1);
  ASSERT_EQ (sll_pop_back (sll, &result), DS_STS_EMPTY);
  ASSERT_EQ (sll_size (sll), 0);
}

TEST_F (Singly_Linked_List_Test, TestRemoveAfterAPI)
{
  void *result = nullptr;
  sll_iter_t pos;
  ASSERT_EQ (sll_remove_after (nullptr, nullptr, nullptr), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_remove_after (sll, nullptr, nullptr), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_remove_after (sll, nullptr, &result), DS_STS_INVALID_PARAM);
  sll_push_back (sll, (void *)3);
  pos = sll_begin (sll);
  ASSERT_EQ (sll_remove_after (sll, &pos, nullptr), DS_STS_INVALID_PARAM);
  ASSERT_EQ (sll_remove_after (sll, &pos, &result), DS_STS_EMPTY);
  sll_push_back (sll, (void *)4);
  pos = sll_begin (sll);

  /* Remove tail */
  ASSERT_EQ (sll_remove_after (sll, &pos, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)4);
  ASSERT_EQ (sll_front (sll, &result), true);
  ASSERT_EQ (result, (void *)3);
  ASSERT_EQ (sll_back (sll, &result), true);
  ASSERT_EQ (result, (void *)3);
  ASSERT_EQ (sll_size (sll), 1);

  /* Clear list */
  sll_clear (sll, nullptr);

  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);

  pos = sll_begin (sll);
  pos = sll_next (pos);
  ASSERT_EQ (sll_remove_after (sll, &pos, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)3);

  pos = sll_begin (sll);
  pos = sll_next (pos);
  ASSERT_EQ (sll_remove_after (sll, &pos, &result), DS_STS_OK);
  ASSERT_EQ (result, (void *)4);

  pos = sll_begin (sll);
  pos = sll_next (pos);
  ASSERT_EQ (sll_remove_after (sll, &pos, &result), DS_STS_EMPTY);

  ASSERT_EQ (sll_size (sll), 2);
}

TEST_F (Singly_Linked_List_Test, TestRemoveIfAPI)
{
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)5);
  sll_push_back (sll, (void *)6);
  sll_push_back (sll, (void *)7);
  sll_push_back (sll, (void *)8);

  sll_match_fn_t even_cond = [] (void *data, void *) { return (intptr_t)data % 2 == 0; };
  ASSERT_EQ (sll_remove_if (sll, even_cond, nullptr, nullptr), 4);

  sll_iter_t iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)3);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)5);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)7);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 4);

  sll_match_fn_t remove_3_cond = [] (void *data, void *) { return (intptr_t)data == 3; };
  ASSERT_EQ (sll_remove_if (sll, remove_3_cond, nullptr, nullptr), 1);
  iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)5);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)7);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 3);

  sll_match_fn_t remove_1_cond = [] (void *data, void *) { return (intptr_t)data == 1; };
  ASSERT_EQ (sll_remove_if (sll, remove_1_cond, nullptr, nullptr), 1);
  iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)5);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)7);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 2);

  sll_match_fn_t remove_all_cond = [] (void *, void *) { return true; };
  ASSERT_EQ (sll_remove_if (sll, remove_all_cond, nullptr, nullptr), 2);
  iter = sll_begin (sll);
  ASSERT_EQ (sll_size (sll), 0);

  void *data = nullptr;
  ASSERT_EQ (sll_front (sll, &data), 0);
  ASSERT_EQ (sll_back (sll, &data), 0);
}

TEST_F (Singly_Linked_List_Test, TestReverseAPI)
{
  void *data = nullptr;

  sll_push_back (sll, (void *)1);
  sll_reverse (sll); // dummy operation
  sll_front (sll, &data);
  ASSERT_EQ (data, (void *)1);

  sll_push_back (sll, (void *)2);
  sll_reverse (sll);
  sll_front (sll, &data);
  ASSERT_EQ (data, (void *)2);
  sll_back (sll, &data);
  ASSERT_EQ (data, (void *)1);

  sll_push_front (sll, (void *)3);
  sll_reverse (sll);
  sll_iter_t iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)2);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)3);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 3);
}

extern "C" int
comp (const void *lhs, const void *rhs)
{
  return (intptr_t)lhs - (intptr_t)rhs;
}

TEST_F (Singly_Linked_List_Test, TestSortAPI)
{
  sll_iter_t iter;
  void *data = nullptr;

  sll_push_back (sll, (void *)5);
  sll_sort (sll, comp);
  sll_front (sll, &data);
  ASSERT_EQ (data, (void *)5);

  sll_clear (sll, nullptr);

  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)1);
  sll_sort (sll, comp);

  iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)2);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)3);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)4);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 4);

  sll_clear (sll, nullptr);

  sll_push_back (sll, (void *)9);
  sll_push_back (sll, (void *)5);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)7);
  sll_push_back (sll, (void *)8);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)10);
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)3);
  sll_sort (sll, comp);

  iter = sll_begin (sll);
  ASSERT_EQ (sll_iter_data (&iter), (void *)1);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)2);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)3);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)4);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)5);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)7);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)8);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)9);
  iter = sll_next (iter);
  ASSERT_EQ (sll_iter_data (&iter), (void *)10);
  iter = sll_next (iter);
  ASSERT_EQ (sll_size (sll), 9);
}

TEST_F (Singly_Linked_List_Test, TestSpliceAfterAPI)
{
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_push_back (sll, (void *)5);

  sll_t *src = sll_create ();
  sll_push_back (src, (void *)10);
  sll_push_back (src, (void *)11);
  sll_push_back (src, (void *)12);

  sll_iter_t it = sll_begin (sll);
  sll_iter_t first = sll_begin (src); // 10
  sll_iter_t end = sll_end (src); // null
  sll_splice_after (sll, &it, src, &first, &end);

  sll_free (src, nullptr);
}

TEST_F (Singly_Linked_List_Test, TestEmptyList)
{
  ASSERT_TRUE (sll_is_empty (sll));
  ASSERT_EQ (sll_size (sll), 0);
}

TEST_F (Singly_Linked_List_Test, TestNonEmptyList)
{
  sll_push_front (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  ASSERT_FALSE (sll_is_empty (sll));
  ASSERT_EQ (sll_size (sll), 2);
}

TEST_F (Singly_Linked_List_Test, TestFullTraversal)
{
  sll_push_front (sll, (void *)30);
  sll_push_front (sll, (void *)20);
  sll_push_front (sll, (void *)10);

  sll_iter_t it = sll_begin (sll);

  ASSERT_EQ (sll_iter_data (&it), (void *)10);

  it = sll_next (it);
  ASSERT_EQ (sll_iter_data (&it), (void *)20);

  it = sll_next (it);
  ASSERT_EQ (sll_iter_data (&it), (void *)30);

  it = sll_next (it);
}

TEST_F (Singly_Linked_List_Test, TestForLoopTraversal)
{
  intptr_t array[5] = { 1, 2, 3, 4, 5 };

  for (int i = 4; i >= 0; i--)
    {
      sll_push_front (sll, (void *)array[i]);
    }

  {
    size_t idx = 0;
    for (sll_iter_t iter = sll_begin (sll); !sll_iter_is_end (&iter); iter = sll_next (iter))
      {
        const intptr_t value = (intptr_t)sll_iter_data (&iter);
        ASSERT_EQ (value, array[idx++]);
      }
    ASSERT_EQ (idx, 5);
  }
}