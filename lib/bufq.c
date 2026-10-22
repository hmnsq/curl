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

स्थिर bool chunk_is_रिक्तानि(ध्रुव संरचना buf_chunk *chunk)
{
  प्रत्यागम chunk->r_विस्थापनम् >= chunk->w_विस्थापनम्;
}

स्थिर bool chunk_is_पूर्णम्(ध्रुव संरचना buf_chunk *chunk)
{
  प्रत्यागम chunk->w_विस्थापनम् >= chunk->dlen;
}

स्थिर size_t chunk_len(ध्रुव संरचना buf_chunk *chunk)
{
  प्रत्यागम chunk->w_विस्थापनम् - chunk->r_विस्थापनम्;
}

स्थिर शून्य chunk_reset(संरचना buf_chunk *chunk)
{
  chunk->अग्र = रिक्त;
  chunk->r_विस्थापनम् = chunk->w_विस्थापनम् = 0;
}

स्थिर size_t chunk_append(संरचना buf_chunk *chunk,
                           ध्रुव uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.दत्त[chunk->w_विस्थापनम्];
  size_t n = chunk->dlen - chunk->w_विस्थापनम्;
  DEBUGASSERT(chunk->dlen >= chunk->w_विस्थापनम्);
  यदि(n) {
    n = CURLMIN(n, len);
    स्मृतिं_प्रतिलिख(p, buf, n);
    chunk->w_विस्थापनम् += n;
  }
  प्रत्यागम n;
}

स्थिर size_t chunk_read(संरचना buf_chunk *chunk,
                         uint8_t *buf, size_t len)
{
  uint8_t *p = &chunk->x.दत्त[chunk->r_विस्थापनम्];
  size_t n = chunk->w_विस्थापनम् - chunk->r_विस्थापनम्;
  DEBUGASSERT(chunk->w_विस्थापनम् >= chunk->r_विस्थापनम्);
  यदि(!n) {
    प्रत्यागम 0;
  }
  अन्यथा यदि(n <= len) {
    स्मृतिं_प्रतिलिख(buf, p, n);
    chunk->r_विस्थापनम् = chunk->w_विस्थापनम् = 0;
    प्रत्यागम n;
  }
  अन्यथा {
    स्मृतिं_प्रतिलिख(buf, p, len);
    chunk->r_विस्थापनम् += len;
    प्रत्यागम len;
  }
}

स्थिर CURLcode chunk_slurpn(संरचना buf_chunk *chunk, size_t max_len,
                             Curl_bufq_reader *reader,
                             शून्य *reader_ctx, size_t *pnread)
{
  uint8_t *p = &chunk->x.दत्त[chunk->w_विस्थापनम्];
  size_t n = chunk->dlen - chunk->w_विस्थापनम्; /* free amount */
  CURLcode परिणाम;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_विस्थापनम्);
  यदि(!n)
    प्रत्यागम CURLE_AGAIN;
  यदि(max_len && n > max_len)
    n = max_len;
  परिणाम = reader(reader_ctx, p, n, pnread);
  यदि(!परिणाम) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_विस्थापनम् += *pnread;
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य chunk_peek(ध्रुव संरचना buf_chunk *chunk,
                       ध्रुव uint8_t **pbuf, size_t *plen)
{
  DEBUGASSERT(chunk->w_विस्थापनम् >= chunk->r_विस्थापनम्);
  *pbuf = &chunk->x.दत्त[chunk->r_विस्थापनम्];
  *plen = chunk->w_विस्थापनम् - chunk->r_विस्थापनम्;
}

स्थिर शून्य chunk_peek_at(ध्रुव संरचना buf_chunk *chunk, size_t विस्थापनम्,
                          ध्रुव uint8_t **pbuf, size_t *plen)
{
  विस्थापनम् += chunk->r_विस्थापनम्;
  DEBUGASSERT(chunk->w_विस्थापनम् >= विस्थापनम्);
  *pbuf = &chunk->x.दत्त[विस्थापनम्];
  *plen = chunk->w_विस्थापनम् - विस्थापनम्;
}

