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

#ಆದರೆ defined(_WIN32) && defined(UNICODE)

#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

/*
 * MultiByte conversions using Windows kernel32 library.
 */

wchar_t *curlx_convert_UTF8_ಗಮ್ಯ_wchar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str_utf8)
{
  wchar_t *str_w = ಖಾಲಿ;

  ಆದರೆ(str_utf8) {
    ಪೂರ್ಣಾಂಕ str_w_len = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ಖಾಲಿ, 0);
    ಆದರೆ(str_w_len > 0) {
      str_w = curlx_malloc(str_w_len * ಗಾತ್ರ(wchar_t));
      ಆದರೆ(str_w) {
        ಆದರೆ(Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          curlx_free(str_w);
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
      }
    }
  }

  ಹಿಂತಿರುಗಿ str_w;
}

ಅಕ್ಷರ *curlx_convert_wchar_ಗಮ್ಯ_UTF8(ಸ್ಥಿರಾಂಕ wchar_t *str_w)
{
  ಅಕ್ಷರ *str_utf8 = ಖಾಲಿ;

  ಆದರೆ(str_w) {
    ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, str_w, -1,
                                    ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(ಬೈಟುಗಳು > 0) {
      str_utf8 = curlx_malloc(ಬೈಟುಗಳು);
      ಆದರೆ(str_utf8) {
        ಆದರೆ(Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, str_w, -1, str_utf8, ಬೈಟುಗಳು,
                               ಖಾಲಿ, ಖಾಲಿ) == 0) {
          curlx_free(str_utf8);
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
      }
    }
  }

  ಹಿಂತಿರುಗಿ str_utf8;
}

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 && UNICODE */
