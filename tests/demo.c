// NOLINTBEGIN

#include <stdint.h>
#include <stdio.h>

#include "ds/lists/sll.h"

int
main ()
{
  sll_t *sll = sll_create ();
  sll_push_back (sll, (void *)0);
  sll_push_back (sll, (void *)1);
  sll_push_back (sll, (void *)2);
  sll_push_back (sll, (void *)3);
  sll_push_back (sll, (void *)4);
  sll_iter_t pos = sll_begin (sll); // 0
  pos = sll_next (pos);             // 1
  pos = sll_next (pos);             // 2
  pos = sll_next (pos);             // 3
  pos = sll_next (pos);             // 4
  // sll_iter_t pos = sll_before_begin (sll);

  sll_t *src = sll_create ();
  sll_push_back (src, (void *)10);
  sll_push_back (src, (void *)11);
  sll_push_back (src, (void *)12);
  sll_push_back (src, (void *)13);
  sll_push_back (src, (void *)14);

  // sll_iter_t before_begin = sll_before_begin (src);
  sll_iter_t first = sll_begin (src); // 10 (X)
  sll_iter_t last = sll_next (first); // 11
  last = sll_next (last);             // 12
  last = sll_next (last);             // 13
  last = sll_next (last);             // 14
  last = sll_next (last);             // null

  sll_splice_after (sll, pos, src, first, last);

  sll_free (src, nullptr);
  sll_free (sll, nullptr);
  return 0;
}

// NOLINTEND