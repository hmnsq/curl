#यदि_अपरिभाषितम् शीर्षकम्_CURL_संस्करणम्_WIN32_H
#परिभाषय शीर्षकम्_CURL_संस्करणम्_WIN32_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् _WIN32

/* Version condition */
प्रकारनिर्देश गणना {
  संस्करणम्_LESS_THAN,
  संस्करणम्_LESS_THAN_EQUAL,
  संस्करणम्_EQUAL,
  संस्करणम्_GREATER_THAN_EQUAL,
  संस्करणम्_GREATER_THAN
} संस्करणम्_Condition;

/* Platform identifier */
प्रकारनिर्देश गणना {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#यदि_परिभाषितम् CURL_WINDOWS_UWP
#परिभाषय curlx_verify_windows_init() Curl_nop_stmt
#अन्यथा
शून्य curlx_verify_windows_init(शून्य);
#यदि_अन्तः
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_संस्करणम्(ध्रुव अचिह्नित पूर्णाङ्क major_संस्करणम्,
                                  ध्रुव अचिह्नित पूर्णाङ्क minor_संस्करणम्,
                                  ध्रुव अचिह्नित पूर्णाङ्क build_संस्करणम्,
                                  ध्रुव PlatformIdentifier platform,
                                  ध्रुव संस्करणम्_Condition condition);

#यदि_अन्तः /* _WIN32 */

#यदि_अन्तः /* HEADER_CURL_VERSION_WIN32_H */
