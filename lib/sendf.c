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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای

#که_تعریف_شوی HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#د_که_پای

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

ساكن فراغ cl_reset_writer(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer = معلومات->req.writer.ډېری;
  طالما(writer) {
    معلومات->req.writer.ډېری = writer->بل;
    writer->cwt->do_close(معلومات, writer);
    curlx_free(writer);
    writer = معلومات->req.writer.ډېری;
  }
  معلومات->req.writer.paused = FALSE;
}

ساكن فراغ cl_reset_reader(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ډېری;
  معلومات->req.reader_started = FALSE;
  طالما(reader) {
    معلومات->req.reader.ډېری = reader->بل;
    reader->crt->do_close(معلومات, reader);
    curlx_free(reader);
    reader = معلومات->req.reader.ډېری;
  }
}

فراغ Curl_client_cleanup(بنية Curl_easy *معلومات)
{
  cl_reset_reader(معلومات);
  cl_reset_writer(معلومات);

  معلومات->req.bytecount = 0;
  معلومات->req.headerline = 0;
}

فراغ Curl_client_reset(بنية Curl_easy *معلومات)
{
  إذا(معلومات->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(معلومات, "client_reset, will rewind reader");
  }
  وإلا {
    CURL_TRC_READ(معلومات, "client_reset, clear readers");
    cl_reset_reader(معلومات);
  }
  cl_reset_writer(معلومات);

  معلومات->req.bytecount = 0;
  معلومات->req.headerline = 0;
}

CURLcode Curl_client_پیل(بنية Curl_easy *معلومات)
{
  إذا(معلومات->req.rewind_read) {
    بنية Curl_creader *r = معلومات->req.reader.ډېری;
    CURLcode پایله = CURLE_OK;

    CURL_TRC_READ(معلومات, "client start, rewind readers");
    طالما(r) {
      پایله = r->crt->cntrl(معلومات, r, CURL_CRCNTRL_REWIND);
      إذا(پایله) {
        failf(معلومات, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحيح)پایله);
        أرجع پایله;
      }
      r = r->بل;
    }
    معلومات->req.rewind_read = FALSE;
    cl_reset_reader(معلومات);
  }
  أرجع CURLE_OK;
}

bool Curl_creader_will_rewind(بنية Curl_easy *معلومات)
{
  أرجع (bool)معلومات->req.rewind_read;
}

