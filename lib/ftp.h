#నిర్వచితం_కాకపోతే శీర్షిక_CURL_FTP_H
#నిర్వచించు శీర్షిక_CURL_FTP_H
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
బాహ్య స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ftp;

bool Curl_ftp_conns_సరిపోలిక(నిర్మాణం connectdata *needle,
                          నిర్మాణం connectdata *conn);

రకనిర్వచనం గణన {
  FTPFILE_MULTICWD  = 1, /* as defined by RFC1738 */
  FTPFILE_NOCWD     = 2, /* use SIZE / RETR / STOR on the full path */
  FTPFILE_SINGLECWD = 3  /* make one CWD, then SIZE / RETR / STOR on the
                            file */
} curl_ftpfile;

#అయితే_ముగింపు /* CURL_DISABLE_FTP */

#నిర్వచించు DEFAULT_ACCEPT_TIMEOUT   60000 /* milliseconds == one minute */

#అయితే_ముగింపు /* HEADER_CURL_FTP_H */
