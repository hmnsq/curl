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

#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه LLISTINIT 0x100cc001 /* random pattern */
#تعریف_کړه NODEINIT  0x12344321 /* random pattern */
#تعریف_کړه NODEREM   0x54321012 /* random pattern */

#تعریف_کړه VERIFYNODE(x) verifynode(x)
ساكن بنية Curl_llist_غوټه *verifynode(بنية Curl_llist_غوټه *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  أرجع n;
}
#وإلا
#تعریف_کړه VERIFYNODE(x) x
#د_که_پای
/*
 * @unittest 1300
 */
فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor)
{
  l->اندازه_2 = 0;
  l->_dtor = dtor;
  l->سر_3 = عدم;
  l->لکۍ_2 = عدم;
#که_تعریف_شوی DEBUGBUILD
  l->_init = LLISTINIT;
#د_که_پای
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
UNITTEST فراغ llist_ورننباسه_بل(بنية Curl_llist *list,
                                بنية Curl_llist_غوټه *e, ثابت فراغ *p,
                                بنية Curl_llist_غوټه *ne);
UNITTEST فراغ llist_ورننباسه_بل(بنية Curl_llist *list,
                                بنية Curl_llist_غوټه *e, /* may be NULL */
                                ثابت فراغ *p,
                                بنية Curl_llist_غوټه *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#که_تعریف_شوی DEBUGBUILD
  ne->_init = NODEINIT;
#د_که_پای
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  إذا(list->اندازه_2 == 0) {
    list->سر_3 = ne;
    list->سر_3->_prev = عدم;
    list->سر_3->بل_2 = عدم;
    list->لکۍ_2 = ne;
  }
  وإلا {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->بل_2 = e ? e->بل_2 : list->سر_3;
    ne->_prev = e;
    إذا(!e) {
      list->سر_3->_prev = ne;
      list->سر_3 = ne;
    }
    وإلا إذا(e->بل_2) {
      e->بل_2->_prev = ne;
    }
    وإلا {
      list->لکۍ_2 = ne;
    }
    إذا(e)
      e->بل_2 = ne;
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
                       بنية Curl_llist_غوټه *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_ورننباسه_بل(list, list->لکۍ_2, p, ne);
}

فراغ *Curl_غوټه_take_elem(بنية Curl_llist_غوټه *e)
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
    إذا(e == list->سر_3) {
      list->سر_3 = e->بل_2;

      إذا(!list->سر_3)
        list->لکۍ_2 = عدم;
      وإلا
        e->بل_2->_prev = عدم;
    }
    وإلا {
      إذا(e->_prev)
        e->_prev->بل_2 = e->بل_2;

      إذا(!e->بل_2)
        list->لکۍ_2 = e->_prev;
      وإلا
        e->بل_2->_prev = e->_prev;
    }
    --list->اندازه_2;
  }
  ptr = e->_ptr;

  e->_list = عدم;
  e->_ptr  = عدم;
  e->_prev = عدم;
  e->بل_2 = عدم;
#که_تعریف_شوی DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#د_که_پای

  أرجع ptr;
}

ساكن فراغ غوټه_uremove(بنية Curl_llist_غوټه *e, فراغ *user)
{
  بنية Curl_llist *list;
  فراغ *ptr;
  إذا(!e)
    أرجع;

  list = e->_list;
  DEBUGASSERT(list);
  إذا(list) {
    ptr = Curl_غوټه_take_elem(e);
    إذا(list->_dtor)
      list->_dtor(user, ptr);
  }
}

فراغ Curl_غوټه_remove(بنية Curl_llist_غوټه *e)
{
  غوټه_uremove(e, عدم);
}

فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user)
{
  إذا(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    طالما(list->اندازه_2 > 0)
      غوټه_uremove(list->لکۍ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_غوټه *Curl_llist_سر(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->سر_3);
}

#که_تعریف_شوی UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST بنية Curl_llist_غوټه *llist_لکۍ(ثابت بنية Curl_llist *list);
UNITTEST بنية Curl_llist_غوټه *llist_لکۍ(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع VERIFYNODE(list->لکۍ_2);
}
#د_که_پای

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمېر(ثابت بنية Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  أرجع list->اندازه_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_غوټه_elem(ثابت بنية Curl_llist_غوټه *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_غوټه *Curl_غوټه_بل(ثابت بنية Curl_llist_غوټه *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->بل_2);
}

#که_تعریف_شوی UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST بنية Curl_llist_غوټه *llist_غوټه_prev(
  ثابت بنية Curl_llist_غوټه *n);
UNITTEST بنية Curl_llist_غوټه *llist_غوټه_prev(
  ثابت بنية Curl_llist_غوټه *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  أرجع VERIFYNODE(n->_prev);
}
#د_که_پای

بنية Curl_llist *Curl_غوټه_llist(ثابت بنية Curl_llist_غوټه *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  أرجع n->_list;
}
