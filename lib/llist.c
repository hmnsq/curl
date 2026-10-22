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
#شامل "curl_setup.h"

#شامل "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
ایستا ساختار Curl_llist_گره *verifynode(ساختار Curl_llist_گره *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  بازگشت n;
}
#وگرنه
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
تهی Curl_llist_init(ساختار Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = پوچ;
  l->_tail = پوچ;
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
UNITTEST تهی llist_درج_next(ساختار Curl_llist *list,
                                ساختار Curl_llist_گره *e, ثابت تهی *p,
                                ساختار Curl_llist_گره *ne);
UNITTEST تهی llist_درج_next(ساختار Curl_llist *list,
                                ساختار Curl_llist_گره *e, /* may be NULL */
                                ثابت تهی *p,
                                ساختار Curl_llist_گره *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  اگر(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = پوچ;
    list->_head->_next = پوچ;
    list->_tail = ne;
  }
  وگرنه {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    اگر(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    وگرنه اگر(e->_next) {
      e->_next->_prev = ne;
    }
    وگرنه {
      list->_tail = ne;
    }
    اگر(e)
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
تهی Curl_llist_append(ساختار Curl_llist *list, ثابت تهی *p,
                       ساختار Curl_llist_گره *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_درج_next(list, list->_tail, p, ne);
}

تهی *Curl_گره_take_elem(ساختار Curl_llist_گره *e)
{
  تهی *ptr;
  ساختار Curl_llist *list;
  اگر(!e)
    بازگشت پوچ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  اگر(list) {
    اگر(e == list->_head) {
      list->_head = e->_next;

      اگر(!list->_head)
        list->_tail = پوچ;
      وگرنه
        e->_next->_prev = پوچ;
    }
    وگرنه {
      اگر(e->_prev)
        e->_prev->_next = e->_next;

      اگر(!e->_next)
        list->_tail = e->_prev;
      وگرنه
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = پوچ;
  e->_ptr  = پوچ;
  e->_prev = پوچ;
  e->_next = پوچ;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  بازگشت ptr;
}

ایستا تهی گره_uremove(ساختار Curl_llist_گره *e, تهی *user)
{
  ساختار Curl_llist *list;
  تهی *ptr;
  اگر(!e)
    بازگشت;

  list = e->_list;
  DEBUGASSERT(list);
  اگر(list) {
    ptr = Curl_گره_take_elem(e);
    اگر(list->_dtor)
      list->_dtor(user, ptr);
  }
}

تهی Curl_گره_remove(ساختار Curl_llist_گره *e)
{
  گره_uremove(e, پوچ);
}

تهی Curl_llist_destroy(ساختار Curl_llist *list, تهی *user)
{
  اگر(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    وقتی(list->_size > 0)
      گره_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ساختار Curl_llist_گره *Curl_llist_head(ثابت ساختار Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  بازگشت VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ساختار Curl_llist_گره *llist_tail(ثابت ساختار Curl_llist *list);
UNITTEST ساختار Curl_llist_گره *llist_tail(ثابت ساختار Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  بازگشت VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمار(ثابت ساختار Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  بازگشت list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
تهی *Curl_گره_elem(ثابت ساختار Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  بازگشت n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ساختار Curl_llist_گره *Curl_گره_next(ثابت ساختار Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  بازگشت VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ساختار Curl_llist_گره *llist_گره_prev(
  ثابت ساختار Curl_llist_گره *n);
UNITTEST ساختار Curl_llist_گره *llist_گره_prev(
  ثابت ساختار Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  بازگشت VERIFYNODE(n->_prev);
}
#endif

ساختار Curl_llist *Curl_گره_llist(ثابت ساختار Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  بازگشت n->_list;
}
