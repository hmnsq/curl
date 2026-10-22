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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#ଅନ୍ତର୍ଭାଵ "urldata.h" /* it includes http_chunks.h */
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "http.h"    /* for Curl_verify_header */
#ଅନ୍ତର୍ଭାଵ "sendf.h"   /* for the client write stuff */
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

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

ଶୂନ୍ୟ Curl_httpchunk_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (ଶୂନ୍ୟ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

ଶୂନ୍ୟ Curl_httpchunk_reset(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_chunker *ch,
                          bool ignore_body)
{
  (ଶୂନ୍ୟ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

ଶୂନ୍ୟ Curl_httpchunk_free(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_chunker *ch)
{
  (ଶୂନ୍ୟ)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_chunker *ch)
{
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ch->state == CHUNK_DONE;
}

ସ୍ଥିର CURLcode httpchunk_readwrite(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା Curl_chunker *ch,
                                    ସଂରଚନା Curl_cwriter *cw_next,
                                    ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  ଯଦି(ch->state == CHUNK_DONE)
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(ch->state == CHUNK_FAILED)
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  ଯଦି(data->set.http_te_skip && !ch->ignore_body) {
    ଯଦି(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    ଅନ୍ୟଥା
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    ଯଦି(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      ଫେରନ୍ତୁ result;
    }
  }

  ଯେତେବେଳେ(blen) {
    ବାଛ(ch->state) {
    ସ୍ଥିତି CHUNK_HEX:
      ଯଦି(ISXDIGIT(*buf)) {
        ଯଦି(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ଅନ୍ୟଥା {
        ଅଚଳ ଅକ୍ଷର *p;
        ଯଦି(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        ଯଦି(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_LF:
      /* waiting for the LF after a chunk size */
      ଯଦି(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        ଯଦି(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        ଅନ୍ୟଥା {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      ଯଦି(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      ଯଦି(!data->set.http_te_skip && !ch->ignore_body) {
        ଯଦି(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        ଅନ୍ୟଥା
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        ଯଦି(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          ଫେରନ୍ତୁ result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      ଯଦି(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_POSTLF:
      ଯଦି(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      ଅନ୍ୟଥା ଯଦି(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_TRAILER:
      ଯଦି((*buf == 0x0d) || (*buf == 0x0a)) {
        ଅଚଳ ଅକ୍ଷର *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        ଯଦି(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          ଯଦି(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            ଫେରନ୍ତୁ result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          ଯଦି(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            ଫେରନ୍ତୁ result;
          }

          ଯଦି(!data->set.http_te_skip) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            ଯଦି(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            ଯଦି(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            ଅନ୍ୟଥା
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            ଯଦି(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              ଫେରନ୍ତୁ result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            ଯଦି(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              ଫେରନ୍ତୁ result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          ଯଦି(*buf == 0x0a)
            /* already on the LF */
            ଭାଙ୍ଗ;
        }
        ଅନ୍ୟଥା {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ଭାଙ୍ଗ; /* do not advance the pointer */
        }
      }
      ଅନ୍ୟଥା {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        ଯଦି(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          ଫେରନ୍ତୁ result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_TRAILER_CR:
      ଯଦି(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ଅନ୍ୟଥା {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      ଯଦି((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ଭାଙ୍ଗ;
      }
      ଯଦି(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CHUNK_STOP:
      ଯଦି(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*buf);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
    ସ୍ଥିତି CHUNK_DONE:
      ଫେରନ୍ତୁ CURLE_OK;

    ସ୍ଥିତି CHUNK_FAILED:
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *Curl_chunked_strerror(CHUNKcode code)
{
  ବାଛ(code) {
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "OK";
  ସ୍ଥିତି CHUNKE_TOO_LONG_HEX:
    ଫେରନ୍ତୁ "Too long hexadecimal number";
  ସ୍ଥିତି CHUNKE_ILLEGAL_HEX:
    ଫେରନ୍ତୁ "Illegal or missing hexadecimal sequence";
  ସ୍ଥିତି CHUNKE_BAD_CHUNK:
    ଫେରନ୍ତୁ "Malformed encoding found";
  ସ୍ଥିତି CHUNKE_PASSTHRU_ERROR:
    ଫେରନ୍ତୁ "Error writing data to client";
  ସ୍ଥିତି CHUNKE_BAD_ENCODING:
    ଫେରନ୍ତୁ "Bad content-encoding found";
  ସ୍ଥିତି CHUNKE_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ "Out of memory";
  ସ୍ଥିତି CHUNKE_TRAILER_LIMIT:
    ଫେରନ୍ତୁ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_chunker *ch,
                             ଅକ୍ଷର *buf, size_t blen,
                             size_t *pconsumed)
{
  ଫେରନ୍ତୁ httpchunk_readwrite(data, ch, ଖାଲି, buf, blen, pconsumed);
}

ସଂରଚନା chunked_writer {
  ସଂରଚନା Curl_cwriter super;
  ସଂରଚନା Curl_chunker ch;
};

ସ୍ଥିର CURLcode cw_chunked_init(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cw_chunked_close(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

ସ୍ଥିର CURLcode cw_chunked_write(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                 ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  ଯଦି(!(type & CLIENTWRITE_BODY))
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  ଯଦି(result) {
    ଯଦି(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    ଅନ୍ୟଥା {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    ଫେରନ୍ତୁ result;
  }

  blen -= consumed;
  ଯଦି(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    ଯଦି(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  ଅନ୍ୟଥା ଯଦି((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    ଫେରନ୍ତୁ CURLE_PARTIAL_FILE;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ଅଚଳ ସଂରଚନା Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ଖାଲି,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  ଆକାର(ସଂରଚନା chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

ସଂରଚନା chunked_reader {
  ସଂରଚନା Curl_creader super;
  ସଂରଚନା bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ସ୍ଥିର CURLcode cr_chunked_init(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା chunked_reader *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_chunked_close(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା chunked_reader *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

ସ୍ଥିର CURLcode add_last_chunk(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା chunked_reader *ctx = reader->ctx;
  ସଂରଚନା curl_slist *trailers = ଖାଲି, *tr;
  CURLcode result;
  size_t n;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    ଫେରନ୍ତୁ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  ଯଦି(result)
    ଯାଅ out;

  {
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  ଯଦି(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    ଯାଅ out;
  }

  ପାଇଁ(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ଅଚଳ ଅକ୍ଷର *ptr = strchr(tr->data, ':');
    ଯଦି(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      ଜାରି;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    ଯଦି(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    ଯଦି(result)
      ଯାଅ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode add_chunk(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_creader *reader,
                          ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା chunked_reader *ctx = reader->ctx;
  CURLcode result;
  ଅକ୍ଷର tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  ଯଦି(blen < ଆକାର(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = ଆକାର(tmp);
  }
  ଅନ୍ୟଥା {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(eos)
    ctx->read_eos = TRUE;

  ଯଦି(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    ଅକ୍ଷର hd[11] = "";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, ଆକାର(hd), "%zx\r\n", nread);
    ଯଦି(hdlen <= 0)
      ଫେରନ୍ତୁ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    ଯଦି(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    ଯଦି(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(ctx->read_eos)
    ଫେରନ୍ତୁ add_last_chunk(data, reader);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cr_chunked_read(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_creader *reader,
                                ଅକ୍ଷର *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ସଂରଚନା chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  ଯଦି(!ctx->eos) {
    ଯଦି(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }

    ଯଦି(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      ଯଦି(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ଫେରନ୍ତୁ result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର curl_off_t cr_chunked_total_length(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ଅଚଳ ସଂରଚନା Curl_crtype Curl_httpchunk_encoder = {
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
  ଆକାର(ସଂରଚନା chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = ଖାଲି;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  ଯଦି(!result)
    result = Curl_creader_add(data, reader);

  ଯଦି(result && reader)
    Curl_creader_free(data, reader);
  ଫେରନ୍ତୁ result;
}

#endif /* CURL_DISABLE_HTTP */
