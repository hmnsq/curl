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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#অন্তর্ভাৱ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wvla"
#endif
#অন্তর্ভাৱ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বেৰ_কৰো
#endif
#endif

#ifdef HAVE_ZSTD
#অন্তর্ভাৱ <zstd.h>
#endif

#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_স্তূপ 5

#যদি defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#যদি !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

প্রকারনির্দেশ গণনা {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
সংরচনা zlib_writer {
  সংরচনা Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  ৱর্ণ buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

স্থির voidpf zalloc_cb(voidpf opaque, unsigned পূর্ণাঙ্ক বস্তুসমূহ, unsigned পূর্ণাঙ্ক size)
{
  (শূন্য)opaque;
  /* not a typo, keep it curlx_calloc() */
  ঘূৰাই curlx_calloc(বস্তুসমূহ, size);
}

স্থির শূন্য zfree_cb(voidpf opaque, voidpf ptr)
{
  (শূন্য)opaque;
  curlx_free(ptr);
}

স্থির CURLcode process_zlib_error(সংরচনা Curl_easy *data, z_stream *z)
{
  যদি(z->বাৰ্তা)
    failf(data, "Error while processing content unencoding: %s", z->বাৰ্তা);
  অন্যথা
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
}

স্থির CURLcode exit_zlib(সংরচনা Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  যদি(*zlib_init != ZLIB_UNINIT) {
    যদি(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ঘূৰাই result;
}

স্থির CURLcode process_trailer(সংরচনা Curl_easy *data, সংরচনা zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  যদি(z->avail_in)
    result = CURLE_WRITE_ERROR;
  যদি(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  অন্যথা {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ঘূৰাই result;
}

স্থির CURLcode inflate_stream(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                               zlibInitState started)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  পূর্ণাঙ্ক i = 0;

  /* Check state. */
  যদি(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ঘূৰাই exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  যেতিয়ালৈ(!done) {
    পূর্ণাঙ্ক status; /* zlib status */
    done = TRUE;

    যদি(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ঘূৰাই exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    যদি(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      যদি(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        যদি(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          ভঙ্গ;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ৱিকল্প(status) {
    পক্ষ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      ভঙ্গ;
    পক্ষ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ভঙ্গ;
    পক্ষ Z_STREAM_END:
      যদি((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ভঙ্গ;
      }
      result = process_trailer(data, zp);
      ভঙ্গ;
    পক্ষ Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      যদি(zp->zlib_init == ZLIB_INIT) {
        যদি(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          ভঙ্গ;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ভঙ্গ;
    স্ৱভাৱ:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      ভঙ্গ;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  যদি(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ঘূৰাই result;
}

/* Deflate handler. */
স্থির CURLcode deflate_do_init(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  যদি(inflateInit(z) != Z_OK)
    ঘূৰাই process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode deflate_do_write(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                 ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  যদি(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ঘূৰাই process_trailer(data, zp);

  /* Now uncompress the data */
  ঘূৰাই inflate_stream(data, writer, type, ZLIB_INFLATING);
}

স্থির শূন্য deflate_do_close(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype deflate_encoding = {
  "deflate",
  রিক্ত,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  পরিমাণ(সংরচনা zlib_writer)
};

/*
 * Gzip handler.
 */

স্থির CURLcode gzip_do_init(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  যদি(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ঘূৰাই process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode gzip_do_write(সংরচনা Curl_easy *data,
                              সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                              ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  যদি(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    ঘূৰাই inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ঘূৰাই exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

স্থির শূন্য gzip_do_close(সংরচনা Curl_easy *data,
                          সংরচনা Curl_cwriter *writer)
{
  সংরচনা zlib_writer *zp = (সংরচনা zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  পরিমাণ(সংরচনা zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
সংরচনা brotli_writer {
  সংরচনা Curl_cwriter super;
  ৱর্ণ buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

স্থির CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  ৱিকল্প(be) {
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  পক্ষ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  পক্ষ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  পক্ষ BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  পক্ষ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_বৃক্ষ_GROUPS:
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  পক্ষ BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই CURLE_WRITE_ERROR;
}

স্থির CURLcode brotli_do_init(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *writer)
{
  সংরচনা brotli_writer *bp = (সংরচনা brotli_writer *)writer;
  (শূন্য)data;

  bp->br = BrotliDecoderCreateInstance(রিক্ত, রিক্ত, রিক্ত);
  ঘূৰাই bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির CURLcode brotli_do_write(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  সংরচনা brotli_writer *bp = (সংরচনা brotli_writer *)writer;
  ধ্রুৱ uint8_t *src = (ধ্রুৱ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  পূর্ণাঙ্ক i = 0;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  যদি(!bp->br)
    ঘূৰাই CURLE_WRITE_ERROR; /* Stream already ended. */

  যেতিয়ালৈ((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    যদি(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ঘূৰাই CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, রিক্ত);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    যদি(result)
      ভঙ্গ;
    ৱিকল্প(r) {
    পক্ষ BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    পক্ষ BROTLI_DECODER_RESULT_NEEDS_MORE_প্ৰবেশ:
      ভঙ্গ;
    পক্ষ BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = রিক্ত;
      যদি(nbytes)
        result = CURLE_WRITE_ERROR;
      ভঙ্গ;
    স্ৱভাৱ:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      ভঙ্গ;
    }
  }
  ঘূৰাই result;
}

স্থির শূন্য brotli_do_close(সংরচনা Curl_easy *data,
                            সংরচনা Curl_cwriter *writer)
{
  সংরচনা brotli_writer *bp = (সংরচনা brotli_writer *)writer;
  (শূন্য)data;

  যদি(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = রিক্ত;
  }
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype brotli_encoding = {
  "br",
  রিক্ত,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  পরিমাণ(সংরচনা brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
সংরচনা zstd_writer {
  সংরচনা Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  ৱর্ণ buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
স্থির শূন্য *Curl_zstd_alloc(শূন্য *opaque, size_t size)
{
  (শূন্য)opaque;
  ঘূৰাই Curl_cmalloc(size);
}

স্থির শূন্য Curl_zstd_free(শূন্য *opaque, শূন্য *address)
{
  (শূন্য)opaque;
  Curl_cfree(address);
}
#endif

স্থির CURLcode zstd_do_init(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer)
{
  সংরচনা zstd_writer *zp = (সংরচনা zstd_writer *)writer;

  (শূন্য)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = রিক্ত
  });
#অন্যথা
  zp->zds = ZSTD_createDStream();
#endif

  ঘূৰাই zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির CURLcode zstd_do_write(সংরচনা Curl_easy *data,
                              সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                              ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  সংরচনা zstd_writer *zp = (সংরচনা zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  পূর্ণাঙ্ক i = 0;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  বাবে(;;) {
    যদি(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        ঘূৰাই CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    যদি(ZSTD_isError(errorCode)) {
      ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
    }
    যদি(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      যদি(result)
        ভঙ্গ;
    }
    যদি((in.pos == nbytes) && (out.pos < out.size))
      ভঙ্গ;
  }

  ঘূৰাই result;
}

স্থির শূন্য zstd_do_close(সংরচনা Curl_easy *data,
                          সংরচনা Curl_cwriter *writer)
{
  সংরচনা zstd_writer *zp = (সংরচনা zstd_writer *)writer;
  (শূন্য)data;

  যদি(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = রিক্ত;
  }
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype zstd_encoding = {
  "zstd",
  রিক্ত,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  পরিমাণ(সংরচনা zstd_writer)
};
#endif

/* Identity handler. */
স্থির ধ্রুৱ সংরচনা Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  পরিমাণ(সংরচনা Curl_cwriter)
};

/* supported general content decoders. */
স্থির ধ্রুৱ সংরচনা Curl_cwtype * ধ্রুৱ general_unencoders[] = {
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
  রিক্ত
};

/* supported content decoders only for transfer encodings */
স্থির ধ্রুৱ সংরচনা Curl_cwtype * ধ্রুৱ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  রিক্ত
};

/* Return the list of comma-separated names of supported encodings.
 */
ৱর্ণ *Curl_get_content_encodings(শূন্য)
{
  সংরচনা dynbuf enc;
  ধ্রুৱ সংরচনা Curl_cwtype * ধ্রুৱ *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  বাবে(cep = general_unencoders; *cep && !result; cep++) {
    ধ্রুৱ সংরচনা Curl_cwtype *ce = *cep;
    যদি(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      যদি(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      যদি(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  যদি(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  যদি(!result)
    ঘূৰাই curlx_dyn_ptr(&enc);
  ঘূৰাই রিক্ত;
}

/* Deferred error dummy writer. */
স্থির CURLcode error_do_init(সংরচনা Curl_easy *data,
                              সংরচনা Curl_cwriter *writer)
{
  (শূন্য)data;
  (শূন্য)writer;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode error_do_write(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                               ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  (শূন্য)writer;
  (শূন্য)buf;
  (শূন্য)nbytes;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
}

স্থির শূন্য error_do_close(সংরচনা Curl_easy *data,
                           সংরচনা Curl_cwriter *writer)
{
  (শূন্য)data;
  (শূন্য)writer;
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype error_writer = {
  "ce-error",
  রিক্ত,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  পরিমাণ(সংরচনা Curl_cwriter)
};

/* Find the content encoding by name. */
স্থির ধ্রুৱ সংরচনা Curl_cwtype *find_unencode_writer(ধ্রুৱ ৱর্ণ *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ধ্রুৱ সংরচনা Curl_cwtype * ধ্রুৱ *cep;

  যদি(phase == CURL_CW_TRANSFER_DECODE) {
    বাবে(cep = transfer_unencoders; *cep; cep++) {
      ধ্রুৱ সংরচনা Curl_cwtype *ce = *cep;
      যদি((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ঘূৰাই ce;
    }
  }
  /* look among the general decoders */
  বাবে(cep = general_unencoders; *cep; cep++) {
    ধ্রুৱ সংরচনা Curl_cwtype *ce = *cep;
    যদি((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ঘূৰাই ce;
  }
  ঘূৰাই রিক্ত;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_স্তূপ(সংরচনা Curl_easy *data,
                                     ধ্রুৱ ৱর্ণ *enclist, পূর্ণাঙ্ক is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  কৰা {
    ধ্রুৱ ৱর্ণ *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    যেতিয়ালৈ(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    বাবে(namelen = 0; *enclist && *enclist != ','; enclist++)
      যদি(*enclist > ' ')
        namelen = enclist - name + 1;

    যদি(namelen) {
      ধ্রুৱ সংরচনা Curl_cwtype *cwt;
      সংরচনা Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (পূর্ণাঙ্ক)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      যদি((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (পূর্ণাঙ্ক)namelen, name);
        যদি(is_transfer && !data->set.http_te_skip) {
          যদি(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (পূর্ণাঙ্ক)namelen, name);
          অন্যথা যদি(is_identity)
            অনুৱর্ত;
          অন্যথা
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (পূর্ণাঙ্ক)namelen, name);
          ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
        }
        ঘূৰাই CURLE_OK;
      }

      যদি(Curl_cwriter_গুনতি(data, phase) >= MAX_ENCODE_স্তূপ) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_স্তূপ,
              is_transfer ? "transfer" : "content");
        ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      যদি(is_transfer && !is_chunked &&
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
        ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
      }
      যদি(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      অন্যথা {
        যদি(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (পূর্ণাঙ্ক)result);
        যদি(result)
          ঘূৰাই result;

        result = Curl_cwriter_add(data, writer);
        যদি(result) {
          Curl_cwriter_free(data, writer);
          ঘূৰাই result;
        }
      }
      যদি(is_chunked)
        has_chunked = TRUE;
    }
  } যেতিয়ালৈ(*enclist);

  ঘূৰাই CURLE_OK;
}

#অন্যথা
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_স্তূপ(সংরচনা Curl_easy *data,
                                     ধ্রুৱ ৱর্ণ *enclist, পূর্ণাঙ্ক is_transfer)
{
  (শূন্য)data;
  (শূন্য)enclist;
  (শূন্য)is_transfer;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

ৱর্ণ *Curl_get_content_encodings(শূন্য)
{
  ঘূৰাই curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
