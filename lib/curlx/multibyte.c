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
#अन्तर्भाव "curl_setup.h"

#जइ defined(_WIN32) && defined(UNICODE)

#अन्तर्भाव "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(ध्रुव वर्ण *str_utf8)
{
  wchar_t *str_w = रिक्त;

  जइ(str_utf8) {
    पूर्णाङ्क str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, रिक्त, 0);
    जइ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * परिमाण(wchar_t));
      जइ(str_w) {
        जइ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          प्रत्यागम रिक्त;
        }
      }
    }
  }

  प्रत्यागम str_w;
}

वर्ण *curlx_convert_wchar_to_UTF8(ध्रुव wchar_t *str_w)
{
  वर्ण *str_utf8 = रिक्त;

  जइ(str_w) {
    पूर्णाङ्क bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    रिक्त, 0, रिक्त, रिक्त);
    जइ(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      जइ(str_utf8) {
        जइ(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               रिक्त, रिक्त) == 0) {
          curlx_free(str_utf8);
          प्रत्यागम रिक्त;
        }
      }
    }
  }

  प्रत्यागम str_utf8;
}

#endif /* _WIN32 && UNICODE */
