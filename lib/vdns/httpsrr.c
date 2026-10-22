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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_HTTPSRR

#شامل "urldata.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/strdup.h"
#شامل "curlx/inet_ntop.h"

ساکن CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    غیرعلامتی حرف *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  صحیح idnum = 0;

  جب(len > 0) {
    size_t tlen = *cp++;
    شمار alpnid id;
    len--;
    اگر(tlen > len)
      واپس CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    اگر(id != ALPN_none) {
      اگر(idnum == MAX_HTTPSRR_ALPNS)
        توڑو;
      اگر(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      ورنہ
        alpns[idnum++] = (غیرعلامتی حرف)id;
    }
    cp += tlen;
    len -= tlen;
  }
  اگر(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURLVERBOSE

ساکن CURLcode httpsrr_print_addr(ساخت dynbuf *dyn,
                                   صحیح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t کل_len)
{
  حرف buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت حرف *sep = "";
  CURLcode نتیجہ = CURLE_OK;

  برائے(i = 0; (i < (کل_len / alen)) && !نتیجہ; ++i) {
    اگر(curlx_inet_ntop(ai_family, addr + (i * alen), buf, حجم(buf)))
      نتیجہ = curlx_dyn_جوڑو(dyn, "<error parsing address>");
    ورنہ
      نتیجہ = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  واپس نتیجہ;
}

خالی Curl_httpsrr_trace(ساخت Curl_easy *ڈیٹا,
                        ساخت Curl_https_rrinfo *rr)
{
  ساخت dynbuf tmp;
  CURLcode نتیجہ;

  اگر(!rr) {
    CURL_TRC_DNS(ڈیٹا, "[HTTPS] no record available");
    واپس;
  }
  curlx_dyn_init(&tmp, 1024);
  نتیجہ = Curl_httpsrr_print(&tmp, rr);
  اگر(!نتیجہ)
    CURL_TRC_DNS(ڈیٹا, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  ورنہ
    CURL_TRC_DNS(ڈیٹا, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ساخت dynbuf *tmp,
                            ساخت Curl_https_rrinfo *rr)
{
  CURLcode نتیجہ;
  صحیح i;

  curlx_dyn_reset(tmp);
  نتیجہ = curlx_dyn_addf(tmp, "%u %s", rr->ترجیح,
                          rr->ہدف ? rr->ہدف : ".");
  اگر(!نتیجہ && rr->mandatory)
    نتیجہ = curlx_dyn_جوڑو(tmp, " mandatory-keys(ignored)");
  اگر(!نتیجہ && rr->alpns[0]) {
    ثابت حرف *sep = "", *name;
    نتیجہ = curlx_dyn_جوڑو(tmp, " alpn=");
    برائے(i = 0; !نتیجہ && (i < 4); ++i) {
      انتخاب(rr->alpns[i]) {
      حالت ALPN_h1:
        name = "http/1.1";
        توڑو;
      حالت ALPN_h2:
        name = "h2";
        توڑو;
      حالت ALPN_h3:
        name = "h3";
        توڑو;
      طےشدہ:
        name = تہی;
      }
      اگر(name) {
        نتیجہ = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  اگر(!نتیجہ && rr->پورٹ_سیٹ) {
    نتیجہ = curlx_dyn_addf(tmp, " port=%u", rr->پورٹ);
  }
  اگر(!نتیجہ && rr->no_def_alpn)
    نتیجہ = curlx_dyn_جوڑو(tmp, " no-default-alpn");
  اگر(!نتیجہ && rr->ipv6hints_len) {
    نتیجہ = curlx_dyn_جوڑو(tmp, " ipv6hint=");
    اگر(!نتیجہ)
      نتیجہ = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  اگر(!نتیجہ && rr->ipv4hints_len) {
    نتیجہ = curlx_dyn_جوڑو(tmp, " ipv4hint=");
    اگر(!نتیجہ)
      نتیجہ = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  اگر(!نتیجہ && rr->echconfiglist_len)
    نتیجہ = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  واپس نتیجہ;
}

#اگر_ختم /* CURLVERBOSE */

CURLcode Curl_httpsrr_سیٹ(ساخت Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode نتیجہ = CURLE_OK;
  انتخاب(rrkey) {
  حالت HTTPS_RR_رمز_MANDATORY:
    rr->mandatory = TRUE;
    توڑو;
  حالت HTTPS_RR_رمز_ALPN: /* str_list */
    نتیجہ = httpsrr_decode_alpn(val, vlen, rr->alpns);
    توڑو;
  حالت HTTPS_RR_رمز_NO_DEF_ALPN:
    اگر(vlen) /* no data */
      واپس CURLE_BAD_FUNCTION_دلیل;
    rr->no_def_alpn = TRUE;
    توڑو;
  حالت HTTPS_RR_رمز_IPV4: /* addr4 list */
    اگر(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      واپس CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    اگر(!rr->ipv4hints)
      واپس CURLE_برآمد_کا_حافظہ;
    rr->ipv4hints_len = vlen;
    توڑو;
  حالت HTTPS_RR_رمز_ECH:
    اگر(!vlen)
      واپس CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    اگر(!rr->echconfiglist)
      واپس CURLE_برآمد_کا_حافظہ;
    rr->echconfiglist_len = vlen;
    توڑو;
  حالت HTTPS_RR_رمز_IPV6: /* addr6 list */
    اگر(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      واپس CURLE_BAD_FUNCTION_دلیل;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    اگر(!rr->ipv6hints)
      واپس CURLE_برآمد_کا_حافظہ;
    rr->ipv6hints_len = vlen;
    توڑو;
  حالت HTTPS_RR_رمز_پورٹ:
    اگر(vlen != 2)
      واپس CURLE_BAD_FUNCTION_دلیل;
    rr->پورٹ = (uint16_t)((val[0] << 8) | val[1]);
    rr->پورٹ_سیٹ = TRUE;
    توڑو;
  طےشدہ:
    /* unknown code */
    توڑو;
  }
  واپس نتیجہ;
}

خالی Curl_httpsrr_destroy(ساخت Curl_https_rrinfo *rrinfo)
{
  اگر(rrinfo) {
    curlx_free(rrinfo->ہدف);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت Curl_https_rrinfo *rr)
{
  اگر(!ڈیٹا->conn)
    واپس FALSE;
  واپس Curl_httpsrr_is_for_peer(ڈیٹا->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ساخت Curl_peer *peer,
                              ثابت ساخت Curl_https_rrinfo *rr)
{
  اگر(!peer || !rr)
    واپس FALSE;
  واپس (!rr->ہدف || !rr->ہدف[0] ||
          (rr->ہدف[0] == '.' && !rr->ہدف[1])) &&
         (!rr->پورٹ_سیٹ || rr->پورٹ == peer->پورٹ);
}

#اگر_تعریف_شدہ USE_ARES

ساکن CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_کلید_t کلید, size_t idx,
                            ساخت Curl_https_rrinfo *hinfo)
{
  ثابت غیرعلامتی حرف *val = تہی;
  غیرعلامتی مختصر رمز;
  size_t len = 0;

  رمز = ares_dns_rr_get_opt(rr, کلید, idx, &val, &len);
  واپس Curl_httpsrr_سیٹ(hinfo, رمز, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_ریکارڈ_t *dnsrec,
                                ساخت Curl_https_rrinfo **phinfo)
{
  ساخت Curl_https_rrinfo *hinfo = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  size_t i;

  hinfo = curlx_calloc(1, حجم(*hinfo));
  اگر(!hinfo)
    جاؤ برآمد;

  برائے(i = 0; i < ares_dns_ریکارڈ_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت حرف *ہدف;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_ریکارڈ_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    اگر(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      جاری;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    ہدف = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_ہدف);
    اگر(ہدف && ہدف[0]) {
      curlx_free(hinfo->ہدف);
      hinfo->ہدف = curlx_strdup(ہدف);
      اگر(!hinfo->ہدف) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
    hinfo->ترجیح = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ترجیح);
    برائے(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      نتیجہ = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }
  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ) {
    *phinfo = تہی;
    Curl_httpsrr_destroy(hinfo);
  }
  ورنہ {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  واپس نتیجہ;
}

#اگر_ختم /* USE_ARES */

#اگر_ختم /* USE_HTTPSRR */
