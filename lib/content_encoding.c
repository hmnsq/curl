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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#अन्तर्भाव <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic धकेलो
#pragma GCC diagnostic ignored "-Wvla"
#endif
#अन्तर्भाव <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic निकालो
#endif
#endif

#ifdef HAVE_ZSTD
#अन्तर्भाव <zstd.h>
#endif

#अन्तर्भाव "connect.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_ढेर 5

#यदि defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#यदि !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

प्रकारनिर्देश गणना {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
संरचना zlib_writer {
  संरचना Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  वर्ण buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

स्थिर voidpf zalloc_cb(voidpf opaque, अचिन्हित पूर्णाङ्क वस्तुएँ, अचिन्हित पूर्णाङ्क size)
{
  (शून्य)opaque;
  /* not a typo, keep it curlx_calloc() */
  प्रत्यागम curlx_calloc(वस्तुएँ, size);
}

स्थिर शून्य zfree_cb(voidpf opaque, voidpf ptr)
{
  (शून्य)opaque;
  curlx_free(ptr);
}

स्थिर CURLcode process_zlib_error(संरचना Curl_easy *data, z_stream *z)
{
  यदि(z->संदेश)
    failf(data, "Error while processing content unencoding: %s", z->संदेश);
  अन्यथा
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
}

स्थिर CURLcode exit_zlib(संरचना Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  यदि(*zlib_init != ZLIB_UNINIT) {
    यदि(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  प्रत्यागम result;
}

स्थिर CURLcode process_trailer(संरचना Curl_easy *data, संरचना zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  यदि(z->avail_in)
    result = CURLE_WRITE_ERROR;
  यदि(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  अन्यथा {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  प्रत्यागम result;
}

स्थिर CURLcode inflate_stream(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                               zlibInitState started)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  पूर्णाङ्क i = 0;

  /* Check state. */
  यदि(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    प्रत्यागम exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  यावत्(!done) {
    पूर्णाङ्क status; /* zlib status */
    done = TRUE;

    यदि(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      यदि(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        प्रत्यागम exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    यदि(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      यदि(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        यदि(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          भङ्ग;
        }
      }
    }

    /* Dispatch by inflate() status. */
    विकल्प(status) {
    पक्ष Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      भङ्ग;
    पक्ष Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      भङ्ग;
    पक्ष Z_STREAM_END:
      यदि((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        भङ्ग;
      }
      result = process_trailer(data, zp);
      भङ्ग;
    पक्ष Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      यदि(zp->zlib_init == ZLIB_INIT) {
        यदि(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          भङ्ग;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      भङ्ग;
    स्वभाव:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      भङ्ग;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  यदि(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  प्रत्यागम result;
}

/* Deflate handler. */
स्थिर CURLcode deflate_do_init(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  यदि(inflateInit(z) != Z_OK)
    प्रत्यागम process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode deflate_do_write(संरचना Curl_easy *data,
                                 संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                 ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  यदि(!(type & CLIENTWRITE_BODY) || !nbytes)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  यदि(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    प्रत्यागम process_trailer(data, zp);

  /* Now uncompress the data */
  प्रत्यागम inflate_stream(data, writer, type, ZLIB_INFLATING);
}

स्थिर शून्य deflate_do_close(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

स्थिर ध्रुव संरचना Curl_cwtype deflate_encoding = {
  "deflate",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  परिमाण(संरचना zlib_writer)
};

/*
 * Gzip handler.
 */

स्थिर CURLcode gzip_do_init(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  यदि(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    प्रत्यागम process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode gzip_do_write(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                              ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  यदि(!(type & CLIENTWRITE_BODY) || !nbytes)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  यदि(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    प्रत्यागम inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  प्रत्यागम exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

स्थिर शून्य gzip_do_close(संरचना Curl_easy *data,
                          संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

स्थिर ध्रुव संरचना Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  परिमाण(संरचना zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
संरचना brotli_writer {
  संरचना Curl_cwriter super;
  वर्ण buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

स्थिर CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  विकल्प(be) {
  पक्ष BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  पक्ष BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  पक्ष BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  पक्ष BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  पक्ष BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  पक्ष BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  पक्ष BROTLI_DECODER_ERROR_ALLOC_वृक्ष_GROUPS:
  पक्ष BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  पक्ष BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  पक्ष BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  पक्ष BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम CURLE_WRITE_ERROR;
}

स्थिर CURLcode brotli_do_init(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  (शून्य)data;

  bp->br = BrotliDecoderCreateInstance(रिक्त, रिक्त, रिक्त);
  प्रत्यागम bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode brotli_do_write(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  ध्रुव uint8_t *src = (ध्रुव uint8_t *)buf;
  uint8_t *dst;
  माप_प्रकार dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  पूर्णाङ्क i = 0;

  यदि(!(type & CLIENTWRITE_BODY) || !nbytes)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  यदि(!bp->br)
    प्रत्यागम CURLE_WRITE_ERROR; /* Stream already ended. */

  यावत्((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    यदि(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      यदि(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        प्रत्यागम CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, रिक्त);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    यदि(result)
      भङ्ग;
    विकल्प(r) {
    पक्ष BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    पक्ष BROTLI_DECODER_RESULT_NEEDS_MORE_निवेश:
      भङ्ग;
    पक्ष BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = रिक्त;
      यदि(nbytes)
        result = CURLE_WRITE_ERROR;
      भङ्ग;
    स्वभाव:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      भङ्ग;
    }
  }
  प्रत्यागम result;
}

स्थिर शून्य brotli_do_close(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  (शून्य)data;

  यदि(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = रिक्त;
  }
}

स्थिर ध्रुव संरचना Curl_cwtype brotli_encoding = {
  "br",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  परिमाण(संरचना brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
संरचना zstd_writer {
  संरचना Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  वर्ण buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
स्थिर शून्य *Curl_zstd_alloc(शून्य *opaque, माप_प्रकार size)
{
  (शून्य)opaque;
  प्रत्यागम Curl_cmalloc(size);
}

स्थिर शून्य Curl_zstd_free(शून्य *opaque, शून्य *address)
{
  (शून्य)opaque;
  Curl_cfree(address);
}
#endif

स्थिर CURLcode zstd_do_init(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना zstd_writer *zp = (संरचना zstd_writer *)writer;

  (शून्य)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = रिक्त
  });
#अन्यथा
  zp->zds = ZSTD_createDStream();
#endif

  प्रत्यागम zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode zstd_do_write(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                              ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  CURLcode result = CURLE_OK;
  संरचना zstd_writer *zp = (संरचना zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  माप_प्रकार errorCode;
  पूर्णाङ्क i = 0;

  यदि(!(type & CLIENTWRITE_BODY) || !nbytes)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  प्रति(;;) {
    यदि(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      यदि(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        प्रत्यागम CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    यदि(ZSTD_isError(errorCode)) {
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
    }
    यदि(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      यदि(result)
        भङ्ग;
    }
    यदि((in.pos == nbytes) && (out.pos < out.size))
      भङ्ग;
  }

  प्रत्यागम result;
}

स्थिर शून्य zstd_do_close(संरचना Curl_easy *data,
                          संरचना Curl_cwriter *writer)
{
  संरचना zstd_writer *zp = (संरचना zstd_writer *)writer;
  (शून्य)data;

  यदि(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = रिक्त;
  }
}

स्थिर ध्रुव संरचना Curl_cwtype zstd_encoding = {
  "zstd",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  परिमाण(संरचना zstd_writer)
};
#endif

/* Identity handler. */
स्थिर ध्रुव संरचना Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  परिमाण(संरचना Curl_cwriter)
};

/* supported general content decoders. */
स्थिर ध्रुव संरचना Curl_cwtype * ध्रुव general_unencoders[] = {
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
  रिक्त
};

/* supported content decoders only for transfer encodings */
स्थिर ध्रुव संरचना Curl_cwtype * ध्रुव transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  रिक्त
};

/* Return the list of comma-separated names of supported encodings.
 */
वर्ण *Curl_get_content_encodings(शून्य)
{
  संरचना dynbuf enc;
  ध्रुव संरचना Curl_cwtype * ध्रुव *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  प्रति(cep = general_unencoders; *cep && !result; cep++) {
    ध्रुव संरचना Curl_cwtype *ce = *cep;
    यदि(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      यदि(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      यदि(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  यदि(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  यदि(!result)
    प्रत्यागम curlx_dyn_ptr(&enc);
  प्रत्यागम रिक्त;
}

/* Deferred error dummy writer. */
स्थिर CURLcode error_do_init(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode error_do_write(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                               ध्रुव वर्ण *buf, माप_प्रकार nbytes)
{
  (शून्य)writer;
  (शून्य)buf;
  (शून्य)nbytes;

  यदि(!(type & CLIENTWRITE_BODY) || !nbytes)
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
}

स्थिर शून्य error_do_close(संरचना Curl_easy *data,
                           संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
}

स्थिर ध्रुव संरचना Curl_cwtype error_writer = {
  "ce-error",
  रिक्त,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  परिमाण(संरचना Curl_cwriter)
};

/* Find the content encoding by name. */
स्थिर ध्रुव संरचना Curl_cwtype *find_unencode_writer(ध्रुव वर्ण *name,
                                                      माप_प्रकार len,
                                                      Curl_cwriter_phase phase)
{
  ध्रुव संरचना Curl_cwtype * ध्रुव *cep;

  यदि(phase == CURL_CW_TRANSFER_DECODE) {
    प्रति(cep = transfer_unencoders; *cep; cep++) {
      ध्रुव संरचना Curl_cwtype *ce = *cep;
      यदि((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        प्रत्यागम ce;
    }
  }
  /* look among the general decoders */
  प्रति(cep = general_unencoders; *cep; cep++) {
    ध्रुव संरचना Curl_cwtype *ce = *cep;
    यदि((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      प्रत्यागम ce;
  }
  प्रत्यागम रिक्त;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ढेर(संरचना Curl_easy *data,
                                     ध्रुव वर्ण *enclist, पूर्णाङ्क is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  कुरु {
    ध्रुव वर्ण *name;
    माप_प्रकार namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    यावत्(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    प्रति(namelen = 0; *enclist && *enclist != ','; enclist++)
      यदि(*enclist > ' ')
        namelen = enclist - name + 1;

    यदि(namelen) {
      ध्रुव संरचना Curl_cwtype *cwt;
      संरचना Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (पूर्णाङ्क)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      यदि((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (पूर्णाङ्क)namelen, name);
        यदि(is_transfer && !data->set.http_te_skip) {
          यदि(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (पूर्णाङ्क)namelen, name);
          अन्यथा यदि(is_identity)
            अनुवर्त;
          अन्यथा
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (पूर्णाङ्क)namelen, name);
          प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
        }
        प्रत्यागम CURLE_OK;
      }

      यदि(Curl_cwriter_गिनती(data, phase) >= MAX_ENCODE_ढेर) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ढेर,
              is_transfer ? "transfer" : "content");
        प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      यदि(is_transfer && !is_chunked &&
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
        प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
      }
      यदि(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      अन्यथा {
        यदि(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (पूर्णाङ्क)result);
        यदि(result)
          प्रत्यागम result;

        result = Curl_cwriter_add(data, writer);
        यदि(result) {
          Curl_cwriter_free(data, writer);
          प्रत्यागम result;
        }
      }
      यदि(is_chunked)
        has_chunked = TRUE;
    }
  } यावत्(*enclist);

  प्रत्यागम CURLE_OK;
}

#अन्यथा
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ढेर(संरचना Curl_easy *data,
                                     ध्रुव वर्ण *enclist, पूर्णाङ्क is_transfer)
{
  (शून्य)data;
  (शून्य)enclist;
  (शून्य)is_transfer;
  प्रत्यागम CURLE_NOT_BUILT_IN;
}

वर्ण *Curl_get_content_encodings(शून्य)
{
  प्रत्यागम curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
