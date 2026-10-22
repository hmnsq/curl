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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

ಸಂರಚನಾ cw_pause_buf {
  ಸಂರಚನಾ cw_pause_buf *next;
  ಸಂರಚನಾ bufq b;
  ಪೂರ್ಣಾಙ್ಕ type;
};

ಸ್ಥಿರ ಸಂರಚನಾ cw_pause_buf *cw_pause_buf_create(ಪೂರ್ಣಾಙ್ಕ type, size_t buflen)
{
  ಸಂರಚನಾ cw_pause_buf *cwbuf = curlx_calloc(1, ಪರಿಮಾಣ(*cwbuf));
  ಯದಿ(cwbuf) {
    cwbuf->type = type;
    ಯದಿ(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    ಅನ್ಯಥಾ
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ಪ್ರತ್ಯಾಗಮ cwbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_buf_free(ಸಂರಚನಾ cw_pause_buf *cwbuf)
{
  ಯದಿ(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ಸಂರಚನಾ cw_pause_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
  ಸಂರಚನಾ cw_pause_buf *buf;
  ಸಂರಚನಾ cw_pause_buf *tail;
  size_t buf_total;
};

ಸ್ಥಿರ CURLcode cw_pause_init(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ cw_pause_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)data;
  ctx->buf = ರಿಕ್ತ;
  ctx->tail = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_bufs_free(ಸಂರಚನಾ cw_pause_ctx *ctx)
{
  ಯಾವತ್(ctx->buf) {
    ಸಂರಚನಾ cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ರಿಕ್ತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ cw_pause_ctx *ctx = writer->ctx;

  (ಶೂನ್ಯ)data;
  cw_pause_bufs_free(ctx);
}

ಸ್ಥಿರ CURLcode cw_pause_flush(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *cw_pause)
{
  ಸಂರಚನಾ cw_pause_ctx *ctx = (ಸಂರಚನಾ cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  ಯಾವತ್(ctx->buf && !Curl_cwriter_is_paused(data)) {
    ಸಂರಚನಾ cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ಧ್ರುವ unsigned ವರ್ಣ *buf = ರಿಕ್ತ;

    ಯದಿ(Curl_bufq_peek(&first->b, &buf, &blen)) {
      ಪೂರ್ಣಾಙ್ಕ wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ಯದಿ((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (ಧ್ರುವ ವರ್ಣ *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned ಪೂರ್ಣಾಙ್ಕ)wtype,
                     (ಪೂರ್ಣಾಙ್ಕ)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ ಯದಿ(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (ಧ್ರುವ ವರ್ಣ *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned ಪೂರ್ಣಾಙ್ಕ)first->type,
                     (ಪೂರ್ಣಾಙ್ಕ)result);
    }

    ಯದಿ(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      ಯದಿ(!ctx->buf)
        ctx->tail = ರಿಕ್ತ;
      cw_pause_buf_free(first);
    }
  }

  ಯದಿ(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cw_pause_write(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                               ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  ಯದಿ(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯಾವತ್(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    ಪೂರ್ಣಾಙ್ಕ wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ಯದಿ(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    buf += wlen;
    blen -= wlen;
    ಯದಿ(!blen)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಕುರು {
    size_t nwritten = 0;
    ಯದಿ(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    ಅನ್ಯಥಾ {
      /* Need a new buf, type changed */
      ಸಂರಚನಾ cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ಯದಿ(!cwbuf)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಯದಿ(ctx->tail)
        ctx->tail->next = cwbuf;
      ಅನ್ಯಥಾ
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned ಪೂರ್ಣಾಙ್ಕ)type,
                   ctx->buf_total + wlen, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } ಯಾವತ್(blen);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ರಿಕ್ತ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ cw_pause_ctx)
};
