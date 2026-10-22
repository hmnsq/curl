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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

સંરચના cw_pause_buf {
  સંરચના cw_pause_buf *next;
  સંરચના bufq b;
  પૂર્ણાંક type;
};

સ્થિર સંરચના cw_pause_buf *cw_pause_buf_create(પૂર્ણાંક type, size_t buflen)
{
  સંરચના cw_pause_buf *cwbuf = curlx_calloc(1, કદ(*cwbuf));
  જો(cwbuf) {
    cwbuf->type = type;
    જો(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    નહીંતર
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  પાછા cwbuf;
}

સ્થિર શૂન્ય cw_pause_buf_free(સંરચના cw_pause_buf *cwbuf)
{
  જો(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

સંરચના cw_pause_ctx {
  સંરચના Curl_cwriter super;
  સંરચના cw_pause_buf *buf;
  સંરચના cw_pause_buf *tail;
  size_t buf_total;
};

સ્થિર CURLcode cw_pause_init(સંરચના Curl_easy *data,
                              સંરચના Curl_cwriter *writer)
{
  સંરચના cw_pause_ctx *ctx = writer->ctx;
  (શૂન્ય)data;
  ctx->buf = ખાલી;
  ctx->tail = ખાલી;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cw_pause_bufs_free(સંરચના cw_pause_ctx *ctx)
{
  જ્યારે(ctx->buf) {
    સંરચના cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ખાલી;
}

સ્થિર શૂન્ય cw_pause_close(સંરચના Curl_easy *data, સંરચના Curl_cwriter *writer)
{
  સંરચના cw_pause_ctx *ctx = writer->ctx;

  (શૂન્ય)data;
  cw_pause_bufs_free(ctx);
}

સ્થિર CURLcode cw_pause_flush(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *cw_pause)
{
  સંરચના cw_pause_ctx *ctx = (સંરચના cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  જ્યારે(ctx->buf && !Curl_cwriter_is_paused(data)) {
    સંરચના cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    અચલ unsigned અક્ષર *buf = ખાલી;

    જો(Curl_bufq_peek(&first->b, &buf, &blen)) {
      પૂર્ણાંક wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      જો((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (અચલ અક્ષર *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned પૂર્ણાંક)wtype,
                     (પૂર્ણાંક)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      જો(result)
        પાછા result;
    }
    નહીંતર જો(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (અચલ અક્ષર *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned પૂર્ણાંક)first->type,
                     (પૂર્ણાંક)result);
    }

    જો(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      જો(!ctx->buf)
        ctx->tail = ખાલી;
      cw_pause_buf_free(first);
    }
  }

  જો(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  પાછા result;
}

સ્થિર CURLcode cw_pause_write(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                               અચલ અક્ષર *buf, size_t blen)
{
  સંરચના cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  જો(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    જો(result)
      પાછા result;
  }

  જ્યારે(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    પૂર્ણાંક wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    જો(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    જો(result)
      પાછા result;
    buf += wlen;
    blen -= wlen;
    જો(!blen)
      પાછા result;
  }

  કરો {
    size_t nwritten = 0;
    જો(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    નહીંતર {
      /* Need a new buf, type changed */
      સંરચના cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      જો(!cwbuf)
        પાછા CURLE_OUT_OF_MEMORY;
      જો(ctx->tail)
        ctx->tail->next = cwbuf;
      નહીંતર
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned પૂર્ણાંક)type,
                   ctx->buf_total + wlen, (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } જ્યારે(blen);

  પાછા result;
}

અચલ સંરચના Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ખાલી,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  કદ(સંરચના cw_pause_ctx)
};
