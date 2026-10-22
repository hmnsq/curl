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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

#అన్తర్భావ "urldata.h" /* it includes http_chunks.h */
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "http.h"    /* for Curl_verify_header */
#అన్తర్భావ "sendf.h"   /* for the client write stuff */
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curlx/strparse.h"

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

శూన్య Curl_httpchunk_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_chunker *ch,
                         bool ignore_body, bool లోపల_connect)
{
  (శూన్య)దత్తాంశం;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->చివరి_సంకేతం = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->లోపల_connect = లోపల_connect;
}

శూన్య Curl_httpchunk_reset(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_chunker *ch,
                          bool ignore_body)
{
  (శూన్య)దత్తాంశం;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->చివరి_సంకేతం = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

శూన్య Curl_httpchunk_free(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_chunker *ch)
{
  (శూన్య)దత్తాంశం;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_chunker *ch)
{
  (శూన్య)దత్తాంశం;
  తిరిగి ch->state == CHUNK_పూర్తి;
}

స్థిర CURLcode httpchunk_readwrite(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_chunker *ch,
                                    నిర్మాణం Curl_cwriter *cw_తదుపరి,
                                    స్థిరరాశి అక్షరం *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  ఒకవేళ(ch->state == CHUNK_పూర్తి)
    తిరిగి CURLE_OK;
  ఒకవేళ(ch->state == CHUNK_FAILED)
    తిరిగి CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  ఒకవేళ(దత్తాంశం->సమితి.http_te_skip && !ch->ignore_body) {
    ఒకవేళ(cw_తదుపరి)
      ఫలితం = Curl_cwriter_write(దత్తాంశం, cw_తదుపరి, CLIENTWRITE_BODY, buf, blen);
    లేకపోతే
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, buf, blen);
    ఒకవేళ(ఫలితం) {
      ch->state = CHUNK_FAILED;
      ch->చివరి_సంకేతం = CHUNKE_PASSTHRU_ERROR;
      తిరిగి ఫలితం;
    }
  }

  అయితే(blen) {
    ఎంపిక(ch->state) {
    స్థితి CHUNK_HEX:
      ఒకవేళ(ISXDIGIT(*buf)) {
        ఒకవేళ(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(దత్తాంశం, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->చివరి_సంకేతం = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          తిరిగి CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      లేకపోతే {
        స్థిరరాశి అక్షరం *p;
        ఒకవేళ(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(దత్తాంశం, "chunk hex-length char not a hex digit: 0x%x",
                (గుర్తులేని పూర్ణాంకం)*buf);
          ch->state = CHUNK_FAILED;
          ch->చివరి_సంకేతం = CHUNKE_ILLEGAL_HEX;
          తిరిగి CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        ఒకవేళ(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(దత్తాంశం, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->చివరి_సంకేతం = CHUNKE_ILLEGAL_HEX;
          తిరిగి CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      ఆపు;

    స్థితి CHUNK_LF:
      /* waiting for the LF after a chunk size */
      ఒకవేళ(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        ఒకవేళ(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        లేకపోతే {
          ch->state = CHUNK_దత్తాంశం;
          CURL_TRC_WRITE(దత్తాంశం, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      ఆపు;

    స్థితి CHUNK_దత్తాంశం:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      ఒకవేళ(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      ఒకవేళ(!దత్తాంశం->సమితి.http_te_skip && !ch->ignore_body) {
        ఒకవేళ(cw_తదుపరి)
          ఫలితం = Curl_cwriter_write(దత్తాంశం, cw_తదుపరి, CLIENTWRITE_BODY,
                                      buf, piece);
        లేకపోతే
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, buf, piece);
        ఒకవేళ(ఫలితం) {
          ch->state = CHUNK_FAILED;
          ch->చివరి_సంకేతం = CHUNKE_PASSTHRU_ERROR;
          తిరిగి ఫలితం;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(దత్తాంశం, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      ఒకవేళ(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      ఆపు;

    స్థితి CHUNK_POSTLF:
      ఒకవేళ(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(దత్తాంశం, ch, (bool)ch->ignore_body);
      }
      లేకపోతే ఒకవేళ(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->చివరి_సంకేతం = CHUNKE_BAD_CHUNK;
        తిరిగి CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ఆపు;

    స్థితి CHUNK_TRAILER:
      ఒకవేళ((*buf == 0x0d) || (*buf == 0x0a)) {
        స్థిరరాశి అక్షరం *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        ఒకవేళ(tr) {
          size_t trlen;
          ఫలితం = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          ఒకవేళ(ఫలితం) {
            ch->state = CHUNK_FAILED;
            ch->చివరి_సంకేతం = CHUNKE_బహిర్గతం_యొక్క_జ్ఞాపకం;
            తిరిగి ఫలితం;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          ఫలితం = Curl_verify_శీర్షిక(దత్తాంశం, tr, trlen);
          ఒకవేళ(ఫలితం) {
            ch->state = CHUNK_FAILED;
            ch->చివరి_సంకేతం = CHUNKE_BAD_CHUNK;
            తిరిగి ఫలితం;
          }

          ఒకవేళ(!దత్తాంశం->సమితి.http_te_skip) {
            పూర్ణాంకం hd_type = CLIENTWRITE_శీర్షిక | CLIENTWRITE_TRAILER;
            ఒకవేళ(ch->లోపల_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            ఒకవేళ(cw_తదుపరి)
              ఫలితం = Curl_cwriter_write(దత్తాంశం, cw_తదుపరి, hd_type, tr, trlen);
            లేకపోతే
              ఫలితం = Curl_client_write(దత్తాంశం, hd_type, tr, trlen);
            CURL_TRC_WRITE(దత్తాంశం, "wrote trailer '%s'", tr);
            ఒకవేళ(ఫలితం) {
              ch->state = CHUNK_FAILED;
              ch->చివరి_సంకేతం = CHUNKE_PASSTHRU_ERROR;
              తిరిగి ఫలితం;
            }
            ఫలితం = Curl_bump_headersize(దత్తాంశం, trlen, FALSE);
            ఒకవేళ(ఫలితం) {
              ch->state = CHUNK_FAILED;
              ch->చివరి_సంకేతం = CHUNKE_TRAILER_హద్దు;
              తిరిగి ఫలితం;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          ఒకవేళ(*buf == 0x0a)
            /* already on the LF */
            ఆపు;
        }
        లేకపోతే {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          ఆపు; /* do not advance the pointer */
        }
      }
      లేకపోతే {
        ఫలితం = curlx_dyn_addn(&ch->trailer, buf, 1);
        ఒకవేళ(ఫలితం) {
          ch->state = CHUNK_FAILED;
          ch->చివరి_సంకేతం = CHUNKE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          తిరిగి ఫలితం;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      ఆపు;

    స్థితి CHUNK_TRAILER_CR:
      ఒకవేళ(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      లేకపోతే {
        ch->state = CHUNK_FAILED;
        ch->చివరి_సంకేతం = CHUNKE_BAD_CHUNK;
        తిరిగి CURLE_RECV_ERROR;
      }
      ఆపు;

    స్థితి CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      ఒకవేళ((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        ఆపు;
      }
      ఒకవేళ(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      ఆపు;

    స్థితి CHUNK_STOP:
      ఒకవేళ(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_పూర్తి;
        CURL_TRC_WRITE(దత్తాంశం, "http_chunk, response complete");
        తిరిగి CURLE_OK;
      }
      లేకపోతే {
        ch->state = CHUNK_FAILED;
        ch->చివరి_సంకేతం = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(దత్తాంశం, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (గుర్తులేని పూర్ణాంకం)*buf);
        తిరిగి CURLE_RECV_ERROR;
      }
    స్థితి CHUNK_పూర్తి:
      తిరిగి CURLE_OK;

    స్థితి CHUNK_FAILED:
      తిరిగి CURLE_RECV_ERROR;
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి అక్షరం *Curl_chunked_strerror(CHUNKcode సంకేతం_2)
{
  ఎంపిక(సంకేతం_2) {
  అప్రమేయ:
    తిరిగి "OK";
  స్థితి CHUNKE_TOO_LONG_HEX:
    తిరిగి "Too long hexadecimal number";
  స్థితి CHUNKE_ILLEGAL_HEX:
    తిరిగి "Illegal or missing hexadecimal sequence";
  స్థితి CHUNKE_BAD_CHUNK:
    తిరిగి "Malformed encoding found";
  స్థితి CHUNKE_PASSTHRU_ERROR:
    తిరిగి "Error writing data to client";
  స్థితి CHUNKE_BAD_ENCODING:
    తిరిగి "Bad content-encoding found";
  స్థితి CHUNKE_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి "Out of memory";
  స్థితి CHUNKE_TRAILER_హద్దు:
    తిరిగి "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_chunker *ch,
                             అక్షరం *buf, size_t blen,
                             size_t *pconsumed)
{
  తిరిగి httpchunk_readwrite(దత్తాంశం, ch, ఖాళీ, buf, blen, pconsumed);
}

నిర్మాణం chunked_writer {
  నిర్మాణం Curl_cwriter super;
  నిర్మాణం Curl_chunker ch;
};

స్థిర CURLcode cw_chunked_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం chunked_writer *ctx = writer->ctx;

  దత్తాంశం->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(దత్తాంశం, &ctx->ch, FALSE, FALSE);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cw_chunked_close(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(దత్తాంశం, &ctx->ch);
}

స్థిర CURLcode cw_chunked_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                 స్థిరరాశి అక్షరం *buf, size_t blen)
{
  నిర్మాణం chunked_writer *ctx = writer->ctx;
  CURLcode ఫలితం;
  size_t consumed;

  ఒకవేళ(!(type & CLIENTWRITE_BODY))
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, blen);

  consumed = 0;
  ఫలితం = httpchunk_readwrite(దత్తాంశం, &ctx->ch, writer->తదుపరి, buf, blen,
                               &consumed);

  ఒకవేళ(ఫలితం) {
    ఒకవేళ(CHUNKE_PASSTHRU_ERROR == ctx->ch.చివరి_సంకేతం) {
      failf(దత్తాంశం, "Failed reading the chunked-encoded stream");
    }
    లేకపోతే {
      failf(దత్తాంశం, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.చివరి_సంకేతం));
    }
    తిరిగి ఫలితం;
  }

  blen -= consumed;
  ఒకవేళ(CHUNK_పూర్తి == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    దత్తాంశం->req.download_పూర్తి = TRUE;
    ఒకవేళ(blen) {
      infof(దత్తాంశం, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  లేకపోతే ఒకవేళ((type & CLIENTWRITE_EOS) && !దత్తాంశం->req.no_body) {
    failf(దత్తాంశం, "transfer closed with outstanding read data remaining");
    తిరిగి CURLE_PARTIAL_FILE;
  }

  తిరిగి CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
స్థిరరాశి నిర్మాణం Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  ఖాళీ,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  పరిమాణం(నిర్మాణం chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#నిర్వచించు CURL_CHUNKED_MINLEN   1024
#నిర్వచించు CURL_CHUNKED_MAXLEN   (64 * 1024)

నిర్మాణం chunked_reader {
  నిర్మాణం Curl_creader super;
  నిర్మాణం bufq chunkbuf;
  బిట్టు(read_eos);  /* we read an EOS from the next reader */
  బిట్టు(eos);       /* we have returned an EOS */
};

స్థిర CURLcode cr_chunked_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం chunked_reader *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_హద్దు);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cr_chunked_close(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం chunked_reader *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_free(&ctx->chunkbuf);
}

స్థిర CURLcode కూడు_చివరి_chunk(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం chunked_reader *ctx = reader->ctx;
  నిర్మాణం curl_slist *trailers = ఖాళీ, *tr;
  CURLcode ఫలితం;
  size_t n;
  పూర్ణాంకం rc;

  ఒకవేళ(!దత్తాంశం->సమితి.trailer_callback) {
    CURL_TRC_READ(దత్తాంశం, "http_chunk, added last, empty chunk");
    తిరిగి Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  {
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_trailer_callback);
    rc = దత్తాంశం->సమితి.trailer_callback(&trailers, దత్తాంశం->సమితి.trailer_దత్తాంశం);
    CURL_CBAPI_చివర(&guard);
  }

  ఒకవేళ(rc != CURL_TRAILERFUNC_OK) {
    failf(దత్తాంశం, "operation aborted by trailing headers callback");
    ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    వెళ్ళు బహిర్గతం;
  }

  కొరకు(tr = trailers; tr; tr = tr->తదుపరి) {
    /* only add correctly formatted trailers */
    స్థిరరాశి అక్షరం *ptr = strchr(tr->దత్తాంశం, ':');
    ఒకవేళ(!ptr || *(ptr + 1) != ' ') {
      infof(దత్తాంశం, "Malformatted trailing header, skipping trailer");
      కొనసాగు;
    }

    ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, tr->దత్తాంశం, అక్షరశ్రేణి_పొడవు(tr->దత్తాంశం), &n);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

బహిర్గతం:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(దత్తాంశం, "http_chunk, added last chunk with trailers "
                "from client -> %d", (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode కూడు_chunk(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_creader *reader,
                          అక్షరం *buf, size_t blen)
{
  నిర్మాణం chunked_reader *ctx = reader->ctx;
  CURLcode ఫలితం;
  అక్షరం tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  ఒకవేళ(blen < పరిమాణం(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = పరిమాణం(tmp);
  }
  లేకపోతే {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  ఫలితం = Curl_creader_read(దత్తాంశం, reader->తదుపరి, buf, blen, &nread, &eos);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(eos)
    ctx->read_eos = TRUE;

  ఒకవేళ(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    అక్షరం hd[11] = "";
    పూర్ణాంకం hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, పరిమాణం(hd), "%zx\r\n", nread);
    ఒకవేళ(hdlen <= 0)
      తిరిగి CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(దత్తాంశం, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(ctx->read_eos)
    తిరిగి కూడు_చివరి_chunk(దత్తాంశం, reader);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cr_chunked_read(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_creader *reader,
                                అక్షరం *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  నిర్మాణం chunked_reader *ctx = reader->ctx;
  CURLcode ఫలితం = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  ఒకవేళ(!ctx->eos) {
    ఒకవేళ(!ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      ఫలితం = కూడు_chunk(దత్తాంశం, reader, buf, blen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->chunkbuf)) {
      ఫలితం = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      ఒకవేళ(!ఫలితం && ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      తిరిగి ఫలితం;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  తిరిగి CURLE_OK;
}

స్థిర curl_off_t cr_chunked_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి -1;
}

/* HTTP chunked Transfer-Encoding encoder */
స్థిరరాశి నిర్మాణం Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం chunked_reader)
};

CURLcode Curl_httpchunk_కూడు_reader(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = ఖాళీ;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&reader, దత్తాంశం, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_కూడు(దత్తాంశం, reader);

  ఒకవేళ(ఫలితం && reader)
    Curl_creader_free(దత్తాంశం, reader);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* CURL_DISABLE_HTTP */
