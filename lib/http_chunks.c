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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

#ಅನ್ತರ್ಭಾವ "urldata.h" /* it includes http_chunks.h */
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "http.h"    /* for Curl_verify_header */
#ಅನ್ತರ್ಭಾವ "sendf.h"   /* for the client write stuff */
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/*
   Chunk format (simplified):

   <HEX SIZE>[ chunk extension ] CRLF
   <DATA> CRLF

   Highlights from RFC2616 section 3.6 say:

   The chunked encoding modifies the body of a message in order to
   transfer it as a series of chunks, each with its own size indicator,
   followed by an OPTIONAL trailer containing entity-header fields. This
   allows dynamically produced content to be transferred along with the
   information necessary for the recipient to verify that it has
   received the full message.

       Chunked-Body   = *chunk
                        last-chunk
                        trailer
                        CRLF

       chunk          = chunk-size [ chunk-extension ] CRLF
                        chunk-data CRLF
       chunk-size     = 1*HEX
       last-chunk     = 1*("0") [ chunk-extension ] CRLF

       chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
       chunk-ext-name = token
       chunk-ext-val  = token | quoted-string
       chunk-data     = chunk-size(OCTET)
       trailer        = *(entity-header CRLF)

   The chunk-size field is a string of hex digits indicating the size of
   the chunk. The chunked encoding is ended by any chunk whose size is
   zero, followed by the trailer, which is terminated by an empty line.

 */

ಶೂನ್ಯ Curl_httpchunk_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_chunker *ch,
                         bool ignore_body, bool ಒಳ_connect)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->ಒಳ_connect = ಒಳ_connect;
}

ಶೂನ್ಯ Curl_httpchunk_reset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_chunker *ch,
                          bool ignore_body)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

ಶೂನ್ಯ Curl_httpchunk_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_chunker *ch)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_chunker *ch)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ch->state == CHUNK_ಮುಗಿದ;
}

