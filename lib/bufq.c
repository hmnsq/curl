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
#ضمن "curl_setup.h"

#ضمن "bufq.h"

ساكن bool chunk_is_خالی(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->r_جابه_جایی >= chunk->w_جابه_جایی;
}

ساكن bool chunk_is_پر(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_جابه_جایی >= chunk->dlen;
}

ساكن size_t chunk_len(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_جابه_جایی - chunk->r_جابه_جایی;
}

ساكن فراغ chunk_reset(بنية buf_chunk *chunk)
{
  chunk->بعدی = عدم;
  chunk->r_جابه_جایی = chunk->w_جابه_جایی = 0;
}

ساكن size_t chunk_append(بنية buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.داده[chunk->w_جابه_جایی];
  size_t n = chunk->dlen - chunk->w_جابه_جایی;
  DEBUGASSERT(chunk->dlen >= chunk->w_جابه_جایی);
  إذا(n) {
    n = CURLMIN(n, len);
    کپی_حافظه(p, buf, n);
    chunk->w_جابه_جایی += n;
  }
  أرجع n;
}

ساكن size_t chunk_read(بنية buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.داده[chunk->r_جابه_جایی];
  size_t n = chunk->w_جابه_جایی - chunk->r_جابه_جایی;
  DEBUGASSERT(chunk->w_جابه_جایی >= chunk->r_جابه_جایی);
  إذا(!n) {
    أرجع 0;
  }
  وإلا إذا(n <= len) {
    کپی_حافظه(buf, p, n);
    chunk->r_جابه_جایی = chunk->w_جابه_جایی = 0;
    أرجع n;
  }
  وإلا {
    کپی_حافظه(buf, p, len);
    chunk->r_جابه_جایی += len;
    أرجع len;
  }
}

ساكن CURLcode chunk_slurpn(بنية buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             فراغ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.داده[chunk->w_جابه_جایی];
  size_t n = chunk->dlen - chunk->w_جابه_جایی; /* free amount */
  CURLcode نتیجه;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_جابه_جایی);
  إذا(!n)
    أرجع CURLE_AGAIN;
  إذا(max_len && n > max_len)
    n = max_len;
  نتیجه = reader(reader_ctx, p, n, pnread);
  إذا(!نتیجه) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_جابه_جایی += *pnread;
  }
  أرجع نتیجه;
}

ساكن فراغ chunk_peek(ثابت بنية buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_جابه_جایی >= chunk->r_جابه_جایی);
  *pbuf = &chunk->x.داده[chunk->r_جابه_جایی];
  *plen = chunk->w_جابه_جایی - chunk->r_جابه_جایی;
}

ساكن فراغ chunk_peek_at(ثابت بنية buf_chunk *chunk, size_t جابه_جایی,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  جابه_جایی += chunk->r_جابه_جایی;
  DEBUGASSERT(chunk->w_جابه_جایی >= جابه_جایی);
  *pbuf = &chunk->x.داده[جابه_جایی];
  *plen = chunk->w_جابه_جایی - جابه_جایی;
}

ساكن size_t chunk_skip(بنية buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_جابه_جایی - chunk->r_جابه_جایی;
  DEBUGASSERT(chunk->w_جابه_جایی >= chunk->r_جابه_جایی);
  إذا(n) {
    n = CURLMIN(n, amount);
    chunk->r_جابه_جایی += n;
    إذا(chunk->r_جابه_جایی == chunk->w_جابه_جایی)
      chunk->r_جابه_جایی = chunk->w_جابه_جایی = 0;
  }
  أرجع n;
}

ساكن فراغ chunk_list_free(بنية buf_chunk **anchor)
{
  بنية buf_chunk *chunk;
  طالما(*anchor) {
    chunk = *anchor;
    *anchor = chunk->بعدی;
    curlx_free(chunk);
  }
}

فراغ Curl_bufcp_init(بنية bufc_pool *pool,
                     size_t chunk_اندازه, size_t یدکی_max)
{
  DEBUGASSERT(chunk_اندازه > 0);
  DEBUGASSERT(یدکی_max > 0);
  memset(pool, 0, حجم(*pool));
  pool->chunk_اندازه = chunk_اندازه;
  pool->یدکی_max = یدکی_max;
}

