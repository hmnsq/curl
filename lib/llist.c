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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "llist.h"

#ifdef DEBUGBUILD
#define LLISTINIT 0x100cc001 /* random pattern */
#define NODEINIT  0x12344321 /* random pattern */
#define NODEREM   0x54321012 /* random pattern */

#define VERIFYNODE(x) verifynode(x)
স্থির কাঠামো Curl_llist_গ্রন্থি *verifynode(কাঠামো Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  ফেরত n;
}
#অন্যথা
#define VERIFYNODE(x) x
#endif
/*
 * @unittest 1300
 */
শূন্য Curl_llist_init(কাঠামো Curl_llist *l, Curl_llist_dtor dtor)
{
  l->আকার_3 = 0;
  l->_dtor = dtor;
  l->শীর্ষ_3 = ফাঁকা;
  l->লেজ_2 = ফাঁকা;
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
UNITTEST শূন্য llist_ঢোকাও_পরবর্তী(কাঠামো Curl_llist *list,
                                কাঠামো Curl_llist_গ্রন্থি *e, ধ্রুবক শূন্য *p,
                                কাঠামো Curl_llist_গ্রন্থি *ne);
UNITTEST শূন্য llist_ঢোকাও_পরবর্তী(কাঠামো Curl_llist *list,
                                কাঠামো Curl_llist_গ্রন্থি *e, /* may be NULL */
                                ধ্রুবক শূন্য *p,
                                কাঠামো Curl_llist_গ্রন্থি *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#ifdef DEBUGBUILD
  ne->_init = NODEINIT;
#endif
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  যদি(list->আকার_3 == 0) {
    list->শীর্ষ_3 = ne;
    list->শীর্ষ_3->পূর্ব_2 = ফাঁকা;
    list->শীর্ষ_3->পরবর্তী_2 = ফাঁকা;
    list->লেজ_2 = ne;
  }
  অন্যথা {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->পরবর্তী_2 = e ? e->পরবর্তী_2 : list->শীর্ষ_3;
    ne->পূর্ব_2 = e;
    যদি(!e) {
      list->শীর্ষ_3->পূর্ব_2 = ne;
      list->শীর্ষ_3 = ne;
    }
    অন্যথা যদি(e->পরবর্তী_2) {
      e->পরবর্তী_2->পূর্ব_2 = ne;
    }
    অন্যথা {
      list->লেজ_2 = ne;
    }
    যদি(e)
      e->পরবর্তী_2 = ne;
  }

  ++list->আকার_3;
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
শূন্য Curl_llist_append(কাঠামো Curl_llist *list, ধ্রুবক শূন্য *p,
                       কাঠামো Curl_llist_গ্রন্থি *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ঢোকাও_পরবর্তী(list, list->লেজ_2, p, ne);
}

শূন্য *Curl_গ্রন্থি_take_elem(কাঠামো Curl_llist_গ্রন্থি *e)
{
  শূন্য *ptr;
  কাঠামো Curl_llist *list;
  যদি(!e)
    ফেরত ফাঁকা;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->আকার_3);
  DEBUGASSERT(e->_init == NODEINIT);
  যদি(list) {
    যদি(e == list->শীর্ষ_3) {
      list->শীর্ষ_3 = e->পরবর্তী_2;

      যদি(!list->শীর্ষ_3)
        list->লেজ_2 = ফাঁকা;
      অন্যথা
        e->পরবর্তী_2->পূর্ব_2 = ফাঁকা;
    }
    অন্যথা {
      যদি(e->পূর্ব_2)
        e->পূর্ব_2->পরবর্তী_2 = e->পরবর্তী_2;

      যদি(!e->পরবর্তী_2)
        list->লেজ_2 = e->পূর্ব_2;
      অন্যথা
        e->পরবর্তী_2->পূর্ব_2 = e->পূর্ব_2;
    }
    --list->আকার_3;
  }
  ptr = e->_ptr;

  e->_list = ফাঁকা;
  e->_ptr  = ফাঁকা;
  e->পূর্ব_2 = ফাঁকা;
  e->পরবর্তী_2 = ফাঁকা;
#ifdef DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#endif

  ফেরত ptr;
}

স্থির শূন্য গ্রন্থি_uremove(কাঠামো Curl_llist_গ্রন্থি *e, শূন্য *user)
{
  কাঠামো Curl_llist *list;
  শূন্য *ptr;
  যদি(!e)
    ফেরত;

  list = e->_list;
  DEBUGASSERT(list);
  যদি(list) {
    ptr = Curl_গ্রন্থি_take_elem(e);
    যদি(list->_dtor)
      list->_dtor(user, ptr);
  }
}

শূন্য Curl_গ্রন্থি_remove(কাঠামো Curl_llist_গ্রন্থি *e)
{
  গ্রন্থি_uremove(e, ফাঁকা);
}

শূন্য Curl_llist_destroy(কাঠামো Curl_llist *list, শূন্য *user)
{
  যদি(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    যখন(list->আকার_3 > 0)
      গ্রন্থি_uremove(list->লেজ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
কাঠামো Curl_llist_গ্রন্থি *Curl_llist_শীর্ষ(ধ্রুবক কাঠামো Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ফেরত VERIFYNODE(list->শীর্ষ_3);
}

#ifdef UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST কাঠামো Curl_llist_গ্রন্থি *llist_লেজ(ধ্রুবক কাঠামো Curl_llist *list);
UNITTEST কাঠামো Curl_llist_গ্রন্থি *llist_লেজ(ধ্রুবক কাঠামো Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ফেরত VERIFYNODE(list->লেজ_2);
}
#endif

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_গুনতি(ধ্রুবক কাঠামো Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  ফেরত list->আকার_3;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
শূন্য *Curl_গ্রন্থি_elem(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ফেরত n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
কাঠামো Curl_llist_গ্রন্থি *Curl_গ্রন্থি_পরবর্তী(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ফেরত VERIFYNODE(n->পরবর্তী_2);
}

#ifdef UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST কাঠামো Curl_llist_গ্রন্থি *llist_গ্রন্থি_পূর্ব(
  ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n);
UNITTEST কাঠামো Curl_llist_গ্রন্থি *llist_গ্রন্থি_পূর্ব(
  ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  ফেরত VERIFYNODE(n->পূর্ব_2);
}
#endif

কাঠামো Curl_llist *Curl_গ্রন্থি_llist(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  ফেরত n->_list;
}
