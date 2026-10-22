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

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "cfilters.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "cw-pause.h"


/* body dynbuf sizes */
#وصف_ڪريو CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#وصف_ڪريو CW_PAUSE_DEC_WRITE_CHUNK   4096

بنية cw_pause_buf {
  بنية cw_pause_buf *ايندڙ;
  بنية bufq b;
  صحيح type;
};

ساكن بنية cw_pause_buf *cw_pause_buf_create(صحيح type, size_t buflen)
{
  بنية cw_pause_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  إذا(cwbuf) {
    cwbuf->type = type;
    إذا(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_حد_بندي | BUFQ_OPT_NO_SPARES));
    وإلا
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  أرجع cwbuf;
}

ساكن فراغ cw_pause_buf_free(بنية cw_pause_buf *cwbuf)
{
  إذا(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_pause_ctx {
  بنية Curl_cwriter super;
  بنية cw_pause_buf *buf;
  بنية cw_pause_buf *پڇ;
  size_t buf_ڪل;
};

ساكن CURLcode cw_pause_init(بنية Curl_easy *معلومات,
                              بنية Curl_cwriter *writer)
{
  بنية cw_pause_ctx *ctx = writer->ctx;
  (فراغ)معلومات;
  ctx->buf = عدم;
  ctx->پڇ = عدم;
  أرجع CURLE_OK;
}

ساكن فراغ cw_pause_bufs_free(بنية cw_pause_ctx *ctx)
{
  طالما(ctx->buf) {
    بنية cw_pause_buf *ايندڙ = ctx->buf->ايندڙ;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = ايندڙ;
  }
  ctx->پڇ = عدم;
}

ساكن فراغ cw_pause_close(بنية Curl_easy *معلومات, بنية Curl_cwriter *writer)
{
  بنية cw_pause_ctx *ctx = writer->ctx;

  (فراغ)معلومات;
  cw_pause_bufs_free(ctx);
}

ساكن CURLcode cw_pause_flush(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *cw_pause)
{
  بنية cw_pause_ctx *ctx = (بنية cw_pause_ctx *)cw_pause;
  CURLcode نتيجو = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  طالما(ctx->buf && !Curl_cwriter_is_paused(معلومات)) {
    بنية cw_pause_buf *پهريون = ctx->buf;
    size_t blen, wlen = 0;
    ثابت بنانشان حرف *buf = عدم;

    إذا(Curl_bufq_peek(&پهريون->b, &buf, &blen)) {
      صحيح wtype = پهريون->type;
      wlen = (پهريون->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      إذا((wlen < Curl_bufq_len(&پهريون->b)) || پهريون->ايندڙ)
        wtype &= ~CLIENTWRITE_EOS;
      نتيجو = Curl_cwriter_write(معلومات, cw_pause->ايندڙ, wtype,
                                  (ثابت حرف *)buf, wlen);
      CURL_TRC_WRITE(معلومات, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_ڪل, (بنانشان صحيح)wtype,
                     (صحيح)نتيجو);
      Curl_bufq_skip(&پهريون->b, wlen);
      DEBUGASSERT(ctx->buf_ڪل >= wlen);
      ctx->buf_ڪل -= wlen;
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا إذا(پهريون->type & CLIENTWRITE_EOS) {
      نتيجو = Curl_cwriter_write(معلومات, cw_pause->ايندڙ, پهريون->type,
                                  (ثابت حرف *)buf, 0);
      CURL_TRC_WRITE(معلومات, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_ڪل, (بنانشان صحيح)پهريون->type,
                     (صحيح)نتيجو);
    }

    إذا(Curl_bufq_is_خالي_جايون(&پهريون->b)) {
      ctx->buf = پهريون->ايندڙ;
      إذا(!ctx->buf)
        ctx->پڇ = عدم;
      cw_pause_buf_free(پهريون);
    }
  }

  إذا(!نتيجو)
    نتيجو = Curl_cwriter_flush(معلومات, cw_pause->ايندڙ);
  أرجع نتيجو;
}

ساكن CURLcode cw_pause_write(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *writer, صحيح type,
                               ثابت حرف *buf, size_t blen)
{
  بنية cw_pause_ctx *ctx = writer->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t wlen = 0;

  إذا(ctx->buf && !Curl_cwriter_is_paused(معلومات)) {
    نتيجو = cw_pause_flush(معلومات, writer);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  طالما(!ctx->buf && !Curl_cwriter_is_paused(معلومات)) {
    صحيح wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    إذا(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, wtype, buf, wlen);
    إذا(نتيجو)
      أرجع نتيجو;
    buf += wlen;
    blen -= wlen;
    إذا(!blen)
      أرجع نتيجو;
  }

  افعل {
    size_t nwritten = 0;
    إذا(ctx->پڇ && (ctx->پڇ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      نتيجو = Curl_bufq_cwrite(&ctx->پڇ->b, buf, blen, &nwritten);
    }
    وإلا {
      /* Need a new buf, type changed */
      بنية cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      إذا(!cwbuf)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      إذا(ctx->پڇ)
        ctx->پڇ->ايندڙ = cwbuf;
      وإلا
        ctx->buf = cwbuf;
      ctx->پڇ = cwbuf;
      نتيجو = Curl_bufq_cwrite(&ctx->پڇ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(معلومات, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (بنانشان صحيح)type,
                   ctx->buf_ڪل + wlen, (صحيح)نتيجو);
    إذا(نتيجو)
      أرجع نتيجو;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_ڪل += nwritten;
  } طالما(blen);

  أرجع نتيجو;
}

ثابت بنية Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  عدم,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  حجم(بنية cw_pause_ctx)
};
