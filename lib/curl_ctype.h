#यदि_अपरिभाषितम् शीर्षकम्_CURL_CTYPE_H
#परिभाषय शीर्षकम्_CURL_CTYPE_H
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

#परिभाषय ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#परिभाषय ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#परिभाषय ISLOWCNTRL(x) ((अचिह्नित वर्ण)(x) <= 0x1f)
#परिभाषय IS7F(x)       ((x) == 0x7f)

#परिभाषय ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#परिभाषय ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#परिभाषय ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#परिभाषय ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#परिभाषय ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#परिभाषय ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#परिभाषय ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#परिभाषय ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#परिभाषय ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#परिभाषय ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#परिभाषय ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#परिभाषय ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#परिभाषय ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#परिभाषय ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#परिभाषय ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#यदि_अन्तः /* HEADER_CURL_CTYPE_H */
