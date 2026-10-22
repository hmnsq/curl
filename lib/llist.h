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
#അന്തര്ഭാവ "curl_setup.h"

തരനിർവചനം ശൂന്യം (*Curl_llist_dtor)(ശൂന്യം *user, ശൂന്യം *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ഘടന Curl_llist {
  ഘടന Curl_llist_കണ്ണി *_head;
  ഘടന Curl_llist_കണ്ണി *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ _init;      /* detect API usage mistakes */
#endif
};

ഘടന Curl_llist_കണ്ണി {
  ഘടന Curl_llist *_list; /* the list where this belongs */
  ശൂന്യം *_ptr;
  ഘടന Curl_llist_കണ്ണി *_prev;
  ഘടന Curl_llist_കണ്ണി *_next;
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ _init;      /* detect API usage mistakes */
#endif
};

ശൂന്യം Curl_llist_init(ഘടന Curl_llist *l, Curl_llist_dtor dtor);
ശൂന്യം Curl_llist_append(ഘടന Curl_llist *list, സ്ഥിരം ശൂന്യം *p,
                       ഘടന Curl_llist_കണ്ണി *ne);
ശൂന്യം Curl_കണ്ണി_remove(ഘടന Curl_llist_കണ്ണി *e);
ശൂന്യം Curl_llist_destroy(ഘടന Curl_llist *list, ശൂന്യം *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ഘടന Curl_llist_കണ്ണി *Curl_llist_head(സ്ഥിരം ഘടന Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_എണ്ണം(സ്ഥിരം ഘടന Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ശൂന്യം *Curl_കണ്ണി_elem(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
ശൂന്യം *Curl_കണ്ണി_take_elem(ഘടന Curl_llist_കണ്ണി *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ഘടന Curl_llist_കണ്ണി *Curl_കണ്ണി_next(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ഘടന Curl_llist *Curl_കണ്ണി_llist(സ്ഥിരം ഘടന Curl_llist_കണ്ണി *n);

#endif /* HEADER_CURL_LLIST_H */
