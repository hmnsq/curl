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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "curlx/dynbuf.h"

#अगर_घोषित HAVE_LIBZ
#समावेश <zlib.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_BROTLI
#अगर_घोषित CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wvla"
#पूर्ण_अगर
#समावेश <brotli/decode.h>
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित HAVE_ZSTD
#समावेश <zstd.h>
#पूर्ण_अगर

#समावेश "connect.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "content_encoding.h"

#घोषणा CONTENT_ENCODING_DEFAULT  "identity"

#अगर_अघोषित CURL_DISABLE_HTTP

/* allow no more than 5 "chained" compression steps */
#घोषणा MAX_ENCODE_ढेर 5

#अगर defined(HAVE_LIBZ) || defined(HAVE_BROTLI) || defined(HAVE_ZSTD)
#घोषणा DECOMPRESS_बफर_आकार 16384 /* buffer size for decompressed data */
#पूर्ण_अगर

#अगर_घोषित HAVE_LIBZ

#अगर !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1252)
#त्रुटि "zlib 1.2.5.2 or greater required"
#पूर्ण_अगर

प्रकार क्रमागत {
  ZLIB_UNINIT,               /* uninitialized */
  ZLIB_INIT,                 /* initialized */
  ZLIB_INFLATING,            /* inflating started. */
  ZLIB_EXTERNAL_TRAILER,     /* reading external trailer */
  ZLIB_INIT_GZIP             /* initialized in transparent gzip mode */
} zlibInitState;

/* Deflate and gzip writer. */
काष्ठा zlib_writer {
  काष्ठा Curl_cwriter super;
  zlibInitState zlib_init;   /* zlib init state */
  अक्षर बफर[DECOMPRESS_बफर_आकार]; /* Put the decompressed data here. */
  uInt trailerlen;           /* Remaining trailer byte count. */
  z_stream z;                /* State structure for zlib. */
};

अटल voidpf zalloc_cb(voidpf opaque, अचिन्हित पूर्णांक वस्तुएँ, अचिन्हित पूर्णांक आकार)
{
  (व्योम)opaque;
  /* not a typo, keep it curlx_calloc() */
  वापस curlx_calloc(वस्तुएँ, आकार);
}

अटल व्योम zfree_cb(voidpf opaque, voidpf ptr)
{
  (व्योम)opaque;
  curlx_free(ptr);
}

