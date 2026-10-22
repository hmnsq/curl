#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_DYNHDS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_DYNHDS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"

ਬਣਤਰ dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
ਬਣਤਰ dynhds_ਇੰਦਰਾਜ {
  ਅੱਖਰ *name;
  ਅੱਖਰ *ਮੁੱਲ;
  size_t namelen;
  size_t valuelen;
};

ਬਣਤਰ dynhds {
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_ਮਾਪ; /* max length of all strings */
  ਪੂਰਨ opts;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYNHDS_OPT_NONE          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
ਖਾਲੀ Curl_dynhds_init(ਬਣਤਰ dynhds *dynhds, size_t max_entries,
                      size_t max_strs_ਮਾਪ);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
ਖਾਲੀ Curl_dynhds_free(ਬਣਤਰ dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
ਖਾਲੀ Curl_dynhds_reset(ਬਣਤਰ dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_ਗਿਣਤੀ(ਬਣਤਰ dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
ਖਾਲੀ Curl_dynhds_ਸੈੱਟ_opts(ਬਣਤਰ dynhds *dynhds, ਪੂਰਨ opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_getn(ਬਣਤਰ dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_get(ਬਣਤਰ dynhds *dynhds,
                                     ਅਚਲ ਅੱਖਰ *name, size_t namelen);
ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_cget(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name);

CURLcode Curl_dynhds_cset(ਬਣਤਰ dynhds *dynhds,
                          ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_ਜੋੜੋ(ਬਣਤਰ dynhds *dynhds,
                         ਅਚਲ ਅੱਖਰ *name, size_t namelen,
                         ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(ਬਣਤਰ dynhds *dynhds,
                          ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_ਸਤਰ(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *ਸਤਰ);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_ਜੋੜੋ_ਸਤਰ(ਬਣਤਰ dynhds *dynhds,
                                 ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t ਸਤਰ_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(ਬਣਤਰ dynhds *dynhds, ਬਣਤਰ dynbuf *dbuf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2

#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_ਮੰਜ਼ਿਲ_nva(ਬਣਤਰ dynhds *dynhds, size_t *pcount);

#ਜੇ_ਅੰਤ /* USE_NGHTTP2 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_DYNHDS_H */
