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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು LLISTINIT 0x100cc001 /* random pattern */
#ವ್ಯಾಖ್ಯಾನಿಸು NODEINIT  0x12344321 /* random pattern */
#ವ್ಯಾಖ್ಯಾನಿಸು NODEREM   0x54321012 /* random pattern */

#ವ್ಯಾಖ್ಯಾನಿಸು VERIFYNODE(x) verifynode(x)
ಸ್ಥಿರ ರಚನೆ Curl_llist_ಗಂಟು *verifynode(ರಚನೆ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  ಹಿಂತಿರುಗಿ n;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು VERIFYNODE(x) x
#ಆದರೆ_ಅಂತ್ಯ
/*
 * @unittest 1300
 */
ಶೂನ್ಯ Curl_llist_init(ರಚನೆ Curl_llist *l, Curl_llist_dtor dtor)
{
  l->ಗಾತ್ರ_3 = 0;
  l->_dtor = dtor;
  l->ತಲೆ_2 = ಖಾಲಿ;
  l->ಬಾಲ_2 = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  l->_init = LLISTINIT;
#ಆದರೆ_ಅಂತ್ಯ
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
UNITTEST ಶೂನ್ಯ llist_ಸೇರಿಸು_ಮುಂದಿನ(ರಚನೆ Curl_llist *list,
                                ರಚನೆ Curl_llist_ಗಂಟು *e, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                                ರಚನೆ Curl_llist_ಗಂಟು *ne);
UNITTEST ಶೂನ್ಯ llist_ಸೇರಿಸು_ಮುಂದಿನ(ರಚನೆ Curl_llist *list,
                                ರಚನೆ Curl_llist_ಗಂಟು *e, /* may be NULL */
                                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                                ರಚನೆ Curl_llist_ಗಂಟು *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ne->_init = NODEINIT;
#ಆದರೆ_ಅಂತ್ಯ
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  ಆದರೆ(list->ಗಾತ್ರ_3 == 0) {
    list->ತಲೆ_2 = ne;
    list->ತಲೆ_2->_prev = ಖಾಲಿ;
    list->ತಲೆ_2->ಮುಂದಿನ_2 = ಖಾಲಿ;
    list->ಬಾಲ_2 = ne;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->ಮುಂದಿನ_2 = e ? e->ಮುಂದಿನ_2 : list->ತಲೆ_2;
    ne->_prev = e;
    ಆದರೆ(!e) {
      list->ತಲೆ_2->_prev = ne;
      list->ತಲೆ_2 = ne;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(e->ಮುಂದಿನ_2) {
      e->ಮುಂದಿನ_2->_prev = ne;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      list->ಬಾಲ_2 = ne;
    }
    ಆದರೆ(e)
      e->ಮುಂದಿನ_2 = ne;
  }

  ++list->ಗಾತ್ರ_3;
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
ಶೂನ್ಯ Curl_llist_append(ರಚನೆ Curl_llist *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                       ರಚನೆ Curl_llist_ಗಂಟು *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ಸೇರಿಸು_ಮುಂದಿನ(list, list->ಬಾಲ_2, p, ne);
}

ಶೂನ್ಯ *Curl_ಗಂಟು_take_elem(ರಚನೆ Curl_llist_ಗಂಟು *e)
{
  ಶೂನ್ಯ *ptr;
  ರಚನೆ Curl_llist *list;
  ಆದರೆ(!e)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->ಗಾತ್ರ_3);
  DEBUGASSERT(e->_init == NODEINIT);
  ಆದರೆ(list) {
    ಆದರೆ(e == list->ತಲೆ_2) {
      list->ತಲೆ_2 = e->ಮುಂದಿನ_2;

      ಆದರೆ(!list->ತಲೆ_2)
        list->ಬಾಲ_2 = ಖಾಲಿ;
      ಇಲ್ಲದಿದ್ದರೆ
        e->ಮುಂದಿನ_2->_prev = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(e->_prev)
        e->_prev->ಮುಂದಿನ_2 = e->ಮುಂದಿನ_2;

      ಆದರೆ(!e->ಮುಂದಿನ_2)
        list->ಬಾಲ_2 = e->_prev;
      ಇಲ್ಲದಿದ್ದರೆ
        e->ಮುಂದಿನ_2->_prev = e->_prev;
    }
    --list->ಗಾತ್ರ_3;
  }
  ptr = e->_ptr;

  e->_list = ಖಾಲಿ;
  e->_ptr  = ಖಾಲಿ;
  e->_prev = ಖಾಲಿ;
  e->ಮುಂದಿನ_2 = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ ptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಂಟು_uremove(ರಚನೆ Curl_llist_ಗಂಟು *e, ಶೂನ್ಯ *user)
{
  ರಚನೆ Curl_llist *list;
  ಶೂನ್ಯ *ptr;
  ಆದರೆ(!e)
    ಹಿಂತಿರುಗಿ;

  list = e->_list;
  DEBUGASSERT(list);
  ಆದರೆ(list) {
    ptr = Curl_ಗಂಟು_take_elem(e);
    ಆದರೆ(list->_dtor)
      list->_dtor(user, ptr);
  }
}

ಶೂನ್ಯ Curl_ಗಂಟು_remove(ರಚನೆ Curl_llist_ಗಂಟು *e)
{
  ಗಂಟು_uremove(e, ಖಾಲಿ);
}

ಶೂನ್ಯ Curl_llist_destroy(ರಚನೆ Curl_llist *list, ಶೂನ್ಯ *user)
{
  ಆದರೆ(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    ಆಗ(list->ಗಾತ್ರ_3 > 0)
      ಗಂಟು_uremove(list->ಬಾಲ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ರಚನೆ Curl_llist_ಗಂಟು *Curl_llist_ತಲೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಹಿಂತಿರುಗಿ VERIFYNODE(list->ತಲೆ_2);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ರಚನೆ Curl_llist_ಗಂಟು *llist_ಬಾಲ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list);
UNITTEST ರಚನೆ Curl_llist_ಗಂಟು *llist_ಬಾಲ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಹಿಂತಿರುಗಿ VERIFYNODE(list->ಬಾಲ_2);
}
#ಆದರೆ_ಅಂತ್ಯ

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ಹಿಂತಿರುಗಿ list->ಗಾತ್ರ_3;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ಶೂನ್ಯ *Curl_ಗಂಟು_elem(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಹಿಂತಿರುಗಿ n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ರಚನೆ Curl_llist_ಗಂಟು *Curl_ಗಂಟು_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಹಿಂತಿರುಗಿ VERIFYNODE(n->ಮುಂದಿನ_2);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ರಚನೆ Curl_llist_ಗಂಟು *llist_ಗಂಟು_prev(
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n);
UNITTEST ರಚನೆ Curl_llist_ಗಂಟು *llist_ಗಂಟು_prev(
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ಹಿಂತಿರುಗಿ VERIFYNODE(n->_prev);
}
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ Curl_llist *Curl_ಗಂಟು_llist(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  ಹಿಂತಿರುಗಿ n->_list;
}
