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
  CW_बहिर्गमनम्_NONE,
  CW_बहिर्गमनम्_BODY,
  CW_बहिर्गमनम्_BODY_0_LEN,
  CW_बहिर्गमनम्_HDS
} cw_बहिर्गमनम्_type;

संरचना cw_बहिर्गमनम्_buf {
  संरचना cw_बहिर्गमनम्_buf *अग्र;
  संरचना dynbuf b;
  cw_बहिर्गमनम्_type type;
};

स्थिर संरचना cw_बहिर्गमनम्_buf *cw_बहिर्गमनम्_buf_create(cw_बहिर्गमनम्_type otype)
{
  संरचना cw_बहिर्गमनम्_buf *cwbuf = curlx_calloc(1, परिमाण(*cwbuf));
  यदि(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_बफर);
  }
  प्रत्यागम cwbuf;
}

स्थिर शून्य cw_बहिर्गमनम्_buf_free(संरचना cw_बहिर्गमनम्_buf *cwbuf)
{
  यदि(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

संरचना cw_बहिर्गमनम्_ctx {
  संरचना Curl_cwriter super;
  संरचना cw_बहिर्गमनम्_buf *buf;
  संरचना cw_बहिर्गमनम्_buf *पुच्छ;
  size_t buffered_len;
  BIT(errored);
};

स्थिर CURLcode cw_बहिर्गमनम्_init(संरचना Curl_easy *दत्त,
                            संरचना Curl_cwriter *writer)
{
  संरचना cw_बहिर्गमनम्_ctx *ctx = writer->ctx;
  (शून्य)दत्त;
  ctx->buf = रिक्त;
  ctx->पुच्छ = रिक्त;
  ctx->buffered_len = 0;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cw_बहिर्गमनम्_bufs_free(संरचना cw_बहिर्गमनम्_ctx *ctx)
{
  यावत्(ctx->buf) {
    संरचना cw_बहिर्गमनम्_buf *अग्र = ctx->buf->अग्र;
    cw_बहिर्गमनम्_buf_free(ctx->buf);
    ctx->buf = अग्र;
  }
  ctx->पुच्छ = रिक्त;
  ctx->buffered_len = 0;
}

स्थिर शून्य cw_बहिर्गमनम्_close(संरचना Curl_easy *दत्त, संरचना Curl_cwriter *writer)
{
  संरचना cw_बहिर्गमनम्_ctx *ctx = writer->ctx;

  (शून्य)दत्त;
  cw_बहिर्गमनम्_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
स्थिर शून्य cw_get_writefunc(संरचना Curl_easy *दत्त, cw_बहिर्गमनम्_type otype,
                             curl_write_callback *pwcb, शून्य **pwcb_दत्त,
                             size_t *pmax_write, size_t *pmin_write)
{
  विकल्प(otype) {
  पक्ष CW_बहिर्गमनम्_BODY:
  पक्ष CW_बहिर्गमनम्_BODY_0_LEN:
    यदि(Curl_ftp_listmode(दत्त)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_दत्त = दत्त;
    }
    अन्यथा {
      *pwcb = दत्त->समुच्चयः.fwrite_func;
      *pwcb_दत्त = दत्त->समुच्चयः.बहिर्गमनम्;
    }
    *pmax_write = CURL_MAX_WRITE_आकार;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    भङ्ग;
  पक्ष CW_बहिर्गमनम्_HDS:
    *pwcb = दत्त->समुच्चयः.fwrite_शीर्षकम् ? दत्त->समुच्चयः.fwrite_शीर्षकम् :
             (दत्त->समुच्चयः.writeheader ? दत्त->समुच्चयः.fwrite_func : रिक्त);
    *pwcb_दत्त = दत्त->समुच्चयः.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    भङ्ग;
  स्वभाव:
    *pwcb = रिक्त;
    *pwcb_दत्त = रिक्त;
    *pmax_write = CURL_MAX_WRITE_आकार;
    *pmin_write = 0;
  }
}

स्थिर CURLcode cw_बहिर्गमनम्_cb_write(संरचना Curl_easy *दत्त,
                                curl_write_callback wcb,
                                शून्य *wcb_दत्त,
                                cw_बहिर्गमनम्_type otype,
                                ध्रुव वर्ण *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode परिणाम;

  NOVERBOSE((शून्य)otype);

  DEBUGASSERT(दत्त->conn);
  *pnwritten = 0;
  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_cw_बहिर्गमनम्_cb);
    nwritten = wcb((वर्ण *)CURL_UNCONST(buf), 1, blen, wcb_दत्त);
    CURL_CBAPI_अन्तः(&guard);
  }
  CURL_TRC_WRITE(दत्त, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_बहिर्गमनम्_HDS) ? "header" : "body",
                 (अचिह्नित पूर्णाङ्क)otype, nwritten);
  यदि(nwritten == CURL_WRITEFUNC_PAUSE) {
    यदि(दत्त->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(दत्त, "Write callback asked for PAUSE when not supported");
      प्रत्यागम CURLE_WRITE_ERROR;
    }
    दत्त->req.writer.paused = TRUE;
    CURL_TRC_WRITE(दत्त, "[OUT] PAUSE requested by client");
    परिणाम = Curl_xfer_pause_recv(दत्त, TRUE);
    प्रत्यागम परिणाम ? परिणाम : CURLE_AGAIN;
  }
  अन्यथा यदि(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(दत्त, "client returned ERROR on write of %zu bytes", blen);
    प्रत्यागम CURLE_WRITE_ERROR;
  }
  अन्यथा यदि(nwritten != blen) {
    failf(दत्त, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    प्रत्यागम CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_बहिर्गमनम्_ptr_flush(संरचना cw_बहिर्गमनम्_ctx *ctx,
                                 संरचना Curl_easy *दत्त,
                                 cw_बहिर्गमनम्_type otype,
                                 bool flush_all,
                                 ध्रुव वर्ण *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = रिक्त;
  शून्य *wcb_दत्त;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode परिणाम = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  यदि(ctx->errored)
    प्रत्यागम CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(दत्त, otype, &wcb, &wcb_दत्त, &max_write, &min_write);
  यदि(!wcb) {
    *pconsumed = blen;
    प्रत्यागम CURLE_OK;
  }

  *pconsumed = 0;
  यदि(otype == CW_बहिर्गमनम्_BODY_0_LEN) {
    DEBUGASSERT(!blen);
    प्रत्यागम cw_बहिर्गमनम्_cb_write(दत्त, wcb, wcb_दत्त, otype,
                           buf, blen, &nwritten);
  }
  अन्यथा {
    यावत्(blen && !दत्त->req.writer.paused) {
      यदि(!flush_all && blen < min_write)
        भङ्ग;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      यदि(otype == CW_बहिर्गमनम्_BODY)
        परिणाम = Curl_pgrs_deliver_check(दत्त, wlen);
      यदि(!परिणाम)
        परिणाम = cw_बहिर्गमनम्_cb_write(दत्त, wcb, wcb_दत्त, otype,
                                 buf, wlen, &nwritten);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      यदि(otype == CW_बहिर्गमनम्_BODY)
        Curl_pgrs_deliver_inc(दत्त, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(दत्त, otype, &wcb, &wcb_दत्त, &max_write, &min_write);
      यदि(!wcb) {
        *pconsumed += blen;
        भङ्ग;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_बहिर्गमनम्_buf_flush(संरचना cw_बहिर्गमनम्_ctx *ctx,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cw_बहिर्गमनम्_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_बहिर्गमनम्_BODY_0_LEN)) {
    size_t consumed;

    परिणाम = cw_बहिर्गमनम्_ptr_flush(ctx, दत्त, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    यदि(परिणाम && (परिणाम != CURLE_AGAIN))
      प्रत्यागम परिणाम;
    परिणाम = CURLE_OK;

    यदि(consumed) {
      यदि(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      अन्यथा {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        परिणाम = curlx_dyn_पुच्छ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cw_बहिर्गमनम्_flush_chain(संरचना cw_बहिर्गमनम्_ctx *ctx,
                                   संरचना Curl_easy *दत्त,
                                   bool flush_all)
{
  CURLcode परिणाम;

  /* write the chain oldest first until it blocks or gets empty */
  यावत्(ctx->buf) {
    संरचना cw_बहिर्गमनम्_buf *cwbuf = ctx->buf;
    size_t blen_before;

    यदि(दत्त->req.writer.paused)
      प्रत्यागम CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    परिणाम = cw_बहिर्गमनम्_buf_flush(ctx, दत्त, cwbuf, flush_all);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    यदि(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(दत्त->req.writer.paused);
      प्रत्यागम CURLE_OK;
    }

    ctx->buf = cwbuf->अग्र;
    यदि(!ctx->buf)
      ctx->पुच्छ = रिक्त;
    cw_बहिर्गमनम्_buf_free(cwbuf);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_बहिर्गमनम्_append(संरचना cw_बहिर्गमनम्_ctx *ctx,
                              संरचना Curl_easy *दत्त,
                              cw_बहिर्गमनम्_type otype,
                              ध्रुव वर्ण *buf, size_t blen)
{
  CURLcode परिणाम;

  CURL_TRC_WRITE(दत्त, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_बफर);
  यदि(ctx->buffered_len + blen > DYN_PAUSE_बफर) {
    failf(दत्त, "pause buffer not large enough -> CURLE_TOO_LARGE");
    प्रत्यागम CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  यदि(!ctx->पुच्छ || (ctx->पुच्छ->type != otype) || (otype == CW_बहिर्गमनम्_HDS)) {
    संरचना cw_बहिर्गमनम्_buf *cwbuf = cw_बहिर्गमनम्_buf_create(otype);
    यदि(!cwbuf)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(ctx->पुच्छ)
      ctx->पुच्छ->अग्र = cwbuf;
    अन्यथा
      ctx->buf = cwbuf;
    ctx->पुच्छ = cwbuf;
  }
  DEBUGASSERT(ctx->पुच्छ && (ctx->पुच्छ->type == otype));
  परिणाम = curlx_dyn_addn(&ctx->पुच्छ->b, buf, blen);
  यदि(!परिणाम)
    ctx->buffered_len += blen;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cw_बहिर्गमनम्_do_write(संरचना cw_बहिर्गमनम्_ctx *ctx,
                                संरचना Curl_easy *दत्त,
                                cw_बहिर्गमनम्_type otype,
                                bool flush_all,
                                ध्रुव वर्ण *buf, size_t blen)
{
  CURLcode परिणाम = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  यदि(ctx->पुच्छ && ctx->पुच्छ->type != otype) {
    परिणाम = cw_बहिर्गमनम्_flush_chain(ctx, दत्त, TRUE);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(ctx->buf) {
    /* still have buffered data, append and flush */
    परिणाम = cw_बहिर्गमनम्_append(ctx, दत्त, otype, buf, blen);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    परिणाम = cw_बहिर्गमनम्_flush_chain(ctx, दत्त, flush_all);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    /* nothing buffered, try direct write */
    size_t consumed;
    परिणाम = cw_बहिर्गमनम्_ptr_flush(ctx, दत्त, otype, flush_all,
                              buf, blen, &consumed);
    यदि(परिणाम && (परिणाम != CURLE_AGAIN))
      प्रत्यागम परिणाम;
    परिणाम = CURLE_OK;
    यदि(consumed < blen) {
      /* did not write all, append the rest */
      परिणाम = cw_बहिर्गमनम्_append(ctx, दत्त, otype,
                             buf + consumed, blen - consumed);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  यदि(परिणाम) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_बहिर्गमनम्_bufs_free(ctx);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cw_बहिर्गमनम्_write(संरचना Curl_easy *दत्त,
                             संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                             ध्रुव वर्ण *buf, size_t blen)
{
  संरचना cw_बहिर्गमनम्_ctx *ctx = writer->ctx;
  CURLcode परिणाम;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  यदि((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_शीर्षकम्) && दत्त->समुच्चयः.include_शीर्षकम्)) {
    cw_बहिर्गमनम्_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_बहिर्गमनम्_BODY_0_LEN : CW_बहिर्गमनम्_BODY;
    परिणाम = cw_बहिर्गमनम्_do_write(ctx, दत्त, otype, flush_all, buf, blen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(type & (CLIENTWRITE_शीर्षकम् | CLIENTWRITE_INFO)) {
    परिणाम = cw_बहिर्गमनम्_do_write(ctx, दत्त, CW_बहिर्गमनम्_HDS, flush_all, buf, blen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_बहिर्गमनम्_do_flush(संरचना Curl_easy *दत्त,
                                संरचना Curl_cwriter *cw_बहिर्गमनम्,
                                bool flush_all)
{
  संरचना cw_बहिर्गमनम्_ctx *ctx = (संरचना cw_बहिर्गमनम्_ctx *)cw_बहिर्गमनम्;

  यदि(ctx->errored)
    प्रत्यागम CURLE_WRITE_ERROR;

  यदि(!दत्त->req.writer.paused && ctx->buf) {
    CURLcode परिणाम;

    CURL_TRC_WRITE(दत्त, "[OUT] flush");
    परिणाम = cw_बहिर्गमनम्_flush_chain(ctx, दत्त, flush_all);
    यदि(परिणाम) {
      ctx->errored = TRUE;
      cw_बहिर्गमनम्_bufs_free(ctx);
      प्रत्यागम परिणाम;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cw_बहिर्गमनम्_flush(संरचना Curl_easy *दत्त,
                             संरचना Curl_cwriter *writer)
{
  प्रत्यागम cw_बहिर्गमनम्_do_flush(दत्त, writer, FALSE);
}

ध्रुव संरचना Curl_cwtype Curl_cwt_बहिर्गमनम् = {
  "cw-out",
  रिक्त,
  0,
  cw_बहिर्गमनम्_init,
  cw_बहिर्गमनम्_write,
  cw_बहिर्गमनम्_flush,
  cw_बहिर्गमनम्_close,
  परिमाण(संरचना cw_बहिर्गमनम्_ctx)
};

CURLcode Curl_cw_बहिर्गमनम्_समाप्तम्(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cwriter *cw_बहिर्गमनम्;
  CURLcode परिणाम = CURLE_OK;

  cw_बहिर्गमनम् = Curl_cwriter_get_by_type(दत्त, &Curl_cwt_बहिर्गमनम्);
  यदि(cw_बहिर्गमनम्) {
    CURL_TRC_WRITE(दत्त, "[OUT] done");
    परिणाम = Curl_client_flush(दत्त);
    यदि(!परिणाम)
      परिणाम = cw_बहिर्गमनम्_do_flush(दत्त, cw_बहिर्गमनम्, TRUE);
  }
  प्रत्यागम परिणाम;
}
