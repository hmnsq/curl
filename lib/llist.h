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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*Curl_llist_dtor)(ಶೂನ್ಯ *user, ಶೂನ್ಯ *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ಸಂರಚನಾ Curl_llist {
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *_head;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ _init;      /* detect API usage mistakes */
#endif
};

ಸಂರಚನಾ Curl_llist_ಗಂಟು {
  ಸಂರಚನಾ Curl_llist *_list; /* the list where this belongs */
  ಶೂನ್ಯ *_ptr;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *_prev;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *_next;
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ _init;      /* detect API usage mistakes */
#endif
};

ಶೂನ್ಯ Curl_llist_init(ಸಂರಚನಾ Curl_llist *l, Curl_llist_dtor dtor);
ಶೂನ್ಯ Curl_llist_append(ಸಂರಚನಾ Curl_llist *list, ಧ್ರುವ ಶೂನ್ಯ *p,
                       ಸಂರಚನಾ Curl_llist_ಗಂಟು *ne);
ಶೂನ್ಯ Curl_ಗಂಟು_remove(ಸಂರಚನಾ Curl_llist_ಗಂಟು *e);
ಶೂನ್ಯ Curl_llist_destroy(ಸಂರಚನಾ Curl_llist *list, ಶೂನ್ಯ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ಸಂರಚನಾ Curl_llist_ಗಂಟು *Curl_llist_head(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ಎಣಿಕೆ(ಧ್ರುವ ಸಂರಚನಾ Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ಶೂನ್ಯ *Curl_ಗಂಟು_elem(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
ಶೂನ್ಯ *Curl_ಗಂಟು_take_elem(ಸಂರಚನಾ Curl_llist_ಗಂಟು *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ಸಂರಚನಾ Curl_llist_ಗಂಟು *Curl_ಗಂಟು_next(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ಸಂರಚನಾ Curl_llist *Curl_ಗಂಟು_llist(ಧ್ರುವ ಸಂರಚನಾ Curl_llist_ಗಂಟು *n);

#endif /* HEADER_CURL_LLIST_H */
