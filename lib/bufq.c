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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "bufq.h"

স্থির bool chunk_is_empty(ধ্রুৱ সংরচনা buf_chunk *chunk)
{
  ঘূৰাই chunk->r_offset >= chunk->w_offset;
}

স্থির bool chunk_is_full(ধ্রুৱ সংরচনা buf_chunk *chunk)
{
  ঘূৰাই chunk->w_offset >= chunk->dlen;
}

স্থির size_t chunk_len(ধ্রুৱ সংরচনা buf_chunk *chunk)
{
  ঘূৰাই chunk->w_offset - chunk->r_offset;
}

স্থির শূন্য chunk_reset(সংরচনা buf_chunk *chunk)
{
  chunk->next = রিক্ত;
  chunk->r_offset = chunk->w_offset = 0;
}

স্থির size_t chunk_append(সংরচনা buf_chunk *chunk,
                           ধ্রুৱ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  যদি(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  ঘূৰাই n;
}

স্থির size_t chunk_read(সংরচনা buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  যদি(!n) {
    ঘূৰাই 0;
  }
  অন্যথা যদি(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    ঘূৰাই n;
  }
  অন্যথা {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    ঘূৰাই len;
  }
}

স্থির CURLcode chunk_slurpn(সংরচনা buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             শূন্য *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  যদি(!n)
    ঘূৰাই CURLE_AGAIN;
  যদি(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  যদি(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  ঘূৰাই result;
}

স্থির শূন্য chunk_peek(ধ্রুৱ সংরচনা buf_chunk *chunk,
                       ধ্রুৱ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

স্থির শূন্য chunk_peek_at(ধ্রুৱ সংরচনা buf_chunk *chunk, size_t offset,
                          ধ্রুৱ uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

স্থির size_t chunk_skip(সংরচনা buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  যদি(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    যদি(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  ঘূৰাই n;
}

স্থির শূন্য chunk_list_free(সংরচনা buf_chunk **anchor)
{
  সংরচনা buf_chunk *chunk;
  যেতিয়ালৈ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

শূন্য Curl_bufcp_init(সংরচনা bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, পরিমাণ(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

স্থির CURLcode bufcp_take(সংরচনা bufc_pool *pool,
                           সংরচনা buf_chunk **pchunk)
{
  সংরচনা buf_chunk *chunk = রিক্ত;

  যদি(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_গুনতি;
    chunk_reset(chunk);
    *pchunk = chunk;
    ঘূৰাই CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  যদি(pool->chunk_size > SIZE_MAX - পরিমাণ(*chunk)) {
    *pchunk = রিক্ত;
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, পরিমাণ(*chunk) + pool->chunk_size);
  যদি(!chunk) {
    *pchunk = রিক্ত;
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য bufcp_put(সংরচনা bufc_pool *pool,
                      সংরচনা buf_chunk *chunk)
{
  যদি(pool->spare_গুনতি >= pool->spare_max) {
    curlx_free(chunk);
  }
  অন্যথা {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_গুনতি;
  }
}

শূন্য Curl_bufcp_free(সংরচনা bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_গুনতি = 0;
}

স্থির শূন্য bufq_init(সংরচনা bufq *q, সংরচনা bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, পূর্ণাঙ্ক opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, পরিমাণ(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

শূন্য Curl_bufq_init2(সংরচনা bufq *q, size_t chunk_size, size_t max_chunks,
                     পূর্ণাঙ্ক opts)
{
  bufq_init(q, রিক্ত, chunk_size, max_chunks, opts);
}

শূন্য Curl_bufq_init(সংরচনা bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, রিক্ত, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

শূন্য Curl_bufq_initp(সংরচনা bufq *q, সংরচনা bufc_pool *pool,
                     size_t max_chunks, পূর্ণাঙ্ক opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

শূন্য Curl_bufq_free(সংরচনা bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = রিক্ত;
  q->chunk_গুনতি = 0;
}

শূন্য Curl_bufq_reset(সংরচনা bufq *q)
{
  সংরচনা buf_chunk *chunk;
  যেতিয়ালৈ(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = রিক্ত;
}

size_t Curl_bufq_len(ধ্রুৱ সংরচনা bufq *q)
{
  ধ্রুৱ সংরচনা buf_chunk *chunk = q->head;
  size_t len = 0;
  যেতিয়ালৈ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  ঘূৰাই len;
}

bool Curl_bufq_is_empty(ধ্রুৱ সংরচনা bufq *q)
{
  ঘূৰাই !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ধ্রুৱ সংরচনা bufq *q)
{
  যদি(!q->tail || q->spare)
    ঘূৰাই FALSE;
  যদি(q->chunk_গুনতি < q->max_chunks)
    ঘূৰাই FALSE;
  যদি(q->chunk_গুনতি > q->max_chunks)
    ঘূৰাই TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ঘূৰাই chunk_is_full(q->tail);
}

স্থির সংরচনা buf_chunk *get_spare(সংরচনা bufq *q)
{
  সংরচনা buf_chunk *chunk = রিক্ত;

  যদি(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    ঘূৰাই chunk;
  }

  যদি(q->chunk_গুনতি >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    ঘূৰাই রিক্ত;

  যদি(q->pool) {
    যদি(bufcp_take(q->pool, &chunk))
      ঘূৰাই রিক্ত;
    ++q->chunk_গুনতি;
    ঘূৰাই chunk;
  }
  অন্যথা {
    /* Check for integer overflow before allocation */
    যদি(q->chunk_size > SIZE_MAX - পরিমাণ(*chunk)) {
      ঘূৰাই রিক্ত;
    }

    chunk = curlx_calloc(1, পরিমাণ(*chunk) + q->chunk_size);
    যদি(!chunk)
      ঘূৰাই রিক্ত;
    chunk->dlen = q->chunk_size;
    ++q->chunk_গুনতি;
    ঘূৰাই chunk;
  }
}

স্থির শূন্য prune_head(সংরচনা bufq *q)
{
  সংরচনা buf_chunk *chunk;

  যেতিয়ালৈ(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    যদি(q->tail == chunk)
      q->tail = q->head;
    যদি(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_গুনতি;
    }
    অন্যথা যদি((q->chunk_গুনতি > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_গুনতি;
    }
    অন্যথা {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

স্থির সংরচনা buf_chunk *get_non_full_tail(সংরচনা bufq *q)
{
  সংরচনা buf_chunk *chunk;

  যদি(q->tail && !chunk_is_full(q->tail))
    ঘূৰাই q->tail;
  chunk = get_spare(q);
  যদি(chunk) {
    /* new tail, and possibly new head */
    যদি(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    অন্যথা {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  ঘূৰাই chunk;
}

CURLcode Curl_bufq_write(সংরচনা bufq *q,
                         ধ্রুৱ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  সংরচনা buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  যেতিয়ালৈ(len) {
    tail = get_non_full_tail(q);
    যদি(!tail) {
      যদি((q->chunk_গুনতি < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        ঘূৰাই CURLE_OUT_OF_MEMORY;
      ভঙ্গ;
    }
    n = chunk_append(tail, buf, len);
    যদি(!n)
      ভঙ্গ;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ঘূৰাই (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(সংরচনা bufq *q,
                          ধ্রুৱ ৱর্ণ *buf, size_t len,
                          size_t *pnwritten)
{
  ঘূৰাই Curl_bufq_write(q, (ধ্রুৱ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(সংরচনা bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  যেতিয়ালৈ(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    যদি(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  ঘূৰাই (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(সংরচনা bufq *q, ৱর্ণ *buf, size_t len,
                         size_t *pnread)
{
  ঘূৰাই Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(সংরচনা bufq *q,
                    ধ্রুৱ uint8_t **pbuf, size_t *plen)
{
  যদি(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  যদি(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    ঘূৰাই TRUE;
  }
  *pbuf = রিক্ত;
  *plen = 0;
  ঘূৰাই FALSE;
}

bool Curl_bufq_peek_at(সংরচনা bufq *q, size_t offset,
                       ধ্রুৱ uint8_t **pbuf, size_t *plen)
{
  সংরচনা buf_chunk *c = q->head;
  size_t clen;

  যেতিয়ালৈ(c) {
    clen = chunk_len(c);
    যদি(!clen)
      ভঙ্গ;
    যদি(offset >= clen) {
      offset -= clen;
      c = c->next;
      অনুৱর্ত;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    ঘূৰাই TRUE;
  }
  *pbuf = রিক্ত;
  *plen = 0;
  ঘূৰাই FALSE;
}

শূন্য Curl_bufq_skip(সংরচনা bufq *q, size_t amount)
{
  size_t n;

  যেতিয়ালৈ(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(সংরচনা bufq *q, Curl_bufq_writer *writer,
                        শূন্য *writer_ctx, size_t *pwritten)
{
  ধ্রুৱ uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  যেতিয়ালৈ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    যদি(result) {
      যদি((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      ভঙ্গ;
    }
    যদি(!chunk_written) {
      যদি(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      ভঙ্গ;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ঘূৰাই result;
}

CURLcode Curl_bufq_write_pass(সংরচনা bufq *q,
                              ধ্রুৱ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, শূন্য *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  যেতিয়ালৈ(len) {
    যদি(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      যদি(result) {
        যদি(result != CURLE_AGAIN) {
          /* real error, fail */
          ঘূৰাই result;
        }
        /* would block, bufq is full, give up */
        ভঙ্গ;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    যদি(result) {
      যদি(result != CURLE_AGAIN)
        /* real error, fail */
        ঘূৰাই result;
      /* result == CURLE_AGAIN */
      যদি(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      ঘূৰাই result;
    }
    অন্যথা যদি(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ভঙ্গ;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ঘূৰাই (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(সংরচনা bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, শূন্য *reader_ctx,
                        size_t *pnread)
{
  সংরচনা buf_chunk *tail = রিক্ত;

  *pnread = 0;
  tail = get_non_full_tail(q);
  যদি(!tail) {
    যদি(q->chunk_গুনতি < q->max_chunks)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    ঘূৰাই CURLE_AGAIN;
  }

  ঘূৰাই chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
স্থির CURLcode bufq_slurpn(সংরচনা bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, শূন্য *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  যেতিয়ালৈ(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    যদি(result) {
      যদি(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ঘূৰাই result;
      }
      result = CURLE_OK;
      ভঙ্গ;
    }
    অন্যথা যদি(n == 0) {
      /* eof, result remains CURLE_OK */
      ভঙ্গ;
    }
    *pnread += n;
    যদি(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      যদি(!max_len)
        ভঙ্গ;
    }
    /* give up slurping when we get less bytes than we asked for */
    যদি(q->tail && !chunk_is_full(q->tail))
      ভঙ্গ;
  }
  ঘূৰাই result;
}

CURLcode Curl_bufq_slurp(সংরচনা bufq *q, Curl_bufq_reader *reader,
                         শূন্য *reader_ctx, size_t *pnread)
{
  ঘূৰাই bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
