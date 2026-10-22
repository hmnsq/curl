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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাৱ "urldata.h" /* it includes http_chunks.h */
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "http.h"    /* for Curl_verify_header */
#অন্তর্ভাৱ "sendf.h"   /* for the client write stuff */
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "curlx/strparse.h"

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

শূন্য Curl_httpchunk_init(সংরচনা Curl_easy *data, সংরচনা Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (শূন্য)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

শূন্য Curl_httpchunk_reset(সংরচনা Curl_easy *data, সংরচনা Curl_chunker *ch,
                          bool ignore_body)
{
  (শূন্য)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

শূন্য Curl_httpchunk_free(সংরচনা Curl_easy *data, সংরচনা Curl_chunker *ch)
{
  (শূন্য)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(সংরচনা Curl_easy *data, সংরচনা Curl_chunker *ch)
{
  (শূন্য)data;
  ঘূৰাই ch->state == CHUNK_DONE;
}

স্থির CURLcode httpchunk_readwrite(সংরচনা Curl_easy *data,
                                    সংরচনা Curl_chunker *ch,
                                    সংরচনা Curl_cwriter *cw_next,
                                    ধ্রুৱ ৱর্ণ *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  যদি(ch->state == CHUNK_DONE)
    ঘূৰাই CURLE_OK;
  যদি(ch->state == CHUNK_FAILED)
    ঘূৰাই CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  যদি(data->set.http_te_skip && !ch->ignore_body) {
    যদি(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    অন্যথা
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    যদি(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      ঘূৰাই result;
    }
  }

  যেতিয়ালৈ(blen) {
    ৱিকল্প(ch->state) {
    পক্ষ CHUNK_HEX:
      যদি(ISXDIGIT(*buf)) {
        যদি(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ঘূৰাই CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      অন্যথা {
        ধ্রুৱ ৱর্ণ *p;
        যদি(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned পূর্ণাঙ্ক)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ঘূৰাই CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        যদি(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ঘূৰাই CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ভঙ্গ;

    পক্ষ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      যদি(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        যদি(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        অন্যথা {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ভঙ্গ;

    পক্ষ CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      যদি(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      যদি(!data->set.http_te_skip && !ch->ignore_body) {
        যদি(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        অন্যথা
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        যদি(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          ঘূৰাই result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      যদি(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ভঙ্গ;

    পক্ষ CHUNK_POSTLF:
      যদি(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      অন্যথা যদি(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ঘূৰাই CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ভঙ্গ;

    পক্ষ CHUNK_TRAILER:
      যদি((*buf == 0x0d) || (*buf == 0x0a)) {
        ধ্রুৱ ৱর্ণ *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        যদি(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          যদি(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            ঘূৰাই result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          যদি(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            ঘূৰাই result;
          }

          যদি(!data->set.http_te_skip) {
            পূর্ণাঙ্ক hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            যদি(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            যদি(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            অন্যথা
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            যদি(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              ঘূৰাই result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            যদি(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              ঘূৰাই result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          যদি(*buf == 0x0a)
            /* already on the LF */
            ভঙ্গ;
        }
        অন্যথা {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ভঙ্গ; /* do not advance the pointer */
        }
      }
      অন্যথা {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        যদি(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          ঘূৰাই result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ভঙ্গ;

    পক্ষ CHUNK_TRAILER_CR:
      যদি(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      অন্যথা {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ঘূৰাই CURLE_RECV_ERROR;
      }
      ভঙ্গ;

    পক্ষ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      যদি((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ভঙ্গ;
      }
      যদি(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ভঙ্গ;

    পক্ষ CHUNK_STOP:
      যদি(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        ঘূৰাই CURLE_OK;
      }
      অন্যথা {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned পূর্ণাঙ্ক)*buf);
        ঘূৰাই CURLE_RECV_ERROR;
      }
    পক্ষ CHUNK_DONE:
      ঘূৰাই CURLE_OK;

    পক্ষ CHUNK_FAILED:
      ঘূৰাই CURLE_RECV_ERROR;
    }
  }
  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ ৱর্ণ *Curl_chunked_strerror(CHUNKcode code)
{
  ৱিকল্প(code) {
  স্ৱভাৱ:
    ঘূৰাই "OK";
  পক্ষ CHUNKE_TOO_LONG_HEX:
    ঘূৰাই "Too long hexadecimal number";
  পক্ষ CHUNKE_ILLEGAL_HEX:
    ঘূৰাই "Illegal or missing hexadecimal sequence";
  পক্ষ CHUNKE_BAD_CHUNK:
    ঘূৰাই "Malformed encoding found";
  পক্ষ CHUNKE_PASSTHRU_ERROR:
    ঘূৰাই "Error writing data to client";
  পক্ষ CHUNKE_BAD_ENCODING:
    ঘূৰাই "Bad content-encoding found";
  পক্ষ CHUNKE_OUT_OF_MEMORY:
    ঘূৰাই "Out of memory";
  পক্ষ CHUNKE_TRAILER_LIMIT:
    ঘূৰাই "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(সংরচনা Curl_easy *data,
                             সংরচনা Curl_chunker *ch,
                             ৱর্ণ *buf, size_t blen,
                             size_t *pconsumed)
{
  ঘূৰাই httpchunk_readwrite(data, ch, রিক্ত, buf, blen, pconsumed);
}

সংরচনা chunked_writer {
  সংরচনা Curl_cwriter super;
  সংরচনা Curl_chunker ch;
};

স্থির CURLcode cw_chunked_init(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer)
{
  সংরচনা chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cw_chunked_close(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer)
{
  সংরচনা chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

স্থির CURLcode cw_chunked_write(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                 ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  সংরচনা chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  যদি(!(type & CLIENTWRITE_BODY))
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  যদি(result) {
    যদি(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    অন্যথা {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    ঘূৰাই result;
  }

  blen -= consumed;
  যদি(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    যদি(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  অন্যথা যদি((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    ঘূৰাই CURLE_PARTIAL_FILE;
  }

  ঘূৰাই CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ধ্রুৱ সংরচনা Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  রিক্ত,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  পরিমাণ(সংরচনা chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

সংরচনা chunked_reader {
  সংরচনা Curl_creader super;
  সংরচনা bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_chunked_init(সংরচনা Curl_easy *data,
                                সংরচনা Curl_creader *reader)
{
  সংরচনা chunked_reader *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cr_chunked_close(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader)
{
  সংরচনা chunked_reader *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

স্থির CURLcode add_last_chunk(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creader *reader)
{
  সংরচনা chunked_reader *ctx = reader->ctx;
  সংরচনা curl_slist *trailers = রিক্ত, *tr;
  CURLcode result;
  size_t n;
  পূর্ণাঙ্ক rc;

  যদি(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    ঘূৰাই Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  যদি(result)
    গচ্ছ out;

  {
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  যদি(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    গচ্ছ out;
  }

  বাবে(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    ধ্রুৱ ৱর্ণ *ptr = strchr(tr->data, ':');
    যদি(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      অনুৱর্ত;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    যদি(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    যদি(result)
      গচ্ছ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

স্থির CURLcode add_chunk(সংরচনা Curl_easy *data,
                          সংরচনা Curl_creader *reader,
                          ৱর্ণ *buf, size_t blen)
{
  সংরচনা chunked_reader *ctx = reader->ctx;
  CURLcode result;
  ৱর্ণ tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  যদি(blen < পরিমাণ(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = পরিমাণ(tmp);
  }
  অন্যথা {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  যদি(result)
    ঘূৰাই result;
  যদি(eos)
    ctx->read_eos = TRUE;

  যদি(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    ৱর্ণ hd[11] = "";
    পূর্ণাঙ্ক hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, পরিমাণ(hd), "%zx\r\n", nread);
    যদি(hdlen <= 0)
      ঘূৰাই CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    যদি(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    যদি(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (পূর্ণাঙ্ক)result);
    যদি(result)
      ঘূৰাই result;
  }

  যদি(ctx->read_eos)
    ঘূৰাই add_last_chunk(data, reader);
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cr_chunked_read(সংরচনা Curl_easy *data,
                                সংরচনা Curl_creader *reader,
                                ৱর্ণ *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  সংরচনা chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  যদি(!ctx->eos) {
    যদি(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      যদি(result)
        ঘূৰাই result;
    }

    যদি(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      যদি(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ঘূৰাই result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ঘূৰাই CURLE_OK;
}

স্থির curl_off_t cr_chunked_total_length(সংরচনা Curl_easy *data,
                                          সংরচনা Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ধ্রুৱ সংরচনা Curl_crtype Curl_httpchunk_encoder = {
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
  পরিমাণ(সংরচনা chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = রিক্ত;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  যদি(!result)
    result = Curl_creader_add(data, reader);

  যদি(result && reader)
    Curl_creader_free(data, reader);
  ঘূৰাই result;
}

#endif /* CURL_DISABLE_HTTP */
