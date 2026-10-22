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
#অন্তর্ভাৱ "curl_setup.h"

প্রকারনির্দেশ শূন্য (*Curl_llist_dtor)(শূন্য *user, শূন্য *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

সংরচনা Curl_llist {
  সংরচনা Curl_llist_গ্ৰন্থি *_head;
  সংরচনা Curl_llist_গ্ৰন্থি *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  পূর্ণাঙ্ক _init;      /* detect API usage mistakes */
#endif
};

সংরচনা Curl_llist_গ্ৰন্থি {
  সংরচনা Curl_llist *_list; /* the list where this belongs */
  শূন্য *_ptr;
  সংরচনা Curl_llist_গ্ৰন্থি *_prev;
  সংরচনা Curl_llist_গ্ৰন্থি *_next;
#ifdef DEBUGBUILD
  পূর্ণাঙ্ক _init;      /* detect API usage mistakes */
#endif
};

শূন্য Curl_llist_init(সংরচনা Curl_llist *l, Curl_llist_dtor dtor);
শূন্য Curl_llist_append(সংরচনা Curl_llist *list, ধ্রুৱ শূন্য *p,
                       সংরচনা Curl_llist_গ্ৰন্থি *ne);
শূন্য Curl_গ্ৰন্থি_remove(সংরচনা Curl_llist_গ্ৰন্থি *e);
শূন্য Curl_llist_destroy(সংরচনা Curl_llist *list, শূন্য *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
সংরচনা Curl_llist_গ্ৰন্থি *Curl_llist_head(ধ্রুৱ সংরচনা Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_গুনতি(ধ্রুৱ সংরচনা Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
শূন্য *Curl_গ্ৰন্থি_elem(ধ্রুৱ সংরচনা Curl_llist_গ্ৰন্থি *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
শূন্য *Curl_গ্ৰন্থি_take_elem(সংরচনা Curl_llist_গ্ৰন্থি *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
সংরচনা Curl_llist_গ্ৰন্থি *Curl_গ্ৰন্থি_next(ধ্রুৱ সংরচনা Curl_llist_গ্ৰন্থি *n);

/* Curl_node_llist() return the list the node is in or NULL. */
সংরচনা Curl_llist *Curl_গ্ৰন্থি_llist(ধ্রুৱ সংরচনা Curl_llist_গ্ৰন্থি *n);

#endif /* HEADER_CURL_LLIST_H */
