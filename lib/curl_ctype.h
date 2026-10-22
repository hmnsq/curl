#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_CTYPE_H
#وصف_ڪريو سرو_CURL_CTYPE_H
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

#وصف_ڪريو ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#وصف_ڪريو ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#وصف_ڪريو ISLOWCNTRL(x) ((بنانشان حرف)(x) <= 0x1f)
#وصف_ڪريو IS7F(x)       ((x) == 0x7f)

#وصف_ڪريو ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#وصف_ڪريو ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#وصف_ڪريو ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#وصف_ڪريو ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#وصف_ڪريو ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#وصف_ڪريو ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#وصف_ڪريو ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#وصف_ڪريو ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#وصف_ڪريو ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#وصف_ڪريو ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#وصف_ڪريو ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#وصف_ڪريو ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#وصف_ڪريو ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#وصف_ڪريو ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#وصف_ڪريو ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#جيڪڏهن_پورو /* HEADER_CURL_CTYPE_H */
