#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_DYNHDS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_DYNHDS_H
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

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

ರಚನೆ dynbuf;

/**
 * A single header entry.
 * `name` and `value` are non-NULL and always null-terminated.
 */
ರಚನೆ dynhds_ನಮೂದು {
  ಅಕ್ಷರ *name;
  ಅಕ್ಷರ *ಮೌಲ್ಯ;
  size_t namelen;
  size_t valuelen;
};

ರಚನೆ dynhds {
  ರಚನೆ dynhds_ನಮೂದು **hds;
  size_t hds_len;   /* number of entries in hds */
  size_t hds_allc;  /* size of hds allocation */
  size_t max_entries;   /* size limit number of entries */
  size_t strs_len; /* length of all strings */
  size_t max_strs_ಗಾತ್ರ; /* max length of all strings */
  ಪೂರ್ಣಾಂಕ opts;
};

#ವ್ಯಾಖ್ಯಾನಿಸು DYNHDS_OPT_NONE          0
#ವ್ಯಾಖ್ಯಾನಿಸು DYNHDS_OPT_LOWERCASE     (1 << 0)

/**
 * Init for use on first time or after a reset.
 * Allow `max_entries` headers to be added, 0 for unlimited.
 * Allow size of all name and values added to not exceed `max_strs_size``
 */
ಶೂನ್ಯ Curl_dynhds_init(ರಚನೆ dynhds *dynhds, size_t max_entries,
                      size_t max_strs_ಗಾತ್ರ);
/**
 * Frees all data held in `dynhds`, but not the struct itself.
 */
ಶೂನ್ಯ Curl_dynhds_free(ರಚನೆ dynhds *dynhds);

/**
 * Reset `dyndns` to the initial init state. May keep allocations
 * around.
 */
ಶೂನ್ಯ Curl_dynhds_reset(ರಚನೆ dynhds *dynhds);

/**
 * Return the number of header entries.
 */
size_t Curl_dynhds_ಎಣಿಕೆ(ರಚನೆ dynhds *dynhds);

/**
 * Set the options to use, replacing any existing ones.
 * This will not have an effect on already existing headers.
 */
ಶೂನ್ಯ Curl_dynhds_ಗಣ_opts(ರಚನೆ dynhds *dynhds, ಪೂರ್ಣಾಂಕ opts);

/**
 * Return the n-th header entry or NULL if it does not exist.
 */
ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_getn(ರಚನೆ dynhds *dynhds, size_t n);

/**
 * Return the 1st header entry of the name or NULL if none exists.
 */
ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_get(ರಚನೆ dynhds *dynhds,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen);
ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_cget(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

CURLcode Curl_dynhds_cset(ರಚನೆ dynhds *dynhds,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

/**
 * Add a header, name + value, to `dynhds` at the end. Does *not*
 * check for duplicate names.
 */
CURLcode Curl_dynhds_ಕೂಡಿಸು(ರಚನೆ dynhds *dynhds,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t valuelen);

/**
 * Add a header, c-string name + value, to `dynhds` at the end.
 */
CURLcode Curl_dynhds_cadd(ರಚನೆ dynhds *dynhds,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_cadd_ಸಾಲು(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು);

/**
 * Add a single header from an HTTP/1.1 formatted line at the end. Line may
 * contain a delimiting CRLF or LF. Any characters after that will be ignored.
 */
CURLcode Curl_dynhds_h1_ಕೂಡಿಸು_ಸಾಲು(ರಚನೆ dynhds *dynhds,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t ಸಾಲು_len);

/**
 * Add the headers to the given `dynbuf` in HTTP/1.1 format with
 * CR+LF line endings. Does NOT output a last empty line.
 */
CURLcode Curl_dynhds_h1_dprint(ರಚನೆ dynhds *dynhds, ರಚನೆ dynbuf *dbuf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2

#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>

nghttp2_nv *Curl_dynhds_ಗಮ್ಯ_nva(ರಚನೆ dynhds *dynhds, size_t *pcount);

#ಆದರೆ_ಅಂತ್ಯ /* USE_NGHTTP2 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_DYNHDS_H */
