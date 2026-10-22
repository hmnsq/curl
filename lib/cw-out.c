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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "cw-out.h"
#অন্তর্ভাৱ "cw-pause.h"
#অন্তর্ভাৱ "progress.h"

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
প্রকারনির্দেশ গণনা {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

সংরচনা cw_out_buf {
  সংরচনা cw_out_buf *next;
  সংরচনা dynbuf b;
  cw_out_type type;
};

স্থির সংরচনা cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  সংরচনা cw_out_buf *cwbuf = curlx_calloc(1, পরিমাণ(*cwbuf));
  যদি(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  ঘূৰাই cwbuf;
}

স্থির শূন্য cw_out_buf_free(সংরচনা cw_out_buf *cwbuf)
{
  যদি(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

সংরচনা cw_out_ctx {
  সংরচনা Curl_cwriter super;
  সংরচনা cw_out_buf *buf;
  সংরচনা cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

স্থির CURLcode cw_out_init(সংরচনা Curl_easy *data,
                            সংরচনা Curl_cwriter *writer)
{
  সংরচনা cw_out_ctx *ctx = writer->ctx;
  (শূন্য)data;
  ctx->buf = রিক্ত;
  ctx->tail = রিক্ত;
  ctx->buffered_len = 0;
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cw_out_bufs_free(সংরচনা cw_out_ctx *ctx)
{
  যেতিয়ালৈ(ctx->buf) {
    সংরচনা cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = রিক্ত;
  ctx->buffered_len = 0;
}

স্থির শূন্য cw_out_close(সংরচনা Curl_easy *data, সংরচনা Curl_cwriter *writer)
{
  সংরচনা cw_out_ctx *ctx = writer->ctx;

  (শূন্য)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
স্থির শূন্য cw_get_writefunc(সংরচনা Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, শূন্য **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  ৱিকল্প(otype) {
  পক্ষ CW_OUT_BODY:
  পক্ষ CW_OUT_BODY_0LEN:
    যদি(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    অন্যথা {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ভঙ্গ;
  পক্ষ CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : রিক্ত);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ভঙ্গ;
  স্ৱভাৱ:
    *pwcb = রিক্ত;
    *pwcb_data = রিক্ত;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

স্থির CURLcode cw_out_cb_write(সংরচনা Curl_easy *data,
                                curl_write_callback wcb,
                                শূন্য *wcb_data,
                                cw_out_type otype,
                                ধ্রুৱ ৱর্ণ *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((শূন্য)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((ৱর্ণ *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned পূর্ণাঙ্ক)otype, nwritten);
  যদি(nwritten == CURL_WRITEFUNC_PAUSE) {
    যদি(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      ঘূৰাই CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    ঘূৰাই result ? result : CURLE_AGAIN;
  }
  অন্যথা যদি(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    ঘূৰাই CURLE_WRITE_ERROR;
  }
  অন্যথা যদি(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ঘূৰাই CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cw_out_ptr_flush(সংরচনা cw_out_ctx *ctx,
                                 সংরচনা Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ধ্রুৱ ৱর্ণ *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = রিক্ত;
  শূন্য *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  যদি(ctx->errored)
    ঘূৰাই CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  যদি(!wcb) {
    *pconsumed = blen;
    ঘূৰাই CURLE_OK;
  }

  *pconsumed = 0;
  যদি(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    ঘূৰাই cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  অন্যথা {
    যেতিয়ালৈ(blen && !data->req.writer.paused) {
      যদি(!flush_all && blen < min_write)
        ভঙ্গ;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      যদি(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      যদি(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      যদি(result)
        ঘূৰাই result;
      যদি(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      যদি(!wcb) {
        *pconsumed += blen;
        ভঙ্গ;
      }
    }
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cw_out_buf_flush(সংরচনা cw_out_ctx *ctx,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  যদি(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    যদি(result && (result != CURLE_AGAIN))
      ঘূৰাই result;
    result = CURLE_OK;

    যদি(consumed) {
      যদি(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      অন্যথা {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        যদি(result)
          ঘূৰাই result;
      }
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode cw_out_flush_chain(সংরচনা cw_out_ctx *ctx,
                                   সংরচনা Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  যেতিয়ালৈ(ctx->buf) {
    সংরচনা cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    যদি(data->req.writer.paused)
      ঘূৰাই CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    যদি(result)
      ঘূৰাই result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    যদি(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      ঘূৰাই CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    যদি(!ctx->buf)
      ctx->tail = রিক্ত;
    cw_out_buf_free(cwbuf);
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cw_out_append(সংরচনা cw_out_ctx *ctx,
                              সংরচনা Curl_easy *data,
                              cw_out_type otype,
                              ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  যদি(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ঘূৰাই CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  যদি(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    সংরচনা cw_out_buf *cwbuf = cw_out_buf_create(otype);
    যদি(!cwbuf)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    যদি(ctx->tail)
      ctx->tail->next = cwbuf;
    অন্যথা
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  যদি(!result)
    ctx->buffered_len += blen;
  ঘূৰাই result;
}

স্থির CURLcode cw_out_do_write(সংরচনা cw_out_ctx *ctx,
                                সংরচনা Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  যদি(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    যদি(result)
      গচ্ছ out;
  }

  যদি(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    যদি(result)
      গচ্ছ out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    যদি(result && (result != CURLE_AGAIN))
      ঘূৰাই result;
    result = CURLE_OK;
    যদি(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      যদি(result)
        গচ্ছ out;
    }
  }

out:
  যদি(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  ঘূৰাই result;
}

স্থির CURLcode cw_out_write(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                             ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  সংরচনা cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  যদি((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    যদি(result)
      ঘূৰাই result;
  }

  যদি(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    যদি(result)
      ঘূৰাই result;
  }

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cw_out_do_flush(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *cw_out,
                                bool flush_all)
{
  সংরচনা cw_out_ctx *ctx = (সংরচনা cw_out_ctx *)cw_out;

  যদি(ctx->errored)
    ঘূৰাই CURLE_WRITE_ERROR;

  যদি(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    যদি(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      ঘূৰাই result;
    }
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cw_out_flush(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer)
{
  ঘূৰাই cw_out_do_flush(data, writer, FALSE);
}

ধ্রুৱ সংরচনা Curl_cwtype Curl_cwt_out = {
  "cw-out",
  রিক্ত,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  পরিমাণ(সংরচনা cw_out_ctx)
};

CURLcode Curl_cw_out_done(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  যদি(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    যদি(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  ঘূৰাই result;
}
