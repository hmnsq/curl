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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#ضمن <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ضمن <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
#endif

#ifdef HAVE_ZSTD
#ضمن <zstd.h>
#endif

#ضمن "connect.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_STACK 5

#אם defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#אם !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

تعريفنوع تعداد {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
بنية zlib_writer {
  بنية Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  حرف buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

ساكن voidpf zalloc_cb(voidpf opaque, unsigned صحيح items, unsigned صحيح size)
{
  (فراغ)opaque;
  /* not a typo, keep it curlx_calloc() */
  החזר curlx_calloc(items, size);
}

ساكن فراغ zfree_cb(voidpf opaque, voidpf ptr)
{
  (فراغ)opaque;
  curlx_free(ptr);
}

ساكن CURLcode process_zlib_error(بنية Curl_easy *data, z_stream *z)
{
  אם(z->msg)
    failf(data, "Error while processing content unencoding: %s", z->msg);
  אחרת
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  החזר CURLE_BAD_CONTENT_ENCODING;
}

ساكن CURLcode exit_zlib(بنية Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  אם(*zlib_init != ZLIB_UNINIT) {
    אם(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  החזר result;
}

ساكن CURLcode process_trailer(بنية Curl_easy *data, بنية zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  אם(z->avail_in)
    result = CURLE_WRITE_ERROR;
  אם(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  אחרת {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  החזר result;
}

ساكن CURLcode inflate_stream(بنية Curl_easy *data,
                               بنية Curl_cwriter *writer, صحيح type,
                               zlibInitState started)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  صحيح i = 0;

  /* Check state. */
  אם(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    החזר exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  כלעוד(!done) {
    صحيح status; /* zlib status */
    done = TRUE;

    אם(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      אם(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        החזר exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    אם(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      אם(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        אם(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          اكسر;
        }
      }
    }

    /* Dispatch by inflate() status. */
    بدل(status) {
    حالة Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      اكسر;
    حالة Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      اكسر;
    حالة Z_STREAM_END:
      אם((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        اكسر;
      }
      result = process_trailer(data, zp);
      اكسر;
    حالة Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      אם(zp->zlib_init == ZLIB_INIT) {
        אם(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          اكسر;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      اكسر;
    افتراضي:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      اكسر;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  אם(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  החזר result;
}

/* Deflate handler. */
ساكن CURLcode deflate_do_init(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  אם(inflateInit(z) != Z_OK)
    החזר process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  החזר CURLE_OK;
}

ساكن CURLcode deflate_do_write(بنية Curl_easy *data,
                                 بنية Curl_cwriter *writer, صحيح type,
                                 ثابت حرف *buf, size_t nbytes)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  אם(!(type & CLIENTWRITE_BODY) || !nbytes)
    החזר Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  אם(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    החזר process_trailer(data, zp);

  /* Now uncompress the data */
  החזר inflate_stream(data, writer, type, ZLIB_INFLATING);
}

ساكن فراغ deflate_do_close(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ساكن ثابت بنية Curl_cwtype deflate_encoding = {
  "deflate",
  عدم,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  حجم(بنية zlib_writer)
};

/*
 * Gzip handler.
 */

ساكن CURLcode gzip_do_init(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  אם(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    החזר process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  החזר CURLE_OK;
}

ساكن CURLcode gzip_do_write(بنية Curl_easy *data,
                              بنية Curl_cwriter *writer, صحيح type,
                              ثابت حرف *buf, size_t nbytes)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  אם(!(type & CLIENTWRITE_BODY) || !nbytes)
    החזר Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  אם(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    החזר inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  החזר exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ساكن فراغ gzip_do_close(بنية Curl_easy *data,
                          بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

ساكن ثابت بنية Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  حجم(بنية zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
بنية brotli_writer {
  بنية Curl_cwriter super;
  حرف buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ساكن CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  بدل(be) {
  حالة BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  حالة BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  حالة BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  حالة BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  حالة BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  حالة BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  حالة BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  حالة BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  حالة BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  حالة BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  حالة BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  حالة BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  حالة BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  حالة BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  حالة BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  حالة BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  حالة BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    החזר CURLE_BAD_CONTENT_ENCODING;
  حالة BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  حالة BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS:
  حالة BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  حالة BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  حالة BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  حالة BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    החזר CURLE_OUT_OF_MEMORY;
  افتراضي:
    اكسر;
  }
  החזר CURLE_WRITE_ERROR;
}

ساكن CURLcode brotli_do_init(بنية Curl_easy *data,
                               بنية Curl_cwriter *writer)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  (فراغ)data;

  bp->br = BrotliDecoderCreateInstance(عدم, عدم, عدم);
  החזר bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ساكن CURLcode brotli_do_write(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  ثابت uint8_t *src = (ثابت uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  صحيح i = 0;

  אם(!(type & CLIENTWRITE_BODY) || !nbytes)
    החזר Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  אם(!bp->br)
    החזר CURLE_WRITE_ERROR; /* Stream already ended. */

  כלעוד((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    אם(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      אם(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        החזר CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, عدم);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    אם(result)
      اكسر;
    بدل(r) {
    حالة BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    حالة BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT:
      اكسر;
    حالة BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = عدم;
      אם(nbytes)
        result = CURLE_WRITE_ERROR;
      اكسر;
    افتراضي:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      اكسر;
    }
  }
  החזר result;
}

ساكن فراغ brotli_do_close(بنية Curl_easy *data,
                            بنية Curl_cwriter *writer)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  (فراغ)data;

  אם(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = عدم;
  }
}

ساكن ثابت بنية Curl_cwtype brotli_encoding = {
  "br",
  عدم,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  حجم(بنية brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
بنية zstd_writer {
  بنية Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  حرف buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
ساكن فراغ *Curl_zstd_alloc(فراغ *opaque, size_t size)
{
  (فراغ)opaque;
  החזר Curl_cmalloc(size);
}

ساكن فراغ Curl_zstd_free(فراغ *opaque, فراغ *address)
{
  (فراغ)opaque;
  Curl_cfree(address);
}
#endif

ساكن CURLcode zstd_do_init(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer)
{
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;

  (فراغ)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = عدم
  });
#אחרת
  zp->zds = ZSTD_createDStream();
#endif

  החזר zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ساكن CURLcode zstd_do_write(بنية Curl_easy *data,
                              بنية Curl_cwriter *writer, صحيح type,
                              ثابت حرف *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  صحيح i = 0;

  אם(!(type & CLIENTWRITE_BODY) || !nbytes)
    החזר Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  עבור(;;) {
    אם(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      אם(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        החזר CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    אם(ZSTD_isError(errorCode)) {
      החזר CURLE_BAD_CONTENT_ENCODING;
    }
    אם(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      אם(result)
        اكسر;
    }
    אם((in.pos == nbytes) && (out.pos < out.size))
      اكسر;
  }

  החזר result;
}

ساكن فراغ zstd_do_close(بنية Curl_easy *data,
                          بنية Curl_cwriter *writer)
{
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;
  (فراغ)data;

  אם(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = عدم;
  }
}

ساكن ثابت بنية Curl_cwtype zstd_encoding = {
  "zstd",
  عدم,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  حجم(بنية zstd_writer)
};
#endif

/* Identity handler. */
ساكن ثابت بنية Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية Curl_cwriter)
};

/* supported general content decoders. */
ساكن ثابت بنية Curl_cwtype * ثابت general_unencoders[] = {
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
  عدم
};

/* supported content decoders only for transfer encodings */
ساكن ثابت بنية Curl_cwtype * ثابت transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  عدم
};

/* Return the list of comma-separated names of supported encodings.
 */
حرف *Curl_get_content_encodings(فراغ)
{
  بنية dynbuf enc;
  ثابت بنية Curl_cwtype * ثابت *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  עבור(cep = general_unencoders; *cep && !result; cep++) {
    ثابت بنية Curl_cwtype *ce = *cep;
    אם(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      אם(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      אם(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  אם(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  אם(!result)
    החזר curlx_dyn_ptr(&enc);
  החזר عدم;
}

/* Deferred error dummy writer. */
ساكن CURLcode error_do_init(بنية Curl_easy *data,
                              بنية Curl_cwriter *writer)
{
  (فراغ)data;
  (فراغ)writer;
  החזר CURLE_OK;
}

ساكن CURLcode error_do_write(بنية Curl_easy *data,
                               بنية Curl_cwriter *writer, صحيح type,
                               ثابت حرف *buf, size_t nbytes)
{
  (فراغ)writer;
  (فراغ)buf;
  (فراغ)nbytes;

  אם(!(type & CLIENTWRITE_BODY) || !nbytes)
    החזר Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  החזר CURLE_BAD_CONTENT_ENCODING;
}

ساكن فراغ error_do_close(بنية Curl_easy *data,
                           بنية Curl_cwriter *writer)
{
  (فراغ)data;
  (فراغ)writer;
}

ساكن ثابت بنية Curl_cwtype error_writer = {
  "ce-error",
  عدم,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  حجم(بنية Curl_cwriter)
};

/* Find the content encoding by name. */
ساكن ثابت بنية Curl_cwtype *find_unencode_writer(ثابت حرف *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ثابت بنية Curl_cwtype * ثابت *cep;

  אם(phase == CURL_CW_TRANSFER_DECODE) {
    עבור(cep = transfer_unencoders; *cep; cep++) {
      ثابت بنية Curl_cwtype *ce = *cep;
      אם((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        החזר ce;
    }
  }
  /* look among the general decoders */
  עבור(cep = general_unencoders; *cep; cep++) {
    ثابت بنية Curl_cwtype *ce = *cep;
    אם((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      החזר ce;
  }
  החזר عدم;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_stack(بنية Curl_easy *data,
                                     ثابت حرف *enclist, صحيح is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  עשה {
    ثابت حرف *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    כלעוד(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    עבור(namelen = 0; *enclist && *enclist != ','; enclist++)
      אם(*enclist > ' ')
        namelen = enclist - name + 1;

    אם(namelen) {
      ثابت بنية Curl_cwtype *cwt;
      بنية Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (صحيح)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      אם((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (صحيح)namelen, name);
        אם(is_transfer && !data->set.http_te_skip) {
          אם(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (صحيح)namelen, name);
          אחרת אם(is_identity)
            استمر;
          אחרת
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (صحيح)namelen, name);
          החזר CURLE_BAD_CONTENT_ENCODING;
        }
        החזר CURLE_OK;
      }

      אם(Curl_cwriter_count(data, phase) >= MAX_ENCODE_STACK) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_STACK,
              is_transfer ? "transfer" : "content");
        החזר CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      אם(is_transfer && !is_chunked &&
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
        החזר CURLE_BAD_CONTENT_ENCODING;
      }
      אם(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      אחרת {
        אם(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (صحيح)result);
        אם(result)
          החזר result;

        result = Curl_cwriter_add(data, writer);
        אם(result) {
          Curl_cwriter_free(data, writer);
          החזר result;
        }
      }
      אם(is_chunked)
        has_chunked = TRUE;
    }
  } כלעוד(*enclist);

  החזר CURLE_OK;
}

#אחרת
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_stack(بنية Curl_easy *data,
                                     ثابت حرف *enclist, صحيح is_transfer)
{
  (فراغ)data;
  (فراغ)enclist;
  (فراغ)is_transfer;
  החזר CURLE_NOT_BUILT_IN;
}

حرف *Curl_get_content_encodings(فراغ)
{
  החזר curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
