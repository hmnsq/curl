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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "cw-out.h"
#অন্তর্ভাব "cw-pause.h"
#অন্তর্ভাব "progress.h"

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
প্রকারলেখ গণনা {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

কাঠামো cw_out_buf {
  কাঠামো cw_out_buf *পরবর্তী;
  কাঠামো dynbuf b;
  cw_out_type type;
};

স্থির কাঠামো cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  কাঠামো cw_out_buf *cwbuf = curlx_calloc(1, আকার(*cwbuf));
  যদি(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_বাফার);
  }
  ফেরত cwbuf;
}

স্থির শূন্য cw_out_buf_free(কাঠামো cw_out_buf *cwbuf)
{
  যদি(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

কাঠামো cw_out_ctx {
  কাঠামো Curl_cwriter super;
  কাঠামো cw_out_buf *buf;
  কাঠামো cw_out_buf *লেজ;
  size_t buffered_len;
  BIT(errored);
};

স্থির CURLcode cw_out_init(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_cwriter *writer)
{
  কাঠামো cw_out_ctx *ctx = writer->ctx;
  (শূন্য)তথ্য;
  ctx->buf = ফাঁকা;
  ctx->লেজ = ফাঁকা;
  ctx->buffered_len = 0;
  ফেরত CURLE_OK;
}

স্থির শূন্য cw_out_bufs_free(কাঠামো cw_out_ctx *ctx)
{
  যখন(ctx->buf) {
    কাঠামো cw_out_buf *পরবর্তী = ctx->buf->পরবর্তী;
    cw_out_buf_free(ctx->buf);
    ctx->buf = পরবর্তী;
  }
  ctx->লেজ = ফাঁকা;
  ctx->buffered_len = 0;
}

স্থির শূন্য cw_out_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_cwriter *writer)
{
  কাঠামো cw_out_ctx *ctx = writer->ctx;

  (শূন্য)তথ্য;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
স্থির শূন্য cw_get_writefunc(কাঠামো Curl_easy *তথ্য, cw_out_type otype,
                             curl_write_callback *pwcb, শূন্য **pwcb_তথ্য,
                             size_t *pmax_write, size_t *pmin_write)
{
  নির্বাচন(otype) {
  ক্ষেত্র CW_OUT_BODY:
  ক্ষেত্র CW_OUT_BODY_0LEN:
    যদি(Curl_ftp_listmode(তথ্য)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_তথ্য = তথ্য;
    }
    অন্যথা {
      *pwcb = তথ্য->set.fwrite_func;
      *pwcb_তথ্য = তথ্য->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_আকার;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    বিরতি;
  ক্ষেত্র CW_OUT_HDS:
    *pwcb = তথ্য->set.fwrite_header ? তথ্য->set.fwrite_header :
             (তথ্য->set.writeheader ? তথ্য->set.fwrite_func : ফাঁকা);
    *pwcb_তথ্য = তথ্য->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    বিরতি;
  পূর্বনির্ধারিত:
    *pwcb = ফাঁকা;
    *pwcb_তথ্য = ফাঁকা;
    *pmax_write = CURL_MAX_WRITE_আকার;
    *pmin_write = 0;
  }
}

স্থির CURLcode cw_out_cb_write(কাঠামো Curl_easy *তথ্য,
                                curl_write_callback wcb,
                                শূন্য *wcb_তথ্য,
                                cw_out_type otype,
                                ধ্রুবক অক্ষর *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode ফলাফল;

  NOVERBOSE((শূন্য)otype);

  DEBUGASSERT(তথ্য->conn);
  *pnwritten = 0;
  {
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_cw_out_cb);
    nwritten = wcb((অক্ষর *)CURL_UNCONST(buf), 1, blen, wcb_তথ্য);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(তথ্য, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned পূর্ণসংখ্যা)otype, nwritten);
  যদি(nwritten == CURL_WRITEFUNC_PAUSE) {
    যদি(তথ্য->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(তথ্য, "Write callback asked for PAUSE when not supported");
      ফেরত CURLE_WRITE_ERROR;
    }
    তথ্য->req.writer.paused = TRUE;
    CURL_TRC_WRITE(তথ্য, "[OUT] PAUSE requested by client");
    ফলাফল = Curl_xfer_pause_recv(তথ্য, TRUE);
    ফেরত ফলাফল ? ফলাফল : CURLE_AGAIN;
  }
  অন্যথা যদি(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(তথ্য, "client returned ERROR on write of %zu bytes", blen);
    ফেরত CURLE_WRITE_ERROR;
  }
  অন্যথা যদি(nwritten != blen) {
    failf(তথ্য, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    ফেরত CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  ফেরত CURLE_OK;
}

স্থির CURLcode cw_out_ptr_flush(কাঠামো cw_out_ctx *ctx,
                                 কাঠামো Curl_easy *তথ্য,
                                 cw_out_type otype,
                                 bool flush_all,
                                 ধ্রুবক অক্ষর *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = ফাঁকা;
  শূন্য *wcb_তথ্য;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode ফলাফল = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  যদি(ctx->errored)
    ফেরত CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(তথ্য, otype, &wcb, &wcb_তথ্য, &max_write, &min_write);
  যদি(!wcb) {
    *pconsumed = blen;
    ফেরত CURLE_OK;
  }

  *pconsumed = 0;
  যদি(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    ফেরত cw_out_cb_write(তথ্য, wcb, wcb_তথ্য, otype,
                           buf, blen, &nwritten);
  }
  অন্যথা {
    যখন(blen && !তথ্য->req.writer.paused) {
      যদি(!flush_all && blen < min_write)
        বিরতি;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      যদি(otype == CW_OUT_BODY)
        ফলাফল = Curl_pgrs_deliver_check(তথ্য, wlen);
      যদি(!ফলাফল)
        ফলাফল = cw_out_cb_write(তথ্য, wcb, wcb_তথ্য, otype,
                                 buf, wlen, &nwritten);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      যদি(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(তথ্য, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(তথ্য, otype, &wcb, &wcb_তথ্য, &max_write, &min_write);
      যদি(!wcb) {
        *pconsumed += blen;
        বিরতি;
      }
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cw_out_buf_flush(কাঠামো cw_out_ctx *ctx,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    ফলাফল = cw_out_ptr_flush(ctx, তথ্য, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
      ফেরত ফলাফল;
    ফলাফল = CURLE_OK;

    যদি(consumed) {
      যদি(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      অন্যথা {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        ফলাফল = curlx_dyn_লেজ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cw_out_flush_chain(কাঠামো cw_out_ctx *ctx,
                                   কাঠামো Curl_easy *তথ্য,
                                   bool flush_all)
{
  CURLcode ফলাফল;

  /* write the chain oldest first until it blocks or gets empty */
  যখন(ctx->buf) {
    কাঠামো cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    যদি(তথ্য->req.writer.paused)
      ফেরত CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    ফলাফল = cw_out_buf_flush(ctx, তথ্য, cwbuf, flush_all);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    যদি(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(তথ্য->req.writer.paused);
      ফেরত CURLE_OK;
    }

    ctx->buf = cwbuf->পরবর্তী;
    যদি(!ctx->buf)
      ctx->লেজ = ফাঁকা;
    cw_out_buf_free(cwbuf);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cw_out_append(কাঠামো cw_out_ctx *ctx,
                              কাঠামো Curl_easy *তথ্য,
                              cw_out_type otype,
                              ধ্রুবক অক্ষর *buf, size_t blen)
{
  CURLcode ফলাফল;

  CURL_TRC_WRITE(তথ্য, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_বাফার);
  যদি(ctx->buffered_len + blen > DYN_PAUSE_বাফার) {
    failf(তথ্য, "pause buffer not large enough -> CURLE_TOO_LARGE");
    ফেরত CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  যদি(!ctx->লেজ || (ctx->লেজ->type != otype) || (otype == CW_OUT_HDS)) {
    কাঠামো cw_out_buf *cwbuf = cw_out_buf_create(otype);
    যদি(!cwbuf)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(ctx->লেজ)
      ctx->লেজ->পরবর্তী = cwbuf;
    অন্যথা
      ctx->buf = cwbuf;
    ctx->লেজ = cwbuf;
  }
  DEBUGASSERT(ctx->লেজ && (ctx->লেজ->type == otype));
  ফলাফল = curlx_dyn_addn(&ctx->লেজ->b, buf, blen);
  যদি(!ফলাফল)
    ctx->buffered_len += blen;
  ফেরত ফলাফল;
}

স্থির CURLcode cw_out_do_write(কাঠামো cw_out_ctx *ctx,
                                কাঠামো Curl_easy *তথ্য,
                                cw_out_type otype,
                                bool flush_all,
                                ধ্রুবক অক্ষর *buf, size_t blen)
{
  CURLcode ফলাফল = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  যদি(ctx->লেজ && ctx->লেজ->type != otype) {
    ফলাফল = cw_out_flush_chain(ctx, তথ্য, TRUE);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(ctx->buf) {
    /* still have buffered data, append and flush */
    ফলাফল = cw_out_append(ctx, তথ্য, otype, buf, blen);
    যদি(ফলাফল)
      যাও out;
    ফলাফল = cw_out_flush_chain(ctx, তথ্য, flush_all);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা {
    /* nothing buffered, try direct write */
    size_t consumed;
    ফলাফল = cw_out_ptr_flush(ctx, তথ্য, otype, flush_all,
                              buf, blen, &consumed);
    যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
      ফেরত ফলাফল;
    ফলাফল = CURLE_OK;
    যদি(consumed < blen) {
      /* did not write all, append the rest */
      ফলাফল = cw_out_append(ctx, তথ্য, otype,
                             buf + consumed, blen - consumed);
      যদি(ফলাফল)
        যাও out;
    }
  }

out:
  যদি(ফলাফল) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cw_out_write(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                             ধ্রুবক অক্ষর *buf, size_t blen)
{
  কাঠামো cw_out_ctx *ctx = writer->ctx;
  CURLcode ফলাফল;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  যদি((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && তথ্য->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    ফলাফল = cw_out_do_write(ctx, তথ্য, otype, flush_all, buf, blen);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    ফলাফল = cw_out_do_write(ctx, তথ্য, CW_OUT_HDS, flush_all, buf, blen);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode cw_out_do_flush(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *cw_out,
                                bool flush_all)
{
  কাঠামো cw_out_ctx *ctx = (কাঠামো cw_out_ctx *)cw_out;

  যদি(ctx->errored)
    ফেরত CURLE_WRITE_ERROR;

  যদি(!তথ্য->req.writer.paused && ctx->buf) {
    CURLcode ফলাফল;

    CURL_TRC_WRITE(তথ্য, "[OUT] flush");
    ফলাফল = cw_out_flush_chain(ctx, তথ্য, flush_all);
    যদি(ফলাফল) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      ফেরত ফলাফল;
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cw_out_flush(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer)
{
  ফেরত cw_out_do_flush(তথ্য, writer, FALSE);
}

ধ্রুবক কাঠামো Curl_cwtype Curl_cwt_out = {
  "cw-out",
  ফাঁকা,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  আকার(কাঠামো cw_out_ctx)
};

CURLcode Curl_cw_out_done(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cwriter *cw_out;
  CURLcode ফলাফল = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(তথ্য, &Curl_cwt_out);
  যদি(cw_out) {
    CURL_TRC_WRITE(তথ্য, "[OUT] done");
    ফলাফল = Curl_client_flush(তথ্য);
    যদি(!ফলাফল)
      ফলাফল = cw_out_do_flush(তথ্য, cw_out, TRUE);
  }
  ফেরত ফলাফল;
}
