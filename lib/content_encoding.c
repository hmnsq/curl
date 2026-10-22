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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#شامل <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wvla"
#endif
#شامل <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic برداشت
#endif
#endif

#ifdef HAVE_ZSTD
#شامل <zstd.h>
#endif

#شامل "connect.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_پشته 5

#اگر defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#اگر !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

تعریفنوع شمارش {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ساختار zlib_writer {
  ساختار Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  نویسه buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

ایستا voidpf zalloc_cb(voidpf opaque, unsigned صحیح اقلام, unsigned صحیح size)
{
  (تهی)opaque;
  /* not a typo, keep it curlx_calloc() */
  بازگشت curlx_calloc(اقلام, size);
}

ایستا تهی zfree_cb(voidpf opaque, voidpf ptr)
{
  (تهی)opaque;
  curlx_free(ptr);
}

ایستا CURLcode process_zlib_error(ساختار Curl_easy *data, z_stream *z)
{
  اگر(z->پیام)
    failf(data, "Error while processing content unencoding: %s", z->پیام);
  وگرنه
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  بازگشت CURLE_BAD_CONTENT_ENCODING;
}

ایستا CURLcode exit_zlib(ساختار Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  اگر(*zlib_init != ZLIB_UNINIT) {
    اگر(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  بازگشت result;
}

ایستا CURLcode process_trailer(ساختار Curl_easy *data, ساختار zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  اگر(z->avail_in)
    result = CURLE_WRITE_ERROR;
  اگر(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  وگرنه {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  بازگشت result;
}

ایستا CURLcode inflate_stream(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *writer, صحیح type,
                               zlibInitState started)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  صحیح i = 0;

  /* Check state. */
  اگر(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    بازگشت exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  وقتی(!done) {
    صحیح status; /* zlib status */
    done = TRUE;

    اگر(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        بازگشت exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    اگر(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      اگر(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        اگر(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          بشکن;
        }
      }
    }

    /* Dispatch by inflate() status. */
    انتخاب(status) {
    حالت Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      بشکن;
    حالت Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      بشکن;
    حالت Z_STREAM_END:
      اگر((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        بشکن;
      }
      result = process_trailer(data, zp);
      بشکن;
    حالت Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      اگر(zp->zlib_init == ZLIB_INIT) {
        اگر(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          بشکن;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      بشکن;
    پیشفرض:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      بشکن;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  اگر(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  بازگشت result;
}

/* Deflate handler. */
ایستا CURLcode deflate_do_init(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  اگر(inflateInit(z) != Z_OK)
    بازگشت process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  بازگشت CURLE_OK;
}

ایستا CURLcode deflate_do_write(ساختار Curl_easy *data,
                                 ساختار Curl_cwriter *writer, صحیح type,
                                 ثابت نویسه *buf, size_t nbytes)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  اگر(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    بازگشت process_trailer(data, zp);

  /* Now uncompress the data */
  بازگشت inflate_stream(data, writer, type, ZLIB_INFLATING);
}

ایستا تهی deflate_do_close(ساختار Curl_easy *data,
                             ساختار Curl_cwriter *writer)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ایستا ثابت ساختار Curl_cwtype deflate_encoding = {
  "deflate",
  پوچ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  اندازه(ساختار zlib_writer)
};

/*
 * Gzip handler.
 */

ایستا CURLcode gzip_do_init(ساختار Curl_easy *data,
                             ساختار Curl_cwriter *writer)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  اگر(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    بازگشت process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  بازگشت CURLE_OK;
}

ایستا CURLcode gzip_do_write(ساختار Curl_easy *data,
                              ساختار Curl_cwriter *writer, صحیح type,
                              ثابت نویسه *buf, size_t nbytes)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  اگر(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    بازگشت inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  بازگشت exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ایستا تهی gzip_do_close(ساختار Curl_easy *data,
                          ساختار Curl_cwriter *writer)
{
  ساختار zlib_writer *zp = (ساختار zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ایستا ثابت ساختار Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  اندازه(ساختار zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
ساختار brotli_writer {
  ساختار Curl_cwriter super;
  نویسه buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ایستا CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  انتخاب(be) {
  حالت BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  حالت BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  حالت BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  حالت BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  حالت BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  حالت BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  حالت BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  حالت BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  حالت BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  حالت BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  حالت BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  حالت BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  حالت BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  حالت BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  حالت BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  حالت BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  حالت BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    بازگشت CURLE_BAD_CONTENT_ENCODING;
  حالت BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  حالت BROTLI_DECODER_ERROR_ALLOC_درخت_GROUPS:
  حالت BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  حالت BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  حالت BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  حالت BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    بازگشت CURLE_OUT_OF_MEMORY;
  پیشفرض:
    بشکن;
  }
  بازگشت CURLE_WRITE_ERROR;
}

ایستا CURLcode brotli_do_init(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *writer)
{
  ساختار brotli_writer *bp = (ساختار brotli_writer *)writer;
  (تهی)data;

  bp->br = BrotliDecoderCreateInstance(پوچ, پوچ, پوچ);
  بازگشت bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ایستا CURLcode brotli_do_write(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer, صحیح type,
                                ثابت نویسه *buf, size_t nbytes)
{
  ساختار brotli_writer *bp = (ساختار brotli_writer *)writer;
  ثابت uint8_t *src = (ثابت uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  صحیح i = 0;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  اگر(!bp->br)
    بازگشت CURLE_WRITE_ERROR; /* Stream already ended. */

  وقتی((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    اگر(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        بازگشت CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, پوچ);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    اگر(result)
      بشکن;
    انتخاب(r) {
    حالت BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    حالت BROTLI_DECODER_RESULT_NEEDS_MORE_درون_داد:
      بشکن;
    حالت BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = پوچ;
      اگر(nbytes)
        result = CURLE_WRITE_ERROR;
      بشکن;
    پیشفرض:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      بشکن;
    }
  }
  بازگشت result;
}

ایستا تهی brotli_do_close(ساختار Curl_easy *data,
                            ساختار Curl_cwriter *writer)
{
  ساختار brotli_writer *bp = (ساختار brotli_writer *)writer;
  (تهی)data;

  اگر(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = پوچ;
  }
}

ایستا ثابت ساختار Curl_cwtype brotli_encoding = {
  "br",
  پوچ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  اندازه(ساختار brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
ساختار zstd_writer {
  ساختار Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  نویسه buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
ایستا تهی *Curl_zstd_alloc(تهی *opaque, size_t size)
{
  (تهی)opaque;
  بازگشت Curl_cmalloc(size);
}

ایستا تهی Curl_zstd_free(تهی *opaque, تهی *address)
{
  (تهی)opaque;
  Curl_cfree(address);
}
#endif

ایستا CURLcode zstd_do_init(ساختار Curl_easy *data,
                             ساختار Curl_cwriter *writer)
{
  ساختار zstd_writer *zp = (ساختار zstd_writer *)writer;

  (تهی)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = پوچ
  });
#وگرنه
  zp->zds = ZSTD_createDStream();
#endif

  بازگشت zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ایستا CURLcode zstd_do_write(ساختار Curl_easy *data,
                              ساختار Curl_cwriter *writer, صحیح type,
                              ثابت نویسه *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  ساختار zstd_writer *zp = (ساختار zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  صحیح i = 0;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  برای(;;) {
    اگر(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        بازگشت CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    اگر(ZSTD_isError(errorCode)) {
      بازگشت CURLE_BAD_CONTENT_ENCODING;
    }
    اگر(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      اگر(result)
        بشکن;
    }
    اگر((in.pos == nbytes) && (out.pos < out.size))
      بشکن;
  }

  بازگشت result;
}

ایستا تهی zstd_do_close(ساختار Curl_easy *data,
                          ساختار Curl_cwriter *writer)
{
  ساختار zstd_writer *zp = (ساختار zstd_writer *)writer;
  (تهی)data;

  اگر(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = پوچ;
  }
}

ایستا ثابت ساختار Curl_cwtype zstd_encoding = {
  "zstd",
  پوچ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  اندازه(ساختار zstd_writer)
};
#endif

/* Identity handler. */
ایستا ثابت ساختار Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  اندازه(ساختار Curl_cwriter)
};

/* supported general content decoders. */
ایستا ثابت ساختار Curl_cwtype * ثابت general_unencoders[] = {
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
  پوچ
};

/* supported content decoders only for transfer encodings */
ایستا ثابت ساختار Curl_cwtype * ثابت transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  پوچ
};

/* Return the list of comma-separated names of supported encodings.
 */
نویسه *Curl_get_content_encodings(تهی)
{
  ساختار dynbuf enc;
  ثابت ساختار Curl_cwtype * ثابت *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  برای(cep = general_unencoders; *cep && !result; cep++) {
    ثابت ساختار Curl_cwtype *ce = *cep;
    اگر(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      اگر(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      اگر(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  اگر(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  اگر(!result)
    بازگشت curlx_dyn_ptr(&enc);
  بازگشت پوچ;
}

/* Deferred error dummy writer. */
ایستا CURLcode error_do_init(ساختار Curl_easy *data,
                              ساختار Curl_cwriter *writer)
{
  (تهی)data;
  (تهی)writer;
  بازگشت CURLE_OK;
}

ایستا CURLcode error_do_write(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *writer, صحیح type,
                               ثابت نویسه *buf, size_t nbytes)
{
  (تهی)writer;
  (تهی)buf;
  (تهی)nbytes;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  بازگشت CURLE_BAD_CONTENT_ENCODING;
}

ایستا تهی error_do_close(ساختار Curl_easy *data,
                           ساختار Curl_cwriter *writer)
{
  (تهی)data;
  (تهی)writer;
}

ایستا ثابت ساختار Curl_cwtype error_writer = {
  "ce-error",
  پوچ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  اندازه(ساختار Curl_cwriter)
};

/* Find the content encoding by name. */
ایستا ثابت ساختار Curl_cwtype *find_unencode_writer(ثابت نویسه *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ثابت ساختار Curl_cwtype * ثابت *cep;

  اگر(phase == CURL_CW_TRANSFER_DECODE) {
    برای(cep = transfer_unencoders; *cep; cep++) {
      ثابت ساختار Curl_cwtype *ce = *cep;
      اگر((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        بازگشت ce;
    }
  }
  /* look among the general decoders */
  برای(cep = general_unencoders; *cep; cep++) {
    ثابت ساختار Curl_cwtype *ce = *cep;
    اگر((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      بازگشت ce;
  }
  بازگشت پوچ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_پشته(ساختار Curl_easy *data,
                                     ثابت نویسه *enclist, صحیح is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  انجامبده {
    ثابت نویسه *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    وقتی(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    برای(namelen = 0; *enclist && *enclist != ','; enclist++)
      اگر(*enclist > ' ')
        namelen = enclist - name + 1;

    اگر(namelen) {
      ثابت ساختار Curl_cwtype *cwt;
      ساختار Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (صحیح)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      اگر((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (صحیح)namelen, name);
        اگر(is_transfer && !data->set.http_te_skip) {
          اگر(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (صحیح)namelen, name);
          وگرنه اگر(is_identity)
            ادامه;
          وگرنه
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (صحیح)namelen, name);
          بازگشت CURLE_BAD_CONTENT_ENCODING;
        }
        بازگشت CURLE_OK;
      }

      اگر(Curl_cwriter_شمار(data, phase) >= MAX_ENCODE_پشته) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_پشته,
              is_transfer ? "transfer" : "content");
        بازگشت CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      اگر(is_transfer && !is_chunked &&
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
        بازگشت CURLE_BAD_CONTENT_ENCODING;
      }
      اگر(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      وگرنه {
        اگر(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (صحیح)result);
        اگر(result)
          بازگشت result;

        result = Curl_cwriter_add(data, writer);
        اگر(result) {
          Curl_cwriter_free(data, writer);
          بازگشت result;
        }
      }
      اگر(is_chunked)
        has_chunked = TRUE;
    }
  } وقتی(*enclist);

  بازگشت CURLE_OK;
}

#وگرنه
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_پشته(ساختار Curl_easy *data,
                                     ثابت نویسه *enclist, صحیح is_transfer)
{
  (تهی)data;
  (تهی)enclist;
  (تهی)is_transfer;
  بازگشت CURLE_NOT_BUILT_IN;
}

نویسه *Curl_get_content_encodings(تهی)
{
  بازگشت curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
