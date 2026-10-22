#جے_بنا_تعریف سرخی_CURL_نسخہ_WIN32_H
#تعریف_کرو سرخی_CURL_نسخہ_WIN32_H
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

#جے_تعریف_شدہ _WIN32

/* Version condition */
تعريفنوع تعداد {
  نسخہ_LESS_THAN,
  نسخہ_LESS_THAN_EQUAL,
  نسخہ_EQUAL,
  نسخہ_GREATER_THAN_EQUAL,
  نسخہ_GREATER_THAN
} نسخہ_Condition;

/* Platform identifier */
تعريفنوع تعداد {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#جے_تعریف_شدہ CURL_WINDOWS_UWP
#تعریف_کرو curlx_verify_windows_init() Curl_nop_stmt
#وإلا
فراغ curlx_verify_windows_init(فراغ);
#جے_انت
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_نسخہ(ثابت بنانشانی صحيح major_نسخہ,
                                  ثابت بنانشانی صحيح minor_نسخہ,
                                  ثابت بنانشانی صحيح build_نسخہ,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخہ_Condition condition);

#جے_انت /* _WIN32 */

#جے_انت /* HEADER_CURL_VERSION_WIN32_H */
