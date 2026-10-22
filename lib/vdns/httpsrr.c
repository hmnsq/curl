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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"

ਸਥਿਰ CURLcode httpsrr_decode_alpn(ਅਚਲ uint8_t *cp, size_t len,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  ਪੂਰਨ idnum = 0;

  ਜਦੋਂ(len > 0) {
    size_t tlen = *cp++;
    ਗਿਣਤੀ alpnid id;
    len--;
    ਜੇ(tlen > len)
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    ਜੇ(id != ALPN_none) {
      ਜੇ(idnum == MAX_HTTPSRR_ALPNS)
        ਰੋਕੋ;
      ਜੇ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      ਨਹੀਂਤਾਂ
        alpns[idnum++] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)id;
    }
    cp += tlen;
    len -= tlen;
  }
  ਜੇ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE

ਸਥਿਰ CURLcode httpsrr_print_addr(ਬਣਤਰ dynbuf *dyn,
                                   ਪੂਰਨ ai_family,
                                   ਅਚਲ uint8_t *addr,
                                   size_t ਕੁੱਲ_len)
{
  ਅੱਖਰ buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ਅਚਲ ਅੱਖਰ *sep = "";
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਲਈ(i = 0; (i < (ਕੁੱਲ_len / alen)) && !ਨਤੀਜਾ; ++i) {
    ਜੇ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, ਆਕਾਰ(buf)))
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(dyn, "<error parsing address>");
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_httpsrr_trace(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_https_rrinfo *rr)
{
  ਬਣਤਰ dynbuf tmp;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!rr) {
    CURL_TRC_DNS(ਅੰਕੜੇ, "[HTTPS] no record available");
    ਵਾਪਸ;
  }
  curlx_dyn_init(&tmp, 1024);
  ਨਤੀਜਾ = Curl_httpsrr_print(&tmp, rr);
  ਜੇ(!ਨਤੀਜਾ)
    CURL_TRC_DNS(ਅੰਕੜੇ, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  ਨਹੀਂਤਾਂ
    CURL_TRC_DNS(ਅੰਕੜੇ, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ਬਣਤਰ dynbuf *tmp,
                            ਬਣਤਰ Curl_https_rrinfo *rr)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ i;

  curlx_dyn_reset(tmp);
  ਨਤੀਜਾ = curlx_dyn_addf(tmp, "%u %s", rr->ਤਰਜੀਹ,
                          rr->ਨਿਸ਼ਾਨਾ ? rr->ਨਿਸ਼ਾਨਾ : ".");
  ਜੇ(!ਨਤੀਜਾ && rr->mandatory)
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(tmp, " mandatory-keys(ignored)");
  ਜੇ(!ਨਤੀਜਾ && rr->alpns[0]) {
    ਅਚਲ ਅੱਖਰ *sep = "", *name;
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(tmp, " alpn=");
    ਲਈ(i = 0; !ਨਤੀਜਾ && (i < 4); ++i) {
      ਚੋਣ(rr->alpns[i]) {
      ਹਾਲਤ ALPN_h1:
        name = "http/1.1";
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h2:
        name = "h2";
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h3:
        name = "h3";
        ਰੋਕੋ;
      ਮੂਲ:
        name = ਨਿਰਾ;
      }
      ਜੇ(name) {
        ਨਤੀਜਾ = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  ਜੇ(!ਨਤੀਜਾ && rr->ਪੋਰਟ_ਸੈੱਟ) {
    ਨਤੀਜਾ = curlx_dyn_addf(tmp, " port=%u", rr->ਪੋਰਟ);
  }
  ਜੇ(!ਨਤੀਜਾ && rr->no_def_alpn)
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(tmp, " no-default-alpn");
  ਜੇ(!ਨਤੀਜਾ && rr->ipv6hints_len) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(tmp, " ipv6hint=");
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  ਜੇ(!ਨਤੀਜਾ && rr->ipv4hints_len) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(tmp, " ipv4hint=");
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  ਜੇ(!ਨਤੀਜਾ && rr->echconfiglist_len)
    ਨਤੀਜਾ = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* CURLVERBOSE */

CURLcode Curl_httpsrr_ਸੈੱਟ(ਬਣਤਰ Curl_https_rrinfo *rr,
                          uint16_t rrkey, ਅਚਲ uint8_t *val, size_t vlen)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਚੋਣ(rrkey) {
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_MANDATORY:
    rr->mandatory = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_ALPN: /* str_list */
    ਨਤੀਜਾ = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_NO_DEF_ALPN:
    ਜੇ(vlen) /* no data */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    rr->no_def_alpn = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_IPV4: /* addr4 list */
    ਜੇ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    ਜੇ(!rr->ipv4hints)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    rr->ipv4hints_len = vlen;
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_ECH:
    ਜੇ(!vlen)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    ਜੇ(!rr->echconfiglist)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    rr->echconfiglist_len = vlen;
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_IPV6: /* addr6 list */
    ਜੇ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    ਜੇ(!rr->ipv6hints)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    rr->ipv6hints_len = vlen;
    ਰੋਕੋ;
  ਹਾਲਤ HTTPS_RR_ਕੋਡ_ਪੋਰਟ:
    ਜੇ(vlen != 2)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    rr->ਪੋਰਟ = (uint16_t)((val[0] << 8) | val[1]);
    rr->ਪੋਰਟ_ਸੈੱਟ = TRUE;
    ਰੋਕੋ;
  ਮੂਲ:
    /* unknown code */
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_httpsrr_destroy(ਬਣਤਰ Curl_https_rrinfo *rrinfo)
{
  ਜੇ(rrinfo) {
    curlx_free(rrinfo->ਨਿਸ਼ਾਨਾ);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rr)
{
  ਜੇ(!ਅੰਕੜੇ->conn)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ Curl_httpsrr_is_for_peer(ਅੰਕੜੇ->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ਬਣਤਰ Curl_peer *peer,
                              ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rr)
{
  ਜੇ(!peer || !rr)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ (!rr->ਨਿਸ਼ਾਨਾ || !rr->ਨਿਸ਼ਾਨਾ[0] ||
          (rr->ਨਿਸ਼ਾਨਾ[0] == '.' && !rr->ਨਿਸ਼ਾਨਾ[1])) &&
         (!rr->ਪੋਰਟ_ਸੈੱਟ || rr->ਪੋਰਟ == peer->ਪੋਰਟ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES

ਸਥਿਰ CURLcode httpsrr_opt(ਅਚਲ ares_dns_rr_t *rr,
                            ares_dns_rr_ਕੁੰਜੀ_t ਕੁੰਜੀ, size_t idx,
                            ਬਣਤਰ Curl_https_rrinfo *hinfo)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *val = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਕੋਡ;
  size_t len = 0;

  ਕੋਡ = ares_dns_rr_get_opt(rr, ਕੁੰਜੀ, idx, &val, &len);
  ਵਾਪਸ Curl_httpsrr_ਸੈੱਟ(hinfo, ਕੋਡ, val, len);
}

CURLcode Curl_httpsrr_from_ares(ਅਚਲ ares_dns_ਰਿਕਾਰਡ_t *dnsrec,
                                ਬਣਤਰ Curl_https_rrinfo **phinfo)
{
  ਬਣਤਰ Curl_https_rrinfo *hinfo = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  size_t i;

  hinfo = curlx_calloc(1, ਆਕਾਰ(*hinfo));
  ਜੇ(!hinfo)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਲਈ(i = 0; i < ares_dns_ਰਿਕਾਰਡ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ;
    size_t opt;
    ਅਚਲ ares_dns_rr_t *rr =
      ares_dns_ਰਿਕਾਰਡ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    ਜੇ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ਜਾਰੀ;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    ਨਿਸ਼ਾਨਾ = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_ਨਿਸ਼ਾਨਾ);
    ਜੇ(ਨਿਸ਼ਾਨਾ && ਨਿਸ਼ਾਨਾ[0]) {
      curlx_free(hinfo->ਨਿਸ਼ਾਨਾ);
      hinfo->ਨਿਸ਼ਾਨਾ = curlx_strdup(ਨਿਸ਼ਾਨਾ);
      ਜੇ(!hinfo->ਨਿਸ਼ਾਨਾ) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    hinfo->ਤਰਜੀਹ = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ਤਰਜੀਹ);
    ਲਈ(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      ਨਤੀਜਾ = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    *phinfo = ਨਿਰਾ;
    Curl_httpsrr_destroy(hinfo);
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_ARES */

#ਜੇ_ਅੰਤ /* USE_HTTPSRR */
