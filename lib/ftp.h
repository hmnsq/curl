#अगर_अघोषित HEADER_CURL_FTP_H
#घोषणा HEADER_CURL_FTP_H
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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_FTP
बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_ftp;

bool Curl_ftp_conns_match(काष्ठा connectdata *needle,
                          काष्ठा connectdata *conn);

प्रकार क्रमागत {
  FTPFILE_MULTICWD  = 1, /* as defined by RFC1738 */
  FTPFILE_NOCWD     = 2, /* use SIZE / RETR / STOR on the full path */
  FTPFILE_SINGLECWD = 3  /* make one CWD, then SIZE / RETR / STOR on the
                            file */
} curl_ftpfile;

#पूर्ण_अगर /* CURL_DISABLE_FTP */

#घोषणा DEFAULT_ACCEPT_TIMEOUT   60000 /* milliseconds == one minute */

#पूर्ण_अगर /* HEADER_CURL_FTP_H */
