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
#समावेश "curl_setup.h"

#अगर_घोषित USE_HTTPSRR

#समावेश "urldata.h"
#समावेश "connect.h"
#समावेश "curl_trc.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/inet_ntop.h"

अटल CURLcode httpsrr_decode_alpn(स्थिर uint8_t *cp, माप_प्रकार len,
                                    अचिन्हित अक्षर *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  पूर्णांक idnum = 0;

  जबतक(len > 0) {
    माप_प्रकार tlen = *cp++;
    क्रमागत alpnid id;
    len--;
    अगर(tlen > len)
      वापस CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    अगर(id != ALPN_none) {
      अगर(idnum == MAX_HTTPSRR_ALPNS)
        अवरोध;
      अगर(idnum && स_प्रथम(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      अन्यथा
        alpns[idnum++] = (अचिन्हित अक्षर)id;
    }
    cp += tlen;
    len -= tlen;
  }
  अगर(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  वापस CURLE_OK;
}

#अगर_घोषित CURLVERBOSE

अटल CURLcode httpsrr_print_addr(काष्ठा dynbuf *dyn,
                                   पूर्णांक ai_family,
                                   स्थिर uint8_t *addr,
                                   माप_प्रकार योग_len)
{
  अक्षर buf[MAX_IPADR_LEN];
  माप_प्रकार i, alen = (ai_family == AF_INET6) ? 16 : 4;
  स्थिर अक्षर *sep = "";
  CURLcode परिणाम = CURLE_OK;

  क्रम(i = 0; (i < (योग_len / alen)) && !परिणाम; ++i) {
    अगर(curlx_inet_ntop(ai_family, addr + (i * alen), buf, माप(buf)))
      परिणाम = curlx_dyn_add(dyn, "<error parsing address>");
    अन्यथा
      परिणाम = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  वापस परिणाम;
}

व्योम Curl_httpsrr_trace(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_https_rrinfo *rr)
{
  काष्ठा dynbuf tmp;
  CURLcode परिणाम;

  अगर(!rr) {
    CURL_TRC_DNS(तथ्य, "[HTTPS] no record available");
    वापस;
  }
  curlx_dyn_init(&tmp, 1024);
  परिणाम = Curl_httpsrr_print(&tmp, rr);
  अगर(!परिणाम)
    CURL_TRC_DNS(तथ्य, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  अन्यथा
    CURL_TRC_DNS(तथ्य, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(काष्ठा dynbuf *tmp,
                            काष्ठा Curl_https_rrinfo *rr)
{
  CURLcode परिणाम;
  पूर्णांक i;

  curlx_dyn_reset(tmp);
  परिणाम = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  अगर(!परिणाम && rr->mandatory)
    परिणाम = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  अगर(!परिणाम && rr->alpns[0]) {
    स्थिर अक्षर *sep = "", *name;
    परिणाम = curlx_dyn_add(tmp, " alpn=");
    क्रम(i = 0; !परिणाम && (i < 4); ++i) {
      चयन(rr->alpns[i]) {
      हाल ALPN_h1:
        name = "http/1.1";
        अवरोध;
      हाल ALPN_h2:
        name = "h2";
        अवरोध;
      हाल ALPN_h3:
        name = "h3";
        अवरोध;
      शेष:
        name = शून्य;
      }
      अगर(name) {
        परिणाम = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  अगर(!परिणाम && rr->port_set) {
    परिणाम = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  अगर(!परिणाम && rr->no_def_alpn)
    परिणाम = curlx_dyn_add(tmp, " no-default-alpn");
  अगर(!परिणाम && rr->ipv6hints_len) {
    परिणाम = curlx_dyn_add(tmp, " ipv6hint=");
    अगर(!परिणाम)
      परिणाम = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  अगर(!परिणाम && rr->ipv4hints_len) {
    परिणाम = curlx_dyn_add(tmp, " ipv4hint=");
    अगर(!परिणाम)
      परिणाम = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  अगर(!परिणाम && rr->echconfiglist_len)
    परिणाम = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  वापस परिणाम;
}

#पूर्ण_अगर /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(काष्ठा Curl_https_rrinfo *rr,
                          uint16_t rrkey, स्थिर uint8_t *val, माप_प्रकार vlen)
{
  CURLcode परिणाम = CURLE_OK;
  चयन(rrkey) {
  हाल HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    अवरोध;
  हाल HTTPS_RR_CODE_ALPN: /* str_list */
    परिणाम = httpsrr_decode_alpn(val, vlen, rr->alpns);
    अवरोध;
  हाल HTTPS_RR_CODE_NO_DEF_ALPN:
    अगर(vlen) /* no data */
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    अवरोध;
  हाल HTTPS_RR_CODE_IPV4: /* addr4 list */
    अगर(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    अगर(!rr->ipv4hints)
      वापस CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    अवरोध;
  हाल HTTPS_RR_CODE_ECH:
    अगर(!vlen)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    अगर(!rr->echconfiglist)
      वापस CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    अवरोध;
  हाल HTTPS_RR_CODE_IPV6: /* addr6 list */
    अगर(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    अगर(!rr->ipv6hints)
      वापस CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    अवरोध;
  हाल HTTPS_RR_CODE_PORT:
    अगर(vlen != 2)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    अवरोध;
  शेष:
    /* unknown code */
    अवरोध;
  }
  वापस परिणाम;
}

व्योम Curl_httpsrr_destroy(काष्ठा Curl_https_rrinfo *rrinfo)
{
  अगर(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_https_rrinfo *rr)
{
  अगर(!तथ्य->conn)
    वापस FALSE;
  वापस Curl_httpsrr_is_for_peer(तथ्य->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(काष्ठा Curl_peer *peer,
                              स्थिर काष्ठा Curl_https_rrinfo *rr)
{
  अगर(!peer || !rr)
    वापस FALSE;
  वापस (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#अगर_घोषित USE_ARES

अटल CURLcode httpsrr_opt(स्थिर ares_dns_rr_t *rr,
                            ares_dns_rr_कुंजी_t कुंजी, माप_प्रकार idx,
                            काष्ठा Curl_https_rrinfo *hinfo)
{
  स्थिर अचिन्हित अक्षर *val = शून्य;
  अचिन्हित लघु code;
  माप_प्रकार len = 0;

  code = ares_dns_rr_get_opt(rr, कुंजी, idx, &val, &len);
  वापस Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(स्थिर ares_dns_record_t *dnsrec,
                                काष्ठा Curl_https_rrinfo **phinfo)
{
  काष्ठा Curl_https_rrinfo *hinfo = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  माप_प्रकार i;

  hinfo = curlx_calloc(1, माप(*hinfo));
  अगर(!hinfo)
    जाओ out;

  क्रम(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    स्थिर अक्षर *target;
    माप_प्रकार opt;
    स्थिर ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    अगर(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      जारी;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    अगर(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      अगर(!hinfo->target) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    क्रम(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      परिणाम = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      अगर(परिणाम)
        जाओ out;
    }
  }
  परिणाम = CURLE_OK;

out:
  अगर(परिणाम) {
    *phinfo = शून्य;
    Curl_httpsrr_destroy(hinfo);
  }
  अन्यथा {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_ARES */

#पूर्ण_अगर /* USE_HTTPSRR */
