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

#ប្រសិនបើ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#ប្រសិនបើ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

typedef ga_nanaa {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
sa_nracanaa zlib_writer {
  sa_nracanaa Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  var_na buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

static voidpf zalloc_cb(voidpf opaque, unsigned ចំនួនគត់ items, unsigned ចំនួនគត់ size)
{
  (void)opaque;
  /* not a typo, keep it curlx_calloc() */
  ត្រឡប់ curlx_calloc(items, size);
}

static void zfree_cb(voidpf opaque, voidpf ptr)
{
  (void)opaque;
  curlx_free(ptr);
}

static CURLcode process_zlib_error(sa_nracanaa Curl_easy *data, z_stream *z)
{
  ប្រសិនបើ(z->msg)
    failf(data, "Error while processing content unencoding: %s", z->msg);
  បើមិនដូច្នេះ
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
}

static CURLcode exit_zlib(sa_nracanaa Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  ប្រសិនបើ(*zlib_init != ZLIB_UNINIT) {
    ប្រសិនបើ(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ត្រឡប់ result;
}

static CURLcode process_trailer(sa_nracanaa Curl_easy *data, sa_nracanaa zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  ប្រសិនបើ(z->avail_in)
    result = CURLE_WRITE_ERROR;
  ប្រសិនបើ(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  បើមិនដូច្នេះ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ត្រឡប់ result;
}

static CURLcode inflate_stream(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                               zlibInitState started)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  ចំនួនគត់ i = 0;

  /* Check state. */
  ប្រសិនបើ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ត្រឡប់ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ខណៈពេល(!done) {
    ចំនួនគត់ status; /* zlib status */
    done = TRUE;

    ប្រសិនបើ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ប្រសិនបើ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ត្រឡប់ exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    ប្រសិនបើ(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      ប្រសិនបើ(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        ប្រសិនបើ(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          bha_nga;
        }
      }
    }

    /* Dispatch by inflate() status. */
    switch(status) {
    pak_sa Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      bha_nga;
    pak_sa Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      bha_nga;
    pak_sa Z_STREAM_END:
      ប្រសិនបើ((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        bha_nga;
      }
      result = process_trailer(data, zp);
      bha_nga;
    pak_sa Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ប្រសិនបើ(zp->zlib_init == ZLIB_INIT) {
        ប្រសិនបើ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          bha_nga;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      bha_nga;
    default:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      bha_nga;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ប្រសិនបើ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ត្រឡប់ result;
}

/* Deflate handler. */
static CURLcode deflate_do_init(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ប្រសិនបើ(inflateInit(z) != Z_OK)
    ត្រឡប់ process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  ត្រឡប់ CURLE_OK;
}

static CURLcode deflate_do_write(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                                 const var_na *buf, size_t nbytes)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ប្រសិនបើ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ត្រឡប់ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  ប្រសិនបើ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ត្រឡប់ process_trailer(data, zp);

  /* Now uncompress the data */
  ត្រឡប់ inflate_stream(data, writer, type, ZLIB_INFLATING);
}

static void deflate_do_close(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

static const sa_nracanaa Curl_cwtype deflate_encoding = {
  "deflate",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  parimaa_na(sa_nracanaa zlib_writer)
};

/*
 * Gzip handler.
 */

static CURLcode gzip_do_init(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ប្រសិនបើ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ត្រឡប់ process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ត្រឡប់ CURLE_OK;
}

static CURLcode gzip_do_write(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                              const var_na *buf, size_t nbytes)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ប្រសិនបើ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ត្រឡប់ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ប្រសិនបើ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    ត្រឡប់ inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ត្រឡប់ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

static void gzip_do_close(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zlib_writer *zp = (sa_nracanaa zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

static const sa_nracanaa Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  parimaa_na(sa_nracanaa zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
sa_nracanaa brotli_writer {
  sa_nracanaa Curl_cwriter super;
  var_na buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

static CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  switch(be) {
  pak_sa BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  pak_sa BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  pak_sa BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  pak_sa BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  pak_sa BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
  pak_sa BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  pak_sa BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS:
  pak_sa BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  pak_sa BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  pak_sa BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  pak_sa BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    ត្រឡប់ CURLE_OUT_OF_MEMORY;
  default:
    bha_nga;
  }
  ត្រឡប់ CURLE_WRITE_ERROR;
}

static CURLcode brotli_do_init(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa brotli_writer *bp = (sa_nracanaa brotli_writer *)writer;
  (void)data;

  bp->br = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  ត្រឡប់ bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode brotli_do_write(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                                const var_na *buf, size_t nbytes)
{
  sa_nracanaa brotli_writer *bp = (sa_nracanaa brotli_writer *)writer;
  const uint8_t *src = (const uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  ចំនួនគត់ i = 0;

  ប្រសិនបើ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ត្រឡប់ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ប្រសិនបើ(!bp->br)
    ត្រឡប់ CURLE_WRITE_ERROR; /* Stream already ended. */

  ខណៈពេល((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    ប្រសិនបើ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ប្រសិនបើ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ត្រឡប់ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, NULL);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    ប្រសិនបើ(result)
      bha_nga;
    switch(r) {
    pak_sa BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    pak_sa BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT:
      bha_nga;
    pak_sa BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = NULL;
      ប្រសិនបើ(nbytes)
        result = CURLE_WRITE_ERROR;
      bha_nga;
    default:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      bha_nga;
    }
  }
  ត្រឡប់ result;
}

static void brotli_do_close(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa brotli_writer *bp = (sa_nracanaa brotli_writer *)writer;
  (void)data;

  ប្រសិនបើ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = NULL;
  }
}

static const sa_nracanaa Curl_cwtype brotli_encoding = {
  "br",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  parimaa_na(sa_nracanaa brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
sa_nracanaa zstd_writer {
  sa_nracanaa Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  var_na buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
static void *Curl_zstd_alloc(void *opaque, size_t size)
{
  (void)opaque;
  ត្រឡប់ Curl_cmalloc(size);
}

static void Curl_zstd_free(void *opaque, void *address)
{
  (void)opaque;
  Curl_cfree(address);
}
#endif

static CURLcode zstd_do_init(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zstd_writer *zp = (sa_nracanaa zstd_writer *)writer;

  (void)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = NULL
  });
#បើមិនដូច្នេះ
  zp->zds = ZSTD_createDStream();
#endif

  ត្រឡប់ zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static CURLcode zstd_do_write(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                              const var_na *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa zstd_writer *zp = (sa_nracanaa zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  ចំនួនគត់ i = 0;

  ប្រសិនបើ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ត្រឡប់ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  សម្រាប់(;;) {
    ប្រសិនបើ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ប្រសិនបើ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ត្រឡប់ CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    ប្រសិនបើ(ZSTD_isError(errorCode)) {
      ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
    }
    ប្រសិនបើ(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      ប្រសិនបើ(result)
        bha_nga;
    }
    ប្រសិនបើ((in.pos == nbytes) && (out.pos < out.size))
      bha_nga;
  }

  ត្រឡប់ result;
}

static void zstd_do_close(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa zstd_writer *zp = (sa_nracanaa zstd_writer *)writer;
  (void)data;

  ប្រសិនបើ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = NULL;
  }
}

static const sa_nracanaa Curl_cwtype zstd_encoding = {
  "zstd",
  NULL,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  parimaa_na(sa_nracanaa zstd_writer)
};
#endif

/* Identity handler. */
static const sa_nracanaa Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  parimaa_na(sa_nracanaa Curl_cwriter)
};

/* supported general content decoders. */
static const sa_nracanaa Curl_cwtype * const general_unencoders[] = {
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
static const sa_nracanaa Curl_cwtype * const transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  NULL
};

/* Return the list of comma-separated names of supported encodings.
 */
var_na *Curl_get_content_encodings(void)
{
  sa_nracanaa dynbuf enc;
  const sa_nracanaa Curl_cwtype * const *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  សម្រាប់(cep = general_unencoders; *cep && !result; cep++) {
    const sa_nracanaa Curl_cwtype *ce = *cep;
    ប្រសិនបើ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ប្រសិនបើ(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      ប្រសិនបើ(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  ប្រសិនបើ(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  ប្រសិនបើ(!result)
    ត្រឡប់ curlx_dyn_ptr(&enc);
  ត្រឡប់ NULL;
}

/* Deferred error dummy writer. */
static CURLcode error_do_init(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
  ត្រឡប់ CURLE_OK;
}

static CURLcode error_do_write(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *writer, ចំនួនគត់ type,
                               const var_na *buf, size_t nbytes)
{
  (void)writer;
  (void)buf;
  (void)nbytes;

  ប្រសិនបើ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ត្រឡប់ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
}

static void error_do_close(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
}

static const sa_nracanaa Curl_cwtype error_writer = {
  "ce-error",
  NULL,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  parimaa_na(sa_nracanaa Curl_cwriter)
};

/* Find the content encoding by name. */
static const sa_nracanaa Curl_cwtype *find_unencode_writer(const var_na *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  const sa_nracanaa Curl_cwtype * const *cep;

  ប្រសិនបើ(phase == CURL_CW_TRANSFER_DECODE) {
    សម្រាប់(cep = transfer_unencoders; *cep; cep++) {
      const sa_nracanaa Curl_cwtype *ce = *cep;
      ប្រសិនបើ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ត្រឡប់ ce;
    }
  }
  /* look among the general decoders */
  សម្រាប់(cep = general_unencoders; *cep; cep++) {
    const sa_nracanaa Curl_cwtype *ce = *cep;
    ប្រសិនបើ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ត្រឡប់ ce;
  }
  ត្រឡប់ NULL;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_stack(sa_nracanaa Curl_easy *data,
                                     const var_na *enclist, ចំនួនគត់ is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  do {
    const var_na *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ខណៈពេល(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    សម្រាប់(namelen = 0; *enclist && *enclist != ','; enclist++)
      ប្រសិនបើ(*enclist > ' ')
        namelen = enclist - name + 1;

    ប្រសិនបើ(namelen) {
      const sa_nracanaa Curl_cwtype *cwt;
      sa_nracanaa Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (ចំនួនគត់)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ប្រសិនបើ((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (ចំនួនគត់)namelen, name);
        ប្រសិនបើ(is_transfer && !data->set.http_te_skip) {
          ប្រសិនបើ(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (ចំនួនគត់)namelen, name);
          បើមិនដូច្នេះ ប្រសិនបើ(is_identity)
            continue;
          បើមិនដូច្នេះ
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (ចំនួនគត់)namelen, name);
          ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
        }
        ត្រឡប់ CURLE_OK;
      }

      ប្រសិនបើ(Curl_cwriter_count(data, phase) >= MAX_ENCODE_STACK) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_STACK,
              is_transfer ? "transfer" : "content");
        ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      ប្រសិនបើ(is_transfer && !is_chunked &&
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
        ត្រឡប់ CURLE_BAD_CONTENT_ENCODING;
      }
      ប្រសិនបើ(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      បើមិនដូច្នេះ {
        ប្រសិនបើ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (ចំនួនគត់)result);
        ប្រសិនបើ(result)
          ត្រឡប់ result;

        result = Curl_cwriter_add(data, writer);
        ប្រសិនបើ(result) {
          Curl_cwriter_free(data, writer);
          ត្រឡប់ result;
        }
      }
      ប្រសិនបើ(is_chunked)
        has_chunked = TRUE;
    }
  } ខណៈពេល(*enclist);

  ត្រឡប់ CURLE_OK;
}

#បើមិនដូច្នេះ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_stack(sa_nracanaa Curl_easy *data,
                                     const var_na *enclist, ចំនួនគត់ is_transfer)
{
  (void)data;
  (void)enclist;
  (void)is_transfer;
  ត្រឡប់ CURLE_NOT_BUILT_IN;
}

var_na *Curl_get_content_encodings(void)
{
  ត្រឡប់ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
