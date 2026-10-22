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
                                    unsigned char *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  정수 idnum = 0;

  동안(len > 0) {
    size_t tlen = *cp++;
    enum alpnid id;
    len--;
    만약(tlen > len)
      반환 CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    만약(id != ALPN_none) {
      만약(idnum == MAX_HTTPSRR_ALPNS)
        break;
      만약(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      아니면
        alpns[idnum++] = (unsigned char)id;
    }
    cp += tlen;
    len -= tlen;
  }
  만약(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  반환 CURLE_OK;
}

#ifdef CURLVERBOSE

static CURLcode httpsrr_print_addr(struct dynbuf *dyn,
                                   정수 ai_family,
                                   const uint8_t *addr,
                                   size_t total_len)
{
  char buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  const char *sep = "";
  CURLcode result = CURLE_OK;

  위해(i = 0; (i < (total_len / alen)) && !result; ++i) {
    만약(curlx_inet_ntop(ai_family, addr + (i * alen), buf, sizeof(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    아니면
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  반환 result;
}

void Curl_httpsrr_trace(struct Curl_easy *data,
                        struct Curl_https_rrinfo *rr)
{
  struct dynbuf tmp;
  CURLcode result;

  만약(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    반환;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  만약(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  아니면
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(struct dynbuf *tmp,
                            struct Curl_https_rrinfo *rr)
{
  CURLcode result;
  정수 i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  만약(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  만약(!result && rr->alpns[0]) {
    const char *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    위해(i = 0; !result && (i < 4); ++i) {
      switch(rr->alpns[i]) {
      case ALPN_h1:
        name = "http/1.1";
        break;
      case ALPN_h2:
        name = "h2";
        break;
      case ALPN_h3:
        name = "h3";
        break;
      default:
        name = NULL;
      }
      만약(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  만약(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  만약(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  만약(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    만약(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  만약(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    만약(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  만약(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  반환 result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(struct Curl_https_rrinfo *rr,
                          uint16_t rrkey, const uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  switch(rrkey) {
  case HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    break;
  case HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    break;
  case HTTPS_RR_CODE_NO_DEF_ALPN:
    만약(vlen) /* no data */
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    break;
  case HTTPS_RR_CODE_IPV4: /* addr4 list */
    만약(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    만약(!rr->ipv4hints)
      반환 CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    break;
  case HTTPS_RR_CODE_ECH:
    만약(!vlen)
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    만약(!rr->echconfiglist)
      반환 CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    break;
  case HTTPS_RR_CODE_IPV6: /* addr6 list */
    만약(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    만약(!rr->ipv6hints)
      반환 CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    break;
  case HTTPS_RR_CODE_PORT:
    만약(vlen != 2)
      반환 CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    break;
  default:
    /* unknown code */
    break;
  }
  반환 result;
}

void Curl_httpsrr_destroy(struct Curl_https_rrinfo *rrinfo)
{
  만약(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(struct Curl_easy *data,
                             const struct Curl_https_rrinfo *rr)
{
  만약(!data->conn)
    반환 FALSE;
  반환 Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(struct Curl_peer *peer,
                              const struct Curl_https_rrinfo *rr)
{
  만약(!peer || !rr)
    반환 FALSE;
  반환 (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

static CURLcode httpsrr_opt(const ares_dns_rr_t *rr,
                            ares_dns_rr_key_t key, size_t idx,
                            struct Curl_https_rrinfo *hinfo)
{
  const unsigned char *val = NULL;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, key, idx, &val, &len);
  반환 Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(const ares_dns_record_t *dnsrec,
                                struct Curl_https_rrinfo **phinfo)
{
  struct Curl_https_rrinfo *hinfo = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, sizeof(*hinfo));
  만약(!hinfo)
    goto out;

  위해(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    const char *target;
    size_t opt;
    const ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    만약(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      continue;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    만약(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      만약(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    위해(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      만약(result)
        goto out;
    }
  }
  result = CURLE_OK;

out:
  만약(result) {
    *phinfo = NULL;
    Curl_httpsrr_destroy(hinfo);
  }
  아니면 {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  반환 result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
