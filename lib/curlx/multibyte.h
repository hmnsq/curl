#ifndef HEADER_CURL_MULTIBYTE_H
#define HEADER_CURL_MULTIBYTE_H
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
#அந்தர்பாவ "curl_setup.h"

#ifdef _WIN32

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

#ifdef UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_to_wchar(மாறிலி எழுத்து *str_utf8);
எழுத்து *curlx_convert_wchar_to_UTF8(மாறிலி wchar_t *str_w);

#define curlx_convert_UTF8_to_tchar(ptr) curlx_convert_UTF8_to_wchar(ptr)
#define curlx_convert_tchar_to_UTF8(ptr) curlx_convert_wchar_to_UTF8(ptr)

வகைவரை ஒன்றிப்பு {
  unsigned short       *tchar_ptr;
  மாறிலி unsigned short *const_tchar_ptr;
  unsigned short       *tbyte_ptr;
  மாறிலி unsigned short *const_tbyte_ptr;
} xcharp_u;

#இல்லையெனில் /* !UNICODE */

#define curlx_convert_UTF8_to_tchar(ptr) curlx_strdup(ptr)
#define curlx_convert_tchar_to_UTF8(ptr) curlx_strdup(ptr)

வகைவரை ஒன்றிப்பு {
  எழுத்து                *tchar_ptr;
  மாறிலி எழுத்து          *const_tchar_ptr;
  unsigned எழுத்து       *tbyte_ptr;
  மாறிலி unsigned எழுத்து *const_tbyte_ptr;
} xcharp_u;

#endif /* UNICODE */
#endif /* _WIN32 */

#endif /* HEADER_CURL_MULTIBYTE_H */
