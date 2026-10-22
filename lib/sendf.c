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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#پایان_اگر

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

ساكن فراغ cl_reset_writer(بنية Curl_easy *داده)
{
  بنية Curl_cwriter *writer = داده->req.writer.پشته;
  طالما(writer) {
    داده->req.writer.پشته = writer->بعدی;
    writer->cwt->do_close(داده, writer);
    curlx_free(writer);
    writer = داده->req.writer.پشته;
  }
  داده->req.writer.paused = FALSE;
}

ساكن فراغ cl_reset_reader(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = داده->req.reader.پشته;
  داده->req.reader_started = FALSE;
  طالما(reader) {
    داده->req.reader.پشته = reader->بعدی;
    reader->crt->do_close(داده, reader);
    curlx_free(reader);
    reader = داده->req.reader.پشته;
  }
}

فراغ Curl_client_cleanup(بنية Curl_easy *داده)
{
  cl_reset_reader(داده);
  cl_reset_writer(داده);

  داده->req.bytecount = 0;
  داده->req.headerline = 0;
}

فراغ Curl_client_reset(بنية Curl_easy *داده)
{
  إذا(داده->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(داده, "client_reset, will rewind reader");
  }
  وإلا {
    CURL_TRC_READ(داده, "client_reset, clear readers");
    cl_reset_reader(داده);
  }
  cl_reset_writer(داده);

  داده->req.bytecount = 0;
  داده->req.headerline = 0;
}

CURLcode Curl_client_شروع(بنية Curl_easy *داده)
{
  إذا(داده->req.rewind_read) {
    بنية Curl_creader *r = داده->req.reader.پشته;
    CURLcode نتیجه = CURLE_OK;

    CURL_TRC_READ(داده, "client start, rewind readers");
    طالما(r) {
      نتیجه = r->crt->cntrl(داده, r, CURL_CRCNTRL_REWIND);
      إذا(نتیجه) {
        failf(داده, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحيح)نتیجه);
        أرجع نتیجه;
      }
      r = r->بعدی;
    }
    داده->req.rewind_read = FALSE;
    cl_reset_reader(داده);
  }
  أرجع CURLE_OK;
}

bool Curl_creader_will_rewind(بنية Curl_easy *داده)
{
  أرجع (bool)داده->req.rewind_read;
}

