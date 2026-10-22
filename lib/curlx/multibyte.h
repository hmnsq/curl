#اگر_تعریف_نشده سرآیند_CURL_MULTIBYTE_H
#تعریف_کن سرآیند_CURL_MULTIBYTE_H
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
#ضمن "curl_setup.h"

#اگر_تعریف_شده _WIN32

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

#اگر_تعریف_شده UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_مقصد_wchar(ثابت حرف *str_utf8);
حرف *curlx_convert_wchar_مقصد_UTF8(ثابت wchar_t *str_w);

#تعریف_کن curlx_convert_UTF8_مقصد_tchar(ptr) curlx_convert_UTF8_مقصد_wchar(ptr)
#تعریف_کن curlx_convert_tchar_مقصد_UTF8(ptr) curlx_convert_wchar_مقصد_UTF8(ptr)

تعريفنوع اتحاد {
  بیعلامت کوتاه       *tchar_ptr;
  ثابت بیعلامت کوتاه *const_tchar_ptr;
  بیعلامت کوتاه       *tbyte_ptr;
  ثابت بیعلامت کوتاه *const_tbyte_ptr;
} xcharp_u;

#وإلا /* !UNICODE */

#تعریف_کن curlx_convert_UTF8_مقصد_tchar(ptr) curlx_strdup(ptr)
#تعریف_کن curlx_convert_tchar_مقصد_UTF8(ptr) curlx_strdup(ptr)

تعريفنوع اتحاد {
  حرف                *tchar_ptr;
  ثابت حرف          *const_tchar_ptr;
  بیعلامت حرف       *tbyte_ptr;
  ثابت بیعلامت حرف *const_tbyte_ptr;
} xcharp_u;

#پایان_اگر /* UNICODE */
#پایان_اگر /* _WIN32 */

#پایان_اگر /* HEADER_CURL_MULTIBYTE_H */
