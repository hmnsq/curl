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
#شامل "curl_setup.h"

#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/vtls_spack.h"
#شامل "curlx/strdup.h"

#تعریف_کرو CURL_SPACK_نسخہ       0x01
#تعریف_کرو CURL_SPACK_IETF_ID       0x02
#تعریف_کرو CURL_SPACK_VALID_UNTIL   0x03
#تعریف_کرو CURL_SPACK_TICKET        0x04
#تعریف_کرو CURL_SPACK_ALPN          0x05
#تعریف_کرو CURL_SPACK_EARLYDATA     0x06
#تعریف_کرو CURL_SPACK_QUICTP        0x07
#تعریف_کرو CURL_SPACK_SECTRUST      0x08

ساکن CURLcode spack_enc8(ساخت dynbuf *buf, uint8_t b)
{
  واپس curlx_dyn_addn(buf, &b, 1);
}

ساکن CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *انتہا)
{
  اگر(انتہا - *src < 1)
    واپس CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  واپس CURLE_OK;
}

ساکن CURLcode spack_enc16(ساخت dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  واپس curlx_dyn_addn(buf, nval, حجم(nval));
}

ساکن CURLcode spack_dec16(uint16_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتہا)
{
  اگر(انتہا - *src < 2)
    واپس CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  واپس CURLE_OK;
}

ساکن CURLcode spack_enc32(ساخت dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  واپس curlx_dyn_addn(buf, nval, حجم(nval));
}

ساکن CURLcode spack_dec32(uint32_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتہا)
{
  اگر(انتہا - *src < 4)
    واپس CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  واپس CURLE_OK;
}

ساکن CURLcode spack_enc64(ساخت dynbuf *buf, uint64_t val)
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
  واپس curlx_dyn_addn(buf, nval, حجم(nval));
}

ساکن CURLcode spack_dec64(uint64_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتہا)
{
  اگر(انتہا - *src < 8)
    واپس CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  واپس CURLE_OK;
}

ساکن CURLcode spack_encstr16(ساخت dynbuf *buf, ثابت حرف *s)
{
  size_t slen = لڑی_لمبائی(s);
  CURLcode نتیجہ;
  اگر(slen > UINT16_MAX)
    واپس CURLE_BAD_FUNCTION_دلیل;
  نتیجہ = spack_enc16(buf, (uint16_t)slen);
  اگر(!نتیجہ) {
    نتیجہ = curlx_dyn_addn(buf, s, slen);
  }
  واپس نتیجہ;
}

ساکن CURLcode spack_decstr16(حرف **val, ثابت uint8_t **src,
                               ثابت uint8_t *انتہا)
{
  uint16_t slen;
  CURLcode نتیجہ;

  *val = تہی;
  نتیجہ = spack_dec16(&slen, src, انتہا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(انتہا - *src < slen)
    واپس CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), slen);
  *src += slen;
  واپس *val ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

ساکن CURLcode spack_encdata16(ساخت dynbuf *buf, ثابت uint8_t *ڈیٹا,
                                size_t ڈیٹا_len)
{
  CURLcode نتیجہ;
  اگر(ڈیٹا_len > UINT16_MAX)
    واپس CURLE_BAD_FUNCTION_دلیل;
  نتیجہ = spack_enc16(buf, (uint16_t)ڈیٹا_len);
  اگر(!نتیجہ) {
    نتیجہ = curlx_dyn_addn(buf, ڈیٹا, ڈیٹا_len);
  }
  واپس نتیجہ;
}

ساکن CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *انتہا)
{
  uint16_t ڈیٹا_len;
  CURLcode نتیجہ;

  *val = تہی;
  نتیجہ = spack_dec16(&ڈیٹا_len, src, انتہا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(انتہا - *src < ڈیٹا_len)
    واپس CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), ڈیٹا_len);
  *val_len = ڈیٹا_len;
  *src += ڈیٹا_len;
  واپس *val ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

