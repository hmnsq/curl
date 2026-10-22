#جے_بنا_تعریف سرخی_CURLX_BASENAME_H
#تعریف_کرو سرخی_CURLX_BASENAME_H
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

#جے_بنا_تعریف HAVE_BASENAME
حرف *curlx_basename(حرف *رستہ);
#وإلا

#جے_تعریف_شدہ HAVE_LIBGEN_H
#ضمن <libgen.h>
#جے_انت

#تعریف_کرو curlx_basename(x) basename(x)
#جے_انت /* !HAVE_BASENAME */

#جے_انت /* HEADER_CURLX_BASENAME_H */
