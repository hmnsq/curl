#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_NETRC_H
#وصف_ڪريو سرو_CURL_NETRC_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_NETRC

#ضمن "curlx/dynbuf.h"

بنية Curl_easy;
بنية Curl_creds;

بنية store_netrc {
  بنية dynbuf filebuf;
  حرف *filename;
  ٻٽو_انگ(loaded);
};

تعريفنوع تعداد {
  NETRC_OK,
  NETRC_NO_ميلاپ,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_ٻاهريون_جو_يادگيري, /* while parsing netrc */
  NETRC_آخري /* never used */
} NETRCcode;

ثابت حرف *Curl_netrc_strerror(NETRCcode ret);
فراغ Curl_netrc_init(بنية store_netrc *store);
فراغ Curl_netrc_cleanup(بنية store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_جاچ(بنية Curl_easy *معلومات,
                          بنية store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          بنية Curl_creds **pcreds);
#وإلا
/* disabled */
#وصف_ڪريو Curl_netrc_init(x)
#وصف_ڪريو Curl_netrc_cleanup(x)
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_NETRC_H */
