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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_HTTPSRR

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/inet_ntop.h"

स्थिर CURLcode httpsrr_decode_alpn(ध्रुव uint8_t *cp, size_t len,
                                    unsigned वर्ण *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  पूर्णाङ्क idnum = 0;

  याव(len > 0) {
    size_t tlen = *cp++;
    गणना alpnid id;
    len--;
    सचे(tlen > len)
      पटिनिवत्त CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    सचे(id != ALPN_none) {
      सचे(idnum == MAX_HTTPSRR_ALPNS)
        भङ्ग;
      सचे(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      अञ्ञथा
        alpns[idnum++] = (unsigned वर्ण)id;
    }
    cp += tlen;
    len -= tlen;
  }
  सचे(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  पटिनिवत्त CURLE_OK;
}

#ifdef CURLVERBOSE

स्थिर CURLcode httpsrr_print_addr(संरचना dynbuf *dyn,
                                   पूर्णाङ्क ai_family,
                                   ध्रुव uint8_t *addr,
                                   size_t total_len)
{
  वर्ण buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ध्रुव वर्ण *sep = "";
  CURLcode result = CURLE_OK;

  अत्थाय(i = 0; (i < (total_len / alen)) && !result; ++i) {
    सचे(curlx_inet_ntop(ai_family, addr + (i * alen), buf, परिमाण(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    अञ्ञथा
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  पटिनिवत्त result;
}

शून्य Curl_httpsrr_trace(संरचना Curl_easy *data,
                        संरचना Curl_https_rrinfo *rr)
{
  संरचना dynbuf tmp;
  CURLcode result;

  सचे(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    पटिनिवत्त;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  सचे(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  अञ्ञथा
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(संरचना dynbuf *tmp,
                            संरचना Curl_https_rrinfo *rr)
{
  CURLcode result;
  पूर्णाङ्क i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  सचे(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  सचे(!result && rr->alpns[0]) {
    ध्रुव वर्ण *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    अत्थाय(i = 0; !result && (i < 4); ++i) {
      विकल्प(rr->alpns[i]) {
      पक्ष ALPN_h1:
        name = "http/1.1";
        भङ्ग;
      पक्ष ALPN_h2:
        name = "h2";
        भङ्ग;
      पक्ष ALPN_h3:
        name = "h3";
        भङ्ग;
      स्वभाव:
        name = रिक्त;
      }
      सचे(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  सचे(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  सचे(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  सचे(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    सचे(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  सचे(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    सचे(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  सचे(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  पटिनिवत्त result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(संरचना Curl_https_rrinfo *rr,
                          uint16_t rrkey, ध्रुव uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  विकल्प(rrkey) {
  पक्ष HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    भङ्ग;
  पक्ष HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    भङ्ग;
  पक्ष HTTPS_RR_CODE_NO_DEF_ALPN:
    सचे(vlen) /* no data */
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    भङ्ग;
  पक्ष HTTPS_RR_CODE_IPV4: /* addr4 list */
    सचे(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    सचे(!rr->ipv4hints)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_CODE_ECH:
    सचे(!vlen)
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    सचे(!rr->echconfiglist)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_CODE_IPV6: /* addr6 list */
    सचे(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    सचे(!rr->ipv6hints)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_CODE_PORT:
    सचे(vlen != 2)
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    भङ्ग;
  स्वभाव:
    /* unknown code */
    भङ्ग;
  }
  पटिनिवत्त result;
}

शून्य Curl_httpsrr_destroy(संरचना Curl_https_rrinfo *rrinfo)
{
  सचे(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(संरचना Curl_easy *data,
                             ध्रुव संरचना Curl_https_rrinfo *rr)
{
  सचे(!data->conn)
    पटिनिवत्त FALSE;
  पटिनिवत्त Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(संरचना Curl_peer *peer,
                              ध्रुव संरचना Curl_https_rrinfo *rr)
{
  सचे(!peer || !rr)
    पटिनिवत्त FALSE;
  पटिनिवत्त (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

स्थिर CURLcode httpsrr_opt(ध्रुव ares_dns_rr_t *rr,
                            ares_dns_rr_कुञ्चिका_t कुञ्चिका, size_t idx,
                            संरचना Curl_https_rrinfo *hinfo)
{
  ध्रुव unsigned वर्ण *val = रिक्त;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, कुञ्चिका, idx, &val, &len);
  पटिनिवत्त Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ध्रुव ares_dns_record_t *dnsrec,
                                संरचना Curl_https_rrinfo **phinfo)
{
  संरचना Curl_https_rrinfo *hinfo = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, परिमाण(*hinfo));
  सचे(!hinfo)
    गच्छ out;

  अत्थाय(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ध्रुव वर्ण *target;
    size_t opt;
    ध्रुव ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    सचे(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      अनुवर्त;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    सचे(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      सचे(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    अत्थाय(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      सचे(result)
        गच्छ out;
    }
  }
  result = CURLE_OK;

out:
  सचे(result) {
    *phinfo = रिक्त;
    Curl_httpsrr_destroy(hinfo);
  }
  अञ्ञथा {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  पटिनिवत्त result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
