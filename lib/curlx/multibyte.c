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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(_WIN32) && defined(UNICODE)

#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(ಧ್ರುವ ವರ್ಣ *str_utf8)
{
  wchar_t *str_w = ರಿಕ್ತ;

  ಯದಿ(str_utf8) {
    ಪೂರ್ಣಾಙ್ಕ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ರಿಕ್ತ, 0);
    ಯದಿ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * ಪರಿಮಾಣ(wchar_t));
      ಯದಿ(str_w) {
        ಯದಿ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ str_w;
}

ವರ್ಣ *curlx_convert_wchar_to_UTF8(ಧ್ರುವ wchar_t *str_w)
{
  ವರ್ಣ *str_utf8 = ರಿಕ್ತ;

  ಯದಿ(str_w) {
    ಪೂರ್ಣಾಙ್ಕ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      ಯದಿ(str_utf8) {
        ಯದಿ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               ರಿಕ್ತ, ರಿಕ್ತ) == 0) {
          curlx_free(str_utf8);
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ str_utf8;
}

#endif /* _WIN32 && UNICODE */
