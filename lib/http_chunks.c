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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাব "urldata.h" /* it includes http_chunks.h */
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "http.h"    /* for Curl_verify_header */
#অন্তর্ভাব "sendf.h"   /* for the client write stuff */
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curlx/strparse.h"

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

শূন্য Curl_httpchunk_init(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_chunker *ch,
                         bool ignore_body, bool in_connect)
{
  (শূন্য)তথ্য;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->in_connect = in_connect;
}

শূন্য Curl_httpchunk_reset(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_chunker *ch,
                          bool ignore_body)
{
  (শূন্য)তথ্য;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->last_code = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

শূন্য Curl_httpchunk_free(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_chunker *ch)
{
  (শূন্য)তথ্য;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_done(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_chunker *ch)
{
  (শূন্য)তথ্য;
  ফেরত ch->state == CHUNK_DONE;
}

স্থির CURLcode httpchunk_readwrite(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো Curl_chunker *ch,
                                    কাঠামো Curl_cwriter *cw_পরবর্তী,
                                    ধ্রুবক অক্ষর *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  যদি(ch->state == CHUNK_DONE)
    ফেরত CURLE_OK;
  যদি(ch->state == CHUNK_FAILED)
    ফেরত CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  যদি(তথ্য->set.http_te_skip && !ch->ignore_body) {
    যদি(cw_পরবর্তী)
      ফলাফল = Curl_cwriter_write(তথ্য, cw_পরবর্তী, CLIENTWRITE_BODY, buf, blen);
    অন্যথা
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, buf, blen);
    যদি(ফলাফল) {
      ch->state = CHUNK_FAILED;
      ch->last_code = CHUNKE_PASSTHRU_ERROR;
      ফেরত ফলাফল;
    }
  }

  যখন(blen) {
    নির্বাচন(ch->state) {
    ক্ষেত্র CHUNK_HEX:
      যদি(ISXDIGIT(*buf)) {
        যদি(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(তথ্য, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          ফেরত CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      অন্যথা {
        ধ্রুবক অক্ষর *p;
        যদি(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(তথ্য, "chunk hex-length char not a hex digit: 0x%x",
                (unsigned পূর্ণসংখ্যা)*buf);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ফেরত CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        যদি(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(তথ্য, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_ILLEGAL_HEX;
          ফেরত CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      বিরতি;

    ক্ষেত্র CHUNK_LF:
      /* waiting for the LF after a chunk size */
      যদি(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        যদি(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        অন্যথা {
          ch->state = CHUNK_তথ্য;
          CURL_TRC_WRITE(তথ্য, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      বিরতি;

    ক্ষেত্র CHUNK_তথ্য:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      যদি(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      যদি(!তথ্য->set.http_te_skip && !ch->ignore_body) {
        যদি(cw_পরবর্তী)
          ফলাফল = Curl_cwriter_write(তথ্য, cw_পরবর্তী, CLIENTWRITE_BODY,
                                      buf, piece);
        অন্যথা
          ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, buf, piece);
        যদি(ফলাফল) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_PASSTHRU_ERROR;
          ফেরত ফলাফল;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(তথ্য, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      যদি(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      বিরতি;

    ক্ষেত্র CHUNK_POSTLF:
      যদি(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(তথ্য, ch, (bool)ch->ignore_body);
      }
      অন্যথা যদি(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ফেরত CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      বিরতি;

    ক্ষেত্র CHUNK_TRAILER:
      যদি((*buf == 0x0d) || (*buf == 0x0a)) {
        ধ্রুবক অক্ষর *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        যদি(tr) {
          size_t trlen;
          ফলাফল = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          যদি(ফলাফল) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_OUT_OF_MEMORY;
            ফেরত ফলাফল;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          ফলাফল = Curl_verify_header(তথ্য, tr, trlen);
          যদি(ফলাফল) {
            ch->state = CHUNK_FAILED;
            ch->last_code = CHUNKE_BAD_CHUNK;
            ফেরত ফলাফল;
          }

          যদি(!তথ্য->set.http_te_skip) {
            পূর্ণসংখ্যা hd_type = CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER;
            যদি(ch->in_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            যদি(cw_পরবর্তী)
              ফলাফল = Curl_cwriter_write(তথ্য, cw_পরবর্তী, hd_type, tr, trlen);
            অন্যথা
              ফলাফল = Curl_client_write(তথ্য, hd_type, tr, trlen);
            CURL_TRC_WRITE(তথ্য, "wrote trailer '%s'", tr);
            যদি(ফলাফল) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_PASSTHRU_ERROR;
              ফেরত ফলাফল;
            }
            ফলাফল = Curl_bump_headersize(তথ্য, trlen, FALSE);
            যদি(ফলাফল) {
              ch->state = CHUNK_FAILED;
              ch->last_code = CHUNKE_TRAILER_LIMIT;
              ফেরত ফলাফল;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          যদি(*buf == 0x0a)
            /* already on the LF */
            বিরতি;
        }
        অন্যথা {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          বিরতি; /* do not advance the pointer */
        }
      }
      অন্যথা {
        ফলাফল = curlx_dyn_addn(&ch->trailer, buf, 1);
        যদি(ফলাফল) {
          ch->state = CHUNK_FAILED;
          ch->last_code = CHUNKE_OUT_OF_MEMORY;
          ফেরত ফলাফল;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      বিরতি;

    ক্ষেত্র CHUNK_TRAILER_CR:
      যদি(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      অন্যথা {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        ফেরত CURLE_RECV_ERROR;
      }
      বিরতি;

    ক্ষেত্র CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      যদি((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        বিরতি;
      }
      যদি(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      বিরতি;

    ক্ষেত্র CHUNK_STOP:
      যদি(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_DONE;
        CURL_TRC_WRITE(তথ্য, "http_chunk, response complete");
        ফেরত CURLE_OK;
      }
      অন্যথা {
        ch->state = CHUNK_FAILED;
        ch->last_code = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(তথ্য, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (unsigned পূর্ণসংখ্যা)*buf);
        ফেরত CURLE_RECV_ERROR;
      }
    ক্ষেত্র CHUNK_DONE:
      ফেরত CURLE_OK;

    ক্ষেত্র CHUNK_FAILED:
      ফেরত CURLE_RECV_ERROR;
    }
  }
  ফেরত CURLE_OK;
}

স্থির ধ্রুবক অক্ষর *Curl_chunked_strerror(CHUNKcode code)
{
  নির্বাচন(code) {
  পূর্বনির্ধারিত:
    ফেরত "OK";
  ক্ষেত্র CHUNKE_TOO_LONG_HEX:
    ফেরত "Too long hexadecimal number";
  ক্ষেত্র CHUNKE_ILLEGAL_HEX:
    ফেরত "Illegal or missing hexadecimal sequence";
  ক্ষেত্র CHUNKE_BAD_CHUNK:
    ফেরত "Malformed encoding found";
  ক্ষেত্র CHUNKE_PASSTHRU_ERROR:
    ফেরত "Error writing data to client";
  ক্ষেত্র CHUNKE_BAD_ENCODING:
    ফেরত "Bad content-encoding found";
  ক্ষেত্র CHUNKE_OUT_OF_MEMORY:
    ফেরত "Out of memory";
  ক্ষেত্র CHUNKE_TRAILER_LIMIT:
    ফেরত "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_chunker *ch,
                             অক্ষর *buf, size_t blen,
                             size_t *pconsumed)
{
  ফেরত httpchunk_readwrite(তথ্য, ch, ফাঁকা, buf, blen, pconsumed);
}

কাঠামো chunked_writer {
  কাঠামো Curl_cwriter super;
  কাঠামো Curl_chunker ch;
};

স্থির CURLcode cw_chunked_init(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer)
{
  কাঠামো chunked_writer *ctx = writer->ctx;

  তথ্য->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(তথ্য, &ctx->ch, FALSE, FALSE);
  ফেরত CURLE_OK;
}

স্থির শূন্য cw_chunked_close(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer)
{
  কাঠামো chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(তথ্য, &ctx->ch);
}

স্থির CURLcode cw_chunked_write(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                 ধ্রুবক অক্ষর *buf, size_t blen)
{
  কাঠামো chunked_writer *ctx = writer->ctx;
  CURLcode ফলাফল;
  size_t consumed;

  যদি(!(type & CLIENTWRITE_BODY))
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, blen);

  consumed = 0;
  ফলাফল = httpchunk_readwrite(তথ্য, &ctx->ch, writer->পরবর্তী, buf, blen,
                               &consumed);

  যদি(ফলাফল) {
    যদি(CHUNKE_PASSTHRU_ERROR == ctx->ch.last_code) {
      failf(তথ্য, "Failed reading the chunked-encoded stream");
    }
    অন্যথা {
      failf(তথ্য, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.last_code));
    }
    ফেরত ফলাফল;
  }

  blen -= consumed;
  যদি(CHUNK_DONE == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    তথ্য->req.download_done = TRUE;
    যদি(blen) {
      infof(তথ্য, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  অন্যথা যদি((type & CLIENTWRITE_EOS) && !তথ্য->req.no_body) {
    failf(তথ্য, "transfer closed with outstanding read data remaining");
    ফেরত CURLE_PARTIAL_FILE;
  }

  ফেরত CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ধ্রুবক কাঠামো Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ফাঁকা,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  আকার(কাঠামো chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#define CURL_CHUNKED_MINLEN   1024
#define CURL_CHUNKED_MAXLEN   (64 * 1024)

কাঠামো chunked_reader {
  কাঠামো Curl_creader super;
  কাঠামো bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_chunked_init(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_creader *reader)
{
  কাঠামো chunked_reader *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_LIMIT);
  ফেরত CURLE_OK;
}

স্থির শূন্য cr_chunked_close(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader)
{
  কাঠামো chunked_reader *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_free(&ctx->chunkbuf);
}

স্থির CURLcode add_last_chunk(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creader *reader)
{
  কাঠামো chunked_reader *ctx = reader->ctx;
  কাঠামো curl_slist *trailers = ফাঁকা, *tr;
  CURLcode ফলাফল;
  size_t n;
  পূর্ণসংখ্যা rc;

  যদি(!তথ্য->set.trailer_callback) {
    CURL_TRC_READ(তথ্য, "http_chunk, added last, empty chunk");
    ফেরত Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  যদি(ফলাফল)
    যাও out;

  {
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_trailer_callback);
    rc = তথ্য->set.trailer_callback(&trailers, তথ্য->set.trailer_তথ্য);
    CURL_CBAPI_END(&guard);
  }

  যদি(rc != CURL_TRAILERFUNC_OK) {
    failf(তথ্য, "operation aborted by trailing headers callback");
    ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    যাও out;
  }

  জন্য(tr = trailers; tr; tr = tr->পরবর্তী) {
    /* only add correctly formatted trailers */
    ধ্রুবক অক্ষর *ptr = strchr(tr->তথ্য, ':');
    যদি(!ptr || *(ptr + 1) != ' ') {
      infof(তথ্য, "Malformatted trailing header, skipping trailer");
      চালু;
    }

    ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, tr->তথ্য, strlen(tr->তথ্য), &n);
    যদি(!ফলাফল)
      ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    যদি(ফলাফল)
      যাও out;
  }

  ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

out:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(তথ্য, "http_chunk, added last chunk with trailers "
                "from client -> %d", (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

স্থির CURLcode add_chunk(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_creader *reader,
                          অক্ষর *buf, size_t blen)
{
  কাঠামো chunked_reader *ctx = reader->ctx;
  CURLcode ফলাফল;
  অক্ষর tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  যদি(blen < আকার(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = আকার(tmp);
  }
  অন্যথা {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  ফলাফল = Curl_creader_read(তথ্য, reader->পরবর্তী, buf, blen, &nread, &eos);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(eos)
    ctx->read_eos = TRUE;

  যদি(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    অক্ষর hd[11] = "";
    পূর্ণসংখ্যা hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, আকার(hd), "%zx\r\n", nread);
    যদি(hdlen <= 0)
      ফেরত CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    যদি(!ফলাফল)
      ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    যদি(!ফলাফল)
      ফলাফল = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(তথ্য, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(ctx->read_eos)
    ফেরত add_last_chunk(তথ্য, reader);
  ফেরত CURLE_OK;
}

স্থির CURLcode cr_chunked_read(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_creader *reader,
                                অক্ষর *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  কাঠামো chunked_reader *ctx = reader->ctx;
  CURLcode ফলাফল = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  যদি(!ctx->eos) {
    যদি(!ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      ফলাফল = add_chunk(তথ্য, reader, buf, blen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }

    যদি(!Curl_bufq_is_empty(&ctx->chunkbuf)) {
      ফলাফল = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      যদি(!ফলাফল && ctx->read_eos && Curl_bufq_is_empty(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      ফেরত ফলাফল;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  ফেরত CURLE_OK;
}

স্থির curl_off_t cr_chunked_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ধ্রুবক কাঠামো Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো chunked_reader)
};

CURLcode Curl_httpchunk_add_reader(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = ফাঁকা;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&reader, তথ্য, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_add(তথ্য, reader);

  যদি(ফলাফল && reader)
    Curl_creader_free(তথ্য, reader);
  ফেরত ফলাফল;
}

#endif /* CURL_DISABLE_HTTP */
