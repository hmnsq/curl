#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_LLIST_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_LLIST_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Curl_llist_dtor)(ಶೂನ್ಯ *user, ಶೂನ್ಯ *elem);

/* none of these struct members should be referenced directly, use the
   dedicated functions */

ರಚನೆ Curl_llist {
  ರಚನೆ Curl_llist_ಗಂಟು *ತಲೆ_2;
  ರಚನೆ Curl_llist_ಗಂಟು *ಬಾಲ_2;
  Curl_llist_dtor _dtor;
  size_t ಗಾತ್ರ_3;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ _init;      /* detect API usage mistakes */
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ Curl_llist_ಗಂಟು {
  ರಚನೆ Curl_llist *_list; /* the list where this belongs */
  ಶೂನ್ಯ *_ptr;
  ರಚನೆ Curl_llist_ಗಂಟು *_prev;
  ರಚನೆ Curl_llist_ಗಂಟು *ಮುಂದಿನ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ _init;      /* detect API usage mistakes */
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ Curl_llist_init(ರಚನೆ Curl_llist *l, Curl_llist_dtor dtor);
ಶೂನ್ಯ Curl_llist_append(ರಚನೆ Curl_llist *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                       ರಚನೆ Curl_llist_ಗಂಟು *ne);
ಶೂನ್ಯ Curl_ಗಂಟು_remove(ರಚನೆ Curl_llist_ಗಂಟು *e);
ಶೂನ್ಯ Curl_llist_destroy(ರಚನೆ Curl_llist *list, ಶೂನ್ಯ *user);

/* Curl_llist_head() returns the first 'struct Curl_llist_node *', which
   might be NULL */
ರಚನೆ Curl_llist_ಗಂಟು *Curl_llist_ತಲೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list);

/* Curl_llist_count() returns a size_t the number of nodes in the list */
size_t Curl_llist_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist *list);

/* Curl_node_elem() returns the custom data from a Curl_llist_node */
ಶೂನ್ಯ *Curl_ಗಂಟು_elem(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n);

/* Remove the node from the list and return the custom data
 * from a Curl_llist_node. Does NOT invoke a registered `dtor`. */
ಶೂನ್ಯ *Curl_ಗಂಟು_take_elem(ರಚನೆ Curl_llist_ಗಂಟು *e);

/* Curl_node_next() returns the next element in a list from a given
   Curl_llist_node */
ರಚನೆ Curl_llist_ಗಂಟು *Curl_ಗಂಟು_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n);

/* Curl_node_llist() return the list the node is in or NULL. */
ರಚನೆ Curl_llist *Curl_ಗಂಟು_llist(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_llist_ಗಂಟು *n);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_LLIST_H */
