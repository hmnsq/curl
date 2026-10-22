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
  फर्काऊ ch->state == CHUNK_DONE;
}

स्थिर CURLcode httpchunk_readwrite(संरचना Curl_easy *data,
                                    संरचना Curl_chunker *ch,
                                    संरचना Curl_cwriter *cw_next,
                                    अचर वर्ण *buf, माप_प्रकार blen,
                                    माप_प्रकार *pconsumed)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  यदि(ch->state == CHUNK_DONE)
    फर्काऊ CURLE_OK;
  यदि(ch->state == CHUNK_FAILED)
    फर्काऊ CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  यदि(data->set.http_te_skip && !ch->ignore_body) {
    यदि(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    अन्यथा
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    यदि(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      फर्काऊ result;
    }
  }

  जबसम्म(blen) {
    विकल्प(ch->state) {
    पक्ष CHUNK_HEX:
      यदि(ISXDIGIT(*buf)) {
        यदि(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          फर्काऊ CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        अचर वर्ण *p;
        यदि(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (अचिन्हित पूर्णाङ्क)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          फर्काऊ CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        यदि(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          फर्काऊ CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      रोक;

    पक्ष CHUNK_LF:
      /* waiting for the LF after a chunk size */
      यदि(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        यदि(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        अन्यथा {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      रोक;

    पक्ष CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      यदि(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      यदि(!data->set.http_te_skip && !ch->ignore_body) {
        यदि(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        अन्यथा
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        यदि(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          फर्काऊ result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      यदि(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      रोक;

    पक्ष CHUNK_POSTLF:
      यदि(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      अन्यथा यदि(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        फर्काऊ CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      रोक;

    पक्ष CHUNK_TRAILER:
      यदि((*buf == 0x0d) || (*buf == 0x0a)) {
        अचर वर्ण *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        यदि(tr) {
          माप_प्रकार trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          यदि(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            फर्काऊ result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          यदि(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            फर्काऊ result;
          }

          यदि(!data->set.http_te_skip) {
            पूर्णाङ्क hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            यदि(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            यदि(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            अन्यथा
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            यदि(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              फर्काऊ result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            यदि(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              फर्काऊ result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          यदि(*buf == 0x0a)
            /* already on the LF */
            रोक;
        }
        अन्यथा {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          रोक; /* do not advance the pointer */
        }
      }
      अन्यथा {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        यदि(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          फर्काऊ result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      रोक;

    पक्ष CHUNK_TRAILER_CR:
      यदि(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        फर्काऊ CURLE_RECV_ERROR;
      }
      रोक;

    पक्ष CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      यदि((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        रोक;
      }
      यदि(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      रोक;

    पक्ष CHUNK_STOP:
      यदि(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        फर्काऊ CURLE_OK;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (अचिन्हित पूर्णाङ्क)*buf);
        फर्काऊ CURLE_RECV_ERROR;
      }
    पक्ष CHUNK_DONE:
      फर्काऊ CURLE_OK;

    पक्ष CHUNK_FAILED:
      फर्काऊ CURLE_RECV_ERROR;
    }
  }
  फर्काऊ CURLE_OK;
}

स्थिर अचर वर्ण *Curl_chunked_strerror(CHUNKcode code)
{
  विकल्प(code) {
  स्वभाव:
    फर्काऊ "OK";
  पक्ष CHUNKE_TOO_LONG_HEX:
    फर्काऊ "Too long hexadecimal number";
  पक्ष CHUNKE_ILLEGAL_HEX:
    फर्काऊ "Illegal or missing hexadecimal sequence";
  पक्ष CHUNKE_BAD_CHUNK:
    फर्काऊ "Malformed encoding found";
  पक्ष CHUNKE_PASSTHRU_ERROR:
    फर्काऊ "Error writing data to client";
  पक्ष CHUNKE_BAD_ENCODING:
    फर्काऊ "Bad content-encoding found";
  पक्ष CHUNKE_OUT_OF_MEMORY:
    फर्काऊ "Out of memory";
  पक्ष CHUNKE_TRAILER_LIMIT:
    फर्काऊ "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(संरचना Curl_easy *data,
                             संरचना Curl_chunker *ch,
                             वर्ण *buf, माप_प्रकार blen,
                             माप_प्रकार *pconsumed)
{
  फर्काऊ httpchunk_readwrite(data, ch, रिक्त, buf, blen, pconsumed);
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
  फर्काऊ CURLE_OK;
}

स्थिर शून्य cw_chunked_close(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer)
{
  संरचना chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

स्थिर CURLcode cw_chunked_write(संरचना Curl_easy *data,
                                 संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                 अचर वर्ण *buf, माप_प्रकार blen)
{
  संरचना chunked_writer *ctx = writer->ctx;
  CURLcode result;
  माप_प्रकार consumed;

  यदि(!(type & CLIENTWRITE_BODY))
    फर्काऊ Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  यदि(result) {
    यदि(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    अन्यथा {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    फर्काऊ result;
  }

  blen -= consumed;
  यदि(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    यदि(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  अन्यथा यदि((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    फर्काऊ CURLE_PARTIAL_FILE;
  }

  फर्काऊ CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
अचर संरचना Curl_cwtype Curl_httpchunk_unencoder = {
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
  फर्काऊ CURLE_OK;
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
  माप_प्रकार n;
  पूर्णाङ्क rc;

  यदि(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    फर्काऊ Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  यदि(result)
    गच्छ out;

  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  यदि(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    गच्छ out;
  }

  लागि(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    अचर वर्ण *ptr = म_अक्षर(tr->data, ':');
    यदि(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      जारी;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, म_माप(tr->data), &n);
    यदि(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    यदि(result)
      गच्छ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (पूर्णाङ्क)result);
  फर्काऊ result;
}

स्थिर CURLcode add_chunk(संरचना Curl_easy *data,
                          संरचना Curl_creader *reader,
                          वर्ण *buf, माप_प्रकार blen)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode result;
  वर्ण tmp[CURL_CHUNKED_MINLEN];
  माप_प्रकार nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  यदि(blen < परिमाण(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = परिमाण(tmp);
  }
  अन्यथा {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  यदि(result)
    फर्काऊ result;
  यदि(eos)
    ctx->read_eos = TRUE;

  यदि(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    वर्ण hd[11] = "";
    पूर्णाङ्क hdlen;
    माप_प्रकार n;

    hdlen = curl_msnprintf(hd, परिमाण(hd), "%zx\r\n", nread);
    यदि(hdlen <= 0)
      फर्काऊ CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    यदि(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    यदि(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (पूर्णाङ्क)result);
    यदि(result)
      फर्काऊ result;
  }

  यदि(ctx->read_eos)
    फर्काऊ add_last_chunk(data, reader);
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cr_chunked_read(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader,
                                वर्ण *buf, माप_प्रकार blen,
                                माप_प्रकार *pnread, bool *peos)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  यदि(!ctx->eos) {
    यदि(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      यदि(result)
        फर्काऊ result;
    }

    यदि(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      यदि(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      फर्काऊ result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  फर्काऊ CURLE_OK;
}

स्थिर curl_off_t cr_chunked_total_length(संरचना Curl_easy *data,
                                          संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  फर्काऊ -1;
}

/* HTTP chunked Transfer-Encoding encoder */
अचर संरचना Curl_crtype Curl_httpchunk_encoder = {
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
  यदि(!result)
    result = Curl_creader_add(data, reader);

  यदि(result && reader)
    Curl_creader_free(data, reader);
  फर्काऊ result;
}

#endif /* CURL_DISABLE_HTTP */
