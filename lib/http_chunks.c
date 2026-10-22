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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#अन्तर्भाव "urldata.h" /* it includes http_chunks.h */
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "http.h"    /* for Curl_verify_header */
#अन्तर्भाव "sendf.h"   /* for the client write stuff */
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/strparse.h"

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

शून्य Curl_httpchunk_init(संरचना Curl_easy *data, संरचना Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (शून्य)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

शून्य Curl_httpchunk_reset(संरचना Curl_easy *data, संरचना Curl_chunker *ch,
                          bool ignore_body)
{
  (शून्य)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

शून्य Curl_httpchunk_free(संरचना Curl_easy *data, संरचना Curl_chunker *ch)
{
  (शून्य)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(संरचना Curl_easy *data, संरचना Curl_chunker *ch)
{
  (शून्य)data;
  पटिनिवत्त ch->state == CHUNK_DONE;
}

स्थिर CURLcode httpchunk_readwrite(संरचना Curl_easy *data,
                                    संरचना Curl_chunker *ch,
                                    संरचना Curl_cwriter *cw_next,
                                    ध्रुव वर्ण *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  सचे(ch->state == CHUNK_DONE)
    पटिनिवत्त CURLE_OK;
  सचे(ch->state == CHUNK_FAILED)
    पटिनिवत्त CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  सचे(data->set.http_te_skip && !ch->ignore_body) {
    सचे(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    अञ्ञथा
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    सचे(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      पटिनिवत्त result;
    }
  }

  याव(blen) {
    विकल्प(ch->state) {
    पक्ष CHUNK_HEX:
      सचे(ISXDIGIT(*buf)) {
        सचे(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          पटिनिवत्त CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अञ्ञथा {
        ध्रुव वर्ण *p;
        सचे(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned पूर्णाङ्क)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          पटिनिवत्त CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        सचे(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          पटिनिवत्त CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      भङ्ग;

    पक्ष CHUNK_LF:
      /* waiting for the LF after a chunk size */
      सचे(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        सचे(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        अञ्ञथा {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      सचे(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      सचे(!data->set.http_te_skip && !ch->ignore_body) {
        सचे(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        अञ्ञथा
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        सचे(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          पटिनिवत्त result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      सचे(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      भङ्ग;

    पक्ष CHUNK_POSTLF:
      सचे(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      अञ्ञथा सचे(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        पटिनिवत्त CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_TRAILER:
      सचे((*buf == 0x0d) || (*buf == 0x0a)) {
        ध्रुव वर्ण *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        सचे(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          सचे(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            पटिनिवत्त result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          सचे(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            पटिनिवत्त result;
          }

          सचे(!data->set.http_te_skip) {
            पूर्णाङ्क hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            सचे(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            सचे(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            अञ्ञथा
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            सचे(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              पटिनिवत्त result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            सचे(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              पटिनिवत्त result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          सचे(*buf == 0x0a)
            /* already on the LF */
            भङ्ग;
        }
        अञ्ञथा {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          भङ्ग; /* do not advance the pointer */
        }
      }
      अञ्ञथा {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        सचे(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          पटिनिवत्त result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_TRAILER_CR:
      सचे(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अञ्ञथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        पटिनिवत्त CURLE_RECV_ERROR;
      }
      भङ्ग;

    पक्ष CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      सचे((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        भङ्ग;
      }
      सचे(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      भङ्ग;

    पक्ष CHUNK_STOP:
      सचे(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        पटिनिवत्त CURLE_OK;
      }
      अञ्ञथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned पूर्णाङ्क)*buf);
        पटिनिवत्त CURLE_RECV_ERROR;
      }
    पक्ष CHUNK_DONE:
      पटिनिवत्त CURLE_OK;

    पक्ष CHUNK_FAILED:
      पटिनिवत्त CURLE_RECV_ERROR;
    }
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर ध्रुव वर्ण *Curl_chunked_strerror(CHUNKcode code)
{
  विकल्प(code) {
  स्वभाव:
    पटिनिवत्त "OK";
  पक्ष CHUNKE_TOO_LONG_HEX:
    पटिनिवत्त "Too long hexadecimal number";
  पक्ष CHUNKE_ILLEGAL_HEX:
    पटिनिवत्त "Illegal or missing hexadecimal sequence";
  पक्ष CHUNKE_BAD_CHUNK:
    पटिनिवत्त "Malformed encoding found";
  पक्ष CHUNKE_PASSTHRU_ERROR:
    पटिनिवत्त "Error writing data to client";
  पक्ष CHUNKE_BAD_ENCODING:
    पटिनिवत्त "Bad content-encoding found";
  पक्ष CHUNKE_OUT_OF_MEMORY:
    पटिनिवत्त "Out of memory";
  पक्ष CHUNKE_TRAILER_LIMIT:
    पटिनिवत्त "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(संरचना Curl_easy *data,
                             संरचना Curl_chunker *ch,
                             वर्ण *buf, size_t blen,
                             size_t *pconsumed)
{
  पटिनिवत्त httpchunk_readwrite(data, ch, रिक्त, buf, blen, pconsumed);
}

संरचना chunked_writer {
  संरचना Curl_cwriter super;
  संरचना Curl_chunker ch;
};

स्थिर CURLcode cw_chunked_init(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer)
{
  संरचना chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य cw_chunked_close(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

स्थिर CURLcode cw_chunked_write(संरचना Curl_easy *data,
                                 संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                 ध्रुव वर्ण *buf, size_t blen)
{
  संरचना chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  सचे(!(type & CLIENTWRITE_BODY))
    पटिनिवत्त Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  सचे(result) {
    सचे(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    अञ्ञथा {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    पटिनिवत्त result;
  }

  blen -= consumed;
  सचे(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    सचे(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  अञ्ञथा सचे((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    पटिनिवत्त CURLE_PARTIAL_FILE;
  }

  पटिनिवत्त CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ध्रुव संरचना Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  रिक्त,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  परिमाण(संरचना chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

संरचना chunked_reader {
  संरचना Curl_creader super;
  संरचना bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_chunked_init(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य cr_chunked_close(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

स्थिर CURLcode add_last_chunk(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  संरचना curl_slist *trailers = रिक्त, *tr;
  CURLcode result;
  size_t n;
  पूर्णाङ्क rc;

  सचे(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    पटिनिवत्त Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  सचे(result)
    गच्छ out;

  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  सचे(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    गच्छ out;
  }

  अत्थाय(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ध्रुव वर्ण *ptr = strchr(tr->data, ':');
    सचे(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      अनुवर्त;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    सचे(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    सचे(result)
      गच्छ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (पूर्णाङ्क)result);
  पटिनिवत्त result;
}

स्थिर CURLcode add_chunk(संरचना Curl_easy *data,
                          संरचना Curl_creader *reader,
                          वर्ण *buf, size_t blen)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode result;
  वर्ण tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  सचे(blen < परिमाण(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = परिमाण(tmp);
  }
  अञ्ञथा {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  सचे(result)
    पटिनिवत्त result;
  सचे(eos)
    ctx->read_eos = TRUE;

  सचे(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    वर्ण hd[11] = "";
    पूर्णाङ्क hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, परिमाण(hd), "%zx\r\n", nread);
    सचे(hdlen <= 0)
      पटिनिवत्त CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    सचे(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    सचे(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (पूर्णाङ्क)result);
    सचे(result)
      पटिनिवत्त result;
  }

  सचे(ctx->read_eos)
    पटिनिवत्त add_last_chunk(data, reader);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cr_chunked_read(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader,
                                वर्ण *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  सचे(!ctx->eos) {
    सचे(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      सचे(result)
        पटिनिवत्त result;
    }

    सचे(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      सचे(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      पटिनिवत्त result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  पटिनिवत्त CURLE_OK;
}

स्थिर curl_off_t cr_chunked_total_length(संरचना Curl_easy *data,
                                          संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  पटिनिवत्त -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ध्रुव संरचना Curl_crtype Curl_httpchunk_encoder = {
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
  परिमाण(संरचना chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  सचे(!result)
    result = Curl_creader_add(data, reader);

  सचे(result && reader)
    Curl_creader_free(data, reader);
  पटिनिवत्त result;
}

#endif /* CURL_DISABLE_HTTP */
