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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "llist.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు LLISTINIT 0x100cc001 /* random pattern */
#నిర్వచించు NODEINIT  0x12344321 /* random pattern */
#నిర్వచించు NODEREM   0x54321012 /* random pattern */

#నిర్వచించు VERIFYNODE(x) verifynode(x)
స్థిర నిర్మాణం Curl_llist_కణుపు *verifynode(నిర్మాణం Curl_llist_కణుపు *n)
{
  DEBUGASSERT(!n || (n->_init == NODEINIT));
  తిరిగి n;
}
#లేకపోతే
#నిర్వచించు VERIFYNODE(x) x
#అయితే_ముగింపు
/*
 * @unittest 1300
 */
శూన్య Curl_llist_init(నిర్మాణం Curl_llist *l, Curl_llist_dtor dtor)
{
  l->కొలత_2 = 0;
  l->_dtor = dtor;
  l->తల_2 = ఖాళీ;
  l->తోక_2 = ఖాళీ;
#నిర్వచితమైతే DEBUGBUILD
  l->_init = LLISTINIT;
#అయితే_ముగింపు
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
UNITTEST శూన్య llist_చొప్పించు_తదుపరి(నిర్మాణం Curl_llist *list,
                                నిర్మాణం Curl_llist_కణుపు *e, స్థిరరాశి శూన్య *p,
                                నిర్మాణం Curl_llist_కణుపు *ne);
UNITTEST శూన్య llist_చొప్పించు_తదుపరి(నిర్మాణం Curl_llist *list,
                                నిర్మాణం Curl_llist_కణుపు *e, /* may be NULL */
                                స్థిరరాశి శూన్య *p,
                                నిర్మాణం Curl_llist_కణుపు *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);

#నిర్వచితమైతే DEBUGBUILD
  ne->_init = NODEINIT;
#అయితే_ముగింపు
  ne->_ptr = CURL_UNCONST(p);
  ne->_list = list;
  ఒకవేళ(list->కొలత_2 == 0) {
    list->తల_2 = ne;
    list->తల_2->_prev = ఖాళీ;
    list->తల_2->తదుపరి_2 = ఖాళీ;
    list->తోక_2 = ne;
  }
  లేకపోతే {
    /* if 'e' is NULL here, we insert the new element first in the list */
    ne->తదుపరి_2 = e ? e->తదుపరి_2 : list->తల_2;
    ne->_prev = e;
    ఒకవేళ(!e) {
      list->తల_2->_prev = ne;
      list->తల_2 = ne;
    }
    లేకపోతే ఒకవేళ(e->తదుపరి_2) {
      e->తదుపరి_2->_prev = ne;
    }
    లేకపోతే {
      list->తోక_2 = ne;
    }
    ఒకవేళ(e)
      e->తదుపరి_2 = ne;
  }

  ++list->కొలత_2;
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
శూన్య Curl_llist_append(నిర్మాణం Curl_llist *list, స్థిరరాశి శూన్య *p,
                       నిర్మాణం Curl_llist_కణుపు *ne)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(ne);
  llist_చొప్పించు_తదుపరి(list, list->తోక_2, p, ne);
}

శూన్య *Curl_కణుపు_take_elem(నిర్మాణం Curl_llist_కణుపు *e)
{
  శూన్య *ptr;
  నిర్మాణం Curl_llist *list;
  ఒకవేళ(!e)
    తిరిగి ఖాళీ;

  list = e->_list;
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  DEBUGASSERT(list->కొలత_2);
  DEBUGASSERT(e->_init == NODEINIT);
  ఒకవేళ(list) {
    ఒకవేళ(e == list->తల_2) {
      list->తల_2 = e->తదుపరి_2;

      ఒకవేళ(!list->తల_2)
        list->తోక_2 = ఖాళీ;
      లేకపోతే
        e->తదుపరి_2->_prev = ఖాళీ;
    }
    లేకపోతే {
      ఒకవేళ(e->_prev)
        e->_prev->తదుపరి_2 = e->తదుపరి_2;

      ఒకవేళ(!e->తదుపరి_2)
        list->తోక_2 = e->_prev;
      లేకపోతే
        e->తదుపరి_2->_prev = e->_prev;
    }
    --list->కొలత_2;
  }
  ptr = e->_ptr;

  e->_list = ఖాళీ;
  e->_ptr  = ఖాళీ;
  e->_prev = ఖాళీ;
  e->తదుపరి_2 = ఖాళీ;
#నిర్వచితమైతే DEBUGBUILD
  e->_init = NODEREM; /* specific pattern on remove - not zero */
#అయితే_ముగింపు

  తిరిగి ptr;
}

స్థిర శూన్య కణుపు_uremove(నిర్మాణం Curl_llist_కణుపు *e, శూన్య *user)
{
  నిర్మాణం Curl_llist *list;
  శూన్య *ptr;
  ఒకవేళ(!e)
    తిరిగి;

  list = e->_list;
  DEBUGASSERT(list);
  ఒకవేళ(list) {
    ptr = Curl_కణుపు_take_elem(e);
    ఒకవేళ(list->_dtor)
      list->_dtor(user, ptr);
  }
}

శూన్య Curl_కణుపు_remove(నిర్మాణం Curl_llist_కణుపు *e)
{
  కణుపు_uremove(e, ఖాళీ);
}

శూన్య Curl_llist_destroy(నిర్మాణం Curl_llist *list, శూన్య *user)
{
  ఒకవేళ(list) {
    DEBUGASSERT(list->_init == LLISTINIT);
    అయితే(list->కొలత_2 > 0)
      కణుపు_uremove(list->తోక_2, user);
  }
}

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
నిర్మాణం Curl_llist_కణుపు *Curl_llist_తల(స్థిరరాశి నిర్మాణం Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  తిరిగి VERIFYNODE(list->తల_2);
}

#నిర్వచితమైతే UNITTESTS
/* llist_tail() returns the last 'struct Curl_llist_node *', which might be
   NULL

   @unittest 1300 */
UNITTEST నిర్మాణం Curl_llist_కణుపు *llist_తోక(స్థిరరాశి నిర్మాణం Curl_llist *list);
UNITTEST నిర్మాణం Curl_llist_కణుపు *llist_తోక(స్థిరరాశి నిర్మాణం Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  తిరిగి VERIFYNODE(list->తోక_2);
}
#అయితే_ముగింపు

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_లెక్క(స్థిరరాశి నిర్మాణం Curl_llist *list)
{
  DEBUGASSERT(list);
  DEBUGASSERT(list->_init == LLISTINIT);
  తిరిగి list->కొలత_2;
}

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
శూన్య *Curl_కణుపు_elem(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  తిరిగి n->_ptr;
}

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
నిర్మాణం Curl_llist_కణుపు *Curl_కణుపు_తదుపరి(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  తిరిగి VERIFYNODE(n->తదుపరి_2);
}

#నిర్వచితమైతే UNITTESTS
/* llist_node_prev() returns the previous element in a list from a given
   Curl_llist_node

   @unittest 1300 */
UNITTEST నిర్మాణం Curl_llist_కణుపు *llist_కణుపు_prev(
  స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n);
UNITTEST నిర్మాణం Curl_llist_కణుపు *llist_కణుపు_prev(
  స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(n->_init == NODEINIT);
  తిరిగి VERIFYNODE(n->_prev);
}
#అయితే_ముగింపు

నిర్మాణం Curl_llist *Curl_కణుపు_llist(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n)
{
  DEBUGASSERT(n);
  DEBUGASSERT(!n->_list || n->_init == NODEINIT);
  తిరిగి n->_list;
}
