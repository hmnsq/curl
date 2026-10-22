#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_نسخو_WIN32_H
#وصف_ڪريو سرو_CURL_نسخو_WIN32_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل _WIN32

/* Version condition */
تعريفنوع تعداد {
  نسخو_LESS_THAN,
  نسخو_LESS_THAN_EQUAL,
  نسخو_EQUAL,
  نسخو_GREATER_THAN_EQUAL,
  نسخو_GREATER_THAN
} نسخو_Condition;

/* Platform identifier */
تعريفنوع تعداد {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#جيڪڏهن_وصف_ٿيل CURL_WINDOWS_UWP
#وصف_ڪريو curlx_verify_windows_init() Curl_nop_stmt
#وإلا
فراغ curlx_verify_windows_init(فراغ);
#جيڪڏهن_پورو
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_نسخو(ثابت بنانشان صحيح major_نسخو,
                                  ثابت بنانشان صحيح minor_نسخو,
                                  ثابت بنانشان صحيح build_نسخو,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخو_Condition condition);

#جيڪڏهن_پورو /* _WIN32 */

#جيڪڏهن_پورو /* HEADER_CURL_VERSION_WIN32_H */
