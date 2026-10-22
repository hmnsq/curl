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

void Curl_httpchunk_init(struct Curl_easy *data, struct Curl_chunker *ch,
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

void Curl_httpchunk_reset(struct Curl_easy *data, struct Curl_chunker *ch,
                          bool ignore_body)
{
  (void)data;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

void Curl_httpchunk_free(struct Curl_easy *data, struct Curl_chunker *ch)
{
  (void)data;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(struct Curl_easy *data, struct Curl_chunker *ch)
{
  (void)data;
  trả_về ch->state == CHUNK_DONE;
}

static CURLcode httpchunk_readwrite(struct Curl_easy *data,
                                    struct Curl_chunker *ch,
                                    struct Curl_cwriter *cw_next,
                                    const char *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  nếu(ch->state == CHUNK_DONE)
    trả_về CURLE_OK;
  nếu(ch->state == CHUNK_FAILED)
    trả_về CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  nếu(data->set.http_te_skip && !ch->ignore_body) {
    nếu(cw_next)
      result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY, buf, blen);
    không_thì
      result = Curl_client_write(data, CLIENTWRITE_BODY, buf, blen);
    nếu(result) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      trả_về result;
    }
  }

  trong_khi(blen) {
    switch(ch->state) {
    case CHUNK_HEX:
      nếu(ISXDIGIT(*buf)) {
        nếu(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(data, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          trả_về CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      không_thì {
        const char *p;
        nếu(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(data, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned số_nguyên)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          trả_về CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        nếu(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(data, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          trả_về CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      break;

    case CHUNK_LF:
      /* waiting for the LF after a chunk size */
      nếu(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        nếu(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        không_thì {
          ch->state = CHUNK_DATA;
          CURL_TRC_WRITE(data, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      break;

    case CHUNK_DATA:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      nếu(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      nếu(!data->set.http_te_skip && !ch->ignore_body) {
        nếu(cw_next)
          result = Curl_cwriter_write(data, cw_next, CLIENTWRITE_BODY,
                                      buf, piece);
        không_thì
          result = Curl_client_write(data, CLIENTWRITE_BODY, buf, piece);
        nếu(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          trả_về result;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(data, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      nếu(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      break;

    case CHUNK_POSTLF:
      nếu(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(data, ch, (bool)ch->ignore_body);
      }
      không_thì nếu(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        trả_về CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      break;

    case CHUNK_TRAILER:
      nếu((*buf == 0x0d) || (*buf == 0x0a)) {
        const char *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        nếu(tr) {
          size_t trlen;
          result = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          nếu(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            trả_về result;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          result = Curl_verify_header(data, tr, trlen);
          nếu(result) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            trả_về result;
          }

          nếu(!data->set.http_te_skip) {
            số_nguyên hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            nếu(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            nếu(cw_next)
              result = Curl_cwriter_write(data, cw_next, hd_type, tr, trlen);
            không_thì
              result = Curl_client_write(data, hd_type, tr, trlen);
            CURL_TRC_WRITE(data, "wrote trailer '%s'", tr);
            nếu(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              trả_về result;
            }
            result = Curl_bump_headersize(data, trlen, FALSE);
            nếu(result) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              trả_về result;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          nếu(*buf == 0x0a)
            /* already on the LF */
            break;
        }
        không_thì {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          break; /* do not advance the pointer */
        }
      }
      không_thì {
        result = curlx_dyn_addn(&ch->trailer, buf, 1);
        nếu(result) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          trả_về result;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      break;

    case CHUNK_TRAILER_CR:
      nếu(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      không_thì {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        trả_về CURLE_RECV_ERROR;
      }
      break;

    case CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      nếu((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        break;
      }
      nếu(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      break;

    case CHUNK_STOP:
      nếu(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(data, "http_chunk, response complete");
        trả_về CURLE_OK;
      }
      không_thì {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(data, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned số_nguyên)*buf);
        trả_về CURLE_RECV_ERROR;
      }
    case CHUNK_DONE:
      trả_về CURLE_OK;

    case CHUNK_FAILED:
      trả_về CURLE_RECV_ERROR;
    }
  }
  trả_về CURLE_OK;
}

static const char *Curl_chunked_strerror(CHUNKcode code)
{
  switch(code) {
  default:
    trả_về "OK";
  case CHUNKE_TOO_LONG_HEX:
    trả_về "Too long hexadecimal number";
  case CHUNKE_ILLEGAL_HEX:
    trả_về "Illegal or missing hexadecimal sequence";
  case CHUNKE_BAD_CHUNK:
    trả_về "Malformed encoding found";
  case CHUNKE_PASSTHRU_ERROR:
    trả_về "Error writing data to client";
  case CHUNKE_BAD_ENCODING:
    trả_về "Bad content-encoding found";
  case CHUNKE_OUT_OF_MEMORY:
    trả_về "Out of memory";
  case CHUNKE_TRAILER_LIMIT:
    trả_về "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(struct Curl_easy *data,
                             struct Curl_chunker *ch,
                             char *buf, size_t blen,
                             size_t *pconsumed)
{
  trả_về httpchunk_readwrite(data, ch, NULL, buf, blen, pconsumed);
}

struct chunked_writer {
  struct Curl_cwriter super;
  struct Curl_chunker ch;
};

static CURLcode cw_chunked_init(struct Curl_easy *data,
                                struct Curl_cwriter *writer)
{
  struct chunked_writer *ctx = writer->ctx;

  data->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(data, &ctx->ch, FALSE, FALSE);
  trả_về CURLE_OK;
}

static void cw_chunked_close(struct Curl_easy *data,
                             struct Curl_cwriter *writer)
{
  struct chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(data, &ctx->ch);
}

static CURLcode cw_chunked_write(struct Curl_easy *data,
                                 struct Curl_cwriter *writer, số_nguyên type,
                                 const char *buf, size_t blen)
{
  struct chunked_writer *ctx = writer->ctx;
  CURLcode result;
  size_t consumed;

  nếu(!(type & CLIENTWRITE_BODY))
    trả_về Curl_cwriter_write(data, writer->next, type, buf, blen);

  consumed = 0;
  result = httpchunk_readwrite(data, &ctx->ch, writer->next, buf, blen,
                               &consumed);

  nếu(result) {
    nếu(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(data, "Failed reading the chunked-encoded stream");
    }
    không_thì {
      failf(data, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    trả_về result;
  }

  blen -= consumed;
  nếu(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    data->req.download_done = TRUE;
    nếu(blen) {
      infof(data, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  không_thì nếu((type & CLIENTWRITE_EOS) && !data->req.no_body) {
    failf(data, "transfer closed with outstanding read data remaining");
    trả_về CURLE_PARTIAL_FILE;
  }

  trả_về CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
const struct Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  NULL,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  sizeof(struct chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

struct chunked_reader {
  struct Curl_creader super;
  struct bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

static CURLcode cr_chunked_init(struct Curl_easy *data,
                                struct Curl_creader *reader)
{
  struct chunked_reader *ctx = reader->ctx;
  (void)data;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  trả_về CURLE_OK;
}

static void cr_chunked_close(struct Curl_easy *data,
                             struct Curl_creader *reader)
{
  struct chunked_reader *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->chunkbuf);
}

static CURLcode add_last_chunk(struct Curl_easy *data,
                               struct Curl_creader *reader)
{
  struct chunked_reader *ctx = reader->ctx;
  struct curl_slist *trailers = NULL, *tr;
  CURLcode result;
  size_t n;
  số_nguyên rc;

  nếu(!data->set.trailer_callback) {
    CURL_TRC_READ(data, "http_chunk, added last, empty chunk");
    trả_về Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  nếu(result)
    goto out;

  {
    struct Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_trailer_callback);
    rc = data->set.trailer_callback(&trailers, data->set.trailer_data);
    CURL_CBAPI_END(&guard);
  }

  nếu(rc != CURL_TRAILERFUNC_OK) {
    failf(data, "operation aborted by trailing headers callback");
    result = CURLE_ABORTED_BY_CALLBACK;
    goto out;
  }

  cho(tr = trailers; tr; tr = tr->next) {
    /* only add correctly formatted trailers */
    const char *ptr = strchr(tr->data, ':');
    nếu(!ptr || *(ptr + 1) != ' ') {
      infof(data, "Malformatted trailing header, skipping trailer");
      continue;
    }

    result = Curl_bufq_cwrite(&ctx->chunkbuf, tr->data, strlen(tr->data), &n);
    nếu(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    nếu(result)
      goto out;
  }

  result = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(data, "http_chunk, added last chunk with trailers "
                "from client -> %d", (số_nguyên)result);
  trả_về result;
}

static CURLcode add_chunk(struct Curl_easy *data,
                          struct Curl_creader *reader,
                          char *buf, size_t blen)
{
  struct chunked_reader *ctx = reader->ctx;
  CURLcode result;
  char tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  nếu(blen < sizeof(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = sizeof(tmp);
  }
  không_thì {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
  nếu(result)
    trả_về result;
  nếu(eos)
    ctx->read_eos = TRUE;

  nếu(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    char hd[11] = "";
    số_nguyên hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, sizeof(hd), "%zx\r\n", nread);
    nếu(hdlen <= 0)
      trả_về CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    result = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    nếu(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    nếu(!result)
      result = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(data, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (số_nguyên)result);
    nếu(result)
      trả_về result;
  }

  nếu(ctx->read_eos)
    trả_về add_last_chunk(data, reader);
  trả_về CURLE_OK;
}

static CURLcode cr_chunked_read(struct Curl_easy *data,
                                struct Curl_creader *reader,
                                char *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  struct chunked_reader *ctx = reader->ctx;
  CURLcode result = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  nếu(!ctx->eos) {
    nếu(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      result = add_chunk(data, reader, buf, blen);
      nếu(result)
        trả_về result;
    }

    nếu(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      result = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      nếu(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      trả_về result;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  trả_về CURLE_OK;
}

static curl_off_t cr_chunked_total_length(struct Curl_easy *data,
                                          struct Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  trả_về -1;
}

/* HTTP chunked Transfer-Encoding encoder */
const struct Curl_crtype Curl_httpchunk_encoder = {
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
  sizeof(struct chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(struct Curl_easy *data)
{
  struct Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  nếu(!result)
    result = Curl_creader_add(data, reader);

  nếu(result && reader)
    Curl_creader_free(data, reader);
  trả_về result;
}

#endif /* CURL_DISABLE_HTTP */
