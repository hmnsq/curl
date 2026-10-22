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
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

بنية cw_pause_buf {
  بنية cw_pause_buf *next;
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
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    وإلا
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  إرجاع cwbuf;
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
  بنية cw_pause_buf *tail;
  size_t buf_total;
};

ساكن CURLcode cw_pause_init(بنية Curl_easy *data,
                              بنية Curl_cwriter *writer)
{
  بنية cw_pause_ctx *ctx = writer->ctx;
  (فراغ)data;
  ctx->buf = عدم;
  ctx->tail = عدم;
  إرجاع CURLE_OK;
}

ساكن فراغ cw_pause_bufs_free(بنية cw_pause_ctx *ctx)
{
  بينما(ctx->buf) {
    بنية cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = عدم;
}

ساكن فراغ cw_pause_close(بنية Curl_easy *data, بنية Curl_cwriter *writer)
{
  بنية cw_pause_ctx *ctx = writer->ctx;

  (فراغ)data;
  cw_pause_bufs_free(ctx);
}

ساكن CURLcode cw_pause_flush(بنية Curl_easy *data,
                               بنية Curl_cwriter *cw_pause)
{
  بنية cw_pause_ctx *ctx = (بنية cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  بينما(ctx->buf && !Curl_cwriter_is_paused(data)) {
    بنية cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ثابت unsigned حرف *buf = عدم;

    إذا(Curl_bufq_peek(&first->b, &buf, &blen)) {
      صحيح wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      إذا((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (ثابت حرف *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned صحيح)wtype,
                     (صحيح)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      إذا(result)
        إرجاع result;
    }
    وإلا إذا(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (ثابت حرف *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned صحيح)first->type,
                     (صحيح)result);
    }

    إذا(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      إذا(!ctx->buf)
        ctx->tail = عدم;
      cw_pause_buf_free(first);
    }
  }

  إذا(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  إرجاع result;
}

ساكن CURLcode cw_pause_write(بنية Curl_easy *data,
                               بنية Curl_cwriter *writer, صحيح type,
                               ثابت حرف *buf, size_t blen)
{
  بنية cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  إذا(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    إذا(result)
      إرجاع result;
  }

  بينما(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    صحيح wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    إذا(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    إذا(result)
      إرجاع result;
    buf += wlen;
    blen -= wlen;
    إذا(!blen)
      إرجاع result;
  }

  افعل {
    size_t nwritten = 0;
    إذا(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    وإلا {
      /* Need a new buf, type changed */
      بنية cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      إذا(!cwbuf)
        إرجاع CURLE_OUT_OF_MEMORY;
      إذا(ctx->tail)
        ctx->tail->next = cwbuf;
      وإلا
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned صحيح)type,
                   ctx->buf_total + wlen, (صحيح)result);
    إذا(result)
      إرجاع result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } بينما(blen);

  إرجاع result;
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
