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
#شامل "curl_setup.h"

#شامل "bufq.h"

ساکن bool chunk_is_خالی_خانے(ثابت ساخت buf_chunk *chunk)
{
  واپس chunk->r_انحراف >= chunk->w_انحراف;
}

ساکن bool chunk_is_بھرا(ثابت ساخت buf_chunk *chunk)
{
  واپس chunk->w_انحراف >= chunk->dlen;
}

ساکن size_t chunk_len(ثابت ساخت buf_chunk *chunk)
{
  واپس chunk->w_انحراف - chunk->r_انحراف;
}

ساکن خالی chunk_reset(ساخت buf_chunk *chunk)
{
  chunk->اگلا = تہی;
  chunk->r_انحراف = chunk->w_انحراف = 0;
}

ساکن size_t chunk_append(ساخت buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ڈیٹا[chunk->w_انحراف];
  size_t n = chunk->dlen - chunk->w_انحراف;
  DEBUGASSERT(chunk->dlen >= chunk->w_انحراف);
  اگر(n) {
    n = CURLMIN(n, len);
    حافظہ_نقل(p, buf, n);
    chunk->w_انحراف += n;
  }
  واپس n;
}

ساکن size_t chunk_read(ساخت buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.ڈیٹا[chunk->r_انحراف];
  size_t n = chunk->w_انحراف - chunk->r_انحراف;
  DEBUGASSERT(chunk->w_انحراف >= chunk->r_انحراف);
  اگر(!n) {
    واپس 0;
  }
  ورنہ اگر(n <= len) {
    حافظہ_نقل(buf, p, n);
    chunk->r_انحراف = chunk->w_انحراف = 0;
    واپس n;
  }
  ورنہ {
    حافظہ_نقل(buf, p, len);
    chunk->r_انحراف += len;
    واپس len;
  }
}

ساکن CURLcode chunk_slurpn(ساخت buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             خالی *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.ڈیٹا[chunk->w_انحراف];
  size_t n = chunk->dlen - chunk->w_انحراف; /* free amount */
  CURLcode نتیجہ;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_انحراف);
  اگر(!n)
    واپس CURLE_AGAIN;
  اگر(max_len && n > max_len)
    n = max_len;
  نتیجہ = reader(reader_ctx, p, n, pnread);
  اگر(!نتیجہ) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_انحراف += *pnread;
  }
  واپس نتیجہ;
}

ساکن خالی chunk_peek(ثابت ساخت buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_انحراف >= chunk->r_انحراف);
  *pbuf = &chunk->x.ڈیٹا[chunk->r_انحراف];
  *plen = chunk->w_انحراف - chunk->r_انحراف;
}

ساکن خالی chunk_peek_at(ثابت ساخت buf_chunk *chunk, size_t انحراف,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  انحراف += chunk->r_انحراف;
  DEBUGASSERT(chunk->w_انحراف >= انحراف);
  *pbuf = &chunk->x.ڈیٹا[انحراف];
  *plen = chunk->w_انحراف - انحراف;
}

ساکن size_t chunk_skip(ساخت buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_انحراف - chunk->r_انحراف;
  DEBUGASSERT(chunk->w_انحراف >= chunk->r_انحراف);
  اگر(n) {
    n = CURLMIN(n, amount);
    chunk->r_انحراف += n;
    اگر(chunk->r_انحراف == chunk->w_انحراف)
      chunk->r_انحراف = chunk->w_انحراف = 0;
  }
  واپس n;
}

ساکن خالی chunk_list_free(ساخت buf_chunk **anchor)
{
  ساخت buf_chunk *chunk;
  جب(*anchor) {
    chunk = *anchor;
    *anchor = chunk->اگلا;
    curlx_free(chunk);
  }
}

