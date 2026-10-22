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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "bufq.h"

ସ୍ଥିର bool chunk_is_empty(ଅଚଳ ସଂରଚନା buf_chunk *chunk)
{
  ଫେରନ୍ତୁ chunk->r_offset >= chunk->w_offset;
}

ସ୍ଥିର bool chunk_is_full(ଅଚଳ ସଂରଚନା buf_chunk *chunk)
{
  ଫେରନ୍ତୁ chunk->w_offset >= chunk->dlen;
}

ସ୍ଥିର size_t chunk_len(ଅଚଳ ସଂରଚନା buf_chunk *chunk)
{
  ଫେରନ୍ତୁ chunk->w_offset - chunk->r_offset;
}

ସ୍ଥିର ଶୂନ୍ୟ chunk_reset(ସଂରଚନା buf_chunk *chunk)
{
  chunk->next = ଖାଲି;
  chunk->r_offset = chunk->w_offset = 0;
}

ସ୍ଥିର size_t chunk_append(ସଂରଚନା buf_chunk *chunk,
                           ଅଚଳ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ଯଦି(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର size_t chunk_read(ସଂରଚନା buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ଯଦି(!n) {
    ଫେରନ୍ତୁ 0;
  }
  ଅନ୍ୟଥା ଯଦି(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    ଫେରନ୍ତୁ n;
  }
  ଅନ୍ୟଥା {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    ଫେରନ୍ତୁ len;
  }
}

ସ୍ଥିର CURLcode chunk_slurpn(ସଂରଚନା buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             ଶୂନ୍ୟ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  ଯଦି(!n)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଯଦି(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  ଯଦି(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ chunk_peek(ଅଚଳ ସଂରଚନା buf_chunk *chunk,
                       ଅଚଳ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

ସ୍ଥିର ଶୂନ୍ୟ chunk_peek_at(ଅଚଳ ସଂରଚନା buf_chunk *chunk, size_t offset,
                          ଅଚଳ uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

ସ୍ଥିର size_t chunk_skip(ସଂରଚନା buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  ଯଦି(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    ଯଦି(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର ଶୂନ୍ୟ chunk_list_free(ସଂରଚନା buf_chunk **anchor)
{
  ସଂରଚନା buf_chunk *chunk;
  ଯେତେବେଳେ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

ଶୂନ୍ୟ Curl_bufcp_init(ସଂରଚନା bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, ଆକାର(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

ସ୍ଥିର CURLcode bufcp_take(ସଂରଚନା bufc_pool *pool,
                           ସଂରଚନା buf_chunk **pchunk)
{
  ସଂରଚନା buf_chunk *chunk = ଖାଲି;

  ଯଦି(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_ଗଣନା;
    chunk_reset(chunk);
    *pchunk = chunk;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ଯଦି(pool->chunk_size > SIZE_MAX - ଆକାର(*chunk)) {
    *pchunk = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, ଆକାର(*chunk) + pool->chunk_size);
  ଯଦି(!chunk) {
    *pchunk = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ bufcp_put(ସଂରଚନା bufc_pool *pool,
                      ସଂରଚନା buf_chunk *chunk)
{
  ଯଦି(pool->spare_ଗଣନା >= pool->spare_max) {
    curlx_free(chunk);
  }
  ଅନ୍ୟଥା {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_ଗଣନା;
  }
}

ଶୂନ୍ୟ Curl_bufcp_free(ସଂରଚନା bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_ଗଣନା = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ bufq_init(ସଂରଚନା bufq *q, ସଂରଚନା bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, ଆକାର(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

ଶୂନ୍ୟ Curl_bufq_init2(ସଂରଚନା bufq *q, size_t chunk_size, size_t max_chunks,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts)
{
  bufq_init(q, ଖାଲି, chunk_size, max_chunks, opts);
}

ଶୂନ୍ୟ Curl_bufq_init(ସଂରଚନା bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, ଖାଲି, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

ଶୂନ୍ୟ Curl_bufq_initp(ସଂରଚନା bufq *q, ସଂରଚନା bufc_pool *pool,
                     size_t max_chunks, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

ଶୂନ୍ୟ Curl_bufq_free(ସଂରଚନା bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = ଖାଲି;
  q->chunk_ଗଣନା = 0;
}

ଶୂନ୍ୟ Curl_bufq_reset(ସଂରଚନା bufq *q)
{
  ସଂରଚନା buf_chunk *chunk;
  ଯେତେବେଳେ(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = ଖାଲି;
}

size_t Curl_bufq_len(ଅଚଳ ସଂରଚନା bufq *q)
{
  ଅଚଳ ସଂରଚନା buf_chunk *chunk = q->head;
  size_t len = 0;
  ଯେତେବେଳେ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  ଫେରନ୍ତୁ len;
}

bool Curl_bufq_is_empty(ଅଚଳ ସଂରଚନା bufq *q)
{
  ଫେରନ୍ତୁ !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ଅଚଳ ସଂରଚନା bufq *q)
{
  ଯଦି(!q->tail || q->spare)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(q->chunk_ଗଣନା < q->max_chunks)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(q->chunk_ଗଣନା > q->max_chunks)
    ଫେରନ୍ତୁ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ଫେରନ୍ତୁ chunk_is_full(q->tail);
}

ସ୍ଥିର ସଂରଚନା buf_chunk *get_spare(ସଂରଚନା bufq *q)
{
  ସଂରଚନା buf_chunk *chunk = ଖାଲି;

  ଯଦି(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    ଫେରନ୍ତୁ chunk;
  }

  ଯଦି(q->chunk_ଗଣନା >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    ଫେରନ୍ତୁ ଖାଲି;

  ଯଦି(q->pool) {
    ଯଦି(bufcp_take(q->pool, &chunk))
      ଫେରନ୍ତୁ ଖାଲି;
    ++q->chunk_ଗଣନା;
    ଫେରନ୍ତୁ chunk;
  }
  ଅନ୍ୟଥା {
    /* Check for integer overflow before allocation */
    ଯଦି(q->chunk_size > SIZE_MAX - ଆକାର(*chunk)) {
      ଫେରନ୍ତୁ ଖାଲି;
    }

    chunk = curlx_calloc(1, ଆକାର(*chunk) + q->chunk_size);
    ଯଦି(!chunk)
      ଫେରନ୍ତୁ ଖାଲି;
    chunk->dlen = q->chunk_size;
    ++q->chunk_ଗଣନା;
    ଫେରନ୍ତୁ chunk;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ prune_head(ସଂରଚନା bufq *q)
{
  ସଂରଚନା buf_chunk *chunk;

  ଯେତେବେଳେ(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    ଯଦି(q->tail == chunk)
      q->tail = q->head;
    ଯଦି(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_ଗଣନା;
    }
    ଅନ୍ୟଥା ଯଦି((q->chunk_ଗଣନା > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_ଗଣନା;
    }
    ଅନ୍ୟଥା {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

ସ୍ଥିର ସଂରଚନା buf_chunk *get_non_full_tail(ସଂରଚନା bufq *q)
{
  ସଂରଚନା buf_chunk *chunk;

  ଯଦି(q->tail && !chunk_is_full(q->tail))
    ଫେରନ୍ତୁ q->tail;
  chunk = get_spare(q);
  ଯଦି(chunk) {
    /* new tail, and possibly new head */
    ଯଦି(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    ଅନ୍ୟଥା {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  ଫେରନ୍ତୁ chunk;
}

CURLcode Curl_bufq_write(ସଂରଚନା bufq *q,
                         ଅଚଳ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ସଂରଚନା buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ଯେତେବେଳେ(len) {
    tail = get_non_full_tail(q);
    ଯଦି(!tail) {
      ଯଦି((q->chunk_ଗଣନା < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଭାଙ୍ଗ;
    }
    n = chunk_append(tail, buf, len);
    ଯଦି(!n)
      ଭାଙ୍ଗ;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ଫେରନ୍ତୁ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ସଂରଚନା bufq *q,
                          ଅଚଳ ଅକ୍ଷର *buf, size_t len,
                          size_t *pnwritten)
{
  ଫେରନ୍ତୁ Curl_bufq_write(q, (ଅଚଳ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ସଂରଚନା bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ଯେତେବେଳେ(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    ଯଦି(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  ଫେରନ୍ତୁ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ସଂରଚନା bufq *q, ଅକ୍ଷର *buf, size_t len,
                         size_t *pnread)
{
  ଫେରନ୍ତୁ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ସଂରଚନା bufq *q,
                    ଅଚଳ uint8_t **pbuf, size_t *plen)
{
  ଯଦି(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  ଯଦି(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    ଫେରନ୍ତୁ TRUE;
  }
  *pbuf = ଖାଲି;
  *plen = 0;
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_bufq_peek_at(ସଂରଚନା bufq *q, size_t offset,
                       ଅଚଳ uint8_t **pbuf, size_t *plen)
{
  ସଂରଚନା buf_chunk *c = q->head;
  size_t clen;

  ଯେତେବେଳେ(c) {
    clen = chunk_len(c);
    ଯଦି(!clen)
      ଭାଙ୍ଗ;
    ଯଦି(offset >= clen) {
      offset -= clen;
      c = c->next;
      ଜାରି;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    ଫେରନ୍ତୁ TRUE;
  }
  *pbuf = ଖାଲି;
  *plen = 0;
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_bufq_skip(ସଂରଚନା bufq *q, size_t amount)
{
  size_t n;

  ଯେତେବେଳେ(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(ସଂରଚନା bufq *q, Curl_bufq_writer *writer,
                        ଶୂନ୍ୟ *writer_ctx, size_t *pwritten)
{
  ଅଚଳ uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  ଯେତେବେଳେ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    ଯଦି(result) {
      ଯଦି((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      ଭାଙ୍ଗ;
    }
    ଯଦି(!chunk_written) {
      ଯଦି(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      ଭାଙ୍ଗ;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_bufq_write_pass(ସଂରଚନା bufq *q,
                              ଅଚଳ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ଶୂନ୍ୟ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ଯେତେବେଳେ(len) {
    ଯଦି(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ଯଦି(result) {
        ଯଦି(result != CURLE_AGAIN) {
          /* real error, fail */
          ଫେରନ୍ତୁ result;
        }
        /* would block, bufq is full, give up */
        ଭାଙ୍ଗ;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN)
        /* real error, fail */
        ଫେରନ୍ତୁ result;
      /* result == CURLE_AGAIN */
      ଯଦି(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା ଯଦି(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ଭାଙ୍ଗ;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ଫେରନ୍ତୁ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ସଂରଚନା bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ଶୂନ୍ୟ *reader_ctx,
                        size_t *pnread)
{
  ସଂରଚନା buf_chunk *tail = ଖାଲି;

  *pnread = 0;
  tail = get_non_full_tail(q);
  ଯଦି(!tail) {
    ଯଦି(q->chunk_ଗଣନା < q->max_chunks)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }

  ଫେରନ୍ତୁ chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ସ୍ଥିର CURLcode bufq_slurpn(ସଂରଚନା bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, ଶୂନ୍ୟ *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ଯେତେବେଳେ(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ଯଦି(result) {
      ଯଦି(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ଫେରନ୍ତୁ result;
      }
      result = CURLE_OK;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(n == 0) {
      /* eof, result remains CURLE_OK */
      ଭାଙ୍ଗ;
    }
    *pnread += n;
    ଯଦି(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ଯଦି(!max_len)
        ଭାଙ୍ଗ;
    }
    /* give up slurping when we get less bytes than we asked for */
    ଯଦି(q->tail && !chunk_is_full(q->tail))
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_bufq_slurp(ସଂରଚନା bufq *q, Curl_bufq_reader *reader,
                         ଶୂନ୍ୟ *reader_ctx, size_t *pnread)
{
  ଫେରନ୍ତୁ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
