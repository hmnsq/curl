#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HTTPSRR_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HTTPSRR_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MAXLEN_HOST_NAME 253
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HTTPSRR_ALPNS     4

ರಚನೆ Curl_easy;
ರಚನೆ dynbuf;

ರಚನೆ Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  ಅಕ್ಷರ *ಗುರಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t ಪೋರ್ಟ್;
  uint16_t ಆದ್ಯತೆ;
  ಬಿಟ್(no_def_alpn); /* keytag = 2 */
  ಬಿಟ್(mandatory); /* keytag = 0 */
  ಬಿಟ್(ಪೋರ್ಟ್_ಗಣ); /* port value has been assigned */
};

CURLcode Curl_httpsrr_ಗಣ(ರಚನೆ Curl_https_rrinfo *rr,
                          uint16_t rrkey, ಸ್ಥಿರಾಂಕ uint8_t *val, size_t vlen);

ಶೂನ್ಯ Curl_httpsrr_destroy(ರಚನೆ Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(ರಚನೆ Curl_peer *peer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_MANDATORY       0x00
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_ALPN            0x01
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_NO_DEF_ALPN     0x02
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_ಪೋರ್ಟ್            0x03
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_IPV4            0x04
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_ECH             0x05
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPS_RR_ಸಂಕೇತ_IPV6            0x06

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
CURLcode Curl_httpsrr_from_ares(ಸ್ಥಿರಾಂಕ ares_dns_ದಾಖಲೆ_t *dnsrec,
                                ರಚನೆ Curl_https_rrinfo **phinfo);
#ಆದರೆ_ಅಂತ್ಯ /* USE_ARES */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
CURLcode Curl_httpsrr_print(ರಚನೆ dynbuf *tmp,
                            ರಚನೆ Curl_https_rrinfo *rr);
ಶೂನ್ಯ Curl_httpsrr_trace(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_https_rrinfo *rr);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_httpsrr_trace(a, b) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HTTPSRR_H */