अटल CURLcode process_zlib_error(काष्ठा Curl_easy *तथ्य, z_stream *z)
{
  अगर(z->संदेश)
    failf(तथ्य, "Error while processing content unencoding: %s", z->संदेश);
  अन्यथा
    failf(तथ्य, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  वापस CURLE_BAD_CONTENT_ENCODING;
}

अटल CURLcode exit_zlib(काष्ठा Curl_easy *तथ्य, z_stream *z,
                          zlibInitState *zlib_init, CURLcode परिणाम)
{
  अगर(*zlib_init != ZLIB_UNINIT) {
    अगर(inflateEnd(z) != Z_OK && परिणाम == CURLE_OK)
      परिणाम = process_zlib_error(तथ्य, z);
    *zlib_init = ZLIB_UNINIT;
  }

  वापस परिणाम;
}

अटल CURLcode process_trailer(काष्ठा Curl_easy *तथ्य, काष्ठा zlib_writer *zp)
{
  z_stream *z = &zp->z;
  CURLcode परिणाम = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen ? z->avail_in : zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->अग्र_in += len;
  अगर(z->avail_in)
    परिणाम = CURLE_WRITE_ERROR;
  अगर(परिणाम || !zp->trailerlen)
    परिणाम = exit_zlib(तथ्य, z, &zp->zlib_init, परिणाम);
  अन्यथा {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  वापस परिणाम;
}

अटल CURLcode inflate_stream(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer, पूर्णांक type,
                               zlibInitState started)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z;         /* zlib state structure */
  uInt nread = z->avail_in;
  z_const Bytef *orig_in = z->अग्र_in;
  bool done = FALSE;
  CURLcode परिणाम = CURLE_OK;   /* Curl_client_write status */
  पूर्णांक i = 0;

  /* Check state. */
  अगर(zp->zlib_init != ZLIB_INIT &&
     zp->zlib_init != ZLIB_INFLATING &&
     zp->zlib_init != ZLIB_INIT_GZIP)
    वापस exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_WRITE_ERROR);

  /* because the buffer size is fixed, iteratively decompress and transfer to
     the client via next_write function. */
  जबतक(!done) {
    पूर्णांक status; /* zlib status */
    done = TRUE;

    अगर(++i > (1024 * 1024 / DECOMPRESS_बफर_आकार)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      अगर(Curl_timeleft_ms(तथ्य) < 0) {
        failf(तथ्य, "Operation timed out while decoding payload");
        वापस exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_OPERATION_TIMEDOUT);
      }
    }

    /* (re)set buffer for decompressed output for every iteration */
    z->अग्र_out = (Bytef *)zp->बफर;
    z->avail_out = DECOMPRESS_बफर_आकार;

    status = inflate(z, Z_BLOCK);

    /* Flush output data if some. */
    अगर(z->avail_out != DECOMPRESS_बफर_आकार) {
      अगर(status == Z_OK || status == Z_STREAM_END) {
        zp->zlib_init = started; /* Data started. */
        परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, type, zp->बफर,
                                    DECOMPRESS_बफर_आकार - z->avail_out);
        अगर(परिणाम) {
          exit_zlib(तथ्य, z, &zp->zlib_init, परिणाम);
          अवरोध;
        }
      }
    }

    /* Dispatch by inflate() status. */
    चयन(status) {
    हाल Z_OK:
      /* Always loop: there may be unflushed latched data in zlib state. */
      done = FALSE;
      अवरोध;
    हाल Z_BUF_ERROR:
      /* No more data to flush: exit loop. */
      अवरोध;
    हाल Z_STREAM_END:
      अगर((started == ZLIB_INIT_GZIP) && (z->avail_in >= 2) &&
         (z->अग्र_in[0] == 0x1f) && (z->अग्र_in[1] == 0x8b)) {
        /* a second gzip member follows; curl does not support
           multi-member gzip responses */
        failf(तथ्य, "Multi-member gzip response not supported");
        परिणाम = exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_WRITE_ERROR);
        अवरोध;
      }
      परिणाम = process_trailer(तथ्य, zp);
      अवरोध;
    हाल Z_तथ्य_ERROR:
      /* some servers seem to not generate zlib headers, so this is an attempt
         to fix and continue anyway */
      अगर(zp->zlib_init == ZLIB_INIT) {
        अगर(inflateReset2(z, -MAX_WBITS) == Z_OK) {
          z->अग्र_in = orig_in;
          z->avail_in = nread;
          zp->zlib_init = ZLIB_INFLATING;
          zp->trailerlen = 4; /* Tolerate up to 4 unknown trailer bytes. */
          done = FALSE;
          अवरोध;
        }
        zp->zlib_init = ZLIB_UNINIT; /* inflateEnd() already called. */
      }
      परिणाम = exit_zlib(तथ्य, z, &zp->zlib_init, process_zlib_error(तथ्य, z));
      अवरोध;
    शेष:
      परिणाम = exit_zlib(तथ्य, z, &zp->zlib_init, process_zlib_error(तथ्य, z));
      अवरोध;
    }
  }

  /* We are about to leave this call so the `nread' data bytes will not be seen
     again. If we are in a state that would wrongly allow restart in raw mode
     at the next call, assume output has already started. */
  अगर(nread && zp->zlib_init == ZLIB_INIT)
    zp->zlib_init = started; /* Cannot restart anymore. */

  वापस परिणाम;
}

/* Deflate handler. */
अटल CURLcode deflate_do_init(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  अगर(inflateInit(z) != Z_OK)
    वापस process_zlib_error(तथ्य, z);
  zp->zlib_init = ZLIB_INIT;
  वापस CURLE_OK;
}

