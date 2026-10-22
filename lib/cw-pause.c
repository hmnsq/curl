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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

সংরচনা cw_pause_buf {
  সংরচনা cw_pause_buf *next;
  সংরচনা bufq b;
  পূর্ণাঙ্ক type;
};

স্থির সংরচনা cw_pause_buf *cw_pause_buf_create(পূর্ণাঙ্ক type, size_t buflen)
{
  সংরচনা cw_pause_buf *cwbuf = curlx_calloc(1, পরিমাণ(*cwbuf));
  যদি(cwbuf) {
    cwbuf->type = type;
    যদি(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    অন্যথা
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ঘূৰাই cwbuf;
}

স্থির শূন্য cw_pause_buf_free(সংরচনা cw_pause_buf *cwbuf)
{
  যদি(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

সংরচনা cw_pause_ctx {
  সংরচনা Curl_cwriter super;
  সংরচনা cw_pause_buf *buf;
  সংরচনা cw_pause_buf *tail;
  size_t buf_total;
};

স্থির CURLcode cw_pause_init(সংরচনা Curl_easy *data,
                              সংরচনা Curl_cwriter *writer)
{
  সংরচনা cw_pause_ctx *ctx = writer->ctx;
  (শূন্য)data;
  ctx->buf = রিক্ত;
  ctx->tail = রিক্ত;
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cw_pause_bufs_free(সংরচনা cw_pause_ctx *ctx)
{
  যেতিয়ালৈ(ctx->buf) {
    সংরচনা cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = রিক্ত;
}

স্থির শূন্য cw_pause_close(সংরচনা Curl_easy *data, সংরচনা Curl_cwriter *writer)
{
  সংরচনা cw_pause_ctx *ctx = writer->ctx;

  (শূন্য)data;
  cw_pause_bufs_free(ctx);
}

স্থির CURLcode cw_pause_flush(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *cw_pause)
{
  সংরচনা cw_pause_ctx *ctx = (সংরচনা cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  যেতিয়ালৈ(ctx->buf && !Curl_cwriter_is_paused(data)) {
    সংরচনা cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ধ্রুৱ unsigned ৱর্ণ *buf = রিক্ত;

    যদি(Curl_bufq_peek(&first->b, &buf, &blen)) {
      পূর্ণাঙ্ক wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      যদি((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (ধ্রুৱ ৱর্ণ *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned পূর্ণাঙ্ক)wtype,
                     (পূর্ণাঙ্ক)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      যদি(result)
        ঘূৰাই result;
    }
    অন্যথা যদি(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (ধ্রুৱ ৱর্ণ *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned পূর্ণাঙ্ক)first->type,
                     (পূর্ণাঙ্ক)result);
    }

    যদি(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      যদি(!ctx->buf)
        ctx->tail = রিক্ত;
      cw_pause_buf_free(first);
    }
  }

  যদি(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  ঘূৰাই result;
}

স্থির CURLcode cw_pause_write(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                               ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  সংরচনা cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  যদি(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    যদি(result)
      ঘূৰাই result;
  }

  যেতিয়ালৈ(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    পূর্ণাঙ্ক wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    যদি(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    যদি(result)
      ঘূৰাই result;
    buf += wlen;
    blen -= wlen;
    যদি(!blen)
      ঘূৰাই result;
  }

  কৰা {
    size_t nwritten = 0;
    যদি(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    অন্যথা {
      /* Need a new buf, type changed */
      সংরচনা cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      যদি(!cwbuf)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
      যদি(ctx->tail)
        ctx->tail->next = cwbuf;
      অন্যথা
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned পূর্ণাঙ্ক)type,
                   ctx->buf_total + wlen, (পূর্ণাঙ্ক)result);
    যদি(result)
      ঘূৰাই result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } যেতিয়ালৈ(blen);

  ঘূৰাই result;
}

ধ্রুৱ সংরচনা Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  রিক্ত,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  পরিমাণ(সংরচনা cw_pause_ctx)
};
