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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "cw-out.h"
#அந்தர்பாவ "cw-pause.h"
#அந்தர்பாவ "progress.h"

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
வகைவரை எண்ணிக்கை {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

கட்டமைப்பு cw_out_buf {
  கட்டமைப்பு cw_out_buf *next;
  கட்டமைப்பு dynbuf b;
  cw_out_type type;
};

நிலையான கட்டமைப்பு cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  கட்டமைப்பு cw_out_buf *cwbuf = curlx_calloc(1, அளவு(*cwbuf));
  எனில்(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  திரும்பு cwbuf;
}

நிலையான வெறுமை cw_out_buf_free(கட்டமைப்பு cw_out_buf *cwbuf)
{
  எனில்(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

கட்டமைப்பு cw_out_ctx {
  கட்டமைப்பு Curl_cwriter super;
  கட்டமைப்பு cw_out_buf *buf;
  கட்டமைப்பு cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

நிலையான CURLcode cw_out_init(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு cw_out_ctx *ctx = writer->ctx;
  (வெறுமை)data;
  ctx->buf = வெற்று;
  ctx->tail = வெற்று;
  ctx->buffered_len = 0;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cw_out_bufs_free(கட்டமைப்பு cw_out_ctx *ctx)
{
  போது(ctx->buf) {
    கட்டமைப்பு cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = வெற்று;
  ctx->buffered_len = 0;
}

நிலையான வெறுமை cw_out_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு cw_out_ctx *ctx = writer->ctx;

  (வெறுமை)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
நிலையான வெறுமை cw_get_writefunc(கட்டமைப்பு Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, வெறுமை **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  தேர்வு(otype) {
  நிலை CW_OUT_BODY:
  நிலை CW_OUT_BODY_0LEN:
    எனில்(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    இல்லையெனில் {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    நிறுத்து;
  நிலை CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : வெற்று);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    நிறுத்து;
  இயல்பு:
    *pwcb = வெற்று;
    *pwcb_data = வெற்று;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

நிலையான CURLcode cw_out_cb_write(கட்டமைப்பு Curl_easy *data,
                                curl_write_callback wcb,
                                வெறுமை *wcb_data,
                                cw_out_type otype,
                                மாறிலி எழுத்து *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((வெறுமை)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((எழுத்து *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned முழு)otype, nwritten);
  எனில்(nwritten == CURL_WRITEFUNC_PAUSE) {
    எனில்(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      திரும்பு CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    திரும்பு result ? result : CURLE_AGAIN;
  }
  இல்லையெனில் எனில்(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    திரும்பு CURLE_WRITE_ERROR;
  }
  இல்லையெனில் எனில்(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    திரும்பு CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cw_out_ptr_flush(கட்டமைப்பு cw_out_ctx *ctx,
                                 கட்டமைப்பு Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 மாறிலி எழுத்து *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = வெற்று;
  வெறுமை *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  எனில்(ctx->errored)
    திரும்பு CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  எனில்(!wcb) {
    *pconsumed = blen;
    திரும்பு CURLE_OK;
  }

  *pconsumed = 0;
  எனில்(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    திரும்பு cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  இல்லையெனில் {
    போது(blen && !data->req.writer.paused) {
      எனில்(!flush_all && blen < min_write)
        நிறுத்து;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      எனில்(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      எனில்(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      எனில்(result)
        திரும்பு result;
      எனில்(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      எனில்(!wcb) {
        *pconsumed += blen;
        நிறுத்து;
      }
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cw_out_buf_flush(கட்டமைப்பு cw_out_ctx *ctx,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  எனில்(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    எனில்(result && (result != CURLE_AGAIN))
      திரும்பு result;
    result = CURLE_OK;

    எனில்(consumed) {
      எனில்(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      இல்லையெனில் {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        எனில்(result)
          திரும்பு result;
      }
    }
  }
  திரும்பு result;
}

நிலையான CURLcode cw_out_flush_chain(கட்டமைப்பு cw_out_ctx *ctx,
                                   கட்டமைப்பு Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  போது(ctx->buf) {
    கட்டமைப்பு cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    எனில்(data->req.writer.paused)
      திரும்பு CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    எனில்(result)
      திரும்பு result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    எனில்(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      திரும்பு CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    எனில்(!ctx->buf)
      ctx->tail = வெற்று;
    cw_out_buf_free(cwbuf);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cw_out_append(கட்டமைப்பு cw_out_ctx *ctx,
                              கட்டமைப்பு Curl_easy *data,
                              cw_out_type otype,
                              மாறிலி எழுத்து *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  எனில்(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    திரும்பு CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  எனில்(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    கட்டமைப்பு cw_out_buf *cwbuf = cw_out_buf_create(otype);
    எனில்(!cwbuf)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(ctx->tail)
      ctx->tail->next = cwbuf;
    இல்லையெனில்
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  எனில்(!result)
    ctx->buffered_len += blen;
  திரும்பு result;
}

நிலையான CURLcode cw_out_do_write(கட்டமைப்பு cw_out_ctx *ctx,
                                கட்டமைப்பு Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                மாறிலி எழுத்து *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  எனில்(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    எனில்(result)
      செல் out;
  }

  எனில்(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    எனில்(result)
      செல் out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில் {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    எனில்(result && (result != CURLE_AGAIN))
      திரும்பு result;
    result = CURLE_OK;
    எனில்(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      எனில்(result)
        செல் out;
    }
  }

out:
  எனில்(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  திரும்பு result;
}

நிலையான CURLcode cw_out_write(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer, முழு type,
                             மாறிலி எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  எனில்((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    எனில்(result)
      திரும்பு result;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode cw_out_do_flush(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *cw_out,
                                bool flush_all)
{
  கட்டமைப்பு cw_out_ctx *ctx = (கட்டமைப்பு cw_out_ctx *)cw_out;

  எனில்(ctx->errored)
    திரும்பு CURLE_WRITE_ERROR;

  எனில்(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    எனில்(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      திரும்பு result;
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cw_out_flush(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer)
{
  திரும்பு cw_out_do_flush(data, writer, FALSE);
}

மாறிலி கட்டமைப்பு Curl_cwtype Curl_cwt_out = {
  "cw-out",
  வெற்று,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  அளவு(கட்டமைப்பு cw_out_ctx)
};

CURLcode Curl_cw_out_done(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  எனில்(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    எனில்(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  திரும்பு result;
}
