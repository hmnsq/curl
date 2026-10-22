#اگر_غیر_تعریف_شدہ سرخی_CURL_MEMRCHR_H
#تعریف_کرو سرخی_CURL_MEMRCHR_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_MEMRCHR

#اگر_تعریف_شدہ HAVE_لڑیاں_H
#  include <strings.h>
#اگر_ختم

#ورنہ /* HAVE_MEMRCHR */
خالی *Curl_memrchr(ثابت خالی *s, صحیح c, size_t n);
#تعریف_کرو memrchr(x, y, z)  Curl_memrchr(x, y, z)

#اگر_ختم /* HAVE_MEMRCHR */

#اگر_ختم /* HEADER_CURL_MEMRCHR_H */
