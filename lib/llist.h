#నిర్వచితం_కాకపోతే శీర్షిక_CURL_LLIST_H
#నిర్వచించు శీర్షిక_CURL_LLIST_H
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

రకనిర్వచనం శూన్య (*Curl_llist_dtor)(శూన్య *user, శూన్య *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

నిర్మాణం Curl_llist {
  నిర్మాణం Curl_llist_కణుపు *తల_2;
  నిర్మాణం Curl_llist_కణుపు *తోక_2;
  Curl_llist_dtor _dtor;
  size_t కొలత_2;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం _init;      /* detect API usage mistakes */
#అయితే_ముగింపు
};

నిర్మాణం Curl_llist_కణుపు {
  నిర్మాణం Curl_llist *_list; /* the list where this belongs */
  శూన్య *_ptr;
  నిర్మాణం Curl_llist_కణుపు *_prev;
  నిర్మాణం Curl_llist_కణుపు *తదుపరి_2;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం _init;      /* detect API usage mistakes */
#అయితే_ముగింపు
};

శూన్య Curl_llist_init(నిర్మాణం Curl_llist *l, Curl_llist_dtor dtor);
శూన్య Curl_llist_append(నిర్మాణం Curl_llist *list, స్థిరరాశి శూన్య *p,
                       నిర్మాణం Curl_llist_కణుపు *ne);
శూన్య Curl_కణుపు_remove(నిర్మాణం Curl_llist_కణుపు *e);
శూన్య Curl_llist_destroy(నిర్మాణం Curl_llist *list, శూన్య *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
నిర్మాణం Curl_llist_కణుపు *Curl_llist_తల(స్థిరరాశి నిర్మాణం Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_లెక్క(స్థిరరాశి నిర్మాణం Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
శూన్య *Curl_కణుపు_elem(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
శూన్య *Curl_కణుపు_take_elem(నిర్మాణం Curl_llist_కణుపు *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
నిర్మాణం Curl_llist_కణుపు *Curl_కణుపు_తదుపరి(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n);

/* Curl_node_llist() return the list the node is in or NULL. */
నిర్మాణం Curl_llist *Curl_కణుపు_llist(స్థిరరాశి నిర్మాణం Curl_llist_కణుపు *n);

#అయితే_ముగింపు /* HEADER_CURL_LLIST_H */
