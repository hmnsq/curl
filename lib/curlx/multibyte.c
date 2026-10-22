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
#समावेश "curl_setup.h"

#अगर defined(_WIN32) && defined(UNICODE)

#समावेश "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

ब_अक्षर_प्रकार *curlx_convert_UTF8_to_wchar(स्थिर अक्षर *str_utf8)
{
  ब_अक्षर_प्रकार *str_w = शून्य;

  अगर(str_utf8) {
    पूर्णांक str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, शून्य, 0);
    अगर(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * माप(ब_अक्षर_प्रकार));
      अगर(str_w) {
        अगर(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          वापस शून्य;
        }
      }
    }
  }

  वापस str_w;
}

अक्षर *curlx_convert_wchar_to_UTF8(स्थिर ब_अक्षर_प्रकार *str_w)
{
  अक्षर *str_utf8 = शून्य;

  अगर(str_w) {
    पूर्णांक bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, -1,
                                    शून्य, 0, शून्य, शून्य);
    अगर(bytes > 0) {
      str_utf8 = curlx_malloc(bytes);
      अगर(str_utf8) {
        अगर(WideCharToMultiByte(CP_UTF8, 0, str_w, -1, str_utf8, bytes,
                               शून्य, शून्य) == 0) {
          curlx_free(str_utf8);
          वापस शून्य;
        }
      }
    }
  }

  वापस str_utf8;
}

#पूर्ण_अगर /* _WIN32 && UNICODE */
