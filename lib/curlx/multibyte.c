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

wchar_t *curlx_convert_UTF8_to_wchar(ثابت نویسه *str_utf8)
{
  wchar_t *str_w = پوچ;

  اگر(str_utf8) {
    صحیح str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, پوچ, 0);
    اگر(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * اندازه(wchar_t));
      اگر(str_w) {
        اگر(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          بازگشت پوچ;
        }
      }
    }
  }

  بازگشت str_w;
}

نویسه *curlx_convert_wchar_to_UTF8(ثابت wchar_t *str_w)
{
  نویسه *str_utf8 = پوچ;

  اگر(str_w) {
    صحیح bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    پوچ, 0, پوچ, پوچ);
    اگر(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      اگر(str_utf8) {
        اگر(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               پوچ, پوچ) == 0) {
          curlx_free(str_utf8);
          بازگشت پوچ;
        }
      }
    }
  }

  بازگشت str_utf8;
}

#endif /* _WIN32 && UNICODE */