अटल CURLcode deflate_do_write(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                 स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  अगर(!(type & CLIENTWRITE_BODY) || !nbytes)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);

  /* Set the compressed input when this function is called */
  z->अग्र_in = (z_const Bytef *)buf;
  z->avail_in = (uInt)nbytes;

  अगर(zp->zlib_init == ZLIB_EXTERNAL_TRAILER)
    वापस process_trailer(तथ्य, zp);

  /* Now uncompress the data */
  वापस inflate_stream(तथ्य, writer, type, ZLIB_INFLATING);
}

अटल व्योम deflate_do_close(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_OK);
}

अटल स्थिर काष्ठा Curl_cwtype deflate_encoding = {
  "deflate",
  शून्य,
  CURL_CW_FLAG_BLOWUP,
  deflate_do_init,
  deflate_do_write,
  Curl_cwriter_def_flush,
  deflate_do_close,
  माप(काष्ठा zlib_writer)
};

/*
 * Gzip handler.
 */

अटल CURLcode gzip_do_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  /* Initialize zlib */
  z->zalloc = (alloc_func)zalloc_cb;
  z->zfree = (free_func)zfree_cb;

  अगर(inflateInit2(z, MAX_WBITS + 32) != Z_OK)
    वापस process_zlib_error(तथ्य, z);

  zp->zlib_init = ZLIB_INIT_GZIP; /* Transparent gzip decompress state */
  वापस CURLE_OK;
}

अटल CURLcode gzip_do_write(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_cwriter *writer, पूर्णांक type,
                              स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  अगर(!(type & CLIENTWRITE_BODY) || !nbytes)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);

  अगर(zp->zlib_init == ZLIB_INIT_GZIP) {
    /* Let zlib handle the gzip decompression entirely */
    z->अग्र_in = (z_const Bytef *)buf;
    z->avail_in = (uInt)nbytes;
    /* Now uncompress the data */
    वापस inflate_stream(तथ्य, writer, type, ZLIB_INIT_GZIP);
  }

  /* We are running with an old version: return error. */
  वापस exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_WRITE_ERROR);
}

अटल व्योम gzip_do_close(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_cwriter *writer)
{
  काष्ठा zlib_writer *zp = (काष्ठा zlib_writer *)writer;
  z_stream *z = &zp->z; /* zlib state structure */

  exit_zlib(तथ्य, z, &zp->zlib_init, CURLE_OK);
}

अटल स्थिर काष्ठा Curl_cwtype gzip_encoding = {
  "gzip",
  "x-gzip",
  CURL_CW_FLAG_BLOWUP,
  gzip_do_init,
  gzip_do_write,
  Curl_cwriter_def_flush,
  gzip_do_close,
  माप(काष्ठा zlib_writer)
};

#पूर्ण_अगर /* HAVE_LIBZ */

#अगर_घोषित HAVE_BROTLI
/* Brotli writer. */
काष्ठा brotli_writer {
  काष्ठा Curl_cwriter super;
  अक्षर बफर[DECOMPRESS_बफर_आकार];
  BrotliDecoderState *br; /* State structure for brotli. */
};

अटल CURLcode brotli_map_error(BrotliDecoderErrorCode be)
{
  चयन(be) {
  हाल BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
  हाल BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
  हाल BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
  हाल BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
  हाल BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
  हाल BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
  हाल BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
  हाल BROTLI_DECODER_ERROR_FORMAT_BLOCK_लंबाई_1:
  हाल BROTLI_DECODER_ERROR_FORMAT_BLOCK_लंबाई_2:
  हाल BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
  हाल BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
  हाल BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
  हाल BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
  हाल BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
#अगर_घोषित BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY  /* brotli v1.1.0+ */
  हाल BROTLI_DECODER_ERROR_COMPOUND_DICTIONARY:
#पूर्ण_अगर
  हाल BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET:
  हाल BROTLI_DECODER_ERROR_INVALID_ARGUMENTS:
    वापस CURLE_BAD_CONTENT_ENCODING;
  हाल BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
  हाल BROTLI_DECODER_ERROR_ALLOC_वृक्ष_GROUPS:
  हाल BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
  हाल BROTLI_DECODER_ERROR_ALLOC_RING_बफर_1:
  हाल BROTLI_DECODER_ERROR_ALLOC_RING_बफर_2:
  हाल BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
    वापस CURLE_OUT_OF_MEMORY;
  शेष:
    अवरोध;
  }
  वापस CURLE_WRITE_ERROR;
}

