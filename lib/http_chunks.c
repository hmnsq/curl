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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

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

خالی Curl_httpchunk_init(ساخت Curl_easy *ڈیٹا, ساخت Curl_chunker *ch,
                         bool ignore_body, bool اندر_connect)
{
  (خالی)ڈیٹا;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->آخری_رمز = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->اندر_connect = اندر_connect;
}

خالی Curl_httpchunk_reset(ساخت Curl_easy *ڈیٹا, ساخت Curl_chunker *ch,
                          bool ignore_body)
{
  (خالی)ڈیٹا;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->آخری_رمز = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

خالی Curl_httpchunk_free(ساخت Curl_easy *ڈیٹا, ساخت Curl_chunker *ch)
{
  (خالی)ڈیٹا;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_مکمل(ساخت Curl_easy *ڈیٹا, ساخت Curl_chunker *ch)
{
  (خالی)ڈیٹا;
  واپس ch->state == CHUNK_مکمل;
}

ساکن CURLcode httpchunk_readwrite(ساخت Curl_easy *ڈیٹا,
                                    ساخت Curl_chunker *ch,
                                    ساخت Curl_cwriter *cw_اگلا,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  اگر(ch->state == CHUNK_مکمل)
    واپس CURLE_OK;
  اگر(ch->state == CHUNK_FAILED)
    واپس CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  اگر(ڈیٹا->سیٹ.http_te_skip && !ch->ignore_body) {
    اگر(cw_اگلا)
      نتیجہ = Curl_cwriter_write(ڈیٹا, cw_اگلا, CLIENTWRITE_BODY, buf, blen);
    ورنہ
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, buf, blen);
    اگر(نتیجہ) {
      ch->state = CHUNK_FAILED;
      ch->آخری_رمز = CHUNKE_PASSTHRU_ERROR;
      واپس نتیجہ;
    }
  }

  جب(blen) {
    انتخاب(ch->state) {
    حالت CHUNK_HEX:
      اگر(ISXDIGIT(*buf)) {
        اگر(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(ڈیٹا, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->آخری_رمز = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          واپس CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ورنہ {
        ثابت حرف *p;
        اگر(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(ڈیٹا, "chunk hex-length char not a hex digit: 0x%x",
                (غیرعلامتی صحیح)*buf);
          ch->state = CHUNK_FAILED;
          ch->آخری_رمز = CHUNKE_ILLEGAL_HEX;
          واپس CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        اگر(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(ڈیٹا, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->آخری_رمز = CHUNKE_ILLEGAL_HEX;
          واپس CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      توڑو;

    حالت CHUNK_LF:
      /* waiting for the LF after a chunk size */
      اگر(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        اگر(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        ورنہ {
          ch->state = CHUNK_ڈیٹا;
          CURL_TRC_WRITE(ڈیٹا, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      توڑو;

    حالت CHUNK_ڈیٹا:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      اگر(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      اگر(!ڈیٹا->سیٹ.http_te_skip && !ch->ignore_body) {
        اگر(cw_اگلا)
          نتیجہ = Curl_cwriter_write(ڈیٹا, cw_اگلا, CLIENTWRITE_BODY,
                                      buf, piece);
        ورنہ
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, buf, piece);
        اگر(نتیجہ) {
          ch->state = CHUNK_FAILED;
          ch->آخری_رمز = CHUNKE_PASSTHRU_ERROR;
          واپس نتیجہ;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(ڈیٹا, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      اگر(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      توڑو;

    حالت CHUNK_POSTLF:
      اگر(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(ڈیٹا, ch, (bool)ch->ignore_body);
      }
      ورنہ اگر(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->آخری_رمز = CHUNKE_BAD_CHUNK;
        واپس CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      توڑو;

    حالت CHUNK_TRAILER:
      اگر((*buf == 0x0d) || (*buf == 0x0a)) {
        ثابت حرف *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        اگر(tr) {
          size_t trlen;
          نتیجہ = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          اگر(نتیجہ) {
            ch->state = CHUNK_FAILED;
            ch->آخری_رمز = CHUNKE_برآمد_کا_حافظہ;
            واپس نتیجہ;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          نتیجہ = Curl_verify_سرخی(ڈیٹا, tr, trlen);
          اگر(نتیجہ) {
            ch->state = CHUNK_FAILED;
            ch->آخری_رمز = CHUNKE_BAD_CHUNK;
            واپس نتیجہ;
          }

          اگر(!ڈیٹا->سیٹ.http_te_skip) {
            صحیح hd_type = CLIENTWRITE_سرخی | CLIENTWRITE_TRAILER;
            اگر(ch->اندر_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            اگر(cw_اگلا)
              نتیجہ = Curl_cwriter_write(ڈیٹا, cw_اگلا, hd_type, tr, trlen);
            ورنہ
              نتیجہ = Curl_client_write(ڈیٹا, hd_type, tr, trlen);
            CURL_TRC_WRITE(ڈیٹا, "wrote trailer '%s'", tr);
            اگر(نتیجہ) {
              ch->state = CHUNK_FAILED;
              ch->آخری_رمز = CHUNKE_PASSTHRU_ERROR;
              واپس نتیجہ;
            }
            نتیجہ = Curl_bump_headersize(ڈیٹا, trlen, FALSE);
            اگر(نتیجہ) {
              ch->state = CHUNK_FAILED;
              ch->آخری_رمز = CHUNKE_TRAILER_حد_بندی;
              واپس نتیجہ;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          اگر(*buf == 0x0a)
            /* already on the LF */
            توڑو;
        }
        ورنہ {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          توڑو; /* do not advance the pointer */
        }
      }
      ورنہ {
        نتیجہ = curlx_dyn_addn(&ch->trailer, buf, 1);
        اگر(نتیجہ) {
          ch->state = CHUNK_FAILED;
          ch->آخری_رمز = CHUNKE_برآمد_کا_حافظہ;
          واپس نتیجہ;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      توڑو;

    حالت CHUNK_TRAILER_CR:
      اگر(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      ورنہ {
        ch->state = CHUNK_FAILED;
        ch->آخری_رمز = CHUNKE_BAD_CHUNK;
        واپس CURLE_RECV_ERROR;
      }
      توڑو;

    حالت CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      اگر((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        توڑو;
      }
      اگر(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      توڑو;

    حالت CHUNK_STOP:
      اگر(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_مکمل;
        CURL_TRC_WRITE(ڈیٹا, "http_chunk, response complete");
        واپس CURLE_OK;
      }
      ورنہ {
        ch->state = CHUNK_FAILED;
        ch->آخری_رمز = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(ڈیٹا, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (غیرعلامتی صحیح)*buf);
        واپس CURLE_RECV_ERROR;
      }
    حالت CHUNK_مکمل:
      واپس CURLE_OK;

    حالت CHUNK_FAILED:
      واپس CURLE_RECV_ERROR;
    }
  }
  واپس CURLE_OK;
}

ساکن ثابت حرف *Curl_chunked_strerror(CHUNKcode رمز)
{
  انتخاب(رمز) {
  طےشدہ:
    واپس "OK";
  حالت CHUNKE_TOO_LONG_HEX:
    واپس "Too long hexadecimal number";
  حالت CHUNKE_ILLEGAL_HEX:
    واپس "Illegal or missing hexadecimal sequence";
  حالت CHUNKE_BAD_CHUNK:
    واپس "Malformed encoding found";
  حالت CHUNKE_PASSTHRU_ERROR:
    واپس "Error writing data to client";
  حالت CHUNKE_BAD_ENCODING:
    واپس "Bad content-encoding found";
  حالت CHUNKE_برآمد_کا_حافظہ:
    واپس "Out of memory";
  حالت CHUNKE_TRAILER_حد_بندی:
    واپس "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_chunker *ch,
                             حرف *buf, size_t blen,
                             size_t *pconsumed)
{
  واپس httpchunk_readwrite(ڈیٹا, ch, تہی, buf, blen, pconsumed);
}

ساخت chunked_writer {
  ساخت Curl_cwriter super;
  ساخت Curl_chunker ch;
};

ساکن CURLcode cw_chunked_init(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer)
{
  ساخت chunked_writer *ctx = writer->ctx;

  ڈیٹا->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(ڈیٹا, &ctx->ch, FALSE, FALSE);
  واپس CURLE_OK;
}

ساکن خالی cw_chunked_close(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer)
{
  ساخت chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(ڈیٹا, &ctx->ch);
}

ساکن CURLcode cw_chunked_write(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_cwriter *writer, صحیح type,
                                 ثابت حرف *buf, size_t blen)
{
  ساخت chunked_writer *ctx = writer->ctx;
  CURLcode نتیجہ;
  size_t consumed;

  اگر(!(type & CLIENTWRITE_BODY))
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, blen);

  consumed = 0;
  نتیجہ = httpchunk_readwrite(ڈیٹا, &ctx->ch, writer->اگلا, buf, blen,
                               &consumed);

  اگر(نتیجہ) {
    اگر(CHUNKE_PASSTHRU_ERROR == ctx->ch.آخری_رمز) {
      failf(ڈیٹا, "Failed reading the chunked-encoded stream");
    }
    ورنہ {
      failf(ڈیٹا, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.آخری_رمز));
    }
    واپس نتیجہ;
  }

  blen -= consumed;
  اگر(CHUNK_مکمل == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    ڈیٹا->req.download_مکمل = TRUE;
    اگر(blen) {
      infof(ڈیٹا, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  ورنہ اگر((type & CLIENTWRITE_EOS) && !ڈیٹا->req.no_body) {
    failf(ڈیٹا, "transfer closed with outstanding read data remaining");
    واپس CURLE_PARTIAL_FILE;
  }

  واپس CURLE_OK;
}

/* HTTP chunked Transfer-Encoding decoder */
ثابت ساخت Curl_cwtype Curl_httpchunk_unencoder = {
  "chunked",
  تہی,
  0,
  cw_chunked_init,
  cw_chunked_write,
  Curl_cwriter_def_flush,
  cw_chunked_close,
  حجم(ساخت chunked_writer)
};

/* max length of an HTTP chunk that we want to generate */
#تعریف_کرو CURL_CHUNKED_MINLEN   1024
#تعریف_کرو CURL_CHUNKED_MAXLEN   (64 * 1024)

ساخت chunked_reader {
  ساخت Curl_creader super;
  ساخت bufq chunkbuf;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساکن CURLcode cr_chunked_init(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_creader *reader)
{
  ساخت chunked_reader *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_حد_بندی);
  واپس CURLE_OK;
}

ساکن خالی cr_chunked_close(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader)
{
  ساخت chunked_reader *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_free(&ctx->chunkbuf);
}

ساکن CURLcode جوڑو_آخری_chunk(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creader *reader)
{
  ساخت chunked_reader *ctx = reader->ctx;
  ساخت curl_slist *trailers = تہی, *tr;
  CURLcode نتیجہ;
  size_t n;
  صحیح rc;

  اگر(!ڈیٹا->سیٹ.trailer_callback) {
    CURL_TRC_READ(ڈیٹا, "http_chunk, added last, empty chunk");
    واپس Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  اگر(نتیجہ)
    جاؤ برآمد;

  {
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_trailer_callback);
    rc = ڈیٹا->سیٹ.trailer_callback(&trailers, ڈیٹا->سیٹ.trailer_ڈیٹا);
    CURL_CBAPI_انتہا(&guard);
  }

  اگر(rc != CURL_TRAILERFUNC_OK) {
    failf(ڈیٹا, "operation aborted by trailing headers callback");
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    جاؤ برآمد;
  }

  برائے(tr = trailers; tr; tr = tr->اگلا) {
    /* only add correctly formatted trailers */
    ثابت حرف *ptr = strchr(tr->ڈیٹا, ':');
    اگر(!ptr || *(ptr + 1) != ' ') {
      infof(ڈیٹا, "Malformatted trailing header, skipping trailer");
      جاری;
    }

    نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, tr->ڈیٹا, لڑی_لمبائی(tr->ڈیٹا), &n);
    اگر(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

برآمد:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(ڈیٹا, "http_chunk, added last chunk with trailers "
                "from client -> %d", (صحیح)نتیجہ);
  واپس نتیجہ;
}

ساکن CURLcode جوڑو_chunk(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_creader *reader,
                          حرف *buf, size_t blen)
{
  ساخت chunked_reader *ctx = reader->ctx;
  CURLcode نتیجہ;
  حرف tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  اگر(blen < حجم(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = حجم(tmp);
  }
  ورنہ {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  نتیجہ = Curl_creader_read(ڈیٹا, reader->اگلا, buf, blen, &nread, &eos);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(eos)
    ctx->read_eos = TRUE;

  اگر(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    حرف hd[11] = "";
    صحیح hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, حجم(hd), "%zx\r\n", nread);
    اگر(hdlen <= 0)
      واپس CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    اگر(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    اگر(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(ڈیٹا, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(ctx->read_eos)
    واپس جوڑو_آخری_chunk(ڈیٹا, reader);
  واپس CURLE_OK;
}

ساکن CURLcode cr_chunked_read(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_creader *reader,
                                حرف *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  ساخت chunked_reader *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  اگر(!ctx->eos) {
    اگر(!ctx->read_eos && Curl_bufq_is_خالی_خانے(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      نتیجہ = جوڑو_chunk(ڈیٹا, reader, buf, blen);
      اگر(نتیجہ)
        واپس نتیجہ;
    }

    اگر(!Curl_bufq_is_خالی_خانے(&ctx->chunkbuf)) {
      نتیجہ = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      اگر(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_خانے(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      واپس نتیجہ;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  واپس CURLE_OK;
}

ساکن curl_off_t cr_chunked_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ثابت ساخت Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت chunked_reader)
};

CURLcode Curl_httpchunk_جوڑو_reader(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = تہی;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, ڈیٹا, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(ڈیٹا, reader);

  اگر(نتیجہ && reader)
    Curl_creader_free(ڈیٹا, reader);
  واپس نتیجہ;
}

#اگر_ختم /* CURL_DISABLE_HTTP */
