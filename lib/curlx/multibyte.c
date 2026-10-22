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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(_WIN32) && defined(UNICODE)

#అన్తర్భావ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_గమ్యం_wchar(స్థిరరాశి అక్షరం *str_utf8)
{
  wchar_t *str_w = ఖాళీ;

  ఒకవేళ(str_utf8) {
    పూర్ణాంకం str_w_len = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ఖాళీ, 0);
    ఒకవేళ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * పరిమాణం(wchar_t));
      ఒకవేళ(str_w) {
        ఒకవేళ(Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          తిరిగి ఖాళీ;
        }
      }
    }
  }

  తిరిగి str_w;
}

అక్షరం *curlx_convert_wchar_గమ్యం_UTF8(స్థిరరాశి wchar_t *str_w)
{
  అక్షరం *str_utf8 = ఖాళీ;

  ఒకవేళ(str_w) {
    పూర్ణాంకం బైట్లు = Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, str_w, -1,
                                    ఖాళీ, 0, ఖాళీ, ఖాళీ);
    ఒకవేళ(బైట్లు > 0) {
      str_utf8 = curlx_malloc(బైట్లు);
      ఒకవేళ(str_utf8) {
        ఒకవేళ(Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, str_w, -1, str_utf8, బైట్లు,
                               ఖాళీ, ఖాళీ) == 0) {
          curlx_free(str_utf8);
          తిరిగి ఖాళీ;
        }
      }
    }
  }

  తిరిగి str_utf8;
}

#అయితే_ముగింపు /* _WIN32 && UNICODE */
