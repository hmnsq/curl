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

#यदि_परिभाषितम् USE_HTTPSRR

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/inet_ntop.h"

स्थिर CURLcode httpsrr_decode_alpn(ध्रुव uint8_t *cp, size_t len,
                                    अचिह्नित वर्ण *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  पूर्णाङ्क idnum = 0;

  यावत्(len > 0) {
    size_t tlen = *cp++;
    गणना alpnid id;
    len--;
    यदि(tlen > len)
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    यदि(id != ALPN_none) {
      यदि(idnum == MAX_HTTPSRR_ALPNS)
        भङ्ग;
      यदि(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      अन्यथा
        alpns[idnum++] = (अचिह्नित वर्ण)id;
    }
    cp += tlen;
    len -= tlen;
  }
  यदि(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURLVERBOSE

स्थिर CURLcode httpsrr_print_addr(संरचना dynbuf *dyn,
                                   पूर्णाङ्क ai_family,
                                   ध्रुव uint8_t *addr,
                                   size_t योग_len)
{
  वर्ण buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  ध्रुव वर्ण *sep = "";
  CURLcode परिणाम = CURLE_OK;

  प्रति(i = 0; (i < (योग_len / alen)) && !परिणाम; ++i) {
    यदि(curlx_inet_ntop(ai_family, addr + (i * alen), buf, परिमाण(buf)))
      परिणाम = curlx_dyn_सङ्कलय(dyn, "<error parsing address>");
    अन्यथा
      परिणाम = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  प्रत्यागम परिणाम;
}

शून्य Curl_httpsrr_trace(संरचना Curl_easy *दत्त,
                        संरचना Curl_https_rrinfo *rr)
{
  संरचना dynbuf tmp;
  CURLcode परिणाम;

  यदि(!rr) {
    CURL_TRC_DNS(दत्त, "[HTTPS] no record available");
    प्रत्यागम;
  }
  curlx_dyn_init(&tmp, 1024);
  परिणाम = Curl_httpsrr_print(&tmp, rr);
  यदि(!परिणाम)
    CURL_TRC_DNS(दत्त, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  अन्यथा
    CURL_TRC_DNS(दत्त, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(संरचना dynbuf *tmp,
                            संरचना Curl_https_rrinfo *rr)
{
  CURLcode परिणाम;
  पूर्णाङ्क i;

  curlx_dyn_reset(tmp);
  परिणाम = curlx_dyn_addf(tmp, "%u %s", rr->प्राथमिकता,
                          rr->target ? rr->target : ".");
  यदि(!परिणाम && rr->mandatory)
    परिणाम = curlx_dyn_सङ्कलय(tmp, " mandatory-keys(ignored)");
  यदि(!परिणाम && rr->alpns[0]) {
    ध्रुव वर्ण *sep = "", *name;
    परिणाम = curlx_dyn_सङ्कलय(tmp, " alpn=");
    प्रति(i = 0; !परिणाम && (i < 4); ++i) {
      विकल्प(rr->alpns[i]) {
      पक्ष ALPN_h1:
        name = "http/1.1";
        भङ्ग;
      पक्ष ALPN_h2:
        name = "h2";
        भङ्ग;
      पक्ष ALPN_h3:
        name = "h3";
        भङ्ग;
      स्वभाव:
        name = रिक्त;
      }
      यदि(name) {
        परिणाम = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  यदि(!परिणाम && rr->पत्तनम्_समुच्चयः) {
    परिणाम = curlx_dyn_addf(tmp, " port=%u", rr->पत्तनम्);
  }
  यदि(!परिणाम && rr->no_def_alpn)
    परिणाम = curlx_dyn_सङ्कलय(tmp, " no-default-alpn");
  यदि(!परिणाम && rr->ipv6hints_len) {
    परिणाम = curlx_dyn_सङ्कलय(tmp, " ipv6hint=");
    यदि(!परिणाम)
      परिणाम = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  यदि(!परिणाम && rr->ipv4hints_len) {
    परिणाम = curlx_dyn_सङ्कलय(tmp, " ipv4hint=");
    यदि(!परिणाम)
      परिणाम = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  यदि(!परिणाम && rr->echconfiglist_len)
    परिणाम = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* CURLVERBOSE */

CURLcode Curl_httpsrr_समुच्चयः(संरचना Curl_https_rrinfo *rr,
                          uint16_t rrkey, ध्रुव uint8_t *val, size_t vlen)
{
  CURLcode परिणाम = CURLE_OK;
  विकल्प(rrkey) {
  पक्ष HTTPS_RR_संहिता_MANDATORY:
    rr->mandatory = TRUE;
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_ALPN: /* str_list */
    परिणाम = httpsrr_decode_alpn(val, vlen, rr->alpns);
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_NO_DEF_ALPN:
    यदि(vlen) /* no data */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    rr->no_def_alpn = TRUE;
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_IPV4: /* addr4 list */
    यदि(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    यदि(!rr->ipv4hints)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    rr->ipv4hints_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_ECH:
    यदि(!vlen)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    यदि(!rr->echconfiglist)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    rr->echconfiglist_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_IPV6: /* addr6 list */
    यदि(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    यदि(!rr->ipv6hints)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    rr->ipv6hints_len = vlen;
    भङ्ग;
  पक्ष HTTPS_RR_संहिता_पत्तनम्:
    यदि(vlen != 2)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    rr->पत्तनम् = (uint16_t)((val[0] << 8) | val[1]);
    rr->पत्तनम्_समुच्चयः = TRUE;
    भङ्ग;
  स्वभाव:
    /* unknown code */
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

शून्य Curl_httpsrr_destroy(संरचना Curl_https_rrinfo *rrinfo)
{
  यदि(rrinfo) {
    curlx_free(rrinfo->target);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(संरचना Curl_easy *दत्त,
                             ध्रुव संरचना Curl_https_rrinfo *rr)
{
  यदि(!दत्त->conn)
    प्रत्यागम FALSE;
  प्रत्यागम Curl_httpsrr_is_for_peer(दत्त->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(संरचना Curl_peer *peer,
                              ध्रुव संरचना Curl_https_rrinfo *rr)
{
  यदि(!peer || !rr)
    प्रत्यागम FALSE;
  प्रत्यागम (!rr->target || !rr->target[0] ||
          (rr->target[0] == '.' && !rr->target[1])) &&
         (!rr->पत्तनम्_समुच्चयः || rr->पत्तनम् == peer->पत्तनम्);
}

#यदि_परिभाषितम् USE_ARES

स्थिर CURLcode httpsrr_opt(ध्रुव ares_dns_rr_t *rr,
                            ares_dns_rr_कुञ्चिका_t कुञ्चिका, size_t idx,
                            संरचना Curl_https_rrinfo *hinfo)
{
  ध्रुव अचिह्नित वर्ण *val = रिक्त;
  अचिह्नित ह्रस्व संहिता;
  size_t len = 0;

  संहिता = ares_dns_rr_get_opt(rr, कुञ्चिका, idx, &val, &len);
  प्रत्यागम Curl_httpsrr_समुच्चयः(hinfo, संहिता, val, len);
}

CURLcode Curl_httpsrr_from_ares(ध्रुव ares_dns_लेखः_t *dnsrec,
                                संरचना Curl_https_rrinfo **phinfo)
{
  संरचना Curl_https_rrinfo *hinfo = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  size_t i;

  hinfo = curlx_calloc(1, परिमाण(*hinfo));
  यदि(!hinfo)
    गच्छ बहिर्गमनम्;

  प्रति(i = 0; i < ares_dns_लेखः_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    ध्रुव वर्ण *target;
    size_t opt;
    ध्रुव ares_dns_rr_t *rr =
      ares_dns_लेखः_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    यदि(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      अनुवर्त;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    target = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_TARGET);
    यदि(target && target[0]) {
      curlx_free(hinfo->target);
      hinfo->target = curlx_strdup(target);
      यदि(!hinfo->target) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
    hinfo->प्राथमिकता = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_प्राथमिकता);
    प्रति(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      परिणाम = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम) {
    *phinfo = रिक्त;
    Curl_httpsrr_destroy(hinfo);
  }
  अन्यथा {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_ARES */

#यदि_अन्तः /* USE_HTTPSRR */
