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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(_WIN32) && defined(UNICODE)

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਅਚਲ ਅੱਖਰ *str_utf8)
{
  wchar_t *str_w = ਨਿਰਾ;

  ਜੇ(str_utf8) {
    ਪੂਰਨ str_w_len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ਨਿਰਾ, 0);
    ਜੇ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * ਆਕਾਰ(wchar_t));
      ਜੇ(str_w) {
        ਜੇ(Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ਵਾਪਸ ਨਿਰਾ;
        }
      }
    }
  }

  ਵਾਪਸ str_w;
}

ਅੱਖਰ *curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(ਅਚਲ wchar_t *str_w)
{
  ਅੱਖਰ *str_utf8 = ਨਿਰਾ;

  ਜੇ(str_w) {
    ਪੂਰਨ ਬਾਈਟਾਂ = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, str_w, -1,
                                    ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(ਬਾਈਟਾਂ > 0) {
      str_utf8 = curlx_malloc(ਬਾਈਟਾਂ);
      ਜੇ(str_utf8) {
        ਜੇ(Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, str_w, -1, str_utf8, ਬਾਈਟਾਂ,
                               ਨਿਰਾ, ਨਿਰਾ) == 0) {
          curlx_free(str_utf8);
          ਵਾਪਸ ਨਿਰਾ;
        }
      }
    }
  }

  ਵਾਪਸ str_utf8;
}

#ਜੇ_ਅੰਤ /* _WIN32 && UNICODE */
