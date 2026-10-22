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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_HTTPSRR

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/inet_ntop.h"

സ്ഥിതം CURLcode httpsrr_decode_alpn(സ്ഥിരം uint8_t *cp, size_t len,
                                    unsigned അക്ഷരം *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  പൂർണ്ണസംഖ്യ idnum = 0;

  ആകുമ്പോൾ(len > 0) {
    size_t tlen = *cp++;
    എണ്ണൽ alpnid id;
    len--;
    എങ്കിൽ(tlen > len)
      തിരികെ CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    എങ്കിൽ(id != ALPN_none) {
      എങ്കിൽ(idnum == MAX_HTTPSRR_ALPNS)
        നിർത്തുക;
      എങ്കിൽ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      അല്ലെങ്കിൽ
        alpns[idnum++] = (unsigned അക്ഷരം)id;
    }
    cp += tlen;
    len -= tlen;
  }
  എങ്കിൽ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  തിരികെ CURLE_OK;
}

#ifdef CURLVERBOSE

സ്ഥിതം CURLcode httpsrr_print_addr(ഘടന dynbuf *dyn,
                                   പൂർണ്ണസംഖ്യ ai_family,
                                   സ്ഥിരം uint8_t *addr,
                                   size_t total_len)
{
  അക്ഷരം buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  സ്ഥിരം അക്ഷരം *sep = "";
  CURLcode result = CURLE_OK;

  വേണ്ടി(i = 0; (i < (total_len / alen)) && !result; ++i) {
    എങ്കിൽ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, വലുപ്പം(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    അല്ലെങ്കിൽ
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  തിരികെ result;
}

ശൂന്യം Curl_httpsrr_trace(ഘടന Curl_easy *data,
                        ഘടന Curl_https_rrinfo *rr)
{
  ഘടന dynbuf tmp;
  CURLcode result;

  എങ്കിൽ(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    തിരികെ;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  എങ്കിൽ(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  അല്ലെങ്കിൽ
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ഘടന dynbuf *tmp,
                            ഘടന Curl_https_rrinfo *rr)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  എങ്കിൽ(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  എങ്കിൽ(!result && rr->alpns[0]) {
    സ്ഥിരം അക്ഷരം *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    വേണ്ടി(i = 0; !result && (i < 4); ++i) {
      തിരഞ്ഞെടുപ്പ്(rr->alpns[i]) {
      അവസ്ഥ ALPN_h1:
        name = "http/1.1";
        നിർത്തുക;
      അവസ്ഥ ALPN_h2:
        name = "h2";
        നിർത്തുക;
      അവസ്ഥ ALPN_h3:
        name = "h3";
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        name = ശൂന്യ;
      }
      എങ്കിൽ(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  എങ്കിൽ(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  എങ്കിൽ(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  എങ്കിൽ(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    എങ്കിൽ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  എങ്കിൽ(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    എങ്കിൽ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  എങ്കിൽ(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  തിരികെ result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(ഘടന Curl_https_rrinfo *rr,
                          uint16_t rrkey, സ്ഥിരം uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  തിരഞ്ഞെടുപ്പ്(rrkey) {
  അവസ്ഥ HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_NO_DEF_ALPN:
    എങ്കിൽ(vlen) /* no data */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_IPV4: /* addr4 list */
    എങ്കിൽ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    എങ്കിൽ(!rr->ipv4hints)
      തിരികെ CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_ECH:
    എങ്കിൽ(!vlen)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    എങ്കിൽ(!rr->echconfiglist)
      തിരികെ CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_IPV6: /* addr6 list */
    എങ്കിൽ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    എങ്കിൽ(!rr->ipv6hints)
      തിരികെ CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    നിർത്തുക;
  അവസ്ഥ HTTPS_RR_CODE_PORT:
    എങ്കിൽ(vlen != 2)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* unknown code */
    നിർത്തുക;
  }
  തിരികെ result;
}

ശൂന്യം Curl_httpsrr_destroy(ഘടന Curl_https_rrinfo *rrinfo)
{
  എങ്കിൽ(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന Curl_https_rrinfo *rr)
{
  എങ്കിൽ(!data->conn)
    തിരികെ FALSE;
  തിരികെ Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ഘടന Curl_peer *peer,
                              സ്ഥിരം ഘടന Curl_https_rrinfo *rr)
{
  എങ്കിൽ(!peer || !rr)
    തിരികെ FALSE;
  തിരികെ (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

സ്ഥിതം CURLcode httpsrr_opt(സ്ഥിരം ares_dns_rr_t *rr,
                            ares_dns_rr_താക്കോൽ_t താക്കോൽ, size_t idx,
                            ഘടന Curl_https_rrinfo *hinfo)
{
  സ്ഥിരം unsigned അക്ഷരം *val = ശൂന്യ;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, താക്കോൽ, idx, &val, &len);
  തിരികെ Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(സ്ഥിരം ares_dns_record_t *dnsrec,
                                ഘടന Curl_https_rrinfo **phinfo)
{
  ഘടന Curl_https_rrinfo *hinfo = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, വലുപ്പം(*hinfo));
  എങ്കിൽ(!hinfo)
    പോകുക out;

  വേണ്ടി(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    സ്ഥിരം അക്ഷരം *target;
    size_t opt;
    സ്ഥിരം ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    എങ്കിൽ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      തുടരുക;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    എങ്കിൽ(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      എങ്കിൽ(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    വേണ്ടി(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }
  result = CURLE_OK;

out:
  എങ്കിൽ(result) {
    *phinfo = ശൂന്യ;
    Curl_httpsrr_destroy(hinfo);
  }
  അല്ലെങ്കിൽ {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  തിരികെ result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
