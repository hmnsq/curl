#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_MULTIBYTE_H
#وصف_ڪريو سرو_CURL_MULTIBYTE_H
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

#جيڪڏهن_وصف_ٿيل _WIN32

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

#جيڪڏهن_وصف_ٿيل UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_منزل_wchar(ثابت حرف *str_utf8);
حرف *curlx_convert_wchar_منزل_UTF8(ثابت wchar_t *str_w);

#وصف_ڪريو curlx_convert_UTF8_منزل_tchar(ptr) curlx_convert_UTF8_منزل_wchar(ptr)
#وصف_ڪريو curlx_convert_tchar_منزل_UTF8(ptr) curlx_convert_wchar_منزل_UTF8(ptr)

تعريفنوع اتحاد {
  بنانشان ننڍو       *tchar_ptr;
  ثابت بنانشان ننڍو *const_tchar_ptr;
  بنانشان ننڍو       *tbyte_ptr;
  ثابت بنانشان ننڍو *const_tbyte_ptr;
} xcharp_u;

#وإلا /* !UNICODE */

#وصف_ڪريو curlx_convert_UTF8_منزل_tchar(ptr) curlx_strdup(ptr)
#وصف_ڪريو curlx_convert_tchar_منزل_UTF8(ptr) curlx_strdup(ptr)

تعريفنوع اتحاد {
  حرف                *tchar_ptr;
  ثابت حرف          *const_tchar_ptr;
  بنانشان حرف       *tbyte_ptr;
  ثابت بنانشان حرف *const_tbyte_ptr;
} xcharp_u;

#جيڪڏهن_پورو /* UNICODE */
#جيڪڏهن_پورو /* _WIN32 */

#جيڪڏهن_پورو /* HEADER_CURL_MULTIBYTE_H */
