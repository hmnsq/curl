#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HASH_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HASH_H
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Curl_hash_dtor)(ಶೂನ್ಯ *);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Curl_hash_elem_dtor)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p);

/* How keys in a hash are interpreted. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_HASH_TYPE_ಬೈಟುಗಳು,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_ಕೊನೆಯ
} Curl_hash_type;

ರಚನೆ Curl_hash_element {
  ರಚನೆ Curl_hash_element *ಮುಂದಿನ;
  ಶೂನ್ಯ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ಕೀಲಿ_len;
  ಅಕ್ಷರ ಕೀಲಿ[1]; /* allocated memory following the struct */
};

ರಚನೆ Curl_hash {
  ರಚನೆ Curl_hash_element **ಕೋಷ್ಟಕ;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t ಗಾತ್ರ_2;
  uint8_t type; /* Curl_hash_type */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ init;
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ Curl_hash_iterator {
  ರಚನೆ Curl_hash *hash;
  size_t ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ;
  ರಚನೆ Curl_hash_element *current;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ init;
#ಆದರೆ_ಅಂತ್ಯ
};

CURLcode Curl_hash_global_init(ಶೂನ್ಯ);

ಶೂನ್ಯ Curl_hash_init(ರಚನೆ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

ಶೂನ್ಯ *Curl_hash_ಕೂಡಿಸು(ರಚನೆ Curl_hash *h,
                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p);
ಶೂನ್ಯ *Curl_hash_add2(ರಚನೆ Curl_hash *h,
                     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p,
                     Curl_hash_elem_dtor dtor);
ಪೂರ್ಣಾಂಕ Curl_hash_delete(ರಚನೆ Curl_hash *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len);
ಶೂನ್ಯ *Curl_hash_pick(ರಚನೆ Curl_hash *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len);

ಶೂನ್ಯ Curl_hash_destroy(ರಚನೆ Curl_hash *h);
size_t Curl_hash_ಎಣಿಕೆ(ರಚನೆ Curl_hash *h);
ಶೂನ್ಯ Curl_hash_clean(ರಚನೆ Curl_hash *h);
ಶೂನ್ಯ Curl_hash_clean_with_criterium(ರಚನೆ Curl_hash *h, ಶೂನ್ಯ *user,
                                    ಪೂರ್ಣಾಂಕ (*comp)(ಶೂನ್ಯ *, ಶೂನ್ಯ *));
ಶೂನ್ಯ Curl_hash_ಆರಂಭ_iterate(ರಚನೆ Curl_hash *hash,
                             ರಚನೆ Curl_hash_iterator *iter);
ರಚನೆ Curl_hash_element *Curl_hash_ಮುಂದಿನ_element(
  ರಚನೆ Curl_hash_iterator *iter);

ಶೂನ್ಯ Curl_hash_print(ರಚನೆ Curl_hash *h, ಶೂನ್ಯ (*func)(ಶೂನ್ಯ *));

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HASH_H */
