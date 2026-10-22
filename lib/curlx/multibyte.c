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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(_WIN32) && defined(UNICODE)

#অন্তর্ভাব "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_to_wchar(ধ্রুবক অক্ষর *str_utf8)
{
  wchar_t *str_w = ফাঁকা;

  যদি(str_utf8) {
    পূর্ণসংখ্যা str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ফাঁকা, 0);
    যদি(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * আকার(wchar_t));
      যদি(str_w) {
        যদি(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ফেরত ফাঁকা;
        }
      }
    }
  }

  ফেরত str_w;
}

অক্ষর *curlx_convert_wchar_to_UTF8(ধ্রুবক wchar_t *str_w)
{
  অক্ষর *str_utf8 = ফাঁকা;

  যদি(str_w) {
    পূর্ণসংখ্যা bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    ফাঁকা, 0, ফাঁকা, ফাঁকা);
    যদি(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      যদি(str_utf8) {
        যদি(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               ফাঁকা, ফাঁকা) == 0) {
          curlx_free(str_utf8);
          ফেরত ফাঁকা;
        }
      }
    }
  }

  ফেরত str_utf8;
}

#endif /* _WIN32 && UNICODE */
