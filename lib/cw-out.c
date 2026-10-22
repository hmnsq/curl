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
  CW_ٻاهريون_NONE,
  CW_ٻاهريون_BODY,
  CW_ٻاهريون_BODY_0_LEN,
  CW_ٻاهريون_HDS
} cw_ٻاهريون_type;

بنية cw_ٻاهريون_buf {
  بنية cw_ٻاهريون_buf *ايندڙ;
  بنية dynbuf b;
  cw_ٻاهريون_type type;
};

ساكن بنية cw_ٻاهريون_buf *cw_ٻاهريون_buf_create(cw_ٻاهريون_type otype)
{
  بنية cw_ٻاهريون_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  إذا(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_بفر);
  }
  أرجع cwbuf;
}

ساكن فراغ cw_ٻاهريون_buf_free(بنية cw_ٻاهريون_buf *cwbuf)
{
  إذا(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_ٻاهريون_ctx {
  بنية Curl_cwriter super;
  بنية cw_ٻاهريون_buf *buf;
  بنية cw_ٻاهريون_buf *پڇ;
  size_t buffered_len;
  ٻٽو_انگ(errored);
};

ساكن CURLcode cw_ٻاهريون_init(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  بنية cw_ٻاهريون_ctx *ctx = writer->ctx;
  (فراغ)معلومات;
  ctx->buf = عدم;
  ctx->پڇ = عدم;
  ctx->buffered_len = 0;
  أرجع CURLE_OK;
}

ساكن فراغ cw_ٻاهريون_bufs_free(بنية cw_ٻاهريون_ctx *ctx)
{
  طالما(ctx->buf) {
    بنية cw_ٻاهريون_buf *ايندڙ = ctx->buf->ايندڙ;
    cw_ٻاهريون_buf_free(ctx->buf);
    ctx->buf = ايندڙ;
  }
  ctx->پڇ = عدم;
  ctx->buffered_len = 0;
}

ساكن فراغ cw_ٻاهريون_close(بنية Curl_easy *معلومات, بنية Curl_cwriter *writer)
{
  بنية cw_ٻاهريون_ctx *ctx = writer->ctx;

  (فراغ)معلومات;
  cw_ٻاهريون_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساكن فراغ cw_get_writefunc(بنية Curl_easy *معلومات, cw_ٻاهريون_type otype,
                             curl_write_callback *pwcb, فراغ **pwcb_معلومات,
                             size_t *pmax_write, size_t *pmin_write)
{
  بدل(otype) {
  حالة CW_ٻاهريون_BODY:
  حالة CW_ٻاهريون_BODY_0_LEN:
    إذا(Curl_ftp_listmode(معلومات)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_معلومات = معلومات;
    }
    وإلا {
      *pwcb = معلومات->سيٽ.fwrite_func;
      *pwcb_معلومات = معلومات->سيٽ.ٻاهريون;
    }
    *pmax_write = CURL_MAX_WRITE_سائيز;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    اكسر;
  حالة CW_ٻاهريون_HDS:
    *pwcb = معلومات->سيٽ.fwrite_سرو ? معلومات->سيٽ.fwrite_سرو :
             (معلومات->سيٽ.writeheader ? معلومات->سيٽ.fwrite_func : عدم);
    *pwcb_معلومات = معلومات->سيٽ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    اكسر;
  افتراضي:
    *pwcb = عدم;
    *pwcb_معلومات = عدم;
    *pmax_write = CURL_MAX_WRITE_سائيز;
    *pmin_write = 0;
  }
}

ساكن CURLcode cw_ٻاهريون_cb_write(بنية Curl_easy *معلومات,
                                curl_write_callback wcb,
                                فراغ *wcb_معلومات,
                                cw_ٻاهريون_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode نتيجو;

  NOVERBOSE((فراغ)otype);

  DEBUGASSERT(معلومات->conn);
  *pnwritten = 0;
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروعات(&guard, معلومات, easy_cw_ٻاهريون_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_معلومات);
    CURL_CBAPI_پڇاڙي(&guard);
  }
  CURL_TRC_WRITE(معلومات, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_ٻاهريون_HDS) ? "header" : "body",
                 (بنانشان صحيح)otype, nwritten);
  إذا(nwritten == CURL_WRITEFUNC_PAUSE) {
    إذا(معلومات->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(معلومات, "Write callback asked for PAUSE when not supported");
      أرجع CURLE_WRITE_ERROR;
    }
    معلومات->req.writer.paused = TRUE;
    CURL_TRC_WRITE(معلومات, "[OUT] PAUSE requested by client");
    نتيجو = Curl_xfer_pause_recv(معلومات, TRUE);
    أرجع نتيجو ? نتيجو : CURLE_AGAIN;
  }
  وإلا إذا(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(معلومات, "client returned ERROR on write of %zu bytes", blen);
    أرجع CURLE_WRITE_ERROR;
  }
  وإلا إذا(nwritten != blen) {
    failf(معلومات, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    أرجع CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  أرجع CURLE_OK;
}

ساكن CURLcode cw_ٻاهريون_ptr_flush(بنية cw_ٻاهريون_ctx *ctx,
                                 بنية Curl_easy *معلومات,
                                 cw_ٻاهريون_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = عدم;
  فراغ *wcb_معلومات;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode نتيجو = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(معلومات, otype, &wcb, &wcb_معلومات, &max_write, &min_write);
  إذا(!wcb) {
    *pconsumed = blen;
    أرجع CURLE_OK;
  }

  *pconsumed = 0;
  إذا(otype == CW_ٻاهريون_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    أرجع cw_ٻاهريون_cb_write(معلومات, wcb, wcb_معلومات, otype,
                           buf, blen, &nwritten);
  }
  وإلا {
    طالما(blen && !معلومات->req.writer.paused) {
      إذا(!flush_all && blen < min_write)
        اكسر;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      إذا(otype == CW_ٻاهريون_BODY)
        نتيجو = Curl_pgrs_deliver_check(معلومات, wlen);
      إذا(!نتيجو)
        نتيجو = cw_ٻاهريون_cb_write(معلومات, wcb, wcb_معلومات, otype,
                                 buf, wlen, &nwritten);
      إذا(نتيجو)
        أرجع نتيجو;
      إذا(otype == CW_ٻاهريون_BODY)
        Curl_pgrs_deliver_inc(معلومات, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(معلومات, otype, &wcb, &wcb_معلومات, &max_write, &min_write);
      إذا(!wcb) {
        *pconsumed += blen;
        اكسر;
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_ٻاهريون_buf_flush(بنية cw_ٻاهريون_ctx *ctx,
                                 بنية Curl_easy *معلومات,
                                 بنية cw_ٻاهريون_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode نتيجو = CURLE_OK;

  إذا(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_ٻاهريون_BODY_0_LEN)) {
    size_t consumed;

    نتيجو = cw_ٻاهريون_ptr_flush(ctx, معلومات, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    إذا(نتيجو && (نتيجو != CURLE_AGAIN))
      أرجع نتيجو;
    نتيجو = CURLE_OK;

    إذا(consumed) {
      إذا(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      وإلا {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        نتيجو = curlx_dyn_پڇ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        إذا(نتيجو)
          أرجع نتيجو;
      }
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode cw_ٻاهريون_flush_chain(بنية cw_ٻاهريون_ctx *ctx,
                                   بنية Curl_easy *معلومات,
                                   bool flush_all)
{
  CURLcode نتيجو;

  /* write the chain oldest first until it blocks or gets empty */
  طالما(ctx->buf) {
    بنية cw_ٻاهريون_buf *cwbuf = ctx->buf;
    size_t blen_before;

    إذا(معلومات->req.writer.paused)
      أرجع CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    نتيجو = cw_ٻاهريون_buf_flush(ctx, معلومات, cwbuf, flush_all);
    إذا(نتيجو)
      أرجع نتيجو;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    إذا(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(معلومات->req.writer.paused);
      أرجع CURLE_OK;
    }

    ctx->buf = cwbuf->ايندڙ;
    إذا(!ctx->buf)
      ctx->پڇ = عدم;
    cw_ٻاهريون_buf_free(cwbuf);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_ٻاهريون_append(بنية cw_ٻاهريون_ctx *ctx,
                              بنية Curl_easy *معلومات,
                              cw_ٻاهريون_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتيجو;

  CURL_TRC_WRITE(معلومات, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_بفر);
  إذا(ctx->buffered_len + blen > DYN_PAUSE_بفر) {
    failf(معلومات, "pause buffer not large enough -> CURLE_TOO_LARGE");
    أرجع CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  إذا(!ctx->پڇ || (ctx->پڇ->type != otype) || (otype == CW_ٻاهريون_HDS)) {
    بنية cw_ٻاهريون_buf *cwbuf = cw_ٻاهريون_buf_create(otype);
    إذا(!cwbuf)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    إذا(ctx->پڇ)
      ctx->پڇ->ايندڙ = cwbuf;
    وإلا
      ctx->buf = cwbuf;
    ctx->پڇ = cwbuf;
  }
  DEBUGASSERT(ctx->پڇ && (ctx->پڇ->type == otype));
  نتيجو = curlx_dyn_addn(&ctx->پڇ->b, buf, blen);
  إذا(!نتيجو)
    ctx->buffered_len += blen;
  أرجع نتيجو;
}

ساكن CURLcode cw_ٻاهريون_do_write(بنية cw_ٻاهريون_ctx *ctx,
                                بنية Curl_easy *معلومات,
                                cw_ٻاهريون_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode نتيجو = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  إذا(ctx->پڇ && ctx->پڇ->type != otype) {
    نتيجو = cw_ٻاهريون_flush_chain(ctx, معلومات, TRUE);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  إذا(ctx->buf) {
    /* still have buffered data, append and flush */
    نتيجو = cw_ٻاهريون_append(ctx, معلومات, otype, buf, blen);
    إذا(نتيجو)
      اذهب ٻاهريون;
    نتيجو = cw_ٻاهريون_flush_chain(ctx, معلومات, flush_all);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  وإلا {
    /* nothing buffered, try direct write */
    size_t consumed;
    نتيجو = cw_ٻاهريون_ptr_flush(ctx, معلومات, otype, flush_all,
                              buf, blen, &consumed);
    إذا(نتيجو && (نتيجو != CURLE_AGAIN))
      أرجع نتيجو;
    نتيجو = CURLE_OK;
    إذا(consumed < blen) {
      /* did not write all, append the rest */
      نتيجو = cw_ٻاهريون_append(ctx, معلومات, otype,
                             buf + consumed, blen - consumed);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }

ٻاهريون:
  إذا(نتيجو) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_ٻاهريون_bufs_free(ctx);
  }
  أرجع نتيجو;
}

ساكن CURLcode cw_ٻاهريون_write(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t blen)
{
  بنية cw_ٻاهريون_ctx *ctx = writer->ctx;
  CURLcode نتيجو;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  إذا((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_سرو) && معلومات->سيٽ.include_سرو)) {
    cw_ٻاهريون_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_ٻاهريون_BODY_0_LEN : CW_ٻاهريون_BODY;
    نتيجو = cw_ٻاهريون_do_write(ctx, معلومات, otype, flush_all, buf, blen);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(type & (CLIENTWRITE_سرو | CLIENTWRITE_INFO)) {
    نتيجو = cw_ٻاهريون_do_write(ctx, معلومات, CW_ٻاهريون_HDS, flush_all, buf, blen);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cw_ٻاهريون_do_flush(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *cw_ٻاهريون,
                                bool flush_all)
{
  بنية cw_ٻاهريون_ctx *ctx = (بنية cw_ٻاهريون_ctx *)cw_ٻاهريون;

  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  إذا(!معلومات->req.writer.paused && ctx->buf) {
    CURLcode نتيجو;

    CURL_TRC_WRITE(معلومات, "[OUT] flush");
    نتيجو = cw_ٻاهريون_flush_chain(ctx, معلومات, flush_all);
    إذا(نتيجو) {
      ctx->errored = TRUE;
      cw_ٻاهريون_bufs_free(ctx);
      أرجع نتيجو;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_ٻاهريون_flush(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer)
{
  أرجع cw_ٻاهريون_do_flush(معلومات, writer, FALSE);
}

ثابت بنية Curl_cwtype Curl_cwt_ٻاهريون = {
  "cw-out",
  عدم,
  0,
  cw_ٻاهريون_init,
  cw_ٻاهريون_write,
  cw_ٻاهريون_flush,
  cw_ٻاهريون_close,
  حجم(بنية cw_ٻاهريون_ctx)
};

CURLcode Curl_cw_ٻاهريون_پورو(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *cw_ٻاهريون;
  CURLcode نتيجو = CURLE_OK;

  cw_ٻاهريون = Curl_cwriter_get_by_type(معلومات, &Curl_cwt_ٻاهريون);
  إذا(cw_ٻاهريون) {
    CURL_TRC_WRITE(معلومات, "[OUT] done");
    نتيجو = Curl_client_flush(معلومات);
    إذا(!نتيجو)
      نتيجو = cw_ٻاهريون_do_flush(معلومات, cw_ٻاهريون, TRUE);
  }
  أرجع نتيجو;
}
