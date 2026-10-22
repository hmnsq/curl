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

#ᮚᮦᮔ᮪ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

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

static CURLcode spack_enc8(sa_nracanaa dynbuf *buf, uint8_t b)
{
  ᮝᮜᮤ curlx_dyn_addn(buf, &b, 1);
}

static CURLcode spack_dec8(uint8_t *val, const uint8_t **src,
                           const uint8_t *end)
{
  ᮚᮦᮔ᮪(end - *src < 1)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode spack_enc16(sa_nracanaa dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  ᮝᮜᮤ curlx_dyn_addn(buf, nval, parimaa_na(nval));
}

static CURLcode spack_dec16(uint16_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  ᮚᮦᮔ᮪(end - *src < 2)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode spack_enc32(sa_nracanaa dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  ᮝᮜᮤ curlx_dyn_addn(buf, nval, parimaa_na(nval));
}

static CURLcode spack_dec32(uint32_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  ᮚᮦᮔ᮪(end - *src < 4)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode spack_enc64(sa_nracanaa dynbuf *buf, uint64_t val)
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
  ᮝᮜᮤ curlx_dyn_addn(buf, nval, parimaa_na(nval));
}

static CURLcode spack_dec64(uint64_t *val, const uint8_t **src,
                            const uint8_t *end)
{
  ᮚᮦᮔ᮪(end - *src < 8)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode spack_encstr16(sa_nracanaa dynbuf *buf, const var_na *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  ᮚᮦᮔ᮪(slen > UINT16_MAX)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  ᮚᮦᮔ᮪(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  ᮝᮜᮤ result;
}

static CURLcode spack_decstr16(var_na **val, const uint8_t **src,
                               const uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = NULL;
  result = spack_dec16(&slen, src, end);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  ᮚᮦᮔ᮪(end - *src < slen)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = curlx_memdup0((const var_na *)(*src), slen);
  *src += slen;
  ᮝᮜᮤ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode spack_encdata16(sa_nracanaa dynbuf *buf, const uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  ᮚᮦᮔ᮪(data_len > UINT16_MAX)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  ᮚᮦᮔ᮪(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  ᮝᮜᮤ result;
}

static CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                const uint8_t **src, const uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = NULL;
  result = spack_dec16(&data_len, src, end);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  ᮚᮦᮔ᮪(end - *src < data_len)
    ᮝᮜᮤ CURLE_READ_ERROR;
  *val = curlx_memdup0((const var_na *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  ᮝᮜᮤ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_ssl_session *s,
                               sa_nracanaa dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  ᮚᮦᮔ᮪(s->valid_until < 0)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  ᮚᮦᮔ᮪(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  ᮚᮦᮔ᮪(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  ᮚᮦᮔ᮪(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  ᮚᮦᮔ᮪(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  ᮚᮦᮔ᮪(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  ᮚᮦᮔ᮪(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  ᮚᮦᮔ᮪(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    ᮚᮦᮔ᮪(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  ᮚᮦᮔ᮪(!result && s->earlydata_max) {
    ᮚᮦᮔ᮪(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ᮚᮦᮔ᮪(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    ᮚᮦᮔ᮪(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  ᮚᮦᮔ᮪(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  ᮚᮦᮔ᮪(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    ᮚᮦᮔ᮪(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  ᮚᮦᮔ᮪(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (puur_naa_nka)result);
  ᮝᮜᮤ result;
}

CURLcode Curl_ssl_session_unpack(sa_nracanaa Curl_easy *data,
                                 const void *bufv, size_t buflen,
                                 sa_nracanaa Curl_ssl_session **ps)
{
  sa_nracanaa Curl_ssl_session *s = NULL;
  const unsigned var_na *buf = (const unsigned var_na *)bufv;
  const unsigned var_na *end = buf + buflen;
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
  ᮚᮦᮔ᮪(result)
    goto out;
  ᮚᮦᮔ᮪(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    goto out;
  }

  s = curlx_calloc(1, parimaa_na(*s));
  ᮚᮦᮔ᮪(!s) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ᮞᮨᮓᮨᮜ(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    ᮚᮦᮔ᮪(result)
      goto out;

    switch(val8) {
    pak_sa CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      bha_nga;
    pak_sa CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      s->earlydata_max = val32;
      bha_nga;
    pak_sa CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      s->ietf_tls_id = val16;
      bha_nga;
    pak_sa CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      bha_nga;
    }
    pak_sa CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      bha_nga;
    }
    pak_sa CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      ᮚᮦᮔ᮪(result)
        goto out;
      s->valid_until = (curl_off_t)val64;
      bha_nga;
    pak_sa CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      bha_nga;
    default:  /* unknown tag */
      result = CURLE_READ_ERROR;
      goto out;
    }
  }

out:
  ᮚᮦᮔ᮪(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (puur_naa_nka)result);
    Curl_ssl_session_destroy(s);
  }
  ᮜᮤᮚ
    *ps = s;
  ᮝᮜᮤ result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
