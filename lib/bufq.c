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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "bufq.h"

స్థిర bool chunk_is_ఖాళీలు(స్థిరరాశి నిర్మాణం buf_chunk *chunk)
{
  తిరిగి chunk->r_స్థానభ్రంశం >= chunk->w_స్థానభ్రంశం;
}

స్థిర bool chunk_is_నిండిన(స్థిరరాశి నిర్మాణం buf_chunk *chunk)
{
  తిరిగి chunk->w_స్థానభ్రంశం >= chunk->dlen;
}

స్థిర size_t chunk_len(స్థిరరాశి నిర్మాణం buf_chunk *chunk)
{
  తిరిగి chunk->w_స్థానభ్రంశం - chunk->r_స్థానభ్రంశం;
}

స్థిర శూన్య chunk_reset(నిర్మాణం buf_chunk *chunk)
{
  chunk->తదుపరి = ఖాళీ;
  chunk->r_స్థానభ్రంశం = chunk->w_స్థానభ్రంశం = 0;
}

స్థిర size_t chunk_append(నిర్మాణం buf_chunk *chunk,
                           స్థిరరాశి uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.దత్తాంశం[chunk->w_స్థానభ్రంశం];
  size_t n = chunk->dlen - chunk->w_స్థానభ్రంశం;
  DEBUGASSERT(chunk->dlen >= chunk->w_స్థానభ్రంశం);
  ఒకవేళ(n) {
    n = CURLMIN(n, len);
    జ్ఞాపకం_నకలు(p, buf, n);
    chunk->w_స్థానభ్రంశం += n;
  }
  తిరిగి n;
}

స్థిర size_t chunk_read(నిర్మాణం buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.దత్తాంశం[chunk->r_స్థానభ్రంశం];
  size_t n = chunk->w_స్థానభ్రంశం - chunk->r_స్థానభ్రంశం;
  DEBUGASSERT(chunk->w_స్థానభ్రంశం >= chunk->r_స్థానభ్రంశం);
  ఒకవేళ(!n) {
    తిరిగి 0;
  }
  లేకపోతే ఒకవేళ(n <= len) {
    జ్ఞాపకం_నకలు(buf, p, n);
    chunk->r_స్థానభ్రంశం = chunk->w_స్థానభ్రంశం = 0;
    తిరిగి n;
  }
  లేకపోతే {
    జ్ఞాపకం_నకలు(buf, p, len);
    chunk->r_స్థానభ్రంశం += len;
    తిరిగి len;
  }
}

స్థిర CURLcode chunk_slurpn(నిర్మాణం buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             శూన్య *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.దత్తాంశం[chunk->w_స్థానభ్రంశం];
  size_t n = chunk->dlen - chunk->w_స్థానభ్రంశం; /* free amount */
  CURLcode ఫలితం;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_స్థానభ్రంశం);
  ఒకవేళ(!n)
    తిరిగి CURLE_AGAIN;
  ఒకవేళ(max_len && n > max_len)
    n = max_len;
  ఫలితం = reader(reader_ctx, p, n, pnread);
  ఒకవేళ(!ఫలితం) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_స్థానభ్రంశం += *pnread;
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య chunk_peek(స్థిరరాశి నిర్మాణం buf_chunk *chunk,
                       స్థిరరాశి uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_స్థానభ్రంశం >= chunk->r_స్థానభ్రంశం);
  *pbuf = &chunk->x.దత్తాంశం[chunk->r_స్థానభ్రంశం];
  *plen = chunk->w_స్థానభ్రంశం - chunk->r_స్థానభ్రంశం;
}

స్థిర శూన్య chunk_peek_at(స్థిరరాశి నిర్మాణం buf_chunk *chunk, size_t స్థానభ్రంశం,
                          స్థిరరాశి uint8_t **pbuf, size_t *plen)
{
  స్థానభ్రంశం += chunk->r_స్థానభ్రంశం;
  DEBUGASSERT(chunk->w_స్థానభ్రంశం >= స్థానభ్రంశం);
  *pbuf = &chunk->x.దత్తాంశం[స్థానభ్రంశం];
  *plen = chunk->w_స్థానభ్రంశం - స్థానభ్రంశం;
}

స్థిర size_t chunk_skip(నిర్మాణం buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_స్థానభ్రంశం - chunk->r_స్థానభ్రంశం;
  DEBUGASSERT(chunk->w_స్థానభ్రంశం >= chunk->r_స్థానభ్రంశం);
  ఒకవేళ(n) {
    n = CURLMIN(n, amount);
    chunk->r_స్థానభ్రంశం += n;
    ఒకవేళ(chunk->r_స్థానభ్రంశం == chunk->w_స్థానభ్రంశం)
      chunk->r_స్థానభ్రంశం = chunk->w_స్థానభ్రంశం = 0;
  }
  తిరిగి n;
}

