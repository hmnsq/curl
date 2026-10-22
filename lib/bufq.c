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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "bufq.h"

સ્થિર bool chunk_is_empty(અચલ સંરચના buf_chunk *chunk)
{
  પાછા chunk->r_offset >= chunk->w_offset;
}

સ્થિર bool chunk_is_full(અચલ સંરચના buf_chunk *chunk)
{
  પાછા chunk->w_offset >= chunk->dlen;
}

સ્થિર size_t chunk_len(અચલ સંરચના buf_chunk *chunk)
{
  પાછા chunk->w_offset - chunk->r_offset;
}

સ્થિર શૂન્ય chunk_reset(સંરચના buf_chunk *chunk)
{
  chunk->next = ખાલી;
  chunk->r_offset = chunk->w_offset = 0;
}

સ્થિર size_t chunk_append(સંરચના buf_chunk *chunk,
                           અચલ uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  જો(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  પાછા n;
}

સ્થિર size_t chunk_read(સંરચના buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.data[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  જો(!n) {
    પાછા 0;
  }
  નહીંતર જો(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    પાછા n;
  }
  નહીંતર {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    પાછા len;
  }
}

સ્થિર CURLcode chunk_slurpn(સંરચના buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             શૂન્ય *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.data[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode result;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  જો(!n)
    પાછા CURLE_AGAIN;
  જો(max_len && n > max_len)
    n = max_len;
  result = reader(reader_ctx, p, n, pnread);
  જો(!result) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  પાછા result;
}

સ્થિર શૂન્ય chunk_peek(અચલ સંરચના buf_chunk *chunk,
                       અચલ uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.data[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

સ્થિર શૂન્ય chunk_peek_at(અચલ સંરચના buf_chunk *chunk, size_t offset,
                          અચલ uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.data[offset];
  *plen = chunk->w_offset - offset;
}

સ્થિર size_t chunk_skip(સંરચના buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  જો(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    જો(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  પાછા n;
}

સ્થિર શૂન્ય chunk_list_free(સંરચના buf_chunk **anchor)
{
  સંરચના buf_chunk *chunk;
  જ્યારે(*anchor) {
    chunk = *anchor;
    *anchor = chunk->next;
    curlx_free(chunk);
  }
}

શૂન્ય Curl_bufcp_init(સંરચના bufc_pool *pool,
                     size_t chunk_size, size_t spare_max)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, કદ(*pool));
  pool->chunk_size = chunk_size;
  pool->spare_max = spare_max;
}

સ્થિર CURLcode bufcp_take(સંરચના bufc_pool *pool,
                           સંરચના buf_chunk **pchunk)
{
  સંરચના buf_chunk *chunk = ખાલી;

  જો(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->next;
    --pool->spare_ગણતરી;
    chunk_reset(chunk);
    *pchunk = chunk;
    પાછા CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  જો(pool->chunk_size > SIZE_MAX - કદ(*chunk)) {
    *pchunk = ખાલી;
    પાછા CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, કદ(*chunk) + pool->chunk_size);
  જો(!chunk) {
    *pchunk = ખાલી;
    પાછા CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_size;
  *pchunk = chunk;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય bufcp_put(સંરચના bufc_pool *pool,
                      સંરચના buf_chunk *chunk)
{
  જો(pool->spare_ગણતરી >= pool->spare_max) {
    curlx_free(chunk);
  }
  નહીંતર {
    chunk_reset(chunk);
    chunk->next = pool->spare;
    pool->spare = chunk;
    ++pool->spare_ગણતરી;
  }
}

શૂન્ય Curl_bufcp_free(સંરચના bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_ગણતરી = 0;
}

સ્થિર શૂન્ય bufq_init(સંરચના bufq *q, સંરચના bufc_pool *pool,
                      size_t chunk_size, size_t max_chunks, પૂર્ણાંક opts)
{
  DEBUGASSERT(chunk_size > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, કદ(*q));
  q->chunk_size = chunk_size;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

શૂન્ય Curl_bufq_init2(સંરચના bufq *q, size_t chunk_size, size_t max_chunks,
                     પૂર્ણાંક opts)
{
  bufq_init(q, ખાલી, chunk_size, max_chunks, opts);
}

શૂન્ય Curl_bufq_init(સંરચના bufq *q, size_t chunk_size, size_t max_chunks)
{
  bufq_init(q, ખાલી, chunk_size, max_chunks, BUFQ_OPT_NONE);
}

શૂન્ય Curl_bufq_initp(સંરચના bufq *q, સંરચના bufc_pool *pool,
                     size_t max_chunks, પૂર્ણાંક opts)
{
  bufq_init(q, pool, pool->chunk_size, max_chunks, opts);
}

શૂન્ય Curl_bufq_free(સંરચના bufq *q)
{
  chunk_list_free(&q->head);
  chunk_list_free(&q->spare);
  q->tail = ખાલી;
  q->chunk_ગણતરી = 0;
}

શૂન્ય Curl_bufq_reset(સંરચના bufq *q)
{
  સંરચના buf_chunk *chunk;
  જ્યારે(q->head) {
    chunk = q->head;
    q->head = chunk->next;
    chunk->next = q->spare;
    q->spare = chunk;
  }
  q->tail = ખાલી;
}

size_t Curl_bufq_len(અચલ સંરચના bufq *q)
{
  અચલ સંરચના buf_chunk *chunk = q->head;
  size_t len = 0;
  જ્યારે(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->next;
  }
  પાછા len;
}

bool Curl_bufq_is_empty(અચલ સંરચના bufq *q)
{
  પાછા !q->head || chunk_is_empty(q->head);
}

bool Curl_bufq_is_full(અચલ સંરચના bufq *q)
{
  જો(!q->tail || q->spare)
    પાછા FALSE;
  જો(q->chunk_ગણતરી < q->max_chunks)
    પાછા FALSE;
  જો(q->chunk_ગણતરી > q->max_chunks)
    પાછા TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  પાછા chunk_is_full(q->tail);
}

સ્થિર સંરચના buf_chunk *get_spare(સંરચના bufq *q)
{
  સંરચના buf_chunk *chunk = ખાલી;

  જો(q->spare) {
    chunk = q->spare;
    q->spare = chunk->next;
    chunk_reset(chunk);
    પાછા chunk;
  }

  જો(q->chunk_ગણતરી >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    પાછા ખાલી;

  જો(q->pool) {
    જો(bufcp_take(q->pool, &chunk))
      પાછા ખાલી;
    ++q->chunk_ગણતરી;
    પાછા chunk;
  }
  નહીંતર {
    /* Check for integer overflow before allocation */
    જો(q->chunk_size > SIZE_MAX - કદ(*chunk)) {
      પાછા ખાલી;
    }

    chunk = curlx_calloc(1, કદ(*chunk) + q->chunk_size);
    જો(!chunk)
      પાછા ખાલી;
    chunk->dlen = q->chunk_size;
    ++q->chunk_ગણતરી;
    પાછા chunk;
  }
}

સ્થિર શૂન્ય prune_head(સંરચના bufq *q)
{
  સંરચના buf_chunk *chunk;

  જ્યારે(q->head && chunk_is_empty(q->head)) {
    chunk = q->head;
    q->head = chunk->next;
    જો(q->tail == chunk)
      q->tail = q->head;
    જો(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_ગણતરી;
    }
    નહીંતર જો((q->chunk_ગણતરી > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_ગણતરી;
    }
    નહીંતર {
      chunk->next = q->spare;
      q->spare = chunk;
    }
  }
}

સ્થિર સંરચના buf_chunk *get_non_full_tail(સંરચના bufq *q)
{
  સંરચના buf_chunk *chunk;

  જો(q->tail && !chunk_is_full(q->tail))
    પાછા q->tail;
  chunk = get_spare(q);
  જો(chunk) {
    /* new tail, and possibly new head */
    જો(q->tail) {
      q->tail->next = chunk;
      q->tail = chunk;
    }
    નહીંતર {
      DEBUGASSERT(!q->head);
      q->head = q->tail = chunk;
    }
  }
  પાછા chunk;
}

CURLcode Curl_bufq_write(સંરચના bufq *q,
                         અચલ uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  સંરચના buf_chunk *tail;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  જ્યારે(len) {
    tail = get_non_full_tail(q);
    જો(!tail) {
      જો((q->chunk_ગણતરી < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        પાછા CURLE_OUT_OF_MEMORY;
      તોડો;
    }
    n = chunk_append(tail, buf, len);
    જો(!n)
      તોડો;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  પાછા (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(સંરચના bufq *q,
                          અચલ અક્ષર *buf, size_t len,
                          size_t *pnwritten)
{
  પાછા Curl_bufq_write(q, (અચલ uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(સંરચના bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  જ્યારે(len && q->head) {
    size_t n = chunk_read(q->head, buf, len);
    જો(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_head(q);
  }
  પાછા (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(સંરચના bufq *q, અક્ષર *buf, size_t len,
                         size_t *pnread)
{
  પાછા Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(સંરચના bufq *q,
                    અચલ uint8_t **pbuf, size_t *plen)
{
  જો(q->head && chunk_is_empty(q->head)) {
    prune_head(q);
  }
  જો(q->head && !chunk_is_empty(q->head)) {
    chunk_peek(q->head, pbuf, plen);
    પાછા TRUE;
  }
  *pbuf = ખાલી;
  *plen = 0;
  પાછા FALSE;
}

bool Curl_bufq_peek_at(સંરચના bufq *q, size_t offset,
                       અચલ uint8_t **pbuf, size_t *plen)
{
  સંરચના buf_chunk *c = q->head;
  size_t clen;

  જ્યારે(c) {
    clen = chunk_len(c);
    જો(!clen)
      તોડો;
    જો(offset >= clen) {
      offset -= clen;
      c = c->next;
      ચાલુ;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    પાછા TRUE;
  }
  *pbuf = ખાલી;
  *plen = 0;
  પાછા FALSE;
}

શૂન્ય Curl_bufq_skip(સંરચના bufq *q, size_t amount)
{
  size_t n;

  જ્યારે(amount && q->head) {
    n = chunk_skip(q->head, amount);
    amount -= n;
    prune_head(q);
  }
}

CURLcode Curl_bufq_pass(સંરચના bufq *q, Curl_bufq_writer *writer,
                        શૂન્ય *writer_ctx, size_t *pwritten)
{
  અચલ uint8_t *buf;
  size_t blen;
  CURLcode result = CURLE_OK;

  *pwritten = 0;
  જ્યારે(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    result = writer(writer_ctx, buf, blen, &chunk_written);
    જો(result) {
      જો((result == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        result = CURLE_OK;
      }
      તોડો;
    }
    જો(!chunk_written) {
      જો(!*pwritten) {
        /* treat as blocked */
        result = CURLE_AGAIN;
      }
      તોડો;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  પાછા result;
}

CURLcode Curl_bufq_write_pass(સંરચના bufq *q,
                              અચલ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, શૂન્ય *writer_ctx,
                              size_t *pwritten)
{
  CURLcode result = CURLE_OK;
  size_t n;

  *pwritten = 0;
  જ્યારે(len) {
    જો(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      result = Curl_bufq_pass(q, writer, writer_ctx, &n);
      જો(result) {
        જો(result != CURLE_AGAIN) {
          /* real error, fail */
          પાછા result;
        }
        /* would block, bufq is full, give up */
        તોડો;
      }
    }

    /* Add to bufq as much as there is room for */
    result = Curl_bufq_write(q, buf, len, &n);
    જો(result) {
      જો(result != CURLE_AGAIN)
        /* real error, fail */
        પાછા result;
      /* result == CURLE_AGAIN */
      જો(*pwritten)
        /* we did write successfully before */
        result = CURLE_OK;
      પાછા result;
    }
    નહીંતર જો(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      તોડો;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  પાછા (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(સંરચના bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, શૂન્ય *reader_ctx,
                        size_t *pnread)
{
  સંરચના buf_chunk *tail = ખાલી;

  *pnread = 0;
  tail = get_non_full_tail(q);
  જો(!tail) {
    જો(q->chunk_ગણતરી < q->max_chunks)
      પાછા CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    પાછા CURLE_AGAIN;
  }

  પાછા chunk_slurpn(tail, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
સ્થિર CURLcode bufq_slurpn(સંરચના bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, શૂન્ય *reader_ctx,
                            size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  જ્યારે(1) {
    size_t n;
    result = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    જો(result) {
      જો(!*pnread || result != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        પાછા result;
      }
      result = CURLE_OK;
      તોડો;
    }
    નહીંતર જો(n == 0) {
      /* eof, result remains CURLE_OK */
      તોડો;
    }
    *pnread += n;
    જો(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      જો(!max_len)
        તોડો;
    }
    /* give up slurping when we get less bytes than we asked for */
    જો(q->tail && !chunk_is_full(q->tail))
      તોડો;
  }
  પાછા result;
}

CURLcode Curl_bufq_slurp(સંરચના bufq *q, Curl_bufq_reader *reader,
                         શૂન્ય *reader_ctx, size_t *pnread)
{
  પાછા bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
