#अगर_अघोषित HEADER_CURL_LLIST_H
#घोषणा HEADER_CURL_LLIST_H
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
#समावेश "curl_setup.h"

प्रकार व्योम (*Curl_llist_dtor)(व्योम *user, व्योम *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

काष्ठा Curl_llist {
  काष्ठा Curl_llist_गाँठ *शीर्ष_3;
  काष्ठा Curl_llist_गाँठ *पुच्छ_2;
  Curl_llist_dtor _dtor;
  माप_प्रकार आकार_2;
#अगर_घोषित DEBUGBUILD
  पूर्णांक _init;      /* detect API usage mistakes */
#पूर्ण_अगर
};

काष्ठा Curl_llist_गाँठ {
  काष्ठा Curl_llist *_list; /* the list where this belongs */
  व्योम *_ptr;
  काष्ठा Curl_llist_गाँठ *पूर्व_2;
  काष्ठा Curl_llist_गाँठ *अग्र_2;
#अगर_घोषित DEBUGBUILD
  पूर्णांक _init;      /* detect API usage mistakes */
#पूर्ण_अगर
};

व्योम Curl_llist_init(काष्ठा Curl_llist *l, Curl_llist_dtor dtor);
व्योम Curl_llist_append(काष्ठा Curl_llist *list, स्थिर व्योम *p,
                       काष्ठा Curl_llist_गाँठ *ne);
व्योम Curl_गाँठ_remove(काष्ठा Curl_llist_गाँठ *e);
व्योम Curl_llist_destroy(काष्ठा Curl_llist *list, व्योम *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
काष्ठा Curl_llist_गाँठ *Curl_llist_शीर्ष(स्थिर काष्ठा Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
माप_प्रकार Curl_llist_गिनती(स्थिर काष्ठा Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
व्योम *Curl_गाँठ_elem(स्थिर काष्ठा Curl_llist_गाँठ *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
व्योम *Curl_गाँठ_take_elem(काष्ठा Curl_llist_गाँठ *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
काष्ठा Curl_llist_गाँठ *Curl_गाँठ_अग्र(स्थिर काष्ठा Curl_llist_गाँठ *n);

/* Curl_node_llist() return the list the node is in or NULL. */
काष्ठा Curl_llist *Curl_गाँठ_llist(स्थिर काष्ठा Curl_llist_गाँठ *n);

#पूर्ण_अगर /* HEADER_CURL_LLIST_H */
