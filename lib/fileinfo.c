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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#അന്തര്ഭാവ "fileinfo.h"

ഘടന fileinfo *Curl_fileinfo_alloc(ശൂന്യം)
{
  തിരികെ curlx_calloc(1, വലുപ്പം(ഘടന fileinfo));
}

ശൂന്യം Curl_fileinfo_cleanup(ഘടന fileinfo *finfo)
{
  എങ്കിൽ(!finfo)
    തിരികെ;

  curlx_dyn_free(&finfo->buf);
  curlx_free(finfo);
}

#endif /* !CURL_DISABLE_FTP */
