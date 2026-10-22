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

#જો defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vtls/vtls_spack.h"
#અન્તર્ભાવ "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

સ્થિર CURLcode spack_enc8(સંરચના dynbuf *buf, uint8_t b)
{
  પાછા curlx_dyn_addn(buf, &b, 1);
}

સ્થિર CURLcode spack_dec8(uint8_t *val, અચલ uint8_t **src,
                           અચલ uint8_t *end)
{
  જો(end - *src < 1)
    પાછા CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  પાછા CURLE_OK;
}

સ્થિર CURLcode spack_enc16(સંરચના dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  પાછા curlx_dyn_addn(buf, nval, કદ(nval));
}

સ્થિર CURLcode spack_dec16(uint16_t *val, અચલ uint8_t **src,
                            અચલ uint8_t *end)
{
  જો(end - *src < 2)
    પાછા CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  પાછા CURLE_OK;
}

સ્થિર CURLcode spack_enc32(સંરચના dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  પાછા curlx_dyn_addn(buf, nval, કદ(nval));
}

સ્થિર CURLcode spack_dec32(uint32_t *val, અચલ uint8_t **src,
                            અચલ uint8_t *end)
{
  જો(end - *src < 4)
    પાછા CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  પાછા CURLE_OK;
}

સ્થિર CURLcode spack_enc64(સંરચના dynbuf *buf, uint64_t val)
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
  પાછા curlx_dyn_addn(buf, nval, કદ(nval));
}

સ્થિર CURLcode spack_dec64(uint64_t *val, અચલ uint8_t **src,
                            અચલ uint8_t *end)
{
  જો(end - *src < 8)
    પાછા CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  પાછા CURLE_OK;
}

સ્થિર CURLcode spack_encstr16(સંરચના dynbuf *buf, અચલ અક્ષર *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  જો(slen > UINT16_MAX)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  જો(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  પાછા result;
}

સ્થિર CURLcode spack_decstr16(અક્ષર **val, અચલ uint8_t **src,
                               અચલ uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = ખાલી;
  result = spack_dec16(&slen, src, end);
  જો(result)
    પાછા result;
  જો(end - *src < slen)
    પાછા CURLE_READ_ERROR;
  *val = curlx_memdup0((અચલ અક્ષર *)(*src), slen);
  *src += slen;
  પાછા *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

સ્થિર CURLcode spack_encdata16(સંરચના dynbuf *buf, અચલ uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  જો(data_len > UINT16_MAX)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  જો(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  પાછા result;
}

સ્થિર CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                અચલ uint8_t **src, અચલ uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = ખાલી;
  result = spack_dec16(&data_len, src, end);
  જો(result)
    પાછા result;
  જો(end - *src < data_len)
    પાછા CURLE_READ_ERROR;
  *val = curlx_memdup0((અચલ અક્ષર *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  પાછા *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(સંરચના Curl_easy *data,
                               સંરચના Curl_ssl_session *s,
                               સંરચના dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  જો(s->valid_until < 0)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  જો(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  જો(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  જો(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  જો(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  જો(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  જો(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  જો(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    જો(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  જો(!result && s->earlydata_max) {
    જો(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    જો(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    જો(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  જો(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  જો(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    જો(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  જો(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (પૂર્ણાંક)result);
  પાછા result;
}

CURLcode Curl_ssl_session_unpack(સંરચના Curl_easy *data,
                                 અચલ શૂન્ય *bufv, size_t buflen,
                                 સંરચના Curl_ssl_session **ps)
{
  સંરચના Curl_ssl_session *s = ખાલી;
  અચલ unsigned અક્ષર *buf = (અચલ unsigned અક્ષર *)bufv;
  અચલ unsigned અક્ષર *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ખાલી;

  result = spack_dec8(&val8, &buf, end);
  જો(result)
    જાઓ out;
  જો(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    જાઓ out;
  }

  s = curlx_calloc(1, કદ(*s));
  જો(!s) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  જ્યારે(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    જો(result)
      જાઓ out;

    પસંદગી(val8) {
    સ્થિતિ CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      જો(result)
        જાઓ out;
      તોડો;
    સ્થિતિ CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      જો(result)
        જાઓ out;
      s->earlydata_max = val32;
      તોડો;
    સ્થિતિ CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      જો(result)
        જાઓ out;
      s->ietf_tls_id = val16;
      તોડો;
    સ્થિતિ CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      જો(result)
        જાઓ out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      તોડો;
    }
    સ્થિતિ CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      જો(result)
        જાઓ out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      તોડો;
    }
    સ્થિતિ CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      જો(result)
        જાઓ out;
      s->valid_until = (curl_off_t)val64;
      તોડો;
    સ્થિતિ CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      તોડો;
    મૂળભૂત:  /* unknown tag */
      result = CURLE_READ_ERROR;
      જાઓ out;
    }
  }

out:
  જો(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (પૂર્ણાંક)result);
    Curl_ssl_session_destroy(s);
  }
  નહીંતર
    *ps = s;
  પાછા result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
