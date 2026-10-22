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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

ସଂରଚନା dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
ସଂରଚନା dynhds_entry {
  ଅକ୍ଷର *name;
  ଅକ୍ଷର *ମୂଲ୍ୟ;
  size_t namelen;
  size_t valuelen;
};

ସଂରଚନା dynhds {
  ସଂରଚନା dynhds_entry **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_size; /* max length of all strings */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts;
};

#define DYNHDS_OPT_NONE          0
#define DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
ଶୂନ୍ୟ Curl_dynhds_init(ସଂରଚନା dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
ଶୂନ୍ୟ Curl_dynhds_free(ସଂରଚନା dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
ଶୂନ୍ୟ Curl_dynhds_reset(ସଂରଚନା dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_ଗଣନା(ସଂରଚନା dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
ଶୂନ୍ୟ Curl_dynhds_set_opts(ସଂରଚନା dynhds *dynhds, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
ସଂରଚନା dynhds_entry *Curl_dynhds_getn(ସଂରଚନା dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
ସଂରଚନା dynhds_entry *Curl_dynhds_get(ସଂରଚନା dynhds *dynhds,
                                     ଅଚଳ ଅକ୍ଷର *name, size_t namelen);
ସଂରଚନା dynhds_entry *Curl_dynhds_cget(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name);

CURLcode Curl_dynhds_cset(ସଂରଚନା dynhds *dynhds,
                          ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_add(ସଂରଚନା dynhds *dynhds,
                         ଅଚଳ ଅକ୍ଷର *name, size_t namelen,
                         ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(ସଂରଚନା dynhds *dynhds,
                          ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_ଧାଡ଼ି(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_add_ଧାଡ଼ି(ସଂରଚନା dynhds *dynhds,
                                 ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t ଧାଡ଼ି_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(ସଂରଚନା dynhds *dynhds, ସଂରଚନା dynbuf *dbuf);

#ifdef USE_NGHTTP2

#ଅନ୍ତର୍ଭାଵ <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_to_nva(ସଂରଚନା dynhds *dynhds, size_t *pcount);

#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_DYNHDS_H */
