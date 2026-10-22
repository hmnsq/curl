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

#اگر_تعریف_شدہ HAVE_LIBZ
#شامل <zlib.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_BROTLI
#اگر_تعریف_شدہ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wvla"
#اگر_ختم
#شامل <brotli/decode.h>
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ZSTD
#شامل <zstd.h>
#اگر_ختم

#شامل "connect.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "content_encoding.h"

#تعریف_کرو CONTENT_ENCODING_DEFAULT  "identity"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#تعریف_کرو MAX_ENCODE_ڈھیر 5

#اگر defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#تعریف_کرو DECOMPRESS_بفر_سائز 16384 /* buffer size for decompressed data */
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LIBZ

#اگر !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#غلطی "zlib 1.2.5.2 or greater required"
#اگر_ختم

قسمتعریف شمار {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
ساخت zlib_writer {
  ساخت Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  حرف بفر[DECOMPRESS_بفر_سائز]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_دھارا z;                /* State structure for zlib. */
};

ساکن voidpf zalloc_cb(voidpf opaque, غیرعلامتی صحیح اشیاء, غیرعلامتی صحیح سائز)
{
  (خالی)opaque;
  /* not a typo, keep it curlx_calloc() */
  واپس curlx_calloc(اشیاء, سائز);
}

ساکن خالی zfree_cb(voidpf opaque, voidpf ptr)
{
  (خالی)opaque;
  curlx_free(ptr);
}

ساکن CURLcode process_zlib_error(ساخت Curl_easy *ڈیٹا, z_دھارا *z)
{
  اگر(z->پیغام)
    failf(ڈیٹا, "Error while processing content unencoding: %s", z->پیغام);
  ورنہ
    failf(ڈیٹا, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  واپس CURLE_BAD_CONTENT_ENCODING;
}

ساکن CURLcode exit_zlib(ساخت Curl_easy *ڈیٹا, z_دھارا *z,
                          zlibInitState *zlib_init, CURLcode نتیجہ)
{
  اگر(*zlib_init != ZLIB_UNINIT) {
    اگر(inflate_انتہا(z) != Z_OK && نتیجہ == CURLE_OK)
      نتیجہ = process_zlib_error(ڈیٹا, z);
    *zlib_init = ZLIB_UNINIT;
  }

  واپس نتیجہ;
}

ساکن CURLcode process_trailer(ساخت Curl_easy *ڈیٹا, ساخت zlib_writer *zp)
{
  z_دھارا *z = &zp->z;
  CURLcode نتیجہ = CURLE_OK;
  uInt len = z->avail_اندر < zp->trailerlen ? z->avail_اندر : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_اندر -= len;
  z->اگلا_اندر += len;
  اگر(z->avail_اندر)
    نتیجہ = CURLE_WRITE_ERROR;
  اگر(نتیجہ || !zp->trailerlen)
    نتیجہ = exit_zlib(ڈیٹا, z, &zp->zlib_init, نتیجہ);
  ورنہ {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  واپس نتیجہ;
}

ساکن CURLcode inflate_دھارا(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *writer, صحیح type,
                               zlibInitState started)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_اندر;
  z_const Bytef *orig_اندر = z->اگلا_اندر;
  bool مکمل = FALSE;
  CURLcode نتیجہ = CURLE_OK;   /* Curl_client_write status */
  صحیح i = 0;

  /* Check state. */
  اگر(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    واپس exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  جب(!مکمل) {
    صحیح حالت_2; /* zlib status */
    مکمل = TRUE;

    اگر(++i > (1024 * 1024 / DECOMPRESS_بفر_سائز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
        failf(ڈیٹا, "Operation timed out while decoding payload");
        واپس exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->اگلا_برآمد = (Bytef *)zp->بفر;
    z->avail_برآمد = DECOMPRESS_بفر_سائز;

    حالت_2 = inflate(z, Z_بلاک);

    /* Flush output data if some. */
    اگر(z->avail_برآمد != DECOMPRESS_بفر_سائز) {
      اگر(حالت_2 == Z_OK || حالت_2 == Z_دھارا_انتہا) {
        zp->zlib_init = started; /* Data started. */
        نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, type, zp->بفر,
                                    DECOMPRESS_بفر_سائز - z->avail_برآمد);
        اگر(نتیجہ) {
          exit_zlib(ڈیٹا, z, &zp->zlib_init, نتیجہ);
          توڑو;
        }
      }
    }

    /* Dispatch by inflate() status. */
    انتخاب(حالت_2) {
    حالت Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      مکمل = FALSE;
      توڑو;
    حالت Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      توڑو;
    حالت Z_دھارا_انتہا:
      اگر((started == ZLIB_INIT_GZIP) && (z->avail_اندر >= 2) &&
         (z->اگلا_اندر[0] == 0x1f) && (z->اگلا_اندر[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(ڈیٹا, "Multi-member gzip response not supported");
        نتیجہ = exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        توڑو;
      }
      نتیجہ = process_trailer(ڈیٹا, zp);
      توڑو;
    حالت Z_ڈیٹا_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      اگر(zp->zlib_init == ZLIB_INIT) {
        اگر(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->اگلا_اندر = orig_اندر;
          z->avail_اندر = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          مکمل = FALSE;
          توڑو;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      نتیجہ = exit_zlib(ڈیٹا, z, &zp->zlib_init, process_zlib_error(ڈیٹا, z));
      توڑو;
    طےشدہ:
      نتیجہ = exit_zlib(ڈیٹا, z, &zp->zlib_init, process_zlib_error(ڈیٹا, z));
      توڑو;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  اگر(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  واپس نتیجہ;
}

/* Deflate handler. */
ساکن CURLcode deflate_do_init(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  اگر(inflateInit(z) != Z_OK)
    واپس process_zlib_error(ڈیٹا, z);
  zp->zlib_init = ZLIB_INIT;
  واپس CURLE_OK;
}

ساکن CURLcode deflate_do_write(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_cwriter *writer, صحیح type,
                                 ثابت حرف *buf, size_t nbytes)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->اگلا_اندر = (z_const Bytef *)buf;
  z->avail_اندر = (uInt)nbytes;

  اگر(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    واپس process_trailer(ڈیٹا, zp);

  /* Now uncompress the data */
  واپس inflate_دھارا(ڈیٹا, writer, type, ZLIB_INFLATING);
}

ساکن خالی deflate_do_close(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_OK);
}

ساکن ثابت ساخت Curl_cwtype deflate_encoding = {
  "deflate",
  تہی,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  حجم(ساخت zlib_writer)
};

/*
 * Gzip handler.
 */

ساکن CURLcode gzip_do_init(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  اگر(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    واپس process_zlib_error(ڈیٹا, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  واپس CURLE_OK;
}

ساکن CURLcode gzip_do_write(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_cwriter *writer, صحیح type,
                              ثابت حرف *buf, size_t nbytes)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);

  اگر(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->اگلا_اندر = (z_const Bytef *)buf;
    z->avail_اندر = (uInt)nbytes;
    /* Now uncompress the data */
    واپس inflate_دھارا(ڈیٹا, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  واپس exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

ساکن خالی gzip_do_close(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_cwriter *writer)
{
  ساخت zlib_writer *zp = (ساخت zlib_writer *)writer;
  z_دھارا *z = &zp->z; /* zlib state structure */

  exit_zlib(ڈیٹا, z, &zp->zlib_init, CURLE_OK);
}

ساکن ثابت ساخت Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  حجم(ساخت zlib_writer)
};

#اگر_ختم /* HAVE_LIBZ */

#اگر_تعریف_شدہ HAVE_BROTLI
/* Brotli writer. */
ساخت brotli_writer {
  ساخت Curl_cwriter super;
  حرف بفر[DECOMPRESS_بفر_سائز];
  BrotliDecoderState *br; /* State structure for brotli. */
};

ساکن CURLcode brotli_map_error(Brotli_Decoder_Error_رمز be)
{
  انتخاب(be) {
  حالت BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  حالت BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  حالت BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  حالت BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  حالت BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  حالت BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  حالت BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  حالت BROTLI_DECODER_ERROR_FORMAT_بلاک_لمبائی_1:
  حالت BROTLI_DECODER_ERROR_FORMAT_بلاک_لمبائی_2:
  حالت BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  حالت BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  حالت BROTLI_DECODER_ERROR_FORMAT_کھڑکی_بٹ:
  حالت BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  حالت BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#اگر_تعریف_شدہ BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  حالت BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#اگر_ختم
  حالت BROTLI_DECODER_ERROR_DICTIONARY_NOT_سیٹ:
  حالت BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    واپس CURLE_BAD_CONTENT_ENCODING;
  حالت BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  حالت BROTLI_DECODER_ERROR_ALLOC_درخت_GROUPS:
  حالت BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  حالت BROTLI_DECODER_ERROR_ALLOC_RING_بفر_1:
  حالت BROTLI_DECODER_ERROR_ALLOC_RING_بفر_2:
  حالت BROTLI_DECODER_ERROR_ALLOC_بلاک_TYPE_TREES:
    واپس CURLE_برآمد_کا_حافظہ;
  طےشدہ:
    توڑو;
  }
  واپس CURLE_WRITE_ERROR;
}

ساکن CURLcode brotli_do_init(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *writer)
{
  ساخت brotli_writer *bp = (ساخت brotli_writer *)writer;
  (خالی)ڈیٹا;

  bp->br = BrotliDecoderCreateInstance(تہی, تہی, تہی);
  واپس bp->br ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

ساکن CURLcode brotli_do_write(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer, صحیح type,
                                ثابت حرف *buf, size_t nbytes)
{
  ساخت brotli_writer *bp = (ساخت brotli_writer *)writer;
  ثابت uint8_t *src = (ثابت uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode نتیجہ = CURLE_OK;
  Brotli_Decoder_نتیجہ r = BROTLI_DECODER_نتیجہ_NEEDS_مزید_OUTPUT;
  صحیح i = 0;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);

  اگر(!bp->br)
    واپس CURLE_WRITE_ERROR; /* Stream already ended. */

  جب((nbytes || r == BROTLI_DECODER_نتیجہ_NEEDS_مزید_OUTPUT) &&
        نتیجہ == CURLE_OK) {

    اگر(++i > (1024 * 1024 / DECOMPRESS_بفر_سائز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
        failf(ڈیٹا, "Operation timed out while decoding payload");
        واپس CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->بفر;
    dstleft = DECOMPRESS_بفر_سائز;
    r = Brotli_Decoder_Decompress_دھارا(bp->br,
                                      &nbytes, &src, &dstleft, &dst, تہی);
    نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, type,
                                bp->بفر, DECOMPRESS_بفر_سائز - dstleft);
    اگر(نتیجہ)
      توڑو;
    انتخاب(r) {
    حالت BROTLI_DECODER_نتیجہ_NEEDS_مزید_OUTPUT:
    حالت BROTLI_DECODER_نتیجہ_NEEDS_مزید_داخلہ:
      توڑو;
    حالت BROTLI_DECODER_نتیجہ_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = تہی;
      اگر(nbytes)
        نتیجہ = CURLE_WRITE_ERROR;
      توڑو;
    طےشدہ:
      نتیجہ = brotli_map_error(Brotli_Decoder_Get_Error_رمز(bp->br));
      توڑو;
    }
  }
  واپس نتیجہ;
}

ساکن خالی brotli_do_close(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_cwriter *writer)
{
  ساخت brotli_writer *bp = (ساخت brotli_writer *)writer;
  (خالی)ڈیٹا;

  اگر(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = تہی;
  }
}

ساکن ثابت ساخت Curl_cwtype brotli_encoding = {
  "br",
  تہی,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  حجم(ساخت brotli_writer)
};
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ZSTD
/* Zstd writer. */
ساخت zstd_writer {
  ساخت Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  حرف بفر[DECOMPRESS_بفر_سائز];
};

#اگر_تعریف_شدہ ZSTD_STATIC_LINKING_ONLY
ساکن خالی *Curl_zstd_alloc(خالی *opaque, size_t سائز)
{
  (خالی)opaque;
  واپس Curl_cmalloc(سائز);
}

ساکن خالی Curl_zstd_free(خالی *opaque, خالی *پتہ)
{
  (خالی)opaque;
  Curl_cfree(پتہ);
}
#اگر_ختم

ساکن CURLcode zstd_do_init(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer)
{
  ساخت zstd_writer *zp = (ساخت zstd_writer *)writer;

  (خالی)ڈیٹا;

#اگر_تعریف_شدہ ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = تہی
  });
#ورنہ
  zp->zds = ZSTD_createDStream();
#اگر_ختم

  واپس zp->zds ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

ساکن CURLcode zstd_do_write(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_cwriter *writer, صحیح type,
                              ثابت حرف *buf, size_t nbytes)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت zstd_writer *zp = (ساخت zstd_writer *)writer;
  ZSTD_اندر_بفر اندر;
  ZSTD_برآمد_بفر برآمد;
  size_t error_رمز_2;
  صحیح i = 0;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);

  اندر.pos = 0;
  اندر.src = buf;
  اندر.سائز = nbytes;

  برائے(;;) {
    اگر(++i > (1024 * 1024 / DECOMPRESS_بفر_سائز)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
        failf(ڈیٹا, "Operation timed out while decoding payload");
        واپس CURLE_OPERATION_TIMEDOUT;
      }
    }

    برآمد.pos = 0;
    برآمد.dst = zp->بفر;
    برآمد.سائز = DECOMPRESS_بفر_سائز;

    error_رمز_2 = ZSTD_decompress_دھارا(zp->zds, &برآمد, &اندر);
    اگر(ZSTD_isError(error_رمز_2)) {
      واپس CURLE_BAD_CONTENT_ENCODING;
    }
    اگر(برآمد.pos > 0) {
      نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, type,
                                  zp->بفر, برآمد.pos);
      اگر(نتیجہ)
        توڑو;
    }
    اگر((اندر.pos == nbytes) && (برآمد.pos < برآمد.سائز))
      توڑو;
  }

  واپس نتیجہ;
}

ساکن خالی zstd_do_close(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_cwriter *writer)
{
  ساخت zstd_writer *zp = (ساخت zstd_writer *)writer;
  (خالی)ڈیٹا;

  اگر(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = تہی;
  }
}

ساکن ثابت ساخت Curl_cwtype zstd_encoding = {
  "zstd",
  تہی,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  حجم(ساخت zstd_writer)
};
#اگر_ختم

/* Identity handler. */
ساکن ثابت ساخت Curl_cwtype شناختی_عنصر_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(ساخت Curl_cwriter)
};

/* supported general content decoders. */
ساکن ثابت ساخت Curl_cwtype * ثابت general_unencoders[] = {
  &شناختی_عنصر_encoding,
#اگر_تعریف_شدہ HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#اگر_ختم
#اگر_تعریف_شدہ HAVE_BROTLI
  &brotli_encoding,
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ZSTD
  &zstd_encoding,
#اگر_ختم
  تہی
};

/* supported content decoders only for transfer encodings */
ساکن ثابت ساخت Curl_cwtype * ثابت transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  تہی
};

/* Return the list of comma-separated names of supported encodings.
 */
حرف *Curl_get_content_encodings(خالی)
{
  ساخت dynbuf enc;
  ثابت ساخت Curl_cwtype * ثابت *cep;
  CURLcode نتیجہ = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  برائے(cep = general_unencoders; *cep && !نتیجہ; cep++) {
    ثابت ساخت Curl_cwtype *ce = *cep;
    اگر(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      اگر(curlx_dyn_len(&enc))
        نتیجہ = curlx_dyn_addn(&enc, ", ", 2);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_جوڑو(&enc, ce->name);
    }
  }
  اگر(!نتیجہ && !curlx_dyn_len(&enc))
    نتیجہ = curlx_dyn_جوڑو(&enc, CONTENT_ENCODING_DEFAULT);

  اگر(!نتیجہ)
    واپس curlx_dyn_ptr(&enc);
  واپس تہی;
}

/* Deferred error dummy writer. */
ساکن CURLcode error_do_init(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_cwriter *writer)
{
  (خالی)ڈیٹا;
  (خالی)writer;
  واپس CURLE_OK;
}

ساکن CURLcode error_do_write(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *writer, صحیح type,
                               ثابت حرف *buf, size_t nbytes)
{
  (خالی)writer;
  (خالی)buf;
  (خالی)nbytes;

  اگر(!(type & CLIENTWRITE_BODY) || !nbytes)
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);
  failf(ڈیٹا, "Unrecognized content encoding type");
  واپس CURLE_BAD_CONTENT_ENCODING;
}

ساکن خالی error_do_close(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_cwriter *writer)
{
  (خالی)ڈیٹا;
  (خالی)writer;
}

ساکن ثابت ساخت Curl_cwtype error_writer = {
  "ce-error",
  تہی,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  حجم(ساخت Curl_cwriter)
};

/* Find the content encoding by name. */
ساکن ثابت ساخت Curl_cwtype *ڈھونڈو_unencode_writer(ثابت حرف *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  ثابت ساخت Curl_cwtype * ثابت *cep;

  اگر(phase == CURL_CW_TRANSFER_DECODE) {
    برائے(cep = transfer_unencoders; *cep; cep++) {
      ثابت ساخت Curl_cwtype *ce = *cep;
      اگر((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        واپس ce;
    }
  }
  /* look among the general decoders */
  برائے(cep = general_unencoders; *cep; cep++) {
    ثابت ساخت Curl_cwtype *ce = *cep;
    اگر((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      واپس ce;
  }
  واپس تہی;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ڈھیر(ساخت Curl_easy *ڈیٹا,
                                     ثابت حرف *enclist, صحیح is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode نتیجہ;
  bool has_chunked = FALSE;

  کرو {
    ثابت حرف *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    جب(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    برائے(namelen = 0; *enclist && *enclist != ','; enclist++)
      اگر(*enclist > ' ')
        namelen = enclist - name + 1;

    اگر(namelen) {
      ثابت ساخت Curl_cwtype *cwt;
      ساخت Curl_cwriter *writer;

      CURL_TRC_WRITE(ڈیٹا, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (صحیح)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      اگر((is_transfer && !ڈیٹا->سیٹ.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && ڈیٹا->سیٹ.http_ce_skip)) {
        bool is_شناختی_عنصر = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(ڈیٹا, "decoder not requested, ignored: %.*s",
                       (صحیح)namelen, name);
        اگر(is_transfer && !ڈیٹا->سیٹ.http_te_skip) {
          اگر(has_chunked)
            failf(ڈیٹا, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (صحیح)namelen, name);
          ورنہ اگر(is_شناختی_عنصر)
            جاری;
          ورنہ
            failf(ڈیٹا, "Unsolicited Transfer-Encoding (%.*s) found",
                  (صحیح)namelen, name);
          واپس CURLE_BAD_CONTENT_ENCODING;
        }
        واپس CURLE_OK;
      }

      اگر(Curl_cwriter_گنتی(ڈیٹا, phase) >= MAX_ENCODE_ڈھیر) {
        failf(ڈیٹا, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ڈھیر,
              is_transfer ? "transfer" : "content");
        واپس CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = ڈھونڈو_unencode_writer(name, namelen, phase);
      اگر(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(ڈیٹا, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(ڈیٹا, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        واپس CURLE_BAD_CONTENT_ENCODING;
      }
      اگر(cwt && is_chunked && Curl_cwriter_get_by_type(ڈیٹا, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(ڈیٹا, "ignoring duplicate 'chunked' decoder");
      }
      ورنہ {
        اگر(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        نتیجہ = Curl_cwriter_create(&writer, ڈیٹا, cwt, phase);
        CURL_TRC_WRITE(ڈیٹا, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (صحیح)نتیجہ);
        اگر(نتیجہ)
          واپس نتیجہ;

        نتیجہ = Curl_cwriter_جوڑو(ڈیٹا, writer);
        اگر(نتیجہ) {
          Curl_cwriter_free(ڈیٹا, writer);
          واپس نتیجہ;
        }
      }
      اگر(is_chunked)
        has_chunked = TRUE;
    }
  } جب(*enclist);

  واپس CURLE_OK;
}

#ورنہ
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ڈھیر(ساخت Curl_easy *ڈیٹا,
                                     ثابت حرف *enclist, صحیح is_transfer)
{
  (خالی)ڈیٹا;
  (خالی)enclist;
  (خالی)is_transfer;
  واپس CURLE_NOT_BUILT_اندر;
}

حرف *Curl_get_content_encodings(خالی)
{
  واپس curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#اگر_ختم /* CURL_DISABLE_HTTP */
