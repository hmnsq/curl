#اگر_تعریف_نشده سرآیند_CURL_NETRC_H
#تعریف_کن سرآیند_CURL_NETRC_H
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

#اگر_تعریف_نشده CURL_DISABLE_NETRC

#ضمن "curlx/dynbuf.h"

بنية Curl_easy;
بنية Curl_creds;

بنية store_netrc {
  بنية dynbuf filebuf;
  حرف *filename;
  بیت(loaded);
};

تعريفنوع تعداد {
  NETRC_OK,
  NETRC_NO_تطابق,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_بیرون_از_حافظه, /* while parsing netrc */
  NETRC_آخرین /* never used */
} NETRCcode;

ثابت حرف *Curl_netrc_strerror(NETRCcode ret);
فراغ Curl_netrc_init(بنية store_netrc *store);
فراغ Curl_netrc_cleanup(بنية store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_پیمایش(بنية Curl_easy *داده,
                          بنية store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          بنية Curl_creds **pcreds);
#وإلا
/* disabled */
#تعریف_کن Curl_netrc_init(x)
#تعریف_کن Curl_netrc_cleanup(x)
#پایان_اگر

#پایان_اگر /* HEADER_CURL_NETRC_H */
