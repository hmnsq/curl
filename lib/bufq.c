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

static bool chunk_is_empty(const sa_nracanaa buf_chunk *chunk)
{
  ᱨᱩᱣᱟᱹᱲ chunk->r_offset >= chunk->w_offset;
}

static bool chunk_is_full(const sa_nracanaa buf_chunk *chunk)
{
  ᱨᱩᱣᱟᱹᱲ chunk->w_offset >= chunk->dlen;
}

static size_t chunk_len(const sa_nracanaa buf_chunk *chunk)
{
  ᱨᱩᱣᱟᱹᱲ chunk->w_offset - chunk->r_offset;
}

static void chunk_reset(sa_nracanaa buf_chunk *chunk)
{
  chunk->next = NULL;
  chunk->r_offset = chunk->w_offset = 0;
}

static size_t chunk_append(sa_nracanaa buf_chunk *chunk,
                           const uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ᱡᱩᱫᱤ(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  ᱨᱩᱣᱟᱹᱲ n;
}

static size_t chunk_read(sa_nracanaa buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ᱡᱩᱫᱤ(!n) {
    ᱨᱩᱣᱟᱹᱲ 0;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    ᱨᱩᱣᱟᱹᱲ n;
  }
  ᱵᱟᱝ {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    ᱨᱩᱣᱟᱹᱲ len;
  }
}

static CURLcode chunk_slurpn(sa_nracanaa buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             void *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ᱡᱩᱫᱤ(!n)
    ᱨᱩᱣᱟᱹᱲ CURLE_AGAIN;
  ᱡᱩᱫᱤ(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  ᱡᱩᱫᱤ(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

static void chunk_peek(const sa_nracanaa buf_chunk *chunk,
                       const uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

static void chunk_peek_at(const sa_nracanaa buf_chunk *chunk, size_t offset,
                          const uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

static size_t chunk_skip(sa_nracanaa buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ᱡᱩᱫᱤ(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    ᱡᱩᱫᱤ(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  ᱨᱩᱣᱟᱹᱲ n;
}

static void chunk_list_free(sa_nracanaa buf_chunk **anchor)
{
  sa_nracanaa buf_chunk *chunk;
  ᱡᱚᱠᱷᱚᱱ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

void Curl_bufcp_init(sa_nracanaa bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, parimaa_na(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

static CURLcode bufcp_take(sa_nracanaa bufc_pool *pool,
                           sa_nracanaa buf_chunk **pchunk)
{
  sa_nracanaa buf_chunk *chunk = NULL;

  ᱡᱩᱫᱤ(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_count;
    chunk_reset(chunk);
    *pchunk = chunk;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ᱡᱩᱫᱤ(pool->chunk_size > SIZE_MAX - parimaa_na(*chunk)) {
    *pchunk = NULL;
    ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, parimaa_na(*chunk) + pool->chunk_size);
  ᱡᱩᱫᱤ(!chunk) {
    *pchunk = NULL;
    ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static void bufcp_put(sa_nracanaa bufc_pool *pool,
                      sa_nracanaa buf_chunk *chunk)
{
  ᱡᱩᱫᱤ(pool->spare_count >= pool->spare_max) {
    curlx_free(chunk);
  }
  ᱵᱟᱝ {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_count;
  }
}

void Curl_bufcp_free(sa_nracanaa bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_count = 0;
}

static void bufq_init(sa_nracanaa bufq *q, sa_nracanaa bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, puur_naa_nka opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, parimaa_na(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

void Curl_bufq_init2(sa_nracanaa bufq *q, size_t chunk_size, size_t max_chunks,
                     puur_naa_nka opts)
{
  bufq_init(q, NULL, chunk_size, max_chunks, opts);
}

void Curl_bufq_init(sa_nracanaa bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, NULL, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

void Curl_bufq_initp(sa_nracanaa bufq *q, sa_nracanaa bufc_pool *pool,
                     size_t max_chunks, puur_naa_nka opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

void Curl_bufq_free(sa_nracanaa bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = NULL;
  q->chunk_count = 0;
}

void Curl_bufq_reset(sa_nracanaa bufq *q)
{
  sa_nracanaa buf_chunk *chunk;
  ᱡᱚᱠᱷᱚᱱ(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = NULL;
}

size_t Curl_bufq_len(const sa_nracanaa bufq *q)
{
  const sa_nracanaa buf_chunk *chunk = q->head;
  size_t len = 0;
  ᱡᱚᱠᱷᱚᱱ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  ᱨᱩᱣᱟᱹᱲ len;
}

bool Curl_bufq_is_empty(const sa_nracanaa bufq *q)
{
  ᱨᱩᱣᱟᱹᱲ !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(const sa_nracanaa bufq *q)
{
  ᱡᱩᱫᱤ(!q->tail || q->spare)
    ᱨᱩᱣᱟᱹᱲ FALSE;
  ᱡᱩᱫᱤ(q->chunk_count < q->max_chunks)
    ᱨᱩᱣᱟᱹᱲ FALSE;
  ᱡᱩᱫᱤ(q->chunk_count > q->max_chunks)
    ᱨᱩᱣᱟᱹᱲ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ᱨᱩᱣᱟᱹᱲ chunk_is_full(q->tail);
}

static sa_nracanaa buf_chunk *get_spare(sa_nracanaa bufq *q)
{
  sa_nracanaa buf_chunk *chunk = NULL;

  ᱡᱩᱫᱤ(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    ᱨᱩᱣᱟᱹᱲ chunk;
  }

  ᱡᱩᱫᱤ(q->chunk_count >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    ᱨᱩᱣᱟᱹᱲ NULL;

  ᱡᱩᱫᱤ(q->pool) {
    ᱡᱩᱫᱤ(bufcp_take(q->pool, &chunk))
      ᱨᱩᱣᱟᱹᱲ NULL;
    ++q->chunk_count;
    ᱨᱩᱣᱟᱹᱲ chunk;
  }
  ᱵᱟᱝ {
    /* Check for integer overflow before allocation */
    ᱡᱩᱫᱤ(q->chunk_size > SIZE_MAX - parimaa_na(*chunk)) {
      ᱨᱩᱣᱟᱹᱲ NULL;
    }

    chunk = curlx_calloc(1, parimaa_na(*chunk) + q->chunk_size);
    ᱡᱩᱫᱤ(!chunk)
      ᱨᱩᱣᱟᱹᱲ NULL;
    chunk->dlen = q->chunk_size;
    ++q->chunk_count;
    ᱨᱩᱣᱟᱹᱲ chunk;
  }
}

static void prune_head(sa_nracanaa bufq *q)
{
  sa_nracanaa buf_chunk *chunk;

  ᱡᱚᱠᱷᱚᱱ(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    ᱡᱩᱫᱤ(q->tail == chunk)
      q->tail = q->head;
    ᱡᱩᱫᱤ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_count;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((q->chunk_count > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_count;
    }
    ᱵᱟᱝ {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

static sa_nracanaa buf_chunk *get_non_full_tail(sa_nracanaa bufq *q)
{
  sa_nracanaa buf_chunk *chunk;

  ᱡᱩᱫᱤ(q->tail && !chunk_is_full(q->tail))
    ᱨᱩᱣᱟᱹᱲ q->tail;
  chunk = get_spare(q);
  ᱡᱩᱫᱤ(chunk) {
    /* new tail, and possibly new head */
    ᱡᱩᱫᱤ(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    ᱵᱟᱝ {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  ᱨᱩᱣᱟᱹᱲ chunk;
}

CURLcode Curl_bufq_write(sa_nracanaa bufq *q,
                         const uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  sa_nracanaa buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ᱡᱚᱠᱷᱚᱱ(len) {
    tail = get_non_full_tail(q);
    ᱡᱩᱫᱤ(!tail) {
      ᱡᱩᱫᱤ((q->chunk_count < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
      bha_nga;
    }
    n = chunk_append(tail, buf, len);
    ᱡᱩᱫᱤ(!n)
      bha_nga;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ᱨᱩᱣᱟᱹᱲ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(sa_nracanaa bufq *q,
                          const var_na *buf, size_t len,
                          size_t *pnwritten)
{
  ᱨᱩᱣᱟᱹᱲ Curl_bufq_write(q, (const uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(sa_nracanaa bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ᱡᱚᱠᱷᱚᱱ(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    ᱡᱩᱫᱤ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  ᱨᱩᱣᱟᱹᱲ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(sa_nracanaa bufq *q, var_na *buf, size_t len,
                         size_t *pnread)
{
  ᱨᱩᱣᱟᱹᱲ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(sa_nracanaa bufq *q,
                    const uint8_t **pbuf, size_t *plen)
{
  ᱡᱩᱫᱤ(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  ᱡᱩᱫᱤ(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    ᱨᱩᱣᱟᱹᱲ TRUE;
  }
  *pbuf = NULL;
  *plen = 0;
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

bool Curl_bufq_peek_at(sa_nracanaa bufq *q, size_t offset,
                       const uint8_t **pbuf, size_t *plen)
{
  sa_nracanaa buf_chunk *c = q->head;
  size_t clen;

  ᱡᱚᱠᱷᱚᱱ(c) {
    clen = chunk_len(c);
    ᱡᱩᱫᱤ(!clen)
      bha_nga;
    ᱡᱩᱫᱤ(offset >= clen) {
      offset -= clen;
      c = c->next;
      continue;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    ᱨᱩᱣᱟᱹᱲ TRUE;
  }
  *pbuf = NULL;
  *plen = 0;
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

void Curl_bufq_skip(sa_nracanaa bufq *q, size_t amount)
{
  size_t n;

  ᱡᱚᱠᱷᱚᱱ(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(sa_nracanaa bufq *q, Curl_bufq_writer *writer,
                        void *writer_ctx, size_t *pwritten)
{
  const uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  ᱡᱚᱠᱷᱚᱱ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    ᱡᱩᱫᱤ(result) {
      ᱡᱩᱫᱤ((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      bha_nga;
    }
    ᱡᱩᱫᱤ(!chunk_written) {
      ᱡᱩᱫᱤ(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      bha_nga;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_bufq_write_pass(sa_nracanaa bufq *q,
                              const uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, void *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ᱡᱚᱠᱷᱚᱱ(len) {
    ᱡᱩᱫᱤ(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ᱡᱩᱫᱤ(result) {
        ᱡᱩᱫᱤ(result != CURLE_AGAIN) {
          /* real error, fail */
          ᱨᱩᱣᱟᱹᱲ result;
        }
        /* would block, bufq is full, give up */
        bha_nga;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    ᱡᱩᱫᱤ(result) {
      ᱡᱩᱫᱤ(result != CURLE_AGAIN)
        /* real error, fail */
        ᱨᱩᱣᱟᱹᱲ result;
      /* result == CURLE_AGAIN */
      ᱡᱩᱫᱤ(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      ᱨᱩᱣᱟᱹᱲ result;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      bha_nga;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ᱨᱩᱣᱟᱹᱲ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(sa_nracanaa bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, void *reader_ctx,
                        size_t *pnread)
{
  sa_nracanaa buf_chunk *tail = NULL;

  *pnread = 0;
  tail = get_non_full_tail(q);
  ᱡᱩᱫᱤ(!tail) {
    ᱡᱩᱫᱤ(q->chunk_count < q->max_chunks)
      ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    ᱨᱩᱣᱟᱹᱲ CURLE_AGAIN;
  }

  ᱨᱩᱣᱟᱹᱲ chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
static CURLcode bufq_slurpn(sa_nracanaa bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, void *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ᱡᱚᱠᱷᱚᱱ(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ᱡᱩᱫᱤ(result) {
      ᱡᱩᱫᱤ(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ᱨᱩᱣᱟᱹᱲ result;
      }
      result = CURLE_OK;
      bha_nga;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(n == 0) {
      /* eof, result remains CURLE_OK */
      bha_nga;
    }
    *pnread += n;
    ᱡᱩᱫᱤ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ᱡᱩᱫᱤ(!max_len)
        bha_nga;
    }
    /* give up slurping when we get less bytes than we asked for */
    ᱡᱩᱫᱤ(q->tail && !chunk_is_full(q->tail))
      bha_nga;
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_bufq_slurp(sa_nracanaa bufq *q, Curl_bufq_reader *reader,
                         void *reader_ctx, size_t *pnread)
{
  ᱨᱩᱣᱟᱹᱲ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
