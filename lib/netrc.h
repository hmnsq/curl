#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_NETRC_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_NETRC_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

ರಚನೆ Curl_easy;
ರಚನೆ Curl_creds;

ರಚನೆ store_netrc {
  ರಚನೆ dynbuf filebuf;
  ಅಕ್ಷರ *filename;
  ಬಿಟ್(loaded);
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  NETRC_OK,
  NETRC_NO_ಹೊಂದಿಕೆ,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ, /* while parsing netrc */
  NETRC_ಕೊನೆಯ /* never used */
} NETRCcode;

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_netrc_strerror(NETRCcode ret);
ಶೂನ್ಯ Curl_netrc_init(ರಚನೆ store_netrc *store);
ಶೂನ್ಯ Curl_netrc_cleanup(ರಚನೆ store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_ಪರಿಶೀಲನೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ store_netrc *store,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *netrcfile,
                          ರಚನೆ Curl_creds **pcreds);
#ಇಲ್ಲದಿದ್ದರೆ
/* disabled */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_netrc_init(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_netrc_cleanup(x)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_NETRC_H */
