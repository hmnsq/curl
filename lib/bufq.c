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

ساكن bool chunk_is_خالی_تھاواں(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->r_وتھ >= chunk->w_وتھ;
}

ساكن bool chunk_is_بھریا(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_وتھ >= chunk->dlen;
}

ساكن size_t chunk_len(ثابت بنية buf_chunk *chunk)
{
  أرجع chunk->w_وتھ - chunk->r_وتھ;
}

ساكن فراغ chunk_reset(بنية buf_chunk *chunk)
{
  chunk->اگلا = عدم;
  chunk->r_وتھ = chunk->w_وتھ = 0;
}

ساكن size_t chunk_append(بنية buf_chunk *chunk,
                           ثابت uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.انکڑے[chunk->w_وتھ];
  size_t n = chunk->dlen - chunk->w_وتھ;
  DEBUGASSERT(chunk->dlen >= chunk->w_وتھ);
  إذا(n) {
    n = CURLMIN(n, len);
    حافظہ_نقل(p, buf, n);
    chunk->w_وتھ += n;
  }
  أرجع n;
}

ساكن size_t chunk_read(بنية buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.انکڑے[chunk->r_وتھ];
  size_t n = chunk->w_وتھ - chunk->r_وتھ;
  DEBUGASSERT(chunk->w_وتھ >= chunk->r_وتھ);
  إذا(!n) {
    أرجع 0;
  }
  وإلا إذا(n <= len) {
    حافظہ_نقل(buf, p, n);
    chunk->r_وتھ = chunk->w_وتھ = 0;
    أرجع n;
  }
  وإلا {
    حافظہ_نقل(buf, p, len);
    chunk->r_وتھ += len;
    أرجع len;
  }
}

ساكن CURLcode chunk_slurpn(بنية buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             فراغ *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.انکڑے[chunk->w_وتھ];
  size_t n = chunk->dlen - chunk->w_وتھ; /* free amount */
  CURLcode نتیجہ;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_وتھ);
  إذا(!n)
    أرجع CURLE_AGAIN;
  إذا(max_len && n > max_len)
    n = max_len;
  نتیجہ = reader(reader_ctx, p, n, pnread);
  إذا(!نتیجہ) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_وتھ += *pnread;
  }
  أرجع نتیجہ;
}

ساكن فراغ chunk_peek(ثابت بنية buf_chunk *chunk,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_وتھ >= chunk->r_وتھ);
  *pbuf = &chunk->x.انکڑے[chunk->r_وتھ];
  *plen = chunk->w_وتھ - chunk->r_وتھ;
}

ساكن فراغ chunk_peek_at(ثابت بنية buf_chunk *chunk, size_t وتھ,
                          ثابت uint8_t **pbuf, size_t *plen)
{
  وتھ += chunk->r_وتھ;
  DEBUGASSERT(chunk->w_وتھ >= وتھ);
  *pbuf = &chunk->x.انکڑے[وتھ];
  *plen = chunk->w_وتھ - وتھ;
}

ساكن size_t chunk_skip(بنية buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_وتھ - chunk->r_وتھ;
  DEBUGASSERT(chunk->w_وتھ >= chunk->r_وتھ);
  إذا(n) {
    n = CURLMIN(n, amount);
    chunk->r_وتھ += n;
    إذا(chunk->r_وتھ == chunk->w_وتھ)
      chunk->r_وتھ = chunk->w_وتھ = 0;
  }
  أرجع n;
}

ساكن فراغ chunk_list_free(بنية buf_chunk **anchor)
{
  بنية buf_chunk *chunk;
  طالما(*anchor) {
    chunk = *anchor;
    *anchor = chunk->اگلا;
    curlx_free(chunk);
  }
}

فراغ Curl_bufcp_init(بنية bufc_pool *pool,
                     size_t chunk_ناپ, size_t وادھو_max)
{
  DEBUGASSERT(chunk_ناپ > 0);
  DEBUGASSERT(وادھو_max > 0);
  memset(pool, 0, حجم(*pool));
  pool->chunk_ناپ = chunk_ناپ;
  pool->وادھو_max = وادھو_max;
}

