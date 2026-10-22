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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#അന്തര്ഭാവ "urldata.h" /* it includes http_chunks.h */
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "http.h"    /* for Curl_verify_header */
#അന്തര്ഭാവ "sendf.h"   /* for the client write stuff */
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curlx/strparse.h"

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

ശൂന്യം Curl_httpchunk_init(ഘടന Curl_easy *data, ഘടന Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (ശൂന്യം)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

ശൂന്യം Curl_httpchunk_reset(ഘടന Curl_easy *data, ഘടന Curl_chunker *ch,
                          bool ignore_body)
{
  (ശൂന്യം)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

ശൂന്യം Curl_httpchunk_free(ഘടന Curl_easy *data, ഘടന Curl_chunker *ch)
{
  (ശൂന്യം)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(ഘടന Curl_easy *data, ഘടന Curl_chunker *ch)
{
  (ശൂന്യം)data;
  തിരികെ ch->state == CHUNK_DONE;
}

സ്ഥിതം CURLcode httpchunk_readwrite(ഘടന Curl_easy *data,
                                    ഘടന Curl_chunker *ch,
                                    ഘടന Curl_cwriter *cw_next,
                                    സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  എങ്കിൽ(ch->state == CHUNK_DONE)
    തിരികെ CURLE_OK;
  എങ്കിൽ(ch->state == CHUNK_FAILED)
    തിരികെ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  എങ്കിൽ(data->set.http_te_skip && !ch->ignore_body) {
    എങ്കിൽ(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    അല്ലെങ്കിൽ
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    എങ്കിൽ(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      തിരികെ result;
    }
  }

  ആകുമ്പോൾ(blen) {
    തിരഞ്ഞെടുപ്പ്(ch->state) {
    അവസ്ഥ CHUNK_HEX:
      എങ്കിൽ(ISXDIGIT(*buf)) {
        എങ്കിൽ(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          തിരികെ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      അല്ലെങ്കിൽ {
        സ്ഥിരം അക്ഷരം *p;
        എങ്കിൽ(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned പൂർണ്ണസംഖ്യ)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          തിരികെ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        എങ്കിൽ(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          തിരികെ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      നിർത്തുക;

    അവസ്ഥ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      എങ്കിൽ(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        എങ്കിൽ(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        അല്ലെങ്കിൽ {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      നിർത്തുക;

    അവസ്ഥ CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      എങ്കിൽ(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      എങ്കിൽ(!data->set.http_te_skip && !ch->ignore_body) {
        എങ്കിൽ(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        അല്ലെങ്കിൽ
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        എങ്കിൽ(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          തിരികെ result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      എങ്കിൽ(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      നിർത്തുക;

    അവസ്ഥ CHUNK_POSTLF:
      എങ്കിൽ(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        തിരികെ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      നിർത്തുക;

    അവസ്ഥ CHUNK_TRAILER:
      എങ്കിൽ((*buf == 0x0d) || (*buf == 0x0a)) {
        സ്ഥിരം അക്ഷരം *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        എങ്കിൽ(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          എങ്കിൽ(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            തിരികെ result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          എങ്കിൽ(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            തിരികെ result;
          }

          എങ്കിൽ(!data->set.http_te_skip) {
            പൂർണ്ണസംഖ്യ hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            എങ്കിൽ(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            എങ്കിൽ(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            അല്ലെങ്കിൽ
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            എങ്കിൽ(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              തിരികെ result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            എങ്കിൽ(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              തിരികെ result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          എങ്കിൽ(*buf == 0x0a)
            /* already on the LF */
            നിർത്തുക;
        }
        അല്ലെങ്കിൽ {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          നിർത്തുക; /* do not advance the pointer */
        }
      }
      അല്ലെങ്കിൽ {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        എങ്കിൽ(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          തിരികെ result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      നിർത്തുക;

    അവസ്ഥ CHUNK_TRAILER_CR:
      എങ്കിൽ(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      അല്ലെങ്കിൽ {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        തിരികെ CURLE_RECV_ERROR;
      }
      നിർത്തുക;

    അവസ്ഥ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      എങ്കിൽ((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        നിർത്തുക;
      }
      എങ്കിൽ(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      നിർത്തുക;

    അവസ്ഥ CHUNK_STOP:
      എങ്കിൽ(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned പൂർണ്ണസംഖ്യ)*buf);
        തിരികെ CURLE_RECV_ERROR;
      }
    അവസ്ഥ CHUNK_DONE:
      തിരികെ CURLE_OK;

    അവസ്ഥ CHUNK_FAILED:
      തിരികെ CURLE_RECV_ERROR;
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *Curl_chunked_strerror(CHUNKcode code)
{
  തിരഞ്ഞെടുപ്പ്(code) {
  സ്ഥിരസ്ഥിതി:
    തിരികെ "OK";
  അവസ്ഥ CHUNKE_TOO_LONG_HEX:
    തിരികെ "Too long hexadecimal number";
  അവസ്ഥ CHUNKE_ILLEGAL_HEX:
    തിരികെ "Illegal or missing hexadecimal sequence";
  അവസ്ഥ CHUNKE_BAD_CHUNK:
    തിരികെ "Malformed encoding found";
  അവസ്ഥ CHUNKE_PASSTHRU_ERROR:
    തിരികെ "Error writing data to client";
  അവസ്ഥ CHUNKE_BAD_ENCODING:
    തിരികെ "Bad content-encoding found";
  അവസ്ഥ CHUNKE_OUT_OF_MEMORY:
    തിരികെ "Out of memory";
  അവസ്ഥ CHUNKE_TRAILER_LIMIT:
    തിരികെ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ഘടന Curl_easy *data,
                             ഘടന Curl_chunker *ch,
                             അക്ഷരം *buf, size_t blen,
                             size_t *pconsumed)
{
  തിരികെ httpchunk_readwrite(data, ch, ശൂന്യ, buf, blen, pconsumed);
}

ഘടന chunked_writer {
  ഘടന Curl_cwriter super;
  ഘടന Curl_chunker ch;
};

സ്ഥിതം CURLcode cw_chunked_init(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer)
{
  ഘടന chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cw_chunked_close(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer)
{
  ഘടന chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

സ്ഥിതം CURLcode cw_chunked_write(ഘടന Curl_easy *data,
                                 ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                 സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  ഘടന chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  എങ്കിൽ(!(type & CLIENTWRITE_BODY))
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  എങ്കിൽ(result) {
    എങ്കിൽ(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    അല്ലെങ്കിൽ {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    തിരികെ result;
  }

  blen -= consumed;
  എങ്കിൽ(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    എങ്കിൽ(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    തിരികെ CURLE_PARTIAL_FILE;
  }

  തിരികെ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
സ്ഥിരം ഘടന Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ശൂന്യ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  വലുപ്പം(ഘടന chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

ഘടന chunked_reader {
  ഘടന Curl_creader super;
  ഘടന bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

സ്ഥിതം CURLcode cr_chunked_init(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader)
{
  ഘടന chunked_reader *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cr_chunked_close(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader)
{
  ഘടന chunked_reader *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

സ്ഥിതം CURLcode add_last_chunk(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader)
{
  ഘടന chunked_reader *ctx = reader->ctx;
  ഘടന curl_slist *trailers = ശൂന്യ, *tr;
  CURLcode result;
  size_t n;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    തിരികെ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  എങ്കിൽ(result)
    പോകുക out;

  {
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  എങ്കിൽ(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    പോകുക out;
  }

  വേണ്ടി(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    സ്ഥിരം അക്ഷരം *ptr = strchr(tr->data, ':');
    എങ്കിൽ(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      തുടരുക;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    എങ്കിൽ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    എങ്കിൽ(result)
      പോകുക out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

സ്ഥിതം CURLcode add_chunk(ഘടന Curl_easy *data,
                          ഘടന Curl_creader *reader,
                          അക്ഷരം *buf, size_t blen)
{
  ഘടന chunked_reader *ctx = reader->ctx;
  CURLcode result;
  അക്ഷരം tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  എങ്കിൽ(blen < വലുപ്പം(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = വലുപ്പം(tmp);
  }
  അല്ലെങ്കിൽ {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(eos)
    ctx->read_eos = TRUE;

  എങ്കിൽ(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    അക്ഷരം hd[11] = "";
    പൂർണ്ണസംഖ്യ hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, വലുപ്പം(hd), "%zx\r\n", nread);
    എങ്കിൽ(hdlen <= 0)
      തിരികെ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    എങ്കിൽ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    എങ്കിൽ(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(ctx->read_eos)
    തിരികെ add_last_chunk(data, reader);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cr_chunked_read(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader,
                                അക്ഷരം *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ഘടന chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  എങ്കിൽ(!ctx->eos) {
    എങ്കിൽ(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      എങ്കിൽ(result)
        തിരികെ result;
    }

    എങ്കിൽ(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      എങ്കിൽ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      തിരികെ result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  തിരികെ CURLE_OK;
}

സ്ഥിതം curl_off_t cr_chunked_total_length(ഘടന Curl_easy *data,
                                          ഘടന Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
സ്ഥിരം ഘടന Curl_crtype Curl_httpchunk_encoder = {
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
  വലുപ്പം(ഘടന chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = ശൂന്യ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  എങ്കിൽ(!result)
    result = Curl_creader_add(data, reader);

  എങ്കിൽ(result && reader)
    Curl_creader_free(data, reader);
  തിരികെ result;
}

#endif /* CURL_DISABLE_HTTP */