خالی Curl_bufcp_init(ساخت bufc_pool *pool,
                     size_t chunk_سائز, size_t فالتو_max)
{
  DEBUGASSERT(chunk_سائز > 0);
  DEBUGASSERT(فالتو_max > 0);
  memset(pool, 0, حجم(*pool));
  pool->chunk_سائز = chunk_سائز;
  pool->فالتو_max = فالتو_max;
}

ساکن CURLcode bufcp_take(ساخت bufc_pool *pool,
                           ساخت buf_chunk **pchunk)
{
  ساخت buf_chunk *chunk = تہی;

  اگر(pool->فالتو) {
    chunk = pool->فالتو;
    pool->فالتو = chunk->اگلا;
    --pool->فالتو_گنتی;
    chunk_reset(chunk);
    *pchunk = chunk;
    واپس CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  اگر(pool->chunk_سائز > SIZE_MAX - حجم(*chunk)) {
    *pchunk = تہی;
    واپس CURLE_برآمد_کا_حافظہ;
  }

  chunk = curlx_calloc(1, حجم(*chunk) + pool->chunk_سائز);
  اگر(!chunk) {
    *pchunk = تہی;
    واپس CURLE_برآمد_کا_حافظہ;
  }
  chunk->dlen = pool->chunk_سائز;
  *pchunk = chunk;
  واپس CURLE_OK;
}

ساکن خالی bufcp_put(ساخت bufc_pool *pool,
                      ساخت buf_chunk *chunk)
{
  اگر(pool->فالتو_گنتی >= pool->فالتو_max) {
    curlx_free(chunk);
  }
  ورنہ {
    chunk_reset(chunk);
    chunk->اگلا = pool->فالتو;
    pool->فالتو = chunk;
    ++pool->فالتو_گنتی;
  }
}

خالی Curl_bufcp_free(ساخت bufc_pool *pool)
{
  chunk_list_free(&pool->فالتو);
  pool->فالتو_گنتی = 0;
}

ساکن خالی bufq_init(ساخت bufq *q, ساخت bufc_pool *pool,
                      size_t chunk_سائز, size_t max_chunks, صحیح opts)
{
  DEBUGASSERT(chunk_سائز > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, حجم(*q));
  q->chunk_سائز = chunk_سائز;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

خالی Curl_bufq_init2(ساخت bufq *q, size_t chunk_سائز, size_t max_chunks,
                     صحیح opts)
{
  bufq_init(q, تہی, chunk_سائز, max_chunks, opts);
}

خالی Curl_bufq_init(ساخت bufq *q, size_t chunk_سائز, size_t max_chunks)
{
  bufq_init(q, تہی, chunk_سائز, max_chunks, BUFQ_OPT_NONE);
}

خالی Curl_bufq_initp(ساخت bufq *q, ساخت bufc_pool *pool,
                     size_t max_chunks, صحیح opts)
{
  bufq_init(q, pool, pool->chunk_سائز, max_chunks, opts);
}

خالی Curl_bufq_free(ساخت bufq *q)
{
  chunk_list_free(&q->سر);
  chunk_list_free(&q->فالتو);
  q->دم = تہی;
  q->chunk_گنتی = 0;
}

خالی Curl_bufq_reset(ساخت bufq *q)
{
  ساخت buf_chunk *chunk;
  جب(q->سر) {
    chunk = q->سر;
    q->سر = chunk->اگلا;
    chunk->اگلا = q->فالتو;
    q->فالتو = chunk;
  }
  q->دم = تہی;
}

size_t Curl_bufq_len(ثابت ساخت bufq *q)
{
  ثابت ساخت buf_chunk *chunk = q->سر;
  size_t len = 0;
  جب(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->اگلا;
  }
  واپس len;
}

bool Curl_bufq_is_خالی_خانے(ثابت ساخت bufq *q)
{
  واپس !q->سر || chunk_is_خالی_خانے(q->سر);
}

bool Curl_bufq_is_بھرا(ثابت ساخت bufq *q)
{
  اگر(!q->دم || q->فالتو)
    واپس FALSE;
  اگر(q->chunk_گنتی < q->max_chunks)
    واپس FALSE;
  اگر(q->chunk_گنتی > q->max_chunks)
    واپس TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  واپس chunk_is_بھرا(q->دم);
}

ساکن ساخت buf_chunk *get_فالتو(ساخت bufq *q)
{
  ساخت buf_chunk *chunk = تہی;

  اگر(q->فالتو) {
    chunk = q->فالتو;
    q->فالتو = chunk->اگلا;
    chunk_reset(chunk);
    واپس chunk;
  }

  اگر(q->chunk_گنتی >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_حد_بندی)))
    واپس تہی;

  اگر(q->pool) {
    اگر(bufcp_take(q->pool, &chunk))
      واپس تہی;
    ++q->chunk_گنتی;
    واپس chunk;
  }
  ورنہ {
    /* Check for integer overflow before allocation */
    اگر(q->chunk_سائز > SIZE_MAX - حجم(*chunk)) {
      واپس تہی;
    }

    chunk = curlx_calloc(1, حجم(*chunk) + q->chunk_سائز);
    اگر(!chunk)
      واپس تہی;
    chunk->dlen = q->chunk_سائز;
    ++q->chunk_گنتی;
    واپس chunk;
  }
}

