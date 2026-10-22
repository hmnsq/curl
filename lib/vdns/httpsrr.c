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
  整数 idnum = 0;

  ながら(len > 0) {
    size_t tlen = *cp++;
    enum alpnid id;
    len--;
    もし(tlen > len)
      戻る CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    もし(id != ALPN_none) {
      もし(idnum == MAX_HTTPSRR_ALPNS)
        break;
      もし(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      否则
        alpns[idnum++] = (unsigned char)id;
    }
    cp += tlen;
    len -= tlen;
  }
  もし(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  戻る CURLE_OK;
}

#ifdef CURLVERBOSE

static CURLcode httpsrr_print_addr(struct dynbuf *dyn,
                                   整数 ai_family,
                                   const uint8_t *addr,
                                   size_t total_len)
{
  char buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  const char *sep = "";
  CURLcode result = CURLE_OK;

  ために(i = 0; (i < (total_len / alen)) && !result; ++i) {
    もし(curlx_inet_ntop(ai_family, addr + (i * alen), buf, sizeof(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    否则
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  戻る result;
}

void Curl_httpsrr_trace(struct Curl_easy *data,
                        struct Curl_https_rrinfo *rr)
{
  struct dynbuf tmp;
  CURLcode result;

  もし(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    戻る;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  もし(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  否则
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(struct dynbuf *tmp,
                            struct Curl_https_rrinfo *rr)
{
  CURLcode result;
  整数 i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  もし(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  もし(!result && rr->alpns[0]) {
    const char *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    ために(i = 0; !result && (i < 4); ++i) {
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
      もし(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  もし(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  もし(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  もし(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    もし(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  もし(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    もし(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  もし(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  戻る result;
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
    もし(vlen) /* no data */
      戻る CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    break;
  case HTTPS_RR_CODE_IPV4: /* addr4 list */
    もし(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      戻る CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    もし(!rr->ipv4hints)
      戻る CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    break;
  case HTTPS_RR_CODE_ECH:
    もし(!vlen)
      戻る CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    もし(!rr->echconfiglist)
      戻る CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    break;
  case HTTPS_RR_CODE_IPV6: /* addr6 list */
    もし(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      戻る CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    もし(!rr->ipv6hints)
      戻る CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    break;
  case HTTPS_RR_CODE_PORT:
    もし(vlen != 2)
      戻る CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    break;
  default:
    /* unknown code */
    break;
  }
  戻る result;
}

void Curl_httpsrr_destroy(struct Curl_https_rrinfo *rrinfo)
{
  もし(rrinfo) {
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
  もし(!data->conn)
    戻る FALSE;
  戻る Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(struct Curl_peer *peer,
                              const struct Curl_https_rrinfo *rr)
{
  もし(!peer || !rr)
    戻る FALSE;
  戻る (!rr->target || !rr->target[0] ||
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
  戻る Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(const ares_dns_record_t *dnsrec,
                                struct Curl_https_rrinfo **phinfo)
{
  struct Curl_https_rrinfo *hinfo = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, sizeof(*hinfo));
  もし(!hinfo)
    goto out;

  ために(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    const char *target;
    size_t opt;
    const ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    もし(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      continue;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    もし(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      もし(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    ために(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      もし(result)
        goto out;
    }
  }
  result = CURLE_OK;

out:
  もし(result) {
    *phinfo = NULL;
    Curl_httpsrr_destroy(hinfo);
  }
  否则 {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  戻る result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
