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
  प्रत्यागम chunk->r_offset >= chunk->w_offset;
}

स्थिर bool chunk_is_full(ध्रुव संरचना buf_chunk *chunk)
{
  प्रत्यागम chunk->w_offset >= chunk->dlen;
}

स्थिर size_t chunk_len(ध्रुव संरचना buf_chunk *chunk)
{
  प्रत्यागम chunk->w_offset - chunk->r_offset;
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
  यदि(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  प्रत्यागम n;
}

स्थिर size_t chunk_read(संरचना buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  यदि(!n) {
    प्रत्यागम 0;
  }
  अन्यथा यदि(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    प्रत्यागम n;
  }
  अन्यथा {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    प्रत्यागम len;
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
  यदि(!n)
    प्रत्यागम CURLE_AGAIN;
  यदि(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  यदि(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  प्रत्यागम result;
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
  यदि(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    यदि(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  प्रत्यागम n;
}

स्थिर शून्य chunk_list_free(संरचना buf_chunk **anchor)
{
  संरचना buf_chunk *chunk;
  यावत्(*anchor) {
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

  यदि(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_मोजणी;
    chunk_reset(chunk);
    *pchunk = chunk;
    प्रत्यागम CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  यदि(pool->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
    *pchunk = रिक्त;
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, परिमाण(*chunk) + pool->chunk_size);
  यदि(!chunk) {
    *pchunk = रिक्त;
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य bufcp_put(संरचना bufc_pool *pool,
                      संरचना buf_chunk *chunk)
{
  यदि(pool->spare_मोजणी >= pool->spare_max) {
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
  q->chunk_मोजणी = 0;
}

शून्य Curl_bufq_reset(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;
  यावत्(q->head) {
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
  यावत्(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  प्रत्यागम len;
}

bool Curl_bufq_is_empty(ध्रुव संरचना bufq *q)
{
  प्रत्यागम !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(ध्रुव संरचना bufq *q)
{
  यदि(!q->tail || q->spare)
    प्रत्यागम FALSE;
  यदि(q->chunk_मोजणी < q->max_chunks)
    प्रत्यागम FALSE;
  यदि(q->chunk_मोजणी > q->max_chunks)
    प्रत्यागम TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  प्रत्यागम chunk_is_full(q->tail);
}

स्थिर संरचना buf_chunk *get_spare(संरचना bufq *q)
{
  संरचना buf_chunk *chunk = रिक्त;

  यदि(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    प्रत्यागम chunk;
  }

  यदि(q->chunk_मोजणी >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    प्रत्यागम रिक्त;

  यदि(q->pool) {
    यदि(bufcp_take(q->pool, &chunk))
      प्रत्यागम रिक्त;
    ++q->chunk_मोजणी;
    प्रत्यागम chunk;
  }
  अन्यथा {
    /* Check for integer overflow before allocation */
    यदि(q->chunk_size > SIZE_MAX - परिमाण(*chunk)) {
      प्रत्यागम रिक्त;
    }

    chunk = curlx_calloc(1, परिमाण(*chunk) + q->chunk_size);
    यदि(!chunk)
      प्रत्यागम रिक्त;
    chunk->dlen = q->chunk_size;
    ++q->chunk_मोजणी;
    प्रत्यागम chunk;
  }
}

स्थिर शून्य prune_head(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  यावत्(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    यदि(q->tail == chunk)
      q->tail = q->head;
    यदि(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_मोजणी;
    }
    अन्यथा यदि((q->chunk_मोजणी > q->max_chunks) ||
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

  यदि(q->tail && !chunk_is_full(q->tail))
    प्रत्यागम q->tail;
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
  प्रत्यागम chunk;
}

CURLcode Curl_bufq_write(संरचना bufq *q,
                         ध्रुव uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  संरचना buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  यावत्(len) {
    tail = get_non_full_tail(q);
    यदि(!tail) {
      यदि((q->chunk_मोजणी < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      भङ्ग;
    }
    n = chunk_append(tail, buf, len);
    यदि(!n)
      भङ्ग;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  प्रत्यागम (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(संरचना bufq *q,
                          ध्रुव वर्ण *buf, size_t len,
                          size_t *pnwritten)
{
  प्रत्यागम Curl_bufq_write(q, (ध्रुव uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(संरचना bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  यावत्(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    यदि(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  प्रत्यागम (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(संरचना bufq *q, वर्ण *buf, size_t len,
                         size_t *pnread)
{
  प्रत्यागम Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(संरचना bufq *q,
                    ध्रुव uint8_t **pbuf, size_t *plen)
{
  यदि(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  यदि(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    प्रत्यागम TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  प्रत्यागम FALSE;
}

bool Curl_bufq_peek_at(संरचना bufq *q, size_t offset,
                       ध्रुव uint8_t **pbuf, size_t *plen)
{
  संरचना buf_chunk *c = q->head;
  size_t clen;

  यावत्(c) {
    clen = chunk_len(c);
    यदि(!clen)
      भङ्ग;
    यदि(offset >= clen) {
      offset -= clen;
      c = c->next;
      अनुवर्त;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    प्रत्यागम TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  प्रत्यागम FALSE;
}

शून्य Curl_bufq_skip(संरचना bufq *q, size_t amount)
{
  size_t n;

  यावत्(amount && q->head) {
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
  यावत्(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    यदि(result) {
      यदि((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      भङ्ग;
    }
    यदि(!chunk_written) {
      यदि(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      भङ्ग;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  प्रत्यागम result;
}

CURLcode Curl_bufq_write_pass(संरचना bufq *q,
                              ध्रुव uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, शून्य *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  यावत्(len) {
    यदि(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      यदि(result) {
        यदि(result != CURLE_AGAIN) {
          /* real error, fail */
          प्रत्यागम result;
        }
        /* would block, bufq is full, give up */
        भङ्ग;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    यदि(result) {
      यदि(result != CURLE_AGAIN)
        /* real error, fail */
        प्रत्यागम result;
      /* result == CURLE_AGAIN */
      यदि(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      प्रत्यागम result;
    }
    अन्यथा यदि(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      भङ्ग;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  प्रत्यागम (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(संरचना bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, शून्य *reader_ctx,
                        size_t *pnread)
{
  संरचना buf_chunk *tail = रिक्त;

  *pnread = 0;
  tail = get_non_full_tail(q);
  यदि(!tail) {
    यदि(q->chunk_मोजणी < q->max_chunks)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    प्रत्यागम CURLE_AGAIN;
  }

  प्रत्यागम chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
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
  यावत्(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    यदि(result) {
      यदि(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        प्रत्यागम result;
      }
      result = CURLE_OK;
      भङ्ग;
    }
    अन्यथा यदि(n == 0) {
      /* eof, result remains CURLE_OK */
      भङ्ग;
    }
    *pnread += n;
    यदि(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      यदि(!max_len)
        भङ्ग;
    }
    /* give up slurping when we get less bytes than we asked for */
    यदि(q->tail && !chunk_is_full(q->tail))
      भङ्ग;
  }
  प्रत्यागम result;
}

CURLcode Curl_bufq_slurp(संरचना bufq *q, Curl_bufq_reader *reader,
                         शून्य *reader_ctx, size_t *pnread)
{
  प्रत्यागम bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
