#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CTYPE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CTYPE_H
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

#ವ್ಯಾಖ್ಯಾನಿಸು ISLOWHEXALPHA(x) (((x) >= 'a') && ((x) <= 'f'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISUPHEXALPHA(x)  (((x) >= 'A') && ((x) <= 'F'))

#ವ್ಯಾಖ್ಯಾನಿಸು ISLOWCNTRL(x) ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(x) <= 0x1f)
#ವ್ಯಾಖ್ಯಾನಿಸು IS7F(x)       ((x) == 0x7f)

#ವ್ಯಾಖ್ಯಾನಿಸು ISPRINT(x)  (((x) >= ' ') && ((x) <= 0x7e))
#ವ್ಯಾಖ್ಯಾನಿಸು ISGRAPH(x)  (((x) > ' ') && ((x) <= 0x7e))
#ವ್ಯಾಖ್ಯಾನಿಸು ISCNTRL(x)  (ISLOWCNTRL(x) || IS7F(x))
#ವ್ಯಾಖ್ಯಾನಿಸು ISALPHA(x)  (ISLOWER(x) || ISUPPER(x))
#ವ್ಯಾಖ್ಯಾನಿಸು ISXDIGIT(x) (ISDIGIT(x) || ISLOWHEXALPHA(x) || ISUPHEXALPHA(x))
#ವ್ಯಾಖ್ಯಾನಿಸು ISODIGIT(x) (((x) >= '0') && ((x) <= '7'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISALNUM(x)  (ISDIGIT(x) || ISLOWER(x) || ISUPPER(x))
#ವ್ಯಾಖ್ಯಾನಿಸು ISUPPER(x)  (((x) >= 'A') && ((x) <= 'Z'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISLOWER(x)  (((x) >= 'a') && ((x) <= 'z'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISDIGIT(x)  (((x) >= '0') && ((x) <= '9'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISBLANK(x)  (((x) == ' ') || ((x) == '\t'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISSPACE(x)  (ISBLANK(x) || (((x) >= 0xa) && ((x) <= 0x0d)))
#ವ್ಯಾಖ್ಯಾನಿಸು ISURLPUNTCS(x)                                           \
  (((x) == '-') || ((x) == '.') || ((x) == '_') || ((x) == '~'))
#ವ್ಯಾಖ್ಯಾನಿಸು ISUNRESERVED(x) (ISALNUM(x) || ISURLPUNTCS(x))
#ವ್ಯಾಖ್ಯಾನಿಸು ISNEWLINE(x)    (((x) == '\n') || (x) == '\r')

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CTYPE_H */
