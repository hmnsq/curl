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

#ຖ້າ defined(_WIN32) && defined(UNICODE)

#include "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(const var_na *str_utf8)
{
  wchar_t *str_w = NULL;

  ຖ້າ(str_utf8) {
    ຈຳນວນເຕັມ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, NULL, 0);
    ຖ້າ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * parimaa_na(wchar_t));
      ຖ້າ(str_w) {
        ຖ້າ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ສົ່ງຄືນ NULL;
        }
      }
    }
  }

  ສົ່ງຄືນ str_w;
}

var_na *curlx_convert_wchar_to_UTF8(const wchar_t *str_w)
{
  var_na *str_utf8 = NULL;

  ຖ້າ(str_w) {
    ຈຳນວນເຕັມ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    NULL, 0, NULL, NULL);
    ຖ້າ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      ຖ້າ(str_utf8) {
        ຖ້າ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               NULL, NULL) == 0) {
          curlx_free(str_utf8);
          ສົ່ງຄືນ NULL;
        }
      }
    }
  }

  ສົ່ງຄືນ str_utf8;
}

#endif /* _WIN32 && UNICODE */
