#اگر_غیر_تعریف_شدہ سرخی_CURL_CTYPE_H
#تعریف_کرو سرخی_CURL_CTYPE_H
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

#تعریف_کرو ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#تعریف_کرو ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#تعریف_کرو ISLOWCNTRL(x) ((غیرعلامتی حرف)(x) <= 0x1f)
#تعریف_کرو IS7F(x)       ((x) == 0x7f)

#تعریف_کرو ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#تعریف_کرو ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#تعریف_کرو ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#تعریف_کرو ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#تعریف_کرو ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#تعریف_کرو ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#تعریف_کرو ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#تعریف_کرو ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#تعریف_کرو ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#تعریف_کرو ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#تعریف_کرو ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#تعریف_کرو ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#تعریف_کرو ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#تعریف_کرو ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#تعریف_کرو ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#اگر_ختم /* HEADER_CURL_CTYPE_H */
