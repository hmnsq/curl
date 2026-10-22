#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_LLIST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_LLIST_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Curl_llist_dtor)(ਖਾਲੀ *user, ਖਾਲੀ *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ਬਣਤਰ Curl_llist {
  ਬਣਤਰ Curl_llist_ਗੰਢ *ਸਿਰ_2;
  ਬਣਤਰ Curl_llist_ਗੰਢ *ਪੂਛ_2;
  Curl_llist_dtor _dtor;
  size_t ਮਾਪ_2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ _init;      /* detect API usage mistakes */
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ Curl_llist_ਗੰਢ {
  ਬਣਤਰ Curl_llist *_list; /* the list where this belongs */
  ਖਾਲੀ *_ptr;
  ਬਣਤਰ Curl_llist_ਗੰਢ *_prev;
  ਬਣਤਰ Curl_llist_ਗੰਢ *ਅਗਲਾ_2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ _init;      /* detect API usage mistakes */
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ Curl_llist_init(ਬਣਤਰ Curl_llist *l, Curl_llist_dtor dtor);
ਖਾਲੀ Curl_llist_append(ਬਣਤਰ Curl_llist *list, ਅਚਲ ਖਾਲੀ *p,
                       ਬਣਤਰ Curl_llist_ਗੰਢ *ne);
ਖਾਲੀ Curl_ਗੰਢ_remove(ਬਣਤਰ Curl_llist_ਗੰਢ *e);
ਖਾਲੀ Curl_llist_destroy(ਬਣਤਰ Curl_llist *list, ਖਾਲੀ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ਬਣਤਰ Curl_llist_ਗੰਢ *Curl_llist_ਸਿਰ(ਅਚਲ ਬਣਤਰ Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ਗਿਣਤੀ(ਅਚਲ ਬਣਤਰ Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ਖਾਲੀ *Curl_ਗੰਢ_elem(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
ਖਾਲੀ *Curl_ਗੰਢ_take_elem(ਬਣਤਰ Curl_llist_ਗੰਢ *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ਬਣਤਰ Curl_llist_ਗੰਢ *Curl_ਗੰਢ_ਅਗਲਾ(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ਬਣਤਰ Curl_llist *Curl_ਗੰਢ_llist(ਅਚਲ ਬਣਤਰ Curl_llist_ਗੰਢ *n);

#ਜੇ_ਅੰਤ /* HEADER_CURL_LLIST_H */
