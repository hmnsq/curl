#నిర్వచితం_కాకపోతే శీర్షిక_CURL_FORMDATA_H
#నిర్వచించు శీర్షిక_CURL_FORMDATA_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_FORM_API

#అన్తర్భావ "bufref.h"

/* used by FormAdd for temporary storage */
నిర్మాణం FormInfo {
  నిర్మాణం bufref name;
  నిర్మాణం bufref విలువ;
  నిర్మాణం bufref contenttype;
  నిర్మాణం bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  అక్షరం *బఫరు;      /* pointer to existing buffer used for file upload */
  అక్షరం *userp;        /* pointer for the read callback */
  నిర్మాణం FormInfo *మరిన్ని;
  నిర్మాణం curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  గుర్తులేని అక్షరం flags;
};

CURLcode Curl_getformdata(CURL *దత్తాంశం,
                          curl_mimepart *finalform,
                          నిర్మాణం curl_httppost *post,
                          curl_read_callback fread_func);
#అయితే_ముగింపు /* CURL_DISABLE_FORM_API */

#అయితే_ముగింపు /* HEADER_CURL_FORMDATA_H */
