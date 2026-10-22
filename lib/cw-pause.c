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
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

ساختار cw_pause_buf {
  ساختار cw_pause_buf *next;
  ساختار bufq b;
  صحیح type;
};

ایستا ساختار cw_pause_buf *cw_pause_buf_create(صحیح type, size_t buflen)
{
  ساختار cw_pause_buf *cwbuf = curlx_calloc(1, اندازه(*cwbuf));
  اگر(cwbuf) {
    cwbuf->type = type;
    اگر(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    وگرنه
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  بازگشت cwbuf;
}

ایستا تهی cw_pause_buf_free(ساختار cw_pause_buf *cwbuf)
{
  اگر(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ساختار cw_pause_ctx {
  ساختار Curl_cwriter super;
  ساختار cw_pause_buf *buf;
  ساختار cw_pause_buf *tail;
  size_t buf_total;
};

ایستا CURLcode cw_pause_init(ساختار Curl_easy *data,
                              ساختار Curl_cwriter *writer)
{
  ساختار cw_pause_ctx *ctx = writer->ctx;
  (تهی)data;
  ctx->buf = پوچ;
  ctx->tail = پوچ;
  بازگشت CURLE_OK;
}

ایستا تهی cw_pause_bufs_free(ساختار cw_pause_ctx *ctx)
{
  وقتی(ctx->buf) {
    ساختار cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = پوچ;
}

ایستا تهی cw_pause_close(ساختار Curl_easy *data, ساختار Curl_cwriter *writer)
{
  ساختار cw_pause_ctx *ctx = writer->ctx;

  (تهی)data;
  cw_pause_bufs_free(ctx);
}

ایستا CURLcode cw_pause_flush(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *cw_pause)
{
  ساختار cw_pause_ctx *ctx = (ساختار cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  وقتی(ctx->buf && !Curl_cwriter_is_paused(data)) {
    ساختار cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ثابت unsigned نویسه *buf = پوچ;

    اگر(Curl_bufq_peek(&first->b, &buf, &blen)) {
      صحیح wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      اگر((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (ثابت نویسه *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned صحیح)wtype,
                     (صحیح)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      اگر(result)
        بازگشت result;
    }
    وگرنه اگر(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (ثابت نویسه *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned صحیح)first->type,
                     (صحیح)result);
    }

    اگر(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      اگر(!ctx->buf)
        ctx->tail = پوچ;
      cw_pause_buf_free(first);
    }
  }

  اگر(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  بازگشت result;
}

ایستا CURLcode cw_pause_write(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *writer, صحیح type,
                               ثابت نویسه *buf, size_t blen)
{
  ساختار cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  اگر(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    اگر(result)
      بازگشت result;
  }

  وقتی(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    صحیح wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    اگر(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    اگر(result)
      بازگشت result;
    buf += wlen;
    blen -= wlen;
    اگر(!blen)
      بازگشت result;
  }

  انجامبده {
    size_t nwritten = 0;
    اگر(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    وگرنه {
      /* Need a new buf, type changed */
      ساختار cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      اگر(!cwbuf)
        بازگشت CURLE_OUT_OF_MEMORY;
      اگر(ctx->tail)
        ctx->tail->next = cwbuf;
      وگرنه
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned صحیح)type,
                   ctx->buf_total + wlen, (صحیح)result);
    اگر(result)
      بازگشت result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } وقتی(blen);

  بازگشت result;
}

ثابت ساختار Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  پوچ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  اندازه(ساختار cw_pause_ctx)
};
