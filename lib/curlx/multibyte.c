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
#અન્તર્ભાવ "curl_setup.h"

#જો defined(_WIN32) && defined(UNICODE)

#અન્તર્ભાવ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(અચલ અક્ષર *str_utf8)
{
  wchar_t *str_w = ખાલી;

  જો(str_utf8) {
    પૂર્ણાંક str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ખાલી, 0);
    જો(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * કદ(wchar_t));
      જો(str_w) {
        જો(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          પાછા ખાલી;
        }
      }
    }
  }

  પાછા str_w;
}

અક્ષર *curlx_convert_wchar_to_UTF8(અચલ wchar_t *str_w)
{
  અક્ષર *str_utf8 = ખાલી;

  જો(str_w) {
    પૂર્ણાંક bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    ખાલી, 0, ખાલી, ખાલી);
    જો(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      જો(str_utf8) {
        જો(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               ખાલી, ખાલી) == 0) {
          curlx_free(str_utf8);
          પાછા ખાલી;
        }
      }
    }
  }

  પાછા str_utf8;
}

#endif /* _WIN32 && UNICODE */
