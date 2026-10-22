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
#define CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#define CW_PAUSE_DEC_WRITE_CHUNK   4096

संरचना cw_pause_buf {
  संरचना cw_pause_buf *next;
  संरचना bufq b;
  पूर्णाङ्क type;
};

स्थिर संरचना cw_pause_buf *cw_pause_buf_create(पूर्णाङ्क type, माप_प्रकार buflen)
{
  संरचना cw_pause_buf *cwbuf = curlx_calloc(1, परिमाण(*cwbuf));
  यदि(cwbuf) {
    cwbuf->type = type;
    यदि(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
    अन्यथा
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  फर्काऊ cwbuf;
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
  संरचना cw_pause_buf *tail;
  माप_प्रकार buf_total;
};

स्थिर CURLcode cw_pause_init(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;
  (शून्य)data;
  ctx->buf = रिक्त;
  ctx->tail = रिक्त;
  फर्काऊ CURLE_OK;
}

स्थिर शून्य cw_pause_bufs_free(संरचना cw_pause_ctx *ctx)
{
  जबसम्म(ctx->buf) {
    संरचना cw_pause_buf *next = ctx->buf->next;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = next;
  }
  ctx->tail = रिक्त;
}

स्थिर शून्य cw_pause_close(संरचना Curl_easy *data, संरचना Curl_cwriter *writer)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;

  (शून्य)data;
  cw_pause_bufs_free(ctx);
}

स्थिर CURLcode cw_pause_flush(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *cw_pause)
{
  संरचना cw_pause_ctx *ctx = (संरचना cw_pause_ctx *)cw_pause;
  CURLcode result = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  जबसम्म(ctx->buf && !Curl_cwriter_is_paused(data)) {
    संरचना cw_pause_buf *first = ctx->buf;
    माप_प्रकार blen, wlen = 0;
    अचर अचिन्हित वर्ण *buf = रिक्त;

    यदि(Curl_bufq_peek(&first->b, &buf, &blen)) {
      पूर्णाङ्क wtype = first->type;
      wlen = (first->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      यदि((wlen < Curl_bufq_len(&first->b)) || first->next)
        wtype &= ~CLIENTWRITE_EOS;
      result = Curl_cwriter_write(data, cw_pause->next, wtype,
                                  (अचर वर्ण *)buf, wlen);
      CURL_TRC_WRITE(data, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_total, (अचिन्हित पूर्णाङ्क)wtype,
                     (पूर्णाङ्क)result);
      Curl_bufq_skip(&first->b, wlen);
      DEBUGASSERT(ctx->buf_total >= wlen);
      ctx->buf_total -= wlen;
      यदि(result)
        फर्काऊ result;
    }
    अन्यथा यदि(first->type & CLIENTWRITE_EOS) {
      result = Curl_cwriter_write(data, cw_pause->next, first->type,
                                  (अचर वर्ण *)buf, 0);
      CURL_TRC_WRITE(data, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_total, (अचिन्हित पूर्णाङ्क)first->type,
                     (पूर्णाङ्क)result);
    }

    यदि(Curl_bufq_is_empty(&first->b)) {
      ctx->buf = first->next;
      यदि(!ctx->buf)
        ctx->tail = रिक्त;
      cw_pause_buf_free(first);
    }
  }

  यदि(!result)
    result = Curl_cwriter_flush(data, cw_pause->next);
  फर्काऊ result;
}

स्थिर CURLcode cw_pause_write(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                               अचर वर्ण *buf, माप_प्रकार blen)
{
  संरचना cw_pause_ctx *ctx = writer->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार wlen = 0;

  यदि(ctx->buf && !Curl_cwriter_is_paused(data)) {
    result = cw_pause_flush(data, writer);
    यदि(result)
      फर्काऊ result;
  }

  जबसम्म(!ctx->buf && !Curl_cwriter_is_paused(data)) {
    पूर्णाङ्क wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    यदि(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    result = Curl_cwriter_write(data, writer->next, wtype, buf, wlen);
    यदि(result)
      फर्काऊ result;
    buf += wlen;
    blen -= wlen;
    यदि(!blen)
      फर्काऊ result;
  }

  गर {
    माप_प्रकार nwritten = 0;
    यदि(ctx->tail && (ctx->tail->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    अन्यथा {
      /* Need a new buf, type changed */
      संरचना cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      यदि(!cwbuf)
        फर्काऊ CURLE_OUT_OF_MEMORY;
      यदि(ctx->tail)
        ctx->tail->next = cwbuf;
      अन्यथा
        ctx->buf = cwbuf;
      ctx->tail = cwbuf;
      result = Curl_bufq_cwrite(&ctx->tail->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(data, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (अचिन्हित पूर्णाङ्क)type,
                   ctx->buf_total + wlen, (पूर्णाङ्क)result);
    यदि(result)
      फर्काऊ result;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_total += nwritten;
  } जबसम्म(blen);

  फर्काऊ result;
}

अचर संरचना Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  रिक्त,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  परिमाण(संरचना cw_pause_ctx)
};
