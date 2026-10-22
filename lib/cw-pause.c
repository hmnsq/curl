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
#समावेश "bufq.h"
#समावेश "cfilters.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "cw-pause.h"


/* body dynbuf sizes */
#घोषणा CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#घोषणा CW_PAUSE_DEC_WRITE_CHUNK   4096

काष्ठा cw_pause_buf {
  काष्ठा cw_pause_buf *अग्र;
  काष्ठा bufq b;
  पूर्णांक type;
};

अटल काष्ठा cw_pause_buf *cw_pause_buf_create(पूर्णांक type, माप_प्रकार buflen)
{
  काष्ठा cw_pause_buf *cwbuf = curlx_calloc(1, माप(*cwbuf));
  अगर(cwbuf) {
    cwbuf->type = type;
    अगर(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    अन्यथा
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  वापस cwbuf;
}

अटल व्योम cw_pause_buf_free(काष्ठा cw_pause_buf *cwbuf)
{
  अगर(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

काष्ठा cw_pause_ctx {
  काष्ठा Curl_cwriter super;
  काष्ठा cw_pause_buf *buf;
  काष्ठा cw_pause_buf *पुच्छ;
  माप_प्रकार buf_योग;
};

अटल CURLcode cw_pause_init(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_cwriter *writer)
{
  काष्ठा cw_pause_ctx *ctx = writer->ctx;
  (व्योम)तथ्य;
  ctx->buf = शून्य;
  ctx->पुच्छ = शून्य;
  वापस CURLE_OK;
}

अटल व्योम cw_pause_bufs_free(काष्ठा cw_pause_ctx *ctx)
{
  जबतक(ctx->buf) {
    काष्ठा cw_pause_buf *अग्र = ctx->buf->अग्र;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = अग्र;
  }
  ctx->पुच्छ = शून्य;
}

अटल व्योम cw_pause_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_cwriter *writer)
{
  काष्ठा cw_pause_ctx *ctx = writer->ctx;

  (व्योम)तथ्य;
  cw_pause_bufs_free(ctx);
}

अटल CURLcode cw_pause_flush(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *cw_pause)
{
  काष्ठा cw_pause_ctx *ctx = (काष्ठा cw_pause_ctx *)cw_pause;
  CURLcode परिणाम = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  जबतक(ctx->buf && !Curl_cwriter_is_paused(तथ्य)) {
    काष्ठा cw_pause_buf *first = ctx->buf;
    माप_प्रकार blen, wlen = 0;
    स्थिर अचिन्हित अक्षर *buf = शून्य;

    अगर(Curl_bufq_peek(&first->b, &buf, &blen)) {
      पूर्णांक wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      अगर((wlen < Curl_bufq_len(&first->b)) || first->अग्र)
        wtype &= ~CLIENTWRITE_EOS;
      परिणाम = Curl_cwriter_write(तथ्य, cw_pause->अग्र, wtype,
                                  (स्थिर अक्षर *)buf, wlen);
      CURL_TRC_WRITE(तथ्य, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_योग, (अचिन्हित पूर्णांक)wtype,
                     (पूर्णांक)परिणाम);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_योग >= wlen);
      ctx->buf_योग -= wlen;
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा अगर(first->type & CLIENTWRITE_EOS) {
      परिणाम = Curl_cwriter_write(तथ्य, cw_pause->अग्र, first->type,
                                  (स्थिर अक्षर *)buf, 0);
      CURL_TRC_WRITE(तथ्य, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_योग, (अचिन्हित पूर्णांक)first->type,
                     (पूर्णांक)परिणाम);
    }

    अगर(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->अग्र;
      अगर(!ctx->buf)
        ctx->पुच्छ = शून्य;
      cw_pause_buf_free(first);
    }
  }

  अगर(!परिणाम)
    परिणाम = Curl_cwriter_flush(तथ्य, cw_pause->अग्र);
  वापस परिणाम;
}

अटल CURLcode cw_pause_write(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer, पूर्णांक type,
                               स्थिर अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा cw_pause_ctx *ctx = writer->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार wlen = 0;

  अगर(ctx->buf && !Curl_cwriter_is_paused(तथ्य)) {
    परिणाम = cw_pause_flush(तथ्य, writer);
    अगर(परिणाम)
      वापस परिणाम;
  }

  जबतक(!ctx->buf && !Curl_cwriter_is_paused(तथ्य)) {
    पूर्णांक wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    अगर(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, wtype, buf, wlen);
    अगर(परिणाम)
      वापस परिणाम;
    buf += wlen;
    blen -= wlen;
    अगर(!blen)
      वापस परिणाम;
  }

  करो {
    माप_प्रकार nwritten = 0;
    अगर(ctx->पुच्छ && (ctx->पुच्छ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      परिणाम = Curl_bufq_cwrite(&ctx->पुच्छ->b, buf, blen, &nwritten);
    }
    अन्यथा {
      /* Need a new buf, type changed */
      काष्ठा cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      अगर(!cwbuf)
        वापस CURLE_OUT_OF_MEMORY;
      अगर(ctx->पुच्छ)
        ctx->पुच्छ->अग्र = cwbuf;
      अन्यथा
        ctx->buf = cwbuf;
      ctx->पुच्छ = cwbuf;
      परिणाम = Curl_bufq_cwrite(&ctx->पुच्छ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(तथ्य, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (अचिन्हित पूर्णांक)type,
                   ctx->buf_योग + wlen, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_योग += nwritten;
  } जबतक(blen);

  वापस परिणाम;
}

स्थिर काष्ठा Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  शून्य,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  माप(काष्ठा cw_pause_ctx)
};
