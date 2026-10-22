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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_spack.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

ସ୍ଥିର CURLcode spack_enc8(ସଂରଚନା dynbuf *buf, uint8_t b)
{
  ଫେରନ୍ତୁ curlx_dyn_addn(buf, &b, 1);
}

ସ୍ଥିର CURLcode spack_dec8(uint8_t *val, ଅଚଳ uint8_t **src,
                           ଅଚଳ uint8_t *end)
{
  ଯଦି(end - *src < 1)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode spack_enc16(ସଂରଚନା dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  ଫେରନ୍ତୁ curlx_dyn_addn(buf, nval, ଆକାର(nval));
}

ସ୍ଥିର CURLcode spack_dec16(uint16_t *val, ଅଚଳ uint8_t **src,
                            ଅଚଳ uint8_t *end)
{
  ଯଦି(end - *src < 2)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode spack_enc32(ସଂରଚନା dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  ଫେରନ୍ତୁ curlx_dyn_addn(buf, nval, ଆକାର(nval));
}

ସ୍ଥିର CURLcode spack_dec32(uint32_t *val, ଅଚଳ uint8_t **src,
                            ଅଚଳ uint8_t *end)
{
  ଯଦି(end - *src < 4)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode spack_enc64(ସଂରଚନା dynbuf *buf, uint64_t val)
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
  ଫେରନ୍ତୁ curlx_dyn_addn(buf, nval, ଆକାର(nval));
}

ସ୍ଥିର CURLcode spack_dec64(uint64_t *val, ଅଚଳ uint8_t **src,
                            ଅଚଳ uint8_t *end)
{
  ଯଦି(end - *src < 8)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode spack_encstr16(ସଂରଚନା dynbuf *buf, ଅଚଳ ଅକ୍ଷର *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  ଯଦି(slen > UINT16_MAX)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  ଯଦି(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode spack_decstr16(ଅକ୍ଷର **val, ଅଚଳ uint8_t **src,
                               ଅଚଳ uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = ଖାଲି;
  result = spack_dec16(&slen, src, end);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(end - *src < slen)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)(*src), slen);
  *src += slen;
  ଫେରନ୍ତୁ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ସ୍ଥିର CURLcode spack_encdata16(ସଂରଚନା dynbuf *buf, ଅଚଳ uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  ଯଦି(data_len > UINT16_MAX)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  ଯଦି(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ଅଚଳ uint8_t **src, ଅଚଳ uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = ଖାଲି;
  result = spack_dec16(&data_len, src, end);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(end - *src < data_len)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  *val = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  ଫେରନ୍ତୁ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_ssl_session *s,
                               ସଂରଚନା dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  ଯଦି(s->valid_until < 0)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  ଯଦି(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  ଯଦି(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  ଯଦି(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  ଯଦି(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  ଯଦି(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  ଯଦି(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  ଯଦି(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    ଯଦି(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  ଯଦି(!result && s->earlydata_max) {
    ଯଦି(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    ଯଦି(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  ଯଦି(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  ଯଦି(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    ଯଦି(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  ଯଦି(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_session_unpack(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଶୂନ୍ୟ *bufv, size_t buflen,
                                 ସଂରଚନା Curl_ssl_session **ps)
{
  ସଂରଚନା Curl_ssl_session *s = ଖାଲି;
  ଅଚଳ unsigned ଅକ୍ଷର *buf = (ଅଚଳ unsigned ଅକ୍ଷର *)bufv;
  ଅଚଳ unsigned ଅକ୍ଷର *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ଖାଲି;

  result = spack_dec8(&val8, &buf, end);
  ଯଦି(result)
    ଯାଅ out;
  ଯଦି(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    ଯାଅ out;
  }

  s = curlx_calloc(1, ଆକାର(*s));
  ଯଦି(!s) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ଯେତେବେଳେ(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    ଯଦି(result)
      ଯାଅ out;

    ବାଛ(val8) {
    ସ୍ଥିତି CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      s->earlydata_max = val32;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      s->ietf_tls_id = val16;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      ଯଦି(result)
        ଯାଅ out;
      s->valid_until = (curl_off_t)val64;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  /* unknown tag */
      result = CURLE_READ_ERROR;
      ଯାଅ out;
    }
  }

out:
  ଯଦି(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    Curl_ssl_session_destroy(s);
  }
  ଅନ୍ୟଥା
    *ps = s;
  ଫେରନ୍ତୁ result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
