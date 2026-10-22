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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#ಅನ್ತರ್ಭಾವ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ಅನ್ತರ್ಭಾವ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif
#endif

#ifdef HAVE_ZSTD
#ಅನ್ತರ್ಭಾವ <zstd.h>
#endif

#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_ರಾಶಿ 5

#ಯದಿ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#ಯದಿ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ಸಂರಚನಾ zlib_writer {
  ಸಂರಚನಾ Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  ವರ್ಣ buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

ಸ್ಥಿರ voidpf zalloc_cb(voidpf opaque, unsigned ಪೂರ್ಣಾಙ್ಕ ವಸ್ತುಗಳು, unsigned ಪೂರ್ಣಾಙ್ಕ size)
{
  (ಶೂನ್ಯ)opaque;
  /* not a typo, keep it curlx_calloc() */
  ಪ್ರತ್ಯಾಗಮ curlx_calloc(ವಸ್ತುಗಳು, size);
}

ಸ್ಥಿರ ಶೂನ್ಯ zfree_cb(voidpf opaque, voidpf ptr)
{
  (ಶೂನ್ಯ)opaque;
  curlx_free(ptr);
}

ಸ್ಥಿರ CURLcode process_zlib_error(ಸಂರಚನಾ Curl_easy *data, z_stream *z)
{
  ಯದಿ(z->ಸಂದೇಶ)
    failf(data, "Error while processing content unencoding: %s", z->ಸಂದೇಶ);
  ಅನ್ಯಥಾ
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
}

ಸ್ಥಿರ CURLcode exit_zlib(ಸಂರಚನಾ Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  ಯದಿ(*zlib_init != ZLIB_UNINIT) {
    ಯದಿ(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode process_trailer(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  ಯದಿ(z->avail_in)
    result = CURLE_WRITE_ERROR;
  ಯದಿ(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  ಅನ್ಯಥಾ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode inflate_stream(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                               zlibInitState started)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  ಪೂರ್ಣಾಙ್ಕ i = 0;

  /* Check state. */
  ಯದಿ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ಪ್ರತ್ಯಾಗಮ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ಯಾವತ್(!done) {
    ಪೂರ್ಣಾಙ್ಕ status; /* zlib status */
    done = TRUE;

    ಯದಿ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಯದಿ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ಪ್ರತ್ಯಾಗಮ exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    ಯದಿ(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      ಯದಿ(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        ಯದಿ(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          ಭಙ್ಗ;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ವಿಕಲ್ಪ(status) {
    ಪಕ್ಷ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      ಭಙ್ಗ;
    ಪಕ್ಷ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ಭಙ್ಗ;
    ಪಕ್ಷ Z_STREAM_END:
      ಯದಿ((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ಭಙ್ಗ;
      }
      result = process_trailer(data, zp);
      ಭಙ್ಗ;
    ಪಕ್ಷ Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ಯದಿ(zp->zlib_init == ZLIB_INIT) {
        ಯದಿ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          ಭಙ್ಗ;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ಭಙ್ಗ;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ಯದಿ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Deflate handler. */
ಸ್ಥಿರ CURLcode deflate_do_init(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ಯದಿ(inflateInit(z) != Z_OK)
    ಪ್ರತ್ಯಾಗಮ process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode deflate_do_write(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                 ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ಯದಿ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  ಯದಿ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ಪ್ರತ್ಯಾಗಮ process_trailer(data, zp);

  /* Now uncompress the data */
  ಪ್ರತ್ಯಾಗಮ inflate_stream(data, writer, type, ZLIB_INFLATING);
}

ಸ್ಥಿರ ಶೂನ್ಯ deflate_do_close(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype deflate_encoding = {
  "deflate",
  ರಿಕ್ತ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ zlib_writer)
};

/*
 * Gzip handler.
 */

ಸ್ಥಿರ CURLcode gzip_do_init(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ಯದಿ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ಪ್ರತ್ಯಾಗಮ process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode gzip_do_write(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  ಯದಿ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ಯದಿ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    ಪ್ರತ್ಯಾಗಮ inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ಪ್ರತ್ಯಾಗಮ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ಸ್ಥಿರ ಶೂನ್ಯ gzip_do_close(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zlib_writer *zp = (ಸಂರಚನಾ zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
ಸಂರಚನಾ brotli_writer {
  ಸಂರಚನಾ Curl_cwriter super;
  ವರ್ಣ buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ಸ್ಥಿರ CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  ವಿಕಲ್ಪ(be) {
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  ಪಕ್ಷ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  ಪಕ್ಷ BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_ವೃಕ್ಷ_GROUPS:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  ಪಕ್ಷ BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
}

ಸ್ಥಿರ CURLcode brotli_do_init(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ brotli_writer *bp = (ಸಂರಚನಾ brotli_writer *)writer;
  (ಶೂನ್ಯ)data;

  bp->br = BrotliDecoderCreateInstance(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ಸ್ಥಿರ CURLcode brotli_do_write(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಸಂರಚನಾ brotli_writer *bp = (ಸಂರಚನಾ brotli_writer *)writer;
  ಧ್ರುವ uint8_t *src = (ಧ್ರುವ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  ಪೂರ್ಣಾಙ್ಕ i = 0;

  ಯದಿ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  ಯದಿ(!bp->br)
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR; /* Stream already ended. */

  ಯಾವತ್((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    ಯದಿ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಯದಿ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ರಿಕ್ತ);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    ಯದಿ(result)
      ಭಙ್ಗ;
    ವಿಕಲ್ಪ(r) {
    ಪಕ್ಷ BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    ಪಕ್ಷ BROTLI_DECODER_RESULT_NEEDS_MORE_ಒಳಹರಿವು:
      ಭಙ್ಗ;
    ಪಕ್ಷ BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ರಿಕ್ತ;
      ಯದಿ(nbytes)
        result = CURLE_WRITE_ERROR;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ brotli_do_close(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ brotli_writer *bp = (ಸಂರಚನಾ brotli_writer *)writer;
  (ಶೂನ್ಯ)data;

  ಯದಿ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype brotli_encoding = {
  "br",
  ರಿಕ್ತ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
ಸಂರಚನಾ zstd_writer {
  ಸಂರಚನಾ Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  ವರ್ಣ buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
ಸ್ಥಿರ ಶೂನ್ಯ *Curl_zstd_alloc(ಶೂನ್ಯ *opaque, size_t size)
{
  (ಶೂನ್ಯ)opaque;
  ಪ್ರತ್ಯಾಗಮ Curl_cmalloc(size);
}

ಸ್ಥಿರ ಶೂನ್ಯ Curl_zstd_free(ಶೂನ್ಯ *opaque, ಶೂನ್ಯ *address)
{
  (ಶೂನ್ಯ)opaque;
  Curl_cfree(address);
}
#endif

ಸ್ಥಿರ CURLcode zstd_do_init(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zstd_writer *zp = (ಸಂರಚನಾ zstd_writer *)writer;

  (ಶೂನ್ಯ)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ರಿಕ್ತ
  });
#ಅನ್ಯಥಾ
  zp->zds = ZSTD_createDStream();
#endif

  ಪ್ರತ್ಯಾಗಮ zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ಸ್ಥಿರ CURLcode zstd_do_write(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ zstd_writer *zp = (ಸಂರಚನಾ zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  ಪೂರ್ಣಾಙ್ಕ i = 0;

  ಯದಿ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  ಪ್ರತಿ(;;) {
    ಯದಿ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಯದಿ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    ಯದಿ(ZSTD_isError(errorCode)) {
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
    }
    ಯದಿ(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      ಯದಿ(result)
        ಭಙ್ಗ;
    }
    ಯದಿ((in.pos == nbytes) && (out.pos < out.size))
      ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ zstd_do_close(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ zstd_writer *zp = (ಸಂರಚನಾ zstd_writer *)writer;
  (ಶೂನ್ಯ)data;

  ಯದಿ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype zstd_encoding = {
  "zstd",
  ರಿಕ್ತ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ zstd_writer)
};
#endif

/* Identity handler. */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_cwriter)
};

/* supported general content decoders. */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype * ಧ್ರುವ general_unencoders[] = {
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
  ರಿಕ್ತ
};

/* supported content decoders only for transfer encodings */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype * ಧ್ರುವ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ರಿಕ್ತ
};

/* Return the list of comma-separated names of supported encodings.
 */
ವರ್ಣ *Curl_get_content_encodings(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ dynbuf enc;
  ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype * ಧ್ರುವ *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  ಪ್ರತಿ(cep = general_unencoders; *cep && !result; cep++) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *ce = *cep;
    ಯದಿ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ಯದಿ(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      ಯದಿ(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  ಯದಿ(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  ಯದಿ(!result)
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&enc);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/* Deferred error dummy writer. */
ಸ್ಥಿರ CURLcode error_do_init(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)writer;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode error_do_write(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                               ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  (ಶೂನ್ಯ)writer;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)nbytes;

  ಯದಿ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
}

ಸ್ಥಿರ ಶೂನ್ಯ error_do_close(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)writer;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype error_writer = {
  "ce-error",
  ರಿಕ್ತ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_cwriter)
};

/* Find the content encoding by name. */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *find_unencode_writer(ಧ್ರುವ ವರ್ಣ *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype * ಧ್ರುವ *cep;

  ಯದಿ(phase == CURL_CW_TRANSFER_DECODE) {
    ಪ್ರತಿ(cep = transfer_unencoders; *cep; cep++) {
      ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *ce = *cep;
      ಯದಿ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ಪ್ರತ್ಯಾಗಮ ce;
    }
  }
  /* look among the general decoders */
  ಪ್ರತಿ(cep = general_unencoders; *cep; cep++) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *ce = *cep;
    ಯದಿ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ಪ್ರತ್ಯಾಗಮ ce;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ರಾಶಿ(ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ವರ್ಣ *enclist, ಪೂರ್ಣಾಙ್ಕ is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  ಕುರು {
    ಧ್ರುವ ವರ್ಣ *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ಯಾವತ್(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    ಪ್ರತಿ(namelen = 0; *enclist && *enclist != ','; enclist++)
      ಯದಿ(*enclist > ' ')
        namelen = enclist - name + 1;

    ಯದಿ(namelen) {
      ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *cwt;
      ಸಂರಚನಾ Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (ಪೂರ್ಣಾಙ್ಕ)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ಯದಿ((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (ಪೂರ್ಣಾಙ್ಕ)namelen, name);
        ಯದಿ(is_transfer && !data->set.http_te_skip) {
          ಯದಿ(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (ಪೂರ್ಣಾಙ್ಕ)namelen, name);
          ಅನ್ಯಥಾ ಯದಿ(is_identity)
            ಅನುವರ್ತ;
          ಅನ್ಯಥಾ
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (ಪೂರ್ಣಾಙ್ಕ)namelen, name);
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
        }
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }

      ಯದಿ(Curl_cwriter_ಎಣಿಕೆ(data, phase) >= MAX_ENCODE_ರಾಶಿ) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ರಾಶಿ,
              is_transfer ? "transfer" : "content");
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      ಯದಿ(is_transfer && !is_chunked &&
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
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
      }
      ಯದಿ(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (ಪೂರ್ಣಾಙ್ಕ)result);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        result = Curl_cwriter_add(data, writer);
        ಯದಿ(result) {
          Curl_cwriter_free(data, writer);
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }
      ಯದಿ(is_chunked)
        has_chunked = TRUE;
    }
  } ಯಾವತ್(*enclist);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಅನ್ಯಥಾ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ರಾಶಿ(ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ವರ್ಣ *enclist, ಪೂರ್ಣಾಙ್ಕ is_transfer)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)enclist;
  (ಶೂನ್ಯ)is_transfer;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

ವರ್ಣ *Curl_get_content_encodings(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
