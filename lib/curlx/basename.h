#جيڪڏهن_وصف_نه_ٿيل سرو_CURLX_BASENAME_H
#وصف_ڪريو سرو_CURLX_BASENAME_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_نه_ٿيل HAVE_BASENAME
حرف *curlx_basename(حرف *رستو);
#وإلا

#جيڪڏهن_وصف_ٿيل HAVE_LIBGEN_H
#ضمن <libgen.h>
#جيڪڏهن_پورو

#وصف_ڪريو curlx_basename(x) basename(x)
#جيڪڏهن_پورو /* !HAVE_BASENAME */

#جيڪڏهن_پورو /* HEADER_CURLX_BASENAME_H */
