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

#ఒకవేళ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vtls/vtls_spack.h"
#అన్తర్భావ "curlx/strdup.h"

#నిర్వచించు CURL_SPACK_సంస్కరణ       0x01
#నిర్వచించు CURL_SPACK_IETF_ID       0x02
#నిర్వచించు CURL_SPACK_VALID_UNTIL   0x03
#నిర్వచించు CURL_SPACK_TICKET        0x04
#నిర్వచించు CURL_SPACK_ALPN          0x05
#నిర్వచించు CURL_SPACK_EARLYDATA     0x06
#నిర్వచించు CURL_SPACK_QUICTP        0x07
#నిర్వచించు CURL_SPACK_SECTRUST      0x08

స్థిర CURLcode spack_enc8(నిర్మాణం dynbuf *buf, uint8_t b)
{
  తిరిగి curlx_dyn_addn(buf, &b, 1);
}

స్థిర CURLcode spack_dec8(uint8_t *val, స్థిరరాశి uint8_t **src,
                           స్థిరరాశి uint8_t *చివర)
{
  ఒకవేళ(చివర - *src < 1)
    తిరిగి CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode spack_enc16(నిర్మాణం dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  తిరిగి curlx_dyn_addn(buf, nval, పరిమాణం(nval));
}

స్థిర CURLcode spack_dec16(uint16_t *val, స్థిరరాశి uint8_t **src,
                            స్థిరరాశి uint8_t *చివర)
{
  ఒకవేళ(చివర - *src < 2)
    తిరిగి CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode spack_enc32(నిర్మాణం dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  తిరిగి curlx_dyn_addn(buf, nval, పరిమాణం(nval));
}

స్థిర CURLcode spack_dec32(uint32_t *val, స్థిరరాశి uint8_t **src,
                            స్థిరరాశి uint8_t *చివర)
{
  ఒకవేళ(చివర - *src < 4)
    తిరిగి CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode spack_enc64(నిర్మాణం dynbuf *buf, uint64_t val)
{
  uint8_t nval[8];
  nval[0] = (uint8_t)(val >> 56);
  nval[1] = (uint8_t)(val >> 48);
  nval[2] = (uint8_t)(val >> 40);
  nval[3] = (uint8_t)(val >> 32);
  nval[4] = (uint8_t)(val >> 24);
  nval[5] = (uint8_t)(val >> 16);
  nval[6] = (uint8_t)(val >> 8);
  nval[7] = (uint8_t)val;
  తిరిగి curlx_dyn_addn(buf, nval, పరిమాణం(nval));
}

స్థిర CURLcode spack_dec64(uint64_t *val, స్థిరరాశి uint8_t **src,
                            స్థిరరాశి uint8_t *చివర)
{
  ఒకవేళ(చివర - *src < 8)
    తిరిగి CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode spack_encstr16(నిర్మాణం dynbuf *buf, స్థిరరాశి అక్షరం *s)
{
  size_t slen = అక్షరశ్రేణి_పొడవు(s);
  CURLcode ఫలితం;
  ఒకవేళ(slen > UINT16_MAX)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఫలితం = spack_enc16(buf, (uint16_t)slen);
  ఒకవేళ(!ఫలితం) {
    ఫలితం = curlx_dyn_addn(buf, s, slen);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode spack_decstr16(అక్షరం **val, స్థిరరాశి uint8_t **src,
                               స్థిరరాశి uint8_t *చివర)
{
  uint16_t slen;
  CURLcode ఫలితం;

  *val = ఖాళీ;
  ఫలితం = spack_dec16(&slen, src, చివర);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(చివర - *src < slen)
    తిరిగి CURLE_READ_ERROR;
  *val = curlx_memdup0((స్థిరరాశి అక్షరం *)(*src), slen);
  *src += slen;
  తిరిగి *val ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

స్థిర CURLcode spack_encdata16(నిర్మాణం dynbuf *buf, స్థిరరాశి uint8_t *దత్తాంశం,
                                size_t దత్తాంశం_len)
{
  CURLcode ఫలితం;
  ఒకవేళ(దత్తాంశం_len > UINT16_MAX)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఫలితం = spack_enc16(buf, (uint16_t)దత్తాంశం_len);
  ఒకవేళ(!ఫలితం) {
    ఫలితం = curlx_dyn_addn(buf, దత్తాంశం, దత్తాంశం_len);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                స్థిరరాశి uint8_t **src, స్థిరరాశి uint8_t *చివర)
{
  uint16_t దత్తాంశం_len;
  CURLcode ఫలితం;

  *val = ఖాళీ;
  ఫలితం = spack_dec16(&దత్తాంశం_len, src, చివర);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(చివర - *src < దత్తాంశం_len)
    తిరిగి CURLE_READ_ERROR;
  *val = curlx_memdup0((స్థిరరాశి అక్షరం *)(*src), దత్తాంశం_len);
  *val_len = దత్తాంశం_len;
  *src += దత్తాంశం_len;
  తిరిగి *val ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

CURLcode Curl_ssl_session_pack(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_ssl_session *s,
                               నిర్మాణం dynbuf *buf)
{
  CURLcode ఫలితం;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  ఒకవేళ(s->valid_until < 0)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఫలితం = spack_enc8(buf, CURL_SPACK_సంస్కరణ);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_enc8(buf, CURL_SPACK_TICKET);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_encdata16(buf, s->sdata, s->sdata_len);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_enc8(buf, CURL_SPACK_IETF_ID);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  ఒకవేళ(!ఫలితం)
    ఫలితం = spack_enc64(buf, (uint64_t)s->valid_until);
  ఒకవేళ(!ఫలితం && s->alpn) {
    ఫలితం = spack_enc8(buf, CURL_SPACK_ALPN);
    ఒకవేళ(!ఫలితం)
      ఫలితం = spack_encstr16(buf, s->alpn);
  }
  ఒకవేళ(!ఫలితం && s->earlydata_max) {
    ఒకవేళ(s->earlydata_max > UINT32_MAX)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(!ఫలితం)
      ఫలితం = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    ఒకవేళ(!ఫలితం)
      ఫలితం = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  ఒకవేళ(!ఫలితం && s->sectrust_verified) {
    ఫలితం = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  ఒకవేళ(!ఫలితం && s->quic_tp && s->quic_tp_len) {
    ఫలితం = spack_enc8(buf, CURL_SPACK_QUICTP);
    ఒకవేళ(!ఫలితం)
      ఫలితం = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  ఒకవేళ(ఫలితం)
    CURL_TRC_SSLS(దత్తాంశం, "error packing data: %d", (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_session_unpack(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి శూన్య *bufv, size_t buflen,
                                 నిర్మాణం Curl_ssl_session **ps)
{
  నిర్మాణం Curl_ssl_session *s = ఖాళీ;
  స్థిరరాశి గుర్తులేని అక్షరం *buf = (స్థిరరాశి గుర్తులేని అక్షరం *)bufv;
  స్థిరరాశి గుర్తులేని అక్షరం *చివర = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode ఫలితం;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ఖాళీ;

  ఫలితం = spack_dec8(&val8, &buf, చివర);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(val8 != CURL_SPACK_సంస్కరణ) {
    ఫలితం = CURLE_READ_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  s = curlx_calloc(1, పరిమాణం(*s));
  ఒకవేళ(!s) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  అయితే(buf < చివర) {
    ఫలితం = spack_dec8(&val8, &buf, చివర);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఎంపిక(val8) {
    స్థితి CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      ఫలితం = spack_decstr16(&s->alpn, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఆపు;
    స్థితి CURL_SPACK_EARLYDATA:
      ఫలితం = spack_dec32(&val32, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      s->earlydata_max = val32;
      ఆపు;
    స్థితి CURL_SPACK_IETF_ID:
      ఫలితం = spack_dec16(&val16, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      s->ietf_tls_id = val16;
      ఆపు;
    స్థితి CURL_SPACK_QUICTP: {
      ఫలితం = spack_decdata16(&pval8, &dlen, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      ఆపు;
    }
    స్థితి CURL_SPACK_TICKET: {
      ఫలితం = spack_decdata16(&pval8, &dlen, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      ఆపు;
    }
    స్థితి CURL_SPACK_VALID_UNTIL:
      ఫలితం = spack_dec64(&val64, &buf, చివర);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      s->valid_until = (curl_off_t)val64;
      ఆపు;
    స్థితి CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      ఆపు;
    అప్రమేయ:  /* unknown tag */
      ఫలితం = CURLE_READ_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    CURL_TRC_SSLS(దత్తాంశం, "error unpacking data: %d", (పూర్ణాంకం)ఫలితం);
    Curl_ssl_session_destroy(s);
  }
  లేకపోతే
    *ps = s;
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_SSL && USE_SSLS_EXPORT */
