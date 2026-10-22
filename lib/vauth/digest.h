#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_DIGEST_H
#وصف_ڪريو سرو_CURL_DIGEST_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH

#وصف_ڪريو DIGEST_MAX_قدر_ڊيگهائي           256
#وصف_ڪريو DIGEST_MAX_CONTENT_ڊيگهائي         1024

بنية Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(ثابت حرف *str, بنية Curl_str *قدر,
                               حرف *content, ثابت حرف **endptr);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_DIGEST_H */
