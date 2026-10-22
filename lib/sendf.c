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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#جيڪڏهن_پورو

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
  بنية Curl_cwriter *writer = معلومات->req.writer.ڍير;
  طالما(writer) {
    معلومات->req.writer.ڍير = writer->ايندڙ;
    writer->cwt->do_close(معلومات, writer);
    curlx_free(writer);
    writer = معلومات->req.writer.ڍير;
  }
  معلومات->req.writer.paused = FALSE;
}

ساكن فراغ cl_reset_reader(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ڍير;
  معلومات->req.reader_started = FALSE;
  طالما(reader) {
    معلومات->req.reader.ڍير = reader->ايندڙ;
    reader->crt->do_close(معلومات, reader);
    curlx_free(reader);
    reader = معلومات->req.reader.ڍير;
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

CURLcode Curl_client_شروعات(بنية Curl_easy *معلومات)
{
  إذا(معلومات->req.rewind_read) {
    بنية Curl_creader *r = معلومات->req.reader.ڍير;
    CURLcode نتيجو = CURLE_OK;

    CURL_TRC_READ(معلومات, "client start, rewind readers");
    طالما(r) {
      نتيجو = r->crt->cntrl(معلومات, r, CURL_CRCNTRL_REWIND);
      إذا(نتيجو) {
        failf(معلومات, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحيح)نتيجو);
        أرجع نتيجو;
      }
      r = r->ايندڙ;
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

فراغ Curl_creader_سيٽ_rewind(بنية Curl_easy *معلومات, bool enable)
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
  أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(بنية Curl_easy *معلومات,
                                بنية Curl_cwriter *writer)
{
  أرجع Curl_cwriter_flush(معلومات, writer->ايندڙ);
}

فراغ Curl_cwriter_def_close(بنية Curl_easy *معلومات,
                            بنية Curl_cwriter *writer)
{
  (فراغ)معلومات;
  (فراغ)writer;
}

ساكن size_t get_max_body_write_len(بنية Curl_easy *معلومات, curl_off_t حد_بندي)
{
  إذا(حد_بندي != -1) {
    /* How much more are we allowed to write? */
    أرجع curlx_sotouz_range(حد_بندي - معلومات->req.bytecount, 0, SIZE_MAX);
  }
  أرجع SIZE_MAX;
}

بنية cw_download_ctx {
  بنية Curl_cwriter super;
  ٻٽو_انگ(started_response);
  ٻٽو_انگ(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساكن CURLcode cw_download_write(بنية Curl_easy *معلومات,
                                  بنية Curl_cwriter *writer, صحيح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  بنية cw_download_ctx *ctx = writer->ctx;
  CURLcode نتيجو;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  إذا(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || معلومات->req.upload_پورو)) {
    Curl_pgrs_وقت(معلومات, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  إذا(!(type & CLIENTWRITE_BODY)) {
    إذا(is_connect && معلومات->سيٽ.suppress_connect_سرا)
      أرجع CURLE_OK;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    إذا(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      أرجع CURLE_OK;
#جيڪڏهن_پورو
    نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);
    CURL_TRC_WRITE(معلومات, "download_write header(type=%x, blen=%zu) -> %d",
                   (بنانشان صحيح)type, nbytes, (صحيح)نتيجو);
    أرجع نتيجو;
  }

  إذا(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_شروعات(&معلومات->ترقي.dl.rlimit, Curl_pgrs_now(معلومات),
                      معلومات->req.سائيز);
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
                   "did not want a BODY", (بنانشان صحيح)type, nbytes);
    معلومات->req.download_پورو = TRUE;
    إذا(معلومات->info.سرو_سائيز)
      /* if headers have been received, this is fine */
      أرجع CURLE_OK;
    أرجع CURLE_WEIRD_SERVER_جواب;
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
      معلومات->req.download_پورو = TRUE;
    }

    إذا((type & CLIENTWRITE_EOS) && !معلومات->req.no_body &&
       (معلومات->req.سائيز > معلومات->req.bytecount)) {
      failf(معلومات, "end of response with %" FMT_OFF_T " bytes missing",
            معلومات->req.سائيز - معلومات->req.bytecount);
      أرجع CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  إذا(معلومات->سيٽ.max_filesize && !معلومات->req.ignorebody) {
    size_t wmax = get_max_body_write_len(معلومات, معلومات->سيٽ.max_filesize);
    إذا(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  إذا(!معلومات->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    نتيجو = Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nwrite);
    CURL_TRC_WRITE(معلومات, "download_write body(type=%x, blen=%zu) -> %d",
                   (بنانشان صحيح)type, nbytes, (صحيح)نتيجو);
    إذا(نتيجو)
      أرجع نتيجو;
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
            excess_len, معلومات->req.سائيز, معلومات->req.maxdownload,
            معلومات->req.bytecount);
      connclose(معلومات->conn);
    }
  }
  وإلا إذا((nwrite < nbytes) && !معلومات->req.ignorebody) {
    failf(معلومات, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          معلومات->سيٽ.max_filesize, معلومات->req.bytecount);
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
  إذا(type & CLIENTWRITE_BODY && معلومات->سيٽ.verbose && !معلومات->req.ignorebody) {
    Curl_debug(معلومات, CURLINFO_معلومات_اندريون, buf, nbytes);
  }
  أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, nbytes);
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

ساكن فراغ cwriter_جوڙيو(بنية Curl_easy *معلومات,
                        بنية Curl_cwriter *writer)
{
  بنية Curl_cwriter **anchor = &معلومات->req.writer.ڍير;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  طالما(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->ايندڙ;
  writer->ايندڙ = *anchor;
  *anchor = writer;
}

ساكن CURLcode do_init_writer_ڍير(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer;
  CURLcode نتيجو;

  DEBUGASSERT(!معلومات->req.writer.ڍير);
  نتيجو = Curl_cwriter_create(&معلومات->req.writer.ڍير,
                               معلومات, &Curl_cwt_ٻاهريون, CURL_CW_CLIENT);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_cwriter_create(&writer, معلومات, &cw_download, CURL_CW_PROTOCOL);
  إذا(نتيجو)
    أرجع نتيجو;
  cwriter_جوڙيو(معلومات, writer);

  نتيجو = Curl_cwriter_create(&writer, معلومات, &cw_raw, CURL_CW_RAW);
  إذا(نتيجو)
    أرجع نتيجو;
  cwriter_جوڙيو(معلومات, writer);

  أرجع نتيجو;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(بنية Curl_easy *معلومات, صحيح type, ثابت حرف *buf,
                           size_t len)
{
  CURLcode نتيجو;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_سرو | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  إذا(!معلومات->req.writer.ڍير) {
    نتيجو = do_init_writer_ڍير(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    DEBUGASSERT(معلومات->req.writer.ڍير);
  }

  نتيجو = Curl_cwriter_write(معلومات, معلومات->req.writer.ڍير, type, buf, len);
  CURL_TRC_WRITE(معلومات, "client_write(type=%x, len=%zu) -> %d",
                 (بنانشان صحيح)type, len, (صحيح)نتيجو);
  أرجع نتيجو;
}

CURLcode Curl_client_flush(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;

  إذا(!معلومات->req.writer.ڍير) {
    نتيجو = do_init_writer_ڍير(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    DEBUGASSERT(معلومات->req.writer.ڍير);
  }

  نتيجو = Curl_cwriter_flush(معلومات, معلومات->req.writer.ڍير);
  CURL_TRC_WRITE(معلومات, "client_flush() -> %d", (صحيح)نتيجو);
  أرجع نتيجو;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(بنية Curl_cwriter **pwriter,
                             بنية Curl_easy *معلومات,
                             ثابت بنية Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *writer = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  فراغ *p;

  DEBUGASSERT(cwt->cwriter_سائيز >= حجم(بنية Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_سائيز);
  إذا(!p)
    اذهب ٻاهريون;

  writer = (بنية Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  نتيجو = cwt->do_init(معلومات, writer);

ٻاهريون:
  *pwriter = نتيجو ? عدم : writer;
  إذا(نتيجو)
    curlx_free(writer);
  أرجع نتيجو;
}

فراغ Curl_cwriter_free(بنية Curl_easy *معلومات,
                       بنية Curl_cwriter *writer)
{
  إذا(writer) {
    writer->cwt->do_close(معلومات, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ڳڻپ(بنية Curl_easy *معلومات, Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *w;
  size_t n = 0;

  لكل(w = معلومات->req.writer.ڍير; w; w = w->ايندڙ) {
    إذا(w->phase == phase)
      ++n;
  }
  أرجع n;
}

ساكن CURLcode cwriter_ensure_pause_writer(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer =
    Curl_cwriter_get_by_type(معلومات, &Curl_cwt_pause);
  CURLcode نتيجو = CURLE_OK;

  إذا(!writer) {
    نتيجو = Curl_cwriter_create(&writer, معلومات, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    إذا(!نتيجو)
      cwriter_جوڙيو(معلومات, writer);
  }
  أرجع نتيجو;
}

CURLcode Curl_cwriter_جوڙيو(بنية Curl_easy *معلومات,
                          بنية Curl_cwriter *writer)
{
  CURLcode نتيجو;
  بنية Curl_cwriter **anchor = &معلومات->req.writer.ڍير;

  إذا(!*anchor) {
    نتيجو = do_init_writer_ڍير(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    نتيجو = cwriter_ensure_pause_writer(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  cwriter_جوڙيو(معلومات, writer);
  أرجع CURLE_OK;
}

بنية Curl_cwriter *Curl_cwriter_get_by_name(بنية Curl_easy *معلومات,
                                              ثابت حرف *name)
{
  بنية Curl_cwriter *writer;
  لكل(writer = معلومات->req.writer.ڍير; writer; writer = writer->ايندڙ) {
    إذا(!strcmp(name, writer->cwt->name))
      أرجع writer;
  }
  أرجع عدم;
}

بنية Curl_cwriter *Curl_cwriter_get_by_type(بنية Curl_easy *معلومات,
                                              ثابت بنية Curl_cwtype *cwt)
{
  بنية Curl_cwriter *writer;
  لكل(writer = معلومات->req.writer.ڍير; writer; writer = writer->ايندڙ) {
    إذا(writer->cwt == cwt)
      أرجع writer;
  }
  أرجع عدم;
}

CURLcode Curl_cwriter_unpause(بنية Curl_easy *معلومات)
{
  معلومات->req.writer.paused = FALSE;
  أرجع Curl_cwriter_flush(معلومات, معلومات->req.writer.ڍير);
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
    reader = reader->ايندڙ;
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
  إذا(reader->ايندڙ)
    أرجع reader->ايندڙ->crt->do_read(معلومات, reader->ايندڙ, buf, blen,
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

curl_off_t Curl_creader_def_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                         بنية Curl_creader *reader)
{
  أرجع reader->ايندڙ ?
         reader->ايندڙ->crt->ڪل_ڊيگهائي(معلومات, reader->ايندڙ) : -1;
}

CURLcode Curl_creader_def_resume_from(بنية Curl_easy *معلومات,
                                      بنية Curl_creader *reader,
                                      curl_off_t وٿي)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)وٿي;
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

فراغ Curl_creader_def_پورو(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader, صحيح premature)
{
  (فراغ)معلومات;
  (فراغ)reader;
  (فراغ)premature;
}

بنية cr_اندريون_ctx {
  بنية Curl_creader super;
  curl_read_callback read_cb;
  فراغ *cb_user_معلومات;
  curl_off_t ڪل_len;
  curl_off_t read_len;
  CURLcode error_نتيجو;
  ٻٽو_انگ(ڏٺل_eos);
  ٻٽو_انگ(errored);
  ٻٽو_انگ(has_استعمال_ٿيل_cb);
  ٻٽو_انگ(is_paused);
};

ساكن CURLcode cr_اندريون_init(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  ctx->read_cb = معلومات->state.fread_func;
  ctx->cb_user_معلومات = معلومات->state.اندريون;
  ctx->ڪل_len = -1;
  ctx->read_len = 0;
  أرجع CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_اندريون_read(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  إذا(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_نتيجو;
  }
  إذا(ctx->ڏٺل_eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  إذا(ctx->ڪل_len >= 0) {
    blen = curlx_sotouz_range(ctx->ڪل_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  إذا(ctx->read_cb && blen) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروعات(&guard, معلومات, easy_cr_اندريون_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_معلومات);
    CURL_CBAPI_پڇاڙي(&guard);
    ctx->has_استعمال_ٿيل_cb = TRUE;
  }

  بدل(nread) {
  حالة 0:
    إذا((ctx->ڪل_len >= 0) && (ctx->read_len < ctx->ڪل_len)) {
      failf(معلومات, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->ڪل_len);
      نتيجو = CURLE_READ_ERROR;
      اكسر;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ڏٺل_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(معلومات, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتيجو = CURLE_ABORTED_BY_CALLBACK;
    نتيجو = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    إذا(معلومات->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(معلومات, "Read callback asked for PAUSE when not supported");
      نتيجو = CURLE_READ_ERROR;
      اكسر;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(معلومات, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    نتيجو = Curl_xfer_pause_send(معلومات, TRUE);
    اكسر; /* nothing was read */

  افتراضي:
    إذا(nread > blen) {
      /* the read function returned a too large value */
      failf(معلومات, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتيجو = CURLE_READ_ERROR;
      نتيجو = CURLE_READ_ERROR;
      اكسر;
    }
    ctx->read_len += nread;
    إذا(ctx->ڪل_len >= 0)
      ctx->ڏٺل_eos = (ctx->read_len >= ctx->ڪل_len);
    *pnread = nread;
    *peos = (bool)ctx->ڏٺل_eos;
    اكسر;
  }
  CURL_TRC_READ(معلومات, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->ڪل_len, ctx->read_len, (صحيح)نتيجو,
                *pnread, *peos);
  أرجع نتيجو;
}

ساكن bool cr_اندريون_needs_rewind(بنية Curl_easy *معلومات,
                               بنية Curl_creader *reader)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (bool)ctx->has_استعمال_ٿيل_cb;
}

ساكن curl_off_t cr_اندريون_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *reader)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->ڪل_len;
}

ساكن CURLcode cr_اندريون_resume_from(بنية Curl_easy *معلومات,
                                  بنية Curl_creader *reader,
                                  curl_off_t وٿي)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  صحيح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(معلومات->conn);
  /* already started reading? */
  إذا(ctx->read_len)
    أرجع CURLE_READ_ERROR;

  إذا(معلومات->سيٽ.seek_func) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروعات(&guard, معلومات, easy_seek_func);
    seekerr = معلومات->سيٽ.seek_func(معلومات->سيٽ.seek_client, وٿي, SEEK_SET);
    CURL_CBAPI_پڇاڙي(&guard);
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
        (وٿي - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(وٿي - passed);
      size_t actuallyread;

      CURL_CBAPI_شروعات(&guard, معلومات, easy_cr_اندريون_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_معلومات);
      CURL_CBAPI_پڇاڙي(&guard);

      passed += actuallyread;
      إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(معلومات, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < وٿي);
  }

  /* now, decrease the size of the read */
  إذا(ctx->ڪل_len > 0) {
    ctx->ڪل_len -= وٿي;

    إذا(ctx->ڪل_len <= 0) {
      failf(معلومات, "File already completely uploaded");
      أرجع CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  أرجع CURLE_OK;
}

ساكن CURLcode cr_اندريون_rewind(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  إذا(!ctx->has_استعمال_ٿيل_cb)
    أرجع CURLE_OK;

  إذا(معلومات->سيٽ.seek_func) {
    بنية Curl_mapi_guard guard;
    صحيح err;

    CURL_CBAPI_شروعات(&guard, معلومات, easy_seek_func);
    err = معلومات->سيٽ.seek_func(معلومات->سيٽ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_پڇاڙي(&guard);
    CURL_TRC_READ(معلومات, "cr_in, rewind via set.seek_func -> %d", err);
    إذا(err) {
      failf(معلومات, "seek callback returned error %d", err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا إذا(معلومات->سيٽ.ioctl_func) {
    بنية Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_شروعات(&guard, معلومات, easy_ioctl_func);
    err = معلومات->سيٽ.ioctl_func(معلومات, CURLIOCMD_RESTARTREAD,
                               معلومات->سيٽ.ioctl_client);
    CURL_CBAPI_پڇاڙي(&guard);
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
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wcast-function-type-strict"
#جيڪڏهن_پورو
    إذا(معلومات->state.fread_func == (curl_read_callback)fread) {
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
      صحيح err = fseek(معلومات->state.اندريون, 0, SEEK_SET);
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

ساكن CURLcode cr_اندريون_cntrl(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;

  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    أرجع cr_اندريون_rewind(معلومات, reader);
  حالة CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->ڏٺل_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_اندريون_is_paused(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader)
{
  بنية cr_اندريون_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (bool)ctx->is_paused;
}

ساكن ثابت بنية Curl_crtype cr_اندريون = {
  "cr-in",
  cr_اندريون_init,
  cr_اندريون_read,
  Curl_creader_def_close,
  cr_اندريون_needs_rewind,
  cr_اندريون_ڪل_ڊيگهائي,
  cr_اندريون_resume_from,
  cr_اندريون_cntrl,
  cr_اندريون_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_اندريون_ctx)
};

CURLcode Curl_creader_create(بنية Curl_creader **preader,
                             بنية Curl_easy *معلومات,
                             ثابت بنية Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  فراغ *p;

  DEBUGASSERT(crt->creader_سائيز >= حجم(بنية Curl_creader));
  p = curlx_calloc(1, crt->creader_سائيز);
  إذا(!p)
    اذهب ٻاهريون;

  reader = (بنية Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  نتيجو = crt->do_init(معلومات, reader);

ٻاهريون:
  *preader = نتيجو ? عدم : reader;
  إذا(نتيجو)
    curlx_free(reader);
  أرجع نتيجو;
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
  ٻٽو_انگ(read_eos);  /* we read an EOS from the next reader */
  ٻٽو_انگ(eos);       /* we have returned an EOS */
  ٻٽو_انگ(prev_cr);   /* the last byte was a CR */
};

ساكن CURLcode cr_lc_init(بنية Curl_easy *معلومات, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندي);
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
  CURLcode نتيجو;
  size_t nread, i, شروعات, n;
  bool eos;

  إذا(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  إذا(Curl_bufq_is_خالي_جايون(&ctx->buf)) {
    إذا(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      أرجع CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    نتيجو = Curl_creader_read(معلومات, reader->ايندڙ, buf, blen, &nread, &eos);
    إذا(نتيجو)
      أرجع نتيجو;
    ctx->read_eos = eos;

    إذا(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      إذا(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      إذا(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب ٻاهريون;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    لكل(i = شروعات = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      إذا((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        استمر;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      نتيجو = Curl_bufq_cwrite(&ctx->buf, buf + شروعات, i - شروعات, &n);
      إذا(!نتيجو)
        نتيجو = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      إذا(نتيجو)
        أرجع نتيجو;
      شروعات = i + 1;
    }

    إذا(شروعات < i) { /* leftover */
      نتيجو = Curl_bufq_cwrite(&ctx->buf, buf + شروعات, i - شروعات, &n);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_خالي_جايون(&ctx->buf));
  *peos = FALSE;
  نتيجو = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  إذا(!نتيجو && ctx->read_eos && Curl_bufq_is_خالي_جايون(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ٻاهريون:
  CURL_TRC_READ(معلومات, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتيجو, *pnread, *peos);
  أرجع نتيجو;
}

ساكن curl_off_t cr_lc_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
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
  cr_lc_ڪل_ڊيگهائي,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_lc_ctx)
};

ساكن CURLcode cr_lc_جوڙيو(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتيجو;

  نتيجو = Curl_creader_create(&reader, معلومات, &cr_lc, CURL_CR_CONTENT_ENCODE);
  إذا(!نتيجو)
    نتيجو = Curl_creader_جوڙيو(معلومات, reader);

  إذا(نتيجو && reader)
    Curl_creader_free(معلومات, reader);
  أرجع نتيجو;
}

ساكن CURLcode do_init_reader_ڍير(بنية Curl_easy *معلومات,
                                     بنية Curl_creader *r)
{
  CURLcode نتيجو = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!معلومات->req.reader.ڍير);

  معلومات->req.reader.ڍير = r;
  clen = r->crt->ڪل_ڊيگهائي(معلومات, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  إذا(clen &&
#جيڪڏهن_وصف_ٿيل CURL_PREFER_LF_LINEENDS
    (معلومات->سيٽ.crlf || معلومات->state.prefer_ascii)
#وإلا
    معلومات->سيٽ.crlf
#جيڪڏهن_پورو
    ) {
    نتيجو = cr_lc_جوڙيو(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  أرجع نتيجو;
}

CURLcode Curl_creader_سيٽ_fread(بنية Curl_easy *معلومات, curl_off_t len)
{
  CURLcode نتيجو;
  بنية Curl_creader *r;
  بنية cr_اندريون_ctx *ctx;

  نتيجو = Curl_creader_create(&r, معلومات, &cr_اندريون, CURL_CR_CLIENT);
  إذا(نتيجو || !r)
    اذهب ٻاهريون;
  ctx = r->ctx;
  ctx->ڪل_len = len;

  cl_reset_reader(معلومات);
  نتيجو = do_init_reader_ڍير(معلومات, r);
ٻاهريون:
  CURL_TRC_READ(معلومات, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحيح)نتيجو);
  أرجع نتيجو;
}

CURLcode Curl_creader_جوڙيو(بنية Curl_easy *معلومات,
                          بنية Curl_creader *reader)
{
  CURLcode نتيجو;
  بنية Curl_creader **anchor = &معلومات->req.reader.ڍير;

  إذا(!*anchor) {
    نتيجو = Curl_creader_سيٽ_fread(معلومات, معلومات->state.infilesize);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  طالما(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->ايندڙ;
  reader->ايندڙ = *anchor;
  *anchor = reader;
  أرجع CURLE_OK;
}

CURLcode Curl_creader_سيٽ(بنية Curl_easy *معلومات, بنية Curl_creader *r)
{
  CURLcode نتيجو;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(معلومات);
  نتيجو = do_init_reader_ڍير(معلومات, r);
  إذا(نتيجو)
    Curl_creader_free(معلومات, r);
  أرجع نتيجو;
}

CURLcode Curl_client_read(بنية Curl_easy *معلومات, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode نتيجو;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  إذا(!معلومات->req.reader.ڍير) {
    نتيجو = Curl_creader_سيٽ_fread(معلومات, معلومات->state.infilesize);
    إذا(نتيجو)
      أرجع نتيجو;
    DEBUGASSERT(معلومات->req.reader.ڍير);
  }
  إذا(!معلومات->req.reader_started) {
    Curl_rlimit_شروعات(&معلومات->ترقي.ul.rlimit, Curl_pgrs_now(معلومات), -1);
    معلومات->req.reader_started = TRUE;
  }

  إذا(Curl_rlimit_سرگرم(&معلومات->ترقي.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&معلومات->ترقي.ul.rlimit, عدم);
    إذا(ul_avail <= 0) {
      نتيجو = CURLE_OK;
      *eos = FALSE;
      اذهب ٻاهريون;
    }
    إذا(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  نتيجو = Curl_creader_read(معلومات, معلومات->req.reader.ڍير, buf, blen,
                             nread, eos);

ٻاهريون:
  CURL_TRC_READ(معلومات, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)نتيجو, *nread, *eos);
  أرجع نتيجو;
}

bool Curl_creader_needs_rewind(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ڍير;
  طالما(reader) {
    إذا(reader->crt->needs_rewind(معلومات, reader)) {
      CURL_TRC_READ(معلومات, "client reader needs rewind before next request");
      أرجع TRUE;
    }
    reader = reader->ايندڙ;
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

ساكن curl_off_t cr_null_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
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
  cr_null_ڪل_ڊيگهائي,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية Curl_creader)
};

CURLcode Curl_creader_سيٽ_null(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r;
  CURLcode نتيجو;

  نتيجو = Curl_creader_create(&r, معلومات, &cr_null, CURL_CR_CLIENT);
  إذا(نتيجو)
    أرجع نتيجو;

  cl_reset_reader(معلومات);
  أرجع do_init_reader_ڍير(معلومات, r);
}

بنية cr_buf_ctx {
  بنية Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t اشاريو;
};

ساكن CURLcode cr_buf_read(بنية Curl_easy *معلومات,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->اشاريو;

  إذا(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  وإلا {
    إذا(nread > blen)
      nread = blen;
    يادگيري_نقل(buf, ctx->buf + ctx->اشاريو, nread);
    *pnread = nread;
    ctx->اشاريو += nread;
    *peos = (ctx->اشاريو == ctx->blen);
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
  أرجع ctx->اشاريو > 0;
}

ساكن CURLcode cr_buf_cntrl(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    ctx->اشاريو = 0;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_buf_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                      بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع (curl_off_t)ctx->blen;
}

ساكن CURLcode cr_buf_resume_from(بنية Curl_easy *معلومات,
                                   بنية Curl_creader *reader,
                                   curl_off_t وٿي)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (فراغ)معلومات;
  DEBUGASSERT(معلومات->conn);
  /* already started reading? */
  إذا(ctx->اشاريو)
    أرجع CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(وٿي, 0, SIZE_MAX);
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
  cr_buf_ڪل_ڊيگهائي,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_buf_ctx)
};

CURLcode Curl_creader_سيٽ_buf(بنية Curl_easy *معلومات,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode نتيجو;
  بنية Curl_creader *r;
  بنية cr_buf_ctx *ctx;

  نتيجو = Curl_creader_create(&r, معلومات, &cr_buf, CURL_CR_CLIENT);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->اشاريو = 0;

  cl_reset_reader(معلومات);
  نتيجو = do_init_reader_ڍير(معلومات, r);
ٻاهريون:
  CURL_TRC_READ(معلومات, "add buf reader, len=%zu -> %d", blen, (صحيح)نتيجو);
  أرجع نتيجو;
}

curl_off_t Curl_creader_ڪل_ڊيگهائي(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = معلومات->req.reader.ڍير;
  أرجع r ? r->crt->ڪل_ڊيگهائي(معلومات, r) : -1;
}

curl_off_t Curl_creader_client_ڊيگهائي(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = معلومات->req.reader.ڍير;
  طالما(r && r->phase != CURL_CR_CLIENT)
    r = r->ايندڙ;
  أرجع r ? r->crt->ڪل_ڊيگهائي(معلومات, r) : -1;
}

CURLcode Curl_creader_unpause(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ڍير;
  CURLcode نتيجو = CURLE_OK;

  طالما(reader) {
    نتيجو = reader->crt->cntrl(معلومات, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(معلومات, "unpausing %s -> %d", reader->crt->name, (صحيح)نتيجو);
    إذا(نتيجو)
      اكسر;
    reader = reader->ايندڙ;
  }
  أرجع نتيجو;
}

bool Curl_creader_is_paused(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = معلومات->req.reader.ڍير;

  طالما(reader) {
    إذا(reader->crt->is_paused(معلومات, reader))
      أرجع TRUE;
    reader = reader->ايندڙ;
  }
  أرجع FALSE;
}

فراغ Curl_creader_پورو(بنية Curl_easy *معلومات, صحيح premature)
{
  بنية Curl_creader *reader = معلومات->req.reader.ڍير;
  طالما(reader) {
    reader->crt->پورو(معلومات, reader, premature);
    reader = reader->ايندڙ;
  }
}

بنية Curl_creader *Curl_creader_get_by_type(بنية Curl_easy *معلومات,
                                              ثابت بنية Curl_crtype *crt)
{
  بنية Curl_creader *r;
  لكل(r = معلومات->req.reader.ڍير; r; r = r->ايندڙ) {
    إذا(r->crt == crt)
      أرجع r;
  }
  أرجع عدم;
}
