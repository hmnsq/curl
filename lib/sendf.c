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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت

#جے_تعریف_شدہ HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#جے_انت

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "cw-out.h"
#ضمن "cw-pause.h"
#ضمن "multiif.h"
#ضمن "progress.h"

ساكن فراغ cl_reset_writer(بنية Curl_easy *انکڑے)
{
  بنية Curl_cwriter *writer = انکڑے->req.writer.ڈھیر;
  طالما(writer) {
    انکڑے->req.writer.ڈھیر = writer->اگلا;
    writer->cwt->do_close(انکڑے, writer);
    curlx_free(writer);
    writer = انکڑے->req.writer.ڈھیر;
  }
  انکڑے->req.writer.paused = FALSE;
}

ساكن فراغ cl_reset_reader(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = انکڑے->req.reader.ڈھیر;
  انکڑے->req.reader_started = FALSE;
  طالما(reader) {
    انکڑے->req.reader.ڈھیر = reader->اگلا;
    reader->crt->do_close(انکڑے, reader);
    curlx_free(reader);
    reader = انکڑے->req.reader.ڈھیر;
  }
}

فراغ Curl_client_cleanup(بنية Curl_easy *انکڑے)
{
  cl_reset_reader(انکڑے);
  cl_reset_writer(انکڑے);

  انکڑے->req.bytecount = 0;
  انکڑے->req.headerline = 0;
}

فراغ Curl_client_reset(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(انکڑے, "client_reset, will rewind reader");
  }
  وإلا {
    CURL_TRC_READ(انکڑے, "client_reset, clear readers");
    cl_reset_reader(انکڑے);
  }
  cl_reset_writer(انکڑے);

  انکڑے->req.bytecount = 0;
  انکڑے->req.headerline = 0;
}

