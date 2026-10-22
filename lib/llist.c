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
#ضمن "curl_setup.h"

#ضمن "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
ساكن بنية Curl_llist_node *verifynode(بنية Curl_llist_node *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  أرجع n;
}
#وإلا
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = عدم;
  l->_tail = عدم;
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
UNITTEST فراغ llist_insert_next(بنية Curl_llist *list,
                                بنية Curl_llist_node *e, ثابت فراغ *p,
                                بنية Curl_llist_node *ne);
UNITTEST فراغ llist_insert_next(بنية Curl_llist *list,
                                بنية Curl_llist_node *e, /* may be NULL */
                                ثابت فراغ *p,
                                بنية Curl_llist_node *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  𐡄𐡍(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = عدم;
    list->_head->_next = عدم;
    list->_tail = ne;
  }
  وإلا {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    𐡄𐡍(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    وإلا 𐡄𐡍(e->_next) {
      e->_next->_prev = ne;
    }
    وإلا {
      list->_tail = ne;
    }
    𐡄𐡍(e)
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
فراغ Curl_llist_append(بنية Curl_llist *list, ثابت فراغ *p,
                       بنية Curl_llist_node *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_insert_next(list, list->_tail, p, ne);
}

فراغ *Curl_node_take_elem(بنية Curl_llist_node *e)
{
  فراغ *ptr;
  بنية Curl_llist *list;
  𐡄𐡍(!e)
    أرجع عدم;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  𐡄𐡍(list) {
    𐡄𐡍(e == list->_head) {
      list->_head = e->_next;

      𐡄𐡍(!list->_head)
        list->_tail = عدم;
      وإلا
        e->_next->_prev = عدم;
    }
    وإلا {
      𐡄𐡍(e->_prev)
        e->_prev->_next = e->_next;

      𐡄𐡍(!e->_next)
        list->_tail = e->_prev;
      وإلا
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = عدم;
  e->_ptr  = عدم;
  e->_prev = عدم;
  e->_next = عدم;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  أرجع ptr;
}

ساكن فراغ node_uremove(بنية Curl_llist_node *e, فراغ *user)
{
  بنية Curl_llist *list;
  فراغ *ptr;
  𐡄𐡍(!e)
    أرجع;

  list = e->_list;
  DEBUGASSERT(list);
  𐡄𐡍(list) {
    ptr = Curl_node_take_elem(e);
    𐡄𐡍(list->_dtor)
      list->_dtor(user, ptr);
  }
}

فراغ Curl_node_remove(بنية Curl_llist_node *e)
{
  node_uremove(e, عدم);
}

فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user)
{
  𐡄𐡍(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    طالما(list->_size > 0)
      node_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_node *Curl_llist_head(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST بنية Curl_llist_node *llist_tail(ثابت بنية Curl_llist *list);
UNITTEST بنية Curl_llist_node *llist_tail(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_count(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_node_elem(ثابت بنية Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_node *Curl_node_next(ثابت بنية Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST بنية Curl_llist_node *llist_node_prev(
  ثابت بنية Curl_llist_node *n);
UNITTEST بنية Curl_llist_node *llist_node_prev(
  ثابت بنية Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->_prev);
}
#endif

بنية Curl_llist *Curl_node_llist(ثابت بنية Curl_llist_node *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  أرجع n->_list;
}
