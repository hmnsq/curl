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
#ضمن "curl_setup.h"

#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/vtls_spack.h"
#ضمن "curlx/strdup.h"

#تعریف_کرو CURL_SPACK_نسخہ       0x01
#تعریف_کرو CURL_SPACK_IETF_ID       0x02
#تعریف_کرو CURL_SPACK_VALID_UNTIL   0x03
#تعریف_کرو CURL_SPACK_TICKET        0x04
#تعریف_کرو CURL_SPACK_ALPN          0x05
#تعریف_کرو CURL_SPACK_EARLYDATA     0x06
#تعریف_کرو CURL_SPACK_QUICTP        0x07
#تعریف_کرو CURL_SPACK_SECTRUST      0x08

ساكن CURLcode spack_enc8(بنية dynbuf *buf, uint8_t b)
{
  أرجع curlx_dyn_addn(buf, &b, 1);
}

ساكن CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *سرا)
{
  إذا(سرا - *src < 1)
    أرجع CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_enc16(بنية dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec16(uint16_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *سرا)
{
  إذا(سرا - *src < 2)
    أرجع CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_enc32(بنية dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec32(uint32_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *سرا)
{
  إذا(سرا - *src < 4)
    أرجع CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_enc64(بنية dynbuf *buf, uint64_t val)
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
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec64(uint64_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *سرا)
{
  إذا(سرا - *src < 8)
    أرجع CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_encstr16(بنية dynbuf *buf, ثابت حرف *s)
{
  size_t slen = لڑی_لمبائی(s);
  CURLcode نتیجہ;
  إذا(slen > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  نتیجہ = spack_enc16(buf, (uint16_t)slen);
  إذا(!نتیجہ) {
    نتیجہ = curlx_dyn_addn(buf, s, slen);
  }
  أرجع نتیجہ;
}

ساكن CURLcode spack_decstr16(حرف **val, ثابت uint8_t **src,
                               ثابت uint8_t *سرا)
{
  uint16_t slen;
  CURLcode نتیجہ;

  *val = عدم;
  نتیجہ = spack_dec16(&slen, src, سرا);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(سرا - *src < slen)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), slen);
  *src += slen;
  أرجع *val ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
}

ساكن CURLcode spack_encdata16(بنية dynbuf *buf, ثابت uint8_t *انکڑے,
                                size_t انکڑے_len)
{
  CURLcode نتیجہ;
  إذا(انکڑے_len > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  نتیجہ = spack_enc16(buf, (uint16_t)انکڑے_len);
  إذا(!نتیجہ) {
    نتیجہ = curlx_dyn_addn(buf, انکڑے, انکڑے_len);
  }
  أرجع نتیجہ;
}

ساكن CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *سرا)
{
  uint16_t انکڑے_len;
  CURLcode نتیجہ;

  *val = عدم;
  نتیجہ = spack_dec16(&انکڑے_len, src, سرا);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(سرا - *src < انکڑے_len)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), انکڑے_len);
  *val_len = انکڑے_len;
  *src += انکڑے_len;
  أرجع *val ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
}

CURLcode Curl_ssl_session_pack(بنية Curl_easy *انکڑے,
                               بنية Curl_ssl_session *s,
                               بنية dynbuf *buf)
{
  CURLcode نتیجہ;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  إذا(s->valid_until < 0)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  نتیجہ = spack_enc8(buf, CURL_SPACK_نسخہ);
  إذا(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_TICKET);
  إذا(!نتیجہ)
    نتیجہ = spack_encdata16(buf, s->sdata, s->sdata_len);
  إذا(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_IETF_ID);
  إذا(!نتیجہ)
    نتیجہ = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  إذا(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  إذا(!نتیجہ)
    نتیجہ = spack_enc64(buf, (uint64_t)s->valid_until);
  إذا(!نتیجہ && s->alpn) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_ALPN);
    إذا(!نتیجہ)
      نتیجہ = spack_encstr16(buf, s->alpn);
  }
  إذا(!نتیجہ && s->earlydata_max) {
    إذا(s->earlydata_max > UINT32_MAX)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    إذا(!نتیجہ)
      نتیجہ = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    إذا(!نتیجہ)
      نتیجہ = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  إذا(!نتیجہ && s->sectrust_verified) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  إذا(!نتیجہ && s->quic_tp && s->quic_tp_len) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_QUICTP);
    إذا(!نتیجہ)
      نتیجہ = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  إذا(نتیجہ)
    CURL_TRC_SSLS(انکڑے, "error packing data: %d", (صحيح)نتیجہ);
  أرجع نتیجہ;
}

CURLcode Curl_ssl_session_unpack(بنية Curl_easy *انکڑے,
                                 ثابت فراغ *bufv, size_t buflen,
                                 بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_session *s = عدم;
  ثابت بنانشانی حرف *buf = (ثابت بنانشانی حرف *)bufv;
  ثابت بنانشانی حرف *سرا = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode نتیجہ;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = عدم;

  نتیجہ = spack_dec8(&val8, &buf, سرا);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(val8 != CURL_SPACK_نسخہ) {
    نتیجہ = CURLE_READ_ERROR;
    اذهب باہرلا;
  }

  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  طالما(buf < سرا) {
    نتیجہ = spack_dec8(&val8, &buf, سرا);
    إذا(نتیجہ)
      اذهب باہرلا;

    بدل(val8) {
    حالة CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      نتیجہ = spack_decstr16(&s->alpn, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      اكسر;
    حالة CURL_SPACK_EARLYDATA:
      نتیجہ = spack_dec32(&val32, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      s->earlydata_max = val32;
      اكسر;
    حالة CURL_SPACK_IETF_ID:
      نتیجہ = spack_dec16(&val16, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      s->ietf_tls_id = val16;
      اكسر;
    حالة CURL_SPACK_QUICTP: {
      نتیجہ = spack_decdata16(&pval8, &dlen, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_TICKET: {
      نتیجہ = spack_decdata16(&pval8, &dlen, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_VALID_UNTIL:
      نتیجہ = spack_dec64(&val64, &buf, سرا);
      إذا(نتیجہ)
        اذهب باہرلا;
      s->valid_until = (curl_off_t)val64;
      اكسر;
    حالة CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      اكسر;
    افتراضي:  /* unknown tag */
      نتیجہ = CURLE_READ_ERROR;
      اذهب باہرلا;
    }
  }

باہرلا:
  إذا(نتیجہ) {
    CURL_TRC_SSLS(انکڑے, "error unpacking data: %d", (صحيح)نتیجہ);
    Curl_ssl_session_destroy(s);
  }
  وإلا
    *ps = s;
  أرجع نتیجہ;
}

#جے_انت /* USE_SSL && USE_SSLS_EXPORT */
