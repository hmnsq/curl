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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#അന്തര്ഭാവ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wvla"
#endif
#അന്തര്ഭാവ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif
#endif

#ifdef HAVE_ZSTD
#അന്തര്ഭാവ <zstd.h>
#endif

#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_അട്ടി 5

#എങ്കിൽ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#എങ്കിൽ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

തരനിർവചനം എണ്ണൽ {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ഘടന zlib_writer {
  ഘടന Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  അക്ഷരം buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

സ്ഥിതം voidpf zalloc_cb(voidpf opaque, unsigned പൂർണ്ണസംഖ്യ ഇനങ്ങൾ, unsigned പൂർണ്ണസംഖ്യ size)
{
  (ശൂന്യം)opaque;
  /* not a typo, keep it curlx_calloc() */
  തിരികെ curlx_calloc(ഇനങ്ങൾ, size);
}

സ്ഥിതം ശൂന്യം zfree_cb(voidpf opaque, voidpf ptr)
{
  (ശൂന്യം)opaque;
  curlx_free(ptr);
}

സ്ഥിതം CURLcode process_zlib_error(ഘടന Curl_easy *data, z_stream *z)
{
  എങ്കിൽ(z->സന്ദേശം)
    failf(data, "Error while processing content unencoding: %s", z->സന്ദേശം);
  അല്ലെങ്കിൽ
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  തിരികെ CURLE_BAD_CONTENT_ENCODING;
}

സ്ഥിതം CURLcode exit_zlib(ഘടന Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  എങ്കിൽ(*zlib_init != ZLIB_UNINIT) {
    എങ്കിൽ(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode process_trailer(ഘടന Curl_easy *data, ഘടന zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  എങ്കിൽ(z->avail_in)
    result = CURLE_WRITE_ERROR;
  എങ്കിൽ(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  അല്ലെങ്കിൽ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode inflate_stream(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                               zlibInitState started)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  പൂർണ്ണസംഖ്യ i = 0;

  /* Check state. */
  എങ്കിൽ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    തിരികെ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ആകുമ്പോൾ(!done) {
    പൂർണ്ണസംഖ്യ status; /* zlib status */
    done = TRUE;

    എങ്കിൽ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        തിരികെ exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    എങ്കിൽ(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      എങ്കിൽ(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        എങ്കിൽ(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          നിർത്തുക;
        }
      }
    }

    /* Dispatch by inflate() status. */
    തിരഞ്ഞെടുപ്പ്(status) {
    അവസ്ഥ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      നിർത്തുക;
    അവസ്ഥ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      നിർത്തുക;
    അവസ്ഥ Z_STREAM_END:
      എങ്കിൽ((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        നിർത്തുക;
      }
      result = process_trailer(data, zp);
      നിർത്തുക;
    അവസ്ഥ Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      എങ്കിൽ(zp->zlib_init == ZLIB_INIT) {
        എങ്കിൽ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          നിർത്തുക;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      നിർത്തുക;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  എങ്കിൽ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  തിരികെ result;
}

/* Deflate handler. */
സ്ഥിതം CURLcode deflate_do_init(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  എങ്കിൽ(inflateInit(z) != Z_OK)
    തിരികെ process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode deflate_do_write(ഘടന Curl_easy *data,
                                 ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                 സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || !nbytes)
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  എങ്കിൽ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    തിരികെ process_trailer(data, zp);

  /* Now uncompress the data */
  തിരികെ inflate_stream(data, writer, type, ZLIB_INFLATING);
}

സ്ഥിതം ശൂന്യം deflate_do_close(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype deflate_encoding = {
  "deflate",
  ശൂന്യ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  വലുപ്പം(ഘടന zlib_writer)
};

/*
 * Gzip handler.
 */

സ്ഥിതം CURLcode gzip_do_init(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  എങ്കിൽ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    തിരികെ process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode gzip_do_write(ഘടന Curl_easy *data,
                              ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                              സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || !nbytes)
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  എങ്കിൽ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    തിരികെ inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  തിരികെ exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

സ്ഥിതം ശൂന്യം gzip_do_close(ഘടന Curl_easy *data,
                          ഘടന Curl_cwriter *writer)
{
  ഘടന zlib_writer *zp = (ഘടന zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  വലുപ്പം(ഘടന zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
ഘടന brotli_writer {
  ഘടന Curl_cwriter super;
  അക്ഷരം buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

സ്ഥിതം CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  തിരഞ്ഞെടുപ്പ്(be) {
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  അവസ്ഥ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  അവസ്ഥ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  അവസ്ഥ BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  അവസ്ഥ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    തിരികെ CURLE_BAD_CONTENT_ENCODING;
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_വൃക്ഷം_GROUPS:
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  അവസ്ഥ BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    തിരികെ CURLE_OUT_OF_MEMORY;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ CURLE_WRITE_ERROR;
}

സ്ഥിതം CURLcode brotli_do_init(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer)
{
  ഘടന brotli_writer *bp = (ഘടന brotli_writer *)writer;
  (ശൂന്യം)data;

  bp->br = BrotliDecoderCreateInstance(ശൂന്യ, ശൂന്യ, ശൂന്യ);
  തിരികെ bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

സ്ഥിതം CURLcode brotli_do_write(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  ഘടന brotli_writer *bp = (ഘടന brotli_writer *)writer;
  സ്ഥിരം uint8_t *src = (സ്ഥിരം uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  പൂർണ്ണസംഖ്യ i = 0;

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || !nbytes)
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  എങ്കിൽ(!bp->br)
    തിരികെ CURLE_WRITE_ERROR; /* Stream already ended. */

  ആകുമ്പോൾ((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    എങ്കിൽ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        തിരികെ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ശൂന്യ);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    എങ്കിൽ(result)
      നിർത്തുക;
    തിരഞ്ഞെടുപ്പ്(r) {
    അവസ്ഥ BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    അവസ്ഥ BROTLI_DECODER_RESULT_NEEDS_MORE_നിവേശം:
      നിർത്തുക;
    അവസ്ഥ BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ശൂന്യ;
      എങ്കിൽ(nbytes)
        result = CURLE_WRITE_ERROR;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      നിർത്തുക;
    }
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം brotli_do_close(ഘടന Curl_easy *data,
                            ഘടന Curl_cwriter *writer)
{
  ഘടന brotli_writer *bp = (ഘടന brotli_writer *)writer;
  (ശൂന്യം)data;

  എങ്കിൽ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ശൂന്യ;
  }
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype brotli_encoding = {
  "br",
  ശൂന്യ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  വലുപ്പം(ഘടന brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
ഘടന zstd_writer {
  ഘടന Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  അക്ഷരം buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
സ്ഥിതം ശൂന്യം *Curl_zstd_alloc(ശൂന്യം *opaque, size_t size)
{
  (ശൂന്യം)opaque;
  തിരികെ Curl_cmalloc(size);
}

സ്ഥിതം ശൂന്യം Curl_zstd_free(ശൂന്യം *opaque, ശൂന്യം *address)
{
  (ശൂന്യം)opaque;
  Curl_cfree(address);
}
#endif

സ്ഥിതം CURLcode zstd_do_init(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer)
{
  ഘടന zstd_writer *zp = (ഘടന zstd_writer *)writer;

  (ശൂന്യം)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ശൂന്യ
  });
#അല്ലെങ്കിൽ
  zp->zds = ZSTD_createDStream();
#endif

  തിരികെ zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

സ്ഥിതം CURLcode zstd_do_write(ഘടന Curl_easy *data,
                              ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                              സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  ഘടന zstd_writer *zp = (ഘടന zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  പൂർണ്ണസംഖ്യ i = 0;

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || !nbytes)
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  വേണ്ടി(;;) {
    എങ്കിൽ(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        തിരികെ CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    എങ്കിൽ(ZSTD_isError(errorCode)) {
      തിരികെ CURLE_BAD_CONTENT_ENCODING;
    }
    എങ്കിൽ(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      എങ്കിൽ(result)
        നിർത്തുക;
    }
    എങ്കിൽ((in.pos == nbytes) && (out.pos < out.size))
      നിർത്തുക;
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം zstd_do_close(ഘടന Curl_easy *data,
                          ഘടന Curl_cwriter *writer)
{
  ഘടന zstd_writer *zp = (ഘടന zstd_writer *)writer;
  (ശൂന്യം)data;

  എങ്കിൽ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ശൂന്യ;
  }
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype zstd_encoding = {
  "zstd",
  ശൂന്യ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  വലുപ്പം(ഘടന zstd_writer)
};
#endif

/* Identity handler. */
സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  വലുപ്പം(ഘടന Curl_cwriter)
};

/* supported general content decoders. */
സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype * സ്ഥിരം general_unencoders[] = {
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
  ശൂന്യ
};

/* supported content decoders only for transfer encodings */
സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype * സ്ഥിരം transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ശൂന്യ
};

/* Return the list of comma-separated names of supported encodings.
 */
അക്ഷരം *Curl_get_content_encodings(ശൂന്യം)
{
  ഘടന dynbuf enc;
  സ്ഥിരം ഘടന Curl_cwtype * സ്ഥിരം *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  വേണ്ടി(cep = general_unencoders; *cep && !result; cep++) {
    സ്ഥിരം ഘടന Curl_cwtype *ce = *cep;
    എങ്കിൽ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      എങ്കിൽ(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      എങ്കിൽ(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  എങ്കിൽ(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  എങ്കിൽ(!result)
    തിരികെ curlx_dyn_ptr(&enc);
  തിരികെ ശൂന്യ;
}

/* Deferred error dummy writer. */
സ്ഥിതം CURLcode error_do_init(ഘടന Curl_easy *data,
                              ഘടന Curl_cwriter *writer)
{
  (ശൂന്യം)data;
  (ശൂന്യം)writer;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode error_do_write(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                               സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  (ശൂന്യം)writer;
  (ശൂന്യം)buf;
  (ശൂന്യം)nbytes;

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || !nbytes)
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  തിരികെ CURLE_BAD_CONTENT_ENCODING;
}

സ്ഥിതം ശൂന്യം error_do_close(ഘടന Curl_easy *data,
                           ഘടന Curl_cwriter *writer)
{
  (ശൂന്യം)data;
  (ശൂന്യം)writer;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype error_writer = {
  "ce-error",
  ശൂന്യ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  വലുപ്പം(ഘടന Curl_cwriter)
};

/* Find the content encoding by name. */
സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype *find_unencode_writer(സ്ഥിരം അക്ഷരം *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  സ്ഥിരം ഘടന Curl_cwtype * സ്ഥിരം *cep;

  എങ്കിൽ(phase == CURL_CW_TRANSFER_DECODE) {
    വേണ്ടി(cep = transfer_unencoders; *cep; cep++) {
      സ്ഥിരം ഘടന Curl_cwtype *ce = *cep;
      എങ്കിൽ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        തിരികെ ce;
    }
  }
  /* look among the general decoders */
  വേണ്ടി(cep = general_unencoders; *cep; cep++) {
    സ്ഥിരം ഘടന Curl_cwtype *ce = *cep;
    എങ്കിൽ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      തിരികെ ce;
  }
  തിരികെ ശൂന്യ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_അട്ടി(ഘടന Curl_easy *data,
                                     സ്ഥിരം അക്ഷരം *enclist, പൂർണ്ണസംഖ്യ is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  ചെയ്യുക {
    സ്ഥിരം അക്ഷരം *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ആകുമ്പോൾ(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    വേണ്ടി(namelen = 0; *enclist && *enclist != ','; enclist++)
      എങ്കിൽ(*enclist > ' ')
        namelen = enclist - name + 1;

    എങ്കിൽ(namelen) {
      സ്ഥിരം ഘടന Curl_cwtype *cwt;
      ഘടന Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (പൂർണ്ണസംഖ്യ)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      എങ്കിൽ((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (പൂർണ്ണസംഖ്യ)namelen, name);
        എങ്കിൽ(is_transfer && !data->set.http_te_skip) {
          എങ്കിൽ(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (പൂർണ്ണസംഖ്യ)namelen, name);
          അല്ലെങ്കിൽ എങ്കിൽ(is_identity)
            തുടരുക;
          അല്ലെങ്കിൽ
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (പൂർണ്ണസംഖ്യ)namelen, name);
          തിരികെ CURLE_BAD_CONTENT_ENCODING;
        }
        തിരികെ CURLE_OK;
      }

      എങ്കിൽ(Curl_cwriter_എണ്ണം(data, phase) >= MAX_ENCODE_അട്ടി) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_അട്ടി,
              is_transfer ? "transfer" : "content");
        തിരികെ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      എങ്കിൽ(is_transfer && !is_chunked &&
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
        തിരികെ CURLE_BAD_CONTENT_ENCODING;
      }
      എങ്കിൽ(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (പൂർണ്ണസംഖ്യ)result);
        എങ്കിൽ(result)
          തിരികെ result;

        result = Curl_cwriter_add(data, writer);
        എങ്കിൽ(result) {
          Curl_cwriter_free(data, writer);
          തിരികെ result;
        }
      }
      എങ്കിൽ(is_chunked)
        has_chunked = TRUE;
    }
  } ആകുമ്പോൾ(*enclist);

  തിരികെ CURLE_OK;
}

#അല്ലെങ്കിൽ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_അട്ടി(ഘടന Curl_easy *data,
                                     സ്ഥിരം അക്ഷരം *enclist, പൂർണ്ണസംഖ്യ is_transfer)
{
  (ശൂന്യം)data;
  (ശൂന്യം)enclist;
  (ശൂന്യം)is_transfer;
  തിരികെ CURLE_NOT_BUILT_IN;
}

അക്ഷരം *Curl_get_content_encodings(ശൂന്യം)
{
  തിരികെ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
