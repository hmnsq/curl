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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h" /* it includes http_chunks.h */
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"    /* for Curl_verify_header */
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"   /* for the client write stuff */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

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

ਖਾਲੀ Curl_httpchunk_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_chunker *ch,
                         bool ignore_body, bool ਵਿੱਚ_connect)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->ਵਿੱਚ_connect = ਵਿੱਚ_connect;
}

ਖਾਲੀ Curl_httpchunk_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_chunker *ch,
                          bool ignore_body)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

ਖਾਲੀ Curl_httpchunk_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_chunker *ch)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_chunker *ch)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ch->state == CHUNK_ਪੂਰਾ;
}

ਸਥਿਰ CURLcode httpchunk_readwrite(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_chunker *ch,
                                    ਬਣਤਰ Curl_cwriter *cw_ਅਗਲਾ,
                                    ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  ਜੇ(ch->state == CHUNK_ਪੂਰਾ)
    ਵਾਪਸ CURLE_OK;
  ਜੇ(ch->state == CHUNK_FAILED)
    ਵਾਪਸ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.http_te_skip && !ch->ignore_body) {
    ਜੇ(cw_ਅਗਲਾ)
      ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, cw_ਅਗਲਾ, CLIENTWRITE_BODY, buf, blen);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, buf, blen);
    ਜੇ(ਨਤੀਜਾ) {
      ch->state = CHUNK_FAILED;
      ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_PASSTHRU_ERROR;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜਦੋਂ(blen) {
    ਚੋਣ(ch->state) {
    ਹਾਲਤ CHUNK_HEX:
      ਜੇ(ISXDIGIT(*buf)) {
        ਜੇ(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(ਅੰਕੜੇ, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ਨਹੀਂਤਾਂ {
        ਅਚਲ ਅੱਖਰ *p;
        ਜੇ(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(ਅੰਕੜੇ, "chunk hex-length char not a hex digit: 0x%x",
                (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)*buf);
          ch->state = CHUNK_FAILED;
          ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_ILLEGAL_HEX;
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        ਜੇ(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(ਅੰਕੜੇ, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_ILLEGAL_HEX;
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      ਜੇ(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        ਜੇ(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        ਨਹੀਂਤਾਂ {
          ch->state = CHUNK_ਅੰਕੜੇ;
          CURL_TRC_WRITE(ਅੰਕੜੇ, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_ਅੰਕੜੇ:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      ਜੇ(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.http_te_skip && !ch->ignore_body) {
        ਜੇ(cw_ਅਗਲਾ)
          ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, cw_ਅਗਲਾ, CLIENTWRITE_BODY,
                                      buf, piece);
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, buf, piece);
        ਜੇ(ਨਤੀਜਾ) {
          ch->state = CHUNK_FAILED;
          ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_PASSTHRU_ERROR;
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(ਅੰਕੜੇ, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      ਜੇ(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_POSTLF:
      ਜੇ(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(ਅੰਕੜੇ, ch, (bool)ch->ignore_body);
      }
      ਨਹੀਂਤਾਂ ਜੇ(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_BAD_CHUNK;
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_TRAILER:
      ਜੇ((*buf == 0x0d) || (*buf == 0x0a)) {
        ਅਚਲ ਅੱਖਰ *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        ਜੇ(tr) {
          size_t trlen;
          ਨਤੀਜਾ = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          ਜੇ(ਨਤੀਜਾ) {
            ch->state = CHUNK_FAILED;
            ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
            ਵਾਪਸ ਨਤੀਜਾ;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          ਨਤੀਜਾ = Curl_verify_ਸਿਰਲੇਖ(ਅੰਕੜੇ, tr, trlen);
          ਜੇ(ਨਤੀਜਾ) {
            ch->state = CHUNK_FAILED;
            ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_BAD_CHUNK;
            ਵਾਪਸ ਨਤੀਜਾ;
          }

          ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.http_te_skip) {
            ਪੂਰਨ hd_type = CLIENTWRITE_ਸਿਰਲੇਖ | CLIENTWRITE_TRAILER;
            ਜੇ(ch->ਵਿੱਚ_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            ਜੇ(cw_ਅਗਲਾ)
              ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, cw_ਅਗਲਾ, hd_type, tr, trlen);
            ਨਹੀਂਤਾਂ
              ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, hd_type, tr, trlen);
            CURL_TRC_WRITE(ਅੰਕੜੇ, "wrote trailer '%s'", tr);
            ਜੇ(ਨਤੀਜਾ) {
              ch->state = CHUNK_FAILED;
              ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_PASSTHRU_ERROR;
              ਵਾਪਸ ਨਤੀਜਾ;
            }
            ਨਤੀਜਾ = Curl_bump_headersize(ਅੰਕੜੇ, trlen, FALSE);
            ਜੇ(ਨਤੀਜਾ) {
              ch->state = CHUNK_FAILED;
              ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_TRAILER_ਹੱਦਬੰਦੀ;
              ਵਾਪਸ ਨਤੀਜਾ;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          ਜੇ(*buf == 0x0a)
            /* already on the LF */
            ਰੋਕੋ;
        }
        ਨਹੀਂਤਾਂ {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ਰੋਕੋ; /* do not advance the pointer */
        }
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = curlx_dyn_addn(&ch->trailer, buf, 1);
        ਜੇ(ਨਤੀਜਾ) {
          ch->state = CHUNK_FAILED;
          ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_TRAILER_CR:
      ਜੇ(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ਨਹੀਂਤਾਂ {
        ch->state = CHUNK_FAILED;
        ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_BAD_CHUNK;
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      ਜੇ((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ਰੋਕੋ;
      }
      ਜੇ(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ਰੋਕੋ;

    ਹਾਲਤ CHUNK_STOP:
      ਜੇ(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_ਪੂਰਾ;
        CURL_TRC_WRITE(ਅੰਕੜੇ, "http_chunk, response complete");
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ {
        ch->state = CHUNK_FAILED;
        ch->ਆਖਰੀ_ਕੋਡ = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(ਅੰਕੜੇ, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)*buf);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
    ਹਾਲਤ CHUNK_ਪੂਰਾ:
      ਵਾਪਸ CURLE_OK;

    ਹਾਲਤ CHUNK_FAILED:
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *Curl_chunked_strerror(CHUNKcode ਕੋਡ)
{
  ਚੋਣ(ਕੋਡ) {
  ਮੂਲ:
    ਵਾਪਸ "OK";
  ਹਾਲਤ CHUNKE_TOO_LONG_HEX:
    ਵਾਪਸ "Too long hexadecimal number";
  ਹਾਲਤ CHUNKE_ILLEGAL_HEX:
    ਵਾਪਸ "Illegal or missing hexadecimal sequence";
  ਹਾਲਤ CHUNKE_BAD_CHUNK:
    ਵਾਪਸ "Malformed encoding found";
  ਹਾਲਤ CHUNKE_PASSTHRU_ERROR:
    ਵਾਪਸ "Error writing data to client";
  ਹਾਲਤ CHUNKE_BAD_ENCODING:
    ਵਾਪਸ "Bad content-encoding found";
  ਹਾਲਤ CHUNKE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ "Out of memory";
  ਹਾਲਤ CHUNKE_TRAILER_ਹੱਦਬੰਦੀ:
    ਵਾਪਸ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_chunker *ch,
                             ਅੱਖਰ *buf, size_t blen,
                             size_t *pconsumed)
{
  ਵਾਪਸ httpchunk_readwrite(ਅੰਕੜੇ, ch, ਨਿਰਾ, buf, blen, pconsumed);
}

ਬਣਤਰ chunked_writer {
  ਬਣਤਰ Curl_cwriter super;
  ਬਣਤਰ Curl_chunker ch;
};

ਸਥਿਰ CURLcode cw_chunked_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ chunked_writer *ctx = writer->ctx;

  ਅੰਕੜੇ->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(ਅੰਕੜੇ, &ctx->ch, FALSE, FALSE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cw_chunked_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(ਅੰਕੜੇ, &ctx->ch);
}

ਸਥਿਰ CURLcode cw_chunked_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                 ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ chunked_writer *ctx = writer->ctx;
  CURLcode ਨਤੀਜਾ;
  size_t consumed;

  ਜੇ(!(type & CLIENTWRITE_BODY))
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, blen);

  consumed = 0;
  ਨਤੀਜਾ = httpchunk_readwrite(ਅੰਕੜੇ, &ctx->ch, writer->ਅਗਲਾ, buf, blen,
                               &consumed);

  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(CHUNKE_PASSTHRU_ERROR == ctx->ch.ਆਖਰੀ_ਕੋਡ) {
      failf(ਅੰਕੜੇ, "Failed reading the chunked-encoded stream");
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.ਆਖਰੀ_ਕੋਡ));
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  blen -= consumed;
  ਜੇ(CHUNK_ਪੂਰਾ == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
    ਜੇ(blen) {
      infof(ਅੰਕੜੇ, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ((type & CLIENTWRITE_EOS) && !ਅੰਕੜੇ->req.no_body) {
    failf(ਅੰਕੜੇ, "transfer closed with outstanding read data remaining");
    ਵਾਪਸ CURLE_PARTIAL_FILE;
  }

  ਵਾਪਸ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ਅਚਲ ਬਣਤਰ Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ਨਿਰਾ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  ਆਕਾਰ(ਬਣਤਰ chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CHUNKED_MINLEN   1024
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CHUNKED_MAXLEN   (64 * 1024)

ਬਣਤਰ chunked_reader {
  ਬਣਤਰ Curl_creader super;
  ਬਣਤਰ bufq chunkbuf;
  ਦੋਆਧਾਰੀ_ਅੰਕ(read_eos);  /* we read an EOS from the next reader */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos);       /* we have returned an EOS */
};

ਸਥਿਰ CURLcode cr_chunked_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ chunked_reader *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cr_chunked_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ chunked_reader *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_free(&ctx->chunkbuf);
}

ਸਥਿਰ CURLcode ਜੋੜੋ_ਆਖਰੀ_chunk(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ chunked_reader *ctx = reader->ctx;
  ਬਣਤਰ curl_slist *trailers = ਨਿਰਾ, *tr;
  CURLcode ਨਤੀਜਾ;
  size_t n;
  ਪੂਰਨ rc;

  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.trailer_callback) {
    CURL_TRC_READ(ਅੰਕੜੇ, "http_chunk, added last, empty chunk");
    ਵਾਪਸ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  {
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_trailer_callback);
    rc = ਅੰਕੜੇ->ਸੈੱਟ.trailer_callback(&trailers, ਅੰਕੜੇ->ਸੈੱਟ.trailer_ਅੰਕੜੇ);
    CURL_CBAPI_ਸਿਰਾ(&guard);
  }

  ਜੇ(rc != CURL_TRAILERFUNC_OK) {
    failf(ਅੰਕੜੇ, "operation aborted by trailing headers callback");
    ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਲਈ(tr = trailers; tr; tr = tr->ਅਗਲਾ) {
    /* only add correctly formatted trailers */
    ਅਚਲ ਅੱਖਰ *ptr = strchr(tr->ਅੰਕੜੇ, ':');
    ਜੇ(!ptr || *(ptr + 1) != ' ') {
      infof(ਅੰਕੜੇ, "Malformatted trailing header, skipping trailer");
      ਜਾਰੀ;
    }

    ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, tr->ਅੰਕੜੇ, ਲੜੀ_ਲੰਬਾਈ(tr->ਅੰਕੜੇ), &n);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

ਬਾਹਰਲਾ:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(ਅੰਕੜੇ, "http_chunk, added last chunk with trailers "
                "from client -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ਜੋੜੋ_chunk(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_creader *reader,
                          ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ chunked_reader *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  ਜੇ(blen < ਆਕਾਰ(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = ਆਕਾਰ(tmp);
  }
  ਨਹੀਂਤਾਂ {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  ਨਤੀਜਾ = Curl_creader_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen, &nread, &eos);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(eos)
    ctx->read_eos = TRUE;

  ਜੇ(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    ਅੱਖਰ hd[11] = "";
    ਪੂਰਨ hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, ਆਕਾਰ(hd), "%zx\r\n", nread);
    ਜੇ(hdlen <= 0)
      ਵਾਪਸ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(ਅੰਕੜੇ, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ctx->read_eos)
    ਵਾਪਸ ਜੋੜੋ_ਆਖਰੀ_chunk(ਅੰਕੜੇ, reader);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cr_chunked_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader,
                                ਅੱਖਰ *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ਬਣਤਰ chunked_reader *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  ਜੇ(!ctx->eos) {
    ਜੇ(!ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      ਨਤੀਜਾ = ਜੋੜੋ_chunk(ਅੰਕੜੇ, reader, buf, blen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->chunkbuf)) {
      ਨਤੀਜਾ = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      ਜੇ(!ਨਤੀਜਾ && ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ curl_off_t cr_chunked_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ਅਚਲ ਬਣਤਰ Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ chunked_reader)
};

CURLcode Curl_httpchunk_ਜੋੜੋ_reader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&reader, ਅੰਕੜੇ, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਜੋੜੋ(ਅੰਕੜੇ, reader);

  ਜੇ(ਨਤੀਜਾ && reader)
    Curl_creader_free(ਅੰਕੜੇ, reader);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */
