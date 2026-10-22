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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_HTTPSRR

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"

ସ୍ଥିର CURLcode httpsrr_decode_alpn(ଅଚଳ uint8_t *cp, size_t len,
                                    unsigned ଅକ୍ଷର *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା idnum = 0;

  ଯେତେବେଳେ(len > 0) {
    size_t tlen = *cp++;
    ଗଣନା alpnid id;
    len--;
    ଯଦି(tlen > len)
      ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    ଯଦି(id != ALPN_none) {
      ଯଦି(idnum == MAX_HTTPSRR_ALPNS)
        ଭାଙ୍ଗ;
      ଯଦି(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      ଅନ୍ୟଥା
        alpns[idnum++] = (unsigned ଅକ୍ଷର)id;
    }
    cp += tlen;
    len -= tlen;
  }
  ଯଦି(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURLVERBOSE

ସ୍ଥିର CURLcode httpsrr_print_addr(ସଂରଚନା dynbuf *dyn,
                                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                   ଅଚଳ uint8_t *addr,
                                   size_t total_len)
{
  ଅକ୍ଷର buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ଅଚଳ ଅକ୍ଷର *sep = "";
  CURLcode result = CURLE_OK;

  ପାଇଁ(i = 0; (i < (total_len / alen)) && !result; ++i) {
    ଯଦି(curlx_inet_ntop(ai_family, addr + (i * alen), buf, ଆକାର(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    ଅନ୍ୟଥା
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_httpsrr_trace(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା Curl_https_rrinfo *rr)
{
  ସଂରଚନା dynbuf tmp;
  CURLcode result;

  ଯଦି(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    ଫେରନ୍ତୁ;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  ଯଦି(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  ଅନ୍ୟଥା
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ସଂରଚନା dynbuf *tmp,
                            ସଂରଚନା Curl_https_rrinfo *rr)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  ଯଦି(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  ଯଦି(!result && rr->alpns[0]) {
    ଅଚଳ ଅକ୍ଷର *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    ପାଇଁ(i = 0; !result && (i < 4); ++i) {
      ବାଛ(rr->alpns[i]) {
      ସ୍ଥିତି ALPN_h1:
        name = "http/1.1";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPN_h2:
        name = "h2";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPN_h3:
        name = "h3";
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        name = ଖାଲି;
      }
      ଯଦି(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  ଯଦି(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  ଯଦି(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  ଯଦି(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    ଯଦି(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  ଯଦି(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    ଯଦି(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  ଯଦି(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ଫେରନ୍ତୁ result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(ସଂରଚନା Curl_https_rrinfo *rr,
                          uint16_t rrkey, ଅଚଳ uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  ବାଛ(rrkey) {
  ସ୍ଥିତି HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_NO_DEF_ALPN:
    ଯଦି(vlen) /* no data */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_IPV4: /* addr4 list */
    ଯଦି(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    ଯଦି(!rr->ipv4hints)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_ECH:
    ଯଦି(!vlen)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    ଯଦି(!rr->echconfiglist)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_IPV6: /* addr6 list */
    ଯଦି(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    ଯଦି(!rr->ipv6hints)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPS_RR_CODE_PORT:
    ଯଦି(vlen != 2)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* unknown code */
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_httpsrr_destroy(ସଂରଚନା Curl_https_rrinfo *rrinfo)
{
  ଯଦି(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା Curl_https_rrinfo *rr)
{
  ଯଦି(!data->conn)
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ସଂରଚନା Curl_peer *peer,
                              ଅଚଳ ସଂରଚନା Curl_https_rrinfo *rr)
{
  ଯଦି(!peer || !rr)
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

ସ୍ଥିର CURLcode httpsrr_opt(ଅଚଳ ares_dns_rr_t *rr,
                            ares_dns_rr_ଚାବି_t ଚାବି, size_t idx,
                            ସଂରଚନା Curl_https_rrinfo *hinfo)
{
  ଅଚଳ unsigned ଅକ୍ଷର *val = ଖାଲି;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, ଚାବି, idx, &val, &len);
  ଫେରନ୍ତୁ Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ଅଚଳ ares_dns_record_t *dnsrec,
                                ସଂରଚନା Curl_https_rrinfo **phinfo)
{
  ସଂରଚନା Curl_https_rrinfo *hinfo = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, ଆକାର(*hinfo));
  ଯଦି(!hinfo)
    ଯାଅ out;

  ପାଇଁ(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ଅଚଳ ଅକ୍ଷର *target;
    size_t opt;
    ଅଚଳ ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    ଯଦି(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ଜାରି;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    ଯଦି(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      ଯଦି(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    ପାଇଁ(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      ଯଦି(result)
        ଯାଅ out;
    }
  }
  result = CURLE_OK;

out:
  ଯଦି(result) {
    *phinfo = ଖାଲି;
    Curl_httpsrr_destroy(hinfo);
  }
  ଅନ୍ୟଥା {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ଫେରନ୍ତୁ result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
