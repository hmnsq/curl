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

#شامل "urldata.h"
#شامل "bufq.h"
#شامل "cfilters.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "cw-pause.h"


/* body dynbuf sizes */
#تعریف_کرو CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#تعریف_کرو CW_PAUSE_DEC_WRITE_CHUNK   4096

ساخت cw_pause_buf {
  ساخت cw_pause_buf *اگلا;
  ساخت bufq b;
  صحیح type;
};

ساکن ساخت cw_pause_buf *cw_pause_buf_create(صحیح type, size_t buflen)
{
  ساخت cw_pause_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  اگر(cwbuf) {
    cwbuf->type = type;
    اگر(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_حد_بندی | BUFQ_OPT_NO_SPARES));
    ورنہ
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  واپس cwbuf;
}

ساکن خالی cw_pause_buf_free(ساخت cw_pause_buf *cwbuf)
{
  اگر(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ساخت cw_pause_ctx {
  ساخت Curl_cwriter super;
  ساخت cw_pause_buf *buf;
  ساخت cw_pause_buf *دم;
  size_t buf_کل;
};

ساکن CURLcode cw_pause_init(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_cwriter *writer)
{
  ساخت cw_pause_ctx *ctx = writer->ctx;
  (خالی)ڈیٹا;
  ctx->buf = تہی;
  ctx->دم = تہی;
  واپس CURLE_OK;
}

ساکن خالی cw_pause_bufs_free(ساخت cw_pause_ctx *ctx)
{
  جب(ctx->buf) {
    ساخت cw_pause_buf *اگلا = ctx->buf->اگلا;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = اگلا;
  }
  ctx->دم = تہی;
}

ساکن خالی cw_pause_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_cwriter *writer)
{
  ساخت cw_pause_ctx *ctx = writer->ctx;

  (خالی)ڈیٹا;
  cw_pause_bufs_free(ctx);
}

ساکن CURLcode cw_pause_flush(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *cw_pause)
{
  ساخت cw_pause_ctx *ctx = (ساخت cw_pause_ctx *)cw_pause;
  CURLcode نتیجہ = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  جب(ctx->buf && !Curl_cwriter_is_paused(ڈیٹا)) {
    ساخت cw_pause_buf *پہلا = ctx->buf;
    size_t blen, wlen = 0;
    ثابت غیرعلامتی حرف *buf = تہی;

    اگر(Curl_bufq_peek(&پہلا->b, &buf, &blen)) {
      صحیح wtype = پہلا->type;
      wlen = (پہلا->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      اگر((wlen < Curl_bufq_len(&پہلا->b)) || پہلا->اگلا)
        wtype &= ~CLIENTWRITE_EOS;
      نتیجہ = Curl_cwriter_write(ڈیٹا, cw_pause->اگلا, wtype,
                                  (ثابت حرف *)buf, wlen);
      CURL_TRC_WRITE(ڈیٹا, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_کل, (غیرعلامتی صحیح)wtype,
                     (صحیح)نتیجہ);
      Curl_bufq_skip(&پہلا->b, wlen);
      DEBUGASSERT(ctx->buf_کل >= wlen);
      ctx->buf_کل -= wlen;
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ اگر(پہلا->type & CLIENTWRITE_EOS) {
      نتیجہ = Curl_cwriter_write(ڈیٹا, cw_pause->اگلا, پہلا->type,
                                  (ثابت حرف *)buf, 0);
      CURL_TRC_WRITE(ڈیٹا, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_کل, (غیرعلامتی صحیح)پہلا->type,
                     (صحیح)نتیجہ);
    }

    اگر(Curl_bufq_is_خالی_خانے(&پہلا->b)) {
      ctx->buf = پہلا->اگلا;
      اگر(!ctx->buf)
        ctx->دم = تہی;
      cw_pause_buf_free(پہلا);
    }
  }

  اگر(!نتیجہ)
    نتیجہ = Curl_cwriter_flush(ڈیٹا, cw_pause->اگلا);
  واپس نتیجہ;
}

ساکن CURLcode cw_pause_write(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *writer, صحیح type,
                               ثابت حرف *buf, size_t blen)
{
  ساخت cw_pause_ctx *ctx = writer->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t wlen = 0;

  اگر(ctx->buf && !Curl_cwriter_is_paused(ڈیٹا)) {
    نتیجہ = cw_pause_flush(ڈیٹا, writer);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  جب(!ctx->buf && !Curl_cwriter_is_paused(ڈیٹا)) {
    صحیح wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    اگر(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, wtype, buf, wlen);
    اگر(نتیجہ)
      واپس نتیجہ;
    buf += wlen;
    blen -= wlen;
    اگر(!blen)
      واپس نتیجہ;
  }

  کرو {
    size_t nwritten = 0;
    اگر(ctx->دم && (ctx->دم->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      نتیجہ = Curl_bufq_cwrite(&ctx->دم->b, buf, blen, &nwritten);
    }
    ورنہ {
      /* Need a new buf, type changed */
      ساخت cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      اگر(!cwbuf)
        واپس CURLE_برآمد_کا_حافظہ;
      اگر(ctx->دم)
        ctx->دم->اگلا = cwbuf;
      ورنہ
        ctx->buf = cwbuf;
      ctx->دم = cwbuf;
      نتیجہ = Curl_bufq_cwrite(&ctx->دم->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(ڈیٹا, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (غیرعلامتی صحیح)type,
                   ctx->buf_کل + wlen, (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_کل += nwritten;
  } جب(blen);

  واپس نتیجہ;
}

ثابت ساخت Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  تہی,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  حجم(ساخت cw_pause_ctx)
};
