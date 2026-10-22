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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#அந்தர்பாவ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wvla"
#endif
#அந்தர்பாவ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif
#endif

#ifdef HAVE_ZSTD
#அந்தர்பாவ <zstd.h>
#endif

#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_அடுக்கு 5

#எனில் defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#எனில் !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

வகைவரை எண்ணிக்கை {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
கட்டமைப்பு zlib_writer {
  கட்டமைப்பு Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  எழுத்து buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

நிலையான voidpf zalloc_cb(voidpf opaque, unsigned முழு பொருட்கள், unsigned முழு size)
{
  (வெறுமை)opaque;
  /* not a typo, keep it curlx_calloc() */
  திரும்பு curlx_calloc(பொருட்கள், size);
}

நிலையான வெறுமை zfree_cb(voidpf opaque, voidpf ptr)
{
  (வெறுமை)opaque;
  curlx_free(ptr);
}

நிலையான CURLcode process_zlib_error(கட்டமைப்பு Curl_easy *data, z_stream *z)
{
  எனில்(z->செய்தி)
    failf(data, "Error while processing content unencoding: %s", z->செய்தி);
  இல்லையெனில்
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  திரும்பு CURLE_BAD_CONTENT_ENCODING;
}

நிலையான CURLcode exit_zlib(கட்டமைப்பு Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  எனில்(*zlib_init != ZLIB_UNINIT) {
    எனில்(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  திரும்பு result;
}

நிலையான CURLcode process_trailer(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  எனில்(z->avail_in)
    result = CURLE_WRITE_ERROR;
  எனில்(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  இல்லையெனில் {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  திரும்பு result;
}

நிலையான CURLcode inflate_stream(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *writer, முழு type,
                               zlibInitState started)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  முழு i = 0;

  /* Check state. */
  எனில்(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    திரும்பு exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  போது(!done) {
    முழு status; /* zlib status */
    done = TRUE;

    எனில்(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      எனில்(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        திரும்பு exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    எனில்(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      எனில்(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        எனில்(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          நிறுத்து;
        }
      }
    }

    /* Dispatch by inflate() status. */
    தேர்வு(status) {
    நிலை Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      நிறுத்து;
    நிலை Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      நிறுத்து;
    நிலை Z_STREAM_END:
      எனில்((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        நிறுத்து;
      }
      result = process_trailer(data, zp);
      நிறுத்து;
    நிலை Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      எனில்(zp->zlib_init == ZLIB_INIT) {
        எனில்(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          நிறுத்து;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      நிறுத்து;
    இயல்பு:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      நிறுத்து;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  எனில்(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  திரும்பு result;
}

/* Deflate handler. */
நிலையான CURLcode deflate_do_init(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  எனில்(inflateInit(z) != Z_OK)
    திரும்பு process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode deflate_do_write(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                 மாறிலி எழுத்து *buf, size_t nbytes)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  எனில்(!(type & CLIENTWRITE_BODY) || !nbytes)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  எனில்(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    திரும்பு process_trailer(data, zp);

  /* Now uncompress the data */
  திரும்பு inflate_stream(data, writer, type, ZLIB_INFLATING);
}

நிலையான வெறுமை deflate_do_close(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype deflate_encoding = {
  "deflate",
  வெற்று,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  அளவு(கட்டமைப்பு zlib_writer)
};

/*
 * Gzip handler.
 */

நிலையான CURLcode gzip_do_init(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  எனில்(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    திரும்பு process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  திரும்பு CURLE_OK;
}

நிலையான CURLcode gzip_do_write(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_cwriter *writer, முழு type,
                              மாறிலி எழுத்து *buf, size_t nbytes)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  எனில்(!(type & CLIENTWRITE_BODY) || !nbytes)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  எனில்(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    திரும்பு inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  திரும்பு exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

நிலையான வெறுமை gzip_do_close(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zlib_writer *zp = (கட்டமைப்பு zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  அளவு(கட்டமைப்பு zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
கட்டமைப்பு brotli_writer {
  கட்டமைப்பு Curl_cwriter super;
  எழுத்து buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

நிலையான CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  தேர்வு(be) {
  நிலை BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  நிலை BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  நிலை BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  நிலை BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  நிலை BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  நிலை BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  நிலை BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  நிலை BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  நிலை BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  நிலை BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  நிலை BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  நிலை BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  நிலை BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  நிலை BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  நிலை BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  நிலை BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  நிலை BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    திரும்பு CURLE_BAD_CONTENT_ENCODING;
  நிலை BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  நிலை BROTLI_DECODER_ERROR_ALLOC_மரம்_GROUPS:
  நிலை BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  நிலை BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  நிலை BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  நிலை BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    திரும்பு CURLE_OUT_OF_MEMORY;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு CURLE_WRITE_ERROR;
}

நிலையான CURLcode brotli_do_init(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு brotli_writer *bp = (கட்டமைப்பு brotli_writer *)writer;
  (வெறுமை)data;

  bp->br = BrotliDecoderCreateInstance(வெற்று, வெற்று, வெற்று);
  திரும்பு bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

நிலையான CURLcode brotli_do_write(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                மாறிலி எழுத்து *buf, size_t nbytes)
{
  கட்டமைப்பு brotli_writer *bp = (கட்டமைப்பு brotli_writer *)writer;
  மாறிலி uint8_t *src = (மாறிலி uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  முழு i = 0;

  எனில்(!(type & CLIENTWRITE_BODY) || !nbytes)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  எனில்(!bp->br)
    திரும்பு CURLE_WRITE_ERROR; /* Stream already ended. */

  போது((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    எனில்(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      எனில்(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        திரும்பு CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, வெற்று);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    எனில்(result)
      நிறுத்து;
    தேர்வு(r) {
    நிலை BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    நிலை BROTLI_DECODER_RESULT_NEEDS_MORE_உள்ளீடு:
      நிறுத்து;
    நிலை BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = வெற்று;
      எனில்(nbytes)
        result = CURLE_WRITE_ERROR;
      நிறுத்து;
    இயல்பு:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      நிறுத்து;
    }
  }
  திரும்பு result;
}

நிலையான வெறுமை brotli_do_close(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு brotli_writer *bp = (கட்டமைப்பு brotli_writer *)writer;
  (வெறுமை)data;

  எனில்(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = வெற்று;
  }
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype brotli_encoding = {
  "br",
  வெற்று,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  அளவு(கட்டமைப்பு brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
கட்டமைப்பு zstd_writer {
  கட்டமைப்பு Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  எழுத்து buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
நிலையான வெறுமை *Curl_zstd_alloc(வெறுமை *opaque, size_t size)
{
  (வெறுமை)opaque;
  திரும்பு Curl_cmalloc(size);
}

நிலையான வெறுமை Curl_zstd_free(வெறுமை *opaque, வெறுமை *address)
{
  (வெறுமை)opaque;
  Curl_cfree(address);
}
#endif

நிலையான CURLcode zstd_do_init(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zstd_writer *zp = (கட்டமைப்பு zstd_writer *)writer;

  (வெறுமை)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = வெற்று
  });
#இல்லையெனில்
  zp->zds = ZSTD_createDStream();
#endif

  திரும்பு zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

நிலையான CURLcode zstd_do_write(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_cwriter *writer, முழு type,
                              மாறிலி எழுத்து *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு zstd_writer *zp = (கட்டமைப்பு zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  முழு i = 0;

  எனில்(!(type & CLIENTWRITE_BODY) || !nbytes)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  க்கு(;;) {
    எனில்(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      எனில்(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        திரும்பு CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    எனில்(ZSTD_isError(errorCode)) {
      திரும்பு CURLE_BAD_CONTENT_ENCODING;
    }
    எனில்(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      எனில்(result)
        நிறுத்து;
    }
    எனில்((in.pos == nbytes) && (out.pos < out.size))
      நிறுத்து;
  }

  திரும்பு result;
}

நிலையான வெறுமை zstd_do_close(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு zstd_writer *zp = (கட்டமைப்பு zstd_writer *)writer;
  (வெறுமை)data;

  எனில்(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = வெற்று;
  }
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype zstd_encoding = {
  "zstd",
  வெற்று,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  அளவு(கட்டமைப்பு zstd_writer)
};
#endif

/* Identity handler. */
நிலையான மாறிலி கட்டமைப்பு Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  அளவு(கட்டமைப்பு Curl_cwriter)
};

/* supported general content decoders. */
நிலையான மாறிலி கட்டமைப்பு Curl_cwtype * மாறிலி general_unencoders[] = {
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
  வெற்று
};

/* supported content decoders only for transfer encodings */
நிலையான மாறிலி கட்டமைப்பு Curl_cwtype * மாறிலி transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  வெற்று
};

/* Return the list of comma-separated names of supported encodings.
 */
எழுத்து *Curl_get_content_encodings(வெறுமை)
{
  கட்டமைப்பு dynbuf enc;
  மாறிலி கட்டமைப்பு Curl_cwtype * மாறிலி *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  க்கு(cep = general_unencoders; *cep && !result; cep++) {
    மாறிலி கட்டமைப்பு Curl_cwtype *ce = *cep;
    எனில்(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      எனில்(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      எனில்(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  எனில்(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  எனில்(!result)
    திரும்பு curlx_dyn_ptr(&enc);
  திரும்பு வெற்று;
}

/* Deferred error dummy writer. */
நிலையான CURLcode error_do_init(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_cwriter *writer)
{
  (வெறுமை)data;
  (வெறுமை)writer;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode error_do_write(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *writer, முழு type,
                               மாறிலி எழுத்து *buf, size_t nbytes)
{
  (வெறுமை)writer;
  (வெறுமை)buf;
  (வெறுமை)nbytes;

  எனில்(!(type & CLIENTWRITE_BODY) || !nbytes)
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  திரும்பு CURLE_BAD_CONTENT_ENCODING;
}

நிலையான வெறுமை error_do_close(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_cwriter *writer)
{
  (வெறுமை)data;
  (வெறுமை)writer;
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype error_writer = {
  "ce-error",
  வெற்று,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  அளவு(கட்டமைப்பு Curl_cwriter)
};

/* Find the content encoding by name. */
நிலையான மாறிலி கட்டமைப்பு Curl_cwtype *find_unencode_writer(மாறிலி எழுத்து *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  மாறிலி கட்டமைப்பு Curl_cwtype * மாறிலி *cep;

  எனில்(phase == CURL_CW_TRANSFER_DECODE) {
    க்கு(cep = transfer_unencoders; *cep; cep++) {
      மாறிலி கட்டமைப்பு Curl_cwtype *ce = *cep;
      எனில்((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        திரும்பு ce;
    }
  }
  /* look among the general decoders */
  க்கு(cep = general_unencoders; *cep; cep++) {
    மாறிலி கட்டமைப்பு Curl_cwtype *ce = *cep;
    எனில்((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      திரும்பு ce;
  }
  திரும்பு வெற்று;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_அடுக்கு(கட்டமைப்பு Curl_easy *data,
                                     மாறிலி எழுத்து *enclist, முழு is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  செய் {
    மாறிலி எழுத்து *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    போது(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    க்கு(namelen = 0; *enclist && *enclist != ','; enclist++)
      எனில்(*enclist > ' ')
        namelen = enclist - name + 1;

    எனில்(namelen) {
      மாறிலி கட்டமைப்பு Curl_cwtype *cwt;
      கட்டமைப்பு Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (முழு)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      எனில்((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (முழு)namelen, name);
        எனில்(is_transfer && !data->set.http_te_skip) {
          எனில்(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (முழு)namelen, name);
          இல்லையெனில் எனில்(is_identity)
            தொடர்;
          இல்லையெனில்
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (முழு)namelen, name);
          திரும்பு CURLE_BAD_CONTENT_ENCODING;
        }
        திரும்பு CURLE_OK;
      }

      எனில்(Curl_cwriter_மொத்தம்(data, phase) >= MAX_ENCODE_அடுக்கு) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_அடுக்கு,
              is_transfer ? "transfer" : "content");
        திரும்பு CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      எனில்(is_transfer && !is_chunked &&
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
        திரும்பு CURLE_BAD_CONTENT_ENCODING;
      }
      எனில்(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      இல்லையெனில் {
        எனில்(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (முழு)result);
        எனில்(result)
          திரும்பு result;

        result = Curl_cwriter_add(data, writer);
        எனில்(result) {
          Curl_cwriter_free(data, writer);
          திரும்பு result;
        }
      }
      எனில்(is_chunked)
        has_chunked = TRUE;
    }
  } போது(*enclist);

  திரும்பு CURLE_OK;
}

#இல்லையெனில்
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_அடுக்கு(கட்டமைப்பு Curl_easy *data,
                                     மாறிலி எழுத்து *enclist, முழு is_transfer)
{
  (வெறுமை)data;
  (வெறுமை)enclist;
  (வெறுமை)is_transfer;
  திரும்பு CURLE_NOT_BUILT_IN;
}

எழுத்து *Curl_get_content_encodings(வெறுமை)
{
  திரும்பு curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
