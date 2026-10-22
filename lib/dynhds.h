#اگر_غیر_تعریف_شدہ سرخی_CURL_DYNHDS_H
#تعریف_کرو سرخی_CURL_DYNHDS_H
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
#شامل "curl_setup.h"

#شامل "curlx/dynbuf.h"

ساخت dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
ساخت dynhds_اندراج {
  حرف *name;
  حرف *قدر;
  size_t namelen;
  size_t valuelen;
};

ساخت dynhds {
  ساخت dynhds_اندراج **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_سائز; /* max length of all strings */
  صحیح opts;
};

#تعریف_کرو DYNHDS_OPT_NONE          0
#تعریف_کرو DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
خالی Curl_dynhds_init(ساخت dynhds *dynhds, size_t max_entries,
                      size_t max_strs_سائز);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
خالی Curl_dynhds_free(ساخت dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
خالی Curl_dynhds_reset(ساخت dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_گنتی(ساخت dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
خالی Curl_dynhds_سیٹ_opts(ساخت dynhds *dynhds, صحیح opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
ساخت dynhds_اندراج *Curl_dynhds_getn(ساخت dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
ساخت dynhds_اندراج *Curl_dynhds_get(ساخت dynhds *dynhds,
                                     ثابت حرف *name, size_t namelen);
ساخت dynhds_اندراج *Curl_dynhds_cget(ساخت dynhds *dynhds, ثابت حرف *name);

CURLcode Curl_dynhds_cset(ساخت dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *قدر);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_جوڑو(ساخت dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *قدر, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(ساخت dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *قدر);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_سطر(ساخت dynhds *dynhds, ثابت حرف *سطر);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_جوڑو_سطر(ساخت dynhds *dynhds,
                                 ثابت حرف *سطر, size_t سطر_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(ساخت dynhds *dynhds, ساخت dynbuf *dbuf);

#اگر_تعریف_شدہ USE_NGHTTP2

#شامل <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_مقصد_nva(ساخت dynhds *dynhds, size_t *pcount);

#اگر_ختم /* USE_NGHTTP2 */

#اگر_ختم /* HEADER_CURL_DYNHDS_H */
