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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#அந்தர்பாவ "urldata.h" /* it includes http_chunks.h */
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "http.h"    /* for Curl_verify_header */
#அந்தர்பாவ "sendf.h"   /* for the client write stuff */
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curlx/strparse.h"

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

வெறுமை Curl_httpchunk_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (வெறுமை)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

வெறுமை Curl_httpchunk_reset(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_chunker *ch,
                          bool ignore_body)
{
  (வெறுமை)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

வெறுமை Curl_httpchunk_free(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_chunker *ch)
{
  (வெறுமை)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_chunker *ch)
{
  (வெறுமை)data;
  திரும்பு ch->state == CHUNK_DONE;
}

நிலையான CURLcode httpchunk_readwrite(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு Curl_chunker *ch,
                                    கட்டமைப்பு Curl_cwriter *cw_next,
                                    மாறிலி எழுத்து *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  எனில்(ch->state == CHUNK_DONE)
    திரும்பு CURLE_OK;
  எனில்(ch->state == CHUNK_FAILED)
    திரும்பு CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  எனில்(data->set.http_te_skip && !ch->ignore_body) {
    எனில்(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    இல்லையெனில்
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    எனில்(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      திரும்பு result;
    }
  }

  போது(blen) {
    தேர்வு(ch->state) {
    நிலை CHUNK_HEX:
      எனில்(ISXDIGIT(*buf)) {
        எனில்(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          திரும்பு CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      இல்லையெனில் {
        மாறிலி எழுத்து *p;
        எனில்(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned முழு)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          திரும்பு CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        எனில்(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          திரும்பு CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      நிறுத்து;

    நிலை CHUNK_LF:
      /* waiting for the LF after a chunk size */
      எனில்(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        எனில்(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        இல்லையெனில் {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      நிறுத்து;

    நிலை CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      எனில்(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      எனில்(!data->set.http_te_skip && !ch->ignore_body) {
        எனில்(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        இல்லையெனில்
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        எனில்(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          திரும்பு result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      எனில்(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      நிறுத்து;

    நிலை CHUNK_POSTLF:
      எனில்(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      இல்லையெனில் எனில்(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        திரும்பு CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      நிறுத்து;

    நிலை CHUNK_TRAILER:
      எனில்((*buf == 0x0d) || (*buf == 0x0a)) {
        மாறிலி எழுத்து *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        எனில்(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          எனில்(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            திரும்பு result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          எனில்(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            திரும்பு result;
          }

          எனில்(!data->set.http_te_skip) {
            முழு hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            எனில்(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            எனில்(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            இல்லையெனில்
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            எனில்(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              திரும்பு result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            எனில்(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              திரும்பு result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          எனில்(*buf == 0x0a)
            /* already on the LF */
            நிறுத்து;
        }
        இல்லையெனில் {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          நிறுத்து; /* do not advance the pointer */
        }
      }
      இல்லையெனில் {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        எனில்(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          திரும்பு result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      நிறுத்து;

    நிலை CHUNK_TRAILER_CR:
      எனில்(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      இல்லையெனில் {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        திரும்பு CURLE_RECV_ERROR;
      }
      நிறுத்து;

    நிலை CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      எனில்((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        நிறுத்து;
      }
      எனில்(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      நிறுத்து;

    நிலை CHUNK_STOP:
      எனில்(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned முழு)*buf);
        திரும்பு CURLE_RECV_ERROR;
      }
    நிலை CHUNK_DONE:
      திரும்பு CURLE_OK;

    நிலை CHUNK_FAILED:
      திரும்பு CURLE_RECV_ERROR;
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான மாறிலி எழுத்து *Curl_chunked_strerror(CHUNKcode code)
{
  தேர்வு(code) {
  இயல்பு:
    திரும்பு "OK";
  நிலை CHUNKE_TOO_LONG_HEX:
    திரும்பு "Too long hexadecimal number";
  நிலை CHUNKE_ILLEGAL_HEX:
    திரும்பு "Illegal or missing hexadecimal sequence";
  நிலை CHUNKE_BAD_CHUNK:
    திரும்பு "Malformed encoding found";
  நிலை CHUNKE_PASSTHRU_ERROR:
    திரும்பு "Error writing data to client";
  நிலை CHUNKE_BAD_ENCODING:
    திரும்பு "Bad content-encoding found";
  நிலை CHUNKE_OUT_OF_MEMORY:
    திரும்பு "Out of memory";
  நிலை CHUNKE_TRAILER_LIMIT:
    திரும்பு "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_chunker *ch,
                             எழுத்து *buf, size_t blen,
                             size_t *pconsumed)
{
  திரும்பு httpchunk_readwrite(data, ch, வெற்று, buf, blen, pconsumed);
}

கட்டமைப்பு chunked_writer {
  கட்டமைப்பு Curl_cwriter super;
  கட்டமைப்பு Curl_chunker ch;
};

நிலையான CURLcode cw_chunked_init(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cw_chunked_close(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

நிலையான CURLcode cw_chunked_write(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                 மாறிலி எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  எனில்(!(type & CLIENTWRITE_BODY))
    திரும்பு Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  எனில்(result) {
    எனில்(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    இல்லையெனில் {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    திரும்பு result;
  }

  blen -= consumed;
  எனில்(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    எனில்(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  இல்லையெனில் எனில்((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    திரும்பு CURLE_PARTIAL_FILE;
  }

  திரும்பு CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
மாறிலி கட்டமைப்பு Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  வெற்று,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  அளவு(கட்டமைப்பு chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

கட்டமைப்பு chunked_reader {
  கட்டமைப்பு Curl_creader super;
  கட்டமைப்பு bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

நிலையான CURLcode cr_chunked_init(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு chunked_reader *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cr_chunked_close(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு chunked_reader *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

நிலையான CURLcode add_last_chunk(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு chunked_reader *ctx = reader->ctx;
  கட்டமைப்பு curl_slist *trailers = வெற்று, *tr;
  CURLcode result;
  size_t n;
  முழு rc;

  எனில்(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    திரும்பு Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  எனில்(result)
    செல் out;

  {
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  எனில்(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    செல் out;
  }

  க்கு(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    மாறிலி எழுத்து *ptr = strchr(tr->data, ':');
    எனில்(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      தொடர்;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    எனில்(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    எனில்(result)
      செல் out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (முழு)result);
  திரும்பு result;
}

நிலையான CURLcode add_chunk(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_creader *reader,
                          எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு chunked_reader *ctx = reader->ctx;
  CURLcode result;
  எழுத்து tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  எனில்(blen < அளவு(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = அளவு(tmp);
  }
  இல்லையெனில் {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  எனில்(result)
    திரும்பு result;
  எனில்(eos)
    ctx->read_eos = TRUE;

  எனில்(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    எழுத்து hd[11] = "";
    முழு hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, அளவு(hd), "%zx\r\n", nread);
    எனில்(hdlen <= 0)
      திரும்பு CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    எனில்(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    எனில்(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (முழு)result);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(ctx->read_eos)
    திரும்பு add_last_chunk(data, reader);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cr_chunked_read(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_creader *reader,
                                எழுத்து *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  கட்டமைப்பு chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  எனில்(!ctx->eos) {
    எனில்(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      எனில்(result)
        திரும்பு result;
    }

    எனில்(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      எனில்(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      திரும்பு result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  திரும்பு CURLE_OK;
}

நிலையான curl_off_t cr_chunked_total_length(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு -1;
}

/* HTTP chunked Transfer-Encoding encoder */
மாறிலி கட்டமைப்பு Curl_crtype Curl_httpchunk_encoder = {
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
  அளவு(கட்டமைப்பு chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = வெற்று;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  எனில்(!result)
    result = Curl_creader_add(data, reader);

  எனில்(result && reader)
    Curl_creader_free(data, reader);
  திரும்பு result;
}

#endif /* CURL_DISABLE_HTTP */
