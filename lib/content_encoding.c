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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
#ਅਨ੍ਤਰ੍ਭਾਵ <zlib.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wvla"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <brotli/decode.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
#ਅਨ੍ਤਰ੍ਭਾਵ <zstd.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "content_encoding.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTENT_ENCODING_DEFAULT  "identity"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ENCODE_ਢੇਰ 5

#ਜੇ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECOMPRESS_ਬਫਰ_ਮਾਪ 16384 /* buffer size for decompressed data */
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ

#ਜੇ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#ਗਲਤੀ "zlib 1.2.5.2 or greater required"
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ਬਣਤਰ zlib_writer {
  ਬਣਤਰ Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  ਅੱਖਰ ਬਫਰ[DECOMPRESS_ਬਫਰ_ਮਾਪ]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_ਧਾਰਾ z;                /* State structure for zlib. */
};

ਸਥਿਰ voidpf zalloc_cb(voidpf opaque, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੀਜ਼ਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ)
{
  (ਖਾਲੀ)opaque;
  /* not a typo, keep it curlx_calloc() */
  ਵਾਪਸ curlx_calloc(ਚੀਜ਼ਾਂ, ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ zfree_cb(voidpf opaque, voidpf ptr)
{
  (ਖਾਲੀ)opaque;
  curlx_free(ptr);
}

ਸਥਿਰ CURLcode process_zlib_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, z_ਧਾਰਾ *z)
{
  ਜੇ(z->ਸੁਨੇਹਾ)
    failf(ਅੰਕੜੇ, "Error while processing content unencoding: %s", z->ਸੁਨੇਹਾ);
  ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
}

ਸਥਿਰ CURLcode exit_zlib(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, z_ਧਾਰਾ *z,
                          zlibInitState *zlib_init, CURLcode ਨਤੀਜਾ)
{
  ਜੇ(*zlib_init != ZLIB_UNINIT) {
    ਜੇ(inflate_ਸਿਰਾ(z) != Z_OK && ਨਤੀਜਾ == CURLE_OK)
      ਨਤੀਜਾ = process_zlib_error(ਅੰਕੜੇ, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode process_trailer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ zlib_writer *zp)
{
  z_ਧਾਰਾ *z = &zp->z;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uInt len = z->avail_ਵਿੱਚ < zp->trailerlen ? z->avail_ਵਿੱਚ : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_ਵਿੱਚ -= len;
  z->ਅਗਲਾ_ਵਿੱਚ += len;
  ਜੇ(z->avail_ਵਿੱਚ)
    ਨਤੀਜਾ = CURLE_WRITE_ERROR;
  ਜੇ(ਨਤੀਜਾ || !zp->trailerlen)
    ਨਤੀਜਾ = exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, ਨਤੀਜਾ);
  ਨਹੀਂਤਾਂ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode inflate_ਧਾਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                               zlibInitState started)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_ਵਿੱਚ;
  z_const Bytef *orig_ਵਿੱਚ = z->ਅਗਲਾ_ਵਿੱਚ;
  bool ਪੂਰਾ = FALSE;
  CURLcode ਨਤੀਜਾ = CURLE_OK;   /* Curl_client_write status */
  ਪੂਰਨ i = 0;

  /* Check state. */
  ਜੇ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ਵਾਪਸ exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ਜਦੋਂ(!ਪੂਰਾ) {
    ਪੂਰਨ ਸਥਿਤੀ; /* zlib status */
    ਪੂਰਾ = TRUE;

    ਜੇ(++i > (1024 * 1024 / DECOMPRESS_ਬਫਰ_ਮਾਪ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
        failf(ਅੰਕੜੇ, "Operation timed out while decoding payload");
        ਵਾਪਸ exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->ਅਗਲਾ_ਬਾਹਰਲਾ = (Bytef *)zp->ਬਫਰ;
    z->avail_ਬਾਹਰਲਾ = DECOMPRESS_ਬਫਰ_ਮਾਪ;

    ਸਥਿਤੀ = inflate(z, Z_ਖੰਡ);

    /* Flush output data if some. */
    ਜੇ(z->avail_ਬਾਹਰਲਾ != DECOMPRESS_ਬਫਰ_ਮਾਪ) {
      ਜੇ(ਸਥਿਤੀ == Z_OK || ਸਥਿਤੀ == Z_ਧਾਰਾ_ਸਿਰਾ) {
        zp->zlib_init = started; /* Data started. */
        ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, zp->ਬਫਰ,
                                    DECOMPRESS_ਬਫਰ_ਮਾਪ - z->avail_ਬਾਹਰਲਾ);
        ਜੇ(ਨਤੀਜਾ) {
          exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, ਨਤੀਜਾ);
          ਰੋਕੋ;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ਚੋਣ(ਸਥਿਤੀ) {
    ਹਾਲਤ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      ਪੂਰਾ = FALSE;
      ਰੋਕੋ;
    ਹਾਲਤ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ਰੋਕੋ;
    ਹਾਲਤ Z_ਧਾਰਾ_ਸਿਰਾ:
      ਜੇ((started == ZLIB_INIT_GZIP) && (z->avail_ਵਿੱਚ >= 2) &&
         (z->ਅਗਲਾ_ਵਿੱਚ[0] == 0x1f) && (z->ਅਗਲਾ_ਵਿੱਚ[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(ਅੰਕੜੇ, "Multi-member gzip response not supported");
        ਨਤੀਜਾ = exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ਰੋਕੋ;
      }
      ਨਤੀਜਾ = process_trailer(ਅੰਕੜੇ, zp);
      ਰੋਕੋ;
    ਹਾਲਤ Z_ਅੰਕੜੇ_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ਜੇ(zp->zlib_init == ZLIB_INIT) {
        ਜੇ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->ਅਗਲਾ_ਵਿੱਚ = orig_ਵਿੱਚ;
          z->avail_ਵਿੱਚ = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          ਪੂਰਾ = FALSE;
          ਰੋਕੋ;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      ਨਤੀਜਾ = exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, process_zlib_error(ਅੰਕੜੇ, z));
      ਰੋਕੋ;
    ਮੂਲ:
      ਨਤੀਜਾ = exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, process_zlib_error(ਅੰਕੜੇ, z));
      ਰੋਕੋ;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ਜੇ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Deflate handler. */
ਸਥਿਰ CURLcode deflate_do_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ਜੇ(inflateInit(z) != Z_OK)
    ਵਾਪਸ process_zlib_error(ਅੰਕੜੇ, z);
  zp->zlib_init = ZLIB_INIT;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode deflate_do_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                 ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  ਜੇ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->ਅਗਲਾ_ਵਿੱਚ = (z_const Bytef *)buf;
  z->avail_ਵਿੱਚ = (uInt)nbytes;

  ਜੇ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ਵਾਪਸ process_trailer(ਅੰਕੜੇ, zp);

  /* Now uncompress the data */
  ਵਾਪਸ inflate_ਧਾਰਾ(ਅੰਕੜੇ, writer, type, ZLIB_INFLATING);
}

ਸਥਿਰ ਖਾਲੀ deflate_do_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_OK);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype deflate_encoding = {
  "deflate",
  ਨਿਰਾ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  ਆਕਾਰ(ਬਣਤਰ zlib_writer)
};

/*
 * Gzip handler.
 */

ਸਥਿਰ CURLcode gzip_do_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ਜੇ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ਵਾਪਸ process_zlib_error(ਅੰਕੜੇ, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode gzip_do_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                              ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  ਜੇ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);

  ਜੇ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->ਅਗਲਾ_ਵਿੱਚ = (z_const Bytef *)buf;
    z->avail_ਵਿੱਚ = (uInt)nbytes;
    /* Now uncompress the data */
    ਵਾਪਸ inflate_ਧਾਰਾ(ਅੰਕੜੇ, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ਵਾਪਸ exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ਸਥਿਰ ਖਾਲੀ gzip_do_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zlib_writer *zp = (ਬਣਤਰ zlib_writer *)writer;
  z_ਧਾਰਾ *z = &zp->z; /* zlib state structure */

  exit_zlib(ਅੰਕੜੇ, z, &zp->zlib_init, CURLE_OK);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  ਆਕਾਰ(ਬਣਤਰ zlib_writer)
};

#ਜੇ_ਅੰਤ /* HAVE_LIBZ */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
/* Brotli writer. */
ਬਣਤਰ brotli_writer {
  ਬਣਤਰ Curl_cwriter super;
  ਅੱਖਰ ਬਫਰ[DECOMPRESS_ਬਫਰ_ਮਾਪ];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ਸਥਿਰ CURLcode brotli_map_error(Brotli_Decoder_Error_ਕੋਡ be)
{
  ਚੋਣ(be) {
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_ਖੰਡ_ਲੰਮਾਈ_1:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_ਖੰਡ_ਲੰਮਾਈ_2:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_ਖਿੜਕੀ_ਬਿੱਟ:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  ਹਾਲਤ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  ਹਾਲਤ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#ਜੇ_ਅੰਤ
  ਹਾਲਤ BROTLI_DECODER_ERROR_DICTIONARY_NOT_ਸੈੱਟ:
  ਹਾਲਤ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_ਰੁੱਖ_GROUPS:
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_RING_ਬਫਰ_1:
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_RING_ਬਫਰ_2:
  ਹਾਲਤ BROTLI_DECODER_ERROR_ALLOC_ਖੰਡ_TYPE_TREES:
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_WRITE_ERROR;
}

ਸਥਿਰ CURLcode brotli_do_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ brotli_writer *bp = (ਬਣਤਰ brotli_writer *)writer;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  bp->br = BrotliDecoderCreateInstance(ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
  ਵਾਪਸ bp->br ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

ਸਥਿਰ CURLcode brotli_do_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਬਣਤਰ brotli_writer *bp = (ਬਣਤਰ brotli_writer *)writer;
  ਅਚਲ uint8_t *src = (ਅਚਲ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  Brotli_Decoder_ਨਤੀਜਾ r = BROTLI_DECODER_ਨਤੀਜਾ_NEEDS_ਹੋਰ_OUTPUT;
  ਪੂਰਨ i = 0;

  ਜੇ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);

  ਜੇ(!bp->br)
    ਵਾਪਸ CURLE_WRITE_ERROR; /* Stream already ended. */

  ਜਦੋਂ((nbytes || r == BROTLI_DECODER_ਨਤੀਜਾ_NEEDS_ਹੋਰ_OUTPUT) &&
        ਨਤੀਜਾ == CURLE_OK) {

    ਜੇ(++i > (1024 * 1024 / DECOMPRESS_ਬਫਰ_ਮਾਪ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
        failf(ਅੰਕੜੇ, "Operation timed out while decoding payload");
        ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->ਬਫਰ;
    dstleft = DECOMPRESS_ਬਫਰ_ਮਾਪ;
    r = Brotli_Decoder_Decompress_ਧਾਰਾ(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ਨਿਰਾ);
    ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type,
                                bp->ਬਫਰ, DECOMPRESS_ਬਫਰ_ਮਾਪ - dstleft);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    ਚੋਣ(r) {
    ਹਾਲਤ BROTLI_DECODER_ਨਤੀਜਾ_NEEDS_ਹੋਰ_OUTPUT:
    ਹਾਲਤ BROTLI_DECODER_ਨਤੀਜਾ_NEEDS_ਹੋਰ_ਨਿਵੇਸ਼:
      ਰੋਕੋ;
    ਹਾਲਤ BROTLI_DECODER_ਨਤੀਜਾ_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ਨਿਰਾ;
      ਜੇ(nbytes)
        ਨਤੀਜਾ = CURLE_WRITE_ERROR;
      ਰੋਕੋ;
    ਮੂਲ:
      ਨਤੀਜਾ = brotli_map_error(Brotli_Decoder_Get_Error_ਕੋਡ(bp->br));
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ brotli_do_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ brotli_writer *bp = (ਬਣਤਰ brotli_writer *)writer;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਜੇ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ਨਿਰਾ;
  }
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype brotli_encoding = {
  "br",
  ਨਿਰਾ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  ਆਕਾਰ(ਬਣਤਰ brotli_writer)
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
/* Zstd writer. */
ਬਣਤਰ zstd_writer {
  ਬਣਤਰ Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  ਅੱਖਰ ਬਫਰ[DECOMPRESS_ਬਫਰ_ਮਾਪ];
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ZSTD_STATIC_LINKING_ONLY
ਸਥਿਰ ਖਾਲੀ *Curl_zstd_alloc(ਖਾਲੀ *opaque, size_t ਮਾਪ)
{
  (ਖਾਲੀ)opaque;
  ਵਾਪਸ Curl_cmalloc(ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ Curl_zstd_free(ਖਾਲੀ *opaque, ਖਾਲੀ *ਪਤਾ)
{
  (ਖਾਲੀ)opaque;
  Curl_cfree(ਪਤਾ);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode zstd_do_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zstd_writer *zp = (ਬਣਤਰ zstd_writer *)writer;

  (ਖਾਲੀ)ਅੰਕੜੇ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ਨਿਰਾ
  });
#ਨਹੀਂਤਾਂ
  zp->zds = ZSTD_createDStream();
#ਜੇ_ਅੰਤ

  ਵਾਪਸ zp->zds ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

ਸਥਿਰ CURLcode zstd_do_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                              ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ zstd_writer *zp = (ਬਣਤਰ zstd_writer *)writer;
  ZSTD_ਵਿੱਚ_ਬਫਰ ਵਿੱਚ;
  ZSTD_ਬਾਹਰਲਾ_ਬਫਰ ਬਾਹਰਲਾ;
  size_t error_ਕੋਡ_2;
  ਪੂਰਨ i = 0;

  ਜੇ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);

  ਵਿੱਚ.pos = 0;
  ਵਿੱਚ.src = buf;
  ਵਿੱਚ.ਮਾਪ = nbytes;

  ਲਈ(;;) {
    ਜੇ(++i > (1024 * 1024 / DECOMPRESS_ਬਫਰ_ਮਾਪ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
        failf(ਅੰਕੜੇ, "Operation timed out while decoding payload");
        ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
      }
    }

    ਬਾਹਰਲਾ.pos = 0;
    ਬਾਹਰਲਾ.dst = zp->ਬਫਰ;
    ਬਾਹਰਲਾ.ਮਾਪ = DECOMPRESS_ਬਫਰ_ਮਾਪ;

    error_ਕੋਡ_2 = ZSTD_decompress_ਧਾਰਾ(zp->zds, &ਬਾਹਰਲਾ, &ਵਿੱਚ);
    ਜੇ(ZSTD_isError(error_ਕੋਡ_2)) {
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
    }
    ਜੇ(ਬਾਹਰਲਾ.pos > 0) {
      ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type,
                                  zp->ਬਫਰ, ਬਾਹਰਲਾ.pos);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }
    ਜੇ((ਵਿੱਚ.pos == nbytes) && (ਬਾਹਰਲਾ.pos < ਬਾਹਰਲਾ.ਮਾਪ))
      ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ zstd_do_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ zstd_writer *zp = (ਬਣਤਰ zstd_writer *)writer;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਜੇ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ਨਿਰਾ;
  }
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype zstd_encoding = {
  "zstd",
  ਨਿਰਾ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  ਆਕਾਰ(ਬਣਤਰ zstd_writer)
};
#ਜੇ_ਅੰਤ

/* Identity handler. */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype ਪਛਾਣ_ਤੱਤ_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ਆਕਾਰ(ਬਣਤਰ Curl_cwriter)
};

/* supported general content decoders. */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype * ਅਚਲ general_unencoders[] = {
  &ਪਛਾਣ_ਤੱਤ_encoding,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  &brotli_encoding,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  &zstd_encoding,
#ਜੇ_ਅੰਤ
  ਨਿਰਾ
};

/* supported content decoders only for transfer encodings */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype * ਅਚਲ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ਨਿਰਾ
};

/* Return the list of comma-separated names of supported encodings.
 */
ਅੱਖਰ *Curl_get_content_encodings(ਖਾਲੀ)
{
  ਬਣਤਰ dynbuf enc;
  ਅਚਲ ਬਣਤਰ Curl_cwtype * ਅਚਲ *cep;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  ਲਈ(cep = general_unencoders; *cep && !ਨਤੀਜਾ; cep++) {
    ਅਚਲ ਬਣਤਰ Curl_cwtype *ce = *cep;
    ਜੇ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ਜੇ(curlx_dyn_len(&enc))
        ਨਤੀਜਾ = curlx_dyn_addn(&enc, ", ", 2);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&enc, ce->name);
    }
  }
  ਜੇ(!ਨਤੀਜਾ && !curlx_dyn_len(&enc))
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&enc, CONTENT_ENCODING_DEFAULT);

  ਜੇ(!ਨਤੀਜਾ)
    ਵਾਪਸ curlx_dyn_ptr(&enc);
  ਵਾਪਸ ਨਿਰਾ;
}

/* Deferred error dummy writer. */
ਸਥਿਰ CURLcode error_do_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_cwriter *writer)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)writer;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode error_do_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                               ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  (ਖਾਲੀ)writer;
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)nbytes;

  ਜੇ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);
  failf(ਅੰਕੜੇ, "Unrecognized content encoding type");
  ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
}

ਸਥਿਰ ਖਾਲੀ error_do_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_cwriter *writer)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)writer;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype error_writer = {
  "ce-error",
  ਨਿਰਾ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  ਆਕਾਰ(ਬਣਤਰ Curl_cwriter)
};

/* Find the content encoding by name. */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype *ਲੱਭੋ_unencode_writer(ਅਚਲ ਅੱਖਰ *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ਅਚਲ ਬਣਤਰ Curl_cwtype * ਅਚਲ *cep;

  ਜੇ(phase == CURL_CW_TRANSFER_DECODE) {
    ਲਈ(cep = transfer_unencoders; *cep; cep++) {
      ਅਚਲ ਬਣਤਰ Curl_cwtype *ce = *cep;
      ਜੇ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ਵਾਪਸ ce;
    }
  }
  /* look among the general decoders */
  ਲਈ(cep = general_unencoders; *cep; cep++) {
    ਅਚਲ ਬਣਤਰ Curl_cwtype *ce = *cep;
    ਜੇ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ਵਾਪਸ ce;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ਢੇਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ ਅੱਖਰ *enclist, ਪੂਰਨ is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode ਨਤੀਜਾ;
  bool has_chunked = FALSE;

  ਕਰੋ {
    ਅਚਲ ਅੱਖਰ *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ਜਦੋਂ(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    ਲਈ(namelen = 0; *enclist && *enclist != ','; enclist++)
      ਜੇ(*enclist > ' ')
        namelen = enclist - name + 1;

    ਜੇ(namelen) {
      ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt;
      ਬਣਤਰ Curl_cwriter *writer;

      CURL_TRC_WRITE(ਅੰਕੜੇ, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (ਪੂਰਨ)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ਜੇ((is_transfer && !ਅੰਕੜੇ->ਸੈੱਟ.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && ਅੰਕੜੇ->ਸੈੱਟ.http_ce_skip)) {
        bool is_ਪਛਾਣ_ਤੱਤ = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(ਅੰਕੜੇ, "decoder not requested, ignored: %.*s",
                       (ਪੂਰਨ)namelen, name);
        ਜੇ(is_transfer && !ਅੰਕੜੇ->ਸੈੱਟ.http_te_skip) {
          ਜੇ(has_chunked)
            failf(ਅੰਕੜੇ, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (ਪੂਰਨ)namelen, name);
          ਨਹੀਂਤਾਂ ਜੇ(is_ਪਛਾਣ_ਤੱਤ)
            ਜਾਰੀ;
          ਨਹੀਂਤਾਂ
            failf(ਅੰਕੜੇ, "Unsolicited Transfer-Encoding (%.*s) found",
                  (ਪੂਰਨ)namelen, name);
          ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
        }
        ਵਾਪਸ CURLE_OK;
      }

      ਜੇ(Curl_cwriter_ਗਿਣਤੀ(ਅੰਕੜੇ, phase) >= MAX_ENCODE_ਢੇਰ) {
        failf(ਅੰਕੜੇ, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ਢੇਰ,
              is_transfer ? "transfer" : "content");
        ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = ਲੱਭੋ_unencode_writer(name, namelen, phase);
      ਜੇ(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(ਅੰਕੜੇ, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(ਅੰਕੜੇ, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
      }
      ਜੇ(cwt && is_chunked && Curl_cwriter_get_by_type(ਅੰਕੜੇ, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(ਅੰਕੜੇ, "ignoring duplicate 'chunked' decoder");
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        ਨਤੀਜਾ = Curl_cwriter_create(&writer, ਅੰਕੜੇ, cwt, phase);
        CURL_TRC_WRITE(ਅੰਕੜੇ, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (ਪੂਰਨ)ਨਤੀਜਾ);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        ਨਤੀਜਾ = Curl_cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);
        ਜੇ(ਨਤੀਜਾ) {
          Curl_cwriter_free(ਅੰਕੜੇ, writer);
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
      ਜੇ(is_chunked)
        has_chunked = TRUE;
    }
  } ਜਦੋਂ(*enclist);

  ਵਾਪਸ CURLE_OK;
}

#ਨਹੀਂਤਾਂ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ਢੇਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ ਅੱਖਰ *enclist, ਪੂਰਨ is_transfer)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)enclist;
  (ਖਾਲੀ)is_transfer;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

ਅੱਖਰ *Curl_get_content_encodings(ਖਾਲੀ)
{
  ਵਾਪਸ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */
