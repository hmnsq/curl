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
#include "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#include "urldata.h" /* it includes http_chunks.h */
#include "curl_trc.h"
#include "http.h"    /* for Curl_verify_header */
#include "sendf.h"   /* for the client write stuff */
#include "curlx/dynbuf.h"
#include "multiif.h"
#include "curlx/strparse.h"

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

void Curl_httpchunk_init(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (void)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

void Curl_httpchunk_reset(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_chunker *ch,
                          bool ignore_body)
{
  (void)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

void Curl_httpchunk_free(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_chunker *ch)
{
  (void)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_chunker *ch)
{
  (void)data;
  ལོག ch->state == CHUNK_DONE;
}

static CURLcode httpchunk_readwrite(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_chunker *ch,
                                    sa_nracanaa Curl_cwriter *cw_next,
                                    const var_na *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  གལ་སྲིད(ch->state == CHUNK_DONE)
    ལོག CURLE_OK;
  གལ་སྲིད(ch->state == CHUNK_FAILED)
    ལོག CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  གལ་སྲིད(data->set.http_te_skip && !ch->ignore_body) {
    གལ་སྲིད(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    དེ་མིན
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    གལ་སྲིད(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      ལོག result;
    }
  }

  སྐབས(blen) {
    switch(ch->state) {
    pak_sa CHUNK_HEX:
      གལ་སྲིད(ISXDIGIT(*buf)) {
        གལ་སྲིད(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ལོག CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      དེ་མིན {
        const var_na *p;
        གལ་སྲིད(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned ཧྲིལ)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ལོག CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        གལ་སྲིད(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ལོག CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      bha_nga;

    pak_sa CHUNK_LF:
      /* waiting for the LF after a chunk size */
      གལ་སྲིད(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        གལ་སྲིད(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        དེ་མིན {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      bha_nga;

    pak_sa CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      གལ་སྲིད(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      གལ་སྲིད(!data->set.http_te_skip && !ch->ignore_body) {
        གལ་སྲིད(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        དེ་མིན
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        གལ་སྲིད(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          ལོག result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      གལ་སྲིད(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      bha_nga;

    pak_sa CHUNK_POSTLF:
      གལ་སྲིད(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      དེ་མིན གལ་སྲིད(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ལོག CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      bha_nga;

    pak_sa CHUNK_TRAILER:
      གལ་སྲིད((*buf == 0x0d) || (*buf == 0x0a)) {
        const var_na *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        གལ་སྲིད(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          གལ་སྲིད(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            ལོག result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          གལ་སྲིད(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            ལོག result;
          }

          གལ་སྲིད(!data->set.http_te_skip) {
            ཧྲིལ hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            གལ་སྲིད(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            གལ་སྲིད(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            དེ་མིན
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            གལ་སྲིད(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              ལོག result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            གལ་སྲིད(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              ལོག result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          གལ་སྲིད(*buf == 0x0a)
            /* already on the LF */
            bha_nga;
        }
        དེ་མིན {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          bha_nga; /* do not advance the pointer */
        }
      }
      དེ་མིན {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        གལ་སྲིད(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          ལོག result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      bha_nga;

    pak_sa CHUNK_TRAILER_CR:
      གལ་སྲིད(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      དེ་མིན {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ལོག CURLE_RECV_ERROR;
      }
      bha_nga;

    pak_sa CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      གལ་སྲིད((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        bha_nga;
      }
      གལ་སྲིད(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      bha_nga;

    pak_sa CHUNK_STOP:
      གལ་སྲིད(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        ལོག CURLE_OK;
      }
      དེ་མིན {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned ཧྲིལ)*buf);
        ལོག CURLE_RECV_ERROR;
      }
    pak_sa CHUNK_DONE:
      ལོག CURLE_OK;

    pak_sa CHUNK_FAILED:
      ལོག CURLE_RECV_ERROR;
    }
  }
  ལོག CURLE_OK;
}

static const var_na *Curl_chunked_strerror(CHUNKcode code)
{
  switch(code) {
  default:
    ལོག "OK";
  pak_sa CHUNKE_TOO_LONG_HEX:
    ལོག "Too long hexadecimal number";
  pak_sa CHUNKE_ILLEGAL_HEX:
    ལོག "Illegal or missing hexadecimal sequence";
  pak_sa CHUNKE_BAD_CHUNK:
    ལོག "Malformed encoding found";
  pak_sa CHUNKE_PASSTHRU_ERROR:
    ལོག "Error writing data to client";
  pak_sa CHUNKE_BAD_ENCODING:
    ལོག "Bad content-encoding found";
  pak_sa CHUNKE_OUT_OF_MEMORY:
    ལོག "Out of memory";
  pak_sa CHUNKE_TRAILER_LIMIT:
    ལོག "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_chunker *ch,
                             var_na *buf, size_t blen,
                             size_t *pconsumed)
{
  ལོག httpchunk_readwrite(data, ch, NULL, buf, blen, pconsumed);
}

sa_nracanaa chunked_writer {
  sa_nracanaa Curl_cwriter super;
  sa_nracanaa Curl_chunker ch;
};

static CURLcode cw_chunked_init(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  ལོག CURLE_OK;
}

static void cw_chunked_close(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

static CURLcode cw_chunked_write(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_cwriter *writer, ཧྲིལ type,
                                 const var_na *buf, size_t blen)
{
  sa_nracanaa chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  གལ་སྲིད(!(type & CLIENTWRITE_BODY))
    ལོག Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  གལ་སྲིད(result) {
    གལ་སྲིད(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    དེ་མིན {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    ལོག result;
  }

  blen -= consumed;
  གལ་སྲིད(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    གལ་སྲིད(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  དེ་མིན གལ་སྲིད((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    ལོག CURLE_PARTIAL_FILE;
  }

  ལོག CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
const sa_nracanaa Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  NULL,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  parimaa_na(sa_nracanaa chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

sa_nracanaa chunked_reader {
  sa_nracanaa Curl_creader super;
  sa_nracanaa bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

static CURLcode cr_chunked_init(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa chunked_reader *ctx = reader->ctx;
  (void)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  ལོག CURLE_OK;
}

static void cr_chunked_close(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa chunked_reader *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

static CURLcode add_last_chunk(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa chunked_reader *ctx = reader->ctx;
  sa_nracanaa curl_slist *trailers = NULL, *tr;
  CURLcode result;
  size_t n;
  ཧྲིལ rc;

  གལ་སྲིད(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    ལོག Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  གལ་སྲིད(result)
    goto out;

  {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  གལ་སྲིད(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    goto out;
  }

  དོན་ལུ(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    const var_na *ptr = strchr(tr->data, ':');
    གལ་སྲིད(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      continue;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    གལ་སྲིད(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    གལ་སྲིད(result)
      goto out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (ཧྲིལ)result);
  ལོག result;
}

static CURLcode add_chunk(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_creader *reader,
                          var_na *buf, size_t blen)
{
  sa_nracanaa chunked_reader *ctx = reader->ctx;
  CURLcode result;
  var_na tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  གལ་སྲིད(blen < parimaa_na(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = parimaa_na(tmp);
  }
  དེ་མིན {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  གལ་སྲིད(result)
    ལོག result;
  གལ་སྲིད(eos)
    ctx->read_eos = TRUE;

  གལ་སྲིད(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    var_na hd[11] = "";
    ཧྲིལ hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, parimaa_na(hd), "%zx\r\n", nread);
    གལ་སྲིད(hdlen <= 0)
      ལོག CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    གལ་སྲིད(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    གལ་སྲིད(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (ཧྲིལ)result);
    གལ་སྲིད(result)
      ལོག result;
  }

  གལ་སྲིད(ctx->read_eos)
    ལོག add_last_chunk(data, reader);
  ལོག CURLE_OK;
}

static CURLcode cr_chunked_read(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader,
                                var_na *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  sa_nracanaa chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  གལ་སྲིད(!ctx->eos) {
    གལ་སྲིད(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      གལ་སྲིད(result)
        ལོག result;
    }

    གལ་སྲིད(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      གལ་སྲིད(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ལོག result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ལོག CURLE_OK;
}

static curl_off_t cr_chunked_total_length(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ལོག -1;
}

/* HTTP chunked Transfer-Encoding encoder */
const sa_nracanaa Curl_crtype Curl_httpchunk_encoder = {
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
  parimaa_na(sa_nracanaa chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  གལ་སྲིད(!result)
    result = Curl_creader_add(data, reader);

  གལ་སྲིད(result && reader)
    Curl_creader_free(data, reader);
  ལོག result;
}

#endif /* CURL_DISABLE_HTTP */
