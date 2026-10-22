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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
நிலையான கட்டமைப்பு Curl_llist_முனை *verifynode(கட்டமைப்பு Curl_llist_முனை *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  திரும்பு n;
}
#இல்லையெனில்
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
வெறுமை Curl_llist_init(கட்டமைப்பு Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = வெற்று;
  l->_tail = வெற்று;
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
UNITTEST வெறுமை llist_செருகு_next(கட்டமைப்பு Curl_llist *list,
                                கட்டமைப்பு Curl_llist_முனை *e, மாறிலி வெறுமை *p,
                                கட்டமைப்பு Curl_llist_முனை *ne);
UNITTEST வெறுமை llist_செருகு_next(கட்டமைப்பு Curl_llist *list,
                                கட்டமைப்பு Curl_llist_முனை *e, /* may be NULL */
                                மாறிலி வெறுமை *p,
                                கட்டமைப்பு Curl_llist_முனை *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  எனில்(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = வெற்று;
    list->_head->_next = வெற்று;
    list->_tail = ne;
  }
  இல்லையெனில் {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    எனில்(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    இல்லையெனில் எனில்(e->_next) {
      e->_next->_prev = ne;
    }
    இல்லையெனில் {
      list->_tail = ne;
    }
    எனில்(e)
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
வெறுமை Curl_llist_append(கட்டமைப்பு Curl_llist *list, மாறிலி வெறுமை *p,
                       கட்டமைப்பு Curl_llist_முனை *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_செருகு_next(list, list->_tail, p, ne);
}

வெறுமை *Curl_முனை_take_elem(கட்டமைப்பு Curl_llist_முனை *e)
{
  வெறுமை *ptr;
  கட்டமைப்பு Curl_llist *list;
  எனில்(!e)
    திரும்பு வெற்று;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  எனில்(list) {
    எனில்(e == list->_head) {
      list->_head = e->_next;

      எனில்(!list->_head)
        list->_tail = வெற்று;
      இல்லையெனில்
        e->_next->_prev = வெற்று;
    }
    இல்லையெனில் {
      எனில்(e->_prev)
        e->_prev->_next = e->_next;

      எனில்(!e->_next)
        list->_tail = e->_prev;
      இல்லையெனில்
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = வெற்று;
  e->_ptr  = வெற்று;
  e->_prev = வெற்று;
  e->_next = வெற்று;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  திரும்பு ptr;
}

நிலையான வெறுமை முனை_uremove(கட்டமைப்பு Curl_llist_முனை *e, வெறுமை *user)
{
  கட்டமைப்பு Curl_llist *list;
  வெறுமை *ptr;
  எனில்(!e)
    திரும்பு;

  list = e->_list;
  DEBUGASSERT(list);
  எனில்(list) {
    ptr = Curl_முனை_take_elem(e);
    எனில்(list->_dtor)
      list->_dtor(user, ptr);
  }
}

வெறுமை Curl_முனை_remove(கட்டமைப்பு Curl_llist_முனை *e)
{
  முனை_uremove(e, வெற்று);
}

வெறுமை Curl_llist_destroy(கட்டமைப்பு Curl_llist *list, வெறுமை *user)
{
  எனில்(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    போது(list->_size > 0)
      முனை_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
கட்டமைப்பு Curl_llist_முனை *Curl_llist_head(மாறிலி கட்டமைப்பு Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  திரும்பு VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST கட்டமைப்பு Curl_llist_முனை *llist_tail(மாறிலி கட்டமைப்பு Curl_llist *list);
UNITTEST கட்டமைப்பு Curl_llist_முனை *llist_tail(மாறிலி கட்டமைப்பு Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  திரும்பு VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_மொத்தம்(மாறிலி கட்டமைப்பு Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  திரும்பு list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
வெறுமை *Curl_முனை_elem(மாறிலி கட்டமைப்பு Curl_llist_முனை *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  திரும்பு n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
கட்டமைப்பு Curl_llist_முனை *Curl_முனை_next(மாறிலி கட்டமைப்பு Curl_llist_முனை *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  திரும்பு VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST கட்டமைப்பு Curl_llist_முனை *llist_முனை_prev(
  மாறிலி கட்டமைப்பு Curl_llist_முனை *n);
UNITTEST கட்டமைப்பு Curl_llist_முனை *llist_முனை_prev(
  மாறிலி கட்டமைப்பு Curl_llist_முனை *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  திரும்பு VERIFYNODE(n->_prev);
}
#endif

கட்டமைப்பு Curl_llist *Curl_முனை_llist(மாறிலி கட்டமைப்பு Curl_llist_முனை *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  திரும்பு n->_list;
}
