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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*Curl_llist_dtor)(ଶୂନ୍ୟ *user, ଶୂନ୍ୟ *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ସଂରଚନା Curl_llist {
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *_head;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା _init;      /* detect API usage mistakes */
#endif
};

ସଂରଚନା Curl_llist_ଗଣ୍ଠି {
  ସଂରଚନା Curl_llist *_list; /* the list where this belongs */
  ଶୂନ୍ୟ *_ptr;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *_prev;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *_next;
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା _init;      /* detect API usage mistakes */
#endif
};

ଶୂନ୍ୟ Curl_llist_init(ସଂରଚନା Curl_llist *l, Curl_llist_dtor dtor);
ଶୂନ୍ୟ Curl_llist_append(ସଂରଚନା Curl_llist *list, ଅଚଳ ଶୂନ୍ୟ *p,
                       ସଂରଚନା Curl_llist_ଗଣ୍ଠି *ne);
ଶୂନ୍ୟ Curl_ଗଣ୍ଠି_remove(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e);
ଶୂନ୍ୟ Curl_llist_destroy(ସଂରଚନା Curl_llist *list, ଶୂନ୍ୟ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ସଂରଚନା Curl_llist_ଗଣ୍ଠି *Curl_llist_head(ଅଚଳ ସଂରଚନା Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ଗଣନା(ଅଚଳ ସଂରଚନା Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ଶୂନ୍ୟ *Curl_ଗଣ୍ଠି_elem(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
ଶୂନ୍ୟ *Curl_ଗଣ୍ଠି_take_elem(ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ସଂରଚନା Curl_llist_ଗଣ୍ଠି *Curl_ଗଣ୍ଠି_next(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ସଂରଚନା Curl_llist *Curl_ଗଣ୍ଠି_llist(ଅଚଳ ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n);

#endif /* HEADER_CURL_LLIST_H */
