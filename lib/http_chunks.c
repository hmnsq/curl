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

#جے_بنا_تعریف CURL_DISABLE_HTTP

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

فراغ Curl_httpchunk_init(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch,
                         bool ignore_body, bool وچ_connect)
{
  (فراغ)انکڑے;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->آخری_کوڈ = CHUNKE_OK;
  curlx_dyn_init(&ch->trailer, DYN_H1_TRAILER);
  ch->ignore_body = ignore_body;
  ch->وچ_connect = وچ_connect;
}

فراغ Curl_httpchunk_reset(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch,
                          bool ignore_body)
{
  (فراغ)انکڑے;
  ch->hexindex = 0;      /* start at 0 */
  ch->state = CHUNK_HEX; /* we get hex first! */
  ch->آخری_کوڈ = CHUNKE_OK;
  curlx_dyn_reset(&ch->trailer);
  ch->ignore_body = ignore_body;
}

فراغ Curl_httpchunk_free(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch)
{
  (فراغ)انکڑے;
  curlx_dyn_free(&ch->trailer);
}

bool Curl_httpchunk_is_پورا(بنية Curl_easy *انکڑے, بنية Curl_chunker *ch)
{
  (فراغ)انکڑے;
  أرجع ch->state == CHUNK_پورا;
}

