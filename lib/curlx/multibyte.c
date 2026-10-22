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
#شامل "curl_setup.h"

#اگر defined(_WIN32) && defined(UNICODE)

#شامل "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_مقصد_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = تہی;

  اگر(str_utf8) {
    صحیح str_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, تہی, 0);
    اگر(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * حجم(wchar_t));
      اگر(str_w) {
        اگر(Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          واپس تہی;
        }
      }
    }
  }

  واپس str_w;
}

حرف *curlx_convert_wchar_مقصد_UTF8(ثابت wchar_t *str_w)
{
  حرف *str_utf8 = تہی;

  اگر(str_w) {
    صحیح بائٹیں = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, -1,
                                    تہی, 0, تہی, تہی);
    اگر(بائٹیں > 0) {
      str_utf8 = curlx_malloc(بائٹیں);
      اگر(str_utf8) {
        اگر(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, -1, str_utf8, بائٹیں,
                               تہی, تہی) == 0) {
          curlx_free(str_utf8);
          واپس تہی;
        }
      }
    }
  }

  واپس str_utf8;
}

#اگر_ختم /* _WIN32 && UNICODE */
