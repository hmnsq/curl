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
  CW_بیرون_NONE,
  CW_بیرون_BODY,
  CW_بیرون_BODY_0_LEN,
  CW_بیرون_HDS
} cw_بیرون_type;

بنية cw_بیرون_buf {
  بنية cw_بیرون_buf *بعدی;
  بنية dynbuf b;
  cw_بیرون_type type;
};

ساكن بنية cw_بیرون_buf *cw_بیرون_buf_create(cw_بیرون_type otype)
{
  بنية cw_بیرون_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  إذا(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_میانگیر);
  }
  أرجع cwbuf;
}

ساكن فراغ cw_بیرون_buf_free(بنية cw_بیرون_buf *cwbuf)
{
  إذا(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_بیرون_ctx {
  بنية Curl_cwriter super;
  بنية cw_بیرون_buf *buf;
  بنية cw_بیرون_buf *دم;
  size_t buffered_len;
  بیت(errored);
};

ساكن CURLcode cw_بیرون_init(بنية Curl_easy *داده,
                            بنية Curl_cwriter *writer)
{
  بنية cw_بیرون_ctx *ctx = writer->ctx;
  (فراغ)داده;
  ctx->buf = عدم;
  ctx->دم = عدم;
  ctx->buffered_len = 0;
  أرجع CURLE_OK;
}

ساكن فراغ cw_بیرون_bufs_free(بنية cw_بیرون_ctx *ctx)
{
  طالما(ctx->buf) {
    بنية cw_بیرون_buf *بعدی = ctx->buf->بعدی;
    cw_بیرون_buf_free(ctx->buf);
    ctx->buf = بعدی;
  }
  ctx->دم = عدم;
  ctx->buffered_len = 0;
}

ساكن فراغ cw_بیرون_close(بنية Curl_easy *داده, بنية Curl_cwriter *writer)
{
  بنية cw_بیرون_ctx *ctx = writer->ctx;

  (فراغ)داده;
  cw_بیرون_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساكن فراغ cw_get_writefunc(بنية Curl_easy *داده, cw_بیرون_type otype,
                             curl_write_callback *pwcb, فراغ **pwcb_داده,
                             size_t *pmax_write, size_t *pmin_write)
{
  بدل(otype) {
  حالة CW_بیرون_BODY:
  حالة CW_بیرون_BODY_0_LEN:
    إذا(Curl_ftp_listmode(داده)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_داده = داده;
    }
    وإلا {
      *pwcb = داده->مجموعه.fwrite_func;
      *pwcb_داده = داده->مجموعه.بیرون;
    }
    *pmax_write = CURL_MAX_WRITE_اندازه;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    اكسر;
  حالة CW_بیرون_HDS:
    *pwcb = داده->مجموعه.fwrite_سرآیند ? داده->مجموعه.fwrite_سرآیند :
             (داده->مجموعه.writeheader ? داده->مجموعه.fwrite_func : عدم);
    *pwcb_داده = داده->مجموعه.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    اكسر;
  افتراضي:
    *pwcb = عدم;
    *pwcb_داده = عدم;
    *pmax_write = CURL_MAX_WRITE_اندازه;
    *pmin_write = 0;
  }
}

ساكن CURLcode cw_بیرون_cb_write(بنية Curl_easy *داده,
                                curl_write_callback wcb,
                                فراغ *wcb_داده,
                                cw_بیرون_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode نتیجه;

  NOVERBOSE((فراغ)otype);

  DEBUGASSERT(داده->conn);
  *pnwritten = 0;
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, داده, easy_cw_بیرون_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_داده);
    CURL_CBAPI_انتها(&guard);
  }
  CURL_TRC_WRITE(داده, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_بیرون_HDS) ? "header" : "body",
                 (بیعلامت صحيح)otype, nwritten);
  إذا(nwritten == CURL_WRITEFUNC_PAUSE) {
    إذا(داده->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(داده, "Write callback asked for PAUSE when not supported");
      أرجع CURLE_WRITE_ERROR;
    }
    داده->req.writer.paused = TRUE;
    CURL_TRC_WRITE(داده, "[OUT] PAUSE requested by client");
    نتیجه = Curl_xfer_pause_recv(داده, TRUE);
    أرجع نتیجه ? نتیجه : CURLE_AGAIN;
  }
  وإلا إذا(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(داده, "client returned ERROR on write of %zu bytes", blen);
    أرجع CURLE_WRITE_ERROR;
  }
  وإلا إذا(nwritten != blen) {
    failf(داده, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    أرجع CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بیرون_ptr_flush(بنية cw_بیرون_ctx *ctx,
                                 بنية Curl_easy *داده,
                                 cw_بیرون_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = عدم;
  فراغ *wcb_داده;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode نتیجه = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(داده, otype, &wcb, &wcb_داده, &max_write, &min_write);
  إذا(!wcb) {
    *pconsumed = blen;
    أرجع CURLE_OK;
  }

  *pconsumed = 0;
  إذا(otype == CW_بیرون_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    أرجع cw_بیرون_cb_write(داده, wcb, wcb_داده, otype,
                           buf, blen, &nwritten);
  }
  وإلا {
    طالما(blen && !داده->req.writer.paused) {
      إذا(!flush_all && blen < min_write)
        اكسر;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      إذا(otype == CW_بیرون_BODY)
        نتیجه = Curl_pgrs_deliver_check(داده, wlen);
      إذا(!نتیجه)
        نتیجه = cw_بیرون_cb_write(داده, wcb, wcb_داده, otype,
                                 buf, wlen, &nwritten);
      إذا(نتیجه)
        أرجع نتیجه;
      إذا(otype == CW_بیرون_BODY)
        Curl_pgrs_deliver_inc(داده, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(داده, otype, &wcb, &wcb_داده, &max_write, &min_write);
      إذا(!wcb) {
        *pconsumed += blen;
        اكسر;
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بیرون_buf_flush(بنية cw_بیرون_ctx *ctx,
                                 بنية Curl_easy *داده,
                                 بنية cw_بیرون_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_بیرون_BODY_0_LEN)) {
    size_t consumed;

    نتیجه = cw_بیرون_ptr_flush(ctx, داده, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    إذا(نتیجه && (نتیجه != CURLE_AGAIN))
      أرجع نتیجه;
    نتیجه = CURLE_OK;

    إذا(consumed) {
      إذا(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      وإلا {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        نتیجه = curlx_dyn_دم(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        إذا(نتیجه)
          أرجع نتیجه;
      }
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode cw_بیرون_flush_chain(بنية cw_بیرون_ctx *ctx,
                                   بنية Curl_easy *داده,
                                   bool flush_all)
{
  CURLcode نتیجه;

  /* write the chain oldest first until it blocks or gets empty */
  طالما(ctx->buf) {
    بنية cw_بیرون_buf *cwbuf = ctx->buf;
    size_t blen_before;

    إذا(داده->req.writer.paused)
      أرجع CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    نتیجه = cw_بیرون_buf_flush(ctx, داده, cwbuf, flush_all);
    إذا(نتیجه)
      أرجع نتیجه;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    إذا(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(داده->req.writer.paused);
      أرجع CURLE_OK;
    }

    ctx->buf = cwbuf->بعدی;
    إذا(!ctx->buf)
      ctx->دم = عدم;
    cw_بیرون_buf_free(cwbuf);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بیرون_append(بنية cw_بیرون_ctx *ctx,
                              بنية Curl_easy *داده,
                              cw_بیرون_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجه;

  CURL_TRC_WRITE(داده, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_میانگیر);
  إذا(ctx->buffered_len + blen > DYN_PAUSE_میانگیر) {
    failf(داده, "pause buffer not large enough -> CURLE_TOO_LARGE");
    أرجع CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  إذا(!ctx->دم || (ctx->دم->type != otype) || (otype == CW_بیرون_HDS)) {
    بنية cw_بیرون_buf *cwbuf = cw_بیرون_buf_create(otype);
    إذا(!cwbuf)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(ctx->دم)
      ctx->دم->بعدی = cwbuf;
    وإلا
      ctx->buf = cwbuf;
    ctx->دم = cwbuf;
  }
  DEBUGASSERT(ctx->دم && (ctx->دم->type == otype));
  نتیجه = curlx_dyn_addn(&ctx->دم->b, buf, blen);
  إذا(!نتیجه)
    ctx->buffered_len += blen;
  أرجع نتیجه;
}

ساكن CURLcode cw_بیرون_do_write(بنية cw_بیرون_ctx *ctx,
                                بنية Curl_easy *داده,
                                cw_بیرون_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجه = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  إذا(ctx->دم && ctx->دم->type != otype) {
    نتیجه = cw_بیرون_flush_chain(ctx, داده, TRUE);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(ctx->buf) {
    /* still have buffered data, append and flush */
    نتیجه = cw_بیرون_append(ctx, داده, otype, buf, blen);
    إذا(نتیجه)
      اذهب بیرون;
    نتیجه = cw_بیرون_flush_chain(ctx, داده, flush_all);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا {
    /* nothing buffered, try direct write */
    size_t consumed;
    نتیجه = cw_بیرون_ptr_flush(ctx, داده, otype, flush_all,
                              buf, blen, &consumed);
    إذا(نتیجه && (نتیجه != CURLE_AGAIN))
      أرجع نتیجه;
    نتیجه = CURLE_OK;
    إذا(consumed < blen) {
      /* did not write all, append the rest */
      نتیجه = cw_بیرون_append(ctx, داده, otype,
                             buf + consumed, blen - consumed);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }

بیرون:
  إذا(نتیجه) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_بیرون_bufs_free(ctx);
  }
  أرجع نتیجه;
}

ساكن CURLcode cw_بیرون_write(بنية Curl_easy *داده,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t blen)
{
  بنية cw_بیرون_ctx *ctx = writer->ctx;
  CURLcode نتیجه;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  إذا((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_سرآیند) && داده->مجموعه.include_سرآیند)) {
    cw_بیرون_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_بیرون_BODY_0_LEN : CW_بیرون_BODY;
    نتیجه = cw_بیرون_do_write(ctx, داده, otype, flush_all, buf, blen);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(type & (CLIENTWRITE_سرآیند | CLIENTWRITE_INFO)) {
    نتیجه = cw_بیرون_do_write(ctx, داده, CW_بیرون_HDS, flush_all, buf, blen);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cw_بیرون_do_flush(بنية Curl_easy *داده,
                                بنية Curl_cwriter *cw_بیرون,
                                bool flush_all)
{
  بنية cw_بیرون_ctx *ctx = (بنية cw_بیرون_ctx *)cw_بیرون;

  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  إذا(!داده->req.writer.paused && ctx->buf) {
    CURLcode نتیجه;

    CURL_TRC_WRITE(داده, "[OUT] flush");
    نتیجه = cw_بیرون_flush_chain(ctx, داده, flush_all);
    إذا(نتیجه) {
      ctx->errored = TRUE;
      cw_بیرون_bufs_free(ctx);
      أرجع نتیجه;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بیرون_flush(بنية Curl_easy *داده,
                             بنية Curl_cwriter *writer)
{
  أرجع cw_بیرون_do_flush(داده, writer, FALSE);
}

ثابت بنية Curl_cwtype Curl_cwt_بیرون = {
  "cw-out",
  عدم,
  0,
  cw_بیرون_init,
  cw_بیرون_write,
  cw_بیرون_flush,
  cw_بیرون_close,
  حجم(بنية cw_بیرون_ctx)
};

CURLcode Curl_cw_بیرون_انجامشده(بنية Curl_easy *داده)
{
  بنية Curl_cwriter *cw_بیرون;
  CURLcode نتیجه = CURLE_OK;

  cw_بیرون = Curl_cwriter_get_by_type(داده, &Curl_cwt_بیرون);
  إذا(cw_بیرون) {
    CURL_TRC_WRITE(داده, "[OUT] done");
    نتیجه = Curl_client_flush(داده);
    إذا(!نتیجه)
      نتیجه = cw_بیرون_do_flush(داده, cw_بیرون, TRUE);
  }
  أرجع نتیجه;
}
