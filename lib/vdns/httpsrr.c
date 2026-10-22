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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_HTTPSRR

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/inet_ntop.h"

స్థిర CURLcode httpsrr_decode_alpn(స్థిరరాశి uint8_t *cp, size_t len,
                                    గుర్తులేని అక్షరం *alpns)
{
  /*
   * The wire-format value for "alpn" consists of at least one alpn-id
   * prefixed by its length as a single octet, and these length-value pairs
   * are concatenated to form the SvcParamValue. These pairs MUST exactly fill
   * the SvcParamValue; otherwise, the SvcParamValue is malformed.
   */
  పూర్ణాంకం idnum = 0;

  అయితే(len > 0) {
    size_t tlen = *cp++;
    గణన alpnid id;
    len--;
    ఒకవేళ(tlen > len)
      తిరిగి CURLE_BAD_CONTENT_ENCODING;

    /* we only store ALPN ids we know about */
    id = Curl_alpn2alpnid(cp, tlen);
    ఒకవేళ(id != ALPN_none) {
      ఒకవేళ(idnum == MAX_HTTPSRR_ALPNS)
        ఆపు;
      ఒకవేళ(idnum && memchr(alpns, id, idnum))
        /* this ALPN id is already stored */
        ;
      లేకపోతే
        alpns[idnum++] = (గుర్తులేని అక్షరం)id;
    }
    cp += tlen;
    len -= tlen;
  }
  ఒకవేళ(idnum < MAX_HTTPSRR_ALPNS)
    alpns[idnum] = ALPN_none; /* terminate the list */
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURLVERBOSE

స్థిర CURLcode httpsrr_print_addr(నిర్మాణం dynbuf *dyn,
                                   పూర్ణాంకం ai_family,
                                   స్థిరరాశి uint8_t *addr,
                                   size_t మొత్తం_len)
{
  అక్షరం buf[MAX_IPADR_LEN];
  size_t i, alen = (ai_family == AF_INET6) ? 16 : 4;
  స్థిరరాశి అక్షరం *sep = "";
  CURLcode ఫలితం = CURLE_OK;

  కొరకు(i = 0; (i < (మొత్తం_len / alen)) && !ఫలితం; ++i) {
    ఒకవేళ(curlx_inet_ntop(ai_family, addr + (i * alen), buf, పరిమాణం(buf)))
      ఫలితం = curlx_dyn_కూడు(dyn, "<error parsing address>");
    లేకపోతే
      ఫలితం = curlx_dyn_addf(dyn, "%s%s", sep, buf);
    sep = ",";
  }
  తిరిగి ఫలితం;
}

శూన్య Curl_httpsrr_trace(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_https_rrinfo *rr)
{
  నిర్మాణం dynbuf tmp;
  CURLcode ఫలితం;

  ఒకవేళ(!rr) {
    CURL_TRC_DNS(దత్తాంశం, "[HTTPS] no record available");
    తిరిగి;
  }
  curlx_dyn_init(&tmp, 1024);
  ఫలితం = Curl_httpsrr_print(&tmp, rr);
  ఒకవేళ(!ఫలితం)
    CURL_TRC_DNS(దత్తాంశం, "[HTTPS] record: %s", curlx_dyn_ptr(&tmp));
  లేకపోతే
    CURL_TRC_DNS(దత్తాంశం, "[HTTPS] error printing information");
  curlx_dyn_free(&tmp);
}

CURLcode Curl_httpsrr_print(నిర్మాణం dynbuf *tmp,
                            నిర్మాణం Curl_https_rrinfo *rr)
{
  CURLcode ఫలితం;
  పూర్ణాంకం i;

  curlx_dyn_reset(tmp);
  ఫలితం = curlx_dyn_addf(tmp, "%u %s", rr->ప్రాధాన్యత,
                          rr->లక్ష్యం ? rr->లక్ష్యం : ".");
  ఒకవేళ(!ఫలితం && rr->mandatory)
    ఫలితం = curlx_dyn_కూడు(tmp, " mandatory-keys(ignored)");
  ఒకవేళ(!ఫలితం && rr->alpns[0]) {
    స్థిరరాశి అక్షరం *sep = "", *name;
    ఫలితం = curlx_dyn_కూడు(tmp, " alpn=");
    కొరకు(i = 0; !ఫలితం && (i < 4); ++i) {
      ఎంపిక(rr->alpns[i]) {
      స్థితి ALPN_h1:
        name = "http/1.1";
        ఆపు;
      స్థితి ALPN_h2:
        name = "h2";
        ఆపు;
      స్థితి ALPN_h3:
        name = "h3";
        ఆపు;
      అప్రమేయ:
        name = ఖాళీ;
      }
      ఒకవేళ(name) {
        ఫలితం = curlx_dyn_addf(tmp, "%s%s", sep, name);
        sep = ",";
      }
    }
  }
  ఒకవేళ(!ఫలితం && rr->పోర్టు_సమితి) {
    ఫలితం = curlx_dyn_addf(tmp, " port=%u", rr->పోర్టు);
  }
  ఒకవేళ(!ఫలితం && rr->no_def_alpn)
    ఫలితం = curlx_dyn_కూడు(tmp, " no-default-alpn");
  ఒకవేళ(!ఫలితం && rr->ipv6hints_len) {
    ఫలితం = curlx_dyn_కూడు(tmp, " ipv6hint=");
    ఒకవేళ(!ఫలితం)
      ఫలితం = httpsrr_print_addr(
        tmp, AF_INET6, rr->ipv6hints, rr->ipv6hints_len);
  }
  ఒకవేళ(!ఫలితం && rr->ipv4hints_len) {
    ఫలితం = curlx_dyn_కూడు(tmp, " ipv4hint=");
    ఒకవేళ(!ఫలితం)
      ఫలితం = httpsrr_print_addr(
        tmp, AF_INET, rr->ipv4hints, rr->ipv4hints_len);
  }
  ఒకవేళ(!ఫలితం && rr->echconfiglist_len)
    ఫలితం = curlx_dyn_addf(tmp, " ech=<%zu bytes>", rr->echconfiglist_len);

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* CURLVERBOSE */

CURLcode Curl_httpsrr_సమితి(నిర్మాణం Curl_https_rrinfo *rr,
                          uint16_t rrkey, స్థిరరాశి uint8_t *val, size_t vlen)
{
  CURLcode ఫలితం = CURLE_OK;
  ఎంపిక(rrkey) {
  స్థితి HTTPS_RR_సంకేతం_MANDATORY:
    rr->mandatory = TRUE;
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_ALPN: /* str_list */
    ఫలితం = httpsrr_decode_alpn(val, vlen, rr->alpns);
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_NO_DEF_ALPN:
    ఒకవేళ(vlen) /* no data */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    rr->no_def_alpn = TRUE;
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_IPV4: /* addr4 list */
    ఒకవేళ(!vlen || (vlen & 3)) /* the size must be 4-byte aligned */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    curlx_free(rr->ipv4hints);
    rr->ipv4hints = curlx_memdup(val, vlen);
    ఒకవేళ(!rr->ipv4hints)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    rr->ipv4hints_len = vlen;
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_ECH:
    ఒకవేళ(!vlen)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    curlx_free(rr->echconfiglist);
    rr->echconfiglist = curlx_memdup(val, vlen);
    ఒకవేళ(!rr->echconfiglist)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    rr->echconfiglist_len = vlen;
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_IPV6: /* addr6 list */
    ఒకవేళ(!vlen || (vlen & 15)) /* the size must be 16-byte aligned */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    curlx_free(rr->ipv6hints);
    rr->ipv6hints = curlx_memdup(val, vlen);
    ఒకవేళ(!rr->ipv6hints)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    rr->ipv6hints_len = vlen;
    ఆపు;
  స్థితి HTTPS_RR_సంకేతం_పోర్టు:
    ఒకవేళ(vlen != 2)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    rr->పోర్టు = (uint16_t)((val[0] << 8) | val[1]);
    rr->పోర్టు_సమితి = TRUE;
    ఆపు;
  అప్రమేయ:
    /* unknown code */
    ఆపు;
  }
  తిరిగి ఫలితం;
}

శూన్య Curl_httpsrr_destroy(నిర్మాణం Curl_https_rrinfo *rrinfo)
{
  ఒకవేళ(rrinfo) {
    curlx_free(rrinfo->లక్ష్యం);
    curlx_free(rrinfo->echconfiglist);
    curlx_free(rrinfo->ipv4hints);
    curlx_free(rrinfo->ipv6hints);
    curlx_free(rrinfo);
  }
}

bool Curl_httpsrr_applicable(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rr)
{
  ఒకవేళ(!దత్తాంశం->conn)
    తిరిగి FALSE;
  తిరిగి Curl_httpsrr_is_for_peer(దత్తాంశం->conn->origin, rr);
}

bool Curl_httpsrr_is_for_peer(నిర్మాణం Curl_peer *peer,
                              స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rr)
{
  ఒకవేళ(!peer || !rr)
    తిరిగి FALSE;
  తిరిగి (!rr->లక్ష్యం || !rr->లక్ష్యం[0] ||
          (rr->లక్ష్యం[0] == '.' && !rr->లక్ష్యం[1])) &&
         (!rr->పోర్టు_సమితి || rr->పోర్టు == peer->పోర్టు);
}

#నిర్వచితమైతే USE_ARES

స్థిర CURLcode httpsrr_opt(స్థిరరాశి ares_dns_rr_t *rr,
                            ares_dns_rr_కీలకం_t కీలకం, size_t idx,
                            నిర్మాణం Curl_https_rrinfo *hinfo)
{
  స్థిరరాశి గుర్తులేని అక్షరం *val = ఖాళీ;
  గుర్తులేని పొట్టి సంకేతం_2;
  size_t len = 0;

  సంకేతం_2 = ares_dns_rr_get_opt(rr, కీలకం, idx, &val, &len);
  తిరిగి Curl_httpsrr_సమితి(hinfo, సంకేతం_2, val, len);
}

CURLcode Curl_httpsrr_from_ares(స్థిరరాశి ares_dns_రికార్డు_t *dnsrec,
                                నిర్మాణం Curl_https_rrinfo **phinfo)
{
  నిర్మాణం Curl_https_rrinfo *hinfo = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  size_t i;

  hinfo = curlx_calloc(1, పరిమాణం(*hinfo));
  ఒకవేళ(!hinfo)
    వెళ్ళు బహిర్గతం;

  కొరకు(i = 0; i < ares_dns_రికార్డు_rr_cnt(dnsrec, ARES_SECTION_ANSWER); i++) {
    స్థిరరాశి అక్షరం *లక్ష్యం;
    size_t opt;
    స్థిరరాశి ares_dns_rr_t *rr =
      ares_dns_రికార్డు_rr_get_const(dnsrec, ARES_SECTION_ANSWER, i);
    ఒకవేళ(ares_dns_rr_get_type(rr) != ARES_REC_TYPE_HTTPS)
      కొనసాగు;
    /* When SvcPriority is 0, the SVCB record is in AliasMode. Otherwise, it
       is in ServiceMode */
    లక్ష్యం = ares_dns_rr_get_str(rr, ARES_RR_HTTPS_లక్ష్యం);
    ఒకవేళ(లక్ష్యం && లక్ష్యం[0]) {
      curlx_free(hinfo->లక్ష్యం);
      hinfo->లక్ష్యం = curlx_strdup(లక్ష్యం);
      ఒకవేళ(!hinfo->లక్ష్యం) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
    hinfo->ప్రాధాన్యత = ares_dns_rr_get_u16(rr, ARES_RR_HTTPS_ప్రాధాన్యత);
    కొరకు(opt = 0; opt < ares_dns_rr_get_opt_cnt(rr, ARES_RR_HTTPS_PARAMS);
        opt++) {
      ఫలితం = httpsrr_opt(rr, ARES_RR_HTTPS_PARAMS, opt, hinfo);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    *phinfo = ఖాళీ;
    Curl_httpsrr_destroy(hinfo);
  }
  లేకపోతే {
    DEBUGASSERT(hinfo);
    *phinfo = hinfo;
  }
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_ARES */

#అయితే_ముగింపు /* USE_HTTPSRR */
