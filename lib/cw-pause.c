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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cw-pause.h"


/* body dynbuf sizes */
#ವ್ಯಾಖ್ಯಾನಿಸು CW_PAUSE_BUF_CHUNK         (16 * 1024)
/* when content decoding, write data in chunks */
#ವ್ಯಾಖ್ಯಾನಿಸು CW_PAUSE_DEC_WRITE_CHUNK   4096

ರಚನೆ cw_pause_buf {
  ರಚನೆ cw_pause_buf *ಮುಂದಿನ;
  ರಚನೆ bufq b;
  ಪೂರ್ಣಾಂಕ type;
};

ಸ್ಥಿರ ರಚನೆ cw_pause_buf *cw_pause_buf_create(ಪೂರ್ಣಾಂಕ type, size_t buflen)
{
  ರಚನೆ cw_pause_buf *cwbuf = curlx_calloc(1, ಗಾತ್ರ(*cwbuf));
  ಆದರೆ(cwbuf) {
    cwbuf->type = type;
    ಆದರೆ(type & CLIENTWRITE_BODY)
      Curl_bufq_init2(&cwbuf->b, CW_PAUSE_BUF_CHUNK, 1,
                      (BUFQ_OPT_SOFT_ಎಲ್ಲೆ | BUFQ_OPT_NO_SPARES));
    ಇಲ್ಲದಿದ್ದರೆ
      Curl_bufq_init(&cwbuf->b, buflen, 1);
  }
  ಹಿಂತಿರುಗಿ cwbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_buf_free(ರಚನೆ cw_pause_buf *cwbuf)
{
  ಆದರೆ(cwbuf) {
    Curl_bufq_free(&cwbuf->b);
    curlx_free(cwbuf);
  }
}

ರಚನೆ cw_pause_ctx {
  ರಚನೆ Curl_cwriter super;
  ರಚನೆ cw_pause_buf *buf;
  ರಚನೆ cw_pause_buf *ಬಾಲ;
  size_t buf_ಒಟ್ಟು;
};

ಸ್ಥಿರ CURLcode cw_pause_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ cw_pause_ctx *ctx = writer->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx->buf = ಖಾಲಿ;
  ctx->ಬಾಲ = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_bufs_free(ರಚನೆ cw_pause_ctx *ctx)
{
  ಆಗ(ctx->buf) {
    ರಚನೆ cw_pause_buf *ಮುಂದಿನ = ctx->buf->ಮುಂದಿನ;
    cw_pause_buf_free(ctx->buf);
    ctx->buf = ಮುಂದಿನ;
  }
  ctx->ಬಾಲ = ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_pause_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ cw_pause_ctx *ctx = writer->ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  cw_pause_bufs_free(ctx);
}

ಸ್ಥಿರ CURLcode cw_pause_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *cw_pause)
{
  ರಚನೆ cw_pause_ctx *ctx = (ರಚನೆ cw_pause_ctx *)cw_pause;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* write the front of the chain until it blocks or gets empty */
  ಆಗ(ctx->buf && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ರಚನೆ cw_pause_buf *ಮೊದಲ = ctx->buf;
    size_t blen, wlen = 0;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf = ಖಾಲಿ;

    ಆದರೆ(Curl_bufq_peek(&ಮೊದಲ->b, &buf, &blen)) {
      ಪೂರ್ಣಾಂಕ wtype = ಮೊದಲ->type;
      wlen = (ಮೊದಲ->type & CLIENTWRITE_BODY) ?
             CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
      /* EOS only goes with the very last piece, same as in cw_pause_write */
      ಆದರೆ((wlen < Curl_bufq_len(&ಮೊದಲ->b)) || ಮೊದಲ->ಮುಂದಿನ)
        wtype &= ~CLIENTWRITE_EOS;
      ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, cw_pause->ಮುಂದಿನ, wtype,
                                  (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, wlen);
      CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[PAUSE] flushed %zu/%zu bytes, type=%x -> %d",
                     wlen, ctx->buf_ಒಟ್ಟು, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)wtype,
                     (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      Curl_bufq_skip(&ಮೊದಲ->b, wlen);
      DEBUGASSERT(ctx->buf_ಒಟ್ಟು >= wlen);
      ctx->buf_ಒಟ್ಟು -= wlen;
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೊದಲ->type & CLIENTWRITE_EOS) {
      ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, cw_pause->ಮುಂದಿನ, ಮೊದಲ->type,
                                  (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, 0);
      CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[PAUSE] flushed 0/%zu bytes, type=%x -> %d",
                     ctx->buf_ಒಟ್ಟು, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಮೊದಲ->type,
                     (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    }

    ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ಮೊದಲ->b)) {
      ctx->buf = ಮೊದಲ->ಮುಂದಿನ;
      ಆದರೆ(!ctx->buf)
        ctx->ಬಾಲ = ಖಾಲಿ;
      cw_pause_buf_free(ಮೊದಲ);
    }
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cwriter_flush(ದತ್ತಾಂಶ, cw_pause->ಮುಂದಿನ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cw_pause_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ cw_pause_ctx *ctx = writer->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t wlen = 0;

  ಆದರೆ(ctx->buf && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = cw_pause_flush(ದತ್ತಾಂಶ, writer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆಗ(!ctx->buf && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ಪೂರ್ಣಾಂಕ wtype = type;
    DEBUGASSERT(!ctx->buf);
    /* content decoding might blow up size considerably, write smaller
     * chunks to make pausing need buffer less. */
    wlen = (type & CLIENTWRITE_BODY) ?
           CURLMIN(blen, CW_PAUSE_DEC_WRITE_CHUNK) : blen;
    ಆದರೆ(wlen < blen)
      wtype &= ~CLIENTWRITE_EOS;
    ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, wtype, buf, wlen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    buf += wlen;
    blen -= wlen;
    ಆದರೆ(!blen)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಮಾಡು {
    size_t nwritten = 0;
    ಆದರೆ(ctx->ಬಾಲ && (ctx->ಬಾಲ->type == type) && (type & CLIENTWRITE_BODY)) {
      /* same type and body, append to tail buffer which has a soft
       * limit and should take everything up to OOM. */
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->ಬಾಲ->b, buf, blen, &nwritten);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Need a new buf, type changed */
      ರಚನೆ cw_pause_buf *cwbuf = cw_pause_buf_create(type, blen);
      ಆದರೆ(!cwbuf)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಆದರೆ(ctx->ಬಾಲ)
        ctx->ಬಾಲ->ಮುಂದಿನ = cwbuf;
      ಇಲ್ಲದಿದ್ದರೆ
        ctx->buf = cwbuf;
      ctx->ಬಾಲ = cwbuf;
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->ಬಾಲ->b, buf, blen, &nwritten);
    }
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "[PAUSE] buffer %zu more bytes of type %x, "
                   "total=%zu -> %d", nwritten, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type,
                   ctx->buf_ಒಟ್ಟು + wlen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    buf += nwritten;
    blen -= nwritten;
    ctx->buf_ಒಟ್ಟು += nwritten;
  } ಆಗ(blen);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype Curl_cwt_pause = {
  "cw-pause",
  ಖಾಲಿ,
  0,
  cw_pause_init,
  cw_pause_write,
  cw_pause_flush,
  cw_pause_close,
  ಗಾತ್ರ(ರಚನೆ cw_pause_ctx)
};
