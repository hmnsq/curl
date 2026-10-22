#ifndef HEADER_CURL_NETRC_H
#define HEADER_CURL_NETRC_H
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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#अन्तर्भाव "curlx/dynbuf.h"

संरचना Curl_easy;
संरचना Curl_creds;

संरचना store_netrc {
  संरचना dynbuf filebuf;
  वर्ण *filename;
  BIT(loaded);
};

प्रकारनिर्देश गणना {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

ध्रुव वर्ण *Curl_netrc_strerror(NETRCcode ret);
शून्य Curl_netrc_init(संरचना store_netrc *store);
शून्य Curl_netrc_cleanup(संरचना store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(संरचना Curl_easy *data,
                          संरचना store_netrc *store,
                          ध्रुव वर्ण *hostname,
                          ध्रुव वर्ण *user,
                          ध्रुव वर्ण *netrcfile,
                          संरचना Curl_creds **pcreds);
#अन्यथा
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