अटल CURLcode brotli_do_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer)
{
  काष्ठा brotli_writer *bp = (काष्ठा brotli_writer *)writer;
  (व्योम)तथ्य;

  bp->br = BrotliDecoderCreateInstance(शून्य, शून्य, शून्य);
  वापस bp->br ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

अटल CURLcode brotli_do_write(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  काष्ठा brotli_writer *bp = (काष्ठा brotli_writer *)writer;
  स्थिर uint8_t *src = (स्थिर uint8_t *)buf;
  uint8_t *dst;
  माप_प्रकार dstleft;
  CURLcode परिणाम = CURLE_OK;
  Brotli_Decoder_परिणाम r = BROTLI_DECODER_परिणाम_NEEDS_MORE_OUTPUT;
  पूर्णांक i = 0;

  अगर(!(type & CLIENTWRITE_BODY) || !nbytes)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);

  अगर(!bp->br)
    वापस CURLE_WRITE_ERROR; /* Stream already ended. */

  जबतक((nbytes || r == BROTLI_DECODER_परिणाम_NEEDS_MORE_OUTPUT) &&
        परिणाम == CURLE_OK) {

    अगर(++i > (1024 * 1024 / DECOMPRESS_बफर_आकार)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      अगर(Curl_timeleft_ms(तथ्य) < 0) {
        failf(तथ्य, "Operation timed out while decoding payload");
        वापस CURLE_OPERATION_TIMEDOUT;
      }
    }

    dst = (uint8_t *)bp->बफर;
    dstleft = DECOMPRESS_बफर_आकार;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, शून्य);
    परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, type,
                                bp->बफर, DECOMPRESS_बफर_आकार - dstleft);
    अगर(परिणाम)
      अवरोध;
    चयन(r) {
    हाल BROTLI_DECODER_परिणाम_NEEDS_MORE_OUTPUT:
    हाल BROTLI_DECODER_परिणाम_NEEDS_MORE_निवेश:
      अवरोध;
    हाल BROTLI_DECODER_परिणाम_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = शून्य;
      अगर(nbytes)
        परिणाम = CURLE_WRITE_ERROR;
      अवरोध;
    शेष:
      परिणाम = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      अवरोध;
    }
  }
  वापस परिणाम;
}

अटल व्योम brotli_do_close(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer)
{
  काष्ठा brotli_writer *bp = (काष्ठा brotli_writer *)writer;
  (व्योम)तथ्य;

  अगर(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = शून्य;
  }
}

अटल स्थिर काष्ठा Curl_cwtype brotli_encoding = {
  "br",
  शून्य,
  CURL_CW_FLAG_BLOWUP,
  brotli_do_init,
  brotli_do_write,
  Curl_cwriter_def_flush,
  brotli_do_close,
  माप(काष्ठा brotli_writer)
};
#पूर्ण_अगर

#अगर_घोषित HAVE_ZSTD
/* Zstd writer. */
काष्ठा zstd_writer {
  काष्ठा Curl_cwriter super;
  ZSTD_DStream *zds; /* State structure for zstd. */
  अक्षर बफर[DECOMPRESS_बफर_आकार];
};

#अगर_घोषित ZSTD_STATIC_LINKING_ONLY
अटल व्योम *Curl_zstd_alloc(व्योम *opaque, माप_प्रकार आकार)
{
  (व्योम)opaque;
  वापस Curl_cmalloc(आकार);
}

अटल व्योम Curl_zstd_free(व्योम *opaque, व्योम *address)
{
  (व्योम)opaque;
  Curl_cfree(address);
}
#पूर्ण_अगर

