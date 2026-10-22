#अगर_अघोषित HEADER_CURL_CTYPE_H
#घोषणा HEADER_CURL_CTYPE_H
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

#घोषणा ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#घोषणा ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#घोषणा ISLOWCNTRL(x) ((अचिन्हित अक्षर)(x) <= 0x1f)
#घोषणा IS7F(x)       ((x) == 0x7f)

#घोषणा ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#घोषणा ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#घोषणा ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#घोषणा ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#घोषणा ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#घोषणा ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#घोषणा ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#घोषणा ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#घोषणा ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#घोषणा ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#घोषणा ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#घोषणा ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#घोषणा ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#घोषणा ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#घोषणा ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#पूर्ण_अगर /* HEADER_CURL_CTYPE_H */
