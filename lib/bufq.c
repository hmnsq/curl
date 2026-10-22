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
#समावेश "curl_setup.h"

#समावेश "bufq.h"

अटल bool chunk_is_empty(स्थिर काष्ठा buf_chunk *chunk)
{
  वापस chunk->r_offset >= chunk->w_offset;
}

अटल bool chunk_is_full(स्थिर काष्ठा buf_chunk *chunk)
{
  वापस chunk->w_offset >= chunk->dlen;
}

अटल माप_प्रकार chunk_len(स्थिर काष्ठा buf_chunk *chunk)
{
  वापस chunk->w_offset - chunk->r_offset;
}

अटल व्योम chunk_reset(काष्ठा buf_chunk *chunk)
{
  chunk->अग्र = शून्य;
  chunk->r_offset = chunk->w_offset = 0;
}

अटल माप_प्रकार chunk_append(काष्ठा buf_chunk *chunk,
                           स्थिर uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.तथ्य[chunk->w_offset];
  माप_प्रकार n = chunk->dlen - chunk->w_offset;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  अगर(n) {
    n = CURLMIN(n, len);
    स_नकल(p, buf, n);
    chunk->w_offset += n;
  }
  वापस n;
}

अटल माप_प्रकार chunk_read(काष्ठा buf_chunk *chunk,
                         uint8_t *buf, माप_प्रकार len)
{
  uint8_t *p = &chunk->x.तथ्य[chunk->r_offset];
  माप_प्रकार n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  अगर(!n) {
    वापस 0;
  }
  अन्यथा अगर(n <= len) {
    स_नकल(buf, p, n);
    chunk->r_offset = chunk->w_offset = 0;
    वापस n;
  }
  अन्यथा {
    स_नकल(buf, p, len);
    chunk->r_offset += len;
    वापस len;
  }
}

अटल CURLcode chunk_slurpn(काष्ठा buf_chunk *chunk, माप_प्रकार max_len,
                             Curl_bufq_reader *reader,
                             व्योम *reader_ctx, माप_प्रकार *pnread)
{
  uint8_t *p = &chunk->x.तथ्य[chunk->w_offset];
  माप_प्रकार n = chunk->dlen - chunk->w_offset; /* free amount */
  CURLcode परिणाम;

  *pnread = 0;
  DEBUGASSERT(chunk->dlen >= chunk->w_offset);
  अगर(!n)
    वापस CURLE_AGAIN;
  अगर(max_len && n > max_len)
    n = max_len;
  परिणाम = reader(reader_ctx, p, n, pnread);
  अगर(!परिणाम) {
    DEBUGASSERT(*pnread <= n);
    chunk->w_offset += *pnread;
  }
  वापस परिणाम;
}

अटल व्योम chunk_peek(स्थिर काष्ठा buf_chunk *chunk,
                       स्थिर uint8_t **pbuf, माप_प्रकार *plen)
{
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  *pbuf = &chunk->x.तथ्य[chunk->r_offset];
  *plen = chunk->w_offset - chunk->r_offset;
}

अटल व्योम chunk_peek_at(स्थिर काष्ठा buf_chunk *chunk, माप_प्रकार offset,
                          स्थिर uint8_t **pbuf, माप_प्रकार *plen)
{
  offset += chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= offset);
  *pbuf = &chunk->x.तथ्य[offset];
  *plen = chunk->w_offset - offset;
}

अटल माप_प्रकार chunk_skip(काष्ठा buf_chunk *chunk, माप_प्रकार amount)
{
  माप_प्रकार n = chunk->w_offset - chunk->r_offset;
  DEBUGASSERT(chunk->w_offset >= chunk->r_offset);
  अगर(n) {
    n = CURLMIN(n, amount);
    chunk->r_offset += n;
    अगर(chunk->r_offset == chunk->w_offset)
      chunk->r_offset = chunk->w_offset = 0;
  }
  वापस n;
}

अटल व्योम chunk_list_free(काष्ठा buf_chunk **anchor)
{
  काष्ठा buf_chunk *chunk;
  जबतक(*anchor) {
    chunk = *anchor;
    *anchor = chunk->अग्र;
    curlx_free(chunk);
  }
}

