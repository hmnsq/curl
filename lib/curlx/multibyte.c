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
#include "curl_setup.h"

#ប្រសិនបើ defined(_WIN32) && defined(UNICODE)

#include "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(const var_na *str_utf8)
{
  wchar_t *str_w = NULL;

  ប្រសិនបើ(str_utf8) {
    ចំនួនគត់ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, NULL, 0);
    ប្រសិនបើ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * parimaa_na(wchar_t));
      ប្រសិនបើ(str_w) {
        ប្រសិនបើ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ត្រឡប់ NULL;
        }
      }
    }
  }

  ត្រឡប់ str_w;
}

var_na *curlx_convert_wchar_to_UTF8(const wchar_t *str_w)
{
  var_na *str_utf8 = NULL;

  ប្រសិនបើ(str_w) {
    ចំនួនគត់ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    NULL, 0, NULL, NULL);
    ប្រសិនបើ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      ប្រសិនបើ(str_utf8) {
        ប្រសិនបើ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               NULL, NULL) == 0) {
          curlx_free(str_utf8);
          ត្រឡប់ NULL;
        }
      }
    }
  }

  ត្រឡប់ str_utf8;
}

#endif /* _WIN32 && UNICODE */
