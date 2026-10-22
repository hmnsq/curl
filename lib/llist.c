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

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय LLISTINIT 0x100cc001 /* random pattern */
#परिभाषय NODEINIT  0x12344321 /* random pattern */
#परिभाषय NODEREM   0x54321012 /* random pattern */

#परिभाषय VERIFYNODE(x) verifynode(x)
स्थिर संरचना Curl_llist_ग्रन्थिः *verifynode(संरचना Curl_llist_ग्रन्थिः *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  प्रत्यागम n;
}
#अन्यथा
#परिभाषय VERIFYNODE(x) x
#यदि_अन्तः
/*
 * @unittest 1300
 */
शून्य Curl_llist_init(संरचना Curl_llist *l, Curl_llist_dtor dtor)
{
  l->आकार_2 = 0;
  l->_dtor = dtor;
  l->शीर्ष_2 = रिक्त;
  l->पुच्छ_2 = रिक्त;
#यदि_परिभाषितम् DEBUGBUILD
  l->_init = LLISTINIT;
#यदि_अन्तः
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
UNITTEST शून्य llist_निवेशय_अग्र(संरचना Curl_llist *list,
                                संरचना Curl_llist_ग्रन्थिः *e, ध्रुव शून्य *p,
                                संरचना Curl_llist_ग्रन्थिः *ne);
UNITTEST शून्य llist_निवेशय_अग्र(संरचना Curl_llist *list,
                                संरचना Curl_llist_ग्रन्थिः *e, /* may be NULL */
                                ध्रुव शून्य *p,
                                संरचना Curl_llist_ग्रन्थिः *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#यदि_परिभाषितम् DEBUGBUILD
  ne->_init = NODEINIT;
#यदि_अन्तः
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  यदि(list->आकार_2 == 0) {
    list->शीर्ष_2 = ne;
    list->शीर्ष_2->पूर्व_2 = रिक्त;
    list->शीर्ष_2->अग्र_2 = रिक्त;
    list->पुच्छ_2 = ne;
  }
  अन्यथा {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->अग्र_2 = e ? e->अग्र_2 : list->शीर्ष_2;
    ne->पूर्व_2 = e;
    यदि(!e) {
      list->शीर्ष_2->पूर्व_2 = ne;
      list->शीर्ष_2 = ne;
    }
    अन्यथा यदि(e->अग्र_2) {
      e->अग्र_2->पूर्व_2 = ne;
    }
    अन्यथा {
      list->पुच्छ_2 = ne;
    }
    यदि(e)
      e->अग्र_2 = ne;
  }

  ++list->आकार_2;
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
शून्य Curl_llist_append(संरचना Curl_llist *list, ध्रुव शून्य *p,
                       संरचना Curl_llist_ग्रन्थिः *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_निवेशय_अग्र(list, list->पुच्छ_2, p, ne);
}

शून्य *Curl_ग्रन्थिः_take_elem(संरचना Curl_llist_ग्रन्थिः *e)
{
  शून्य *ptr;
  संरचना Curl_llist *list;
  यदि(!e)
    प्रत्यागम रिक्त;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->आकार_2);
  DEBUGASSERT(e->_init == NODEINIT);
  यदि(list) {
    यदि(e == list->शीर्ष_2) {
      list->शीर्ष_2 = e->अग्र_2;

      यदि(!list->शीर्ष_2)
        list->पुच्छ_2 = रिक्त;
      अन्यथा
        e->अग्र_2->पूर्व_2 = रिक्त;
    }
    अन्यथा {
      यदि(e->पूर्व_2)
        e->पूर्व_2->अग्र_2 = e->अग्र_2;

      यदि(!e->अग्र_2)
        list->पुच्छ_2 = e->पूर्व_2;
      अन्यथा
        e->अग्र_2->पूर्व_2 = e->पूर्व_2;
    }
    --list->आकार_2;
  }
  ptr = e->_ptr;

  e->_list = रिक्त;
  e->_ptr  = रिक्त;
  e->पूर्व_2 = रिक्त;
  e->अग्र_2 = रिक्त;
#यदि_परिभाषितम् DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#यदि_अन्तः

  प्रत्यागम ptr;
}

स्थिर शून्य ग्रन्थिः_uremove(संरचना Curl_llist_ग्रन्थिः *e, शून्य *user)
{
  संरचना Curl_llist *list;
  शून्य *ptr;
  यदि(!e)
    प्रत्यागम;

  list = e->_list;
  DEBUGASSERT(list);
  यदि(list) {
    ptr = Curl_ग्रन्थिः_take_elem(e);
    यदि(list->_dtor)
      list->_dtor(user, ptr);
  }
}

शून्य Curl_ग्रन्थिः_remove(संरचना Curl_llist_ग्रन्थिः *e)
{
  ग्रन्थिः_uremove(e, रिक्त);
}

शून्य Curl_llist_destroy(संरचना Curl_llist *list, शून्य *user)
{
  यदि(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    यावत्(list->आकार_2 > 0)
      ग्रन्थिः_uremove(list->पुच्छ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
संरचना Curl_llist_ग्रन्थिः *Curl_llist_शीर्ष(ध्रुव संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  प्रत्यागम VERIFYNODE(list->शीर्ष_2);
}

#यदि_परिभाषितम् UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST संरचना Curl_llist_ग्रन्थिः *llist_पुच्छ(ध्रुव संरचना Curl_llist *list);
UNITTEST संरचना Curl_llist_ग्रन्थिः *llist_पुच्छ(ध्रुव संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  प्रत्यागम VERIFYNODE(list->पुच्छ_2);
}
#यदि_अन्तः

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_गणनम्(ध्रुव संरचना Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  प्रत्यागम list->आकार_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
शून्य *Curl_ग्रन्थिः_elem(ध्रुव संरचना Curl_llist_ग्रन्थिः *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  प्रत्यागम n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
संरचना Curl_llist_ग्रन्थिः *Curl_ग्रन्थिः_अग्र(ध्रुव संरचना Curl_llist_ग्रन्थिः *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  प्रत्यागम VERIFYNODE(n->अग्र_2);
}

#यदि_परिभाषितम् UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST संरचना Curl_llist_ग्रन्थिः *llist_ग्रन्थिः_पूर्व(
  ध्रुव संरचना Curl_llist_ग्रन्थिः *n);
UNITTEST संरचना Curl_llist_ग्रन्थिः *llist_ग्रन्थिः_पूर्व(
  ध्रुव संरचना Curl_llist_ग्रन्थिः *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  प्रत्यागम VERIFYNODE(n->पूर्व_2);
}
#यदि_अन्तः

संरचना Curl_llist *Curl_ग्रन्थिः_llist(ध्रुव संरचना Curl_llist_ग्रन्थिः *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  प्रत्यागम n->_list;
}
