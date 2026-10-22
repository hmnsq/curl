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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

கட்டமைப்பு cw_pause_buf {
  கட்டமைப்பு cw_pause_buf *next;
  கட்டமைப்பு bufq b;
  முழு type;
};

நிலையான கட்டமைப்பு cw_pause_buf *cw_pause_buf_create(முழு type, size_t buflen)
{
  கட்டமைப்பு cw_pause_buf *cwbuf = curlx_calloc(1, அளவு(*cwbuf));
  எனில்(cwbuf) {
    cwbuf->type = type;
    எனில்(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    இல்லையெனில்
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  திரும்பு cwbuf;
}

நிலையான வெறுமை cw_pause_buf_free(கட்டமைப்பு cw_pause_buf *cwbuf)
{
  எனில்(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

கட்டமைப்பு cw_pause_ctx {
  கட்டமைப்பு Curl_cwriter super;
  கட்டமைப்பு cw_pause_buf *buf;
  கட்டமைப்பு cw_pause_buf *tail;
  size_t buf_total;
};

நிலையான CURLcode cw_pause_init(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு cw_pause_ctx *ctx = writer->ctx;
  (வெறுமை)data;
  ctx->buf = வெற்று;
  ctx->tail = வெற்று;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cw_pause_bufs_free(கட்டமைப்பு cw_pause_ctx *ctx)
{
  போது(ctx->buf) {
    கட்டமைப்பு cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = வெற்று;
}

நிலையான வெறுமை cw_pause_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு cw_pause_ctx *ctx = writer->ctx;

  (வெறுமை)data;
  cw_pause_bufs_free(ctx);
}

நிலையான CURLcode cw_pause_flush(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *cw_pause)
{
  கட்டமைப்பு cw_pause_ctx *ctx = (கட்டமைப்பு cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  போது(ctx->buf && !Curl_cwriter_is_paused(data)) {
    கட்டமைப்பு cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    மாறிலி unsigned எழுத்து *buf = வெற்று;

    எனில்(Curl_bufq_peek(&first->b, &buf, &blen)) {
      முழு wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      எனில்((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (மாறிலி எழுத்து *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned முழு)wtype,
                     (முழு)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில் எனில்(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (மாறிலி எழுத்து *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned முழு)first->type,
                     (முழு)result);
    }

    எனில்(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      எனில்(!ctx->buf)
        ctx->tail = வெற்று;
      cw_pause_buf_free(first);
    }
  }

  எனில்(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  திரும்பு result;
}

நிலையான CURLcode cw_pause_write(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *writer, முழு type,
                               மாறிலி எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  எனில்(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    எனில்(result)
      திரும்பு result;
  }

  போது(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    முழு wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    எனில்(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    எனில்(result)
      திரும்பு result;
    buf += wlen;
    blen -= wlen;
    எனில்(!blen)
      திரும்பு result;
  }

  செய் {
    size_t nwritten = 0;
    எனில்(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    இல்லையெனில் {
      /* Need a new buf, type changed */
      கட்டமைப்பு cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      எனில்(!cwbuf)
        திரும்பு CURLE_OUT_OF_MEMORY;
      எனில்(ctx->tail)
        ctx->tail->next = cwbuf;
      இல்லையெனில்
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned முழு)type,
                   ctx->buf_total + wlen, (முழு)result);
    எனில்(result)
      திரும்பு result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } போது(blen);

  திரும்பு result;
}

மாறிலி கட்டமைப்பு Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  வெற்று,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  அளவு(கட்டமைப்பு cw_pause_ctx)
};