ಸ್ಥಿರ CURLcode httpchunk_readwrite(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_chunker *ch,
                                    ರಚನೆ Curl_cwriter *cw_ಮುಂದಿನ,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  ಆದರೆ(ch->state == CHUNK_ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(ch->state == CHUNK_FAILED)
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.http_te_skip && !ch->ignore_body) {
    ಆದರೆ(cw_ಮುಂದಿನ)
      ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, cw_ಮುಂದಿನ, CLIENTWRITE_BODY, buf, blen);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, buf, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ch->state = CHUNK_FAILED;
      ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_PASSTHRU_ERROR;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆಗ(blen) {
    ಆಯ್ಕೆ(ch->state) {
    ಸ್ಥಿತಿ CHUNK_HEX:
      ಆದರೆ(ISXDIGIT(*buf)) {
        ಆದರೆ(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(ದತ್ತಾಂಶ, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಆದರೆ(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(ದತ್ತಾಂಶ, "chunk hex-length char not a hex digit: 0x%x",
                (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)*buf);
          ch->state = CHUNK_FAILED;
          ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_ILLEGAL_HEX;
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        ಆದರೆ(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(ದತ್ತಾಂಶ, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_ILLEGAL_HEX;
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      ಆದರೆ(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        ಆದರೆ(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ch->state = CHUNK_ದತ್ತಾಂಶ;
          CURL_TRC_WRITE(ದತ್ತಾಂಶ, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_ದತ್ತಾಂಶ:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      ಆದರೆ(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.http_te_skip && !ch->ignore_body) {
        ಆದರೆ(cw_ಮುಂದಿನ)
          ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, cw_ಮುಂದಿನ, CLIENTWRITE_BODY,
                                      buf, piece);
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, buf, piece);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          ch->state = CHUNK_FAILED;
          ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_PASSTHRU_ERROR;
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(ದತ್ತಾಂಶ, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      ಆದರೆ(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_POSTLF:
      ಆದರೆ(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(ದತ್ತಾಂಶ, ch, (bool)ch->ignore_body);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_BAD_CHUNK;
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_TRAILER:
      ಆದರೆ((*buf == 0x0d) || (*buf == 0x0a)) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        ಆದರೆ(tr) {
          size_t trlen;
          ಫಲಿತಾಂಶ = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          ಆದರೆ(ಫಲಿತಾಂಶ) {
            ch->state = CHUNK_FAILED;
            ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          ಫಲಿತಾಂಶ = Curl_verify_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, tr, trlen);
          ಆದರೆ(ಫಲಿತಾಂಶ) {
            ch->state = CHUNK_FAILED;
            ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_BAD_CHUNK;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          }

          ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.http_te_skip) {
            ಪೂರ್ಣಾಂಕ hd_type = CLIENTWRITE_ಶೀರ್ಷಿಕೆ | CLIENTWRITE_TRAILER;
            ಆದರೆ(ch->ಒಳ_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            ಆದರೆ(cw_ಮುಂದಿನ)
              ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, cw_ಮುಂದಿನ, hd_type, tr, trlen);
            ಇಲ್ಲದಿದ್ದರೆ
              ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, hd_type, tr, trlen);
            CURL_TRC_WRITE(ದತ್ತಾಂಶ, "wrote trailer '%s'", tr);
            ಆದರೆ(ಫಲಿತಾಂಶ) {
              ch->state = CHUNK_FAILED;
              ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_PASSTHRU_ERROR;
              ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            ಫಲಿತಾಂಶ = Curl_bump_headersize(ದತ್ತಾಂಶ, trlen, FALSE);
            ಆದರೆ(ಫಲಿತಾಂಶ) {
              ch->state = CHUNK_FAILED;
              ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_TRAILER_ಎಲ್ಲೆ;
              ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          ಆದರೆ(*buf == 0x0a)
            /* already on the LF */
            ನಿಲ್ಲಿಸು;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ನಿಲ್ಲಿಸು; /* do not advance the pointer */
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ch->trailer, buf, 1);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          ch->state = CHUNK_FAILED;
          ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_TRAILER_CR:
      ಆದರೆ(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ch->state = CHUNK_FAILED;
        ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_BAD_CHUNK;
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      ಆದರೆ((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CHUNK_STOP:
      ಆದರೆ(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_ಮುಗಿದ;
        CURL_TRC_WRITE(ದತ್ತಾಂಶ, "http_chunk, response complete");
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ch->state = CHUNK_FAILED;
        ch->ಕೊನೆಯ_ಸಂಕೇತ = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(ದತ್ತಾಂಶ, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)*buf);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
    ಸ್ಥಿತಿ CHUNK_ಮುಗಿದ:
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಸ್ಥಿತಿ CHUNK_FAILED:
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_chunked_strerror(CHUNKcode ಸಂಕೇತ_2)
{
  ಆಯ್ಕೆ(ಸಂಕೇತ_2) {
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "OK";
  ಸ್ಥಿತಿ CHUNKE_TOO_LONG_HEX:
    ಹಿಂತಿರುಗಿ "Too long hexadecimal number";
  ಸ್ಥಿತಿ CHUNKE_ILLEGAL_HEX:
    ಹಿಂತಿರುಗಿ "Illegal or missing hexadecimal sequence";
  ಸ್ಥಿತಿ CHUNKE_BAD_CHUNK:
    ಹಿಂತಿರುಗಿ "Malformed encoding found";
  ಸ್ಥಿತಿ CHUNKE_PASSTHRU_ERROR:
    ಹಿಂತಿರುಗಿ "Error writing data to client";
  ಸ್ಥಿತಿ CHUNKE_BAD_ENCODING:
    ಹಿಂತಿರುಗಿ "Bad content-encoding found";
  ಸ್ಥಿತಿ CHUNKE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ "Out of memory";
  ಸ್ಥಿತಿ CHUNKE_TRAILER_ಎಲ್ಲೆ:
    ಹಿಂತಿರುಗಿ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_chunker *ch,
                             ಅಕ್ಷರ *buf, size_t blen,
                             size_t *pconsumed)
{
  ಹಿಂತಿರುಗಿ httpchunk_readwrite(ದತ್ತಾಂಶ, ch, ಖಾಲಿ, buf, blen, pconsumed);
}

ರಚನೆ chunked_writer {
  ರಚನೆ Curl_cwriter super;
  ರಚನೆ Curl_chunker ch;
};

ಸ್ಥಿರ CURLcode cw_chunked_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ chunked_writer *ctx = writer->ctx;

  ದತ್ತಾಂಶ->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(ದತ್ತಾಂಶ, &ctx->ch, FALSE, FALSE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_chunked_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(ದತ್ತಾಂಶ, &ctx->ch);
}

ಸ್ಥಿರ CURLcode cw_chunked_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ chunked_writer *ctx = writer->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  size_t consumed;

  ಆದರೆ(!(type & CLIENTWRITE_BODY))
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, blen);

  consumed = 0;
  ಫಲಿತಾಂಶ = httpchunk_readwrite(ದತ್ತಾಂಶ, &ctx->ch, writer->ಮುಂದಿನ, buf, blen,
                               &consumed);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(CHUNKE_PASSTHRU_ERROR == ctx->ch.ಕೊನೆಯ_ಸಂಕೇತ) {
      failf(ದತ್ತಾಂಶ, "Failed reading the chunked-encoded stream");
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.ಕೊನೆಯ_ಸಂಕೇತ));
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  blen -= consumed;
  ಆದರೆ(CHUNK_ಮುಗಿದ == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
    ಆದರೆ(blen) {
      infof(ದತ್ತಾಂಶ, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((type & CLIENTWRITE_EOS) && !ದತ್ತಾಂಶ->req.no_body) {
    failf(ದತ್ತಾಂಶ, "transfer closed with outstanding read data remaining");
    ಹಿಂತಿರುಗಿ CURLE_PARTIAL_FILE;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ಖಾಲಿ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  ಗಾತ್ರ(ರಚನೆ chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CHUNKED_MINLEN   1024
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CHUNKED_MAXLEN   (64 * 1024)

ರಚನೆ chunked_reader {
  ರಚನೆ Curl_creader super;
  ರಚನೆ bufq chunkbuf;
  ಬಿಟ್(read_eos);  /* we read an EOS from the next reader */
  ಬಿಟ್(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_chunked_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ chunked_reader *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_chunked_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ chunked_reader *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_free(&ctx->chunkbuf);
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_ಕೊನೆಯ_chunk(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ chunked_reader *ctx = reader->ctx;
  ರಚನೆ curl_slist *trailers = ಖಾಲಿ, *tr;
  CURLcode ಫಲಿತಾಂಶ;
  size_t n;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.trailer_callback) {
    CURL_TRC_READ(ದತ್ತಾಂಶ, "http_chunk, added last, empty chunk");
    ಹಿಂತಿರುಗಿ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  {
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_trailer_callback);
    rc = ದತ್ತಾಂಶ->ಗಣ.trailer_callback(&trailers, ದತ್ತಾಂಶ->ಗಣ.trailer_ದತ್ತಾಂಶ);
    CURL_CBAPI_ಕೊನೆ(&guard);
  }

  ಆದರೆ(rc != CURL_TRAILERFUNC_OK) {
    failf(ದತ್ತಾಂಶ, "operation aborted by trailing headers callback");
    ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಗಾಗಿ(tr = trailers; tr; tr = tr->ಮುಂದಿನ) {
    /* only add correctly formatted trailers */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = strchr(tr->ದತ್ತಾಂಶ, ':');
    ಆದರೆ(!ptr || *(ptr + 1) != ' ') {
      infof(ದತ್ತಾಂಶ, "Malformatted trailing header, skipping trailer");
      ಮುಂದುವರಿ;
    }

    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, tr->ದತ್ತಾಂಶ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tr->ದತ್ತಾಂಶ), &n);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

ಹೊರಹೋಗು:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(ದತ್ತಾಂಶ, "http_chunk, added last chunk with trailers "
                "from client -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_chunk(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_creader *reader,
                          ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ chunked_reader *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  ಆದರೆ(blen < ಗಾತ್ರ(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = ಗಾತ್ರ(tmp);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  ಫಲಿತಾಂಶ = Curl_creader_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen, &nread, &eos);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(eos)
    ctx->read_eos = TRUE;

  ಆದರೆ(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    ಅಕ್ಷರ hd[11] = "";
    ಪೂರ್ಣಾಂಕ hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, ಗಾತ್ರ(hd), "%zx\r\n", nread);
    ಆದರೆ(hdlen <= 0)
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(ದತ್ತಾಂಶ, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ctx->read_eos)
    ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಕೊನೆಯ_chunk(ದತ್ತಾಂಶ, reader);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_chunked_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_creader *reader,
                                ಅಕ್ಷರ *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ರಚನೆ chunked_reader *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  ಆದರೆ(!ctx->eos) {
    ಆದರೆ(!ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      ಫಲಿತಾಂಶ = ಕೂಡಿಸು_chunk(ದತ್ತಾಂಶ, reader, buf, blen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->chunkbuf)) {
      ಫಲಿತಾಂಶ = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      ಆದರೆ(!ಫಲಿತಾಂಶ && ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_chunked_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ chunked_reader)
};

CURLcode Curl_httpchunk_ಕೂಡಿಸು_reader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&reader, ದತ್ತಾಂಶ, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಕೂಡಿಸು(ದತ್ತಾಂಶ, reader);

  ಆದರೆ(ಫಲಿತಾಂಶ && reader)
    Curl_creader_free(ದತ್ತಾಂಶ, reader);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */
