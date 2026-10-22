#اگر_تعریف_نشده سرآیند_CURL_LLIST_H
#تعریف_کن سرآیند_CURL_LLIST_H
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
  بنية Curl_llist_گره *سر_2;
  بنية Curl_llist_گره *دم_2;
  Curl_llist_dtor _dtor;
  size_t اندازه_2;
#اگر_تعریف_شده DEBUGBUILD
  صحيح _init;      /* detect API usage mistakes */
#پایان_اگر
};

بنية Curl_llist_گره {
  بنية Curl_llist *_list; /* the list where this belongs */
  فراغ *_ptr;
  بنية Curl_llist_گره *_prev;
  بنية Curl_llist_گره *بعدی_2;
#اگر_تعریف_شده DEBUGBUILD
  صحيح _init;      /* detect API usage mistakes */
#پایان_اگر
};

فراغ Curl_llist_init(بنية Curl_llist *l, Curl_llist_dtor dtor);
فراغ Curl_llist_append(بنية Curl_llist *list, ثابت فراغ *p,
                       بنية Curl_llist_گره *ne);
فراغ Curl_گره_remove(بنية Curl_llist_گره *e);
فراغ Curl_llist_destroy(بنية Curl_llist *list, فراغ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
بنية Curl_llist_گره *Curl_llist_سر(ثابت بنية Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_شمار(ثابت بنية Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
فراغ *Curl_گره_elem(ثابت بنية Curl_llist_گره *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
فراغ *Curl_گره_take_elem(بنية Curl_llist_گره *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
بنية Curl_llist_گره *Curl_گره_بعدی(ثابت بنية Curl_llist_گره *n);

/* Curl_node_llist() return the list the node is in or NULL. */
بنية Curl_llist *Curl_گره_llist(ثابت بنية Curl_llist_گره *n);

#پایان_اگر /* HEADER_CURL_LLIST_H */
