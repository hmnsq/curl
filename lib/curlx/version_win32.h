#که_نه_تعریف_شوی سرلیک_CURL_نسخه_WIN32_H
#تعریف_کړه سرلیک_CURL_نسخه_WIN32_H
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

#که_تعریف_شوی _WIN32

/* Version condition */
تعريفنوع تعداد {
  نسخه_LESS_THAN,
  نسخه_LESS_THAN_EQUAL,
  نسخه_EQUAL,
  نسخه_GREATER_THAN_EQUAL,
  نسخه_GREATER_THAN
} نسخه_Condition;

/* Platform identifier */
تعريفنوع تعداد {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#که_تعریف_شوی CURL_WINDOWS_UWP
#تعریف_کړه curlx_verify_windows_init() Curl_nop_stmt
#وإلا
فراغ curlx_verify_windows_init(فراغ);
#د_که_پای
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_نسخه(ثابت بینښه صحيح major_نسخه,
                                  ثابت بینښه صحيح minor_نسخه,
                                  ثابت بینښه صحيح build_نسخه,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخه_Condition condition);

#د_که_پای /* _WIN32 */

#د_که_پای /* HEADER_CURL_VERSION_WIN32_H */
