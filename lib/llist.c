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
#समावेश "curl_setup.h"

#समावेश "llist.h"

#अगर_घोषित DEBUGBUILD
#घोषणा LLISTINIT 0x100cc001 /* random pattern */
#घोषणा NODEINIT  0x12344321 /* random pattern */
#घोषणा NODEREM   0x54321012 /* random pattern */

#घोषणा VERIFYNODE(x) verifynode(x)
अटल काष्ठा Curl_llist_गाँठ *verifynode(काष्ठा Curl_llist_गाँठ *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  वापस n;
}
#अन्यथा
#घोषणा VERIFYNODE(x) x
#पूर्ण_अगर
/*
 * @unittest 1300
 */
व्योम Curl_llist_init(काष्ठा Curl_llist *l, Curl_llist_dtor dtor)
{
  l->आकार_2 = 0;
  l->_dtor = dtor;
  l->शीर्ष_3 = शून्य;
  l->पुच्छ_2 = शून्य;
#अगर_घोषित DEBUGBUILD
  l->_init = LLISTINIT;
#पूर्ण_अगर
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
UNITTEST व्योम llist_जोड़ो_अग्र(काष्ठा Curl_llist *list,
                                काष्ठा Curl_llist_गाँठ *e, स्थिर व्योम *p,
                                काष्ठा Curl_llist_गाँठ *ne);
UNITTEST व्योम llist_जोड़ो_अग्र(काष्ठा Curl_llist *list,
                                काष्ठा Curl_llist_गाँठ *e, /* may be NULL */
                                स्थिर व्योम *p,
                                काष्ठा Curl_llist_गाँठ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#अगर_घोषित DEBUGBUILD
  ne->_init = NODEINIT;
#पूर्ण_अगर
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  अगर(list->आकार_2 == 0) {
    list->शीर्ष_3 = ne;
    list->शीर्ष_3->पूर्व_2 = शून्य;
    list->शीर्ष_3->अग्र_2 = शून्य;
    list->पुच्छ_2 = ne;
  }
  अन्यथा {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->अग्र_2 = e ? e->अग्र_2 : list->शीर्ष_3;
    ne->पूर्व_2 = e;
    अगर(!e) {
      list->शीर्ष_3->पूर्व_2 = ne;
      list->शीर्ष_3 = ne;
    }
    अन्यथा अगर(e->अग्र_2) {
      e->अग्र_2->पूर्व_2 = ne;
    }
    अन्यथा {
      list->पुच्छ_2 = ne;
    }
    अगर(e)
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
व्योम Curl_llist_append(काष्ठा Curl_llist *list, स्थिर व्योम *p,
                       काष्ठा Curl_llist_गाँठ *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_जोड़ो_अग्र(list, list->पुच्छ_2, p, ne);
}

व्योम *Curl_गाँठ_take_elem(काष्ठा Curl_llist_गाँठ *e)
{
  व्योम *ptr;
  काष्ठा Curl_llist *list;
  अगर(!e)
    वापस शून्य;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->आकार_2);
  DEBUGASSERT(e->_init == NODEINIT);
  अगर(list) {
    अगर(e == list->शीर्ष_3) {
      list->शीर्ष_3 = e->अग्र_2;

      अगर(!list->शीर्ष_3)
        list->पुच्छ_2 = शून्य;
      अन्यथा
        e->अग्र_2->पूर्व_2 = शून्य;
    }
    अन्यथा {
      अगर(e->पूर्व_2)
        e->पूर्व_2->अग्र_2 = e->अग्र_2;

      अगर(!e->अग्र_2)
        list->पुच्छ_2 = e->पूर्व_2;
      अन्यथा
        e->अग्र_2->पूर्व_2 = e->पूर्व_2;
    }
    --list->आकार_2;
  }
  ptr = e->_ptr;

  e->_list = शून्य;
  e->_ptr  = शून्य;
  e->पूर्व_2 = शून्य;
  e->अग्र_2 = शून्य;
#अगर_घोषित DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#पूर्ण_अगर

  वापस ptr;
}

अटल व्योम गाँठ_uremove(काष्ठा Curl_llist_गाँठ *e, व्योम *user)
{
  काष्ठा Curl_llist *list;
  व्योम *ptr;
  अगर(!e)
    वापस;

  list = e->_list;
  DEBUGASSERT(list);
  अगर(list) {
    ptr = Curl_गाँठ_take_elem(e);
    अगर(list->_dtor)
      list->_dtor(user, ptr);
  }
}

व्योम Curl_गाँठ_remove(काष्ठा Curl_llist_गाँठ *e)
{
  गाँठ_uremove(e, शून्य);
}

व्योम Curl_llist_destroy(काष्ठा Curl_llist *list, व्योम *user)
{
  अगर(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    जबतक(list->आकार_2 > 0)
      गाँठ_uremove(list->पुच्छ_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
काष्ठा Curl_llist_गाँठ *Curl_llist_शीर्ष(स्थिर काष्ठा Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  वापस VERIFYNODE(list->शीर्ष_3);
}

#अगर_घोषित UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST काष्ठा Curl_llist_गाँठ *llist_पुच्छ(स्थिर काष्ठा Curl_llist *list);
UNITTEST काष्ठा Curl_llist_गाँठ *llist_पुच्छ(स्थिर काष्ठा Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  वापस VERIFYNODE(list->पुच्छ_2);
}
#पूर्ण_अगर

/* Curl_llist_count() returns a size_t the number of nodes in the list */
माप_प्रकार Curl_llist_गिनती(स्थिर काष्ठा Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  वापस list->आकार_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
व्योम *Curl_गाँठ_elem(स्थिर काष्ठा Curl_llist_गाँठ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  वापस n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
काष्ठा Curl_llist_गाँठ *Curl_गाँठ_अग्र(स्थिर काष्ठा Curl_llist_गाँठ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  वापस VERIFYNODE(n->अग्र_2);
}

#अगर_घोषित UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST काष्ठा Curl_llist_गाँठ *llist_गाँठ_पूर्व(
  स्थिर काष्ठा Curl_llist_गाँठ *n);
UNITTEST काष्ठा Curl_llist_गाँठ *llist_गाँठ_पूर्व(
  स्थिर काष्ठा Curl_llist_गाँठ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  वापस VERIFYNODE(n->पूर्व_2);
}
#पूर्ण_अगर

काष्ठा Curl_llist *Curl_गाँठ_llist(स्थिर काष्ठा Curl_llist_गाँठ *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  वापस n->_list;
}
