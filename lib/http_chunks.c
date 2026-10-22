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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#شامل "urldata.h" /* it includes http_chunks.h */
#شامل "curl_trc.h"
#شامل "http.h"    /* for Curl_verify_header */
#شامل "sendf.h"   /* for the client write stuff */
#شامل "curlx/dynbuf.h"
#شامل "multiif.h"
#شامل "curlx/strparse.h"

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

تهی Curl_httpchunk_init(ساختار Curl_easy *data, ساختار Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (تهی)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

تهی Curl_httpchunk_reset(ساختار Curl_easy *data, ساختار Curl_chunker *ch,
                          bool ignore_body)
{
  (تهی)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

تهی Curl_httpchunk_free(ساختار Curl_easy *data, ساختار Curl_chunker *ch)
{
  (تهی)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(ساختار Curl_easy *data, ساختار Curl_chunker *ch)
{
  (تهی)data;
  بازگشت ch->state == CHUNK_DONE;
}

ایستا CURLcode httpchunk_readwrite(ساختار Curl_easy *data,
                                    ساختار Curl_chunker *ch,
                                    ساختار Curl_cwriter *cw_next,
                                    ثابت نویسه *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  اگر(ch->state == CHUNK_DONE)
    بازگشت CURLE_OK;
  اگر(ch->state == CHUNK_FAILED)
    بازگشت CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  اگر(data->set.http_te_skip && !ch->ignore_body) {
    اگر(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    وگرنه
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    اگر(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      بازگشت result;
    }
  }

  وقتی(blen) {
    انتخاب(ch->state) {
    حالت CHUNK_HEX:
      اگر(ISXDIGIT(*buf)) {
        اگر(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          بازگشت CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      وگرنه {
        ثابت نویسه *p;
        اگر(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned صحیح)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          بازگشت CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        اگر(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          بازگشت CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      بشکن;

    حالت CHUNK_LF:
      /* waiting for the LF after a chunk size */
      اگر(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        اگر(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        وگرنه {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      بشکن;

    حالت CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      اگر(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      اگر(!data->set.http_te_skip && !ch->ignore_body) {
        اگر(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        وگرنه
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        اگر(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          بازگشت result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      اگر(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      بشکن;

    حالت CHUNK_POSTLF:
      اگر(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      وگرنه اگر(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        بازگشت CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      بشکن;

    حالت CHUNK_TRAILER:
      اگر((*buf == 0x0d) || (*buf == 0x0a)) {
        ثابت نویسه *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        اگر(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          اگر(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            بازگشت result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          اگر(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            بازگشت result;
          }

          اگر(!data->set.http_te_skip) {
            صحیح hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            اگر(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            اگر(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            وگرنه
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            اگر(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              بازگشت result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            اگر(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              بازگشت result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          اگر(*buf == 0x0a)
            /* already on the LF */
            بشکن;
        }
        وگرنه {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          بشکن; /* do not advance the pointer */
        }
      }
      وگرنه {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        اگر(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          بازگشت result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      بشکن;

    حالت CHUNK_TRAILER_CR:
      اگر(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      وگرنه {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        بازگشت CURLE_RECV_ERROR;
      }
      بشکن;

    حالت CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      اگر((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        بشکن;
      }
      اگر(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      بشکن;

    حالت CHUNK_STOP:
      اگر(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        بازگشت CURLE_OK;
      }
      وگرنه {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned صحیح)*buf);
        بازگشت CURLE_RECV_ERROR;
      }
    حالت CHUNK_DONE:
      بازگشت CURLE_OK;

    حالت CHUNK_FAILED:
      بازگشت CURLE_RECV_ERROR;
    }
  }
  بازگشت CURLE_OK;
}

ایستا ثابت نویسه *Curl_chunked_strerror(CHUNKcode code)
{
  انتخاب(code) {
  پیشفرض:
    بازگشت "OK";
  حالت CHUNKE_TOO_LONG_HEX:
    بازگشت "Too long hexadecimal number";
  حالت CHUNKE_ILLEGAL_HEX:
    بازگشت "Illegal or missing hexadecimal sequence";
  حالت CHUNKE_BAD_CHUNK:
    بازگشت "Malformed encoding found";
  حالت CHUNKE_PASSTHRU_ERROR:
    بازگشت "Error writing data to client";
  حالت CHUNKE_BAD_ENCODING:
    بازگشت "Bad content-encoding found";
  حالت CHUNKE_OUT_OF_MEMORY:
    بازگشت "Out of memory";
  حالت CHUNKE_TRAILER_LIMIT:
    بازگشت "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ساختار Curl_easy *data,
                             ساختار Curl_chunker *ch,
                             نویسه *buf, size_t blen,
                             size_t *pconsumed)
{
  بازگشت httpchunk_readwrite(data, ch, پوچ, buf, blen, pconsumed);
}

ساختار chunked_writer {
  ساختار Curl_cwriter super;
  ساختار Curl_chunker ch;
};

ایستا CURLcode cw_chunked_init(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer)
{
  ساختار chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  بازگشت CURLE_OK;
}

ایستا تهی cw_chunked_close(ساختار Curl_easy *data,
                             ساختار Curl_cwriter *writer)
{
  ساختار chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

ایستا CURLcode cw_chunked_write(ساختار Curl_easy *data,
                                 ساختار Curl_cwriter *writer, صحیح type,
                                 ثابت نویسه *buf, size_t blen)
{
  ساختار chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  اگر(!(type & CLIENTWRITE_BODY))
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  اگر(result) {
    اگر(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    وگرنه {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    بازگشت result;
  }

  blen -= consumed;
  اگر(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    اگر(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  وگرنه اگر((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    بازگشت CURLE_PARTIAL_FILE;
  }

  بازگشت CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ثابت ساختار Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  پوچ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  اندازه(ساختار chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

ساختار chunked_reader {
  ساختار Curl_creader super;
  ساختار bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ایستا CURLcode cr_chunked_init(ساختار Curl_easy *data,
                                ساختار Curl_creader *reader)
{
  ساختار chunked_reader *ctx = reader->ctx;
  (تهی)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  بازگشت CURLE_OK;
}

ایستا تهی cr_chunked_close(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader)
{
  ساختار chunked_reader *ctx = reader->ctx;
  (تهی)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

ایستا CURLcode add_last_chunk(ساختار Curl_easy *data,
                               ساختار Curl_creader *reader)
{
  ساختار chunked_reader *ctx = reader->ctx;
  ساختار curl_slist *trailers = پوچ, *tr;
  CURLcode result;
  size_t n;
  صحیح rc;

  اگر(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    بازگشت Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  اگر(result)
    برو out;

  {
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  اگر(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    برو out;
  }

  برای(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ثابت نویسه *ptr = strchr(tr->data, ':');
    اگر(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      ادامه;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    اگر(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    اگر(result)
      برو out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (صحیح)result);
  بازگشت result;
}

ایستا CURLcode add_chunk(ساختار Curl_easy *data,
                          ساختار Curl_creader *reader,
                          نویسه *buf, size_t blen)
{
  ساختار chunked_reader *ctx = reader->ctx;
  CURLcode result;
  نویسه tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  اگر(blen < اندازه(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = اندازه(tmp);
  }
  وگرنه {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  اگر(result)
    بازگشت result;
  اگر(eos)
    ctx->read_eos = TRUE;

  اگر(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    نویسه hd[11] = "";
    صحیح hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, اندازه(hd), "%zx\r\n", nread);
    اگر(hdlen <= 0)
      بازگشت CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    اگر(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    اگر(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (صحیح)result);
    اگر(result)
      بازگشت result;
  }

  اگر(ctx->read_eos)
    بازگشت add_last_chunk(data, reader);
  بازگشت CURLE_OK;
}

ایستا CURLcode cr_chunked_read(ساختار Curl_easy *data,
                                ساختار Curl_creader *reader,
                                نویسه *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ساختار chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  اگر(!ctx->eos) {
    اگر(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      اگر(result)
        بازگشت result;
    }

    اگر(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      اگر(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      بازگشت result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  بازگشت CURLE_OK;
}

ایستا curl_off_t cr_chunked_total_length(ساختار Curl_easy *data,
                                          ساختار Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (تهی)data;
  (تهی)reader;
  بازگشت -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ثابت ساختار Curl_crtype Curl_httpchunk_encoder = {
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
  اندازه(ساختار chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = پوچ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  اگر(!result)
    result = Curl_creader_add(data, reader);

  اگر(result && reader)
    Curl_creader_free(data, reader);
  بازگشت result;
}

#endif /* CURL_DISABLE_HTTP */
