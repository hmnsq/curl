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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "bufq.h"

स्थिर bool chunk_is_empty(ध्रुव संरचना buf_chunk *chunk)
{
  पटिनिवत्त chunk->r_offset >= chunk->w_offset;
}

स्थिर bool chunk_is_full(ध्रुव संरचना buf_chunk *chunk)
{
  पटिनिवत्त chunk->w_offset >= chunk->dlen;
}

स्थिर size_t chunk_len(ध्रुव संरचना buf_chunk *chunk)
{
  पटिनिवत्त chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_reset(संरचना buf_chunk *chunk)
{
  chunk->next = रिक्त;
  chunk->r_offset = chunk->w_offset = 0;
}

स्थिर size_t chunk_append(संरचना buf_chunk *chunk,
                           ध्रुव uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  सचे(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  पटिनिवत्त n;
}

स्थिर size_t chunk_read(संरचना buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  सचे(!n) {
    पटिनिवत्त 0;
  }
  अञ्ञथा सचे(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    पटिनिवत्त n;
  }
  अञ्ञथा {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    पटिनिवत्त len;
  }
}

स्थिर CURLcode chunk_slurpn(संरचना buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             शून्य *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  सचे(!n)
    पटिनिवत्त CURLE_AGAIN;
  सचे(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  सचे(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  पटिनिवत्त result;
}

स्थिर शून्य chunk_peek(ध्रुव संरचना buf_chunk *chunk,
                       ध्रुव uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_peek_at(ध्रुव संरचना buf_chunk *chunk, size_t offset,
                          ध्रुव uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

स्थिर size_t chunk_skip(संरचना buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  सचे(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    सचे(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  पटिनिवत्त n;
}

स्थिर शून्य chunk_list_free(संरचना buf_chunk **anchor)
{
  संरचना buf_chunk *chunk;
  याव(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

शून्य Curl_bufcp_init(संरचना bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, परिमाण(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

स्थिर CURLcode bufcp_take(संरचना bufc_pool *pool,
                           संरचना buf_chunk **pchunk)
{
  संरचना buf_chunk *chunk = रिक्त;

  सचे(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_गणनम्;
    chunk_reset(chunk);
    *pchunk = chunk;
    पटिनिवत्त CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  सचे(pool->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
    *pchunk = रिक्त;
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, परिमाण(*chunk) + pool->chunk_size);
  सचे(!chunk) {
    *pchunk = रिक्त;
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य bufcp_put(संरचना bufc_pool *pool,
                      संरचना buf_chunk *chunk)
{
  सचे(pool->spare_गणनम् >= pool->spare_max) {
    curlx_free(chunk);
  }
  अञ्ञथा {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_गणनम्;
  }
}

शून्य Curl_bufcp_free(संरचना bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_गणनम् = 0;
}

स्थिर शून्य bufq_init(संरचना bufq *q, संरचना bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, पूर्णाङ्क opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, परिमाण(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

शून्य Curl_bufq_init2(संरचना bufq *q, size_t chunk_size, size_t max_chunks,
                     पूर्णाङ्क opts)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_init(संरचना bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

शून्य Curl_bufq_initp(संरचना bufq *q, संरचना bufc_pool *pool,
                     size_t max_chunks, पूर्णाङ्क opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_free(संरचना bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = रिक्त;
  q->chunk_गणनम् = 0;
}

शून्य Curl_bufq_reset(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;
  याव(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = रिक्त;
}

size_t Curl_bufq_len(ध्रुव संरचना bufq *q)
{
  ध्रुव संरचना buf_chunk *chunk = q->head;
  size_t len = 0;
  याव(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  पटिनिवत्त len;
}

bool Curl_bufq_is_empty(ध्रुव संरचना bufq *q)
{
  पटिनिवत्त !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ध्रुव संरचना bufq *q)
{
  सचे(!q->tail || q->spare)
    पटिनिवत्त FALSE;
  सचे(q->chunk_गणनम् < q->max_chunks)
    पटिनिवत्त FALSE;
  सचे(q->chunk_गणनम् > q->max_chunks)
    पटिनिवत्त TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  पटिनिवत्त chunk_is_full(q->tail);
}

स्थिर संरचना buf_chunk *get_spare(संरचना bufq *q)
{
  संरचना buf_chunk *chunk = रिक्त;

  सचे(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    पटिनिवत्त chunk;
  }

  सचे(q->chunk_गणनम् >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    पटिनिवत्त रिक्त;

  सचे(q->pool) {
    सचे(bufcp_take(q->pool, &chunk))
      पटिनिवत्त रिक्त;
    ++q->chunk_गणनम्;
    पटिनिवत्त chunk;
  }
  अञ्ञथा {
    /* Check for integer overflow before allocation */
    सचे(q->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
      पटिनिवत्त रिक्त;
    }

    chunk = curlx_calloc(1, परिमाण(*chunk) + q->chunk_size);
    सचे(!chunk)
      पटिनिवत्त रिक्त;
    chunk->dlen = q->chunk_size;
    ++q->chunk_गणनम्;
    पटिनिवत्त chunk;
  }
}

स्थिर शून्य prune_head(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  याव(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    सचे(q->tail == chunk)
      q->tail = q->head;
    सचे(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_गणनम्;
    }
    अञ्ञथा सचे((q->chunk_गणनम् > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_गणनम्;
    }
    अञ्ञथा {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

स्थिर संरचना buf_chunk *get_non_full_tail(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  सचे(q->tail && !chunk_is_full(q->tail))
    पटिनिवत्त q->tail;
  chunk = get_spare(q);
  सचे(chunk) {
    /* new tail, and possibly new head */
    सचे(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    अञ्ञथा {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  पटिनिवत्त chunk;
}

CURLcode Curl_bufq_write(संरचना bufq *q,
                         ध्रुव uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  संरचना buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  याव(len) {
    tail = get_non_full_tail(q);
    सचे(!tail) {
      सचे((q->chunk_गणनम् < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      भङ्ग;
    }
    n = chunk_append(tail, buf, len);
    सचे(!n)
      भङ्ग;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  पटिनिवत्त (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(संरचना bufq *q,
                          ध्रुव वर्ण *buf, size_t len,
                          size_t *pnwritten)
{
  पटिनिवत्त Curl_bufq_write(q, (ध्रुव uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(संरचना bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  याव(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    सचे(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  पटिनिवत्त (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(संरचना bufq *q, वर्ण *buf, size_t len,
                         size_t *pnread)
{
  पटिनिवत्त Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(संरचना bufq *q,
                    ध्रुव uint8_t **pbuf, size_t *plen)
{
  सचे(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  सचे(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    पटिनिवत्त TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  पटिनिवत्त FALSE;
}

bool Curl_bufq_peek_at(संरचना bufq *q, size_t offset,
                       ध्रुव uint8_t **pbuf, size_t *plen)
{
  संरचना buf_chunk *c = q->head;
  size_t clen;

  याव(c) {
    clen = chunk_len(c);
    सचे(!clen)
      भङ्ग;
    सचे(offset >= clen) {
      offset -= clen;
      c = c->next;
      अनुवर्त;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    पटिनिवत्त TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  पटिनिवत्त FALSE;
}

शून्य Curl_bufq_skip(संरचना bufq *q, size_t amount)
{
  size_t n;

  याव(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(संरचना bufq *q, Curl_bufq_writer *writer,
                        शून्य *writer_ctx, size_t *pwritten)
{
  ध्रुव uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  याव(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    सचे(result) {
      सचे((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      भङ्ग;
    }
    सचे(!chunk_written) {
      सचे(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      भङ्ग;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  पटिनिवत्त result;
}

CURLcode Curl_bufq_write_pass(संरचना bufq *q,
                              ध्रुव uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, शून्य *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  याव(len) {
    सचे(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      सचे(result) {
        सचे(result != CURLE_AGAIN) {
          /* real error, fail */
          पटिनिवत्त result;
        }
        /* would block, bufq is full, give up */
        भङ्ग;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    सचे(result) {
      सचे(result != CURLE_AGAIN)
        /* real error, fail */
        पटिनिवत्त result;
      /* result == CURLE_AGAIN */
      सचे(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      पटिनिवत्त result;
    }
    अञ्ञथा सचे(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      भङ्ग;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  पटिनिवत्त (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(संरचना bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, शून्य *reader_ctx,
                        size_t *pnread)
{
  संरचना buf_chunk *tail = रिक्त;

  *pnread = 0;
  tail = get_non_full_tail(q);
  सचे(!tail) {
    सचे(q->chunk_गणनम् < q->max_chunks)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    पटिनिवत्त CURLE_AGAIN;
  }

  पटिनिवत्त chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
स्थिर CURLcode bufq_slurpn(संरचना bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, शून्य *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  याव(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    सचे(result) {
      सचे(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        पटिनिवत्त result;
      }
      result = CURLE_OK;
      भङ्ग;
    }
    अञ्ञथा सचे(n == 0) {
      /* eof, result remains CURLE_OK */
      भङ्ग;
    }
    *pnread += n;
    सचे(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      सचे(!max_len)
        भङ्ग;
    }
    /* give up slurping when we get less bytes than we asked for */
    सचे(q->tail && !chunk_is_full(q->tail))
      भङ्ग;
  }
  पटिनिवत्त result;
}

CURLcode Curl_bufq_slurp(संरचना bufq *q, Curl_bufq_reader *reader,
                         शून्य *reader_ctx, size_t *pnread)
{
  पटिनिवत्त bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
