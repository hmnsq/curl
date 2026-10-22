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
#شامل "curl_setup.h"

#شامل "llist.h"

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو LLISTINIT 0x100cc001 /* random pattern */
#تعریف_کرو NODEINIT  0x12344321 /* random pattern */
#تعریف_کرو NODEREM   0x54321012 /* random pattern */

#تعریف_کرو VERIFYNODE(x) verifynode(x)
ساکن ساخت Curl_llist_گرہ *verifynode(ساخت Curl_llist_گرہ *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  واپس n;
}
#ورنہ
#تعریف_کرو VERIFYNODE(x) x
#اگر_ختم
/*
 * @unittest 1300
 */
خالی Curl_llist_init(ساخت Curl_llist *l, Curl_llist_dtor dtor)
{
  l->سائز_2 = 0;
  l->_dtor = dtor;
  l->سر_2 = تہی;
  l->دم_2 = تہی;
#اگر_تعریف_شدہ DEBUGBUILD
  l->_init = LLISTINIT;
#اگر_ختم
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
UNITTEST خالی llist_داخل_کرو_اگلا(ساخت Curl_llist *list,
                                ساخت Curl_llist_گرہ *e, ثابت خالی *p,
                                ساخت Curl_llist_گرہ *ne);
UNITTEST خالی llist_داخل_کرو_اگلا(ساخت Curl_llist *list,
                                ساخت Curl_llist_گرہ *e, /* may be NULL */
                                ثابت خالی *p,
                                ساخت Curl_llist_گرہ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#اگر_تعریف_شدہ DEBUGBUILD
  ne->_init = NODEINIT;
#اگر_ختم
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  اگر(list->سائز_2 == 0) {
    list->سر_2 = ne;
    list->سر_2->پچھلا_2 = تہی;
    list->سر_2->اگلا_2 = تہی;
    list->دم_2 = ne;
  }
  ورنہ {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->اگلا_2 = e ? e->اگلا_2 : list->سر_2;
    ne->پچھلا_2 = e;
    اگر(!e) {
      list->سر_2->پچھلا_2 = ne;
      list->سر_2 = ne;
    }
    ورنہ اگر(e->اگلا_2) {
      e->اگلا_2->پچھلا_2 = ne;
    }
    ورنہ {
      list->دم_2 = ne;
    }
    اگر(e)
      e->اگلا_2 = ne;
  }

  ++list->سائز_2;
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
خالی Curl_llist_append(ساخت Curl_llist *list, ثابت خالی *p,
                       ساخت Curl_llist_گرہ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_داخل_کرو_اگلا(list, list->دم_2, p, ne);
}

خالی *Curl_گرہ_take_elem(ساخت Curl_llist_گرہ *e)
{
  خالی *ptr;
  ساخت Curl_llist *list;
  اگر(!e)
    واپس تہی;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->سائز_2);
  DEBUGASSERT(e->_init == NODEINIT);
  اگر(list) {
    اگر(e == list->سر_2) {
      list->سر_2 = e->اگلا_2;

      اگر(!list->سر_2)
        list->دم_2 = تہی;
      ورنہ
        e->اگلا_2->پچھلا_2 = تہی;
    }
    ورنہ {
      اگر(e->پچھلا_2)
        e->پچھلا_2->اگلا_2 = e->اگلا_2;

      اگر(!e->اگلا_2)
        list->دم_2 = e->پچھلا_2;
      ورنہ
        e->اگلا_2->پچھلا_2 = e->پچھلا_2;
    }
    --list->سائز_2;
  }
  ptr = e->_ptr;

  e->_list = تہی;
  e->_ptr  = تہی;
  e->پچھلا_2 = تہی;
  e->اگلا_2 = تہی;
#اگر_تعریف_شدہ DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#اگر_ختم

  واپس ptr;
}

ساکن خالی گرہ_uremove(ساخت Curl_llist_گرہ *e, خالی *user)
{
  ساخت Curl_llist *list;
  خالی *ptr;
  اگر(!e)
    واپس;

  list = e->_list;
  DEBUGASSERT(list);
  اگر(list) {
    ptr = Curl_گرہ_take_elem(e);
    اگر(list->_dtor)
      list->_dtor(user, ptr);
  }
}

خالی Curl_گرہ_remove(ساخت Curl_llist_گرہ *e)
{
  گرہ_uremove(e, تہی);
}

خالی Curl_llist_destroy(ساخت Curl_llist *list, خالی *user)
{
  اگر(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    جب(list->سائز_2 > 0)
      گرہ_uremove(list->دم_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ساخت Curl_llist_گرہ *Curl_llist_سر(ثابت ساخت Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  واپس VERIFYNODE(list->سر_2);
}

#اگر_تعریف_شدہ UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST ساخت Curl_llist_گرہ *llist_دم(ثابت ساخت Curl_llist *list);
UNITTEST ساخت Curl_llist_گرہ *llist_دم(ثابت ساخت Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  واپس VERIFYNODE(list->دم_2);
}
#اگر_ختم

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_گنتی(ثابت ساخت Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  واپس list->سائز_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
خالی *Curl_گرہ_elem(ثابت ساخت Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  واپس n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ساخت Curl_llist_گرہ *Curl_گرہ_اگلا(ثابت ساخت Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  واپس VERIFYNODE(n->اگلا_2);
}

#اگر_تعریف_شدہ UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST ساخت Curl_llist_گرہ *llist_گرہ_پچھلا(
  ثابت ساخت Curl_llist_گرہ *n);
UNITTEST ساخت Curl_llist_گرہ *llist_گرہ_پچھلا(
  ثابت ساخت Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  واپس VERIFYNODE(n->پچھلا_2);
}
#اگر_ختم

ساخت Curl_llist *Curl_گرہ_llist(ثابت ساخت Curl_llist_گرہ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  واپس n->_list;
}
