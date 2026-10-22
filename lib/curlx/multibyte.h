#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MULTIBYTE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MULTIBYTE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਅਚਲ ਅੱਖਰ *str_utf8);
ਅੱਖਰ *curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(ਅਚਲ wchar_t *str_w);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(ptr) curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ptr)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(ptr) curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(ptr)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ       *tchar_ptr;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *const_tchar_ptr;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ       *tbyte_ptr;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *const_tbyte_ptr;
} xcharp_u;

#ਨਹੀਂਤਾਂ /* !UNICODE */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(ptr) curlx_strdup(ptr)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(ptr) curlx_strdup(ptr)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
  ਅੱਖਰ                *tchar_ptr;
  ਅਚਲ ਅੱਖਰ          *const_tchar_ptr;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ       *tbyte_ptr;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *const_tbyte_ptr;
} xcharp_u;

#ਜੇ_ਅੰਤ /* UNICODE */
#ਜੇ_ਅੰਤ /* _WIN32 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_MULTIBYTE_H */
