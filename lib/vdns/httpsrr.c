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

#ifdef USE_HTTPSRR

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/inet_ntop.h"

ساكن CURLcode httpsrr_decode_alpn(ثابت uint8_t *cp, size_t len,
                                    unsigned حرف *alpns)
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
    𐤀𐤌(tlen > len)
      أرجع CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    𐤀𐤌(id != ALPN_none) {
      𐤀𐤌(idnum == MAX_HTTPSRR_ALPNS)
        اكسر;
      𐤀𐤌(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      وإلا
        alpns[idnum++] = (unsigned حرف)id;
    }
    cp += tlen;
    len -= tlen;
  }
  𐤀𐤌(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  أرجع CURLE_OK;
}

#ifdef CURLVERBOSE

ساكن CURLcode httpsrr_print_addr(بنية dynbuf *dyn,
                                   صحيح ai_family,
                                   ثابت uint8_t *addr,
                                   size_t total_len)
{
  حرف buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ثابت حرف *sep = "";
  CURLcode result = CURLE_OK;

  لكل(i = 0; (i < (total_len / alen)) && !result; ++i) {
    𐤀𐤌(curlx_inet_ntop(ai_family, addr + (i * alen), buf, حجم(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    وإلا
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  أرجع result;
}

فراغ Curl_httpsrr_trace(بنية Curl_easy *data,
                        بنية Curl_https_rrinfo *rr)
{
  بنية dynbuf tmp;
  CURLcode result;

  𐤀𐤌(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    أرجع;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  𐤀𐤌(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  وإلا
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr)
{
  CURLcode result;
  صحيح i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  𐤀𐤌(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  𐤀𐤌(!result && rr->alpns[0]) {
    ثابت حرف *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    لكل(i = 0; !result && (i < 4); ++i) {
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
      𐤀𐤌(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  𐤀𐤌(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  𐤀𐤌(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  𐤀𐤌(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    𐤀𐤌(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  𐤀𐤌(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    𐤀𐤌(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  𐤀𐤌(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  أرجع result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  بدل(rrkey) {
  حالة HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    اكسر;
  حالة HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    اكسر;
  حالة HTTPS_RR_CODE_NO_DEF_ALPN:
    𐤀𐤌(vlen) /* no data */
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    اكسر;
  حالة HTTPS_RR_CODE_IPV4: /* addr4 list */
    𐤀𐤌(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    𐤀𐤌(!rr->ipv4hints)
      أرجع CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_CODE_ECH:
    𐤀𐤌(!vlen)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    𐤀𐤌(!rr->echconfiglist)
      أرجع CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    اكسر;
  حالة HTTPS_RR_CODE_IPV6: /* addr6 list */
    𐤀𐤌(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    𐤀𐤌(!rr->ipv6hints)
      أرجع CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    اكسر;
  حالة HTTPS_RR_CODE_PORT:
    𐤀𐤌(vlen != 2)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    اكسر;
  افتراضي:
    /* unknown code */
    اكسر;
  }
  أرجع result;
}

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo)
{
  𐤀𐤌(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(بنية Curl_easy *data,
                             ثابت بنية Curl_https_rrinfo *rr)
{
  𐤀𐤌(!data->conn)
    أرجع FALSE;
  أرجع Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr)
{
  𐤀𐤌(!peer || !rr)
    أرجع FALSE;
  أرجع (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

ساكن CURLcode httpsrr_opt(ثابت ares_dns_rr_t *rr,
                            ares_dns_rr_key_t key, size_t idx,
                            بنية Curl_https_rrinfo *hinfo)
{
  ثابت unsigned حرف *val = عدم;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, key, idx, &val, &len);
  أرجع Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_record_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo)
{
  بنية Curl_https_rrinfo *hinfo = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, حجم(*hinfo));
  𐤀𐤌(!hinfo)
    اذهب out;

  لكل(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ثابت حرف *target;
    size_t opt;
    ثابت ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    𐤀𐤌(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      استمر;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    𐤀𐤌(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      𐤀𐤌(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    لكل(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      𐤀𐤌(result)
        اذهب out;
    }
  }
  result = CURLE_OK;

out:
  𐤀𐤌(result) {
    *phinfo = عدم;
    Curl_httpsrr_destroy(hinfo);
  }
  وإلا {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  أرجع result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
