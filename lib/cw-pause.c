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
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cw-pause.h"


/* body dynbuf sizes */
#परिभाषय CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#परिभाषय CW_PAUSE_DEC_WRITE_CHUNK   4096

संरचना cw_pause_buf {
  संरचना cw_pause_buf *अग्र;
  संरचना bufq b;
  पूर्णाङ्क type;
};

स्थिर संरचना cw_pause_buf *cw_pause_buf_create(पूर्णाङ्क type, size_t buflen)
{
  संरचना cw_pause_buf *cwbuf = curlx_calloc(1, परिमाण(*cwbuf));
  यदि(cwbuf) {
    cwbuf->type = type;
    यदि(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_सीमा | BUFQ_OPT_NO_SPARES));
    अन्यथा
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  प्रत्यागम cwbuf;
}

स्थिर शून्य cw_pause_buf_free(संरचना cw_pause_buf *cwbuf)
{
  यदि(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

संरचना cw_pause_ctx {
  संरचना Curl_cwriter super;
  संरचना cw_pause_buf *buf;
  संरचना cw_pause_buf *पुच्छ;
  size_t buf_योग;
};

स्थिर CURLcode cw_pause_init(संरचना Curl_easy *दत्त,
                              संरचना Curl_cwriter *writer)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;
  (शून्य)दत्त;
  ctx->buf = रिक्त;
  ctx->पुच्छ = रिक्त;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cw_pause_bufs_free(संरचना cw_pause_ctx *ctx)
{
  यावत्(ctx->buf) {
    संरचना cw_pause_buf *अग्र = ctx->buf->अग्र;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = अग्र;
  }
  ctx->पुच्छ = रिक्त;
}

स्थिर शून्य cw_pause_close(संरचना Curl_easy *दत्त, संरचना Curl_cwriter *writer)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;

  (शून्य)दत्त;
  cw_pause_bufs_free(ctx);
}

स्थिर CURLcode cw_pause_flush(संरचना Curl_easy *दत्त,
                               संरचना Curl_cwriter *cw_pause)
{
  संरचना cw_pause_ctx *ctx = (संरचना cw_pause_ctx *)cw_pause;
  CURLcode परिणाम = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  यावत्(ctx->buf && !Curl_cwriter_is_paused(दत्त)) {
    संरचना cw_pause_buf *first = ctx->buf;
    size_t blen, wlen = 0;
    ध्रुव अचिह्नित वर्ण *buf = रिक्त;

    यदि(Curl_bufq_peek(&first->b, &buf, &blen)) {
      पूर्णाङ्क wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      यदि((wlen < Curl_bufq_len(&first->b)) || first->अग्र)
        wtype &= ~CLIENTWRITE_EOS;
      परिणाम = Curl_cwriter_write(दत्त, cw_pause->अग्र, wtype,
                                  (ध्रुव वर्ण *)buf, wlen);
      CURL_TRC_WRITE(दत्त, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_योग, (अचिह्नित पूर्णाङ्क)wtype,
                     (पूर्णाङ्क)परिणाम);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_योग >= wlen);
      ctx->buf_योग -= wlen;
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा यदि(first->type & CLIENTWRITE_EOS) {
      परिणाम = Curl_cwriter_write(दत्त, cw_pause->अग्र, first->type,
                                  (ध्रुव वर्ण *)buf, 0);
      CURL_TRC_WRITE(दत्त, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_योग, (अचिह्नित पूर्णाङ्क)first->type,
                     (पूर्णाङ्क)परिणाम);
    }

    यदि(Curl_bufq_is_रिक्तानि(&first->b)) {
      ctx->buf = first->अग्र;
      यदि(!ctx->buf)
        ctx->पुच्छ = रिक्त;
      cw_pause_buf_free(first);
    }
  }

  यदि(!परिणाम)
    परिणाम = Curl_cwriter_flush(दत्त, cw_pause->अग्र);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cw_pause_write(संरचना Curl_easy *दत्त,
                               संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                               ध्रुव वर्ण *buf, size_t blen)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t wlen = 0;

  यदि(ctx->buf && !Curl_cwriter_is_paused(दत्त)) {
    परिणाम = cw_pause_flush(दत्त, writer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यावत्(!ctx->buf && !Curl_cwriter_is_paused(दत्त)) {
    पूर्णाङ्क wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    यदि(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    परिणाम = Curl_cwriter_write(दत्त, writer->अग्र, wtype, buf, wlen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    buf += wlen;
    blen -= wlen;
    यदि(!blen)
      प्रत्यागम परिणाम;
  }

  कुरु {
    size_t nwritten = 0;
    यदि(ctx->पुच्छ && (ctx->पुच्छ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      परिणाम = Curl_bufq_cwrite(&ctx->पुच्छ->b, buf, blen, &nwritten);
    }
    अन्यथा {
      /* Need a new buf, type changed */
      संरचना cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      यदि(!cwbuf)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      यदि(ctx->पुच्छ)
        ctx->पुच्छ->अग्र = cwbuf;
      अन्यथा
        ctx->buf = cwbuf;
      ctx->पुच्छ = cwbuf;
      परिणाम = Curl_bufq_cwrite(&ctx->पुच्छ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(दत्त, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (अचिह्नित पूर्णाङ्क)type,
                   ctx->buf_योग + wlen, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_योग += nwritten;
  } यावत्(blen);

  प्रत्यागम परिणाम;
}

ध्रुव संरचना Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  रिक्त,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  परिमाण(संरचना cw_pause_ctx)
};
