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
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

بنية cw_out_buf {
  بنية cw_out_buf *next;
  بنية dynbuf b;
  cw_out_type type;
};

ساكن بنية cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  بنية cw_out_buf *cwbuf = curlx_calloc(1, حجم(*cwbuf));
  אם(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  החזר cwbuf;
}

ساكن فراغ cw_out_buf_free(بنية cw_out_buf *cwbuf)
{
  אם(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

بنية cw_out_ctx {
  بنية Curl_cwriter super;
  بنية cw_out_buf *buf;
  بنية cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

ساكن CURLcode cw_out_init(بنية Curl_easy *data,
                            بنية Curl_cwriter *writer)
{
  بنية cw_out_ctx *ctx = writer->ctx;
  (فراغ)data;
  ctx->buf = عدم;
  ctx->tail = عدم;
  ctx->buffered_len = 0;
  החזר CURLE_OK;
}

ساكن فراغ cw_out_bufs_free(بنية cw_out_ctx *ctx)
{
  כלעוד(ctx->buf) {
    بنية cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = عدم;
  ctx->buffered_len = 0;
}

ساكن فراغ cw_out_close(بنية Curl_easy *data, بنية Curl_cwriter *writer)
{
  بنية cw_out_ctx *ctx = writer->ctx;

  (فراغ)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ساكن فراغ cw_get_writefunc(بنية Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, فراغ **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  بدل(otype) {
  حالة CW_OUT_BODY:
  حالة CW_OUT_BODY_0LEN:
    אם(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    אחרת {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    اكسر;
  حالة CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : عدم);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    اكسر;
  افتراضي:
    *pwcb = عدم;
    *pwcb_data = عدم;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

ساكن CURLcode cw_out_cb_write(بنية Curl_easy *data,
                                curl_write_callback wcb,
                                فراغ *wcb_data,
                                cw_out_type otype,
                                ثابت حرف *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((فراغ)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((حرف *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned صحيح)otype, nwritten);
  אם(nwritten == CURL_WRITEFUNC_PAUSE) {
    אם(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      החזר CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    החזר result ? result : CURLE_AGAIN;
  }
  אחרת אם(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    החזר CURLE_WRITE_ERROR;
  }
  אחרת אם(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    החזר CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  החזר CURLE_OK;
}

ساكن CURLcode cw_out_ptr_flush(بنية cw_out_ctx *ctx,
                                 بنية Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = عدم;
  فراغ *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  אם(ctx->errored)
    החזר CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  אם(!wcb) {
    *pconsumed = blen;
    החזר CURLE_OK;
  }

  *pconsumed = 0;
  אם(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    החזר cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  אחרת {
    כלעוד(blen && !data->req.writer.paused) {
      אם(!flush_all && blen < min_write)
        اكسر;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      אם(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      אם(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      אם(result)
        החזר result;
      אם(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      אם(!wcb) {
        *pconsumed += blen;
        اكسر;
      }
    }
  }
  החזר CURLE_OK;
}

ساكن CURLcode cw_out_buf_flush(بنية cw_out_ctx *ctx,
                                 بنية Curl_easy *data,
                                 بنية cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  אם(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    אם(result && (result != CURLE_AGAIN))
      החזר result;
    result = CURLE_OK;

    אם(consumed) {
      אם(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      אחרת {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        אם(result)
          החזר result;
      }
    }
  }
  החזר result;
}

ساكن CURLcode cw_out_flush_chain(بنية cw_out_ctx *ctx,
                                   بنية Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  כלעוד(ctx->buf) {
    بنية cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    אם(data->req.writer.paused)
      החזר CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    אם(result)
      החזר result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    אם(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      החזר CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    אם(!ctx->buf)
      ctx->tail = عدم;
    cw_out_buf_free(cwbuf);
  }
  החזר CURLE_OK;
}

ساكن CURLcode cw_out_append(بنية cw_out_ctx *ctx,
                              بنية Curl_easy *data,
                              cw_out_type otype,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  אם(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    החזר CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  אם(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    بنية cw_out_buf *cwbuf = cw_out_buf_create(otype);
    אם(!cwbuf)
      החזר CURLE_OUT_OF_MEMORY;
    אם(ctx->tail)
      ctx->tail->next = cwbuf;
    אחרת
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  אם(!result)
    ctx->buffered_len += blen;
  החזר result;
}

ساكن CURLcode cw_out_do_write(بنية cw_out_ctx *ctx,
                                بنية Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                ثابت حرف *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  אם(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    אם(result)
      اذهب out;
  }

  אם(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    אם(result)
      اذهب out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    אם(result)
      اذهب out;
  }
  אחרת {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    אם(result && (result != CURLE_AGAIN))
      החזר result;
    result = CURLE_OK;
    אם(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      אם(result)
        اذهب out;
    }
  }

out:
  אם(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  החזר result;
}

ساكن CURLcode cw_out_write(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t blen)
{
  بنية cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  אם((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    אם(result)
      החזר result;
  }

  אם(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    אם(result)
      החזר result;
  }

  החזר CURLE_OK;
}

ساكن CURLcode cw_out_do_flush(بنية Curl_easy *data,
                                بنية Curl_cwriter *cw_out,
                                bool flush_all)
{
  بنية cw_out_ctx *ctx = (بنية cw_out_ctx *)cw_out;

  אם(ctx->errored)
    החזר CURLE_WRITE_ERROR;

  אם(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    אם(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      החזר result;
    }
  }
  החזר CURLE_OK;
}

ساكن CURLcode cw_out_flush(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer)
{
  החזר cw_out_do_flush(data, writer, FALSE);
}

ثابت بنية Curl_cwtype Curl_cwt_out = {
  "cw-out",
  عدم,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  حجم(بنية cw_out_ctx)
};

CURLcode Curl_cw_out_done(بنية Curl_easy *data)
{
  بنية Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  אם(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    אם(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  החזר result;
}
