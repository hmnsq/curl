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
#अन्तर्भाव "curl_setup.h"

प्रकारनिर्देश शून्य (*Curl_llist_dtor)(शून्य *user, शून्य *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

संरचना Curl_llist {
  संरचना Curl_llist_गाठ *_head;
  संरचना Curl_llist_गाठ *_tail;
  Curl_llist_dtor _dtor;
  size_t _size;
#ifdef DEBUGBUILD
  पूर्णाङ्क _init;      /* detect API usage mistakes */
#endif
};

संरचना Curl_llist_गाठ {
  संरचना Curl_llist *_list; /* the list where this belongs */
  शून्य *_ptr;
  संरचना Curl_llist_गाठ *_prev;
  संरचना Curl_llist_गाठ *_next;
#ifdef DEBUGBUILD
  पूर्णाङ्क _init;      /* detect API usage mistakes */
#endif
};

शून्य Curl_llist_init(संरचना Curl_llist *l, Curl_llist_dtor dtor);
शून्य Curl_llist_append(संरचना Curl_llist *list, ध्रुव शून्य *p,
                       संरचना Curl_llist_गाठ *ne);
शून्य Curl_गाठ_remove(संरचना Curl_llist_गाठ *e);
शून्य Curl_llist_destroy(संरचना Curl_llist *list, शून्य *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
संरचना Curl_llist_गाठ *Curl_llist_head(ध्रुव संरचना Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_मोजणी(ध्रुव संरचना Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
शून्य *Curl_गाठ_elem(ध्रुव संरचना Curl_llist_गाठ *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
शून्य *Curl_गाठ_take_elem(संरचना Curl_llist_गाठ *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
संरचना Curl_llist_गाठ *Curl_गाठ_next(ध्रुव संरचना Curl_llist_गाठ *n);

/* Curl_node_llist() return the list the node is in or NULL. */
संरचना Curl_llist *Curl_गाठ_llist(ध्रुव संरचना Curl_llist_गाठ *n);

#endif /* HEADER_CURL_LLIST_H */
