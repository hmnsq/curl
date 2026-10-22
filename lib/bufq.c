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

#include "bufq.h"

static bool chunk_is_empty(const struct buf_chunk *chunk)
{
  반환 chunk->r_offset >= chunk->w_offset;
}

static bool chunk_is_full(const struct buf_chunk *chunk)
{
  반환 chunk->w_offset >= chunk->dlen;
}

static size_t chunk_len(const struct buf_chunk *chunk)
{
  반환 chunk->w_offset - chunk->r_offset;
}

static void chunk_reset(struct buf_chunk *chunk)
{
  chunk->next = NULL;
  chunk->r_offset = chunk->w_offset = 0;
}

static size_t chunk_append(struct buf_chunk *chunk,
                           const uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  만약(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  반환 n;
}

static size_t chunk_read(struct buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  만약(!n) {
    반환 0;
  }
  아니면 만약(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    반환 n;
  }
  아니면 {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    반환 len;
  }
}

static CURLcode chunk_slurpn(struct buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             void *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  만약(!n)
    반환 CURLE_AGAIN;
  만약(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  만약(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  반환 result;
}

static void chunk_peek(const struct buf_chunk *chunk,
                       const uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

static void chunk_peek_at(const struct buf_chunk *chunk, size_t offset,
                          const uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

static size_t chunk_skip(struct buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  만약(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    만약(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  반환 n;
}

static void chunk_list_free(struct buf_chunk **anchor)
{
  struct buf_chunk *chunk;
  동안(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

void Curl_bufcp_init(struct bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, sizeof(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

static CURLcode bufcp_take(struct bufc_pool *pool,
                           struct buf_chunk **pchunk)
{
  struct buf_chunk *chunk = NULL;

  만약(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_count;
    chunk_reset(chunk);
    *pchunk = chunk;
    반환 CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  만약(pool->chunk_size > SIZE_MAX - sizeof(*chunk)) {
    *pchunk = NULL;
    반환 CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, sizeof(*chunk) + pool->chunk_size);
  만약(!chunk) {
    *pchunk = NULL;
    반환 CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  반환 CURLE_OK;
}

static void bufcp_put(struct bufc_pool *pool,
                      struct buf_chunk *chunk)
{
  만약(pool->spare_count >= pool->spare_max) {
    curlx_free(chunk);
  }
  아니면 {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_count;
  }
}

void Curl_bufcp_free(struct bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_count = 0;
}

static void bufq_init(struct bufq *q, struct bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, 정수 opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, sizeof(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

void Curl_bufq_init2(struct bufq *q, size_t chunk_size, size_t max_chunks,
                     정수 opts)
{
  bufq_init(q, NULL, chunk_size, max_chunks, opts);
}

void Curl_bufq_init(struct bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, NULL, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

void Curl_bufq_initp(struct bufq *q, struct bufc_pool *pool,
                     size_t max_chunks, 정수 opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

void Curl_bufq_free(struct bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = NULL;
  q->chunk_count = 0;
}

void Curl_bufq_reset(struct bufq *q)
{
  struct buf_chunk *chunk;
  동안(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = NULL;
}

size_t Curl_bufq_len(const struct bufq *q)
{
  const struct buf_chunk *chunk = q->head;
  size_t len = 0;
  동안(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  반환 len;
}

bool Curl_bufq_is_empty(const struct bufq *q)
{
  반환 !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(const struct bufq *q)
{
  만약(!q->tail || q->spare)
    반환 FALSE;
  만약(q->chunk_count < q->max_chunks)
    반환 FALSE;
  만약(q->chunk_count > q->max_chunks)
    반환 TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  반환 chunk_is_full(q->tail);
}

static struct buf_chunk *get_spare(struct bufq *q)
{
  struct buf_chunk *chunk = NULL;

  만약(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    반환 chunk;
  }

  만약(q->chunk_count >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    반환 NULL;

  만약(q->pool) {
    만약(bufcp_take(q->pool, &chunk))
      반환 NULL;
    ++q->chunk_count;
    반환 chunk;
  }
  아니면 {
    /* Check for integer overflow before allocation */
    만약(q->chunk_size > SIZE_MAX - sizeof(*chunk)) {
      반환 NULL;
    }

    chunk = curlx_calloc(1, sizeof(*chunk) + q->chunk_size);
    만약(!chunk)
      반환 NULL;
    chunk->dlen = q->chunk_size;
    ++q->chunk_count;
    반환 chunk;
  }
}

static void prune_head(struct bufq *q)
{
  struct buf_chunk *chunk;

  동안(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    만약(q->tail == chunk)
      q->tail = q->head;
    만약(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_count;
    }
    아니면 만약((q->chunk_count > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_count;
    }
    아니면 {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

static struct buf_chunk *get_non_full_tail(struct bufq *q)
{
  struct buf_chunk *chunk;

  만약(q->tail && !chunk_is_full(q->tail))
    반환 q->tail;
  chunk = get_spare(q);
  만약(chunk) {
    /* new tail, and possibly new head */
    만약(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    아니면 {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  반환 chunk;
}

CURLcode Curl_bufq_write(struct bufq *q,
                         const uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  struct buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  동안(len) {
    tail = get_non_full_tail(q);
    만약(!tail) {
      만약((q->chunk_count < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        반환 CURLE_OUT_OF_MEMORY;
      break;
    }
    n = chunk_append(tail, buf, len);
    만약(!n)
      break;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  반환 (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(struct bufq *q,
                          const char *buf, size_t len,
                          size_t *pnwritten)
{
  반환 Curl_bufq_write(q, (const uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(struct bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  동안(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    만약(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  반환 (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(struct bufq *q, char *buf, size_t len,
                         size_t *pnread)
{
  반환 Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(struct bufq *q,
                    const uint8_t **pbuf, size_t *plen)
{
  만약(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  만약(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    반환 TRUE;
  }
  *pbuf = NULL;
  *plen = 0;
  반환 FALSE;
}

bool Curl_bufq_peek_at(struct bufq *q, size_t offset,
                       const uint8_t **pbuf, size_t *plen)
{
  struct buf_chunk *c = q->head;
  size_t clen;

  동안(c) {
    clen = chunk_len(c);
    만약(!clen)
      break;
    만약(offset >= clen) {
      offset -= clen;
      c = c->next;
      continue;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    반환 TRUE;
  }
  *pbuf = NULL;
  *plen = 0;
  반환 FALSE;
}

void Curl_bufq_skip(struct bufq *q, size_t amount)
{
  size_t n;

  동안(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(struct bufq *q, Curl_bufq_writer *writer,
                        void *writer_ctx, size_t *pwritten)
{
  const uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  동안(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    만약(result) {
      만약((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      break;
    }
    만약(!chunk_written) {
      만약(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      break;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  반환 result;
}

CURLcode Curl_bufq_write_pass(struct bufq *q,
                              const uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, void *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  동안(len) {
    만약(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      만약(result) {
        만약(result != CURLE_AGAIN) {
          /* real error, fail */
          반환 result;
        }
        /* would block, bufq is full, give up */
        break;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    만약(result) {
      만약(result != CURLE_AGAIN)
        /* real error, fail */
        반환 result;
      /* result == CURLE_AGAIN */
      만약(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      반환 result;
    }
    아니면 만약(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      break;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  반환 (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(struct bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, void *reader_ctx,
                        size_t *pnread)
{
  struct buf_chunk *tail = NULL;

  *pnread = 0;
  tail = get_non_full_tail(q);
  만약(!tail) {
    만약(q->chunk_count < q->max_chunks)
      반환 CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    반환 CURLE_AGAIN;
  }

  반환 chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
static CURLcode bufq_slurpn(struct bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, void *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  동안(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    만약(result) {
      만약(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        반환 result;
      }
      result = CURLE_OK;
      break;
    }
    아니면 만약(n == 0) {
      /* eof, result remains CURLE_OK */
      break;
    }
    *pnread += n;
    만약(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      만약(!max_len)
        break;
    }
    /* give up slurping when we get less bytes than we asked for */
    만약(q->tail && !chunk_is_full(q->tail))
      break;
  }
  반환 result;
}

CURLcode Curl_bufq_slurp(struct bufq *q, Curl_bufq_reader *reader,
                         void *reader_ctx, size_t *pnread)
{
  반환 bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
