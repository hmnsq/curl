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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

ಸಂರಚನಾ cw_out_buf {
  ಸಂರಚನಾ cw_out_buf *next;
  ಸಂರಚನಾ dynbuf b;
  cw_out_type type;
};

ಸ್ಥಿರ ಸಂರಚನಾ cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  ಸಂರಚನಾ cw_out_buf *cwbuf = curlx_calloc(1, ಪರಿಮಾಣ(*cwbuf));
  ಯದಿ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  ಪ್ರತ್ಯಾಗಮ cwbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_out_buf_free(ಸಂರಚನಾ cw_out_buf *cwbuf)
{
  ಯದಿ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ಸಂರಚನಾ cw_out_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
  ಸಂರಚನಾ cw_out_buf *buf;
  ಸಂರಚನಾ cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

ಸ್ಥಿರ CURLcode cw_out_init(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ cw_out_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)data;
  ctx->buf = ರಿಕ್ತ;
  ctx->tail = ರಿಕ್ತ;
  ctx->buffered_len = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_out_bufs_free(ಸಂರಚನಾ cw_out_ctx *ctx)
{
  ಯಾವತ್(ctx->buf) {
    ಸಂರಚನಾ cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ರಿಕ್ತ;
  ctx->buffered_len = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_out_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ cw_out_ctx *ctx = writer->ctx;

  (ಶೂನ್ಯ)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ಸ್ಥಿರ ಶೂನ್ಯ cw_get_writefunc(ಸಂರಚನಾ Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, ಶೂನ್ಯ **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  ವಿಕಲ್ಪ(otype) {
  ಪಕ್ಷ CW_OUT_BODY:
  ಪಕ್ಷ CW_OUT_BODY_0LEN:
    ಯದಿ(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    ಅನ್ಯಥಾ {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ಭಙ್ಗ;
  ಪಕ್ಷ CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : ರಿಕ್ತ);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    *pwcb = ರಿಕ್ತ;
    *pwcb_data = ರಿಕ್ತ;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

ಸ್ಥಿರ CURLcode cw_out_cb_write(ಸಂರಚನಾ Curl_easy *data,
                                curl_write_callback wcb,
                                ಶೂನ್ಯ *wcb_data,
                                cw_out_type otype,
                                ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((ಶೂನ್ಯ)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((ವರ್ಣ *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned ಪೂರ್ಣಾಙ್ಕ)otype, nwritten);
  ಯದಿ(nwritten == CURL_WRITEFUNC_PAUSE) {
    ಯದಿ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    ಪ್ರತ್ಯಾಗಮ result ? result : CURLE_AGAIN;
  }
  ಅನ್ಯಥಾ ಯದಿ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_out_ptr_flush(ಸಂರಚನಾ cw_out_ctx *ctx,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ರಿಕ್ತ;
  ಶೂನ್ಯ *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ಯದಿ(ctx->errored)
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  ಯದಿ(!wcb) {
    *pconsumed = blen;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *pconsumed = 0;
  ಯದಿ(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    ಪ್ರತ್ಯಾಗಮ cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  ಅನ್ಯಥಾ {
    ಯಾವತ್(blen && !data->req.writer.paused) {
      ಯದಿ(!flush_all && blen < min_write)
        ಭಙ್ಗ;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ಯದಿ(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      ಯದಿ(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      ಯದಿ(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      ಯದಿ(!wcb) {
        *pconsumed += blen;
        ಭಙ್ಗ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_out_buf_flush(ಸಂರಚನಾ cw_out_ctx *ctx,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ಯದಿ(result && (result != CURLE_AGAIN))
      ಪ್ರತ್ಯಾಗಮ result;
    result = CURLE_OK;

    ಯದಿ(consumed) {
      ಯದಿ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      ಅನ್ಯಥಾ {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cw_out_flush_chain(ಸಂರಚನಾ cw_out_ctx *ctx,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  ಯಾವತ್(ctx->buf) {
    ಸಂರಚನಾ cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ಯದಿ(data->req.writer.paused)
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ಯದಿ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    ಯದಿ(!ctx->buf)
      ctx->tail = ರಿಕ್ತ;
    cw_out_buf_free(cwbuf);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_out_append(ಸಂರಚನಾ cw_out_ctx *ctx,
                              ಸಂರಚನಾ Curl_easy *data,
                              cw_out_type otype,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  ಯದಿ(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ಯದಿ(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    ಸಂರಚನಾ cw_out_buf *cwbuf = cw_out_buf_create(otype);
    ಯದಿ(!cwbuf)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(ctx->tail)
      ctx->tail->next = cwbuf;
    ಅನ್ಯಥಾ
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  ಯದಿ(!result)
    ctx->buffered_len += blen;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cw_out_do_write(ಸಂರಚನಾ cw_out_ctx *ctx,
                                ಸಂರಚನಾ Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ಯದಿ(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    ಯದಿ(result && (result != CURLE_AGAIN))
      ಪ್ರತ್ಯಾಗಮ result;
    result = CURLE_OK;
    ಯದಿ(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }

out:
  ಯದಿ(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cw_out_write(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                             ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ಯದಿ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_out_do_flush(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *cw_out,
                                bool flush_all)
{
  ಸಂರಚನಾ cw_out_ctx *ctx = (ಸಂರಚನಾ cw_out_ctx *)cw_out;

  ಯದಿ(ctx->errored)
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;

  ಯದಿ(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    ಯದಿ(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cw_out_flush(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಪ್ರತ್ಯಾಗಮ cw_out_do_flush(data, writer, FALSE);
}

ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype Curl_cwt_out = {
  "cw-out",
  ರಿಕ್ತ,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ cw_out_ctx)
};

CURLcode Curl_cw_out_done(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  ಯದಿ(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    ಯದಿ(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
