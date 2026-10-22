#ifndef HEADER_CURL_LLIST_H
#define HEADER_CURL_LLIST_H
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
#அந்தர்பாவ "curl_setup.h"

வகைவரை வெறுமை (*Curl_llist_dtor)(வெறுமை *user, வெறுமை *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

கட்டமைப்பு Curl_llist {
  கட்டமைப்பு Curl_llist_முனை *_head;
  கட்டமைப்பு Curl_llist_முனை *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  முழு _init;      /* detect API usage mistakes */
#endif
};

கட்டமைப்பு Curl_llist_முனை {
  கட்டமைப்பு Curl_llist *_list; /* the list where this belongs */
  வெறுமை *_ptr;
  கட்டமைப்பு Curl_llist_முனை *_prev;
  கட்டமைப்பு Curl_llist_முனை *_next;
#ifdef DEBUGBUILD
  முழு _init;      /* detect API usage mistakes */
#endif
};

வெறுமை Curl_llist_init(கட்டமைப்பு Curl_llist *l, Curl_llist_dtor dtor);
வெறுமை Curl_llist_append(கட்டமைப்பு Curl_llist *list, மாறிலி வெறுமை *p,
                       கட்டமைப்பு Curl_llist_முனை *ne);
வெறுமை Curl_முனை_remove(கட்டமைப்பு Curl_llist_முனை *e);
வெறுமை Curl_llist_destroy(கட்டமைப்பு Curl_llist *list, வெறுமை *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
கட்டமைப்பு Curl_llist_முனை *Curl_llist_head(மாறிலி கட்டமைப்பு Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_மொத்தம்(மாறிலி கட்டமைப்பு Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
வெறுமை *Curl_முனை_elem(மாறிலி கட்டமைப்பு Curl_llist_முனை *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
வெறுமை *Curl_முனை_take_elem(கட்டமைப்பு Curl_llist_முனை *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
கட்டமைப்பு Curl_llist_முனை *Curl_முனை_next(மாறிலி கட்டமைப்பு Curl_llist_முனை *n);

/* Curl_node_llist() return the list the node is in or NULL. */
கட்டமைப்பு Curl_llist *Curl_முனை_llist(மாறிலி கட்டமைப்பு Curl_llist_முனை *n);

#endif /* HEADER_CURL_LLIST_H */