స్థిర శూన్య chunk_list_free(నిర్మాణం buf_chunk **anchor)
{
  నిర్మాణం buf_chunk *chunk;
  అయితే(*anchor) {
    chunk = *anchor;
    *anchor = chunk->తదుపరి;
    curlx_free(chunk);
  }
}

శూన్య Curl_bufcp_init(నిర్మాణం bufc_pool *pool,
                     size_t chunk_కొలత, size_t అదనపు_max)
{
  DEBUGASSERT(chunk_కొలత > 0);
  DEBUGASSERT(అదనపు_max > 0);
  memset(pool, 0, పరిమాణం(*pool));
  pool->chunk_కొలత = chunk_కొలత;
  pool->అదనపు_max = అదనపు_max;
}

స్థిర CURLcode bufcp_take(నిర్మాణం bufc_pool *pool,
                           నిర్మాణం buf_chunk **pchunk)
{
  నిర్మాణం buf_chunk *chunk = ఖాళీ;

  ఒకవేళ(pool->అదనపు) {
    chunk = pool->అదనపు;
    pool->అదనపు = chunk->తదుపరి;
    --pool->అదనపు_లెక్క;
    chunk_reset(chunk);
    *pchunk = chunk;
    తిరిగి CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  ఒకవేళ(pool->chunk_కొలత > SIZE_MAX - పరిమాణం(*chunk)) {
    *pchunk = ఖాళీ;
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  chunk = curlx_calloc(1, పరిమాణం(*chunk) + pool->chunk_కొలత);
  ఒకవేళ(!chunk) {
    *pchunk = ఖాళీ;
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  chunk->dlen = pool->chunk_కొలత;
  *pchunk = chunk;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య bufcp_put(నిర్మాణం bufc_pool *pool,
                      నిర్మాణం buf_chunk *chunk)
{
  ఒకవేళ(pool->అదనపు_లెక్క >= pool->అదనపు_max) {
    curlx_free(chunk);
  }
  లేకపోతే {
    chunk_reset(chunk);
    chunk->తదుపరి = pool->అదనపు;
    pool->అదనపు = chunk;
    ++pool->అదనపు_లెక్క;
  }
}

శూన్య Curl_bufcp_free(నిర్మాణం bufc_pool *pool)
{
  chunk_list_free(&pool->అదనపు);
  pool->అదనపు_లెక్క = 0;
}

స్థిర శూన్య bufq_init(నిర్మాణం bufq *q, నిర్మాణం bufc_pool *pool,
                      size_t chunk_కొలత, size_t max_chunks, పూర్ణాంకం opts)
{
  DEBUGASSERT(chunk_కొలత > 0);
  DEBUGASSERT(max_chunks > 0);
  memset(q, 0, పరిమాణం(*q));
  q->chunk_కొలత = chunk_కొలత;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

శూన్య Curl_bufq_init2(నిర్మాణం bufq *q, size_t chunk_కొలత, size_t max_chunks,
                     పూర్ణాంకం opts)
{
  bufq_init(q, ఖాళీ, chunk_కొలత, max_chunks, opts);
}

శూన్య Curl_bufq_init(నిర్మాణం bufq *q, size_t chunk_కొలత, size_t max_chunks)
{
  bufq_init(q, ఖాళీ, chunk_కొలత, max_chunks, BUFQ_OPT_NONE);
}

శూన్య Curl_bufq_initp(నిర్మాణం bufq *q, నిర్మాణం bufc_pool *pool,
                     size_t max_chunks, పూర్ణాంకం opts)
{
  bufq_init(q, pool, pool->chunk_కొలత, max_chunks, opts);
}

శూన్య Curl_bufq_free(నిర్మాణం bufq *q)
{
  chunk_list_free(&q->తల);
  chunk_list_free(&q->అదనపు);
  q->తోక = ఖాళీ;
  q->chunk_లెక్క = 0;
}

శూన్య Curl_bufq_reset(నిర్మాణం bufq *q)
{
  నిర్మాణం buf_chunk *chunk;
  అయితే(q->తల) {
    chunk = q->తల;
    q->తల = chunk->తదుపరి;
    chunk->తదుపరి = q->అదనపు;
    q->అదనపు = chunk;
  }
  q->తోక = ఖాళీ;
}

size_t Curl_bufq_len(స్థిరరాశి నిర్మాణం bufq *q)
{
  స్థిరరాశి నిర్మాణం buf_chunk *chunk = q->తల;
  size_t len = 0;
  అయితే(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->తదుపరి;
  }
  తిరిగి len;
}

bool Curl_bufq_is_ఖాళీలు(స్థిరరాశి నిర్మాణం bufq *q)
{
  తిరిగి !q->తల || chunk_is_ఖాళీలు(q->తల);
}

bool Curl_bufq_is_నిండిన(స్థిరరాశి నిర్మాణం bufq *q)
{
  ఒకవేళ(!q->తోక || q->అదనపు)
    తిరిగి FALSE;
  ఒకవేళ(q->chunk_లెక్క < q->max_chunks)
    తిరిగి FALSE;
  ఒకవేళ(q->chunk_లెక్క > q->max_chunks)
    తిరిగి TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  తిరిగి chunk_is_నిండిన(q->తోక);
}

స్థిర నిర్మాణం buf_chunk *get_అదనపు(నిర్మాణం bufq *q)
{
  నిర్మాణం buf_chunk *chunk = ఖాళీ;

  ఒకవేళ(q->అదనపు) {
    chunk = q->అదనపు;
    q->అదనపు = chunk->తదుపరి;
    chunk_reset(chunk);
    తిరిగి chunk;
  }

  ఒకవేళ(q->chunk_లెక్క >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_హద్దు)))
    తిరిగి ఖాళీ;

  ఒకవేళ(q->pool) {
    ఒకవేళ(bufcp_take(q->pool, &chunk))
      తిరిగి ఖాళీ;
    ++q->chunk_లెక్క;
    తిరిగి chunk;
  }
  లేకపోతే {
    /* Check for integer overflow before allocation */
    ఒకవేళ(q->chunk_కొలత > SIZE_MAX - పరిమాణం(*chunk)) {
      తిరిగి ఖాళీ;
    }

    chunk = curlx_calloc(1, పరిమాణం(*chunk) + q->chunk_కొలత);
    ఒకవేళ(!chunk)
      తిరిగి ఖాళీ;
    chunk->dlen = q->chunk_కొలత;
    ++q->chunk_లెక్క;
    తిరిగి chunk;
  }
}

స్థిర శూన్య prune_తల(నిర్మాణం bufq *q)
{
  నిర్మాణం buf_chunk *chunk;

  అయితే(q->తల && chunk_is_ఖాళీలు(q->తల)) {
    chunk = q->తల;
    q->తల = chunk->తదుపరి;
    ఒకవేళ(q->తోక == chunk)
      q->తోక = q->తల;
    ఒకవేళ(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_లెక్క;
    }
    లేకపోతే ఒకవేళ((q->chunk_లెక్క > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_లెక్క;
    }
    లేకపోతే {
      chunk->తదుపరి = q->అదనపు;
      q->అదనపు = chunk;
    }
  }
}

స్థిర నిర్మాణం buf_chunk *get_non_నిండిన_తోక(నిర్మాణం bufq *q)
{
  నిర్మాణం buf_chunk *chunk;

  ఒకవేళ(q->తోక && !chunk_is_నిండిన(q->తోక))
    తిరిగి q->తోక;
  chunk = get_అదనపు(q);
  ఒకవేళ(chunk) {
    /* new tail, and possibly new head */
    ఒకవేళ(q->తోక) {
      q->తోక->తదుపరి = chunk;
      q->తోక = chunk;
    }
    లేకపోతే {
      DEBUGASSERT(!q->తల);
      q->తల = q->తోక = chunk;
    }
  }
  తిరిగి chunk;
}

CURLcode Curl_bufq_write(నిర్మాణం bufq *q,
                         స్థిరరాశి uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  నిర్మాణం buf_chunk *తోక;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  అయితే(len) {
    తోక = get_non_నిండిన_తోక(q);
    ఒకవేళ(!తోక) {
      ఒకవేళ((q->chunk_లెక్క < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_హద్దు))
        /* should have gotten a tail, but did not */
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఆపు;
    }
    n = chunk_append(తోక, buf, len);
    ఒకవేళ(!n)
      ఆపు;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  తిరిగి (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(నిర్మాణం bufq *q,
                          స్థిరరాశి అక్షరం *buf, size_t len,
                          size_t *pnwritten)
{
  తిరిగి Curl_bufq_write(q, (స్థిరరాశి uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(నిర్మాణం bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread)
{
  *pnread = 0;
  అయితే(len && q->తల) {
    size_t n = chunk_read(q->తల, buf, len);
    ఒకవేళ(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_తల(q);
  }
  తిరిగి (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(నిర్మాణం bufq *q, అక్షరం *buf, size_t len,
                         size_t *pnread)
{
  తిరిగి Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(నిర్మాణం bufq *q,
                    స్థిరరాశి uint8_t **pbuf, size_t *plen)
{
  ఒకవేళ(q->తల && chunk_is_ఖాళీలు(q->తల)) {
    prune_తల(q);
  }
  ఒకవేళ(q->తల && !chunk_is_ఖాళీలు(q->తల)) {
    chunk_peek(q->తల, pbuf, plen);
    తిరిగి TRUE;
  }
  *pbuf = ఖాళీ;
  *plen = 0;
  తిరిగి FALSE;
}

bool Curl_bufq_peek_at(నిర్మాణం bufq *q, size_t స్థానభ్రంశం,
                       స్థిరరాశి uint8_t **pbuf, size_t *plen)
{
  నిర్మాణం buf_chunk *c = q->తల;
  size_t clen;

  అయితే(c) {
    clen = chunk_len(c);
    ఒకవేళ(!clen)
      ఆపు;
    ఒకవేళ(స్థానభ్రంశం >= clen) {
      స్థానభ్రంశం -= clen;
      c = c->తదుపరి;
      కొనసాగు;
    }
    chunk_peek_at(c, స్థానభ్రంశం, pbuf, plen);
    తిరిగి TRUE;
  }
  *pbuf = ఖాళీ;
  *plen = 0;
  తిరిగి FALSE;
}

శూన్య Curl_bufq_skip(నిర్మాణం bufq *q, size_t amount)
{
  size_t n;

  అయితే(amount && q->తల) {
    n = chunk_skip(q->తల, amount);
    amount -= n;
    prune_తల(q);
  }
}

CURLcode Curl_bufq_pass(నిర్మాణం bufq *q, Curl_bufq_writer *writer,
                        శూన్య *writer_ctx, size_t *pwritten)
{
  స్థిరరాశి uint8_t *buf;
  size_t blen;
  CURLcode ఫలితం = CURLE_OK;

  *pwritten = 0;
  అయితే(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    ఫలితం = writer(writer_ctx, buf, blen, &chunk_written);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ((ఫలితం == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        ఫలితం = CURLE_OK;
      }
      ఆపు;
    }
    ఒకవేళ(!chunk_written) {
      ఒకవేళ(!*pwritten) {
        /* treat as blocked */
        ఫలితం = CURLE_AGAIN;
      }
      ఆపు;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_bufq_write_pass(నిర్మాణం bufq *q,
                              స్థిరరాశి uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, శూన్య *writer_ctx,
                              size_t *pwritten)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t n;

  *pwritten = 0;
  అయితే(len) {
    ఒకవేళ(Curl_bufq_is_నిండిన(q)) {
      /* try to make room in case we are full */
      ఫలితం = Curl_bufq_pass(q, writer, writer_ctx, &n);
      ఒకవేళ(ఫలితం) {
        ఒకవేళ(ఫలితం != CURLE_AGAIN) {
          /* real error, fail */
          తిరిగి ఫలితం;
        }
        /* would block, bufq is full, give up */
        ఆపు;
      }
    }

    /* Add to bufq as much as there is room for */
    ఫలితం = Curl_bufq_write(q, buf, len, &n);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN)
        /* real error, fail */
        తిరిగి ఫలితం;
      /* result == CURLE_AGAIN */
      ఒకవేళ(*pwritten)
        /* we did write successfully before */
        ఫలితం = CURLE_OK;
      తిరిగి ఫలితం;
    }
    లేకపోతే ఒకవేళ(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      ఆపు;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  తిరిగి (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(నిర్మాణం bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, శూన్య *reader_ctx,
                        size_t *pnread)
{
  నిర్మాణం buf_chunk *తోక = ఖాళీ;

  *pnread = 0;
  తోక = get_non_నిండిన_తోక(q);
  ఒకవేళ(!తోక) {
    ఒకవేళ(q->chunk_లెక్క < q->max_chunks)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    /* full, blocked */
    తిరిగి CURLE_AGAIN;
  }

  తిరిగి chunk_slurpn(తోక, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
స్థిర CURLcode bufq_slurpn(నిర్మాణం bufq *q, size_t max_len,
                            Curl_bufq_reader *reader, శూన్య *reader_ctx,
                            size_t *pnread)
{
  CURLcode ఫలితం;

  *pnread = 0;
  అయితే(1) {
    size_t n;
    ఫలితం = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(!*pnread || ఫలితం != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        తిరిగి ఫలితం;
      }
      ఫలితం = CURLE_OK;
      ఆపు;
    }
    లేకపోతే ఒకవేళ(n == 0) {
      /* eof, result remains CURLE_OK */
      ఆపు;
    }
    *pnread += n;
    ఒకవేళ(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      ఒకవేళ(!max_len)
        ఆపు;
    }
    /* give up slurping when we get less bytes than we asked for */
    ఒకవేళ(q->తోక && !chunk_is_నిండిన(q->తోక))
      ఆపు;
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_bufq_slurp(నిర్మాణం bufq *q, Curl_bufq_reader *reader,
                         శూన్య *reader_ctx, size_t *pnread)
{
  తిరిగి bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
