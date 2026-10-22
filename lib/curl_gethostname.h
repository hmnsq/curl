#నిర్వచితం_కాకపోతే శీర్షిక_CURL_GETHOSTNAME_H
#నిర్వచించు శీర్షిక_CURL_GETHOSTNAME_H
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

/* Hostname buffer size */
#నిర్వచించు HOSTNAME_MAX 1024

/* This returns the local machine's un-qualified hostname */
పూర్ణాంకం Curl_gethostname(అక్షరం * స్థిరరాశి name, GETHOSTNAME_TYPE_ARG2 namelen);

#అయితే_ముగింపు /* HEADER_CURL_GETHOSTNAME_H */
