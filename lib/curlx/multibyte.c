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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(_WIN32) && defined(UNICODE)

#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(ଅଚଳ ଅକ୍ଷର *str_utf8)
{
  wchar_t *str_w = ଖାଲି;

  ଯଦି(str_utf8) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ଖାଲି, 0);
    ଯଦି(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * ଆକାର(wchar_t));
      ଯଦି(str_w) {
        ଯଦି(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ଫେରନ୍ତୁ ଖାଲି;
        }
      }
    }
  }

  ଫେରନ୍ତୁ str_w;
}

ଅକ୍ଷର *curlx_convert_wchar_to_UTF8(ଅଚଳ wchar_t *str_w)
{
  ଅକ୍ଷର *str_utf8 = ଖାଲି;

  ଯଦି(str_w) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    ଖାଲି, 0, ଖାଲି, ଖାଲି);
    ଯଦି(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      ଯଦି(str_utf8) {
        ଯଦି(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               ଖାଲି, ଖାଲି) == 0) {
          curlx_free(str_utf8);
          ଫେରନ୍ତୁ ଖାଲି;
        }
      }
    }
  }

  ଫେରନ୍ତୁ str_utf8;
}

#endif /* _WIN32 && UNICODE */
