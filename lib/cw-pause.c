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

sa_nracanaa cw_pause_buf {
  sa_nracanaa cw_pause_buf *next;
  sa_nracanaa bufq b;
  පූර්ණ_සංඛ්‍යා type;
};

static sa_nracanaa cw_pause_buf *cw_pause_buf_create(පූර්ණ_සංඛ්‍යා type, size_t buflen)
{
  sa_nracanaa cw_pause_buf *cwbuf = curlx_calloc(1, parimaa_na(*cwbuf));
  නම්(cwbuf) {
    cwbuf->type = type;
    නම්(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    නැතහොත්
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ආපසු cwbuf;
}

static void cw_pause_buf_free(sa_nracanaa cw_pause_buf *cwbuf)
{
  නම්(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

sa_nracanaa cw_pause_ctx {
  sa_nracanaa Curl_cwriter super;
  sa_nracanaa cw_pause_buf *buf;
  sa_nracanaa cw_pause_buf *tail;
  size_t buf_total;
};

static CURLcode cw_pause_init(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa cw_pause_ctx *ctx = writer->ctx;
  (void)data;
  ctx->buf = NULL;
  ctx->tail = NULL;
  ආපසු CURLE_OK;
}

static void cw_pause_bufs_free(sa_nracanaa cw_pause_ctx *ctx)
{
  අතරතුර(ctx->buf) {
    sa_nracanaa cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = NULL;
}

static void cw_pause_close(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa cw_pause_ctx *ctx = writer->ctx;

  (void)data;
  cw_pause_bufs_free(ctx);
}

static CURLcode cw_pause_flush(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *cw_pause)
{
  sa_nracanaa cw_pause_ctx *ctx = (sa_nracanaa cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  අතරතුර(ctx->buf && !Curl_cwriter_is_paused(data)) {
    sa_nracanaa cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    const unsigned var_na *buf = NULL;

    නම්(Curl_bufq_peek(&first->b, &buf, &blen)) {
      පූර්ණ_සංඛ්‍යා wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      නම්((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (const var_na *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned පූර්ණ_සංඛ්‍යා)wtype,
                     (පූර්ණ_සංඛ්‍යා)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      නම්(result)
        ආපසු result;
    }
    නැතහොත් නම්(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (const var_na *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned පූර්ණ_සංඛ්‍යා)first->type,
                     (පූර්ණ_සංඛ්‍යා)result);
    }

    නම්(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      නම්(!ctx->buf)
        ctx->tail = NULL;
      cw_pause_buf_free(first);
    }
  }

  නම්(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  ආපසු result;
}

static CURLcode cw_pause_write(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *writer, පූර්ණ_සංඛ්‍යා type,
                               const var_na *buf, size_t blen)
{
  sa_nracanaa cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  නම්(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    නම්(result)
      ආපසු result;
  }

  අතරතුර(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    පූර්ණ_සංඛ්‍යා wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    නම්(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    නම්(result)
      ආපසු result;
    buf += wlen;
    blen -= wlen;
    නම්(!blen)
      ආපසු result;
  }

  do {
    size_t nwritten = 0;
    නම්(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    නැතහොත් {
      /* Need a new buf, type changed */
      sa_nracanaa cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      නම්(!cwbuf)
        ආපසු CURLE_OUT_OF_MEMORY;
      නම්(ctx->tail)
        ctx->tail->next = cwbuf;
      නැතහොත්
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned පූර්ණ_සංඛ්‍යා)type,
                   ctx->buf_total + wlen, (පූර්ණ_සංඛ්‍යා)result);
    නම්(result)
      ආපසු result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } අතරතුර(blen);

  ආපසු result;
}

const sa_nracanaa Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  NULL,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  parimaa_na(sa_nracanaa cw_pause_ctx)
};
