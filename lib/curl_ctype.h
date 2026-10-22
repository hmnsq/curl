#که_نه_تعریف_شوی سرلیک_CURL_CTYPE_H
#تعریف_کړه سرلیک_CURL_CTYPE_H
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

#تعریف_کړه ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#تعریف_کړه ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#تعریف_کړه ISLOWCNTRL(x) ((بینښه حرف)(x) <= 0x1f)
#تعریف_کړه IS7F(x)       ((x) == 0x7f)

#تعریف_کړه ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#تعریف_کړه ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#تعریف_کړه ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#تعریف_کړه ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#تعریف_کړه ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#تعریف_کړه ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#تعریف_کړه ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#تعریف_کړه ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#تعریف_کړه ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#تعریف_کړه ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#تعریف_کړه ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#تعریف_کړه ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#تعریف_کړه ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#تعریف_کړه ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#تعریف_کړه ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#د_که_پای /* HEADER_CURL_CTYPE_H */
