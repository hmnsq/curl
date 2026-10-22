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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(_WIN32) && defined(UNICODE)

#அந்தர்பாவ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(மாறிலி எழுத்து *str_utf8)
{
  wchar_t *str_w = வெற்று;

  எனில்(str_utf8) {
    முழு str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, வெற்று, 0);
    எனில்(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * அளவு(wchar_t));
      எனில்(str_w) {
        எனில்(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          திரும்பு வெற்று;
        }
      }
    }
  }

  திரும்பு str_w;
}

எழுத்து *curlx_convert_wchar_to_UTF8(மாறிலி wchar_t *str_w)
{
  எழுத்து *str_utf8 = வெற்று;

  எனில்(str_w) {
    முழு bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    வெற்று, 0, வெற்று, வெற்று);
    எனில்(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      எனில்(str_utf8) {
        எனில்(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               வெற்று, வெற்று) == 0) {
          curlx_free(str_utf8);
          திரும்பு வெற்று;
        }
      }
    }
  }

  திரும்பு str_utf8;
}

#endif /* _WIN32 && UNICODE */