अटल CURLcode zstd_do_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer)
{
  काष्ठा zstd_writer *zp = (काष्ठा zstd_writer *)writer;

  (व्योम)तथ्य;

#अगर_घोषित ZSTD_STATIC_LINKING_ONLY
  zp->zds = ZSTD_createDStream_advanced((ZSTD_customMem) {
    .customAlloc = Curl_zstd_alloc,
    .customFree  = Curl_zstd_free,
    .opaque      = शून्य
  });
#अन्यथा
  zp->zds = ZSTD_createDStream();
#पूर्ण_अगर

  वापस zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

अटल CURLcode zstd_do_write(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_cwriter *writer, पूर्णांक type,
                              स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा zstd_writer *zp = (काष्ठा zstd_writer *)writer;
  ZSTD_in_बफर in;
  ZSTD_out_बफर out;
  माप_प्रकार errorCode;
  पूर्णांक i = 0;

  अगर(!(type & CLIENTWRITE_BODY) || !nbytes)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);

  in.pos = 0;
  in.src = buf;
  in.आकार = nbytes;

  क्रम(;;) {
    अगर(++i > (1024 * 1024 / DECOMPRESS_बफर_आकार)) {
      /* check every MB of output if we are not exceeding time limit */
      i = 0;
      अगर(Curl_timeleft_ms(तथ्य) < 0) {
        failf(तथ्य, "Operation timed out while decoding payload");
        वापस CURLE_OPERATION_TIMEDOUT;
      }
    }

    out.pos = 0;
    out.dst = zp->बफर;
    out.आकार = DECOMPRESS_बफर_आकार;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    अगर(ZSTD_isError(errorCode)) {
      वापस CURLE_BAD_CONTENT_ENCODING;
    }
    अगर(out.pos > 0) {
      परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, type,
                                  zp->बफर, out.pos);
      अगर(परिणाम)
        अवरोध;
    }
    अगर((in.pos == nbytes) && (out.pos < out.आकार))
      अवरोध;
  }

  वापस परिणाम;
}

अटल व्योम zstd_do_close(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_cwriter *writer)
{
  काष्ठा zstd_writer *zp = (काष्ठा zstd_writer *)writer;
  (व्योम)तथ्य;

  अगर(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = शून्य;
  }
}

अटल स्थिर काष्ठा Curl_cwtype zstd_encoding = {
  "zstd",
  शून्य,
  CURL_CW_FLAG_BLOWUP,
  zstd_do_init,
  zstd_do_write,
  Curl_cwriter_def_flush,
  zstd_do_close,
  माप(काष्ठा zstd_writer)
};
#पूर्ण_अगर

/* Identity handler. */
अटल स्थिर काष्ठा Curl_cwtype identity_encoding = {
  "identity",
  "none",
  0,
  Curl_cwriter_def_init,
  Curl_cwriter_def_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  माप(काष्ठा Curl_cwriter)
};

/* supported general content decoders. */
अटल स्थिर काष्ठा Curl_cwtype * स्थिर general_unencoders[] = {
  &identity_encoding,
#अगर_घोषित HAVE_LIBZ
  &deflate_encoding,
  &gzip_encoding,
#पूर्ण_अगर
#अगर_घोषित HAVE_BROTLI
  &brotli_encoding,
#पूर्ण_अगर
#अगर_घोषित HAVE_ZSTD
  &zstd_encoding,
#पूर्ण_अगर
  शून्य
};

/* supported content decoders only for transfer encodings */
अटल स्थिर काष्ठा Curl_cwtype * स्थिर transfer_unencoders[] = {
  &Curl_httpchunk_unencoder,
  शून्य
};

/* Return the list of comma-separated names of supported encodings.
 */
