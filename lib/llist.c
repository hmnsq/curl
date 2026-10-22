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

#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن LLISTINIT 0x100cc001 /* random pattern */
#تعریف_کن NODEINIT  0x12344321 /* random pattern */
#تعریف_کن NODEREM   0x54321012 /* random pattern */

#تعریف_کن VERIFYNODE(x) verifynode(x)
ساكن بنية Curl_llist_گره *verifynode(بنية Curl_llist_گره *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  أرجع n;
}
#وإلا
#تعریف_کن VERIFYNODE(x) x
#پایان_اگر
/*
 * @unittest 1300
 */
فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor)
{
  l->اندازه_2 = 0;
  l->_dtor = dtor;
  l->سر_2 = عدم;
  l->دم_2 = عدم;
#اگر_تعریف_شده DEBUGBUILD
  l->_init = LLISTINIT;
#پایان_اگر
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
UNITTEST فراغ llist_درج_بعدی(بنية Curl_llist *list,
                                بنية Curl_llist_گره *e, ثابت فراغ *p,
                                بنية Curl_llist_گره *ne);
UNITTEST فراغ llist_درج_بعدی(بنية Curl_llist *list,
                                بنية Curl_llist_گره *e, /* may be NULL */
                                ثابت فراغ *p,
                                بنية Curl_llist_گره *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#اگر_تعریف_شده DEBUGBUILD
  ne->_init = NODEINIT;
#پایان_اگر
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  إذا(list->اندازه_2 == 0) {
    list->سر_2 = ne;
    list->سر_2->_prev = عدم;
    list->سر_2->بعدی_2 = عدم;
    list->دم_2 = ne;
  }
  وإلا {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->بعدی_2 = e ? e->بعدی_2 : list->سر_2;
    ne->_prev = e;
    إذا(!e) {
      list->سر_2->_prev = ne;
      list->سر_2 = ne;
    }
    وإلا إذا(e->بعدی_2) {
      e->بعدی_2->_prev = ne;
    }
    وإلا {
      list->دم_2 = ne;
    }
    إذا(e)
      e->بعدی_2 = ne;
  }

  ++list->اندازه_2;
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
                       بنية Curl_llist_گره *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_درج_بعدی(list, list->دم_2, p, ne);
}

فراغ *Curl_گره_take_elem(بنية Curl_llist_گره *e)
{
  فراغ *ptr;
  بنية Curl_llist *list;
  إذا(!e)
    أرجع عدم;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->اندازه_2);
  DEBUGASSERT(e->_init == NODEINIT);
  إذا(list) {
    إذا(e == list->سر_2) {
      list->سر_2 = e->بعدی_2;

      إذا(!list->سر_2)
        list->دم_2 = عدم;
      وإلا
        e->بعدی_2->_prev = عدم;
    }
    وإلا {
      إذا(e->_prev)
        e->_prev->بعدی_2 = e->بعدی_2;

      إذا(!e->بعدی_2)
        list->دم_2 = e->_prev;
      وإلا
        e->بعدی_2->_prev = e->_prev;
    }
    --list->اندازه_2;
  }
  ptr = e->_ptr;

  e->_list = عدم;
  e->_ptr  = عدم;
  e->_prev = عدم;
  e->بعدی_2 = عدم;
#اگر_تعریف_شده DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#پایان_اگر

  أرجع ptr;
}

ساكن فراغ گره_uremove(بنية Curl_llist_گره *e, فراغ *user)
{
  بنية Curl_llist *list;
  فراغ *ptr;
  إذا(!e)
    أرجع;

  list = e->_list;
  DEBUGASSERT(list);
  إذا(list) {
    ptr = Curl_گره_take_elem(e);
    إذا(list->_dtor)
      list->_dtor(user, ptr);
  }
}

فراغ Curl_گره_remove(بنية Curl_llist_گره *e)
{
  گره_uremove(e, عدم);
}

فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user)
{
  إذا(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    طالما(list->اندازه_2 > 0)
      گره_uremove(list->دم_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_گره *Curl_llist_سر(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->سر_2);
}

#اگر_تعریف_شده UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST بنية Curl_llist_گره *llist_دم(ثابت بنية Curl_llist *list);
UNITTEST بنية Curl_llist_گره *llist_دم(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->دم_2);
}
#پایان_اگر

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمار(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع list->اندازه_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_گره_elem(ثابت بنية Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_گره *Curl_گره_بعدی(ثابت بنية Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->بعدی_2);
}

#اگر_تعریف_شده UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST بنية Curl_llist_گره *llist_گره_prev(
  ثابت بنية Curl_llist_گره *n);
UNITTEST بنية Curl_llist_گره *llist_گره_prev(
  ثابت بنية Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->_prev);
}
#پایان_اگر

بنية Curl_llist *Curl_گره_llist(ثابت بنية Curl_llist_گره *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  أرجع n->_list;
}
