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
#شامل "cfilters.h"
#شامل "multiif.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "transfer.h"
#شامل "cw-out.h"
#شامل "cw-pause.h"
#شامل "progress.h"

/**
 * OVERALL DESIGN of this client writer
 *
 * The 'cw-out' writer is supposed to be the last writer in a transfer's
 * stack. It is always added when that stack is initialized. Its purpose
 * is to pass BODY and HEADER bytes to the client-installed callback
 * functions.
 *
 * These callback may return `CURL_WRITEFUNC_PAUSE` to indicate that the
 * data had not been written and the whole transfer should stop receiving
 * new data. Or at least, stop calling the functions. When the transfer
 * is "unpaused" by the client, the previous data shall be passed as
 * if nothing happened.
 *
 * The `cw-out` writer therefore manages buffers for bytes that could
 * not be written. Data that was already in flight from the server also
 * needs buffering on paused transfer when it arrives.
 *
 * In addition, the writer allows buffering of "small" body writes,
 * so client functions are called less often. That is only enabled on a
 * number of conditions.
 *
 * HEADER and BODY data may arrive in any order. For paused transfers,
 * a list of `struct cw_out_buf` is kept for `cw_out_type` types. The
 * list may be: [BODY]->[HEADER]->[BODY]->[HEADER]....
 * When unpausing, this list is "played back" to the client callbacks.
 *
 * The amount of bytes being buffered is limited by `DYN_PAUSE_BUFFER`
 * and when that is exceeded `CURLE_TOO_LARGE` is returned as error.
 */
قسمتعریف شمار {
  CW_برآمد_NONE,
  CW_برآمد_BODY,
  CW_برآمد_BODY_0_LEN,
  CW_برآمد_HDS
} cw_برآمد_type;

ساخت cw_برآمد_buf {
  ساخت cw_برآمد_buf *اگلا;
  ساخت dynbuf b;
  cw_برآمد_type type;
};

ساکن ساخت cw_برآمد_buf *cw_برآمد_buf_create(cw_برآمد_type otype)
{
  ساخت cw_برآمد_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  اگر(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_بفر);
  }
  واپس cwbuf;
}

