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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_HTTPSRR

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/inet_ntop.h"

স্থির CURLcode httpsrr_decode_alpn(ধ্রুবক uint8_t *cp, size_t len,
                                    unsigned অক্ষর *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  পূর্ণসংখ্যা idnum = 0;

  যখন(len > 0) {
    size_t tlen = *cp++;
    গণনা alpnid id;
    len--;
    যদি(tlen > len)
      ফেরত CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    যদি(id != ALPN_none) {
      যদি(idnum == MAX_HTTPSRR_ALPNS)
        বিরতি;
      যদি(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      অন্যথা
        alpns[idnum++] = (unsigned অক্ষর)id;
    }
    cp += tlen;
    len -= tlen;
  }
  যদি(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ফেরত CURLE_OK;
}

#ifdef CURLVERBOSE

স্থির CURLcode httpsrr_print_addr(কাঠামো dynbuf *dyn,
                                   পূর্ণসংখ্যা ai_family,
                                   ধ্রুবক uint8_t *addr,
                                   size_t যোগ_len)
{
  অক্ষর buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ধ্রুবক অক্ষর *sep = "";
  CURLcode ফলাফল = CURLE_OK;

  জন্য(i = 0; (i < (যোগ_len / alen)) && !ফলাফল; ++i) {
    যদি(curlx_inet_ntop(ai_family, addr + (i * alen), buf, আকার(buf)))
      ফলাফল = curlx_dyn_add(dyn, "<error parsing address>");
    অন্যথা
      ফলাফল = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ফেরত ফলাফল;
}

শূন্য Curl_httpsrr_trace(কাঠামো Curl_easy *তথ্য,
                        কাঠামো Curl_https_rrinfo *rr)
{
  কাঠামো dynbuf tmp;
  CURLcode ফলাফল;

  যদি(!rr) {
    CURL_TRC_DNS(তথ্য, "[HTTPS] no record available");
    ফেরত;
  }
  curlx_dyn_init(&tmp, 1024);
  ফলাফল = Curl_httpsrr_print(&tmp, rr);
  যদি(!ফলাফল)
    CURL_TRC_DNS(তথ্য, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  অন্যথা
    CURL_TRC_DNS(তথ্য, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(কাঠামো dynbuf *tmp,
                            কাঠামো Curl_https_rrinfo *rr)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা i;

  curlx_dyn_reset(tmp);
  ফলাফল = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  যদি(!ফলাফল && rr->mandatory)
    ফলাফল = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  যদি(!ফলাফল && rr->alpns[0]) {
    ধ্রুবক অক্ষর *sep = "", *name;
    ফলাফল = curlx_dyn_add(tmp, " alpn=");
    জন্য(i = 0; !ফলাফল && (i < 4); ++i) {
      নির্বাচন(rr->alpns[i]) {
      ক্ষেত্র ALPN_h1:
        name = "http/1.1";
        বিরতি;
      ক্ষেত্র ALPN_h2:
        name = "h2";
        বিরতি;
      ক্ষেত্র ALPN_h3:
        name = "h3";
        বিরতি;
      পূর্বনির্ধারিত:
        name = ফাঁকা;
      }
      যদি(name) {
        ফলাফল = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  যদি(!ফলাফল && rr->port_set) {
    ফলাফল = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  যদি(!ফলাফল && rr->no_def_alpn)
    ফলাফল = curlx_dyn_add(tmp, " no-default-alpn");
  যদি(!ফলাফল && rr->ipv6hints_len) {
    ফলাফল = curlx_dyn_add(tmp, " ipv6hint=");
    যদি(!ফলাফল)
      ফলাফল = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  যদি(!ফলাফল && rr->ipv4hints_len) {
    ফলাফল = curlx_dyn_add(tmp, " ipv4hint=");
    যদি(!ফলাফল)
      ফলাফল = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  যদি(!ফলাফল && rr->echconfiglist_len)
    ফলাফল = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ফেরত ফলাফল;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(কাঠামো Curl_https_rrinfo *rr,
                          uint16_t rrkey, ধ্রুবক uint8_t *val, size_t vlen)
{
  CURLcode ফলাফল = CURLE_OK;
  নির্বাচন(rrkey) {
  ক্ষেত্র HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_ALPN: /* str_list */
    ফলাফল = httpsrr_decode_alpn(val, vlen, rr->alpns);
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_NO_DEF_ALPN:
    যদি(vlen) /* no data */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_IPV4: /* addr4 list */
    যদি(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    যদি(!rr->ipv4hints)
      ফেরত CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_ECH:
    যদি(!vlen)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    যদি(!rr->echconfiglist)
      ফেরত CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_IPV6: /* addr6 list */
    যদি(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    যদি(!rr->ipv6hints)
      ফেরত CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    বিরতি;
  ক্ষেত্র HTTPS_RR_CODE_PORT:
    যদি(vlen != 2)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    বিরতি;
  পূর্বনির্ধারিত:
    /* unknown code */
    বিরতি;
  }
  ফেরত ফলাফল;
}

শূন্য Curl_httpsrr_destroy(কাঠামো Curl_https_rrinfo *rrinfo)
{
  যদি(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো Curl_https_rrinfo *rr)
{
  যদি(!তথ্য->conn)
    ফেরত FALSE;
  ফেরত Curl_httpsrr_is_for_peer(তথ্য->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(কাঠামো Curl_peer *peer,
                              ধ্রুবক কাঠামো Curl_https_rrinfo *rr)
{
  যদি(!peer || !rr)
    ফেরত FALSE;
  ফেরত (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

স্থির CURLcode httpsrr_opt(ধ্রুবক ares_dns_rr_t *rr,
                            ares_dns_rr_চাবি_t চাবি, size_t idx,
                            কাঠামো Curl_https_rrinfo *hinfo)
{
  ধ্রুবক unsigned অক্ষর *val = ফাঁকা;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, চাবি, idx, &val, &len);
  ফেরত Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ধ্রুবক ares_dns_record_t *dnsrec,
                                কাঠামো Curl_https_rrinfo **phinfo)
{
  কাঠামো Curl_https_rrinfo *hinfo = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, আকার(*hinfo));
  যদি(!hinfo)
    যাও out;

  জন্য(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ধ্রুবক অক্ষর *target;
    size_t opt;
    ধ্রুবক ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    যদি(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      চালু;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    যদি(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      যদি(!hinfo->target) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    জন্য(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      ফলাফল = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      যদি(ফলাফল)
        যাও out;
    }
  }
  ফলাফল = CURLE_OK;

out:
  যদি(ফলাফল) {
    *phinfo = ফাঁকা;
    Curl_httpsrr_destroy(hinfo);
  }
  অন্যথা {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ফেরত ফলাফল;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
