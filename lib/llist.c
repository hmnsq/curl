/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
স্থির সংরচনা Curl_llist_গ্রন্থি *verifynode(সংরচনা Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  প্রত্যাগম n;
}
#অন্যথা
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
শূন্য Curl_llist_init(সংরচনা Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = রিক্ত;
  l->_tail = রিক্ত;
#ifdef DEBUGBUILD
  l->_init = LLISTINIT;
#endif
}

/*
 * llist_insert_next()
 *
 * Inserts a new list element after the given one 'e'. If the given existing
 * entry is NULL and the list already has elements, the new one will be
 * inserted first in the list.
 *
 * The 'ne' argument should be a pointer into the object to store.
 *
 * @unittest 1300
 */
UNITTEST শূন্য llist_ঢোকাও_next(সংরচনা Curl_llist *list,
                                সংরচনা Curl_llist_গ্রন্থি *e, ধ্রুব শূন্য *p,
                                সংরচনা Curl_llist_গ্রন্থি *ne);
UNITTEST শূন্য llist_ঢোকাও_next(সংরচনা Curl_llist *list,
                                সংরচনা Curl_llist_গ্রন্থি *e, /* may be NULL */
                                ধ্রুব শূন্য *p,
                                সংরচনা Curl_llist_গ্রন্থি *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  যদি(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = রিক্ত;
    list->_head->_next = রিক্ত;
    list->_tail = ne;
  }
  অন্যথা {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    যদি(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    অন্যথা যদি(e->_next) {
      e->_next->_prev = ne;
    }
    অন্যথা {
      list->_tail = ne;
    }
    যদি(e)
      e->_next = ne;
  }

  ++list->_size;
}

/*
 * Curl_llist_append()
 *
 * Adds a new list element to the end of the list.
 *
 * The 'ne' argument should be a pointer into the object to store.
 *
 * @unittest 1300
 */
শূন্য Curl_llist_append(সংরচনা Curl_llist *list, ধ্রুব শূন্য *p,
                       সংরচনা Curl_llist_গ্রন্থি *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ঢোকাও_next(list, list->_tail, p, ne);
}

শূন্য *Curl_গ্রন্থি_take_elem(সংরচনা Curl_llist_গ্রন্থি *e)
{
  শূন্য *ptr;
  সংরচনা Curl_llist *list;
  যদি(!e)
    প্রত্যাগম রিক্ত;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  যদি(list) {
    যদি(e == list->_head) {
      list->_head = e->_next;

      যদি(!list->_head)
        list->_tail = রিক্ত;
      অন্যথা
        e->_next->_prev = রিক্ত;
    }
    অন্যথা {
      যদি(e->_prev)
        e->_prev->_next = e->_next;

      যদি(!e->_next)
        list->_tail = e->_prev;
      অন্যথা
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = রিক্ত;
  e->_ptr  = রিক্ত;
  e->_prev = রিক্ত;
  e->_next = রিক্ত;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  প্রত্যাগম ptr;
}

স্থির শূন্য গ্রন্থি_uremove(সংরচনা Curl_llist_গ্রন্থি *e, শূন্য *user)
{
  সংরচনা Curl_llist *list;
  শূন্য *ptr;
  যদি(!e)
    প্রত্যাগম;

  list = e->_list;
  DEBUGASSERT(list);
  যদি(list) {
    ptr = Curl_গ্রন্থি_take_elem(e);
    যদি(list->_dtor)
      list->_dtor(user, ptr);
  }
}

শূন্য Curl_গ্রন্থি_remove(সংরচনা Curl_llist_গ্রন্থি *e)
{
  গ্রন্থি_uremove(e, রিক্ত);
}

শূন্য Curl_llist_destroy(সংরচনা Curl_llist *list, শূন্য *user)
{
  যদি(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    যাবত্(list->_size > 0)
      গ্রন্থি_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
সংরচনা Curl_llist_গ্রন্থি *Curl_llist_head(ধ্রুব সংরচনা Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  প্রত্যাগম VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST সংরচনা Curl_llist_গ্রন্থি *llist_tail(ধ্রুব সংরচনা Curl_llist *list);
UNITTEST সংরচনা Curl_llist_গ্রন্থি *llist_tail(ধ্রুব সংরচনা Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  প্রত্যাগম VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_গুনতি(ধ্রুব সংরচনা Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  প্রত্যাগম list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
শূন্য *Curl_গ্রন্থি_elem(ধ্রুব সংরচনা Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  প্রত্যাগম n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
সংরচনা Curl_llist_গ্রন্থি *Curl_গ্রন্থি_next(ধ্রুব সংরচনা Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  প্রত্যাগম VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST সংরচনা Curl_llist_গ্রন্থি *llist_গ্রন্থি_prev(
  ধ্রুব সংরচনা Curl_llist_গ্রন্থি *n);
UNITTEST সংরচনা Curl_llist_গ্রন্থি *llist_গ্রন্থি_prev(
  ধ্রুব সংরচনা Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  প্রত্যাগম VERIFYNODE(n->_prev);
}
#endif

সংরচনা Curl_llist *Curl_গ্রন্থি_llist(ধ্রুব সংরচনা Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  প্রত্যাগম n->_list;
}
