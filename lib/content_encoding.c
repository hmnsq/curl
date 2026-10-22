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
#include "curl_setup.h"

#include "urldata.h"
#include "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
#endif
#include <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
#endif

#ifdef HAVE_ZSTD
#include <zstd.h>
#endif

#include "connect.h"
#include "sendf.h"
#include "curl_trc.h"
#include "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_STACK 5

#만약 defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#만약 !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

typedef enum {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
struct zlib_writer {
  struct Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  char buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

static voidpf zalloc_cb(voidpf opaque, unsigned 정수 items, unsigned 정수 size)
{
  (void)opaque;
  /* not a typo, keep it curlx_calloc() */
  반환 curlx_calloc(items, size);
}

static void zfree_cb(voidpf opaque, voidpf ptr)
{
  (void)opaque;
  curlx_free(ptr);
}

static CURLcode process_zlib_error(struct Curl_easy *data, z_stream *z)
{
  만약(z->msg)
    failf(data, "Error while processing content unencoding: %s", z->msg);
  아니면
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  반환 CURLE_BAD_CONTENT_ENCODING;
}

static CURLcode exit_zlib(struct Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  만약(*zlib_init != ZLIB_UNINIT) {
    만약(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  반환 result;
}

static CURLcode process_trailer(struct Curl_easy *data, struct zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  만약(z->avail_in)
    result = CURLE_WRITE_ERROR;
  만약(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  아니면 {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  반환 result;
}

static CURLcode inflate_stream(struct Curl_easy *data,
                               struct Curl_cwriter *writer, 정수 type,
                               zlibInitState started)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  정수 i = 0;

  /* Check state. */
  만약(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    반환 exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  동안(!done) {
    정수 status; /* zlib status */
    done = TRUE;

    만약(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      만약(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        반환 exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    만약(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      만약(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        만약(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          break;
        }
      }
    }

    /* Dispatch by inflate() status. */
    switch(status) {
    case Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      break;
    case Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      break;
    case Z_STREAM_END:
      만약((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        break;
      }
      result = process_trailer(data, zp);
      break;
    case Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      만약(zp->zlib_init == ZLIB_INIT) {
        만약(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          break;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      break;
    default:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      break;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  만약(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  반환 result;
}

/* Deflate handler. */
static CURLcode deflate_do_init(struct Curl_easy *data,
                                struct Curl_cwriter *writer)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  만약(inflateInit(z) != Z_OK)
    반환 process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  반환 CURLE_OK;
}

static CURLcode deflate_do_write(struct Curl_easy *data,
                                 struct Curl_cwriter *writer, 정수 type,
                                 const char *buf, size_t nbytes)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  만약(!(type & CLIENTWRITE_BODY) || !nbytes)
    반환 Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  만약(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    반환 process_trailer(data, zp);

  /* Now uncompress the data */
  반환 inflate_stream(data, writer, type, ZLIB_INFLATING);
}

static void deflate_do_close(struct Curl_easy *data,
                             struct Curl_cwriter *writer)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

static const struct Curl_cwtype deflate_encoding = {
  "deflate",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  sizeof(struct zlib_writer)
};

/*
 * Gzip handler.
 */

static CURLcode gzip_do_init(struct Curl_easy *data,
                             struct Curl_cwriter *writer)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  만약(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    반환 process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  반환 CURLE_OK;
}

static CURLcode gzip_do_write(struct Curl_easy *data,
                              struct Curl_cwriter *writer, 정수 type,
                              const char *buf, size_t nbytes)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  만약(!(type & CLIENTWRITE_BODY) || !nbytes)
    반환 Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  만약(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    반환 inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  반환 exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

static void gzip_do_close(struct Curl_easy *data,
                          struct Curl_cwriter *writer)
{
  struct zlib_writer *zp = (struct zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

static const struct Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  sizeof(struct zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
struct brotli_writer {
  struct Curl_cwriter super;
  char buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

static CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  switch(be) {
  case BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  case BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  case BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  case BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  case BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  case BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  case BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  case BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  case BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  case BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  case BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  case BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  case BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  case BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  case BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  case BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  case BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    반환 CURLE_BAD_CONTENT_ENCODING;
  case BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  case BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS:
  case BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  case BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  case BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  case BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    반환 CURLE_OUT_OF_MEMORY;
  default:
    break;
  }
  반환 CURLE_WRITE_ERROR;
}

static CURLcode brotli_do_init(struct Curl_easy *data,
                               struct Curl_cwriter *writer)
{
  struct brotli_writer *bp = (struct brotli_writer *)writer;
  (void)data;

  bp->br = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  반환 bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode brotli_do_write(struct Curl_easy *data,
                                struct Curl_cwriter *writer, 정수 type,
                                const char *buf, size_t nbytes)
{
  struct brotli_writer *bp = (struct brotli_writer *)writer;
  const uint8_t *src = (const uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  정수 i = 0;

  만약(!(type & CLIENTWRITE_BODY) || !nbytes)
    반환 Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  만약(!bp->br)
    반환 CURLE_WRITE_ERROR; /* Stream already ended. */

  동안((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    만약(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      만약(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        반환 CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, NULL);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    만약(result)
      break;
    switch(r) {
    case BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    case BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT:
      break;
    case BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = NULL;
      만약(nbytes)
        result = CURLE_WRITE_ERROR;
      break;
    default:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      break;
    }
  }
  반환 result;
}

static void brotli_do_close(struct Curl_easy *data,
                            struct Curl_cwriter *writer)
{
  struct brotli_writer *bp = (struct brotli_writer *)writer;
  (void)data;

  만약(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = NULL;
  }
}

static const struct Curl_cwtype brotli_encoding = {
  "br",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  sizeof(struct brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
struct zstd_writer {
  struct Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  char buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
static void *Curl_zstd_alloc(void *opaque, size_t size)
{
  (void)opaque;
  반환 Curl_cmalloc(size);
}

static void Curl_zstd_free(void *opaque, void *address)
{
  (void)opaque;
  Curl_cfree(address);
}
#endif

static CURLcode zstd_do_init(struct Curl_easy *data,
                             struct Curl_cwriter *writer)
{
  struct zstd_writer *zp = (struct zstd_writer *)writer;

  (void)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = NULL
  });
#아니면
  zp->zds = ZSTD_createDStream();
#endif

  반환 zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode zstd_do_write(struct Curl_easy *data,
                              struct Curl_cwriter *writer, 정수 type,
                              const char *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  struct zstd_writer *zp = (struct zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  정수 i = 0;

  만약(!(type & CLIENTWRITE_BODY) || !nbytes)
    반환 Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  위해(;;) {
    만약(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      만약(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        반환 CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    만약(ZSTD_isError(errorCode)) {
      반환 CURLE_BAD_CONTENT_ENCODING;
    }
    만약(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      만약(result)
        break;
    }
    만약((in.pos == nbytes) && (out.pos < out.size))
      break;
  }

  반환 result;
}

static void zstd_do_close(struct Curl_easy *data,
                          struct Curl_cwriter *writer)
{
  struct zstd_writer *zp = (struct zstd_writer *)writer;
  (void)data;

  만약(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = NULL;
  }
}

static const struct Curl_cwtype zstd_encoding = {
  "zstd",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  sizeof(struct zstd_writer)
};
#endif

/* Identity handler. */
static const struct Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  sizeof(struct Curl_cwriter)
};

/* supported general content decoders. */
static const struct Curl_cwtype * const general_unencoders[] = {
  &identity_encoding,
#ifdef HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#endif
#ifdef HAVE_BROTLI
  &brotli_encoding,
#endif
#ifdef HAVE_ZSTD
  &zstd_encoding,
#endif
  NULL
};

/* supported content decoders only for transfer encodings */
static const struct Curl_cwtype * const transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  NULL
};

/* Return the list of comma-separated names of supported encodings.
 */
char *Curl_get_content_encodings(void)
{
  struct dynbuf enc;
  const struct Curl_cwtype * const *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  위해(cep = general_unencoders; *cep && !result; cep++) {
    const struct Curl_cwtype *ce = *cep;
    만약(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      만약(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      만약(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  만약(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  만약(!result)
    반환 curlx_dyn_ptr(&enc);
  반환 NULL;
}

/* Deferred error dummy writer. */
static CURLcode error_do_init(struct Curl_easy *data,
                              struct Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
  반환 CURLE_OK;
}

static CURLcode error_do_write(struct Curl_easy *data,
                               struct Curl_cwriter *writer, 정수 type,
                               const char *buf, size_t nbytes)
{
  (void)writer;
  (void)buf;
  (void)nbytes;

  만약(!(type & CLIENTWRITE_BODY) || !nbytes)
    반환 Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  반환 CURLE_BAD_CONTENT_ENCODING;
}

static void error_do_close(struct Curl_easy *data,
                           struct Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
}

static const struct Curl_cwtype error_writer = {
  "ce-error",
  NULL,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  sizeof(struct Curl_cwriter)
};

/* Find the content encoding by name. */
static const struct Curl_cwtype *find_unencode_writer(const char *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  const struct Curl_cwtype * const *cep;

  만약(phase == CURL_CW_TRANSFER_DECODE) {
    위해(cep = transfer_unencoders; *cep; cep++) {
      const struct Curl_cwtype *ce = *cep;
      만약((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        반환 ce;
    }
  }
  /* look among the general decoders */
  위해(cep = general_unencoders; *cep; cep++) {
    const struct Curl_cwtype *ce = *cep;
    만약((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      반환 ce;
  }
  반환 NULL;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_stack(struct Curl_easy *data,
                                     const char *enclist, 정수 is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  do {
    const char *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    동안(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    위해(namelen = 0; *enclist && *enclist != ','; enclist++)
      만약(*enclist > ' ')
        namelen = enclist - name + 1;

    만약(namelen) {
      const struct Curl_cwtype *cwt;
      struct Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (정수)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      만약((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (정수)namelen, name);
        만약(is_transfer && !data->set.http_te_skip) {
          만약(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (정수)namelen, name);
          아니면 만약(is_identity)
            continue;
          아니면
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (정수)namelen, name);
          반환 CURLE_BAD_CONTENT_ENCODING;
        }
        반환 CURLE_OK;
      }

      만약(Curl_cwriter_count(data, phase) >= MAX_ENCODE_STACK) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_STACK,
              is_transfer ? "transfer" : "content");
        반환 CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      만약(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(data, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(data, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        반환 CURLE_BAD_CONTENT_ENCODING;
      }
      만약(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      아니면 {
        만약(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (정수)result);
        만약(result)
          반환 result;

        result = Curl_cwriter_add(data, writer);
        만약(result) {
          Curl_cwriter_free(data, writer);
          반환 result;
        }
      }
      만약(is_chunked)
        has_chunked = TRUE;
    }
  } 동안(*enclist);

  반환 CURLE_OK;
}

#아니면
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_stack(struct Curl_easy *data,
                                     const char *enclist, 정수 is_transfer)
{
  (void)data;
  (void)enclist;
  (void)is_transfer;
  반환 CURLE_NOT_BUILT_IN;
}

char *Curl_get_content_encodings(void)
{
  반환 curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
