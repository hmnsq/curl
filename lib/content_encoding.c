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
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wvla"
#endif
#अन्तर्भाव <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic काढा
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
#define MAX_ENCODE_चळत 5

#जर defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#जर !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

प्रकारव्याख्या गणना {
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
  अक्षर buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

स्थिर voidpf zalloc_cb(voidpf opaque, अचिन्हित पूर्णांक वस्तुसंच, अचिन्हित पूर्णांक size)
{
  (शून्य)opaque;
  /* not a typo, keep it curlx_calloc() */
  परत curlx_calloc(वस्तुसंच, size);
}

स्थिर शून्य zfree_cb(voidpf opaque, voidpf ptr)
{
  (शून्य)opaque;
  curlx_free(ptr);
}

स्थिर CURLcode process_zlib_error(संरचना Curl_easy *data, z_stream *z)
{
  जर(z->संदेश)
    failf(data, "Error while processing content unencoding: %s", z->संदेश);
  अन्यथा
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  परत CURLE_BAD_CONTENT_ENCODING;
}

स्थिर CURLcode exit_zlib(संरचना Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  जर(*zlib_init != ZLIB_UNINIT) {
    जर(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  परत result;
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
  जर(z->avail_in)
    result = CURLE_WRITE_ERROR;
  जर(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  अन्यथा {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  परत result;
}

स्थिर CURLcode inflate_stream(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer, पूर्णांक type,
                               zlibInitState started)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  पूर्णांक i = 0;

  /* Check state. */
  जर(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    परत exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  पर्यंत(!done) {
    पूर्णांक status; /* zlib status */
    done = TRUE;

    जर(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      जर(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        परत exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    जर(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      जर(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        जर(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          थांबा;
        }
      }
    }

    /* Dispatch by inflate() status. */
    निवड(status) {
    स्थिती Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      थांबा;
    स्थिती Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      थांबा;
    स्थिती Z_STREAM_END:
      जर((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        थांबा;
      }
      result = process_trailer(data, zp);
      थांबा;
    स्थिती Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      जर(zp->zlib_init == ZLIB_INIT) {
        जर(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          थांबा;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      थांबा;
    पूर्वनिर्धारित:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      थांबा;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  जर(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  परत result;
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

  जर(inflateInit(z) != Z_OK)
    परत process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  परत CURLE_OK;
}

स्थिर CURLcode deflate_do_write(संरचना Curl_easy *data,
                                 संरचना Curl_cwriter *writer, पूर्णांक type,
                                 स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  जर(!(type & CLIENTWRITE_BODY) || !nbytes)
    परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  जर(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    परत process_trailer(data, zp);

  /* Now uncompress the data */
  परत inflate_stream(data, writer, type, ZLIB_INFLATING);
}

स्थिर शून्य deflate_do_close(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

स्थिर स्थिरांक संरचना Curl_cwtype deflate_encoding = {
  "deflate",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  आकार(संरचना zlib_writer)
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

  जर(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    परत process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  परत CURLE_OK;
}

स्थिर CURLcode gzip_do_write(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer, पूर्णांक type,
                              स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  जर(!(type & CLIENTWRITE_BODY) || !nbytes)
    परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  जर(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    परत inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  परत exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

स्थिर शून्य gzip_do_close(संरचना Curl_easy *data,
                          संरचना Curl_cwriter *writer)
{
  संरचना zlib_writer *zp = (संरचना zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

स्थिर स्थिरांक संरचना Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  आकार(संरचना zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
संरचना brotli_writer {
  संरचना Curl_cwriter super;
  अक्षर buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

स्थिर CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  निवड(be) {
  स्थिती BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  स्थिती BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  स्थिती BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  स्थिती BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  स्थिती BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    परत CURLE_BAD_CONTENT_ENCODING;
  स्थिती BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  स्थिती BROTLI_DECODER_ERROR_ALLOC_वृक्ष_GROUPS:
  स्थिती BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  स्थिती BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  स्थिती BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  स्थिती BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    परत CURLE_OUT_OF_MEMORY;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत CURLE_WRITE_ERROR;
}

स्थिर CURLcode brotli_do_init(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  (शून्य)data;

  bp->br = BrotliDecoderCreateInstance(रिक्त, रिक्त, रिक्त);
  परत bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode brotli_do_write(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer, पूर्णांक type,
                                स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  स्थिरांक uint8_t *src = (स्थिरांक uint8_t *)buf;
  uint8_t *dst;
  माप_प्रकार dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  पूर्णांक i = 0;

  जर(!(type & CLIENTWRITE_BODY) || !nbytes)
    परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  जर(!bp->br)
    परत CURLE_WRITE_ERROR; /* Stream already ended. */

  पर्यंत((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    जर(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      जर(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        परत CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, रिक्त);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    जर(result)
      थांबा;
    निवड(r) {
    स्थिती BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    स्थिती BROTLI_DECODER_RESULT_NEEDS_MORE_आदान:
      थांबा;
    स्थिती BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = रिक्त;
      जर(nbytes)
        result = CURLE_WRITE_ERROR;
      थांबा;
    पूर्वनिर्धारित:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      थांबा;
    }
  }
  परत result;
}

स्थिर शून्य brotli_do_close(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer)
{
  संरचना brotli_writer *bp = (संरचना brotli_writer *)writer;
  (शून्य)data;

  जर(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = रिक्त;
  }
}

स्थिर स्थिरांक संरचना Curl_cwtype brotli_encoding = {
  "br",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  आकार(संरचना brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
संरचना zstd_writer {
  संरचना Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  अक्षर buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
स्थिर शून्य *Curl_zstd_alloc(शून्य *opaque, माप_प्रकार size)
{
  (शून्य)opaque;
  परत Curl_cmalloc(size);
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

  परत zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode zstd_do_write(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer, पूर्णांक type,
                              स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  CURLcode result = CURLE_OK;
  संरचना zstd_writer *zp = (संरचना zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  माप_प्रकार errorCode;
  पूर्णांक i = 0;

  जर(!(type & CLIENTWRITE_BODY) || !nbytes)
    परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  साठी(;;) {
    जर(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      जर(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        परत CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    जर(ZSTD_isError(errorCode)) {
      परत CURLE_BAD_CONTENT_ENCODING;
    }
    जर(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      जर(result)
        थांबा;
    }
    जर((in.pos == nbytes) && (out.pos < out.size))
      थांबा;
  }

  परत result;
}

स्थिर शून्य zstd_do_close(संरचना Curl_easy *data,
                          संरचना Curl_cwriter *writer)
{
  संरचना zstd_writer *zp = (संरचना zstd_writer *)writer;
  (शून्य)data;

  जर(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = रिक्त;
  }
}

स्थिर स्थिरांक संरचना Curl_cwtype zstd_encoding = {
  "zstd",
  रिक्त,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  आकार(संरचना zstd_writer)
};
#endif

/* Identity handler. */
स्थिर स्थिरांक संरचना Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  आकार(संरचना Curl_cwriter)
};

/* supported general content decoders. */
स्थिर स्थिरांक संरचना Curl_cwtype * स्थिरांक general_unencoders[] = {
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
स्थिर स्थिरांक संरचना Curl_cwtype * स्थिरांक transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  रिक्त
};

/* Return the list of comma-separated names of supported encodings.
 */
अक्षर *Curl_get_content_encodings(शून्य)
{
  संरचना dynbuf enc;
  स्थिरांक संरचना Curl_cwtype * स्थिरांक *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  साठी(cep = general_unencoders; *cep && !result; cep++) {
    स्थिरांक संरचना Curl_cwtype *ce = *cep;
    जर(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      जर(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      जर(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  जर(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  जर(!result)
    परत curlx_dyn_ptr(&enc);
  परत रिक्त;
}

/* Deferred error dummy writer. */
स्थिर CURLcode error_do_init(संरचना Curl_easy *data,
                              संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
  परत CURLE_OK;
}

स्थिर CURLcode error_do_write(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer, पूर्णांक type,
                               स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  (शून्य)writer;
  (शून्य)buf;
  (शून्य)nbytes;

  जर(!(type & CLIENTWRITE_BODY) || !nbytes)
    परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  परत CURLE_BAD_CONTENT_ENCODING;
}

स्थिर शून्य error_do_close(संरचना Curl_easy *data,
                           संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
}

स्थिर स्थिरांक संरचना Curl_cwtype error_writer = {
  "ce-error",
  रिक्त,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  आकार(संरचना Curl_cwriter)
};

/* Find the content encoding by name. */
स्थिर स्थिरांक संरचना Curl_cwtype *find_unencode_writer(स्थिरांक अक्षर *name,
                                                      माप_प्रकार len,
                                                      Curl_cwriter_phase phase)
{
  स्थिरांक संरचना Curl_cwtype * स्थिरांक *cep;

  जर(phase == CURL_CW_TRANSFER_DECODE) {
    साठी(cep = transfer_unencoders; *cep; cep++) {
      स्थिरांक संरचना Curl_cwtype *ce = *cep;
      जर((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        परत ce;
    }
  }
  /* look among the general decoders */
  साठी(cep = general_unencoders; *cep; cep++) {
    स्थिरांक संरचना Curl_cwtype *ce = *cep;
    जर((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      परत ce;
  }
  परत रिक्त;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_चळत(संरचना Curl_easy *data,
                                     स्थिरांक अक्षर *enclist, पूर्णांक is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  करा {
    स्थिरांक अक्षर *name;
    माप_प्रकार namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    पर्यंत(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    साठी(namelen = 0; *enclist && *enclist != ','; enclist++)
      जर(*enclist > ' ')
        namelen = enclist - name + 1;

    जर(namelen) {
      स्थिरांक संरचना Curl_cwtype *cwt;
      संरचना Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (पूर्णांक)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      जर((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (पूर्णांक)namelen, name);
        जर(is_transfer && !data->set.http_te_skip) {
          जर(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (पूर्णांक)namelen, name);
          अन्यथा जर(is_identity)
            सुरूठेवा;
          अन्यथा
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (पूर्णांक)namelen, name);
          परत CURLE_BAD_CONTENT_ENCODING;
        }
        परत CURLE_OK;
      }

      जर(Curl_cwriter_मोजणी(data, phase) >= MAX_ENCODE_चळत) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_चळत,
              is_transfer ? "transfer" : "content");
        परत CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      जर(is_transfer && !is_chunked &&
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
        परत CURLE_BAD_CONTENT_ENCODING;
      }
      जर(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      अन्यथा {
        जर(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (पूर्णांक)result);
        जर(result)
          परत result;

        result = Curl_cwriter_add(data, writer);
        जर(result) {
          Curl_cwriter_free(data, writer);
          परत result;
        }
      }
      जर(is_chunked)
        has_chunked = TRUE;
    }
  } पर्यंत(*enclist);

  परत CURLE_OK;
}

#अन्यथा
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_चळत(संरचना Curl_easy *data,
                                     स्थिरांक अक्षर *enclist, पूर्णांक is_transfer)
{
  (शून्य)data;
  (शून्य)enclist;
  (शून्य)is_transfer;
  परत CURLE_NOT_BUILT_IN;
}

अक्षर *Curl_get_content_encodings(शून्य)
{
  परत curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
