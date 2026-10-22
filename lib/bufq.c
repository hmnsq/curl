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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"

ਸਥਿਰ bool chunk_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਬਣਤਰ buf_chunk *chunk)
{
  ਵਾਪਸ chunk->r_ਵਿੱਥ >= chunk->w_ਵਿੱਥ;
}

ਸਥਿਰ bool chunk_is_ਭਰਿਆ(ਅਚਲ ਬਣਤਰ buf_chunk *chunk)
{
  ਵਾਪਸ chunk->w_ਵਿੱਥ >= chunk->dlen;
}

ਸਥਿਰ size_t chunk_len(ਅਚਲ ਬਣਤਰ buf_chunk *chunk)
{
  ਵਾਪਸ chunk->w_ਵਿੱਥ - chunk->r_ਵਿੱਥ;
}

ਸਥਿਰ ਖਾਲੀ chunk_reset(ਬਣਤਰ buf_chunk *chunk)
{
  chunk->ਅਗਲਾ = ਨਿਰਾ;
  chunk->r_ਵਿੱਥ = chunk->w_ਵਿੱਥ = 0;
}

ਸਥਿਰ size_t chunk_append(ਬਣਤਰ buf_chunk *chunk,
                           ਅਚਲ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ਅੰਕੜੇ[chunk->w_ਵਿੱਥ];
  size_t n = chunk->dlen - chunk->w_ਵਿੱਥ;
  DEBUGASSERT(chunk->dlen >= chunk->w_ਵਿੱਥ);
  ਜੇ(n) {
    n = CURLMIN(n, len);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, buf, n);
    chunk->w_ਵਿੱਥ += n;
  }
  ਵਾਪਸ n;
}

ਸਥਿਰ size_t chunk_read(ਬਣਤਰ buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ਅੰਕੜੇ[chunk->r_ਵਿੱਥ];
  size_t n = chunk->w_ਵਿੱਥ - chunk->r_ਵਿੱਥ;
  DEBUGASSERT(chunk->w_ਵਿੱਥ >= chunk->r_ਵਿੱਥ);
  ਜੇ(!n) {
    ਵਾਪਸ 0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(n <= len) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, p, n);
    chunk->r_ਵਿੱਥ = chunk->w_ਵਿੱਥ = 0;
    ਵਾਪਸ n;
  }
  ਨਹੀਂਤਾਂ {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, p, len);
    chunk->r_ਵਿੱਥ += len;
    ਵਾਪਸ len;
  }
}

ਸਥਿਰ CURLcode chunk_slurpn(ਬਣਤਰ buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             ਖਾਲੀ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.ਅੰਕੜੇ[chunk->w_ਵਿੱਥ];
  size_t n = chunk->dlen - chunk->w_ਵਿੱਥ; /* free amount */
  CURLcode ਨਤੀਜਾ;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_ਵਿੱਥ);
  ਜੇ(!n)
    ਵਾਪਸ CURLE_AGAIN;
  ਜੇ(max_len && n > max_len)
    n = max_len;
  ਨਤੀਜਾ = reader(reader_ctx, p, n, pnread);
  ਜੇ(!ਨਤੀਜਾ) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_ਵਿੱਥ += *pnread;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ chunk_peek(ਅਚਲ ਬਣਤਰ buf_chunk *chunk,
                       ਅਚਲ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_ਵਿੱਥ >= chunk->r_ਵਿੱਥ);
  *pbuf = &chunk->x.ਅੰਕੜੇ[chunk->r_ਵਿੱਥ];
  *plen = chunk->w_ਵਿੱਥ - chunk->r_ਵਿੱਥ;
}

ਸਥਿਰ ਖਾਲੀ chunk_peek_at(ਅਚਲ ਬਣਤਰ buf_chunk *chunk, size_t ਵਿੱਥ,
                          ਅਚਲ uint8_t **pbuf, size_t *plen)
{
  ਵਿੱਥ += chunk->r_ਵਿੱਥ;
  DEBUGASSERT(chunk->w_ਵਿੱਥ >= ਵਿੱਥ);
  *pbuf = &chunk->x.ਅੰਕੜੇ[ਵਿੱਥ];
  *plen = chunk->w_ਵਿੱਥ - ਵਿੱਥ;
}

ਸਥਿਰ size_t chunk_skip(ਬਣਤਰ buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_ਵਿੱਥ - chunk->r_ਵਿੱਥ;
  DEBUGASSERT(chunk->w_ਵਿੱਥ >= chunk->r_ਵਿੱਥ);
  ਜੇ(n) {
    n = CURLMIN(n, amount);
    chunk->r_ਵਿੱਥ += n;
    ਜੇ(chunk->r_ਵਿੱਥ == chunk->w_ਵਿੱਥ)
      chunk->r_ਵਿੱਥ = chunk->w_ਵਿੱਥ = 0;
  }
  ਵਾਪਸ n;
}

ਸਥਿਰ ਖਾਲੀ chunk_list_free(ਬਣਤਰ buf_chunk **anchor)
{
  ਬਣਤਰ buf_chunk *chunk;
  ਜਦੋਂ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->ਅਗਲਾ;
    curlx_free(chunk);
  }
}

ਖਾਲੀ Curl_bufcp_init(ਬਣਤਰ bufc_pool *pool,
                     size_t chunk_ਮਾਪ, size_t ਵਾਧੂ_max)
{
  DEBUGASSERT(chunk_ਮਾਪ > 0);
  DEBUGASSERT(ਵਾਧੂ_max > 0);
  memset(pool, 0, ਆਕਾਰ(*pool));
  pool->chunk_ਮਾਪ = chunk_ਮਾਪ;
  pool->ਵਾਧੂ_max = ਵਾਧੂ_max;
}

