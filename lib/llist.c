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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
સ્થિર સંરચના Curl_llist_ગાંઠ *verifynode(સંરચના Curl_llist_ગાંઠ *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  પાછા n;
}
#નહીંતર
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
શૂન્ય Curl_llist_init(સંરચના Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = ખાલી;
  l->_tail = ખાલી;
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
UNITTEST શૂન્ય llist_ઉમેરો_next(સંરચના Curl_llist *list,
                                સંરચના Curl_llist_ગાંઠ *e, અચલ શૂન્ય *p,
                                સંરચના Curl_llist_ગાંઠ *ne);
UNITTEST શૂન્ય llist_ઉમેરો_next(સંરચના Curl_llist *list,
                                સંરચના Curl_llist_ગાંઠ *e, /* may be NULL */
                                અચલ શૂન્ય *p,
                                સંરચના Curl_llist_ગાંઠ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  જો(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = ખાલી;
    list->_head->_next = ખાલી;
    list->_tail = ne;
  }
  નહીંતર {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    જો(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    નહીંતર જો(e->_next) {
      e->_next->_prev = ne;
    }
    નહીંતર {
      list->_tail = ne;
    }
    જો(e)
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
શૂન્ય Curl_llist_append(સંરચના Curl_llist *list, અચલ શૂન્ય *p,
                       સંરચના Curl_llist_ગાંઠ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ઉમેરો_next(list, list->_tail, p, ne);
}

શૂન્ય *Curl_ગાંઠ_take_elem(સંરચના Curl_llist_ગાંઠ *e)
{
  શૂન્ય *ptr;
  સંરચના Curl_llist *list;
  જો(!e)
    પાછા ખાલી;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  જો(list) {
    જો(e == list->_head) {
      list->_head = e->_next;

      જો(!list->_head)
        list->_tail = ખાલી;
      નહીંતર
        e->_next->_prev = ખાલી;
    }
    નહીંતર {
      જો(e->_prev)
        e->_prev->_next = e->_next;

      જો(!e->_next)
        list->_tail = e->_prev;
      નહીંતર
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = ખાલી;
  e->_ptr  = ખાલી;
  e->_prev = ખાલી;
  e->_next = ખાલી;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  પાછા ptr;
}

સ્થિર શૂન્ય ગાંઠ_uremove(સંરચના Curl_llist_ગાંઠ *e, શૂન્ય *user)
{
  સંરચના Curl_llist *list;
  શૂન્ય *ptr;
  જો(!e)
    પાછા;

  list = e->_list;
  DEBUGASSERT(list);
  જો(list) {
    ptr = Curl_ગાંઠ_take_elem(e);
    જો(list->_dtor)
      list->_dtor(user, ptr);
  }
}

શૂન્ય Curl_ગાંઠ_remove(સંરચના Curl_llist_ગાંઠ *e)
{
  ગાંઠ_uremove(e, ખાલી);
}

શૂન્ય Curl_llist_destroy(સંરચના Curl_llist *list, શૂન્ય *user)
{
  જો(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    જ્યારે(list->_size > 0)
      ગાંઠ_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
સંરચના Curl_llist_ગાંઠ *Curl_llist_head(અચલ સંરચના Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  પાછા VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST સંરચના Curl_llist_ગાંઠ *llist_tail(અચલ સંરચના Curl_llist *list);
UNITTEST સંરચના Curl_llist_ગાંઠ *llist_tail(અચલ સંરચના Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  પાછા VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ગણતરી(અચલ સંરચના Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  પાછા list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
શૂન્ય *Curl_ગાંઠ_elem(અચલ સંરચના Curl_llist_ગાંઠ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  પાછા n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
સંરચના Curl_llist_ગાંઠ *Curl_ગાંઠ_next(અચલ સંરચના Curl_llist_ગાંઠ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  પાછા VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST સંરચના Curl_llist_ગાંઠ *llist_ગાંઠ_prev(
  અચલ સંરચના Curl_llist_ગાંઠ *n);
UNITTEST સંરચના Curl_llist_ગાંઠ *llist_ગાંઠ_prev(
  અચલ સંરચના Curl_llist_ગાંઠ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  પાછા VERIFYNODE(n->_prev);
}
#endif

સંરચના Curl_llist *Curl_ગાંઠ_llist(અચલ સંરચના Curl_llist_ગાંઠ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  પાછા n->_list;
}
