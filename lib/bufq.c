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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "bufq.h"

স্থির bool chunk_is_empty(ধ্রুবক কাঠামো buf_chunk *chunk)
{
  ফেরত chunk->r_offset >= chunk->w_offset;
}

স্থির bool chunk_is_full(ধ্রুবক কাঠামো buf_chunk *chunk)
{
  ফেরত chunk->w_offset >= chunk->dlen;
}

স্থির size_t chunk_len(ধ্রুবক কাঠামো buf_chunk *chunk)
{
  ফেরত chunk->w_offset - chunk->r_offset;
}

স্থির শূন্য chunk_reset(কাঠামো buf_chunk *chunk)
{
  chunk->পরবর্তী = ফাঁকা;
  chunk->r_offset = chunk->w_offset = 0;
}

স্থির size_t chunk_append(কাঠামো buf_chunk *chunk,
                           ধ্রুবক uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.তথ্য[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  যদি(n) {
    n = CURLMIN(n, len);
    memcpy(p, buf, n);
    chunk->w_offset += n;
  }
  ফেরত n;
}

স্থির size_t chunk_read(কাঠামো buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.তথ্য[chunk->r_offset];
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  যদি(!n) {
    ফেরত 0;
  }
  অন্যথা যদি(n <= len) {
    memcpy(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    ফেরত n;
  }
  অন্যথা {
    memcpy(buf, p, len);
    chunk->r_offset += len;
    ফেরত len;
  }
}

স্থির CURLcode chunk_slurpn(কাঠামো buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             শূন্য *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.তথ্য[chunk->w_offset];
  size_t n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode ফলাফল;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  যদি(!n)
    ফেরত CURLE_AGAIN;
  যদি(max_len && n > max_len)
    n = max_len;
  ফলাফল = reader(reader_ctx, p, n, pnread);
  যদি(!ফলাফল) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  ফেরত ফলাফল;
}

স্থির শূন্য chunk_peek(ধ্রুবক কাঠামো buf_chunk *chunk,
                       ধ্রুবক uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.তথ্য[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

স্থির শূন্য chunk_peek_at(ধ্রুবক কাঠামো buf_chunk *chunk, size_t offset,
                          ধ্রুবক uint8_t **pbuf, size_t *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.তথ্য[offset];
  *plen = chunk->w_offset - offset;
}

স্থির size_t chunk_skip(কাঠামো buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  যদি(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    যদি(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  ফেরত n;
}

স্থির শূন্য chunk_list_free(কাঠামো buf_chunk **anchor)
{
  কাঠামো buf_chunk *chunk;
  যখন(*anchor) {
    chunk = *anchor;
    *anchor = chunk->পরবর্তী;
    curlx_free(chunk);
  }
}

শূন্য Curl_bufcp_init(কাঠামো bufc_pool *pool,
                     size_t chunk_আকার, size_t spare_max)
{
  DEBUGASSERT(chunk_আকার > 0);
  DEBUGASSERT(spare_max > 0);
  memset(pool, 0, আকার(*pool));
  pool->chunk_আকার = chunk_আকার;
  pool->spare_max = spare_max;
}

স্থির CURLcode bufcp_take(কাঠামো bufc_pool *pool,
                           কাঠামো buf_chunk **pchunk)
{
  কাঠামো buf_chunk *chunk = ফাঁকা;

  যদি(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->পরবর্তী;
    --pool->spare_গুনতি;
    chunk_reset(chunk);
    *pchunk = chunk;
    ফেরত CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  যদি(pool->chunk_আকার > SIZE_MAX - আকার(*chunk)) {
    *pchunk = ফাঁকা;
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, আকার(*chunk) + pool->chunk_আকার);
  যদি(!chunk) {
    *pchunk = ফাঁকা;
    ফেরত CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_আকার;
  *pchunk = chunk;
  ফেরত CURLE_OK;
}

স্থির শূন্য bufcp_put(কাঠামো bufc_pool *pool,
                      কাঠামো buf_chunk *chunk)
{
  যদি(pool->spare_গুনতি >= pool->spare_max) {
    curlx_free(chunk);
  }
  অন্যথা {
    chunk_reset(chunk);
    chunk->পরবর্তী = pool->spare;
    pool->spare = chunk;
    ++pool->spare_গুনতি;
  }
}

শূন্য Curl_bufcp_free(কাঠামো bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_গুনতি = 0;
}

স্থির শূন্য bufq_init(কাঠামো bufq *q, কাঠামো bufc_pool *pool,
                      size_t chunk_আকার, size_t max_chunks, পূর্ণসংখ্যা opts)
{
  DEBUGASSERT(chunk_আকার > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, আকার(*q));
  q->chunk_আকার = chunk_আকার;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

শূন্য Curl_bufq_init2(কাঠামো bufq *q, size_t chunk_আকার, size_t max_chunks,
                     পূর্ণসংখ্যা opts)
{
  bufq_init(q, ফাঁকা, chunk_আকার, max_chunks, opts);
}

শূন্য Curl_bufq_init(কাঠামো bufq *q, size_t chunk_আকার, size_t max_chunks)
{
  bufq_init(q, ফাঁকা, chunk_আকার, max_chunks, BUFQ_OPT_NONE);
}

শূন্য Curl_bufq_initp(কাঠামো bufq *q, কাঠামো bufc_pool *pool,
                     size_t max_chunks, পূর্ণসংখ্যা opts)
{
  bufq_init(q, pool, pool->chunk_আকার, max_chunks, opts);
}

শূন্য Curl_bufq_free(কাঠামো bufq *q)
{
  chunk_list_free(&q->শীর্ষ);
  chunk_list_free(&q->spare);
  q->লেজ = ফাঁকা;
  q->chunk_গুনতি = 0;
}

শূন্য Curl_bufq_reset(কাঠামো bufq *q)
{
  কাঠামো buf_chunk *chunk;
  যখন(q->শীর্ষ) {
    chunk = q->শীর্ষ;
    q->শীর্ষ = chunk->পরবর্তী;
    chunk->পরবর্তী = q->spare;
    q->spare = chunk;
  }
  q->লেজ = ফাঁকা;
}

size_t Curl_bufq_len(ধ্রুবক কাঠামো bufq *q)
{
  ধ্রুবক কাঠামো buf_chunk *chunk = q->শীর্ষ;
  size_t len = 0;
  যখন(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->পরবর্তী;
  }
  ফেরত len;
}

bool Curl_bufq_is_empty(ধ্রুবক কাঠামো bufq *q)
{
  ফেরত !q->শীর্ষ || chunk_is_empty(q->শীর্ষ);
}

bool Curl_bufq_is_full(ধ্রুবক কাঠামো bufq *q)
{
  যদি(!q->লেজ || q->spare)
    ফেরত FALSE;
  যদি(q->chunk_গুনতি < q->max_chunks)
    ফেরত FALSE;
  যদি(q->chunk_গুনতি > q->max_chunks)
    ফেরত TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  ফেরত chunk_is_full(q->লেজ);
}

স্থির কাঠামো buf_chunk *get_spare(কাঠামো bufq *q)
{
  কাঠামো buf_chunk *chunk = ফাঁকা;

  যদি(q->spare) {
    chunk = q->spare;
    q->spare = chunk->পরবর্তী;
    chunk_reset(chunk);
    ফেরত chunk;
  }

  যদি(q->chunk_গুনতি >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    ফেরত ফাঁকা;

  যদি(q->pool) {
    যদি(bufcp_take(q->pool, &chunk))
      ফেরত ফাঁকা;
    ++q->chunk_গুনতি;
    ফেরত chunk;
  }
  অন্যথা {
    /* Check for integer overflow before allocation */
    যদি(q->chunk_আকার > SIZE_MAX - আকার(*chunk)) {
      ফেরত ফাঁকা;
    }

    chunk = curlx_calloc(1, আকার(*chunk) + q->chunk_আকার);
    যদি(!chunk)
      ফেরত ফাঁকা;
    chunk->dlen = q->chunk_আকার;
    ++q->chunk_গুনতি;
    ফেরত chunk;
  }
}

স্থির শূন্য prune_শীর্ষ(কাঠামো bufq *q)
{
  কাঠামো buf_chunk *chunk;

  যখন(q->শীর্ষ && chunk_is_empty(q->শীর্ষ)) {
    chunk = q->শীর্ষ;
    q->শীর্ষ = chunk->পরবর্তী;
    যদি(q->লেজ == chunk)
      q->লেজ = q->শীর্ষ;
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
      chunk->পরবর্তী = q->spare;
      q->spare = chunk;
    }
  }
}

স্থির কাঠামো buf_chunk *get_non_full_লেজ(কাঠামো bufq *q)
{
  কাঠামো buf_chunk *chunk;

  যদি(q->লেজ && !chunk_is_full(q->লেজ))
    ফেরত q->লেজ;
  chunk = get_spare(q);
  যদি(chunk) {
    /* new tail, and possibly new head */
    যদি(q->লেজ) {
      q->লেজ->পরবর্তী = chunk;
      q->লেজ = chunk;
    }
    অন্যথা {
      DEBUGASSERT(!q->শীর্ষ);
      q->শীর্ষ = q->লেজ = chunk;
    }
  }
  ফেরত chunk;
}

CURLcode Curl_bufq_write(কাঠামো bufq *q,
                         ধ্রুবক uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  কাঠামো buf_chunk *লেজ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  যখন(len) {
    লেজ = get_non_full_লেজ(q);
    যদি(!লেজ) {
      যদি((q->chunk_গুনতি < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        ফেরত CURLE_OUT_OF_MEMORY;
      বিরতি;
    }
    n = chunk_append(লেজ, buf, len);
    যদি(!n)
      বিরতি;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  ফেরত (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(কাঠামো bufq *q,
                          ধ্রুবক অক্ষর *buf, size_t len,
                          size_t *pnwritten)
{
  ফেরত Curl_bufq_write(q, (ধ্রুবক uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(কাঠামো bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  যখন(len && q->শীর্ষ) {
    size_t n = chunk_read(q->শীর্ষ, buf, len);
    যদি(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_শীর্ষ(q);
  }
  ফেরত (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(কাঠামো bufq *q, অক্ষর *buf, size_t len,
                         size_t *pnread)
{
  ফেরত Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(কাঠামো bufq *q,
                    ধ্রুবক uint8_t **pbuf, size_t *plen)
{
  যদি(q->শীর্ষ && chunk_is_empty(q->শীর্ষ)) {
    prune_শীর্ষ(q);
  }
  যদি(q->শীর্ষ && !chunk_is_empty(q->শীর্ষ)) {
    chunk_peek(q->শীর্ষ, pbuf, plen);
    ফেরত TRUE;
  }
  *pbuf = ফাঁকা;
  *plen = 0;
  ফেরত FALSE;
}

bool Curl_bufq_peek_at(কাঠামো bufq *q, size_t offset,
                       ধ্রুবক uint8_t **pbuf, size_t *plen)
{
  কাঠামো buf_chunk *c = q->শীর্ষ;
  size_t clen;

  যখন(c) {
    clen = chunk_len(c);
    যদি(!clen)
      বিরতি;
    যদি(offset >= clen) {
      offset -= clen;
      c = c->পরবর্তী;
      চালু;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    ফেরত TRUE;
  }
  *pbuf = ফাঁকা;
  *plen = 0;
  ফেরত FALSE;
}

শূন্য Curl_bufq_skip(কাঠামো bufq *q, size_t amount)
{
  size_t n;

  যখন(amount && q->শীর্ষ) {
    n = chunk_skip(q->শীর্ষ, amount);
    amount -= n;
    prune_শীর্ষ(q);
  }
}

CURLcode Curl_bufq_pass(কাঠামো bufq *q, Curl_bufq_writer *writer,
                        শূন্য *writer_ctx, size_t *pwritten)
{
  ধ্রুবক uint8_t *buf;
  size_t blen;
  CURLcode ফলাফল = CURLE_OK;

  *pwritten = 0;
  যখন(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    ফলাফল = writer(writer_ctx, buf, blen, &chunk_written);
    যদি(ফলাফল) {
      যদি((ফলাফল == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        ফলাফল = CURLE_OK;
      }
      বিরতি;
    }
    যদি(!chunk_written) {
      যদি(!*pwritten) {
        /* treat as blocked */
        ফলাফল = CURLE_AGAIN;
      }
      বিরতি;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_bufq_write_pass(কাঠামো bufq *q,
                              ধ্রুবক uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, শূন্য *writer_ctx,
                              size_t *pwritten)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t n;

  *pwritten = 0;
  যখন(len) {
    যদি(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      ফলাফল = Curl_bufq_pass(q, writer, writer_ctx, &n);
      যদি(ফলাফল) {
        যদি(ফলাফল != CURLE_AGAIN) {
          /* real error, fail */
          ফেরত ফলাফল;
        }
        /* would block, bufq is full, give up */
        বিরতি;
      }
    }

    /* Add to bufq as much as there is room for */
    ফলাফল = Curl_bufq_write(q, buf, len, &n);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN)
        /* real error, fail */
        ফেরত ফলাফল;
      /* result == CURLE_AGAIN */
      যদি(*pwritten)
        /* we did write successfully before */
        ফলাফল = CURLE_OK;
      ফেরত ফলাফল;
    }
    অন্যথা যদি(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      বিরতি;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  ফেরত (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(কাঠামো bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, শূন্য *reader_ctx,
                        size_t *pnread)
{
  কাঠামো buf_chunk *লেজ = ফাঁকা;

  *pnread = 0;
  লেজ = get_non_full_লেজ(q);
  যদি(!লেজ) {
    যদি(q->chunk_গুনতি < q->max_chunks)
      ফেরত CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    ফেরত CURLE_AGAIN;
  }

  ফেরত chunk_slurpn(লেজ, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
স্থির CURLcode bufq_slurpn(কাঠামো bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, শূন্য *reader_ctx,
                            size_t *pnread)
{
  CURLcode ফলাফল;

  *pnread = 0;
  যখন(1) {
    size_t n;
    ফলাফল = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    যদি(ফলাফল) {
      যদি(!*pnread || ফলাফল != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        ফেরত ফলাফল;
      }
      ফলাফল = CURLE_OK;
      বিরতি;
    }
    অন্যথা যদি(n == 0) {
      /* eof, result remains CURLE_OK */
      বিরতি;
    }
    *pnread += n;
    যদি(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      যদি(!max_len)
        বিরতি;
    }
    /* give up slurping when we get less bytes than we asked for */
    যদি(q->লেজ && !chunk_is_full(q->লেজ))
      বিরতি;
  }
  ফেরত ফলাফল;
}

CURLcode Curl_bufq_slurp(কাঠামো bufq *q, Curl_bufq_reader *reader,
                         শূন্য *reader_ctx, size_t *pnread)
{
  ফেরত bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
