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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/dynbuf.h"

संरचना dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
संरचना dynhds_entry {
  वर्ण *name;
  वर्ण *मानम्;
  size_t namelen;
  size_t valuelen;
};

संरचना dynhds {
  संरचना dynhds_entry **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_size; /* max length of all strings */
  पूर्णाङ्क opts;
};

#define DYNHDS_OPT_NONE          0
#define DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
शून्य Curl_dynhds_init(संरचना dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
शून्य Curl_dynhds_free(संरचना dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
शून्य Curl_dynhds_reset(संरचना dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_गणनम्(संरचना dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
शून्य Curl_dynhds_set_opts(संरचना dynhds *dynhds, पूर्णाङ्क opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
संरचना dynhds_entry *Curl_dynhds_getn(संरचना dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
संरचना dynhds_entry *Curl_dynhds_get(संरचना dynhds *dynhds,
                                     ध्रुव वर्ण *name, size_t namelen);
संरचना dynhds_entry *Curl_dynhds_cget(संरचना dynhds *dynhds, ध्रुव वर्ण *name);

CURLcode Curl_dynhds_cset(संरचना dynhds *dynhds,
                          ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_add(संरचना dynhds *dynhds,
                         ध्रुव वर्ण *name, size_t namelen,
                         ध्रुव वर्ण *मानम्, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(संरचना dynhds *dynhds,
                          ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_पङ्क्तिः(संरचना dynhds *dynhds, ध्रुव वर्ण *पङ्क्तिः);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_add_पङ्क्तिः(संरचना dynhds *dynhds,
                                 ध्रुव वर्ण *पङ्क्तिः, size_t पङ्क्तिः_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(संरचना dynhds *dynhds, संरचना dynbuf *dbuf);

#ifdef USE_NGHTTP2

#अन्तर्भाव <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_to_nva(संरचना dynhds *dynhds, size_t *pcount);

#endif /* USE_NGHTTP2 */

#endif /* HEADER_CURL_DYNHDS_H */
