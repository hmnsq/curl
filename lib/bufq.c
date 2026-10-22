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

#ಅನ್ತರ್ಭಾವ "bufq.h"

ಸ್ಥಿರ bool chunk_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk)
{
  ಹಿಂತಿರುಗಿ chunk->r_ಸ್ಥಾನಾಂತರ >= chunk->w_ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ bool chunk_is_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk)
{
  ಹಿಂತಿರುಗಿ chunk->w_ಸ್ಥಾನಾಂತರ >= chunk->dlen;
}

ಸ್ಥಿರ size_t chunk_len(ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk)
{
  ಹಿಂತಿರುಗಿ chunk->w_ಸ್ಥಾನಾಂತರ - chunk->r_ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_reset(ರಚನೆ buf_chunk *chunk)
{
  chunk->ಮುಂದಿನ = ಖಾಲಿ;
  chunk->r_ಸ್ಥಾನಾಂತರ = chunk->w_ಸ್ಥಾನಾಂತರ = 0;
}

ಸ್ಥಿರ size_t chunk_append(ರಚನೆ buf_chunk *chunk,
                           ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ದತ್ತಾಂಶ[chunk->w_ಸ್ಥಾನಾಂತರ];
  size_t n = chunk->dlen - chunk->w_ಸ್ಥಾನಾಂತರ;
  DEBUGASSERT(chunk->dlen >= chunk->w_ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(n) {
    n = CURLMIN(n, len);
    ಸ್ಮರಣೆ_ನಕಲು(p, buf, n);
    chunk->w_ಸ್ಥಾನಾಂತರ += n;
  }
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ size_t chunk_read(ರಚನೆ buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ದತ್ತಾಂಶ[chunk->r_ಸ್ಥಾನಾಂತರ];
  size_t n = chunk->w_ಸ್ಥಾನಾಂತರ - chunk->r_ಸ್ಥಾನಾಂತರ;
  DEBUGASSERT(chunk->w_ಸ್ಥಾನಾಂತರ >= chunk->r_ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(!n) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n <= len) {
    ಸ್ಮರಣೆ_ನಕಲು(buf, p, n);
    chunk->r_ಸ್ಥಾನಾಂತರ = chunk->w_ಸ್ಥಾನಾಂತರ = 0;
    ಹಿಂತಿರುಗಿ n;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಮರಣೆ_ನಕಲು(buf, p, len);
    chunk->r_ಸ್ಥಾನಾಂತರ += len;
    ಹಿಂತಿರುಗಿ len;
  }
}

ಸ್ಥಿರ CURLcode chunk_slurpn(ರಚನೆ buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             ಶೂನ್ಯ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.ದತ್ತಾಂಶ[chunk->w_ಸ್ಥಾನಾಂತರ];
  size_t n = chunk->dlen - chunk->w_ಸ್ಥಾನಾಂತರ; /* free amount */
  CURLcode ಫಲಿತಾಂಶ;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(!n)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಆದರೆ(max_len && n > max_len)
    n = max_len;
  ಫಲಿತಾಂಶ = reader(reader_ctx, p, n, pnread);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_ಸ್ಥಾನಾಂತರ += *pnread;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_peek(ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk,
                       ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_ಸ್ಥಾನಾಂತರ >= chunk->r_ಸ್ಥಾನಾಂತರ);
  *pbuf = &chunk->x.ದತ್ತಾಂಶ[chunk->r_ಸ್ಥಾನಾಂತರ];
  *plen = chunk->w_ಸ್ಥಾನಾಂತರ - chunk->r_ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_peek_at(ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk, size_t ಸ್ಥಾನಾಂತರ,
                          ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen)
{
  ಸ್ಥಾನಾಂತರ += chunk->r_ಸ್ಥಾನಾಂತರ;
  DEBUGASSERT(chunk->w_ಸ್ಥಾನಾಂತರ >= ಸ್ಥಾನಾಂತರ);
  *pbuf = &chunk->x.ದತ್ತಾಂಶ[ಸ್ಥಾನಾಂತರ];
  *plen = chunk->w_ಸ್ಥಾನಾಂತರ - ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ size_t chunk_skip(ರಚನೆ buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_ಸ್ಥಾನಾಂತರ - chunk->r_ಸ್ಥಾನಾಂತರ;
  DEBUGASSERT(chunk->w_ಸ್ಥಾನಾಂತರ >= chunk->r_ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(n) {
    n = CURLMIN(n, amount);
    chunk->r_ಸ್ಥಾನಾಂತರ += n;
    ಆದರೆ(chunk->r_ಸ್ಥಾನಾಂತರ == chunk->w_ಸ್ಥಾನಾಂತರ)
      chunk->r_ಸ್ಥಾನಾಂತರ = chunk->w_ಸ್ಥಾನಾಂತರ = 0;
  }
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_list_free(ರಚನೆ buf_chunk **anchor)
{
  ರಚನೆ buf_chunk *chunk;
  ಆಗ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->ಮುಂದಿನ;
    curlx_free(chunk);
  }
}

ಶೂನ್ಯ Curl_bufcp_init(ರಚನೆ bufc_pool *pool,
                     size_t chunk_ಗಾತ್ರ, size_t ಹೆಚ್ಚುವರಿ_max)
{
  DEBUGASSERT(chunk_ಗಾತ್ರ > 0);
  DEBUGASSERT(ಹೆಚ್ಚುವರಿ_max > 0);
  memset(pool, 0, ಗಾತ್ರ(*pool));
  pool->chunk_ಗಾತ್ರ = chunk_ಗಾತ್ರ;
  pool->ಹೆಚ್ಚುವರಿ_max = ಹೆಚ್ಚುವರಿ_max;
}

ಸ್ಥಿರ CURLcode bufcp_take(ರಚನೆ bufc_pool *pool,
                           ರಚನೆ buf_chunk **pchunk)
{
  ರಚನೆ buf_chunk *chunk = ಖಾಲಿ;

  ಆದರೆ(pool->ಹೆಚ್ಚುವರಿ) {
    chunk = pool->ಹೆಚ್ಚುವರಿ;
    pool->ಹೆಚ್ಚುವರಿ = chunk->ಮುಂದಿನ;
    --pool->ಹೆಚ್ಚುವರಿ_ಎಣಿಕೆ;
    chunk_reset(chunk);
    *pchunk = chunk;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ಆದರೆ(pool->chunk_ಗಾತ್ರ > SIZE_MAX - ಗಾತ್ರ(*chunk)) {
    *pchunk = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  chunk = curlx_calloc(1, ಗಾತ್ರ(*chunk) + pool->chunk_ಗಾತ್ರ);
  ಆದರೆ(!chunk) {
    *pchunk = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  chunk->dlen = pool->chunk_ಗಾತ್ರ;
  *pchunk = chunk;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ bufcp_put(ರಚನೆ bufc_pool *pool,
                      ರಚನೆ buf_chunk *chunk)
{
  ಆದರೆ(pool->ಹೆಚ್ಚುವರಿ_ಎಣಿಕೆ >= pool->ಹೆಚ್ಚುವರಿ_max) {
    curlx_free(chunk);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    chunk_reset(chunk);
    chunk->ಮುಂದಿನ = pool->ಹೆಚ್ಚುವರಿ;
    pool->ಹೆಚ್ಚುವರಿ = chunk;
    ++pool->ಹೆಚ್ಚುವರಿ_ಎಣಿಕೆ;
  }
}

ಶೂನ್ಯ Curl_bufcp_free(ರಚನೆ bufc_pool *pool)
{
  chunk_list_free(&pool->ಹೆಚ್ಚುವರಿ);
  pool->ಹೆಚ್ಚುವರಿ_ಎಣಿಕೆ = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ bufq_init(ರಚನೆ bufq *q, ರಚನೆ bufc_pool *pool,
                      size_t chunk_ಗಾತ್ರ, size_t max_chunks, ಪೂರ್ಣಾಂಕ opts)
{
  DEBUGASSERT(chunk_ಗಾತ್ರ > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, ಗಾತ್ರ(*q));
  q->chunk_ಗಾತ್ರ = chunk_ಗಾತ್ರ;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

ಶೂನ್ಯ Curl_bufq_init2(ರಚನೆ bufq *q, size_t chunk_ಗಾತ್ರ, size_t max_chunks,
                     ಪೂರ್ಣಾಂಕ opts)
{
  bufq_init(q, ಖಾಲಿ, chunk_ಗಾತ್ರ, max_chunks, opts);
}

ಶೂನ್ಯ Curl_bufq_init(ರಚನೆ bufq *q, size_t chunk_ಗಾತ್ರ, size_t max_chunks)
{
  bufq_init(q, ಖಾಲಿ, chunk_ಗಾತ್ರ, max_chunks, BUFQ_OPT_NONE);
}

ಶೂನ್ಯ Curl_bufq_initp(ರಚನೆ bufq *q, ರಚನೆ bufc_pool *pool,
                     size_t max_chunks, ಪೂರ್ಣಾಂಕ opts)
{
  bufq_init(q, pool, pool->chunk_ಗಾತ್ರ, max_chunks, opts);
}

ಶೂನ್ಯ Curl_bufq_free(ರಚನೆ bufq *q)
{
  chunk_list_free(&q->ತಲೆ);
  chunk_list_free(&q->ಹೆಚ್ಚುವರಿ);
  q->ಬಾಲ = ಖಾಲಿ;
  q->chunk_ಎಣಿಕೆ = 0;
}

ಶೂನ್ಯ Curl_bufq_reset(ರಚನೆ bufq *q)
{
  ರಚನೆ buf_chunk *chunk;
  ಆಗ(q->ತಲೆ) {
    chunk = q->ತಲೆ;
    q->ತಲೆ = chunk->ಮುಂದಿನ;
    chunk->ಮುಂದಿನ = q->ಹೆಚ್ಚುವರಿ;
    q->ಹೆಚ್ಚುವರಿ = chunk;
  }
  q->ಬಾಲ = ಖಾಲಿ;
}

size_t Curl_bufq_len(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ buf_chunk *chunk = q->ತಲೆ;
  size_t len = 0;
  ಆಗ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ len;
}

bool Curl_bufq_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q)
{
  ಹಿಂತಿರುಗಿ !q->ತಲೆ || chunk_is_ಖಾಲಿಗಳು(q->ತಲೆ);
}

bool Curl_bufq_is_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q)
{
  ಆದರೆ(!q->ಬಾಲ || q->ಹೆಚ್ಚುವರಿ)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(q->chunk_ಎಣಿಕೆ < q->max_chunks)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(q->chunk_ಎಣಿಕೆ > q->max_chunks)
    ಹಿಂತಿರುಗಿ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ಹಿಂತಿರುಗಿ chunk_is_ತುಂಬಿದ(q->ಬಾಲ);
}

ಸ್ಥಿರ ರಚನೆ buf_chunk *get_ಹೆಚ್ಚುವರಿ(ರಚನೆ bufq *q)
{
  ರಚನೆ buf_chunk *chunk = ಖಾಲಿ;

  ಆದರೆ(q->ಹೆಚ್ಚುವರಿ) {
    chunk = q->ಹೆಚ್ಚುವರಿ;
    q->ಹೆಚ್ಚುವರಿ = chunk->ಮುಂದಿನ;
    chunk_reset(chunk);
    ಹಿಂತಿರುಗಿ chunk;
  }

  ಆದರೆ(q->chunk_ಎಣಿಕೆ >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_ಎಲ್ಲೆ)))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆದರೆ(q->pool) {
    ಆದರೆ(bufcp_take(q->pool, &chunk))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ++q->chunk_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ chunk;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Check for integer overflow before allocation */
    ಆದರೆ(q->chunk_ಗಾತ್ರ > SIZE_MAX - ಗಾತ್ರ(*chunk)) {
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    chunk = curlx_calloc(1, ಗಾತ್ರ(*chunk) + q->chunk_ಗಾತ್ರ);
    ಆದರೆ(!chunk)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    chunk->dlen = q->chunk_ಗಾತ್ರ;
    ++q->chunk_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ chunk;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ prune_ತಲೆ(ರಚನೆ bufq *q)
{
  ರಚನೆ buf_chunk *chunk;

  ಆಗ(q->ತಲೆ && chunk_is_ಖಾಲಿಗಳು(q->ತಲೆ)) {
    chunk = q->ತಲೆ;
    q->ತಲೆ = chunk->ಮುಂದಿನ;
    ಆದರೆ(q->ಬಾಲ == chunk)
      q->ಬಾಲ = q->ತಲೆ;
    ಆದರೆ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((q->chunk_ಎಣಿಕೆ > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      chunk->ಮುಂದಿನ = q->ಹೆಚ್ಚುವರಿ;
      q->ಹೆಚ್ಚುವರಿ = chunk;
    }
  }
}

ಸ್ಥಿರ ರಚನೆ buf_chunk *get_non_ತುಂಬಿದ_ಬಾಲ(ರಚನೆ bufq *q)
{
  ರಚನೆ buf_chunk *chunk;

  ಆದರೆ(q->ಬಾಲ && !chunk_is_ತುಂಬಿದ(q->ಬಾಲ))
    ಹಿಂತಿರುಗಿ q->ಬಾಲ;
  chunk = get_ಹೆಚ್ಚುವರಿ(q);
  ಆದರೆ(chunk) {
    /* new tail, and possibly new head */
    ಆದರೆ(q->ಬಾಲ) {
      q->ಬಾಲ->ಮುಂದಿನ = chunk;
      q->ಬಾಲ = chunk;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      DEBUGASSERT(!q->ತಲೆ);
      q->ತಲೆ = q->ಬಾಲ = chunk;
    }
  }
  ಹಿಂತಿರುಗಿ chunk;
}

CURLcode Curl_bufq_write(ರಚನೆ bufq *q,
                         ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ರಚನೆ buf_chunk *ಬಾಲ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ಆಗ(len) {
    ಬಾಲ = get_non_ತುಂಬಿದ_ಬಾಲ(q);
    ಆದರೆ(!ಬಾಲ) {
      ಆದರೆ((q->chunk_ಎಣಿಕೆ < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_ಎಲ್ಲೆ))
        /* should have gotten a tail, but did not */
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;
    }
    n = chunk_append(ಬಾಲ, buf, len);
    ಆದರೆ(!n)
      ನಿಲ್ಲಿಸು;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ಹಿಂತಿರುಗಿ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ರಚನೆ bufq *q,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len,
                          size_t *pnwritten)
{
  ಹಿಂತಿರುಗಿ Curl_bufq_write(q, (ಸ್ಥಿರಾಂಕ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ರಚನೆ bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ಆಗ(len && q->ತಲೆ) {
    size_t n = chunk_read(q->ತಲೆ, buf, len);
    ಆದರೆ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_ತಲೆ(q);
  }
  ಹಿಂತಿರುಗಿ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ರಚನೆ bufq *q, ಅಕ್ಷರ *buf, size_t len,
                         size_t *pnread)
{
  ಹಿಂತಿರುಗಿ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ರಚನೆ bufq *q,
                    ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen)
{
  ಆದರೆ(q->ತಲೆ && chunk_is_ಖಾಲಿಗಳು(q->ತಲೆ)) {
    prune_ತಲೆ(q);
  }
  ಆದರೆ(q->ತಲೆ && !chunk_is_ಖಾಲಿಗಳು(q->ತಲೆ)) {
    chunk_peek(q->ತಲೆ, pbuf, plen);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  *pbuf = ಖಾಲಿ;
  *plen = 0;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_bufq_peek_at(ರಚನೆ bufq *q, size_t ಸ್ಥಾನಾಂತರ,
                       ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen)
{
  ರಚನೆ buf_chunk *c = q->ತಲೆ;
  size_t clen;

  ಆಗ(c) {
    clen = chunk_len(c);
    ಆದರೆ(!clen)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(ಸ್ಥಾನಾಂತರ >= clen) {
      ಸ್ಥಾನಾಂತರ -= clen;
      c = c->ಮುಂದಿನ;
      ಮುಂದುವರಿ;
    }
    chunk_peek_at(c, ಸ್ಥಾನಾಂತರ, pbuf, plen);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  *pbuf = ಖಾಲಿ;
  *plen = 0;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_bufq_skip(ರಚನೆ bufq *q, size_t amount)
{
  size_t n;

  ಆಗ(amount && q->ತಲೆ) {
    n = chunk_skip(q->ತಲೆ, amount);
    amount -= n;
    prune_ತಲೆ(q);
  }
}

CURLcode Curl_bufq_pass(ರಚನೆ bufq *q, Curl_bufq_writer *writer,
                        ಶೂನ್ಯ *writer_ctx, size_t *pwritten)
{
  ಸ್ಥಿರಾಂಕ uint8_t *buf;
  size_t blen;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pwritten = 0;
  ಆಗ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    ಫಲಿತಾಂಶ = writer(writer_ctx, buf, blen, &chunk_written);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ((ಫಲಿತಾಂಶ == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        ಫಲಿತಾಂಶ = CURLE_OK;
      }
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!chunk_written) {
      ಆದರೆ(!*pwritten) {
        /* treat as blocked */
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
      }
      ನಿಲ್ಲಿಸು;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_bufq_write_pass(ರಚನೆ bufq *q,
                              ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ಶೂನ್ಯ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ಆಗ(len) {
    ಆದರೆ(Curl_bufq_is_ತುಂಬಿದ(q)) {
      /* try to make room in case we are full */
      ಫಲಿತಾಂಶ = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
          /* real error, fail */
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        /* would block, bufq is full, give up */
        ನಿಲ್ಲಿಸು;
      }
    }

    /* Add to bufq as much as there is room for */
    ಫಲಿತಾಂಶ = Curl_bufq_write(q, buf, len, &n);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
        /* real error, fail */
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      /* result == CURLE_AGAIN */
      ಆದರೆ(*pwritten)
        /* we did write successfully before */
        ಫಲಿತಾಂಶ = CURLE_OK;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ನಿಲ್ಲಿಸು;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ಹಿಂತಿರುಗಿ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ರಚನೆ bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ಶೂನ್ಯ *reader_ctx,
                        size_t *pnread)
{
  ರಚನೆ buf_chunk *ಬಾಲ = ಖಾಲಿ;

  *pnread = 0;
  ಬಾಲ = get_non_ತುಂಬಿದ_ಬಾಲ(q);
  ಆದರೆ(!ಬಾಲ) {
    ಆದರೆ(q->chunk_ಎಣಿಕೆ < q->max_chunks)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    /* full, blocked */
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }

  ಹಿಂತಿರುಗಿ chunk_slurpn(ಬಾಲ, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ಸ್ಥಿರ CURLcode bufq_slurpn(ರಚನೆ bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, ಶೂನ್ಯ *reader_ctx,
                            size_t *pnread)
{
  CURLcode ಫಲಿತಾಂಶ;

  *pnread = 0;
  ಆಗ(1) {
    size_t n;
    ಫಲಿತಾಂಶ = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(!*pnread || ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಫಲಿತಾಂಶ = CURLE_OK;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n == 0) {
      /* eof, result remains CURLE_OK */
      ನಿಲ್ಲಿಸು;
    }
    *pnread += n;
    ಆದರೆ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ಆದರೆ(!max_len)
        ನಿಲ್ಲಿಸು;
    }
    /* give up slurping when we get less bytes than we asked for */
    ಆದರೆ(q->ಬಾಲ && !chunk_is_ತುಂಬಿದ(q->ಬಾಲ))
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_bufq_slurp(ರಚನೆ bufq *q, Curl_bufq_reader *reader,
                         ಶೂನ್ಯ *reader_ctx, size_t *pnread)
{
  ಹಿಂತಿರುಗಿ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
