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

ساكن bool chunk_is_تشه(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->r_دننه_واټن >= chunk->w_دننه_واټن;
}

ساكن bool chunk_is_ډک(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_دننه_واټن >= chunk->dlen;
}

ساكن size_t chunk_len(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_دننه_واټن - chunk->r_دننه_واټن;
}

ساكن فراغ chunk_reset(بنية buf_chunk *chunk)
{
  chunk->بل = عدم;
  chunk->r_دننه_واټن = chunk->w_دننه_واټن = 0;
}

ساكن size_t chunk_append(بنية buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.معلومات[chunk->w_دننه_واټن];
  size_t n = chunk->dlen - chunk->w_دننه_واټن;
  DEBUGASSERT(chunk->dlen >= chunk->w_دننه_واټن);
  إذا(n) {
    n = CURLMIN(n, len);
    حافظه_نقل(p, buf, n);
    chunk->w_دننه_واټن += n;
  }
  أرجع n;
}

ساكن size_t chunk_read(بنية buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.معلومات[chunk->r_دننه_واټن];
  size_t n = chunk->w_دننه_واټن - chunk->r_دننه_واټن;
  DEBUGASSERT(chunk->w_دننه_واټن >= chunk->r_دننه_واټن);
  إذا(!n) {
    أرجع 0;
  }
  وإلا إذا(n <= len) {
    حافظه_نقل(buf, p, n);
    chunk->r_دننه_واټن = chunk->w_دننه_واټن = 0;
    أرجع n;
  }
  وإلا {
    حافظه_نقل(buf, p, len);
    chunk->r_دننه_واټن += len;
    أرجع len;
  }
}

ساكن CURLcode chunk_slurpn(بنية buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             فراغ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.معلومات[chunk->w_دننه_واټن];
  size_t n = chunk->dlen - chunk->w_دننه_واټن; /* free amount */
  CURLcode پایله;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_دننه_واټن);
  إذا(!n)
    أرجع CURLE_AGAIN;
  إذا(max_len && n > max_len)
    n = max_len;
  پایله = reader(reader_ctx, p, n, pnread);
  إذا(!پایله) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_دننه_واټن += *pnread;
  }
  أرجع پایله;
}

ساكن فراغ chunk_peek(ثابت بنية buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_دننه_واټن >= chunk->r_دننه_واټن);
  *pbuf = &chunk->x.معلومات[chunk->r_دننه_واټن];
  *plen = chunk->w_دننه_واټن - chunk->r_دننه_واټن;
}

ساكن فراغ chunk_peek_at(ثابت بنية buf_chunk *chunk, size_t دننه_واټن,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  دننه_واټن += chunk->r_دننه_واټن;
  DEBUGASSERT(chunk->w_دننه_واټن >= دننه_واټن);
  *pbuf = &chunk->x.معلومات[دننه_واټن];
  *plen = chunk->w_دننه_واټن - دننه_واټن;
}

ساكن size_t chunk_skip(بنية buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_دننه_واټن - chunk->r_دننه_واټن;
  DEBUGASSERT(chunk->w_دننه_واټن >= chunk->r_دننه_واټن);
  إذا(n) {
    n = CURLMIN(n, amount);
    chunk->r_دننه_واټن += n;
    إذا(chunk->r_دننه_واټن == chunk->w_دننه_واټن)
      chunk->r_دننه_واټن = chunk->w_دننه_واټن = 0;
  }
  أرجع n;
}

ساكن فراغ chunk_list_free(بنية buf_chunk **anchor)
{
  بنية buf_chunk *chunk;
  طالما(*anchor) {
    chunk = *anchor;
    *anchor = chunk->بل;
    curlx_free(chunk);
  }
}

فراغ Curl_bufcp_init(بنية bufc_pool *pool,
                     size_t chunk_اندازه, size_t اضافي_max)
{
  DEBUGASSERT(chunk_اندازه > 0);
  DEBUGASSERT(اضافي_max > 0);
  memset(pool, 0, حجم(*pool));
  pool->chunk_اندازه = chunk_اندازه;
  pool->اضافي_max = اضافي_max;
}

