#यदि_अपरिभाषितम् शीर्षकम्_CURLX_BASENAME_H
#परिभाषय शीर्षकम्_CURLX_BASENAME_H
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

#यदि_अपरिभाषितम् HAVE_BASENAME
वर्ण *curlx_basename(वर्ण *मार्गः);
#अन्यथा

#यदि_परिभाषितम् HAVE_LIBGEN_H
#अन्तर्भाव <libgen.h>
#यदि_अन्तः

#परिभाषय curlx_basename(x) basename(x)
#यदि_अन्तः /* !HAVE_BASENAME */

#यदि_अन्तः /* HEADER_CURLX_BASENAME_H */
