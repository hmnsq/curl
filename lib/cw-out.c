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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "multiif.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "transfer.h"
#समावेश "cw-out.h"
#समावेश "cw-pause.h"
#समावेश "progress.h"

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
प्रकार क्रमागत {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

काष्ठा cw_out_buf {
  काष्ठा cw_out_buf *अग्र;
  काष्ठा dynbuf b;
  cw_out_type type;
};

अटल काष्ठा cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  काष्ठा cw_out_buf *cwbuf = curlx_calloc(1, माप(*cwbuf));
  अगर(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_बफर);
  }
  वापस cwbuf;
}

अटल व्योम cw_out_buf_free(काष्ठा cw_out_buf *cwbuf)
{
  अगर(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

काष्ठा cw_out_ctx {
  काष्ठा Curl_cwriter super;
  काष्ठा cw_out_buf *buf;
  काष्ठा cw_out_buf *पुच्छ;
  माप_प्रकार buffered_len;
  BIT(errored);
};

अटल CURLcode cw_out_init(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer)
{
  काष्ठा cw_out_ctx *ctx = writer->ctx;
  (व्योम)तथ्य;
  ctx->buf = शून्य;
  ctx->पुच्छ = शून्य;
  ctx->buffered_len = 0;
  वापस CURLE_OK;
}

अटल व्योम cw_out_bufs_free(काष्ठा cw_out_ctx *ctx)
{
  जबतक(ctx->buf) {
    काष्ठा cw_out_buf *अग्र = ctx->buf->अग्र;
    cw_out_buf_free(ctx->buf);
    ctx->buf = अग्र;
  }
  ctx->पुच्छ = शून्य;
  ctx->buffered_len = 0;
}

अटल व्योम cw_out_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_cwriter *writer)
{
  काष्ठा cw_out_ctx *ctx = writer->ctx;

  (व्योम)तथ्य;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
अटल व्योम cw_get_writefunc(काष्ठा Curl_easy *तथ्य, cw_out_type otype,
                             curl_write_callback *pwcb, व्योम **pwcb_तथ्य,
                             माप_प्रकार *pmax_write, माप_प्रकार *pmin_write)
{
  चयन(otype) {
  हाल CW_OUT_BODY:
  हाल CW_OUT_BODY_0LEN:
    अगर(Curl_ftp_listmode(तथ्य)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_तथ्य = तथ्य;
    }
    अन्यथा {
      *pwcb = तथ्य->set.fwrite_func;
      *pwcb_तथ्य = तथ्य->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_आकार;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    अवरोध;
  हाल CW_OUT_HDS:
    *pwcb = तथ्य->set.fwrite_header ? तथ्य->set.fwrite_header :
             (तथ्य->set.writeheader ? तथ्य->set.fwrite_func : शून्य);
    *pwcb_तथ्य = तथ्य->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    अवरोध;
  शेष:
    *pwcb = शून्य;
    *pwcb_तथ्य = शून्य;
    *pmax_write = CURL_MAX_WRITE_आकार;
    *pmin_write = 0;
  }
}

अटल CURLcode cw_out_cb_write(काष्ठा Curl_easy *तथ्य,
                                curl_write_callback wcb,
                                व्योम *wcb_तथ्य,
                                cw_out_type otype,
                                स्थिर अक्षर *buf, माप_प्रकार blen,
                                माप_प्रकार *pnwritten)
{
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  NOVERBOSE((व्योम)otype);

  DEBUGASSERT(तथ्य->conn);
  *pnwritten = 0;
  {
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_cw_out_cb);
    nwritten = wcb((अक्षर *)CURL_UNCONST(buf), 1, blen, wcb_तथ्य);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(तथ्य, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (अचिन्हित पूर्णांक)otype, nwritten);
  अगर(nwritten == CURL_WRITEFUNC_PAUSE) {
    अगर(तथ्य->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(तथ्य, "Write callback asked for PAUSE when not supported");
      वापस CURLE_WRITE_ERROR;
    }
    तथ्य->req.writer.paused = TRUE;
    CURL_TRC_WRITE(तथ्य, "[OUT] PAUSE requested by client");
    परिणाम = Curl_xfer_pause_recv(तथ्य, TRUE);
    वापस परिणाम ? परिणाम : CURLE_AGAIN;
  }
  अन्यथा अगर(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(तथ्य, "client returned ERROR on write of %zu bytes", blen);
    वापस CURLE_WRITE_ERROR;
  }
  अन्यथा अगर(nwritten != blen) {
    failf(तथ्य, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    वापस CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  वापस CURLE_OK;
}

अटल CURLcode cw_out_ptr_flush(काष्ठा cw_out_ctx *ctx,
                                 काष्ठा Curl_easy *तथ्य,
                                 cw_out_type otype,
                                 bool flush_all,
                                 स्थिर अक्षर *buf, माप_प्रकार blen,
                                 माप_प्रकार *pconsumed)
{
  curl_write_callback wcb = शून्य;
  व्योम *wcb_तथ्य;
  माप_प्रकार max_write, min_write;
  माप_प्रकार wlen, nwritten = 0;
  CURLcode परिणाम = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  अगर(ctx->errored)
    वापस CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(तथ्य, otype, &wcb, &wcb_तथ्य, &max_write, &min_write);
  अगर(!wcb) {
    *pconsumed = blen;
    वापस CURLE_OK;
  }

  *pconsumed = 0;
  अगर(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    वापस cw_out_cb_write(तथ्य, wcb, wcb_तथ्य, otype,
                           buf, blen, &nwritten);
  }
  अन्यथा {
    जबतक(blen && !तथ्य->req.writer.paused) {
      अगर(!flush_all && blen < min_write)
        अवरोध;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      अगर(otype == CW_OUT_BODY)
        परिणाम = Curl_pgrs_deliver_check(तथ्य, wlen);
      अगर(!परिणाम)
        परिणाम = cw_out_cb_write(तथ्य, wcb, wcb_तथ्य, otype,
                                 buf, wlen, &nwritten);
      अगर(परिणाम)
        वापस परिणाम;
      अगर(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(तथ्य, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(तथ्य, otype, &wcb, &wcb_तथ्य, &max_write, &min_write);
      अगर(!wcb) {
        *pconsumed += blen;
        अवरोध;
      }
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode cw_out_buf_flush(काष्ठा cw_out_ctx *ctx,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    माप_प्रकार consumed;

    परिणाम = cw_out_ptr_flush(ctx, तथ्य, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    अगर(परिणाम && (परिणाम != CURLE_AGAIN))
      वापस परिणाम;
    परिणाम = CURLE_OK;

    अगर(consumed) {
      अगर(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      अन्यथा {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        परिणाम = curlx_dyn_पुच्छ(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
  }
  वापस परिणाम;
}

अटल CURLcode cw_out_flush_chain(काष्ठा cw_out_ctx *ctx,
                                   काष्ठा Curl_easy *तथ्य,
                                   bool flush_all)
{
  CURLcode परिणाम;

  /* write the chain oldest first until it blocks or gets empty */
  जबतक(ctx->buf) {
    काष्ठा cw_out_buf *cwbuf = ctx->buf;
    माप_प्रकार blen_before;

    अगर(तथ्य->req.writer.paused)
      वापस CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    परिणाम = cw_out_buf_flush(ctx, तथ्य, cwbuf, flush_all);
    अगर(परिणाम)
      वापस परिणाम;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    अगर(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(तथ्य->req.writer.paused);
      वापस CURLE_OK;
    }

    ctx->buf = cwbuf->अग्र;
    अगर(!ctx->buf)
      ctx->पुच्छ = शून्य;
    cw_out_buf_free(cwbuf);
  }
  वापस CURLE_OK;
}

अटल CURLcode cw_out_append(काष्ठा cw_out_ctx *ctx,
                              काष्ठा Curl_easy *तथ्य,
                              cw_out_type otype,
                              स्थिर अक्षर *buf, माप_प्रकार blen)
{
  CURLcode परिणाम;

  CURL_TRC_WRITE(तथ्य, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_बफर);
  अगर(ctx->buffered_len + blen > DYN_PAUSE_बफर) {
    failf(तथ्य, "pause buffer not large enough -> CURLE_TOO_LARGE");
    वापस CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  अगर(!ctx->पुच्छ || (ctx->पुच्छ->type != otype) || (otype == CW_OUT_HDS)) {
    काष्ठा cw_out_buf *cwbuf = cw_out_buf_create(otype);
    अगर(!cwbuf)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(ctx->पुच्छ)
      ctx->पुच्छ->अग्र = cwbuf;
    अन्यथा
      ctx->buf = cwbuf;
    ctx->पुच्छ = cwbuf;
  }
  DEBUGASSERT(ctx->पुच्छ && (ctx->पुच्छ->type == otype));
  परिणाम = curlx_dyn_addn(&ctx->पुच्छ->b, buf, blen);
  अगर(!परिणाम)
    ctx->buffered_len += blen;
  वापस परिणाम;
}

अटल CURLcode cw_out_do_write(काष्ठा cw_out_ctx *ctx,
                                काष्ठा Curl_easy *तथ्य,
                                cw_out_type otype,
                                bool flush_all,
                                स्थिर अक्षर *buf, माप_प्रकार blen)
{
  CURLcode परिणाम = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  अगर(ctx->पुच्छ && ctx->पुच्छ->type != otype) {
    परिणाम = cw_out_flush_chain(ctx, तथ्य, TRUE);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(ctx->buf) {
    /* still have buffered data, append and flush */
    परिणाम = cw_out_append(ctx, तथ्य, otype, buf, blen);
    अगर(परिणाम)
      जाओ out;
    परिणाम = cw_out_flush_chain(ctx, तथ्य, flush_all);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा {
    /* nothing buffered, try direct write */
    माप_प्रकार consumed;
    परिणाम = cw_out_ptr_flush(ctx, तथ्य, otype, flush_all,
                              buf, blen, &consumed);
    अगर(परिणाम && (परिणाम != CURLE_AGAIN))
      वापस परिणाम;
    परिणाम = CURLE_OK;
    अगर(consumed < blen) {
      /* did not write all, append the rest */
      परिणाम = cw_out_append(ctx, तथ्य, otype,
                             buf + consumed, blen - consumed);
      अगर(परिणाम)
        जाओ out;
    }
  }

out:
  अगर(परिणाम) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  वापस परिणाम;
}

अटल CURLcode cw_out_write(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer, पूर्णांक type,
                             स्थिर अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा cw_out_ctx *ctx = writer->ctx;
  CURLcode परिणाम;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  अगर((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && तथ्य->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    परिणाम = cw_out_do_write(ctx, तथ्य, otype, flush_all, buf, blen);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    परिणाम = cw_out_do_write(ctx, तथ्य, CW_OUT_HDS, flush_all, buf, blen);
    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस CURLE_OK;
}

अटल CURLcode cw_out_do_flush(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *cw_out,
                                bool flush_all)
{
  काष्ठा cw_out_ctx *ctx = (काष्ठा cw_out_ctx *)cw_out;

  अगर(ctx->errored)
    वापस CURLE_WRITE_ERROR;

  अगर(!तथ्य->req.writer.paused && ctx->buf) {
    CURLcode परिणाम;

    CURL_TRC_WRITE(तथ्य, "[OUT] flush");
    परिणाम = cw_out_flush_chain(ctx, तथ्य, flush_all);
    अगर(परिणाम) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      वापस परिणाम;
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode cw_out_flush(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer)
{
  वापस cw_out_do_flush(तथ्य, writer, FALSE);
}

स्थिर काष्ठा Curl_cwtype Curl_cwt_out = {
  "cw-out",
  शून्य,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  माप(काष्ठा cw_out_ctx)
};

CURLcode Curl_cw_out_done(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cwriter *cw_out;
  CURLcode परिणाम = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(तथ्य, &Curl_cwt_out);
  अगर(cw_out) {
    CURL_TRC_WRITE(तथ्य, "[OUT] done");
    परिणाम = Curl_client_flush(तथ्य);
    अगर(!परिणाम)
      परिणाम = cw_out_do_flush(तथ्य, cw_out, TRUE);
  }
  वापस परिणाम;
}
