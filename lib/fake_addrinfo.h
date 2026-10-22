#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_FAKE_ADDRINFO_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_FAKE_ADDRINFO_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_ಆವೃತ್ತಿ >= 0x011a00) /* >= 1.26.0 */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_FAKE_GETADDRINFO 1
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FAKE_GETADDRINFO

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#  include <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ r_freeaddrinfo(ರಚನೆ addrinfo *cahead);
ಪೂರ್ಣಾಂಕ r_getaddrinfo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗಂಟು,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service,
                  ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                  ರಚನೆ addrinfo **res);
#ಆದರೆ_ಅಂತ್ಯ /* USE_FAKE_GETADDRINFO */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_FAKE_ADDRINFO_H */