ਸਥਿਰ CURLcode bufcp_take(ਬਣਤਰ bufc_pool *pool,
                           ਬਣਤਰ buf_chunk **pchunk)
{
  ਬਣਤਰ buf_chunk *chunk = ਨਿਰਾ;

  ਜੇ(pool->ਵਾਧੂ) {
    chunk = pool->ਵਾਧੂ;
    pool->ਵਾਧੂ = chunk->ਅਗਲਾ;
    --pool->ਵਾਧੂ_ਗਿਣਤੀ;
    chunk_reset(chunk);
    *pchunk = chunk;
    ਵਾਪਸ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ਜੇ(pool->chunk_ਮਾਪ > SIZE_MAX - ਆਕਾਰ(*chunk)) {
    *pchunk = ਨਿਰਾ;
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  chunk = curlx_calloc(1, ਆਕਾਰ(*chunk) + pool->chunk_ਮਾਪ);
  ਜੇ(!chunk) {
    *pchunk = ਨਿਰਾ;
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  chunk->dlen = pool->chunk_ਮਾਪ;
  *pchunk = chunk;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ bufcp_put(ਬਣਤਰ bufc_pool *pool,
                      ਬਣਤਰ buf_chunk *chunk)
{
  ਜੇ(pool->ਵਾਧੂ_ਗਿਣਤੀ >= pool->ਵਾਧੂ_max) {
    curlx_free(chunk);
  }
  ਨਹੀਂਤਾਂ {
    chunk_reset(chunk);
    chunk->ਅਗਲਾ = pool->ਵਾਧੂ;
    pool->ਵਾਧੂ = chunk;
    ++pool->ਵਾਧੂ_ਗਿਣਤੀ;
  }
}

ਖਾਲੀ Curl_bufcp_free(ਬਣਤਰ bufc_pool *pool)
{
  chunk_list_free(&pool->ਵਾਧੂ);
  pool->ਵਾਧੂ_ਗਿਣਤੀ = 0;
}

ਸਥਿਰ ਖਾਲੀ bufq_init(ਬਣਤਰ bufq *q, ਬਣਤਰ bufc_pool *pool,
                      size_t chunk_ਮਾਪ, size_t max_chunks, ਪੂਰਨ opts)
{
  DEBUGASSERT(chunk_ਮਾਪ > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, ਆਕਾਰ(*q));
  q->chunk_ਮਾਪ = chunk_ਮਾਪ;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

ਖਾਲੀ Curl_bufq_init2(ਬਣਤਰ bufq *q, size_t chunk_ਮਾਪ, size_t max_chunks,
                     ਪੂਰਨ opts)
{
  bufq_init(q, ਨਿਰਾ, chunk_ਮਾਪ, max_chunks, opts);
}

ਖਾਲੀ Curl_bufq_init(ਬਣਤਰ bufq *q, size_t chunk_ਮਾਪ, size_t max_chunks)
{
  bufq_init(q, ਨਿਰਾ, chunk_ਮਾਪ, max_chunks, BUFQ_OPT_NONE);
}

ਖਾਲੀ Curl_bufq_initp(ਬਣਤਰ bufq *q, ਬਣਤਰ bufc_pool *pool,
                     size_t max_chunks, ਪੂਰਨ opts)
{
  bufq_init(q, pool, pool->chunk_ਮਾਪ, max_chunks, opts);
}

ਖਾਲੀ Curl_bufq_free(ਬਣਤਰ bufq *q)
{
  chunk_list_free(&q->ਸਿਰ);
  chunk_list_free(&q->ਵਾਧੂ);
  q->ਪੂਛ = ਨਿਰਾ;
  q->chunk_ਗਿਣਤੀ = 0;
}

ਖਾਲੀ Curl_bufq_reset(ਬਣਤਰ bufq *q)
{
  ਬਣਤਰ buf_chunk *chunk;
  ਜਦੋਂ(q->ਸਿਰ) {
    chunk = q->ਸਿਰ;
    q->ਸਿਰ = chunk->ਅਗਲਾ;
    chunk->ਅਗਲਾ = q->ਵਾਧੂ;
    q->ਵਾਧੂ = chunk;
  }
  q->ਪੂਛ = ਨਿਰਾ;
}

size_t Curl_bufq_len(ਅਚਲ ਬਣਤਰ bufq *q)
{
  ਅਚਲ ਬਣਤਰ buf_chunk *chunk = q->ਸਿਰ;
  size_t len = 0;
  ਜਦੋਂ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->ਅਗਲਾ;
  }
  ਵਾਪਸ len;
}

bool Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਬਣਤਰ bufq *q)
{
  ਵਾਪਸ !q->ਸਿਰ || chunk_is_ਖਾਲੀ_ਥਾਵਾਂ(q->ਸਿਰ);
}

bool Curl_bufq_is_ਭਰਿਆ(ਅਚਲ ਬਣਤਰ bufq *q)
{
  ਜੇ(!q->ਪੂਛ || q->ਵਾਧੂ)
    ਵਾਪਸ FALSE;
  ਜੇ(q->chunk_ਗਿਣਤੀ < q->max_chunks)
    ਵਾਪਸ FALSE;
  ਜੇ(q->chunk_ਗਿਣਤੀ > q->max_chunks)
    ਵਾਪਸ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ਵਾਪਸ chunk_is_ਭਰਿਆ(q->ਪੂਛ);
}

ਸਥਿਰ ਬਣਤਰ buf_chunk *get_ਵਾਧੂ(ਬਣਤਰ bufq *q)
{
  ਬਣਤਰ buf_chunk *chunk = ਨਿਰਾ;

  ਜੇ(q->ਵਾਧੂ) {
    chunk = q->ਵਾਧੂ;
    q->ਵਾਧੂ = chunk->ਅਗਲਾ;
    chunk_reset(chunk);
    ਵਾਪਸ chunk;
  }

  ਜੇ(q->chunk_ਗਿਣਤੀ >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ)))
    ਵਾਪਸ ਨਿਰਾ;

  ਜੇ(q->pool) {
    ਜੇ(bufcp_take(q->pool, &chunk))
      ਵਾਪਸ ਨਿਰਾ;
    ++q->chunk_ਗਿਣਤੀ;
    ਵਾਪਸ chunk;
  }
  ਨਹੀਂਤਾਂ {
    /* Check for integer overflow before allocation */
    ਜੇ(q->chunk_ਮਾਪ > SIZE_MAX - ਆਕਾਰ(*chunk)) {
      ਵਾਪਸ ਨਿਰਾ;
    }

    chunk = curlx_calloc(1, ਆਕਾਰ(*chunk) + q->chunk_ਮਾਪ);
    ਜੇ(!chunk)
      ਵਾਪਸ ਨਿਰਾ;
    chunk->dlen = q->chunk_ਮਾਪ;
    ++q->chunk_ਗਿਣਤੀ;
    ਵਾਪਸ chunk;
  }
}

ਸਥਿਰ ਖਾਲੀ prune_ਸਿਰ(ਬਣਤਰ bufq *q)
{
  ਬਣਤਰ buf_chunk *chunk;

  ਜਦੋਂ(q->ਸਿਰ && chunk_is_ਖਾਲੀ_ਥਾਵਾਂ(q->ਸਿਰ)) {
    chunk = q->ਸਿਰ;
    q->ਸਿਰ = chunk->ਅਗਲਾ;
    ਜੇ(q->ਪੂਛ == chunk)
      q->ਪੂਛ = q->ਸਿਰ;
    ਜੇ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ((q->chunk_ਗਿਣਤੀ > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ {
      chunk->ਅਗਲਾ = q->ਵਾਧੂ;
      q->ਵਾਧੂ = chunk;
    }
  }
}

ਸਥਿਰ ਬਣਤਰ buf_chunk *get_non_ਭਰਿਆ_ਪੂਛ(ਬਣਤਰ bufq *q)
{
  ਬਣਤਰ buf_chunk *chunk;

  ਜੇ(q->ਪੂਛ && !chunk_is_ਭਰਿਆ(q->ਪੂਛ))
    ਵਾਪਸ q->ਪੂਛ;
  chunk = get_ਵਾਧੂ(q);
  ਜੇ(chunk) {
    /* new tail, and possibly new head */
    ਜੇ(q->ਪੂਛ) {
      q->ਪੂਛ->ਅਗਲਾ = chunk;
      q->ਪੂਛ = chunk;
    }
    ਨਹੀਂਤਾਂ {
      DEBUGASSERT(!q->ਸਿਰ);
      q->ਸਿਰ = q->ਪੂਛ = chunk;
    }
  }
  ਵਾਪਸ chunk;
}

CURLcode Curl_bufq_write(ਬਣਤਰ bufq *q,
                         ਅਚਲ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ਬਣਤਰ buf_chunk *ਪੂਛ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ਜਦੋਂ(len) {
    ਪੂਛ = get_non_ਭਰਿਆ_ਪੂਛ(q);
    ਜੇ(!ਪੂਛ) {
      ਜੇ((q->chunk_ਗਿਣਤੀ < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ))
        /* should have gotten a tail, but did not */
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;
    }
    n = chunk_append(ਪੂਛ, buf, len);
    ਜੇ(!n)
      ਰੋਕੋ;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ਵਾਪਸ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ਬਣਤਰ bufq *q,
                          ਅਚਲ ਅੱਖਰ *buf, size_t len,
                          size_t *pnwritten)
{
  ਵਾਪਸ Curl_bufq_write(q, (ਅਚਲ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ਬਣਤਰ bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ਜਦੋਂ(len && q->ਸਿਰ) {
    size_t n = chunk_read(q->ਸਿਰ, buf, len);
    ਜੇ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_ਸਿਰ(q);
  }
  ਵਾਪਸ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ਬਣਤਰ bufq *q, ਅੱਖਰ *buf, size_t len,
                         size_t *pnread)
{
  ਵਾਪਸ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ਬਣਤਰ bufq *q,
                    ਅਚਲ uint8_t **pbuf, size_t *plen)
{
  ਜੇ(q->ਸਿਰ && chunk_is_ਖਾਲੀ_ਥਾਵਾਂ(q->ਸਿਰ)) {
    prune_ਸਿਰ(q);
  }
  ਜੇ(q->ਸਿਰ && !chunk_is_ਖਾਲੀ_ਥਾਵਾਂ(q->ਸਿਰ)) {
    chunk_peek(q->ਸਿਰ, pbuf, plen);
    ਵਾਪਸ TRUE;
  }
  *pbuf = ਨਿਰਾ;
  *plen = 0;
  ਵਾਪਸ FALSE;
}

bool Curl_bufq_peek_at(ਬਣਤਰ bufq *q, size_t ਵਿੱਥ,
                       ਅਚਲ uint8_t **pbuf, size_t *plen)
{
  ਬਣਤਰ buf_chunk *c = q->ਸਿਰ;
  size_t clen;

  ਜਦੋਂ(c) {
    clen = chunk_len(c);
    ਜੇ(!clen)
      ਰੋਕੋ;
    ਜੇ(ਵਿੱਥ >= clen) {
      ਵਿੱਥ -= clen;
      c = c->ਅਗਲਾ;
      ਜਾਰੀ;
    }
    chunk_peek_at(c, ਵਿੱਥ, pbuf, plen);
    ਵਾਪਸ TRUE;
  }
  *pbuf = ਨਿਰਾ;
  *plen = 0;
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_bufq_skip(ਬਣਤਰ bufq *q, size_t amount)
{
  size_t n;

  ਜਦੋਂ(amount && q->ਸਿਰ) {
    n = chunk_skip(q->ਸਿਰ, amount);
    amount -= n;
    prune_ਸਿਰ(q);
  }
}

CURLcode Curl_bufq_pass(ਬਣਤਰ bufq *q, Curl_bufq_writer *writer,
                        ਖਾਲੀ *writer_ctx, size_t *pwritten)
{
  ਅਚਲ uint8_t *buf;
  size_t blen;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pwritten = 0;
  ਜਦੋਂ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    ਨਤੀਜਾ = writer(writer_ctx, buf, blen, &chunk_written);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ((ਨਤੀਜਾ == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        ਨਤੀਜਾ = CURLE_OK;
      }
      ਰੋਕੋ;
    }
    ਜੇ(!chunk_written) {
      ਜੇ(!*pwritten) {
        /* treat as blocked */
        ਨਤੀਜਾ = CURLE_AGAIN;
      }
      ਰੋਕੋ;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_bufq_write_pass(ਬਣਤਰ bufq *q,
                              ਅਚਲ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ਖਾਲੀ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ਜਦੋਂ(len) {
    ਜੇ(Curl_bufq_is_ਭਰਿਆ(q)) {
      /* try to make room in case we are full */
      ਨਤੀਜਾ = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ਜੇ(ਨਤੀਜਾ) {
        ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
          /* real error, fail */
          ਵਾਪਸ ਨਤੀਜਾ;
        }
        /* would block, bufq is full, give up */
        ਰੋਕੋ;
      }
    }

    /* Add to bufq as much as there is room for */
    ਨਤੀਜਾ = Curl_bufq_write(q, buf, len, &n);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
        /* real error, fail */
        ਵਾਪਸ ਨਤੀਜਾ;
      /* result == CURLE_AGAIN */
      ਜੇ(*pwritten)
        /* we did write successfully before */
        ਨਤੀਜਾ = CURLE_OK;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ਰੋਕੋ;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ਵਾਪਸ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ਬਣਤਰ bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ਖਾਲੀ *reader_ctx,
                        size_t *pnread)
{
  ਬਣਤਰ buf_chunk *ਪੂਛ = ਨਿਰਾ;

  *pnread = 0;
  ਪੂਛ = get_non_ਭਰਿਆ_ਪੂਛ(q);
  ਜੇ(!ਪੂਛ) {
    ਜੇ(q->chunk_ਗਿਣਤੀ < q->max_chunks)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    /* full, blocked */
    ਵਾਪਸ CURLE_AGAIN;
  }

  ਵਾਪਸ chunk_slurpn(ਪੂਛ, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ਸਥਿਰ CURLcode bufq_slurpn(ਬਣਤਰ bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, ਖਾਲੀ *reader_ctx,
                            size_t *pnread)
{
  CURLcode ਨਤੀਜਾ;

  *pnread = 0;
  ਜਦੋਂ(1) {
    size_t n;
    ਨਤੀਜਾ = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(!*pnread || ਨਤੀਜਾ != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਤੀਜਾ = CURLE_OK;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(n == 0) {
      /* eof, result remains CURLE_OK */
      ਰੋਕੋ;
    }
    *pnread += n;
    ਜੇ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ਜੇ(!max_len)
        ਰੋਕੋ;
    }
    /* give up slurping when we get less bytes than we asked for */
    ਜੇ(q->ਪੂਛ && !chunk_is_ਭਰਿਆ(q->ਪੂਛ))
      ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_bufq_slurp(ਬਣਤਰ bufq *q, Curl_bufq_reader *reader,
                         ਖਾਲੀ *reader_ctx, size_t *pnread)
{
  ਵਾਪਸ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
