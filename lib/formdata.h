#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_FORMDATA_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_FORMDATA_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FORM_API

#ಅನ್ತರ್ಭಾವ "bufref.h"

/* used by FormAdd for temporary storage */
ರಚನೆ FormInfo {
  ರಚನೆ bufref name;
  ರಚನೆ bufref ಮೌಲ್ಯ;
  ರಚನೆ bufref contenttype;
  ರಚನೆ bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  ಅಕ್ಷರ *ಬಫರ್;      /* pointer to existing buffer used for file upload */
  ಅಕ್ಷರ *userp;        /* pointer for the read callback */
  ರಚನೆ FormInfo *ಇನ್ನಷ್ಟು;
  ರಚನೆ curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ flags;
};

CURLcode Curl_getformdata(CURL *ದತ್ತಾಂಶ,
                          curl_mimepart *finalform,
                          ರಚನೆ curl_httppost *post,
                          curl_read_callback fread_func);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_FORM_API */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_FORMDATA_H */