فراغ Curl_creader_سېټ_rewind(بنية Curl_easy *معلومات, bool enable)
{
  معلومات->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(بنية Curl_easy *معلومات,
                               بنية Curl_cwriter *writer)
{
  (فراغ)معلومات;
  (فراغ)writer;
  أرجع CURLE_OK;
}

CURLcode Curl_cwriter_def_write(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  أرجع Curl_cwriter_write(معلومات, writer->بل, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *writer)
{
  أرجع Curl_cwriter_flush(معلومات, writer->بل);
}

فراغ Curl_cwriter_def_close(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  (فراغ)معلومات;
  (فراغ)writer;
}

ساكن size_t get_max_body_write_len(بنية Curl_easy *معلومات, curl_off_t حد)
{
  إذا(حد != -1) {
    /* How much more are we allowed to write? */
    أرجع curlx_sotouz_range(حد - معلومات->req.bytecount, 0, SIZE_MAX);
  }
  أرجع SIZE_MAX;
}

بنية cw_download_ctx {
  بنية Curl_cwriter super;
  بټ(started_response);
  بټ(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساكن CURLcode cw_download_write(بنية Curl_easy *معلومات,
                                  بنية Curl_cwriter *writer, صحيح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  بنية cw_download_ctx *ctx = writer->ctx;
  CURLcode پایله;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  إذا(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || معلومات->req.upload_بشپړ)) {
    Curl_pgrs_وخت(معلومات, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  إذا(!(type & CLIENTWRITE_BODY)) {
    إذا(is_connect && معلومات->سېټ.suppress_connect_سرلیکونه)
      أرجع CURLE_OK;
#که_تعریف_شوی DEBUGBUILD
    إذا(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      أرجع CURLE_OK;
#د_که_پای
    پایله = Curl_cwriter_write(معلومات, writer->بل, type, buf, nbytes);
    CURL_TRC_WRITE(معلومات, "download_write header(type=%x, blen=%zu) -> %d",
                   (بینښه صحيح)type, nbytes, (صحيح)پایله);
    أرجع پایله;
  }

  إذا(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_پیل(&معلومات->پرمختګ.dl.rlimit, Curl_pgrs_now(معلومات),
                      معلومات->req.اندازه);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  إذا(معلومات->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(معلومات->conn);
    CURL_TRC_WRITE(معلومات, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (بینښه صحيح)type, nbytes);
    معلومات->req.download_بشپړ = TRUE;
    إذا(معلومات->info.سرلیک_اندازه)
      /* if headers have been received, this is fine */
      أرجع CURLE_OK;
    أرجع CURLE_WEIRD_SERVER_ځواب;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  إذا(معلومات->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(معلومات, معلومات->req.maxdownload);
    إذا(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    إذا(nwrite == wmax) {
      معلومات->req.download_بشپړ = TRUE;
    }

    إذا((type & CLIENTWRITE_EOS) && !معلومات->req.no_body &&
       (معلومات->req.اندازه > معلومات->req.bytecount)) {
      failf(معلومات, "end of response with %" FMT_OFF_T " bytes missing",
            معلومات->req.اندازه - معلومات->req.bytecount);
      أرجع CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  إذا(معلومات->سېټ.max_filesize && !معلومات->req.ignorebody) {
    size_t wmax = get_max_body_write_len(معلومات, معلومات->سېټ.max_filesize);
    إذا(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  إذا(!معلومات->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    پایله = Curl_cwriter_write(معلومات, writer->بل, type, buf, nwrite);
    CURL_TRC_WRITE(معلومات, "download_write body(type=%x, blen=%zu) -> %d",
                   (بینښه صحيح)type, nbytes, (صحيح)پایله);
    إذا(پایله)
      أرجع پایله;
  }

  /* Update stats, write and report progress */
  إذا(nwrite) {
    معلومات->req.bytecount += nwrite;
    Curl_pgrs_download_inc(معلومات, nwrite);
  }

  إذا(excess_len) {
    إذا(!معلومات->req.ignorebody) {
      infof(معلومات,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, معلومات->req.اندازه, معلومات->req.maxdownload,
            معلومات->req.bytecount);
      connclose(معلومات->conn);
    }
  }
  وإلا إذا((nwrite < nbytes) && !معلومات->req.ignorebody) {
    failf(معلومات, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          معلومات->سېټ.max_filesize, معلومات->req.bytecount);
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
ساكن CURLcode cw_raw_write(بنية Curl_easy *معلومات,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t nbytes)
{
  إذا(type & CLIENTWRITE_BODY && معلومات->سېټ.verbose && !معلومات->req.ignorebody) {
    Curl_debug(معلومات, CURLINFO_معلومات_دننه, buf, nbytes);
  }
  أرجع Curl_cwriter_write(معلومات, writer->بل, type, buf, nbytes);
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

ساكن فراغ cwriter_جمع_کړه(بنية Curl_easy *معلومات,
                        بنية Curl_cwriter *writer)
{
  بنية Curl_cwriter **anchor = &معلومات->req.writer.ډېری;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  طالما(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->بل;
  writer->بل = *anchor;
  *anchor = writer;
}

ساكن CURLcode do_init_writer_ډېری(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer;
  CURLcode پایله;

  DEBUGASSERT(!معلومات->req.writer.ډېری);
  پایله = Curl_cwriter_create(&معلومات->req.writer.ډېری,
                               معلومات, &Curl_cwt_بهر, CURL_CW_CLIENT);
  إذا(پایله)
    أرجع پایله;

  پایله = Curl_cwriter_create(&writer, معلومات, &cw_download, CURL_CW_PROTOCOL);
  إذا(پایله)
    أرجع پایله;
  cwriter_جمع_کړه(معلومات, writer);

  پایله = Curl_cwriter_create(&writer, معلومات, &cw_raw, CURL_CW_RAW);
  إذا(پایله)
    أرجع پایله;
  cwriter_جمع_کړه(معلومات, writer);

  أرجع پایله;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(بنية Curl_easy *معلومات, صحيح type, ثابت حرف *buf,
                           size_t len)
{
  CURLcode پایله;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_سرلیک | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  إذا(!معلومات->req.writer.ډېری) {
    پایله = do_init_writer_ډېری(معلومات);
    إذا(پایله)
      أرجع پایله;
    DEBUGASSERT(معلومات->req.writer.ډېری);
  }

  پایله = Curl_cwriter_write(معلومات, معلومات->req.writer.ډېری, type, buf, len);
  CURL_TRC_WRITE(معلومات, "client_write(type=%x, len=%zu) -> %d",
                 (بینښه صحيح)type, len, (صحيح)پایله);
  أرجع پایله;
}

CURLcode Curl_client_flush(بنية Curl_easy *معلومات)
{
  CURLcode پایله;

  إذا(!معلومات->req.writer.ډېری) {
    پایله = do_init_writer_ډېری(معلومات);
    إذا(پایله)
      أرجع پایله;
    DEBUGASSERT(معلومات->req.writer.ډېری);
  }

  پایله = Curl_cwriter_flush(معلومات, معلومات->req.writer.ډېری);
  CURL_TRC_WRITE(معلومات, "client_flush() -> %d", (صحيح)پایله);
  أرجع پایله;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(بنية Curl_cwriter **pwriter,
                             بنية Curl_easy *معلومات,
                             ثابت بنية Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *writer = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  فراغ *p;

  DEBUGASSERT(cwt->cwriter_اندازه >= حجم(بنية Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_اندازه);
  إذا(!p)
    اذهب بهر;

  writer = (بنية Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  پایله = cwt->do_init(معلومات, writer);

بهر:
  *pwriter = پایله ? عدم : writer;
  إذا(پایله)
    curlx_free(writer);
  أرجع پایله;
}

فراغ Curl_cwriter_free(بنية Curl_easy *معلومات,
                       بنية Curl_cwriter *writer)
{
  إذا(writer) {
    writer->cwt->do_close(معلومات, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_شمېر(بنية Curl_easy *معلومات, Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *w;
  size_t n = 0;

  لكل(w = معلومات->req.writer.ډېری; w; w = w->بل) {
    إذا(w->phase == phase)
      ++n;
  }
  أرجع n;
}

ساكن CURLcode cwriter_ensure_pause_writer(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer =
    Curl_cwriter_get_by_type(معلومات, &Curl_cwt_pause);
  CURLcode پایله = CURLE_OK;

  إذا(!writer) {
    پایله = Curl_cwriter_create(&writer, معلومات, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    إذا(!پایله)
      cwriter_جمع_کړه(معلومات, writer);
  }
  أرجع پایله;
}

CURLcode Curl_cwriter_جمع_کړه(بنية Curl_easy *معلومات,
                          بنية Curl_cwriter *writer)
{
  CURLcode پایله;
  بنية Curl_cwriter **anchor = &معلومات->req.writer.ډېری;

  إذا(!*anchor) {
    پایله = do_init_writer_ډېری(معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    پایله = cwriter_ensure_pause_writer(معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  cwriter_جمع_کړه(معلومات, writer);
  أرجع CURLE_OK;
}

بنية Curl_cwriter *Curl_cwriter_get_by_name(بنية Curl_easy *معلومات,
                                              ثابت حرف *name)
{
  بنية Curl_cwriter *writer;
  لكل(writer = معلومات->req.writer.ډېری; writer; writer = writer->بل) {
    إذا(!strcmp(name, writer->cwt->name))
      أرجع writer;
  }
  أرجع عدم;
}

بنية Curl_cwriter *Curl_cwriter_get_by_type(بنية Curl_easy *معلومات,
                                              ثابت بنية Curl_cwtype *cwt)
{
  بنية Curl_cwriter *writer;
  لكل(writer = معلومات->req.writer.ډېری; writer; writer = writer->بل) {
    إذا(writer->cwt == cwt)
      أرجع writer;
  }
  أرجع عدم;
}

CURLcode Curl_cwriter_unpause(بنية Curl_easy *معلومات)
{
  معلومات->req.writer.paused = FALSE;
  أرجع Curl_cwriter_flush(معلومات, معلومات->req.writer.ډېری);
}

CURLcode Curl_creader_read(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  إذا(!reader)
    أرجع CURLE_READ_ERROR;
  أرجع reader->crt->do_read(معلومات, reader, buf, blen, nread, eos);
}

فراغ Curl_creader_clear_eos(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader)
{
  طالما(reader) {
    (فراغ)reader->crt->cntrl(معلومات, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->بل;
  }
}

CURLcode Curl_creader_def_init(بنية Curl_easy *معلومات,
                               بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع CURLE_OK;
}

فراغ Curl_creader_def_close(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
}

CURLcode Curl_creader_def_read(بنية Curl_easy *معلومات,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  إذا(reader->بل)
    أرجع reader->بل->crt->do_read(معلومات, reader->بل, buf, blen,
                                      nread, eos);
  وإلا {
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(بنية Curl_easy *معلومات,
                                   بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع FALSE;
}

curl_off_t Curl_creader_def_ټول_اوږدتیا(بنية Curl_easy *معلومات,
                                         بنية Curl_creader *reader)
{
  أرجع reader->بل ?
         reader->بل->crt->ټول_اوږدتیا(معلومات, reader->بل) : -1;
}

CURLcode Curl_creader_def_resume_from(بنية Curl_easy *معلومات,
                                      بنية Curl_creader *reader,
                                      curl_off_t دننه_واټن)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)دننه_واټن;
  أرجع CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(بنية Curl_easy *معلومات,
                                بنية Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)opcode;
  أرجع CURLE_OK;
}

bool Curl_creader_def_is_paused(بنية Curl_easy *معلومات,
                                بنية Curl_creader *reader)
{
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع FALSE;
}

فراغ Curl_creader_def_بشپړ(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader, صحيح premature)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)premature;
}

بنية cr_دننه_ctx {
  بنية Curl_creader super;
  curl_read_callback read_cb;
  فراغ *cb_user_معلومات;
  curl_off_t ټول_len;
  curl_off_t read_len;
  CURLcode error_پایله;
  بټ(کتل_شوی_eos);
  بټ(errored);
  بټ(has_کارول_شوی_cb);
  بټ(is_paused);
};

ساكن CURLcode cr_دننه_init(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  ctx->read_cb = معلومات->state.fread_func;
  ctx->cb_user_معلومات = معلومات->state.دننه;
  ctx->ټول_len = -1;
  ctx->read_len = 0;
  أرجع CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_دننه_read(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  CURLcode پایله = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  إذا(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_پایله;
  }
  إذا(ctx->کتل_شوی_eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  إذا(ctx->ټول_len >= 0) {
    blen = curlx_sotouz_range(ctx->ټول_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  إذا(ctx->read_cb && blen) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_پیل(&guard, معلومات, easy_cr_دننه_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_معلومات);
    CURL_CBAPI_پای_ټکی(&guard);
    ctx->has_کارول_شوی_cb = TRUE;
  }

  بدل(nread) {
  حالة 0:
    إذا((ctx->ټول_len >= 0) && (ctx->read_len < ctx->ټول_len)) {
      failf(معلومات, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->ټول_len);
      پایله = CURLE_READ_ERROR;
      اكسر;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->کتل_شوی_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(معلومات, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_پایله = CURLE_ABORTED_BY_CALLBACK;
    پایله = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    إذا(معلومات->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(معلومات, "Read callback asked for PAUSE when not supported");
      پایله = CURLE_READ_ERROR;
      اكسر;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(معلومات, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    پایله = Curl_xfer_pause_send(معلومات, TRUE);
    اكسر; /* nothing was read */

  افتراضي:
    إذا(nread > blen) {
      /* the read function returned a too large value */
      failf(معلومات, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_پایله = CURLE_READ_ERROR;
      پایله = CURLE_READ_ERROR;
      اكسر;
    }
    ctx->read_len += nread;
    إذا(ctx->ټول_len >= 0)
      ctx->کتل_شوی_eos = (ctx->read_len >= ctx->ټول_len);
    *pnread = nread;
    *peos = (bool)ctx->کتل_شوی_eos;
    اكسر;
  }
  CURL_TRC_READ(معلومات, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->ټول_len, ctx->read_len, (صحيح)پایله,
                *pnread, *peos);
  أرجع پایله;
}

ساكن bool cr_دننه_needs_rewind(بنية Curl_easy *معلومات,
                               بنية Curl_creader *reader)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (bool)ctx->has_کارول_شوی_cb;
}

ساكن curl_off_t cr_دننه_ټول_اوږدتیا(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *reader)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->ټول_len;
}

ساكن CURLcode cr_دننه_resume_from(بنية Curl_easy *معلومات,
                                  بنية Curl_creader *reader,
                                  curl_off_t دننه_واټن)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  صحيح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(معلومات->conn);
  /* already started reading? */
  إذا(ctx->read_len)
    أرجع CURLE_READ_ERROR;

  إذا(معلومات->سېټ.seek_func) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_پیل(&guard, معلومات, easy_seek_func);
    seekerr = معلومات->سېټ.seek_func(معلومات->سېټ.seek_client, دننه_واټن, SEEK_SET);
    CURL_CBAPI_پای_ټکی(&guard);
  }

  إذا(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(معلومات, "Could not seek stream");
      أرجع CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    افعل {
      بنية Curl_mapi_guard guard;
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (دننه_واټن - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(دننه_واټن - passed);
      size_t actuallyread;

      CURL_CBAPI_پیل(&guard, معلومات, easy_cr_دننه_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_معلومات);
      CURL_CBAPI_پای_ټکی(&guard);

      passed += actuallyread;
      إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(معلومات, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < دننه_واټن);
  }

  /* now, decrease the size of the read */
  إذا(ctx->ټول_len > 0) {
    ctx->ټول_len -= دننه_واټن;

    إذا(ctx->ټول_len <= 0) {
      failf(معلومات, "File already completely uploaded");
      أرجع CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  أرجع CURLE_OK;
}

ساكن CURLcode cr_دننه_rewind(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  إذا(!ctx->has_کارول_شوی_cb)
    أرجع CURLE_OK;

  إذا(معلومات->سېټ.seek_func) {
    بنية Curl_mapi_guard guard;
    صحيح err;

    CURL_CBAPI_پیل(&guard, معلومات, easy_seek_func);
    err = معلومات->سېټ.seek_func(معلومات->سېټ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_پای_ټکی(&guard);
    CURL_TRC_READ(معلومات, "cr_in, rewind via set.seek_func -> %d", err);
    إذا(err) {
      failf(معلومات, "seek callback returned error %d", err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا إذا(معلومات->سېټ.ioctl_func) {
    بنية Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_پیل(&guard, معلومات, easy_ioctl_func);
    err = معلومات->سېټ.ioctl_func(معلومات, CURLIOCMD_RESTARTREAD,
                               معلومات->سېټ.ioctl_client);
    CURL_CBAPI_پای_ټکی(&guard);
    CURL_TRC_READ(معلومات, "cr_in, rewind via set.ioctl_func -> %d", (صحيح)err);
    إذا(err) {
      failf(معلومات, "ioctl callback returned error %d", (صحيح)err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
    إذا(معلومات->state.fread_func == (curl_read_callback)fread) {
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
      صحيح err = fseek(معلومات->state.دننه, 0, SEEK_SET);
      CURL_TRC_READ(معلومات, "cr_in, rewind via fseek -> %d(%d)",
                    (صحيح)err, (صحيح)errno);
      إذا(err != -1)
        /* successful rewind */
        أرجع CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(معلومات, "necessary data rewind was not possible");
    أرجع CURLE_SEND_FAIL_REWIND;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cr_دننه_cntrl(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;

  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    أرجع cr_دننه_rewind(معلومات, reader);
  حالة CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->کتل_شوی_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_دننه_is_paused(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader)
{
  بنية cr_دننه_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (bool)ctx->is_paused;
}

ساكن ثابت بنية Curl_crtype cr_دننه = {
  "cr-in",
  cr_دننه_init,
  cr_دننه_read,
  Curl_creader_def_close,
  cr_دننه_needs_rewind,
  cr_دننه_ټول_اوږدتیا,
  cr_دننه_resume_from,
  cr_دننه_cntrl,
  cr_دننه_is_paused,
  Curl_creader_def_بشپړ,
  حجم(بنية cr_دننه_ctx)
};

CURLcode Curl_creader_create(بنية Curl_creader **preader,
                             بنية Curl_easy *معلومات,
                             ثابت بنية Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  بنية Curl_creader *reader = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  فراغ *p;

  DEBUGASSERT(crt->creader_اندازه >= حجم(بنية Curl_creader));
  p = curlx_calloc(1, crt->creader_اندازه);
  إذا(!p)
    اذهب بهر;

  reader = (بنية Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  پایله = crt->do_init(معلومات, reader);

بهر:
  *preader = پایله ? عدم : reader;
  إذا(پایله)
    curlx_free(reader);
  أرجع پایله;
}

فراغ Curl_creader_free(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  إذا(reader) {
    reader->crt->do_close(معلومات, reader);
    curlx_free(reader);
  }
}

بنية cr_lc_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  بټ(read_eos);  /* we read an EOS from the next reader */
  بټ(eos);       /* we have returned an EOS */
  بټ(prev_cr);   /* the last byte was a CR */
};

ساكن CURLcode cr_lc_init(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد);
  أرجع CURLE_OK;
}

ساكن فراغ cr_lc_close(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ساكن CURLcode cr_lc_read(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  CURLcode پایله;
  size_t nread, i, پیل, n;
  bool eos;

  إذا(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  إذا(Curl_bufq_is_تشه(&ctx->buf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      أرجع CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    پایله = Curl_creader_read(معلومات, reader->بل, buf, blen, &nread, &eos);
    إذا(پایله)
      أرجع پایله;
    ctx->read_eos = eos;

    إذا(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      إذا(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب بهر;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    لكل(i = پیل = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      إذا((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        استمر;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      پایله = Curl_bufq_cwrite(&ctx->buf, buf + پیل, i - پیل, &n);
      إذا(!پایله)
        پایله = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      إذا(پایله)
        أرجع پایله;
      پیل = i + 1;
    }

    إذا(پیل < i) { /* leftover */
      پایله = Curl_bufq_cwrite(&ctx->buf, buf + پیل, i - پیل, &n);
      إذا(پایله)
        أرجع پایله;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_تشه(&ctx->buf));
  *peos = FALSE;
  پایله = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  إذا(!پایله && ctx->read_eos && Curl_bufq_is_تشه(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

بهر:
  CURL_TRC_READ(معلومات, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)پایله, *pnread, *peos);
  أرجع پایله;
}

ساكن curl_off_t cr_lc_ټول_اوږدتیا(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_ټول_اوږدتیا,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_بشپړ,
  حجم(بنية cr_lc_ctx)
};

ساكن CURLcode cr_lc_جمع_کړه(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = عدم;
  CURLcode پایله;

  پایله = Curl_creader_create(&reader, معلومات, &cr_lc, CURL_CR_CONTENT_ENCODE);
  إذا(!پایله)
    پایله = Curl_creader_جمع_کړه(معلومات, reader);

  إذا(پایله && reader)
    Curl_creader_free(معلومات, reader);
  أرجع پایله;
}

ساكن CURLcode do_init_reader_ډېری(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *r)
{
  CURLcode پایله = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!معلومات->req.reader.ډېری);

  معلومات->req.reader.ډېری = r;
  clen = r->crt->ټول_اوږدتیا(معلومات, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  إذا(clen &&
#که_تعریف_شوی CURL_PREFER_LF_LINEENDS
    (معلومات->سېټ.crlf || معلومات->state.prefer_ascii)
#وإلا
    معلومات->سېټ.crlf
#د_که_پای
    ) {
    پایله = cr_lc_جمع_کړه(معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  أرجع پایله;
}

CURLcode Curl_creader_سېټ_fread(بنية Curl_easy *معلومات, curl_off_t len)
{
  CURLcode پایله;
  بنية Curl_creader *r;
  بنية cr_دننه_ctx *ctx;

  پایله = Curl_creader_create(&r, معلومات, &cr_دننه, CURL_CR_CLIENT);
  إذا(پایله || !r)
    اذهب بهر;
  ctx = r->ctx;
  ctx->ټول_len = len;

  cl_reset_reader(معلومات);
  پایله = do_init_reader_ډېری(معلومات, r);
بهر:
  CURL_TRC_READ(معلومات, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحيح)پایله);
  أرجع پایله;
}

CURLcode Curl_creader_جمع_کړه(بنية Curl_easy *معلومات,
                          بنية Curl_creader *reader)
{
  CURLcode پایله;
  بنية Curl_creader **anchor = &معلومات->req.reader.ډېری;

  إذا(!*anchor) {
    پایله = Curl_creader_سېټ_fread(معلومات, معلومات->state.infilesize);
    إذا(پایله)
      أرجع پایله;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  طالما(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->بل;
  reader->بل = *anchor;
  *anchor = reader;
  أرجع CURLE_OK;
}

CURLcode Curl_creader_سېټ(بنية Curl_easy *معلومات, بنية Curl_creader *r)
{
  CURLcode پایله;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(معلومات);
  پایله = do_init_reader_ډېری(معلومات, r);
  إذا(پایله)
    Curl_creader_free(معلومات, r);
  أرجع پایله;
}

CURLcode Curl_client_read(بنية Curl_easy *معلومات, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode پایله;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  إذا(!معلومات->req.reader.ډېری) {
    پایله = Curl_creader_سېټ_fread(معلومات, معلومات->state.infilesize);
    إذا(پایله)
      أرجع پایله;
    DEBUGASSERT(معلومات->req.reader.ډېری);
  }
  إذا(!معلومات->req.reader_started) {
    Curl_rlimit_پیل(&معلومات->پرمختګ.ul.rlimit, Curl_pgrs_now(معلومات), -1);
    معلومات->req.reader_started = TRUE;
  }

  إذا(Curl_rlimit_فعال(&معلومات->پرمختګ.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&معلومات->پرمختګ.ul.rlimit, عدم);
    إذا(ul_avail <= 0) {
      پایله = CURLE_OK;
      *eos = FALSE;
      اذهب بهر;
    }
    إذا(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  پایله = Curl_creader_read(معلومات, معلومات->req.reader.ډېری, buf, blen,
                             nread, eos);

بهر:
  CURL_TRC_READ(معلومات, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)پایله, *nread, *eos);
  أرجع پایله;
}

bool Curl_creader_needs_rewind(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ډېری;
  طالما(reader) {
    إذا(reader->crt->needs_rewind(معلومات, reader)) {
      CURL_TRC_READ(معلومات, "client reader needs rewind before next request");
      أرجع TRUE;
    }
    reader = reader->بل;
  }
  أرجع FALSE;
}

ساكن CURLcode cr_null_read(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)buf;
  (فراغ)blen;
  *pnread = 0;
  *peos = TRUE;
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_null_ټول_اوږدتیا(بنية Curl_easy *معلومات,
                                       بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)معلومات;
  (فراغ)reader;
  أرجع 0;
}

ساكن ثابت بنية Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_ټول_اوږدتیا,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_بشپړ,
  حجم(بنية Curl_creader)
};

CURLcode Curl_creader_سېټ_null(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r;
  CURLcode پایله;

  پایله = Curl_creader_create(&r, معلومات, &cr_null, CURL_CR_CLIENT);
  إذا(پایله)
    أرجع پایله;

  cl_reset_reader(معلومات);
  أرجع do_init_reader_ډېری(معلومات, r);
}

بنية cr_buf_ctx {
  بنية Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t شاخص;
};

ساكن CURLcode cr_buf_read(بنية Curl_easy *معلومات,
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
    حافظه_نقل(buf, ctx->buf + ctx->شاخص, nread);
    *pnread = nread;
    ctx->شاخص += nread;
    *peos = (ctx->شاخص == ctx->blen);
  }
  CURL_TRC_READ(معلومات, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  أرجع CURLE_OK;
}

ساكن bool cr_buf_needs_rewind(بنية Curl_easy *معلومات,
                                بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->شاخص > 0;
}

ساكن CURLcode cr_buf_cntrl(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    ctx->شاخص = 0;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_buf_ټول_اوږدتیا(بنية Curl_easy *معلومات,
                                      بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (curl_off_t)ctx->blen;
}

ساكن CURLcode cr_buf_resume_from(بنية Curl_easy *معلومات,
                                   بنية Curl_creader *reader,
                                   curl_off_t دننه_واټن)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (فراغ)معلومات;
  DEBUGASSERT(معلومات->conn);
  /* already started reading? */
  إذا(ctx->شاخص)
    أرجع CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(دننه_واټن, 0, SIZE_MAX);
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
  cr_buf_ټول_اوږدتیا,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_بشپړ,
  حجم(بنية cr_buf_ctx)
};

CURLcode Curl_creader_سېټ_buf(بنية Curl_easy *معلومات,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode پایله;
  بنية Curl_creader *r;
  بنية cr_buf_ctx *ctx;

  پایله = Curl_creader_create(&r, معلومات, &cr_buf, CURL_CR_CLIENT);
  إذا(پایله)
    اذهب بهر;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->شاخص = 0;

  cl_reset_reader(معلومات);
  پایله = do_init_reader_ډېری(معلومات, r);
بهر:
  CURL_TRC_READ(معلومات, "add buf reader, len=%zu -> %d", blen, (صحيح)پایله);
  أرجع پایله;
}

curl_off_t Curl_creader_ټول_اوږدتیا(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = معلومات->req.reader.ډېری;
  أرجع r ? r->crt->ټول_اوږدتیا(معلومات, r) : -1;
}

curl_off_t Curl_creader_client_اوږدتیا(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = معلومات->req.reader.ډېری;
  طالما(r && r->phase != CURL_CR_CLIENT)
    r = r->بل;
  أرجع r ? r->crt->ټول_اوږدتیا(معلومات, r) : -1;
}

CURLcode Curl_creader_unpause(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ډېری;
  CURLcode پایله = CURLE_OK;

  طالما(reader) {
    پایله = reader->crt->cntrl(معلومات, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(معلومات, "unpausing %s -> %d", reader->crt->name, (صحيح)پایله);
    إذا(پایله)
      اكسر;
    reader = reader->بل;
  }
  أرجع پایله;
}

bool Curl_creader_is_paused(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ډېری;

  طالما(reader) {
    إذا(reader->crt->is_paused(معلومات, reader))
      أرجع TRUE;
    reader = reader->بل;
  }
  أرجع FALSE;
}

فراغ Curl_creader_بشپړ(بنية Curl_easy *معلومات, صحيح premature)
{
  بنية Curl_creader *reader = معلومات->req.reader.ډېری;
  طالما(reader) {
    reader->crt->بشپړ(معلومات, reader, premature);
    reader = reader->بل;
  }
}

بنية Curl_creader *Curl_creader_get_by_type(بنية Curl_easy *معلومات,
                                              ثابت بنية Curl_crtype *crt)
{
  بنية Curl_creader *r;
  لكل(r = معلومات->req.reader.ډېری; r; r = r->بل) {
    إذا(r->crt == crt)
      أرجع r;
  }
  أرجع عدم;
}
