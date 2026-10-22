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

स्थिर CURLcode httpsrr_decode_alpn(स्थिरांक uint8_t *cp, माप_प्रकार len,
                                    अचिन्हित अक्षर *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  पूर्णांक idnum = 0;

  पर्यंत(len > 0) {
    माप_प्रकार tlen = *cp++;
    गणना alpnid id;
    len--;
    जर(tlen > len)
      परत CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    जर(id != ALPN_none) {
      जर(idnum == MAX_HTTPSRR_ALPNS)
        थांबा;
      जर(idnum && स_प्रथम(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      अन्यथा
        alpns[idnum++] = (अचिन्हित अक्षर)id;
    }
    cp += tlen;
    len -= tlen;
  }
  जर(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  परत CURLE_OK;
}

#ifdef CURLVERBOSE

स्थिर CURLcode httpsrr_print_addr(संरचना dynbuf *dyn,
                                   पूर्णांक ai_family,
                                   स्थिरांक uint8_t *addr,
                                   माप_प्रकार total_len)
{
  अक्षर buf[MAX_IPADR_LEN];
  माप_प्रकार i, alen = (ai_family == AF_INET6) ? 16 : 4;
  स्थिरांक अक्षर *sep = "";
  CURLcode result = CURLE_OK;

  साठी(i = 0; (i < (total_len / alen)) && !result; ++i) {
    जर(curlx_inet_ntop(ai_family, addr + (i * alen), buf, आकार(buf)))
      result = curlx_dyn_add(dyn, "<error parsing address>");
    अन्यथा
      result = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  परत result;
}

शून्य Curl_httpsrr_trace(संरचना Curl_easy *data,
                        संरचना Curl_https_rrinfo *rr)
{
  संरचना dynbuf tmp;
  CURLcode result;

  जर(!rr) {
    CURL_TRC_DNS(data, "[HTTPS] no record available");
    परत;
  }
  curlx_dyn_init(&tmp, 1024);
  result = Curl_httpsrr_print(&tmp, rr);
  जर(!result)
    CURL_TRC_DNS(data, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  अन्यथा
    CURL_TRC_DNS(data, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(संरचना dynbuf *tmp,
                            संरचना Curl_https_rrinfo *rr)
{
  CURLcode result;
  पूर्णांक i;

  curlx_dyn_reset(tmp);
  result = curlx_dyn_addf(tmp, "%u %s", rr->priority,
                          rr->target ? rr->target : ".");
  जर(!result && rr->mandatory)
    result = curlx_dyn_add(tmp, " mandatory-keys(ignored)");
  जर(!result && rr->alpns[0]) {
    स्थिरांक अक्षर *sep = "", *name;
    result = curlx_dyn_add(tmp, " alpn=");
    साठी(i = 0; !result && (i < 4); ++i) {
      निवड(rr->alpns[i]) {
      स्थिती ALPN_h1:
        name = "http/1.1";
        थांबा;
      स्थिती ALPN_h2:
        name = "h2";
        थांबा;
      स्थिती ALPN_h3:
        name = "h3";
        थांबा;
      पूर्वनिर्धारित:
        name = रिक्त;
      }
      जर(name) {
        result = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  जर(!result && rr->port_set) {
    result = curlx_dyn_addf(tmp, " port=%u", rr->port);
  }
  जर(!result && rr->no_def_alpn)
    result = curlx_dyn_add(tmp, " no-default-alpn");
  जर(!result && rr->ipv6hints_len) {
    result = curlx_dyn_add(tmp, " ipv6hint=");
    जर(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  जर(!result && rr->ipv4hints_len) {
    result = curlx_dyn_add(tmp, " ipv4hint=");
    जर(!result)
      result = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  जर(!result && rr->echconfiglist_len)
    result = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  परत result;
}

#endif /* CURLVERBOSE */

CURLcode Curl_httpsrr_set(संरचना Curl_https_rrinfo *rr,
                          uint16_t rrkey, स्थिरांक uint8_t *val, माप_प्रकार vlen)
{
  CURLcode result = CURLE_OK;
  निवड(rrkey) {
  स्थिती HTTPS_RR_CODE_MANDATORY:
    rr->mandatory = TRUE;
    थांबा;
  स्थिती HTTPS_RR_CODE_ALPN: /* str_list */
    result = httpsrr_decode_alpn(val, vlen, rr->alpns);
    थांबा;
  स्थिती HTTPS_RR_CODE_NO_DEF_ALPN:
    जर(vlen) /* no data */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    rr->no_def_alpn = TRUE;
    थांबा;
  स्थिती HTTPS_RR_CODE_IPV4: /* addr4 list */
    जर(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    जर(!rr->ipv4hints)
      परत CURLE_OUT_OF_MEMORY;
    rr->ipv4hints_len = vlen;
    थांबा;
  स्थिती HTTPS_RR_CODE_ECH:
    जर(!vlen)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    जर(!rr->echconfiglist)
      परत CURLE_OUT_OF_MEMORY;
    rr->echconfiglist_len = vlen;
    थांबा;
  स्थिती HTTPS_RR_CODE_IPV6: /* addr6 list */
    जर(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    जर(!rr->ipv6hints)
      परत CURLE_OUT_OF_MEMORY;
    rr->ipv6hints_len = vlen;
    थांबा;
  स्थिती HTTPS_RR_CODE_PORT:
    जर(vlen != 2)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    rr->port = (uint16_t)((val[0] << 8) | val[1]);
    rr->port_set = TRUE;
    थांबा;
  पूर्वनिर्धारित:
    /* unknown code */
    थांबा;
  }
  परत result;
}

शून्य Curl_httpsrr_destroy(संरचना Curl_https_rrinfo *rrinfo)
{
  जर(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(संरचना Curl_easy *data,
                             स्थिरांक संरचना Curl_https_rrinfo *rr)
{
  जर(!data->conn)
    परत FALSE;
  परत Curl_httpsrr_is_for_peer(data->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(संरचना Curl_peer *peer,
                              स्थिरांक संरचना Curl_https_rrinfo *rr)
{
  जर(!peer || !rr)
    परत FALSE;
  परत (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->port_set || rr->port == peer->port);
}

#ifdef USE_ARES

स्थिर CURLcode httpsrr_opt(स्थिरांक ares_dns_rr_t *rr,
                            ares_dns_rr_किल्ली_t किल्ली, माप_प्रकार idx,
                            संरचना Curl_https_rrinfo *hinfo)
{
  स्थिरांक अचिन्हित अक्षर *val = रिक्त;
  अचिन्हित लघु code;
  माप_प्रकार len = 0;

  code = ares_dns_rr_get_opt(rr, किल्ली, idx, &val, &len);
  परत Curl_httpsrr_set(hinfo, code, val, len);
}

CURLcode Curl_httpsrr_from_ares(स्थिरांक ares_dns_record_t *dnsrec,
                                संरचना Curl_https_rrinfo **phinfo)
{
  संरचना Curl_https_rrinfo *hinfo = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  माप_प्रकार i;

  hinfo = curlx_calloc(1, आकार(*hinfo));
  जर(!hinfo)
    जा out;

  साठी(i = 0; i < ares_dns_record_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    स्थिरांक अक्षर *target;
    माप_प्रकार opt;
    स्थिरांक ares_dns_rr_t *rr =
      ares_dns_record_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    जर(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      सुरूठेवा;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    जर(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      जर(!hinfo->target) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
    hinfo->priority = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_PRIORITY);
    साठी(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      result = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      जर(result)
        जा out;
    }
  }
  result = CURLE_OK;

out:
  जर(result) {
    *phinfo = रिक्त;
    Curl_httpsrr_destroy(hinfo);
  }
  अन्यथा {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  परत result;
}

#endif /* USE_ARES */

#endif /* USE_HTTPSRR */
