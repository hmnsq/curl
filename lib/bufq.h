#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_BUFQ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_BUFQ_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/**
 * A chunk of bytes for reading and writing.
 * The size is fixed a creation with read and write offset
 * for where unread content is.
 */
ರಚನೆ buf_chunk {
  ರಚನೆ buf_chunk *ಮುಂದಿನ;  /* to keep it in a list */
  size_t dlen;             /* the amount of allocated x.data[] */
  size_t r_ಸ್ಥಾನಾಂತರ;         /* first unread bytes */
  size_t w_ಸ್ಥಾನಾಂತರ;         /* one after last written byte */
  ಒಕ್ಕೂಟ {
    uint8_t ದತ್ತಾಂಶ[1];       /* the buffer for `dlen` bytes */
    ಶೂನ್ಯ *dummy;           /* alignment */
  } x;
};

/**
 * A pool for providing/keeping a number of chunks of the same size
 *
 * The same pool can be shared by many `bufq` instances. A pool is not thread
 * safe. All bufqs using it are supposed to operate in the same thread.
 */
ರಚನೆ bufc_pool {
  ರಚನೆ buf_chunk *ಹೆಚ್ಚುವರಿ;  /* list of available spare chunks */
  size_t chunk_ಗಾತ್ರ;        /* the size of chunks in this pool */
  size_t ಹೆಚ್ಚುವರಿ_ಎಣಿಕೆ;       /* current number of spare chunks in list */
  size_t ಹೆಚ್ಚುವರಿ_max;         /* max number of spares to keep */
};

ಶೂನ್ಯ Curl_bufcp_init(ರಚನೆ bufc_pool *pool,
                     size_t chunk_ಗಾತ್ರ, size_t ಹೆಚ್ಚುವರಿ_max);

ಶೂನ್ಯ Curl_bufcp_free(ರಚನೆ bufc_pool *pool);

/**
 * A queue of byte chunks for reading and writing.
 * Reading is done from `head`, writing is done to `tail`.
 *
 * `bufq`s can be empty or full or neither. Its `len` is the number
 * of bytes that can be read. For an empty bufq, `len` will be 0.
 *
 * By default, a bufq can hold up to `max_chunks * chunk_size` number
 * of bytes. When `max_chunks` are used (in the `head` list) and the
 * `tail` chunk is full, the bufq will report that it is full.
 *
 * On a full bufq, `len` may be less than the maximum number of bytes,
 * e.g. when the head chunk is partially read. `len` may also become
 * larger than the max when option `BUFQ_OPT_SOFT_LIMIT` is used.
 *
 * By default, writing to a full bufq will return (-1, CURLE_AGAIN). Same
 * as reading from an empty bufq.
 * With `BUFQ_OPT_SOFT_LIMIT` set, a bufq will allow writing beyond this limit
 * and use more than `max_chunks`. It will report that it is full
 * nevertheless. This is provided for situation where writes preferably never
 * fail (except for memory exhaustion).
 *
 * By default and without a pool, a bufq will keep chunks that read
 * empty in its `spare` list. Option `BUFQ_OPT_NO_SPARES` will
 * disable that and free chunks once they become empty.
 *
 * When providing a pool to a bufq, all chunk creation and spare handling
 * will be delegated to that pool.
 */
ರಚನೆ bufq {
  ರಚನೆ buf_chunk *ತಲೆ;       /* chunk with bytes to read from */
  ರಚನೆ buf_chunk *ಬಾಲ;       /* chunk to write to */
  ರಚನೆ buf_chunk *ಹೆಚ್ಚುವರಿ;      /* list of free chunks, unless `pool` */
  ರಚನೆ bufc_pool *pool;       /* optional pool for free chunks */
  size_t chunk_ಎಣಿಕೆ;           /* current number of chunks in `head+spare` */
  size_t max_chunks;            /* max `head` chunks to use */
  size_t chunk_ಗಾತ್ರ;            /* size of chunks to manage */
  ಪೂರ್ಣಾಂಕ opts;                     /* options for handling queue, see below */
};

/**
 * Default behaviour: chunk limit is "hard", meaning attempts to write
 * more bytes than can be hold in `max_chunks` is refused and will return
 * -1, CURLE_AGAIN. */
#ವ್ಯಾಖ್ಯಾನಿಸು BUFQ_OPT_NONE        0
/**
 * Make `max_chunks` a "soft" limit. A bufq will report that it is "full"
 * when `max_chunks` are used, but allows writing beyond this limit.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BUFQ_OPT_SOFT_ಎಲ್ಲೆ  (1 << 0)
/**
 * Do not keep spare chunks.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BUFQ_OPT_NO_SPARES   (1 << 1)

/**
 * Initialize a buffer queue that can hold up to `max_chunks` buffers
 * each of size `chunk_size`. The bufq will not allow writing of
 * more bytes than can be held in `max_chunks`.
 */
ಶೂನ್ಯ Curl_bufq_init(ರಚನೆ bufq *q, size_t chunk_ಗಾತ್ರ, size_t max_chunks);

/**
 * Initialize a buffer queue that can hold up to `max_chunks` buffers
 * each of size `chunk_size` with the given options. See `BUFQ_OPT_*`.
 */
