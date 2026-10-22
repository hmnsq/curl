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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_spack.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_ਸੰਸਕਰਣ       0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_IETF_ID       0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_VALID_UNTIL   0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_TICKET        0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_ALPN          0x05
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_EARLYDATA     0x06
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_QUICTP        0x07
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SPACK_SECTRUST      0x08

ਸਥਿਰ CURLcode spack_enc8(ਬਣਤਰ dynbuf *buf, uint8_t b)
{
  ਵਾਪਸ curlx_dyn_addn(buf, &b, 1);
}

ਸਥਿਰ CURLcode spack_dec8(uint8_t *val, ਅਚਲ uint8_t **src,
                           ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਜੇ(ਸਿਰਾ - *src < 1)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode spack_enc16(ਬਣਤਰ dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  ਵਾਪਸ curlx_dyn_addn(buf, nval, ਆਕਾਰ(nval));
}

ਸਥਿਰ CURLcode spack_dec16(uint16_t *val, ਅਚਲ uint8_t **src,
                            ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਜੇ(ਸਿਰਾ - *src < 2)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode spack_enc32(ਬਣਤਰ dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  ਵਾਪਸ curlx_dyn_addn(buf, nval, ਆਕਾਰ(nval));
}

ਸਥਿਰ CURLcode spack_dec32(uint32_t *val, ਅਚਲ uint8_t **src,
                            ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਜੇ(ਸਿਰਾ - *src < 4)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode spack_enc64(ਬਣਤਰ dynbuf *buf, uint64_t val)
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
  ਵਾਪਸ curlx_dyn_addn(buf, nval, ਆਕਾਰ(nval));
}

ਸਥਿਰ CURLcode spack_dec64(uint64_t *val, ਅਚਲ uint8_t **src,
                            ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਜੇ(ਸਿਰਾ - *src < 8)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode spack_encstr16(ਬਣਤਰ dynbuf *buf, ਅਚਲ ਅੱਖਰ *s)
{
  size_t slen = ਲੜੀ_ਲੰਬਾਈ(s);
  CURLcode ਨਤੀਜਾ;
  ਜੇ(slen > UINT16_MAX)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਨਤੀਜਾ = spack_enc16(buf, (uint16_t)slen);
  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = curlx_dyn_addn(buf, s, slen);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode spack_decstr16(ਅੱਖਰ **val, ਅਚਲ uint8_t **src,
                               ਅਚਲ uint8_t *ਸਿਰਾ)
{
  uint16_t slen;
  CURLcode ਨਤੀਜਾ;

  *val = ਨਿਰਾ;
  ਨਤੀਜਾ = spack_dec16(&slen, src, ਸਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(ਸਿਰਾ - *src < slen)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)(*src), slen);
  *src += slen;
  ਵਾਪਸ *val ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

ਸਥਿਰ CURLcode spack_encdata16(ਬਣਤਰ dynbuf *buf, ਅਚਲ uint8_t *ਅੰਕੜੇ,
                                size_t ਅੰਕੜੇ_len)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(ਅੰਕੜੇ_len > UINT16_MAX)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਨਤੀਜਾ = spack_enc16(buf, (uint16_t)ਅੰਕੜੇ_len);
  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = curlx_dyn_addn(buf, ਅੰਕੜੇ, ਅੰਕੜੇ_len);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ਅਚਲ uint8_t **src, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  uint16_t ਅੰਕੜੇ_len;
  CURLcode ਨਤੀਜਾ;

  *val = ਨਿਰਾ;
  ਨਤੀਜਾ = spack_dec16(&ਅੰਕੜੇ_len, src, ਸਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(ਸਿਰਾ - *src < ਅੰਕੜੇ_len)
    ਵਾਪਸ CURLE_READ_ERROR;
  *val = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)(*src), ਅੰਕੜੇ_len);
  *val_len = ਅੰਕੜੇ_len;
  *src += ਅੰਕੜੇ_len;
  ਵਾਪਸ *val ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

CURLcode Curl_ssl_session_pack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_ssl_session *s,
                               ਬਣਤਰ dynbuf *buf)
{
  CURLcode ਨਤੀਜਾ;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  ਜੇ(s->valid_until < 0)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_ਸੰਸਕਰਣ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_TICKET);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_encdata16(buf, s->sdata, s->sdata_len);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_IETF_ID);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = spack_enc64(buf, (uint64_t)s->valid_until);
  ਜੇ(!ਨਤੀਜਾ && s->alpn) {
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_ALPN);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = spack_encstr16(buf, s->alpn);
  }
  ਜੇ(!ਨਤੀਜਾ && s->earlydata_max) {
    ਜੇ(s->earlydata_max > UINT32_MAX)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  ਜੇ(!ਨਤੀਜਾ && s->sectrust_verified) {
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  ਜੇ(!ਨਤੀਜਾ && s->quic_tp && s->quic_tp_len) {
    ਨਤੀਜਾ = spack_enc8(buf, CURL_SPACK_QUICTP);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  ਜੇ(ਨਤੀਜਾ)
    CURL_TRC_SSLS(ਅੰਕੜੇ, "error packing data: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_session_unpack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਖਾਲੀ *bufv, size_t buflen,
                                 ਬਣਤਰ Curl_ssl_session **ps)
{
  ਬਣਤਰ Curl_ssl_session *s = ਨਿਰਾ;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)bufv;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਿਰਾ = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ਨਿਰਾ;

  ਨਤੀਜਾ = spack_dec8(&val8, &buf, ਸਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(val8 != CURL_SPACK_ਸੰਸਕਰਣ) {
    ਨਤੀਜਾ = CURLE_READ_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  s = curlx_calloc(1, ਆਕਾਰ(*s));
  ਜੇ(!s) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜਦੋਂ(buf < ਸਿਰਾ) {
    ਨਤੀਜਾ = spack_dec8(&val8, &buf, ਸਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਚੋਣ(val8) {
    ਹਾਲਤ CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      ਨਤੀਜਾ = spack_decstr16(&s->alpn, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SPACK_EARLYDATA:
      ਨਤੀਜਾ = spack_dec32(&val32, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      s->earlydata_max = val32;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SPACK_IETF_ID:
      ਨਤੀਜਾ = spack_dec16(&val16, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      s->ietf_tls_id = val16;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SPACK_QUICTP: {
      ਨਤੀਜਾ = spack_decdata16(&pval8, &dlen, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      ਰੋਕੋ;
    }
    ਹਾਲਤ CURL_SPACK_TICKET: {
      ਨਤੀਜਾ = spack_decdata16(&pval8, &dlen, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      ਰੋਕੋ;
    }
    ਹਾਲਤ CURL_SPACK_VALID_UNTIL:
      ਨਤੀਜਾ = spack_dec64(&val64, &buf, ਸਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      s->valid_until = (curl_off_t)val64;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      ਰੋਕੋ;
    ਮੂਲ:  /* unknown tag */
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_SSLS(ਅੰਕੜੇ, "error unpacking data: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    Curl_ssl_session_destroy(s);
  }
  ਨਹੀਂਤਾਂ
    *ps = s;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_SSL && USE_SSLS_EXPORT */
