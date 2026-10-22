#अगर_अघोषित HEADER_CURL_MULTIBYTE_H
#घोषणा HEADER_CURL_MULTIBYTE_H
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

#अगर_घोषित _WIN32

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

#अगर_घोषित UNICODE

/* MultiByte conversions using Windows kernel32 library. */
ब_अक्षर_प्रकार *curlx_convert_UTF8_to_wchar(स्थिर अक्षर *str_utf8);
अक्षर *curlx_convert_wchar_to_UTF8(स्थिर ब_अक्षर_प्रकार *str_w);

#घोषणा curlx_convert_UTF8_to_tchar(ptr) curlx_convert_UTF8_to_wchar(ptr)
#घोषणा curlx_convert_tchar_to_UTF8(ptr) curlx_convert_wchar_to_UTF8(ptr)

प्रकार जोड़ {
  अचिन्हित लघु       *tchar_ptr;
  स्थिर अचिन्हित लघु *const_tchar_ptr;
  अचिन्हित लघु       *tbyte_ptr;
  स्थिर अचिन्हित लघु *const_tbyte_ptr;
} xcharp_u;

#अन्यथा /* !UNICODE */

#घोषणा curlx_convert_UTF8_to_tchar(ptr) curlx_strdup(ptr)
#घोषणा curlx_convert_tchar_to_UTF8(ptr) curlx_strdup(ptr)

प्रकार जोड़ {
  अक्षर                *tchar_ptr;
  स्थिर अक्षर          *const_tchar_ptr;
  अचिन्हित अक्षर       *tbyte_ptr;
  स्थिर अचिन्हित अक्षर *const_tbyte_ptr;
} xcharp_u;

#पूर्ण_अगर /* UNICODE */
#पूर्ण_अगर /* _WIN32 */

#पूर्ण_अगर /* HEADER_CURL_MULTIBYTE_H */