ساکن خالی prune_سر(ساخت bufq *q)
{
  ساخت buf_chunk *chunk;

  جب(q->سر && chunk_is_خالی_خانے(q->سر)) {
    chunk = q->سر;
    q->سر = chunk->اگلا;
    اگر(q->دم == chunk)
      q->دم = q->سر;
    اگر(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_گنتی;
    }
    ورنہ اگر((q->chunk_گنتی > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_گنتی;
    }
    ورنہ {
      chunk->اگلا = q->فالتو;
      q->فالتو = chunk;
    }
  }
}

ساکن ساخت buf_chunk *get_non_بھرا_دم(ساخت bufq *q)
{
  ساخت buf_chunk *chunk;

  اگر(q->دم && !chunk_is_بھرا(q->دم))
    واپس q->دم;
  chunk = get_فالتو(q);
  اگر(chunk) {
    /* new tail, and possibly new head */
    اگر(q->دم) {
      q->دم->اگلا = chunk;
      q->دم = chunk;
    }
    ورنہ {
      DEBUGASSERT(!q->سر);
      q->سر = q->دم = chunk;
    }
  }
  واپس chunk;
}

CURLcode Curl_bufq_write(ساخت bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  ساخت buf_chunk *دم;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  جب(len) {
    دم = get_non_بھرا_دم(q);
    اگر(!دم) {
      اگر((q->chunk_گنتی < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_حد_بندی))
        /* should have gotten a tail, but did not */
        واپس CURLE_برآمد_کا_حافظہ;
      توڑو;
    }
    n = chunk_append(دم, buf, len);
    اگر(!n)
      توڑو;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  واپس (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(ساخت bufq *q,
                          ثابت حرف *buf, size_t len,
                          size_t *pnwritten)
{
  واپس Curl_bufq_write(q, (ثابت uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(ساخت bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  جب(len && q->سر) {
    size_t n = chunk_read(q->سر, buf, len);
    اگر(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_سر(q);
  }
  واپس (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(ساخت bufq *q, حرف *buf, size_t len,
                         size_t *pnread)
{
  واپس Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(ساخت bufq *q,
                    ثابت uint8_t **pbuf, size_t *plen)
{
  اگر(q->سر && chunk_is_خالی_خانے(q->سر)) {
    prune_سر(q);
  }
  اگر(q->سر && !chunk_is_خالی_خانے(q->سر)) {
    chunk_peek(q->سر, pbuf, plen);
    واپس TRUE;
  }
  *pbuf = تہی;
  *plen = 0;
  واپس FALSE;
}

bool Curl_bufq_peek_at(ساخت bufq *q, size_t انحراف,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  ساخت buf_chunk *c = q->سر;
  size_t clen;

  جب(c) {
    clen = chunk_len(c);
    اگر(!clen)
      توڑو;
    اگر(انحراف >= clen) {
      انحراف -= clen;
      c = c->اگلا;
      جاری;
    }
    chunk_peek_at(c, انحراف, pbuf, plen);
    واپس TRUE;
  }
  *pbuf = تہی;
  *plen = 0;
  واپس FALSE;
}

خالی Curl_bufq_skip(ساخت bufq *q, size_t amount)
{
  size_t n;

  جب(amount && q->سر) {
    n = chunk_skip(q->سر, amount);
    amount -= n;
    prune_سر(q);
  }
}

CURLcode Curl_bufq_pass(ساخت bufq *q, Curl_bufq_writer *writer,
                        خالی *writer_ctx, size_t *pwritten)
{
  ثابت uint8_t *buf;
  size_t blen;
  CURLcode نتیجہ = CURLE_OK;

  *pwritten = 0;
  جب(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    نتیجہ = writer(writer_ctx, buf, blen, &chunk_written);
    اگر(نتیجہ) {
      اگر((نتیجہ == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        نتیجہ = CURLE_OK;
      }
      توڑو;
    }
    اگر(!chunk_written) {
      اگر(!*pwritten) {
        /* treat as blocked */
        نتیجہ = CURLE_AGAIN;
      }
      توڑو;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  واپس نتیجہ;
}

CURLcode Curl_bufq_write_pass(ساخت bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, خالی *writer_ctx,
                              size_t *pwritten)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t n;

  *pwritten = 0;
  جب(len) {
    اگر(Curl_bufq_is_بھرا(q)) {
      /* try to make room in case we are full */
      نتیجہ = Curl_bufq_pass(q, writer, writer_ctx, &n);
      اگر(نتیجہ) {
        اگر(نتیجہ != CURLE_AGAIN) {
          /* real error, fail */
          واپس نتیجہ;
        }
        /* would block, bufq is full, give up */
        توڑو;
      }
    }

    /* Add to bufq as much as there is room for */
    نتیجہ = Curl_bufq_write(q, buf, len, &n);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN)
        /* real error, fail */
        واپس نتیجہ;
      /* result == CURLE_AGAIN */
      اگر(*pwritten)
        /* we did write successfully before */
        نتیجہ = CURLE_OK;
      واپس نتیجہ;
    }
    ورنہ اگر(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      توڑو;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  واپس (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(ساخت bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, خالی *reader_ctx,
                        size_t *pnread)
{
  ساخت buf_chunk *دم = تہی;

  *pnread = 0;
  دم = get_non_بھرا_دم(q);
  اگر(!دم) {
    اگر(q->chunk_گنتی < q->max_chunks)
      واپس CURLE_برآمد_کا_حافظہ;
    /* full, blocked */
    واپس CURLE_AGAIN;
  }

  واپس chunk_slurpn(دم, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
ساکن CURLcode bufq_slurpn(ساخت bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, خالی *reader_ctx,
                            size_t *pnread)
{
  CURLcode نتیجہ;

  *pnread = 0;
  جب(1) {
    size_t n;
    نتیجہ = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    اگر(نتیجہ) {
      اگر(!*pnread || نتیجہ != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        واپس نتیجہ;
      }
      نتیجہ = CURLE_OK;
      توڑو;
    }
    ورنہ اگر(n == 0) {
      /* eof, result remains CURLE_OK */
      توڑو;
    }
    *pnread += n;
    اگر(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      اگر(!max_len)
        توڑو;
    }
    /* give up slurping when we get less bytes than we asked for */
    اگر(q->دم && !chunk_is_بھرا(q->دم))
      توڑو;
  }
  واپس نتیجہ;
}

CURLcode Curl_bufq_slurp(ساخت bufq *q, Curl_bufq_reader *reader,
                         خالی *reader_ctx, size_t *pnread)
{
  واپس bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