व्योम Curl_bufcp_init(काष्ठा bufc_pool *pool,
                     माप_प्रकार chunk_आकार, माप_प्रकार spare_max)
{
  DEBUGASSERT(chunk_आकार > 0);
  DEBUGASSERT(spare_max > 0);
  स_रखो(pool, 0, माप(*pool));
  pool->chunk_आकार = chunk_आकार;
  pool->spare_max = spare_max;
}

अटल CURLcode bufcp_take(काष्ठा bufc_pool *pool,
                           काष्ठा buf_chunk **pchunk)
{
  काष्ठा buf_chunk *chunk = शून्य;

  अगर(pool->spare) {
    chunk = pool->spare;
    pool->spare = chunk->अग्र;
    --pool->spare_गिनती;
    chunk_reset(chunk);
    *pchunk = chunk;
    वापस CURLE_OK;
  }

  /* Check for integer overflow before allocation */
  अगर(pool->chunk_आकार > SIZE_MAX - माप(*chunk)) {
    *pchunk = शून्य;
    वापस CURLE_OUT_OF_MEMORY;
  }

  chunk = curlx_calloc(1, माप(*chunk) + pool->chunk_आकार);
  अगर(!chunk) {
    *pchunk = शून्य;
    वापस CURLE_OUT_OF_MEMORY;
  }
  chunk->dlen = pool->chunk_आकार;
  *pchunk = chunk;
  वापस CURLE_OK;
}

अटल व्योम bufcp_put(काष्ठा bufc_pool *pool,
                      काष्ठा buf_chunk *chunk)
{
  अगर(pool->spare_गिनती >= pool->spare_max) {
    curlx_free(chunk);
  }
  अन्यथा {
    chunk_reset(chunk);
    chunk->अग्र = pool->spare;
    pool->spare = chunk;
    ++pool->spare_गिनती;
  }
}

व्योम Curl_bufcp_free(काष्ठा bufc_pool *pool)
{
  chunk_list_free(&pool->spare);
  pool->spare_गिनती = 0;
}

अटल व्योम bufq_init(काष्ठा bufq *q, काष्ठा bufc_pool *pool,
                      माप_प्रकार chunk_आकार, माप_प्रकार max_chunks, पूर्णांक opts)
{
  DEBUGASSERT(chunk_आकार > 0);
  DEBUGASSERT(max_chunks > 0);
  स_रखो(q, 0, माप(*q));
  q->chunk_आकार = chunk_आकार;
  q->max_chunks = max_chunks;
  q->pool = pool;
  q->opts = opts;
}

व्योम Curl_bufq_init2(काष्ठा bufq *q, माप_प्रकार chunk_आकार, माप_प्रकार max_chunks,
                     पूर्णांक opts)
{
  bufq_init(q, शून्य, chunk_आकार, max_chunks, opts);
}

व्योम Curl_bufq_init(काष्ठा bufq *q, माप_प्रकार chunk_आकार, माप_प्रकार max_chunks)
{
  bufq_init(q, शून्य, chunk_आकार, max_chunks, BUFQ_OPT_NONE);
}

व्योम Curl_bufq_initp(काष्ठा bufq *q, काष्ठा bufc_pool *pool,
                     माप_प्रकार max_chunks, पूर्णांक opts)
{
  bufq_init(q, pool, pool->chunk_आकार, max_chunks, opts);
}

व्योम Curl_bufq_free(काष्ठा bufq *q)
{
  chunk_list_free(&q->शीर्ष);
  chunk_list_free(&q->spare);
  q->पुच्छ = शून्य;
  q->chunk_गिनती = 0;
}

व्योम Curl_bufq_reset(काष्ठा bufq *q)
{
  काष्ठा buf_chunk *chunk;
  जबतक(q->शीर्ष) {
    chunk = q->शीर्ष;
    q->शीर्ष = chunk->अग्र;
    chunk->अग्र = q->spare;
    q->spare = chunk;
  }
  q->पुच्छ = शून्य;
}

