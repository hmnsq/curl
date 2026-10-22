#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_WARNLESS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_WARNLESS_H
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

#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FUNCTION_CAST(ಗುರಿ_type, func) \
  (ಗುರಿ_type)(ಶೂನ್ಯ (*)(ಶೂನ್ಯ))(func)

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ curlx_ultouc(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulnum);

ಪೂರ್ಣಾಂಕ curlx_uztosi(size_t uznum);

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ curlx_uztoul(size_t uznum);

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curlx_uztoui(size_t uznum);

ಪೂರ್ಣಾಂಕ curlx_sltosi(ದೀರ್ಘ slnum);

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curlx_sltoui(ದೀರ್ಘ slnum);

ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ curlx_sltous(ದೀರ್ಘ slnum);

ssize_t curlx_uztosz(size_t uznum);

size_t curlx_sotouz(curl_off_t sonum);

ಪೂರ್ಣಾಂಕ curlx_sztosi(ssize_t sznum);

ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ curlx_uitous(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uinum);

size_t curlx_sitouz(ಪೂರ್ಣಾಂಕ sinum);

size_t curlx_uitouz(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(size_t uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, size_t *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_ಹೊಂದುತ್ತದೆ(curl_off_t sonum, size_t *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(ದೀರ್ಘ slnum, size_t *puznum);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_WARNLESS_H */