فراغ Curl_creader_مجموعه_rewind(بنية Curl_easy *داده, bool enable)
{
  داده->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(بنية Curl_easy *داده,
                               بنية Curl_cwriter *writer)
{
  (فراغ)داده;
  (فراغ)writer;
  أرجع CURLE_OK;
}

CURLcode Curl_cwriter_def_write(بنية Curl_easy *داده,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(بنية Curl_easy *داده,
                                بنية Curl_cwriter *writer)
{
  أرجع Curl_cwriter_flush(داده, writer->بعدی);
}

فراغ Curl_cwriter_def_close(بنية Curl_easy *داده,
                            بنية Curl_cwriter *writer)
{
  (فراغ)داده;
  (فراغ)writer;
}

ساكن size_t get_max_body_write_len(بنية Curl_easy *داده, curl_off_t حد)
{
  إذا(حد != -1) {
    /* How much more are we allowed to write? */
    أرجع curlx_sotouz_range(حد - داده->req.bytecount, 0, SIZE_MAX);
  }
  أرجع SIZE_MAX;
}

بنية cw_download_ctx {
  بنية Curl_cwriter super;
  بیت(started_response);
  بیت(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساكن CURLcode cw_download_write(بنية Curl_easy *داده,
                                  بنية Curl_cwriter *writer, صحيح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  بنية cw_download_ctx *ctx = writer->ctx;
  CURLcode نتیجه;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  إذا(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || داده->req.upload_انجامشده)) {
    Curl_pgrs_زمان(داده, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  إذا(!(type & CLIENTWRITE_BODY)) {
    إذا(is_connect && داده->مجموعه.suppress_connect_سرآیندها)
      أرجع CURLE_OK;
#اگر_تعریف_شده DEBUGBUILD
    إذا(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      أرجع CURLE_OK;
#پایان_اگر
    نتیجه = Curl_cwriter_write(داده, writer->بعدی, type, buf, nbytes);
    CURL_TRC_WRITE(داده, "download_write header(type=%x, blen=%zu) -> %d",
                   (بیعلامت صحيح)type, nbytes, (صحيح)نتیجه);
    أرجع نتیجه;
  }

  إذا(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_شروع(&داده->پیشرفت.dl.rlimit, Curl_pgrs_now(داده),
                      داده->req.اندازه);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  إذا(داده->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(داده->conn);
    CURL_TRC_WRITE(داده, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (بیعلامت صحيح)type, nbytes);
    داده->req.download_انجامشده = TRUE;
    إذا(داده->info.سرآیند_اندازه)
      /* if headers have been received, this is fine */
      أرجع CURLE_OK;
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  إذا(داده->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(داده, داده->req.maxdownload);
    إذا(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    إذا(nwrite == wmax) {
      داده->req.download_انجامشده = TRUE;
    }

    إذا((type & CLIENTWRITE_EOS) && !داده->req.no_body &&
       (داده->req.اندازه > داده->req.bytecount)) {
      failf(داده, "end of response with %" FMT_OFF_T " bytes missing",
            داده->req.اندازه - داده->req.bytecount);
      أرجع CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  إذا(داده->مجموعه.max_filesize && !داده->req.ignorebody) {
    size_t wmax = get_max_body_write_len(داده, داده->مجموعه.max_filesize);
    إذا(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  إذا(!داده->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    نتیجه = Curl_cwriter_write(داده, writer->بعدی, type, buf, nwrite);
    CURL_TRC_WRITE(داده, "download_write body(type=%x, blen=%zu) -> %d",
                   (بیعلامت صحيح)type, nbytes, (صحيح)نتیجه);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* Update stats, write and report progress */
  إذا(nwrite) {
    داده->req.bytecount += nwrite;
    Curl_pgrs_download_inc(داده, nwrite);
  }

  إذا(excess_len) {
    إذا(!داده->req.ignorebody) {
      infof(داده,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, داده->req.اندازه, داده->req.maxdownload,
            داده->req.bytecount);
      connclose(داده->conn);
    }
  }
  وإلا إذا((nwrite < nbytes) && !داده->req.ignorebody) {
    failf(داده, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          داده->مجموعه.max_filesize, داده->req.bytecount);
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
ساكن CURLcode cw_raw_write(بنية Curl_easy *داده,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t nbytes)
{
  إذا(type & CLIENTWRITE_BODY && داده->مجموعه.verbose && !داده->req.ignorebody) {
    Curl_debug(داده, CURLINFO_داده_درون, buf, nbytes);
  }
  أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, nbytes);
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

ساكن فراغ cwriter_جمع_کن(بنية Curl_easy *داده,
                        بنية Curl_cwriter *writer)
{
  بنية Curl_cwriter **anchor = &داده->req.writer.پشته;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  طالما(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->بعدی;
  writer->بعدی = *anchor;
  *anchor = writer;
}

ساكن CURLcode do_init_writer_پشته(بنية Curl_easy *داده)
{
  بنية Curl_cwriter *writer;
  CURLcode نتیجه;

  DEBUGASSERT(!داده->req.writer.پشته);
  نتیجه = Curl_cwriter_create(&داده->req.writer.پشته,
                               داده, &Curl_cwt_بیرون, CURL_CW_CLIENT);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_cwriter_create(&writer, داده, &cw_download, CURL_CW_PROTOCOL);
  إذا(نتیجه)
    أرجع نتیجه;
  cwriter_جمع_کن(داده, writer);

  نتیجه = Curl_cwriter_create(&writer, داده, &cw_raw, CURL_CW_RAW);
  إذا(نتیجه)
    أرجع نتیجه;
  cwriter_جمع_کن(داده, writer);

  أرجع نتیجه;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(بنية Curl_easy *داده, صحيح type, ثابت حرف *buf,
                           size_t len)
{
  CURLcode نتیجه;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_سرآیند | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  إذا(!داده->req.writer.پشته) {
    نتیجه = do_init_writer_پشته(داده);
    إذا(نتیجه)
      أرجع نتیجه;
    DEBUGASSERT(داده->req.writer.پشته);
  }

  نتیجه = Curl_cwriter_write(داده, داده->req.writer.پشته, type, buf, len);
  CURL_TRC_WRITE(داده, "client_write(type=%x, len=%zu) -> %d",
                 (بیعلامت صحيح)type, len, (صحيح)نتیجه);
  أرجع نتیجه;
}

CURLcode Curl_client_flush(بنية Curl_easy *داده)
{
  CURLcode نتیجه;

  إذا(!داده->req.writer.پشته) {
    نتیجه = do_init_writer_پشته(داده);
    إذا(نتیجه)
      أرجع نتیجه;
    DEBUGASSERT(داده->req.writer.پشته);
  }

  نتیجه = Curl_cwriter_flush(داده, داده->req.writer.پشته);
  CURL_TRC_WRITE(داده, "client_flush() -> %d", (صحيح)نتیجه);
  أرجع نتیجه;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(بنية Curl_cwriter **pwriter,
                             بنية Curl_easy *داده,
                             ثابت بنية Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *writer = عدم;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  فراغ *p;

  DEBUGASSERT(cwt->cwriter_اندازه >= حجم(بنية Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_اندازه);
  إذا(!p)
    اذهب بیرون;

  writer = (بنية Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  نتیجه = cwt->do_init(داده, writer);

بیرون:
  *pwriter = نتیجه ? عدم : writer;
  إذا(نتیجه)
    curlx_free(writer);
  أرجع نتیجه;
}

فراغ Curl_cwriter_free(بنية Curl_easy *داده,
                       بنية Curl_cwriter *writer)
{
  إذا(writer) {
    writer->cwt->do_close(داده, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_شمار(بنية Curl_easy *داده, Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *w;
  size_t n = 0;

  لكل(w = داده->req.writer.پشته; w; w = w->بعدی) {
    إذا(w->phase == phase)
      ++n;
  }
  أرجع n;
}

ساكن CURLcode cwriter_ensure_pause_writer(بنية Curl_easy *داده)
{
  بنية Curl_cwriter *writer =
    Curl_cwriter_get_by_type(داده, &Curl_cwt_pause);
  CURLcode نتیجه = CURLE_OK;

  إذا(!writer) {
    نتیجه = Curl_cwriter_create(&writer, داده, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    إذا(!نتیجه)
      cwriter_جمع_کن(داده, writer);
  }
  أرجع نتیجه;
}

CURLcode Curl_cwriter_جمع_کن(بنية Curl_easy *داده,
                          بنية Curl_cwriter *writer)
{
  CURLcode نتیجه;
  بنية Curl_cwriter **anchor = &داده->req.writer.پشته;

  إذا(!*anchor) {
    نتیجه = do_init_writer_پشته(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    نتیجه = cwriter_ensure_pause_writer(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  cwriter_جمع_کن(داده, writer);
  أرجع CURLE_OK;
}

بنية Curl_cwriter *Curl_cwriter_get_by_name(بنية Curl_easy *داده,
                                              ثابت حرف *name)
{
  بنية Curl_cwriter *writer;
  لكل(writer = داده->req.writer.پشته; writer; writer = writer->بعدی) {
    إذا(!strcmp(name, writer->cwt->name))
      أرجع writer;
  }
  أرجع عدم;
}

بنية Curl_cwriter *Curl_cwriter_get_by_type(بنية Curl_easy *داده,
                                              ثابت بنية Curl_cwtype *cwt)
{
  بنية Curl_cwriter *writer;
  لكل(writer = داده->req.writer.پشته; writer; writer = writer->بعدی) {
    إذا(writer->cwt == cwt)
      أرجع writer;
  }
  أرجع عدم;
}

CURLcode Curl_cwriter_unpause(بنية Curl_easy *داده)
{
  داده->req.writer.paused = FALSE;
  أرجع Curl_cwriter_flush(داده, داده->req.writer.پشته);
}

CURLcode Curl_creader_read(بنية Curl_easy *داده,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  إذا(!reader)
    أرجع CURLE_READ_ERROR;
  أرجع reader->crt->do_read(داده, reader, buf, blen, nread, eos);
}

فراغ Curl_creader_clear_eos(بنية Curl_easy *داده,
                            بنية Curl_creader *reader)
{
  طالما(reader) {
    (فراغ)reader->crt->cntrl(داده, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->بعدی;
  }
}

CURLcode Curl_creader_def_init(بنية Curl_easy *داده,
                               بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
  أرجع CURLE_OK;
}

فراغ Curl_creader_def_close(بنية Curl_easy *داده,
                            بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
}

CURLcode Curl_creader_def_read(بنية Curl_easy *داده,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  إذا(reader->بعدی)
    أرجع reader->بعدی->crt->do_read(داده, reader->بعدی, buf, blen,
                                      nread, eos);
  وإلا {
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(بنية Curl_easy *داده,
                                   بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
  أرجع FALSE;
}

curl_off_t Curl_creader_def_مجموع_درازی(بنية Curl_easy *داده,
                                         بنية Curl_creader *reader)
{
  أرجع reader->بعدی ?
         reader->بعدی->crt->مجموع_درازی(داده, reader->بعدی) : -1;
}

CURLcode Curl_creader_def_resume_from(بنية Curl_easy *داده,
                                      بنية Curl_creader *reader,
                                      curl_off_t جابه_جایی)
{
  (فراغ)داده;
  (فراغ)reader;
  (فراغ)جابه_جایی;
  أرجع CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(بنية Curl_easy *داده,
                                بنية Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (فراغ)داده;
  (فراغ)reader;
  (فراغ)opcode;
  أرجع CURLE_OK;
}

bool Curl_creader_def_is_paused(بنية Curl_easy *داده,
                                بنية Curl_creader *reader)
{
  (فراغ)داده;
  (فراغ)reader;
  أرجع FALSE;
}

فراغ Curl_creader_def_انجامشده(بنية Curl_easy *داده,
                           بنية Curl_creader *reader, صحيح premature)
{
  (فراغ)داده;
  (فراغ)reader;
  (فراغ)premature;
}

بنية cr_درون_ctx {
  بنية Curl_creader super;
  curl_read_callback read_cb;
  فراغ *cb_user_داده;
  curl_off_t مجموع_len;
  curl_off_t read_len;
  CURLcode error_نتیجه;
  بیت(دیده_eos);
  بیت(errored);
  بیت(has_استفاده_شده_cb);
  بیت(is_paused);
};

ساكن CURLcode cr_درون_init(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  ctx->read_cb = داده->state.fread_func;
  ctx->cb_user_داده = داده->state.درون;
  ctx->مجموع_len = -1;
  ctx->read_len = 0;
  أرجع CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_درون_read(بنية Curl_easy *داده,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  إذا(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_نتیجه;
  }
  إذا(ctx->دیده_eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  إذا(ctx->مجموع_len >= 0) {
    blen = curlx_sotouz_range(ctx->مجموع_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  إذا(ctx->read_cb && blen) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, داده, easy_cr_درون_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_داده);
    CURL_CBAPI_انتها(&guard);
    ctx->has_استفاده_شده_cb = TRUE;
  }

  بدل(nread) {
  حالة 0:
    إذا((ctx->مجموع_len >= 0) && (ctx->read_len < ctx->مجموع_len)) {
      failf(داده, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->مجموع_len);
      نتیجه = CURLE_READ_ERROR;
      اكسر;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->دیده_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(داده, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتیجه = CURLE_ABORTED_BY_CALLBACK;
    نتیجه = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    إذا(داده->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(داده, "Read callback asked for PAUSE when not supported");
      نتیجه = CURLE_READ_ERROR;
      اكسر;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(داده, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    نتیجه = Curl_xfer_pause_send(داده, TRUE);
    اكسر; /* nothing was read */

  افتراضي:
    إذا(nread > blen) {
      /* the read function returned a too large value */
      failf(داده, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتیجه = CURLE_READ_ERROR;
      نتیجه = CURLE_READ_ERROR;
      اكسر;
    }
    ctx->read_len += nread;
    إذا(ctx->مجموع_len >= 0)
      ctx->دیده_eos = (ctx->read_len >= ctx->مجموع_len);
    *pnread = nread;
    *peos = (bool)ctx->دیده_eos;
    اكسر;
  }
  CURL_TRC_READ(داده, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->مجموع_len, ctx->read_len, (صحيح)نتیجه,
                *pnread, *peos);
  أرجع نتیجه;
}

ساكن bool cr_درون_needs_rewind(بنية Curl_easy *داده,
                               بنية Curl_creader *reader)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  (فراغ)داده;
  أرجع (bool)ctx->has_استفاده_شده_cb;
}

ساكن curl_off_t cr_درون_مجموع_درازی(بنية Curl_easy *داده,
                                     بنية Curl_creader *reader)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  (فراغ)داده;
  أرجع ctx->مجموع_len;
}

ساكن CURLcode cr_درون_resume_from(بنية Curl_easy *داده,
                                  بنية Curl_creader *reader,
                                  curl_off_t جابه_جایی)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  صحيح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(داده->conn);
  /* already started reading? */
  إذا(ctx->read_len)
    أرجع CURLE_READ_ERROR;

  إذا(داده->مجموعه.seek_func) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, داده, easy_seek_func);
    seekerr = داده->مجموعه.seek_func(داده->مجموعه.seek_client, جابه_جایی, SEEK_SET);
    CURL_CBAPI_انتها(&guard);
  }

  إذا(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(داده, "Could not seek stream");
      أرجع CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    افعل {
      بنية Curl_mapi_guard guard;
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (جابه_جایی - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(جابه_جایی - passed);
      size_t actuallyread;

      CURL_CBAPI_شروع(&guard, داده, easy_cr_درون_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_داده);
      CURL_CBAPI_انتها(&guard);

      passed += actuallyread;
      إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(داده, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < جابه_جایی);
  }

  /* now, decrease the size of the read */
  إذا(ctx->مجموع_len > 0) {
    ctx->مجموع_len -= جابه_جایی;

    إذا(ctx->مجموع_len <= 0) {
      failf(داده, "File already completely uploaded");
      أرجع CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  أرجع CURLE_OK;
}

ساكن CURLcode cr_درون_rewind(بنية Curl_easy *داده,
                             بنية Curl_creader *reader)
{
  بنية cr_درون_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  إذا(!ctx->has_استفاده_شده_cb)
    أرجع CURLE_OK;

  إذا(داده->مجموعه.seek_func) {
    بنية Curl_mapi_guard guard;
    صحيح err;

    CURL_CBAPI_شروع(&guard, داده, easy_seek_func);
    err = داده->مجموعه.seek_func(داده->مجموعه.seek_client, 0, SEEK_SET);
    CURL_CBAPI_انتها(&guard);
    CURL_TRC_READ(داده, "cr_in, rewind via set.seek_func -> %d", err);
    إذا(err) {
      failf(داده, "seek callback returned error %d", err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا إذا(داده->مجموعه.ioctl_func) {
    بنية Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_شروع(&guard, داده, easy_ioctl_func);
    err = داده->مجموعه.ioctl_func(داده, CURLIOCMD_RESTARTREAD,
                               داده->مجموعه.ioctl_client);
    CURL_CBAPI_انتها(&guard);
    CURL_TRC_READ(داده, "cr_in, rewind via set.ioctl_func -> %d", (صحيح)err);
    إذا(err) {
      failf(داده, "ioctl callback returned error %d", (صحيح)err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
    إذا(داده->state.fread_func == (curl_read_callback)fread) {
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
      صحيح err = fseek(داده->state.درون, 0, SEEK_SET);
      CURL_TRC_READ(داده, "cr_in, rewind via fseek -> %d(%d)",
                    (صحيح)err, (صحيح)errno);
      إذا(err != -1)
        /* successful rewind */
        أرجع CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(داده, "necessary data rewind was not possible");
    أرجع CURLE_SEND_FAIL_REWIND;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cr_درون_cntrl(بنية Curl_easy *داده,
                            بنية Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  بنية cr_درون_ctx *ctx = reader->ctx;

  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    أرجع cr_درون_rewind(داده, reader);
  حالة CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->دیده_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_درون_is_paused(بنية Curl_easy *داده,
                            بنية Curl_creader *reader)
{
  بنية cr_درون_ctx *ctx = reader->ctx;
  (فراغ)داده;
  أرجع (bool)ctx->is_paused;
}

ساكن ثابت بنية Curl_crtype cr_درون = {
  "cr-in",
  cr_درون_init,
  cr_درون_read,
  Curl_creader_def_close,
  cr_درون_needs_rewind,
  cr_درون_مجموع_درازی,
  cr_درون_resume_from,
  cr_درون_cntrl,
  cr_درون_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية cr_درون_ctx)
};

CURLcode Curl_creader_create(بنية Curl_creader **preader,
                             بنية Curl_easy *داده,
                             ثابت بنية Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  فراغ *p;

  DEBUGASSERT(crt->creader_اندازه >= حجم(بنية Curl_creader));
  p = curlx_calloc(1, crt->creader_اندازه);
  إذا(!p)
    اذهب بیرون;

  reader = (بنية Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  نتیجه = crt->do_init(داده, reader);

بیرون:
  *preader = نتیجه ? عدم : reader;
  إذا(نتیجه)
    curlx_free(reader);
  أرجع نتیجه;
}

فراغ Curl_creader_free(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  إذا(reader) {
    reader->crt->do_close(داده, reader);
    curlx_free(reader);
  }
}

بنية cr_lc_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  بیت(read_eos);  /* we read an EOS from the next reader */
  بیت(eos);       /* we have returned an EOS */
  بیت(prev_cr);   /* the last byte was a CR */
};

ساكن CURLcode cr_lc_init(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)داده;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد);
  أرجع CURLE_OK;
}

ساكن فراغ cr_lc_close(بنية Curl_easy *داده, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)داده;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ساكن CURLcode cr_lc_read(بنية Curl_easy *داده,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  CURLcode نتیجه;
  size_t nread, i, شروع, n;
  bool eos;

  إذا(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  إذا(Curl_bufq_is_خالی(&ctx->buf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      أرجع CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    نتیجه = Curl_creader_read(داده, reader->بعدی, buf, blen, &nread, &eos);
    إذا(نتیجه)
      أرجع نتیجه;
    ctx->read_eos = eos;

    إذا(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      إذا(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب بیرون;
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
      نتیجه = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
      إذا(!نتیجه)
        نتیجه = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      إذا(نتیجه)
        أرجع نتیجه;
      شروع = i + 1;
    }

    إذا(شروع < i) { /* leftover */
      نتیجه = Curl_bufq_cwrite(&ctx->buf, buf + شروع, i - شروع, &n);
      إذا(نتیجه)
        أرجع نتیجه;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_خالی(&ctx->buf));
  *peos = FALSE;
  نتیجه = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  إذا(!نتیجه && ctx->read_eos && Curl_bufq_is_خالی(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

بیرون:
  CURL_TRC_READ(داده, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجه, *pnread, *peos);
  أرجع نتیجه;
}

ساكن curl_off_t cr_lc_مجموع_درازی(بنية Curl_easy *داده,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)داده;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_مجموع_درازی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية cr_lc_ctx)
};

ساكن CURLcode cr_lc_جمع_کن(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجه;

  نتیجه = Curl_creader_create(&reader, داده, &cr_lc, CURL_CR_CONTENT_ENCODE);
  إذا(!نتیجه)
    نتیجه = Curl_creader_جمع_کن(داده, reader);

  إذا(نتیجه && reader)
    Curl_creader_free(داده, reader);
  أرجع نتیجه;
}

ساكن CURLcode do_init_reader_پشته(بنية Curl_easy *داده,
                                     بنية Curl_creader *r)
{
  CURLcode نتیجه = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!داده->req.reader.پشته);

  داده->req.reader.پشته = r;
  clen = r->crt->مجموع_درازی(داده, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  إذا(clen &&
#اگر_تعریف_شده CURL_PREFER_LF_LINEENDS
    (داده->مجموعه.crlf || داده->state.prefer_ascii)
#وإلا
    داده->مجموعه.crlf
#پایان_اگر
    ) {
    نتیجه = cr_lc_جمع_کن(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع نتیجه;
}

CURLcode Curl_creader_مجموعه_fread(بنية Curl_easy *داده, curl_off_t len)
{
  CURLcode نتیجه;
  بنية Curl_creader *r;
  بنية cr_درون_ctx *ctx;

  نتیجه = Curl_creader_create(&r, داده, &cr_درون, CURL_CR_CLIENT);
  إذا(نتیجه || !r)
    اذهب بیرون;
  ctx = r->ctx;
  ctx->مجموع_len = len;

  cl_reset_reader(داده);
  نتیجه = do_init_reader_پشته(داده, r);
بیرون:
  CURL_TRC_READ(داده, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحيح)نتیجه);
  أرجع نتیجه;
}

CURLcode Curl_creader_جمع_کن(بنية Curl_easy *داده,
                          بنية Curl_creader *reader)
{
  CURLcode نتیجه;
  بنية Curl_creader **anchor = &داده->req.reader.پشته;

  إذا(!*anchor) {
    نتیجه = Curl_creader_مجموعه_fread(داده, داده->state.infilesize);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  طالما(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->بعدی;
  reader->بعدی = *anchor;
  *anchor = reader;
  أرجع CURLE_OK;
}

CURLcode Curl_creader_مجموعه(بنية Curl_easy *داده, بنية Curl_creader *r)
{
  CURLcode نتیجه;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(داده);
  نتیجه = do_init_reader_پشته(داده, r);
  إذا(نتیجه)
    Curl_creader_free(داده, r);
  أرجع نتیجه;
}

CURLcode Curl_client_read(بنية Curl_easy *داده, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode نتیجه;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  إذا(!داده->req.reader.پشته) {
    نتیجه = Curl_creader_مجموعه_fread(داده, داده->state.infilesize);
    إذا(نتیجه)
      أرجع نتیجه;
    DEBUGASSERT(داده->req.reader.پشته);
  }
  إذا(!داده->req.reader_started) {
    Curl_rlimit_شروع(&داده->پیشرفت.ul.rlimit, Curl_pgrs_now(داده), -1);
    داده->req.reader_started = TRUE;
  }

  إذا(Curl_rlimit_فعال(&داده->پیشرفت.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&داده->پیشرفت.ul.rlimit, عدم);
    إذا(ul_avail <= 0) {
      نتیجه = CURLE_OK;
      *eos = FALSE;
      اذهب بیرون;
    }
    إذا(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  نتیجه = Curl_creader_read(داده, داده->req.reader.پشته, buf, blen,
                             nread, eos);

بیرون:
  CURL_TRC_READ(داده, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتیجه, *nread, *eos);
  أرجع نتیجه;
}

bool Curl_creader_needs_rewind(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = داده->req.reader.پشته;
  طالما(reader) {
    إذا(reader->crt->needs_rewind(داده, reader)) {
      CURL_TRC_READ(داده, "client reader needs rewind before next request");
      أرجع TRUE;
    }
    reader = reader->بعدی;
  }
  أرجع FALSE;
}

ساكن CURLcode cr_null_read(بنية Curl_easy *داده,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (فراغ)داده;
  (فراغ)reader;
  (فراغ)buf;
  (فراغ)blen;
  *pnread = 0;
  *peos = TRUE;
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_null_مجموع_درازی(بنية Curl_easy *داده,
                                       بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)داده;
  (فراغ)reader;
  أرجع 0;
}

ساكن ثابت بنية Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_مجموع_درازی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية Curl_creader)
};

CURLcode Curl_creader_مجموعه_null(بنية Curl_easy *داده)
{
  بنية Curl_creader *r;
  CURLcode نتیجه;

  نتیجه = Curl_creader_create(&r, داده, &cr_null, CURL_CR_CLIENT);
  إذا(نتیجه)
    أرجع نتیجه;

  cl_reset_reader(داده);
  أرجع do_init_reader_پشته(داده, r);
}

بنية cr_buf_ctx {
  بنية Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t شاخص;
};

ساكن CURLcode cr_buf_read(بنية Curl_easy *داده,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->شاخص;

  إذا(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  وإلا {
    إذا(nread > blen)
      nread = blen;
    کپی_حافظه(buf, ctx->buf + ctx->شاخص, nread);
    *pnread = nread;
    ctx->شاخص += nread;
    *peos = (ctx->شاخص == ctx->blen);
  }
  CURL_TRC_READ(داده, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  أرجع CURLE_OK;
}

ساكن bool cr_buf_needs_rewind(بنية Curl_easy *داده,
                                بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)داده;
  أرجع ctx->شاخص > 0;
}

ساكن CURLcode cr_buf_cntrl(بنية Curl_easy *داده,
                             بنية Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)داده;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    ctx->شاخص = 0;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_buf_مجموع_درازی(بنية Curl_easy *داده,
                                      بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)داده;
  أرجع (curl_off_t)ctx->blen;
}

ساكن CURLcode cr_buf_resume_from(بنية Curl_easy *داده,
                                   بنية Curl_creader *reader,
                                   curl_off_t جابه_جایی)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (فراغ)داده;
  DEBUGASSERT(داده->conn);
  /* already started reading? */
  إذا(ctx->شاخص)
    أرجع CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(جابه_جایی, 0, SIZE_MAX);
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
  cr_buf_مجموع_درازی,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_انجامشده,
  حجم(بنية cr_buf_ctx)
};

CURLcode Curl_creader_مجموعه_buf(بنية Curl_easy *داده,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتیجه;
  بنية Curl_creader *r;
  بنية cr_buf_ctx *ctx;

  نتیجه = Curl_creader_create(&r, داده, &cr_buf, CURL_CR_CLIENT);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->شاخص = 0;

  cl_reset_reader(داده);
  نتیجه = do_init_reader_پشته(داده, r);
بیرون:
  CURL_TRC_READ(داده, "add buf reader, len=%zu -> %d", blen, (صحيح)نتیجه);
  أرجع نتیجه;
}

curl_off_t Curl_creader_مجموع_درازی(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = داده->req.reader.پشته;
  أرجع r ? r->crt->مجموع_درازی(داده, r) : -1;
}

curl_off_t Curl_creader_client_درازی(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = داده->req.reader.پشته;
  طالما(r && r->phase != CURL_CR_CLIENT)
    r = r->بعدی;
  أرجع r ? r->crt->مجموع_درازی(داده, r) : -1;
}

CURLcode Curl_creader_unpause(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = داده->req.reader.پشته;
  CURLcode نتیجه = CURLE_OK;

  طالما(reader) {
    نتیجه = reader->crt->cntrl(داده, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(داده, "unpausing %s -> %d", reader->crt->name, (صحيح)نتیجه);
    إذا(نتیجه)
      اكسر;
    reader = reader->بعدی;
  }
  أرجع نتیجه;
}

bool Curl_creader_is_paused(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = داده->req.reader.پشته;

  طالما(reader) {
    إذا(reader->crt->is_paused(داده, reader))
      أرجع TRUE;
    reader = reader->بعدی;
  }
  أرجع FALSE;
}

فراغ Curl_creader_انجامشده(بنية Curl_easy *داده, صحيح premature)
{
  بنية Curl_creader *reader = داده->req.reader.پشته;
  طالما(reader) {
    reader->crt->انجامشده(داده, reader, premature);
    reader = reader->بعدی;
  }
}

بنية Curl_creader *Curl_creader_get_by_type(بنية Curl_easy *داده,
                                              ثابت بنية Curl_crtype *crt)
{
  بنية Curl_creader *r;
  لكل(r = داده->req.reader.پشته; r; r = r->بعدی) {
    إذا(r->crt == crt)
      أرجع r;
  }
  أرجع عدم;
}
