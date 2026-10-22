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

स्थिर bool chunk_is_empty(अचर संरचना buf_chunk *chunk)
{
  फर्काऊ chunk->r_offset >= chunk->w_offset;
}

स्थिर bool chunk_is_full(अचर संरचना buf_chunk *chunk)
{
  फर्काऊ chunk->w_offset >= chunk->dlen;
}

स्थिर माप_प्रकार chunk_len(अचर संरचना buf_chunk *chunk)
{
  फर्काऊ chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_reset(संरचना buf_chunk *chunk)
{
  chunk->next = रिक्त;
  chunk->r_offset = chunk->w_offset = 0;
}

स्थिर माप_प्रकार chunk_append(संरचना buf_chunk *chunk,
                           अचर uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  माप_प्रकार n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  यदि(n) {
    n = CURLMIN(n, len);
    स_नकल(p, buf, n);
    chunk->w_offset += n;
  }
  फर्काऊ n;
}

स्थिर माप_प्रकार chunk_read(संरचना buf_chunk *chunk,
                         uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  माप_प्रकार n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  यदि(!n) {
    फर्काऊ 0;
  }
  अन्यथा यदि(n <= len) {
    स_नकल(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    फर्काऊ n;
  }
  अन्यथा {
    स_नकल(buf, p, len);
    chunk->r_offset += len;
    फर्काऊ len;
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
  यदि(!n)
    फर्काऊ CURLE_AGAIN;
  यदि(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  यदि(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  फर्काऊ result;
}

स्थिर शून्य chunk_peek(अचर संरचना buf_chunk *chunk,
                       अचर uint8_t **pbuf, माप_प्रकार *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

स्थिर शून्य chunk_peek_at(अचर संरचना buf_chunk *chunk, माप_प्रकार offset,
                          अचर uint8_t **pbuf, माप_प्रकार *plen)
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
  यदि(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    यदि(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  फर्काऊ n;
}

स्थिर शून्य chunk_list_free(संरचना buf_chunk **anchor)
{
  संरचना buf_chunk *chunk;
  जबसम्म(*anchor) {
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
  स_रखो(pool, 0, परिमाण(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

स्थिर CURLcode bufcp_take(संरचना bufc_pool *pool,
                           संरचना buf_chunk **pchunk)
{
  संरचना buf_chunk *chunk = रिक्त;

  यदि(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_गन्ती;
    chunk_reset(chunk);
    *pchunk = chunk;
    फर्काऊ CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  यदि(pool->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
    *pchunk = रिक्त;
    फर्काऊ CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, परिमाण(*chunk) + pool->chunk_size);
  यदि(!chunk) {
    *pchunk = रिक्त;
    फर्काऊ CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  फर्काऊ CURLE_OK;
}

स्थिर शून्य bufcp_put(संरचना bufc_pool *pool,
                      संरचना buf_chunk *chunk)
{
  यदि(pool->spare_गन्ती >= pool->spare_max) {
    curlx_free(chunk);
  }
  अन्यथा {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_गन्ती;
  }
}

शून्य Curl_bufcp_free(संरचना bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_गन्ती = 0;
}

स्थिर शून्य bufq_init(संरचना bufq *q, संरचना bufc_pool *pool,
                      माप_प्रकार chunk_size, माप_प्रकार max_chunks, पूर्णाङ्क opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  स_रखो(q, 0, परिमाण(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

शून्य Curl_bufq_init2(संरचना bufq *q, माप_प्रकार chunk_size, माप_प्रकार max_chunks,
                     पूर्णाङ्क opts)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_init(संरचना bufq *q, माप_प्रकार chunk_size, माप_प्रकार max_chunks)
{
  bufq_init(q, रिक्त, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

शून्य Curl_bufq_initp(संरचना bufq *q, संरचना bufc_pool *pool,
                     माप_प्रकार max_chunks, पूर्णाङ्क opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

शून्य Curl_bufq_free(संरचना bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = रिक्त;
  q->chunk_गन्ती = 0;
}

शून्य Curl_bufq_reset(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;
  जबसम्म(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = रिक्त;
}

माप_प्रकार Curl_bufq_len(अचर संरचना bufq *q)
{
  अचर संरचना buf_chunk *chunk = q->head;
  माप_प्रकार len = 0;
  जबसम्म(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  फर्काऊ len;
}

bool Curl_bufq_is_empty(अचर संरचना bufq *q)
{
  फर्काऊ !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(अचर संरचना bufq *q)
{
  यदि(!q->tail || q->spare)
    फर्काऊ FALSE;
  यदि(q->chunk_गन्ती < q->max_chunks)
    फर्काऊ FALSE;
  यदि(q->chunk_गन्ती > q->max_chunks)
    फर्काऊ TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  फर्काऊ chunk_is_full(q->tail);
}

स्थिर संरचना buf_chunk *get_spare(संरचना bufq *q)
{
  संरचना buf_chunk *chunk = रिक्त;

  यदि(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    फर्काऊ chunk;
  }

  यदि(q->chunk_गन्ती >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    फर्काऊ रिक्त;

  यदि(q->pool) {
    यदि(bufcp_take(q->pool, &chunk))
      फर्काऊ रिक्त;
    ++q->chunk_गन्ती;
    फर्काऊ chunk;
  }
  अन्यथा {
    /* Check for integer overflow before allocation */
    यदि(q->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
      फर्काऊ रिक्त;
    }

    chunk = curlx_calloc(1, परिमाण(*chunk) + q->chunk_size);
    यदि(!chunk)
      फर्काऊ रिक्त;
    chunk->dlen = q->chunk_size;
    ++q->chunk_गन्ती;
    फर्काऊ chunk;
  }
}

स्थिर शून्य prune_head(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  जबसम्म(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    यदि(q->tail == chunk)
      q->tail = q->head;
    यदि(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_गन्ती;
    }
    अन्यथा यदि((q->chunk_गन्ती > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_गन्ती;
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

  यदि(q->tail && !chunk_is_full(q->tail))
    फर्काऊ q->tail;
  chunk = get_spare(q);
  यदि(chunk) {
    /* new tail, and possibly new head */
    यदि(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    अन्यथा {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  फर्काऊ chunk;
}

CURLcode Curl_bufq_write(संरचना bufq *q,
                         अचर uint8_t *buf, माप_प्रकार len,
                         माप_प्रकार *pnwritten)
{
  संरचना buf_chunk *tail;
  माप_प्रकार n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  जबसम्म(len) {
    tail = get_non_full_tail(q);
    यदि(!tail) {
      यदि((q->chunk_गन्ती < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        फर्काऊ CURLE_OUT_OF_MEMORY;
      रोक;
    }
    n = chunk_append(tail, buf, len);
    यदि(!n)
      रोक;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  फर्काऊ (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(संरचना bufq *q,
                          अचर वर्ण *buf, माप_प्रकार len,
                          माप_प्रकार *pnwritten)
{
  फर्काऊ Curl_bufq_write(q, (अचर uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(संरचना bufq *q, uint8_t *buf, माप_प्रकार len,
                        माप_प्रकार *pnread)
{
  *pnread = 0;
  जबसम्म(len && q->head) {
    माप_प्रकार n = chunk_read(q->head, buf, len);
    यदि(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  फर्काऊ (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(संरचना bufq *q, वर्ण *buf, माप_प्रकार len,
                         माप_प्रकार *pnread)
{
  फर्काऊ Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(संरचना bufq *q,
                    अचर uint8_t **pbuf, माप_प्रकार *plen)
{
  यदि(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  यदि(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    फर्काऊ TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  फर्काऊ FALSE;
}

bool Curl_bufq_peek_at(संरचना bufq *q, माप_प्रकार offset,
                       अचर uint8_t **pbuf, माप_प्रकार *plen)
{
  संरचना buf_chunk *c = q->head;
  माप_प्रकार clen;

  जबसम्म(c) {
    clen = chunk_len(c);
    यदि(!clen)
      रोक;
    यदि(offset >= clen) {
      offset -= clen;
      c = c->next;
      जारी;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    फर्काऊ TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  फर्काऊ FALSE;
}

शून्य Curl_bufq_skip(संरचना bufq *q, माप_प्रकार amount)
{
  माप_प्रकार n;

  जबसम्म(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(संरचना bufq *q, Curl_bufq_writer *writer,
                        शून्य *writer_ctx, माप_प्रकार *pwritten)
{
  अचर uint8_t *buf;
  माप_प्रकार blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  जबसम्म(Curl_bufq_peek(q, &buf, &blen)) {
    माप_प्रकार chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    यदि(result) {
      यदि((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      रोक;
    }
    यदि(!chunk_written) {
      यदि(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      रोक;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  फर्काऊ result;
}

CURLcode Curl_bufq_write_pass(संरचना bufq *q,
                              अचर uint8_t *buf, माप_प्रकार len,
                              Curl_bufq_writer *writer, शून्य *writer_ctx,
                              माप_प्रकार *pwritten)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार n;

  *pwritten = 0;
  जबसम्म(len) {
    यदि(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      यदि(result) {
        यदि(result != CURLE_AGAIN) {
          /* real error, fail */
          फर्काऊ result;
        }
        /* would block, bufq is full, give up */
        रोक;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    यदि(result) {
      यदि(result != CURLE_AGAIN)
        /* real error, fail */
        फर्काऊ result;
      /* result == CURLE_AGAIN */
      यदि(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      फर्काऊ result;
    }
    अन्यथा यदि(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      रोक;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  फर्काऊ (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(संरचना bufq *q, माप_प्रकार max_len,
                        Curl_bufq_reader *reader, शून्य *reader_ctx,
                        माप_प्रकार *pnread)
{
  संरचना buf_chunk *tail = रिक्त;

  *pnread = 0;
  tail = get_non_full_tail(q);
  यदि(!tail) {
    यदि(q->chunk_गन्ती < q->max_chunks)
      फर्काऊ CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    फर्काऊ CURLE_AGAIN;
  }

  फर्काऊ chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
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
  जबसम्म(1) {
    माप_प्रकार n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    यदि(result) {
      यदि(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        फर्काऊ result;
      }
      result = CURLE_OK;
      रोक;
    }
    अन्यथा यदि(n == 0) {
      /* eof, result remains CURLE_OK */
      रोक;
    }
    *pnread += n;
    यदि(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      यदि(!max_len)
        रोक;
    }
    /* give up slurping when we get less bytes than we asked for */
    यदि(q->tail && !chunk_is_full(q->tail))
      रोक;
  }
  फर्काऊ result;
}

CURLcode Curl_bufq_slurp(संरचना bufq *q, Curl_bufq_reader *reader,
                         शून्य *reader_ctx, माप_प्रकार *pnread)
{
  फर्काऊ bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
