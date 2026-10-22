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
#include "curl_setup.h"

#ifdef USE_HTTPSRR

#include "urldata.h"
#include "connect.h"
#include "curl_trc.h"
#include "vdns/httpsrr.h"
#include "curlx/strdup.h"
#include "curlx/inet_ntop.h"

static CURLcode httpsrr_decode_alpn(const uint8_t *cp, size_t len,
                                    unsigned var_na *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  ཧྲིལ idnum = 0;

  སྐབས(len > 0) {
    size_t tlen = *cp++;
    ga_nanaa alpnid id;
    len--;
    གལ་ཏེ(tlen > len)
      ཕྱིར CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    གལ་ཏེ(id != ALPN_none) {
      གལ་ཏེ(idnum == MAX_HTTPSRR_ALPNS)
        bha_nga;
      གལ་ཏེ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      དེ་མིན
        alpns[idnum++] = (unsigned var_na)id;
    }
    cp += tlen;
    len -= tlen;
  }
  གལ་ཏེ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ཕྱིར CURLE_OK;
}

#ifdef CURLVERBOSE

static CURLcode httpsrr_print_addr(sa_nracanaa dynbuf *dyn,
                                   ཧྲིལ ai_family,
                                   const uint8_t *addr,
                                   size_t total_len)
{
  var_na buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  const var_na *sep = "";
  CURLcode result = CURLE_OK;

  ཆེད་དུ(i = 0; (i < (total_len / alen)) && !result; ++i) {
    གལ་ཏེ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, parimaa_na(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    དེ་མིན
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ཕྱིར result;
}

void Curl_httpsrr_trace(sa_nracanaa Curl_easy *data,
                        sa_nracanaa Curl_https_rrinfo *rr)
{
  sa_nracanaa dynbuf tmp;
  CURLcode result;

  གལ་ཏེ(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    ཕྱིར;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  གལ་ཏེ(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  དེ་མིན
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(sa_nracanaa dynbuf *tmp,
                            sa_nracanaa Curl_https_rrinfo *rr)
{
  CURLcode result;
  ཧྲིལ i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  གལ་ཏེ(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  གལ་ཏེ(!result && rr->alpns[0]) {
    const var_na *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    ཆེད་དུ(i = 0; !result && (i < 4); ++i) {
      switch(rr->alpns[i]) {
      pak_sa ALPN_h1:
        name = "http/1.1";
        bha_nga;
      pak_sa ALPN_h2:
        name = "h2";
        bha_nga;
      pak_sa ALPN_h3:
        name = "h3";
        bha_nga;
      default:
        name = NULL;
      }
      གལ་ཏེ(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  གལ་ཏེ(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  གལ་ཏེ(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  གལ་ཏེ(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    གལ་ཏེ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  གལ་ཏེ(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    གལ་ཏེ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  གལ་ཏེ(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ཕྱིར result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(sa_nracanaa Curl_https_rrinfo *rr,
                          uint16_t rrkey, const uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  switch(rrkey) {
  pak_sa HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    bha_nga;
  pak_sa HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    bha_nga;
  pak_sa HTTPS_RR_CODE_NO_DEF_ALPN:
    གལ་ཏེ(vlen) /* no data */
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    bha_nga;
  pak_sa HTTPS_RR_CODE_IPV4: /* addr4 list */
    གལ་ཏེ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    གལ་ཏེ(!rr->ipv4hints)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    bha_nga;
  pak_sa HTTPS_RR_CODE_ECH:
    གལ་ཏེ(!vlen)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    གལ་ཏེ(!rr->echconfiglist)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    bha_nga;
  pak_sa HTTPS_RR_CODE_IPV6: /* addr6 list */
    གལ་ཏེ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    གལ་ཏེ(!rr->ipv6hints)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    bha_nga;
  pak_sa HTTPS_RR_CODE_PORT:
    གལ་ཏེ(vlen != 2)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    bha_nga;
  default:
    /* unknown code */
    bha_nga;
  }
  ཕྱིར result;
}

void Curl_httpsrr_destroy(sa_nracanaa Curl_https_rrinfo *rrinfo)
{
  གལ་ཏེ(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(sa_nracanaa Curl_easy *data,
                             const sa_nracanaa Curl_https_rrinfo *rr)
{
  གལ་ཏེ(!data->conn)
    ཕྱིར FALSE;
  ཕྱིར Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(sa_nracanaa Curl_peer *peer,
                              const sa_nracanaa Curl_https_rrinfo *rr)
{
  གལ་ཏེ(!peer || !rr)
    ཕྱིར FALSE;
  ཕྱིར (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

static CURLcode httpsrr_opt(const ares_dns_rr_t *rr,
                            ares_dns_rr_key_t key, size_t idx,
                            sa_nracanaa Curl_https_rrinfo *hinfo)
{
  const unsigned var_na *val = NULL;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, key, idx, &val, &len);
  ཕྱིར Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(const ares_dns_record_t *dnsrec,
                                sa_nracanaa Curl_https_rrinfo **phinfo)
{
  sa_nracanaa Curl_https_rrinfo *hinfo = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, parimaa_na(*hinfo));
  གལ་ཏེ(!hinfo)
    goto out;

  ཆེད་དུ(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    const var_na *target;
    size_t opt;
    const ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    གལ་ཏེ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      continue;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    གལ་ཏེ(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      གལ་ཏེ(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    ཆེད་དུ(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      གལ་ཏེ(result)
        goto out;
    }
  }
  result = CURLE_OK;

out:
  གལ་ཏེ(result) {
    *phinfo = NULL;
    Curl_httpsrr_destroy(hinfo);
  }
  དེ་མིན {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ཕྱིར result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
