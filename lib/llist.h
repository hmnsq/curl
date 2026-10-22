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
#شامل "curl_setup.h"

تعریفنوع تهی (*Curl_llist_dtor)(تهی *user, تهی *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ساختار Curl_llist {
  ساختار Curl_llist_گره *_head;
  ساختار Curl_llist_گره *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  صحیح _init;      /* detect API usage mistakes */
#endif
};

ساختار Curl_llist_گره {
  ساختار Curl_llist *_list; /* the list where this belongs */
  تهی *_ptr;
  ساختار Curl_llist_گره *_prev;
  ساختار Curl_llist_گره *_next;
#ifdef DEBUGBUILD
  صحیح _init;      /* detect API usage mistakes */
#endif
};

تهی Curl_llist_init(ساختار Curl_llist *l, Curl_llist_dtor dtor);
تهی Curl_llist_append(ساختار Curl_llist *list, ثابت تهی *p,
                       ساختار Curl_llist_گره *ne);
تهی Curl_گره_remove(ساختار Curl_llist_گره *e);
تهی Curl_llist_destroy(ساختار Curl_llist *list, تهی *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ساختار Curl_llist_گره *Curl_llist_head(ثابت ساختار Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمار(ثابت ساختار Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
تهی *Curl_گره_elem(ثابت ساختار Curl_llist_گره *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
تهی *Curl_گره_take_elem(ساختار Curl_llist_گره *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ساختار Curl_llist_گره *Curl_گره_next(ثابت ساختار Curl_llist_گره *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ساختار Curl_llist *Curl_گره_llist(ثابت ساختار Curl_llist_گره *n);

#endif /* HEADER_CURL_LLIST_H */
