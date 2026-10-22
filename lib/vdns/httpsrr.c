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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_HTTPSRR

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/inet_ntop.h"

স্থির CURLcode httpsrr_decode_alpn(ধ্রুৱ uint8_t *cp, size_t len,
                                    unsigned ৱর্ণ *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  পূর্ণাঙ্ক idnum = 0;

  যেতিয়ালৈ(len > 0) {
    size_t tlen = *cp++;
    গণনা alpnid id;
    len--;
    যদি(tlen > len)
      ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    যদি(id != ALPN_none) {
      যদি(idnum == MAX_HTTPSRR_ALPNS)
        ভঙ্গ;
      যদি(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      অন্যথা
        alpns[idnum++] = (unsigned ৱর্ণ)id;
    }
    cp += tlen;
    len -= tlen;
  }
  যদি(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ঘূৰাই CURLE_OK;
}

#ifdef CURLVERBOSE

স্থির CURLcode httpsrr_print_addr(সংরচনা dynbuf *dyn,
                                   পূর্ণাঙ্ক ai_family,
                                   ধ্রুৱ uint8_t *addr,
                                   size_t total_len)
{
  ৱর্ণ buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ধ্রুৱ ৱর্ণ *sep = "";
  CURLcode result = CURLE_OK;

  বাবে(i = 0; (i < (total_len / alen)) && !result; ++i) {
    যদি(curlx_inet_ntop(ai_family, addr + (i * alen), buf, পরিমাণ(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    অন্যথা
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ঘূৰাই result;
}

শূন্য Curl_httpsrr_trace(সংরচনা Curl_easy *data,
                        সংরচনা Curl_https_rrinfo *rr)
{
  সংরচনা dynbuf tmp;
  CURLcode result;

  যদি(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    ঘূৰাই;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  যদি(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  অন্যথা
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(সংরচনা dynbuf *tmp,
                            সংরচনা Curl_https_rrinfo *rr)
{
  CURLcode result;
  পূর্ণাঙ্ক i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  যদি(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  যদি(!result && rr->alpns[0]) {
    ধ্রুৱ ৱর্ণ *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    বাবে(i = 0; !result && (i < 4); ++i) {
      ৱিকল্প(rr->alpns[i]) {
      পক্ষ ALPN_h1:
        name = "http/1.1";
        ভঙ্গ;
      পক্ষ ALPN_h2:
        name = "h2";
        ভঙ্গ;
      পক্ষ ALPN_h3:
        name = "h3";
        ভঙ্গ;
      স্ৱভাৱ:
        name = রিক্ত;
      }
      যদি(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  যদি(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  যদি(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  যদি(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    যদি(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  যদি(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    যদি(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  যদি(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ঘূৰাই result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(সংরচনা Curl_https_rrinfo *rr,
                          uint16_t rrkey, ধ্রুৱ uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  ৱিকল্প(rrkey) {
  পক্ষ HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_NO_DEF_ALPN:
    যদি(vlen) /* no data */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_IPV4: /* addr4 list */
    যদি(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    যদি(!rr->ipv4hints)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_ECH:
    যদি(!vlen)
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    যদি(!rr->echconfiglist)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_IPV6: /* addr6 list */
    যদি(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    যদি(!rr->ipv6hints)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    ভঙ্গ;
  পক্ষ HTTPS_RR_CODE_PORT:
    যদি(vlen != 2)
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    ভঙ্গ;
  স্ৱভাৱ:
    /* unknown code */
    ভঙ্গ;
  }
  ঘূৰাই result;
}

শূন্য Curl_httpsrr_destroy(সংরচনা Curl_https_rrinfo *rrinfo)
{
  যদি(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(সংরচনা Curl_easy *data,
                             ধ্রুৱ সংরচনা Curl_https_rrinfo *rr)
{
  যদি(!data->conn)
    ঘূৰাই FALSE;
  ঘূৰাই Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(সংরচনা Curl_peer *peer,
                              ধ্রুৱ সংরচনা Curl_https_rrinfo *rr)
{
  যদি(!peer || !rr)
    ঘূৰাই FALSE;
  ঘূৰাই (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

স্থির CURLcode httpsrr_opt(ধ্রুৱ ares_dns_rr_t *rr,
                            ares_dns_rr_চাবি_t চাবি, size_t idx,
                            সংরচনা Curl_https_rrinfo *hinfo)
{
  ধ্রুৱ unsigned ৱর্ণ *val = রিক্ত;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, চাবি, idx, &val, &len);
  ঘূৰাই Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ধ্রুৱ ares_dns_record_t *dnsrec,
                                সংরচনা Curl_https_rrinfo **phinfo)
{
  সংরচনা Curl_https_rrinfo *hinfo = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, পরিমাণ(*hinfo));
  যদি(!hinfo)
    গচ্ছ out;

  বাবে(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ধ্রুৱ ৱর্ণ *target;
    size_t opt;
    ধ্রুৱ ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    যদি(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      অনুৱর্ত;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    যদি(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      যদি(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    বাবে(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      যদি(result)
        গচ্ছ out;
    }
  }
  result = CURLE_OK;

out:
  যদি(result) {
    *phinfo = রিক্ত;
    Curl_httpsrr_destroy(hinfo);
  }
  অন্যথা {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ঘূৰাই result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
