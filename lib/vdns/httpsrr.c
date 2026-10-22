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

#که_تعریف_شوی USE_HTTPSRR

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/inet_ntop.h"

ساكن CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    بینښه حرف *alpns)
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
        alpns[idnum++] = (بینښه حرف)id;
    }
    cp += tlen;
    len -= tlen;
  }
  إذا(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  أرجع CURLE_OK;
}

#که_تعریف_شوی CURLVERBOSE

ساكن CURLcode httpsrr_print_addr(بنية dynbuf *dyn,
                                   صحيح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t ټول_len)
{
  حرف buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت حرف *sep = "";
  CURLcode پایله = CURLE_OK;

  لكل(i = 0; (i < (ټول_len / alen)) && !پایله; ++i) {
    إذا(curlx_inet_ntop(ai_family, addr + (i * alen), buf, حجم(buf)))
      پایله = curlx_dyn_جمع_کړه(dyn, "<error parsing address>");
    وإلا
      پایله = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  أرجع پایله;
}

فراغ Curl_httpsrr_trace(بنية Curl_easy *معلومات,
                        بنية Curl_https_rrinfo *rr)
{
  بنية dynbuf tmp;
  CURLcode پایله;

  إذا(!rr) {
    CURL_TRC_DNS(معلومات, "[HTTPS] no record available");
    أرجع;
  }
  curlx_dyn_init(&tmp, 1024);
  پایله = Curl_httpsrr_print(&tmp, rr);
  إذا(!پایله)
    CURL_TRC_DNS(معلومات, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  وإلا
    CURL_TRC_DNS(معلومات, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr)
{
  CURLcode پایله;
  صحيح i;

  curlx_dyn_reset(tmp);
  پایله = curlx_dyn_addf(tmp, "%u %s", rr->لومړیتوب,
                          rr->موخه ? rr->موخه : ".");
  إذا(!پایله && rr->mandatory)
    پایله = curlx_dyn_جمع_کړه(tmp, " mandatory-keys(ignored)");
  إذا(!پایله && rr->alpns[0]) {
    ثابت حرف *sep = "", *name;
    پایله = curlx_dyn_جمع_کړه(tmp, " alpn=");
    لكل(i = 0; !پایله && (i < 4); ++i) {
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
        پایله = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  إذا(!پایله && rr->پورټ_سېټ) {
    پایله = curlx_dyn_addf(tmp, " port=%u", rr->پورټ);
  }
  إذا(!پایله && rr->no_def_alpn)
    پایله = curlx_dyn_جمع_کړه(tmp, " no-default-alpn");
  إذا(!پایله && rr->ipv6hints_len) {
    پایله = curlx_dyn_جمع_کړه(tmp, " ipv6hint=");
    إذا(!پایله)
      پایله = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  إذا(!پایله && rr->ipv4hints_len) {
    پایله = curlx_dyn_جمع_کړه(tmp, " ipv4hint=");
    إذا(!پایله)
      پایله = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  إذا(!پایله && rr->echconfiglist_len)
    پایله = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  أرجع پایله;
}

#د_که_پای /* CURLVERBOSE */

CURLcode Curl_httpsrr_سېټ(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode پایله = CURLE_OK;
  بدل(rrkey) {
  حالة HTTPS_RR_کوډ_MANDATORY:
    rr->mandatory = TRUE;
    اكسر;
  حالة HTTPS_RR_کوډ_ALPN: /* str_list */
    پایله = httpsrr_decode_alpn(val, vlen, rr->alpns);
    اكسر;
  حالة HTTPS_RR_کوډ_NO_DEF_ALPN:
    إذا(vlen) /* no data */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    rr->no_def_alpn = TRUE;
    اكسر;
  حالة HTTPS_RR_کوډ_IPV4: /* addr4 list */
    إذا(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv4hints)
      أرجع CURLE_بهر_د_حافظه;
    rr->ipv4hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوډ_ECH:
    إذا(!vlen)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    إذا(!rr->echconfiglist)
      أرجع CURLE_بهر_د_حافظه;
    rr->echconfiglist_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوډ_IPV6: /* addr6 list */
    إذا(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv6hints)
      أرجع CURLE_بهر_د_حافظه;
    rr->ipv6hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوډ_پورټ:
    إذا(vlen != 2)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    rr->پورټ = (uint16_t)((val[0] << 8) | val[1]);
    rr->پورټ_سېټ = TRUE;
    اكسر;
  افتراضي:
    /* unknown code */
    اكسر;
  }
  أرجع پایله;
}

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo)
{
  إذا(rrinfo) {
    curlx_free(rrinfo->موخه);
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
  أرجع (!rr->موخه || !rr->موخه[0] ||
          (rr->موخه[0] == '.' && !rr->موخه[1])) &&
         (!rr->پورټ_سېټ || rr->پورټ == peer->پورټ);
}

#که_تعریف_شوی USE_ARES

ساكن CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_کلی_t کلی, size_t idx,
                            بنية Curl_https_rrinfo *hinfo)
{
  ثابت بینښه حرف *val = عدم;
  بینښه لنډ کوډ;
  size_t len = 0;

  کوډ = ares_dns_rr_get_opt(rr, کلی, idx, &val, &len);
  أرجع Curl_httpsrr_سېټ(hinfo, کوډ, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_ریکارډ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo)
{
  بنية Curl_https_rrinfo *hinfo = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  size_t i;

  hinfo = curlx_calloc(1, حجم(*hinfo));
  إذا(!hinfo)
    اذهب بهر;

  لكل(i = 0; i < ares_dns_ریکارډ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت حرف *موخه;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_ریکارډ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    إذا(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      استمر;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    موخه = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_موخه);
    إذا(موخه && موخه[0]) {
      curlx_free(hinfo->موخه);
      hinfo->موخه = curlx_strdup(موخه);
      إذا(!hinfo->موخه) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
    }
    hinfo->لومړیتوب = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_لومړیتوب);
    لكل(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      پایله = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      إذا(پایله)
        اذهب بهر;
    }
  }
  پایله = CURLE_OK;

بهر:
  إذا(پایله) {
    *phinfo = عدم;
    Curl_httpsrr_destroy(hinfo);
  }
  وإلا {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  أرجع پایله;
}

#د_که_پای /* USE_ARES */

#د_که_پای /* USE_HTTPSRR */
