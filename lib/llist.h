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
#અન્તર્ભાવ "curl_setup.h"

પ્રકારવ્યાખ્યા શૂન્ય (*Curl_llist_dtor)(શૂન્ય *user, શૂન્ય *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

સંરચના Curl_llist {
  સંરચના Curl_llist_ગાંઠ *_head;
  સંરચના Curl_llist_ગાંઠ *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  પૂર્ણાંક _init;      /* detect API usage mistakes */
#endif
};

સંરચના Curl_llist_ગાંઠ {
  સંરચના Curl_llist *_list; /* the list where this belongs */
  શૂન્ય *_ptr;
  સંરચના Curl_llist_ગાંઠ *_prev;
  સંરચના Curl_llist_ગાંઠ *_next;
#ifdef DEBUGBUILD
  પૂર્ણાંક _init;      /* detect API usage mistakes */
#endif
};

શૂન્ય Curl_llist_init(સંરચના Curl_llist *l, Curl_llist_dtor dtor);
શૂન્ય Curl_llist_append(સંરચના Curl_llist *list, અચલ શૂન્ય *p,
                       સંરચના Curl_llist_ગાંઠ *ne);
શૂન્ય Curl_ગાંઠ_remove(સંરચના Curl_llist_ગાંઠ *e);
શૂન્ય Curl_llist_destroy(સંરચના Curl_llist *list, શૂન્ય *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
સંરચના Curl_llist_ગાંઠ *Curl_llist_head(અચલ સંરચના Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ગણતરી(અચલ સંરચના Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
શૂન્ય *Curl_ગાંઠ_elem(અચલ સંરચના Curl_llist_ગાંઠ *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
શૂન્ય *Curl_ગાંઠ_take_elem(સંરચના Curl_llist_ગાંઠ *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
સંરચના Curl_llist_ગાંઠ *Curl_ગાંઠ_next(અચલ સંરચના Curl_llist_ગાંઠ *n);

/* Curl_node_llist() return the list the node is in or NULL. */
સંરચના Curl_llist *Curl_ગાંઠ_llist(અચલ સંરચના Curl_llist_ગાંઠ *n);

#endif /* HEADER_CURL_LLIST_H */