माप_प्रकार Curl_bufq_len(स्थिर काष्ठा bufq *q)
{
  स्थिर काष्ठा buf_chunk *chunk = q->शीर्ष;
  माप_प्रकार len = 0;
  जबतक(chunk) {
    len += chunk_len(chunk);
    chunk = chunk->अग्र;
  }
  वापस len;
}

bool Curl_bufq_is_empty(स्थिर काष्ठा bufq *q)
{
  वापस !q->शीर्ष || chunk_is_empty(q->शीर्ष);
}

bool Curl_bufq_is_full(स्थिर काष्ठा bufq *q)
{
  अगर(!q->पुच्छ || q->spare)
    वापस FALSE;
  अगर(q->chunk_गिनती < q->max_chunks)
    वापस FALSE;
  अगर(q->chunk_गिनती > q->max_chunks)
    वापस TRUE;
  /* we have no spares and cannot make more, is the tail full? */
  वापस chunk_is_full(q->पुच्छ);
}

अटल काष्ठा buf_chunk *get_spare(काष्ठा bufq *q)
{
  काष्ठा buf_chunk *chunk = शून्य;

  अगर(q->spare) {
    chunk = q->spare;
    q->spare = chunk->अग्र;
    chunk_reset(chunk);
    वापस chunk;
  }

  अगर(q->chunk_गिनती >= q->max_chunks && (!(q->opts & BUFQ_OPT_SOFT_LIMIT)))
    वापस शून्य;

  अगर(q->pool) {
    अगर(bufcp_take(q->pool, &chunk))
      वापस शून्य;
    ++q->chunk_गिनती;
    वापस chunk;
  }
  अन्यथा {
    /* Check for integer overflow before allocation */
    अगर(q->chunk_आकार > SIZE_MAX - माप(*chunk)) {
      वापस शून्य;
    }

    chunk = curlx_calloc(1, माप(*chunk) + q->chunk_आकार);
    अगर(!chunk)
      वापस शून्य;
    chunk->dlen = q->chunk_आकार;
    ++q->chunk_गिनती;
    वापस chunk;
  }
}

अटल व्योम prune_शीर्ष(काष्ठा bufq *q)
{
  काष्ठा buf_chunk *chunk;

  जबतक(q->शीर्ष && chunk_is_empty(q->शीर्ष)) {
    chunk = q->शीर्ष;
    q->शीर्ष = chunk->अग्र;
    अगर(q->पुच्छ == chunk)
      q->पुच्छ = q->शीर्ष;
    अगर(q->pool) {
      bufcp_put(q->pool, chunk);
      --q->chunk_गिनती;
    }
    अन्यथा अगर((q->chunk_गिनती > q->max_chunks) ||
            (q->opts & BUFQ_OPT_NO_SPARES)) {
      /* SOFT_LIMIT allowed us more than max. free spares until
       * we are at max again. Or free them if we are configured
       * to not use spares. */
      curlx_free(chunk);
      --q->chunk_गिनती;
    }
    अन्यथा {
      chunk->अग्र = q->spare;
      q->spare = chunk;
    }
  }
}

अटल काष्ठा buf_chunk *get_non_full_पुच्छ(काष्ठा bufq *q)
{
  काष्ठा buf_chunk *chunk;

  अगर(q->पुच्छ && !chunk_is_full(q->पुच्छ))
    वापस q->पुच्छ;
  chunk = get_spare(q);
  अगर(chunk) {
    /* new tail, and possibly new head */
    अगर(q->पुच्छ) {
      q->पुच्छ->अग्र = chunk;
      q->पुच्छ = chunk;
    }
    अन्यथा {
      DEBUGASSERT(!q->शीर्ष);
      q->शीर्ष = q->पुच्छ = chunk;
    }
  }
  वापस chunk;
}

