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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "bufq.h"

സ്ഥിതം bool chunk_is_empty(സ്ഥിരം ഘടന buf_chunk *chunk)
{
  തിരികെ chunk->r_offset >= chunk->w_offset;
}

സ്ഥിതം bool chunk_is_full(സ്ഥിരം ഘടന buf_chunk *chunk)
{
  തിരികെ chunk->w_offset >= chunk->dlen;
}

സ്ഥിതം size_t chunk_len(സ്ഥിരം ഘടന buf_chunk *chunk)
{
  തിരികെ chunk->w_offset - chunk->r_offset;
}

സ്ഥിതം ശൂന്യം chunk_reset(ഘടന buf_chunk *chunk)
{
  chunk->next = ശൂന്യ;
  chunk->r_offset = chunk->w_offset = 0;
}

സ്ഥിതം size_t chunk_append(ഘടന buf_chunk *chunk,
                           സ്ഥിരം uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  എങ്കിൽ(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  തിരികെ n;
}

സ്ഥിതം size_t chunk_read(ഘടന buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  എങ്കിൽ(!n) {
    തിരികെ 0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    തിരികെ n;
  }
  അല്ലെങ്കിൽ {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    തിരികെ len;
  }
}

സ്ഥിതം CURLcode chunk_slurpn(ഘടന buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             ശൂന്യം *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  എങ്കിൽ(!n)
    തിരികെ CURLE_AGAIN;
  എങ്കിൽ(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  എങ്കിൽ(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം chunk_peek(സ്ഥിരം ഘടന buf_chunk *chunk,
                       സ്ഥിരം uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

സ്ഥിതം ശൂന്യം chunk_peek_at(സ്ഥിരം ഘടന buf_chunk *chunk, size_t offset,
                          സ്ഥിരം uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

സ്ഥിതം size_t chunk_skip(ഘടന buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  എങ്കിൽ(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    എങ്കിൽ(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  തിരികെ n;
}

സ്ഥിതം ശൂന്യം chunk_list_free(ഘടന buf_chunk **anchor)
{
  ഘടന buf_chunk *chunk;
  ആകുമ്പോൾ(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

ശൂന്യം Curl_bufcp_init(ഘടന bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, വലുപ്പം(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

സ്ഥിതം CURLcode bufcp_take(ഘടന bufc_pool *pool,
                           ഘടന buf_chunk **pchunk)
{
  ഘടന buf_chunk *chunk = ശൂന്യ;

  എങ്കിൽ(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_എണ്ണം;
    chunk_reset(chunk);
    *pchunk = chunk;
    തിരികെ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  എങ്കിൽ(pool->chunk_size > SIZE_MAX - വലുപ്പം(*chunk)) {
    *pchunk = ശൂന്യ;
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, വലുപ്പം(*chunk) + pool->chunk_size);
  എങ്കിൽ(!chunk) {
    *pchunk = ശൂന്യ;
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം bufcp_put(ഘടന bufc_pool *pool,
                      ഘടന buf_chunk *chunk)
{
  എങ്കിൽ(pool->spare_എണ്ണം >= pool->spare_max) {
    curlx_free(chunk);
  }
  അല്ലെങ്കിൽ {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_എണ്ണം;
  }
}

ശൂന്യം Curl_bufcp_free(ഘടന bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_എണ്ണം = 0;
}

സ്ഥിതം ശൂന്യം bufq_init(ഘടന bufq *q, ഘടന bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, പൂർണ്ണസംഖ്യ opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, വലുപ്പം(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

ശൂന്യം Curl_bufq_init2(ഘടന bufq *q, size_t chunk_size, size_t max_chunks,
                     പൂർണ്ണസംഖ്യ opts)
{
  bufq_init(q, ശൂന്യ, chunk_size, max_chunks, opts);
}

ശൂന്യം Curl_bufq_init(ഘടന bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, ശൂന്യ, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

ശൂന്യം Curl_bufq_initp(ഘടന bufq *q, ഘടന bufc_pool *pool,
                     size_t max_chunks, പൂർണ്ണസംഖ്യ opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

ശൂന്യം Curl_bufq_free(ഘടന bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = ശൂന്യ;
  q->chunk_എണ്ണം = 0;
}

ശൂന്യം Curl_bufq_reset(ഘടന bufq *q)
{
  ഘടന buf_chunk *chunk;
  ആകുമ്പോൾ(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = ശൂന്യ;
}

size_t Curl_bufq_len(സ്ഥിരം ഘടന bufq *q)
{
  സ്ഥിരം ഘടന buf_chunk *chunk = q->head;
  size_t len = 0;
  ആകുമ്പോൾ(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  തിരികെ len;
}

bool Curl_bufq_is_empty(സ്ഥിരം ഘടന bufq *q)
{
  തിരികെ !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(സ്ഥിരം ഘടന bufq *q)
{
  എങ്കിൽ(!q->tail || q->spare)
    തിരികെ FALSE;
  എങ്കിൽ(q->chunk_എണ്ണം < q->max_chunks)
    തിരികെ FALSE;
  എങ്കിൽ(q->chunk_എണ്ണം > q->max_chunks)
    തിരികെ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  തിരികെ chunk_is_full(q->tail);
}

സ്ഥിതം ഘടന buf_chunk *get_spare(ഘടന bufq *q)
{
  ഘടന buf_chunk *chunk = ശൂന്യ;

  എങ്കിൽ(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    തിരികെ chunk;
  }

  എങ്കിൽ(q->chunk_എണ്ണം >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    തിരികെ ശൂന്യ;

  എങ്കിൽ(q->pool) {
    എങ്കിൽ(bufcp_take(q->pool, &chunk))
      തിരികെ ശൂന്യ;
    ++q->chunk_എണ്ണം;
    തിരികെ chunk;
  }
  അല്ലെങ്കിൽ {
    /* Check for integer overflow before allocation */
    എങ്കിൽ(q->chunk_size > SIZE_MAX - വലുപ്പം(*chunk)) {
      തിരികെ ശൂന്യ;
    }

    chunk = curlx_calloc(1, വലുപ്പം(*chunk) + q->chunk_size);
    എങ്കിൽ(!chunk)
      തിരികെ ശൂന്യ;
    chunk->dlen = q->chunk_size;
    ++q->chunk_എണ്ണം;
    തിരികെ chunk;
  }
}

സ്ഥിതം ശൂന്യം prune_head(ഘടന bufq *q)
{
  ഘടന buf_chunk *chunk;

  ആകുമ്പോൾ(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    എങ്കിൽ(q->tail == chunk)
      q->tail = q->head;
    എങ്കിൽ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_എണ്ണം;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((q->chunk_എണ്ണം > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_എണ്ണം;
    }
    അല്ലെങ്കിൽ {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

സ്ഥിതം ഘടന buf_chunk *get_non_full_tail(ഘടന bufq *q)
{
  ഘടന buf_chunk *chunk;

  എങ്കിൽ(q->tail && !chunk_is_full(q->tail))
    തിരികെ q->tail;
  chunk = get_spare(q);
  എങ്കിൽ(chunk) {
    /* new tail, and possibly new head */
    എങ്കിൽ(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    അല്ലെങ്കിൽ {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  തിരികെ chunk;
}

CURLcode Curl_bufq_write(ഘടന bufq *q,
                         സ്ഥിരം uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ഘടന buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  ആകുമ്പോൾ(len) {
    tail = get_non_full_tail(q);
    എങ്കിൽ(!tail) {
      എങ്കിൽ((q->chunk_എണ്ണം < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        തിരികെ CURLE_OUT_OF_MEMORY;
      നിർത്തുക;
    }
    n = chunk_append(tail, buf, len);
    എങ്കിൽ(!n)
      നിർത്തുക;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  തിരികെ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ഘടന bufq *q,
                          സ്ഥിരം അക്ഷരം *buf, size_t len,
                          size_t *pnwritten)
{
  തിരികെ Curl_bufq_write(q, (സ്ഥിരം uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ഘടന bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  ആകുമ്പോൾ(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    എങ്കിൽ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  തിരികെ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ഘടന bufq *q, അക്ഷരം *buf, size_t len,
                         size_t *pnread)
{
  തിരികെ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ഘടന bufq *q,
                    സ്ഥിരം uint8_t **pbuf, size_t *plen)
{
  എങ്കിൽ(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  എങ്കിൽ(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    തിരികെ TRUE;
  }
  *pbuf = ശൂന്യ;
  *plen = 0;
  തിരികെ FALSE;
}

bool Curl_bufq_peek_at(ഘടന bufq *q, size_t offset,
                       സ്ഥിരം uint8_t **pbuf, size_t *plen)
{
  ഘടന buf_chunk *c = q->head;
  size_t clen;

  ആകുമ്പോൾ(c) {
    clen = chunk_len(c);
    എങ്കിൽ(!clen)
      നിർത്തുക;
    എങ്കിൽ(offset >= clen) {
      offset -= clen;
      c = c->next;
      തുടരുക;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    തിരികെ TRUE;
  }
  *pbuf = ശൂന്യ;
  *plen = 0;
  തിരികെ FALSE;
}

ശൂന്യം Curl_bufq_skip(ഘടന bufq *q, size_t amount)
{
  size_t n;

  ആകുമ്പോൾ(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(ഘടന bufq *q, Curl_bufq_writer *writer,
                        ശൂന്യം *writer_ctx, size_t *pwritten)
{
  സ്ഥിരം uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  ആകുമ്പോൾ(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    എങ്കിൽ(result) {
      എങ്കിൽ((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      നിർത്തുക;
    }
    എങ്കിൽ(!chunk_written) {
      എങ്കിൽ(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      നിർത്തുക;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  തിരികെ result;
}

CURLcode Curl_bufq_write_pass(ഘടന bufq *q,
                              സ്ഥിരം uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ശൂന്യം *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  ആകുമ്പോൾ(len) {
    എങ്കിൽ(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      എങ്കിൽ(result) {
        എങ്കിൽ(result != CURLE_AGAIN) {
          /* real error, fail */
          തിരികെ result;
        }
        /* would block, bufq is full, give up */
        നിർത്തുക;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_AGAIN)
        /* real error, fail */
        തിരികെ result;
      /* result == CURLE_AGAIN */
      എങ്കിൽ(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      തിരികെ result;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      നിർത്തുക;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  തിരികെ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ഘടന bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ശൂന്യം *reader_ctx,
                        size_t *pnread)
{
  ഘടന buf_chunk *tail = ശൂന്യ;

  *pnread = 0;
  tail = get_non_full_tail(q);
  എങ്കിൽ(!tail) {
    എങ്കിൽ(q->chunk_എണ്ണം < q->max_chunks)
      തിരികെ CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    തിരികെ CURLE_AGAIN;
  }

  തിരികെ chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
സ്ഥിതം CURLcode bufq_slurpn(ഘടന bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, ശൂന്യം *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ആകുമ്പോൾ(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    എങ്കിൽ(result) {
      എങ്കിൽ(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        തിരികെ result;
      }
      result = CURLE_OK;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(n == 0) {
      /* eof, result remains CURLE_OK */
      നിർത്തുക;
    }
    *pnread += n;
    എങ്കിൽ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      എങ്കിൽ(!max_len)
        നിർത്തുക;
    }
    /* give up slurping when we get less bytes than we asked for */
    എങ്കിൽ(q->tail && !chunk_is_full(q->tail))
      നിർത്തുക;
  }
  തിരികെ result;
}

CURLcode Curl_bufq_slurp(ഘടന bufq *q, Curl_bufq_reader *reader,
                         ശൂന്യം *reader_ctx, size_t *pnread)
{
  തിരികെ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
