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
#ضمن "curl_setup.h"

#ضمن "bufq.h"

ساكن bool chunk_is_empty(ثابت بنية buf_chunk *chunk)
{
  החזר chunk->r_offset >= chunk->w_offset;
}

ساكن bool chunk_is_full(ثابت بنية buf_chunk *chunk)
{
  החזר chunk->w_offset >= chunk->dlen;
}

ساكن size_t chunk_len(ثابت بنية buf_chunk *chunk)
{
  החזר chunk->w_offset - chunk->r_offset;
}

ساكن فراغ chunk_reset(بنية buf_chunk *chunk)
{
  chunk->next = عدم;
  chunk->r_offset = chunk->w_offset = 0;
}

ساكن size_t chunk_append(بنية buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  אם(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  החזר n;
}

ساكن size_t chunk_read(بنية buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  אם(!n) {
    החזר 0;
  }
  אחרת אם(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    החזר n;
  }
  אחרת {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    החזר len;
  }
}

ساكن CURLcode chunk_slurpn(بنية buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             فراغ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  אם(!n)
    החזר CURLE_AGAIN;
  אם(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  אם(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  החזר result;
}

ساكن فراغ chunk_peek(ثابت بنية buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

ساكن فراغ chunk_peek_at(ثابت بنية buf_chunk *chunk, size_t offset,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

ساكن size_t chunk_skip(بنية buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  אם(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    אם(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  החזר n;
}

ساكن فراغ chunk_list_free(بنية buf_chunk **anchor)
{
  بنية buf_chunk *chunk;
  כלעוד(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

فراغ Curl_bufcp_init(بنية bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, حجم(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

ساكن CURLcode bufcp_take(بنية bufc_pool *pool,
                           بنية buf_chunk **pchunk)
{
  بنية buf_chunk *chunk = عدم;

  אם(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_count;
    chunk_reset(chunk);
    *pchunk = chunk;
    החזר CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  אם(pool->chunk_size > SIZE_MAX - حجم(*chunk)) {
    *pchunk = عدم;
    החזר CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, حجم(*chunk) + pool->chunk_size);
  אם(!chunk) {
    *pchunk = عدم;
    החזר CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  החזר CURLE_OK;
}

ساكن فراغ bufcp_put(بنية bufc_pool *pool,
                      بنية buf_chunk *chunk)
{
  אם(pool->spare_count >= pool->spare_max) {
    curlx_free(chunk);
  }
  אחרת {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_count;
  }
}

فراغ Curl_bufcp_free(بنية bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_count = 0;
}

ساكن فراغ bufq_init(بنية bufq *q, بنية bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, صحيح opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, حجم(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

فراغ Curl_bufq_init2(بنية bufq *q, size_t chunk_size, size_t max_chunks,
                     صحيح opts)
{
  bufq_init(q, عدم, chunk_size, max_chunks, opts);
}

فراغ Curl_bufq_init(بنية bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, عدم, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

فراغ Curl_bufq_initp(بنية bufq *q, بنية bufc_pool *pool,
                     size_t max_chunks, صحيح opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

فراغ Curl_bufq_free(بنية bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = عدم;
  q->chunk_count = 0;
}

فراغ Curl_bufq_reset(بنية bufq *q)
{
  بنية buf_chunk *chunk;
  כלעוד(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = عدم;
}

size_t Curl_bufq_len(ثابت بنية bufq *q)
{
  ثابت بنية buf_chunk *chunk = q->head;
  size_t len = 0;
  כלעוד(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  החזר len;
}

bool Curl_bufq_is_empty(ثابت بنية bufq *q)
{
  החזר !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ثابت بنية bufq *q)
{
  אם(!q->tail || q->spare)
    החזר FALSE;
  אם(q->chunk_count < q->max_chunks)
    החזר FALSE;
  אם(q->chunk_count > q->max_chunks)
    החזר TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  החזר chunk_is_full(q->tail);
}

ساكن بنية buf_chunk *get_spare(بنية bufq *q)
{
  بنية buf_chunk *chunk = عدم;

  אם(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    החזר chunk;
  }

  אם(q->chunk_count >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    החזר عدم;

  אם(q->pool) {
    אם(bufcp_take(q->pool, &chunk))
      החזר عدم;
    ++q->chunk_count;
    החזר chunk;
  }
  אחרת {
    /* Check for integer overflow before allocation */
    אם(q->chunk_size > SIZE_MAX - حجم(*chunk)) {
      החזר عدم;
    }

    chunk = curlx_calloc(1, حجم(*chunk) + q->chunk_size);
    אם(!chunk)
      החזר عدم;
    chunk->dlen = q->chunk_size;
    ++q->chunk_count;
    החזר chunk;
  }
}

ساكن فراغ prune_head(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  כלעוד(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    אם(q->tail == chunk)
      q->tail = q->head;
    אם(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_count;
    }
    אחרת אם((q->chunk_count > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_count;
    }
    אחרת {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

ساكن بنية buf_chunk *get_non_full_tail(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  אם(q->tail && !chunk_is_full(q->tail))
    החזר q->tail;
  chunk = get_spare(q);
  אם(chunk) {
    /* new tail, and possibly new head */
    אם(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    אחרת {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  החזר chunk;
}

CURLcode Curl_bufq_write(بنية bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  بنية buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  כלעוד(len) {
    tail = get_non_full_tail(q);
    אם(!tail) {
      אם((q->chunk_count < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        החזר CURLE_OUT_OF_MEMORY;
      اكسر;
    }
    n = chunk_append(tail, buf, len);
    אם(!n)
      اكسر;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  החזר (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(بنية bufq *q,
                          ثابت حرف *buf, size_t len,
                          size_t *pnwritten)
{
  החזר Curl_bufq_write(q, (ثابت uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(بنية bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  כלעוד(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    אם(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  החזר (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(بنية bufq *q, حرف *buf, size_t len,
                         size_t *pnread)
{
  החזר Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(بنية bufq *q,
                    ثابت uint8_t **pbuf, size_t *plen)
{
  אם(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  אם(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    החזר TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  החזר FALSE;
}

bool Curl_bufq_peek_at(بنية bufq *q, size_t offset,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  بنية buf_chunk *c = q->head;
  size_t clen;

  כלעוד(c) {
    clen = chunk_len(c);
    אם(!clen)
      اكسر;
    אם(offset >= clen) {
      offset -= clen;
      c = c->next;
      استمر;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    החזר TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  החזר FALSE;
}

فراغ Curl_bufq_skip(بنية bufq *q, size_t amount)
{
  size_t n;

  כלעוד(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(بنية bufq *q, Curl_bufq_writer *writer,
                        فراغ *writer_ctx, size_t *pwritten)
{
  ثابت uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  כלעוד(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    אם(result) {
      אם((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      اكسر;
    }
    אם(!chunk_written) {
      אם(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      اكسر;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  החזר result;
}

CURLcode Curl_bufq_write_pass(بنية bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, فراغ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  כלעוד(len) {
    אם(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      אם(result) {
        אם(result != CURLE_AGAIN) {
          /* real error, fail */
          החזר result;
        }
        /* would block, bufq is full, give up */
        اكسر;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    אם(result) {
      אם(result != CURLE_AGAIN)
        /* real error, fail */
        החזר result;
      /* result == CURLE_AGAIN */
      אם(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      החזר result;
    }
    אחרת אם(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      اكسر;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  החזר (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(بنية bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, فراغ *reader_ctx,
                        size_t *pnread)
{
  بنية buf_chunk *tail = عدم;

  *pnread = 0;
  tail = get_non_full_tail(q);
  אם(!tail) {
    אם(q->chunk_count < q->max_chunks)
      החזר CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    החזר CURLE_AGAIN;
  }

  החזר chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ساكن CURLcode bufq_slurpn(بنية bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, فراغ *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  כלעוד(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    אם(result) {
      אם(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        החזר result;
      }
      result = CURLE_OK;
      اكسر;
    }
    אחרת אם(n == 0) {
      /* eof, result remains CURLE_OK */
      اكسر;
    }
    *pnread += n;
    אם(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      אם(!max_len)
        اكسر;
    }
    /* give up slurping when we get less bytes than we asked for */
    אם(q->tail && !chunk_is_full(q->tail))
      اكسر;
  }
  החזר result;
}

CURLcode Curl_bufq_slurp(بنية bufq *q, Curl_bufq_reader *reader,
                         فراغ *reader_ctx, size_t *pnread)
{
  החזר bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