CURLcode Curl_bufq_write(काष्ठा bufq *q,
                         स्थिर uint8_t *buf, माप_प्रकार len,
                         माप_प्रकार *pnwritten)
{
  काष्ठा buf_chunk *पुच्छ;
  माप_प्रकार n;

  DEBUGASSERT(q->max_chunks > 0);
  *pnwritten = 0;
  जबतक(len) {
    पुच्छ = get_non_full_पुच्छ(q);
    अगर(!पुच्छ) {
      अगर((q->chunk_गिनती < q->max_chunks) || (q->opts & BUFQ_OPT_SOFT_LIMIT))
        /* should have gotten a tail, but did not */
        वापस CURLE_OUT_OF_MEMORY;
      अवरोध;
    }
    n = chunk_append(पुच्छ, buf, len);
    अगर(!n)
      अवरोध;
    *pnwritten += n;
    buf += n;
    len -= n;
  }
  वापस (!*pnwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cwrite(काष्ठा bufq *q,
                          स्थिर अक्षर *buf, माप_प्रकार len,
                          माप_प्रकार *pnwritten)
{
  वापस Curl_bufq_write(q, (स्थिर uint8_t *)buf, len, pnwritten);
}

CURLcode Curl_bufq_read(काष्ठा bufq *q, uint8_t *buf, माप_प्रकार len,
                        माप_प्रकार *pnread)
{
  *pnread = 0;
  जबतक(len && q->शीर्ष) {
    माप_प्रकार n = chunk_read(q->शीर्ष, buf, len);
    अगर(n) {
      *pnread += n;
      buf += n;
      len -= n;
    }
    prune_शीर्ष(q);
  }
  वापस (!*pnread) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_cread(काष्ठा bufq *q, अक्षर *buf, माप_प्रकार len,
                         माप_प्रकार *pnread)
{
  वापस Curl_bufq_read(q, (uint8_t *)buf, len, pnread);
}

bool Curl_bufq_peek(काष्ठा bufq *q,
                    स्थिर uint8_t **pbuf, माप_प्रकार *plen)
{
  अगर(q->शीर्ष && chunk_is_empty(q->शीर्ष)) {
    prune_शीर्ष(q);
  }
  अगर(q->शीर्ष && !chunk_is_empty(q->शीर्ष)) {
    chunk_peek(q->शीर्ष, pbuf, plen);
    वापस TRUE;
  }
  *pbuf = शून्य;
  *plen = 0;
  वापस FALSE;
}

bool Curl_bufq_peek_at(काष्ठा bufq *q, माप_प्रकार offset,
                       स्थिर uint8_t **pbuf, माप_प्रकार *plen)
{
  काष्ठा buf_chunk *c = q->शीर्ष;
  माप_प्रकार clen;

  जबतक(c) {
    clen = chunk_len(c);
    अगर(!clen)
      अवरोध;
    अगर(offset >= clen) {
      offset -= clen;
      c = c->अग्र;
      जारी;
    }
    chunk_peek_at(c, offset, pbuf, plen);
    वापस TRUE;
  }
  *pbuf = शून्य;
  *plen = 0;
  वापस FALSE;
}

व्योम Curl_bufq_skip(काष्ठा bufq *q, माप_प्रकार amount)
{
  माप_प्रकार n;

  जबतक(amount && q->शीर्ष) {
    n = chunk_skip(q->शीर्ष, amount);
    amount -= n;
    prune_शीर्ष(q);
  }
}

CURLcode Curl_bufq_pass(काष्ठा bufq *q, Curl_bufq_writer *writer,
                        व्योम *writer_ctx, माप_प्रकार *pwritten)
{
  स्थिर uint8_t *buf;
  माप_प्रकार blen;
  CURLcode परिणाम = CURLE_OK;

  *pwritten = 0;
  जबतक(Curl_bufq_peek(q, &buf, &blen)) {
    माप_प्रकार chunk_written;

    परिणाम = writer(writer_ctx, buf, blen, &chunk_written);
    अगर(परिणाम) {
      अगर((परिणाम == CURLE_AGAIN) && *pwritten) {
        /* blocked on subsequent write, report success */
        परिणाम = CURLE_OK;
      }
      अवरोध;
    }
    अगर(!chunk_written) {
      अगर(!*pwritten) {
        /* treat as blocked */
        परिणाम = CURLE_AGAIN;
      }
      अवरोध;
    }
    *pwritten += chunk_written;
    Curl_bufq_skip(q, chunk_written);
  }
  वापस परिणाम;
}

CURLcode Curl_bufq_write_pass(काष्ठा bufq *q,
                              स्थिर uint8_t *buf, माप_प्रकार len,
                              Curl_bufq_writer *writer, व्योम *writer_ctx,
                              माप_प्रकार *pwritten)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार n;

  *pwritten = 0;
  जबतक(len) {
    अगर(Curl_bufq_is_full(q)) {
      /* try to make room in case we are full */
      परिणाम = Curl_bufq_pass(q, writer, writer_ctx, &n);
      अगर(परिणाम) {
        अगर(परिणाम != CURLE_AGAIN) {
          /* real error, fail */
          वापस परिणाम;
        }
        /* would block, bufq is full, give up */
        अवरोध;
      }
    }

    /* Add to bufq as much as there is room for */
    परिणाम = Curl_bufq_write(q, buf, len, &n);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN)
        /* real error, fail */
        वापस परिणाम;
      /* result == CURLE_AGAIN */
      अगर(*pwritten)
        /* we did write successfully before */
        परिणाम = CURLE_OK;
      वापस परिणाम;
    }
    अन्यथा अगर(n == 0)
      /* edge case of writer returning 0 (and len is >0)
       * break or we might enter an infinite loop here */
      अवरोध;

    /* Track what we added to bufq */
    buf += n;
    len -= n;
    *pwritten += n;
  }

  वापस (!*pwritten && len) ? CURLE_AGAIN : CURLE_OK;
}

