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

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو LLISTINIT 0x100cc001 /* random pattern */
#تعریف_کرو NODEINIT  0x12344321 /* random pattern */
#تعریف_کرو NODEREM   0x54321012 /* random pattern */

#تعریف_کرو VERIFYNODE(x) verifynode(x)
ساكن بنية Curl_llist_گرہ *verifynode(بنية Curl_llist_گرہ *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  أرجع n;
}
#وإلا
#تعریف_کرو VERIFYNODE(x) x
#جے_انت
/*
 * @unittest 1300
 */
فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor)
{
  l->ناپ_2 = 0;
  l->_dtor = dtor;
  l->سر_2 = عدم;
  l->پوچھ_2 = عدم;
#جے_تعریف_شدہ DEBUGBUILD
  l->_init = LLISTINIT;
#جے_انت
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
UNITTEST فراغ llist_داخل_کرو_اگلا(بنية Curl_llist *list,
                                بنية Curl_llist_گرہ *e, ثابت فراغ *p,
                                بنية Curl_llist_گرہ *ne);
UNITTEST فراغ llist_داخل_کرو_اگلا(بنية Curl_llist *list,
                                بنية Curl_llist_گرہ *e, /* may be NULL */
                                ثابت فراغ *p,
                                بنية Curl_llist_گرہ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#جے_تعریف_شدہ DEBUGBUILD
  ne->_init = NODEINIT;
#جے_انت
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  إذا(list->ناپ_2 == 0) {
    list->سر_2 = ne;
    list->سر_2->_prev = عدم;
    list->سر_2->اگلا_2 = عدم;
    list->پوچھ_2 = ne;
  }
  وإلا {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->اگلا_2 = e ? e->اگلا_2 : list->سر_2;
    ne->_prev = e;
    إذا(!e) {
      list->سر_2->_prev = ne;
      list->سر_2 = ne;
    }
    وإلا إذا(e->اگلا_2) {
      e->اگلا_2->_prev = ne;
    }
    وإلا {
      list->پوچھ_2 = ne;
    }
    إذا(e)
      e->اگلا_2 = ne;
  }

  ++list->ناپ_2;
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
                       بنية Curl_llist_گرہ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_داخل_کرو_اگلا(list, list->پوچھ_2, p, ne);
}

فراغ *Curl_گرہ_take_elem(بنية Curl_llist_گرہ *e)
{
  فراغ *ptr;
  بنية Curl_llist *list;
  إذا(!e)
    أرجع عدم;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->ناپ_2);
  DEBUGASSERT(e->_init == NODEINIT);
  إذا(list) {
    إذا(e == list->سر_2) {
      list->سر_2 = e->اگلا_2;

      إذا(!list->سر_2)
        list->پوچھ_2 = عدم;
      وإلا
        e->اگلا_2->_prev = عدم;
    }
    وإلا {
      إذا(e->_prev)
        e->_prev->اگلا_2 = e->اگلا_2;

      إذا(!e->اگلا_2)
        list->پوچھ_2 = e->_prev;
      وإلا
        e->اگلا_2->_prev = e->_prev;
    }
    --list->ناپ_2;
  }
  ptr = e->_ptr;

  e->_list = عدم;
  e->_ptr  = عدم;
  e->_prev = عدم;
  e->اگلا_2 = عدم;
#جے_تعریف_شدہ DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#جے_انت

  أرجع ptr;
}

ساكن فراغ گرہ_uremove(بنية Curl_llist_گرہ *e, فراغ *user)
{
  بنية Curl_llist *list;
  فراغ *ptr;
  إذا(!e)
    أرجع;

  list = e->_list;
  DEBUGASSERT(list);
  إذا(list) {
    ptr = Curl_گرہ_take_elem(e);
    إذا(list->_dtor)
      list->_dtor(user, ptr);
  }
}

فراغ Curl_گرہ_remove(بنية Curl_llist_گرہ *e)
{
  گرہ_uremove(e, عدم);
}

فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user)
{
  إذا(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    طالما(list->ناپ_2 > 0)
      گرہ_uremove(list->پوچھ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_گرہ *Curl_llist_سر(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->سر_2);
}

#جے_تعریف_شدہ UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST بنية Curl_llist_گرہ *llist_پوچھ(ثابت بنية Curl_llist *list);
UNITTEST بنية Curl_llist_گرہ *llist_پوچھ(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->پوچھ_2);
}
#جے_انت

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_گنتی(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع list->ناپ_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_گرہ_elem(ثابت بنية Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_گرہ *Curl_گرہ_اگلا(ثابت بنية Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->اگلا_2);
}

#جے_تعریف_شدہ UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST بنية Curl_llist_گرہ *llist_گرہ_prev(
  ثابت بنية Curl_llist_گرہ *n);
UNITTEST بنية Curl_llist_گرہ *llist_گرہ_prev(
  ثابت بنية Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->_prev);
}
#جے_انت

بنية Curl_llist *Curl_گرہ_llist(ثابت بنية Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  أرجع n->_list;
}
