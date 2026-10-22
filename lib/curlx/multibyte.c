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

#إذا defined(_WIN32) && defined(UNICODE)

#ضمن "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_منزل_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = عدم;

  إذا(str_utf8) {
    صحيح str_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, عدم, 0);
    إذا(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * حجم(wchar_t));
      إذا(str_w) {
        إذا(Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          أرجع عدم;
        }
      }
    }
  }

  أرجع str_w;
}

حرف *curlx_convert_wchar_منزل_UTF8(ثابت wchar_t *str_w)
{
  حرف *str_utf8 = عدم;

  إذا(str_w) {
    صحيح بائيٽون = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, -1,
                                    عدم, 0, عدم, عدم);
    إذا(بائيٽون > 0) {
      str_utf8 = curlx_malloc(بائيٽون);
      إذا(str_utf8) {
        إذا(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, -1, str_utf8, بائيٽون,
                               عدم, عدم) == 0) {
          curlx_free(str_utf8);
          أرجع عدم;
        }
      }
    }
  }

  أرجع str_utf8;
}

#جيڪڏهن_پورو /* _WIN32 && UNICODE */
