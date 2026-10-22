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

स्थिर bool chunk_is_empty(स्थिरांक संरचना buf_chunk *chunk)
{
  परत chunk->r_offset >= chunk->w_offset;
}

स्थिर bool chunk_is_full(स्थिरांक संरचना buf_chunk *chunk)
{
  परत chunk->w_offset >= chunk->dlen;
}

स्थिर माप_प्रकार chunk_len(स्थिरांक संरचना buf_chunk *chunk)
{
  परत chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_reset(संरचना buf_chunk *chunk)
{
  chunk->next = रिक्त;
  chunk->r_offset = chunk->w_offset = 0;
}

स्थिर माप_प्रकार chunk_append(संरचना buf_chunk *chunk,
                           स्थिरांक uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  माप_प्रकार n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  जर(n) {
    n = CURLMIN(n, len);
    स_नकल(p, buf, n);
    chunk->w_offset += n;
  }
  परत n;
}

स्थिर माप_प्रकार chunk_read(संरचना buf_chunk *chunk,
                         uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  माप_प्रकार n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  जर(!n) {
    परत 0;
  }
  अन्यथा जर(n <= len) {
    स_नकल(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    परत n;
  }
  अन्यथा {
    स_नकल(buf, p, len);
    chunk->r_offset += len;
    परत len;
  }
}

स्थिर CURLcode chunk_slurpn(संरचना buf_chunk *chunk, माप_प्रकार max_len,
                             Curl_bufq_reader *reader,
                             शून्य *reader_ctx, माप_प्रकार *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  माप_प्रकार n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  जर(!n)
    परत CURLE_AGAIN;
  जर(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  जर(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  परत result;
}

स्थिर शून्य chunk_peek(स्थिरांक संरचना buf_chunk *chunk,
                       स्थिरांक uint8_t **pbuf, माप_प्रकार *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_peek_at(स्थिरांक संरचना buf_chunk *chunk, माप_प्रकार offset,
                          स्थिरांक uint8_t **pbuf, माप_प्रकार *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

स्थिर माप_प्रकार chunk_skip(संरचना buf_chunk *chunk, माप_प्रकार amount)
{
  माप_प्रकार n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  जर(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    जर(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  परत n;
}

स्थिर शून्य chunk_list_free(संरचना buf_chunk **anchor)
{
  संरचना buf_chunk *chunk;
  पर्यंत(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

शून्य Curl_bufcp_init(संरचना bufc_pool *pool,
                     माप_प्रकार chunk_size, माप_प्रकार spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  स_रखो(pool, 0, आकार(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

स्थिर CURLcode bufcp_take(संरचना bufc_pool *pool,
                           संरचना buf_chunk **pchunk)
{
  संरचना buf_chunk *chunk = रिक्त;

  जर(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_मोजणी;
    chunk_reset(chunk);
    *pchunk = chunk;
    परत CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  जर(pool->chunk_size > SIZE_MAX - आकार(*chunk)) {
    *pchunk = रिक्त;
    परत CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, आकार(*chunk) + pool->chunk_size);
  जर(!chunk) {
    *pchunk = रिक्त;
    परत CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  परत CURLE_OK;
}

स्थिर शून्य bufcp_put(संरचना bufc_pool *pool,
                      संरचना buf_chunk *chunk)
{
  जर(pool->spare_मोजणी >= pool->spare_max) {
    curlx_free(chunk);
  }
  अन्यथा {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_मोजणी;
  }
}

शून्य Curl_bufcp_free(संरचना bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_मोजणी = 0;
}

स्थिर शून्य bufq_init(संरचना bufq *q, संरचना bufc_pool *pool,
                      माप_प्रकार chunk_size, माप_प्रकार max_chunks, पूर्णांक opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  स_रखो(q, 0, आकार(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

शून्य Curl_bufq_init2(संरचना bufq *q, माप_प्रकार chunk_size, माप_प्रकार max_chunks,
                     पूर्णांक opts)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_init(संरचना bufq *q, माप_प्रकार chunk_size, माप_प्रकार max_chunks)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

शून्य Curl_bufq_initp(संरचना bufq *q, संरचना bufc_pool *pool,
                     माप_प्रकार max_chunks, पूर्णांक opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_free(संरचना bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = रिक्त;
  q->chunk_मोजणी = 0;
}

शून्य Curl_bufq_reset(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;
  पर्यंत(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = रिक्त;
}

माप_प्रकार Curl_bufq_len(स्थिरांक संरचना bufq *q)
{
  स्थिरांक संरचना buf_chunk *chunk = q->head;
  माप_प्रकार len = 0;
  पर्यंत(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  परत len;
}

bool Curl_bufq_is_empty(स्थिरांक संरचना bufq *q)
{
  परत !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(स्थिरांक संरचना bufq *q)
{
  जर(!q->tail || q->spare)
    परत FALSE;
  जर(q->chunk_मोजणी < q->max_chunks)
    परत FALSE;
  जर(q->chunk_मोजणी > q->max_chunks)
    परत TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  परत chunk_is_full(q->tail);
}

स्थिर संरचना buf_chunk *get_spare(संरचना bufq *q)
{
  संरचना buf_chunk *chunk = रिक्त;

  जर(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    परत chunk;
  }

  जर(q->chunk_मोजणी >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    परत रिक्त;

  जर(q->pool) {
    जर(bufcp_take(q->pool, &chunk))
      परत रिक्त;
    ++q->chunk_मोजणी;
    परत chunk;
  }
  अन्यथा {
    /* Check for integer overflow before allocation */
    जर(q->chunk_size > SIZE_MAX - आकार(*chunk)) {
      परत रिक्त;
    }

    chunk = curlx_calloc(1, आकार(*chunk) + q->chunk_size);
    जर(!chunk)
      परत रिक्त;
    chunk->dlen = q->chunk_size;
    ++q->chunk_मोजणी;
    परत chunk;
  }
}

स्थिर शून्य prune_head(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  पर्यंत(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    जर(q->tail == chunk)
      q->tail = q->head;
    जर(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_मोजणी;
    }
    अन्यथा जर((q->chunk_मोजणी > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_मोजणी;
    }
    अन्यथा {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

स्थिर संरचना buf_chunk *get_non_full_tail(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  जर(q->tail && !chunk_is_full(q->tail))
    परत q->tail;
  chunk = get_spare(q);
  जर(chunk) {
    /* new tail, and possibly new head */
    जर(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    अन्यथा {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  परत chunk;
}

CURLcode Curl_bufq_write(संरचना bufq *q,
                         स्थिरांक uint8_t *buf, माप_प्रकार len,
                         माप_प्रकार *pnwritten)
{
  संरचना buf_chunk *tail;
  माप_प्रकार n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  पर्यंत(len) {
    tail = get_non_full_tail(q);
    जर(!tail) {
      जर((q->chunk_मोजणी < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        परत CURLE_OUT_OF_MEMORY;
      थांबा;
    }
    n = chunk_append(tail, buf, len);
    जर(!n)
      थांबा;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  परत (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(संरचना bufq *q,
                          स्थिरांक अक्षर *buf, माप_प्रकार len,
                          माप_प्रकार *pnwritten)
{
  परत Curl_bufq_write(q, (स्थिरांक uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(संरचना bufq *q, uint8_t *buf, माप_प्रकार len,
                        माप_प्रकार *pnread)
{
  *pnread = 0;
  पर्यंत(len && q->head) {
    माप_प्रकार n = chunk_read(q->head, buf, len);
    जर(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  परत (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(संरचना bufq *q, अक्षर *buf, माप_प्रकार len,
                         माप_प्रकार *pnread)
{
  परत Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(संरचना bufq *q,
                    स्थिरांक uint8_t **pbuf, माप_प्रकार *plen)
{
  जर(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  जर(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    परत TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  परत FALSE;
}

bool Curl_bufq_peek_at(संरचना bufq *q, माप_प्रकार offset,
                       स्थिरांक uint8_t **pbuf, माप_प्रकार *plen)
{
  संरचना buf_chunk *c = q->head;
  माप_प्रकार clen;

  पर्यंत(c) {
    clen = chunk_len(c);
    जर(!clen)
      थांबा;
    जर(offset >= clen) {
      offset -= clen;
      c = c->next;
      सुरूठेवा;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    परत TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  परत FALSE;
}

शून्य Curl_bufq_skip(संरचना bufq *q, माप_प्रकार amount)
{
  माप_प्रकार n;

  पर्यंत(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(संरचना bufq *q, Curl_bufq_writer *writer,
                        शून्य *writer_ctx, माप_प्रकार *pwritten)
{
  स्थिरांक uint8_t *buf;
  माप_प्रकार blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  पर्यंत(Curl_bufq_peek(q, &buf, &blen)) {
    माप_प्रकार chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    जर(result) {
      जर((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      थांबा;
    }
    जर(!chunk_written) {
      जर(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      थांबा;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  परत result;
}

CURLcode Curl_bufq_write_pass(संरचना bufq *q,
                              स्थिरांक uint8_t *buf, माप_प्रकार len,
                              Curl_bufq_writer *writer, शून्य *writer_ctx,
                              माप_प्रकार *pwritten)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार n;

  *pwritten = 0;
  पर्यंत(len) {
    जर(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      जर(result) {
        जर(result != CURLE_AGAIN) {
          /* real error, fail */
          परत result;
        }
        /* would block, bufq is full, give up */
        थांबा;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    जर(result) {
      जर(result != CURLE_AGAIN)
        /* real error, fail */
        परत result;
      /* result == CURLE_AGAIN */
      जर(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      परत result;
    }
    अन्यथा जर(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      थांबा;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  परत (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(संरचना bufq *q, माप_प्रकार max_len,
                        Curl_bufq_reader *reader, शून्य *reader_ctx,
                        माप_प्रकार *pnread)
{
  संरचना buf_chunk *tail = रिक्त;

  *pnread = 0;
  tail = get_non_full_tail(q);
  जर(!tail) {
    जर(q->chunk_मोजणी < q->max_chunks)
      परत CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    परत CURLE_AGAIN;
  }

  परत chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
स्थिर CURLcode bufq_slurpn(संरचना bufq *q, माप_प्रकार max_len,
                            Curl_bufq_reader *reader, शून्य *reader_ctx,
                            माप_प्रकार *pnread)
{
  CURLcode result;

  *pnread = 0;
  पर्यंत(1) {
    माप_प्रकार n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    जर(result) {
      जर(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        परत result;
      }
      result = CURLE_OK;
      थांबा;
    }
    अन्यथा जर(n == 0) {
      /* eof, result remains CURLE_OK */
      थांबा;
    }
    *pnread += n;
    जर(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      जर(!max_len)
        थांबा;
    }
    /* give up slurping when we get less bytes than we asked for */
    जर(q->tail && !chunk_is_full(q->tail))
      थांबा;
  }
  परत result;
}

CURLcode Curl_bufq_slurp(संरचना bufq *q, Curl_bufq_reader *reader,
                         शून्य *reader_ctx, माप_प्रकार *pnread)
{
  परत bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
