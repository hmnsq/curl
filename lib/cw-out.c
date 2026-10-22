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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "cw-out.h"
#ಅನ್ತರ್ಭಾವ "cw-pause.h"
#ಅನ್ತರ್ಭಾವ "progress.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CW_ಹೊರಹೋಗು_NONE,
  CW_ಹೊರಹೋಗು_BODY,
  CW_ಹೊರಹೋಗು_BODY_0_LEN,
  CW_ಹೊರಹೋಗು_HDS
} cw_ಹೊರಹೋಗು_type;

ರಚನೆ cw_ಹೊರಹೋಗು_buf {
  ರಚನೆ cw_ಹೊರಹೋಗು_buf *ಮುಂದಿನ;
  ರಚನೆ dynbuf b;
  cw_ಹೊರಹೋಗು_type type;
};

ಸ್ಥಿರ ರಚನೆ cw_ಹೊರಹೋಗು_buf *cw_ಹೊರಹೋಗು_buf_create(cw_ಹೊರಹೋಗು_type otype)
{
  ರಚನೆ cw_ಹೊರಹೋಗು_buf *cwbuf = curlx_calloc(1, ಗಾತ್ರ(*cwbuf));
  ಆದರೆ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_ಬಫರ್);
  }
  ಹಿಂತಿರುಗಿ cwbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_ಹೊರಹೋಗು_buf_free(ರಚನೆ cw_ಹೊರಹೋಗು_buf *cwbuf)
{
  ಆದರೆ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ರಚನೆ cw_ಹೊರಹೋಗು_ctx {
  ರಚನೆ Curl_cwriter super;
  ರಚನೆ cw_ಹೊರಹೋಗು_buf *buf;
  ರಚನೆ cw_ಹೊರಹೋಗು_buf *ಬಾಲ;
  size_t buffered_len;
  ಬಿಟ್(errored);
};

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx->buf = ಖಾಲಿ;
  ctx->ಬಾಲ = ಖಾಲಿ;
  ctx->buffered_len = 0;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_ಹೊರಹೋಗು_bufs_free(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx)
{
  ಆಗ(ctx->buf) {
    ರಚನೆ cw_ಹೊರಹೋಗು_buf *ಮುಂದಿನ = ctx->buf->ಮುಂದಿನ;
    cw_ಹೊರಹೋಗು_buf_free(ctx->buf);
    ctx->buf = ಮುಂದಿನ;
  }
  ctx->ಬಾಲ = ಖಾಲಿ;
  ctx->buffered_len = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_ಹೊರಹೋಗು_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx = writer->ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  cw_ಹೊರಹೋಗು_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ಸ್ಥಿರ ಶೂನ್ಯ cw_get_writefunc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, cw_ಹೊರಹೋಗು_type otype,
                             curl_write_callback *pwcb, ಶೂನ್ಯ **pwcb_ದತ್ತಾಂಶ,
                             size_t *pmax_write, size_t *pmin_write)
{
  ಆಯ್ಕೆ(otype) {
  ಸ್ಥಿತಿ CW_ಹೊರಹೋಗು_BODY:
  ಸ್ಥಿತಿ CW_ಹೊರಹೋಗು_BODY_0_LEN:
    ಆದರೆ(Curl_ftp_listmode(ದತ್ತಾಂಶ)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *pwcb = ದತ್ತಾಂಶ->ಗಣ.fwrite_func;
      *pwcb_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ->ಗಣ.ಹೊರಹೋಗು;
    }
    *pmax_write = CURL_MAX_WRITE_ಗಾತ್ರ;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CW_ಹೊರಹೋಗು_HDS:
    *pwcb = ದತ್ತಾಂಶ->ಗಣ.fwrite_ಶೀರ್ಷಿಕೆ ? ದತ್ತಾಂಶ->ಗಣ.fwrite_ಶೀರ್ಷಿಕೆ :
             (ದತ್ತಾಂಶ->ಗಣ.writeheader ? ದತ್ತಾಂಶ->ಗಣ.fwrite_func : ಖಾಲಿ);
    *pwcb_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ->ಗಣ.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    *pwcb = ಖಾಲಿ;
    *pwcb_ದತ್ತಾಂಶ = ಖಾಲಿ;
    *pmax_write = CURL_MAX_WRITE_ಗಾತ್ರ;
    *pmin_write = 0;
  }
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_cb_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                curl_write_callback wcb,
                                ಶೂನ್ಯ *wcb_ದತ್ತಾಂಶ,
                                cw_ಹೊರಹೋಗು_type otype,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  NOVERBOSE((ಶೂನ್ಯ)otype);

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  *pnwritten = 0;
  {
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_cw_ಹೊರಹೋಗು_cb);
    nwritten = wcb((ಅಕ್ಷರ *)CURL_UNCONST(buf), 1, blen, wcb_ದತ್ತಾಂಶ);
    CURL_CBAPI_ಕೊನೆ(&guard);
  }
  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_ಹೊರಹೋಗು_HDS) ? "header" : "body",
                 (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)otype, nwritten);
  ಆದರೆ(nwritten == CURL_WRITEFUNC_PAUSE) {
    ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(ದತ್ತಾಂಶ, "Write callback asked for PAUSE when not supported");
      ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
    }
    ದತ್ತಾಂಶ->req.writer.paused = TRUE;
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[OUT] PAUSE requested by client");
    ಫಲಿತಾಂಶ = Curl_xfer_pause_recv(ದತ್ತಾಂಶ, TRUE);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : CURLE_AGAIN;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(ದತ್ತಾಂಶ, "client returned ERROR on write of %zu bytes", blen);
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwritten != blen) {
    failf(ದತ್ತಾಂಶ, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_ptr_flush(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 cw_ಹೊರಹೋಗು_type otype,
                                 bool flush_all,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ಖಾಲಿ;
  ಶೂನ್ಯ *wcb_ದತ್ತಾಂಶ;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ಆದರೆ(ctx->errored)
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(ದತ್ತಾಂಶ, otype, &wcb, &wcb_ದತ್ತಾಂಶ, &max_write, &min_write);
  ಆದರೆ(!wcb) {
    *pconsumed = blen;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *pconsumed = 0;
  ಆದರೆ(otype == CW_ಹೊರಹೋಗು_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    ಹಿಂತಿರುಗಿ cw_ಹೊರಹೋಗು_cb_write(ದತ್ತಾಂಶ, wcb, wcb_ದತ್ತಾಂಶ, otype,
                           buf, blen, &nwritten);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆಗ(blen && !ದತ್ತಾಂಶ->req.writer.paused) {
      ಆದರೆ(!flush_all && blen < min_write)
        ನಿಲ್ಲಿಸು;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ಆದರೆ(otype == CW_ಹೊರಹೋಗು_BODY)
        ಫಲಿತಾಂಶ = Curl_pgrs_deliver_check(ದತ್ತಾಂಶ, wlen);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_cb_write(ದತ್ತಾಂಶ, wcb, wcb_ದತ್ತಾಂಶ, otype,
                                 buf, wlen, &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಆದರೆ(otype == CW_ಹೊರಹೋಗು_BODY)
        Curl_pgrs_deliver_inc(ದತ್ತಾಂಶ, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(ದತ್ತಾಂಶ, otype, &wcb, &wcb_ದತ್ತಾಂಶ, &max_write, &min_write);
      ಆದರೆ(!wcb) {
        *pconsumed += blen;
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_buf_flush(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cw_ಹೊರಹೋಗು_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_ಹೊರಹೋಗು_BODY_0_LEN)) {
    size_t consumed;

    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_ptr_flush(ctx, ದತ್ತಾಂಶ, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = CURLE_OK;

    ಆದರೆ(consumed) {
      ಆದರೆ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        ಫಲಿತಾಂಶ = curlx_dyn_ಬಾಲ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_flush_chain(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool flush_all)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* write the chain oldest first until it blocks or gets empty */
  ಆಗ(ctx->buf) {
    ರಚನೆ cw_ಹೊರಹೋಗು_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ಆದರೆ(ದತ್ತಾಂಶ->req.writer.paused)
      ಹಿಂತಿರುಗಿ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_buf_flush(ctx, ದತ್ತಾಂಶ, cwbuf, flush_all);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ಆದರೆ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(ದತ್ತಾಂಶ->req.writer.paused);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    ctx->buf = cwbuf->ಮುಂದಿನ;
    ಆದರೆ(!ctx->buf)
      ctx->ಬಾಲ = ಖಾಲಿ;
    cw_ಹೊರಹೋಗು_buf_free(cwbuf);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_append(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              cw_ಹೊರಹೋಗು_type otype,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  CURLcode ಫಲಿತಾಂಶ;

  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_ಬಫರ್);
  ಆದರೆ(ctx->buffered_len + blen > DYN_PAUSE_ಬಫರ್) {
    failf(ದತ್ತಾಂಶ, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ಆದರೆ(!ctx->ಬಾಲ || (ctx->ಬಾಲ->type != otype) || (otype == CW_ಹೊರಹೋಗು_HDS)) {
    ರಚನೆ cw_ಹೊರಹೋಗು_buf *cwbuf = cw_ಹೊರಹೋಗು_buf_create(otype);
    ಆದರೆ(!cwbuf)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(ctx->ಬಾಲ)
      ctx->ಬಾಲ->ಮುಂದಿನ = cwbuf;
    ಇಲ್ಲದಿದ್ದರೆ
      ctx->buf = cwbuf;
    ctx->ಬಾಲ = cwbuf;
  }
  DEBUGASSERT(ctx->ಬಾಲ && (ctx->ಬಾಲ->type == otype));
  ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->ಬಾಲ->b, buf, blen);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ctx->buffered_len += blen;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_do_write(ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                cw_ಹೊರಹೋಗು_type otype,
                                bool flush_all,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ಆದರೆ(ctx->ಬಾಲ && ctx->ಬಾಲ->type != otype) {
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_flush_chain(ctx, ದತ್ತಾಂಶ, TRUE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ctx->buf) {
    /* still have buffered data, append and flush */
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_append(ctx, ದತ್ತಾಂಶ, otype, buf, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_flush_chain(ctx, ದತ್ತಾಂಶ, flush_all);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* nothing buffered, try direct write */
    size_t consumed;
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_ptr_flush(ctx, ದತ್ತಾಂಶ, otype, flush_all,
                              buf, blen, &consumed);
    ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = CURLE_OK;
    ಆದರೆ(consumed < blen) {
      /* did not write all, append the rest */
      ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_append(ctx, ದತ್ತಾಂಶ, otype,
                             buf + consumed, blen - consumed);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_ಹೊರಹೋಗು_bufs_free(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx = writer->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ಆದರೆ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_ಶೀರ್ಷಿಕೆ) && ದತ್ತಾಂಶ->ಗಣ.include_ಶೀರ್ಷಿಕೆ)) {
    cw_ಹೊರಹೋಗು_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_ಹೊರಹೋಗು_BODY_0_LEN : CW_ಹೊರಹೋಗು_BODY;
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_do_write(ctx, ದತ್ತಾಂಶ, otype, flush_all, buf, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(type & (CLIENTWRITE_ಶೀರ್ಷಿಕೆ | CLIENTWRITE_INFO)) {
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_do_write(ctx, ದತ್ತಾಂಶ, CW_ಹೊರಹೋಗು_HDS, flush_all, buf, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_do_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *cw_ಹೊರಹೋಗು,
                                bool flush_all)
{
  ರಚನೆ cw_ಹೊರಹೋಗು_ctx *ctx = (ರಚನೆ cw_ಹೊರಹೋಗು_ctx *)cw_ಹೊರಹೋಗು;

  ಆದರೆ(ctx->errored)
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;

  ಆದರೆ(!ದತ್ತಾಂಶ->req.writer.paused && ctx->buf) {
    CURLcode ಫಲಿತಾಂಶ;

    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[OUT] flush");
    ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_flush_chain(ctx, ದತ್ತಾಂಶ, flush_all);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ctx->errored = TRUE;
      cw_ಹೊರಹೋಗು_bufs_free(ctx);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_ಹೊರಹೋಗು_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer)
{
  ಹಿಂತಿರುಗಿ cw_ಹೊರಹೋಗು_do_flush(ದತ್ತಾಂಶ, writer, FALSE);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype Curl_cwt_ಹೊರಹೋಗು = {
  "cw-out",
  ಖಾಲಿ,
  0,
  cw_ಹೊರಹೋಗು_init,
  cw_ಹೊರಹೋಗು_write,
  cw_ಹೊರಹೋಗು_flush,
  cw_ಹೊರಹೋಗು_close,
  ಗಾತ್ರ(ರಚನೆ cw_ಹೊರಹೋಗು_ctx)
};

CURLcode Curl_cw_ಹೊರಹೋಗು_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cwriter *cw_ಹೊರಹೋಗು;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  cw_ಹೊರಹೋಗು = Curl_cwriter_get_by_type(ದತ್ತಾಂಶ, &Curl_cwt_ಹೊರಹೋಗು);
  ಆದರೆ(cw_ಹೊರಹೋಗು) {
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[OUT] done");
    ಫಲಿತಾಂಶ = Curl_client_flush(ದತ್ತಾಂಶ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = cw_ಹೊರಹೋಗು_do_flush(ದತ್ತಾಂಶ, cw_ಹೊರಹೋಗು, TRUE);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
