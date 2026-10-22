#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURLX_SNPRINTF_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURLX_SNPRINTF_H
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

/* Raw snprintf() for curlx */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
ಶೂನ್ಯ curlx_win32_snprintf(ಅಕ್ಷರ *buf, size_t maxlen, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
  CURL_PRINTF(3, 4);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITHOUT_LIBCURL /* when built for the test servers */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ವ್ಯಾಖ್ಯಾನಿಸು SNPRINTF curlx_win32_snprintf
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SNPRINTF snprintf
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ /* !WITHOUT_LIBCURL */
#ಅನ್ತರ್ಭಾವ <curl/mprintf.h>
#ವ್ಯಾಖ್ಯಾನಿಸು SNPRINTF curl_msnprintf
#ಆದರೆ_ಅಂತ್ಯ /* WITHOUT_LIBCURL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURLX_SNPRINTF_H */
