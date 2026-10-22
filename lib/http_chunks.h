#جے_بنا_تعریف سرخی_CURL_HTTP_CHUNKS_H
#تعریف_کرو سرخی_CURL_HTTP_CHUNKS_H
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

#جے_بنا_تعریف CURL_DISABLE_HTTP

#ضمن "curlx/dynbuf.h"

بنية connectdata;

/*
 * The longest possible hexadecimal number we support in a chunked transfer.
 * Neither RFC2616 nor the later HTTP specs define a maximum chunk size. With
 * the 64-bit curl_off_t we support 16 digits, excluding leading zeroes. But
 * why would a sane server prefix with zeroes? Let's accept one.
 */
#تعریف_کرو CHUNK_MAXNUM_LEN 17

تعريفنوع تعداد {
  /* await and buffer all hexadecimal digits until we get one that is not a
     hexadecimal digit. When done, we go CHUNK_LF */
  CHUNK_HEX,

  /* wait for LF, ignore all else */
  CHUNK_LF,

  /* We eat the amount of data specified. When done, we move on to the
     POST_CR state. */
  CHUNK_انکڑے,

  /* POSTLF should get a CR and then an LF and nothing else, then move back to
     HEX as the CRLF combination marks the end of a chunk. A missing CR is no
     big deal. */
  CHUNK_POSTLF,

  /* Used to mark that we are out of the game. NOTE: that there is a
     'datasize' field in the struct that will tell how many bytes that were
     not passed to the client in the end of the last buffer! */
  CHUNK_STOP,

  /* At this point optional trailer headers can be found, unless the next line
     is CRLF */
  CHUNK_TRAILER,

  /* A trailer CR has been found - next state is CHUNK_TRAILER_POSTCR.
     Next char must be an LF */
  CHUNK_TRAILER_CR,

  /* A trailer LF must be found now, otherwise CHUNKE_BAD_CHUNK will be
     signalled If this is an empty trailer CHUNKE_STOP will be signalled.
     Otherwise the trailer will be broadcasted via Curl_client_write() and the
     next state will be CHUNK_TRAILER */
  CHUNK_TRAILER_POSTCR,

  /* Successfully de-chunked everything */
  CHUNK_پورا,

  /* Failed on seeing a bad or not correctly terminated chunk */
  CHUNK_FAILED
} ChunkyState;

تعريفنوع تعداد {
  CHUNKE_OK = 0,
  CHUNKE_TOO_LONG_HEX = 1,
  CHUNKE_ILLEGAL_HEX,
  CHUNKE_BAD_CHUNK,
  CHUNKE_BAD_ENCODING,
  CHUNKE_باہرلا_دا_حافظہ,
  CHUNKE_PASSTHRU_ERROR, /* Curl_httpchunk_read() returns a CURLcode to use */
  CHUNKE_TRAILER_حد_بندی /* Max header limit exceeded by trailers */
} CHUNKcode;

بنية Curl_chunker {
  curl_off_t datasize;
  ChunkyState state;
  CHUNKcode آخری_کوڈ;
  بنية dynbuf trailer; /* for chunked-encoded trailer */
  بنانشانی حرف hexindex;
  حرف hexbuffer[CHUNK_MAXNUM_LEN + 1]; /* +1 for null-terminator */
  ثنائی_ہندسہ(ignore_body); /* never write response body data */
  ثنائی_ہندسہ(وچ_connect); /* this is a CONNECT response body */
};

/* The following functions are defined in http_chunks.c */
فراغ Curl_httpchunk_init(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch,
                         bool ignore_body, bool وچ_connect);
فراغ Curl_httpchunk_free(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch);
فراغ Curl_httpchunk_reset(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch,
                          bool ignore_body);

/*
 * Read BODY bytes in HTTP/1.1 chunked encoding from `buf` and return
 * the amount of bytes consumed. The actual response bytes and trailer
 * headers are written out to the client.
 * On success, this will consume all bytes up to the end of the response,
 * e.g. the last chunk, has been processed.
 * @param data   the transfer involved
 * @param ch     the chunker instance keeping state across calls
 * @param buf    the response data
 * @param blen   amount of bytes in `buf`
 * @param pconsumed  on successful return, the number of bytes in `buf`
 *                   consumed
 *
 * This function always uses ASCII hex values to accommodate non-ASCII hosts.
 * For example, 0x0d and 0x0a are used instead of '\r' and '\n'.
 */
CURLcode Curl_httpchunk_read(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch,
                             حرف *buf, size_t blen, size_t *pconsumed);

/**
 * @return TRUE iff chunked decoded has finished successfully.
 */
bool Curl_httpchunk_is_پورا(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch);

خارجي ثابت بنية Curl_cwtype Curl_httpchunk_unencoder;

خارجي ثابت بنية Curl_crtype Curl_httpchunk_encoder;

/**
 * Add a transfer-encoding "chunked" reader to the transfers reader stack
 */
CURLcode Curl_httpchunk_جوڑو_reader(بنية Curl_easy *انکڑے);

#جے_انت /* !CURL_DISABLE_HTTP */

#جے_انت /* HEADER_CURL_HTTP_CHUNKS_H */