ساکن خالی cw_برآمد_buf_free(ساخت cw_برآمد_buf *cwbuf)
{
  اگر(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ساخت cw_برآمد_ctx {
  ساخت Curl_cwriter super;
  ساخت cw_برآمد_buf *buf;
  ساخت cw_برآمد_buf *دم;
  size_t buffered_len;
  ثنائی_ہندسہ(errored);
};

ساکن CURLcode cw_برآمد_init(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_cwriter *writer)
{
  ساخت cw_برآمد_ctx *ctx = writer->ctx;
  (خالی)ڈیٹا;
  ctx->buf = تہی;
  ctx->دم = تہی;
  ctx->buffered_len = 0;
  واپس CURLE_OK;
}

ساکن خالی cw_برآمد_bufs_free(ساخت cw_برآمد_ctx *ctx)
{
  جب(ctx->buf) {
    ساخت cw_برآمد_buf *اگلا = ctx->buf->اگلا;
    cw_برآمد_buf_free(ctx->buf);
    ctx->buf = اگلا;
  }
  ctx->دم = تہی;
  ctx->buffered_len = 0;
}

ساکن خالی cw_برآمد_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_cwriter *writer)
{
  ساخت cw_برآمد_ctx *ctx = writer->ctx;

  (خالی)ڈیٹا;
  cw_برآمد_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساکن خالی cw_get_writefunc(ساخت Curl_easy *ڈیٹا, cw_برآمد_type otype,
                             curl_write_callback *pwcb, خالی **pwcb_ڈیٹا,
                             size_t *pmax_write, size_t *pmin_write)
{
  انتخاب(otype) {
  حالت CW_برآمد_BODY:
  حالت CW_برآمد_BODY_0_LEN:
    اگر(Curl_ftp_listmode(ڈیٹا)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_ڈیٹا = ڈیٹا;
    }
    ورنہ {
      *pwcb = ڈیٹا->سیٹ.fwrite_func;
      *pwcb_ڈیٹا = ڈیٹا->سیٹ.برآمد;
    }
    *pmax_write = CURL_MAX_WRITE_سائز;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    توڑو;
  حالت CW_برآمد_HDS:
    *pwcb = ڈیٹا->سیٹ.fwrite_سرخی ? ڈیٹا->سیٹ.fwrite_سرخی :
             (ڈیٹا->سیٹ.writeheader ? ڈیٹا->سیٹ.fwrite_func : تہی);
    *pwcb_ڈیٹا = ڈیٹا->سیٹ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    توڑو;
  طےشدہ:
    *pwcb = تہی;
    *pwcb_ڈیٹا = تہی;
    *pmax_write = CURL_MAX_WRITE_سائز;
    *pmin_write = 0;
  }
}

ساکن CURLcode cw_برآمد_cb_write(ساخت Curl_easy *ڈیٹا,
                                curl_write_callback wcb,
                                خالی *wcb_ڈیٹا,
                                cw_برآمد_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode نتیجہ;

  NOVERBOSE((خالی)otype);

  DEBUGASSERT(ڈیٹا->conn);
  *pnwritten = 0;
  {
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_cw_برآمد_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_ڈیٹا);
    CURL_CBAPI_انتہا(&guard);
  }
  CURL_TRC_WRITE(ڈیٹا, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_برآمد_HDS) ? "header" : "body",
                 (غیرعلامتی صحیح)otype, nwritten);
  اگر(nwritten == CURL_WRITEFUNC_PAUSE) {
    اگر(ڈیٹا->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(ڈیٹا, "Write callback asked for PAUSE when not supported");
      واپس CURLE_WRITE_ERROR;
    }
    ڈیٹا->req.writer.paused = TRUE;
    CURL_TRC_WRITE(ڈیٹا, "[OUT] PAUSE requested by client");
    نتیجہ = Curl_xfer_pause_recv(ڈیٹا, TRUE);
    واپس نتیجہ ? نتیجہ : CURLE_AGAIN;
  }
  ورنہ اگر(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(ڈیٹا, "client returned ERROR on write of %zu bytes", blen);
    واپس CURLE_WRITE_ERROR;
  }
  ورنہ اگر(nwritten != blen) {
    failf(ڈیٹا, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    واپس CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  واپس CURLE_OK;
}

ساکن CURLcode cw_برآمد_ptr_flush(ساخت cw_برآمد_ctx *ctx,
                                 ساخت Curl_easy *ڈیٹا,
                                 cw_برآمد_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = تہی;
  خالی *wcb_ڈیٹا;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode نتیجہ = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  اگر(ctx->errored)
    واپس CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(ڈیٹا, otype, &wcb, &wcb_ڈیٹا, &max_write, &min_write);
  اگر(!wcb) {
    *pconsumed = blen;
    واپس CURLE_OK;
  }

  *pconsumed = 0;
  اگر(otype == CW_برآمد_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    واپس cw_برآمد_cb_write(ڈیٹا, wcb, wcb_ڈیٹا, otype,
                           buf, blen, &nwritten);
  }
  ورنہ {
    جب(blen && !ڈیٹا->req.writer.paused) {
      اگر(!flush_all && blen < min_write)
        توڑو;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      اگر(otype == CW_برآمد_BODY)
        نتیجہ = Curl_pgrs_deliver_check(ڈیٹا, wlen);
      اگر(!نتیجہ)
        نتیجہ = cw_برآمد_cb_write(ڈیٹا, wcb, wcb_ڈیٹا, otype,
                                 buf, wlen, &nwritten);
      اگر(نتیجہ)
        واپس نتیجہ;
      اگر(otype == CW_برآمد_BODY)
        Curl_pgrs_deliver_inc(ڈیٹا, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(ڈیٹا, otype, &wcb, &wcb_ڈیٹا, &max_write, &min_write);
      اگر(!wcb) {
        *pconsumed += blen;
        توڑو;
      }
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode cw_برآمد_buf_flush(ساخت cw_برآمد_ctx *ctx,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cw_برآمد_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_برآمد_BODY_0_LEN)) {
    size_t consumed;

    نتیجہ = cw_برآمد_ptr_flush(ctx, ڈیٹا, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
      واپس نتیجہ;
    نتیجہ = CURLE_OK;

    اگر(consumed) {
      اگر(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      ورنہ {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        نتیجہ = curlx_dyn_دم(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode cw_برآمد_flush_chain(ساخت cw_برآمد_ctx *ctx,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool flush_all)
{
  CURLcode نتیجہ;

  /* write the chain oldest first until it blocks or gets empty */
  جب(ctx->buf) {
    ساخت cw_برآمد_buf *cwbuf = ctx->buf;
    size_t blen_before;

    اگر(ڈیٹا->req.writer.paused)
      واپس CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    نتیجہ = cw_برآمد_buf_flush(ctx, ڈیٹا, cwbuf, flush_all);
    اگر(نتیجہ)
      واپس نتیجہ;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    اگر(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(ڈیٹا->req.writer.paused);
      واپس CURLE_OK;
    }

    ctx->buf = cwbuf->اگلا;
    اگر(!ctx->buf)
      ctx->دم = تہی;
    cw_برآمد_buf_free(cwbuf);
  }
  واپس CURLE_OK;
}

ساکن CURLcode cw_برآمد_append(ساخت cw_برآمد_ctx *ctx,
                              ساخت Curl_easy *ڈیٹا,
                              cw_برآمد_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ;

  CURL_TRC_WRITE(ڈیٹا, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_بفر);
  اگر(ctx->buffered_len + blen > DYN_PAUSE_بفر) {
    failf(ڈیٹا, "pause buffer not large enough -> CURLE_TOO_LARGE");
    واپس CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  اگر(!ctx->دم || (ctx->دم->type != otype) || (otype == CW_برآمد_HDS)) {
    ساخت cw_برآمد_buf *cwbuf = cw_برآمد_buf_create(otype);
    اگر(!cwbuf)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(ctx->دم)
      ctx->دم->اگلا = cwbuf;
    ورنہ
      ctx->buf = cwbuf;
    ctx->دم = cwbuf;
  }
  DEBUGASSERT(ctx->دم && (ctx->دم->type == otype));
  نتیجہ = curlx_dyn_addn(&ctx->دم->b, buf, blen);
  اگر(!نتیجہ)
    ctx->buffered_len += blen;
  واپس نتیجہ;
}

ساکن CURLcode cw_برآمد_do_write(ساخت cw_برآمد_ctx *ctx,
                                ساخت Curl_easy *ڈیٹا,
                                cw_برآمد_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  اگر(ctx->دم && ctx->دم->type != otype) {
    نتیجہ = cw_برآمد_flush_chain(ctx, ڈیٹا, TRUE);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ctx->buf) {
    /* still have buffered data, append and flush */
    نتیجہ = cw_برآمد_append(ctx, ڈیٹا, otype, buf, blen);
    اگر(نتیجہ)
      جاؤ برآمد;
    نتیجہ = cw_برآمد_flush_chain(ctx, ڈیٹا, flush_all);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ {
    /* nothing buffered, try direct write */
    size_t consumed;
    نتیجہ = cw_برآمد_ptr_flush(ctx, ڈیٹا, otype, flush_all,
                              buf, blen, &consumed);
    اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
      واپس نتیجہ;
    نتیجہ = CURLE_OK;
    اگر(consumed < blen) {
      /* did not write all, append the rest */
      نتیجہ = cw_برآمد_append(ctx, ڈیٹا, otype,
                             buf + consumed, blen - consumed);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }

برآمد:
  اگر(نتیجہ) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_برآمد_bufs_free(ctx);
  }
  واپس نتیجہ;
}

ساکن CURLcode cw_برآمد_write(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer, صحیح type,
                             ثابت حرف *buf, size_t blen)
{
  ساخت cw_برآمد_ctx *ctx = writer->ctx;
  CURLcode نتیجہ;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  اگر((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_سرخی) && ڈیٹا->سیٹ.include_سرخی)) {
    cw_برآمد_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_برآمد_BODY_0_LEN : CW_برآمد_BODY;
    نتیجہ = cw_برآمد_do_write(ctx, ڈیٹا, otype, flush_all, buf, blen);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(type & (CLIENTWRITE_سرخی | CLIENTWRITE_INFO)) {
    نتیجہ = cw_برآمد_do_write(ctx, ڈیٹا, CW_برآمد_HDS, flush_all, buf, blen);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس CURLE_OK;
}

ساکن CURLcode cw_برآمد_do_flush(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *cw_برآمد,
                                bool flush_all)
{
  ساخت cw_برآمد_ctx *ctx = (ساخت cw_برآمد_ctx *)cw_برآمد;

  اگر(ctx->errored)
    واپس CURLE_WRITE_ERROR;

  اگر(!ڈیٹا->req.writer.paused && ctx->buf) {
    CURLcode نتیجہ;

    CURL_TRC_WRITE(ڈیٹا, "[OUT] flush");
    نتیجہ = cw_برآمد_flush_chain(ctx, ڈیٹا, flush_all);
    اگر(نتیجہ) {
      ctx->errored = TRUE;
      cw_برآمد_bufs_free(ctx);
      واپس نتیجہ;
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode cw_برآمد_flush(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer)
{
  واپس cw_برآمد_do_flush(ڈیٹا, writer, FALSE);
}

ثابت ساخت Curl_cwtype Curl_cwt_برآمد = {
  "cw-out",
  تہی,
  0,
  cw_برآمد_init,
  cw_برآمد_write,
  cw_برآمد_flush,
  cw_برآمد_close,
  حجم(ساخت cw_برآمد_ctx)
};

CURLcode Curl_cw_برآمد_مکمل(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cwriter *cw_برآمد;
  CURLcode نتیجہ = CURLE_OK;

  cw_برآمد = Curl_cwriter_get_by_type(ڈیٹا, &Curl_cwt_برآمد);
  اگر(cw_برآمد) {
    CURL_TRC_WRITE(ڈیٹا, "[OUT] done");
    نتیجہ = Curl_client_flush(ڈیٹا);
    اگر(!نتیجہ)
      نتیجہ = cw_برآمد_do_flush(ڈیٹا, cw_برآمد, TRUE);
  }
  واپس نتیجہ;
}
