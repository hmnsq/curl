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
  यदि(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  फर्काऊ cwbuf;
}

स्थिर शून्य cw_out_buf_free(संरचना cw_out_buf *cwbuf)
{
  यदि(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

संरचना cw_out_ctx {
  संरचना Curl_cwriter super;
  संरचना cw_out_buf *buf;
  संरचना cw_out_buf *tail;
  माप_प्रकार buffered_len;
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
  फर्काऊ CURLE_OK;
}

स्थिर शून्य cw_out_bufs_free(संरचना cw_out_ctx *ctx)
{
  जबसम्म(ctx->buf) {
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
                             माप_प्रकार *pmax_write, माप_प्रकार *pmin_write)
{
  विकल्प(otype) {
  पक्ष CW_OUT_BODY:
  पक्ष CW_OUT_BODY_0LEN:
    यदि(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    अन्यथा {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    रोक;
  पक्ष CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : रिक्त);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    रोक;
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
                                अचर वर्ण *buf, माप_प्रकार blen,
                                माप_प्रकार *pnwritten)
{
  माप_प्रकार nwritten;
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
                 (अचिन्हित पूर्णाङ्क)otype, nwritten);
  यदि(nwritten == CURL_WRITEFUNC_PAUSE) {
    यदि(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      फर्काऊ CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    फर्काऊ result ? result : CURLE_AGAIN;
  }
  अन्यथा यदि(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    फर्काऊ CURLE_WRITE_ERROR;
  }
  अन्यथा यदि(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    फर्काऊ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cw_out_ptr_flush(संरचना cw_out_ctx *ctx,
                                 संरचना Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 अचर वर्ण *buf, माप_प्रकार blen,
                                 माप_प्रकार *pconsumed)
{
  curl_write_callback wcb = रिक्त;
  शून्य *wcb_data;
  माप_प्रकार max_write, min_write;
  माप_प्रकार wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  यदि(ctx->errored)
    फर्काऊ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  यदि(!wcb) {
    *pconsumed = blen;
    फर्काऊ CURLE_OK;
  }

  *pconsumed = 0;
  यदि(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    फर्काऊ cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  अन्यथा {
    जबसम्म(blen && !data->req.writer.paused) {
      यदि(!flush_all && blen < min_write)
        रोक;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      यदि(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      यदि(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      यदि(result)
        फर्काऊ result;
      यदि(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      यदि(!wcb) {
        *pconsumed += blen;
        रोक;
      }
    }
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cw_out_buf_flush(संरचना cw_out_ctx *ctx,
                                 संरचना Curl_easy *data,
                                 संरचना cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  यदि(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    माप_प्रकार consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    यदि(result && (result != CURLE_AGAIN))
      फर्काऊ result;
    result = CURLE_OK;

    यदि(consumed) {
      यदि(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      अन्यथा {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        यदि(result)
          फर्काऊ result;
      }
    }
  }
  फर्काऊ result;
}

स्थिर CURLcode cw_out_flush_chain(संरचना cw_out_ctx *ctx,
                                   संरचना Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  जबसम्म(ctx->buf) {
    संरचना cw_out_buf *cwbuf = ctx->buf;
    माप_प्रकार blen_before;

    यदि(data->req.writer.paused)
      फर्काऊ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    यदि(result)
      फर्काऊ result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    यदि(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      फर्काऊ CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    यदि(!ctx->buf)
      ctx->tail = रिक्त;
    cw_out_buf_free(cwbuf);
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cw_out_append(संरचना cw_out_ctx *ctx,
                              संरचना Curl_easy *data,
                              cw_out_type otype,
                              अचर वर्ण *buf, माप_प्रकार blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  यदि(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    फर्काऊ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  यदि(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    संरचना cw_out_buf *cwbuf = cw_out_buf_create(otype);
    यदि(!cwbuf)
      फर्काऊ CURLE_OUT_OF_MEMORY;
    यदि(ctx->tail)
      ctx->tail->next = cwbuf;
    अन्यथा
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  यदि(!result)
    ctx->buffered_len += blen;
  फर्काऊ result;
}

स्थिर CURLcode cw_out_do_write(संरचना cw_out_ctx *ctx,
                                संरचना Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                अचर वर्ण *buf, माप_प्रकार blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  यदि(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    यदि(result)
      गच्छ out;
  }

  यदि(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    यदि(result)
      गच्छ out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    यदि(result)
      गच्छ out;
  }
  अन्यथा {
    /* nothing buffered, try direct write */
    माप_प्रकार consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    यदि(result && (result != CURLE_AGAIN))
      फर्काऊ result;
    result = CURLE_OK;
    यदि(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      यदि(result)
        गच्छ out;
    }
  }

out:
  यदि(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  फर्काऊ result;
}

स्थिर CURLcode cw_out_write(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                             अचर वर्ण *buf, माप_प्रकार blen)
{
  संरचना cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  यदि((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    यदि(result)
      फर्काऊ result;
  }

  यदि(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    यदि(result)
      फर्काऊ result;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cw_out_do_flush(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *cw_out,
                                bool flush_all)
{
  संरचना cw_out_ctx *ctx = (संरचना cw_out_ctx *)cw_out;

  यदि(ctx->errored)
    फर्काऊ CURLE_WRITE_ERROR;

  यदि(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    यदि(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      फर्काऊ result;
    }
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cw_out_flush(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  फर्काऊ cw_out_do_flush(data, writer, FALSE);
}

अचर संरचना Curl_cwtype Curl_cwt_out = {
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
  यदि(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    यदि(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  फर्काऊ result;
}
