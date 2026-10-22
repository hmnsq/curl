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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#અન્તર્ભાવ "urldata.h" /* it includes http_chunks.h */
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "http.h"    /* for Curl_verify_header */
#અન્તર્ભાવ "sendf.h"   /* for the client write stuff */
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curlx/strparse.h"

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

શૂન્ય Curl_httpchunk_init(સંરચના Curl_easy *data, સંરચના Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (શૂન્ય)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

શૂન્ય Curl_httpchunk_reset(સંરચના Curl_easy *data, સંરચના Curl_chunker *ch,
                          bool ignore_body)
{
  (શૂન્ય)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

શૂન્ય Curl_httpchunk_free(સંરચના Curl_easy *data, સંરચના Curl_chunker *ch)
{
  (શૂન્ય)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(સંરચના Curl_easy *data, સંરચના Curl_chunker *ch)
{
  (શૂન્ય)data;
  પાછા ch->state == CHUNK_DONE;
}

સ્થિર CURLcode httpchunk_readwrite(સંરચના Curl_easy *data,
                                    સંરચના Curl_chunker *ch,
                                    સંરચના Curl_cwriter *cw_next,
                                    અચલ અક્ષર *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  જો(ch->state == CHUNK_DONE)
    પાછા CURLE_OK;
  જો(ch->state == CHUNK_FAILED)
    પાછા CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  જો(data->set.http_te_skip && !ch->ignore_body) {
    જો(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    નહીંતર
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    જો(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      પાછા result;
    }
  }

  જ્યારે(blen) {
    પસંદગી(ch->state) {
    સ્થિતિ CHUNK_HEX:
      જો(ISXDIGIT(*buf)) {
        જો(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          પાછા CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      નહીંતર {
        અચલ અક્ષર *p;
        જો(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned પૂર્ણાંક)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          પાછા CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        જો(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          પાછા CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      તોડો;

    સ્થિતિ CHUNK_LF:
      /* waiting for the LF after a chunk size */
      જો(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        જો(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        નહીંતર {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      તોડો;

    સ્થિતિ CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      જો(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      જો(!data->set.http_te_skip && !ch->ignore_body) {
        જો(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        નહીંતર
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        જો(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          પાછા result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      જો(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      તોડો;

    સ્થિતિ CHUNK_POSTLF:
      જો(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      નહીંતર જો(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        પાછા CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      તોડો;

    સ્થિતિ CHUNK_TRAILER:
      જો((*buf == 0x0d) || (*buf == 0x0a)) {
        અચલ અક્ષર *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        જો(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          જો(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            પાછા result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          જો(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            પાછા result;
          }

          જો(!data->set.http_te_skip) {
            પૂર્ણાંક hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            જો(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            જો(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            નહીંતર
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            જો(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              પાછા result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            જો(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              પાછા result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          જો(*buf == 0x0a)
            /* already on the LF */
            તોડો;
        }
        નહીંતર {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          તોડો; /* do not advance the pointer */
        }
      }
      નહીંતર {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        જો(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          પાછા result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      તોડો;

    સ્થિતિ CHUNK_TRAILER_CR:
      જો(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      નહીંતર {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        પાછા CURLE_RECV_ERROR;
      }
      તોડો;

    સ્થિતિ CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      જો((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        તોડો;
      }
      જો(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      તોડો;

    સ્થિતિ CHUNK_STOP:
      જો(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        પાછા CURLE_OK;
      }
      નહીંતર {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned પૂર્ણાંક)*buf);
        પાછા CURLE_RECV_ERROR;
      }
    સ્થિતિ CHUNK_DONE:
      પાછા CURLE_OK;

    સ્થિતિ CHUNK_FAILED:
      પાછા CURLE_RECV_ERROR;
    }
  }
  પાછા CURLE_OK;
}

સ્થિર અચલ અક્ષર *Curl_chunked_strerror(CHUNKcode code)
{
  પસંદગી(code) {
  મૂળભૂત:
    પાછા "OK";
  સ્થિતિ CHUNKE_TOO_LONG_HEX:
    પાછા "Too long hexadecimal number";
  સ્થિતિ CHUNKE_ILLEGAL_HEX:
    પાછા "Illegal or missing hexadecimal sequence";
  સ્થિતિ CHUNKE_BAD_CHUNK:
    પાછા "Malformed encoding found";
  સ્થિતિ CHUNKE_PASSTHRU_ERROR:
    પાછા "Error writing data to client";
  સ્થિતિ CHUNKE_BAD_ENCODING:
    પાછા "Bad content-encoding found";
  સ્થિતિ CHUNKE_OUT_OF_MEMORY:
    પાછા "Out of memory";
  સ્થિતિ CHUNKE_TRAILER_LIMIT:
    પાછા "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(સંરચના Curl_easy *data,
                             સંરચના Curl_chunker *ch,
                             અક્ષર *buf, size_t blen,
                             size_t *pconsumed)
{
  પાછા httpchunk_readwrite(data, ch, ખાલી, buf, blen, pconsumed);
}

સંરચના chunked_writer {
  સંરચના Curl_cwriter super;
  સંરચના Curl_chunker ch;
};

સ્થિર CURLcode cw_chunked_init(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer)
{
  સંરચના chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cw_chunked_close(સંરચના Curl_easy *data,
                             સંરચના Curl_cwriter *writer)
{
  સંરચના chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

સ્થિર CURLcode cw_chunked_write(સંરચના Curl_easy *data,
                                 સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                 અચલ અક્ષર *buf, size_t blen)
{
  સંરચના chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  જો(!(type & CLIENTWRITE_BODY))
    પાછા Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  જો(result) {
    જો(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    નહીંતર {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    પાછા result;
  }

  blen -= consumed;
  જો(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    જો(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  નહીંતર જો((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    પાછા CURLE_PARTIAL_FILE;
  }

  પાછા CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
અચલ સંરચના Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ખાલી,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  કદ(સંરચના chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

સંરચના chunked_reader {
  સંરચના Curl_creader super;
  સંરચના bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

સ્થિર CURLcode cr_chunked_init(સંરચના Curl_easy *data,
                                સંરચના Curl_creader *reader)
{
  સંરચના chunked_reader *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cr_chunked_close(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader)
{
  સંરચના chunked_reader *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

સ્થિર CURLcode add_last_chunk(સંરચના Curl_easy *data,
                               સંરચના Curl_creader *reader)
{
  સંરચના chunked_reader *ctx = reader->ctx;
  સંરચના curl_slist *trailers = ખાલી, *tr;
  CURLcode result;
  size_t n;
  પૂર્ણાંક rc;

  જો(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    પાછા Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  જો(result)
    જાઓ out;

  {
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  જો(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    જાઓ out;
  }

  માટે(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    અચલ અક્ષર *ptr = strchr(tr->data, ':');
    જો(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      ચાલુ;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    જો(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    જો(result)
      જાઓ out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (પૂર્ણાંક)result);
  પાછા result;
}

સ્થિર CURLcode add_chunk(સંરચના Curl_easy *data,
                          સંરચના Curl_creader *reader,
                          અક્ષર *buf, size_t blen)
{
  સંરચના chunked_reader *ctx = reader->ctx;
  CURLcode result;
  અક્ષર tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  જો(blen < કદ(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = કદ(tmp);
  }
  નહીંતર {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  જો(result)
    પાછા result;
  જો(eos)
    ctx->read_eos = TRUE;

  જો(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    અક્ષર hd[11] = "";
    પૂર્ણાંક hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, કદ(hd), "%zx\r\n", nread);
    જો(hdlen <= 0)
      પાછા CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    જો(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    જો(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
  }

  જો(ctx->read_eos)
    પાછા add_last_chunk(data, reader);
  પાછા CURLE_OK;
}

સ્થિર CURLcode cr_chunked_read(સંરચના Curl_easy *data,
                                સંરચના Curl_creader *reader,
                                અક્ષર *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  સંરચના chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  જો(!ctx->eos) {
    જો(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      જો(result)
        પાછા result;
    }

    જો(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      જો(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      પાછા result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  પાછા CURLE_OK;
}

સ્થિર curl_off_t cr_chunked_total_length(સંરચના Curl_easy *data,
                                          સંરચના Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા -1;
}

/* HTTP chunked Transfer-Encoding encoder */
અચલ સંરચના Curl_crtype Curl_httpchunk_encoder = {
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
  કદ(સંરચના chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = ખાલી;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  જો(!result)
    result = Curl_creader_add(data, reader);

  જો(result && reader)
    Curl_creader_free(data, reader);
  પાછા result;
}

#endif /* CURL_DISABLE_HTTP */
