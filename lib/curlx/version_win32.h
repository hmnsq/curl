#अगर_अघोषित HEADER_CURL_VERSION_WIN32_H
#घोषणा HEADER_CURL_VERSION_WIN32_H
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
#समावेश "curl_setup.h"

#अगर_घोषित _WIN32

/* Version condition */
प्रकार क्रमागत {
  VERSION_LESS_THAN,
  VERSION_LESS_THAN_EQUAL,
  VERSION_EQUAL,
  VERSION_GREATER_THAN_EQUAL,
  VERSION_GREATER_THAN
} VersionCondition;

/* Platform identifier */
प्रकार क्रमागत {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#अगर_घोषित CURL_WINDOWS_UWP
#घोषणा curlx_verify_windows_init() Curl_nop_stmt
#अन्यथा
व्योम curlx_verify_windows_init(व्योम);
#पूर्ण_अगर
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_version(स्थिर अचिन्हित पूर्णांक majorVersion,
                                  स्थिर अचिन्हित पूर्णांक minorVersion,
                                  स्थिर अचिन्हित पूर्णांक buildVersion,
                                  स्थिर PlatformIdentifier platform,
                                  स्थिर VersionCondition condition);

#पूर्ण_अगर /* _WIN32 */

#पूर्ण_अगर /* HEADER_CURL_VERSION_WIN32_H */