ಶೂನ್ಯ Curl_bufq_init2(ರಚನೆ bufq *q, size_t chunk_ಗಾತ್ರ,
                     size_t max_chunks, ಪೂರ್ಣಾಂಕ opts);

ಶೂನ್ಯ Curl_bufq_initp(ರಚನೆ bufq *q, ರಚನೆ bufc_pool *pool,
                     size_t max_chunks, ಪೂರ್ಣಾಂಕ opts);

/**
 * Reset the buffer queue to be empty. Keep any allocated buffer chunks
 * around.
 */
ಶೂನ್ಯ Curl_bufq_reset(ರಚನೆ bufq *q);

/**
 * Free all resources held by the buffer queue.
 */
ಶೂನ್ಯ Curl_bufq_free(ರಚನೆ bufq *q);

/**
 * Return the total amount of data in the queue.
 */
size_t Curl_bufq_len(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q);

/**
 * Returns TRUE iff there is no data in the buffer queue.
 */
bool Curl_bufq_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q);

/**
 * Returns TRUE iff there is no space left in the buffer queue.
 */
bool Curl_bufq_is_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ರಚನೆ bufq *q);

/**
 * Write buf to the end of the buffer queue. The buf is copied
 * and the amount of copied bytes is returned.
 * CURLE_AGAIN is returned if the buffer queue is full.
 */
CURLcode Curl_bufq_write(ರಚನೆ bufq *q,
                         ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                         size_t *pnwritten);

CURLcode Curl_bufq_cwrite(ರಚನೆ bufq *q,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len,
                          size_t *pnwritten);

/**
 * Read buf from the start of the buffer queue. The buf is copied
 * and the amount of copied bytes is returned.
 */
CURLcode Curl_bufq_read(ರಚನೆ bufq *q, uint8_t *buf, size_t len,
                        size_t *pnread);

CURLcode Curl_bufq_cread(ರಚನೆ bufq *q, ಅಕ್ಷರ *buf, size_t len,
                         size_t *pnread);

/**
 * Peek at the head chunk in the buffer queue. Returns a pointer to
 * the chunk buf (at the current offset) and its length. Does not
 * modify the buffer queue.
 * Returns TRUE iff bytes are available. Sets `pbuf` to NULL and `plen`
 * to 0 when no bytes are available.
 * Repeated calls return the same information until the buffer queue
 * is modified, see `Curl_bufq_skip()``
 */
bool Curl_bufq_peek(ರಚನೆ bufq *q,
                    ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen);

bool Curl_bufq_peek_at(ರಚನೆ bufq *q, size_t ಸ್ಥಾನಾಂತರ,
                       ಸ್ಥಿರಾಂಕ uint8_t **pbuf, size_t *plen);

/**
 * Tell the buffer queue to discard `amount` buf bytes at the head of the
 * queue. Skipping more buf than is currently buffered will empty the queue.
 */
ಶೂನ್ಯ Curl_bufq_skip(ರಚನೆ bufq *q, size_t amount);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_bufq_writer(ಶೂನ್ಯ *writer_ctx,
                                  ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                                  size_t *pwritten);

/**
 * Passes the chunks in the buffer queue to the writer and returns
 * the amount of buf written. A writer may return -1 and CURLE_AGAIN
 * to indicate blocking at which point the queue will stop and return
 * the amount of buf passed so far.
 * -1 is returned on any other errors reported by the writer.
 * Note that in case of a -1 chunks may have been written and
 * the buffer queue will have different length than before.
 */
CURLcode Curl_bufq_pass(ರಚನೆ bufq *q, Curl_bufq_writer *writer,
                        ಶೂನ್ಯ *writer_ctx, size_t *pwritten);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_bufq_reader(ಶೂನ್ಯ *reader_ctx,
                                  uint8_t *buf, size_t len,
                                  size_t *pnread);

/**
 * Read bytes and append them to the end of the buffer queue until the
 * reader returns blocking or the queue is full. A reader returns
 * CURLE_AGAIN to indicate blocking.
 * Returns the total amount of buf read (may be 0) in `pnread` on success.
 * Note that in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
CURLcode Curl_bufq_slurp(ರಚನೆ bufq *q, Curl_bufq_reader *reader,
                         ಶೂನ್ಯ *reader_ctx, size_t *pnread);

/**
 * Read *once* up to `max_len` bytes and append it to the buffer.
 * if `max_len` is 0, no limit is imposed besides the chunk space.
 * Returns the total amount of buf read (may be 0) or -1 on other
 * reader errors.
 */
CURLcode Curl_bufq_sipn(ರಚನೆ bufq *q, size_t max_len,
                        Curl_bufq_reader *reader, ಶೂನ್ಯ *reader_ctx,
                        size_t *pnread);

/**
 * Write buf to the end of the buffer queue.
 * Write bufq content or passed `buf` directly using the `writer`
 * callback when it sees fit. 'buf' might get passed directly
 * on or is placed into the buffer, depending on `len` and current
 * amount buffered, chunk size, etc.
 */
CURLcode Curl_bufq_write_pass(ರಚನೆ bufq *q,
                              ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                              Curl_bufq_writer *writer, ಶೂನ್ಯ *writer_ctx,
                              size_t *pwritten);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_BUFQ_H */
