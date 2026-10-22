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

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/inet_ntop.h"

ساكن CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    بنانشان حرف *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  صحيح idnum = 0;

  طالما(len > 0) {
    size_t tlen = *cp++;
    تعداد alpnid id;
    len--;
    إذا(tlen > len)
      أرجع CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    إذا(id != ALPN_none) {
      إذا(idnum == MAX_HTTPSRR_ALPNS)
        اكسر;
      إذا(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      وإلا
        alpns[idnum++] = (بنانشان حرف)id;
    }
    cp += tlen;
    len -= tlen;
  }
  إذا(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE

ساكن CURLcode httpsrr_print_addr(بنية dynbuf *dyn,
                                   صحيح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t ڪل_len)
{
  حرف buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت حرف *sep = "";
  CURLcode نتيجو = CURLE_OK;

  لكل(i = 0; (i < (ڪل_len / alen)) && !نتيجو; ++i) {
    إذا(curlx_inet_ntop(ai_family, addr + (i * alen), buf, حجم(buf)))
      نتيجو = curlx_dyn_جوڙيو(dyn, "<error parsing address>");
    وإلا
      نتيجو = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  أرجع نتيجو;
}

فراغ Curl_httpsrr_trace(بنية Curl_easy *معلومات,
                        بنية Curl_https_rrinfo *rr)
{
  بنية dynbuf tmp;
  CURLcode نتيجو;

  إذا(!rr) {
    CURL_TRC_DNS(معلومات, "[HTTPS] no record available");
    أرجع;
  }
  curlx_dyn_init(&tmp, 1024);
  نتيجو = Curl_httpsrr_print(&tmp, rr);
  إذا(!نتيجو)
    CURL_TRC_DNS(معلومات, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  وإلا
    CURL_TRC_DNS(معلومات, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr)
{
  CURLcode نتيجو;
  صحيح i;

  curlx_dyn_reset(tmp);
  نتيجو = curlx_dyn_addf(tmp, "%u %s", rr->ترجيح,
                          rr->هدف ? rr->هدف : ".");
  إذا(!نتيجو && rr->mandatory)
    نتيجو = curlx_dyn_جوڙيو(tmp, " mandatory-keys(ignored)");
  إذا(!نتيجو && rr->alpns[0]) {
    ثابت حرف *sep = "", *name;
    نتيجو = curlx_dyn_جوڙيو(tmp, " alpn=");
    لكل(i = 0; !نتيجو && (i < 4); ++i) {
      بدل(rr->alpns[i]) {
      حالة ALPN_h1:
        name = "http/1.1";
        اكسر;
      حالة ALPN_h2:
        name = "h2";
        اكسر;
      حالة ALPN_h3:
        name = "h3";
        اكسر;
      افتراضي:
        name = عدم;
      }
      إذا(name) {
        نتيجو = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  إذا(!نتيجو && rr->پورٽ_سيٽ) {
    نتيجو = curlx_dyn_addf(tmp, " port=%u", rr->پورٽ);
  }
  إذا(!نتيجو && rr->no_def_alpn)
    نتيجو = curlx_dyn_جوڙيو(tmp, " no-default-alpn");
  إذا(!نتيجو && rr->ipv6hints_len) {
    نتيجو = curlx_dyn_جوڙيو(tmp, " ipv6hint=");
    إذا(!نتيجو)
      نتيجو = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  إذا(!نتيجو && rr->ipv4hints_len) {
    نتيجو = curlx_dyn_جوڙيو(tmp, " ipv4hint=");
    إذا(!نتيجو)
      نتيجو = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  إذا(!نتيجو && rr->echconfiglist_len)
    نتيجو = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  أرجع نتيجو;
}

#جيڪڏهن_پورو /* CURLVERBOSE */

CURLcode Curl_httpsrr_سيٽ(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode نتيجو = CURLE_OK;
  بدل(rrkey) {
  حالة HTTPS_RR_ڪوڊ_MANDATORY:
    rr->mandatory = TRUE;
    اكسر;
  حالة HTTPS_RR_ڪوڊ_ALPN: /* str_list */
    نتيجو = httpsrr_decode_alpn(val, vlen, rr->alpns);
    اكسر;
  حالة HTTPS_RR_ڪوڊ_NO_DEF_ALPN:
    إذا(vlen) /* no data */
      أرجع CURLE_BAD_FUNCTION_دليل;
    rr->no_def_alpn = TRUE;
    اكسر;
  حالة HTTPS_RR_ڪوڊ_IPV4: /* addr4 list */
    إذا(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دليل;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv4hints)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    rr->ipv4hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_ڪوڊ_ECH:
    إذا(!vlen)
      أرجع CURLE_BAD_FUNCTION_دليل;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    إذا(!rr->echconfiglist)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    rr->echconfiglist_len = vlen;
    اكسر;
  حالة HTTPS_RR_ڪوڊ_IPV6: /* addr6 list */
    إذا(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دليل;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv6hints)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    rr->ipv6hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_ڪوڊ_پورٽ:
    إذا(vlen != 2)
      أرجع CURLE_BAD_FUNCTION_دليل;
    rr->پورٽ = (uint16_t)((val[0] << 8) | val[1]);
    rr->پورٽ_سيٽ = TRUE;
    اكسر;
  افتراضي:
    /* unknown code */
    اكسر;
  }
  أرجع نتيجو;
}

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo)
{
  إذا(rrinfo) {
    curlx_free(rrinfo->هدف);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(بنية Curl_easy *معلومات,
                             ثابت بنية Curl_https_rrinfo *rr)
{
  إذا(!معلومات->conn)
    أرجع FALSE;
  أرجع Curl_httpsrr_is_for_peer(معلومات->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr)
{
  إذا(!peer || !rr)
    أرجع FALSE;
  أرجع (!rr->هدف || !rr->هدف[0] ||
          (rr->هدف[0] == '.' && !rr->هدف[1])) &&
         (!rr->پورٽ_سيٽ || rr->پورٽ == peer->پورٽ);
}

#جيڪڏهن_وصف_ٿيل USE_ARES

ساكن CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_ڪنجي_t ڪنجي, size_t idx,
                            بنية Curl_https_rrinfo *hinfo)
{
  ثابت بنانشان حرف *val = عدم;
  بنانشان ننڍو ڪوڊ;
  size_t len = 0;

  ڪوڊ = ares_dns_rr_get_opt(rr, ڪنجي, idx, &val, &len);
  أرجع Curl_httpsrr_سيٽ(hinfo, ڪوڊ, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_رڪارڊ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo)
{
  بنية Curl_https_rrinfo *hinfo = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  size_t i;

  hinfo = curlx_calloc(1, حجم(*hinfo));
  إذا(!hinfo)
    اذهب ٻاهريون;

  لكل(i = 0; i < ares_dns_رڪارڊ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت حرف *هدف;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_رڪارڊ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    إذا(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      استمر;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    هدف = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_هدف);
    إذا(هدف && هدف[0]) {
      curlx_free(hinfo->هدف);
      hinfo->هدف = curlx_strdup(هدف);
      إذا(!hinfo->هدف) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }
    hinfo->ترجيح = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ترجيح);
    لكل(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      نتيجو = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو) {
    *phinfo = عدم;
    Curl_httpsrr_destroy(hinfo);
  }
  وإلا {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_ARES */

#جيڪڏهن_پورو /* USE_HTTPSRR */
