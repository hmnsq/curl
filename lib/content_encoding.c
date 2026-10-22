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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#অন্তর্ভাব <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wvla"
#endif
#অন্তর্ভাব <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif
#endif

#ifdef HAVE_ZSTD
#অন্তর্ভাব <zstd.h>
#endif

#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_স্তূপ 5

#যদি defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_বাফার_আকার 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#যদি !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

প্রকারলেখ গণনা {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
কাঠামো zlib_writer {
  কাঠামো Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  অক্ষর বাফার[DECOMPRESS_বাফার_আকার]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

স্থির voidpf zalloc_cb(voidpf opaque, unsigned পূর্ণসংখ্যা বস্তুসমূহ, unsigned পূর্ণসংখ্যা আকার_2)
{
  (শূন্য)opaque;
  /* not a typo, keep it curlx_calloc() */
  ফেরত curlx_calloc(বস্তুসমূহ, আকার_2);
}

স্থির শূন্য zfree_cb(voidpf opaque, voidpf ptr)
{
  (শূন্য)opaque;
  curlx_free(ptr);
}

স্থির CURLcode process_zlib_error(কাঠামো Curl_easy *তথ্য, z_stream *z)
{
  যদি(z->বার্তা)
    failf(তথ্য, "Error while processing content unencoding: %s", z->বার্তা);
  অন্যথা
    failf(তথ্য, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ফেরত CURLE_BAD_CONTENT_ENCODING;
}

স্থির CURLcode exit_zlib(কাঠামো Curl_easy *তথ্য, z_stream *z,
                          zlibInitState *zlib_init, CURLcode ফলাফল)
{
  যদি(*zlib_init != ZLIB_UNINIT) {
    যদি(inflateEnd(z) != Z_OK && ফলাফল == CURLE_OK)
      ফলাফল = process_zlib_error(তথ্য, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ফেরত ফলাফল;
}

স্থির CURLcode process_trailer(কাঠামো Curl_easy *তথ্য, কাঠামো zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode ফলাফল = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->পরবর্তী_in += len;
  যদি(z->avail_in)
    ফলাফল = CURLE_WRITE_ERROR;
  যদি(ফলাফল || !zp->trailerlen)
    ফলাফল = exit_zlib(তথ্য, z, &zp->zlib_init, ফলাফল);
  অন্যথা {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode inflate_stream(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                               zlibInitState started)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->পরবর্তী_in;
  bool done = FALSE;
  CURLcode ফলাফল = CURLE_OK;   /* Curl_client_write status */
  পূর্ণসংখ্যা i = 0;

  /* Check state. */
  যদি(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ফেরত exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  যখন(!done) {
    পূর্ণসংখ্যা status; /* zlib status */
    done = TRUE;

    যদি(++i > (1024 * 1024 / DECOMPRESS_বাফার_আকার)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(তথ্য) < 0) {
        failf(তথ্য, "Operation timed out while decoding payload");
        ফেরত exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->পরবর্তী_out = (Bytef *)zp->বাফার;
    z->avail_out = DECOMPRESS_বাফার_আকার;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    যদি(z->avail_out != DECOMPRESS_বাফার_আকার) {
      যদি(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, zp->বাফার,
                                    DECOMPRESS_বাফার_আকার - z->avail_out);
        যদি(ফলাফল) {
          exit_zlib(তথ্য, z, &zp->zlib_init, ফলাফল);
          বিরতি;
        }
      }
    }

    /* Dispatch by inflate() status. */
    নির্বাচন(status) {
    ক্ষেত্র Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      বিরতি;
    ক্ষেত্র Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      বিরতি;
    ক্ষেত্র Z_STREAM_END:
      যদি((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->পরবর্তী_in[0] == 0x1f) && (z->পরবর্তী_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(তথ্য, "Multi-member gzip response not supported");
        ফলাফল = exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        বিরতি;
      }
      ফলাফল = process_trailer(তথ্য, zp);
      বিরতি;
    ক্ষেত্র Z_তথ্য_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      যদি(zp->zlib_init == ZLIB_INIT) {
        যদি(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->পরবর্তী_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          বিরতি;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      ফলাফল = exit_zlib(তথ্য, z, &zp->zlib_init, process_zlib_error(তথ্য, z));
      বিরতি;
    পূর্বনির্ধারিত:
      ফলাফল = exit_zlib(তথ্য, z, &zp->zlib_init, process_zlib_error(তথ্য, z));
      বিরতি;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  যদি(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ফেরত ফলাফল;
}

/* Deflate handler. */
স্থির CURLcode deflate_do_init(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  যদি(inflateInit(z) != Z_OK)
    ফেরত process_zlib_error(তথ্য, z);
  zp->zlib_init = ZLIB_INIT;
  ফেরত CURLE_OK;
}

স্থির CURLcode deflate_do_write(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                 ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->পরবর্তী_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  যদি(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ফেরত process_trailer(তথ্য, zp);

  /* Now uncompress the data */
  ফেরত inflate_stream(তথ্য, writer, type, ZLIB_INFLATING);
}

স্থির শূন্য deflate_do_close(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_OK);
}

স্থির ধ্রুবক কাঠামো Curl_cwtype deflate_encoding = {
  "deflate",
  ফাঁকা,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  আকার(কাঠামো zlib_writer)
};

/*
 * Gzip handler.
 */

স্থির CURLcode gzip_do_init(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  যদি(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ফেরত process_zlib_error(তথ্য, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ফেরত CURLE_OK;
}

স্থির CURLcode gzip_do_write(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                              ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);

  যদি(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->পরবর্তী_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    ফেরত inflate_stream(তথ্য, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ফেরত exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

স্থির শূন্য gzip_do_close(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_cwriter *writer)
{
  কাঠামো zlib_writer *zp = (কাঠামো zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(তথ্য, z, &zp->zlib_init, CURLE_OK);
}

স্থির ধ্রুবক কাঠামো Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  আকার(কাঠামো zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
কাঠামো brotli_writer {
  কাঠামো Curl_cwriter super;
  অক্ষর বাফার[DECOMPRESS_বাফার_আকার];
  BrotliDecoderState *br; /* State structure for brotli. */
};

স্থির CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  নির্বাচন(be) {
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_BLOCK_দৈর্ঘ্য_1:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_BLOCK_দৈর্ঘ্য_2:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  ক্ষেত্র BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  ক্ষেত্র BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  ক্ষেত্র BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  ক্ষেত্র BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_বৃক্ষ_GROUPS:
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_RING_বাফার_1:
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_RING_বাফার_2:
  ক্ষেত্র BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    ফেরত CURLE_OUT_OF_MEMORY;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত CURLE_WRITE_ERROR;
}

স্থির CURLcode brotli_do_init(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *writer)
{
  কাঠামো brotli_writer *bp = (কাঠামো brotli_writer *)writer;
  (শূন্য)তথ্য;

  bp->br = BrotliDecoderCreateInstance(ফাঁকা, ফাঁকা, ফাঁকা);
  ফেরত bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির CURLcode brotli_do_write(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  কাঠামো brotli_writer *bp = (কাঠামো brotli_writer *)writer;
  ধ্রুবক uint8_t *src = (ধ্রুবক uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode ফলাফল = CURLE_OK;
  Brotli_Decoder_ফলাফল r = BROTLI_DECODER_ফলাফল_NEEDS_MORE_OUTPUT;
  পূর্ণসংখ্যা i = 0;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);

  যদি(!bp->br)
    ফেরত CURLE_WRITE_ERROR; /* Stream already ended. */

  যখন((nbytes || r == BROTLI_DECODER_ফলাফল_NEEDS_MORE_OUTPUT) &&
        ফলাফল == CURLE_OK) {

    যদি(++i > (1024 * 1024 / DECOMPRESS_বাফার_আকার)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(তথ্য) < 0) {
        failf(তথ্য, "Operation timed out while decoding payload");
        ফেরত CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->বাফার;
    dstleft = DECOMPRESS_বাফার_আকার;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ফাঁকা);
    ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, type,
                                bp->বাফার, DECOMPRESS_বাফার_আকার - dstleft);
    যদি(ফলাফল)
      বিরতি;
    নির্বাচন(r) {
    ক্ষেত্র BROTLI_DECODER_ফলাফল_NEEDS_MORE_OUTPUT:
    ক্ষেত্র BROTLI_DECODER_ফলাফল_NEEDS_MORE_প্রবেশ:
      বিরতি;
    ক্ষেত্র BROTLI_DECODER_ফলাফল_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ফাঁকা;
      যদি(nbytes)
        ফলাফল = CURLE_WRITE_ERROR;
      বিরতি;
    পূর্বনির্ধারিত:
      ফলাফল = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      বিরতি;
    }
  }
  ফেরত ফলাফল;
}

স্থির শূন্য brotli_do_close(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_cwriter *writer)
{
  কাঠামো brotli_writer *bp = (কাঠামো brotli_writer *)writer;
  (শূন্য)তথ্য;

  যদি(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ফাঁকা;
  }
}

স্থির ধ্রুবক কাঠামো Curl_cwtype brotli_encoding = {
  "br",
  ফাঁকা,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  আকার(কাঠামো brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
কাঠামো zstd_writer {
  কাঠামো Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  অক্ষর বাফার[DECOMPRESS_বাফার_আকার];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
স্থির শূন্য *Curl_zstd_alloc(শূন্য *opaque, size_t আকার_2)
{
  (শূন্য)opaque;
  ফেরত Curl_cmalloc(আকার_2);
}

স্থির শূন্য Curl_zstd_free(শূন্য *opaque, শূন্য *address)
{
  (শূন্য)opaque;
  Curl_cfree(address);
}
#endif

স্থির CURLcode zstd_do_init(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer)
{
  কাঠামো zstd_writer *zp = (কাঠামো zstd_writer *)writer;

  (শূন্য)তথ্য;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ফাঁকা
  });
#অন্যথা
  zp->zds = ZSTD_createDStream();
#endif

  ফেরত zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির CURLcode zstd_do_write(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                              ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো zstd_writer *zp = (কাঠামো zstd_writer *)writer;
  ZSTD_in_বাফার in;
  ZSTD_out_বাফার out;
  size_t errorCode;
  পূর্ণসংখ্যা i = 0;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.আকার_2 = nbytes;

  জন্য(;;) {
    যদি(++i > (1024 * 1024 / DECOMPRESS_বাফার_আকার)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      যদি(Curl_timeleft_ms(তথ্য) < 0) {
        failf(তথ্য, "Operation timed out while decoding payload");
        ফেরত CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->বাফার;
    out.আকার_2 = DECOMPRESS_বাফার_আকার;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    যদি(ZSTD_isError(errorCode)) {
      ফেরত CURLE_BAD_CONTENT_ENCODING;
    }
    যদি(out.pos > 0) {
      ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, type,
                                  zp->বাফার, out.pos);
      যদি(ফলাফল)
        বিরতি;
    }
    যদি((in.pos == nbytes) && (out.pos < out.আকার_2))
      বিরতি;
  }

  ফেরত ফলাফল;
}

স্থির শূন্য zstd_do_close(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_cwriter *writer)
{
  কাঠামো zstd_writer *zp = (কাঠামো zstd_writer *)writer;
  (শূন্য)তথ্য;

  যদি(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ফাঁকা;
  }
}

স্থির ধ্রুবক কাঠামো Curl_cwtype zstd_encoding = {
  "zstd",
  ফাঁকা,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  আকার(কাঠামো zstd_writer)
};
#endif

/* Identity handler. */
স্থির ধ্রুবক কাঠামো Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  আকার(কাঠামো Curl_cwriter)
};

/* supported general content decoders. */
স্থির ধ্রুবক কাঠামো Curl_cwtype * ধ্রুবক general_unencoders[] = {
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
  ফাঁকা
};

/* supported content decoders only for transfer encodings */
স্থির ধ্রুবক কাঠামো Curl_cwtype * ধ্রুবক transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ফাঁকা
};

/* Return the list of comma-separated names of supported encodings.
 */
অক্ষর *Curl_get_content_encodings(শূন্য)
{
  কাঠামো dynbuf enc;
  ধ্রুবক কাঠামো Curl_cwtype * ধ্রুবক *cep;
  CURLcode ফলাফল = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  জন্য(cep = general_unencoders; *cep && !ফলাফল; cep++) {
    ধ্রুবক কাঠামো Curl_cwtype *ce = *cep;
    যদি(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      যদি(curlx_dyn_len(&enc))
        ফলাফল = curlx_dyn_addn(&enc, ", ", 2);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_add(&enc, ce->name);
    }
  }
  যদি(!ফলাফল && !curlx_dyn_len(&enc))
    ফলাফল = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  যদি(!ফলাফল)
    ফেরত curlx_dyn_ptr(&enc);
  ফেরত ফাঁকা;
}

/* Deferred error dummy writer. */
স্থির CURLcode error_do_init(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_cwriter *writer)
{
  (শূন্য)তথ্য;
  (শূন্য)writer;
  ফেরত CURLE_OK;
}

স্থির CURLcode error_do_write(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                               ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  (শূন্য)writer;
  (শূন্য)buf;
  (শূন্য)nbytes;

  যদি(!(type & CLIENTWRITE_BODY) || !nbytes)
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);
  failf(তথ্য, "Unrecognized content encoding type");
  ফেরত CURLE_BAD_CONTENT_ENCODING;
}

স্থির শূন্য error_do_close(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_cwriter *writer)
{
  (শূন্য)তথ্য;
  (শূন্য)writer;
}

স্থির ধ্রুবক কাঠামো Curl_cwtype error_writer = {
  "ce-error",
  ফাঁকা,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  আকার(কাঠামো Curl_cwriter)
};

/* Find the content encoding by name. */
স্থির ধ্রুবক কাঠামো Curl_cwtype *find_unencode_writer(ধ্রুবক অক্ষর *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ধ্রুবক কাঠামো Curl_cwtype * ধ্রুবক *cep;

  যদি(phase == CURL_CW_TRANSFER_DECODE) {
    জন্য(cep = transfer_unencoders; *cep; cep++) {
      ধ্রুবক কাঠামো Curl_cwtype *ce = *cep;
      যদি((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ফেরত ce;
    }
  }
  /* look among the general decoders */
  জন্য(cep = general_unencoders; *cep; cep++) {
    ধ্রুবক কাঠামো Curl_cwtype *ce = *cep;
    যদি((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ফেরত ce;
  }
  ফেরত ফাঁকা;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_স্তূপ(কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক অক্ষর *enclist, পূর্ণসংখ্যা is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode ফলাফল;
  bool has_chunked = FALSE;

  করো {
    ধ্রুবক অক্ষর *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    যখন(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    জন্য(namelen = 0; *enclist && *enclist != ','; enclist++)
      যদি(*enclist > ' ')
        namelen = enclist - name + 1;

    যদি(namelen) {
      ধ্রুবক কাঠামো Curl_cwtype *cwt;
      কাঠামো Curl_cwriter *writer;

      CURL_TRC_WRITE(তথ্য, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (পূর্ণসংখ্যা)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      যদি((is_transfer && !তথ্য->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && তথ্য->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(তথ্য, "decoder not requested, ignored: %.*s",
                       (পূর্ণসংখ্যা)namelen, name);
        যদি(is_transfer && !তথ্য->set.http_te_skip) {
          যদি(has_chunked)
            failf(তথ্য, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (পূর্ণসংখ্যা)namelen, name);
          অন্যথা যদি(is_identity)
            চালু;
          অন্যথা
            failf(তথ্য, "Unsolicited Transfer-Encoding (%.*s) found",
                  (পূর্ণসংখ্যা)namelen, name);
          ফেরত CURLE_BAD_CONTENT_ENCODING;
        }
        ফেরত CURLE_OK;
      }

      যদি(Curl_cwriter_গুনতি(তথ্য, phase) >= MAX_ENCODE_স্তূপ) {
        failf(তথ্য, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_স্তূপ,
              is_transfer ? "transfer" : "content");
        ফেরত CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      যদি(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(তথ্য, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(তথ্য, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        ফেরত CURLE_BAD_CONTENT_ENCODING;
      }
      যদি(cwt && is_chunked && Curl_cwriter_get_by_type(তথ্য, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(তথ্য, "ignoring duplicate 'chunked' decoder");
      }
      অন্যথা {
        যদি(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        ফলাফল = Curl_cwriter_create(&writer, তথ্য, cwt, phase);
        CURL_TRC_WRITE(তথ্য, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (পূর্ণসংখ্যা)ফলাফল);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        ফলাফল = Curl_cwriter_add(তথ্য, writer);
        যদি(ফলাফল) {
          Curl_cwriter_free(তথ্য, writer);
          ফেরত ফলাফল;
        }
      }
      যদি(is_chunked)
        has_chunked = TRUE;
    }
  } যখন(*enclist);

  ফেরত CURLE_OK;
}

#অন্যথা
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_স্তূপ(কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক অক্ষর *enclist, পূর্ণসংখ্যা is_transfer)
{
  (শূন্য)তথ্য;
  (শূন্য)enclist;
  (শূন্য)is_transfer;
  ফেরত CURLE_NOT_BUILT_IN;
}

অক্ষর *Curl_get_content_encodings(শূন্য)
{
  ফেরত curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
