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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#ଅନ୍ତର୍ଭାଵ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ଅନ୍ତର୍ଭାଵ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic କାଢ଼
#endif
#endif

#ifdef HAVE_ZSTD
#ଅନ୍ତର୍ଭାଵ <zstd.h>
#endif

#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_ଗଦା 5

#ଯଦି defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#ଯଦି !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ସଂରଚନା zlib_writer {
  ସଂରଚନା Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  ଅକ୍ଷର buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

ସ୍ଥିର voidpf zalloc_cb(voidpf opaque, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବସ୍ତୁଗୁଡ଼ିକ, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା size)
{
  (ଶୂନ୍ୟ)opaque;
  /* not a typo, keep it curlx_calloc() */
  ଫେରନ୍ତୁ curlx_calloc(ବସ୍ତୁଗୁଡ଼ିକ, size);
}

ସ୍ଥିର ଶୂନ୍ୟ zfree_cb(voidpf opaque, voidpf ptr)
{
  (ଶୂନ୍ୟ)opaque;
  curlx_free(ptr);
}

ସ୍ଥିର CURLcode process_zlib_error(ସଂରଚନା Curl_easy *data, z_stream *z)
{
  ଯଦି(z->ସନ୍ଦେଶ)
    failf(data, "Error while processing content unencoding: %s", z->ସନ୍ଦେଶ);
  ଅନ୍ୟଥା
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
}

ସ୍ଥିର CURLcode exit_zlib(ସଂରଚନା Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  ଯଦି(*zlib_init != ZLIB_UNINIT) {
    ଯଦି(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode process_trailer(ସଂରଚନା Curl_easy *data, ସଂରଚନା zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  ଯଦି(z->avail_in)
    result = CURLE_WRITE_ERROR;
  ଯଦି(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  ଅନ୍ୟଥା {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode inflate_stream(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                               zlibInitState started)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

  /* Check state. */
  ଯଦି(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ଫେରନ୍ତୁ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ଯେତେବେଳେ(!done) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status; /* zlib status */
    done = TRUE;

    ଯଦି(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ଯଦି(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ଫେରନ୍ତୁ exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    ଯଦି(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      ଯଦି(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        ଯଦି(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          ଭାଙ୍ଗ;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ବାଛ(status) {
    ସ୍ଥିତି Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ଭାଙ୍ଗ;
    ସ୍ଥିତି Z_STREAM_END:
      ଯଦି((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ଭାଙ୍ଗ;
      }
      result = process_trailer(data, zp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ଯଦି(zp->zlib_init == ZLIB_INIT) {
        ଯଦି(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          ଭାଙ୍ଗ;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ଭାଙ୍ଗ;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ଯଦି(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ଫେରନ୍ତୁ result;
}

/* Deflate handler. */
ସ୍ଥିର CURLcode deflate_do_init(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ଯଦି(inflateInit(z) != Z_OK)
    ଫେରନ୍ତୁ process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode deflate_do_write(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                 ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ଯଦି(!(type & CLIENTWRITE_BODY) || !nbytes)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  ଯଦି(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ଫେରନ୍ତୁ process_trailer(data, zp);

  /* Now uncompress the data */
  ଫେରନ୍ତୁ inflate_stream(data, writer, type, ZLIB_INFLATING);
}

ସ୍ଥିର ଶୂନ୍ୟ deflate_do_close(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype deflate_encoding = {
  "deflate",
  ଖାଲି,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  ଆକାର(ସଂରଚନା zlib_writer)
};

/*
 * Gzip handler.
 */

ସ୍ଥିର CURLcode gzip_do_init(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ଯଦି(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ଫେରନ୍ତୁ process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode gzip_do_write(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ଯଦି(!(type & CLIENTWRITE_BODY) || !nbytes)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ଯଦି(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    ଫେରନ୍ତୁ inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ଫେରନ୍ତୁ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ସ୍ଥିର ଶୂନ୍ୟ gzip_do_close(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zlib_writer *zp = (ସଂରଚନା zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  ଆକାର(ସଂରଚନା zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
ସଂରଚନା brotli_writer {
  ସଂରଚନା Curl_cwriter super;
  ଅକ୍ଷର buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ସ୍ଥିର CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  ବାଛ(be) {
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  ସ୍ଥିତି BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  ସ୍ଥିତି BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_ବୃକ୍ଷ_GROUPS:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  ସ୍ଥିତି BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
}

ସ୍ଥିର CURLcode brotli_do_init(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା brotli_writer *bp = (ସଂରଚନା brotli_writer *)writer;
  (ଶୂନ୍ୟ)data;

  bp->br = BrotliDecoderCreateInstance(ଖାଲି, ଖାଲି, ଖାଲି);
  ଫେରନ୍ତୁ bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ସ୍ଥିର CURLcode brotli_do_write(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ସଂରଚନା brotli_writer *bp = (ସଂରଚନା brotli_writer *)writer;
  ଅଚଳ uint8_t *src = (ଅଚଳ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

  ଯଦି(!(type & CLIENTWRITE_BODY) || !nbytes)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ଯଦି(!bp->br)
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR; /* Stream already ended. */

  ଯେତେବେଳେ((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    ଯଦି(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ଯଦି(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ଖାଲି);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    ବାଛ(r) {
    ସ୍ଥିତି BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    ସ୍ଥିତି BROTLI_DECODER_RESULT_NEEDS_MORE_ନିବେଶ:
      ଭାଙ୍ଗ;
    ସ୍ଥିତି BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ଖାଲି;
      ଯଦି(nbytes)
        result = CURLE_WRITE_ERROR;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ brotli_do_close(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା brotli_writer *bp = (ସଂରଚନା brotli_writer *)writer;
  (ଶୂନ୍ୟ)data;

  ଯଦି(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ଖାଲି;
  }
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype brotli_encoding = {
  "br",
  ଖାଲି,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  ଆକାର(ସଂରଚନା brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
ସଂରଚନା zstd_writer {
  ସଂରଚନା Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  ଅକ୍ଷର buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
ସ୍ଥିର ଶୂନ୍ୟ *Curl_zstd_alloc(ଶୂନ୍ୟ *opaque, size_t size)
{
  (ଶୂନ୍ୟ)opaque;
  ଫେରନ୍ତୁ Curl_cmalloc(size);
}

ସ୍ଥିର ଶୂନ୍ୟ Curl_zstd_free(ଶୂନ୍ୟ *opaque, ଶୂନ୍ୟ *address)
{
  (ଶୂନ୍ୟ)opaque;
  Curl_cfree(address);
}
#endif

ସ୍ଥିର CURLcode zstd_do_init(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zstd_writer *zp = (ସଂରଚନା zstd_writer *)writer;

  (ଶୂନ୍ୟ)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ଖାଲି
  });
#ଅନ୍ୟଥା
  zp->zds = ZSTD_createDStream();
#endif

  ଫେରନ୍ତୁ zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ସ୍ଥିର CURLcode zstd_do_write(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା zstd_writer *zp = (ସଂରଚନା zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

  ଯଦି(!(type & CLIENTWRITE_BODY) || !nbytes)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  ପାଇଁ(;;) {
    ଯଦି(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ଯଦି(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    ଯଦି(ZSTD_isError(errorCode)) {
      ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
    }
    ଯଦି(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }
    ଯଦି((in.pos == nbytes) && (out.pos < out.size))
      ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ zstd_do_close(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା zstd_writer *zp = (ସଂରଚନା zstd_writer *)writer;
  (ଶୂନ୍ୟ)data;

  ଯଦି(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ଖାଲି;
  }
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype zstd_encoding = {
  "zstd",
  ଖାଲି,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  ଆକାର(ସଂରଚନା zstd_writer)
};
#endif

/* Identity handler. */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ଆକାର(ସଂରଚନା Curl_cwriter)
};

/* supported general content decoders. */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype * ଅଚଳ general_unencoders[] = {
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
  ଖାଲି
};

/* supported content decoders only for transfer encodings */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype * ଅଚଳ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ଖାଲି
};

/* Return the list of comma-separated names of supported encodings.
 */
ଅକ୍ଷର *Curl_get_content_encodings(ଶୂନ୍ୟ)
{
  ସଂରଚନା dynbuf enc;
  ଅଚଳ ସଂରଚନା Curl_cwtype * ଅଚଳ *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  ପାଇଁ(cep = general_unencoders; *cep && !result; cep++) {
    ଅଚଳ ସଂରଚନା Curl_cwtype *ce = *cep;
    ଯଦି(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ଯଦି(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      ଯଦି(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  ଯଦି(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  ଯଦି(!result)
    ଫେରନ୍ତୁ curlx_dyn_ptr(&enc);
  ଫେରନ୍ତୁ ଖାଲି;
}

/* Deferred error dummy writer. */
ସ୍ଥିର CURLcode error_do_init(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_cwriter *writer)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)writer;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode error_do_write(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                               ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  (ଶୂନ୍ୟ)writer;
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)nbytes;

  ଯଦି(!(type & CLIENTWRITE_BODY) || !nbytes)
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
}

ସ୍ଥିର ଶୂନ୍ୟ error_do_close(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_cwriter *writer)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)writer;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype error_writer = {
  "ce-error",
  ଖାଲି,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  ଆକାର(ସଂରଚନା Curl_cwriter)
};

/* Find the content encoding by name. */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype *find_unencode_writer(ଅଚଳ ଅକ୍ଷର *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ଅଚଳ ସଂରଚନା Curl_cwtype * ଅଚଳ *cep;

  ଯଦି(phase == CURL_CW_TRANSFER_DECODE) {
    ପାଇଁ(cep = transfer_unencoders; *cep; cep++) {
      ଅଚଳ ସଂରଚନା Curl_cwtype *ce = *cep;
      ଯଦି((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ଫେରନ୍ତୁ ce;
    }
  }
  /* look among the general decoders */
  ପାଇଁ(cep = general_unencoders; *cep; cep++) {
    ଅଚଳ ସଂରଚନା Curl_cwtype *ce = *cep;
    ଯଦି((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ଫେରନ୍ତୁ ce;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ଗଦା(ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ ଅକ୍ଷର *enclist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  କର {
    ଅଚଳ ଅକ୍ଷର *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ଯେତେବେଳେ(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    ପାଇଁ(namelen = 0; *enclist && *enclist != ','; enclist++)
      ଯଦି(*enclist > ' ')
        namelen = enclist - name + 1;

    ଯଦି(namelen) {
      ଅଚଳ ସଂରଚନା Curl_cwtype *cwt;
      ସଂରଚନା Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ଯଦି((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name);
        ଯଦି(is_transfer && !data->set.http_te_skip) {
          ଯଦି(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name);
          ଅନ୍ୟଥା ଯଦି(is_identity)
            ଜାରି;
          ଅନ୍ୟଥା
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name);
          ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
        }
        ଫେରନ୍ତୁ CURLE_OK;
      }

      ଯଦି(Curl_cwriter_ଗଣନା(data, phase) >= MAX_ENCODE_ଗଦା) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ଗଦା,
              is_transfer ? "transfer" : "content");
        ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      ଯଦି(is_transfer && !is_chunked &&
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
        ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
      }
      ଯଦି(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      ଅନ୍ୟଥା {
        ଯଦି(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        result = Curl_cwriter_add(data, writer);
        ଯଦି(result) {
          Curl_cwriter_free(data, writer);
          ଫେରନ୍ତୁ result;
        }
      }
      ଯଦି(is_chunked)
        has_chunked = TRUE;
    }
  } ଯେତେବେଳେ(*enclist);

  ଫେରନ୍ତୁ CURLE_OK;
}

#ଅନ୍ୟଥା
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ଗଦା(ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ ଅକ୍ଷର *enclist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_transfer)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)enclist;
  (ଶୂନ୍ୟ)is_transfer;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

ଅକ୍ଷର *Curl_get_content_encodings(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
