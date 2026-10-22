#اگر_غیر_تعریف_شدہ سرخی_CURL_LLIST_H
#تعریف_کرو سرخی_CURL_LLIST_H
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

قسمتعریف خالی (*Curl_llist_dtor)(خالی *user, خالی *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ساخت Curl_llist {
  ساخت Curl_llist_گرہ *سر_2;
  ساخت Curl_llist_گرہ *دم_2;
  Curl_llist_dtor _dtor;
  size_t سائز_2;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح _init;      /* detect API usage mistakes */
#اگر_ختم
};

ساخت Curl_llist_گرہ {
  ساخت Curl_llist *_list; /* the list where this belongs */
  خالی *_ptr;
  ساخت Curl_llist_گرہ *پچھلا_2;
  ساخت Curl_llist_گرہ *اگلا_2;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح _init;      /* detect API usage mistakes */
#اگر_ختم
};

خالی Curl_llist_init(ساخت Curl_llist *l, Curl_llist_dtor dtor);
خالی Curl_llist_append(ساخت Curl_llist *list, ثابت خالی *p,
                       ساخت Curl_llist_گرہ *ne);
خالی Curl_گرہ_remove(ساخت Curl_llist_گرہ *e);
خالی Curl_llist_destroy(ساخت Curl_llist *list, خالی *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ساخت Curl_llist_گرہ *Curl_llist_سر(ثابت ساخت Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_گنتی(ثابت ساخت Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
خالی *Curl_گرہ_elem(ثابت ساخت Curl_llist_گرہ *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
خالی *Curl_گرہ_take_elem(ساخت Curl_llist_گرہ *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ساخت Curl_llist_گرہ *Curl_گرہ_اگلا(ثابت ساخت Curl_llist_گرہ *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ساخت Curl_llist *Curl_گرہ_llist(ثابت ساخت Curl_llist_گرہ *n);

#اگر_ختم /* HEADER_CURL_LLIST_H */
