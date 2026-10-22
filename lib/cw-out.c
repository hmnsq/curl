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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-out.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-pause.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CW_ਬਾਹਰਲਾ_NONE,
  CW_ਬਾਹਰਲਾ_BODY,
  CW_ਬਾਹਰਲਾ_BODY_0_LEN,
  CW_ਬਾਹਰਲਾ_HDS
} cw_ਬਾਹਰਲਾ_type;

ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf {
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *ਅਗਲਾ;
  ਬਣਤਰ dynbuf b;
  cw_ਬਾਹਰਲਾ_type type;
};

ਸਥਿਰ ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cw_ਬਾਹਰਲਾ_buf_create(cw_ਬਾਹਰਲਾ_type otype)
{
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cwbuf = curlx_calloc(1, ਆਕਾਰ(*cwbuf));
  ਜੇ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_ਬਫਰ);
  }
  ਵਾਪਸ cwbuf;
}

ਸਥਿਰ ਖਾਲੀ cw_ਬਾਹਰਲਾ_buf_free(ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cwbuf)
{
  ਜੇ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx {
  ਬਣਤਰ Curl_cwriter super;
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *buf;
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *ਪੂਛ;
  size_t buffered_len;
  ਦੋਆਧਾਰੀ_ਅੰਕ(errored);
};

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx = writer->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx->buf = ਨਿਰਾ;
  ctx->ਪੂਛ = ਨਿਰਾ;
  ctx->buffered_len = 0;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cw_ਬਾਹਰਲਾ_bufs_free(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx)
{
  ਜਦੋਂ(ctx->buf) {
    ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *ਅਗਲਾ = ctx->buf->ਅਗਲਾ;
    cw_ਬਾਹਰਲਾ_buf_free(ctx->buf);
    ctx->buf = ਅਗਲਾ;
  }
  ctx->ਪੂਛ = ਨਿਰਾ;
  ctx->buffered_len = 0;
}

ਸਥਿਰ ਖਾਲੀ cw_ਬਾਹਰਲਾ_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx = writer->ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  cw_ਬਾਹਰਲਾ_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ਸਥਿਰ ਖਾਲੀ cw_get_writefunc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, cw_ਬਾਹਰਲਾ_type otype,
                             curl_write_callback *pwcb, ਖਾਲੀ **pwcb_ਅੰਕੜੇ,
                             size_t *pmax_write, size_t *pmin_write)
{
  ਚੋਣ(otype) {
  ਹਾਲਤ CW_ਬਾਹਰਲਾ_BODY:
  ਹਾਲਤ CW_ਬਾਹਰਲਾ_BODY_0_LEN:
    ਜੇ(Curl_ftp_listmode(ਅੰਕੜੇ)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_ਅੰਕੜੇ = ਅੰਕੜੇ;
    }
    ਨਹੀਂਤਾਂ {
      *pwcb = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_func;
      *pwcb_ਅੰਕੜੇ = ਅੰਕੜੇ->ਸੈੱਟ.ਬਾਹਰਲਾ;
    }
    *pmax_write = CURL_MAX_WRITE_ਮਾਪ;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ਰੋਕੋ;
  ਹਾਲਤ CW_ਬਾਹਰਲਾ_HDS:
    *pwcb = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_ਸਿਰਲੇਖ ? ਅੰਕੜੇ->ਸੈੱਟ.fwrite_ਸਿਰਲੇਖ :
             (ਅੰਕੜੇ->ਸੈੱਟ.writeheader ? ਅੰਕੜੇ->ਸੈੱਟ.fwrite_func : ਨਿਰਾ);
    *pwcb_ਅੰਕੜੇ = ਅੰਕੜੇ->ਸੈੱਟ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ਰੋਕੋ;
  ਮੂਲ:
    *pwcb = ਨਿਰਾ;
    *pwcb_ਅੰਕੜੇ = ਨਿਰਾ;
    *pmax_write = CURL_MAX_WRITE_ਮਾਪ;
    *pmin_write = 0;
  }
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_cb_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                curl_write_callback wcb,
                                ਖਾਲੀ *wcb_ਅੰਕੜੇ,
                                cw_ਬਾਹਰਲਾ_type otype,
                                ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  NOVERBOSE((ਖਾਲੀ)otype);

  DEBUGASSERT(ਅੰਕੜੇ->conn);
  *pnwritten = 0;
  {
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_cw_ਬਾਹਰਲਾ_cb);
    nwritten = wcb((ਅੱਖਰ *)CURL_UNCONST(buf), 1, blen, wcb_ਅੰਕੜੇ);
    CURL_CBAPI_ਸਿਰਾ(&guard);
  }
  CURL_TRC_WRITE(ਅੰਕੜੇ, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_ਬਾਹਰਲਾ_HDS) ? "header" : "body",
                 (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)otype, nwritten);
  ਜੇ(nwritten == CURL_WRITEFUNC_PAUSE) {
    ਜੇ(ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(ਅੰਕੜੇ, "Write callback asked for PAUSE when not supported");
      ਵਾਪਸ CURLE_WRITE_ERROR;
    }
    ਅੰਕੜੇ->req.writer.paused = TRUE;
    CURL_TRC_WRITE(ਅੰਕੜੇ, "[OUT] PAUSE requested by client");
    ਨਤੀਜਾ = Curl_xfer_pause_recv(ਅੰਕੜੇ, TRUE);
    ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : CURLE_AGAIN;
  }
  ਨਹੀਂਤਾਂ ਜੇ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(ਅੰਕੜੇ, "client returned ERROR on write of %zu bytes", blen);
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(nwritten != blen) {
    failf(ਅੰਕੜੇ, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_ptr_flush(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 cw_ਬਾਹਰਲਾ_type otype,
                                 bool flush_all,
                                 ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ਨਿਰਾ;
  ਖਾਲੀ *wcb_ਅੰਕੜੇ;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ਜੇ(ctx->errored)
    ਵਾਪਸ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(ਅੰਕੜੇ, otype, &wcb, &wcb_ਅੰਕੜੇ, &max_write, &min_write);
  ਜੇ(!wcb) {
    *pconsumed = blen;
    ਵਾਪਸ CURLE_OK;
  }

  *pconsumed = 0;
  ਜੇ(otype == CW_ਬਾਹਰਲਾ_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    ਵਾਪਸ cw_ਬਾਹਰਲਾ_cb_write(ਅੰਕੜੇ, wcb, wcb_ਅੰਕੜੇ, otype,
                           buf, blen, &nwritten);
  }
  ਨਹੀਂਤਾਂ {
    ਜਦੋਂ(blen && !ਅੰਕੜੇ->req.writer.paused) {
      ਜੇ(!flush_all && blen < min_write)
        ਰੋਕੋ;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ਜੇ(otype == CW_ਬਾਹਰਲਾ_BODY)
        ਨਤੀਜਾ = Curl_pgrs_deliver_check(ਅੰਕੜੇ, wlen);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_cb_write(ਅੰਕੜੇ, wcb, wcb_ਅੰਕੜੇ, otype,
                                 buf, wlen, &nwritten);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਜੇ(otype == CW_ਬਾਹਰਲਾ_BODY)
        Curl_pgrs_deliver_inc(ਅੰਕੜੇ, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(ਅੰਕੜੇ, otype, &wcb, &wcb_ਅੰਕੜੇ, &max_write, &min_write);
      ਜੇ(!wcb) {
        *pconsumed += blen;
        ਰੋਕੋ;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_buf_flush(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_ਬਾਹਰਲਾ_BODY_0_LEN)) {
    size_t consumed;

    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_ptr_flush(ctx, ਅੰਕੜੇ, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਤੀਜਾ = CURLE_OK;

    ਜੇ(consumed) {
      ਜੇ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      ਨਹੀਂਤਾਂ {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        ਨਤੀਜਾ = curlx_dyn_ਪੂਛ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_flush_chain(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   bool flush_all)
{
  CURLcode ਨਤੀਜਾ;

  /* write the chain oldest first until it blocks or gets empty */
  ਜਦੋਂ(ctx->buf) {
    ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ਜੇ(ਅੰਕੜੇ->req.writer.paused)
      ਵਾਪਸ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_buf_flush(ctx, ਅੰਕੜੇ, cwbuf, flush_all);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ਜੇ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(ਅੰਕੜੇ->req.writer.paused);
      ਵਾਪਸ CURLE_OK;
    }

    ctx->buf = cwbuf->ਅਗਲਾ;
    ਜੇ(!ctx->buf)
      ctx->ਪੂਛ = ਨਿਰਾ;
    cw_ਬਾਹਰਲਾ_buf_free(cwbuf);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_append(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              cw_ਬਾਹਰਲਾ_type otype,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  CURLcode ਨਤੀਜਾ;

  CURL_TRC_WRITE(ਅੰਕੜੇ, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_ਬਫਰ);
  ਜੇ(ctx->buffered_len + blen > DYN_PAUSE_ਬਫਰ) {
    failf(ਅੰਕੜੇ, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ਵਾਪਸ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ਜੇ(!ctx->ਪੂਛ || (ctx->ਪੂਛ->type != otype) || (otype == CW_ਬਾਹਰਲਾ_HDS)) {
    ਬਣਤਰ cw_ਬਾਹਰਲਾ_buf *cwbuf = cw_ਬਾਹਰਲਾ_buf_create(otype);
    ਜੇ(!cwbuf)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(ctx->ਪੂਛ)
      ctx->ਪੂਛ->ਅਗਲਾ = cwbuf;
    ਨਹੀਂਤਾਂ
      ctx->buf = cwbuf;
    ctx->ਪੂਛ = cwbuf;
  }
  DEBUGASSERT(ctx->ਪੂਛ && (ctx->ਪੂਛ->type == otype));
  ਨਤੀਜਾ = curlx_dyn_addn(&ctx->ਪੂਛ->b, buf, blen);
  ਜੇ(!ਨਤੀਜਾ)
    ctx->buffered_len += blen;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_do_write(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                cw_ਬਾਹਰਲਾ_type otype,
                                bool flush_all,
                                ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ਜੇ(ctx->ਪੂਛ && ctx->ਪੂਛ->type != otype) {
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_flush_chain(ctx, ਅੰਕੜੇ, TRUE);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ctx->buf) {
    /* still have buffered data, append and flush */
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_append(ctx, ਅੰਕੜੇ, otype, buf, blen);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_flush_chain(ctx, ਅੰਕੜੇ, flush_all);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    /* nothing buffered, try direct write */
    size_t consumed;
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_ptr_flush(ctx, ਅੰਕੜੇ, otype, flush_all,
                              buf, blen, &consumed);
    ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਤੀਜਾ = CURLE_OK;
    ਜੇ(consumed < blen) {
      /* did not write all, append the rest */
      ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_append(ctx, ਅੰਕੜੇ, otype,
                             buf + consumed, blen - consumed);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_ਬਾਹਰਲਾ_bufs_free(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                             ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx = writer->ctx;
  CURLcode ਨਤੀਜਾ;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ਜੇ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_ਸਿਰਲੇਖ) && ਅੰਕੜੇ->ਸੈੱਟ.include_ਸਿਰਲੇਖ)) {
    cw_ਬਾਹਰਲਾ_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_ਬਾਹਰਲਾ_BODY_0_LEN : CW_ਬਾਹਰਲਾ_BODY;
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_do_write(ctx, ਅੰਕੜੇ, otype, flush_all, buf, blen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(type & (CLIENTWRITE_ਸਿਰਲੇਖ | CLIENTWRITE_INFO)) {
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_do_write(ctx, ਅੰਕੜੇ, CW_ਬਾਹਰਲਾ_HDS, flush_all, buf, blen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_do_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *cw_ਬਾਹਰਲਾ,
                                bool flush_all)
{
  ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *ctx = (ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx *)cw_ਬਾਹਰਲਾ;

  ਜੇ(ctx->errored)
    ਵਾਪਸ CURLE_WRITE_ERROR;

  ਜੇ(!ਅੰਕੜੇ->req.writer.paused && ctx->buf) {
    CURLcode ਨਤੀਜਾ;

    CURL_TRC_WRITE(ਅੰਕੜੇ, "[OUT] flush");
    ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_flush_chain(ctx, ਅੰਕੜੇ, flush_all);
    ਜੇ(ਨਤੀਜਾ) {
      ctx->errored = TRUE;
      cw_ਬਾਹਰਲਾ_bufs_free(ctx);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cw_ਬਾਹਰਲਾ_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer)
{
  ਵਾਪਸ cw_ਬਾਹਰਲਾ_do_flush(ਅੰਕੜੇ, writer, FALSE);
}

ਅਚਲ ਬਣਤਰ Curl_cwtype Curl_cwt_ਬਾਹਰਲਾ = {
  "cw-out",
  ਨਿਰਾ,
  0,
  cw_ਬਾਹਰਲਾ_init,
  cw_ਬਾਹਰਲਾ_write,
  cw_ਬਾਹਰਲਾ_flush,
  cw_ਬਾਹਰਲਾ_close,
  ਆਕਾਰ(ਬਣਤਰ cw_ਬਾਹਰਲਾ_ctx)
};

CURLcode Curl_cw_ਬਾਹਰਲਾ_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cwriter *cw_ਬਾਹਰਲਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  cw_ਬਾਹਰਲਾ = Curl_cwriter_get_by_type(ਅੰਕੜੇ, &Curl_cwt_ਬਾਹਰਲਾ);
  ਜੇ(cw_ਬਾਹਰਲਾ) {
    CURL_TRC_WRITE(ਅੰਕੜੇ, "[OUT] done");
    ਨਤੀਜਾ = Curl_client_flush(ਅੰਕੜੇ);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = cw_ਬਾਹਰਲਾ_do_flush(ਅੰਕੜੇ, cw_ਬਾਹਰਲਾ, TRUE);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