स्थिर size_t chunk_skip(संरचना buf_chunk *chunk, size_t amount)
{
  size_t n = chunk->w_विस्थापनम् - chunk->r_विस्थापनम्;
  DEBUGASSERT(chunk->w_विस्थापनम् >= chunk->r_विस्थापनम्);
  यदि(n) {
    n = CURLMIN(n, amount);
    chunk->r_विस्थापनम् += n;
    यदि(chunk->r_विस्थापनम् == chunk->w_विस्थापनम्)
      chunk->r_विस्थापनम् = chunk->w_विस्थापनम् = 0;
  }
  प्रत्यागम n;
}

स्थिर शून्य chunk_list_free(संरचना buf_chunk **anchor)
{
  संरचना buf_chunk *chunk;
  यावत्(*anchor) {
    chunk = *anchor;
    *anchor = chunk->अग्र;
    curlx_free(chunk);
  }
}

शून्य Curl_bufcp_init(संरचना bufc_pool *pool,
                     size_t chunk_आकार, size_t अतिरिक्तः_max)
{
  DEBUGASSERT(chunk_आकार > 0);
  DEBUGASSERT(अतिरिक्तः_max > 0);
  स्मृतिं_पूरय(pool, 0, परिमाण(*pool));
  pool->chunk_आकार = chunk_आकार;
  pool->अतिरिक्तः_max = अतिरिक्तः_max;
}

स्थिर CURLcode bufcp_take(संरचना bufc_pool *pool,
                           संरचना buf_chunk **pchunk)
{
  संरचना buf_chunk *chunk = रिक्त;

  यदि(pool->अतिरिक्तः) {
    chunk = pool->अतिरिक्तः;
    pool->अतिरिक्तः = chunk->अग्र;
    --pool->अतिरिक्तः_गणनम्;
    chunk_reset(chunk);
    *pchunk = chunk;
    प्रत्यागम CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  यदि(pool->chunk_आकार > SIZE_MAX - परिमाण(*chunk)) {
    *pchunk = रिक्त;
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  chunk = curlx_calloc(1, परिमाण(*chunk) + pool->chunk_आकार);
  यदि(!chunk) {
    *pchunk = रिक्त;
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  chunk->dlen = pool->chunk_आकार;
  *pchunk = chunk;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य bufcp_put(संरचना bufc_pool *pool,
                      संरचना buf_chunk *chunk)
{
  यदि(pool->अतिरिक्तः_गणनम् >= pool->अतिरिक्तः_max) {
    curlx_free(chunk);
  }
  अन्यथा {
    chunk_reset(chunk);
    chunk->अग्र = pool->अतिरिक्तः;
    pool->अतिरिक्तः = chunk;
    ++pool->अतिरिक्तः_गणनम्;
  }
}

शून्य Curl_bufcp_free(संरचना bufc_pool *pool)
{
  chunk_list_free(&pool->अतिरिक्तः);
  pool->अतिरिक्तः_गणनम् = 0;
}

स्थिर शून्य bufq_init(संरचना bufq *q, संरचना bufc_pool *pool,
                      size_t chunk_आकार, size_t max_chunks, पूर्णाङ्क opts)
{
  DEBUGASSERT(chunk_आकार > 0);
  DEBUGASSERT(max_chunks > 0);
  स्मृतिं_पूरय(q, 0, परिमाण(*q));
  q->chunk_आकार = chunk_आकार;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

शून्य Curl_bufq_init2(संरचना bufq *q, size_t chunk_आकार, size_t max_chunks,
                     पूर्णाङ्क opts)
{
  bufq_init(q, रिक्त, chunk_आकार, max_chunks, opts);
}

शून्य Curl_bufq_init(संरचना bufq *q, size_t chunk_आकार, size_t max_chunks)
{
  bufq_init(q, रिक्त, chunk_आकार, max_chunks, BUFQ_OPT_NONE);
}

शून्य Curl_bufq_initp(संरचना bufq *q, संरचना bufc_pool *pool,
                     size_t max_chunks, पूर्णाङ्क opts)
{
  bufq_init(q, pool, pool->chunk_आकार, max_chunks, opts);
}

शून्य Curl_bufq_free(संरचना bufq *q)
{
  chunk_list_free(&q->शीर्ष);
  chunk_list_free(&q->अतिरिक्तः);
  q->पुच्छ = रिक्त;
  q->chunk_गणनम् = 0;
}

शून्य Curl_bufq_reset(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;
  यावत्(q->शीर्ष) {
    chunk = q->शीर्ष;
    q->शीर्ष = chunk->अग्र;
    chunk->अग्र = q->अतिरिक्तः;
    q->अतिरिक्तः = chunk;
  }
  q->पुच्छ = रिक्त;
}

size_t Curl_bufq_len(ध्रुव संरचना bufq *q)
{
  ध्रुव संरचना buf_chunk *chunk = q->शीर्ष;
  size_t len = 0;
  यावत्(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->अग्र;
  }
  प्रत्यागम len;
}

bool Curl_bufq_is_रिक्तानि(ध्रुव संरचना bufq *q)
{
  प्रत्यागम !q->शीर्ष || chunk_is_रिक्तानि(q->शीर्ष);
}

bool Curl_bufq_is_पूर्णम्(ध्रुव संरचना bufq *q)
{
  यदि(!q->पुच्छ || q->अतिरिक्तः)
    प्रत्यागम FALSE;
  यदि(q->chunk_गणनम् < q->max_chunks)
    प्रत्यागम FALSE;
  यदि(q->chunk_गणनम् > q->max_chunks)
    प्रत्यागम TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  प्रत्यागम chunk_is_पूर्णम्(q->पुच्छ);
}

स्थिर संरचना buf_chunk *get_अतिरिक्तः(संरचना bufq *q)
{
  संरचना buf_chunk *chunk = रिक्त;

  यदि(q->अतिरिक्तः) {
    chunk = q->अतिरिक्तः;
    q->अतिरिक्तः = chunk->अग्र;
    chunk_reset(chunk);
    प्रत्यागम chunk;
  }

  यदि(q->chunk_गणनम् >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_सीमा)))
    प्रत्यागम रिक्त;

  यदि(q->pool) {
    यदि(bufcp_take(q->pool, &chunk))
      प्रत्यागम रिक्त;
    ++q->chunk_गणनम्;
    प्रत्यागम chunk;
  }
  अन्यथा {
    /* Check for integer overflow before allocation */
    यदि(q->chunk_आकार > SIZE_MAX - परिमाण(*chunk)) {
      प्रत्यागम रिक्त;
    }

    chunk = curlx_calloc(1, परिमाण(*chunk) + q->chunk_आकार);
    यदि(!chunk)
      प्रत्यागम रिक्त;
    chunk->dlen = q->chunk_आकार;
    ++q->chunk_गणनम्;
    प्रत्यागम chunk;
  }
}

स्थिर शून्य prune_शीर्ष(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  यावत्(q->शीर्ष && chunk_is_रिक्तानि(q->शीर्ष)) {
    chunk = q->शीर्ष;
    q->शीर्ष = chunk->अग्र;
    यदि(q->पुच्छ == chunk)
      q->पुच्छ = q->शीर्ष;
    यदि(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_गणनम्;
    }
    अन्यथा यदि((q->chunk_गणनम् > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_गणनम्;
    }
    अन्यथा {
      chunk->अग्र = q->अतिरिक्तः;
      q->अतिरिक्तः = chunk;
    }
  }
}

स्थिर संरचना buf_chunk *get_non_पूर्णम्_पुच्छ(संरचना bufq *q)
{
  संरचना buf_chunk *chunk;

  यदि(q->पुच्छ && !chunk_is_पूर्णम्(q->पुच्छ))
    प्रत्यागम q->पुच्छ;
  chunk = get_अतिरिक्तः(q);
  यदि(chunk) {
    /* new tail, and possibly new head */
    यदि(q->पुच्छ) {
      q->पुच्छ->अग्र = chunk;
      q->पुच्छ = chunk;
    }
    अन्यथा {
      DEBUGASSERT(!q->शीर्ष);
      q->शीर्ष = q->पुच्छ = chunk;
    }
  }
  प्रत्यागम chunk;
}

CURLcode Curl_bufq_write(संरचना bufq *q,
                         ध्रुव uint8_t *buf, size_t len,
                         size_t *pnwritten)
{
  संरचना buf_chunk *पुच्छ;
  size_t n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  यावत्(len) {
    पुच्छ = get_non_पूर्णम्_पुच्छ(q);
    यदि(!पुच्छ) {
      यदि((q->chunk_गणनम् < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_सीमा))
        /* should have gotten a tail, but did not */
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      भङ्ग;
    }
    n = chunk_append(पुच्छ, buf, len);
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
  यावत्(len && q->शीर्ष) {
    size_t n = chunk_read(q->शीर्ष, buf, len);
    यदि(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_शीर्ष(q);
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
  यदि(q->शीर्ष && chunk_is_रिक्तानि(q->शीर्ष)) {
    prune_शीर्ष(q);
  }
  यदि(q->शीर्ष && !chunk_is_रिक्तानि(q->शीर्ष)) {
    chunk_peek(q->शीर्ष, pbuf, plen);
    प्रत्यागम TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  प्रत्यागम FALSE;
}

bool Curl_bufq_peek_at(संरचना bufq *q, size_t विस्थापनम्,
                       ध्रुव uint8_t **pbuf, size_t *plen)
{
  संरचना buf_chunk *c = q->शीर्ष;
  size_t clen;

  यावत्(c) {
    clen = chunk_len(c);
    यदि(!clen)
      भङ्ग;
    यदि(विस्थापनम् >= clen) {
      विस्थापनम् -= clen;
      c = c->अग्र;
      अनुवर्त;
    }
    chunk_peek_at(c, विस्थापनम्, pbuf, plen);
    प्रत्यागम TRUE;
  }
  *pbuf = रिक्त;
  *plen = 0;
  प्रत्यागम FALSE;
}

शून्य Curl_bufq_skip(संरचना bufq *q, size_t amount)
{
  size_t n;

  यावत्(amount && q->शीर्ष) {
    n = chunk_skip(q->शीर्ष, amount);
    amount -= n;
    prune_शीर्ष(q);
  }
}

CURLcode Curl_bufq_pass(संरचना bufq *q, Curl_bufq_writer *writer,
                        शून्य *writer_ctx, size_t *pwritten)
{
  ध्रुव uint8_t *buf;
  size_t blen;
  CURLcode परिणाम = CURLE_OK;

  *pwritten = 0;
  यावत्(Curl_bufq_peek(q, &buf, &blen)) {
    size_t chunk_written;

    परिणाम = writer(writer_ctx, buf, blen, &chunk_written);
    यदि(परिणाम) {
      यदि((परिणाम == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        परिणाम = CURLE_OK;
      }
      भङ्ग;
    }
    यदि(!chunk_written) {
      यदि(!*pwritten) {
        /* treat as blocked */
        परिणाम = CURLE_AGAIN;
      }
      भङ्ग;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_bufq_write_pass(संरचना bufq *q,
                              ध्रुव uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, शून्य *writer_ctx,
                              size_t *pwritten)
{
  CURLcode परिणाम = CURLE_OK;
  size_t n;

  *pwritten = 0;
  यावत्(len) {
    यदि(Curl_bufq_is_पूर्णम्(q)) {
      /* try to make room in case we are full */
      परिणाम = Curl_bufq_pass(q, writer, writer_ctx, &n);
      यदि(परिणाम) {
        यदि(परिणाम != CURLE_AGAIN) {
          /* real error, fail */
          प्रत्यागम परिणाम;
        }
        /* would block, bufq is full, give up */
        भङ्ग;
      }
    }

    /* Add to bufq as much as there is room for */
    परिणाम = Curl_bufq_write(q, buf, len, &n);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN)
        /* real error, fail */
        प्रत्यागम परिणाम;
      /* result == CURLE_AGAIN */
      यदि(*pwritten)
        /* we did write successfully before */
        परिणाम = CURLE_OK;
      प्रत्यागम परिणाम;
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
  संरचना buf_chunk *पुच्छ = रिक्त;

  *pnread = 0;
  पुच्छ = get_non_पूर्णम्_पुच्छ(q);
  यदि(!पुच्छ) {
    यदि(q->chunk_गणनम् < q->max_chunks)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    /* full, blocked */
    प्रत्यागम CURLE_AGAIN;
  }

  प्रत्यागम chunk_slurpn(पुच्छ, max_len, reader, reader_ctx, pnread);
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
  CURLcode परिणाम;

  *pnread = 0;
  यावत्(1) {
    size_t n;
    परिणाम = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    यदि(परिणाम) {
      यदि(!*pnread || परिणाम != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        प्रत्यागम परिणाम;
      }
      परिणाम = CURLE_OK;
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
    यदि(q->पुच्छ && !chunk_is_पूर्णम्(q->पुच्छ))
      भङ्ग;
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_bufq_slurp(संरचना bufq *q, Curl_bufq_reader *reader,
                         शून्य *reader_ctx, size_t *pnread)
{
  प्रत्यागम bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
