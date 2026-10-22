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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "multiif.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "cw-out.h"
#ضمن "cw-pause.h"
#ضمن "progress.h"

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
تعريفنوع تعداد {
  CW_باہرلا_NONE,
  CW_باہرلا_BODY,
  CW_باہرلا_BODY_0_LEN,
  CW_باہرلا_HDS
} cw_باہرلا_type;

بنية cw_باہرلا_buf {
  بنية cw_باہرلا_buf *اگلا;
  بنية dynbuf b;
  cw_باہرلا_type type;
};

ساكن بنية cw_باہرلا_buf *cw_باہرلا_buf_create(cw_باہرلا_type otype)
{
  بنية cw_باہرلا_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  إذا(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_بفر);
  }
  أرجع cwbuf;
}

ساكن فراغ cw_باہرلا_buf_free(بنية cw_باہرلا_buf *cwbuf)
{
  إذا(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_باہرلا_ctx {
  بنية Curl_cwriter super;
  بنية cw_باہرلا_buf *buf;
  بنية cw_باہرلا_buf *پوچھ;
  size_t buffered_len;
  ثنائی_ہندسہ(errored);
};

ساكن CURLcode cw_باہرلا_init(بنية Curl_easy *انکڑے,
                            بنية Curl_cwriter *writer)
{
  بنية cw_باہرلا_ctx *ctx = writer->ctx;
  (فراغ)انکڑے;
  ctx->buf = عدم;
  ctx->پوچھ = عدم;
  ctx->buffered_len = 0;
  أرجع CURLE_OK;
}

ساكن فراغ cw_باہرلا_bufs_free(بنية cw_باہرلا_ctx *ctx)
{
  طالما(ctx->buf) {
    بنية cw_باہرلا_buf *اگلا = ctx->buf->اگلا;
    cw_باہرلا_buf_free(ctx->buf);
    ctx->buf = اگلا;
  }
  ctx->پوچھ = عدم;
  ctx->buffered_len = 0;
}

ساكن فراغ cw_باہرلا_close(بنية Curl_easy *انکڑے, بنية Curl_cwriter *writer)
{
  بنية cw_باہرلا_ctx *ctx = writer->ctx;

  (فراغ)انکڑے;
  cw_باہرلا_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساكن فراغ cw_get_writefunc(بنية Curl_easy *انکڑے, cw_باہرلا_type otype,
                             curl_write_callback *pwcb, فراغ **pwcb_انکڑے,
                             size_t *pmax_write, size_t *pmin_write)
{
  بدل(otype) {
  حالة CW_باہرلا_BODY:
  حالة CW_باہرلا_BODY_0_LEN:
    إذا(Curl_ftp_listmode(انکڑے)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_انکڑے = انکڑے;
    }
    وإلا {
      *pwcb = انکڑے->سیٹ.fwrite_func;
      *pwcb_انکڑے = انکڑے->سیٹ.باہرلا;
    }
    *pmax_write = CURL_MAX_WRITE_ناپ;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    اكسر;
  حالة CW_باہرلا_HDS:
    *pwcb = انکڑے->سیٹ.fwrite_سرخی ? انکڑے->سیٹ.fwrite_سرخی :
             (انکڑے->سیٹ.writeheader ? انکڑے->سیٹ.fwrite_func : عدم);
    *pwcb_انکڑے = انکڑے->سیٹ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    اكسر;
  افتراضي:
    *pwcb = عدم;
    *pwcb_انکڑے = عدم;
    *pmax_write = CURL_MAX_WRITE_ناپ;
    *pmin_write = 0;
  }
}

ساكن CURLcode cw_باہرلا_cb_write(بنية Curl_easy *انکڑے,
                                curl_write_callback wcb,
                                فراغ *wcb_انکڑے,
                                cw_باہرلا_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode نتیجہ;

  NOVERBOSE((فراغ)otype);

  DEBUGASSERT(انکڑے->conn);
  *pnwritten = 0;
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, انکڑے, easy_cw_باہرلا_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_انکڑے);
    CURL_CBAPI_سرا(&guard);
  }
  CURL_TRC_WRITE(انکڑے, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_باہرلا_HDS) ? "header" : "body",
                 (بنانشانی صحيح)otype, nwritten);
  إذا(nwritten == CURL_WRITEFUNC_PAUSE) {
    إذا(انکڑے->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(انکڑے, "Write callback asked for PAUSE when not supported");
      أرجع CURLE_WRITE_ERROR;
    }
    انکڑے->req.writer.paused = TRUE;
    CURL_TRC_WRITE(انکڑے, "[OUT] PAUSE requested by client");
    نتیجہ = Curl_xfer_pause_recv(انکڑے, TRUE);
    أرجع نتیجہ ? نتیجہ : CURLE_AGAIN;
  }
  وإلا إذا(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(انکڑے, "client returned ERROR on write of %zu bytes", blen);
    أرجع CURLE_WRITE_ERROR;
  }
  وإلا إذا(nwritten != blen) {
    failf(انکڑے, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    أرجع CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  أرجع CURLE_OK;
}

ساكن CURLcode cw_باہرلا_ptr_flush(بنية cw_باہرلا_ctx *ctx,
                                 بنية Curl_easy *انکڑے,
                                 cw_باہرلا_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = عدم;
  فراغ *wcb_انکڑے;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode نتیجہ = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(انکڑے, otype, &wcb, &wcb_انکڑے, &max_write, &min_write);
  إذا(!wcb) {
    *pconsumed = blen;
    أرجع CURLE_OK;
  }

  *pconsumed = 0;
  إذا(otype == CW_باہرلا_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    أرجع cw_باہرلا_cb_write(انکڑے, wcb, wcb_انکڑے, otype,
                           buf, blen, &nwritten);
  }
  وإلا {
    طالما(blen && !انکڑے->req.writer.paused) {
      إذا(!flush_all && blen < min_write)
        اكسر;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      إذا(otype == CW_باہرلا_BODY)
        نتیجہ = Curl_pgrs_deliver_check(انکڑے, wlen);
      إذا(!نتیجہ)
        نتیجہ = cw_باہرلا_cb_write(انکڑے, wcb, wcb_انکڑے, otype,
                                 buf, wlen, &nwritten);
      إذا(نتیجہ)
        أرجع نتیجہ;
      إذا(otype == CW_باہرلا_BODY)
        Curl_pgrs_deliver_inc(انکڑے, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(انکڑے, otype, &wcb, &wcb_انکڑے, &max_write, &min_write);
      إذا(!wcb) {
        *pconsumed += blen;
        اكسر;
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_باہرلا_buf_flush(بنية cw_باہرلا_ctx *ctx,
                                 بنية Curl_easy *انکڑے,
                                 بنية cw_باہرلا_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_باہرلا_BODY_0_LEN)) {
    size_t consumed;

    نتیجہ = cw_باہرلا_ptr_flush(ctx, انکڑے, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
      أرجع نتیجہ;
    نتیجہ = CURLE_OK;

    إذا(consumed) {
      إذا(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      وإلا {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        نتیجہ = curlx_dyn_پوچھ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode cw_باہرلا_flush_chain(بنية cw_باہرلا_ctx *ctx,
                                   بنية Curl_easy *انکڑے,
                                   bool flush_all)
{
  CURLcode نتیجہ;

  /* write the chain oldest first until it blocks or gets empty */
  طالما(ctx->buf) {
    بنية cw_باہرلا_buf *cwbuf = ctx->buf;
    size_t blen_before;

    إذا(انکڑے->req.writer.paused)
      أرجع CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    نتیجہ = cw_باہرلا_buf_flush(ctx, انکڑے, cwbuf, flush_all);
    إذا(نتیجہ)
      أرجع نتیجہ;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    إذا(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(انکڑے->req.writer.paused);
      أرجع CURLE_OK;
    }

    ctx->buf = cwbuf->اگلا;
    إذا(!ctx->buf)
      ctx->پوچھ = عدم;
    cw_باہرلا_buf_free(cwbuf);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_باہرلا_append(بنية cw_باہرلا_ctx *ctx,
                              بنية Curl_easy *انکڑے,
                              cw_باہرلا_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ;

  CURL_TRC_WRITE(انکڑے, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_بفر);
  إذا(ctx->buffered_len + blen > DYN_PAUSE_بفر) {
    failf(انکڑے, "pause buffer not large enough -> CURLE_TOO_LARGE");
    أرجع CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  إذا(!ctx->پوچھ || (ctx->پوچھ->type != otype) || (otype == CW_باہرلا_HDS)) {
    بنية cw_باہرلا_buf *cwbuf = cw_باہرلا_buf_create(otype);
    إذا(!cwbuf)
      أرجع CURLE_باہرلا_دا_حافظہ;
    إذا(ctx->پوچھ)
      ctx->پوچھ->اگلا = cwbuf;
    وإلا
      ctx->buf = cwbuf;
    ctx->پوچھ = cwbuf;
  }
  DEBUGASSERT(ctx->پوچھ && (ctx->پوچھ->type == otype));
  نتیجہ = curlx_dyn_addn(&ctx->پوچھ->b, buf, blen);
  إذا(!نتیجہ)
    ctx->buffered_len += blen;
  أرجع نتیجہ;
}

ساكن CURLcode cw_باہرلا_do_write(بنية cw_باہرلا_ctx *ctx,
                                بنية Curl_easy *انکڑے,
                                cw_باہرلا_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  إذا(ctx->پوچھ && ctx->پوچھ->type != otype) {
    نتیجہ = cw_باہرلا_flush_chain(ctx, انکڑے, TRUE);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(ctx->buf) {
    /* still have buffered data, append and flush */
    نتیجہ = cw_باہرلا_append(ctx, انکڑے, otype, buf, blen);
    إذا(نتیجہ)
      اذهب باہرلا;
    نتیجہ = cw_باہرلا_flush_chain(ctx, انکڑے, flush_all);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا {
    /* nothing buffered, try direct write */
    size_t consumed;
    نتیجہ = cw_باہرلا_ptr_flush(ctx, انکڑے, otype, flush_all,
                              buf, blen, &consumed);
    إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
      أرجع نتیجہ;
    نتیجہ = CURLE_OK;
    إذا(consumed < blen) {
      /* did not write all, append the rest */
      نتیجہ = cw_باہرلا_append(ctx, انکڑے, otype,
                             buf + consumed, blen - consumed);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }

باہرلا:
  إذا(نتیجہ) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_باہرلا_bufs_free(ctx);
  }
  أرجع نتیجہ;
}

ساكن CURLcode cw_باہرلا_write(بنية Curl_easy *انکڑے,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t blen)
{
  بنية cw_باہرلا_ctx *ctx = writer->ctx;
  CURLcode نتیجہ;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  إذا((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_سرخی) && انکڑے->سیٹ.include_سرخی)) {
    cw_باہرلا_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_باہرلا_BODY_0_LEN : CW_باہرلا_BODY;
    نتیجہ = cw_باہرلا_do_write(ctx, انکڑے, otype, flush_all, buf, blen);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(type & (CLIENTWRITE_سرخی | CLIENTWRITE_INFO)) {
    نتیجہ = cw_باہرلا_do_write(ctx, انکڑے, CW_باہرلا_HDS, flush_all, buf, blen);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cw_باہرلا_do_flush(بنية Curl_easy *انکڑے,
                                بنية Curl_cwriter *cw_باہرلا,
                                bool flush_all)
{
  بنية cw_باہرلا_ctx *ctx = (بنية cw_باہرلا_ctx *)cw_باہرلا;

  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  إذا(!انکڑے->req.writer.paused && ctx->buf) {
    CURLcode نتیجہ;

    CURL_TRC_WRITE(انکڑے, "[OUT] flush");
    نتیجہ = cw_باہرلا_flush_chain(ctx, انکڑے, flush_all);
    إذا(نتیجہ) {
      ctx->errored = TRUE;
      cw_باہرلا_bufs_free(ctx);
      أرجع نتیجہ;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_باہرلا_flush(بنية Curl_easy *انکڑے,
                             بنية Curl_cwriter *writer)
{
  أرجع cw_باہرلا_do_flush(انکڑے, writer, FALSE);
}

ثابت بنية Curl_cwtype Curl_cwt_باہرلا = {
  "cw-out",
  عدم,
  0,
  cw_باہرلا_init,
  cw_باہرلا_write,
  cw_باہرلا_flush,
  cw_باہرلا_close,
  حجم(بنية cw_باہرلا_ctx)
};

CURLcode Curl_cw_باہرلا_پورا(بنية Curl_easy *انکڑے)
{
  بنية Curl_cwriter *cw_باہرلا;
  CURLcode نتیجہ = CURLE_OK;

  cw_باہرلا = Curl_cwriter_get_by_type(انکڑے, &Curl_cwt_باہرلا);
  إذا(cw_باہرلا) {
    CURL_TRC_WRITE(انکڑے, "[OUT] done");
    نتیجہ = Curl_client_flush(انکڑے);
    إذا(!نتیجہ)
      نتیجہ = cw_باہرلا_do_flush(انکڑے, cw_باہرلا, TRUE);
  }
  أرجع نتیجہ;
}
