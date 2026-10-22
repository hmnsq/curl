#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MULTIBYTE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MULTIBYTE_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

/*
 * Macros curlx_convert_UTF8_to_tchar(), curlx_convert_tchar_to_UTF8()
 * main purpose is to minimize the number of preprocessor conditional
 * directives needed by code using these to differentiate Unicode from
 * non-Unicode builds.
 *
 * In the case of a non-Unicode build the tchar strings are char strings that
 * are duplicated via strdup and remain in whatever the passed in encoding is,
 * which is assumed to be UTF-8 but may be other encoding. Therefore the
 * significance of the conversion functions is primarily for Unicode builds.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_ಗಮ್ಯ_wchar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str_utf8);
ಅಕ್ಷರ *curlx_convert_wchar_ಗಮ್ಯ_UTF8(ಸ್ಥಿರಾಂಕ wchar_t *str_w);

#ವ್ಯಾಖ್ಯಾನಿಸು curlx_convert_UTF8_ಗಮ್ಯ_tchar(ptr) curlx_convert_UTF8_ಗಮ್ಯ_wchar(ptr)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_convert_tchar_ಗಮ್ಯ_UTF8(ptr) curlx_convert_wchar_ಗಮ್ಯ_UTF8(ptr)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ       *tchar_ptr;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *const_tchar_ptr;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ       *tbyte_ptr;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *const_tbyte_ptr;
} xcharp_u;

#ಇಲ್ಲದಿದ್ದರೆ /* !UNICODE */

#ವ್ಯಾಖ್ಯಾನಿಸು curlx_convert_UTF8_ಗಮ್ಯ_tchar(ptr) curlx_strdup(ptr)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_convert_tchar_ಗಮ್ಯ_UTF8(ptr) curlx_strdup(ptr)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
  ಅಕ್ಷರ                *tchar_ptr;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ          *const_tchar_ptr;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ       *tbyte_ptr;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *const_tbyte_ptr;
} xcharp_u;

#ಆದರೆ_ಅಂತ್ಯ /* UNICODE */
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MULTIBYTE_H */
