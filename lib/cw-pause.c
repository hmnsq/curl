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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

ସଂରଚନା cw_pause_buf {
  ସଂରଚନା cw_pause_buf *next;
  ସଂରଚନା bufq b;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
};

ସ୍ଥିର ସଂରଚନା cw_pause_buf *cw_pause_buf_create(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, size_t buflen)
{
  ସଂରଚନା cw_pause_buf *cwbuf = curlx_calloc(1, ଆକାର(*cwbuf));
  ଯଦି(cwbuf) {
    cwbuf->type = type;
    ଯଦି(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    ଅନ୍ୟଥା
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ଫେରନ୍ତୁ cwbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_pause_buf_free(ସଂରଚନା cw_pause_buf *cwbuf)
{
  ଯଦି(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ସଂରଚନା cw_pause_ctx {
  ସଂରଚନା Curl_cwriter super;
  ସଂରଚନା cw_pause_buf *buf;
  ସଂରଚନା cw_pause_buf *tail;
  size_t buf_total;
};

ସ୍ଥିର CURLcode cw_pause_init(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା cw_pause_ctx *ctx = writer->ctx;
  (ଶୂନ୍ୟ)data;
  ctx->buf = ଖାଲି;
  ctx->tail = ଖାଲି;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_pause_bufs_free(ସଂରଚନା cw_pause_ctx *ctx)
{
  ଯେତେବେଳେ(ctx->buf) {
    ସଂରଚନା cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ଖାଲି;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_pause_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା cw_pause_ctx *ctx = writer->ctx;

  (ଶୂନ୍ୟ)data;
  cw_pause_bufs_free(ctx);
}

ସ୍ଥିର CURLcode cw_pause_flush(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *cw_pause)
{
  ସଂରଚନା cw_pause_ctx *ctx = (ସଂରଚନା cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  ଯେତେବେଳେ(ctx->buf && !Curl_cwriter_is_paused(data)) {
    ସଂରଚନା cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ଅଚଳ unsigned ଅକ୍ଷର *buf = ଖାଲି;

    ଯଦି(Curl_bufq_peek(&first->b, &buf, &blen)) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ଯଦି((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (ଅଚଳ ଅକ୍ଷର *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wtype,
                     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା ଯଦି(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (ଅଚଳ ଅକ୍ଷର *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)first->type,
                     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    }

    ଯଦି(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      ଯଦି(!ctx->buf)
        ctx->tail = ଖାଲି;
      cw_pause_buf_free(first);
    }
  }

  ଯଦି(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cw_pause_write(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                               ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  ଯଦି(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯେତେବେଳେ(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ଯଦି(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    buf += wlen;
    blen -= wlen;
    ଯଦି(!blen)
      ଫେରନ୍ତୁ result;
  }

  କର {
    size_t nwritten = 0;
    ଯଦି(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    ଅନ୍ୟଥା {
      /* Need a new buf, type changed */
      ସଂରଚନା cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ଯଦି(!cwbuf)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଯଦି(ctx->tail)
        ctx->tail->next = cwbuf;
      ଅନ୍ୟଥା
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type,
                   ctx->buf_total + wlen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } ଯେତେବେଳେ(blen);

  ଫେରନ୍ତୁ result;
}

ଅଚଳ ସଂରଚନା Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ଖାଲି,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  ଆକାର(ସଂରଚନା cw_pause_ctx)
};
