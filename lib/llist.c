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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
ସ୍ଥିର ସଂରଚନା Curl_llist_ଗଣ୍ଠି *verifynode(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  ଫେରନ୍ତୁ n;
}
#ଅନ୍ୟଥା
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
ଶୂନ୍ୟ Curl_llist_init(ସଂରଚନା Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = ଖାଲି;
  l->_tail = ଖାଲି;
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
UNITTEST ଶୂନ୍ୟ llist_ଭର୍ତ୍ତି_next(ସଂରଚନା Curl_llist *list,
                                ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e, ଅଚଳ ଶୂନ୍ୟ *p,
                                ସଂରଚନା Curl_llist_ଗଣ୍ଠି *ne);
UNITTEST ଶୂନ୍ୟ llist_ଭର୍ତ୍ତି_next(ସଂରଚନା Curl_llist *list,
                                ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e, /* may be NULL */
                                ଅଚଳ ଶୂନ୍ୟ *p,
                                ସଂରଚନା Curl_llist_ଗଣ୍ଠି *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  ଯଦି(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = ଖାଲି;
    list->_head->_next = ଖାଲି;
    list->_tail = ne;
  }
  ଅନ୍ୟଥା {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    ଯଦି(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    ଅନ୍ୟଥା ଯଦି(e->_next) {
      e->_next->_prev = ne;
    }
    ଅନ୍ୟଥା {
      list->_tail = ne;
    }
    ଯଦି(e)
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
ଶୂନ୍ୟ Curl_llist_append(ସଂରଚନା Curl_llist *list, ଅଚଳ ଶୂନ୍ୟ *p,
                       ସଂରଚନା Curl_llist_ଗଣ୍ଠି *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ଭର୍ତ୍ତି_next(list, list->_tail, p, ne);
}

ଶୂନ୍ୟ *Curl_ଗଣ୍ଠି_take_elem(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e)
{
  ଶୂନ୍ୟ *ptr;
  ସଂରଚନା Curl_llist *list;
  ଯଦି(!e)
    ଫେରନ୍ତୁ ଖାଲି;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  ଯଦି(list) {
    ଯଦି(e == list->_head) {
      list->_head = e->_next;

      ଯଦି(!list->_head)
        list->_tail = ଖାଲି;
      ଅନ୍ୟଥା
        e->_next->_prev = ଖାଲି;
    }
    ଅନ୍ୟଥା {
      ଯଦି(e->_prev)
        e->_prev->_next = e->_next;

      ଯଦି(!e->_next)
        list->_tail = e->_prev;
      ଅନ୍ୟଥା
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = ଖାଲି;
  e->_ptr  = ଖାଲି;
  e->_prev = ଖାଲି;
  e->_next = ଖାଲି;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  ଫେରନ୍ତୁ ptr;
}

ସ୍ଥିର ଶୂନ୍ୟ ଗଣ୍ଠି_uremove(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e, ଶୂନ୍ୟ *user)
{
  ସଂରଚନା Curl_llist *list;
  ଶୂନ୍ୟ *ptr;
  ଯଦି(!e)
    ଫେରନ୍ତୁ;

  list = e->_list;
  DEBUGASSERT(list);
  ଯଦି(list) {
    ptr = Curl_ଗଣ୍ଠି_take_elem(e);
    ଯଦି(list->_dtor)
      list->_dtor(user, ptr);
  }
}

ଶୂନ୍ୟ Curl_ଗଣ୍ଠି_remove(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e)
{
  ଗଣ୍ଠି_uremove(e, ଖାଲି);
}

ଶୂନ୍ୟ Curl_llist_destroy(ସଂରଚନା Curl_llist *list, ଶୂନ୍ୟ *user)
{
  ଯଦି(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    ଯେତେବେଳେ(list->_size > 0)
      ଗଣ୍ଠି_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ସଂରଚନା Curl_llist_ଗଣ୍ଠି *Curl_llist_head(ଅଚଳ ସଂରଚନା Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ଫେରନ୍ତୁ VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ସଂରଚନା Curl_llist_ଗଣ୍ଠି *llist_tail(ଅଚଳ ସଂରଚନା Curl_llist *list);
UNITTEST ସଂରଚନା Curl_llist_ଗଣ୍ଠି *llist_tail(ଅଚଳ ସଂରଚନା Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ଫେରନ୍ତୁ VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ଗଣନା(ଅଚଳ ସଂରଚନା Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ଫେରନ୍ତୁ list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ଶୂନ୍ୟ *Curl_ଗଣ୍ଠି_elem(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ଫେରନ୍ତୁ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ସଂରଚନା Curl_llist_ଗଣ୍ଠି *Curl_ଗଣ୍ଠି_next(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ଫେରନ୍ତୁ VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ସଂରଚନା Curl_llist_ଗଣ୍ଠି *llist_ଗଣ୍ଠି_prev(
  ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n);
UNITTEST ସଂରଚନା Curl_llist_ଗଣ୍ଠି *llist_ଗଣ୍ଠି_prev(
  ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ଫେରନ୍ତୁ VERIFYNODE(n->_prev);
}
#endif

ସଂରଚନା Curl_llist *Curl_ଗଣ୍ଠି_llist(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  ଫେରନ୍ତୁ n->_list;
}