CURLcode Curl_bufq_sipn(काष्ठा bufq *q, माप_प्रकार max_len,
                        Curl_bufq_reader *reader, व्योम *reader_ctx,
                        माप_प्रकार *pnread)
{
  काष्ठा buf_chunk *पुच्छ = शून्य;

  *pnread = 0;
  पुच्छ = get_non_full_पुच्छ(q);
  अगर(!पुच्छ) {
    अगर(q->chunk_गिनती < q->max_chunks)
      वापस CURLE_OUT_OF_MEMORY;
    /* full, blocked */
    वापस CURLE_AGAIN;
  }

  वापस chunk_slurpn(पुच्छ, max_len, reader, reader_ctx, pnread);
}

/**
 * Read up to `max_len` bytes and append it to the end of the buffer queue.
 * if `max_len` is 0, no limit is imposed and the call behaves exactly
 * the same as `Curl_bufq_slurp()`.
 * Returns the total amount of buf read (may be 0) in `pnread` or error
 * Note that even in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
अटल CURLcode bufq_slurpn(काष्ठा bufq *q, माप_प्रकार max_len,
                            Curl_bufq_reader *reader, व्योम *reader_ctx,
                            माप_प्रकार *pnread)
{
  CURLcode परिणाम;

  *pnread = 0;
  जबतक(1) {
    माप_प्रकार n;
    परिणाम = Curl_bufq_sipn(q, max_len, reader, reader_ctx, &n);
    अगर(परिणाम) {
      अगर(!*pnread || परिणाम != CURLE_AGAIN) {
        /* blocked on first read or real error, fail */
        वापस परिणाम;
      }
      परिणाम = CURLE_OK;
      अवरोध;
    }
    अन्यथा अगर(n == 0) {
      /* eof, result remains CURLE_OK */
      अवरोध;
    }
    *pnread += n;
    अगर(max_len) {
      DEBUGASSERT(n <= max_len);
      max_len -= n;
      अगर(!max_len)
        अवरोध;
    }
    /* give up slurping when we get less bytes than we asked for */
    अगर(q->पुच्छ && !chunk_is_full(q->पुच्छ))
      अवरोध;
  }
  वापस परिणाम;
}

CURLcode Curl_bufq_slurp(काष्ठा bufq *q, Curl_bufq_reader *reader,
                         व्योम *reader_ctx, माप_प्रकार *pnread)
{
  वापस bufq_slurpn(q, 0, reader, reader_ctx, pnread);
}
