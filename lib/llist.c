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
#include "curl_setup.h"

#include "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
static struct Curl_llist_node *verifynode(struct Curl_llist_node *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  trả_về n;
}
#không_thì
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
void Curl_llist_init(struct Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = NULL;
  l->_tail = NULL;
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
UNITTEST void llist_insert_next(struct Curl_llist *list,
                                struct Curl_llist_node *e, const void *p,
                                struct Curl_llist_node *ne);
UNITTEST void llist_insert_next(struct Curl_llist *list,
                                struct Curl_llist_node *e, /* may be NULL */
                                const void *p,
                                struct Curl_llist_node *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  nếu(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = NULL;
    list->_head->_next = NULL;
    list->_tail = ne;
  }
  không_thì {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    nếu(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    không_thì nếu(e->_next) {
      e->_next->_prev = ne;
    }
    không_thì {
      list->_tail = ne;
    }
    nếu(e)
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
void Curl_llist_append(struct Curl_llist *list, const void *p,
                       struct Curl_llist_node *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_insert_next(list, list->_tail, p, ne);
}

void *Curl_node_take_elem(struct Curl_llist_node *e)
{
  void *ptr;
  struct Curl_llist *list;
  nếu(!e)
    trả_về NULL;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  nếu(list) {
    nếu(e == list->_head) {
      list->_head = e->_next;

      nếu(!list->_head)
        list->_tail = NULL;
      không_thì
        e->_next->_prev = NULL;
    }
    không_thì {
      nếu(e->_prev)
        e->_prev->_next = e->_next;

      nếu(!e->_next)
        list->_tail = e->_prev;
      không_thì
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = NULL;
  e->_ptr  = NULL;
  e->_prev = NULL;
  e->_next = NULL;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  trả_về ptr;
}

static void node_uremove(struct Curl_llist_node *e, void *user)
{
  struct Curl_llist *list;
  void *ptr;
  nếu(!e)
    trả_về;

  list = e->_list;
  DEBUGASSERT(list);
  nếu(list) {
    ptr = Curl_node_take_elem(e);
    nếu(list->_dtor)
      list->_dtor(user, ptr);
  }
}

void Curl_node_remove(struct Curl_llist_node *e)
{
  node_uremove(e, NULL);
}

void Curl_llist_destroy(struct Curl_llist *list, void *user)
{
  nếu(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    trong_khi(list->_size > 0)
      node_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
struct Curl_llist_node *Curl_llist_head(const struct Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  trả_về VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST struct Curl_llist_node *llist_tail(const struct Curl_llist *list);
UNITTEST struct Curl_llist_node *llist_tail(const struct Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  trả_về VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_count(const struct Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  trả_về list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
void *Curl_node_elem(const struct Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  trả_về n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
struct Curl_llist_node *Curl_node_next(const struct Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  trả_về VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST struct Curl_llist_node *llist_node_prev(
  const struct Curl_llist_node *n);
UNITTEST struct Curl_llist_node *llist_node_prev(
  const struct Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  trả_về VERIFYNODE(n->_prev);
}
#endif

struct Curl_llist *Curl_node_llist(const struct Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  trả_về n->_list;
}
