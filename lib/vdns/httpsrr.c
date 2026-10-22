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

#جے_تعریف_شدہ USE_HTTPSRR

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/inet_ntop.h"

ساكن CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    بنانشانی حرف *alpns)
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
        alpns[idnum++] = (بنانشانی حرف)id;
    }
    cp += tlen;
    len -= tlen;
  }
  إذا(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  أرجع CURLE_OK;
}

#جے_تعریف_شدہ CURLVERBOSE

ساكن CURLcode httpsrr_print_addr(بنية dynbuf *dyn,
                                   صحيح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t کل_len)
{
  حرف buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت حرف *sep = "";
  CURLcode نتیجہ = CURLE_OK;

  لكل(i = 0; (i < (کل_len / alen)) && !نتیجہ; ++i) {
    إذا(curlx_inet_ntop(ai_family, addr + (i * alen), buf, حجم(buf)))
      نتیجہ = curlx_dyn_جوڑو(dyn, "<error parsing address>");
    وإلا
      نتیجہ = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  أرجع نتیجہ;
}

فراغ Curl_httpsrr_trace(بنية Curl_easy *انکڑے,
                        بنية Curl_https_rrinfo *rr)
{
  بنية dynbuf tmp;
  CURLcode نتیجہ;

  إذا(!rr) {
    CURL_TRC_DNS(انکڑے, "[HTTPS] no record available");
    أرجع;
  }
  curlx_dyn_init(&tmp, 1024);
  نتیجہ = Curl_httpsrr_print(&tmp, rr);
  إذا(!نتیجہ)
    CURL_TRC_DNS(انکڑے, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  وإلا
    CURL_TRC_DNS(انکڑے, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr)
{
  CURLcode نتیجہ;
  صحيح i;

  curlx_dyn_reset(tmp);
  نتیجہ = curlx_dyn_addf(tmp, "%u %s", rr->ترجیح,
                          rr->نشانہ ? rr->نشانہ : ".");
  إذا(!نتیجہ && rr->mandatory)
    نتیجہ = curlx_dyn_جوڑو(tmp, " mandatory-keys(ignored)");
  إذا(!نتیجہ && rr->alpns[0]) {
    ثابت حرف *sep = "", *name;
    نتیجہ = curlx_dyn_جوڑو(tmp, " alpn=");
    لكل(i = 0; !نتیجہ && (i < 4); ++i) {
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
        نتیجہ = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  إذا(!نتیجہ && rr->پورٹ_سیٹ) {
    نتیجہ = curlx_dyn_addf(tmp, " port=%u", rr->پورٹ);
  }
  إذا(!نتیجہ && rr->no_def_alpn)
    نتیجہ = curlx_dyn_جوڑو(tmp, " no-default-alpn");
  إذا(!نتیجہ && rr->ipv6hints_len) {
    نتیجہ = curlx_dyn_جوڑو(tmp, " ipv6hint=");
    إذا(!نتیجہ)
      نتیجہ = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  إذا(!نتیجہ && rr->ipv4hints_len) {
    نتیجہ = curlx_dyn_جوڑو(tmp, " ipv4hint=");
    إذا(!نتیجہ)
      نتیجہ = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  إذا(!نتیجہ && rr->echconfiglist_len)
    نتیجہ = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  أرجع نتیجہ;
}

#جے_انت /* CURLVERBOSE */

CURLcode Curl_httpsrr_سیٹ(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode نتیجہ = CURLE_OK;
  بدل(rrkey) {
  حالة HTTPS_RR_کوڈ_MANDATORY:
    rr->mandatory = TRUE;
    اكسر;
  حالة HTTPS_RR_کوڈ_ALPN: /* str_list */
    نتیجہ = httpsrr_decode_alpn(val, vlen, rr->alpns);
    اكسر;
  حالة HTTPS_RR_کوڈ_NO_DEF_ALPN:
    إذا(vlen) /* no data */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    rr->no_def_alpn = TRUE;
    اكسر;
  حالة HTTPS_RR_کوڈ_IPV4: /* addr4 list */
    إذا(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv4hints)
      أرجع CURLE_باہرلا_دا_حافظہ;
    rr->ipv4hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوڈ_ECH:
    إذا(!vlen)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    إذا(!rr->echconfiglist)
      أرجع CURLE_باہرلا_دا_حافظہ;
    rr->echconfiglist_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوڈ_IPV6: /* addr6 list */
    إذا(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    إذا(!rr->ipv6hints)
      أرجع CURLE_باہرلا_دا_حافظہ;
    rr->ipv6hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_کوڈ_پورٹ:
    إذا(vlen != 2)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    rr->پورٹ = (uint16_t)((val[0] << 8) | val[1]);
    rr->پورٹ_سیٹ = TRUE;
    اكسر;
  افتراضي:
    /* unknown code */
    اكسر;
  }
  أرجع نتیجہ;
}

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo)
{
  إذا(rrinfo) {
    curlx_free(rrinfo->نشانہ);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(بنية Curl_easy *انکڑے,
                             ثابت بنية Curl_https_rrinfo *rr)
{
  إذا(!انکڑے->conn)
    أرجع FALSE;
  أرجع Curl_httpsrr_is_for_peer(انکڑے->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr)
{
  إذا(!peer || !rr)
    أرجع FALSE;
  أرجع (!rr->نشانہ || !rr->نشانہ[0] ||
          (rr->نشانہ[0] == '.' && !rr->نشانہ[1])) &&
         (!rr->پورٹ_سیٹ || rr->پورٹ == peer->پورٹ);
}

#جے_تعریف_شدہ USE_ARES

ساكن CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_کلید_t کلید, size_t idx,
                            بنية Curl_https_rrinfo *hinfo)
{
  ثابت بنانشانی حرف *val = عدم;
  بنانشانی چھوٹا کوڈ;
  size_t len = 0;

  کوڈ = ares_dns_rr_get_opt(rr, کلید, idx, &val, &len);
  أرجع Curl_httpsrr_سیٹ(hinfo, کوڈ, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_ریکارڈ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo)
{
  بنية Curl_https_rrinfo *hinfo = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  size_t i;

  hinfo = curlx_calloc(1, حجم(*hinfo));
  إذا(!hinfo)
    اذهب باہرلا;

  لكل(i = 0; i < ares_dns_ریکارڈ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت حرف *نشانہ;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_ریکارڈ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    إذا(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      استمر;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    نشانہ = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_نشانہ);
    إذا(نشانہ && نشانہ[0]) {
      curlx_free(hinfo->نشانہ);
      hinfo->نشانہ = curlx_strdup(نشانہ);
      إذا(!hinfo->نشانہ) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }
    hinfo->ترجیح = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ترجیح);
    لكل(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      نتیجہ = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ) {
    *phinfo = عدم;
    Curl_httpsrr_destroy(hinfo);
  }
  وإلا {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  أرجع نتیجہ;
}

#جے_انت /* USE_ARES */

#جے_انت /* USE_HTTPSRR */