ساكن CURLcode httpchunk_readwrite(بنية Curl_easy *انکڑے,
                                    بنية Curl_chunker *ch,
                                    بنية Curl_cwriter *cw_اگلا,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t piece;

  *pconsumed = 0; /* nothing's written yet */
  /* first check terminal states that will not progress anywhere */
  إذا(ch->state == CHUNK_پورا)
    أرجع CURLE_OK;
  إذا(ch->state == CHUNK_FAILED)
    أرجع CURLE_RECV_ERROR;

  /* the original data is written to the client, but we go on with the
     chunk read process, to properly calculate the content length */
  إذا(انکڑے->سیٹ.http_te_skip && !ch->ignore_body) {
    إذا(cw_اگلا)
      نتیجہ = Curl_cwriter_write(انکڑے, cw_اگلا, CLIENTWRITE_BODY, buf, blen);
    وإلا
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, buf, blen);
    إذا(نتیجہ) {
      ch->state = CHUNK_FAILED;
      ch->آخری_کوڈ = CHUNKE_PASSTHRU_ERROR;
      أرجع نتیجہ;
    }
  }

  طالما(blen) {
    بدل(ch->state) {
    حالة CHUNK_HEX:
      إذا(ISXDIGIT(*buf)) {
        إذا(ch->hexindex >= CHUNK_MAXNUM_LEN) {
          failf(انکڑے, "chunk hex-length longer than %d", CHUNK_MAXNUM_LEN);
          ch->state = CHUNK_FAILED;
          ch->آخری_کوڈ = CHUNKE_TOO_LONG_HEX; /* longer than we support */
          أرجع CURLE_RECV_ERROR;
        }
        ch->hexbuffer[ch->hexindex++] = *buf;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      وإلا {
        ثابت حرف *p;
        إذا(ch->hexindex == 0) {
          /* This is illegal data, we received junk where we expected
             a hexadecimal digit. */
          failf(انکڑے, "chunk hex-length char not a hex digit: 0x%x",
                (بنانشانی صحيح)*buf);
          ch->state = CHUNK_FAILED;
          ch->آخری_کوڈ = CHUNKE_ILLEGAL_HEX;
          أرجع CURLE_RECV_ERROR;
        }
        /* blen and buf are unmodified */
        ch->hexbuffer[ch->hexindex] = 0;
        p = &ch->hexbuffer[0];
        إذا(curlx_str_hex(&p, &ch->datasize, CURL_OFF_T_MAX)) {
          failf(انکڑے, "invalid chunk size: '%s'", ch->hexbuffer);
          ch->state = CHUNK_FAILED;
          ch->آخری_کوڈ = CHUNKE_ILLEGAL_HEX;
          أرجع CURLE_RECV_ERROR;
        }
        ch->state = CHUNK_LF; /* now wait for the CRLF */
      }
      اكسر;

    حالة CHUNK_LF:
      /* waiting for the LF after a chunk size */
      إذا(*buf == 0x0a) {
        /* we are now expecting data to come, unless size was zero! */
        إذا(ch->datasize == 0) {
          ch->state = CHUNK_TRAILER; /* now check for trailers */
        }
        وإلا {
          ch->state = CHUNK_انکڑے;
          CURL_TRC_WRITE(انکڑے, "http_chunked, chunk start of %"
                         FMT_OFF_T " bytes", ch->datasize);
        }
      }

      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_انکڑے:
      /* We expect 'datasize' of data. We have 'blen' right now, it can be
         more or less than 'datasize'. Get the smallest piece. */
      piece = blen;
      إذا(ch->datasize < (curl_off_t)blen)
        piece = curlx_sotouz(ch->datasize);

      /* Write the data portion available */
      إذا(!انکڑے->سیٹ.http_te_skip && !ch->ignore_body) {
        إذا(cw_اگلا)
          نتیجہ = Curl_cwriter_write(انکڑے, cw_اگلا, CLIENTWRITE_BODY,
                                      buf, piece);
        وإلا
          نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, buf, piece);
        إذا(نتیجہ) {
          ch->state = CHUNK_FAILED;
          ch->آخری_کوڈ = CHUNKE_PASSTHRU_ERROR;
          أرجع نتیجہ;
        }
      }

      *pconsumed += piece;
      ch->datasize -= piece; /* decrease amount left to expect */
      buf += piece;    /* move read pointer forward */
      blen -= piece;   /* decrease space left in this round */
      CURL_TRC_WRITE(انکڑے, "http_chunked, write %zu body bytes, %"
                     FMT_OFF_T " bytes in chunk remain",
                     piece, ch->datasize);

      إذا(ch->datasize == 0)
        /* end of data this round, we now expect a trailing CRLF */
        ch->state = CHUNK_POSTLF;
      اكسر;

    حالة CHUNK_POSTLF:
      إذا(*buf == 0x0a) {
        /* The last one before we go back to hex state and start all over. */
        Curl_httpchunk_reset(انکڑے, ch, (bool)ch->ignore_body);
      }
      وإلا إذا(*buf != 0x0d) {
        ch->state = CHUNK_FAILED;
        ch->آخری_کوڈ = CHUNKE_BAD_CHUNK;
        أرجع CURLE_RECV_ERROR;
      }
      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_TRAILER:
      إذا((*buf == 0x0d) || (*buf == 0x0a)) {
        ثابت حرف *tr = curlx_dyn_ptr(&ch->trailer);
        /* this is the end of a trailer, but if the trailer was zero bytes
           there was no trailer and we move on */

        إذا(tr) {
          size_t trlen;
          نتیجہ = curlx_dyn_addn(&ch->trailer, STRCONST("\x0d\x0a"));
          إذا(نتیجہ) {
            ch->state = CHUNK_FAILED;
            ch->آخری_کوڈ = CHUNKE_باہرلا_دا_حافظہ;
            أرجع نتیجہ;
          }
          tr = curlx_dyn_ptr(&ch->trailer);
          trlen = curlx_dyn_len(&ch->trailer);

          /* a trailer is delivered to the client as a header, so it must pass
             the same checks as a regular response header */
          نتیجہ = Curl_verify_سرخی(انکڑے, tr, trlen);
          إذا(نتیجہ) {
            ch->state = CHUNK_FAILED;
            ch->آخری_کوڈ = CHUNKE_BAD_CHUNK;
            أرجع نتیجہ;
          }

          إذا(!انکڑے->سیٹ.http_te_skip) {
            صحيح hd_type = CLIENTWRITE_سرخی | CLIENTWRITE_TRAILER;
            إذا(ch->وچ_connect)
              hd_type |= CLIENTWRITE_CONNECT;
            إذا(cw_اگلا)
              نتیجہ = Curl_cwriter_write(انکڑے, cw_اگلا, hd_type, tr, trlen);
            وإلا
              نتیجہ = Curl_client_write(انکڑے, hd_type, tr, trlen);
            CURL_TRC_WRITE(انکڑے, "wrote trailer '%s'", tr);
            إذا(نتیجہ) {
              ch->state = CHUNK_FAILED;
              ch->آخری_کوڈ = CHUNKE_PASSTHRU_ERROR;
              أرجع نتیجہ;
            }
            نتیجہ = Curl_bump_headersize(انکڑے, trlen, FALSE);
            إذا(نتیجہ) {
              ch->state = CHUNK_FAILED;
              ch->آخری_کوڈ = CHUNKE_TRAILER_حد_بندی;
              أرجع نتیجہ;
            }
          }
          curlx_dyn_reset(&ch->trailer);
          ch->state = CHUNK_TRAILER_CR;
          إذا(*buf == 0x0a)
            /* already on the LF */
            اكسر;
        }
        وإلا {
          /* no trailer, we are on the final CRLF pair */
          ch->state = CHUNK_TRAILER_POSTCR;
          اكسر; /* do not advance the pointer */
        }
      }
      وإلا {
        نتیجہ = curlx_dyn_addn(&ch->trailer, buf, 1);
        إذا(نتیجہ) {
          ch->state = CHUNK_FAILED;
          ch->آخری_کوڈ = CHUNKE_باہرلا_دا_حافظہ;
          أرجع نتیجہ;
        }
      }
      buf++;
      blen--;
      (*pconsumed)++;
      اكسر;

    حالة CHUNK_TRAILER_CR:
      إذا(*buf == 0x0a) {
        ch->state = CHUNK_TRAILER_POSTCR;
        buf++;
        blen--;
        (*pconsumed)++;
      }
      وإلا {
        ch->state = CHUNK_FAILED;
        ch->آخری_کوڈ = CHUNKE_BAD_CHUNK;
        أرجع CURLE_RECV_ERROR;
      }
      اكسر;

    حالة CHUNK_TRAILER_POSTCR:
      /* We enter this state when a CR should arrive so we expect to
         have to first pass a CR before we wait for LF */
      إذا((*buf != 0x0d) && (*buf != 0x0a)) {
        /* not a CR then it must be another header in the trailer */
        ch->state = CHUNK_TRAILER;
        اكسر;
      }
      إذا(*buf == 0x0d) {
        /* skip if CR */
        buf++;
        blen--;
        (*pconsumed)++;
      }
      /* now wait for the final LF */
      ch->state = CHUNK_STOP;
      اكسر;

    حالة CHUNK_STOP:
      إذا(*buf == 0x0a) {
        blen--;
        (*pconsumed)++;
        /* Record the length of any data left in the end of the buffer
           even if there is no more chunks to read */
        ch->datasize = blen;
        ch->state = CHUNK_پورا;
        CURL_TRC_WRITE(انکڑے, "http_chunk, response complete");
        أرجع CURLE_OK;
      }
      وإلا {
        ch->state = CHUNK_FAILED;
        ch->آخری_کوڈ = CHUNKE_BAD_CHUNK;
        CURL_TRC_WRITE(انکڑے, "http_chunk error, expected 0x0a, seeing 0x%ux",
                       (بنانشانی صحيح)*buf);
        أرجع CURLE_RECV_ERROR;
      }
    حالة CHUNK_پورا:
      أرجع CURLE_OK;

    حالة CHUNK_FAILED:
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

ساكن ثابت حرف *Curl_chunked_strerror(CHUNKcode کوڈ)
{
  بدل(کوڈ) {
  افتراضي:
    أرجع "OK";
  حالة CHUNKE_TOO_LONG_HEX:
    أرجع "Too long hexadecimal number";
  حالة CHUNKE_ILLEGAL_HEX:
    أرجع "Illegal or missing hexadecimal sequence";
  حالة CHUNKE_BAD_CHUNK:
    أرجع "Malformed encoding found";
  حالة CHUNKE_PASSTHRU_ERROR:
    أرجع "Error writing data to client";
  حالة CHUNKE_BAD_ENCODING:
    أرجع "Bad content-encoding found";
  حالة CHUNKE_باہرلا_دا_حافظہ:
    أرجع "Out of memory";
  حالة CHUNKE_TRAILER_حد_بندی:
    أرجع "trailers exceeded header limit";
  }
}

CURLcode Curl_httpchunk_read(بنية Curl_easy *انکڑے,
                             بنية Curl_chunker *ch,
                             حرف *buf, size_t blen,
                             size_t *pconsumed)
{
  أرجع httpchunk_readwrite(انکڑے, ch, عدم, buf, blen, pconsumed);
}

بنية chunked_writer {
  بنية Curl_cwriter super;
  بنية Curl_chunker ch;
};

ساكن CURLcode cw_chunked_init(بنية Curl_easy *انکڑے,
                                بنية Curl_cwriter *writer)
{
  بنية chunked_writer *ctx = writer->ctx;

  انکڑے->req.chunk = TRUE;      /* chunks coming our way. */
  Curl_httpchunk_init(انکڑے, &ctx->ch, FALSE, FALSE);
  أرجع CURLE_OK;
}

ساكن فراغ cw_chunked_close(بنية Curl_easy *انکڑے,
                             بنية Curl_cwriter *writer)
{
  بنية chunked_writer *ctx = writer->ctx;
  Curl_httpchunk_free(انکڑے, &ctx->ch);
}

ساكن CURLcode cw_chunked_write(بنية Curl_easy *انکڑے,
                                 بنية Curl_cwriter *writer, صحيح type,
                                 ثابت حرف *buf, size_t blen)
{
  بنية chunked_writer *ctx = writer->ctx;
  CURLcode نتیجہ;
  size_t consumed;

  إذا(!(type & CLIENTWRITE_BODY))
    أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, blen);

  consumed = 0;
  نتیجہ = httpchunk_readwrite(انکڑے, &ctx->ch, writer->اگلا, buf, blen,
                               &consumed);

  إذا(نتیجہ) {
    إذا(CHUNKE_PASSTHRU_ERROR == ctx->ch.آخری_کوڈ) {
      failf(انکڑے, "Failed reading the chunked-encoded stream");
    }
    وإلا {
      failf(انکڑے, "%s in chunked-encoding",
            Curl_chunked_strerror(ctx->ch.آخری_کوڈ));
    }
    أرجع نتیجہ;
  }

  blen -= consumed;
  إذا(CHUNK_پورا == ctx->ch.state) {
    /* chunks read successfully, download is complete */
    انکڑے->req.download_پورا = TRUE;
    إذا(blen) {
      infof(انکڑے, "Leftovers after chunking: %zu bytes", blen);
    }
  }
  وإلا إذا((type & CLIENTWRITE_EOS) && !انکڑے->req.no_body) {
    failf(انکڑے, "transfer closed with outstanding read data remaining");
    أرجع CURLE_PARTIAL_FILE;
  }

  أرجع CURLE_OK;
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
#تعریف_کرو CURL_CHUNKED_MINLEN   1024
#تعریف_کرو CURL_CHUNKED_MAXLEN   (64 * 1024)

بنية chunked_reader {
  بنية Curl_creader super;
  بنية bufq chunkbuf;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساكن CURLcode cr_chunked_init(بنية Curl_easy *انکڑے,
                                بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  (فراغ)انکڑے;
  Curl_bufq_init2(&ctx->chunkbuf, CURL_CHUNKED_MAXLEN, 2, BUFQ_OPT_SOFT_حد_بندی);
  أرجع CURLE_OK;
}

ساكن فراغ cr_chunked_close(بنية Curl_easy *انکڑے,
                             بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  (فراغ)انکڑے;
  Curl_bufq_free(&ctx->chunkbuf);
}

ساكن CURLcode جوڑو_آخری_chunk(بنية Curl_easy *انکڑے,
                               بنية Curl_creader *reader)
{
  بنية chunked_reader *ctx = reader->ctx;
  بنية curl_slist *trailers = عدم, *tr;
  CURLcode نتیجہ;
  size_t n;
  صحيح rc;

  إذا(!انکڑے->سیٹ.trailer_callback) {
    CURL_TRC_READ(انکڑے, "http_chunk, added last, empty chunk");
    أرجع Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n\r\n"), &n);
  }

  نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("0\r\n"), &n);
  إذا(نتیجہ)
    اذهب باہرلا;

  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, انکڑے, easy_trailer_callback);
    rc = انکڑے->سیٹ.trailer_callback(&trailers, انکڑے->سیٹ.trailer_انکڑے);
    CURL_CBAPI_سرا(&guard);
  }

  إذا(rc != CURL_TRAILERFUNC_OK) {
    failf(انکڑے, "operation aborted by trailing headers callback");
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    اذهب باہرلا;
  }

  لكل(tr = trailers; tr; tr = tr->اگلا) {
    /* only add correctly formatted trailers */
    ثابت حرف *ptr = strchr(tr->انکڑے, ':');
    إذا(!ptr || *(ptr + 1) != ' ') {
      infof(انکڑے, "Malformatted trailing header, skipping trailer");
      استمر;
    }

    نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, tr->انکڑے, لڑی_لمبائی(tr->انکڑے), &n);
    إذا(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, STRCONST("\r\n"), &n);

باہرلا:
  curl_slist_free_all(trailers);
  CURL_TRC_READ(انکڑے, "http_chunk, added last chunk with trailers "
                "from client -> %d", (صحيح)نتیجہ);
  أرجع نتیجہ;
}

ساكن CURLcode جوڑو_chunk(بنية Curl_easy *انکڑے,
                          بنية Curl_creader *reader,
                          حرف *buf, size_t blen)
{
  بنية chunked_reader *ctx = reader->ctx;
  CURLcode نتیجہ;
  حرف tmp[CURL_CHUNKED_MINLEN];
  size_t nread;
  bool eos;

  DEBUGASSERT(!ctx->read_eos);
  blen = CURLMIN(blen, CURL_CHUNKED_MAXLEN); /* respect our buffer pref */
  إذا(blen < حجم(tmp)) {
    /* small read, make a chunk of decent size */
    buf = tmp;
    blen = حجم(tmp);
  }
  وإلا {
    /* larger read, make a chunk that will fit when read back */
    blen -= (8 + 2 + 2); /* deduct max overhead, 8 hex + 2*crlf */
  }

  نتیجہ = Curl_creader_read(انکڑے, reader->اگلا, buf, blen, &nread, &eos);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(eos)
    ctx->read_eos = TRUE;

  إذا(nread) {
    /* actually got bytes, wrap them into the chunkbuf */
    حرف hd[11] = "";
    صحيح hdlen;
    size_t n;

    hdlen = curl_msnprintf(hd, حجم(hd), "%zx\r\n", nread);
    إذا(hdlen <= 0)
      أرجع CURLE_READ_ERROR;
    /* On a soft-limited bufq, we do not need to check that all was written */
    نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, hd, hdlen, &n);
    إذا(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, buf, nread, &n);
    إذا(!نتیجہ)
      نتیجہ = Curl_bufq_cwrite(&ctx->chunkbuf, "\r\n", 2, &n);
    CURL_TRC_READ(انکڑے, "http_chunk, made chunk of %zu bytes -> %d",
                  nread, (صحيح)نتیجہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(ctx->read_eos)
    أرجع جوڑو_آخری_chunk(انکڑے, reader);
  أرجع CURLE_OK;
}

ساكن CURLcode cr_chunked_read(بنية Curl_easy *انکڑے,
                                بنية Curl_creader *reader,
                                حرف *buf, size_t blen,
                                size_t *pnread, bool *peos)
{
  بنية chunked_reader *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_READ_ERROR;

  *pnread = 0;
  *peos = (bool)ctx->eos;

  إذا(!ctx->eos) {
    إذا(!ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ctx->chunkbuf)) {
      /* Still getting data form the next reader, buffer is empty */
      نتیجہ = جوڑو_chunk(انکڑے, reader, buf, blen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->chunkbuf)) {
      نتیجہ = Curl_bufq_cread(&ctx->chunkbuf, buf, blen, pnread);
      إذا(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ctx->chunkbuf)) {
        /* no more data, read all, done. */
        ctx->eos = TRUE;
        *peos = TRUE;
      }
      أرجع نتیجہ;
    }
  }
  /* We may get here, because we are done or because callbacks paused */
  DEBUGASSERT(ctx->eos || !ctx->read_eos);
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_chunked_کل_لمائی(بنية Curl_easy *انکڑے,
                                          بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع -1;
}

/* HTTP chunked Transfer-Encoding encoder */
ثابت بنية Curl_crtype Curl_httpchunk_encoder = {
  "chunked",
  cr_chunked_init,
  cr_chunked_read,
  cr_chunked_close,
  Curl_creader_def_needs_rewind,
  cr_chunked_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية chunked_reader)
};

CURLcode Curl_httpchunk_جوڑو_reader(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, انکڑے, &Curl_httpchunk_encoder,
                               CURL_CR_TRANSFER_ENCODE);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(انکڑے, reader);

  إذا(نتیجہ && reader)
    Curl_creader_free(انکڑے, reader);
  أرجع نتیجہ;
}

#جے_انت /* CURL_DISABLE_HTTP */
