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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_HTTPSRR

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"

સ્થિર CURLcode httpsrr_decode_alpn(અચલ uint8_t *cp, size_t len,
                                    unsigned અક્ષર *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  પૂર્ણાંક idnum = 0;

  જ્યારે(len > 0) {
    size_t tlen = *cp++;
    ગણના alpnid id;
    len--;
    જો(tlen > len)
      પાછા CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    જો(id != ALPN_none) {
      જો(idnum == MAX_HTTPSRR_ALPNS)
        તોડો;
      જો(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      નહીંતર
        alpns[idnum++] = (unsigned અક્ષર)id;
    }
    cp += tlen;
    len -= tlen;
  }
  જો(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  પાછા CURLE_OK;
}

#ifdef CURLVERBOSE

સ્થિર CURLcode httpsrr_print_addr(સંરચના dynbuf *dyn,
                                   પૂર્ણાંક ai_family,
                                   અચલ uint8_t *addr,
                                   size_t total_len)
{
  અક્ષર buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  અચલ અક્ષર *sep = "";
  CURLcode result = CURLE_OK;

  માટે(i = 0; (i < (total_len / alen)) && !result; ++i) {
    જો(curlx_inet_ntop(ai_family, addr + (i * alen), buf, કદ(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    નહીંતર
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  પાછા result;
}

શૂન્ય Curl_httpsrr_trace(સંરચના Curl_easy *data,
                        સંરચના Curl_https_rrinfo *rr)
{
  સંરચના dynbuf tmp;
  CURLcode result;

  જો(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    પાછા;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  જો(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  નહીંતર
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(સંરચના dynbuf *tmp,
                            સંરચના Curl_https_rrinfo *rr)
{
  CURLcode result;
  પૂર્ણાંક i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  જો(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  જો(!result && rr->alpns[0]) {
    અચલ અક્ષર *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    માટે(i = 0; !result && (i < 4); ++i) {
      પસંદગી(rr->alpns[i]) {
      સ્થિતિ ALPN_h1:
        name = "http/1.1";
        તોડો;
      સ્થિતિ ALPN_h2:
        name = "h2";
        તોડો;
      સ્થિતિ ALPN_h3:
        name = "h3";
        તોડો;
      મૂળભૂત:
        name = ખાલી;
      }
      જો(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  જો(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  જો(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  જો(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    જો(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  જો(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    જો(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  જો(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  પાછા result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(સંરચના Curl_https_rrinfo *rr,
                          uint16_t rrkey, અચલ uint8_t *val, size_t vlen)
{
  CURLcode result = CURLE_OK;
  પસંદગી(rrkey) {
  સ્થિતિ HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_NO_DEF_ALPN:
    જો(vlen) /* no data */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_IPV4: /* addr4 list */
    જો(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    જો(!rr->ipv4hints)
      પાછા CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_ECH:
    જો(!vlen)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    જો(!rr->echconfiglist)
      પાછા CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_IPV6: /* addr6 list */
    જો(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    જો(!rr->ipv6hints)
      પાછા CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    તોડો;
  સ્થિતિ HTTPS_RR_CODE_PORT:
    જો(vlen != 2)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    તોડો;
  મૂળભૂત:
    /* unknown code */
    તોડો;
  }
  પાછા result;
}

શૂન્ય Curl_httpsrr_destroy(સંરચના Curl_https_rrinfo *rrinfo)
{
  જો(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(સંરચના Curl_easy *data,
                             અચલ સંરચના Curl_https_rrinfo *rr)
{
  જો(!data->conn)
    પાછા FALSE;
  પાછા Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(સંરચના Curl_peer *peer,
                              અચલ સંરચના Curl_https_rrinfo *rr)
{
  જો(!peer || !rr)
    પાછા FALSE;
  પાછા (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

સ્થિર CURLcode httpsrr_opt(અચલ ares_dns_rr_t *rr,
                            ares_dns_rr_ચાવી_t ચાવી, size_t idx,
                            સંરચના Curl_https_rrinfo *hinfo)
{
  અચલ unsigned અક્ષર *val = ખાલી;
  unsigned short code;
  size_t len = 0;

  code = ares_dns_rr_get_opt(rr, ચાવી, idx, &val, &len);
  પાછા Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(અચલ ares_dns_record_t *dnsrec,
                                સંરચના Curl_https_rrinfo **phinfo)
{
  સંરચના Curl_https_rrinfo *hinfo = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  size_t i;

  hinfo = curlx_calloc(1, કદ(*hinfo));
  જો(!hinfo)
    જાઓ out;

  માટે(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    અચલ અક્ષર *target;
    size_t opt;
    અચલ ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    જો(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      ચાલુ;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    જો(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      જો(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    માટે(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      જો(result)
        જાઓ out;
    }
  }
  result = CURLE_OK;

out:
  જો(result) {
    *phinfo = ખાલી;
    Curl_httpsrr_destroy(hinfo);
  }
  નહીંતર {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  પાછા result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
