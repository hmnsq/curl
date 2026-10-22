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

#ifdef USE_HTTPSRR

#شامل "urldata.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/strdup.h"
#شامل "curlx/inet_ntop.h"

ایستا CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    unsigned نویسه *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  صحیح idnum = 0;

  وقتی(len > 0) {
    size_t tlen = *cp++;
    شمارش alpnid id;
    len--;
    اگر(tlen > len)
      بازگشت CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    اگر(id != ALPN_none) {
      اگر(idnum == MAX_HTTPSRR_ALPNS)
        بشکن;
      اگر(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      وگرنه
        alpns[idnum++] = (unsigned نویسه)id;
    }
    cp += tlen;
    len -= tlen;
  }
  اگر(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  بازگشت CURLE_OK;
}

#ifdef CURLVERBOSE

ایستا CURLcode httpsrr_print_addr(ساختار dynbuf *dyn,
                                   صحیح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t total_len)
{
  نویسه buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت نویسه *sep = "";
  CURLcode result = CURLE_OK;

  برای(i = 0; (i < (total_len / alen)) && !result; ++i) {
    اگر(curlx_inet_ntop(ai_family, addr + (i * alen), buf, اندازه(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    وگرنه
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  بازگشت result;
}

تهی Curl_httpsrr_trace(ساختار Curl_easy *data,
                        ساختار Curl_https_rrinfo *rr)
{
  ساختار dynbuf tmp;
  CURLcode result;

  اگر(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    بازگشت;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  اگر(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  وگرنه
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ساختار dynbuf *tmp,
                            ساختار Curl_https_rrinfo *rr)
{
  CURLcode result;
  صحیح i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  اگر(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  اگر(!result && rr->alpns[0]) {
    ثابت نویسه *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    برای(i = 0; !result && (i < 4); ++i) {
      انتخاب(rr->alpns[i]) {
      حالت ALPN_h1:
        name = "http/1.1";
        بشکن;
      حالت ALPN_h2:
        name = "h2";
        بشکن;
      حالت ALPN_h3:
        name = "h3";
        بشکن;
      پیشفرض:
        name = پوچ;
      }
      اگر(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  اگر(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  اگر(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  اگر(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    اگر(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  اگر(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    اگر(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  اگر(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  بازگشت result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(ساختار Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  انتخاب(rrkey) {
  حالت HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    بشکن;
  حالت HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    بشکن;
  حالت HTTPS_RR_CODE_NO_DEF_ALPN:
    اگر(vlen) /* no data */
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    بشکن;
  حالت HTTPS_RR_CODE_IPV4: /* addr4 list */
    اگر(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    اگر(!rr->ipv4hints)
      بازگشت CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    بشکن;
  حالت HTTPS_RR_CODE_ECH:
    اگر(!vlen)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    اگر(!rr->echconfiglist)
      بازگشت CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    بشکن;
  حالت HTTPS_RR_CODE_IPV6: /* addr6 list */
    اگر(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    اگر(!rr->ipv6hints)
      بازگشت CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    بشکن;
  حالت HTTPS_RR_CODE_PORT:
    اگر(vlen != 2)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    بشکن;
  پیشفرض:
    /* unknown code */
    بشکن;
  }
  بازگشت result;
}

تهی Curl_httpsrr_destroy(ساختار Curl_https_rrinfo *rrinfo)
{
  اگر(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ساختار Curl_easy *data,
                             ثابت ساختار Curl_https_rrinfo *rr)
{
  اگر(!data->conn)
    بازگشت FALSE;
  بازگشت Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ساختار Curl_peer *peer,
                              ثابت ساختار Curl_https_rrinfo *rr)
{
  اگر(!peer || !rr)
    بازگشت FALSE;
  بازگشت (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

ایستا CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_کلید_t کلید, size_t idx,
                            ساختار Curl_https_rrinfo *hinfo)
{
  ثابت unsigned نویسه *val = پوچ;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, کلید, idx, &val, &len);
  بازگشت Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_record_t *dnsrec,
                                ساختار Curl_https_rrinfo **phinfo)
{
  ساختار Curl_https_rrinfo *hinfo = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, اندازه(*hinfo));
  اگر(!hinfo)
    برو out;

  برای(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت نویسه *target;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    اگر(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ادامه;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    اگر(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      اگر(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    برای(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      اگر(result)
        برو out;
    }
  }
  result = CURLE_OK;

out:
  اگر(result) {
    *phinfo = پوچ;
    Curl_httpsrr_destroy(hinfo);
  }
  وگرنه {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  بازگشت result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
