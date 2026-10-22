#ifndef HEADER_CURL_VERSION_WIN32_H
#define HEADER_CURL_VERSION_WIN32_H
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

#ifdef _WIN32

/* Version condition */
प्रकारनिर्देश गणना {
  VERSION_LESS_THAN,
  VERSION_LESS_THAN_EQUAL,
  VERSION_EQUAL,
  VERSION_GREATER_THAN_EQUAL,
  VERSION_GREATER_THAN
} VersionCondition;

/* Platform identifier */
प्रकारनिर्देश गणना {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#ifdef CURL_WINDOWS_UWP
#define curlx_verify_windows_init() Curl_nop_stmt
#अन्यथा
शून्य curlx_verify_windows_init(शून्य);
#endif
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_version(ध्रुव unsigned पूर्णाङ्क majorVersion,
                                  ध्रुव unsigned पूर्णाङ्क minorVersion,
                                  ध्रुव unsigned पूर्णाङ्क buildVersion,
                                  ध्रुव PlatformIdentifier platform,
                                  ध्रुव VersionCondition condition);

#endif /* _WIN32 */

#endif /* HEADER_CURL_VERSION_WIN32_H */
