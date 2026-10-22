#अगर_अघोषित HEADER_CURL_HASH_H
#घोषणा HEADER_CURL_HASH_H
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

प्रकार व्योम (*Curl_hash_dtor)(व्योम *);

प्रकार व्योम (*Curl_hash_elem_dtor)(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p);

/* How keys in a hash are interpreted. */
प्रकार क्रमागत {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

काष्ठा Curl_hash_element {
  काष्ठा Curl_hash_element *अग्र;
  व्योम *ptr;
  Curl_hash_elem_dtor dtor;
  माप_प्रकार कुंजी_len;
  अक्षर कुंजी[1]; /* allocated memory following the struct */
};

काष्ठा Curl_hash {
  काष्ठा Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  माप_प्रकार slots;
  माप_प्रकार आकार;
  uint8_t type; /* Curl_hash_type */
#अगर_घोषित DEBUGBUILD
  पूर्णांक init;
#पूर्ण_अगर
};

काष्ठा Curl_hash_iterator {
  काष्ठा Curl_hash *hash;
  माप_प्रकार slot_सूचकांक;
  काष्ठा Curl_hash_element *current;
#अगर_घोषित DEBUGBUILD
  पूर्णांक init;
#पूर्ण_अगर
};

CURLcode Curl_hash_global_init(व्योम);

व्योम Curl_hash_init(काष्ठा Curl_hash *h,
                    माप_प्रकार slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

व्योम *Curl_hash_add(काष्ठा Curl_hash *h,
                    स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p);
व्योम *Curl_hash_add2(काष्ठा Curl_hash *h,
                     स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p,
                     Curl_hash_elem_dtor dtor);
पूर्णांक Curl_hash_delete(काष्ठा Curl_hash *h, स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len);
व्योम *Curl_hash_pick(काष्ठा Curl_hash *h, स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len);

व्योम Curl_hash_destroy(काष्ठा Curl_hash *h);
माप_प्रकार Curl_hash_गिनती(काष्ठा Curl_hash *h);
व्योम Curl_hash_clean(काष्ठा Curl_hash *h);
व्योम Curl_hash_clean_with_criterium(काष्ठा Curl_hash *h, व्योम *user,
                                    पूर्णांक (*comp)(व्योम *, व्योम *));
व्योम Curl_hash_start_iterate(काष्ठा Curl_hash *hash,
                             काष्ठा Curl_hash_iterator *iter);
काष्ठा Curl_hash_element *Curl_hash_अग्र_element(
  काष्ठा Curl_hash_iterator *iter);

व्योम Curl_hash_print(काष्ठा Curl_hash *h, व्योम (*func)(व्योम *));

#पूर्ण_अगर /* HEADER_CURL_HASH_H */
