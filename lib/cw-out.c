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
  CW_بهر_NONE,
  CW_بهر_BODY,
  CW_بهر_BODY_0_LEN,
  CW_بهر_HDS
} cw_بهر_type;

بنية cw_بهر_buf {
  بنية cw_بهر_buf *بل;
  بنية dynbuf b;
  cw_بهر_type type;
};

ساكن بنية cw_بهر_buf *cw_بهر_buf_create(cw_بهر_type otype)
{
  بنية cw_بهر_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  إذا(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_بفر);
  }
  أرجع cwbuf;
}

ساكن فراغ cw_بهر_buf_free(بنية cw_بهر_buf *cwbuf)
{
  إذا(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_بهر_ctx {
  بنية Curl_cwriter super;
  بنية cw_بهر_buf *buf;
  بنية cw_بهر_buf *لکۍ;
  size_t buffered_len;
  بټ(errored);
};

ساكن CURLcode cw_بهر_init(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  بنية cw_بهر_ctx *ctx = writer->ctx;
  (فراغ)معلومات;
  ctx->buf = عدم;
  ctx->لکۍ = عدم;
  ctx->buffered_len = 0;
  أرجع CURLE_OK;
}

ساكن فراغ cw_بهر_bufs_free(بنية cw_بهر_ctx *ctx)
{
  طالما(ctx->buf) {
    بنية cw_بهر_buf *بل = ctx->buf->بل;
    cw_بهر_buf_free(ctx->buf);
    ctx->buf = بل;
  }
  ctx->لکۍ = عدم;
  ctx->buffered_len = 0;
}

ساكن فراغ cw_بهر_close(بنية Curl_easy *معلومات, بنية Curl_cwriter *writer)
{
  بنية cw_بهر_ctx *ctx = writer->ctx;

  (فراغ)معلومات;
  cw_بهر_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساكن فراغ cw_get_writefunc(بنية Curl_easy *معلومات, cw_بهر_type otype,
                             curl_write_callback *pwcb, فراغ **pwcb_معلومات,
                             size_t *pmax_write, size_t *pmin_write)
{
  بدل(otype) {
  حالة CW_بهر_BODY:
  حالة CW_بهر_BODY_0_LEN:
    إذا(Curl_ftp_listmode(معلومات)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_معلومات = معلومات;
    }
    وإلا {
      *pwcb = معلومات->سېټ.fwrite_func;
      *pwcb_معلومات = معلومات->سېټ.بهر;
    }
    *pmax_write = CURL_MAX_WRITE_اندازه;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    اكسر;
  حالة CW_بهر_HDS:
    *pwcb = معلومات->سېټ.fwrite_سرلیک ? معلومات->سېټ.fwrite_سرلیک :
             (معلومات->سېټ.writeheader ? معلومات->سېټ.fwrite_func : عدم);
    *pwcb_معلومات = معلومات->سېټ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    اكسر;
  افتراضي:
    *pwcb = عدم;
    *pwcb_معلومات = عدم;
    *pmax_write = CURL_MAX_WRITE_اندازه;
    *pmin_write = 0;
  }
}

ساكن CURLcode cw_بهر_cb_write(بنية Curl_easy *معلومات,
                                curl_write_callback wcb,
                                فراغ *wcb_معلومات,
                                cw_بهر_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode پایله;

  NOVERBOSE((فراغ)otype);

  DEBUGASSERT(معلومات->conn);
  *pnwritten = 0;
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_پیل(&guard, معلومات, easy_cw_بهر_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_معلومات);
    CURL_CBAPI_پای_ټکی(&guard);
  }
  CURL_TRC_WRITE(معلومات, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_بهر_HDS) ? "header" : "body",
                 (بینښه صحيح)otype, nwritten);
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
    پایله = Curl_xfer_pause_recv(معلومات, TRUE);
    أرجع پایله ? پایله : CURLE_AGAIN;
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

ساكن CURLcode cw_بهر_ptr_flush(بنية cw_بهر_ctx *ctx,
                                 بنية Curl_easy *معلومات,
                                 cw_بهر_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = عدم;
  فراغ *wcb_معلومات;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode پایله = CURLE_OK;

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
  إذا(otype == CW_بهر_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    أرجع cw_بهر_cb_write(معلومات, wcb, wcb_معلومات, otype,
                           buf, blen, &nwritten);
  }
  وإلا {
    طالما(blen && !معلومات->req.writer.paused) {
      إذا(!flush_all && blen < min_write)
        اكسر;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      إذا(otype == CW_بهر_BODY)
        پایله = Curl_pgrs_deliver_check(معلومات, wlen);
      إذا(!پایله)
        پایله = cw_بهر_cb_write(معلومات, wcb, wcb_معلومات, otype,
                                 buf, wlen, &nwritten);
      إذا(پایله)
        أرجع پایله;
      إذا(otype == CW_بهر_BODY)
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

ساكن CURLcode cw_بهر_buf_flush(بنية cw_بهر_ctx *ctx,
                                 بنية Curl_easy *معلومات,
                                 بنية cw_بهر_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode پایله = CURLE_OK;

  إذا(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_بهر_BODY_0_LEN)) {
    size_t consumed;

    پایله = cw_بهر_ptr_flush(ctx, معلومات, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    إذا(پایله && (پایله != CURLE_AGAIN))
      أرجع پایله;
    پایله = CURLE_OK;

    إذا(consumed) {
      إذا(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      وإلا {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        پایله = curlx_dyn_لکۍ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        إذا(پایله)
          أرجع پایله;
      }
    }
  }
  أرجع پایله;
}

ساكن CURLcode cw_بهر_flush_chain(بنية cw_بهر_ctx *ctx,
                                   بنية Curl_easy *معلومات,
                                   bool flush_all)
{
  CURLcode پایله;

  /* write the chain oldest first until it blocks or gets empty */
  طالما(ctx->buf) {
    بنية cw_بهر_buf *cwbuf = ctx->buf;
    size_t blen_before;

    إذا(معلومات->req.writer.paused)
      أرجع CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    پایله = cw_بهر_buf_flush(ctx, معلومات, cwbuf, flush_all);
    إذا(پایله)
      أرجع پایله;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    إذا(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(معلومات->req.writer.paused);
      أرجع CURLE_OK;
    }

    ctx->buf = cwbuf->بل;
    إذا(!ctx->buf)
      ctx->لکۍ = عدم;
    cw_بهر_buf_free(cwbuf);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بهر_append(بنية cw_بهر_ctx *ctx,
                              بنية Curl_easy *معلومات,
                              cw_بهر_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode پایله;

  CURL_TRC_WRITE(معلومات, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_بفر);
  إذا(ctx->buffered_len + blen > DYN_PAUSE_بفر) {
    failf(معلومات, "pause buffer not large enough -> CURLE_TOO_LARGE");
    أرجع CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  إذا(!ctx->لکۍ || (ctx->لکۍ->type != otype) || (otype == CW_بهر_HDS)) {
    بنية cw_بهر_buf *cwbuf = cw_بهر_buf_create(otype);
    إذا(!cwbuf)
      أرجع CURLE_بهر_د_حافظه;
    إذا(ctx->لکۍ)
      ctx->لکۍ->بل = cwbuf;
    وإلا
      ctx->buf = cwbuf;
    ctx->لکۍ = cwbuf;
  }
  DEBUGASSERT(ctx->لکۍ && (ctx->لکۍ->type == otype));
  پایله = curlx_dyn_addn(&ctx->لکۍ->b, buf, blen);
  إذا(!پایله)
    ctx->buffered_len += blen;
  أرجع پایله;
}

ساكن CURLcode cw_بهر_do_write(بنية cw_بهر_ctx *ctx,
                                بنية Curl_easy *معلومات,
                                cw_بهر_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode پایله = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  إذا(ctx->لکۍ && ctx->لکۍ->type != otype) {
    پایله = cw_بهر_flush_chain(ctx, معلومات, TRUE);
    إذا(پایله)
      اذهب بهر;
  }

  إذا(ctx->buf) {
    /* still have buffered data, append and flush */
    پایله = cw_بهر_append(ctx, معلومات, otype, buf, blen);
    إذا(پایله)
      اذهب بهر;
    پایله = cw_بهر_flush_chain(ctx, معلومات, flush_all);
    إذا(پایله)
      اذهب بهر;
  }
  وإلا {
    /* nothing buffered, try direct write */
    size_t consumed;
    پایله = cw_بهر_ptr_flush(ctx, معلومات, otype, flush_all,
                              buf, blen, &consumed);
    إذا(پایله && (پایله != CURLE_AGAIN))
      أرجع پایله;
    پایله = CURLE_OK;
    إذا(consumed < blen) {
      /* did not write all, append the rest */
      پایله = cw_بهر_append(ctx, معلومات, otype,
                             buf + consumed, blen - consumed);
      إذا(پایله)
        اذهب بهر;
    }
  }

بهر:
  إذا(پایله) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_بهر_bufs_free(ctx);
  }
  أرجع پایله;
}

ساكن CURLcode cw_بهر_write(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t blen)
{
  بنية cw_بهر_ctx *ctx = writer->ctx;
  CURLcode پایله;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  إذا((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_سرلیک) && معلومات->سېټ.include_سرلیک)) {
    cw_بهر_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_بهر_BODY_0_LEN : CW_بهر_BODY;
    پایله = cw_بهر_do_write(ctx, معلومات, otype, flush_all, buf, blen);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(type & (CLIENTWRITE_سرلیک | CLIENTWRITE_INFO)) {
    پایله = cw_بهر_do_write(ctx, معلومات, CW_بهر_HDS, flush_all, buf, blen);
    إذا(پایله)
      أرجع پایله;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cw_بهر_do_flush(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *cw_بهر,
                                bool flush_all)
{
  بنية cw_بهر_ctx *ctx = (بنية cw_بهر_ctx *)cw_بهر;

  إذا(ctx->errored)
    أرجع CURLE_WRITE_ERROR;

  إذا(!معلومات->req.writer.paused && ctx->buf) {
    CURLcode پایله;

    CURL_TRC_WRITE(معلومات, "[OUT] flush");
    پایله = cw_بهر_flush_chain(ctx, معلومات, flush_all);
    إذا(پایله) {
      ctx->errored = TRUE;
      cw_بهر_bufs_free(ctx);
      أرجع پایله;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cw_بهر_flush(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer)
{
  أرجع cw_بهر_do_flush(معلومات, writer, FALSE);
}

ثابت بنية Curl_cwtype Curl_cwt_بهر = {
  "cw-out",
  عدم,
  0,
  cw_بهر_init,
  cw_بهر_write,
  cw_بهر_flush,
  cw_بهر_close,
  حجم(بنية cw_بهر_ctx)
};

CURLcode Curl_cw_بهر_بشپړ(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *cw_بهر;
  CURLcode پایله = CURLE_OK;

  cw_بهر = Curl_cwriter_get_by_type(معلومات, &Curl_cwt_بهر);
  إذا(cw_بهر) {
    CURL_TRC_WRITE(معلومات, "[OUT] done");
    پایله = Curl_client_flush(معلومات);
    إذا(!پایله)
      پایله = cw_بهر_do_flush(معلومات, cw_بهر, TRUE);
  }
  أرجع پایله;
}
