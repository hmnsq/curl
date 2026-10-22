#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ESCAPE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ESCAPE_H
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
/* Escape and unescape URL encoding in strings. The functions return a new
 * allocated string or NULL if an error occurred. */

ಎಣಿಕೆ urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, size_t ಉದ್ದಳತೆ,
                        ಅಕ್ಷರ **ostring, size_t *olen,
                        ಎಣಿಕೆ urlreject ctrl);

ಶೂನ್ಯ Curl_hexencode(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *src, size_t len, /* input length */
                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು, size_t olen); /* output buffer size */

ಶೂನ್ಯ Curl_hexbyte(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest, /* must fit two bytes */
                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ val);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_ESCAPE_H */
