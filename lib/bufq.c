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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "bufq.h"

நிலையான bool chunk_is_empty(மாறிலி கட்டமைப்பு buf_chunk *chunk)
{
  திரும்பு chunk->r_offset >= chunk->w_offset;
}

நிலையான bool chunk_is_full(மாறிலி கட்டமைப்பு buf_chunk *chunk)
{
  திரும்பு chunk->w_offset >= chunk->dlen;
}

நிலையான size_t chunk_len(மாறிலி கட்டமைப்பு buf_chunk *chunk)
{
  திரும்பு chunk->w_offset - chunk->r_offset;
}

நிலையான வெறுமை chunk_reset(கட்டமைப்பு buf_chunk *chunk)
{
  chunk->next = வெற்று;
  chunk->r_offset = chunk->w_offset = 0;
}

நிலையான size_t chunk_append(கட்டமைப்பு buf_chunk *chunk,
                           மாறிலி uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  எனில்(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  திரும்பு n;
}

நிலையான size_t chunk_read(கட்டமைப்பு buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  எனில்(!n) {
    திரும்பு 0;
  }
  இல்லையெனில் எனில்(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    திரும்பு n;
  }
  இல்லையெனில் {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    திரும்பு len;
  }
}

நிலையான CURLcode chunk_slurpn(கட்டமைப்பு buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             வெறுமை *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  எனில்(!n)
    திரும்பு CURLE_AGAIN;
  எனில்(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  எனில்(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  திரும்பு result;
}

நிலையான வெறுமை chunk_peek(மாறிலி கட்டமைப்பு buf_chunk *chunk,
                       மாறிலி uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

நிலையான வெறுமை chunk_peek_at(மாறிலி கட்டமைப்பு buf_chunk *chunk, size_t offset,
                          மாறிலி uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

நிலையான size_t chunk_skip(கட்டமைப்பு buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  எனில்(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    எனில்(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  திரும்பு n;
}

நிலையான வெறுமை chunk_list_free(கட்டமைப்பு buf_chunk **anchor)
{
  கட்டமைப்பு buf_chunk *chunk;
  போது(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

வெறுமை Curl_bufcp_init(கட்டமைப்பு bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, அளவு(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

நிலையான CURLcode bufcp_take(கட்டமைப்பு bufc_pool *pool,
                           கட்டமைப்பு buf_chunk **pchunk)
{
  கட்டமைப்பு buf_chunk *chunk = வெற்று;

  எனில்(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_மொத்தம்;
    chunk_reset(chunk);
    *pchunk = chunk;
    திரும்பு CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  எனில்(pool->chunk_size > SIZE_MAX - அளவு(*chunk)) {
    *pchunk = வெற்று;
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, அளவு(*chunk) + pool->chunk_size);
  எனில்(!chunk) {
    *pchunk = வெற்று;
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை bufcp_put(கட்டமைப்பு bufc_pool *pool,
                      கட்டமைப்பு buf_chunk *chunk)
{
  எனில்(pool->spare_மொத்தம் >= pool->spare_max) {
    curlx_free(chunk);
  }
  இல்லையெனில் {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_மொத்தம்;
  }
}

வெறுமை Curl_bufcp_free(கட்டமைப்பு bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_மொத்தம் = 0;
}

நிலையான வெறுமை bufq_init(கட்டமைப்பு bufq *q, கட்டமைப்பு bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, முழு opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, அளவு(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

வெறுமை Curl_bufq_init2(கட்டமைப்பு bufq *q, size_t chunk_size, size_t max_chunks,
                     முழு opts)
{
  bufq_init(q, வெற்று, chunk_size, max_chunks, opts);
}

வெறுமை Curl_bufq_init(கட்டமைப்பு bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, வெற்று, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

வெறுமை Curl_bufq_initp(கட்டமைப்பு bufq *q, கட்டமைப்பு bufc_pool *pool,
                     size_t max_chunks, முழு opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

வெறுமை Curl_bufq_free(கட்டமைப்பு bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = வெற்று;
  q->chunk_மொத்தம் = 0;
}

வெறுமை Curl_bufq_reset(கட்டமைப்பு bufq *q)
{
  கட்டமைப்பு buf_chunk *chunk;
  போது(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = வெற்று;
}

size_t Curl_bufq_len(மாறிலி கட்டமைப்பு bufq *q)
{
  மாறிலி கட்டமைப்பு buf_chunk *chunk = q->head;
  size_t len = 0;
  போது(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  திரும்பு len;
}

bool Curl_bufq_is_empty(மாறிலி கட்டமைப்பு bufq *q)
{
  திரும்பு !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(மாறிலி கட்டமைப்பு bufq *q)
{
  எனில்(!q->tail || q->spare)
    திரும்பு FALSE;
  எனில்(q->chunk_மொத்தம் < q->max_chunks)
    திரும்பு FALSE;
  எனில்(q->chunk_மொத்தம் > q->max_chunks)
    திரும்பு TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  திரும்பு chunk_is_full(q->tail);
}

நிலையான கட்டமைப்பு buf_chunk *get_spare(கட்டமைப்பு bufq *q)
{
  கட்டமைப்பு buf_chunk *chunk = வெற்று;

  எனில்(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    திரும்பு chunk;
  }

  எனில்(q->chunk_மொத்தம் >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    திரும்பு வெற்று;

  எனில்(q->pool) {
    எனில்(bufcp_take(q->pool, &chunk))
      திரும்பு வெற்று;
    ++q->chunk_மொத்தம்;
    திரும்பு chunk;
  }
  இல்லையெனில் {
    /* Check for integer overflow before allocation */
    எனில்(q->chunk_size > SIZE_MAX - அளவு(*chunk)) {
      திரும்பு வெற்று;
    }

    chunk = curlx_calloc(1, அளவு(*chunk) + q->chunk_size);
    எனில்(!chunk)
      திரும்பு வெற்று;
    chunk->dlen = q->chunk_size;
    ++q->chunk_மொத்தம்;
    திரும்பு chunk;
  }
}

நிலையான வெறுமை prune_head(கட்டமைப்பு bufq *q)
{
  கட்டமைப்பு buf_chunk *chunk;

  போது(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    எனில்(q->tail == chunk)
      q->tail = q->head;
    எனில்(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_மொத்தம்;
    }
    இல்லையெனில் எனில்((q->chunk_மொத்தம் > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_மொத்தம்;
    }
    இல்லையெனில் {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

நிலையான கட்டமைப்பு buf_chunk *get_non_full_tail(கட்டமைப்பு bufq *q)
{
  கட்டமைப்பு buf_chunk *chunk;

  எனில்(q->tail && !chunk_is_full(q->tail))
    திரும்பு q->tail;
  chunk = get_spare(q);
  எனில்(chunk) {
    /* new tail, and possibly new head */
    எனில்(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    இல்லையெனில் {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  திரும்பு chunk;
}

CURLcode Curl_bufq_write(கட்டமைப்பு bufq *q,
                         மாறிலி uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  கட்டமைப்பு buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  போது(len) {
    tail = get_non_full_tail(q);
    எனில்(!tail) {
      எனில்((q->chunk_மொத்தம் < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        திரும்பு CURLE_OUT_OF_MEMORY;
      நிறுத்து;
    }
    n = chunk_append(tail, buf, len);
    எனில்(!n)
      நிறுத்து;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  திரும்பு (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(கட்டமைப்பு bufq *q,
                          மாறிலி எழுத்து *buf, size_t len,
                          size_t *pnwritten)
{
  திரும்பு Curl_bufq_write(q, (மாறிலி uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(கட்டமைப்பு bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  போது(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    எனில்(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  திரும்பு (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(கட்டமைப்பு bufq *q, எழுத்து *buf, size_t len,
                         size_t *pnread)
{
  திரும்பு Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(கட்டமைப்பு bufq *q,
                    மாறிலி uint8_t **pbuf, size_t *plen)
{
  எனில்(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  எனில்(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    திரும்பு TRUE;
  }
  *pbuf = வெற்று;
  *plen = 0;
  திரும்பு FALSE;
}

bool Curl_bufq_peek_at(கட்டமைப்பு bufq *q, size_t offset,
                       மாறிலி uint8_t **pbuf, size_t *plen)
{
  கட்டமைப்பு buf_chunk *c = q->head;
  size_t clen;

  போது(c) {
    clen = chunk_len(c);
    எனில்(!clen)
      நிறுத்து;
    எனில்(offset >= clen) {
      offset -= clen;
      c = c->next;
      தொடர்;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    திரும்பு TRUE;
  }
  *pbuf = வெற்று;
  *plen = 0;
  திரும்பு FALSE;
}

வெறுமை Curl_bufq_skip(கட்டமைப்பு bufq *q, size_t amount)
{
  size_t n;

  போது(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(கட்டமைப்பு bufq *q, Curl_bufq_writer *writer,
                        வெறுமை *writer_ctx, size_t *pwritten)
{
  மாறிலி uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  போது(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    எனில்(result) {
      எனில்((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      நிறுத்து;
    }
    எனில்(!chunk_written) {
      எனில்(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      நிறுத்து;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  திரும்பு result;
}

CURLcode Curl_bufq_write_pass(கட்டமைப்பு bufq *q,
                              மாறிலி uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, வெறுமை *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  போது(len) {
    எனில்(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      எனில்(result) {
        எனில்(result != CURLE_AGAIN) {
          /* real error, fail */
          திரும்பு result;
        }
        /* would block, bufq is full, give up */
        நிறுத்து;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    எனில்(result) {
      எனில்(result != CURLE_AGAIN)
        /* real error, fail */
        திரும்பு result;
      /* result == CURLE_AGAIN */
      எனில்(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      திரும்பு result;
    }
    இல்லையெனில் எனில்(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      நிறுத்து;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  திரும்பு (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(கட்டமைப்பு bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, வெறுமை *reader_ctx,
                        size_t *pnread)
{
  கட்டமைப்பு buf_chunk *tail = வெற்று;

  *pnread = 0;
  tail = get_non_full_tail(q);
  எனில்(!tail) {
    எனில்(q->chunk_மொத்தம் < q->max_chunks)
      திரும்பு CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    திரும்பு CURLE_AGAIN;
  }

  திரும்பு chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
நிலையான CURLcode bufq_slurpn(கட்டமைப்பு bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, வெறுமை *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  போது(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    எனில்(result) {
      எனில்(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        திரும்பு result;
      }
      result = CURLE_OK;
      நிறுத்து;
    }
    இல்லையெனில் எனில்(n == 0) {
      /* eof, result remains CURLE_OK */
      நிறுத்து;
    }
    *pnread += n;
    எனில்(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      எனில்(!max_len)
        நிறுத்து;
    }
    /* give up slurping when we get less bytes than we asked for */
    எனில்(q->tail && !chunk_is_full(q->tail))
      நிறுத்து;
  }
  திரும்பு result;
}

CURLcode Curl_bufq_slurp(கட்டமைப்பு bufq *q, Curl_bufq_reader *reader,
                         வெறுமை *reader_ctx, size_t *pnread)
{
  திரும்பு bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
