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
#അന്തര്ഭാവ "curl_setup.h"

തരനിർവചനം ശൂന്യം (*Curl_hash_dtor)(ശൂന്യം *);

തരനിർവചനം ശൂന്യം (*Curl_hash_elem_dtor)(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p);

/* How keys in a hash are interpreted. */
തരനിർവചനം എണ്ണൽ {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

ഘടന Curl_hash_element {
  ഘടന Curl_hash_element *next;
  ശൂന്യം *ptr;
  Curl_hash_elem_dtor dtor;
  size_t താക്കോൽ_len;
  അക്ഷരം താക്കോൽ[1]; /* allocated memory following the struct */
};

ഘടന Curl_hash {
  ഘടന Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ init;
#endif
};

ഘടന Curl_hash_iterator {
  ഘടന Curl_hash *hash;
  size_t slot_index;
  ഘടന Curl_hash_element *current;
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ init;
#endif
};

CURLcode Curl_hash_global_init(ശൂന്യം);

ശൂന്യം Curl_hash_init(ഘടന Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

ശൂന്യം *Curl_hash_add(ഘടന Curl_hash *h,
                    സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p);
ശൂന്യം *Curl_hash_add2(ഘടന Curl_hash *h,
                     സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p,
                     Curl_hash_elem_dtor dtor);
പൂർണ്ണസംഖ്യ Curl_hash_delete(ഘടന Curl_hash *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len);
ശൂന്യം *Curl_hash_pick(ഘടന Curl_hash *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len);

ശൂന്യം Curl_hash_destroy(ഘടന Curl_hash *h);
size_t Curl_hash_എണ്ണം(ഘടന Curl_hash *h);
ശൂന്യം Curl_hash_clean(ഘടന Curl_hash *h);
ശൂന്യം Curl_hash_clean_with_criterium(ഘടന Curl_hash *h, ശൂന്യം *user,
                                    പൂർണ്ണസംഖ്യ (*comp)(ശൂന്യം *, ശൂന്യം *));
ശൂന്യം Curl_hash_start_iterate(ഘടന Curl_hash *hash,
                             ഘടന Curl_hash_iterator *iter);
ഘടന Curl_hash_element *Curl_hash_next_element(
  ഘടന Curl_hash_iterator *iter);

ശൂന്യം Curl_hash_print(ഘടന Curl_hash *h, ശൂന്യം (*func)(ശൂന്യം *));

#endif /* HEADER_CURL_HASH_H */
