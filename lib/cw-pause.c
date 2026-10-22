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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

ഘടന cw_pause_buf {
  ഘടന cw_pause_buf *next;
  ഘടന bufq b;
  പൂർണ്ണസംഖ്യ type;
};

സ്ഥിതം ഘടന cw_pause_buf *cw_pause_buf_create(പൂർണ്ണസംഖ്യ type, size_t buflen)
{
  ഘടന cw_pause_buf *cwbuf = curlx_calloc(1, വലുപ്പം(*cwbuf));
  എങ്കിൽ(cwbuf) {
    cwbuf->type = type;
    എങ്കിൽ(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    അല്ലെങ്കിൽ
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  തിരികെ cwbuf;
}

സ്ഥിതം ശൂന്യം cw_pause_buf_free(ഘടന cw_pause_buf *cwbuf)
{
  എങ്കിൽ(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ഘടന cw_pause_ctx {
  ഘടന Curl_cwriter super;
  ഘടന cw_pause_buf *buf;
  ഘടന cw_pause_buf *tail;
  size_t buf_total;
};

സ്ഥിതം CURLcode cw_pause_init(ഘടന Curl_easy *data,
                              ഘടന Curl_cwriter *writer)
{
  ഘടന cw_pause_ctx *ctx = writer->ctx;
  (ശൂന്യം)data;
  ctx->buf = ശൂന്യ;
  ctx->tail = ശൂന്യ;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cw_pause_bufs_free(ഘടന cw_pause_ctx *ctx)
{
  ആകുമ്പോൾ(ctx->buf) {
    ഘടന cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ശൂന്യ;
}

സ്ഥിതം ശൂന്യം cw_pause_close(ഘടന Curl_easy *data, ഘടന Curl_cwriter *writer)
{
  ഘടന cw_pause_ctx *ctx = writer->ctx;

  (ശൂന്യം)data;
  cw_pause_bufs_free(ctx);
}

സ്ഥിതം CURLcode cw_pause_flush(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *cw_pause)
{
  ഘടന cw_pause_ctx *ctx = (ഘടന cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  ആകുമ്പോൾ(ctx->buf && !Curl_cwriter_is_paused(data)) {
    ഘടന cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    സ്ഥിരം unsigned അക്ഷരം *buf = ശൂന്യ;

    എങ്കിൽ(Curl_bufq_peek(&first->b, &buf, &blen)) {
      പൂർണ്ണസംഖ്യ wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      എങ്കിൽ((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (സ്ഥിരം അക്ഷരം *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (unsigned പൂർണ്ണസംഖ്യ)wtype,
                     (പൂർണ്ണസംഖ്യ)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (സ്ഥിരം അക്ഷരം *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (unsigned പൂർണ്ണസംഖ്യ)first->type,
                     (പൂർണ്ണസംഖ്യ)result);
    }

    എങ്കിൽ(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      എങ്കിൽ(!ctx->buf)
        ctx->tail = ശൂന്യ;
      cw_pause_buf_free(first);
    }
  }

  എങ്കിൽ(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  തിരികെ result;
}

സ്ഥിതം CURLcode cw_pause_write(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                               സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  ഘടന cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  size_t wlen = 0;

  എങ്കിൽ(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  ആകുമ്പോൾ(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    പൂർണ്ണസംഖ്യ wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    എങ്കിൽ(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    എങ്കിൽ(result)
      തിരികെ result;
    buf += wlen;
    blen -= wlen;
    എങ്കിൽ(!blen)
      തിരികെ result;
  }

  ചെയ്യുക {
    size_t nwritten = 0;
    എങ്കിൽ(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    അല്ലെങ്കിൽ {
      /* Need a new buf, type changed */
      ഘടന cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      എങ്കിൽ(!cwbuf)
        തിരികെ CURLE_OUT_OF_MEMORY;
      എങ്കിൽ(ctx->tail)
        ctx->tail->next = cwbuf;
      അല്ലെങ്കിൽ
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned പൂർണ്ണസംഖ്യ)type,
                   ctx->buf_total + wlen, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } ആകുമ്പോൾ(blen);

  തിരികെ result;
}

സ്ഥിരം ഘടന Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ശൂന്യ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  വലുപ്പം(ഘടന cw_pause_ctx)
};