CURLcode Curl_ssl_session_pack(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_ssl_session *s,
                               ساخت dynbuf *buf)
{
  CURLcode نتیجہ;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  اگر(s->valid_until < 0)
    واپس CURLE_BAD_FUNCTION_دلیل;

  نتیجہ = spack_enc8(buf, CURL_SPACK_نسخہ);
  اگر(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_TICKET);
  اگر(!نتیجہ)
    نتیجہ = spack_encdata16(buf, s->sdata, s->sdata_len);
  اگر(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_IETF_ID);
  اگر(!نتیجہ)
    نتیجہ = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  اگر(!نتیجہ)
    نتیجہ = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  اگر(!نتیجہ)
    نتیجہ = spack_enc64(buf, (uint64_t)s->valid_until);
  اگر(!نتیجہ && s->alpn) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_ALPN);
    اگر(!نتیجہ)
      نتیجہ = spack_encstr16(buf, s->alpn);
  }
  اگر(!نتیجہ && s->earlydata_max) {
    اگر(s->earlydata_max > UINT32_MAX)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اگر(!نتیجہ)
      نتیجہ = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    اگر(!نتیجہ)
      نتیجہ = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  اگر(!نتیجہ && s->sectrust_verified) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  اگر(!نتیجہ && s->quic_tp && s->quic_tp_len) {
    نتیجہ = spack_enc8(buf, CURL_SPACK_QUICTP);
    اگر(!نتیجہ)
      نتیجہ = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  اگر(نتیجہ)
    CURL_TRC_SSLS(ڈیٹا, "error packing data: %d", (صحیح)نتیجہ);
  واپس نتیجہ;
}

CURLcode Curl_ssl_session_unpack(ساخت Curl_easy *ڈیٹا,
                                 ثابت خالی *bufv, size_t buflen,
                                 ساخت Curl_ssl_session **ps)
{
  ساخت Curl_ssl_session *s = تہی;
  ثابت غیرعلامتی حرف *buf = (ثابت غیرعلامتی حرف *)bufv;
  ثابت غیرعلامتی حرف *انتہا = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode نتیجہ;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = تہی;

  نتیجہ = spack_dec8(&val8, &buf, انتہا);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(val8 != CURL_SPACK_نسخہ) {
    نتیجہ = CURLE_READ_ERROR;
    جاؤ برآمد;
  }

  s = curlx_calloc(1, حجم(*s));
  اگر(!s) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  جب(buf < انتہا) {
    نتیجہ = spack_dec8(&val8, &buf, انتہا);
    اگر(نتیجہ)
      جاؤ برآمد;

    انتخاب(val8) {
    حالت CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      نتیجہ = spack_decstr16(&s->alpn, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      توڑو;
    حالت CURL_SPACK_EARLYDATA:
      نتیجہ = spack_dec32(&val32, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      s->earlydata_max = val32;
      توڑو;
    حالت CURL_SPACK_IETF_ID:
      نتیجہ = spack_dec16(&val16, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      s->ietf_tls_id = val16;
      توڑو;
    حالت CURL_SPACK_QUICTP: {
      نتیجہ = spack_decdata16(&pval8, &dlen, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      توڑو;
    }
    حالت CURL_SPACK_TICKET: {
      نتیجہ = spack_decdata16(&pval8, &dlen, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      توڑو;
    }
    حالت CURL_SPACK_VALID_UNTIL:
      نتیجہ = spack_dec64(&val64, &buf, انتہا);
      اگر(نتیجہ)
        جاؤ برآمد;
      s->valid_until = (curl_off_t)val64;
      توڑو;
    حالت CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      توڑو;
    طےشدہ:  /* unknown tag */
      نتیجہ = CURLE_READ_ERROR;
      جاؤ برآمد;
    }
  }

برآمد:
  اگر(نتیجہ) {
    CURL_TRC_SSLS(ڈیٹا, "error unpacking data: %d", (صحیح)نتیجہ);
    Curl_ssl_session_destroy(s);
  }
  ورنہ
    *ps = s;
  واپس نتیجہ;
}

#اگر_ختم /* USE_SSL && USE_SSLS_EXPORT */
