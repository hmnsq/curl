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

#ifndef CURL_DISABLE_FTP

#அந்தர்பாவ "fileinfo.h"

கட்டமைப்பு fileinfo *Curl_fileinfo_alloc(வெறுமை)
{
  திரும்பு curlx_calloc(1, அளவு(கட்டமைப்பு fileinfo));
}

வெறுமை Curl_fileinfo_cleanup(கட்டமைப்பு fileinfo *finfo)
{
  எனில்(!finfo)
    திரும்பு;

  curlx_dyn_free(&finfo->buf);
  curlx_free(finfo);
}

#endif /* !CURL_DISABLE_FTP */
