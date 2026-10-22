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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
ಸ್ಥಿರ ಸಂರಚನಾ Curl_llist_ಗಂಟು *verifynode(ಸಂರಚನಾ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  ಪ್ರತ್ಯಾಗಮ n;
}
#ಅನ್ಯಥಾ
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
ಶೂನ್ಯ Curl_llist_init(ಸಂರಚನಾ Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = ರಿಕ್ತ;
  l->_tail = ರಿಕ್ತ;
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
UNITTEST ಶೂನ್ಯ llist_ಸೇರಿಸು_next(ಸಂರಚನಾ Curl_llist *list,
                                ಸಂರಚನಾ Curl_llist_ಗಂಟು *e, ಧ್ರುವ ಶೂನ್ಯ *p,
                                ಸಂರಚನಾ Curl_llist_ಗಂಟು *ne);
UNITTEST ಶೂನ್ಯ llist_ಸೇರಿಸು_next(ಸಂರಚನಾ Curl_llist *list,
                                ಸಂರಚನಾ Curl_llist_ಗಂಟು *e, /* may be NULL */
                                ಧ್ರುವ ಶೂನ್ಯ *p,
                                ಸಂರಚನಾ Curl_llist_ಗಂಟು *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  ಯದಿ(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = ರಿಕ್ತ;
    list->_head->_next = ರಿಕ್ತ;
    list->_tail = ne;
  }
  ಅನ್ಯಥಾ {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    ಯದಿ(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    ಅನ್ಯಥಾ ಯದಿ(e->_next) {
      e->_next->_prev = ne;
    }
    ಅನ್ಯಥಾ {
      list->_tail = ne;
    }
    ಯದಿ(e)
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
ಶೂನ್ಯ Curl_llist_append(ಸಂರಚನಾ Curl_llist *list, ಧ್ರುವ ಶೂನ್ಯ *p,
                       ಸಂರಚನಾ Curl_llist_ಗಂಟು *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ಸೇರಿಸು_next(list, list->_tail, p, ne);
}

ಶೂನ್ಯ *Curl_ಗಂಟು_take_elem(ಸಂರಚನಾ Curl_llist_ಗಂಟು *e)
{
  ಶೂನ್ಯ *ptr;
  ಸಂರಚನಾ Curl_llist *list;
  ಯದಿ(!e)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  ಯದಿ(list) {
    ಯದಿ(e == list->_head) {
      list->_head = e->_next;

      ಯದಿ(!list->_head)
        list->_tail = ರಿಕ್ತ;
      ಅನ್ಯಥಾ
        e->_next->_prev = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(e->_prev)
        e->_prev->_next = e->_next;

      ಯದಿ(!e->_next)
        list->_tail = e->_prev;
      ಅನ್ಯಥಾ
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = ರಿಕ್ತ;
  e->_ptr  = ರಿಕ್ತ;
  e->_prev = ರಿಕ್ತ;
  e->_next = ರಿಕ್ತ;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  ಪ್ರತ್ಯಾಗಮ ptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಂಟು_uremove(ಸಂರಚನಾ Curl_llist_ಗಂಟು *e, ಶೂನ್ಯ *user)
{
  ಸಂರಚನಾ Curl_llist *list;
  ಶೂನ್ಯ *ptr;
  ಯದಿ(!e)
    ಪ್ರತ್ಯಾಗಮ;

  list = e->_list;
  DEBUGASSERT(list);
  ಯದಿ(list) {
    ptr = Curl_ಗಂಟು_take_elem(e);
    ಯದಿ(list->_dtor)
      list->_dtor(user, ptr);
  }
}

ಶೂನ್ಯ Curl_ಗಂಟು_remove(ಸಂರಚನಾ Curl_llist_ಗಂಟು *e)
{
  ಗಂಟು_uremove(e, ರಿಕ್ತ);
}

ಶೂನ್ಯ Curl_llist_destroy(ಸಂರಚನಾ Curl_llist *list, ಶೂನ್ಯ *user)
{
  ಯದಿ(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    ಯಾವತ್(list->_size > 0)
      ಗಂಟು_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ಸಂರಚನಾ Curl_llist_ಗಂಟು *Curl_llist_head(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಪ್ರತ್ಯಾಗಮ VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ಸಂರಚನಾ Curl_llist_ಗಂಟು *llist_tail(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list);
UNITTEST ಸಂರಚನಾ Curl_llist_ಗಂಟು *llist_tail(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಪ್ರತ್ಯಾಗಮ VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ಎಣಿಕೆ(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಪ್ರತ್ಯಾಗಮ list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ಶೂನ್ಯ *Curl_ಗಂಟು_elem(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಪ್ರತ್ಯಾಗಮ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ಸಂರಚನಾ Curl_llist_ಗಂಟು *Curl_ಗಂಟು_next(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಪ್ರತ್ಯಾಗಮ VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ಸಂರಚನಾ Curl_llist_ಗಂಟು *llist_ಗಂಟು_prev(
  ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n);
UNITTEST ಸಂರಚನಾ Curl_llist_ಗಂಟು *llist_ಗಂಟು_prev(
  ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಪ್ರತ್ಯಾಗಮ VERIFYNODE(n->_prev);
}
#endif

ಸಂರಚನಾ Curl_llist *Curl_ಗಂಟು_llist(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  ಪ್ರತ್ಯಾಗಮ n->_list;
}
