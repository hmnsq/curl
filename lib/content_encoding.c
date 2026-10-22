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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curlx/dynbuf.h"

#ifdef HAVE_LIBZ
#અન્તર્ભાવ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wvla"
#endif
#અન્તર્ભાવ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif
#endif

#ifdef HAVE_ZSTD
#અન્તર્ભાવ <zstd.h>
#endif

#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "content_encoding.h"

#define CONTENT_ENCODING_DEFAULT  "identity"

#ifndef CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#define MAX_ENCODE_ઢગલો 5

#જો defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#define DECOMPRESS_BUFFER_SIZE 16384 /* buffer size for decompressed data */
#endif

#ifdef HAVE_LIBZ

#જો !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#error "zlib 1.2.5.2 or greater required"
#endif

પ્રકારવ્યાખ્યા ગણના {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
સંરચના zlib_writer {
  સંરચના Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  અક્ષર buffer[DECOMPRESS_BUFFER_SIZE]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

સ્થિર voidpf zalloc_cb(voidpf opaque, unsigned પૂર્ણાંક વસ્તુઓ, unsigned પૂર્ણાંક size)
{
  (શૂન્ય)opaque;
  /* not a typo, keep it curlx_calloc() */
  પાછા curlx_calloc(વસ્તુઓ, size);
}

સ્થિર શૂન્ય zfree_cb(voidpf opaque, voidpf ptr)
{
  (શૂન્ય)opaque;
  curlx_free(ptr);
}

સ્થિર CURLcode process_zlib_error(સંરચના Curl_easy *data, z_stream *z)
{
  જો(z->સંદેશ)
    failf(data, "Error while processing content unencoding: %s", z->સંદેશ);
  નહીંતર
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  પાછા CURLE_BAD_CONTENT_ENCODING;
}

સ્થિર CURLcode exit_zlib(સંરચના Curl_easy *data, z_stream *z,
                          zlibInitState *zlib_init, CURLcode result)
{
  જો(*zlib_init != ZLIB_UNINIT) {
    જો(inflateEnd(z) != Z_OK && result == CURLE_OK)
      result = process_zlib_error(data, z);
    *zlib_init = ZLIB_UNINIT;
  }

  પાછા result;
}

સ્થિર CURLcode process_trailer(સંરચના Curl_easy *data, સંરચના zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  જો(z->avail_in)
    result = CURLE_WRITE_ERROR;
  જો(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  નહીંતર {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  પાછા result;
}

સ્થિર CURLcode inflate_stream(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                               zlibInitState started)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->next_in;
  bool done = FALSE;
  CURLcode result = CURLE_OK;   /* Curl_client_write status */
  પૂર્ણાંક i = 0;

  /* Check state. */
  જો(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    પાછા exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  જ્યારે(!done) {
    પૂર્ણાંક status; /* zlib status */
    done = TRUE;

    જો(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      જો(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        પાછા exit_zlib(data, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->next_out = (Bytef *)zp->buffer;
    z->avail_out = DECOMPRESS_BUFFER_SIZE;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    જો(z->avail_out != DECOMPRESS_BUFFER_SIZE) {
      જો(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        result = Curl_cwriter_write(data, writer->next, type, zp->buffer,
                                    DECOMPRESS_BUFFER_SIZE - z->avail_out);
        જો(result) {
          exit_zlib(data, z, &zp->zlib_init, result);
          તોડો;
        }
      }
    }

    /* Dispatch by inflate() status. */
    પસંદગી(status) {
    સ્થિતિ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      તોડો;
    સ્થિતિ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      તોડો;
    સ્થિતિ Z_STREAM_END:
      જો((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->next_in[0] == 0x1f) && (z->next_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(data, "Multi-member gzip response not supported");
        result = exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        તોડો;
      }
      result = process_trailer(data, zp);
      તોડો;
    સ્થિતિ Z_DATA_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      જો(zp->zlib_init == ZLIB_INIT) {
        જો(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->next_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          તોડો;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      તોડો;
    મૂળભૂત:
      result = exit_zlib(data, z, &zp->zlib_init, process_zlib_error(data, z));
      તોડો;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  જો(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  પાછા result;
}

/* Deflate handler. */
સ્થિર CURLcode deflate_do_init(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  જો(inflateInit(z) != Z_OK)
    પાછા process_zlib_error(data, z);
  zp->zlib_init = ZLIB_INIT;
  પાછા CURLE_OK;
}

સ્થિર CURLcode deflate_do_write(સંરચના Curl_easy *data,
                                 સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                 અચલ અક્ષર *buf, size_t nbytes)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  જો(!(type & CLIENTWRITE_BODY) || !nbytes)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->next_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  જો(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    પાછા process_trailer(data, zp);

  /* Now uncompress the data */
  પાછા inflate_stream(data, writer, type, ZLIB_INFLATING);
}

સ્થિર શૂન્ય deflate_do_close(સંરચના Curl_easy *data,
                             સંરચના Curl_cwriter *writer)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

સ્થિર અચલ સંરચના Curl_cwtype deflate_encoding = {
  "deflate",
  ખાલી,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  કદ(સંરચના zlib_writer)
};

/*
 * Gzip handler.
 */

સ્થિર CURLcode gzip_do_init(સંરચના Curl_easy *data,
                             સંરચના Curl_cwriter *writer)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  જો(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    પાછા process_zlib_error(data, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  પાછા CURLE_OK;
}

સ્થિર CURLcode gzip_do_write(સંરચના Curl_easy *data,
                              સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                              અચલ અક્ષર *buf, size_t nbytes)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  જો(!(type & CLIENTWRITE_BODY) || !nbytes)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  જો(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->next_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    પાછા inflate_stream(data, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  પાછા exit_zlib(data, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

સ્થિર શૂન્ય gzip_do_close(સંરચના Curl_easy *data,
                          સંરચના Curl_cwriter *writer)
{
  સંરચના zlib_writer *zp = (સંરચના zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(data, z, &zp->zlib_init, CURLE_OK);
}

સ્થિર અચલ સંરચના Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  કદ(સંરચના zlib_writer)
};

#endif /* HAVE_LIBZ */

#ifdef HAVE_BROTLI
/* Brotli writer. */
સંરચના brotli_writer {
  સંરચના Curl_cwriter super;
  અક્ષર buffer[DECOMPRESS_BUFFER_SIZE];
  BrotliDecoderState *br; /* State structure for brotli. */
};

સ્થિર CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  પસંદગી(be) {
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  સ્થિતિ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ifdef BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  સ્થિતિ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#endif
  સ્થિતિ BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  સ્થિતિ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    પાછા CURLE_BAD_CONTENT_ENCODING;
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_વૃક્ષ_GROUPS:
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
  સ્થિતિ BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    પાછા CURLE_OUT_OF_MEMORY;
  મૂળભૂત:
    તોડો;
  }
  પાછા CURLE_WRITE_ERROR;
}

સ્થિર CURLcode brotli_do_init(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *writer)
{
  સંરચના brotli_writer *bp = (સંરચના brotli_writer *)writer;
  (શૂન્ય)data;

  bp->br = BrotliDecoderCreateInstance(ખાલી, ખાલી, ખાલી);
  પાછા bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

સ્થિર CURLcode brotli_do_write(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                અચલ અક્ષર *buf, size_t nbytes)
{
  સંરચના brotli_writer *bp = (સંરચના brotli_writer *)writer;
  અચલ uint8_t *src = (અચલ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
  પૂર્ણાંક i = 0;

  જો(!(type & CLIENTWRITE_BODY) || !nbytes)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  જો(!bp->br)
    પાછા CURLE_WRITE_ERROR; /* Stream already ended. */

  જ્યારે((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {

    જો(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      જો(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        પાછા CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->buffer;
    dstleft = DECOMPRESS_BUFFER_SIZE;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ખાલી);
    result = Curl_cwriter_write(data, writer->next, type,
                                bp->buffer, DECOMPRESS_BUFFER_SIZE - dstleft);
    જો(result)
      તોડો;
    પસંદગી(r) {
    સ્થિતિ BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    સ્થિતિ BROTLI_DECODER_RESULT_NEEDS_MORE_આગત:
      તોડો;
    સ્થિતિ BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ખાલી;
      જો(nbytes)
        result = CURLE_WRITE_ERROR;
      તોડો;
    મૂળભૂત:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      તોડો;
    }
  }
  પાછા result;
}

સ્થિર શૂન્ય brotli_do_close(સંરચના Curl_easy *data,
                            સંરચના Curl_cwriter *writer)
{
  સંરચના brotli_writer *bp = (સંરચના brotli_writer *)writer;
  (શૂન્ય)data;

  જો(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ખાલી;
  }
}

સ્થિર અચલ સંરચના Curl_cwtype brotli_encoding = {
  "br",
  ખાલી,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  કદ(સંરચના brotli_writer)
};
#endif

#ifdef HAVE_ZSTD
/* Zstd writer. */
સંરચના zstd_writer {
  સંરચના Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  અક્ષર buffer[DECOMPRESS_BUFFER_SIZE];
};

#ifdef ZSTD_STATIC_LINKING_ONLY
સ્થિર શૂન્ય *Curl_zstd_alloc(શૂન્ય *opaque, size_t size)
{
  (શૂન્ય)opaque;
  પાછા Curl_cmalloc(size);
}

સ્થિર શૂન્ય Curl_zstd_free(શૂન્ય *opaque, શૂન્ય *address)
{
  (શૂન્ય)opaque;
  Curl_cfree(address);
}
#endif

સ્થિર CURLcode zstd_do_init(સંરચના Curl_easy *data,
                             સંરચના Curl_cwriter *writer)
{
  સંરચના zstd_writer *zp = (સંરચના zstd_writer *)writer;

  (શૂન્ય)data;

#ifdef ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ખાલી
  });
#નહીંતર
  zp->zds = ZSTD_createDStream();
#endif

  પાછા zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

સ્થિર CURLcode zstd_do_write(સંરચના Curl_easy *data,
                              સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                              અચલ અક્ષર *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  સંરચના zstd_writer *zp = (સંરચના zstd_writer *)writer;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;
  પૂર્ણાંક i = 0;

  જો(!(type & CLIENTWRITE_BODY) || !nbytes)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  માટે(;;) {
    જો(++i > (1024 * 1024 / DECOMPRESS_BUFFER_SIZE)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      જો(Curl_timeleft_ms(data) < 0) {
        failf(data, "Operation timed out while decoding payload");
        પાછા CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->buffer;
    out.size = DECOMPRESS_BUFFER_SIZE;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    જો(ZSTD_isError(errorCode)) {
      પાછા CURLE_BAD_CONTENT_ENCODING;
    }
    જો(out.pos > 0) {
      result = Curl_cwriter_write(data, writer->next, type,
                                  zp->buffer, out.pos);
      જો(result)
        તોડો;
    }
    જો((in.pos == nbytes) && (out.pos < out.size))
      તોડો;
  }

  પાછા result;
}

સ્થિર શૂન્ય zstd_do_close(સંરચના Curl_easy *data,
                          સંરચના Curl_cwriter *writer)
{
  સંરચના zstd_writer *zp = (સંરચના zstd_writer *)writer;
  (શૂન્ય)data;

  જો(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ખાલી;
  }
}

સ્થિર અચલ સંરચના Curl_cwtype zstd_encoding = {
  "zstd",
  ખાલી,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  કદ(સંરચના zstd_writer)
};
#endif

/* Identity handler. */
સ્થિર અચલ સંરચના Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  કદ(સંરચના Curl_cwriter)
};

/* supported general content decoders. */
સ્થિર અચલ સંરચના Curl_cwtype * અચલ general_unencoders[] = {
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
  ખાલી
};

/* supported content decoders only for transfer encodings */
સ્થિર અચલ સંરચના Curl_cwtype * અચલ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ખાલી
};

/* Return the list of comma-separated names of supported encodings.
 */
અક્ષર *Curl_get_content_encodings(શૂન્ય)
{
  સંરચના dynbuf enc;
  અચલ સંરચના Curl_cwtype * અચલ *cep;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  માટે(cep = general_unencoders; *cep && !result; cep++) {
    અચલ સંરચના Curl_cwtype *ce = *cep;
    જો(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      જો(curlx_dyn_len(&enc))
        result = curlx_dyn_addn(&enc, ", ", 2);
      જો(!result)
        result = curlx_dyn_add(&enc, ce->name);
    }
  }
  જો(!result && !curlx_dyn_len(&enc))
    result = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  જો(!result)
    પાછા curlx_dyn_ptr(&enc);
  પાછા ખાલી;
}

/* Deferred error dummy writer. */
સ્થિર CURLcode error_do_init(સંરચના Curl_easy *data,
                              સંરચના Curl_cwriter *writer)
{
  (શૂન્ય)data;
  (શૂન્ય)writer;
  પાછા CURLE_OK;
}

સ્થિર CURLcode error_do_write(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                               અચલ અક્ષર *buf, size_t nbytes)
{
  (શૂન્ય)writer;
  (શૂન્ય)buf;
  (શૂન્ય)nbytes;

  જો(!(type & CLIENTWRITE_BODY) || !nbytes)
    પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);
  failf(data, "Unrecognized content encoding type");
  પાછા CURLE_BAD_CONTENT_ENCODING;
}

સ્થિર શૂન્ય error_do_close(સંરચના Curl_easy *data,
                           સંરચના Curl_cwriter *writer)
{
  (શૂન્ય)data;
  (શૂન્ય)writer;
}

સ્થિર અચલ સંરચના Curl_cwtype error_writer = {
  "ce-error",
  ખાલી,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  કદ(સંરચના Curl_cwriter)
};

/* Find the content encoding by name. */
સ્થિર અચલ સંરચના Curl_cwtype *find_unencode_writer(અચલ અક્ષર *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  અચલ સંરચના Curl_cwtype * અચલ *cep;

  જો(phase == CURL_CW_TRANSFER_DECODE) {
    માટે(cep = transfer_unencoders; *cep; cep++) {
      અચલ સંરચના Curl_cwtype *ce = *cep;
      જો((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        પાછા ce;
    }
  }
  /* look among the general decoders */
  માટે(cep = general_unencoders; *cep; cep++) {
    અચલ સંરચના Curl_cwtype *ce = *cep;
    જો((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      પાછા ce;
  }
  પાછા ખાલી;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ઢગલો(સંરચના Curl_easy *data,
                                     અચલ અક્ષર *enclist, પૂર્ણાંક is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode result;
  bool has_chunked = FALSE;

  કરો {
    અચલ અક્ષર *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    જ્યારે(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    માટે(namelen = 0; *enclist && *enclist != ','; enclist++)
      જો(*enclist > ' ')
        namelen = enclist - name + 1;

    જો(namelen) {
      અચલ સંરચના Curl_cwtype *cwt;
      સંરચના Curl_cwriter *writer;

      CURL_TRC_WRITE(data, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (પૂર્ણાંક)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      જો((is_transfer && !data->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && data->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(data, "decoder not requested, ignored: %.*s",
                       (પૂર્ણાંક)namelen, name);
        જો(is_transfer && !data->set.http_te_skip) {
          જો(has_chunked)
            failf(data, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (પૂર્ણાંક)namelen, name);
          નહીંતર જો(is_identity)
            ચાલુ;
          નહીંતર
            failf(data, "Unsolicited Transfer-Encoding (%.*s) found",
                  (પૂર્ણાંક)namelen, name);
          પાછા CURLE_BAD_CONTENT_ENCODING;
        }
        પાછા CURLE_OK;
      }

      જો(Curl_cwriter_ગણતરી(data, phase) >= MAX_ENCODE_ઢગલો) {
        failf(data, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ઢગલો,
              is_transfer ? "transfer" : "content");
        પાછા CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      જો(is_transfer && !is_chunked &&
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
        પાછા CURLE_BAD_CONTENT_ENCODING;
      }
      જો(cwt && is_chunked && Curl_cwriter_get_by_type(data, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(data, "ignoring duplicate 'chunked' decoder");
      }
      નહીંતર {
        જો(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        result = Curl_cwriter_create(&writer, data, cwt, phase);
        CURL_TRC_WRITE(data, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (પૂર્ણાંક)result);
        જો(result)
          પાછા result;

        result = Curl_cwriter_add(data, writer);
        જો(result) {
          Curl_cwriter_free(data, writer);
          પાછા result;
        }
      }
      જો(is_chunked)
        has_chunked = TRUE;
    }
  } જ્યારે(*enclist);

  પાછા CURLE_OK;
}

#નહીંતર
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ઢગલો(સંરચના Curl_easy *data,
                                     અચલ અક્ષર *enclist, પૂર્ણાંક is_transfer)
{
  (શૂન્ય)data;
  (શૂન્ય)enclist;
  (શૂન્ય)is_transfer;
  પાછા CURLE_NOT_BUILT_IN;
}

અક્ષર *Curl_get_content_encodings(શૂન્ય)
{
  પાછા curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#endif /* CURL_DISABLE_HTTP */