ساكن CURLcode bufcp_take(بنية bufc_pool *pool,
                           بنية buf_chunk **pchunk)
{
  بنية buf_chunk *chunk = عدم;

  إذا(pool->یدکی) {
    chunk = pool->یدکی;
    pool->یدکی = chunk->بعدی;
    --pool->یدکی_شمار;
    chunk_reset(chunk);
    *pchunk = chunk;
    أرجع CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  إذا(pool->chunk_اندازه > SIZE_MAX - حجم(*chunk)) {
    *pchunk = عدم;
    أرجع CURLE_بیرون_از_حافظه;
  }

  chunk = curlx_calloc(1, حجم(*chunk) + pool->chunk_اندازه);
  إذا(!chunk) {
    *pchunk = عدم;
    أرجع CURLE_بیرون_از_حافظه;
  }
  chunk->dlen = pool->chunk_اندازه;
  *pchunk = chunk;
  أرجع CURLE_OK;
}

ساكن فراغ bufcp_put(بنية bufc_pool *pool,
                      بنية buf_chunk *chunk)
{
  إذا(pool->یدکی_شمار >= pool->یدکی_max) {
    curlx_free(chunk);
  }
  وإلا {
    chunk_reset(chunk);
    chunk->بعدی = pool->یدکی;
    pool->یدکی = chunk;
    ++pool->یدکی_شمار;
  }
}

فراغ Curl_bufcp_free(بنية bufc_pool *pool)
{
  chunk_list_free(&pool->یدکی);
  pool->یدکی_شمار = 0;
}

ساكن فراغ bufq_init(بنية bufq *q, بنية bufc_pool *pool,
                      size_t chunk_اندازه, size_t max_chunks, صحيح opts)
{
  DEBUGASSERT(chunk_اندازه > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, حجم(*q));
  q->chunk_اندازه = chunk_اندازه;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

فراغ Curl_bufq_init2(بنية bufq *q, size_t chunk_اندازه, size_t max_chunks,
                     صحيح opts)
{
  bufq_init(q, عدم, chunk_اندازه, max_chunks, opts);
}

فراغ Curl_bufq_init(بنية bufq *q, size_t chunk_اندازه, size_t max_chunks)
{
  bufq_init(q, عدم, chunk_اندازه, max_chunks, BUFQ_OPT_NONE);
}

فراغ Curl_bufq_initp(بنية bufq *q, بنية bufc_pool *pool,
                     size_t max_chunks, صحيح opts)
{
  bufq_init(q, pool, pool->chunk_اندازه, max_chunks, opts);
}

فراغ Curl_bufq_free(بنية bufq *q)
{
  chunk_list_free(&q->سر);
  chunk_list_free(&q->یدکی);
  q->دم = عدم;
  q->chunk_شمار = 0;
}

فراغ Curl_bufq_reset(بنية bufq *q)
{
  بنية buf_chunk *chunk;
  طالما(q->سر) {
    chunk = q->سر;
    q->سر = chunk->بعدی;
    chunk->بعدی = q->یدکی;
    q->یدکی = chunk;
  }
  q->دم = عدم;
}

size_t Curl_bufq_len(ثابت بنية bufq *q)
{
  ثابت بنية buf_chunk *chunk = q->سر;
  size_t len = 0;
  طالما(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->بعدی;
  }
  أرجع len;
}

bool Curl_bufq_is_خالی(ثابت بنية bufq *q)
{
  أرجع !q->سر || chunk_is_خالی(q->سر);
}

bool Curl_bufq_is_پر(ثابت بنية bufq *q)
{
  إذا(!q->دم || q->یدکی)
    أرجع FALSE;
  إذا(q->chunk_شمار < q->max_chunks)
    أرجع FALSE;
  إذا(q->chunk_شمار > q->max_chunks)
    أرجع TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  أرجع chunk_is_پر(q->دم);
}

ساكن بنية buf_chunk *get_یدکی(بنية bufq *q)
{
  بنية buf_chunk *chunk = عدم;

  إذا(q->یدکی) {
    chunk = q->یدکی;
    q->یدکی = chunk->بعدی;
    chunk_reset(chunk);
    أرجع chunk;
  }

  إذا(q->chunk_شمار >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_حد)))
    أرجع عدم;

  إذا(q->pool) {
    إذا(bufcp_take(q->pool, &chunk))
      أرجع عدم;
    ++q->chunk_شمار;
    أرجع chunk;
  }
  وإلا {
    /* Check for integer overflow before allocation */
    إذا(q->chunk_اندازه > SIZE_MAX - حجم(*chunk)) {
      أرجع عدم;
    }

    chunk = curlx_calloc(1, حجم(*chunk) + q->chunk_اندازه);
    إذا(!chunk)
      أرجع عدم;
    chunk->dlen = q->chunk_اندازه;
    ++q->chunk_شمار;
    أرجع chunk;
  }
}

ساكن فراغ prune_سر(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  طالما(q->سر && chunk_is_خالی(q->سر)) {
    chunk = q->سر;
    q->سر = chunk->بعدی;
    إذا(q->دم == chunk)
      q->دم = q->سر;
    إذا(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_شمار;
    }
    وإلا إذا((q->chunk_شمار > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_شمار;
    }
    وإلا {
      chunk->بعدی = q->یدکی;
      q->یدکی = chunk;
    }
  }
}

ساكن بنية buf_chunk *get_non_پر_دم(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  إذا(q->دم && !chunk_is_پر(q->دم))
    أرجع q->دم;
  chunk = get_یدکی(q);
  إذا(chunk) {
    /* new tail, and possibly new head */
    إذا(q->دم) {
      q->دم->بعدی = chunk;
      q->دم = chunk;
    }
    وإلا {
      DEBUGASSERT(!q->سر);
      q->سر = q->دم = chunk;
    }
  }
  أرجع chunk;
}

CURLcode Curl_bufq_write(بنية bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  بنية buf_chunk *دم;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  طالما(len) {
    دم = get_non_پر_دم(q);
    إذا(!دم) {
      إذا((q->chunk_شمار < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_حد))
        /* should have gotten a tail, but did not */
        أرجع CURLE_بیرون_از_حافظه;
      اكسر;
    }
    n = chunk_append(دم, buf, len);
    إذا(!n)
      اكسر;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  أرجع (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(بنية bufq *q,
                          ثابت حرف *buf, size_t len,
                          size_t *pnwritten)
{
  أرجع Curl_bufq_write(q, (ثابت uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(بنية bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  طالما(len && q->سر) {
    size_t n = chunk_read(q->سر, buf, len);
    إذا(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_سر(q);
  }
  أرجع (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(بنية bufq *q, حرف *buf, size_t len,
                         size_t *pnread)
{
  أرجع Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(بنية bufq *q,
                    ثابت uint8_t **pbuf, size_t *plen)
{
  إذا(q->سر && chunk_is_خالی(q->سر)) {
    prune_سر(q);
  }
  إذا(q->سر && !chunk_is_خالی(q->سر)) {
    chunk_peek(q->سر, pbuf, plen);
    أرجع TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  أرجع FALSE;
}

bool Curl_bufq_peek_at(بنية bufq *q, size_t جابه_جایی,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  بنية buf_chunk *c = q->سر;
  size_t clen;

  طالما(c) {
    clen = chunk_len(c);
    إذا(!clen)
      اكسر;
    إذا(جابه_جایی >= clen) {
      جابه_جایی -= clen;
      c = c->بعدی;
      استمر;
    }
    chunk_peek_at(c, جابه_جایی, pbuf, plen);
    أرجع TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  أرجع FALSE;
}

فراغ Curl_bufq_skip(بنية bufq *q, size_t amount)
{
  size_t n;

  طالما(amount && q->سر) {
    n = chunk_skip(q->سر, amount);
    amount -= n;
    prune_سر(q);
  }
}

CURLcode Curl_bufq_pass(بنية bufq *q, Curl_bufq_writer *writer,
                        فراغ *writer_ctx, size_t *pwritten)
{
  ثابت uint8_t *buf;
  size_t blen;
  CURLcode نتیجه = CURLE_OK;

  *pwritten = 0;
  طالما(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    نتیجه = writer(writer_ctx, buf, blen, &chunk_written);
    إذا(نتیجه) {
      إذا((نتیجه == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        نتیجه = CURLE_OK;
      }
      اكسر;
    }
    إذا(!chunk_written) {
      إذا(!*pwritten) {
        /* treat as blocked */
        نتیجه = CURLE_AGAIN;
      }
      اكسر;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  أرجع نتیجه;
}

CURLcode Curl_bufq_write_pass(بنية bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, فراغ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode نتیجه = CURLE_OK;
  size_t n;

  *pwritten = 0;
  طالما(len) {
    إذا(Curl_bufq_is_پر(q)) {
      /* try to make room in case we are full */
      نتیجه = Curl_bufq_pass(q, writer, writer_ctx, &n);
      إذا(نتیجه) {
        إذا(نتیجه != CURLE_AGAIN) {
          /* real error, fail */
          أرجع نتیجه;
        }
        /* would block, bufq is full, give up */
        اكسر;
      }
    }

    /* Add to bufq as much as there is room for */
    نتیجه = Curl_bufq_write(q, buf, len, &n);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN)
        /* real error, fail */
        أرجع نتیجه;
      /* result == CURLE_AGAIN */
      إذا(*pwritten)
        /* we did write successfully before */
        نتیجه = CURLE_OK;
      أرجع نتیجه;
    }
    وإلا إذا(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      اكسر;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  أرجع (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(بنية bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, فراغ *reader_ctx,
                        size_t *pnread)
{
  بنية buf_chunk *دم = عدم;

  *pnread = 0;
  دم = get_non_پر_دم(q);
  إذا(!دم) {
    إذا(q->chunk_شمار < q->max_chunks)
      أرجع CURLE_بیرون_از_حافظه;
    /* full, blocked */
    أرجع CURLE_AGAIN;
  }

  أرجع chunk_slurpn(دم, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ساكن CURLcode bufq_slurpn(بنية bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, فراغ *reader_ctx,
                            size_t *pnread)
{
  CURLcode نتیجه;

  *pnread = 0;
  طالما(1) {
    size_t n;
    نتیجه = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    إذا(نتیجه) {
      إذا(!*pnread || نتیجه != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        أرجع نتیجه;
      }
      نتیجه = CURLE_OK;
      اكسر;
    }
    وإلا إذا(n == 0) {
      /* eof, result remains CURLE_OK */
      اكسر;
    }
    *pnread += n;
    إذا(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      إذا(!max_len)
        اكسر;
    }
    /* give up slurping when we get less bytes than we asked for */
    إذا(q->دم && !chunk_is_پر(q->دم))
      اكسر;
  }
  أرجع نتیجه;
}

CURLcode Curl_bufq_slurp(بنية bufq *q, Curl_bufq_reader *reader,
                         فراغ *reader_ctx, size_t *pnread)
{
  أرجع bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
