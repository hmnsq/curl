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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(_WIN32) && defined(UNICODE)

#അന്തര്ഭാവ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(സ്ഥിരം അക്ഷരം *str_utf8)
{
  wchar_t *str_w = ശൂന്യ;

  എങ്കിൽ(str_utf8) {
    പൂർണ്ണസംഖ്യ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ശൂന്യ, 0);
    എങ്കിൽ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * വലുപ്പം(wchar_t));
      എങ്കിൽ(str_w) {
        എങ്കിൽ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          തിരികെ ശൂന്യ;
        }
      }
    }
  }

  തിരികെ str_w;
}

അക്ഷരം *curlx_convert_wchar_to_UTF8(സ്ഥിരം wchar_t *str_w)
{
  അക്ഷരം *str_utf8 = ശൂന്യ;

  എങ്കിൽ(str_w) {
    പൂർണ്ണസംഖ്യ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    ശൂന്യ, 0, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      എങ്കിൽ(str_utf8) {
        എങ്കിൽ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               ശൂന്യ, ശൂന്യ) == 0) {
          curlx_free(str_utf8);
          തിരികെ ശൂന്യ;
        }
      }
    }
  }

  തിരികെ str_utf8;
}

#endif /* _WIN32 && UNICODE */
