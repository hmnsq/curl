#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CONTENT_ENCODING_H
#నిర్వచించు శీర్షిక_CURL_CONTENT_ENCODING_H
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

నిర్మాణం Curl_cwriter;

/* returns an allocated string or NULL */
అక్షరం *Curl_get_content_encodings(శూన్య);

CURLcode Curl_build_unencoding_దొంతర(నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి అక్షరం *enclist, పూర్ణాంకం is_transfer);
#అయితే_ముగింపు /* HEADER_CURL_CONTENT_ENCODING_H */
