#جے_بنا_تعریف سرخی_CURL_NETRC_H
#تعریف_کرو سرخی_CURL_NETRC_H
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

#جے_بنا_تعریف CURL_DISABLE_NETRC

#ضمن "curlx/dynbuf.h"

بنية Curl_easy;
بنية Curl_creds;

بنية store_netrc {
  بنية dynbuf filebuf;
  حرف *filename;
  ثنائی_ہندسہ(loaded);
};

تعريفنوع تعداد {
  NETRC_OK,
  NETRC_NO_میل,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_باہرلا_دا_حافظہ, /* while parsing netrc */
  NETRC_آخری /* never used */
} NETRCcode;

ثابت حرف *Curl_netrc_strerror(NETRCcode ret);
فراغ Curl_netrc_init(بنية store_netrc *store);
فراغ Curl_netrc_cleanup(بنية store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_جانچ(بنية Curl_easy *انکڑے,
                          بنية store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          بنية Curl_creds **pcreds);
#وإلا
/* disabled */
#تعریف_کرو Curl_netrc_init(x)
#تعریف_کرو Curl_netrc_cleanup(x)
#جے_انت

#جے_انت /* HEADER_CURL_NETRC_H */