CURLcode Curl_client_شروع(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->req.rewind_read) {
    بنية Curl_creader *r = انکڑے->req.reader.ڈھیر;
    CURLcode نتیجہ = CURLE_OK;

    CURL_TRC_READ(انکڑے, "client start, rewind readers");
    طالما(r) {
      نتیجہ = r->crt->cntrl(انکڑے, r, CURL_CRCNTRL_REWIND);
      إذا(نتیجہ) {
        failf(انکڑے, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      r = r->اگلا;
    }
    انکڑے->req.rewind_read = FALSE;
    cl_reset_reader(انکڑے);
  }
  أرجع CURLE_OK;
}

bool Curl_creader_will_rewind(بنية Curl_easy *انکڑے)
{
  أرجع (bool)انکڑے->req.rewind_read;
}

فراغ Curl_creader_سیٹ_rewind(بنية Curl_easy *انکڑے, bool enable)
{
  انکڑے->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(بنية Curl_easy *انکڑے,
                               بنية Curl_cwriter *writer)
{
  (فراغ)انکڑے;
  (فراغ)writer;
  أرجع CURLE_OK;
}

CURLcode Curl_cwriter_def_write(بنية Curl_easy *انکڑے,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(بنية Curl_easy *انکڑے,
                                بنية Curl_cwriter *writer)
{
  أرجع Curl_cwriter_flush(انکڑے, writer->اگلا);
}

فراغ Curl_cwriter_def_close(بنية Curl_easy *انکڑے,
                            بنية Curl_cwriter *writer)
{
  (فراغ)انکڑے;
  (فراغ)writer;
}

ساكن size_t get_max_body_write_len(بنية Curl_easy *انکڑے, curl_off_t حد_بندی)
{
  إذا(حد_بندی != -1) {
    /* How much more are we allowed to write? */
    أرجع curlx_sotouz_range(حد_بندی - انکڑے->req.bytecount, 0, SIZE_MAX);
  }
  أرجع SIZE_MAX;
}

بنية cw_download_ctx {
  بنية Curl_cwriter super;
  ثنائی_ہندسہ(started_response);
  ثنائی_ہندسہ(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساكن CURLcode cw_download_write(بنية Curl_easy *انکڑے,
                                  بنية Curl_cwriter *writer, صحيح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  بنية cw_download_ctx *ctx = writer->ctx;
  CURLcode نتیجہ;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  إذا(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || انکڑے->req.upload_پورا)) {
    Curl_pgrs_ویلا(انکڑے, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  إذا(!(type & CLIENTWRITE_BODY)) {
    إذا(is_connect && انکڑے->سیٹ.suppress_connect_سرخیاں)
      أرجع CURLE_OK;
#جے_تعریف_شدہ DEBUGBUILD
    إذا(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      أرجع CURLE_OK;
#جے_انت
    نتیجہ = Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, nbytes);
    CURL_TRC_WRITE(انکڑے, "download_write header(type=%x, blen=%zu) -> %d",
                   (بنانشانی صحيح)type, nbytes, (صحيح)نتیجہ);
    أرجع نتیجہ;
  }

  إذا(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_شروع(&انکڑے->ترقی.dl.rlimit, Curl_pgrs_now(انکڑے),
                      انکڑے->req.ناپ);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  إذا(انکڑے->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(انکڑے->conn);
    CURL_TRC_WRITE(انکڑے, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (بنانشانی صحيح)type, nbytes);
    انکڑے->req.download_پورا = TRUE;
    إذا(انکڑے->info.سرخی_ناپ)
      /* if headers have been received, this is fine */
      أرجع CURLE_OK;
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  إذا(انکڑے->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(انکڑے, انکڑے->req.maxdownload);
    إذا(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    إذا(nwrite == wmax) {
      انکڑے->req.download_پورا = TRUE;
    }

    إذا((type & CLIENTWRITE_EOS) && !انکڑے->req.no_body &&
       (انکڑے->req.ناپ > انکڑے->req.bytecount)) {
      failf(انکڑے, "end of response with %" FMT_OFF_T " bytes missing",
            انکڑے->req.ناپ - انکڑے->req.bytecount);
      أرجع CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  إذا(انکڑے->سیٹ.max_filesize && !انکڑے->req.ignorebody) {
    size_t wmax = get_max_body_write_len(انکڑے, انکڑے->سیٹ.max_filesize);
    إذا(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  إذا(!انکڑے->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    نتیجہ = Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, nwrite);
    CURL_TRC_WRITE(انکڑے, "download_write body(type=%x, blen=%zu) -> %d",
                   (بنانشانی صحيح)type, nbytes, (صحيح)نتیجہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* Update stats, write and report progress */
  إذا(nwrite) {
    انکڑے->req.bytecount += nwrite;
    Curl_pgrs_download_inc(انکڑے, nwrite);
  }

  إذا(excess_len) {
    إذا(!انکڑے->req.ignorebody) {
      infof(انکڑے,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, انکڑے->req.ناپ, انکڑے->req.maxdownload,
            انکڑے->req.bytecount);
      connclose(انکڑے->conn);
    }
  }
  وإلا إذا((nwrite < nbytes) && !انکڑے->req.ignorebody) {
    failf(انکڑے, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          انکڑے->سیٹ.max_filesize, انکڑے->req.bytecount);
    أرجع CURLE_FILESIZE_EXCEEDED;
  }

  أرجع CURLE_OK;
}

ساكن ثابت بنية Curl_cwtype cw_download = {
  "protocol",
  عدم,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ساكن CURLcode cw_raw_write(بنية Curl_easy *انکڑے,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t nbytes)
{
  إذا(type & CLIENTWRITE_BODY && انکڑے->سیٹ.verbose && !انکڑے->req.ignorebody) {
    Curl_debug(انکڑے, CURLINFO_انکڑے_وچ, buf, nbytes);
  }
  أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, nbytes);
}

ساكن ثابت بنية Curl_cwtype cw_raw = {
  "raw",
  عدم,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية Curl_cwriter)
};

ساكن فراغ cwriter_جوڑو(بنية Curl_easy *انکڑے,
                        بنية Curl_cwriter *writer)
{
  بنية Curl_cwriter **anchor = &انکڑے->req.writer.ڈھیر;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  طالما(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->اگلا;
  writer->اگلا = *anchor;
  *anchor = writer;
}

ساكن CURLcode do_init_writer_ڈھیر(بنية Curl_easy *انکڑے)
{
  بنية Curl_cwriter *writer;
  CURLcode نتیجہ;

  DEBUGASSERT(!انکڑے->req.writer.ڈھیر);
  نتیجہ = Curl_cwriter_create(&انکڑے->req.writer.ڈھیر,
                               انکڑے, &Curl_cwt_باہرلا, CURL_CW_CLIENT);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_cwriter_create(&writer, انکڑے, &cw_download, CURL_CW_PROTOCOL);
  إذا(نتیجہ)
    أرجع نتیجہ;
  cwriter_جوڑو(انکڑے, writer);

  نتیجہ = Curl_cwriter_create(&writer, انکڑے, &cw_raw, CURL_CW_RAW);
  إذا(نتیجہ)
    أرجع نتیجہ;
  cwriter_جوڑو(انکڑے, writer);

  أرجع نتیجہ;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(بنية Curl_easy *انکڑے, صحيح type, ثابت حرف *buf,
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

  إذا(!انکڑے->req.writer.ڈھیر) {
    نتیجہ = do_init_writer_ڈھیر(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
    DEBUGASSERT(انکڑے->req.writer.ڈھیر);
  }

  نتیجہ = Curl_cwriter_write(انکڑے, انکڑے->req.writer.ڈھیر, type, buf, len);
  CURL_TRC_WRITE(انکڑے, "client_write(type=%x, len=%zu) -> %d",
                 (بنانشانی صحيح)type, len, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

CURLcode Curl_client_flush(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;

  إذا(!انکڑے->req.writer.ڈھیر) {
    نتیجہ = do_init_writer_ڈھیر(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
    DEBUGASSERT(انکڑے->req.writer.ڈھیر);
  }

  نتیجہ = Curl_cwriter_flush(انکڑے, انکڑے->req.writer.ڈھیر);
  CURL_TRC_WRITE(انکڑے, "client_flush() -> %d", (صحيح)نتیجہ);
  أرجع نتیجہ;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(بنية Curl_cwriter **pwriter,
                             بنية Curl_easy *انکڑے,
                             ثابت بنية Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *writer = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  فراغ *p;

  DEBUGASSERT(cwt->cwriter_ناپ >= حجم(بنية Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_ناپ);
  إذا(!p)
    اذهب باہرلا;

  writer = (بنية Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  نتیجہ = cwt->do_init(انکڑے, writer);

باہرلا:
  *pwriter = نتیجہ ? عدم : writer;
  إذا(نتیجہ)
    curlx_free(writer);
  أرجع نتیجہ;
}

فراغ Curl_cwriter_free(بنية Curl_easy *انکڑے,
                       بنية Curl_cwriter *writer)
{
  إذا(writer) {
    writer->cwt->do_close(انکڑے, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_گنتی(بنية Curl_easy *انکڑے, Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *w;
  size_t n = 0;

  لكل(w = انکڑے->req.writer.ڈھیر; w; w = w->اگلا) {
    إذا(w->phase == phase)
      ++n;
  }
  أرجع n;
}

ساكن CURLcode cwriter_ensure_pause_writer(بنية Curl_easy *انکڑے)
{
  بنية Curl_cwriter *writer =
    Curl_cwriter_get_by_type(انکڑے, &Curl_cwt_pause);
  CURLcode نتیجہ = CURLE_OK;

  إذا(!writer) {
    نتیجہ = Curl_cwriter_create(&writer, انکڑے, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    إذا(!نتیجہ)
      cwriter_جوڑو(انکڑے, writer);
  }
  أرجع نتیجہ;
}

CURLcode Curl_cwriter_جوڑو(بنية Curl_easy *انکڑے,
                          بنية Curl_cwriter *writer)
{
  CURLcode نتیجہ;
  بنية Curl_cwriter **anchor = &انکڑے->req.writer.ڈھیر;

  إذا(!*anchor) {
    نتیجہ = do_init_writer_ڈھیر(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    نتیجہ = cwriter_ensure_pause_writer(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  cwriter_جوڑو(انکڑے, writer);
  أرجع CURLE_OK;
}

بنية Curl_cwriter *Curl_cwriter_get_by_name(بنية Curl_easy *انکڑے,
                                              ثابت حرف *name)
{
  بنية Curl_cwriter *writer;
  لكل(writer = انکڑے->req.writer.ڈھیر; writer; writer = writer->اگلا) {
    إذا(!strcmp(name, writer->cwt->name))
      أرجع writer;
  }
  أرجع عدم;
}

بنية Curl_cwriter *Curl_cwriter_get_by_type(بنية Curl_easy *انکڑے,
                                              ثابت بنية Curl_cwtype *cwt)
{
  بنية Curl_cwriter *writer;
  لكل(writer = انکڑے->req.writer.ڈھیر; writer; writer = writer->اگلا) {
    إذا(writer->cwt == cwt)
      أرجع writer;
  }
  أرجع عدم;
}

CURLcode Curl_cwriter_unpause(بنية Curl_easy *انکڑے)
{
  انکڑے->req.writer.paused = FALSE;
  أرجع Curl_cwriter_flush(انکڑے, انکڑے->req.writer.ڈھیر);
}

CURLcode Curl_creader_read(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  إذا(!reader)
    أرجع CURLE_READ_ERROR;
  أرجع reader->crt->do_read(انکڑے, reader, buf, blen, nread, eos);
}

فراغ Curl_creader_clear_eos(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader)
{
  طالما(reader) {
    (فراغ)reader->crt->cntrl(انکڑے, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->اگلا;
  }
}

CURLcode Curl_creader_def_init(بنية Curl_easy *انکڑے,
                               بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع CURLE_OK;
}

فراغ Curl_creader_def_close(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
}

CURLcode Curl_creader_def_read(بنية Curl_easy *انکڑے,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  إذا(reader->اگلا)
    أرجع reader->اگلا->crt->do_read(انکڑے, reader->اگلا, buf, blen,
                                      nread, eos);
  وإلا {
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(بنية Curl_easy *انکڑے,
                                   بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع FALSE;
}

curl_off_t Curl_creader_def_کل_لمائی(بنية Curl_easy *انکڑے,
                                         بنية Curl_creader *reader)
{
  أرجع reader->اگلا ?
         reader->اگلا->crt->کل_لمائی(انکڑے, reader->اگلا) : -1;
}

CURLcode Curl_creader_def_resume_from(بنية Curl_easy *انکڑے,
                                      بنية Curl_creader *reader,
                                      curl_off_t وتھ)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  (فراغ)وتھ;
  أرجع CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(بنية Curl_easy *انکڑے,
                                بنية Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  (فراغ)opcode;
  أرجع CURLE_OK;
}

bool Curl_creader_def_is_paused(بنية Curl_easy *انکڑے,
                                بنية Curl_creader *reader)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع FALSE;
}

فراغ Curl_creader_def_پورا(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader, صحيح premature)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  (فراغ)premature;
}

بنية cr_وچ_ctx {
  بنية Curl_creader super;
  curl_read_callback read_cb;
  فراغ *cb_user_انکڑے;
  curl_off_t کل_len;
  curl_off_t read_len;
  CURLcode error_نتیجہ;
  ثنائی_ہندسہ(تکیا_eos);
  ثنائی_ہندسہ(errored);
  ثنائی_ہندسہ(has_ورتیا_cb);
  ثنائی_ہندسہ(is_paused);
};

ساكن CURLcode cr_وچ_init(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  ctx->read_cb = انکڑے->state.fread_func;
  ctx->cb_user_انکڑے = انکڑے->state.وچ;
  ctx->کل_len = -1;
  ctx->read_len = 0;
  أرجع CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_وچ_read(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  إذا(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_نتیجہ;
  }
  إذا(ctx->تکیا_eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  إذا(ctx->کل_len >= 0) {
    blen = curlx_sotouz_range(ctx->کل_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  إذا(ctx->read_cb && blen) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, انکڑے, easy_cr_وچ_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_انکڑے);
    CURL_CBAPI_سرا(&guard);
    ctx->has_ورتیا_cb = TRUE;
  }

  بدل(nread) {
  حالة 0:
    إذا((ctx->کل_len >= 0) && (ctx->read_len < ctx->کل_len)) {
      failf(انکڑے, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->کل_len);
      نتیجہ = CURLE_READ_ERROR;
      اكسر;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->تکیا_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(انکڑے, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    إذا(انکڑے->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(انکڑے, "Read callback asked for PAUSE when not supported");
      نتیجہ = CURLE_READ_ERROR;
      اكسر;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(انکڑے, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    نتیجہ = Curl_xfer_pause_send(انکڑے, TRUE);
    اكسر; /* nothing was read */

  افتراضي:
    إذا(nread > blen) {
      /* the read function returned a too large value */
      failf(انکڑے, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتیجہ = CURLE_READ_ERROR;
      نتیجہ = CURLE_READ_ERROR;
      اكسر;
    }
    ctx->read_len += nread;
    إذا(ctx->کل_len >= 0)
      ctx->تکیا_eos = (ctx->read_len >= ctx->کل_len);
    *pnread = nread;
    *peos = (bool)ctx->تکیا_eos;
    اكسر;
  }
  CURL_TRC_READ(انکڑے, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->کل_len, ctx->read_len, (صحيح)نتیجہ,
                *pnread, *peos);
  أرجع نتیجہ;
}

ساكن bool cr_وچ_needs_rewind(بنية Curl_easy *انکڑے,
                               بنية Curl_creader *reader)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  أرجع (bool)ctx->has_ورتیا_cb;
}

ساكن curl_off_t cr_وچ_کل_لمائی(بنية Curl_easy *انکڑے,
                                     بنية Curl_creader *reader)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  أرجع ctx->کل_len;
}

ساكن CURLcode cr_وچ_resume_from(بنية Curl_easy *انکڑے,
                                  بنية Curl_creader *reader,
                                  curl_off_t وتھ)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  صحيح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(انکڑے->conn);
  /* already started reading? */
  إذا(ctx->read_len)
    أرجع CURLE_READ_ERROR;

  إذا(انکڑے->سیٹ.seek_func) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, انکڑے, easy_seek_func);
    seekerr = انکڑے->سیٹ.seek_func(انکڑے->سیٹ.seek_client, وتھ, SEEK_SET);
    CURL_CBAPI_سرا(&guard);
  }

  إذا(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(انکڑے, "Could not seek stream");
      أرجع CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    افعل {
      بنية Curl_mapi_guard guard;
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (وتھ - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(وتھ - passed);
      size_t actuallyread;

      CURL_CBAPI_شروع(&guard, انکڑے, easy_cr_وچ_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_انکڑے);
      CURL_CBAPI_سرا(&guard);

      passed += actuallyread;
      إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(انکڑے, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < وتھ);
  }

  /* now, decrease the size of the read */
  إذا(ctx->کل_len > 0) {
    ctx->کل_len -= وتھ;

    إذا(ctx->کل_len <= 0) {
      failf(انکڑے, "File already completely uploaded");
      أرجع CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  أرجع CURLE_OK;
}

ساكن CURLcode cr_وچ_rewind(بنية Curl_easy *انکڑے,
                             بنية Curl_creader *reader)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  إذا(!ctx->has_ورتیا_cb)
    أرجع CURLE_OK;

  إذا(انکڑے->سیٹ.seek_func) {
    بنية Curl_mapi_guard guard;
    صحيح err;

    CURL_CBAPI_شروع(&guard, انکڑے, easy_seek_func);
    err = انکڑے->سیٹ.seek_func(انکڑے->سیٹ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_سرا(&guard);
    CURL_TRC_READ(انکڑے, "cr_in, rewind via set.seek_func -> %d", err);
    إذا(err) {
      failf(انکڑے, "seek callback returned error %d", err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا إذا(انکڑے->سیٹ.ioctl_func) {
    بنية Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_شروع(&guard, انکڑے, easy_ioctl_func);
    err = انکڑے->سیٹ.ioctl_func(انکڑے, CURLIOCMD_RESTARTREAD,
                               انکڑے->سیٹ.ioctl_client);
    CURL_CBAPI_سرا(&guard);
    CURL_TRC_READ(انکڑے, "cr_in, rewind via set.ioctl_func -> %d", (صحيح)err);
    إذا(err) {
      failf(انکڑے, "ioctl callback returned error %d", (صحيح)err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#جے_انت
    إذا(انکڑے->state.fread_func == (curl_read_callback)fread) {
#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#جے_انت
      صحيح err = fseek(انکڑے->state.وچ, 0, SEEK_SET);
      CURL_TRC_READ(انکڑے, "cr_in, rewind via fseek -> %d(%d)",
                    (صحيح)err, (صحيح)errno);
      إذا(err != -1)
        /* successful rewind */
        أرجع CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(انکڑے, "necessary data rewind was not possible");
    أرجع CURLE_SEND_FAIL_REWIND;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cr_وچ_cntrl(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;

  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    أرجع cr_وچ_rewind(انکڑے, reader);
  حالة CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->تکیا_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_وچ_is_paused(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader)
{
  بنية cr_وچ_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  أرجع (bool)ctx->is_paused;
}

ساكن ثابت بنية Curl_crtype cr_وچ = {
  "cr-in",
  cr_وچ_init,
  cr_وچ_read,
  Curl_creader_def_close,
  cr_وچ_needs_rewind,
  cr_وچ_کل_لمائی,
  cr_وچ_resume_from,
  cr_وچ_cntrl,
  cr_وچ_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية cr_وچ_ctx)
};

CURLcode Curl_creader_create(بنية Curl_creader **preader,
                             بنية Curl_easy *انکڑے,
                             ثابت بنية Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  فراغ *p;

  DEBUGASSERT(crt->creader_ناپ >= حجم(بنية Curl_creader));
  p = curlx_calloc(1, crt->creader_ناپ);
  إذا(!p)
    اذهب باہرلا;

  reader = (بنية Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  نتیجہ = crt->do_init(انکڑے, reader);

باہرلا:
  *preader = نتیجہ ? عدم : reader;
  إذا(نتیجہ)
    curlx_free(reader);
  أرجع نتیجہ;
}

فراغ Curl_creader_free(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  إذا(reader) {
    reader->crt->do_close(انکڑے, reader);
    curlx_free(reader);
  }
}

بنية cr_lc_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
  ثنائی_ہندسہ(prev_cr);   /* the last byte was a CR */
};

ساكن CURLcode cr_lc_init(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندی);
  أرجع CURLE_OK;
}

ساكن فراغ cr_lc_close(بنية Curl_easy *انکڑے, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ساكن CURLcode cr_lc_read(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  CURLcode نتیجہ;
  size_t nread, i, شروع, n;
  bool eos;

  إذا(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->buf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      أرجع CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    نتیجہ = Curl_creader_read(انکڑے, reader->اگلا, buf, blen, &nread, &eos);
    إذا(نتیجہ)
      أرجع نتیجہ;
    ctx->read_eos = eos;

    إذا(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      إذا(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب باہرلا;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    لكل(i = شروع = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      إذا((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        استمر;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
      إذا(!نتیجہ)
        نتیجہ = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      إذا(نتیجہ)
        أرجع نتیجہ;
      شروع = i + 1;
    }

    إذا(شروع < i) { /* leftover */
      نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_خالی_تھاواں(&ctx->buf));
  *peos = FALSE;
  نتیجہ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  إذا(!نتیجہ && ctx->read_eos && Curl_bufq_is_خالی_تھاواں(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

باہرلا:
  CURL_TRC_READ(انکڑے, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجہ, *pnread, *peos);
  أرجع نتیجہ;
}

ساكن curl_off_t cr_lc_کل_لمائی(بنية Curl_easy *انکڑے,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية cr_lc_ctx)
};

ساكن CURLcode cr_lc_جوڑو(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, انکڑے, &cr_lc, CURL_CR_CONTENT_ENCODE);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(انکڑے, reader);

  إذا(نتیجہ && reader)
    Curl_creader_free(انکڑے, reader);
  أرجع نتیجہ;
}

ساكن CURLcode do_init_reader_ڈھیر(بنية Curl_easy *انکڑے,
                                     بنية Curl_creader *r)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!انکڑے->req.reader.ڈھیر);

  انکڑے->req.reader.ڈھیر = r;
  clen = r->crt->کل_لمائی(انکڑے, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  إذا(clen &&
#جے_تعریف_شدہ CURL_PREFER_LF_LINEENDS
    (انکڑے->سیٹ.crlf || انکڑے->state.prefer_ascii)
#وإلا
    انکڑے->سیٹ.crlf
#جے_انت
    ) {
    نتیجہ = cr_lc_جوڑو(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  أرجع نتیجہ;
}

CURLcode Curl_creader_سیٹ_fread(بنية Curl_easy *انکڑے, curl_off_t len)
{
  CURLcode نتیجہ;
  بنية Curl_creader *r;
  بنية cr_وچ_ctx *ctx;

  نتیجہ = Curl_creader_create(&r, انکڑے, &cr_وچ, CURL_CR_CLIENT);
  إذا(نتیجہ || !r)
    اذهب باہرلا;
  ctx = r->ctx;
  ctx->کل_len = len;

  cl_reset_reader(انکڑے);
  نتیجہ = do_init_reader_ڈھیر(انکڑے, r);
باہرلا:
  CURL_TRC_READ(انکڑے, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

CURLcode Curl_creader_جوڑو(بنية Curl_easy *انکڑے,
                          بنية Curl_creader *reader)
{
  CURLcode نتیجہ;
  بنية Curl_creader **anchor = &انکڑے->req.reader.ڈھیر;

  إذا(!*anchor) {
    نتیجہ = Curl_creader_سیٹ_fread(انکڑے, انکڑے->state.infilesize);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  طالما(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->اگلا;
  reader->اگلا = *anchor;
  *anchor = reader;
  أرجع CURLE_OK;
}

CURLcode Curl_creader_سیٹ(بنية Curl_easy *انکڑے, بنية Curl_creader *r)
{
  CURLcode نتیجہ;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(انکڑے);
  نتیجہ = do_init_reader_ڈھیر(انکڑے, r);
  إذا(نتیجہ)
    Curl_creader_free(انکڑے, r);
  أرجع نتیجہ;
}

CURLcode Curl_client_read(بنية Curl_easy *انکڑے, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode نتیجہ;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  إذا(!انکڑے->req.reader.ڈھیر) {
    نتیجہ = Curl_creader_سیٹ_fread(انکڑے, انکڑے->state.infilesize);
    إذا(نتیجہ)
      أرجع نتیجہ;
    DEBUGASSERT(انکڑے->req.reader.ڈھیر);
  }
  إذا(!انکڑے->req.reader_started) {
    Curl_rlimit_شروع(&انکڑے->ترقی.ul.rlimit, Curl_pgrs_now(انکڑے), -1);
    انکڑے->req.reader_started = TRUE;
  }

  إذا(Curl_rlimit_سرگرم(&انکڑے->ترقی.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&انکڑے->ترقی.ul.rlimit, عدم);
    إذا(ul_avail <= 0) {
      نتیجہ = CURLE_OK;
      *eos = FALSE;
      اذهب باہرلا;
    }
    إذا(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  نتیجہ = Curl_creader_read(انکڑے, انکڑے->req.reader.ڈھیر, buf, blen,
                             nread, eos);

باہرلا:
  CURL_TRC_READ(انکڑے, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجہ, *nread, *eos);
  أرجع نتیجہ;
}

bool Curl_creader_needs_rewind(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = انکڑے->req.reader.ڈھیر;
  طالما(reader) {
    إذا(reader->crt->needs_rewind(انکڑے, reader)) {
      CURL_TRC_READ(انکڑے, "client reader needs rewind before next request");
      أرجع TRUE;
    }
    reader = reader->اگلا;
  }
  أرجع FALSE;
}

ساكن CURLcode cr_null_read(بنية Curl_easy *انکڑے,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (فراغ)انکڑے;
  (فراغ)reader;
  (فراغ)buf;
  (فراغ)blen;
  *pnread = 0;
  *peos = TRUE;
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_null_کل_لمائی(بنية Curl_easy *انکڑے,
                                       بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)انکڑے;
  (فراغ)reader;
  أرجع 0;
}

ساكن ثابت بنية Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية Curl_creader)
};

CURLcode Curl_creader_سیٹ_null(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&r, انکڑے, &cr_null, CURL_CR_CLIENT);
  إذا(نتیجہ)
    أرجع نتیجہ;

  cl_reset_reader(انکڑے);
  أرجع do_init_reader_ڈھیر(انکڑے, r);
}

بنية cr_buf_ctx {
  بنية Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t اشاریہ;
};

ساكن CURLcode cr_buf_read(بنية Curl_easy *انکڑے,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->اشاریہ;

  إذا(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  وإلا {
    إذا(nread > blen)
      nread = blen;
    حافظہ_نقل(buf, ctx->buf + ctx->اشاریہ, nread);
    *pnread = nread;
    ctx->اشاریہ += nread;
    *peos = (ctx->اشاریہ == ctx->blen);
  }
  CURL_TRC_READ(انکڑے, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  أرجع CURLE_OK;
}

ساكن bool cr_buf_needs_rewind(بنية Curl_easy *انکڑے,
                                بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  أرجع ctx->اشاریہ > 0;
}

ساكن CURLcode cr_buf_cntrl(بنية Curl_easy *انکڑے,
                             بنية Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    ctx->اشاریہ = 0;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_buf_کل_لمائی(بنية Curl_easy *انکڑے,
                                      بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)انکڑے;
  أرجع (curl_off_t)ctx->blen;
}

ساكن CURLcode cr_buf_resume_from(بنية Curl_easy *انکڑے,
                                   بنية Curl_creader *reader,
                                   curl_off_t وتھ)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (فراغ)انکڑے;
  DEBUGASSERT(انکڑے->conn);
  /* already started reading? */
  إذا(ctx->اشاریہ)
    أرجع CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(وتھ, 0, SIZE_MAX);
  إذا(!boffset)
    أرجع CURLE_OK;
  إذا(boffset > ctx->blen)
    أرجع CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  أرجع CURLE_OK;
}

ساكن ثابت بنية Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_کل_لمائی,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورا,
  حجم(بنية cr_buf_ctx)
};

CURLcode Curl_creader_سیٹ_buf(بنية Curl_easy *انکڑے,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجہ;
  بنية Curl_creader *r;
  بنية cr_buf_ctx *ctx;

  نتیجہ = Curl_creader_create(&r, انکڑے, &cr_buf, CURL_CR_CLIENT);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->اشاریہ = 0;

  cl_reset_reader(انکڑے);
  نتیجہ = do_init_reader_ڈھیر(انکڑے, r);
باہرلا:
  CURL_TRC_READ(انکڑے, "add buf reader, len=%zu -> %d", blen, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

curl_off_t Curl_creader_کل_لمائی(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = انکڑے->req.reader.ڈھیر;
  أرجع r ? r->crt->کل_لمائی(انکڑے, r) : -1;
}

curl_off_t Curl_creader_client_لمائی(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = انکڑے->req.reader.ڈھیر;
  طالما(r && r->phase != CURL_CR_CLIENT)
    r = r->اگلا;
  أرجع r ? r->crt->کل_لمائی(انکڑے, r) : -1;
}

CURLcode Curl_creader_unpause(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = انکڑے->req.reader.ڈھیر;
  CURLcode نتیجہ = CURLE_OK;

  طالما(reader) {
    نتیجہ = reader->crt->cntrl(انکڑے, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(انکڑے, "unpausing %s -> %d", reader->crt->name, (صحيح)نتیجہ);
    إذا(نتیجہ)
      اكسر;
    reader = reader->اگلا;
  }
  أرجع نتیجہ;
}

bool Curl_creader_is_paused(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = انکڑے->req.reader.ڈھیر;

  طالما(reader) {
    إذا(reader->crt->is_paused(انکڑے, reader))
      أرجع TRUE;
    reader = reader->اگلا;
  }
  أرجع FALSE;
}

فراغ Curl_creader_پورا(بنية Curl_easy *انکڑے, صحيح premature)
{
  بنية Curl_creader *reader = انکڑے->req.reader.ڈھیر;
  طالما(reader) {
    reader->crt->پورا(انکڑے, reader, premature);
    reader = reader->اگلا;
  }
}

بنية Curl_creader *Curl_creader_get_by_type(بنية Curl_easy *انکڑے,
                                              ثابت بنية Curl_crtype *crt)
{
  بنية Curl_creader *r;
  لكل(r = انکڑے->req.reader.ڈھیر; r; r = r->اگلا) {
    إذا(r->crt == crt)
      أرجع r;
  }
  أرجع عدم;
}
