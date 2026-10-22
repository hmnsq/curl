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

#ᨀᨘ defined(_WIN32) && defined(UNICODE)

#include "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(const var_na *str_utf8)
{
  wchar_t *str_w = NULL;

  ᨀᨘ(str_utf8) {
    puur_naa_nka str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, NULL, 0);
    ᨀᨘ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * parimaa_na(wchar_t));
      ᨀᨘ(str_w) {
        ᨀᨘ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ᨀᨘᨒ NULL;
        }
      }
    }
  }

  ᨀᨘᨒ str_w;
}

var_na *curlx_convert_wchar_to_UTF8(const wchar_t *str_w)
{
  var_na *str_utf8 = NULL;

  ᨀᨘ(str_w) {
    puur_naa_nka bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    NULL, 0, NULL, NULL);
    ᨀᨘ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      ᨀᨘ(str_utf8) {
        ᨀᨘ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               NULL, NULL) == 0) {
          curlx_free(str_utf8);
          ᨀᨘᨒ NULL;
        }
      }
    }
  }

  ᨀᨘᨒ str_utf8;
}

#endif /* _WIN32 && UNICODE */
