#अगर_अघोषित HEADER_CURL_BUFQ_H
#घोषणा HEADER_CURL_BUFQ_H
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

/**
 * A chunk of bytes for reading and writing.
 * The size is fixed a creation with read and write offset
 * for where unread content is.
 */
काष्ठा buf_chunk {
  काष्ठा buf_chunk *अग्र;  /* to keep it in a list */
  माप_प्रकार dlen;             /* the amount of allocated x.data[] */
  माप_प्रकार r_offset;         /* first unread bytes */
  माप_प्रकार w_offset;         /* one after last written byte */
  जोड़ {
    uint8_t तथ्य[1];       /* the buffer for `dlen` bytes */
    व्योम *dummy;           /* alignment */
  } x;
};

/**
 * A pool for providing/keeping a number of chunks of the same size
 *
 * The same pool can be shared by many `bufq` instances. A pool is not thread
 * safe. All bufqs using it are supposed to operate in the same thread.
 */
काष्ठा bufc_pool {
  काष्ठा buf_chunk *spare;  /* list of available spare chunks */
  माप_प्रकार chunk_आकार;        /* the size of chunks in this pool */
  माप_प्रकार spare_गिनती;       /* current number of spare chunks in list */
  माप_प्रकार spare_max;         /* max number of spares to keep */
};

व्योम Curl_bufcp_init(काष्ठा bufc_pool *pool,
                     माप_प्रकार chunk_आकार, माप_प्रकार spare_max);

व्योम Curl_bufcp_free(काष्ठा bufc_pool *pool);

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
काष्ठा bufq {
  काष्ठा buf_chunk *शीर्ष;       /* chunk with bytes to read from */
  काष्ठा buf_chunk *पुच्छ;       /* chunk to write to */
  काष्ठा buf_chunk *spare;      /* list of free chunks, unless `pool` */
  काष्ठा bufc_pool *pool;       /* optional pool for free chunks */
  माप_प्रकार chunk_गिनती;           /* current number of chunks in `head+spare` */
  माप_प्रकार max_chunks;            /* max `head` chunks to use */
  माप_प्रकार chunk_आकार;            /* size of chunks to manage */
  पूर्णांक opts;                     /* options for handling queue, see below */
};

/**
 * Default behaviour: chunk limit is "hard", meaning attempts to write
 * more bytes than can be hold in `max_chunks` is refused and will return
 * -1, CURLE_AGAIN. */
#घोषणा BUFQ_OPT_NONE        0
/**
 * Make `max_chunks` a "soft" limit. A bufq will report that it is "full"
 * when `max_chunks` are used, but allows writing beyond this limit.
 */
#घोषणा BUFQ_OPT_SOFT_LIMIT  (1 << 0)
/**
 * Do not keep spare chunks.
 */
#घोषणा BUFQ_OPT_NO_SPARES   (1 << 1)

/**
 * Initialize a buffer queue that can hold up to `max_chunks` buffers
 * each of size `chunk_size`. The bufq will not allow writing of
 * more bytes than can be held in `max_chunks`.
 */
व्योम Curl_bufq_init(काष्ठा bufq *q, माप_प्रकार chunk_आकार, माप_प्रकार max_chunks);

/**
 * Initialize a buffer queue that can hold up to `max_chunks` buffers
 * each of size `chunk_size` with the given options. See `BUFQ_OPT_*`.
 */
व्योम Curl_bufq_init2(काष्ठा bufq *q, माप_प्रकार chunk_आकार,
                     माप_प्रकार max_chunks, पूर्णांक opts);

व्योम Curl_bufq_initp(काष्ठा bufq *q, काष्ठा bufc_pool *pool,
                     माप_प्रकार max_chunks, पूर्णांक opts);

/**
 * Reset the buffer queue to be empty. Keep any allocated buffer chunks
 * around.
 */
व्योम Curl_bufq_reset(काष्ठा bufq *q);

/**
 * Free all resources held by the buffer queue.
 */
व्योम Curl_bufq_free(काष्ठा bufq *q);

/**
 * Return the total amount of data in the queue.
 */
माप_प्रकार Curl_bufq_len(स्थिर काष्ठा bufq *q);

/**
 * Returns TRUE iff there is no data in the buffer queue.
 */
bool Curl_bufq_is_empty(स्थिर काष्ठा bufq *q);

/**
 * Returns TRUE iff there is no space left in the buffer queue.
 */
bool Curl_bufq_is_full(स्थिर काष्ठा bufq *q);

