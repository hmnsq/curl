#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_HASH_H
#وصف_ڪريو سرو_CURL_HASH_H
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

تعريفنوع فراغ (*Curl_hash_dtor)(فراغ *);

تعريفنوع فراغ (*Curl_hash_elem_dtor)(ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p);

/* How keys in a hash are interpreted. */
تعريفنوع تعداد {
  CURL_HASH_TYPE_بائيٽون,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_آخري
} Curl_hash_type;

بنية Curl_hash_element {
  بنية Curl_hash_element *ايندڙ;
  فراغ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ڪنجي_len;
  حرف ڪنجي[1]; /* allocated memory following the struct */
};

بنية Curl_hash {
  بنية Curl_hash_element **جدول;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t سائيز;
  uint8_t type; /* Curl_hash_type */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح init;
#جيڪڏهن_پورو
};

بنية Curl_hash_iterator {
  بنية Curl_hash *hash;
  size_t جڳهه_اشاريو;
  بنية Curl_hash_element *current;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح init;
#جيڪڏهن_پورو
};

CURLcode Curl_hash_global_init(فراغ);

فراغ Curl_hash_init(بنية Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

فراغ *Curl_hash_جوڙيو(بنية Curl_hash *h,
                    ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p);
فراغ *Curl_hash_add2(بنية Curl_hash *h,
                     ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p,
                     Curl_hash_elem_dtor dtor);
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *ڪنجي, size_t ڪنجي_len);
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *ڪنجي, size_t ڪنجي_len);

فراغ Curl_hash_destroy(بنية Curl_hash *h);
size_t Curl_hash_ڳڻپ(بنية Curl_hash *h);
فراغ Curl_hash_clean(بنية Curl_hash *h);
فراغ Curl_hash_clean_with_criterium(بنية Curl_hash *h, فراغ *user,
                                    صحيح (*comp)(فراغ *, فراغ *));
فراغ Curl_hash_شروعات_iterate(بنية Curl_hash *hash,
                             بنية Curl_hash_iterator *iter);
بنية Curl_hash_element *Curl_hash_ايندڙ_element(
  بنية Curl_hash_iterator *iter);

فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *));

#جيڪڏهن_پورو /* HEADER_CURL_HASH_H */
