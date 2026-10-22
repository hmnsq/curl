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

#अगर_अघोषित CURL_DISABLE_HTTP

#समावेश "urldata.h" /* it includes http_chunks.h */
#समावेश "curl_trc.h"
#समावेश "http.h"    /* for Curl_verify_header */
#समावेश "sendf.h"   /* for the client write stuff */
#समावेश "curlx/dynbuf.h"
#समावेश "multiif.h"
#समावेश "curlx/strparse.h"

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

व्योम Curl_httpchunk_init(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (व्योम)तथ्य;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

व्योम Curl_httpchunk_reset(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_chunker *ch,
                          bool ignore_body)
{
  (व्योम)तथ्य;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

व्योम Curl_httpchunk_free(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_chunker *ch)
{
  (व्योम)तथ्य;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_chunker *ch)
{
  (व्योम)तथ्य;
  वापस ch->state == CHUNK_DONE;
}

अटल CURLcode httpchunk_readwrite(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_chunker *ch,
                                    काष्ठा Curl_cwriter *cw_अग्र,
                                    स्थिर अक्षर *buf, माप_प्रकार blen,
                                    माप_प्रकार *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  अगर(ch->state == CHUNK_DONE)
    वापस CURLE_OK;
  अगर(ch->state == CHUNK_FAILED)
    वापस CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  अगर(तथ्य->set.http_te_skip && !ch->ignore_body) {
    अगर(cw_अग्र)
      परिणाम = Curl_cwriter_write(तथ्य, cw_अग्र, CLIENTWRITE_BODY, buf, blen);
    अन्यथा
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, buf, blen);
    अगर(परिणाम) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      वापस परिणाम;
    }
  }

  जबतक(blen) {
    चयन(ch->state) {
    हाल CHUNK_HEX:
      अगर(ISXDIGIT(*buf)) {
        अगर(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(तथ्य, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          वापस CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        स्थिर अक्षर *p;
        अगर(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(तथ्य, "chunk hex-length char not a hex digit: 0x%x",
                (अचिन्हित पूर्णांक)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          वापस CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        अगर(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(तथ्य, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          वापस CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      अवरोध;

    हाल CHUNK_LF:
      /* waiting for the LF after a chunk size */
      अगर(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        अगर(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        अन्यथा {
          ch->state = CHUNK_तथ्य;
          CURL_TRC_WRITE(तथ्य, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      अवरोध;

    हाल CHUNK_तथ्य:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      अगर(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      अगर(!तथ्य->set.http_te_skip && !ch->ignore_body) {
        अगर(cw_अग्र)
          परिणाम = Curl_cwriter_write(तथ्य, cw_अग्र, CLIENTWRITE_BODY,
                                      buf, piece);
        अन्यथा
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, buf, piece);
        अगर(परिणाम) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          वापस परिणाम;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(तथ्य, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      अगर(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      अवरोध;

    हाल CHUNK_POSTLF:
      अगर(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(तथ्य, ch, (bool)ch->ignore_body);
      }
      अन्यथा अगर(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        वापस CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      अवरोध;

    हाल CHUNK_TRAILER:
      अगर((*buf == 0x0d) || (*buf == 0x0a)) {
        स्थिर अक्षर *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        अगर(tr) {
          माप_प्रकार trlen;
          परिणाम = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          अगर(परिणाम) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            वापस परिणाम;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          परिणाम = Curl_verify_header(तथ्य, tr, trlen);
          अगर(परिणाम) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            वापस परिणाम;
          }

          अगर(!तथ्य->set.http_te_skip) {
            पूर्णांक hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            अगर(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            अगर(cw_अग्र)
              परिणाम = Curl_cwriter_write(तथ्य, cw_अग्र, hd_type, tr, trlen);
            अन्यथा
              परिणाम = Curl_client_write(तथ्य, hd_type, tr, trlen);
            CURL_TRC_WRITE(तथ्य, "wrote trailer '%s'", tr);
            अगर(परिणाम) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              वापस परिणाम;
            }
            परिणाम = Curl_bump_headersize(तथ्य, trlen, FALSE);
            अगर(परिणाम) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              वापस परिणाम;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          अगर(*buf == 0x0a)
            /* already on the LF */
            अवरोध;
        }
        अन्यथा {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          अवरोध; /* do not advance the pointer */
        }
      }
      अन्यथा {
        परिणाम = curlx_dyn_addn(&ch->trailer, buf, 1);
        अगर(परिणाम) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          वापस परिणाम;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      अवरोध;

    हाल CHUNK_TRAILER_CR:
      अगर(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        वापस CURLE_RECV_ERROR;
      }
      अवरोध;

    हाल CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      अगर((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        अवरोध;
      }
      अगर(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      अवरोध;

    हाल CHUNK_STOP:
      अगर(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(तथ्य, "http_chunk, response complete");
        वापस CURLE_OK;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(तथ्य, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (अचिन्हित पूर्णांक)*buf);
        वापस CURLE_RECV_ERROR;
      }
    हाल CHUNK_DONE:
      वापस CURLE_OK;

    हाल CHUNK_FAILED:
      वापस CURLE_RECV_ERROR;
    }
  }
  वापस CURLE_OK;
}

अटल स्थिर अक्षर *Curl_chunked_strerror(CHUNKcode code)
{
  चयन(code) {
  शेष:
    वापस "OK";
  हाल CHUNKE_TOO_LONG_HEX:
    वापस "Too long hexadecimal number";
  हाल CHUNKE_ILLEGAL_HEX:
    वापस "Illegal or missing hexadecimal sequence";
  हाल CHUNKE_BAD_CHUNK:
    वापस "Malformed encoding found";
  हाल CHUNKE_PASSTHRU_ERROR:
    वापस "Error writing data to client";
  हाल CHUNKE_BAD_ENCODING:
    वापस "Bad content-encoding found";
  हाल CHUNKE_OUT_OF_MEMORY:
    वापस "Out of memory";
  हाल CHUNKE_TRAILER_LIMIT:
    वापस "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_chunker *ch,
                             अक्षर *buf, माप_प्रकार blen,
                             माप_प्रकार *pconsumed)
{
  वापस httpchunk_readwrite(तथ्य, ch, शून्य, buf, blen, pconsumed);
}

काष्ठा chunked_writer {
  काष्ठा Curl_cwriter super;
  काष्ठा Curl_chunker ch;
};

अटल CURLcode cw_chunked_init(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer)
{
  काष्ठा chunked_writer *ctx = writer->ctx;

  तथ्य->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(तथ्य, &ctx->ch, FALSE, FALSE);
  वापस CURLE_OK;
}

अटल व्योम cw_chunked_close(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer)
{
  काष्ठा chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(तथ्य, &ctx->ch);
}

अटल CURLcode cw_chunked_write(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                 स्थिर अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा chunked_writer *ctx = writer->ctx;
  CURLcode परिणाम;
  माप_प्रकार consumed;

  अगर(!(type & CLIENTWRITE_BODY))
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, blen);

  consumed = 0;
  परिणाम = httpchunk_readwrite(तथ्य, &ctx->ch, writer->अग्र, buf, blen,
                               &consumed);

  अगर(परिणाम) {
    अगर(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(तथ्य, "Failed reading the chunked-encoded stream");
    }
    अन्यथा {
      failf(तथ्य, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    वापस परिणाम;
  }

  blen -= consumed;
  अगर(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    तथ्य->req.download_done = TRUE;
    अगर(blen) {
      infof(तथ्य, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  अन्यथा अगर((type & CLIENTWRITE_EOS) && !तथ्य->req.no_body) {
    failf(तथ्य, "transfer closed with outstanding read data remaining");
    वापस CURLE_PARTIAL_FILE;
  }

  वापस CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
स्थिर काष्ठा Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  शून्य,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  माप(काष्ठा chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#घोषणा CURL_CHUNKED_MINLEN   1024
#घोषणा CURL_CHUNKED_MAXLEN   (64 * 1024)

काष्ठा chunked_reader {
  काष्ठा Curl_creader super;
  काष्ठा bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

अटल CURLcode cr_chunked_init(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader)
{
  काष्ठा chunked_reader *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  वापस CURLE_OK;
}

अटल व्योम cr_chunked_close(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader)
{
  काष्ठा chunked_reader *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_free(&ctx->chunkbuf);
}

अटल CURLcode add_last_chunk(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader)
{
  काष्ठा chunked_reader *ctx = reader->ctx;
  काष्ठा curl_slist *trailers = शून्य, *tr;
  CURLcode परिणाम;
  माप_प्रकार n;
  पूर्णांक rc;

  अगर(!तथ्य->set.trailer_callback) {
    CURL_TRC_READ(तथ्य, "http_chunk, added last, empty chunk");
    वापस Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  अगर(परिणाम)
    जाओ out;

  {
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_trailer_callback);
    rc = तथ्य->set.trailer_callback(&trailers, तथ्य->set.trailer_तथ्य);
    CURL_CBAPI_END(&guard);
  }

  अगर(rc != CURL_TRAILERFUNC_OK) {
    failf(तथ्य, "operation aborted by trailing headers callback");
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    जाओ out;
  }

  क्रम(tr = trailers; tr; tr = tr->अग्र) {
    /* only add correctly formatted trailers */
    स्थिर अक्षर *ptr = म_अक्षर(tr->तथ्य, ':');
    अगर(!ptr || *(ptr + 1) != ' ') {
      infof(तथ्य, "Malformatted trailing header, skipping trailer");
      जारी;
    }

    परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, tr->तथ्य, म_माप(tr->तथ्य), &n);
    अगर(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    अगर(परिणाम)
      जाओ out;
  }

  परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(तथ्य, "http_chunk, added last chunk with trailers "
                "from client -> %d", (पूर्णांक)परिणाम);
  वापस परिणाम;
}

अटल CURLcode add_chunk(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_creader *reader,
                          अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा chunked_reader *ctx = reader->ctx;
  CURLcode परिणाम;
  अक्षर tmp[CURL_CHUNKED_MINLEN];
  माप_प्रकार nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  अगर(blen < माप(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = माप(tmp);
  }
  अन्यथा {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  परिणाम = Curl_creader_read(तथ्य, reader->अग्र, buf, blen, &nread, &eos);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(eos)
    ctx->read_eos = TRUE;

  अगर(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    अक्षर hd[11] = "";
    पूर्णांक hdlen;
    माप_प्रकार n;

    hdlen = curl_msnprintf(hd, माप(hd), "%zx\r\n", nread);
    अगर(hdlen <= 0)
      वापस CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    अगर(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    अगर(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(तथ्य, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(ctx->read_eos)
    वापस add_last_chunk(तथ्य, reader);
  वापस CURLE_OK;
}

अटल CURLcode cr_chunked_read(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader,
                                अक्षर *buf, माप_प्रकार blen,
                                माप_प्रकार *pnread, bool *peos)
{
  काष्ठा chunked_reader *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  अगर(!ctx->eos) {
    अगर(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      परिणाम = add_chunk(तथ्य, reader, buf, blen);
      अगर(परिणाम)
        वापस परिणाम;
    }

    अगर(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      परिणाम = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      अगर(!परिणाम && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      वापस परिणाम;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  वापस CURLE_OK;
}

अटल curl_off_t cr_chunked_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस -1;
}

/* HTTP chunked Transfer-Encoding encoder */
स्थिर काष्ठा Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = शून्य;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, तथ्य, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  अगर(!परिणाम)
    परिणाम = Curl_creader_add(तथ्य, reader);

  अगर(परिणाम && reader)
    Curl_creader_free(तथ्य, reader);
  वापस परिणाम;
}

#पूर्ण_अगर /* CURL_DISABLE_HTTP */