ساكن CURLcode bufcp_take(بنية bufc_pool *pool,
                           بنية buf_chunk **pchunk)
{
  بنية buf_chunk *chunk = عدم;

  إذا(pool->وادھو) {
    chunk = pool->وادھو;
    pool->وادھو = chunk->اگلا;
    --pool->وادھو_گنتی;
    chunk_reset(chunk);
    *pchunk = chunk;
    أرجع CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  إذا(pool->chunk_ناپ > SIZE_MAX - حجم(*chunk)) {
    *pchunk = عدم;
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  chunk = curlx_calloc(1, حجم(*chunk) + pool->chunk_ناپ);
  إذا(!chunk) {
    *pchunk = عدم;
    أرجع CURLE_باہرلا_دا_حافظہ;
  }
  chunk->dlen = pool->chunk_ناپ;
  *pchunk = chunk;
  أرجع CURLE_OK;
}

ساكن فراغ bufcp_put(بنية bufc_pool *pool,
                      بنية buf_chunk *chunk)
{
  إذا(pool->وادھو_گنتی >= pool->وادھو_max) {
    curlx_free(chunk);
  }
  وإلا {
    chunk_reset(chunk);
    chunk->اگلا = pool->وادھو;
    pool->وادھو = chunk;
    ++pool->وادھو_گنتی;
  }
}

فراغ Curl_bufcp_free(بنية bufc_pool *pool)
{
  chunk_list_free(&pool->وادھو);
  pool->وادھو_گنتی = 0;
}

ساكن فراغ bufq_init(بنية bufq *q, بنية bufc_pool *pool,
                      size_t chunk_ناپ, size_t max_chunks, صحيح opts)
{
  DEBUGASSERT(chunk_ناپ > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, حجم(*q));
  q->chunk_ناپ = chunk_ناپ;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

فراغ Curl_bufq_init2(بنية bufq *q, size_t chunk_ناپ, size_t max_chunks,
                     صحيح opts)
{
  bufq_init(q, عدم, chunk_ناپ, max_chunks, opts);
}

فراغ Curl_bufq_init(بنية bufq *q, size_t chunk_ناپ, size_t max_chunks)
{
  bufq_init(q, عدم, chunk_ناپ, max_chunks, BUFQ_OPT_NONE);
}

فراغ Curl_bufq_initp(بنية bufq *q, بنية bufc_pool *pool,
                     size_t max_chunks, صحيح opts)
{
  bufq_init(q, pool, pool->chunk_ناپ, max_chunks, opts);
}

فراغ Curl_bufq_free(بنية bufq *q)
{
  chunk_list_free(&q->سر);
  chunk_list_free(&q->وادھو);
  q->پوچھ = عدم;
  q->chunk_گنتی = 0;
}

فراغ Curl_bufq_reset(بنية bufq *q)
{
  بنية buf_chunk *chunk;
  طالما(q->سر) {
    chunk = q->سر;
    q->سر = chunk->اگلا;
    chunk->اگلا = q->وادھو;
    q->وادھو = chunk;
  }
  q->پوچھ = عدم;
}

size_t Curl_bufq_len(ثابت بنية bufq *q)
{
  ثابت بنية buf_chunk *chunk = q->سر;
  size_t len = 0;
  طالما(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->اگلا;
  }
  أرجع len;
}

bool Curl_bufq_is_خالی_تھاواں(ثابت بنية bufq *q)
{
  أرجع !q->سر || chunk_is_خالی_تھاواں(q->سر);
}

bool Curl_bufq_is_بھریا(ثابت بنية bufq *q)
{
  إذا(!q->پوچھ || q->وادھو)
    أرجع FALSE;
  إذا(q->chunk_گنتی < q->max_chunks)
    أرجع FALSE;
  إذا(q->chunk_گنتی > q->max_chunks)
    أرجع TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  أرجع chunk_is_بھریا(q->پوچھ);
}

ساكن بنية buf_chunk *get_وادھو(بنية bufq *q)
{
  بنية buf_chunk *chunk = عدم;

  إذا(q->وادھو) {
    chunk = q->وادھو;
    q->وادھو = chunk->اگلا;
    chunk_reset(chunk);
    أرجع chunk;
  }

  إذا(q->chunk_گنتی >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_حد_بندی)))
    أرجع عدم;

  إذا(q->pool) {
    إذا(bufcp_take(q->pool, &chunk))
      أرجع عدم;
    ++q->chunk_گنتی;
    أرجع chunk;
  }
  وإلا {
    /* Check for integer overflow before allocation */
    إذا(q->chunk_ناپ > SIZE_MAX - حجم(*chunk)) {
      أرجع عدم;
    }

    chunk = curlx_calloc(1, حجم(*chunk) + q->chunk_ناپ);
    إذا(!chunk)
      أرجع عدم;
    chunk->dlen = q->chunk_ناپ;
    ++q->chunk_گنتی;
    أرجع chunk;
  }
}

ساكن فراغ prune_سر(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  طالما(q->سر && chunk_is_خالی_تھاواں(q->سر)) {
    chunk = q->سر;
    q->سر = chunk->اگلا;
    إذا(q->پوچھ == chunk)
      q->پوچھ = q->سر;
    إذا(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_گنتی;
    }
    وإلا إذا((q->chunk_گنتی > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_گنتی;
    }
    وإلا {
      chunk->اگلا = q->وادھو;
      q->وادھو = chunk;
    }
  }
}

ساكن بنية buf_chunk *get_non_بھریا_پوچھ(بنية bufq *q)
{
  بنية buf_chunk *chunk;

  إذا(q->پوچھ && !chunk_is_بھریا(q->پوچھ))
    أرجع q->پوچھ;
  chunk = get_وادھو(q);
  إذا(chunk) {
    /* new tail, and possibly new head */
    إذا(q->پوچھ) {
      q->پوچھ->اگلا = chunk;
      q->پوچھ = chunk;
    }
    وإلا {
      DEBUGASSERT(!q->سر);
      q->سر = q->پوچھ = chunk;
    }
  }
  أرجع chunk;
}

CURLcode Curl_bufq_write(بنية bufq *q,
                         ثابت uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  بنية buf_chunk *پوچھ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  طالما(len) {
    پوچھ = get_non_بھریا_پوچھ(q);
    إذا(!پوچھ) {
      إذا((q->chunk_گنتی < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_حد_بندی))
        /* should have gotten a tail, but did not */
        أرجع CURLE_باہرلا_دا_حافظہ;
      اكسر;
    }
    n = chunk_append(پوچھ, buf, len);
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
  إذا(q->سر && chunk_is_خالی_تھاواں(q->سر)) {
    prune_سر(q);
  }
  إذا(q->سر && !chunk_is_خالی_تھاواں(q->سر)) {
    chunk_peek(q->سر, pbuf, plen);
    أرجع TRUE;
  }
  *pbuf = عدم;
  *plen = 0;
  أرجع FALSE;
}

bool Curl_bufq_peek_at(بنية bufq *q, size_t وتھ,
                       ثابت uint8_t **pbuf, size_t *plen)
{
  بنية buf_chunk *c = q->سر;
  size_t clen;

  طالما(c) {
    clen = chunk_len(c);
    إذا(!clen)
      اكسر;
    إذا(وتھ >= clen) {
      وتھ -= clen;
      c = c->اگلا;
      استمر;
    }
    chunk_peek_at(c, وتھ, pbuf, plen);
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
  CURLcode نتیجہ = CURLE_OK;

  *pwritten = 0;
  طالما(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    نتیجہ = writer(writer_ctx, buf, blen, &chunk_written);
    إذا(نتیجہ) {
      إذا((نتیجہ == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        نتیجہ = CURLE_OK;
      }
      اكسر;
    }
    إذا(!chunk_written) {
      إذا(!*pwritten) {
        /* treat as blocked */
        نتیجہ = CURLE_AGAIN;
      }
      اكسر;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  أرجع نتیجہ;
}

CURLcode Curl_bufq_write_pass(بنية bufq *q,
                              ثابت uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, فراغ *writer_ctx,
                              size_t *pwritten)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t n;

  *pwritten = 0;
  طالما(len) {
    إذا(Curl_bufq_is_بھریا(q)) {
      /* try to make room in case we are full */
      نتیجہ = Curl_bufq_pass(q, writer, writer_ctx, &n);
      إذا(نتیجہ) {
        إذا(نتیجہ != CURLE_AGAIN) {
          /* real error, fail */
          أرجع نتیجہ;
        }
        /* would block, bufq is full, give up */
        اكسر;
      }
    }

    /* Add to bufq as much as there is room for */
    نتیجہ = Curl_bufq_write(q, buf, len, &n);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN)
        /* real error, fail */
        أرجع نتیجہ;
      /* result == CURLE_AGAIN */
      إذا(*pwritten)
        /* we did write successfully before */
        نتیجہ = CURLE_OK;
      أرجع نتیجہ;
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
  بنية buf_chunk *پوچھ = عدم;

  *pnread = 0;
  پوچھ = get_non_بھریا_پوچھ(q);
  إذا(!پوچھ) {
    إذا(q->chunk_گنتی < q->max_chunks)
      أرجع CURLE_باہرلا_دا_حافظہ;
    /* full, blocked */
    أرجع CURLE_AGAIN;
  }

  أرجع chunk_slurpn(پوچھ, max_len, reader, reader_ctx, pnread);
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
  CURLcode نتیجہ;

  *pnread = 0;
  طالما(1) {
    size_t n;
    نتیجہ = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    إذا(نتیجہ) {
      إذا(!*pnread || نتیجہ != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        أرجع نتیجہ;
      }
      نتیجہ = CURLE_OK;
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
    إذا(q->پوچھ && !chunk_is_بھریا(q->پوچھ))
      اكسر;
  }
  أرجع نتیجہ;
}

CURLcode Curl_bufq_slurp(بنية bufq *q, Curl_bufq_reader *reader,
                         فراغ *reader_ctx, size_t *pnread)
{
  أرجع bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
