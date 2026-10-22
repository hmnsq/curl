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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-pause.h"


/* body dynbuf sizes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CW_PAUSE_DEC_WRITE_CHUNK   4096

ਬਣਤਰ cw_pause_buf {
  ਬਣਤਰ cw_pause_buf *ਅਗਲਾ;
  ਬਣਤਰ bufq b;
  ਪੂਰਨ type;
};

ਸਥਿਰ ਬਣਤਰ cw_pause_buf *cw_pause_buf_create(ਪੂਰਨ type, size_t buflen)
{
  ਬਣਤਰ cw_pause_buf *cwbuf = curlx_calloc(1, ਆਕਾਰ(*cwbuf));
  ਜੇ(cwbuf) {
    cwbuf->type = type;
    ਜੇ(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ | BUFQ_OPT_NO_SPARES));
    ਨਹੀਂਤਾਂ
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ਵਾਪਸ cwbuf;
}

ਸਥਿਰ ਖਾਲੀ cw_pause_buf_free(ਬਣਤਰ cw_pause_buf *cwbuf)
{
  ਜੇ(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ਬਣਤਰ cw_pause_ctx {
  ਬਣਤਰ Curl_cwriter super;
  ਬਣਤਰ cw_pause_buf *buf;
  ਬਣਤਰ cw_pause_buf *ਪੂਛ;
  size_t buf_ਕੁੱਲ;
};

ਸਥਿਰ CURLcode cw_pause_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ cw_pause_ctx *ctx = writer->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx->buf = ਨਿਰਾ;
  ctx->ਪੂਛ = ਨਿਰਾ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cw_pause_bufs_free(ਬਣਤਰ cw_pause_ctx *ctx)
{
  ਜਦੋਂ(ctx->buf) {
    ਬਣਤਰ cw_pause_buf *ਅਗਲਾ = ctx->buf->ਅਗਲਾ;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = ਅਗਲਾ;
  }
  ctx->ਪੂਛ = ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ cw_pause_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ cw_pause_ctx *ctx = writer->ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  cw_pause_bufs_free(ctx);
}

ਸਥਿਰ CURLcode cw_pause_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *cw_pause)
{
  ਬਣਤਰ cw_pause_ctx *ctx = (ਬਣਤਰ cw_pause_ctx *)cw_pause;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  ਜਦੋਂ(ctx->buf && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਬਣਤਰ cw_pause_buf *ਪਹਿਲਾ = ctx->buf;
    size_t blen, wlen = 0;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf = ਨਿਰਾ;

    ਜੇ(Curl_bufq_peek(&ਪਹਿਲਾ->b, &buf, &blen)) {
      ਪੂਰਨ wtype = ਪਹਿਲਾ->type;
      wlen = (ਪਹਿਲਾ->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ਜੇ((wlen < Curl_bufq_len(&ਪਹਿਲਾ->b)) || ਪਹਿਲਾ->ਅਗਲਾ)
        wtype &= ~CLIENTWRITE_EOS;
      ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, cw_pause->ਅਗਲਾ, wtype,
                                  (ਅਚਲ ਅੱਖਰ *)buf, wlen);
      CURL_TRC_WRITE(ਅੰਕੜੇ, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_ਕੁੱਲ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)wtype,
                     (ਪੂਰਨ)ਨਤੀਜਾ);
      Curl_bufq_skip(&ਪਹਿਲਾ->b, wlen);
      DEBUGASSERT(ctx->buf_ਕੁੱਲ >= wlen);
      ctx->buf_ਕੁੱਲ -= wlen;
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਪਹਿਲਾ->type & CLIENTWRITE_EOS) {
      ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, cw_pause->ਅਗਲਾ, ਪਹਿਲਾ->type,
                                  (ਅਚਲ ਅੱਖਰ *)buf, 0);
      CURL_TRC_WRITE(ਅੰਕੜੇ, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_ਕੁੱਲ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਪਹਿਲਾ->type,
                     (ਪੂਰਨ)ਨਤੀਜਾ);
    }

    ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਪਹਿਲਾ->b)) {
      ctx->buf = ਪਹਿਲਾ->ਅਗਲਾ;
      ਜੇ(!ctx->buf)
        ctx->ਪੂਛ = ਨਿਰਾ;
      cw_pause_buf_free(ਪਹਿਲਾ);
    }
  }

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cwriter_flush(ਅੰਕੜੇ, cw_pause->ਅਗਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cw_pause_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                               ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ cw_pause_ctx *ctx = writer->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t wlen = 0;

  ਜੇ(ctx->buf && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = cw_pause_flush(ਅੰਕੜੇ, writer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜਦੋਂ(!ctx->buf && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਪੂਰਨ wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ਜੇ(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, wtype, buf, wlen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    buf += wlen;
    blen -= wlen;
    ਜੇ(!blen)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਕਰੋ {
    size_t nwritten = 0;
    ਜੇ(ctx->ਪੂਛ && (ctx->ਪੂਛ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->ਪੂਛ->b, buf, blen, &nwritten);
    }
    ਨਹੀਂਤਾਂ {
      /* Need a new buf, type changed */
      ਬਣਤਰ cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ਜੇ(!cwbuf)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜੇ(ctx->ਪੂਛ)
        ctx->ਪੂਛ->ਅਗਲਾ = cwbuf;
      ਨਹੀਂਤਾਂ
        ctx->buf = cwbuf;
      ctx->ਪੂਛ = cwbuf;
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->ਪੂਛ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(ਅੰਕੜੇ, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type,
                   ctx->buf_ਕੁੱਲ + wlen, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_ਕੁੱਲ += nwritten;
  } ਜਦੋਂ(blen);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ ਬਣਤਰ Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ਨਿਰਾ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  ਆਕਾਰ(ਬਣਤਰ cw_pause_ctx)
};
