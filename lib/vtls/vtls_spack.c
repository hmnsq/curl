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

#nếu defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#include "urldata.h"
#include "curl_trc.h"
#include "vtls/vtls_scache.h"
#include "vtls/vtls_spack.h"
#include "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

static CURLcode spack_enc8(struct dynbuf *buf, uint8_t b)
{
  trả_về curlx_dyn_addn(buf, &b, 1);
}

static CURLcode spack_dec8(uint8_t *val, const uint8_t **src,
                           const uint8_t *end)
{
  nếu(end - *src < 1)
    trả_về CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  trả_về CURLE_OK;
}

static CURLcode spack_enc16(struct dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  trả_về curlx_dyn_addn(buf, nval, sizeof(nval));
}

static CURLcode spack_dec16(uint16_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  nếu(end - *src < 2)
    trả_về CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  trả_về CURLE_OK;
}

static CURLcode spack_enc32(struct dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  trả_về curlx_dyn_addn(buf, nval, sizeof(nval));
}

static CURLcode spack_dec32(uint32_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  nếu(end - *src < 4)
    trả_về CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  trả_về CURLE_OK;
}

static CURLcode spack_enc64(struct dynbuf *buf, uint64_t val)
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
  trả_về curlx_dyn_addn(buf, nval, sizeof(nval));
}

static CURLcode spack_dec64(uint64_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  nếu(end - *src < 8)
    trả_về CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  trả_về CURLE_OK;
}

static CURLcode spack_encstr16(struct dynbuf *buf, const char *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  nếu(slen > UINT16_MAX)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  nếu(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  trả_về result;
}

static CURLcode spack_decstr16(char **val, const uint8_t **src,
                               const uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = NULL;
  result = spack_dec16(&slen, src, end);
  nếu(result)
    trả_về result;
  nếu(end - *src < slen)
    trả_về CURLE_READ_ERROR;
  *val = curlx_memdup0((const char *)(*src), slen);
  *src += slen;
  trả_về *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode spack_encdata16(struct dynbuf *buf, const uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  nếu(data_len > UINT16_MAX)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  nếu(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  trả_về result;
}

static CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                const uint8_t **src, const uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = NULL;
  result = spack_dec16(&data_len, src, end);
  nếu(result)
    trả_về result;
  nếu(end - *src < data_len)
    trả_về CURLE_READ_ERROR;
  *val = curlx_memdup0((const char *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  trả_về *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(struct Curl_easy *data,
                               struct Curl_ssl_session *s,
                               struct dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  nếu(s->valid_until < 0)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  nếu(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  nếu(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  nếu(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  nếu(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  nếu(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  nếu(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  nếu(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    nếu(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  nếu(!result && s->earlydata_max) {
    nếu(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    nếu(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    nếu(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  nếu(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  nếu(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    nếu(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  nếu(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (số_nguyên)result);
  trả_về result;
}

CURLcode Curl_ssl_session_unpack(struct Curl_easy *data,
                                 const void *bufv, size_t buflen,
                                 struct Curl_ssl_session **ps)
{
  struct Curl_ssl_session *s = NULL;
  const unsigned char *buf = (const unsigned char *)bufv;
  const unsigned char *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = NULL;

  result = spack_dec8(&val8, &buf, end);
  nếu(result)
    goto out;
  nếu(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    goto out;
  }

  s = curlx_calloc(1, sizeof(*s));
  nếu(!s) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  trong_khi(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    nếu(result)
      goto out;

    switch(val8) {
    case CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      nếu(result)
        goto out;
      break;
    case CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      nếu(result)
        goto out;
      s->earlydata_max = val32;
      break;
    case CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      nếu(result)
        goto out;
      s->ietf_tls_id = val16;
      break;
    case CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      nếu(result)
        goto out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      break;
    }
    case CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      nếu(result)
        goto out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      break;
    }
    case CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      nếu(result)
        goto out;
      s->valid_until = (curl_off_t)val64;
      break;
    case CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      break;
    default:  /* unknown tag */
      result = CURLE_READ_ERROR;
      goto out;
    }
  }

out:
  nếu(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (số_nguyên)result);
    Curl_ssl_session_destroy(s);
  }
  không_thì
    *ps = s;
  trả_về result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
