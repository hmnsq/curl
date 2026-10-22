#నిర్వచితం_కాకపోతే శీర్షిక_CURL_DYNHDS_H
#నిర్వచించు శీర్షిక_CURL_DYNHDS_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/dynbuf.h"

నిర్మాణం dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
నిర్మాణం dynhds_నమోదు {
  అక్షరం *name;
  అక్షరం *విలువ;
  size_t namelen;
  size_t valuelen;
};

నిర్మాణం dynhds {
  నిర్మాణం dynhds_నమోదు **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_కొలత; /* max length of all strings */
  పూర్ణాంకం opts;
};

#నిర్వచించు DYNHDS_OPT_NONE          0
#నిర్వచించు DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
శూన్య Curl_dynhds_init(నిర్మాణం dynhds *dynhds, size_t max_entries,
                      size_t max_strs_కొలత);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
శూన్య Curl_dynhds_free(నిర్మాణం dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
శూన్య Curl_dynhds_reset(నిర్మాణం dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_లెక్క(నిర్మాణం dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
శూన్య Curl_dynhds_సమితి_opts(నిర్మాణం dynhds *dynhds, పూర్ణాంకం opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
నిర్మాణం dynhds_నమోదు *Curl_dynhds_getn(నిర్మాణం dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
నిర్మాణం dynhds_నమోదు *Curl_dynhds_get(నిర్మాణం dynhds *dynhds,
                                     స్థిరరాశి అక్షరం *name, size_t namelen);
నిర్మాణం dynhds_నమోదు *Curl_dynhds_cget(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name);

CURLcode Curl_dynhds_cset(నిర్మాణం dynhds *dynhds,
                          స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *విలువ);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_కూడు(నిర్మాణం dynhds *dynhds,
                         స్థిరరాశి అక్షరం *name, size_t namelen,
                         స్థిరరాశి అక్షరం *విలువ, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(నిర్మాణం dynhds *dynhds,
                          స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *విలువ);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_పంక్తి(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *పంక్తి);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_కూడు_పంక్తి(నిర్మాణం dynhds *dynhds,
                                 స్థిరరాశి అక్షరం *పంక్తి, size_t పంక్తి_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(నిర్మాణం dynhds *dynhds, నిర్మాణం dynbuf *dbuf);

#నిర్వచితమైతే USE_NGHTTP2

#అన్తర్భావ <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_గమ్యం_nva(నిర్మాణం dynhds *dynhds, size_t *pcount);

#అయితే_ముగింపు /* USE_NGHTTP2 */

#అయితే_ముగింపు /* HEADER_CURL_DYNHDS_H */
