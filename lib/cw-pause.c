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
#include "curl_setup.h"

#include "urldata.h"
#include "bufq.h"
#include "cfilters.h"
#include "sendf.h"
#include "curl_trc.h"
#include "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

struct cw_pause_buf {
  struct cw_pause_buf *next;
  struct bufq b;
  цео type;
};

static struct cw_pause_buf *cw_pause_buf_create(цео type, size_t buflen)
{
  struct cw_pause_buf *cwbuf = curlx_calloc(1, sizeof(*cwbuf));
  ако(cwbuf) {
    cwbuf->type = type;
    ако(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    инако
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  врати cwbuf;
}

static void cw_pause_buf_free(struct cw_pause_buf *cwbuf)
{
  ако(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

struct cw_pause_ctx {
  struct Curl_cwriter super;
  struct cw_pause_buf *buf;
  struct cw_pause_buf *tail;
  size_t buf_total;
};

static CURLcode cw_pause_init(struct Curl_easy *data,
                              struct Curl_cwriter *writer)
{
  struct cw_pause_ctx *ctx = writer->ctx;
  (void)data;
  ctx->buf = NULL;
  ctx->tail = NULL;
  врати CURLE_OK;
}

static void cw_pause_bufs_free(struct cw_pause_ctx *ctx)
{
  док(ctx->buf) {
    struct cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = NULL;
}

static void cw_pause_close(struct Curl_easy *data, struct Curl_cwriter *writer)
{
  struct cw_pause_ctx *ctx = writer->ctx;

  (void)data;
  cw_pause_bufs_free(ctx);
}

static CURLcode cw_pause_flush(struct Curl_easy *data,
                               struct Curl_cwriter *cw_pause)
{
  struct cw_pause_ctx *ctx = (struct cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  док(ctx->buf && !Curl_cwriter_is_paused(data)) {
    struct cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    const unsigned char *buf = NULL;

    ако(Curl_bufq_peek(&first->b, &buf, &blen)) {
      цео wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ако((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (const char *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned цео)wtype,
                     (цео)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      ако(result)
        врати result;
    }
    инако ако(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (const char *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned цео)first->type,
                     (цео)result);
    }

    ако(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      ако(!ctx->buf)
        ctx->tail = NULL;
      cw_pause_buf_free(first);
    }
  }

  ако(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  врати result;
}

static CURLcode cw_pause_write(struct Curl_easy *data,
                               struct Curl_cwriter *writer, цео type,
                               const char *buf, size_t blen)
{
  struct cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  ако(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    ако(result)
      врати result;
  }

  док(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    цео wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ако(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    ако(result)
      врати result;
    buf += wlen;
    blen -= wlen;
    ако(!blen)
      врати result;
  }

  do {
    size_t nwritten = 0;
    ако(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    инако {
      /* Need a new buf, type changed */
      struct cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ако(!cwbuf)
        врати CURLE_OUT_OF_MEMORY;
      ако(ctx->tail)
        ctx->tail->next = cwbuf;
      инако
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned цео)type,
                   ctx->buf_total + wlen, (цео)result);
    ако(result)
      врати result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } док(blen);

  врати result;
}

const struct Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  NULL,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  sizeof(struct cw_pause_ctx)
};
