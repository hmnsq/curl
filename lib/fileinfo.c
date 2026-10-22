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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

#అన్తర్భావ "fileinfo.h"

నిర్మాణం fileinfo *Curl_fileinfo_alloc(శూన్య)
{
  తిరిగి curlx_calloc(1, పరిమాణం(నిర్మాణం fileinfo));
}

శూన్య Curl_fileinfo_cleanup(నిర్మాణం fileinfo *finfo)
{
  ఒకవేళ(!finfo)
    తిరిగి;

  curlx_dyn_free(&finfo->buf);
  curlx_free(finfo);
}

#అయితే_ముగింపు /* !CURL_DISABLE_FTP */