अक्षर *Curl_get_content_encodings(व्योम)
{
  काष्ठा dynbuf enc;
  स्थिर काष्ठा Curl_cwtype * स्थिर *cep;
  CURLcode परिणाम = CURLE_OK;
  curlx_dyn_init(&enc, 255);

  क्रम(cep = general_unencoders; *cep && !परिणाम; cep++) {
    स्थिर काष्ठा Curl_cwtype *ce = *cep;
    अगर(!curl_strequal(ce->name, CONTENT_ENCODING_DEFAULT)) {
      अगर(curlx_dyn_len(&enc))
        परिणाम = curlx_dyn_addn(&enc, ", ", 2);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_add(&enc, ce->name);
    }
  }
  अगर(!परिणाम && !curlx_dyn_len(&enc))
    परिणाम = curlx_dyn_add(&enc, CONTENT_ENCODING_DEFAULT);

  अगर(!परिणाम)
    वापस curlx_dyn_ptr(&enc);
  वापस शून्य;
}

/* Deferred error dummy writer. */
अटल CURLcode error_do_init(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_cwriter *writer)
{
  (व्योम)तथ्य;
  (व्योम)writer;
  वापस CURLE_OK;
}

अटल CURLcode error_do_write(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer, पूर्णांक type,
                               स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  (व्योम)writer;
  (व्योम)buf;
  (व्योम)nbytes;

  अगर(!(type & CLIENTWRITE_BODY) || !nbytes)
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);
  failf(तथ्य, "Unrecognized content encoding type");
  वापस CURLE_BAD_CONTENT_ENCODING;
}

अटल व्योम error_do_close(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_cwriter *writer)
{
  (व्योम)तथ्य;
  (व्योम)writer;
}

अटल स्थिर काष्ठा Curl_cwtype error_writer = {
  "ce-error",
  शून्य,
  0,
  error_do_init,
  error_do_write,
  Curl_cwriter_def_flush,
  error_do_close,
  माप(काष्ठा Curl_cwriter)
};

