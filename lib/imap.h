#اگر_تعریف_نشده سرآیند_CURL_IMAP_H
#تعریف_کن سرآیند_CURL_IMAP_H
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
#تعریف_کن IMAP_TYPE_CLEARTEXT (1 << 0)
#تعریف_کن IMAP_TYPE_SASL      (1 << 1)

/* Authentication type values */
#تعریف_کن IMAP_TYPE_NONE      0
#تعریف_کن IMAP_TYPE_ANY       (IMAP_TYPE_CLEARTEXT | IMAP_TYPE_SASL)

#اگر_تعریف_نشده CURL_DISABLE_IMAP
خارجي ثابت بنية Curl_protocol Curl_protocol_imap;
#پایان_اگر

#پایان_اگر /* HEADER_CURL_IMAP_H */
