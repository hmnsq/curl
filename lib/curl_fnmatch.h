#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_FNMATCH_H
#وصف_ڪريو سرو_CURL_FNMATCH_H
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
#وصف_ڪريو CURL_FNMATCH_ميلاپ    0
#وصف_ڪريو CURL_FNMATCH_NOMATCH  1
#وصف_ڪريو CURL_FNMATCH_FAIL     2

/* default pattern matching function
 * =================================
 * Implemented with recursive backtracking, if you want to use Curl_fnmatch,
 * please note that there is not implemented UTF/Unicode support.
 *
 * Implemented features:
 * '?' notation, does not match UTF characters
 * '*' can also work with UTF string
 * [a-zA-Z0-9] enumeration support
 *
 * keywords: alnum, digit, xdigit, alpha, print, blank, lower, graph, space
 *           and upper (use as "[[:alnum:]]")
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *نمونو, ثابت حرف *string);

#جيڪڏهن_پورو /* HEADER_CURL_FNMATCH_H */
