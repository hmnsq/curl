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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "cw-out.h"
#अन्तर्भाव "cw-pause.h"
#अन्तर्भाव "progress.h"

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
प्रकारनिर्देश गणना {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

संरचना cw_out_buf {
  संरचना cw_out_buf *next;
  संरचना dynbuf b;
  cw_out_type type;
};

स्थिर संरचना cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  संरचना cw_out_buf *cwbuf = curlx_calloc(1, परिमाण(*cwbuf));
  जइ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  प्रत्यागम cwbuf;
}

स्थिर शून्य cw_out_buf_free(संरचना cw_out_buf *cwbuf)
{
  जइ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

संरचना cw_out_ctx {
  संरचना Curl_cwriter super;
  संरचना cw_out_buf *buf;
  संरचना cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

स्थिर CURLcode cw_out_init(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer)
{
  संरचना cw_out_ctx *ctx = writer->ctx;
  (शून्य)data;
  ctx->buf = रिक्त;
  ctx->tail = रिक्त;
  ctx->buffered_len = 0;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cw_out_bufs_free(संरचना cw_out_ctx *ctx)
{
  जाव(ctx->buf) {
    संरचना cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = रिक्त;
  ctx->buffered_len = 0;
}

स्थिर शून्य cw_out_close(संरचना Curl_easy *data, संरचना Curl_cwriter *writer)
{
  संरचना cw_out_ctx *ctx = writer->ctx;

  (शून्य)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
स्थिर शून्य cw_get_writefunc(संरचना Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, शून्य **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  विकल्प(otype) {
  पक्ष CW_OUT_BODY:
  पक्ष CW_OUT_BODY_0LEN:
    जइ(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    अण्णहा {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    भङ्ग;
  पक्ष CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : रिक्त);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    भङ्ग;
  स्वभाव:
    *pwcb = रिक्त;
    *pwcb_data = रिक्त;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

स्थिर CURLcode cw_out_cb_write(संरचना Curl_easy *data,
                                curl_write_callback wcb,
                                शून्य *wcb_data,
                                cw_out_type otype,
                                ध्रुव वर्ण *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((शून्य)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((वर्ण *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned पूर्णाङ्क)otype, nwritten);
  जइ(nwritten == CURL_WRITEFUNC_PAUSE) {
    जइ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      प्रत्यागम CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    प्रत्यागम result ? result : CURLE_AGAIN;
  }
  अण्णहा जइ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    प्रत्यागम CURLE_WRITE_ERROR;
  }
  अण्णहा जइ(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    प्रत्यागम CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_out_ptr_flush(संरचना cw_out_ctx *ctx,
                                 संरचना Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ध्रुव वर्ण *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = रिक्त;
  शून्य *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  जइ(ctx->errored)
    प्रत्यागम CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  जइ(!wcb) {
    *pconsumed = blen;
    प्रत्यागम CURLE_OK;
  }

  *pconsumed = 0;
  जइ(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    प्रत्यागम cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  अण्णहा {
    जाव(blen && !data->req.writer.paused) {
      जइ(!flush_all && blen < min_write)
        भङ्ग;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      जइ(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      जइ(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      जइ(result)
        प्रत्यागम result;
      जइ(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      जइ(!wcb) {
        *pconsumed += blen;
        भङ्ग;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_out_buf_flush(संरचना cw_out_ctx *ctx,
                                 संरचना Curl_easy *data,
                                 संरचना cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  जइ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    जइ(result && (result != CURLE_AGAIN))
      प्रत्यागम result;
    result = CURLE_OK;

    जइ(consumed) {
      जइ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      अण्णहा {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        जइ(result)
          प्रत्यागम result;
      }
    }
  }
  प्रत्यागम result;
}

स्थिर CURLcode cw_out_flush_chain(संरचना cw_out_ctx *ctx,
                                   संरचना Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  जाव(ctx->buf) {
    संरचना cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    जइ(data->req.writer.paused)
      प्रत्यागम CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    जइ(result)
      प्रत्यागम result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    जइ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      प्रत्यागम CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    जइ(!ctx->buf)
      ctx->tail = रिक्त;
    cw_out_buf_free(cwbuf);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_out_append(संरचना cw_out_ctx *ctx,
                              संरचना Curl_easy *data,
                              cw_out_type otype,
                              ध्रुव वर्ण *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  जइ(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    प्रत्यागम CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  जइ(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    संरचना cw_out_buf *cwbuf = cw_out_buf_create(otype);
    जइ(!cwbuf)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    जइ(ctx->tail)
      ctx->tail->next = cwbuf;
    अण्णहा
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  जइ(!result)
    ctx->buffered_len += blen;
  प्रत्यागम result;
}

स्थिर CURLcode cw_out_do_write(संरचना cw_out_ctx *ctx,
                                संरचना Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                ध्रुव वर्ण *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  जइ(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    जइ(result)
      गच्छ out;
  }

  जइ(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    जइ(result)
      गच्छ out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    जइ(result)
      गच्छ out;
  }
  अण्णहा {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    जइ(result && (result != CURLE_AGAIN))
      प्रत्यागम result;
    result = CURLE_OK;
    जइ(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      जइ(result)
        गच्छ out;
    }
  }

out:
  जइ(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  प्रत्यागम result;
}

स्थिर CURLcode cw_out_write(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                             ध्रुव वर्ण *buf, size_t blen)
{
  संरचना cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  जइ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    जइ(result)
      प्रत्यागम result;
  }

  जइ(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    जइ(result)
      प्रत्यागम result;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_out_do_flush(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *cw_out,
                                bool flush_all)
{
  संरचना cw_out_ctx *ctx = (संरचना cw_out_ctx *)cw_out;

  जइ(ctx->errored)
    प्रत्यागम CURLE_WRITE_ERROR;

  जइ(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    जइ(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      प्रत्यागम result;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_out_flush(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  प्रत्यागम cw_out_do_flush(data, writer, FALSE);
}

ध्रुव संरचना Curl_cwtype Curl_cwt_out = {
  "cw-out",
  रिक्त,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  परिमाण(संरचना cw_out_ctx)
};

CURLcode Curl_cw_out_done(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  जइ(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    जइ(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  प्रत्यागम result;
}
