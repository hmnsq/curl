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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "cw-out.h"
#ଅନ୍ତର୍ଭାଵ "cw-pause.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

ସଂରଚନା cw_out_buf {
  ସଂରଚନା cw_out_buf *next;
  ସଂରଚନା dynbuf b;
  cw_out_type type;
};

ସ୍ଥିର ସଂରଚନା cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  ସଂରଚନା cw_out_buf *cwbuf = curlx_calloc(1, ଆକାର(*cwbuf));
  ଯଦି(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  ଫେରନ୍ତୁ cwbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_out_buf_free(ସଂରଚନା cw_out_buf *cwbuf)
{
  ଯଦି(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ସଂରଚନା cw_out_ctx {
  ସଂରଚନା Curl_cwriter super;
  ସଂରଚନା cw_out_buf *buf;
  ସଂରଚନା cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

ସ୍ଥିର CURLcode cw_out_init(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା cw_out_ctx *ctx = writer->ctx;
  (ଶୂନ୍ୟ)data;
  ctx->buf = ଖାଲି;
  ctx->tail = ଖାଲି;
  ctx->buffered_len = 0;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_out_bufs_free(ସଂରଚନା cw_out_ctx *ctx)
{
  ଯେତେବେଳେ(ctx->buf) {
    ସଂରଚନା cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ଖାଲି;
  ctx->buffered_len = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_out_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା cw_out_ctx *ctx = writer->ctx;

  (ଶୂନ୍ୟ)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
ସ୍ଥିର ଶୂନ୍ୟ cw_get_writefunc(ସଂରଚନା Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, ଶୂନ୍ୟ **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  ବାଛ(otype) {
  ସ୍ଥିତି CW_OUT_BODY:
  ସ୍ଥିତି CW_OUT_BODY_0LEN:
    ଯଦି(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    ଅନ୍ୟଥା {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : ଖାଲି);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    *pwcb = ଖାଲି;
    *pwcb_data = ଖାଲି;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

ସ୍ଥିର CURLcode cw_out_cb_write(ସଂରଚନା Curl_easy *data,
                                curl_write_callback wcb,
                                ଶୂନ୍ୟ *wcb_data,
                                cw_out_type otype,
                                ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((ଶୂନ୍ୟ)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((ଅକ୍ଷର *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)otype, nwritten);
  ଯଦି(nwritten == CURL_WRITEFUNC_PAUSE) {
    ଯଦି(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    ଫେରନ୍ତୁ result ? result : CURLE_AGAIN;
  }
  ଅନ୍ୟଥା ଯଦି(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cw_out_ptr_flush(ସଂରଚନା cw_out_ctx *ctx,
                                 ସଂରଚନା Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ଖାଲି;
  ଶୂନ୍ୟ *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ଯଦି(ctx->errored)
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  ଯଦି(!wcb) {
    *pconsumed = blen;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *pconsumed = 0;
  ଯଦି(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    ଫେରନ୍ତୁ cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  ଅନ୍ୟଥା {
    ଯେତେବେଳେ(blen && !data->req.writer.paused) {
      ଯଦି(!flush_all && blen < min_write)
        ଭାଙ୍ଗ;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ଯଦି(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      ଯଦି(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      ଯଦି(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      ଯଦି(!wcb) {
        *pconsumed += blen;
        ଭାଙ୍ଗ;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cw_out_buf_flush(ସଂରଚନା cw_out_ctx *ctx,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  ଯଦି(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ଯଦି(result && (result != CURLE_AGAIN))
      ଫେରନ୍ତୁ result;
    result = CURLE_OK;

    ଯଦି(consumed) {
      ଯଦି(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      ଅନ୍ୟଥା {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cw_out_flush_chain(ସଂରଚନା cw_out_ctx *ctx,
                                   ସଂରଚନା Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  ଯେତେବେଳେ(ctx->buf) {
    ସଂରଚନା cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ଯଦି(data->req.writer.paused)
      ଫେରନ୍ତୁ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ଯଦି(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      ଫେରନ୍ତୁ CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    ଯଦି(!ctx->buf)
      ctx->tail = ଖାଲି;
    cw_out_buf_free(cwbuf);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cw_out_append(ସଂରଚନା cw_out_ctx *ctx,
                              ସଂରଚନା Curl_easy *data,
                              cw_out_type otype,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  ଯଦି(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ଯଦି(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    ସଂରଚନା cw_out_buf *cwbuf = cw_out_buf_create(otype);
    ଯଦି(!cwbuf)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(ctx->tail)
      ctx->tail->next = cwbuf;
    ଅନ୍ୟଥା
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  ଯଦି(!result)
    ctx->buffered_len += blen;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cw_out_do_write(ସଂରଚନା cw_out_ctx *ctx,
                                ସଂରଚନା Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ଯଦି(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    ଯଦି(result)
      ଯାଅ out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    ଯଦି(result && (result != CURLE_AGAIN))
      ଫେରନ୍ତୁ result;
    result = CURLE_OK;
    ଯଦି(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      ଯଦି(result)
        ଯାଅ out;
    }
  }

out:
  ଯଦି(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cw_out_write(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                             ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ଯଦି((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cw_out_do_flush(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *cw_out,
                                bool flush_all)
{
  ସଂରଚନା cw_out_ctx *ctx = (ସଂରଚନା cw_out_ctx *)cw_out;

  ଯଦି(ctx->errored)
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;

  ଯଦି(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    ଯଦି(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      ଫେରନ୍ତୁ result;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cw_out_flush(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer)
{
  ଫେରନ୍ତୁ cw_out_do_flush(data, writer, FALSE);
}

ଅଚଳ ସଂରଚନା Curl_cwtype Curl_cwt_out = {
  "cw-out",
  ଖାଲି,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  ଆକାର(ସଂରଚନା cw_out_ctx)
};

CURLcode Curl_cw_out_done(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  ଯଦି(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    ଯଦି(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  ଫେରନ୍ତୁ result;
}
