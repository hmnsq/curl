#که_نه_تعریف_شوی سرلیک_CURLX_BASENAME_H
#تعریف_کړه سرلیک_CURLX_BASENAME_H
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

#که_نه_تعریف_شوی HAVE_BASENAME
حرف *curlx_basename(حرف *لار);
#وإلا

#که_تعریف_شوی HAVE_LIBGEN_H
#ضمن <libgen.h>
#د_که_پای

#تعریف_کړه curlx_basename(x) basename(x)
#د_که_پای /* !HAVE_BASENAME */

#د_که_پای /* HEADER_CURLX_BASENAME_H */
