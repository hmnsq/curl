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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
സ്ഥിതം ഘടന Curl_llist_കണ്ണി *verifynode(ഘടന Curl_llist_കണ്ണി *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  തിരികെ n;
}
#അല്ലെങ്കിൽ
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
ശൂന്യം Curl_llist_init(ഘടന Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = ശൂന്യ;
  l->_tail = ശൂന്യ;
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
UNITTEST ശൂന്യം llist_തിരുകുക_next(ഘടന Curl_llist *list,
                                ഘടന Curl_llist_കണ്ണി *e, സ്ഥിരം ശൂന്യം *p,
                                ഘടന Curl_llist_കണ്ണി *ne);
UNITTEST ശൂന്യം llist_തിരുകുക_next(ഘടന Curl_llist *list,
                                ഘടന Curl_llist_കണ്ണി *e, /* may be NULL */
                                സ്ഥിരം ശൂന്യം *p,
                                ഘടന Curl_llist_കണ്ണി *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  എങ്കിൽ(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = ശൂന്യ;
    list->_head->_next = ശൂന്യ;
    list->_tail = ne;
  }
  അല്ലെങ്കിൽ {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    എങ്കിൽ(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(e->_next) {
      e->_next->_prev = ne;
    }
    അല്ലെങ്കിൽ {
      list->_tail = ne;
    }
    എങ്കിൽ(e)
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
ശൂന്യം Curl_llist_append(ഘടന Curl_llist *list, സ്ഥിരം ശൂന്യം *p,
                       ഘടന Curl_llist_കണ്ണി *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_തിരുകുക_next(list, list->_tail, p, ne);
}

ശൂന്യം *Curl_കണ്ണി_take_elem(ഘടന Curl_llist_കണ്ണി *e)
{
  ശൂന്യം *ptr;
  ഘടന Curl_llist *list;
  എങ്കിൽ(!e)
    തിരികെ ശൂന്യ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  എങ്കിൽ(list) {
    എങ്കിൽ(e == list->_head) {
      list->_head = e->_next;

      എങ്കിൽ(!list->_head)
        list->_tail = ശൂന്യ;
      അല്ലെങ്കിൽ
        e->_next->_prev = ശൂന്യ;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(e->_prev)
        e->_prev->_next = e->_next;

      എങ്കിൽ(!e->_next)
        list->_tail = e->_prev;
      അല്ലെങ്കിൽ
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = ശൂന്യ;
  e->_ptr  = ശൂന്യ;
  e->_prev = ശൂന്യ;
  e->_next = ശൂന്യ;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  തിരികെ ptr;
}

സ്ഥിതം ശൂന്യം കണ്ണി_uremove(ഘടന Curl_llist_കണ്ണി *e, ശൂന്യം *user)
{
  ഘടന Curl_llist *list;
  ശൂന്യം *ptr;
  എങ്കിൽ(!e)
    തിരികെ;

  list = e->_list;
  DEBUGASSERT(list);
  എങ്കിൽ(list) {
    ptr = Curl_കണ്ണി_take_elem(e);
    എങ്കിൽ(list->_dtor)
      list->_dtor(user, ptr);
  }
}

ശൂന്യം Curl_കണ്ണി_remove(ഘടന Curl_llist_കണ്ണി *e)
{
  കണ്ണി_uremove(e, ശൂന്യ);
}

ശൂന്യം Curl_llist_destroy(ഘടന Curl_llist *list, ശൂന്യം *user)
{
  എങ്കിൽ(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    ആകുമ്പോൾ(list->_size > 0)
      കണ്ണി_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ഘടന Curl_llist_കണ്ണി *Curl_llist_head(സ്ഥിരം ഘടന Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  തിരികെ VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ഘടന Curl_llist_കണ്ണി *llist_tail(സ്ഥിരം ഘടന Curl_llist *list);
UNITTEST ഘടന Curl_llist_കണ്ണി *llist_tail(സ്ഥിരം ഘടന Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  തിരികെ VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_എണ്ണം(സ്ഥിരം ഘടന Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  തിരികെ list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ശൂന്യം *Curl_കണ്ണി_elem(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  തിരികെ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ഘടന Curl_llist_കണ്ണി *Curl_കണ്ണി_next(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  തിരികെ VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ഘടന Curl_llist_കണ്ണി *llist_കണ്ണി_prev(
  സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n);
UNITTEST ഘടന Curl_llist_കണ്ണി *llist_കണ്ണി_prev(
  സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  തിരികെ VERIFYNODE(n->_prev);
}
#endif

ഘടന Curl_llist *Curl_കണ്ണി_llist(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  തിരികെ n->_list;
}
