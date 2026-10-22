#اگر_تعریف_نشده سرآیند_CURL_نسخه_WIN32_H
#تعریف_کن سرآیند_CURL_نسخه_WIN32_H
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

#اگر_تعریف_شده _WIN32

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

#اگر_تعریف_شده CURL_WINDOWS_UWP
#تعریف_کن curlx_verify_windows_init() Curl_nop_stmt
#وإلا
فراغ curlx_verify_windows_init(فراغ);
#پایان_اگر
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_نسخه(ثابت بیعلامت صحيح major_نسخه,
                                  ثابت بیعلامت صحيح minor_نسخه,
                                  ثابت بیعلامت صحيح build_نسخه,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخه_Condition condition);

#پایان_اگر /* _WIN32 */

#پایان_اگر /* HEADER_CURL_VERSION_WIN32_H */
