#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CTYPE_H
#నిర్వచించు శీర్షిక_CURL_CTYPE_H
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

#నిర్వచించు ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#నిర్వచించు ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#నిర్వచించు ISLOWCNTRL(x) ((గుర్తులేని అక్షరం)(x) <= 0x1f)
#నిర్వచించు IS7F(x)       ((x) == 0x7f)

#నిర్వచించు ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#నిర్వచించు ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#నిర్వచించు ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#నిర్వచించు ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#నిర్వచించు ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#నిర్వచించు ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#నిర్వచించు ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#నిర్వచించు ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#నిర్వచించు ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#నిర్వచించు ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#నిర్వచించు ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#నిర్వచించు ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#నిర్వచించు ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#నిర్వచించు ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#నిర్వచించు ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#అయితే_ముగింపు /* HEADER_CURL_CTYPE_H */
