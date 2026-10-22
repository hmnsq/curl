#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਸੰਸਕਰਣ_WIN32_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਸੰਸਕਰਣ_WIN32_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

/* Version condition */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ਸੰਸਕਰਣ_LESS_THAN,
  ਸੰਸਕਰਣ_LESS_THAN_EQUAL,
  ਸੰਸਕਰਣ_EQUAL,
  ਸੰਸਕਰਣ_GREATER_THAN_EQUAL,
  ਸੰਸਕਰਣ_GREATER_THAN
} ਸੰਸਕਰਣ_Condition;

/* Platform identifier */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WINDOWS_UWP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_verify_windows_init() Curl_nop_stmt
#ਨਹੀਂਤਾਂ
ਖਾਲੀ curlx_verify_windows_init(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_ਸੰਸਕਰਣ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major_ਸੰਸਕਰਣ,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor_ਸੰਸਕਰਣ,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ build_ਸੰਸਕਰਣ,
                                  ਅਚਲ PlatformIdentifier platform,
                                  ਅਚਲ ਸੰਸਕਰਣ_Condition condition);

#ਜੇ_ਅੰਤ /* _WIN32 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VERSION_WIN32_H */
