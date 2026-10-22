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

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

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

शून्य Curl_httpchunk_init(संरचना Curl_easy *दत्त, संरचना Curl_chunker *ch,
                         bool ignore_body, bool अन्तर्_connect)
{
  (शून्य)दत्त;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->चरमः_संहिता = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->अन्तर्_connect = अन्तर्_connect;
}

शून्य Curl_httpchunk_reset(संरचना Curl_easy *दत्त, संरचना Curl_chunker *ch,
                          bool ignore_body)
{
  (शून्य)दत्त;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->चरमः_संहिता = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

शून्य Curl_httpchunk_free(संरचना Curl_easy *दत्त, संरचना Curl_chunker *ch)
{
  (शून्य)दत्त;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_समाप्तम्(संरचना Curl_easy *दत्त, संरचना Curl_chunker *ch)
{
  (शून्य)दत्त;
  प्रत्यागम ch->state == CHUNK_समाप्तम्;
}

स्थिर CURLcode httpchunk_readwrite(संरचना Curl_easy *दत्त,
                                    संरचना Curl_chunker *ch,
                                    संरचना Curl_cwriter *cw_अग्र,
                                    ध्रुव वर्ण *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  यदि(ch->state == CHUNK_समाप्तम्)
    प्रत्यागम CURLE_OK;
  यदि(ch->state == CHUNK_FAILED)
    प्रत्यागम CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  यदि(दत्त->समुच्चयः.http_te_skip && !ch->ignore_body) {
    यदि(cw_अग्र)
      परिणाम = Curl_cwriter_write(दत्त, cw_अग्र, CLIENTWRITE_BODY, buf, blen);
    अन्यथा
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, buf, blen);
    यदि(परिणाम) {
      ch->state = CHUNK_FAILED;
      ch->चरमः_संहिता = CHUNKE_PASSTHRU_ERROR;
      प्रत्यागम परिणाम;
    }
  }

  यावत्(blen) {
    विकल्प(ch->state) {
    पक्ष CHUNK_HEX:
      यदि(ISXDIGIT(*buf)) {
        यदि(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(दत्त, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->चरमः_संहिता = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          प्रत्यागम CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        ध्रुव वर्ण *p;
        यदि(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(दत्त, "chunk hex-length char not a hex digit: 0x%x",
                (अचिह्नित पूर्णाङ्क)*buf);
          ch->state = CHUNK_FAILED;
          ch->चरमः_संहिता = CHUNKE_ILLEGAL_HEX;
          प्रत्यागम CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        यदि(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(दत्त, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->चरमः_संहिता = CHUNKE_ILLEGAL_HEX;
          प्रत्यागम CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      भङ्ग;

    पक्ष CHUNK_LF:
      /* waiting for the LF after a chunk size */
      यदि(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        यदि(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        अन्यथा {
          ch->state = CHUNK_दत्त;
          CURL_TRC_WRITE(दत्त, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_दत्त:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      यदि(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      यदि(!दत्त->समुच्चयः.http_te_skip && !ch->ignore_body) {
        यदि(cw_अग्र)
          परिणाम = Curl_cwriter_write(दत्त, cw_अग्र, CLIENTWRITE_BODY,
                                      buf, piece);
        अन्यथा
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, buf, piece);
        यदि(परिणाम) {
          ch->state = CHUNK_FAILED;
          ch->चरमः_संहिता = CHUNKE_PASSTHRU_ERROR;
          प्रत्यागम परिणाम;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(दत्त, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      यदि(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      भङ्ग;

    पक्ष CHUNK_POSTLF:
      यदि(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(दत्त, ch, (bool)ch->ignore_body);
      }
      अन्यथा यदि(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->चरमः_संहिता = CHUNKE_BAD_CHUNK;
        प्रत्यागम CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_TRAILER:
      यदि((*buf == 0x0d) || (*buf == 0x0a)) {
        ध्रुव वर्ण *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        यदि(tr) {
          size_t trlen;
          परिणाम = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          यदि(परिणाम) {
            ch->state = CHUNK_FAILED;
            ch->चरमः_संहिता = CHUNKE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
            प्रत्यागम परिणाम;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          परिणाम = Curl_verify_शीर्षकम्(दत्त, tr, trlen);
          यदि(परिणाम) {
            ch->state = CHUNK_FAILED;
            ch->चरमः_संहिता = CHUNKE_BAD_CHUNK;
            प्रत्यागम परिणाम;
          }

          यदि(!दत्त->समुच्चयः.http_te_skip) {
            पूर्णाङ्क hd_type = CLIENTWRITE_शीर्षकम् | CLIENTWRITE_TRAILER;
            यदि(ch->अन्तर्_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            यदि(cw_अग्र)
              परिणाम = Curl_cwriter_write(दत्त, cw_अग्र, hd_type, tr, trlen);
            अन्यथा
              परिणाम = Curl_client_write(दत्त, hd_type, tr, trlen);
            CURL_TRC_WRITE(दत्त, "wrote trailer '%s'", tr);
            यदि(परिणाम) {
              ch->state = CHUNK_FAILED;
              ch->चरमः_संहिता = CHUNKE_PASSTHRU_ERROR;
              प्रत्यागम परिणाम;
            }
            परिणाम = Curl_bump_headersize(दत्त, trlen, FALSE);
            यदि(परिणाम) {
              ch->state = CHUNK_FAILED;
              ch->चरमः_संहिता = CHUNKE_TRAILER_सीमा;
              प्रत्यागम परिणाम;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          यदि(*buf == 0x0a)
            /* already on the LF */
            भङ्ग;
        }
        अन्यथा {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          भङ्ग; /* do not advance the pointer */
        }
      }
      अन्यथा {
        परिणाम = curlx_dyn_addn(&ch->trailer, buf, 1);
        यदि(परिणाम) {
          ch->state = CHUNK_FAILED;
          ch->चरमः_संहिता = CHUNKE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          प्रत्यागम परिणाम;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      भङ्ग;

    पक्ष CHUNK_TRAILER_CR:
      यदि(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->चरमः_संहिता = CHUNKE_BAD_CHUNK;
        प्रत्यागम CURLE_RECV_ERROR;
      }
      भङ्ग;

    पक्ष CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      यदि((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        भङ्ग;
      }
      यदि(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      भङ्ग;

    पक्ष CHUNK_STOP:
      यदि(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_समाप्तम्;
        CURL_TRC_WRITE(दत्त, "http_chunk, response complete");
        प्रत्यागम CURLE_OK;
      }
      अन्यथा {
        ch->state = CHUNK_FAILED;
        ch->चरमः_संहिता = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(दत्त, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (अचिह्नित पूर्णाङ्क)*buf);
        प्रत्यागम CURLE_RECV_ERROR;
      }
    पक्ष CHUNK_समाप्तम्:
      प्रत्यागम CURLE_OK;

    पक्ष CHUNK_FAILED:
      प्रत्यागम CURLE_RECV_ERROR;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव वर्ण *Curl_chunked_strerror(CHUNKcode संहिता)
{
  विकल्प(संहिता) {
  स्वभाव:
    प्रत्यागम "OK";
  पक्ष CHUNKE_TOO_LONG_HEX:
    प्रत्यागम "Too long hexadecimal number";
  पक्ष CHUNKE_ILLEGAL_HEX:
    प्रत्यागम "Illegal or missing hexadecimal sequence";
  पक्ष CHUNKE_BAD_CHUNK:
    प्रत्यागम "Malformed encoding found";
  पक्ष CHUNKE_PASSTHRU_ERROR:
    प्रत्यागम "Error writing data to client";
  पक्ष CHUNKE_BAD_ENCODING:
    प्रत्यागम "Bad content-encoding found";
  पक्ष CHUNKE_बहिर्गमनम्_सम्बन्धि_स्मृतिः:
    प्रत्यागम "Out of memory";
  पक्ष CHUNKE_TRAILER_सीमा:
    प्रत्यागम "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(संरचना Curl_easy *दत्त,
                             संरचना Curl_chunker *ch,
                             वर्ण *buf, size_t blen,
                             size_t *pconsumed)
{
  प्रत्यागम httpchunk_readwrite(दत्त, ch, रिक्त, buf, blen, pconsumed);
}

संरचना chunked_writer {
  संरचना Curl_cwriter super;
  संरचना Curl_chunker ch;
};

स्थिर CURLcode cw_chunked_init(संरचना Curl_easy *दत्त,
                                संरचना Curl_cwriter *writer)
{
  संरचना chunked_writer *ctx = writer->ctx;

  दत्त->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(दत्त, &ctx->ch, FALSE, FALSE);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cw_chunked_close(संरचना Curl_easy *दत्त,
                             संरचना Curl_cwriter *writer)
{
  संरचना chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(दत्त, &ctx->ch);
}

स्थिर CURLcode cw_chunked_write(संरचना Curl_easy *दत्त,
                                 संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                 ध्रुव वर्ण *buf, size_t blen)
{
  संरचना chunked_writer *ctx = writer->ctx;
  CURLcode परिणाम;
  size_t consumed;

  यदि(!(type & CLIENTWRITE_BODY))
    प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, blen);

  consumed = 0;
  परिणाम = httpchunk_readwrite(दत्त, &ctx->ch, writer->अग्र, buf, blen,
                               &consumed);

  यदि(परिणाम) {
    यदि(CHUNKE_PASSTHRU_ERROR == ctx->ch.चरमः_संहिता) {
      failf(दत्त, "Failed reading the chunked-encoded stream");
    }
    अन्यथा {
      failf(दत्त, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.चरमः_संहिता));
    }
    प्रत्यागम परिणाम;
  }

  blen -= consumed;
  यदि(CHUNK_समाप्तम् == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    दत्त->req.download_समाप्तम् = TRUE;
    यदि(blen) {
      infof(दत्त, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  अन्यथा यदि((type & CLIENTWRITE_EOS) && !दत्त->req.no_body) {
    failf(दत्त, "transfer closed with outstanding read data remaining");
    प्रत्यागम CURLE_PARTIAL_FILE;
  }

  प्रत्यागम CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ध्रुव संरचना Curl_cwtype Curl_httpchunk_unencoder = {
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
#परिभाषय CURL_CHUNKED_MINLEN   1024
#परिभाषय CURL_CHUNKED_MAXLEN   (64 * 1024)

संरचना chunked_reader {
  संरचना Curl_creader super;
  संरचना bufq chunkbuf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_chunked_init(संरचना Curl_easy *दत्त,
                                संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_सीमा);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_chunked_close(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_free(&ctx->chunkbuf);
}

स्थिर CURLcode सङ्कलय_चरमः_chunk(संरचना Curl_easy *दत्त,
                               संरचना Curl_creader *reader)
{
  संरचना chunked_reader *ctx = reader->ctx;
  संरचना curl_slist *trailers = रिक्त, *tr;
  CURLcode परिणाम;
  size_t n;
  पूर्णाङ्क rc;

  यदि(!दत्त->समुच्चयः.trailer_callback) {
    CURL_TRC_READ(दत्त, "http_chunk, added last, empty chunk");
    प्रत्यागम Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_trailer_callback);
    rc = दत्त->समुच्चयः.trailer_callback(&trailers, दत्त->समुच्चयः.trailer_दत्त);
    CURL_CBAPI_अन्तः(&guard);
  }

  यदि(rc != CURL_TRAILERFUNC_OK) {
    failf(दत्त, "operation aborted by trailing headers callback");
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    गच्छ बहिर्गमनम्;
  }

  प्रति(tr = trailers; tr; tr = tr->अग्र) {
    /* only add correctly formatted trailers */
    ध्रुव वर्ण *ptr = strchr(tr->दत्त, ':');
    यदि(!ptr || *(ptr + 1) != ' ') {
      infof(दत्त, "Malformatted trailing header, skipping trailer");
      अनुवर्त;
    }

    परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, tr->दत्त, वाक्यदैर्घ्यम्(tr->दत्त), &n);
    यदि(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

बहिर्गमनम्:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(दत्त, "http_chunk, added last chunk with trailers "
                "from client -> %d", (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode सङ्कलय_chunk(संरचना Curl_easy *दत्त,
                          संरचना Curl_creader *reader,
                          वर्ण *buf, size_t blen)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode परिणाम;
  वर्ण tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
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

  परिणाम = Curl_creader_read(दत्त, reader->अग्र, buf, blen, &nread, &eos);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(eos)
    ctx->read_eos = TRUE;

  यदि(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    वर्ण hd[11] = "";
    पूर्णाङ्क hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, परिमाण(hd), "%zx\r\n", nread);
    यदि(hdlen <= 0)
      प्रत्यागम CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    यदि(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    यदि(!परिणाम)
      परिणाम = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(दत्त, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(ctx->read_eos)
    प्रत्यागम सङ्कलय_चरमः_chunk(दत्त, reader);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cr_chunked_read(संरचना Curl_easy *दत्त,
                                संरचना Curl_creader *reader,
                                वर्ण *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  संरचना chunked_reader *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  यदि(!ctx->eos) {
    यदि(!ctx->read_eos && Curl_bufq_is_रिक्तानि(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      परिणाम = सङ्कलय_chunk(दत्त, reader, buf, blen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    यदि(!Curl_bufq_is_रिक्तानि(&ctx->chunkbuf)) {
      परिणाम = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      यदि(!परिणाम && ctx->read_eos && Curl_bufq_is_रिक्तानि(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      प्रत्यागम परिणाम;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  प्रत्यागम CURLE_OK;
}

स्थिर curl_off_t cr_chunked_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                          संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ध्रुव संरचना Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना chunked_reader)
};

CURLcode Curl_httpchunk_सङ्कलय_reader(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, दत्त, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  यदि(!परिणाम)
    परिणाम = Curl_creader_सङ्कलय(दत्त, reader);

  यदि(परिणाम && reader)
    Curl_creader_free(दत्त, reader);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* CURL_DISABLE_HTTP */
