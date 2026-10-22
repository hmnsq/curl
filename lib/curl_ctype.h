#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CTYPE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CTYPE_H
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISLOWCNTRL(x) ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(x) <= 0x1f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS7F(x)       ((x) == 0x7f)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#ਜੇ_ਅੰਤ /* HEADER_CURL_CTYPE_H */
