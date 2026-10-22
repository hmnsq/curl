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
#شامل "curl_setup.h"

#شامل "bufq.h"

ایستا bool chunk_is_empty(ثابت ساختار buf_chunk *chunk)
{
  بازگشت chunk->r_offset >= chunk->w_offset;
}

ایستا bool chunk_is_full(ثابت ساختار buf_chunk *chunk)
{
  بازگشت chunk->w_offset >= chunk->dlen;
}

ایستا size_t chunk_len(ثابت ساختار buf_chunk *chunk)
{
  بازگشت chunk->w_offset - chunk->r_offset;
}

ایستا تهی chunk_reset(ساختار buf_chunk *chunk)
{
  chunk->next = پوچ;
  chunk->r_offset = chunk->w_offset = 0;
}

ایستا size_t chunk_append(ساختار buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  اگر(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  بازگشت n;
}

ایستا size_t chunk_read(ساختار buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  اگر(!n) {
    بازگشت 0;
  }
  وگرنه اگر(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    بازگشت n;
  }
  وگرنه {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    بازگشت len;
  }
}

ایستا CURLcode chunk_slurpn(ساختار buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             تهی *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  اگر(!n)
    بازگشت CURLE_AGAIN;
  اگر(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  اگر(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  بازگشت result;
}

ایستا تهی chunk_peek(ثابت ساختار buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

ایستا تهی chunk_peek_at(ثابت ساختار buf_chunk *chunk, size_t offset,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

ایستا size_t chunk_skip(ساختار buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  اگر(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    اگر(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  بازگشت n;
}

ایستا تهی chunk_list_free(ساختار buf_chunk **anchor)
{
  ساختار buf_chunk *chunk;
  وقتی(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

تهی Curl_bufcp_init(ساختار bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, اندازه(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

ایستا CURLcode bufcp_take(ساختار bufc_pool *pool,
                           ساختار buf_chunk **pchunk)
{
  ساختار buf_chunk *chunk = پوچ;

  اگر(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_شمار;
    chunk_reset(chunk);
    *pchunk = chunk;
    بازگشت CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  اگر(pool->chunk_size > SIZE_MAX - اندازه(*chunk)) {
    *pchunk = پوچ;
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, اندازه(*chunk) + pool->chunk_size);
  اگر(!chunk) {
    *pchunk = پوچ;
    بازگشت CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  بازگشت CURLE_OK;
}

ایستا تهی bufcp_put(ساختار bufc_pool *pool,
                      ساختار buf_chunk *chunk)
{
  اگر(pool->spare_شمار >= pool->spare_max) {
    curlx_free(chunk);
  }
  وگرنه {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_شمار;
  }
}

تهی Curl_bufcp_free(ساختار bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_شمار = 0;
}

ایستا تهی bufq_init(ساختار bufq *q, ساختار bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, صحیح opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, اندازه(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

تهی Curl_bufq_init2(ساختار bufq *q, size_t chunk_size, size_t max_chunks,
                     صحیح opts)
{
  bufq_init(q, پوچ, chunk_size, max_chunks, opts);
}

تهی Curl_bufq_init(ساختار bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, پوچ, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

تهی Curl_bufq_initp(ساختار bufq *q, ساختار bufc_pool *pool,
                     size_t max_chunks, صحیح opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

تهی Curl_bufq_free(ساختار bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = پوچ;
  q->chunk_شمار = 0;
}

تهی Curl_bufq_reset(ساختار bufq *q)
{
  ساختار buf_chunk *chunk;
  وقتی(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = پوچ;
}

size_t Curl_bufq_len(ثابت ساختار bufq *q)
{
  ثابت ساختار buf_chunk *chunk = q->head;
  size_t len = 0;
  وقتی(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  بازگشت len;
}

bool Curl_bufq_is_empty(ثابت ساختار bufq *q)
{
  بازگشت !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ثابت ساختار bufq *q)
{
  اگر(!q->tail || q->spare)
    بازگشت FALSE;
  اگر(q->chunk_شمار < q->max_chunks)
    بازگشت FALSE;
  اگر(q->chunk_شمار > q->max_chunks)
    بازگشت TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  بازگشت chunk_is_full(q->tail);
}

ایستا ساختار buf_chunk *get_spare(ساختار bufq *q)
{
  ساختار buf_chunk *chunk = پوچ;

  اگر(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    بازگشت chunk;
  }

  اگر(q->chunk_شمار >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    بازگشت پوچ;

  اگر(q->pool) {
    اگر(bufcp_take(q->pool, &chunk))
      بازگشت پوچ;
    ++q->chunk_شمار;
    بازگشت chunk;
  }
  وگرنه {
    /* Check for integer overflow before allocation */
    اگر(q->chunk_size > SIZE_MAX - اندازه(*chunk)) {
      بازگشت پوچ;
    }

    chunk = curlx_calloc(1, اندازه(*chunk) + q->chunk_size);
    اگر(!chunk)
      بازگشت پوچ;
    chunk->dlen = q->chunk_size;
    ++q->chunk_شمار;
    بازگشت chunk;
  }
}

ایستا تهی prune_head(ساختار bufq *q)
{
  ساختار buf_chunk *chunk;

  وقتی(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    اگر(q->tail == chunk)
      q->tail = q->head;
    اگر(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_شمار;
    }
    وگرنه اگر((q->chunk_شمار > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_شمار;
    }
    وگرنه {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

ایستا ساختار buf_chunk *get_non_full_tail(ساختار bufq *q)
{
  ساختار buf_chunk *chunk;

  اگر(q->tail && !chunk_is_full(q->tail))
    بازگشت q->tail;
  chunk = get_spare(q);
  اگر(chunk) {
    /* new tail, and possibly new head */
    اگر(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    وگرنه {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  بازگشت chunk;
}

CURLcode Curl_bufq_write(ساختار bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ساختار buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  وقتی(len) {
    tail = get_non_full_tail(q);
    اگر(!tail) {
      اگر((q->chunk_شمار < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        بازگشت CURLE_OUT_OF_MEMORY;
      بشکن;
    }
    n = chunk_append(tail, buf, len);
    اگر(!n)
      بشکن;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  بازگشت (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ساختار bufq *q,
                          ثابت نویسه *buf, size_t len,
                          size_t *pnwritten)
{
  بازگشت Curl_bufq_write(q, (ثابت uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ساختار bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  وقتی(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    اگر(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  بازگشت (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ساختار bufq *q, نویسه *buf, size_t len,
                         size_t *pnread)
{
  بازگشت Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ساختار bufq *q,
                    ثابت uint8_t **pbuf, size_t *plen)
{
  اگر(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  اگر(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    بازگشت TRUE;
  }
  *pbuf = پوچ;
  *plen = 0;
  بازگشت FALSE;
}

bool Curl_bufq_peek_at(ساختار bufq *q, size_t offset,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  ساختار buf_chunk *c = q->head;
  size_t clen;

  وقتی(c) {
    clen = chunk_len(c);
    اگر(!clen)
      بشکن;
    اگر(offset >= clen) {
      offset -= clen;
      c = c->next;
      ادامه;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    بازگشت TRUE;
  }
  *pbuf = پوچ;
  *plen = 0;
  بازگشت FALSE;
}

تهی Curl_bufq_skip(ساختار bufq *q, size_t amount)
{
  size_t n;

  وقتی(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(ساختار bufq *q, Curl_bufq_writer *writer,
                        تهی *writer_ctx, size_t *pwritten)
{
  ثابت uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  وقتی(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    اگر(result) {
      اگر((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      بشکن;
    }
    اگر(!chunk_written) {
      اگر(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      بشکن;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  بازگشت result;
}

CURLcode Curl_bufq_write_pass(ساختار bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, تهی *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  وقتی(len) {
    اگر(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      اگر(result) {
        اگر(result != CURLE_AGAIN) {
          /* real error, fail */
          بازگشت result;
        }
        /* would block, bufq is full, give up */
        بشکن;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    اگر(result) {
      اگر(result != CURLE_AGAIN)
        /* real error, fail */
        بازگشت result;
      /* result == CURLE_AGAIN */
      اگر(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      بازگشت result;
    }
    وگرنه اگر(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      بشکن;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  بازگشت (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ساختار bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, تهی *reader_ctx,
                        size_t *pnread)
{
  ساختار buf_chunk *tail = پوچ;

  *pnread = 0;
  tail = get_non_full_tail(q);
  اگر(!tail) {
    اگر(q->chunk_شمار < q->max_chunks)
      بازگشت CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    بازگشت CURLE_AGAIN;
  }

  بازگشت chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ایستا CURLcode bufq_slurpn(ساختار bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, تهی *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  وقتی(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    اگر(result) {
      اگر(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        بازگشت result;
      }
      result = CURLE_OK;
      بشکن;
    }
    وگرنه اگر(n == 0) {
      /* eof, result remains CURLE_OK */
      بشکن;
    }
    *pnread += n;
    اگر(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      اگر(!max_len)
        بشکن;
    }
    /* give up slurping when we get less bytes than we asked for */
    اگر(q->tail && !chunk_is_full(q->tail))
      بشکن;
  }
  بازگشت result;
}

CURLcode Curl_bufq_slurp(ساختار bufq *q, Curl_bufq_reader *reader,
                         تهی *reader_ctx, size_t *pnread)
{
  بازگشت bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
