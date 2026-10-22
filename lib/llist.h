#که_نه_تعریف_شوی سرلیک_CURL_LLIST_H
#تعریف_کړه سرلیک_CURL_LLIST_H
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
#ضمن "curl_setup.h"

تعريفنوع فراغ (*Curl_llist_dtor)(فراغ *user, فراغ *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

بنية Curl_llist {
  بنية Curl_llist_غوټه *سر_3;
  بنية Curl_llist_غوټه *لکۍ_2;
  Curl_llist_dtor _dtor;
  size_t اندازه_2;
#که_تعریف_شوی DEBUGBUILD
  صحيح _init;      /* detect API usage mistakes */
#د_که_پای
};

بنية Curl_llist_غوټه {
  بنية Curl_llist *_list; /* the list where this belongs */
  فراغ *_ptr;
  بنية Curl_llist_غوټه *_prev;
  بنية Curl_llist_غوټه *بل_2;
#که_تعریف_شوی DEBUGBUILD
  صحيح _init;      /* detect API usage mistakes */
#د_که_پای
};

فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor);
فراغ Curl_llist_append(بنية Curl_llist *list, ثابت فراغ *p,
                       بنية Curl_llist_غوټه *ne);
فراغ Curl_غوټه_remove(بنية Curl_llist_غوټه *e);
فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_غوټه *Curl_llist_سر(ثابت بنية Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمېر(ثابت بنية Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_غوټه_elem(ثابت بنية Curl_llist_غوټه *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
فراغ *Curl_غوټه_take_elem(بنية Curl_llist_غوټه *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_غوټه *Curl_غوټه_بل(ثابت بنية Curl_llist_غوټه *n);

/* Curl_node_llist() return the list the node is in or NULL. */
بنية Curl_llist *Curl_غوټه_llist(ثابت بنية Curl_llist_غوټه *n);

#د_که_پای /* HEADER_CURL_LLIST_H */
