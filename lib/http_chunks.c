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

#ifndef CURL_DISABLE_HTTP

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

ಶೂನ್ಯ Curl_httpchunk_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (ಶೂನ್ಯ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

ಶೂನ್ಯ Curl_httpchunk_reset(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_chunker *ch,
                          bool ignore_body)
{
  (ಶೂನ್ಯ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

ಶೂನ್ಯ Curl_httpchunk_free(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_chunker *ch)
{
  (ಶೂನ್ಯ)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_chunker *ch)
{
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ch->state == CHUNK_DONE;
}

ಸ್ಥಿರ CURLcode httpchunk_readwrite(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ Curl_chunker *ch,
                                    ಸಂರಚನಾ Curl_cwriter *cw_next,
                                    ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  ಯದಿ(ch->state == CHUNK_DONE)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(ch->state == CHUNK_FAILED)
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  ಯದಿ(data->set.http_te_skip && !ch->ignore_body) {
    ಯದಿ(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    ಅನ್ಯಥಾ
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    ಯದಿ(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಯಾವತ್(blen) {
    ವಿಕಲ್ಪ(ch->state) {
    ಪಕ್ಷ CHUNK_HEX:
      ಯದಿ(ISXDIGIT(*buf)) {
        ಯದಿ(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ಅನ್ಯಥಾ {
        ಧ್ರುವ ವರ್ಣ *p;
        ಯದಿ(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned ಪೂರ್ಣಾಙ್ಕ)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        ಯದಿ(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      ಯದಿ(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        ಯದಿ(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        ಅನ್ಯಥಾ {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      ಯದಿ(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      ಯದಿ(!data->set.http_te_skip && !ch->ignore_body) {
        ಯದಿ(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        ಅನ್ಯಥಾ
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        ಯದಿ(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      ಯದಿ(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_POSTLF:
      ಯದಿ(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      ಅನ್ಯಥಾ ಯದಿ(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_TRAILER:
      ಯದಿ((*buf == 0x0d) || (*buf == 0x0a)) {
        ಧ್ರುವ ವರ್ಣ *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        ಯದಿ(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          ಯದಿ(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            ಪ್ರತ್ಯಾಗಮ result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          ಯದಿ(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            ಪ್ರತ್ಯಾಗಮ result;
          }

          ಯದಿ(!data->set.http_te_skip) {
            ಪೂರ್ಣಾಙ್ಕ hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            ಯದಿ(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            ಯದಿ(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            ಅನ್ಯಥಾ
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            ಯದಿ(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              ಪ್ರತ್ಯಾಗಮ result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            ಯದಿ(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              ಪ್ರತ್ಯಾಗಮ result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          ಯದಿ(*buf == 0x0a)
            /* already on the LF */
            ಭಙ್ಗ;
        }
        ಅನ್ಯಥಾ {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ಭಙ್ಗ; /* do not advance the pointer */
        }
      }
      ಅನ್ಯಥಾ {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        ಯದಿ(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_TRAILER_CR:
      ಯದಿ(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ಅನ್ಯಥಾ {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      ಯದಿ((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ಭಙ್ಗ;
      }
      ಯದಿ(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ಭಙ್ಗ;

    ಪಕ್ಷ CHUNK_STOP:
      ಯದಿ(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned ಪೂರ್ಣಾಙ್ಕ)*buf);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
    ಪಕ್ಷ CHUNK_DONE:
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    ಪಕ್ಷ CHUNK_FAILED:
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *Curl_chunked_strerror(CHUNKcode code)
{
  ವಿಕಲ್ಪ(code) {
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "OK";
  ಪಕ್ಷ CHUNKE_TOO_LONG_HEX:
    ಪ್ರತ್ಯಾಗಮ "Too long hexadecimal number";
  ಪಕ್ಷ CHUNKE_ILLEGAL_HEX:
    ಪ್ರತ್ಯಾಗಮ "Illegal or missing hexadecimal sequence";
  ಪಕ್ಷ CHUNKE_BAD_CHUNK:
    ಪ್ರತ್ಯಾಗಮ "Malformed encoding found";
  ಪಕ್ಷ CHUNKE_PASSTHRU_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Error writing data to client";
  ಪಕ್ಷ CHUNKE_BAD_ENCODING:
    ಪ್ರತ್ಯಾಗಮ "Bad content-encoding found";
  ಪಕ್ಷ CHUNKE_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ "Out of memory";
  ಪಕ್ಷ CHUNKE_TRAILER_LIMIT:
    ಪ್ರತ್ಯಾಗಮ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_chunker *ch,
                             ವರ್ಣ *buf, size_t blen,
                             size_t *pconsumed)
{
  ಪ್ರತ್ಯಾಗಮ httpchunk_readwrite(data, ch, ರಿಕ್ತ, buf, blen, pconsumed);
}

ಸಂರಚನಾ chunked_writer {
  ಸಂರಚನಾ Curl_cwriter super;
  ಸಂರಚನಾ Curl_chunker ch;
};

ಸ್ಥಿರ CURLcode cw_chunked_init(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cw_chunked_close(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

ಸ್ಥಿರ CURLcode cw_chunked_write(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                 ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  ಯದಿ(!(type & CLIENTWRITE_BODY))
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  ಯದಿ(result) {
    ಯದಿ(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    ಅನ್ಯಥಾ {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }

  blen -= consumed;
  ಯದಿ(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    ಯದಿ(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    ಪ್ರತ್ಯಾಗಮ CURLE_PARTIAL_FILE;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ರಿಕ್ತ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

ಸಂರಚನಾ chunked_reader {
  ಸಂರಚನಾ Curl_creader super;
  ಸಂರಚನಾ bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_chunked_init(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ chunked_reader *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_chunked_close(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ chunked_reader *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

ಸ್ಥಿರ CURLcode add_last_chunk(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ chunked_reader *ctx = reader->ctx;
  ಸಂರಚನಾ curl_slist *trailers = ರಿಕ್ತ, *tr;
  CURLcode result;
  size_t n;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    ಪ್ರತ್ಯಾಗಮ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  ಯದಿ(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ಧ್ರುವ ವರ್ಣ *ptr = strchr(tr->data, ':');
    ಯದಿ(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      ಅನುವರ್ತ;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    ಯದಿ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode add_chunk(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_creader *reader,
                          ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ chunked_reader *ctx = reader->ctx;
  CURLcode result;
  ವರ್ಣ tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  ಯದಿ(blen < ಪರಿಮಾಣ(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = ಪರಿಮಾಣ(tmp);
  }
  ಅನ್ಯಥಾ {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(eos)
    ctx->read_eos = TRUE;

  ಯದಿ(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    ವರ್ಣ hd[11] = "";
    ಪೂರ್ಣಾಙ್ಕ hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, ಪರಿಮಾಣ(hd), "%zx\r\n", nread);
    ಯದಿ(hdlen <= 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    ಯದಿ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    ಯದಿ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(ctx->read_eos)
    ಪ್ರತ್ಯಾಗಮ add_last_chunk(data, reader);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_chunked_read(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_creader *reader,
                                ವರ್ಣ *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  ಯದಿ(!ctx->eos) {
    ಯದಿ(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      ಯದಿ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_chunked_total_length(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ಧ್ರುವ ಸಂರಚನಾ Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  ಪರಿಮಾಣ(ಸಂರಚನಾ chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = ರಿಕ್ತ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  ಯದಿ(!result)
    result = Curl_creader_add(data, reader);

  ಯದಿ(result && reader)
    Curl_creader_free(data, reader);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* CURL_DISABLE_HTTP */
