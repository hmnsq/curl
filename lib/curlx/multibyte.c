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
#ضمن "curl_setup.h"

#𐡄𐡍 defined(_WIN32) && defined(UNICODE)

#ضمن "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = عدم;

  𐡄𐡍(str_utf8) {
    صحيح str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, عدم, 0);
    𐡄𐡍(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * حجم(wchar_t));
      𐡄𐡍(str_w) {
        𐡄𐡍(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          أرجع عدم;
        }
      }
    }
  }

  أرجع str_w;
}

حرف *curlx_convert_wchar_to_UTF8(ثابت wchar_t *str_w)
{
  حرف *str_utf8 = عدم;

  𐡄𐡍(str_w) {
    صحيح bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    عدم, 0, عدم, عدم);
    𐡄𐡍(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      𐡄𐡍(str_utf8) {
        𐡄𐡍(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               عدم, عدم) == 0) {
          curlx_free(str_utf8);
          أرجع عدم;
        }
      }
    }
  }

  أرجع str_utf8;
}

#endif /* _WIN32 && UNICODE */
