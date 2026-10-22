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
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cw-pause.h"


/* body dynbuf sizes */
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

কাঠামো cw_pause_buf {
  কাঠামো cw_pause_buf *পরবর্তী;
  কাঠামো bufq b;
  পূর্ণসংখ্যা type;
};

স্থির কাঠামো cw_pause_buf *cw_pause_buf_create(পূর্ণসংখ্যা type, size_t buflen)
{
  কাঠামো cw_pause_buf *cwbuf = curlx_calloc(1, আকার(*cwbuf));
  যদি(cwbuf) {
    cwbuf->type = type;
    যদি(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    অন্যথা
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ফেরত cwbuf;
}

স্থির শূন্য cw_pause_buf_free(কাঠামো cw_pause_buf *cwbuf)
{
  যদি(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

কাঠামো cw_pause_ctx {
  কাঠামো Curl_cwriter super;
  কাঠামো cw_pause_buf *buf;
  কাঠামো cw_pause_buf *লেজ;
  size_t buf_যোগ;
};

স্থির CURLcode cw_pause_init(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_cwriter *writer)
{
  কাঠামো cw_pause_ctx *ctx = writer->ctx;
  (শূন্য)তথ্য;
  ctx->buf = ফাঁকা;
  ctx->লেজ = ফাঁকা;
  ফেরত CURLE_OK;
}

স্থির শূন্য cw_pause_bufs_free(কাঠামো cw_pause_ctx *ctx)
{
  যখন(ctx->buf) {
    কাঠামো cw_pause_buf *পরবর্তী = ctx->buf->পরবর্তী;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = পরবর্তী;
  }
  ctx->লেজ = ফাঁকা;
}

স্থির শূন্য cw_pause_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_cwriter *writer)
{
  কাঠামো cw_pause_ctx *ctx = writer->ctx;

  (শূন্য)তথ্য;
  cw_pause_bufs_free(ctx);
}

স্থির CURLcode cw_pause_flush(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *cw_pause)
{
  কাঠামো cw_pause_ctx *ctx = (কাঠামো cw_pause_ctx *)cw_pause;
  CURLcode ফলাফল = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  যখন(ctx->buf && !Curl_cwriter_is_paused(তথ্য)) {
    কাঠামো cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ধ্রুবক unsigned অক্ষর *buf = ফাঁকা;

    যদি(Curl_bufq_peek(&first->b, &buf, &blen)) {
      পূর্ণসংখ্যা wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      যদি((wlen < Curl_bufq_len(&first->b)) || first->পরবর্তী)
        wtype &= ~CLIENTWRITE_EOS;
      ফলাফল = Curl_cwriter_write(তথ্য, cw_pause->পরবর্তী, wtype,
                                  (ধ্রুবক অক্ষর *)buf, wlen);
      CURL_TRC_WRITE(তথ্য, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_যোগ, (unsigned পূর্ণসংখ্যা)wtype,
                     (পূর্ণসংখ্যা)ফলাফল);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_যোগ >= wlen);
      ctx->buf_যোগ -= wlen;
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা যদি(first->type & CLIENTWRITE_EOS) {
      ফলাফল = Curl_cwriter_write(তথ্য, cw_pause->পরবর্তী, first->type,
                                  (ধ্রুবক অক্ষর *)buf, 0);
      CURL_TRC_WRITE(তথ্য, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_যোগ, (unsigned পূর্ণসংখ্যা)first->type,
                     (পূর্ণসংখ্যা)ফলাফল);
    }

    যদি(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->পরবর্তী;
      যদি(!ctx->buf)
        ctx->লেজ = ফাঁকা;
      cw_pause_buf_free(first);
    }
  }

  যদি(!ফলাফল)
    ফলাফল = Curl_cwriter_flush(তথ্য, cw_pause->পরবর্তী);
  ফেরত ফলাফল;
}

স্থির CURLcode cw_pause_write(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                               ধ্রুবক অক্ষর *buf, size_t blen)
{
  কাঠামো cw_pause_ctx *ctx = writer->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t wlen = 0;

  যদি(ctx->buf && !Curl_cwriter_is_paused(তথ্য)) {
    ফলাফল = cw_pause_flush(তথ্য, writer);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যখন(!ctx->buf && !Curl_cwriter_is_paused(তথ্য)) {
    পূর্ণসংখ্যা wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    যদি(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, wtype, buf, wlen);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    buf += wlen;
    blen -= wlen;
    যদি(!blen)
      ফেরত ফলাফল;
  }

  করো {
    size_t nwritten = 0;
    যদি(ctx->লেজ && (ctx->লেজ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      ফলাফল = Curl_bufq_cwrite(&ctx->লেজ->b, buf, blen, &nwritten);
    }
    অন্যথা {
      /* Need a new buf, type changed */
      কাঠামো cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      যদি(!cwbuf)
        ফেরত CURLE_OUT_OF_MEMORY;
      যদি(ctx->লেজ)
        ctx->লেজ->পরবর্তী = cwbuf;
      অন্যথা
        ctx->buf = cwbuf;
      ctx->লেজ = cwbuf;
      ফলাফল = Curl_bufq_cwrite(&ctx->লেজ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(তথ্য, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (unsigned পূর্ণসংখ্যা)type,
                   ctx->buf_যোগ + wlen, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_যোগ += nwritten;
  } যখন(blen);

  ফেরত ফলাফল;
}

ধ্রুবক কাঠামো Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ফাঁকা,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  আকার(কাঠামো cw_pause_ctx)
};
