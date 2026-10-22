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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_HTTPSRR

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"

ಸ್ಥಿರ CURLcode httpsrr_decode_alpn(ಧ್ರುವ uint8_t *cp, size_t len,
                                    unsigned ವರ್ಣ *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  ಪೂರ್ಣಾಙ್ಕ idnum = 0;

  ಯಾವತ್(len > 0) {
    size_t tlen = *cp++;
    ಗಣನಾ alpnid id;
    len--;
    ಯದಿ(tlen > len)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    ಯದಿ(id != ALPN_none) {
      ಯದಿ(idnum == MAX_HTTPSRR_ALPNS)
        ಭಙ್ಗ;
      ಯದಿ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      ಅನ್ಯಥಾ
        alpns[idnum++] = (unsigned ವರ್ಣ)id;
    }
    cp += tlen;
    len -= tlen;
  }
  ಯದಿ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURLVERBOSE

ಸ್ಥಿರ CURLcode httpsrr_print_addr(ಸಂರಚನಾ dynbuf *dyn,
                                   ಪೂರ್ಣಾಙ್ಕ ai_family,
                                   ಧ್ರುವ uint8_t *addr,
                                   size_t total_len)
{
  ವರ್ಣ buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ಧ್ರುವ ವರ್ಣ *sep = "";
  CURLcode result = CURLE_OK;

  ಪ್ರತಿ(i = 0; (i < (total_len / alen)) && !result; ++i) {
    ಯದಿ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, ಪರಿಮಾಣ(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    ಅನ್ಯಥಾ
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_httpsrr_trace(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ Curl_https_rrinfo *rr)
{
  ಸಂರಚನಾ dynbuf tmp;
  CURLcode result;

  ಯದಿ(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    ಪ್ರತ್ಯಾಗಮ;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  ಯದಿ(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  ಅನ್ಯಥಾ
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(ಸಂರಚನಾ dynbuf *tmp,
                            ಸಂರಚನಾ Curl_https_rrinfo *rr)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  ಯದಿ(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  ಯದಿ(!result && rr->alpns[0]) {
    ಧ್ರುವ ವರ್ಣ *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    ಪ್ರತಿ(i = 0; !result && (i < 4); ++i) {
      ವಿಕಲ್ಪ(rr->alpns[i]) {
      ಪಕ್ಷ ALPN_h1:
        name = "http/1.1";
        ಭಙ್ಗ;
      ಪಕ್ಷ ALPN_h2:
        name = "h2";
        ಭಙ್ಗ;
      ಪಕ್ಷ ALPN_h3:
        name = "h3";
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        name = ರಿಕ್ತ;
      }
      ಯದಿ(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  ಯದಿ(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  ಯದಿ(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  ಯದಿ(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    ಯದಿ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  ಯದಿ(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    ಯದಿ(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  ಯದಿ(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(ಸಂರಚನಾ Curl_https_rrinfo *rr,
                          uint16_t rrkey, ಧ್ರುವ uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  ವಿಕಲ್ಪ(rrkey) {
  ಪಕ್ಷ HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_NO_DEF_ALPN:
    ಯದಿ(vlen) /* no data */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_IPV4: /* addr4 list */
    ಯದಿ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    ಯದಿ(!rr->ipv4hints)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_ECH:
    ಯದಿ(!vlen)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    ಯದಿ(!rr->echconfiglist)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_IPV6: /* addr6 list */
    ಯದಿ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    ಯದಿ(!rr->ipv6hints)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPS_RR_CODE_PORT:
    ಯದಿ(vlen != 2)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* unknown code */
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_httpsrr_destroy(ಸಂರಚನಾ Curl_https_rrinfo *rrinfo)
{
  ಯದಿ(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *rr)
{
  ಯದಿ(!data->conn)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(ಸಂರಚನಾ Curl_peer *peer,
                              ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *rr)
{
  ಯದಿ(!peer || !rr)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

ಸ್ಥಿರ CURLcode httpsrr_opt(ಧ್ರುವ ares_dns_rr_t *rr,
                            ares_dns_rr_ಕೀಲಿ_t ಕೀಲಿ, size_t idx,
                            ಸಂರಚನಾ Curl_https_rrinfo *hinfo)
{
  ಧ್ರುವ unsigned ವರ್ಣ *val = ರಿಕ್ತ;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, ಕೀಲಿ, idx, &val, &len);
  ಪ್ರತ್ಯಾಗಮ Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(ಧ್ರುವ ares_dns_record_t *dnsrec,
                                ಸಂರಚನಾ Curl_https_rrinfo **phinfo)
{
  ಸಂರಚನಾ Curl_https_rrinfo *hinfo = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, ಪರಿಮಾಣ(*hinfo));
  ಯದಿ(!hinfo)
    ಗಚ್ಛ out;

  ಪ್ರತಿ(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ಧ್ರುವ ವರ್ಣ *target;
    size_t opt;
    ಧ್ರುವ ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    ಯದಿ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ಅನುವರ್ತ;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    ಯದಿ(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      ಯದಿ(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    ಪ್ರತಿ(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }
  result = CURLE_OK;

out:
  ಯದಿ(result) {
    *phinfo = ರಿಕ್ತ;
    Curl_httpsrr_destroy(hinfo);
  }
  ಅನ್ಯಥಾ {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