/**
 * Write buf to the end of the buffer queue. The buf is copied
 * and the amount of copied bytes is returned.
 * CURLE_AGAIN is returned if the buffer queue is full.
 */
CURLcode Curl_bufq_write(काष्ठा bufq *q,
                         स्थिर uint8_t *buf, माप_प्रकार len,
                         माप_प्रकार *pnwritten);

CURLcode Curl_bufq_cwrite(काष्ठा bufq *q,
                          स्थिर अक्षर *buf, माप_प्रकार len,
                          माप_प्रकार *pnwritten);

/**
 * Read buf from the start of the buffer queue. The buf is copied
 * and the amount of copied bytes is returned.
 */
CURLcode Curl_bufq_read(काष्ठा bufq *q, uint8_t *buf, माप_प्रकार len,
                        माप_प्रकार *pnread);

CURLcode Curl_bufq_cread(काष्ठा bufq *q, अक्षर *buf, माप_प्रकार len,
                         माप_प्रकार *pnread);

/**
 * Peek at the head chunk in the buffer queue. Returns a pointer to
 * the chunk buf (at the current offset) and its length. Does not
 * modify the buffer queue.
 * Returns TRUE iff bytes are available. Sets `pbuf` to NULL and `plen`
 * to 0 when no bytes are available.
 * Repeated calls return the same information until the buffer queue
 * is modified, see `Curl_bufq_skip()``
 */
bool Curl_bufq_peek(काष्ठा bufq *q,
                    स्थिर uint8_t **pbuf, माप_प्रकार *plen);

bool Curl_bufq_peek_at(काष्ठा bufq *q, माप_प्रकार offset,
                       स्थिर uint8_t **pbuf, माप_प्रकार *plen);

/**
 * Tell the buffer queue to discard `amount` buf bytes at the head of the
 * queue. Skipping more buf than is currently buffered will empty the queue.
 */
व्योम Curl_bufq_skip(काष्ठा bufq *q, माप_प्रकार amount);

प्रकार CURLcode Curl_bufq_writer(व्योम *writer_ctx,
                                  स्थिर uint8_t *buf, माप_प्रकार len,
                                  माप_प्रकार *pwritten);

/**
 * Passes the chunks in the buffer queue to the writer and returns
 * the amount of buf written. A writer may return -1 and CURLE_AGAIN
 * to indicate blocking at which point the queue will stop and return
 * the amount of buf passed so far.
 * -1 is returned on any other errors reported by the writer.
 * Note that in case of a -1 chunks may have been written and
 * the buffer queue will have different length than before.
 */
CURLcode Curl_bufq_pass(काष्ठा bufq *q, Curl_bufq_writer *writer,
                        व्योम *writer_ctx, माप_प्रकार *pwritten);

प्रकार CURLcode Curl_bufq_reader(व्योम *reader_ctx,
                                  uint8_t *buf, माप_प्रकार len,
                                  माप_प्रकार *pnread);

/**
 * Read bytes and append them to the end of the buffer queue until the
 * reader returns blocking or the queue is full. A reader returns
 * CURLE_AGAIN to indicate blocking.
 * Returns the total amount of buf read (may be 0) in `pnread` on success.
 * Note that in case of an error chunks may have been read and
 * the buffer queue will have different length than before.
 */
CURLcode Curl_bufq_slurp(काष्ठा bufq *q, Curl_bufq_reader *reader,
                         व्योम *reader_ctx, माप_प्रकार *pnread);

/**
 * Read *once* up to `max_len` bytes and append it to the buffer.
 * if `max_len` is 0, no limit is imposed besides the chunk space.
 * Returns the total amount of buf read (may be 0) or -1 on other
 * reader errors.
 */
CURLcode Curl_bufq_sipn(काष्ठा bufq *q, माप_प्रकार max_len,
                        Curl_bufq_reader *reader, व्योम *reader_ctx,
                        माप_प्रकार *pnread);

/**
 * Write buf to the end of the buffer queue.
 * Write bufq content or passed `buf` directly using the `writer`
 * callback when it sees fit. 'buf' might get passed directly
 * on or is placed into the buffer, depending on `len` and current
 * amount buffered, chunk size, etc.
 */
CURLcode Curl_bufq_write_pass(काष्ठा bufq *q,
                              स्थिर uint8_t *buf, माप_प्रकार len,
                              Curl_bufq_writer *writer, व्योम *writer_ctx,
                              माप_प्रकार *pwritten);

#पूर्ण_अगर /* HEADER_CURL_BUFQ_H */
