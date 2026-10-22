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

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
#ضمن <zlib.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wvla"
#جيڪڏهن_پورو
#ضمن <brotli/decode.h>
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
#ضمن <zstd.h>
#جيڪڏهن_پورو

#ضمن "connect.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "content_encoding.h"

#وصف_ڪريو CONTENT_ENCODING_DEFAULT  "identity"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#وصف_ڪريو MAX_ENCODE_ڍير 5

#إذا defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#وصف_ڪريو DECOMPRESS_بفر_سائيز 16384 /* buffer size for decompressed data */
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ

#إذا !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#غلطي "zlib 1.2.5.2 or greater required"
#جيڪڏهن_پورو

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
  حرف بفر[DECOMPRESS_بفر_سائيز]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_وهڪرو z;                /* State structure for zlib. */
};

ساكن voidpf zalloc_cb(voidpf opaque, بنانشان صحيح شيون, بنانشان صحيح سائيز)
{
  (فراغ)opaque;
  /* not a typo, keep it curlx_calloc() */
  أرجع curlx_calloc(شيون, سائيز);
}

ساكن فراغ zfree_cb(voidpf opaque, voidpf ptr)
{
  (فراغ)opaque;
  curlx_free(ptr);
}

ساكن CURLcode process_zlib_error(بنية Curl_easy *معلومات, z_وهڪرو *z)
{
  إذا(z->نياپو)
    failf(معلومات, "Error while processing content unencoding: %s", z->نياپو);
  وإلا
    failf(معلومات, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  أرجع CURLE_BAD_CONTENT_ENCODING;
}

ساكن CURLcode exit_zlib(بنية Curl_easy *معلومات, z_وهڪرو *z,
                          zlibInitState *zlib_init, CURLcode نتيجو)
{
  إذا(*zlib_init != ZLIB_UNINIT) {
    إذا(inflate_پڇاڙي(z) != Z_OK && نتيجو == CURLE_OK)
      نتيجو = process_zlib_error(معلومات, z);
    *zlib_init = ZLIB_UNINIT;
  }

  أرجع نتيجو;
}

ساكن CURLcode process_trailer(بنية Curl_easy *معلومات, بنية zlib_writer *zp)
{
  z_وهڪرو *z = &zp->z;
  CURLcode نتيجو = CURLE_OK;
  uInt len = z->avail_اندريون < zp->trailerlen ? z->avail_اندريون : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_اندريون -= len;
  z->ايندڙ_اندريون += len;
  إذا(z->avail_اندريون)
    نتيجو = CURLE_WRITE_ERROR;
  إذا(نتيجو || !zp->trailerlen)
    نتيجو = exit_zlib(معلومات, z, &zp->zlib_init, نتيجو);
  وإلا {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  أرجع نتيجو;
}

ساكن CURLcode inflate_وهڪرو(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *writer, صحيح type,
                               zlibInitState started)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_اندريون;
  z_const Bytef *orig_اندريون = z->ايندڙ_اندريون;
  bool پورو = FALSE;
  CURLcode نتيجو = CURLE_OK;   /* Curl_client_write status */
  صحيح i = 0;

  /* Check state. */
  إذا(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    أرجع exit_zlib(معلومات, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  طالما(!پورو) {
    صحيح حالت; /* zlib status */
    پورو = TRUE;

    إذا(++i > (1024 * 1024 / DECOMPRESS_بفر_سائيز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      إذا(Curl_timeleft_ms(معلومات) < 0) {
        failf(معلومات, "Operation timed out while decoding payload");
        أرجع exit_zlib(معلومات, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->ايندڙ_ٻاهريون = (Bytef *)zp->بفر;
    z->avail_ٻاهريون = DECOMPRESS_بفر_سائيز;

    حالت = inflate(z, Z_بلاڪ);

    /* Flush output data if some. */
    إذا(z->avail_ٻاهريون != DECOMPRESS_بفر_سائيز) {
      إذا(حالت == Z_OK || حالت == Z_وهڪرو_پڇاڙي) {
        zp->zlib_init = started; /* Data started. */
        نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, type, zp->بفر,
                                    DECOMPRESS_بفر_سائيز - z->avail_ٻاهريون);
        إذا(نتيجو) {
          exit_zlib(معلومات, z, &zp->zlib_init, نتيجو);
          اكسر;
        }
      }
    }

    /* Dispatch by inflate() status. */
    بدل(حالت) {
    حالة Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      پورو = FALSE;
      اكسر;
    حالة Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      اكسر;
    حالة Z_وهڪرو_پڇاڙي:
      إذا((started == ZLIB_INIT_GZIP) && (z->avail_اندريون >= 2) &&
         (z->ايندڙ_اندريون[0] == 0x1f) && (z->ايندڙ_اندريون[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(معلومات, "Multi-member gzip response not supported");
        نتيجو = exit_zlib(معلومات, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        اكسر;
      }
      نتيجو = process_trailer(معلومات, zp);
      اكسر;
    حالة Z_معلومات_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      إذا(zp->zlib_init == ZLIB_INIT) {
        إذا(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->ايندڙ_اندريون = orig_اندريون;
          z->avail_اندريون = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          پورو = FALSE;
          اكسر;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      نتيجو = exit_zlib(معلومات, z, &zp->zlib_init, process_zlib_error(معلومات, z));
      اكسر;
    افتراضي:
      نتيجو = exit_zlib(معلومات, z, &zp->zlib_init, process_zlib_error(معلومات, z));
      اكسر;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  إذا(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  أرجع نتيجو;
}

/* Deflate handler. */
ساكن CURLcode deflate_do_init(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  إذا(inflateInit(z) != Z_OK)
    أرجع process_zlib_error(معلومات, z);
  zp->zlib_init = ZLIB_INIT;
  أرجع CURLE_OK;
}

ساكن CURLcode deflate_do_write(بنية Curl_easy *معلومات,
                                 بنية Curl_cwriter *writer, صحيح type,
                                 ثابت حرف *buf, size_t nbytes)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  إذا(!(type & CLIENTWRITE_BODY) || !nbytes)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->ايندڙ_اندريون = (z_const Bytef *)buf;
  z->avail_اندريون = (uInt)nbytes;

  إذا(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    أرجع process_trailer(معلومات, zp);

  /* Now uncompress the data */
  أرجع inflate_وهڪرو(معلومات, writer, type, ZLIB_INFLATING);
}

ساكن فراغ deflate_do_close(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  exit_zlib(معلومات, z, &zp->zlib_init, CURLE_OK);
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

ساكن CURLcode gzip_do_init(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  إذا(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    أرجع process_zlib_error(معلومات, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  أرجع CURLE_OK;
}

ساكن CURLcode gzip_do_write(بنية Curl_easy *معلومات,
                              بنية Curl_cwriter *writer, صحيح type,
                              ثابت حرف *buf, size_t nbytes)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  إذا(!(type & CLIENTWRITE_BODY) || !nbytes)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);

  إذا(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->ايندڙ_اندريون = (z_const Bytef *)buf;
    z->avail_اندريون = (uInt)nbytes;
    /* Now uncompress the data */
    أرجع inflate_وهڪرو(معلومات, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  أرجع exit_zlib(معلومات, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ساكن فراغ gzip_do_close(بنية Curl_easy *معلومات,
                          بنية Curl_cwriter *writer)
{
  بنية zlib_writer *zp = (بنية zlib_writer *)writer;
  z_وهڪرو *z = &zp->z; /* zlib state structure */

  exit_zlib(معلومات, z, &zp->zlib_init, CURLE_OK);
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

#جيڪڏهن_پورو /* HAVE_LIBZ */

#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
/* Brotli writer. */
بنية brotli_writer {
  بنية Curl_cwriter super;
  حرف بفر[DECOMPRESS_بفر_سائيز];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ساكن CURLcode brotli_map_error(Brotli_Decoder_Error_ڪوڊ be)
{
  بدل(be) {
  حالة BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  حالة BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  حالة BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  حالة BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  حالة BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  حالة BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  حالة BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  حالة BROTLI_DECODER_ERROR_FORMAT_بلاڪ_ڊيگهائي_1:
  حالة BROTLI_DECODER_ERROR_FORMAT_بلاڪ_ڊيگهائي_2:
  حالة BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  حالة BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  حالة BROTLI_DECODER_ERROR_FORMAT_دري_بٽ:
  حالة BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  حالة BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#جيڪڏهن_وصف_ٿيل BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  حالة BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#جيڪڏهن_پورو
  حالة BROTLI_DECODER_ERROR_DICTIONARY_NOT_سيٽ:
  حالة BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    أرجع CURLE_BAD_CONTENT_ENCODING;
  حالة BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  حالة BROTLI_DECODER_ERROR_ALLOC_وڻ_GROUPS:
  حالة BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  حالة BROTLI_DECODER_ERROR_ALLOC_RING_بفر_1:
  حالة BROTLI_DECODER_ERROR_ALLOC_RING_بفر_2:
  حالة BROTLI_DECODER_ERROR_ALLOC_بلاڪ_TYPE_TREES:
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_WRITE_ERROR;
}

ساكن CURLcode brotli_do_init(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *writer)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  (فراغ)معلومات;

  bp->br = BrotliDecoderCreateInstance(عدم, عدم, عدم);
  أرجع bp->br ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}

ساكن CURLcode brotli_do_write(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  ثابت uint8_t *src = (ثابت uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode نتيجو = CURLE_OK;
  Brotli_Decoder_نتيجو r = BROTLI_DECODER_نتيجو_NEEDS_وڌيڪ_OUTPUT;
  صحيح i = 0;

  إذا(!(type & CLIENTWRITE_BODY) || !nbytes)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);

  إذا(!bp->br)
    أرجع CURLE_WRITE_ERROR; /* Stream already ended. */

  طالما((nbytes || r == BROTLI_DECODER_نتيجو_NEEDS_وڌيڪ_OUTPUT) &&
        نتيجو == CURLE_OK) {

    إذا(++i > (1024 * 1024 / DECOMPRESS_بفر_سائيز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      إذا(Curl_timeleft_ms(معلومات) < 0) {
        failf(معلومات, "Operation timed out while decoding payload");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->بفر;
    dstleft = DECOMPRESS_بفر_سائيز;
    r = Brotli_Decoder_Decompress_وهڪرو(bp->br,
                                      &nbytes, &src, &dstleft, &dst, عدم);
    نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, type,
                                bp->بفر, DECOMPRESS_بفر_سائيز - dstleft);
    إذا(نتيجو)
      اكسر;
    بدل(r) {
    حالة BROTLI_DECODER_نتيجو_NEEDS_وڌيڪ_OUTPUT:
    حالة BROTLI_DECODER_نتيجو_NEEDS_وڌيڪ_داخلا:
      اكسر;
    حالة BROTLI_DECODER_نتيجو_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = عدم;
      إذا(nbytes)
        نتيجو = CURLE_WRITE_ERROR;
      اكسر;
    افتراضي:
      نتيجو = brotli_map_error(Brotli_Decoder_Get_Error_ڪوڊ(bp->br));
      اكسر;
    }
  }
  أرجع نتيجو;
}

ساكن فراغ brotli_do_close(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  بنية brotli_writer *bp = (بنية brotli_writer *)writer;
  (فراغ)معلومات;

  إذا(bp->br) {
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
/* Zstd writer. */
بنية zstd_writer {
  بنية Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  حرف بفر[DECOMPRESS_بفر_سائيز];
};

#جيڪڏهن_وصف_ٿيل ZSTD_STATIC_LINKING_ONLY
ساكن فراغ *Curl_zstd_alloc(فراغ *opaque, size_t سائيز)
{
  (فراغ)opaque;
  أرجع Curl_cmalloc(سائيز);
}

ساكن فراغ Curl_zstd_free(فراغ *opaque, فراغ *پتو)
{
  (فراغ)opaque;
  Curl_cfree(پتو);
}
#جيڪڏهن_پورو

ساكن CURLcode zstd_do_init(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer)
{
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;

  (فراغ)معلومات;

#جيڪڏهن_وصف_ٿيل ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = عدم
  });
#وإلا
  zp->zds = ZSTD_createDStream();
#جيڪڏهن_پورو

  أرجع zp->zds ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}

ساكن CURLcode zstd_do_write(بنية Curl_easy *معلومات,
                              بنية Curl_cwriter *writer, صحيح type,
                              ثابت حرف *buf, size_t nbytes)
{
  CURLcode نتيجو = CURLE_OK;
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;
  ZSTD_اندريون_بفر اندريون;
  ZSTD_ٻاهريون_بفر ٻاهريون;
  size_t error_ڪوڊ_2;
  صحيح i = 0;

  إذا(!(type & CLIENTWRITE_BODY) || !nbytes)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);

  اندريون.pos = 0;
  اندريون.src = buf;
  اندريون.سائيز = nbytes;

  لكل(;;) {
    إذا(++i > (1024 * 1024 / DECOMPRESS_بفر_سائيز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      إذا(Curl_timeleft_ms(معلومات) < 0) {
        failf(معلومات, "Operation timed out while decoding payload");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }

    ٻاهريون.pos = 0;
    ٻاهريون.dst = zp->بفر;
    ٻاهريون.سائيز = DECOMPRESS_بفر_سائيز;

    error_ڪوڊ_2 = ZSTD_decompress_وهڪرو(zp->zds, &ٻاهريون, &اندريون);
    إذا(ZSTD_isError(error_ڪوڊ_2)) {
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }
    إذا(ٻاهريون.pos > 0) {
      نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, type,
                                  zp->بفر, ٻاهريون.pos);
      إذا(نتيجو)
        اكسر;
    }
    إذا((اندريون.pos == nbytes) && (ٻاهريون.pos < ٻاهريون.سائيز))
      اكسر;
  }

  أرجع نتيجو;
}

ساكن فراغ zstd_do_close(بنية Curl_easy *معلومات,
                          بنية Curl_cwriter *writer)
{
  بنية zstd_writer *zp = (بنية zstd_writer *)writer;
  (فراغ)معلومات;

  إذا(zp->zds) {
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
#جيڪڏهن_پورو

/* Identity handler. */
ساكن ثابت بنية Curl_cwtype سڃاڻپ_عنصر_encoding = {
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
  &سڃاڻپ_عنصر_encoding,
#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  &brotli_encoding,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  &zstd_encoding,
#جيڪڏهن_پورو
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
  CURLcode نتيجو = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  لكل(cep = general_unencoders; *cep && !نتيجو; cep++) {
    ثابت بنية Curl_cwtype *ce = *cep;
    إذا(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      إذا(curlx_dyn_len(&enc))
        نتيجو = curlx_dyn_addn(&enc, ", ", 2);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_جوڙيو(&enc, ce->name);
    }
  }
  إذا(!نتيجو && !curlx_dyn_len(&enc))
    نتيجو = curlx_dyn_جوڙيو(&enc, CONTENT_ENCODING_DEFAULT);

  إذا(!نتيجو)
    أرجع curlx_dyn_ptr(&enc);
  أرجع عدم;
}

/* Deferred error dummy writer. */
ساكن CURLcode error_do_init(بنية Curl_easy *معلومات,
                              بنية Curl_cwriter *writer)
{
  (فراغ)معلومات;
  (فراغ)writer;
  أرجع CURLE_OK;
}

ساكن CURLcode error_do_write(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *writer, صحيح type,
                               ثابت حرف *buf, size_t nbytes)
{
  (فراغ)writer;
  (فراغ)buf;
  (فراغ)nbytes;

  إذا(!(type & CLIENTWRITE_BODY) || !nbytes)
    أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);
  failf(معلومات, "Unrecognized content encoding type");
  أرجع CURLE_BAD_CONTENT_ENCODING;
}

ساكن فراغ error_do_close(بنية Curl_easy *معلومات,
                           بنية Curl_cwriter *writer)
{
  (فراغ)معلومات;
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
ساكن ثابت بنية Curl_cwtype *ڳوليو_unencode_writer(ثابت حرف *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ثابت بنية Curl_cwtype * ثابت *cep;

  إذا(phase == CURL_CW_TRANSFER_DECODE) {
    لكل(cep = transfer_unencoders; *cep; cep++) {
      ثابت بنية Curl_cwtype *ce = *cep;
      إذا((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        أرجع ce;
    }
  }
  /* look among the general decoders */
  لكل(cep = general_unencoders; *cep; cep++) {
    ثابت بنية Curl_cwtype *ce = *cep;
    إذا((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      أرجع ce;
  }
  أرجع عدم;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ڍير(بنية Curl_easy *معلومات,
                                     ثابت حرف *enclist, صحيح is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode نتيجو;
  bool has_chunked = FALSE;

  افعل {
    ثابت حرف *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    طالما(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    لكل(namelen = 0; *enclist && *enclist != ','; enclist++)
      إذا(*enclist > ' ')
        namelen = enclist - name + 1;

    إذا(namelen) {
      ثابت بنية Curl_cwtype *cwt;
      بنية Curl_cwriter *writer;

      CURL_TRC_WRITE(معلومات, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (صحيح)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      إذا((is_transfer && !معلومات->سيٽ.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && معلومات->سيٽ.http_ce_skip)) {
        bool is_سڃاڻپ_عنصر = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(معلومات, "decoder not requested, ignored: %.*s",
                       (صحيح)namelen, name);
        إذا(is_transfer && !معلومات->سيٽ.http_te_skip) {
          إذا(has_chunked)
            failf(معلومات, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (صحيح)namelen, name);
          وإلا إذا(is_سڃاڻپ_عنصر)
            استمر;
          وإلا
            failf(معلومات, "Unsolicited Transfer-Encoding (%.*s) found",
                  (صحيح)namelen, name);
          أرجع CURLE_BAD_CONTENT_ENCODING;
        }
        أرجع CURLE_OK;
      }

      إذا(Curl_cwriter_ڳڻپ(معلومات, phase) >= MAX_ENCODE_ڍير) {
        failf(معلومات, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ڍير,
              is_transfer ? "transfer" : "content");
        أرجع CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = ڳوليو_unencode_writer(name, namelen, phase);
      إذا(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(معلومات, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(معلومات, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        أرجع CURLE_BAD_CONTENT_ENCODING;
      }
      إذا(cwt && is_chunked && Curl_cwriter_get_by_type(معلومات, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(معلومات, "ignoring duplicate 'chunked' decoder");
      }
      وإلا {
        إذا(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        نتيجو = Curl_cwriter_create(&writer, معلومات, cwt, phase);
        CURL_TRC_WRITE(معلومات, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (صحيح)نتيجو);
        إذا(نتيجو)
          أرجع نتيجو;

        نتيجو = Curl_cwriter_جوڙيو(معلومات, writer);
        إذا(نتيجو) {
          Curl_cwriter_free(معلومات, writer);
          أرجع نتيجو;
        }
      }
      إذا(is_chunked)
        has_chunked = TRUE;
    }
  } طالما(*enclist);

  أرجع CURLE_OK;
}

#وإلا
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ڍير(بنية Curl_easy *معلومات,
                                     ثابت حرف *enclist, صحيح is_transfer)
{
  (فراغ)معلومات;
  (فراغ)enclist;
  (فراغ)is_transfer;
  أرجع CURLE_NOT_BUILT_اندريون;
}

حرف *Curl_get_content_encodings(فراغ)
{
  أرجع curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#جيڪڏهن_پورو /* CURL_DISABLE_HTTP */
