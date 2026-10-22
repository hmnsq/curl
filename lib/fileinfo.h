#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_FILEINFO_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_FILEINFO_H
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

#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

ರಚನೆ fileinfo {
  ರಚನೆ curl_fileinfo info;
  ರಚನೆ Curl_llist_ಗಂಟು list;
  ರಚನೆ dynbuf buf;
};

ರಚನೆ fileinfo *Curl_fileinfo_alloc(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_fileinfo_cleanup(ರಚನೆ fileinfo *finfo);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_FILEINFO_H */
