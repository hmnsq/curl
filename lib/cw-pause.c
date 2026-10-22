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
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cw-pause.h"


/* body dynbuf sizes */
#నిర్వచించు CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#నిర్వచించు CW_PAUSE_DEC_WRITE_CHUNK   4096

నిర్మాణం cw_pause_buf {
  నిర్మాణం cw_pause_buf *తదుపరి;
  నిర్మాణం bufq b;
  పూర్ణాంకం type;
};

స్థిర నిర్మాణం cw_pause_buf *cw_pause_buf_create(పూర్ణాంకం type, size_t buflen)
{
  నిర్మాణం cw_pause_buf *cwbuf = curlx_calloc(1, పరిమాణం(*cwbuf));
  ఒకవేళ(cwbuf) {
    cwbuf->type = type;
    ఒకవేళ(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_హద్దు | BUFQ_OPT_NO_SPARES));
    లేకపోతే
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  తిరిగి cwbuf;
}

స్థిర శూన్య cw_pause_buf_free(నిర్మాణం cw_pause_buf *cwbuf)
{
  ఒకవేళ(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

నిర్మాణం cw_pause_ctx {
  నిర్మాణం Curl_cwriter super;
  నిర్మాణం cw_pause_buf *buf;
  నిర్మాణం cw_pause_buf *తోక;
  size_t buf_మొత్తం;
};

స్థిర CURLcode cw_pause_init(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం cw_pause_ctx *ctx = writer->ctx;
  (శూన్య)దత్తాంశం;
  ctx->buf = ఖాళీ;
  ctx->తోక = ఖాళీ;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cw_pause_bufs_free(నిర్మాణం cw_pause_ctx *ctx)
{
  అయితే(ctx->buf) {
    నిర్మాణం cw_pause_buf *తదుపరి = ctx->buf->తదుపరి;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = తదుపరి;
  }
  ctx->తోక = ఖాళీ;
}

స్థిర శూన్య cw_pause_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం cw_pause_ctx *ctx = writer->ctx;

  (శూన్య)దత్తాంశం;
  cw_pause_bufs_free(ctx);
}

స్థిర CURLcode cw_pause_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *cw_pause)
{
  నిర్మాణం cw_pause_ctx *ctx = (నిర్మాణం cw_pause_ctx *)cw_pause;
  CURLcode ఫలితం = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  అయితే(ctx->buf && !Curl_cwriter_is_paused(దత్తాంశం)) {
    నిర్మాణం cw_pause_buf *మొదటి = ctx->buf;
    size_t blen, wlen = 0;
    స్థిరరాశి గుర్తులేని అక్షరం *buf = ఖాళీ;

    ఒకవేళ(Curl_bufq_peek(&మొదటి->b, &buf, &blen)) {
      పూర్ణాంకం wtype = మొదటి->type;
      wlen = (మొదటి->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ఒకవేళ((wlen < Curl_bufq_len(&మొదటి->b)) || మొదటి->తదుపరి)
        wtype &= ~CLIENTWRITE_EOS;
      ఫలితం = Curl_cwriter_write(దత్తాంశం, cw_pause->తదుపరి, wtype,
                                  (స్థిరరాశి అక్షరం *)buf, wlen);
      CURL_TRC_WRITE(దత్తాంశం, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_మొత్తం, (గుర్తులేని పూర్ణాంకం)wtype,
                     (పూర్ణాంకం)ఫలితం);
      Curl_bufq_skip(&మొదటి->b, wlen);
      DEBUGASSERT(ctx->buf_మొత్తం >= wlen);
      ctx->buf_మొత్తం -= wlen;
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే ఒకవేళ(మొదటి->type & CLIENTWRITE_EOS) {
      ఫలితం = Curl_cwriter_write(దత్తాంశం, cw_pause->తదుపరి, మొదటి->type,
                                  (స్థిరరాశి అక్షరం *)buf, 0);
      CURL_TRC_WRITE(దత్తాంశం, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_మొత్తం, (గుర్తులేని పూర్ణాంకం)మొదటి->type,
                     (పూర్ణాంకం)ఫలితం);
    }

    ఒకవేళ(Curl_bufq_is_ఖాళీలు(&మొదటి->b)) {
      ctx->buf = మొదటి->తదుపరి;
      ఒకవేళ(!ctx->buf)
        ctx->తోక = ఖాళీ;
      cw_pause_buf_free(మొదటి);
    }
  }

  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cwriter_flush(దత్తాంశం, cw_pause->తదుపరి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cw_pause_write(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                               స్థిరరాశి అక్షరం *buf, size_t blen)
{
  నిర్మాణం cw_pause_ctx *ctx = writer->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t wlen = 0;

  ఒకవేళ(ctx->buf && !Curl_cwriter_is_paused(దత్తాంశం)) {
    ఫలితం = cw_pause_flush(దత్తాంశం, writer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  అయితే(!ctx->buf && !Curl_cwriter_is_paused(దత్తాంశం)) {
    పూర్ణాంకం wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ఒకవేళ(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, wtype, buf, wlen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    buf += wlen;
    blen -= wlen;
    ఒకవేళ(!blen)
      తిరిగి ఫలితం;
  }

  చేయి {
    size_t nwritten = 0;
    ఒకవేళ(ctx->తోక && (ctx->తోక->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      ఫలితం = Curl_bufq_cwrite(&ctx->తోక->b, buf, blen, &nwritten);
    }
    లేకపోతే {
      /* Need a new buf, type changed */
      నిర్మాణం cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ఒకవేళ(!cwbuf)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఒకవేళ(ctx->తోక)
        ctx->తోక->తదుపరి = cwbuf;
      లేకపోతే
        ctx->buf = cwbuf;
      ctx->తోక = cwbuf;
      ఫలితం = Curl_bufq_cwrite(&ctx->తోక->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(దత్తాంశం, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (గుర్తులేని పూర్ణాంకం)type,
                   ctx->buf_మొత్తం + wlen, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_మొత్తం += nwritten;
  } అయితే(blen);

  తిరిగి ఫలితం;
}

స్థిరరాశి నిర్మాణం Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ఖాళీ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  పరిమాణం(నిర్మాణం cw_pause_ctx)
};
