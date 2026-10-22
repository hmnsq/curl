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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"

ಸ್ಥಿರ CURLcode httpsrr_decode_alpn(ಸ್ಥಿರಾಂಕ uint8_t *cp, size_t len,
                                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  ಪೂರ್ಣಾಂಕ idnum = 0;

  ಆಗ(len > 0) {
    size_t tlen = *cp++;
    ಎಣಿಕೆ alpnid id;
    len--;
    ಆದರೆ(tlen > len)
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    ಆದರೆ(id != ALPN_none) {
      ಆದರೆ(idnum == MAX_HTTPSRR_ALPNS)
        ನಿಲ್ಲಿಸು;
      ಆದರೆ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      ಇಲ್ಲದಿದ್ದರೆ
        alpns[idnum++] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)id;
    }
    cp += tlen;
    len -= tlen;
  }
  ಆದರೆ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE

ಸ್ಥಿರ CURLcode httpsrr_print_addr(ರಚನೆ dynbuf *dyn,
                                   ಪೂರ್ಣಾಂಕ ai_family,
                                   ಸ್ಥಿರಾಂಕ uint8_t *addr,
                                   size_t ಒಟ್ಟು_len)
{
  ಅಕ್ಷರ buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "";
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಗಾಗಿ(i = 0; (i < (ಒಟ್ಟು_len / alen)) && !ಫಲಿತಾಂಶ; ++i) {
    ಆದರೆ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, ಗಾತ್ರ(buf)))
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(dyn, "<error parsing address>");
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_httpsrr_trace(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_https_rrinfo *rr)
{
  ರಚನೆ dynbuf tmp;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!rr) {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[HTTPS] no record available");
    ಹಿಂತಿರುಗಿ;
  }
  curlx_dyn_init(&tmp, 1024);
  ಫಲಿತಾಂಶ = Curl_httpsrr_print(&tmp, rr);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ರಚನೆ dynbuf *tmp,
                            ರಚನೆ Curl_https_rrinfo *rr)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ i;

  curlx_dyn_reset(tmp);
  ಫಲಿತಾಂಶ = curlx_dyn_addf(tmp, "%u %s", rr->ಆದ್ಯತೆ,
                          rr->ಗುರಿ ? rr->ಗುರಿ : ".");
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->mandatory)
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(tmp, " mandatory-keys(ignored)");
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->alpns[0]) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "", *name;
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(tmp, " alpn=");
    ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && (i < 4); ++i) {
      ಆಯ್ಕೆ(rr->alpns[i]) {
      ಸ್ಥಿತಿ ALPN_h1:
        name = "http/1.1";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h2:
        name = "h2";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h3:
        name = "h3";
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        name = ಖಾಲಿ;
      }
      ಆದರೆ(name) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->ಪೋರ್ಟ್_ಗಣ) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(tmp, " port=%u", rr->ಪೋರ್ಟ್);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->no_def_alpn)
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(tmp, " no-default-alpn");
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->ipv6hints_len) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(tmp, " ipv6hint=");
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->ipv4hints_len) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(tmp, " ipv4hint=");
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && rr->echconfiglist_len)
    ಫಲಿತಾಂಶ = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

CURLcode Curl_httpsrr_ಗಣ(ರಚನೆ Curl_https_rrinfo *rr,
                          uint16_t rrkey, ಸ್ಥಿರಾಂಕ uint8_t *val, size_t vlen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆಯ್ಕೆ(rrkey) {
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_MANDATORY:
    rr->mandatory = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_ALPN: /* str_list */
    ಫಲಿತಾಂಶ = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_NO_DEF_ALPN:
    ಆದರೆ(vlen) /* no data */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    rr->no_def_alpn = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_IPV4: /* addr4 list */
    ಆದರೆ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    ಆದರೆ(!rr->ipv4hints)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    rr->ipv4hints_len = vlen;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_ECH:
    ಆದರೆ(!vlen)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    ಆದರೆ(!rr->echconfiglist)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    rr->echconfiglist_len = vlen;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_IPV6: /* addr6 list */
    ಆದರೆ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    ಆದರೆ(!rr->ipv6hints)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    rr->ipv6hints_len = vlen;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPS_RR_ಸಂಕೇತ_ಪೋರ್ಟ್:
    ಆದರೆ(vlen != 2)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    rr->ಪೋರ್ಟ್ = (uint16_t)((val[0] << 8) | val[1]);
    rr->ಪೋರ್ಟ್_ಗಣ = TRUE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* unknown code */
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_httpsrr_destroy(ರಚನೆ Curl_https_rrinfo *rrinfo)
{
  ಆದರೆ(rrinfo) {
    curlx_free(rrinfo->ಗುರಿ);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rr)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ Curl_httpsrr_is_for_peer(ದತ್ತಾಂಶ->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ರಚನೆ Curl_peer *peer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rr)
{
  ಆದರೆ(!peer || !rr)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ (!rr->ಗುರಿ || !rr->ಗುರಿ[0] ||
          (rr->ಗುರಿ[0] == '.' && !rr->ಗುರಿ[1])) &&
         (!rr->ಪೋರ್ಟ್_ಗಣ || rr->ಪೋರ್ಟ್ == peer->ಪೋರ್ಟ್);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES

ಸ್ಥಿರ CURLcode httpsrr_opt(ಸ್ಥಿರಾಂಕ ares_dns_rr_t *rr,
                            ares_dns_rr_ಕೀಲಿ_t ಕೀಲಿ, size_t idx,
                            ರಚನೆ Curl_https_rrinfo *hinfo)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *val = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಸಂಕೇತ_2;
  size_t len = 0;

  ಸಂಕೇತ_2 = ares_dns_rr_get_opt(rr, ಕೀಲಿ, idx, &val, &len);
  ಹಿಂತಿರುಗಿ Curl_httpsrr_ಗಣ(hinfo, ಸಂಕೇತ_2, val, len);
}

CURLcode Curl_httpsrr_from_ares(ಸ್ಥಿರಾಂಕ ares_dns_ದಾಖಲೆ_t *dnsrec,
                                ರಚನೆ Curl_https_rrinfo **phinfo)
{
  ರಚನೆ Curl_https_rrinfo *hinfo = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  size_t i;

  hinfo = curlx_calloc(1, ಗಾತ್ರ(*hinfo));
  ಆದರೆ(!hinfo)
    ಹೋಗು ಹೊರಹೋಗು;

  ಗಾಗಿ(i = 0; i < ares_dns_ದಾಖಲೆ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ;
    size_t opt;
    ಸ್ಥಿರಾಂಕ ares_dns_rr_t *rr =
      ares_dns_ದಾಖಲೆ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    ಆದರೆ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ಮುಂದುವರಿ;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    ಗುರಿ = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_ಗುರಿ);
    ಆದರೆ(ಗುರಿ && ಗುರಿ[0]) {
      curlx_free(hinfo->ಗುರಿ);
      hinfo->ಗುರಿ = curlx_strdup(ಗುರಿ);
      ಆದರೆ(!hinfo->ಗುರಿ) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    hinfo->ಆದ್ಯತೆ = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ಆದ್ಯತೆ);
    ಗಾಗಿ(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      ಫಲಿತಾಂಶ = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    *phinfo = ಖಾಲಿ;
    Curl_httpsrr_destroy(hinfo);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_ARES */

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */
