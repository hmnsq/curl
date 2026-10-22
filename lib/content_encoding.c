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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
#ಅನ್ತರ್ಭಾವ <zlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wvla"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <brotli/decode.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
#ಅನ್ತರ್ಭಾವ <zstd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "content_encoding.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CONTENT_ENCODING_DEFAULT  "identity"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ENCODE_ರಾಶಿ 5

#ಆದರೆ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#ವ್ಯಾಖ್ಯಾನಿಸು DECOMPRESS_ಬಫರ್_ಗಾತ್ರ 16384 /* buffer size for decompressed data */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ

#ಆದರೆ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#ದೋಷ "zlib 1.2.5.2 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ರಚನೆ zlib_writer {
  ರಚನೆ Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  ಅಕ್ಷರ ಬಫರ್[DECOMPRESS_ಬಫರ್_ಗಾತ್ರ]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_ಹರಿವು z;                /* State structure for zlib. */
};

ಸ್ಥಿರ voidpf zalloc_cb(voidpf opaque, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ವಸ್ತುಗಳು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
  (ಶೂನ್ಯ)opaque;
  /* not a typo, keep it curlx_calloc() */
  ಹಿಂತಿರುಗಿ curlx_calloc(ವಸ್ತುಗಳು, ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ zfree_cb(voidpf opaque, voidpf ptr)
{
  (ಶೂನ್ಯ)opaque;
  curlx_free(ptr);
}

ಸ್ಥಿರ CURLcode process_zlib_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, z_ಹರಿವು *z)
{
  ಆದರೆ(z->ಸಂದೇಶ)
    failf(ದತ್ತಾಂಶ, "Error while processing content unencoding: %s", z->ಸಂದೇಶ);
  ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
}

ಸ್ಥಿರ CURLcode exit_zlib(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, z_ಹರಿವು *z,
                          zlibInitState *zlib_init, CURLcode ಫಲಿತಾಂಶ)
{
  ಆದರೆ(*zlib_init != ZLIB_UNINIT) {
    ಆದರೆ(inflate_ಕೊನೆ(z) != Z_OK && ಫಲಿತಾಂಶ == CURLE_OK)
      ಫಲಿತಾಂಶ = process_zlib_error(ದತ್ತಾಂಶ, z);
    *zlib_init = ZLIB_UNINIT;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode process_trailer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ zlib_writer *zp)
{
  z_ಹರಿವು *z = &zp->z;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uInt len = z->avail_ಒಳ < zp->trailerlen ? z->avail_ಒಳ : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_ಒಳ -= len;
  z->ಮುಂದಿನ_ಒಳ += len;
  ಆದರೆ(z->avail_ಒಳ)
    ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
  ಆದರೆ(ಫಲಿತಾಂಶ || !zp->trailerlen)
    ಫಲಿತಾಂಶ = exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, ಫಲಿತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode inflate_ಹರಿವು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                               zlibInitState started)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_ಒಳ;
  z_const Bytef *orig_ಒಳ = z->ಮುಂದಿನ_ಒಳ;
  bool ಮುಗಿದ = FALSE;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;   /* Curl_client_write status */
  ಪೂರ್ಣಾಂಕ i = 0;

  /* Check state. */
  ಆದರೆ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    ಹಿಂತಿರುಗಿ exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  ಆಗ(!ಮುಗಿದ) {
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2; /* zlib status */
    ಮುಗಿದ = TRUE;

    ಆದರೆ(++i > (1024 * 1024 / DECOMPRESS_ಬಫರ್_ಗಾತ್ರ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
        failf(ದತ್ತಾಂಶ, "Operation timed out while decoding payload");
        ಹಿಂತಿರುಗಿ exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->ಮುಂದಿನ_ಹೊರಹೋಗು = (Bytef *)zp->ಬಫರ್;
    z->avail_ಹೊರಹೋಗು = DECOMPRESS_ಬಫರ್_ಗಾತ್ರ;

    ಸ್ಥಿತಿ_2 = inflate(z, Z_ಖಂಡ);

    /* Flush output data if some. */
    ಆದರೆ(z->avail_ಹೊರಹೋಗು != DECOMPRESS_ಬಫರ್_ಗಾತ್ರ) {
      ಆದರೆ(ಸ್ಥಿತಿ_2 == Z_OK || ಸ್ಥಿತಿ_2 == Z_ಹರಿವು_ಕೊನೆ) {
        zp->zlib_init = started; /* Data started. */
        ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, zp->ಬಫರ್,
                                    DECOMPRESS_ಬಫರ್_ಗಾತ್ರ - z->avail_ಹೊರಹೋಗು);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, ಫಲಿತಾಂಶ);
          ನಿಲ್ಲಿಸು;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ಆಯ್ಕೆ(ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿತಿ Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      ಮುಗಿದ = FALSE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Z_ಹರಿವು_ಕೊನೆ:
      ಆದರೆ((started == ZLIB_INIT_GZIP) && (z->avail_ಒಳ >= 2) &&
         (z->ಮುಂದಿನ_ಒಳ[0] == 0x1f) && (z->ಮುಂದಿನ_ಒಳ[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(ದತ್ತಾಂಶ, "Multi-member gzip response not supported");
        ಫಲಿತಾಂಶ = exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ನಿಲ್ಲಿಸು;
      }
      ಫಲಿತಾಂಶ = process_trailer(ದತ್ತಾಂಶ, zp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Z_ದತ್ತಾಂಶ_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ಆದರೆ(zp->zlib_init == ZLIB_INIT) {
        ಆದರೆ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->ಮುಂದಿನ_ಒಳ = orig_ಒಳ;
          z->avail_ಒಳ = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          ಮುಗಿದ = FALSE;
          ನಿಲ್ಲಿಸು;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      ಫಲಿತಾಂಶ = exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, process_zlib_error(ದತ್ತಾಂಶ, z));
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, process_zlib_error(ದತ್ತಾಂಶ, z));
      ನಿಲ್ಲಿಸು;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ಆದರೆ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Deflate handler. */
ಸ್ಥಿರ CURLcode deflate_do_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ಆದರೆ(inflateInit(z) != Z_OK)
    ಹಿಂತಿರುಗಿ process_zlib_error(ದತ್ತಾಂಶ, z);
  zp->zlib_init = ZLIB_INIT;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode deflate_do_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->ಮುಂದಿನ_ಒಳ = (z_const Bytef *)buf;
  z->avail_ಒಳ = (uInt)nbytes;

  ಆದರೆ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    ಹಿಂತಿರುಗಿ process_trailer(ದತ್ತಾಂಶ, zp);

  /* Now uncompress the data */
  ಹಿಂತಿರುಗಿ inflate_ಹರಿವು(ದತ್ತಾಂಶ, writer, type, ZLIB_INFLATING);
}

ಸ್ಥಿರ ಶೂನ್ಯ deflate_do_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_OK);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype deflate_encoding = {
  "deflate",
  ಖಾಲಿ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  ಗಾತ್ರ(ರಚನೆ zlib_writer)
};

/*
 * Gzip handler.
 */

ಸ್ಥಿರ CURLcode gzip_do_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ಆದರೆ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    ಹಿಂತಿರುಗಿ process_zlib_error(ದತ್ತಾಂಶ, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode gzip_do_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);

  ಆದರೆ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->ಮುಂದಿನ_ಒಳ = (z_const Bytef *)buf;
    z->avail_ಒಳ = (uInt)nbytes;
    /* Now uncompress the data */
    ಹಿಂತಿರುಗಿ inflate_ಹರಿವು(ದತ್ತಾಂಶ, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  ಹಿಂತಿರುಗಿ exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ಸ್ಥಿರ ಶೂನ್ಯ gzip_do_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zlib_writer *zp = (ರಚನೆ zlib_writer *)writer;
  z_ಹರಿವು *z = &zp->z; /* zlib state structure */

  exit_zlib(ದತ್ತಾಂಶ, z, &zp->zlib_init, CURLE_OK);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  ಗಾತ್ರ(ರಚನೆ zlib_writer)
};

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_LIBZ */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
/* Brotli writer. */
ರಚನೆ brotli_writer {
  ರಚನೆ Curl_cwriter super;
  ಅಕ್ಷರ ಬಫರ್[DECOMPRESS_ಬಫರ್_ಗಾತ್ರ];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ಸ್ಥಿರ CURLcode brotli_map_error(Brotli_Decoder_Error_ಸಂಕೇತ be)
{
  ಆಯ್ಕೆ(be) {
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_ಖಂಡ_ಉದ್ದಳತೆ_1:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_ಖಂಡ_ಉದ್ದಳತೆ_2:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_ಕಿಟಕಿ_ಬಿಟ್ಟುಗಳು:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_DICTIONARY_NOT_ಗಣ:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_ವೃಕ್ಷ_GROUPS:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_RING_ಬಫರ್_1:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_RING_ಬಫರ್_2:
  ಸ್ಥಿತಿ BROTLI_DECODER_ERROR_ALLOC_ಖಂಡ_TYPE_TREES:
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
}

ಸ್ಥಿರ CURLcode brotli_do_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ brotli_writer *bp = (ರಚನೆ brotli_writer *)writer;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  bp->br = BrotliDecoderCreateInstance(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ bp->br ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

ಸ್ಥಿರ CURLcode brotli_do_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ರಚನೆ brotli_writer *bp = (ರಚನೆ brotli_writer *)writer;
  ಸ್ಥಿರಾಂಕ uint8_t *src = (ಸ್ಥಿರಾಂಕ uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  Brotli_Decoder_ಫಲಿತಾಂಶ r = BROTLI_DECODER_ಫಲಿತಾಂಶ_NEEDS_ಇನ್ನಷ್ಟು_OUTPUT;
  ಪೂರ್ಣಾಂಕ i = 0;

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);

  ಆದರೆ(!bp->br)
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR; /* Stream already ended. */

  ಆಗ((nbytes || r == BROTLI_DECODER_ಫಲಿತಾಂಶ_NEEDS_ಇನ್ನಷ್ಟು_OUTPUT) &&
        ಫಲಿತಾಂಶ == CURLE_OK) {

    ಆದರೆ(++i > (1024 * 1024 / DECOMPRESS_ಬಫರ್_ಗಾತ್ರ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
        failf(ದತ್ತಾಂಶ, "Operation timed out while decoding payload");
        ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->ಬಫರ್;
    dstleft = DECOMPRESS_ಬಫರ್_ಗಾತ್ರ;
    r = Brotli_Decoder_Decompress_ಹರಿವು(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ಖಾಲಿ);
    ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type,
                                bp->ಬಫರ್, DECOMPRESS_ಬಫರ್_ಗಾತ್ರ - dstleft);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    ಆಯ್ಕೆ(r) {
    ಸ್ಥಿತಿ BROTLI_DECODER_ಫಲಿತಾಂಶ_NEEDS_ಇನ್ನಷ್ಟು_OUTPUT:
    ಸ್ಥಿತಿ BROTLI_DECODER_ಫಲಿತಾಂಶ_NEEDS_ಇನ್ನಷ್ಟು_ಒಳಹರಿವು:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ BROTLI_DECODER_ಫಲಿತಾಂಶ_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ಖಾಲಿ;
      ಆದರೆ(nbytes)
        ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = brotli_map_error(Brotli_Decoder_Get_Error_ಸಂಕೇತ(bp->br));
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ brotli_do_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ brotli_writer *bp = (ರಚನೆ brotli_writer *)writer;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಆದರೆ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype brotli_encoding = {
  "br",
  ಖಾಲಿ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  ಗಾತ್ರ(ರಚನೆ brotli_writer)
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
/* Zstd writer. */
ರಚನೆ zstd_writer {
  ರಚನೆ Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  ಅಕ್ಷರ ಬಫರ್[DECOMPRESS_ಬಫರ್_ಗಾತ್ರ];
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ZSTD_STATIC_LINKING_ONLY
ಸ್ಥಿರ ಶೂನ್ಯ *Curl_zstd_alloc(ಶೂನ್ಯ *opaque, size_t ಗಾತ್ರ_2)
{
  (ಶೂನ್ಯ)opaque;
  ಹಿಂತಿರುಗಿ Curl_cmalloc(ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ Curl_zstd_free(ಶೂನ್ಯ *opaque, ಶೂನ್ಯ *ವಿಳಾಸ)
{
  (ಶೂನ್ಯ)opaque;
  Curl_cfree(ವಿಳಾಸ);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode zstd_do_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zstd_writer *zp = (ರಚನೆ zstd_writer *)writer;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ಖಾಲಿ
  });
#ಇಲ್ಲದಿದ್ದರೆ
  zp->zds = ZSTD_createDStream();
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ zp->zds ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

ಸ್ಥಿರ CURLcode zstd_do_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ zstd_writer *zp = (ರಚನೆ zstd_writer *)writer;
  ZSTD_ಒಳ_ಬಫರ್ ಒಳ;
  ZSTD_ಹೊರಹೋಗು_ಬಫರ್ ಹೊರಹೋಗು;
  size_t error_ಸಂಕೇತ_2;
  ಪೂರ್ಣಾಂಕ i = 0;

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);

  ಒಳ.pos = 0;
  ಒಳ.src = buf;
  ಒಳ.ಗಾತ್ರ_2 = nbytes;

  ಗಾಗಿ(;;) {
    ಆದರೆ(++i > (1024 * 1024 / DECOMPRESS_ಬಫರ್_ಗಾತ್ರ)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
        failf(ದತ್ತಾಂಶ, "Operation timed out while decoding payload");
        ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
      }
    }

    ಹೊರಹೋಗು.pos = 0;
    ಹೊರಹೋಗು.dst = zp->ಬಫರ್;
    ಹೊರಹೋಗು.ಗಾತ್ರ_2 = DECOMPRESS_ಬಫರ್_ಗಾತ್ರ;

    error_ಸಂಕೇತ_2 = ZSTD_decompress_ಹರಿವು(zp->zds, &ಹೊರಹೋಗು, &ಒಳ);
    ಆದರೆ(ZSTD_isError(error_ಸಂಕೇತ_2)) {
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
    }
    ಆದರೆ(ಹೊರಹೋಗು.pos > 0) {
      ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type,
                                  zp->ಬಫರ್, ಹೊರಹೋಗು.pos);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ((ಒಳ.pos == nbytes) && (ಹೊರಹೋಗು.pos < ಹೊರಹೋಗು.ಗಾತ್ರ_2))
      ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ zstd_do_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ zstd_writer *zp = (ರಚನೆ zstd_writer *)writer;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಆದರೆ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype zstd_encoding = {
  "zstd",
  ಖಾಲಿ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  ಗಾತ್ರ(ರಚನೆ zstd_writer)
};
#ಆದರೆ_ಅಂತ್ಯ

/* Identity handler. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype ತಟಸ್ಥ_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಗಾತ್ರ(ರಚನೆ Curl_cwriter)
};

/* supported general content decoders. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype * ಸ್ಥಿರಾಂಕ general_unencoders[] = {
  &ತಟಸ್ಥ_encoding,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  &brotli_encoding,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  &zstd_encoding,
#ಆದರೆ_ಅಂತ್ಯ
  ಖಾಲಿ
};

/* supported content decoders only for transfer encodings */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype * ಸ್ಥಿರಾಂಕ transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ಖಾಲಿ
};

/* Return the list of comma-separated names of supported encodings.
 */
ಅಕ್ಷರ *Curl_get_content_encodings(ಶೂನ್ಯ)
{
  ರಚನೆ dynbuf enc;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype * ಸ್ಥಿರಾಂಕ *cep;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  ಗಾಗಿ(cep = general_unencoders; *cep && !ಫಲಿತಾಂಶ; cep++) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *ce = *cep;
    ಆದರೆ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ಆದರೆ(curlx_dyn_len(&enc))
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&enc, ", ", 2);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&enc, ce->name);
    }
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && !curlx_dyn_len(&enc))
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&enc, CONTENT_ENCODING_DEFAULT);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&enc);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/* Deferred error dummy writer. */
ಸ್ಥಿರ CURLcode error_do_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)writer;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode error_do_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  (ಶೂನ್ಯ)writer;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)nbytes;

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || !nbytes)
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);
  failf(ದತ್ತಾಂಶ, "Unrecognized content encoding type");
  ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
}

ಸ್ಥಿರ ಶೂನ್ಯ error_do_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)writer;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype error_writer = {
  "ce-error",
  ಖಾಲಿ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  ಗಾತ್ರ(ರಚನೆ Curl_cwriter)
};

/* Find the content encoding by name. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *ಕಂಡುಹಿಡಿ_unencode_writer(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype * ಸ್ಥಿರಾಂಕ *cep;

  ಆದರೆ(phase == CURL_CW_TRANSFER_DECODE) {
    ಗಾಗಿ(cep = transfer_unencoders; *cep; cep++) {
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *ce = *cep;
      ಆದರೆ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        ಹಿಂತಿರುಗಿ ce;
    }
  }
  /* look among the general decoders */
  ಗಾಗಿ(cep = general_unencoders; *cep; cep++) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *ce = *cep;
    ಆದರೆ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      ಹಿಂತಿರುಗಿ ce;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ರಾಶಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enclist, ಪೂರ್ಣಾಂಕ is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode ಫಲಿತಾಂಶ;
  bool has_chunked = FALSE;

  ಮಾಡು {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    ಆಗ(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    ಗಾಗಿ(namelen = 0; *enclist && *enclist != ','; enclist++)
      ಆದರೆ(*enclist > ' ')
        namelen = enclist - name + 1;

    ಆದರೆ(namelen) {
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *cwt;
      ರಚನೆ Curl_cwriter *writer;

      CURL_TRC_WRITE(ದತ್ತಾಂಶ, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (ಪೂರ್ಣಾಂಕ)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ಆದರೆ((is_transfer && !ದತ್ತಾಂಶ->ಗಣ.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && ದತ್ತಾಂಶ->ಗಣ.http_ce_skip)) {
        bool is_ತಟಸ್ಥ = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(ದತ್ತಾಂಶ, "decoder not requested, ignored: %.*s",
                       (ಪೂರ್ಣಾಂಕ)namelen, name);
        ಆದರೆ(is_transfer && !ದತ್ತಾಂಶ->ಗಣ.http_te_skip) {
          ಆದರೆ(has_chunked)
            failf(ದತ್ತಾಂಶ, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (ಪೂರ್ಣಾಂಕ)namelen, name);
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_ತಟಸ್ಥ)
            ಮುಂದುವರಿ;
          ಇಲ್ಲದಿದ್ದರೆ
            failf(ದತ್ತಾಂಶ, "Unsolicited Transfer-Encoding (%.*s) found",
                  (ಪೂರ್ಣಾಂಕ)namelen, name);
          ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
        }
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }

      ಆದರೆ(Curl_cwriter_ಎಣಿಕೆ(ದತ್ತಾಂಶ, phase) >= MAX_ENCODE_ರಾಶಿ) {
        failf(ದತ್ತಾಂಶ, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ರಾಶಿ,
              is_transfer ? "transfer" : "content");
        ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = ಕಂಡುಹಿಡಿ_unencode_writer(name, namelen, phase);
      ಆದರೆ(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(ದತ್ತಾಂಶ, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(ದತ್ತಾಂಶ, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
      }
      ಆದರೆ(cwt && is_chunked && Curl_cwriter_get_by_type(ದತ್ತಾಂಶ, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(ದತ್ತಾಂಶ, "ignoring duplicate 'chunked' decoder");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        ಫಲಿತಾಂಶ = Curl_cwriter_create(&writer, ದತ್ತಾಂಶ, cwt, phase);
        CURL_TRC_WRITE(ದತ್ತಾಂಶ, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        ಫಲಿತಾಂಶ = Curl_cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          Curl_cwriter_free(ದತ್ತಾಂಶ, writer);
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
      ಆದರೆ(is_chunked)
        has_chunked = TRUE;
    }
  } ಆಗ(*enclist);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಇಲ್ಲದಿದ್ದರೆ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ರಾಶಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enclist, ಪೂರ್ಣಾಂಕ is_transfer)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)enclist;
  (ಶೂನ್ಯ)is_transfer;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

ಅಕ್ಷರ *Curl_get_content_encodings(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */
