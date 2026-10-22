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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

#ਅਨ੍ਤਰ੍ਭਾਵ "fileinfo.h"

ਬਣਤਰ fileinfo *Curl_fileinfo_alloc(ਖਾਲੀ)
{
  ਵਾਪਸ curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ fileinfo));
}

ਖਾਲੀ Curl_fileinfo_cleanup(ਬਣਤਰ fileinfo *finfo)
{
  ਜੇ(!finfo)
    ਵਾਪਸ;

  curlx_dyn_free(&finfo->buf);
  curlx_free(finfo);
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */
