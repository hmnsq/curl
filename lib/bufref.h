#ifndef HEADER_CURL_BUFREF_H
#define HEADER_CURL_BUFREF_H
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

/*
 * Generic buffer reference.
 */
ഘടന bufref {
  ശൂന്യം (*dtor)(ശൂന്യം *);         /* Associated destructor. */
  സ്ഥിരം unsigned അക്ഷരം *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ signature;                /* Detect API use mistakes. */
#endif
};

ശൂന്യം Curl_bufref_init(ഘടന bufref *br);
ശൂന്യം Curl_bufref_set(ഘടന bufref *br, സ്ഥിരം ശൂന്യം *ptr, size_t len,
                     ശൂന്യം (*dtor)(ശൂന്യം *));
സ്ഥിരം അക്ഷരം *Curl_bufref_ptr(സ്ഥിരം ഘടന bufref *br);
സ്ഥിരം unsigned അക്ഷരം *Curl_bufref_uptr(സ്ഥിരം ഘടന bufref *br);
size_t Curl_bufref_len(സ്ഥിരം ഘടന bufref *br);
CURLcode Curl_bufref_memdup0(ഘടന bufref *br, സ്ഥിരം ശൂന്യം *ptr, size_t len);
ശൂന്യം Curl_bufref_free(ഘടന bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
