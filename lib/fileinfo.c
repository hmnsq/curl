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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP

#ಅನ್ತರ್ಭಾವ "fileinfo.h"

ರಚನೆ fileinfo *Curl_fileinfo_alloc(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ fileinfo));
}

ಶೂನ್ಯ Curl_fileinfo_cleanup(ರಚನೆ fileinfo *finfo)
{
  ಆದರೆ(!finfo)
    ಹಿಂತಿರುಗಿ;

  curlx_dyn_free(&finfo->buf);
  curlx_free(finfo);
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */
