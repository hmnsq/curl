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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
स्थिर संरचना Curl_llist_गाँठो *verifynode(संरचना Curl_llist_गाँठो *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  फर्काऊ n;
}
#अन्यथा
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
शून्य Curl_llist_init(संरचना Curl_llist *l, Curl_llist_dtor dtor)
{
  l->_size = 0;
  l->_dtor = dtor;
  l->_head = रिक्त;
  l->_tail = रिक्त;
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
UNITTEST शून्य llist_घुसाउ_next(संरचना Curl_llist *list,
                                संरचना Curl_llist_गाँठो *e, अचर शून्य *p,
                                संरचना Curl_llist_गाँठो *ne);
UNITTEST शून्य llist_घुसाउ_next(संरचना Curl_llist *list,
                                संरचना Curl_llist_गाँठो *e, /* may be NULL */
                                अचर शून्य *p,
                                संरचना Curl_llist_गाँठो *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  यदि(list->_size == 0) {
    list->_head = ne;
    list->_head->_prev = रिक्त;
    list->_head->_next = रिक्त;
    list->_tail = ne;
  }
  अन्यथा {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->_next = e ? e->_next : list->_head;
    ne->_prev = e;
    यदि(!e) {
      list->_head->_prev = ne;
      list->_head = ne;
    }
    अन्यथा यदि(e->_next) {
      e->_next->_prev = ne;
    }
    अन्यथा {
      list->_tail = ne;
    }
    यदि(e)
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
शून्य Curl_llist_append(संरचना Curl_llist *list, अचर शून्य *p,
                       संरचना Curl_llist_गाँठो *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_घुसाउ_next(list, list->_tail, p, ne);
}

शून्य *Curl_गाँठो_take_elem(संरचना Curl_llist_गाँठो *e)
{
  शून्य *ptr;
  संरचना Curl_llist *list;
  यदि(!e)
    फर्काऊ रिक्त;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->_size);
  DEBUGASSERT(e->_init == NODEINIT);
  यदि(list) {
    यदि(e == list->_head) {
      list->_head = e->_next;

      यदि(!list->_head)
        list->_tail = रिक्त;
      अन्यथा
        e->_next->_prev = रिक्त;
    }
    अन्यथा {
      यदि(e->_prev)
        e->_prev->_next = e->_next;

      यदि(!e->_next)
        list->_tail = e->_prev;
      अन्यथा
        e->_next->_prev = e->_prev;
    }
    --list->_size;
  }
  ptr = e->_ptr;

  e->_list = रिक्त;
  e->_ptr  = रिक्त;
  e->_prev = रिक्त;
  e->_next = रिक्त;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  फर्काऊ ptr;
}

स्थिर शून्य गाँठो_uremove(संरचना Curl_llist_गाँठो *e, शून्य *user)
{
  संरचना Curl_llist *list;
  शून्य *ptr;
  यदि(!e)
    फर्काऊ;

  list = e->_list;
  DEBUGASSERT(list);
  यदि(list) {
    ptr = Curl_गाँठो_take_elem(e);
    यदि(list->_dtor)
      list->_dtor(user, ptr);
  }
}

शून्य Curl_गाँठो_remove(संरचना Curl_llist_गाँठो *e)
{
  गाँठो_uremove(e, रिक्त);
}

शून्य Curl_llist_destroy(संरचना Curl_llist *list, शून्य *user)
{
  यदि(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    जबसम्म(list->_size > 0)
      गाँठो_uremove(list->_tail, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
संरचना Curl_llist_गाँठो *Curl_llist_head(अचर संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  फर्काऊ VERIFYNODE(list->_head);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST संरचना Curl_llist_गाँठो *llist_tail(अचर संरचना Curl_llist *list);
UNITTEST संरचना Curl_llist_गाँठो *llist_tail(अचर संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  फर्काऊ VERIFYNODE(list->_tail);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
माप_प्रकार Curl_llist_गन्ती(अचर संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  फर्काऊ list->_size;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
शून्य *Curl_गाँठो_elem(अचर संरचना Curl_llist_गाँठो *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  फर्काऊ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
संरचना Curl_llist_गाँठो *Curl_गाँठो_next(अचर संरचना Curl_llist_गाँठो *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  फर्काऊ VERIFYNODE(n->_next);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST संरचना Curl_llist_गाँठो *llist_गाँठो_prev(
  अचर संरचना Curl_llist_गाँठो *n);
UNITTEST संरचना Curl_llist_गाँठो *llist_गाँठो_prev(
  अचर संरचना Curl_llist_गाँठो *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  फर्काऊ VERIFYNODE(n->_prev);
}
#endif

संरचना Curl_llist *Curl_गाँठो_llist(अचर संरचना Curl_llist_गाँठो *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  फर्काऊ n->_list;
}
