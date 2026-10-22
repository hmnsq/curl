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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "cw-out.h"
#അന്തര്ഭാവ "cw-pause.h"
#അന്തര്ഭാവ "progress.h"

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
തരനിർവചനം എണ്ണൽ {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

ഘടന cw_out_buf {
  ഘടന cw_out_buf *next;
  ഘടന dynbuf b;
  cw_out_type type;
};

സ്ഥിതം ഘടന cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  ഘടന cw_out_buf *cwbuf = curlx_calloc(1, വലുപ്പം(*cwbuf));
  എങ്കിൽ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  തിരികെ cwbuf;
}

സ്ഥിതം ശൂന്യം cw_out_buf_free(ഘടന cw_out_buf *cwbuf)
{
  എങ്കിൽ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ഘടന cw_out_ctx {
  ഘടന Curl_cwriter super;
  ഘടന cw_out_buf *buf;
  ഘടന cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

സ്ഥിതം CURLcode cw_out_init(ഘടന Curl_easy *data,
                            ഘടന Curl_cwriter *writer)
{
  ഘടന cw_out_ctx *ctx = writer->ctx;
  (ശൂന്യം)data;
  ctx->buf = ശൂന്യ;
  ctx->tail = ശൂന്യ;
  ctx->buffered_len = 0;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cw_out_bufs_free(ഘടന cw_out_ctx *ctx)
{
  ആകുമ്പോൾ(ctx->buf) {
    ഘടന cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = ശൂന്യ;
  ctx->buffered_len = 0;
}

സ്ഥിതം ശൂന്യം cw_out_close(ഘടന Curl_easy *data, ഘടന Curl_cwriter *writer)
{
  ഘടന cw_out_ctx *ctx = writer->ctx;

  (ശൂന്യം)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
സ്ഥിതം ശൂന്യം cw_get_writefunc(ഘടന Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, ശൂന്യം **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  തിരഞ്ഞെടുപ്പ്(otype) {
  അവസ്ഥ CW_OUT_BODY:
  അവസ്ഥ CW_OUT_BODY_0LEN:
    എങ്കിൽ(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    അല്ലെങ്കിൽ {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    നിർത്തുക;
  അവസ്ഥ CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : ശൂന്യ);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    *pwcb = ശൂന്യ;
    *pwcb_data = ശൂന്യ;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

സ്ഥിതം CURLcode cw_out_cb_write(ഘടന Curl_easy *data,
                                curl_write_callback wcb,
                                ശൂന്യം *wcb_data,
                                cw_out_type otype,
                                സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((ശൂന്യം)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((അക്ഷരം *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned പൂർണ്ണസംഖ്യ)otype, nwritten);
  എങ്കിൽ(nwritten == CURL_WRITEFUNC_PAUSE) {
    എങ്കിൽ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      തിരികെ CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    തിരികെ result ? result : CURLE_AGAIN;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    തിരികെ CURLE_WRITE_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    തിരികെ CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cw_out_ptr_flush(ഘടന cw_out_ctx *ctx,
                                 ഘടന Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ശൂന്യ;
  ശൂന്യം *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  എങ്കിൽ(ctx->errored)
    തിരികെ CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  എങ്കിൽ(!wcb) {
    *pconsumed = blen;
    തിരികെ CURLE_OK;
  }

  *pconsumed = 0;
  എങ്കിൽ(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    തിരികെ cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  അല്ലെങ്കിൽ {
    ആകുമ്പോൾ(blen && !data->req.writer.paused) {
      എങ്കിൽ(!flush_all && blen < min_write)
        നിർത്തുക;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      എങ്കിൽ(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      എങ്കിൽ(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      എങ്കിൽ(result)
        തിരികെ result;
      എങ്കിൽ(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      എങ്കിൽ(!wcb) {
        *pconsumed += blen;
        നിർത്തുക;
      }
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cw_out_buf_flush(ഘടന cw_out_ctx *ctx,
                                 ഘടന Curl_easy *data,
                                 ഘടന cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    എങ്കിൽ(result && (result != CURLE_AGAIN))
      തിരികെ result;
    result = CURLE_OK;

    എങ്കിൽ(consumed) {
      എങ്കിൽ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      അല്ലെങ്കിൽ {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cw_out_flush_chain(ഘടന cw_out_ctx *ctx,
                                   ഘടന Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  ആകുമ്പോൾ(ctx->buf) {
    ഘടന cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    എങ്കിൽ(data->req.writer.paused)
      തിരികെ CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    എങ്കിൽ(result)
      തിരികെ result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    എങ്കിൽ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      തിരികെ CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    എങ്കിൽ(!ctx->buf)
      ctx->tail = ശൂന്യ;
    cw_out_buf_free(cwbuf);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cw_out_append(ഘടന cw_out_ctx *ctx,
                              ഘടന Curl_easy *data,
                              cw_out_type otype,
                              സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  എങ്കിൽ(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    തിരികെ CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  എങ്കിൽ(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    ഘടന cw_out_buf *cwbuf = cw_out_buf_create(otype);
    എങ്കിൽ(!cwbuf)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(ctx->tail)
      ctx->tail->next = cwbuf;
    അല്ലെങ്കിൽ
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  എങ്കിൽ(!result)
    ctx->buffered_len += blen;
  തിരികെ result;
}

സ്ഥിതം CURLcode cw_out_do_write(ഘടന cw_out_ctx *ctx,
                                ഘടന Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  എങ്കിൽ(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    എങ്കിൽ(result)
      പോകുക out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    എങ്കിൽ(result && (result != CURLE_AGAIN))
      തിരികെ result;
    result = CURLE_OK;
    എങ്കിൽ(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }

out:
  എങ്കിൽ(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cw_out_write(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                             സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  ഘടന cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  എങ്കിൽ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cw_out_do_flush(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *cw_out,
                                bool flush_all)
{
  ഘടന cw_out_ctx *ctx = (ഘടന cw_out_ctx *)cw_out;

  എങ്കിൽ(ctx->errored)
    തിരികെ CURLE_WRITE_ERROR;

  എങ്കിൽ(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    എങ്കിൽ(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      തിരികെ result;
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cw_out_flush(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer)
{
  തിരികെ cw_out_do_flush(data, writer, FALSE);
}

സ്ഥിരം ഘടന Curl_cwtype Curl_cwt_out = {
  "cw-out",
  ശൂന്യ,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  വലുപ്പം(ഘടന cw_out_ctx)
};

CURLcode Curl_cw_out_done(ഘടന Curl_easy *data)
{
  ഘടന Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  എങ്കിൽ(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    എങ്കിൽ(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  തിരികെ result;
}