/* Find the content encoding by name. */
अटल स्थिर काष्ठा Curl_cwtype *find_unencode_writer(स्थिर अक्षर *name,
                                                      माप_प्रकार len,
                                                      Curl_cwriter_phase phase)
{
  स्थिर काष्ठा Curl_cwtype * स्थिर *cep;

  अगर(phase == CURL_CW_TRANSFER_DECODE) {
    क्रम(cep = transfer_unencoders; *cep; cep++) {
      स्थिर काष्ठा Curl_cwtype *ce = *cep;
      अगर((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
         (ce->alias && curl_strnequal(name, ce->alias, len) &&
          !ce->alias[len]))
        वापस ce;
    }
  }
  /* look among the general decoders */
  क्रम(cep = general_unencoders; *cep; cep++) {
    स्थिर काष्ठा Curl_cwtype *ce = *cep;
    अगर((curl_strnequal(name, ce->name, len) && !ce->name[len]) ||
       (ce->alias && curl_strnequal(name, ce->alias, len) && !ce->alias[len]))
      वापस ce;
  }
  वापस शून्य;
}

/* Setup the unencoding stack from the Content-Encoding header value.
 * See RFC 7231 section 3.1.2.2. */
CURLcode Curl_build_unencoding_ढेर(काष्ठा Curl_easy *तथ्य,
                                     स्थिर अक्षर *enclist, पूर्णांक is_transfer)
{
  Curl_cwriter_phase phase = is_transfer ?
    CURL_CW_TRANSFER_DECODE : CURL_CW_CONTENT_DECODE;
  CURLcode परिणाम;
  bool has_chunked = FALSE;

  करो {
    स्थिर अक्षर *name;
    माप_प्रकार namelen;
    bool is_chunked = FALSE;

    /* Parse a single encoding name. */
    जबतक(ISBLANK(*enclist) || *enclist == ',')
      enclist++;

    name = enclist;

    क्रम(namelen = 0; *enclist && *enclist != ','; enclist++)
      अगर(*enclist > ' ')
        namelen = enclist - name + 1;

    अगर(namelen) {
      स्थिर काष्ठा Curl_cwtype *cwt;
      काष्ठा Curl_cwriter *writer;

      CURL_TRC_WRITE(तथ्य, "looking for %s decoder: %.*s",
                     is_transfer ? "transfer" : "content", (पूर्णांक)namelen, name);
      is_chunked = (is_transfer && (namelen == 7) &&
                    curl_strnequal(name, "chunked", 7));
      /* if we skip the decoding in this phase, do not look further.
       * Exception is "chunked" transfer-encoding which always must happen */
      अगर((is_transfer && !तथ्य->set.http_transfer_encoding && !is_chunked) ||
         (!is_transfer && तथ्य->set.http_ce_skip)) {
        bool is_identity = (namelen == 8) &&
                           curl_strnequal(name, "identity", 8);
        /* not requested, ignore */
        CURL_TRC_WRITE(तथ्य, "decoder not requested, ignored: %.*s",
                       (पूर्णांक)namelen, name);
        अगर(is_transfer && !तथ्य->set.http_te_skip) {
          अगर(has_chunked)
            failf(तथ्य, "A Transfer-Encoding (%.*s) was listed after chunked",
                  (पूर्णांक)namelen, name);
          अन्यथा अगर(is_identity)
            जारी;
          अन्यथा
            failf(तथ्य, "Unsolicited Transfer-Encoding (%.*s) found",
                  (पूर्णांक)namelen, name);
          वापस CURLE_BAD_CONTENT_ENCODING;
        }
        वापस CURLE_OK;
      }

      अगर(Curl_cwriter_गिनती(तथ्य, phase) >= MAX_ENCODE_ढेर) {
        failf(तथ्य, "Reject response exceeding limit of %d %s encodings",
              MAX_ENCODE_ढेर,
              is_transfer ? "transfer" : "content");
        वापस CURLE_BAD_CONTENT_ENCODING;
      }

      cwt = find_unencode_writer(name, namelen, phase);
      अगर(is_transfer && !is_chunked &&
         Curl_cwriter_get_by_name(तथ्य, "chunked")) {
        /* RFC 9112, ch. 6.1:
         * "If any transfer coding other than chunked is applied to a
         *  response's content, the sender MUST either apply chunked as the
         *  final transfer coding or terminate the message by closing the
         *  connection."
         * "chunked" must be the last added to be the first in its phase,
         *  reject this.
         */
        failf(तथ्य, "Reject response due to 'chunked' not being the last "
              "Transfer-Encoding");
        वापस CURLE_BAD_CONTENT_ENCODING;
      }
      अगर(cwt && is_chunked && Curl_cwriter_get_by_type(तथ्य, cwt)) {
        /* A 'chunked' transfer encoding has already been added.
         * Ignore duplicates. See #13451.
         * Also RFC 9112, ch. 6.1:
         * "A sender MUST NOT apply the chunked transfer coding more than
         *  once to a message body."
         */
        CURL_TRC_WRITE(तथ्य, "ignoring duplicate 'chunked' decoder");
      }
      अन्यथा {
        अगर(!cwt)
          cwt = &error_writer; /* Defer error at use. */

        परिणाम = Curl_cwriter_create(&writer, तथ्य, cwt, phase);
        CURL_TRC_WRITE(तथ्य, "added %s decoder %s -> %d",
                       is_transfer ? "transfer" : "content", cwt->name,
                       (पूर्णांक)परिणाम);
        अगर(परिणाम)
          वापस परिणाम;

        परिणाम = Curl_cwriter_add(तथ्य, writer);
        अगर(परिणाम) {
          Curl_cwriter_free(तथ्य, writer);
          वापस परिणाम;
        }
      }
      अगर(is_chunked)
        has_chunked = TRUE;
    }
  } जबतक(*enclist);

  वापस CURLE_OK;
}

#अन्यथा
/* Stubs for builds without HTTP. */
CURLcode Curl_build_unencoding_ढेर(काष्ठा Curl_easy *तथ्य,
                                     स्थिर अक्षर *enclist, पूर्णांक is_transfer)
{
  (व्योम)तथ्य;
  (व्योम)enclist;
  (व्योम)is_transfer;
  वापस CURLE_NOT_BUILT_IN;
}

अक्षर *Curl_get_content_encodings(व्योम)
{
  वापस curlx_strdup(CONTENT_ENCODING_DEFAULT);
}

#पूर्ण_अगर /* CURL_DISABLE_HTTP */
