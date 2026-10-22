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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنہ_اگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "transfer.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "cw-out.h"
#شامل "cw-pause.h"
#شامل "multiif.h"
#شامل "progress.h"

ساکن خالی cl_reset_writer(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cwriter *writer = ڈیٹا->req.writer.ڈھیر;
  جب(writer) {
    ڈیٹا->req.writer.ڈھیر = writer->اگلا;
    writer->cwt->do_close(ڈیٹا, writer);
    curlx_free(writer);
    writer = ڈیٹا->req.writer.ڈھیر;
  }
  ڈیٹا->req.writer.paused = FALSE;
}

ساکن خالی cl_reset_reader(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = ڈیٹا->req.reader.ڈھیر;
  ڈیٹا->req.reader_started = FALSE;
  جب(reader) {
    ڈیٹا->req.reader.ڈھیر = reader->اگلا;
    reader->crt->do_close(ڈیٹا, reader);
    curlx_free(reader);
    reader = ڈیٹا->req.reader.ڈھیر;
  }
}

خالی Curl_client_cleanup(ساخت Curl_easy *ڈیٹا)
{
  cl_reset_reader(ڈیٹا);
  cl_reset_writer(ڈیٹا);

  ڈیٹا->req.bytecount = 0;
  ڈیٹا->req.headerline = 0;
}

خالی Curl_client_reset(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(ڈیٹا, "client_reset, will rewind reader");
  }
  ورنہ {
    CURL_TRC_READ(ڈیٹا, "client_reset, clear readers");
    cl_reset_reader(ڈیٹا);
  }
  cl_reset_writer(ڈیٹا);

  ڈیٹا->req.bytecount = 0;
  ڈیٹا->req.headerline = 0;
}

CURLcode Curl_client_آغاز(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->req.rewind_read) {
    ساخت Curl_creader *r = ڈیٹا->req.reader.ڈھیر;
    CURLcode نتیجہ = CURLE_OK;

    CURL_TRC_READ(ڈیٹا, "client start, rewind readers");
    جب(r) {
      نتیجہ = r->crt->cntrl(ڈیٹا, r, CURL_CRCNTRL_REWIND);
      اگر(نتیجہ) {
        failf(ڈیٹا, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      r = r->اگلا;
    }
    ڈیٹا->req.rewind_read = FALSE;
    cl_reset_reader(ڈیٹا);
  }
  واپس CURLE_OK;
}

bool Curl_creader_will_rewind(ساخت Curl_easy *ڈیٹا)
{
  واپس (bool)ڈیٹا->req.rewind_read;
}

خالی Curl_creader_سیٹ_rewind(ساخت Curl_easy *ڈیٹا, bool enable)
{
  ڈیٹا->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_cwriter *writer)
{
  (خالی)ڈیٹا;
  (خالی)writer;
  واپس CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer, صحیح type,
                                ثابت حرف *buf, size_t nbytes)
{
  واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer)
{
  واپس Curl_cwriter_flush(ڈیٹا, writer->اگلا);
}

خالی Curl_cwriter_def_close(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_cwriter *writer)
{
  (خالی)ڈیٹا;
  (خالی)writer;
}

ساکن size_t get_max_body_write_len(ساخت Curl_easy *ڈیٹا, curl_off_t حد_بندی)
{
  اگر(حد_بندی != -1) {
    /* How much more are we allowed to write? */
    واپس curlx_sotouz_range(حد_بندی - ڈیٹا->req.bytecount, 0, SIZE_MAX);
  }
  واپس SIZE_MAX;
}

ساخت cw_download_ctx {
  ساخت Curl_cwriter super;
  ثنائی_ہندسہ(started_response);
  ثنائی_ہندسہ(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساکن CURLcode cw_download_write(ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_cwriter *writer, صحیح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  ساخت cw_download_ctx *ctx = writer->ctx;
  CURLcode نتیجہ;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  اگر(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || ڈیٹا->req.upload_مکمل)) {
    Curl_pgrs_وقت(ڈیٹا, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  اگر(!(type & CLIENTWRITE_BODY)) {
    اگر(is_connect && ڈیٹا->سیٹ.suppress_connect_سرخیاں)
      واپس CURLE_OK;
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      واپس CURLE_OK;
#اگر_ختم
    نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);
    CURL_TRC_WRITE(ڈیٹا, "download_write header(type=%x, blen=%zu) -> %d",
                   (غیرعلامتی صحیح)type, nbytes, (صحیح)نتیجہ);
    واپس نتیجہ;
  }

  اگر(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_آغاز(&ڈیٹا->پیش_رفت.dl.rlimit, Curl_pgrs_now(ڈیٹا),
                      ڈیٹا->req.سائز);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  اگر(ڈیٹا->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(ڈیٹا->conn);
    CURL_TRC_WRITE(ڈیٹا, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (غیرعلامتی صحیح)type, nbytes);
    ڈیٹا->req.download_مکمل = TRUE;
    اگر(ڈیٹا->info.سرخی_سائز)
      /* if headers have been received, this is fine */
      واپس CURLE_OK;
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  اگر(ڈیٹا->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(ڈیٹا, ڈیٹا->req.maxdownload);
    اگر(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    اگر(nwrite == wmax) {
      ڈیٹا->req.download_مکمل = TRUE;
    }

    اگر((type & CLIENTWRITE_EOS) && !ڈیٹا->req.no_body &&
       (ڈیٹا->req.سائز > ڈیٹا->req.bytecount)) {
      failf(ڈیٹا, "end of response with %" FMT_OFF_T " bytes missing",
            ڈیٹا->req.سائز - ڈیٹا->req.bytecount);
      واپس CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  اگر(ڈیٹا->سیٹ.max_filesize && !ڈیٹا->req.ignorebody) {
    size_t wmax = get_max_body_write_len(ڈیٹا, ڈیٹا->سیٹ.max_filesize);
    اگر(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  اگر(!ڈیٹا->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nwrite);
    CURL_TRC_WRITE(ڈیٹا, "download_write body(type=%x, blen=%zu) -> %d",
                   (غیرعلامتی صحیح)type, nbytes, (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Update stats, write and report progress */
  اگر(nwrite) {
    ڈیٹا->req.bytecount += nwrite;
    Curl_pgrs_download_inc(ڈیٹا, nwrite);
  }

  اگر(excess_len) {
    اگر(!ڈیٹا->req.ignorebody) {
      infof(ڈیٹا,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, ڈیٹا->req.سائز, ڈیٹا->req.maxdownload,
            ڈیٹا->req.bytecount);
      connclose(ڈیٹا->conn);
    }
  }
  ورنہ اگر((nwrite < nbytes) && !ڈیٹا->req.ignorebody) {
    failf(ڈیٹا, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          ڈیٹا->سیٹ.max_filesize, ڈیٹا->req.bytecount);
    واپس CURLE_FILESIZE_EXCEEDED;
  }

  واپس CURLE_OK;
}

ساکن ثابت ساخت Curl_cwtype cw_download = {
  "protocol",
  تہی,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(ساخت cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ساکن CURLcode cw_raw_write(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_cwriter *writer, صحیح type,
                             ثابت حرف *buf, size_t nbytes)
{
  اگر(type & CLIENTWRITE_BODY && ڈیٹا->سیٹ.verbose && !ڈیٹا->req.ignorebody) {
    Curl_debug(ڈیٹا, CURLINFO_ڈیٹا_اندر, buf, nbytes);
  }
  واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, nbytes);
}

ساکن ثابت ساخت Curl_cwtype cw_raw = {
  "raw",
  تہی,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(ساخت Curl_cwriter)
};

ساکن خالی cwriter_جوڑو(ساخت Curl_easy *ڈیٹا,
                        ساخت Curl_cwriter *writer)
{
  ساخت Curl_cwriter **anchor = &ڈیٹا->req.writer.ڈھیر;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  جب(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->اگلا;
  writer->اگلا = *anchor;
  *anchor = writer;
}

ساکن CURLcode do_init_writer_ڈھیر(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cwriter *writer;
  CURLcode نتیجہ;

  DEBUGASSERT(!ڈیٹا->req.writer.ڈھیر);
  نتیجہ = Curl_cwriter_create(&ڈیٹا->req.writer.ڈھیر,
                               ڈیٹا, &Curl_cwt_برآمد, CURL_CW_CLIENT);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_cwriter_create(&writer, ڈیٹا, &cw_download, CURL_CW_PROTOCOL);
  اگر(نتیجہ)
    واپس نتیجہ;
  cwriter_جوڑو(ڈیٹا, writer);

  نتیجہ = Curl_cwriter_create(&writer, ڈیٹا, &cw_raw, CURL_CW_RAW);
  اگر(نتیجہ)
    واپس نتیجہ;
  cwriter_جوڑو(ڈیٹا, writer);

  واپس نتیجہ;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ساخت Curl_easy *ڈیٹا, صحیح type, ثابت حرف *buf,
                           size_t len)
{
  CURLcode نتیجہ;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_سرخی | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  اگر(!ڈیٹا->req.writer.ڈھیر) {
    نتیجہ = do_init_writer_ڈھیر(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    DEBUGASSERT(ڈیٹا->req.writer.ڈھیر);
  }

  نتیجہ = Curl_cwriter_write(ڈیٹا, ڈیٹا->req.writer.ڈھیر, type, buf, len);
  CURL_TRC_WRITE(ڈیٹا, "client_write(type=%x, len=%zu) -> %d",
                 (غیرعلامتی صحیح)type, len, (صحیح)نتیجہ);
  واپس نتیجہ;
}

CURLcode Curl_client_flush(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;

  اگر(!ڈیٹا->req.writer.ڈھیر) {
    نتیجہ = do_init_writer_ڈھیر(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    DEBUGASSERT(ڈیٹا->req.writer.ڈھیر);
  }

  نتیجہ = Curl_cwriter_flush(ڈیٹا, ڈیٹا->req.writer.ڈھیر);
  CURL_TRC_WRITE(ڈیٹا, "client_flush() -> %d", (صحیح)نتیجہ);
  واپس نتیجہ;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ساخت Curl_cwriter **pwriter,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ساخت Curl_cwriter *writer = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  خالی *p;

  DEBUGASSERT(cwt->cwriter_سائز >= حجم(ساخت Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_سائز);
  اگر(!p)
    جاؤ برآمد;

  writer = (ساخت Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  نتیجہ = cwt->do_init(ڈیٹا, writer);

برآمد:
  *pwriter = نتیجہ ? تہی : writer;
  اگر(نتیجہ)
    curlx_free(writer);
  واپس نتیجہ;
}

خالی Curl_cwriter_free(ساخت Curl_easy *ڈیٹا,
                       ساخت Curl_cwriter *writer)
{
  اگر(writer) {
    writer->cwt->do_close(ڈیٹا, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_گنتی(ساخت Curl_easy *ڈیٹا, Curl_cwriter_phase phase)
{
  ساخت Curl_cwriter *w;
  size_t n = 0;

  برائے(w = ڈیٹا->req.writer.ڈھیر; w; w = w->اگلا) {
    اگر(w->phase == phase)
      ++n;
  }
  واپس n;
}

ساکن CURLcode cwriter_ensure_pause_writer(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cwriter *writer =
    Curl_cwriter_get_by_type(ڈیٹا, &Curl_cwt_pause);
  CURLcode نتیجہ = CURLE_OK;

  اگر(!writer) {
    نتیجہ = Curl_cwriter_create(&writer, ڈیٹا, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    اگر(!نتیجہ)
      cwriter_جوڑو(ڈیٹا, writer);
  }
  واپس نتیجہ;
}

CURLcode Curl_cwriter_جوڑو(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_cwriter *writer)
{
  CURLcode نتیجہ;
  ساخت Curl_cwriter **anchor = &ڈیٹا->req.writer.ڈھیر;

  اگر(!*anchor) {
    نتیجہ = do_init_writer_ڈھیر(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    نتیجہ = cwriter_ensure_pause_writer(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  cwriter_جوڑو(ڈیٹا, writer);
  واپس CURLE_OK;
}

ساخت Curl_cwriter *Curl_cwriter_get_by_name(ساخت Curl_easy *ڈیٹا,
                                              ثابت حرف *name)
{
  ساخت Curl_cwriter *writer;
  برائے(writer = ڈیٹا->req.writer.ڈھیر; writer; writer = writer->اگلا) {
    اگر(!strcmp(name, writer->cwt->name))
      واپس writer;
  }
  واپس تہی;
}

ساخت Curl_cwriter *Curl_cwriter_get_by_type(ساخت Curl_easy *ڈیٹا,
                                              ثابت ساخت Curl_cwtype *cwt)
{
  ساخت Curl_cwriter *writer;
  برائے(writer = ڈیٹا->req.writer.ڈھیر; writer; writer = writer->اگلا) {
    اگر(writer->cwt == cwt)
      واپس writer;
  }
  واپس تہی;
}

CURLcode Curl_cwriter_unpause(ساخت Curl_easy *ڈیٹا)
{
  ڈیٹا->req.writer.paused = FALSE;
  واپس Curl_cwriter_flush(ڈیٹا, ڈیٹا->req.writer.ڈھیر);
}

CURLcode Curl_creader_read(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader,
                           حرف *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  اگر(!reader)
    واپس CURLE_READ_ERROR;
  واپس reader->crt->do_read(ڈیٹا, reader, buf, blen, nread, eos);
}

خالی Curl_creader_clear_eos(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader)
{
  جب(reader) {
    (خالی)reader->crt->cntrl(ڈیٹا, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->اگلا;
  }
}

CURLcode Curl_creader_def_init(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس CURLE_OK;
}

خالی Curl_creader_def_close(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
}

CURLcode Curl_creader_def_read(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  اگر(reader->اگلا)
    واپس reader->اگلا->crt->do_read(ڈیٹا, reader->اگلا, buf, blen,
                                      nread, eos);
  ورنہ {
    *nread = 0;
    *eos = FALSE;
    واپس CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس FALSE;
}

curl_off_t Curl_creader_def_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                         ساخت Curl_creader *reader)
{
  واپس reader->اگلا ?
         reader->اگلا->crt->کل_لمبائی(ڈیٹا, reader->اگلا) : -1;
}

CURLcode Curl_creader_def_resume_from(ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_creader *reader,
                                      curl_off_t انحراف)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  (خالی)انحراف;
  واپس CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  (خالی)opcode;
  واپس CURLE_OK;
}

bool Curl_creader_def_is_paused(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_creader *reader)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس FALSE;
}

خالی Curl_creader_def_مکمل(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader, صحیح premature)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  (خالی)premature;
}

ساخت cr_اندر_ctx {
  ساخت Curl_creader super;
  curl_read_callback read_cb;
  خالی *cb_user_ڈیٹا;
  curl_off_t کل_len;
  curl_off_t read_len;
  CURLcode error_نتیجہ;
  ثنائی_ہندسہ(نظر_آیا_eos);
  ثنائی_ہندسہ(errored);
  ثنائی_ہندسہ(has_مستعمل_cb);
  ثنائی_ہندسہ(is_paused);
};

ساکن CURLcode cr_اندر_init(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  ctx->read_cb = ڈیٹا->state.fread_func;
  ctx->cb_user_ڈیٹا = ڈیٹا->state.اندر;
  ctx->کل_len = -1;
  ctx->read_len = 0;
  واپس CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساکن CURLcode cr_اندر_read(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  اگر(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    واپس ctx->error_نتیجہ;
  }
  اگر(ctx->نظر_آیا_eos) {
    *pnread = 0;
    *peos = TRUE;
    واپس CURLE_OK;
  }
  /* respect length limitations */
  اگر(ctx->کل_len >= 0) {
    blen = curlx_sotouz_range(ctx->کل_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  اگر(ctx->read_cb && blen) {
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_cr_اندر_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_ڈیٹا);
    CURL_CBAPI_انتہا(&guard);
    ctx->has_مستعمل_cb = TRUE;
  }

  انتخاب(nread) {
  حالت 0:
    اگر((ctx->کل_len >= 0) && (ctx->read_len < ctx->کل_len)) {
      failf(ڈیٹا, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->کل_len);
      نتیجہ = CURLE_READ_ERROR;
      توڑو;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->نظر_آیا_eos = TRUE;
    توڑو;

  حالت CURL_READFUNC_ABORT:
    failf(ڈیٹا, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    توڑو;

  حالت CURL_READFUNC_PAUSE:
    اگر(ڈیٹا->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(ڈیٹا, "Read callback asked for PAUSE when not supported");
      نتیجہ = CURLE_READ_ERROR;
      توڑو;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ڈیٹا, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    نتیجہ = Curl_xfer_pause_send(ڈیٹا, TRUE);
    توڑو; /* nothing was read */

  طےشدہ:
    اگر(nread > blen) {
      /* the read function returned a too large value */
      failf(ڈیٹا, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتیجہ = CURLE_READ_ERROR;
      نتیجہ = CURLE_READ_ERROR;
      توڑو;
    }
    ctx->read_len += nread;
    اگر(ctx->کل_len >= 0)
      ctx->نظر_آیا_eos = (ctx->read_len >= ctx->کل_len);
    *pnread = nread;
    *peos = (bool)ctx->نظر_آیا_eos;
    توڑو;
  }
  CURL_TRC_READ(ڈیٹا, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->کل_len, ctx->read_len, (صحیح)نتیجہ,
                *pnread, *peos);
  واپس نتیجہ;
}

ساکن bool cr_اندر_needs_rewind(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creader *reader)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس (bool)ctx->has_مستعمل_cb;
}

ساکن curl_off_t cr_اندر_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_creader *reader)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس ctx->کل_len;
}

ساکن CURLcode cr_اندر_resume_from(ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_creader *reader,
                                  curl_off_t انحراف)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  صحیح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(ڈیٹا->conn);
  /* already started reading? */
  اگر(ctx->read_len)
    واپس CURLE_READ_ERROR;

  اگر(ڈیٹا->سیٹ.seek_func) {
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_seek_func);
    seekerr = ڈیٹا->سیٹ.seek_func(ڈیٹا->سیٹ.seek_client, انحراف, SEEK_SET);
    CURL_CBAPI_انتہا(&guard);
  }

  اگر(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(ڈیٹا, "Could not seek stream");
      واپس CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    کرو {
      ساخت Curl_mapi_guard guard;
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (انحراف - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(انحراف - passed);
      size_t actuallyread;

      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_cr_اندر_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_ڈیٹا);
      CURL_CBAPI_انتہا(&guard);

      passed += actuallyread;
      اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ڈیٹا, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        واپس CURLE_READ_ERROR;
      }
    } جب(passed < انحراف);
  }

  /* now, decrease the size of the read */
  اگر(ctx->کل_len > 0) {
    ctx->کل_len -= انحراف;

    اگر(ctx->کل_len <= 0) {
      failf(ڈیٹا, "File already completely uploaded");
      واپس CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  واپس CURLE_OK;
}

ساکن CURLcode cr_اندر_rewind(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  اگر(!ctx->has_مستعمل_cb)
    واپس CURLE_OK;

  اگر(ڈیٹا->سیٹ.seek_func) {
    ساخت Curl_mapi_guard guard;
    صحیح err;

    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_seek_func);
    err = ڈیٹا->سیٹ.seek_func(ڈیٹا->سیٹ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_انتہا(&guard);
    CURL_TRC_READ(ڈیٹا, "cr_in, rewind via set.seek_func -> %d", err);
    اگر(err) {
      failf(ڈیٹا, "seek callback returned error %d", err);
      واپس CURLE_SEND_FAIL_REWIND;
    }
  }
  ورنہ اگر(ڈیٹا->سیٹ.ioctl_func) {
    ساخت Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_ioctl_func);
    err = ڈیٹا->سیٹ.ioctl_func(ڈیٹا, CURLIOCMD_RESTARTREAD,
                               ڈیٹا->سیٹ.ioctl_client);
    CURL_CBAPI_انتہا(&guard);
    CURL_TRC_READ(ڈیٹا, "cr_in, rewind via set.ioctl_func -> %d", (صحیح)err);
    اگر(err) {
      failf(ڈیٹا, "ioctl callback returned error %d", (صحیح)err);
      واپس CURLE_SEND_FAIL_REWIND;
    }
  }
  ورنہ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
    اگر(ڈیٹا->state.fread_func == (curl_read_callback)fread) {
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
      صحیح err = fseek(ڈیٹا->state.اندر, 0, SEEK_SET);
      CURL_TRC_READ(ڈیٹا, "cr_in, rewind via fseek -> %d(%d)",
                    (صحیح)err, (صحیح)errno);
      اگر(err != -1)
        /* successful rewind */
        واپس CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(ڈیٹا, "necessary data rewind was not possible");
    واپس CURLE_SEND_FAIL_REWIND;
  }
  واپس CURLE_OK;
}

ساکن CURLcode cr_اندر_cntrl(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;

  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND:
    واپس cr_اندر_rewind(ڈیٹا, reader);
  حالت CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    توڑو;
  حالت CURL_CRCNTRL_CLEAR_EOS:
    ctx->نظر_آیا_eos = FALSE;
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن bool cr_اندر_is_paused(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader)
{
  ساخت cr_اندر_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس (bool)ctx->is_paused;
}

ساکن ثابت ساخت Curl_crtype cr_اندر = {
  "cr-in",
  cr_اندر_init,
  cr_اندر_read,
  Curl_creader_def_close,
  cr_اندر_needs_rewind,
  cr_اندر_کل_لمبائی,
  cr_اندر_resume_from,
  cr_اندر_cntrl,
  cr_اندر_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_اندر_ctx)
};

CURLcode Curl_creader_create(ساخت Curl_creader **preader,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ساخت Curl_creader *reader = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  خالی *p;

  DEBUGASSERT(crt->creader_سائز >= حجم(ساخت Curl_creader));
  p = curlx_calloc(1, crt->creader_سائز);
  اگر(!p)
    جاؤ برآمد;

  reader = (ساخت Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  نتیجہ = crt->do_init(ڈیٹا, reader);

برآمد:
  *preader = نتیجہ ? تہی : reader;
  اگر(نتیجہ)
    curlx_free(reader);
  واپس نتیجہ;
}

خالی Curl_creader_free(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  اگر(reader) {
    reader->crt->do_close(ڈیٹا, reader);
    curlx_free(reader);
  }
}

ساخت cr_lc_ctx {
  ساخت Curl_creader super;
  ساخت bufq buf;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
  ثنائی_ہندسہ(پچھلا_cr);   /* the last byte was a CR */
};

ساکن CURLcode cr_lc_init(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  ساخت cr_lc_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندی);
  واپس CURLE_OK;
}

ساکن خالی cr_lc_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  ساخت cr_lc_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ساکن CURLcode cr_lc_read(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساخت cr_lc_ctx *ctx = reader->ctx;
  CURLcode نتیجہ;
  size_t nread, i, آغاز, n;
  bool eos;

  اگر(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    واپس CURLE_OK;
  }

  اگر(Curl_bufq_is_خالی_خانے(&ctx->buf)) {
    اگر(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      واپس CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    نتیجہ = Curl_creader_read(ڈیٹا, reader->اگلا, buf, blen, &nread, &eos);
    اگر(نتیجہ)
      واپس نتیجہ;
    ctx->read_eos = eos;

    اگر(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      اگر(nread)
        ctx->پچھلا_cr = (buf[nread - 1] == '\r');
      اگر(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      جاؤ برآمد;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    برائے(i = آغاز = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      اگر((buf[i] != '\n') || ctx->پچھلا_cr) {
        ctx->پچھلا_cr = (buf[i] == '\r');
        جاری;
      }
      ctx->پچھلا_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + آغاز, i - آغاز, &n);
      اگر(!نتیجہ)
        نتیجہ = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      اگر(نتیجہ)
        واپس نتیجہ;
      آغاز = i + 1;
    }

    اگر(آغاز < i) { /* leftover */
      نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + آغاز, i - آغاز, &n);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_خالی_خانے(&ctx->buf));
  *peos = FALSE;
  نتیجہ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  اگر(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_خانے(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

برآمد:
  CURL_TRC_READ(ڈیٹا, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)نتیجہ, *pnread, *peos);
  واپس نتیجہ;
}

ساکن curl_off_t cr_lc_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس -1;
}

ساکن ثابت ساخت Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_lc_ctx)
};

ساکن CURLcode cr_lc_جوڑو(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = تہی;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, ڈیٹا, &cr_lc, CURL_CR_CONTENT_ENCODE);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(ڈیٹا, reader);

  اگر(نتیجہ && reader)
    Curl_creader_free(ڈیٹا, reader);
  واپس نتیجہ;
}

ساکن CURLcode do_init_reader_ڈھیر(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_creader *r)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!ڈیٹا->req.reader.ڈھیر);

  ڈیٹا->req.reader.ڈھیر = r;
  clen = r->crt->کل_لمبائی(ڈیٹا, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  اگر(clen &&
#اگر_تعریف_شدہ CURL_PREFER_LF_LINEENDS
    (ڈیٹا->سیٹ.crlf || ڈیٹا->state.prefer_ascii)
#ورنہ
    ڈیٹا->سیٹ.crlf
#اگر_ختم
    ) {
    نتیجہ = cr_lc_جوڑو(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس نتیجہ;
}

CURLcode Curl_creader_سیٹ_fread(ساخت Curl_easy *ڈیٹا, curl_off_t len)
{
  CURLcode نتیجہ;
  ساخت Curl_creader *r;
  ساخت cr_اندر_ctx *ctx;

  نتیجہ = Curl_creader_create(&r, ڈیٹا, &cr_اندر, CURL_CR_CLIENT);
  اگر(نتیجہ || !r)
    جاؤ برآمد;
  ctx = r->ctx;
  ctx->کل_len = len;

  cl_reset_reader(ڈیٹا);
  نتیجہ = do_init_reader_ڈھیر(ڈیٹا, r);
برآمد:
  CURL_TRC_READ(ڈیٹا, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحیح)نتیجہ);
  واپس نتیجہ;
}

CURLcode Curl_creader_جوڑو(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_creader *reader)
{
  CURLcode نتیجہ;
  ساخت Curl_creader **anchor = &ڈیٹا->req.reader.ڈھیر;

  اگر(!*anchor) {
    نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, ڈیٹا->state.infilesize);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  جب(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->اگلا;
  reader->اگلا = *anchor;
  *anchor = reader;
  واپس CURLE_OK;
}

CURLcode Curl_creader_سیٹ(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *r)
{
  CURLcode نتیجہ;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(ڈیٹا);
  نتیجہ = do_init_reader_ڈھیر(ڈیٹا, r);
  اگر(نتیجہ)
    Curl_creader_free(ڈیٹا, r);
  واپس نتیجہ;
}

CURLcode Curl_client_read(ساخت Curl_easy *ڈیٹا, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode نتیجہ;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  اگر(!ڈیٹا->req.reader.ڈھیر) {
    نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, ڈیٹا->state.infilesize);
    اگر(نتیجہ)
      واپس نتیجہ;
    DEBUGASSERT(ڈیٹا->req.reader.ڈھیر);
  }
  اگر(!ڈیٹا->req.reader_started) {
    Curl_rlimit_آغاز(&ڈیٹا->پیش_رفت.ul.rlimit, Curl_pgrs_now(ڈیٹا), -1);
    ڈیٹا->req.reader_started = TRUE;
  }

  اگر(Curl_rlimit_فعال(&ڈیٹا->پیش_رفت.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&ڈیٹا->پیش_رفت.ul.rlimit, تہی);
    اگر(ul_avail <= 0) {
      نتیجہ = CURLE_OK;
      *eos = FALSE;
      جاؤ برآمد;
    }
    اگر(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  نتیجہ = Curl_creader_read(ڈیٹا, ڈیٹا->req.reader.ڈھیر, buf, blen,
                             nread, eos);

برآمد:
  CURL_TRC_READ(ڈیٹا, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)نتیجہ, *nread, *eos);
  واپس نتیجہ;
}

bool Curl_creader_needs_rewind(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = ڈیٹا->req.reader.ڈھیر;
  جب(reader) {
    اگر(reader->crt->needs_rewind(ڈیٹا, reader)) {
      CURL_TRC_READ(ڈیٹا, "client reader needs rewind before next request");
      واپس TRUE;
    }
    reader = reader->اگلا;
  }
  واپس FALSE;
}

ساکن CURLcode cr_null_read(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (خالی)ڈیٹا;
  (خالی)reader;
  (خالی)buf;
  (خالی)blen;
  *pnread = 0;
  *peos = TRUE;
  واپس CURLE_OK;
}

ساکن curl_off_t cr_null_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس 0;
}

ساکن ثابت ساخت Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت Curl_creader)
};

CURLcode Curl_creader_سیٹ_null(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&r, ڈیٹا, &cr_null, CURL_CR_CLIENT);
  اگر(نتیجہ)
    واپس نتیجہ;

  cl_reset_reader(ڈیٹا);
  واپس do_init_reader_ڈھیر(ڈیٹا, r);
}

ساخت cr_buf_ctx {
  ساخت Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t اشاریہ;
};

ساکن CURLcode cr_buf_read(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ساخت cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->اشاریہ;

  اگر(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ورنہ {
    اگر(nread > blen)
      nread = blen;
    حافظہ_نقل(buf, ctx->buf + ctx->اشاریہ, nread);
    *pnread = nread;
    ctx->اشاریہ += nread;
    *peos = (ctx->اشاریہ == ctx->blen);
  }
  CURL_TRC_READ(ڈیٹا, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  واپس CURLE_OK;
}

ساکن bool cr_buf_needs_rewind(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_creader *reader)
{
  ساخت cr_buf_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس ctx->اشاریہ > 0;
}

ساکن CURLcode cr_buf_cntrl(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ساخت cr_buf_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND:
    ctx->اشاریہ = 0;
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن curl_off_t cr_buf_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_creader *reader)
{
  ساخت cr_buf_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس (curl_off_t)ctx->blen;
}

ساکن CURLcode cr_buf_resume_from(ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_creader *reader,
                                   curl_off_t انحراف)
{
  ساخت cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (خالی)ڈیٹا;
  DEBUGASSERT(ڈیٹا->conn);
  /* already started reading? */
  اگر(ctx->اشاریہ)
    واپس CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(انحراف, 0, SIZE_MAX);
  اگر(!boffset)
    واپس CURLE_OK;
  اگر(boffset > ctx->blen)
    واپس CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  واپس CURLE_OK;
}

ساکن ثابت ساخت Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_کل_لمبائی,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_buf_ctx)
};

CURLcode Curl_creader_سیٹ_buf(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ;
  ساخت Curl_creader *r;
  ساخت cr_buf_ctx *ctx;

  نتیجہ = Curl_creader_create(&r, ڈیٹا, &cr_buf, CURL_CR_CLIENT);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->اشاریہ = 0;

  cl_reset_reader(ڈیٹا);
  نتیجہ = do_init_reader_ڈھیر(ڈیٹا, r);
برآمد:
  CURL_TRC_READ(ڈیٹا, "add buf reader, len=%zu -> %d", blen, (صحیح)نتیجہ);
  واپس نتیجہ;
}

curl_off_t Curl_creader_کل_لمبائی(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = ڈیٹا->req.reader.ڈھیر;
  واپس r ? r->crt->کل_لمبائی(ڈیٹا, r) : -1;
}

curl_off_t Curl_creader_client_لمبائی(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = ڈیٹا->req.reader.ڈھیر;
  جب(r && r->phase != CURL_CR_CLIENT)
    r = r->اگلا;
  واپس r ? r->crt->کل_لمبائی(ڈیٹا, r) : -1;
}

CURLcode Curl_creader_unpause(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = ڈیٹا->req.reader.ڈھیر;
  CURLcode نتیجہ = CURLE_OK;

  جب(reader) {
    نتیجہ = reader->crt->cntrl(ڈیٹا, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(ڈیٹا, "unpausing %s -> %d", reader->crt->name, (صحیح)نتیجہ);
    اگر(نتیجہ)
      توڑو;
    reader = reader->اگلا;
  }
  واپس نتیجہ;
}

bool Curl_creader_is_paused(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = ڈیٹا->req.reader.ڈھیر;

  جب(reader) {
    اگر(reader->crt->is_paused(ڈیٹا, reader))
      واپس TRUE;
    reader = reader->اگلا;
  }
  واپس FALSE;
}

خالی Curl_creader_مکمل(ساخت Curl_easy *ڈیٹا, صحیح premature)
{
  ساخت Curl_creader *reader = ڈیٹا->req.reader.ڈھیر;
  جب(reader) {
    reader->crt->مکمل(ڈیٹا, reader, premature);
    reader = reader->اگلا;
  }
}

ساخت Curl_creader *Curl_creader_get_by_type(ساخت Curl_easy *ڈیٹا,
                                              ثابت ساخت Curl_crtype *crt)
{
  ساخت Curl_creader *r;
  برائے(r = ڈیٹا->req.reader.ڈھیر; r; r = r->اگلا) {
    اگر(r->crt == crt)
      واپس r;
  }
  واپس تہی;
}
