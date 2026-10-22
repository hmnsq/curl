#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_URLAPI_INT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_URLAPI_INT_H
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
#ಅನ್ತರ್ಭಾವ <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
ರಚನೆ Curl_URL {
  ಅಕ್ಷರ *scheme;
  ಅಕ್ಷರ *user;
  ಅಕ್ಷರ *password;
  ಅಕ್ಷರ *options; /* IMAP only? */
  ಅಕ್ಷರ *host;
  ಅಕ್ಷರ *zoneid; /* for numerical IPv6 addresses */
  ಅಕ್ಷರ *ಮಾರ್ಗ;
  ಅಕ್ಷರ *query;
  ಅಕ್ಷರ *fragment;
  uint16_t portnum; /* the numerical port if present */
  ಬಿಟ್(ಪೋರ್ಟ್_present);    /* to support missing port */
  ಬಿಟ್(query_present);    /* to support blank */
  ಬಿಟ್(fragment_present); /* to support blank */
  ಬಿಟ್(guessed_scheme);   /* when a URL without scheme is parsed */
};

#ವ್ಯಾಖ್ಯಾನಿಸು HOST_ERROR   (-1) /* out of memory */
#ವ್ಯಾಖ್ಯಾನಿಸು HOST_NAME    1
#ವ್ಯಾಖ್ಯಾನಿಸು HOST_IPV4    2
#ವ್ಯಾಖ್ಯಾನಿಸು HOST_IPV6    3

#ವ್ಯಾಖ್ಯಾನಿಸು QUERY_NO      2
#ವ್ಯಾಖ್ಯಾನಿಸು QUERY_NOT_YET 3 /* allow to change to query */
#ವ್ಯಾಖ್ಯಾನಿಸು QUERY_YES     4

size_t Curl_is_absolute_url(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, ಅಕ್ಷರ *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_ಗಣ_authority(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority);

CURLUcode Curl_junkscan(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, size_t *urllen, bool allowspace);

#ವ್ಯಾಖ್ಯಾನಿಸು U_CURLU_URLDECODE  (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLU_URLDECODE
#ವ್ಯಾಖ್ಯಾನಿಸು U_CURLU_ಮಾರ್ಗ_AS_IS (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLU_ಮಾರ್ಗ_AS_IS

bool Curl_url_same_origin(CURLU *ತಳ, CURLU *href);

CURLUcode Curl_url_get_ಪೋರ್ಟ್(CURLU *u, uint16_t *pport);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_URLAPI_INT_H */
