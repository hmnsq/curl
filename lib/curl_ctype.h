#اگر_تعریف_نشده سرآیند_CURL_CTYPE_H
#تعریف_کن سرآیند_CURL_CTYPE_H
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

#تعریف_کن ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#تعریف_کن ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#تعریف_کن ISLOWCNTRL(x) ((بیعلامت حرف)(x) <= 0x1f)
#تعریف_کن IS7F(x)       ((x) == 0x7f)

#تعریف_کن ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#تعریف_کن ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#تعریف_کن ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#تعریف_کن ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#تعریف_کن ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#تعریف_کن ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#تعریف_کن ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#تعریف_کن ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#تعریف_کن ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#تعریف_کن ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#تعریف_کن ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#تعریف_کن ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#تعریف_کن ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#تعریف_کن ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#تعریف_کن ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#پایان_اگر /* HEADER_CURL_CTYPE_H */
