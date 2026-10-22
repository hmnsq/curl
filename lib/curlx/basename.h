#ifndef HEADER_CURLX_BASENAME_H
#define HEADER_CURLX_BASENAME_H
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
#அந்தர்பாவ "curl_setup.h"

#ifndef HAVE_BASENAME
எழுத்து *curlx_basename(எழுத்து *path);
#இல்லையெனில்

#ifdef HAVE_LIBGEN_H
#அந்தர்பாவ <libgen.h>
#endif

#define curlx_basename(x) basename(x)
#endif /* !HAVE_BASENAME */

#endif /* HEADER_CURLX_BASENAME_H */
