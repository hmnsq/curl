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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLISTINIT 0x100cc001 /* random pattern */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NODEINIT  0x12344321 /* random pattern */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NODEREM   0x54321012 /* random pattern */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERIFYNODE(x) verifynode(x)
ਸਥਿਰ ਬਣਤਰ Curl_llist_ਗੰਢ *verifynode(ਬਣਤਰ Curl_llist_ਗੰਢ *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  ਵਾਪਸ n;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERIFYNODE(x) x
#ਜੇ_ਅੰਤ
/*
 * @unittest 1300
 */
ਖਾਲੀ Curl_llist_init(ਬਣਤਰ Curl_llist *l, Curl_llist_dtor dtor)
{
  l->ਮਾਪ_2 = 0;
  l->_dtor = dtor;
  l->ਸਿਰ_2 = ਨਿਰਾ;
  l->ਪੂਛ_2 = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  l->_init = LLISTINIT;
#ਜੇ_ਅੰਤ
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
UNITTEST ਖਾਲੀ llist_ਪਾਓ_ਅਗਲਾ(ਬਣਤਰ Curl_llist *list,
                                ਬਣਤਰ Curl_llist_ਗੰਢ *e, ਅਚਲ ਖਾਲੀ *p,
                                ਬਣਤਰ Curl_llist_ਗੰਢ *ne);
UNITTEST ਖਾਲੀ llist_ਪਾਓ_ਅਗਲਾ(ਬਣਤਰ Curl_llist *list,
                                ਬਣਤਰ Curl_llist_ਗੰਢ *e, /* may be NULL */
                                ਅਚਲ ਖਾਲੀ *p,
                                ਬਣਤਰ Curl_llist_ਗੰਢ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ne->_init = NODEINIT;
#ਜੇ_ਅੰਤ
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  ਜੇ(list->ਮਾਪ_2 == 0) {
    list->ਸਿਰ_2 = ne;
    list->ਸਿਰ_2->_prev = ਨਿਰਾ;
    list->ਸਿਰ_2->ਅਗਲਾ_2 = ਨਿਰਾ;
    list->ਪੂਛ_2 = ne;
  }
  ਨਹੀਂਤਾਂ {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->ਅਗਲਾ_2 = e ? e->ਅਗਲਾ_2 : list->ਸਿਰ_2;
    ne->_prev = e;
    ਜੇ(!e) {
      list->ਸਿਰ_2->_prev = ne;
      list->ਸਿਰ_2 = ne;
    }
    ਨਹੀਂਤਾਂ ਜੇ(e->ਅਗਲਾ_2) {
      e->ਅਗਲਾ_2->_prev = ne;
    }
    ਨਹੀਂਤਾਂ {
      list->ਪੂਛ_2 = ne;
    }
    ਜੇ(e)
      e->ਅਗਲਾ_2 = ne;
  }

  ++list->ਮਾਪ_2;
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
ਖਾਲੀ Curl_llist_append(ਬਣਤਰ Curl_llist *list, ਅਚਲ ਖਾਲੀ *p,
                       ਬਣਤਰ Curl_llist_ਗੰਢ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ਪਾਓ_ਅਗਲਾ(list, list->ਪੂਛ_2, p, ne);
}

ਖਾਲੀ *Curl_ਗੰਢ_take_elem(ਬਣਤਰ Curl_llist_ਗੰਢ *e)
{
  ਖਾਲੀ *ptr;
  ਬਣਤਰ Curl_llist *list;
  ਜੇ(!e)
    ਵਾਪਸ ਨਿਰਾ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->ਮਾਪ_2);
  DEBUGASSERT(e->_init == NODEINIT);
  ਜੇ(list) {
    ਜੇ(e == list->ਸਿਰ_2) {
      list->ਸਿਰ_2 = e->ਅਗਲਾ_2;

      ਜੇ(!list->ਸਿਰ_2)
        list->ਪੂਛ_2 = ਨਿਰਾ;
      ਨਹੀਂਤਾਂ
        e->ਅਗਲਾ_2->_prev = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(e->_prev)
        e->_prev->ਅਗਲਾ_2 = e->ਅਗਲਾ_2;

      ਜੇ(!e->ਅਗਲਾ_2)
        list->ਪੂਛ_2 = e->_prev;
      ਨਹੀਂਤਾਂ
        e->ਅਗਲਾ_2->_prev = e->_prev;
    }
    --list->ਮਾਪ_2;
  }
  ptr = e->_ptr;

  e->_list = ਨਿਰਾ;
  e->_ptr  = ਨਿਰਾ;
  e->_prev = ਨਿਰਾ;
  e->ਅਗਲਾ_2 = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#ਜੇ_ਅੰਤ

  ਵਾਪਸ ptr;
}

ਸਥਿਰ ਖਾਲੀ ਗੰਢ_uremove(ਬਣਤਰ Curl_llist_ਗੰਢ *e, ਖਾਲੀ *user)
{
  ਬਣਤਰ Curl_llist *list;
  ਖਾਲੀ *ptr;
  ਜੇ(!e)
    ਵਾਪਸ;

  list = e->_list;
  DEBUGASSERT(list);
  ਜੇ(list) {
    ptr = Curl_ਗੰਢ_take_elem(e);
    ਜੇ(list->_dtor)
      list->_dtor(user, ptr);
  }
}

ਖਾਲੀ Curl_ਗੰਢ_remove(ਬਣਤਰ Curl_llist_ਗੰਢ *e)
{
  ਗੰਢ_uremove(e, ਨਿਰਾ);
}

ਖਾਲੀ Curl_llist_destroy(ਬਣਤਰ Curl_llist *list, ਖਾਲੀ *user)
{
  ਜੇ(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    ਜਦੋਂ(list->ਮਾਪ_2 > 0)
      ਗੰਢ_uremove(list->ਪੂਛ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ਬਣਤਰ Curl_llist_ਗੰਢ *Curl_llist_ਸਿਰ(ਅਚਲ ਬਣਤਰ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ਵਾਪਸ VERIFYNODE(list->ਸਿਰ_2);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ਬਣਤਰ Curl_llist_ਗੰਢ *llist_ਪੂਛ(ਅਚਲ ਬਣਤਰ Curl_llist *list);
UNITTEST ਬਣਤਰ Curl_llist_ਗੰਢ *llist_ਪੂਛ(ਅਚਲ ਬਣਤਰ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ਵਾਪਸ VERIFYNODE(list->ਪੂਛ_2);
}
#ਜੇ_ਅੰਤ

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ਗਿਣਤੀ(ਅਚਲ ਬਣਤਰ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ਵਾਪਸ list->ਮਾਪ_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ਖਾਲੀ *Curl_ਗੰਢ_elem(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ਵਾਪਸ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ਬਣਤਰ Curl_llist_ਗੰਢ *Curl_ਗੰਢ_ਅਗਲਾ(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ਵਾਪਸ VERIFYNODE(n->ਅਗਲਾ_2);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ਬਣਤਰ Curl_llist_ਗੰਢ *llist_ਗੰਢ_prev(
  ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n);
UNITTEST ਬਣਤਰ Curl_llist_ਗੰਢ *llist_ਗੰਢ_prev(
  ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ਵਾਪਸ VERIFYNODE(n->_prev);
}
#ਜੇ_ਅੰਤ

ਬਣਤਰ Curl_llist *Curl_ਗੰਢ_llist(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  ਵਾਪਸ n->_list;
}
