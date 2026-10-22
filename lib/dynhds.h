#ifndef HEADER_CURL_DYNHDS_H
#define HEADER_CURL_DYNHDS_H
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

#ضمن "curlx/dynbuf.h"

بنية dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
بنية dynhds_entry {
  حرف *name;
  حرف *نيلاي;
  size_t namelen;
  size_t valuelen;
};

بنية dynhds {
  بنية dynhds_entry **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_size; /* max length of all strings */
  صحيح opts;
};

#define DYNHDS_OPT_NONE          0
#define DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
فراغ Curl_dynhds_init(بنية dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
فراغ Curl_dynhds_free(بنية dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
فراغ Curl_dynhds_reset(بنية dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_کيراءن(بنية dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
فراغ Curl_dynhds_set_opts(بنية dynhds *dynhds, صحيح opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
بنية dynhds_entry *Curl_dynhds_getn(بنية dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
بنية dynhds_entry *Curl_dynhds_get(بنية dynhds *dynhds,
                                     ثابت حرف *name, size_t namelen);
بنية dynhds_entry *Curl_dynhds_cget(بنية dynhds *dynhds, ثابت حرف *name);

CURLcode Curl_dynhds_cset(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *نيلاي);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_add(بنية dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *نيلاي, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *نيلاي);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_باريس(بنية dynhds *dynhds, ثابت حرف *باريس);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_add_باريس(بنية dynhds *dynhds,
                                 ثابت حرف *باريس, size_t باريس_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(بنية dynhds *dynhds, بنية dynbuf *dbuf);

#ifdef USE_NGHTTP2

#ضمن <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_to_nva(بنية dynhds *dynhds, size_t *pcount);

#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_DYNHDS_H */
