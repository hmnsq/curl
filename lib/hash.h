#ifndef HEADER_CURL_HASH_H
#define HEADER_CURL_HASH_H
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

प्रकारव्याख्या शून्य (*Curl_hash_dtor)(शून्य *);

प्रकारव्याख्या शून्य (*Curl_hash_elem_dtor)(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p);

/* How keys in a hash are interpreted. */
प्रकारव्याख्या गणना {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

संरचना Curl_hash_element {
  संरचना Curl_hash_element *next;
  शून्य *ptr;
  Curl_hash_elem_dtor dtor;
  माप_प्रकार किल्ली_len;
  अक्षर किल्ली[1]; /* allocated memory following the struct */
};

संरचना Curl_hash {
  संरचना Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  माप_प्रकार slots;
  माप_प्रकार size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  पूर्णांक init;
#endif
};

संरचना Curl_hash_iterator {
  संरचना Curl_hash *hash;
  माप_प्रकार slot_index;
  संरचना Curl_hash_element *current;
#ifdef DEBUGBUILD
  पूर्णांक init;
#endif
};

CURLcode Curl_hash_global_init(शून्य);

शून्य Curl_hash_init(संरचना Curl_hash *h,
                    माप_प्रकार slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

शून्य *Curl_hash_add(संरचना Curl_hash *h,
                    स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p);
शून्य *Curl_hash_add2(संरचना Curl_hash *h,
                     स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p,
                     Curl_hash_elem_dtor dtor);
पूर्णांक Curl_hash_delete(संरचना Curl_hash *h, स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len);
शून्य *Curl_hash_pick(संरचना Curl_hash *h, स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len);

शून्य Curl_hash_destroy(संरचना Curl_hash *h);
माप_प्रकार Curl_hash_मोजणी(संरचना Curl_hash *h);
शून्य Curl_hash_clean(संरचना Curl_hash *h);
शून्य Curl_hash_clean_with_criterium(संरचना Curl_hash *h, शून्य *user,
                                    पूर्णांक (*comp)(शून्य *, शून्य *));
शून्य Curl_hash_start_iterate(संरचना Curl_hash *hash,
                             संरचना Curl_hash_iterator *iter);
संरचना Curl_hash_element *Curl_hash_next_element(
  संरचना Curl_hash_iterator *iter);

शून्य Curl_hash_print(संरचना Curl_hash *h, शून्य (*func)(शून्य *));

#endif /* HEADER_CURL_HASH_H */