ساكن CURLcode bufcp_take(بنية bufc_pool *pool,
                           بنية buf_chunk **pchunk)
{
  بنية buf_chunk *chunk = عدم;

  إذا(pool->اضافي) {
    chunk = pool->اضافي;
    pool->اضافي = chunk->بل;
    --pool->اضافي_شمېر;
    chunk_reset(chunk);
    *pchunk = chunk;
    أرجع CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  إذا(pool->chunk_اندازه > SIZE_MAX - حجم(*chunk)) {
    *pchunk = عدم;
    أرجع CURLE_بهر_د_حافظه;
  }

  chunk = curlx_calloc(1, حجم(*chunk) + pool->chunk_اندازه);
  إذا(!chunk) {
    *pchunk = عدم;
    أرجع CURLE_بهر_د_حافظه;
  }
  chunk->dlen = pool->chunk_اندازه;
  *pchunk = chunk;
  أرجع CURLE_OK;
}

ساكن فراغ bufcp_put(بنية bufc_pool *pool,
                      بنية buf_chunk *chunk)
{
  إذا(pool->اضافي_شمېر >= pool->اضافي_max) {
    curlx_free(chunk);
  }
  وإلا {
    chunk_reset(chunk);
    chunk->بل = pool->اضافي;
    pool->اضافي = chunk;
    ++pool->اضافي_شمېر;
  }
}

فراغ Curl_bufcp_free(بنية bufc_pool *pool)
{
  chunk_list_free(&pool->اضافي);
  pool->اضافي_شمېر = 0;
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
  chunk_list_free(&q->اضافي);
  q->لکۍ = عدم;
  q->chunk_شمېر = 0;
}

فراغ Curl_bufq_reset(بنية bufq *q)
{
  بنية buf_chunk *chunk;
  طالما(q->سر) {
    chunk = q->سر;
    q->سر = chunk->بل;
    chunk->بل = q->اضافي;
    q->اضافي = chunk;
  }
  q->لکۍ = عدم;
}

size_t Curl_bufq_len(ثابت بنية bufq *q)
{
  ثابت بنية buf_chunk *chunk = q->سر;
  size_t len = 0;
  طالما(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->بل;
  }
  أرجع len;
}

bool Curl_bufq_is_تشه(ثابت بنية bufq *q)
{
  أرجع !q->سر || chunk_is_تشه(q->سر);
}

bool Curl_bufq_is_ډک(ثابت بنية bufq *q)
{
  إذا(!q->لکۍ || q->اضافي)
    أرجع FALSE;
  إذا(q->chunk_شمېر < q->max_chunks)
    أرجع FALSE;
  إذا(q->chunk_شمېر > q->max_chunks)
    أرجع TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  أرجع chunk_is_ډک(q->لکۍ);
}

ساكن بنية buf_chunk *get_اضافي(بنية bufq *q)
{
  بنية buf_chunk *chunk = عدم;

  إذا(q->اضافي) {
    chunk = q->اضافي;
    q->اضافي = chunk->بل;
    chunk_reset(chunk);
    أرجع chunk;
  }

  إذا(q->chunk_شمېر >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_حد)))
    أرجع عدم;

  إذا(q->pool) {
    إذا(bufcp_take(q->pool, &chunk))
      أرجع عدم;
    ++q->chunk_شمېر;
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
    ++q->chunk_شمېر;
    أرجع chunk;
  }
}

