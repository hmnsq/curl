#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_IMAP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_IMAP_H
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

/* Authentication type flags */
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_TYPE_CLEARTEXT (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_TYPE_SASL      (1 << 1)

/* Authentication type values */
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_TYPE_NONE      0
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_TYPE_ANY       (IMAP_TYPE_CLEARTEXT | IMAP_TYPE_SASL)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_IMAP
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_imap;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_IMAP_H */
