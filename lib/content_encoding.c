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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curlx/dynbuf.h"

#నిర్వచితమైతే HAVE_LIBZ
#అన్తర్భావ <zlib.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_BROTLI
#నిర్వచితమైతే CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wvla"
#అయితే_ముగింపు
#అన్తర్భావ <brotli/decode.h>
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ZSTD
#అన్తర్భావ <zstd.h>
#అయితే_ముగింపు

#అన్తర్భావ "connect.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "content_encoding.h"

#నిర్వచించు CONTENT_ENCODING_DEFAULT  "identity"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#నిర్వచించు MAX_ENCODE_దొంతర 5

#ఒకవేళ defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#నిర్వచించు DECOMPRESS_బఫరు_కొలత 16384 /* buffer size for decompressed data */
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LIBZ

#ఒకవేళ !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#దోషం "zlib 1.2.5.2 or greater required"
#అయితే_ముగింపు

రకనిర్వచనం గణన {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
నిర్మాణం zlib_writer {
  నిర్మాణం Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  అక్షరం బఫరు[DECOMPRESS_బఫరు_కొలత]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_ప్రవాహం z;                /* State structure for zlib. */
};

స్థిర voidpf zalloc_cb(voidpf opaque, గుర్తులేని పూర్ణాంకం వస్తువులు, గుర్తులేని పూర్ణాంకం కొలత)
{
  (శూన్య)opaque;
  /* not a typo, keep it curlx_calloc() */
  తిరిగి curlx_calloc(వస్తువులు, కొలత);
}

స్థిర శూన్య zfree_cb(voidpf opaque, voidpf ptr)
{
  (శూన్య)opaque;
  curlx_free(ptr);
}

స్థిర CURLcode process_zlib_error(నిర్మాణం Curl_easy *దత్తాంశం, z_ప్రవాహం *z)
{
  ఒకవేళ(z->సందేశం)
    failf(దత్తాంశం, "Error while processing content unencoding: %s", z->సందేశం);
  లేకపోతే
    failf(దత్తాంశం, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  తిరిగి CURLE_BAD_CONTENT_ENCODING;
}

స్థిర CURLcode exit_zlib(నిర్మాణం Curl_easy *దత్తాంశం, z_ప్రవాహం *z,
                          zlibInitState *zlib_init, CURLcode ఫలితం)
{
  ఒకవేళ(*zlib_init != ZLIB_UNINIT) {
    ఒకవేళ(inflate_చివర(z) != Z_OK && ఫలితం == CURLE_OK)
      ఫలితం = process_zlib_error(దత్తాంశం, z);
    *zlib_init = ZLIB_UNINIT;
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode process_trailer(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం zlib_writer *zp)
{
  z_ప్రవాహం *z = &zp->z;
  CURLcode ఫలితం = CURLE_OK;
  uInt len = z->avail_లోపల < zp->trailerlen ? z->avail_లోపల : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_లోపల -= len;
  z->తదుపరి_లోపల += len;
  ఒకవేళ(z->avail_లోపల)
    ఫలితం = CURLE_WRITE_ERROR;
  ఒకవేళ(ఫలితం || !zp->trailerlen)
    ఫలితం = exit_zlib(దత్తాంశం, z, &zp->zlib_init, ఫలితం);
  లేకపోతే {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode inflate_ప్రవాహం(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                               zlibInitState started)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_లోపల;
  z_const Bytef *orig_లోపల = z->తదుపరి_లోపల;
  bool పూర్తి = FALSE;
  CURLcode ఫలితం = CURLE_OK;   /* Curl_client_write status */
  పూర్ణాంకం i = 0;

  /* Check state. */
  ఒకవేళ(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    తిరిగి exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  అయితే(!పూర్తి) {
    పూర్ణాంకం స్థితి_2; /* zlib status */
    పూర్తి = TRUE;

    ఒకవేళ(++i > (1024 * 1024 / DECOMPRESS_బఫరు_కొలత)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
        failf(దత్తాంశం, "Operation timed out while decoding payload");
        తిరిగి exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->తదుపరి_బహిర్గతం = (Bytef *)zp->బఫరు;
    z->avail_బహిర్గతం = DECOMPRESS_బఫరు_కొలత;

    స్థితి_2 = inflate(z, Z_ఖండం);

    /* Flush output data if some. */
    ఒకవేళ(z->avail_బహిర్గతం != DECOMPRESS_బఫరు_కొలత) {
      ఒకవేళ(స్థితి_2 == Z_OK || స్థితి_2 == Z_ప్రవాహం_చివర) {
        zp->zlib_init = started; /* Data started. */
        ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, zp->బఫరు,
                                    DECOMPRESS_బఫరు_కొలత - z->avail_బహిర్గతం);
        ఒకవేళ(ఫలితం) {
          exit_zlib(దత్తాంశం, z, &zp->zlib_init, ఫలితం);
          ఆపు;
        }
      }
    }

    /* Dispatch by inflate() status. */
    ఎంపిక(స్థితి_2) {
    స్థితి Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      పూర్తి = FALSE;
      ఆపు;
    స్థితి Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      ఆపు;
    స్థితి Z_ప్రవాహం_చివర:
      ఒకవేళ((started == ZLIB_INIT_GZIP) && (z->avail_లోపల >= 2) &&
         (z->తదుపరి_లోపల[0] == 0x1f) && (z->తదుపరి_లోపల[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(దత్తాంశం, "Multi-member gzip response not supported");
        ఫలితం = exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        ఆపు;
      }
      ఫలితం = process_trailer(దత్తాంశం, zp);
      ఆపు;
    స్థితి Z_దత్తాంశం_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      ఒకవేళ(zp->zlib_init == ZLIB_INIT) {
        ఒకవేళ(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->తదుపరి_లోపల = orig_లోపల;
          z->avail_లోపల = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          పూర్తి = FALSE;
          ఆపు;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      ఫలితం = exit_zlib(దత్తాంశం, z, &zp->zlib_init, process_zlib_error(దత్తాంశం, z));
      ఆపు;
    అప్రమేయ:
      ఫలితం = exit_zlib(దత్తాంశం, z, &zp->zlib_init, process_zlib_error(దత్తాంశం, z));
      ఆపు;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  ఒకవేళ(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  తిరిగి ఫలితం;
}

/* Deflate handler. */
స్థిర CURLcode deflate_do_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ఒకవేళ(inflateInit(z) != Z_OK)
    తిరిగి process_zlib_error(దత్తాంశం, z);
  zp->zlib_init = ZLIB_INIT;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode deflate_do_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                 స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || !nbytes)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->తదుపరి_లోపల = (z_const Bytef *)buf;
  z->avail_లోపల = (uInt)nbytes;

  ఒకవేళ(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    తిరిగి process_trailer(దత్తాంశం, zp);

  /* Now uncompress the data */
  తిరిగి inflate_ప్రవాహం(దత్తాంశం, writer, type, ZLIB_INFLATING);
}

స్థిర శూన్య deflate_do_close(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_OK);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype deflate_encoding = {
  "deflate",
  ఖాళీ,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  పరిమాణం(నిర్మాణం zlib_writer)
};

/*
 * Gzip handler.
 */

స్థిర CURLcode gzip_do_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  ఒకవేళ(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    తిరిగి process_zlib_error(దత్తాంశం, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  తిరిగి CURLE_OK;
}

స్థిర CURLcode gzip_do_write(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                              స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || !nbytes)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);

  ఒకవేళ(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->తదుపరి_లోపల = (z_const Bytef *)buf;
    z->avail_లోపల = (uInt)nbytes;
    /* Now uncompress the data */
    తిరిగి inflate_ప్రవాహం(దత్తాంశం, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  తిరిగి exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

స్థిర శూన్య gzip_do_close(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zlib_writer *zp = (నిర్మాణం zlib_writer *)writer;
  z_ప్రవాహం *z = &zp->z; /* zlib state structure */

  exit_zlib(దత్తాంశం, z, &zp->zlib_init, CURLE_OK);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  పరిమాణం(నిర్మాణం zlib_writer)
};

#అయితే_ముగింపు /* HAVE_LIBZ */

#నిర్వచితమైతే HAVE_BROTLI
/* Brotli writer. */
నిర్మాణం brotli_writer {
  నిర్మాణం Curl_cwriter super;
  అక్షరం బఫరు[DECOMPRESS_బఫరు_కొలత];
  BrotliDecoderState *br; /* State structure for brotli. */
};

స్థిర CURLcode brotli_map_error(Brotli_Decoder_Error_సంకేతం be)
{
  ఎంపిక(be) {
  స్థితి BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_ఖండం_నిడివి_1:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_ఖండం_నిడివి_2:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_కిటికీ_బిట్లు:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  స్థితి BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#నిర్వచితమైతే BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  స్థితి BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#అయితే_ముగింపు
  స్థితి BROTLI_DECODER_ERROR_DICTIONARY_NOT_సమితి:
  స్థితి BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  స్థితి BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  స్థితి BROTLI_DECODER_ERROR_ALLOC_వృక్షం_GROUPS:
  స్థితి BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  స్థితి BROTLI_DECODER_ERROR_ALLOC_RING_బఫరు_1:
  స్థితి BROTLI_DECODER_ERROR_ALLOC_RING_బఫరు_2:
  స్థితి BROTLI_DECODER_ERROR_ALLOC_ఖండం_TYPE_TREES:
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి CURLE_WRITE_ERROR;
}

స్థిర CURLcode brotli_do_init(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం brotli_writer *bp = (నిర్మాణం brotli_writer *)writer;
  (శూన్య)దత్తాంశం;

  bp->br = BrotliDecoderCreateInstance(ఖాళీ, ఖాళీ, ఖాళీ);
  తిరిగి bp->br ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

స్థిర CURLcode brotli_do_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  నిర్మాణం brotli_writer *bp = (నిర్మాణం brotli_writer *)writer;
  స్థిరరాశి uint8_t *src = (స్థిరరాశి uint8_t *)buf;
  uint8_t *dst;
  size_t dstleft;
  CURLcode ఫలితం = CURLE_OK;
  Brotli_Decoder_ఫలితం r = BROTLI_DECODER_ఫలితం_NEEDS_మరిన్ని_OUTPUT;
  పూర్ణాంకం i = 0;

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || !nbytes)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);

  ఒకవేళ(!bp->br)
    తిరిగి CURLE_WRITE_ERROR; /* Stream already ended. */

  అయితే((nbytes || r == BROTLI_DECODER_ఫలితం_NEEDS_మరిన్ని_OUTPUT) &&
        ఫలితం == CURLE_OK) {

    ఒకవేళ(++i > (1024 * 1024 / DECOMPRESS_బఫరు_కొలత)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
        failf(దత్తాంశం, "Operation timed out while decoding payload");
        తిరిగి CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->బఫరు;
    dstleft = DECOMPRESS_బఫరు_కొలత;
    r = Brotli_Decoder_Decompress_ప్రవాహం(bp->br,
                                      &nbytes, &src, &dstleft, &dst, ఖాళీ);
    ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type,
                                bp->బఫరు, DECOMPRESS_బఫరు_కొలత - dstleft);
    ఒకవేళ(ఫలితం)
      ఆపు;
    ఎంపిక(r) {
    స్థితి BROTLI_DECODER_ఫలితం_NEEDS_మరిన్ని_OUTPUT:
    స్థితి BROTLI_DECODER_ఫలితం_NEEDS_మరిన్ని_ఆదానం:
      ఆపు;
    స్థితి BROTLI_DECODER_ఫలితం_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = ఖాళీ;
      ఒకవేళ(nbytes)
        ఫలితం = CURLE_WRITE_ERROR;
      ఆపు;
    అప్రమేయ:
      ఫలితం = brotli_map_error(Brotli_Decoder_Get_Error_సంకేతం(bp->br));
      ఆపు;
    }
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య brotli_do_close(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం brotli_writer *bp = (నిర్మాణం brotli_writer *)writer;
  (శూన్య)దత్తాంశం;

  ఒకవేళ(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = ఖాళీ;
  }
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype brotli_encoding = {
  "br",
  ఖాళీ,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  పరిమాణం(నిర్మాణం brotli_writer)
};
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ZSTD
/* Zstd writer. */
నిర్మాణం zstd_writer {
  నిర్మాణం Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  అక్షరం బఫరు[DECOMPRESS_బఫరు_కొలత];
};

#నిర్వచితమైతే ZSTD_STATIC_LINKING_ONLY
స్థిర శూన్య *Curl_zstd_alloc(శూన్య *opaque, size_t కొలత)
{
  (శూన్య)opaque;
  తిరిగి Curl_cmalloc(కొలత);
}

స్థిర శూన్య Curl_zstd_free(శూన్య *opaque, శూన్య *చిరునామా)
{
  (శూన్య)opaque;
  Curl_cfree(చిరునామా);
}
#అయితే_ముగింపు

స్థిర CURLcode zstd_do_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zstd_writer *zp = (నిర్మాణం zstd_writer *)writer;

  (శూన్య)దత్తాంశం;

#నిర్వచితమైతే ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = ఖాళీ
  });
#లేకపోతే
  zp->zds = ZSTD_createDStream();
#అయితే_ముగింపు

  తిరిగి zp->zds ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

స్థిర CURLcode zstd_do_write(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                              స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం zstd_writer *zp = (నిర్మాణం zstd_writer *)writer;
  ZSTD_లోపల_బఫరు లోపల;
  ZSTD_బహిర్గతం_బఫరు బహిర్గతం;
  size_t error_సంకేతం_2;
  పూర్ణాంకం i = 0;

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || !nbytes)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);

  లోపల.pos = 0;
  లోపల.src = buf;
  లోపల.కొలత = nbytes;

  కొరకు(;;) {
    ఒకవేళ(++i > (1024 * 1024 / DECOMPRESS_బఫరు_కొలత)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
        failf(దత్తాంశం, "Operation timed out while decoding payload");
        తిరిగి CURLE_OPERATION_TIMEDOUT;
      }
    }

    బహిర్గతం.pos = 0;
    బహిర్గతం.dst = zp->బఫరు;
    బహిర్గతం.కొలత = DECOMPRESS_బఫరు_కొలత;

    error_సంకేతం_2 = ZSTD_decompress_ప్రవాహం(zp->zds, &బహిర్గతం, &లోపల);
    ఒకవేళ(ZSTD_isError(error_సంకేతం_2)) {
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
    }
    ఒకవేళ(బహిర్గతం.pos > 0) {
      ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type,
                                  zp->బఫరు, బహిర్గతం.pos);
      ఒకవేళ(ఫలితం)
        ఆపు;
    }
    ఒకవేళ((లోపల.pos == nbytes) && (బహిర్గతం.pos < బహిర్గతం.కొలత))
      ఆపు;
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య zstd_do_close(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం zstd_writer *zp = (నిర్మాణం zstd_writer *)writer;
  (శూన్య)దత్తాంశం;

  ఒకవేళ(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = ఖాళీ;
  }
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype zstd_encoding = {
  "zstd",
  ఖాళీ,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  పరిమాణం(నిర్మాణం zstd_writer)
};
#అయితే_ముగింపు

/* Identity handler. */
స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype తటస్థం_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  పరిమాణం(నిర్మాణం Curl_cwriter)
};

/* supported general content decoders. */
స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype * స్థిరరాశి general_unencoders[] = {
  &తటస్థం_encoding,
#నిర్వచితమైతే HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_BROTLI
  &brotli_encoding,
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ZSTD
  &zstd_encoding,
#అయితే_ముగింపు
  ఖాళీ
};

/* supported content decoders only for transfer encodings */
స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype * స్థిరరాశి transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  ఖాళీ
};

/* Return the list of comma-separated names of supported encodings.
 */
అక్షరం *Curl_get_content_encodings(శూన్య)
{
  నిర్మాణం dynbuf enc;
  స్థిరరాశి నిర్మాణం Curl_cwtype * స్థిరరాశి *cep;
  CURLcode ఫలితం = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  కొరకు(cep = general_unencoders; *cep && !ఫలితం; cep++) {
    స్థిరరాశి నిర్మాణం Curl_cwtype *ce = *cep;
    ఒకవేళ(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      ఒకవేళ(curlx_dyn_len(&enc))
        ఫలితం = curlx_dyn_addn(&enc, ", ", 2);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_కూడు(&enc, ce->name);
    }
  }
  ఒకవేళ(!ఫలితం && !curlx_dyn_len(&enc))
    ఫలితం = curlx_dyn_కూడు(&enc, CONTENT_ENCODING_DEFAULT);

  ఒకవేళ(!ఫలితం)
    తిరిగి curlx_dyn_ptr(&enc);
  తిరిగి ఖాళీ;
}

/* Deferred error dummy writer. */
స్థిర CURLcode error_do_init(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_cwriter *writer)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)writer;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode error_do_write(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                               స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  (శూన్య)writer;
  (శూన్య)buf;
  (శూన్య)nbytes;

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || !nbytes)
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);
  failf(దత్తాంశం, "Unrecognized content encoding type");
  తిరిగి CURLE_BAD_CONTENT_ENCODING;
}

స్థిర శూన్య error_do_close(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_cwriter *writer)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)writer;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype error_writer = {
  "ce-error",
  ఖాళీ,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  పరిమాణం(నిర్మాణం Curl_cwriter)
};

/* Find the content encoding by name. */
స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype *కనుగొను_unencode_writer(స్థిరరాశి అక్షరం *name,
                                                      size_t len,
                                                      Curl_cwriter_phase phase)
{
  స్థిరరాశి నిర్మాణం Curl_cwtype * స్థిరరాశి *cep;

  ఒకవేళ(phase == CURL_CW_TRANSFER_DECODE) {
    కొరకు(cep = transfer_unencoders; *cep; cep++) {
      స్థిరరాశి నిర్మాణం Curl_cwtype *ce = *cep;
      ఒకవేళ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        తిరిగి ce;
    }
  }
  /* look among the general decoders */
  కొరకు(cep = general_unencoders; *cep; cep++) {
    స్థిరరాశి నిర్మాణం Curl_cwtype *ce = *cep;
    ఒకవేళ((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      తిరిగి ce;
  }
  తిరిగి ఖాళీ;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_దొంతర(నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి అక్షరం *enclist, పూర్ణాంకం is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode ఫలితం;
  bool has_chunked = FALSE;

  చేయి {
    స్థిరరాశి అక్షరం *name;
    size_t namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    అయితే(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    కొరకు(namelen = 0; *enclist && *enclist != ','; enclist++)
      ఒకవేళ(*enclist > ' ')
        namelen = enclist - name + 1;

    ఒకవేళ(namelen) {
      స్థిరరాశి నిర్మాణం Curl_cwtype *cwt;
      నిర్మాణం Curl_cwriter *writer;

      CURL_TRC_WRITE(దత్తాంశం, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (పూర్ణాంకం)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      ఒకవేళ((is_transfer && !దత్తాంశం->సమితి.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && దత్తాంశం->సమితి.http_ce_skip)) {
        bool is_తటస్థం = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(దత్తాంశం, "decoder not requested, ignored: %.*s",
                       (పూర్ణాంకం)namelen, name);
        ఒకవేళ(is_transfer && !దత్తాంశం->సమితి.http_te_skip) {
          ఒకవేళ(has_chunked)
            failf(దత్తాంశం, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (పూర్ణాంకం)namelen, name);
          లేకపోతే ఒకవేళ(is_తటస్థం)
            కొనసాగు;
          లేకపోతే
            failf(దత్తాంశం, "Unsolicited Transfer-Encoding (%.*s) found",
                  (పూర్ణాంకం)namelen, name);
          తిరిగి CURLE_BAD_CONTENT_ENCODING;
        }
        తిరిగి CURLE_OK;
      }

      ఒకవేళ(Curl_cwriter_లెక్క(దత్తాంశం, phase) >= MAX_ENCODE_దొంతర) {
        failf(దత్తాంశం, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_దొంతర,
              is_transfer ? "transfer" : "content");
        తిరిగి CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = కనుగొను_unencode_writer(name, namelen, phase);
      ఒకవేళ(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(దత్తాంశం, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(దత్తాంశం, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        తిరిగి CURLE_BAD_CONTENT_ENCODING;
      }
      ఒకవేళ(cwt && is_chunked && Curl_cwriter_get_by_type(దత్తాంశం, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(దత్తాంశం, "ignoring duplicate 'chunked' decoder");
      }
      లేకపోతే {
        ఒకవేళ(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        ఫలితం = Curl_cwriter_create(&writer, దత్తాంశం, cwt, phase);
        CURL_TRC_WRITE(దత్తాంశం, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (పూర్ణాంకం)ఫలితం);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        ఫలితం = Curl_cwriter_కూడు(దత్తాంశం, writer);
        ఒకవేళ(ఫలితం) {
          Curl_cwriter_free(దత్తాంశం, writer);
          తిరిగి ఫలితం;
        }
      }
      ఒకవేళ(is_chunked)
        has_chunked = TRUE;
    }
  } అయితే(*enclist);

  తిరిగి CURLE_OK;
}

#లేకపోతే
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_దొంతర(నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి అక్షరం *enclist, పూర్ణాంకం is_transfer)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)enclist;
  (శూన్య)is_transfer;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

అక్షరం *Curl_get_content_encodings(శూన్య)
{
  తిరిగి curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#అయితే_ముగింపు /* CURL_DISABLE_HTTP */
