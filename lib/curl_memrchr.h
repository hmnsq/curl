#यदि_अपरिभाषितम् शीर्षकम्_CURL_MEMRCHR_H
#परिभाषय शीर्षकम्_CURL_MEMRCHR_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_MEMRCHR

#यदि_परिभाषितम् HAVE_वाक्यानि_H
#  include <strings.h>
#यदि_अन्तः

#अन्यथा /* HAVE_MEMRCHR */
शून्य *Curl_memrchr(ध्रुव शून्य *s, पूर्णाङ्क c, size_t n);
#परिभाषय memrchr(x, y, z)  Curl_memrchr(x, y, z)

#यदि_अन्तः /* HAVE_MEMRCHR */

#यदि_अन्तः /* HEADER_CURL_MEMRCHR_H */