ساكن فراغ prune_سر(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  طالما(q->سر && chunk_is_تشه(q->سر)) {
    chunk = q->سر;
    q->سر = chunk->بل;
    إذا(q->لکۍ == chunk)
      q->لکۍ = q->سر;
    إذا(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_شمېر;
    }
    وإلا إذا((q->chunk_شمېر > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_شمېر;
    }
    وإلا {
      chunk->بل = q->اضافي;
      q->اضافي = chunk;
    }
  }
}

ساكن بنية buf_chunk *get_non_ډک_لکۍ(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  إذا(q->لکۍ && !chunk_is_ډک(q->لکۍ))
    أرجع q->لکۍ;
  chunk = get_اضافي(q);
  إذا(chunk) {
    /* new tail, and possibly new head */
    إذا(q->لکۍ) {
      q->لکۍ->بل = chunk;
      q->لکۍ = chunk;
    }
    وإلا {
      DEBUGASSERT(!q->سر);
      q->سر = q->لکۍ = chunk;
    }
  }
  أرجع chunk;
}

CURLcode Curl_bufq_write(بنية bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  بنية buf_chunk *لکۍ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  طالما(len) {
    لکۍ = get_non_ډک_لکۍ(q);
    إذا(!لکۍ) {
      إذا((q->chunk_شمېر < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_حد))
        /* should have gotten a tail, but did not */
        أرجع CURLE_بهر_د_حافظه;
      اكسر;
    }
    n = chunk_append(لکۍ, buf, len);
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
  إذا(q->سر && chunk_is_تشه(q->سر)) {
    prune_سر(q);
  }
  إذا(q->سر && !chunk_is_تشه(q->سر)) {
    chunk_peek(q->سر, pbuf, plen);
    أرجع TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  أرجع FALSE;
}

bool Curl_bufq_peek_at(بنية bufq *q, size_t دننه_واټن,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  بنية buf_chunk *c = q->سر;
  size_t clen;

  طالما(c) {
    clen = chunk_len(c);
    إذا(!clen)
      اكسر;
    إذا(دننه_واټن >= clen) {
      دننه_واټن -= clen;
      c = c->بل;
      استمر;
    }
    chunk_peek_at(c, دننه_واټن, pbuf, plen);
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
  CURLcode پایله = CURLE_OK;

  *pwritten = 0;
  طالما(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    پایله = writer(writer_ctx, buf, blen, &chunk_written);
    إذا(پایله) {
      إذا((پایله == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        پایله = CURLE_OK;
      }
      اكسر;
    }
    إذا(!chunk_written) {
      إذا(!*pwritten) {
        /* treat as blocked */
        پایله = CURLE_AGAIN;
      }
      اكسر;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  أرجع پایله;
}

CURLcode Curl_bufq_write_pass(بنية bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, فراغ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode پایله = CURLE_OK;
  size_t n;

  *pwritten = 0;
  طالما(len) {
    إذا(Curl_bufq_is_ډک(q)) {
      /* try to make room in case we are full */
      پایله = Curl_bufq_pass(q, writer, writer_ctx, &n);
      إذا(پایله) {
        إذا(پایله != CURLE_AGAIN) {
          /* real error, fail */
          أرجع پایله;
        }
        /* would block, bufq is full, give up */
        اكسر;
      }
    }

    /* Add to bufq as much as there is room for */
    پایله = Curl_bufq_write(q, buf, len, &n);
    إذا(پایله) {
      إذا(پایله != CURLE_AGAIN)
        /* real error, fail */
        أرجع پایله;
      /* result == CURLE_AGAIN */
      إذا(*pwritten)
        /* we did write successfully before */
        پایله = CURLE_OK;
      أرجع پایله;
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
  بنية buf_chunk *لکۍ = عدم;

  *pnread = 0;
  لکۍ = get_non_ډک_لکۍ(q);
  إذا(!لکۍ) {
    إذا(q->chunk_شمېر < q->max_chunks)
      أرجع CURLE_بهر_د_حافظه;
    /* full, blocked */
    أرجع CURLE_AGAIN;
  }

  أرجع chunk_slurpn(لکۍ, max_len, reader, reader_ctx, pnread);
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
  CURLcode پایله;

  *pnread = 0;
  طالما(1) {
    size_t n;
    پایله = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    إذا(پایله) {
      إذا(!*pnread || پایله != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        أرجع پایله;
      }
      پایله = CURLE_OK;
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
    إذا(q->لکۍ && !chunk_is_ډک(q->لکۍ))
      اكسر;
  }
  أرجع پایله;
}

CURLcode Curl_bufq_slurp(بنية bufq *q, Curl_bufq_reader *reader,
                         فراغ *reader_ctx, size_t *pnread)
{
  أرجع bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
