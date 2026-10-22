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
#অন্তর্ভাব "curl_setup.h"

প্রকারলেখ শূন্য (*Curl_llist_dtor)(শূন্য *user, শূন্য *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

কাঠামো Curl_llist {
  কাঠামো Curl_llist_গ্রন্থি *শীর্ষ_3;
  কাঠামো Curl_llist_গ্রন্থি *লেজ_2;
  Curl_llist_dtor _dtor;
  size_t আকার_3;
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা _init;      /* detect API usage mistakes */
#endif
};

কাঠামো Curl_llist_গ্রন্থি {
  কাঠামো Curl_llist *_list; /* the list where this belongs */
  শূন্য *_ptr;
  কাঠামো Curl_llist_গ্রন্থি *পূর্ব_2;
  কাঠামো Curl_llist_গ্রন্থি *পরবর্তী_2;
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা _init;      /* detect API usage mistakes */
#endif
};

শূন্য Curl_llist_init(কাঠামো Curl_llist *l, Curl_llist_dtor dtor);
শূন্য Curl_llist_append(কাঠামো Curl_llist *list, ধ্রুবক শূন্য *p,
                       কাঠামো Curl_llist_গ্রন্থি *ne);
শূন্য Curl_গ্রন্থি_remove(কাঠামো Curl_llist_গ্রন্থি *e);
শূন্য Curl_llist_destroy(কাঠামো Curl_llist *list, শূন্য *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
কাঠামো Curl_llist_গ্রন্থি *Curl_llist_শীর্ষ(ধ্রুবক কাঠামো Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_গুনতি(ধ্রুবক কাঠামো Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
শূন্য *Curl_গ্রন্থি_elem(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
শূন্য *Curl_গ্রন্থি_take_elem(কাঠামো Curl_llist_গ্রন্থি *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
কাঠামো Curl_llist_গ্রন্থি *Curl_গ্রন্থি_পরবর্তী(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n);

/* Curl_node_llist() return the list the node is in or NULL. */
কাঠামো Curl_llist *Curl_গ্রন্থি_llist(ধ্রুবক কাঠামো Curl_llist_গ্রন্থি *n);

#endif /* HEADER_CURL_LLIST_H */
