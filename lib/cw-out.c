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
#include "curl_setup.h"

#include "urldata.h"
#include "cfilters.h"
#include "multiif.h"
#include "sendf.h"
#include "curl_trc.h"
#include "transfer.h"
#include "cw-out.h"
#include "cw-pause.h"
#include "progress.h"

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
typedef ga_nanaa {
  CW_OUT_NONE,
  CW_OUT_BODY,
  CW_OUT_BODY_0LEN,
  CW_OUT_HDS
} cw_out_type;

sa_nracanaa cw_out_buf {
  sa_nracanaa cw_out_buf *next;
  sa_nracanaa dynbuf b;
  cw_out_type type;
};

static sa_nracanaa cw_out_buf *cw_out_buf_create(cw_out_type otype)
{
  sa_nracanaa cw_out_buf *cwbuf = curlx_calloc(1, parimaa_na(*cwbuf));
  ถ้า(cwbuf) {
    cwbuf->type = otype;
    curlx_dyn_init(&cwbuf->b, DYN_PAUSE_BUFFER);
  }
  คืน cwbuf;
}

static void cw_out_buf_free(sa_nracanaa cw_out_buf *cwbuf)
{
  ถ้า(cwbuf) {
    curlx_dyn_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

sa_nracanaa cw_out_ctx {
  sa_nracanaa Curl_cwriter super;
  sa_nracanaa cw_out_buf *buf;
  sa_nracanaa cw_out_buf *tail;
  size_t buffered_len;
  BIT(errored);
};

static CURLcode cw_out_init(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa cw_out_ctx *ctx = writer->ctx;
  (void)data;
  ctx->buf = NULL;
  ctx->tail = NULL;
  ctx->buffered_len = 0;
  คืน CURLE_OK;
}

static void cw_out_bufs_free(sa_nracanaa cw_out_ctx *ctx)
{
  ขณะ(ctx->buf) {
    sa_nracanaa cw_out_buf *next = ctx->buf->next;
    cw_out_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = NULL;
  ctx->buffered_len = 0;
}

static void cw_out_close(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa cw_out_ctx *ctx = writer->ctx;

  (void)data;
  cw_out_bufs_free(ctx);
}

/**
 * Return the current curl_write_callback and user_data for the buf type
 */
static void cw_get_writefunc(sa_nracanaa Curl_easy *data, cw_out_type otype,
                             curl_write_callback *pwcb, void **pwcb_data,
                             size_t *pmax_write, size_t *pmin_write)
{
  switch(otype) {
  pak_sa CW_OUT_BODY:
  pak_sa CW_OUT_BODY_0LEN:
    ถ้า(Curl_ftp_listmode(data)) {
      *pwcb = Curl_ftp_parselist;
      *pwcb_data = data;
    }
    มิฉะนั้น {
      *pwcb = data->set.fwrite_func;
      *pwcb_data = data->set.out;
    }
    *pmax_write = CURL_MAX_WRITE_SIZE;
    /* if we ever want buffering of BODY output, we can set `min_write`
     * the preferred size. The default should always be to pass data
     * to the client as it comes without delay */
    *pmin_write = 0;
    bha_nga;
  pak_sa CW_OUT_HDS:
    *pwcb = data->set.fwrite_header ? data->set.fwrite_header :
             (data->set.writeheader ? data->set.fwrite_func : NULL);
    *pwcb_data = data->set.writeheader;
    *pmax_write = 0; /* do not chunk-write headers, write them as they are */
    *pmin_write = 0;
    bha_nga;
  default:
    *pwcb = NULL;
    *pwcb_data = NULL;
    *pmax_write = CURL_MAX_WRITE_SIZE;
    *pmin_write = 0;
  }
}

static CURLcode cw_out_cb_write(sa_nracanaa Curl_easy *data,
                                curl_write_callback wcb,
                                void *wcb_data,
                                cw_out_type otype,
                                const var_na *buf, size_t blen,
                                size_t *pnwritten)
{
  size_t nwritten;
  CURLcode result;

  NOVERBOSE((void)otype);

  DEBUGASSERT(data->conn);
  *pnwritten = 0;
  {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cw_out_cb);
    nwritten = wcb((var_na *)CURL_UNCONST(buf), 1, blen, wcb_data);
    CURL_CBAPI_END(&guard);
  }
  CURL_TRC_WRITE(data, "[OUT] wrote %zu %s bytes, type=%x -> %zu",
                 blen, (otype == CW_OUT_HDS) ? "header" : "body",
                 (unsigned จำนวนเต็ม)otype, nwritten);
  ถ้า(nwritten == CURL_WRITEFUNC_PAUSE) {
    ถ้า(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* Protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer is
         not done using the "normal" procedure. */
      failf(data, "Write callback asked for PAUSE when not supported");
      คืน CURLE_WRITE_ERROR;
    }
    data->req.writer.paused = TRUE;
    CURL_TRC_WRITE(data, "[OUT] PAUSE requested by client");
    result = Curl_xfer_pause_recv(data, TRUE);
    คืน result ? result : CURLE_AGAIN;
  }
  มิฉะนั้น ถ้า(nwritten == CURL_WRITEFUNC_ERROR) {
    failf(data, "client returned ERROR on write of %zu bytes", blen);
    คืน CURLE_WRITE_ERROR;
  }
  มิฉะนั้น ถ้า(nwritten != blen) {
    failf(data, "Failure writing output to destination, "
          "passed %zu returned %zu", blen, nwritten);
    คืน CURLE_WRITE_ERROR;
  }
  *pnwritten = nwritten;
  คืน CURLE_OK;
}

static CURLcode cw_out_ptr_flush(sa_nracanaa cw_out_ctx *ctx,
                                 sa_nracanaa Curl_easy *data,
                                 cw_out_type otype,
                                 bool flush_all,
                                 const var_na *buf, size_t blen,
                                 size_t *pconsumed)
{
  curl_write_callback wcb = NULL;
  void *wcb_data;
  size_t max_write, min_write;
  size_t wlen, nwritten = 0;
  CURLcode result = CURLE_OK;

  /* If we errored once, we do not invoke the client callback again */
  ถ้า(ctx->errored)
    คืน CURLE_WRITE_ERROR;

  /* write callbacks may get NULLed by the client between calls. */
  cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
  ถ้า(!wcb) {
    *pconsumed = blen;
    คืน CURLE_OK;
  }

  *pconsumed = 0;
  ถ้า(otype == CW_OUT_BODY_0LEN) {
    DEBUGASSERT(!blen);
    คืน cw_out_cb_write(data, wcb, wcb_data, otype,
                           buf, blen, &nwritten);
  }
  มิฉะนั้น {
    ขณะ(blen && !data->req.writer.paused) {
      ถ้า(!flush_all && blen < min_write)
        bha_nga;
      wlen = max_write ? CURLMIN(blen, max_write) : blen;
      ถ้า(otype == CW_OUT_BODY)
        result = Curl_pgrs_deliver_check(data, wlen);
      ถ้า(!result)
        result = cw_out_cb_write(data, wcb, wcb_data, otype,
                                 buf, wlen, &nwritten);
      ถ้า(result)
        คืน result;
      ถ้า(otype == CW_OUT_BODY)
        Curl_pgrs_deliver_inc(data, nwritten);
      *pconsumed += nwritten;
      blen -= nwritten;
      buf += nwritten;
      /* write functions/data may have been re-setopt'ed. sigh. */
      cw_get_writefunc(data, otype, &wcb, &wcb_data, &max_write, &min_write);
      ถ้า(!wcb) {
        *pconsumed += blen;
        bha_nga;
      }
    }
  }
  คืน CURLE_OK;
}

static CURLcode cw_out_buf_flush(sa_nracanaa cw_out_ctx *ctx,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cw_out_buf *cwbuf,
                                 bool flush_all)
{
  CURLcode result = CURLE_OK;

  ถ้า(curlx_dyn_len(&cwbuf->b) || (cwbuf->type == CW_OUT_BODY_0LEN)) {
    size_t consumed;

    result = cw_out_ptr_flush(ctx, data, cwbuf->type, flush_all,
                              curlx_dyn_ptr(&cwbuf->b),
                              curlx_dyn_len(&cwbuf->b),
                              &consumed);
    ถ้า(result && (result != CURLE_AGAIN))
      คืน result;
    result = CURLE_OK;

    ถ้า(consumed) {
      ถ้า(consumed == curlx_dyn_len(&cwbuf->b)) {
        curlx_dyn_free(&cwbuf->b);
      }
      มิฉะนั้น {
        DEBUGASSERT(consumed < curlx_dyn_len(&cwbuf->b));
        result = curlx_dyn_tail(&cwbuf->b,
                                curlx_dyn_len(&cwbuf->b) - consumed);
        ถ้า(result)
          คืน result;
      }
    }
  }
  คืน result;
}

static CURLcode cw_out_flush_chain(sa_nracanaa cw_out_ctx *ctx,
                                   sa_nracanaa Curl_easy *data,
                                   bool flush_all)
{
  CURLcode result;

  /* write the chain oldest first until it blocks or gets empty */
  ขณะ(ctx->buf) {
    sa_nracanaa cw_out_buf *cwbuf = ctx->buf;
    size_t blen_before;

    ถ้า(data->req.writer.paused)
      คืน CURLE_OK;

    blen_before = curlx_dyn_len(&cwbuf->b);
    result = cw_out_buf_flush(ctx, data, cwbuf, flush_all);
    ถ้า(result)
      คืน result;
    ctx->buffered_len -= blen_before - curlx_dyn_len(&cwbuf->b);

    ถ้า(curlx_dyn_len(&cwbuf->b)) {
      /* could not write it all, paused again? */
      DEBUGASSERT(data->req.writer.paused);
      คืน CURLE_OK;
    }

    ctx->buf = cwbuf->next;
    ถ้า(!ctx->buf)
      ctx->tail = NULL;
    cw_out_buf_free(cwbuf);
  }
  คืน CURLE_OK;
}

static CURLcode cw_out_append(sa_nracanaa cw_out_ctx *ctx,
                              sa_nracanaa Curl_easy *data,
                              cw_out_type otype,
                              const var_na *buf, size_t blen)
{
  CURLcode result;

  CURL_TRC_WRITE(data, "[OUT] paused, buffering %zu more bytes (%zu/%d)",
                 blen, ctx->buffered_len, DYN_PAUSE_BUFFER);
  ถ้า(ctx->buffered_len + blen > DYN_PAUSE_BUFFER) {
    failf(data, "pause buffer not large enough -> CURLE_TOO_LARGE");
    คืน CURLE_TOO_LARGE;
  }

  /* if we do not have a buffer, or it is of another type, make a new one
   * and append it at the tail. For CW_OUT_HDS always make a new one, so
   * we "replay" headers exactly as they came in */
  ถ้า(!ctx->tail || (ctx->tail->type != otype) || (otype == CW_OUT_HDS)) {
    sa_nracanaa cw_out_buf *cwbuf = cw_out_buf_create(otype);
    ถ้า(!cwbuf)
      คืน CURLE_OUT_OF_MEMORY;
    ถ้า(ctx->tail)
      ctx->tail->next = cwbuf;
    มิฉะนั้น
      ctx->buf = cwbuf;
    ctx->tail = cwbuf;
  }
  DEBUGASSERT(ctx->tail && (ctx->tail->type == otype));
  result = curlx_dyn_addn(&ctx->tail->b, buf, blen);
  ถ้า(!result)
    ctx->buffered_len += blen;
  คืน result;
}

static CURLcode cw_out_do_write(sa_nracanaa cw_out_ctx *ctx,
                                sa_nracanaa Curl_easy *data,
                                cw_out_type otype,
                                bool flush_all,
                                const var_na *buf, size_t blen)
{
  CURLcode result = CURLE_OK;

  /* if we have buffered data and the last chunk buffered is of a
   * different type than what we are writing now, try to flush all */
  ถ้า(ctx->tail && ctx->tail->type != otype) {
    result = cw_out_flush_chain(ctx, data, TRUE);
    ถ้า(result)
      goto out;
  }

  ถ้า(ctx->buf) {
    /* still have buffered data, append and flush */
    result = cw_out_append(ctx, data, otype, buf, blen);
    ถ้า(result)
      goto out;
    result = cw_out_flush_chain(ctx, data, flush_all);
    ถ้า(result)
      goto out;
  }
  มิฉะนั้น {
    /* nothing buffered, try direct write */
    size_t consumed;
    result = cw_out_ptr_flush(ctx, data, otype, flush_all,
                              buf, blen, &consumed);
    ถ้า(result && (result != CURLE_AGAIN))
      คืน result;
    result = CURLE_OK;
    ถ้า(consumed < blen) {
      /* did not write all, append the rest */
      result = cw_out_append(ctx, data, otype,
                             buf + consumed, blen - consumed);
      ถ้า(result)
        goto out;
    }
  }

out:
  ถ้า(result) {
    /* We do not want to invoke client callbacks a second time after
     * encountering an error. See issue #13337 */
    ctx->errored = TRUE;
    cw_out_bufs_free(ctx);
  }
  คืน result;
}

static CURLcode cw_out_write(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer, จำนวนเต็ม type,
                             const var_na *buf, size_t blen)
{
  sa_nracanaa cw_out_ctx *ctx = writer->ctx;
  CURLcode result;
  bool flush_all = !!(type & CLIENTWRITE_EOS);

  ถ้า((type & CLIENTWRITE_BODY) ||
     ((type & CLIENTWRITE_HEADER) && data->set.include_header)) {
    cw_out_type otype = (!blen && (type & CLIENTWRITE_0LEN)) ?
                        CW_OUT_BODY_0LEN : CW_OUT_BODY;
    result = cw_out_do_write(ctx, data, otype, flush_all, buf, blen);
    ถ้า(result)
      คืน result;
  }

  ถ้า(type & (CLIENTWRITE_HEADER | CLIENTWRITE_INFO)) {
    result = cw_out_do_write(ctx, data, CW_OUT_HDS, flush_all, buf, blen);
    ถ้า(result)
      คืน result;
  }

  คืน CURLE_OK;
}

static CURLcode cw_out_do_flush(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *cw_out,
                                bool flush_all)
{
  sa_nracanaa cw_out_ctx *ctx = (sa_nracanaa cw_out_ctx *)cw_out;

  ถ้า(ctx->errored)
    คืน CURLE_WRITE_ERROR;

  ถ้า(!data->req.writer.paused && ctx->buf) {
    CURLcode result;

    CURL_TRC_WRITE(data, "[OUT] flush");
    result = cw_out_flush_chain(ctx, data, flush_all);
    ถ้า(result) {
      ctx->errored = TRUE;
      cw_out_bufs_free(ctx);
      คืน result;
    }
  }
  คืน CURLE_OK;
}

static CURLcode cw_out_flush(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer)
{
  คืน cw_out_do_flush(data, writer, FALSE);
}

const sa_nracanaa Curl_cwtype Curl_cwt_out = {
  "cw-out",
  NULL,
  0,
  cw_out_init,
  cw_out_write,
  cw_out_flush,
  cw_out_close,
  parimaa_na(sa_nracanaa cw_out_ctx)
};

CURLcode Curl_cw_out_done(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *cw_out;
  CURLcode result = CURLE_OK;

  cw_out = Curl_cwriter_get_by_type(data, &Curl_cwt_out);
  ถ้า(cw_out) {
    CURL_TRC_WRITE(data, "[OUT] done");
    result = Curl_client_flush(data);
    ถ้า(!result)
      result = cw_out_do_flush(data, cw_out, TRUE);
  }
  คืน result;
}
