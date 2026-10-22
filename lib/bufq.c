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

ಸ್ಥಿರ bool chunk_is_empty(ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk)
{
  ಪ್ರತ್ಯಾಗಮ chunk->r_offset >= chunk->w_offset;
}

ಸ್ಥಿರ bool chunk_is_full(ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk)
{
  ಪ್ರತ್ಯಾಗಮ chunk->w_offset >= chunk->dlen;
}

ಸ್ಥಿರ size_t chunk_len(ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk)
{
  ಪ್ರತ್ಯಾಗಮ chunk->w_offset - chunk->r_offset;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_reset(ಸಂರಚನಾ buf_chunk *chunk)
{
  chunk->next = ರಿಕ್ತ;
  chunk->r_offset = chunk->w_offset = 0;
}

ಸ್ಥಿರ size_t chunk_append(ಸಂರಚನಾ buf_chunk *chunk,
                           ಧ್ರುವ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ಯದಿ(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ size_t chunk_read(ಸಂರಚನಾ buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ಯದಿ(!n) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಅನ್ಯಥಾ ಯದಿ(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    ಪ್ರತ್ಯಾಗಮ n;
  }
  ಅನ್ಯಥಾ {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    ಪ್ರತ್ಯಾಗಮ len;
  }
}

ಸ್ಥಿರ CURLcode chunk_slurpn(ಸಂರಚನಾ buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             ಶೂನ್ಯ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ಯದಿ(!n)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಯದಿ(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  ಯದಿ(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_peek(ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk,
                       ಧ್ರುವ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_peek_at(ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk, size_t offset,
                          ಧ್ರುವ uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

ಸ್ಥಿರ size_t chunk_skip(ಸಂರಚನಾ buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ಯದಿ(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    ಯದಿ(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ ಶೂನ್ಯ chunk_list_free(ಸಂರಚನಾ buf_chunk **anchor)
{
  ಸಂರಚನಾ buf_chunk *chunk;
  ಯಾವತ್(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

ಶೂನ್ಯ Curl_bufcp_init(ಸಂರಚನಾ bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, ಪರಿಮಾಣ(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

ಸ್ಥಿರ CURLcode bufcp_take(ಸಂರಚನಾ bufc_pool *pool,
                           ಸಂರಚನಾ buf_chunk **pchunk)
{
  ಸಂರಚನಾ buf_chunk *chunk = ರಿಕ್ತ;

  ಯದಿ(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_ಎಣಿಕೆ;
    chunk_reset(chunk);
    *pchunk = chunk;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ಯದಿ(pool->chunk_size > SIZE_MAX - ಪರಿಮಾಣ(*chunk)) {
    *pchunk = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, ಪರಿಮಾಣ(*chunk) + pool->chunk_size);
  ಯದಿ(!chunk) {
    *pchunk = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ bufcp_put(ಸಂರಚನಾ bufc_pool *pool,
                      ಸಂರಚನಾ buf_chunk *chunk)
{
  ಯದಿ(pool->spare_ಎಣಿಕೆ >= pool->spare_max) {
    curlx_free(chunk);
  }
  ಅನ್ಯಥಾ {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_ಎಣಿಕೆ;
  }
}

ಶೂನ್ಯ Curl_bufcp_free(ಸಂರಚನಾ bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_ಎಣಿಕೆ = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ bufq_init(ಸಂರಚನಾ bufq *q, ಸಂರಚನಾ bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, ಪೂರ್ಣಾಙ್ಕ opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, ಪರಿಮಾಣ(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

ಶೂನ್ಯ Curl_bufq_init2(ಸಂರಚನಾ bufq *q, size_t chunk_size, size_t max_chunks,
                     ಪೂರ್ಣಾಙ್ಕ opts)
{
  bufq_init(q, ರಿಕ್ತ, chunk_size, max_chunks, opts);
}

ಶೂನ್ಯ Curl_bufq_init(ಸಂರಚನಾ bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, ರಿಕ್ತ, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

ಶೂನ್ಯ Curl_bufq_initp(ಸಂರಚನಾ bufq *q, ಸಂರಚನಾ bufc_pool *pool,
                     size_t max_chunks, ಪೂರ್ಣಾಙ್ಕ opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

ಶೂನ್ಯ Curl_bufq_free(ಸಂರಚನಾ bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = ರಿಕ್ತ;
  q->chunk_ಎಣಿಕೆ = 0;
}

ಶೂನ್ಯ Curl_bufq_reset(ಸಂರಚನಾ bufq *q)
{
  ಸಂರಚನಾ buf_chunk *chunk;
  ಯಾವತ್(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = ರಿಕ್ತ;
}

size_t Curl_bufq_len(ಧ್ರುವ ಸಂರಚನಾ bufq *q)
{
  ಧ್ರುವ ಸಂರಚನಾ buf_chunk *chunk = q->head;
  size_t len = 0;
  ಯಾವತ್(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  ಪ್ರತ್ಯಾಗಮ len;
}

bool Curl_bufq_is_empty(ಧ್ರುವ ಸಂರಚನಾ bufq *q)
{
  ಪ್ರತ್ಯಾಗಮ !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ಧ್ರುವ ಸಂರಚನಾ bufq *q)
{
  ಯದಿ(!q->tail || q->spare)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(q->chunk_ಎಣಿಕೆ < q->max_chunks)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(q->chunk_ಎಣಿಕೆ > q->max_chunks)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ಪ್ರತ್ಯಾಗಮ chunk_is_full(q->tail);
}

ಸ್ಥಿರ ಸಂರಚನಾ buf_chunk *get_spare(ಸಂರಚನಾ bufq *q)
{
  ಸಂರಚನಾ buf_chunk *chunk = ರಿಕ್ತ;

  ಯದಿ(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    ಪ್ರತ್ಯಾಗಮ chunk;
  }

  ಯದಿ(q->chunk_ಎಣಿಕೆ >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಯದಿ(q->pool) {
    ಯದಿ(bufcp_take(q->pool, &chunk))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ++q->chunk_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ chunk;
  }
  ಅನ್ಯಥಾ {
    /* Check for integer overflow before allocation */
    ಯದಿ(q->chunk_size > SIZE_MAX - ಪರಿಮಾಣ(*chunk)) {
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    chunk = curlx_calloc(1, ಪರಿಮಾಣ(*chunk) + q->chunk_size);
    ಯದಿ(!chunk)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    chunk->dlen = q->chunk_size;
    ++q->chunk_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ chunk;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ prune_head(ಸಂರಚನಾ bufq *q)
{
  ಸಂರಚನಾ buf_chunk *chunk;

  ಯಾವತ್(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    ಯದಿ(q->tail == chunk)
      q->tail = q->head;
    ಯದಿ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ ಯದಿ((q->chunk_ಎಣಿಕೆ > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

ಸ್ಥಿರ ಸಂರಚನಾ buf_chunk *get_non_full_tail(ಸಂರಚನಾ bufq *q)
{
  ಸಂರಚನಾ buf_chunk *chunk;

  ಯದಿ(q->tail && !chunk_is_full(q->tail))
    ಪ್ರತ್ಯಾಗಮ q->tail;
  chunk = get_spare(q);
  ಯದಿ(chunk) {
    /* new tail, and possibly new head */
    ಯದಿ(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    ಅನ್ಯಥಾ {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  ಪ್ರತ್ಯಾಗಮ chunk;
}

CURLcode Curl_bufq_write(ಸಂರಚನಾ bufq *q,
                         ಧ್ರುವ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ಸಂರಚನಾ buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ಯಾವತ್(len) {
    tail = get_non_full_tail(q);
    ಯದಿ(!tail) {
      ಯದಿ((q->chunk_ಎಣಿಕೆ < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಭಙ್ಗ;
    }
    n = chunk_append(tail, buf, len);
    ಯದಿ(!n)
      ಭಙ್ಗ;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ಪ್ರತ್ಯಾಗಮ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ಸಂರಚನಾ bufq *q,
                          ಧ್ರುವ ವರ್ಣ *buf, size_t len,
                          size_t *pnwritten)
{
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_write(q, (ಧ್ರುವ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ಸಂರಚನಾ bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ಯಾವತ್(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    ಯದಿ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  ಪ್ರತ್ಯಾಗಮ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ಸಂರಚನಾ bufq *q, ವರ್ಣ *buf, size_t len,
                         size_t *pnread)
{
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ಸಂರಚನಾ bufq *q,
                    ಧ್ರುವ uint8_t **pbuf, size_t *plen)
{
  ಯದಿ(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  ಯದಿ(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  *pbuf = ರಿಕ್ತ;
  *plen = 0;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_bufq_peek_at(ಸಂರಚನಾ bufq *q, size_t offset,
                       ಧ್ರುವ uint8_t **pbuf, size_t *plen)
{
  ಸಂರಚನಾ buf_chunk *c = q->head;
  size_t clen;

  ಯಾವತ್(c) {
    clen = chunk_len(c);
    ಯದಿ(!clen)
      ಭಙ್ಗ;
    ಯದಿ(offset >= clen) {
      offset -= clen;
      c = c->next;
      ಅನುವರ್ತ;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  *pbuf = ರಿಕ್ತ;
  *plen = 0;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_bufq_skip(ಸಂರಚನಾ bufq *q, size_t amount)
{
  size_t n;

  ಯಾವತ್(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(ಸಂರಚನಾ bufq *q, Curl_bufq_writer *writer,
                        ಶೂನ್ಯ *writer_ctx, size_t *pwritten)
{
  ಧ್ರುವ uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  ಯಾವತ್(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    ಯದಿ(result) {
      ಯದಿ((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      ಭಙ್ಗ;
    }
    ಯದಿ(!chunk_written) {
      ಯದಿ(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      ಭಙ್ಗ;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_bufq_write_pass(ಸಂರಚನಾ bufq *q,
                              ಧ್ರುವ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ಶೂನ್ಯ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ಯಾವತ್(len) {
    ಯದಿ(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ಯದಿ(result) {
        ಯದಿ(result != CURLE_AGAIN) {
          /* real error, fail */
          ಪ್ರತ್ಯಾಗಮ result;
        }
        /* would block, bufq is full, give up */
        ಭಙ್ಗ;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_AGAIN)
        /* real error, fail */
        ಪ್ರತ್ಯಾಗಮ result;
      /* result == CURLE_AGAIN */
      ಯದಿ(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ ಯದಿ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ಭಙ್ಗ;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ಪ್ರತ್ಯಾಗಮ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ಸಂರಚನಾ bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ಶೂನ್ಯ *reader_ctx,
                        size_t *pnread)
{
  ಸಂರಚನಾ buf_chunk *tail = ರಿಕ್ತ;

  *pnread = 0;
  tail = get_non_full_tail(q);
  ಯದಿ(!tail) {
    ಯದಿ(q->chunk_ಎಣಿಕೆ < q->max_chunks)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }

  ಪ್ರತ್ಯಾಗಮ chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ಸ್ಥಿರ CURLcode bufq_slurpn(ಸಂರಚನಾ bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, ಶೂನ್ಯ *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ಯಾವತ್(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ಯದಿ(result) {
      ಯದಿ(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ಪ್ರತ್ಯಾಗಮ result;
      }
      result = CURLE_OK;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(n == 0) {
      /* eof, result remains CURLE_OK */
      ಭಙ್ಗ;
    }
    *pnread += n;
    ಯದಿ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ಯದಿ(!max_len)
        ಭಙ್ಗ;
    }
    /* give up slurping when we get less bytes than we asked for */
    ಯದಿ(q->tail && !chunk_is_full(q->tail))
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_bufq_slurp(ಸಂರಚನಾ bufq *q, Curl_bufq_reader *reader,
                         ಶೂನ್ಯ *reader_ctx, size_t *pnread)
{
  ಪ್ರತ್ಯಾಗಮ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
