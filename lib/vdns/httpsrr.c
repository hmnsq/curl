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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_HTTPSRR

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/inet_ntop.h"

நிலையான CURLcode httpsrr_decode_alpn(மாறிலி uint8_t *cp, size_t len,
                                    unsigned எழுத்து *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  முழு idnum = 0;

  போது(len > 0) {
    size_t tlen = *cp++;
    எண்ணிக்கை alpnid id;
    len--;
    எனில்(tlen > len)
      திரும்பு CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    எனில்(id != ALPN_none) {
      எனில்(idnum == MAX_HTTPSRR_ALPNS)
        நிறுத்து;
      எனில்(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      இல்லையெனில்
        alpns[idnum++] = (unsigned எழுத்து)id;
    }
    cp += tlen;
    len -= tlen;
  }
  எனில்(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  திரும்பு CURLE_OK;
}

#ifdef CURLVERBOSE

நிலையான CURLcode httpsrr_print_addr(கட்டமைப்பு dynbuf *dyn,
                                   முழு ai_family,
                                   மாறிலி uint8_t *addr,
                                   size_t total_len)
{
  எழுத்து buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  மாறிலி எழுத்து *sep = "";
  CURLcode result = CURLE_OK;

  க்கு(i = 0; (i < (total_len / alen)) && !result; ++i) {
    எனில்(curlx_inet_ntop(ai_family, addr + (i * alen), buf, அளவு(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    இல்லையெனில்
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  திரும்பு result;
}

வெறுமை Curl_httpsrr_trace(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு Curl_https_rrinfo *rr)
{
  கட்டமைப்பு dynbuf tmp;
  CURLcode result;

  எனில்(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    திரும்பு;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  எனில்(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  இல்லையெனில்
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(கட்டமைப்பு dynbuf *tmp,
                            கட்டமைப்பு Curl_https_rrinfo *rr)
{
  CURLcode result;
  முழு i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  எனில்(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  எனில்(!result && rr->alpns[0]) {
    மாறிலி எழுத்து *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    க்கு(i = 0; !result && (i < 4); ++i) {
      தேர்வு(rr->alpns[i]) {
      நிலை ALPN_h1:
        name = "http/1.1";
        நிறுத்து;
      நிலை ALPN_h2:
        name = "h2";
        நிறுத்து;
      நிலை ALPN_h3:
        name = "h3";
        நிறுத்து;
      இயல்பு:
        name = வெற்று;
      }
      எனில்(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  எனில்(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  எனில்(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  எனில்(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    எனில்(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  எனில்(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    எனில்(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  எனில்(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  திரும்பு result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(கட்டமைப்பு Curl_https_rrinfo *rr,
                          uint16_t rrkey, மாறிலி uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  தேர்வு(rrkey) {
  நிலை HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    நிறுத்து;
  நிலை HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    நிறுத்து;
  நிலை HTTPS_RR_CODE_NO_DEF_ALPN:
    எனில்(vlen) /* no data */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    நிறுத்து;
  நிலை HTTPS_RR_CODE_IPV4: /* addr4 list */
    எனில்(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    எனில்(!rr->ipv4hints)
      திரும்பு CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    நிறுத்து;
  நிலை HTTPS_RR_CODE_ECH:
    எனில்(!vlen)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    எனில்(!rr->echconfiglist)
      திரும்பு CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    நிறுத்து;
  நிலை HTTPS_RR_CODE_IPV6: /* addr6 list */
    எனில்(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    எனில்(!rr->ipv6hints)
      திரும்பு CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    நிறுத்து;
  நிலை HTTPS_RR_CODE_PORT:
    எனில்(vlen != 2)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    நிறுத்து;
  இயல்பு:
    /* unknown code */
    நிறுத்து;
  }
  திரும்பு result;
}

வெறுமை Curl_httpsrr_destroy(கட்டமைப்பு Curl_https_rrinfo *rrinfo)
{
  எனில்(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு Curl_https_rrinfo *rr)
{
  எனில்(!data->conn)
    திரும்பு FALSE;
  திரும்பு Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(கட்டமைப்பு Curl_peer *peer,
                              மாறிலி கட்டமைப்பு Curl_https_rrinfo *rr)
{
  எனில்(!peer || !rr)
    திரும்பு FALSE;
  திரும்பு (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

நிலையான CURLcode httpsrr_opt(மாறிலி ares_dns_rr_t *rr,
                            ares_dns_rr_சாவி_t சாவி, size_t idx,
                            கட்டமைப்பு Curl_https_rrinfo *hinfo)
{
  மாறிலி unsigned எழுத்து *val = வெற்று;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, சாவி, idx, &val, &len);
  திரும்பு Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(மாறிலி ares_dns_record_t *dnsrec,
                                கட்டமைப்பு Curl_https_rrinfo **phinfo)
{
  கட்டமைப்பு Curl_https_rrinfo *hinfo = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, அளவு(*hinfo));
  எனில்(!hinfo)
    செல் out;

  க்கு(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    மாறிலி எழுத்து *target;
    size_t opt;
    மாறிலி ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    எனில்(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      தொடர்;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    எனில்(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      எனில்(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    க்கு(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      எனில்(result)
        செல் out;
    }
  }
  result = CURLE_OK;

out:
  எனில்(result) {
    *phinfo = வெற்று;
    Curl_httpsrr_destroy(hinfo);
  }
  இல்லையெனில் {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  திரும்பு result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
