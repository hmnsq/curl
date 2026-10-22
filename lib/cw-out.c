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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "cw-out.h"
#అన్తర్భావ "cw-pause.h"
#అన్తర్భావ "progress.h"

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
రకనిర్వచనం గణన {
  CW_బహిర్గతం_NONE,
  CW_బహిర్గతం_BODY,
  CW_బహిర్గతం_BODY_0_LEN,
  CW_బహిర్గతం_HDS
} cw_బహిర్గతం_type;

నిర్మాణం cw_బహిర్గతం_buf {
  నిర్మాణం cw_బహిర్గతం_buf *తదుపరి;
  నిర్మాణం dynbuf b;
  cw_బహిర్గతం_type type;
};

స్థిర నిర్మాణం cw_బహిర్గతం_buf *cw_బహిర్గతం_buf_create(cw_బహిర్గతం_type otype)
{
  నిర్మాణం cw_బహిర్గతం_buf *cwbuf = curlx_calloc(1, పరిమాణం(*cwbuf));
  ఒకవేళ(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_బఫరు);
  }
  తిరిగి cwbuf;
}

స్థిర శూన్య cw_బహిర్గతం_buf_free(నిర్మాణం cw_బహిర్గతం_buf *cwbuf)
{
  ఒకవేళ(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

నిర్మాణం cw_బహిర్గతం_ctx {
  నిర్మాణం Curl_cwriter super;
  నిర్మాణం cw_బహిర్గతం_buf *buf;
  నిర్మాణం cw_బహిర్గతం_buf *తోక;
  size_t buffered_len;
  బిట్టు(errored);
};

స్థిర CURLcode cw_బహిర్గతం_init(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం cw_బహిర్గతం_ctx *ctx = writer->ctx;
  (శూన్య)దత్తాంశం;
  ctx->buf = ఖాళీ;
  ctx->తోక = ఖాళీ;
  ctx->buffered_len = 0;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cw_బహిర్గతం_bufs_free(నిర్మాణం cw_బహిర్గతం_ctx *ctx)
{
  అయితే(ctx->buf) {
    నిర్మాణం cw_బహిర్గతం_buf *తదుపరి = ctx->buf->తదుపరి;
    cw_బహిర్గతం_buf_free(ctx->buf);
    ctx->buf = తదుపరి;
  }
  ctx->తోక = ఖాళీ;
  ctx->buffered_len = 0;
}

స్థిర శూన్య cw_బహిర్గతం_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం cw_బహిర్గతం_ctx *ctx = writer->ctx;

  (శూన్య)దత్తాంశం;
  cw_బహిర్గతం_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
స్థిర శూన్య cw_get_writefunc(నిర్మాణం Curl_easy *దత్తాంశం, cw_బహిర్గతం_type otype,
                             curl_write_callback *pwcb, శూన్య **pwcb_దత్తాంశం,
                             size_t *pmax_write, size_t *pmin_write)
{
  ఎంపిక(otype) {
  స్థితి CW_బహిర్గతం_BODY:
  స్థితి CW_బహిర్గతం_BODY_0_LEN:
    ఒకవేళ(Curl_ftp_listmode(దత్తాంశం)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_దత్తాంశం = దత్తాంశం;
    }
    లేకపోతే {
      *pwcb = దత్తాంశం->సమితి.fwrite_func;
      *pwcb_దత్తాంశం = దత్తాంశం->సమితి.బహిర్గతం;
    }
    *pmax_write = CURL_MAX_WRITE_కొలత;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    ఆపు;
  స్థితి CW_బహిర్గతం_HDS:
    *pwcb = దత్తాంశం->సమితి.fwrite_శీర్షిక ? దత్తాంశం->సమితి.fwrite_శీర్షిక :
             (దత్తాంశం->సమితి.writeheader ? దత్తాంశం->సమితి.fwrite_func : ఖాళీ);
    *pwcb_దత్తాంశం = దత్తాంశం->సమితి.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    ఆపు;
  అప్రమేయ:
    *pwcb = ఖాళీ;
    *pwcb_దత్తాంశం = ఖాళీ;
    *pmax_write = CURL_MAX_WRITE_కొలత;
    *pmin_write = 0;
  }
}

స్థిర CURLcode cw_బహిర్గతం_cb_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                curl_write_callback wcb,
                                శూన్య *wcb_దత్తాంశం,
                                cw_బహిర్గతం_type otype,
                                స్థిరరాశి అక్షరం *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode ఫలితం;

  NOVERBOSE((శూన్య)otype);

  DEBUGASSERT(దత్తాంశం->conn);
  *pnwritten = 0;
  {
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_cw_బహిర్గతం_cb);
    nwritten = wcb((అక్షరం *)CURL_UNCONST(buf), 1, blen, wcb_దత్తాంశం);
    CURL_CBAPI_చివర(&guard);
  }
  CURL_TRC_WRITE(దత్తాంశం, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_బహిర్గతం_HDS) ? "header" : "body",
                 (గుర్తులేని పూర్ణాంకం)otype, nwritten);
  ఒకవేళ(nwritten == CURL_WRITEFUNC_PAUSE) {
    ఒకవేళ(దత్తాంశం->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(దత్తాంశం, "Write callback asked for PAUSE when not supported");
      తిరిగి CURLE_WRITE_ERROR;
    }
    దత్తాంశం->req.writer.paused = TRUE;
    CURL_TRC_WRITE(దత్తాంశం, "[OUT] PAUSE requested by client");
    ఫలితం = Curl_xfer_pause_recv(దత్తాంశం, TRUE);
    తిరిగి ఫలితం ? ఫలితం : CURLE_AGAIN;
  }
  లేకపోతే ఒకవేళ(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(దత్తాంశం, "client returned ERROR on write of %zu bytes", blen);
    తిరిగి CURLE_WRITE_ERROR;
  }
  లేకపోతే ఒకవేళ(nwritten != blen) {
    failf(దత్తాంశం, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    తిరిగి CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cw_బహిర్గతం_ptr_flush(నిర్మాణం cw_బహిర్గతం_ctx *ctx,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 cw_బహిర్గతం_type otype,
                                 bool flush_all,
                                 స్థిరరాశి అక్షరం *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ఖాళీ;
  శూన్య *wcb_దత్తాంశం;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode ఫలితం = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ఒకవేళ(ctx->errored)
    తిరిగి CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(దత్తాంశం, otype, &wcb, &wcb_దత్తాంశం, &max_write, &min_write);
  ఒకవేళ(!wcb) {
    *pconsumed = blen;
    తిరిగి CURLE_OK;
  }

  *pconsumed = 0;
  ఒకవేళ(otype == CW_బహిర్గతం_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    తిరిగి cw_బహిర్గతం_cb_write(దత్తాంశం, wcb, wcb_దత్తాంశం, otype,
                           buf, blen, &nwritten);
  }
  లేకపోతే {
    అయితే(blen && !దత్తాంశం->req.writer.paused) {
      ఒకవేళ(!flush_all && blen < min_write)
        ఆపు;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ఒకవేళ(otype == CW_బహిర్గతం_BODY)
        ఫలితం = Curl_pgrs_deliver_check(దత్తాంశం, wlen);
      ఒకవేళ(!ఫలితం)
        ఫలితం = cw_బహిర్గతం_cb_write(దత్తాంశం, wcb, wcb_దత్తాంశం, otype,
                                 buf, wlen, &nwritten);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఒకవేళ(otype == CW_బహిర్గతం_BODY)
        Curl_pgrs_deliver_inc(దత్తాంశం, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(దత్తాంశం, otype, &wcb, &wcb_దత్తాంశం, &max_write, &min_write);
      ఒకవేళ(!wcb) {
        *pconsumed += blen;
        ఆపు;
      }
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cw_బహిర్గతం_buf_flush(నిర్మాణం cw_బహిర్గతం_ctx *ctx,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cw_బహిర్గతం_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_బహిర్గతం_BODY_0_LEN)) {
    size_t consumed;

    ఫలితం = cw_బహిర్గతం_ptr_flush(ctx, దత్తాంశం, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
      తిరిగి ఫలితం;
    ఫలితం = CURLE_OK;

    ఒకవేళ(consumed) {
      ఒకవేళ(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      లేకపోతే {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        ఫలితం = curlx_dyn_తోక(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cw_బహిర్గతం_flush_chain(నిర్మాణం cw_బహిర్గతం_ctx *ctx,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool flush_all)
{
  CURLcode ఫలితం;

  /* write the chain oldest first until it blocks or gets empty */
  అయితే(ctx->buf) {
    నిర్మాణం cw_బహిర్గతం_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ఒకవేళ(దత్తాంశం->req.writer.paused)
      తిరిగి CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    ఫలితం = cw_బహిర్గతం_buf_flush(ctx, దత్తాంశం, cwbuf, flush_all);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ఒకవేళ(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(దత్తాంశం->req.writer.paused);
      తిరిగి CURLE_OK;
    }

    ctx->buf = cwbuf->తదుపరి;
    ఒకవేళ(!ctx->buf)
      ctx->తోక = ఖాళీ;
    cw_బహిర్గతం_buf_free(cwbuf);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cw_బహిర్గతం_append(నిర్మాణం cw_బహిర్గతం_ctx *ctx,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              cw_బహిర్గతం_type otype,
                              స్థిరరాశి అక్షరం *buf, size_t blen)
{
  CURLcode ఫలితం;

  CURL_TRC_WRITE(దత్తాంశం, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_బఫరు);
  ఒకవేళ(ctx->buffered_len + blen > DYN_PAUSE_బఫరు) {
    failf(దత్తాంశం, "pause buffer not large enough -> CURLE_TOO_LARGE");
    తిరిగి CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ఒకవేళ(!ctx->తోక || (ctx->తోక->type != otype) || (otype == CW_బహిర్గతం_HDS)) {
    నిర్మాణం cw_బహిర్గతం_buf *cwbuf = cw_బహిర్గతం_buf_create(otype);
    ఒకవేళ(!cwbuf)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(ctx->తోక)
      ctx->తోక->తదుపరి = cwbuf;
    లేకపోతే
      ctx->buf = cwbuf;
    ctx->తోక = cwbuf;
  }
  DEBUGASSERT(ctx->తోక && (ctx->తోక->type == otype));
  ఫలితం = curlx_dyn_addn(&ctx->తోక->b, buf, blen);
  ఒకవేళ(!ఫలితం)
    ctx->buffered_len += blen;
  తిరిగి ఫలితం;
}

స్థిర CURLcode cw_బహిర్గతం_do_write(నిర్మాణం cw_బహిర్గతం_ctx *ctx,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                cw_బహిర్గతం_type otype,
                                bool flush_all,
                                స్థిరరాశి అక్షరం *buf, size_t blen)
{
  CURLcode ఫలితం = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ఒకవేళ(ctx->తోక && ctx->తోక->type != otype) {
    ఫలితం = cw_బహిర్గతం_flush_chain(ctx, దత్తాంశం, TRUE);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ctx->buf) {
    /* still have buffered data, append and flush */
    ఫలితం = cw_బహిర్గతం_append(ctx, దత్తాంశం, otype, buf, blen);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఫలితం = cw_బహిర్గతం_flush_chain(ctx, దత్తాంశం, flush_all);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    /* nothing buffered, try direct write */
    size_t consumed;
    ఫలితం = cw_బహిర్గతం_ptr_flush(ctx, దత్తాంశం, otype, flush_all,
                              buf, blen, &consumed);
    ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
      తిరిగి ఫలితం;
    ఫలితం = CURLE_OK;
    ఒకవేళ(consumed < blen) {
      /* did not write all, append the rest */
      ఫలితం = cw_బహిర్గతం_append(ctx, దత్తాంశం, otype,
                             buf + consumed, blen - consumed);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_బహిర్గతం_bufs_free(ctx);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cw_బహిర్గతం_write(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                             స్థిరరాశి అక్షరం *buf, size_t blen)
{
  నిర్మాణం cw_బహిర్గతం_ctx *ctx = writer->ctx;
  CURLcode ఫలితం;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ఒకవేళ((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_శీర్షిక) && దత్తాంశం->సమితి.include_శీర్షిక)) {
    cw_బహిర్గతం_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_బహిర్గతం_BODY_0_LEN : CW_బహిర్గతం_BODY;
    ఫలితం = cw_బహిర్గతం_do_write(ctx, దత్తాంశం, otype, flush_all, buf, blen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(type & (CLIENTWRITE_శీర్షిక | CLIENTWRITE_INFO)) {
    ఫలితం = cw_బహిర్గతం_do_write(ctx, దత్తాంశం, CW_బహిర్గతం_HDS, flush_all, buf, blen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode cw_బహిర్గతం_do_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *cw_బహిర్గతం,
                                bool flush_all)
{
  నిర్మాణం cw_బహిర్గతం_ctx *ctx = (నిర్మాణం cw_బహిర్గతం_ctx *)cw_బహిర్గతం;

  ఒకవేళ(ctx->errored)
    తిరిగి CURLE_WRITE_ERROR;

  ఒకవేళ(!దత్తాంశం->req.writer.paused && ctx->buf) {
    CURLcode ఫలితం;

    CURL_TRC_WRITE(దత్తాంశం, "[OUT] flush");
    ఫలితం = cw_బహిర్గతం_flush_chain(ctx, దత్తాంశం, flush_all);
    ఒకవేళ(ఫలితం) {
      ctx->errored = TRUE;
      cw_బహిర్గతం_bufs_free(ctx);
      తిరిగి ఫలితం;
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cw_బహిర్గతం_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer)
{
  తిరిగి cw_బహిర్గతం_do_flush(దత్తాంశం, writer, FALSE);
}

స్థిరరాశి నిర్మాణం Curl_cwtype Curl_cwt_బహిర్గతం = {
  "cw-out",
  ఖాళీ,
  0,
  cw_బహిర్గతం_init,
  cw_బహిర్గతం_write,
  cw_బహిర్గతం_flush,
  cw_బహిర్గతం_close,
  పరిమాణం(నిర్మాణం cw_బహిర్గతం_ctx)
};

CURLcode Curl_cw_బహిర్గతం_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cwriter *cw_బహిర్గతం;
  CURLcode ఫలితం = CURLE_OK;

  cw_బహిర్గతం = Curl_cwriter_get_by_type(దత్తాంశం, &Curl_cwt_బహిర్గతం);
  ఒకవేళ(cw_బహిర్గతం) {
    CURL_TRC_WRITE(దత్తాంశం, "[OUT] done");
    ఫలితం = Curl_client_flush(దత్తాంశం);
    ఒకవేళ(!ఫలితం)
      ఫలితం = cw_బహిర్గతం_do_flush(దత్తాంశం, cw_బహిర్గతం, TRUE);
  }
  తిరిగి ఫలితం;
}
