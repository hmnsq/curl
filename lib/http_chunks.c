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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#ضمن "urldata.h" /* it includes http_chunks.h */
#ضمن "curl_trc.h"
#ضمن "http.h"    /* for Curl_verify_header */
#ضمن "sendf.h"   /* for the client write stuff */
#ضمن "curlx/dynbuf.h"
#ضمن "multiif.h"
#ضمن "curlx/strparse.h"

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

فراغ Curl_httpchunk_init(بنية Curl_easy *data, بنية Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (فراغ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

فراغ Curl_httpchunk_reset(بنية Curl_easy *data, بنية Curl_chunker *ch,
                          bool ignore_body)
{
  (فراغ)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

فراغ Curl_httpchunk_free(بنية Curl_easy *data, بنية Curl_chunker *ch)
{
  (فراغ)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(بنية Curl_easy *data, بنية Curl_chunker *ch)
{
  (فراغ)data;
  החזר ch->state == CHUNK_DONE;
}

ساكن CURLcode httpchunk_readwrite(بنية Curl_easy *data,
                                    بنية Curl_chunker *ch,
                                    بنية Curl_cwriter *cw_next,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  אם(ch->state == CHUNK_DONE)
    החזר CURLE_OK;
  אם(ch->state == CHUNK_FAILED)
    החזר CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  אם(data->set.http_te_skip && !ch->ignore_body) {
    אם(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    אחרת
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    אם(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      החזר result;
    }
  }

  כלעוד(blen) {
    بدل(ch->state) {
    حالة CHUNK_HEX:
      אם(ISXDIGIT(*buf)) {
        אם(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          החזר CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      אחרת {
        ثابت حرف *p;
        אם(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned صحيح)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          החזר CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        אם(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          החזר CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      اكسر;

    حالة CHUNK_LF:
      /* waiting for the LF after a chunk size */
      אם(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        אם(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        אחרת {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      אם(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      אם(!data->set.http_te_skip && !ch->ignore_body) {
        אם(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        אחרת
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        אם(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          החזר result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      אם(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      اكسر;

    حالة CHUNK_POSTLF:
      אם(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      אחרת אם(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        החזר CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_TRAILER:
      אם((*buf == 0x0d) || (*buf == 0x0a)) {
        ثابت حرف *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        אם(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          אם(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            החזר result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          אם(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            החזר result;
          }

          אם(!data->set.http_te_skip) {
            صحيح hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            אם(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            אם(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            אחרת
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            אם(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              החזר result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            אם(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              החזר result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          אם(*buf == 0x0a)
            /* already on the LF */
            اكسر;
        }
        אחרת {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          اكسر; /* do not advance the pointer */
        }
      }
      אחרת {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        אם(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          החזר result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_TRAILER_CR:
      אם(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      אחרת {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        החזר CURLE_RECV_ERROR;
      }
      اكسر;

    حالة CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      אם((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        اكسر;
      }
      אם(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      اكسر;

    حالة CHUNK_STOP:
      אם(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        החזר CURLE_OK;
      }
      אחרת {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned صحيح)*buf);
        החזר CURLE_RECV_ERROR;
      }
    حالة CHUNK_DONE:
      החזר CURLE_OK;

    حالة CHUNK_FAILED:
      החזר CURLE_RECV_ERROR;
    }
  }
  החזר CURLE_OK;
}

ساكن ثابت حرف *Curl_chunked_strerror(CHUNKcode code)
{
  بدل(code) {
  افتراضي:
    החזר "OK";
  حالة CHUNKE_TOO_LONG_HEX:
    החזר "Too long hexadecimal number";
  حالة CHUNKE_ILLEGAL_HEX:
    החזר "Illegal or missing hexadecimal sequence";
  حالة CHUNKE_BAD_CHUNK:
    החזר "Malformed encoding found";
  حالة CHUNKE_PASSTHRU_ERROR:
    החזר "Error writing data to client";
  حالة CHUNKE_BAD_ENCODING:
    החזר "Bad content-encoding found";
  حالة CHUNKE_OUT_OF_MEMORY:
    החזר "Out of memory";
  حالة CHUNKE_TRAILER_LIMIT:
    החזר "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(بنية Curl_easy *data,
                             بنية Curl_chunker *ch,
                             حرف *buf, size_t blen,
                             size_t *pconsumed)
{
  החזר httpchunk_readwrite(data, ch, عدم, buf, blen, pconsumed);
}

بنية chunked_writer {
  بنية Curl_cwriter super;
  بنية Curl_chunker ch;
};

ساكن CURLcode cw_chunked_init(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer)
{
  بنية chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  החזר CURLE_OK;
}

ساكن فراغ cw_chunked_close(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer)
{
  بنية chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

ساكن CURLcode cw_chunked_write(بنية Curl_easy *data,
                                 بنية Curl_cwriter *writer, صحيح type,
                                 ثابت حرف *buf, size_t blen)
{
  بنية chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  אם(!(type & CLIENTWRITE_BODY))
    החזר Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  אם(result) {
    אם(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    אחרת {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    החזר result;
  }

  blen -= consumed;
  אם(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    אם(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  אחרת אם((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    החזר CURLE_PARTIAL_FILE;
  }

  החזר CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ثابت بنية Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  عدم,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  حجم(بنية chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

بنية chunked_reader {
  بنية Curl_creader super;
  بنية bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_chunked_init(بنية Curl_easy *data,
                                بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  (فراغ)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  החזר CURLE_OK;
}

ساكن فراغ cr_chunked_close(بنية Curl_easy *data,
                             بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  (فراغ)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

ساكن CURLcode add_last_chunk(بنية Curl_easy *data,
                               بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  بنية curl_slist *trailers = عدم, *tr;
  CURLcode result;
  size_t n;
  صحيح rc;

  אם(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    החזר Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  אם(result)
    اذهب out;

  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  אם(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    اذهب out;
  }

  עבור(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ثابت حرف *ptr = strchr(tr->data, ':');
    אם(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      استمر;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    אם(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    אם(result)
      اذهب out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (صحيح)result);
  החזר result;
}

ساكن CURLcode add_chunk(بنية Curl_easy *data,
                          بنية Curl_creader *reader,
                          حرف *buf, size_t blen)
{
  بنية chunked_reader *ctx = reader->ctx;
  CURLcode result;
  حرف tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  אם(blen < حجم(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = حجم(tmp);
  }
  אחרת {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  אם(result)
    החזר result;
  אם(eos)
    ctx->read_eos = TRUE;

  אם(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    حرف hd[11] = "";
    صحيح hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, حجم(hd), "%zx\r\n", nread);
    אם(hdlen <= 0)
      החזר CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    אם(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    אם(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (صحيح)result);
    אם(result)
      החזר result;
  }

  אם(ctx->read_eos)
    החזר add_last_chunk(data, reader);
  החזר CURLE_OK;
}

ساكن CURLcode cr_chunked_read(بنية Curl_easy *data,
                                بنية Curl_creader *reader,
                                حرف *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  بنية chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  אם(!ctx->eos) {
    אם(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      אם(result)
        החזר result;
    }

    אם(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      אם(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      החזר result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  החזר CURLE_OK;
}

ساكن curl_off_t cr_chunked_total_length(بنية Curl_easy *data,
                                          بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)data;
  (فراغ)reader;
  החזר -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ثابت بنية Curl_crtype Curl_httpchunk_encoder = {
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
  حجم(بنية chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = عدم;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  אם(!result)
    result = Curl_creader_add(data, reader);

  אם(result && reader)
    Curl_creader_free(data, reader);
  החזר result;
}

#endif /* CURL_DISABLE_HTTP */
