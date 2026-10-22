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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#नोचेद्यदि defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "cw-out.h"
#अन्तर्भाव "cw-pause.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"

स्थिर शून्य cl_reset_writer(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cwriter *writer = दत्त->req.writer.राशिः;
  यावत्(writer) {
    दत्त->req.writer.राशिः = writer->अग्र;
    writer->cwt->do_close(दत्त, writer);
    curlx_free(writer);
    writer = दत्त->req.writer.राशिः;
  }
  दत्त->req.writer.paused = FALSE;
}

स्थिर शून्य cl_reset_reader(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = दत्त->req.reader.राशिः;
  दत्त->req.reader_started = FALSE;
  यावत्(reader) {
    दत्त->req.reader.राशिः = reader->अग्र;
    reader->crt->do_close(दत्त, reader);
    curlx_free(reader);
    reader = दत्त->req.reader.राशिः;
  }
}

शून्य Curl_client_cleanup(संरचना Curl_easy *दत्त)
{
  cl_reset_reader(दत्त);
  cl_reset_writer(दत्त);

  दत्त->req.bytecount = 0;
  दत्त->req.headerline = 0;
}

शून्य Curl_client_reset(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(दत्त, "client_reset, will rewind reader");
  }
  अन्यथा {
    CURL_TRC_READ(दत्त, "client_reset, clear readers");
    cl_reset_reader(दत्त);
  }
  cl_reset_writer(दत्त);

  दत्त->req.bytecount = 0;
  दत्त->req.headerline = 0;
}

CURLcode Curl_client_start(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->req.rewind_read) {
    संरचना Curl_creader *r = दत्त->req.reader.राशिः;
    CURLcode परिणाम = CURLE_OK;

    CURL_TRC_READ(दत्त, "client start, rewind readers");
    यावत्(r) {
      परिणाम = r->crt->cntrl(दत्त, r, CURL_CRCNTRL_REWIND);
      यदि(परिणाम) {
        failf(दत्त, "rewind of client reader '%s' failed: %d",
              r->crt->name, (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      r = r->अग्र;
    }
    दत्त->req.rewind_read = FALSE;
    cl_reset_reader(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

bool Curl_creader_will_rewind(संरचना Curl_easy *दत्त)
{
  प्रत्यागम (bool)दत्त->req.rewind_read;
}

शून्य Curl_creader_समुच्चयः_rewind(संरचना Curl_easy *दत्त, bool enable)
{
  दत्त->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(संरचना Curl_easy *दत्त,
                               संरचना Curl_cwriter *writer)
{
  (शून्य)दत्त;
  (शून्य)writer;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cwriter_def_write(संरचना Curl_easy *दत्त,
                                संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                ध्रुव वर्ण *buf, size_t nbytes)
{
  प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(संरचना Curl_easy *दत्त,
                                संरचना Curl_cwriter *writer)
{
  प्रत्यागम Curl_cwriter_flush(दत्त, writer->अग्र);
}

शून्य Curl_cwriter_def_close(संरचना Curl_easy *दत्त,
                            संरचना Curl_cwriter *writer)
{
  (शून्य)दत्त;
  (शून्य)writer;
}

स्थिर size_t get_max_body_write_len(संरचना Curl_easy *दत्त, curl_off_t सीमा)
{
  यदि(सीमा != -1) {
    /* How much more are we allowed to write? */
    प्रत्यागम curlx_sotouz_range(सीमा - दत्त->req.bytecount, 0, SIZE_MAX);
  }
  प्रत्यागम SIZE_MAX;
}

संरचना cw_download_ctx {
  संरचना Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
स्थिर CURLcode cw_download_write(संरचना Curl_easy *दत्त,
                                  संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                  ध्रुव वर्ण *buf, size_t nbytes)
{
  संरचना cw_download_ctx *ctx = writer->ctx;
  CURLcode परिणाम;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  यदि(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || दत्त->req.upload_समाप्तम्)) {
    Curl_pgrsTime(दत्त, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  यदि(!(type & CLIENTWRITE_BODY)) {
    यदि(is_connect && दत्त->समुच्चयः.suppress_connect_शीर्षकाणि)
      प्रत्यागम CURLE_OK;
#यदि_परिभाषितम् DEBUGBUILD
    यदि(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      प्रत्यागम CURLE_OK;
#यदि_अन्तः
    परिणाम = Curl_cwriter_write(दत्त, writer->अग्र, type, buf, nbytes);
    CURL_TRC_WRITE(दत्त, "download_write header(type=%x, blen=%zu) -> %d",
                   (अचिह्नित पूर्णाङ्क)type, nbytes, (पूर्णाङ्क)परिणाम);
    प्रत्यागम परिणाम;
  }

  यदि(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_start(&दत्त->प्रगतिः.dl.rlimit, Curl_pgrs_now(दत्त),
                      दत्त->req.आकार);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  यदि(दत्त->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(दत्त->conn);
    CURL_TRC_WRITE(दत्त, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (अचिह्नित पूर्णाङ्क)type, nbytes);
    दत्त->req.download_समाप्तम् = TRUE;
    यदि(दत्त->info.शीर्षकम्_आकार)
      /* if headers have been received, this is fine */
      प्रत्यागम CURLE_OK;
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  यदि(दत्त->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(दत्त, दत्त->req.maxdownload);
    यदि(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    यदि(nwrite == wmax) {
      दत्त->req.download_समाप्तम् = TRUE;
    }

    यदि((type & CLIENTWRITE_EOS) && !दत्त->req.no_body &&
       (दत्त->req.आकार > दत्त->req.bytecount)) {
      failf(दत्त, "end of response with %" FMT_OFF_T " bytes missing",
            दत्त->req.आकार - दत्त->req.bytecount);
      प्रत्यागम CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  यदि(दत्त->समुच्चयः.max_filesize && !दत्त->req.ignorebody) {
    size_t wmax = get_max_body_write_len(दत्त, दत्त->समुच्चयः.max_filesize);
    यदि(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  यदि(!दत्त->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    परिणाम = Curl_cwriter_write(दत्त, writer->अग्र, type, buf, nwrite);
    CURL_TRC_WRITE(दत्त, "download_write body(type=%x, blen=%zu) -> %d",
                   (अचिह्नित पूर्णाङ्क)type, nbytes, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Update stats, write and report progress */
  यदि(nwrite) {
    दत्त->req.bytecount += nwrite;
    Curl_pgrs_download_inc(दत्त, nwrite);
  }

  यदि(excess_len) {
    यदि(!दत्त->req.ignorebody) {
      infof(दत्त,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, दत्त->req.आकार, दत्त->req.maxdownload,
            दत्त->req.bytecount);
      connclose(दत्त->conn);
    }
  }
  अन्यथा यदि((nwrite < nbytes) && !दत्त->req.ignorebody) {
    failf(दत्त, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          दत्त->समुच्चयः.max_filesize, दत्त->req.bytecount);
    प्रत्यागम CURLE_FILESIZE_EXCEEDED;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव संरचना Curl_cwtype cw_download = {
  "protocol",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  परिमाण(संरचना cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
स्थिर CURLcode cw_raw_write(संरचना Curl_easy *दत्त,
                             संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                             ध्रुव वर्ण *buf, size_t nbytes)
{
  यदि(type & CLIENTWRITE_BODY && दत्त->समुच्चयः.verbose && !दत्त->req.ignorebody) {
    Curl_debug(दत्त, CURLINFO_दत्त_अन्तर्, buf, nbytes);
  }
  प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, nbytes);
}

स्थिर ध्रुव संरचना Curl_cwtype cw_raw = {
  "raw",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  परिमाण(संरचना Curl_cwriter)
};

स्थिर शून्य cwriter_सङ्कलय(संरचना Curl_easy *दत्त,
                        संरचना Curl_cwriter *writer)
{
  संरचना Curl_cwriter **anchor = &दत्त->req.writer.राशिः;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  यावत्(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->अग्र;
  writer->अग्र = *anchor;
  *anchor = writer;
}

स्थिर CURLcode do_init_writer_राशिः(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cwriter *writer;
  CURLcode परिणाम;

  DEBUGASSERT(!दत्त->req.writer.राशिः);
  परिणाम = Curl_cwriter_create(&दत्त->req.writer.राशिः,
                               दत्त, &Curl_cwt_बहिर्गमनम्, CURL_CW_CLIENT);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_cwriter_create(&writer, दत्त, &cw_download, CURL_CW_PROTOCOL);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  cwriter_सङ्कलय(दत्त, writer);

  परिणाम = Curl_cwriter_create(&writer, दत्त, &cw_raw, CURL_CW_RAW);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  cwriter_सङ्कलय(दत्त, writer);

  प्रत्यागम परिणाम;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(संरचना Curl_easy *दत्त, पूर्णाङ्क type, ध्रुव वर्ण *buf,
                           size_t len)
{
  CURLcode परिणाम;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_शीर्षकम् | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  यदि(!दत्त->req.writer.राशिः) {
    परिणाम = do_init_writer_राशिः(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    DEBUGASSERT(दत्त->req.writer.राशिः);
  }

  परिणाम = Curl_cwriter_write(दत्त, दत्त->req.writer.राशिः, type, buf, len);
  CURL_TRC_WRITE(दत्त, "client_write(type=%x, len=%zu) -> %d",
                 (अचिह्नित पूर्णाङ्क)type, len, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

CURLcode Curl_client_flush(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;

  यदि(!दत्त->req.writer.राशिः) {
    परिणाम = do_init_writer_राशिः(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    DEBUGASSERT(दत्त->req.writer.राशिः);
  }

  परिणाम = Curl_cwriter_flush(दत्त, दत्त->req.writer.राशिः);
  CURL_TRC_WRITE(दत्त, "client_flush() -> %d", (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(संरचना Curl_cwriter **pwriter,
                             संरचना Curl_easy *दत्त,
                             ध्रुव संरचना Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  संरचना Curl_cwriter *writer = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  शून्य *p;

  DEBUGASSERT(cwt->cwriter_आकार >= परिमाण(संरचना Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_आकार);
  यदि(!p)
    गच्छ बहिर्गमनम्;

  writer = (संरचना Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  परिणाम = cwt->do_init(दत्त, writer);

बहिर्गमनम्:
  *pwriter = परिणाम ? रिक्त : writer;
  यदि(परिणाम)
    curlx_free(writer);
  प्रत्यागम परिणाम;
}

शून्य Curl_cwriter_free(संरचना Curl_easy *दत्त,
                       संरचना Curl_cwriter *writer)
{
  यदि(writer) {
    writer->cwt->do_close(दत्त, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_गणनम्(संरचना Curl_easy *दत्त, Curl_cwriter_phase phase)
{
  संरचना Curl_cwriter *w;
  size_t n = 0;

  प्रति(w = दत्त->req.writer.राशिः; w; w = w->अग्र) {
    यदि(w->phase == phase)
      ++n;
  }
  प्रत्यागम n;
}

स्थिर CURLcode cwriter_ensure_pause_writer(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cwriter *writer =
    Curl_cwriter_get_by_type(दत्त, &Curl_cwt_pause);
  CURLcode परिणाम = CURLE_OK;

  यदि(!writer) {
    परिणाम = Curl_cwriter_create(&writer, दत्त, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    यदि(!परिणाम)
      cwriter_सङ्कलय(दत्त, writer);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cwriter_सङ्कलय(संरचना Curl_easy *दत्त,
                          संरचना Curl_cwriter *writer)
{
  CURLcode परिणाम;
  संरचना Curl_cwriter **anchor = &दत्त->req.writer.राशिः;

  यदि(!*anchor) {
    परिणाम = do_init_writer_राशिः(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    परिणाम = cwriter_ensure_pause_writer(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  cwriter_सङ्कलय(दत्त, writer);
  प्रत्यागम CURLE_OK;
}

संरचना Curl_cwriter *Curl_cwriter_get_by_name(संरचना Curl_easy *दत्त,
                                              ध्रुव वर्ण *name)
{
  संरचना Curl_cwriter *writer;
  प्रति(writer = दत्त->req.writer.राशिः; writer; writer = writer->अग्र) {
    यदि(!वाक्यं_तुलय(name, writer->cwt->name))
      प्रत्यागम writer;
  }
  प्रत्यागम रिक्त;
}

संरचना Curl_cwriter *Curl_cwriter_get_by_type(संरचना Curl_easy *दत्त,
                                              ध्रुव संरचना Curl_cwtype *cwt)
{
  संरचना Curl_cwriter *writer;
  प्रति(writer = दत्त->req.writer.राशिः; writer; writer = writer->अग्र) {
    यदि(writer->cwt == cwt)
      प्रत्यागम writer;
  }
  प्रत्यागम रिक्त;
}

CURLcode Curl_cwriter_unpause(संरचना Curl_easy *दत्त)
{
  दत्त->req.writer.paused = FALSE;
  प्रत्यागम Curl_cwriter_flush(दत्त, दत्त->req.writer.राशिः);
}

CURLcode Curl_creader_read(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader,
                           वर्ण *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  यदि(!reader)
    प्रत्यागम CURLE_READ_ERROR;
  प्रत्यागम reader->crt->do_read(दत्त, reader, buf, blen, nread, eos);
}

शून्य Curl_creader_clear_eos(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader)
{
  यावत्(reader) {
    (शून्य)reader->crt->cntrl(दत्त, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->अग्र;
  }
}

CURLcode Curl_creader_def_init(संरचना Curl_easy *दत्त,
                               संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_creader_def_close(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
}

CURLcode Curl_creader_def_read(संरचना Curl_easy *दत्त,
                               संरचना Curl_creader *reader,
                               वर्ण *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  यदि(reader->अग्र)
    प्रत्यागम reader->अग्र->crt->do_read(दत्त, reader->अग्र, buf, blen,
                                      nread, eos);
  अन्यथा {
    *nread = 0;
    *eos = FALSE;
    प्रत्यागम CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(संरचना Curl_easy *दत्त,
                                   संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम FALSE;
}

curl_off_t Curl_creader_def_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                         संरचना Curl_creader *reader)
{
  प्रत्यागम reader->अग्र ?
         reader->अग्र->crt->योग_दीर्घता(दत्त, reader->अग्र) : -1;
}

CURLcode Curl_creader_def_resume_from(संरचना Curl_easy *दत्त,
                                      संरचना Curl_creader *reader,
                                      curl_off_t विस्थापनम्)
{
  (शून्य)दत्त;
  (शून्य)reader;
  (शून्य)विस्थापनम्;
  प्रत्यागम CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(संरचना Curl_easy *दत्त,
                                संरचना Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (शून्य)दत्त;
  (शून्य)reader;
  (शून्य)opcode;
  प्रत्यागम CURLE_OK;
}

bool Curl_creader_def_is_paused(संरचना Curl_easy *दत्त,
                                संरचना Curl_creader *reader)
{
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम FALSE;
}

शून्य Curl_creader_def_समाप्तम्(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader, पूर्णाङ्क premature)
{
  (शून्य)दत्त;
  (शून्य)reader;
  (शून्य)premature;
}

संरचना cr_अन्तर्_ctx {
  संरचना Curl_creader super;
  curl_read_callback read_cb;
  शून्य *cb_user_दत्त;
  curl_off_t योग_len;
  curl_off_t read_len;
  CURLcode error_परिणाम;
  BIT(आलोकितम्_eos);
  BIT(errored);
  BIT(has_प्रयुक्तम्_cb);
  BIT(is_paused);
};

स्थिर CURLcode cr_अन्तर्_init(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  ctx->read_cb = दत्त->state.fread_func;
  ctx->cb_user_दत्त = दत्त->state.अन्तर्;
  ctx->योग_len = -1;
  ctx->read_len = 0;
  प्रत्यागम CURLE_OK;
}

/* Real client reader to installed client callbacks. */
स्थिर CURLcode cr_अन्तर्_read(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader,
                           वर्ण *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  यदि(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    प्रत्यागम ctx->error_परिणाम;
  }
  यदि(ctx->आलोकितम्_eos) {
    *pnread = 0;
    *peos = TRUE;
    प्रत्यागम CURLE_OK;
  }
  /* respect length limitations */
  यदि(ctx->योग_len >= 0) {
    blen = curlx_sotouz_range(ctx->योग_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  यदि(ctx->read_cb && blen) {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_cr_अन्तर्_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_दत्त);
    CURL_CBAPI_अन्तः(&guard);
    ctx->has_प्रयुक्तम्_cb = TRUE;
  }

  विकल्प(nread) {
  पक्ष 0:
    यदि((ctx->योग_len >= 0) && (ctx->read_len < ctx->योग_len)) {
      failf(दत्त, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->योग_len);
      परिणाम = CURLE_READ_ERROR;
      भङ्ग;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->आलोकितम्_eos = TRUE;
    भङ्ग;

  पक्ष CURL_READFUNC_ABORT:
    failf(दत्त, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_ABORTED_BY_CALLBACK;
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    भङ्ग;

  पक्ष CURL_READFUNC_PAUSE:
    यदि(दत्त->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(दत्त, "Read callback asked for PAUSE when not supported");
      परिणाम = CURLE_READ_ERROR;
      भङ्ग;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(दत्त, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    परिणाम = Curl_xfer_pause_send(दत्त, TRUE);
    भङ्ग; /* nothing was read */

  स्वभाव:
    यदि(nread > blen) {
      /* the read function returned a too large value */
      failf(दत्त, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_परिणाम = CURLE_READ_ERROR;
      परिणाम = CURLE_READ_ERROR;
      भङ्ग;
    }
    ctx->read_len += nread;
    यदि(ctx->योग_len >= 0)
      ctx->आलोकितम्_eos = (ctx->read_len >= ctx->योग_len);
    *pnread = nread;
    *peos = (bool)ctx->आलोकितम्_eos;
    भङ्ग;
  }
  CURL_TRC_READ(दत्त, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->योग_len, ctx->read_len, (पूर्णाङ्क)परिणाम,
                *pnread, *peos);
  प्रत्यागम परिणाम;
}

स्थिर bool cr_अन्तर्_needs_rewind(संरचना Curl_easy *दत्त,
                               संरचना Curl_creader *reader)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम (bool)ctx->has_प्रयुक्तम्_cb;
}

स्थिर curl_off_t cr_अन्तर्_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                     संरचना Curl_creader *reader)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम ctx->योग_len;
}

स्थिर CURLcode cr_अन्तर्_resume_from(संरचना Curl_easy *दत्त,
                                  संरचना Curl_creader *reader,
                                  curl_off_t विस्थापनम्)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  पूर्णाङ्क seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(दत्त->conn);
  /* already started reading? */
  यदि(ctx->read_len)
    प्रत्यागम CURLE_READ_ERROR;

  यदि(दत्त->समुच्चयः.seek_func) {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_seek_func);
    seekerr = दत्त->समुच्चयः.seek_func(दत्त->समुच्चयः.seek_client, विस्थापनम्, SEEK_SET);
    CURL_CBAPI_अन्तः(&guard);
  }

  यदि(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(दत्त, "Could not seek stream");
      प्रत्यागम CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    कुरु {
      संरचना Curl_mapi_guard guard;
      वर्ण scratch[4 * 1024];
      size_t readthisamountnow =
        (विस्थापनम् - passed > (curl_off_t)परिमाण(scratch)) ?
        परिमाण(scratch) :
        curlx_sotouz(विस्थापनम् - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, दत्त, easy_cr_अन्तर्_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_दत्त);
      CURL_CBAPI_अन्तः(&guard);

      passed += actuallyread;
      यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(दत्त, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        प्रत्यागम CURLE_READ_ERROR;
      }
    } यावत्(passed < विस्थापनम्);
  }

  /* now, decrease the size of the read */
  यदि(ctx->योग_len > 0) {
    ctx->योग_len -= विस्थापनम्;

    यदि(ctx->योग_len <= 0) {
      failf(दत्त, "File already completely uploaded");
      प्रत्यागम CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cr_अन्तर्_rewind(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  यदि(!ctx->has_प्रयुक्तम्_cb)
    प्रत्यागम CURLE_OK;

  यदि(दत्त->समुच्चयः.seek_func) {
    संरचना Curl_mapi_guard guard;
    पूर्णाङ्क err;

    CURL_CBAPI_START(&guard, दत्त, easy_seek_func);
    err = दत्त->समुच्चयः.seek_func(दत्त->समुच्चयः.seek_client, 0, SEEK_SET);
    CURL_CBAPI_अन्तः(&guard);
    CURL_TRC_READ(दत्त, "cr_in, rewind via set.seek_func -> %d", err);
    यदि(err) {
      failf(दत्त, "seek callback returned error %d", err);
      प्रत्यागम CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा यदि(दत्त->समुच्चयः.ioctl_func) {
    संरचना Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, दत्त, easy_ioctl_func);
    err = दत्त->समुच्चयः.ioctl_func(दत्त, CURLIOCMD_RESTARTREAD,
                               दत्त->समुच्चयः.ioctl_client);
    CURL_CBAPI_अन्तः(&guard);
    CURL_TRC_READ(दत्त, "cr_in, rewind via set.ioctl_func -> %d", (पूर्णाङ्क)err);
    यदि(err) {
      failf(दत्त, "ioctl callback returned error %d", (पूर्णाङ्क)err);
      प्रत्यागम CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
    यदि(दत्त->state.fread_func == (curl_read_callback)fread) {
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
      पूर्णाङ्क err = fseek(दत्त->state.अन्तर्, 0, SEEK_SET);
      CURL_TRC_READ(दत्त, "cr_in, rewind via fseek -> %d(%d)",
                    (पूर्णाङ्क)err, (पूर्णाङ्क)errno);
      यदि(err != -1)
        /* successful rewind */
        प्रत्यागम CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(दत्त, "necessary data rewind was not possible");
    प्रत्यागम CURLE_SEND_FAIL_REWIND;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cr_अन्तर्_cntrl(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;

  विकल्प(opcode) {
  पक्ष CURL_CRCNTRL_REWIND:
    प्रत्यागम cr_अन्तर्_rewind(दत्त, reader);
  पक्ष CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    भङ्ग;
  पक्ष CURL_CRCNTRL_CLEAR_EOS:
    ctx->आलोकितम्_eos = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool cr_अन्तर्_is_paused(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader)
{
  संरचना cr_अन्तर्_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम (bool)ctx->is_paused;
}

स्थिर ध्रुव संरचना Curl_crtype cr_अन्तर् = {
  "cr-in",
  cr_अन्तर्_init,
  cr_अन्तर्_read,
  Curl_creader_def_close,
  cr_अन्तर्_needs_rewind,
  cr_अन्तर्_योग_दीर्घता,
  cr_अन्तर्_resume_from,
  cr_अन्तर्_cntrl,
  cr_अन्तर्_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_अन्तर्_ctx)
};

CURLcode Curl_creader_create(संरचना Curl_creader **preader,
                             संरचना Curl_easy *दत्त,
                             ध्रुव संरचना Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  शून्य *p;

  DEBUGASSERT(crt->creader_आकार >= परिमाण(संरचना Curl_creader));
  p = curlx_calloc(1, crt->creader_आकार);
  यदि(!p)
    गच्छ बहिर्गमनम्;

  reader = (संरचना Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  परिणाम = crt->do_init(दत्त, reader);

बहिर्गमनम्:
  *preader = परिणाम ? रिक्त : reader;
  यदि(परिणाम)
    curlx_free(reader);
  प्रत्यागम परिणाम;
}

शून्य Curl_creader_free(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  यदि(reader) {
    reader->crt->do_close(दत्त, reader);
    curlx_free(reader);
  }
}

संरचना cr_lc_ctx {
  संरचना Curl_creader super;
  संरचना bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(पूर्व_cr);   /* the last byte was a CR */
};

स्थिर CURLcode cr_lc_init(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_सीमा);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_lc_close(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
स्थिर CURLcode cr_lc_read(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader,
                           वर्ण *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  CURLcode परिणाम;
  size_t nread, i, start, n;
  bool eos;

  यदि(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    प्रत्यागम CURLE_OK;
  }

  यदि(Curl_bufq_is_रिक्तानि(&ctx->buf)) {
    यदि(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      प्रत्यागम CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    परिणाम = Curl_creader_read(दत्त, reader->अग्र, buf, blen, &nread, &eos);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    ctx->read_eos = eos;

    यदि(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      यदि(nread)
        ctx->पूर्व_cr = (buf[nread - 1] == '\r');
      यदि(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      गच्छ बहिर्गमनम्;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    प्रति(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      यदि((buf[i] != '\n') || ctx->पूर्व_cr) {
        ctx->पूर्व_cr = (buf[i] == '\r');
        अनुवर्त;
      }
      ctx->पूर्व_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      यदि(!परिणाम)
        परिणाम = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      start = i + 1;
    }

    यदि(start < i) { /* leftover */
      परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_रिक्तानि(&ctx->buf));
  *peos = FALSE;
  परिणाम = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  यदि(!परिणाम && ctx->read_eos && Curl_bufq_is_रिक्तानि(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

बहिर्गमनम्:
  CURL_TRC_READ(दत्त, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णाङ्क)परिणाम, *pnread, *peos);
  प्रत्यागम परिणाम;
}

स्थिर curl_off_t cr_lc_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                     संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम -1;
}

स्थिर ध्रुव संरचना Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_lc_ctx)
};

स्थिर CURLcode cr_lc_सङ्कलय(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, दत्त, &cr_lc, CURL_CR_CONTENT_ENCODE);
  यदि(!परिणाम)
    परिणाम = Curl_creader_सङ्कलय(दत्त, reader);

  यदि(परिणाम && reader)
    Curl_creader_free(दत्त, reader);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode do_init_reader_राशिः(संरचना Curl_easy *दत्त,
                                     संरचना Curl_creader *r)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!दत्त->req.reader.राशिः);

  दत्त->req.reader.राशिः = r;
  clen = r->crt->योग_दीर्घता(दत्त, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  यदि(clen &&
#यदि_परिभाषितम् CURL_PREFER_LF_LINEENDS
    (दत्त->समुच्चयः.crlf || दत्त->state.prefer_ascii)
#अन्यथा
    दत्त->समुच्चयः.crlf
#यदि_अन्तः
    ) {
    परिणाम = cr_lc_सङ्कलय(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_creader_समुच्चयः_fread(संरचना Curl_easy *दत्त, curl_off_t len)
{
  CURLcode परिणाम;
  संरचना Curl_creader *r;
  संरचना cr_अन्तर्_ctx *ctx;

  परिणाम = Curl_creader_create(&r, दत्त, &cr_अन्तर्, CURL_CR_CLIENT);
  यदि(परिणाम || !r)
    गच्छ बहिर्गमनम्;
  ctx = r->ctx;
  ctx->योग_len = len;

  cl_reset_reader(दत्त);
  परिणाम = do_init_reader_राशिः(दत्त, r);
बहिर्गमनम्:
  CURL_TRC_READ(दत्त, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

CURLcode Curl_creader_सङ्कलय(संरचना Curl_easy *दत्त,
                          संरचना Curl_creader *reader)
{
  CURLcode परिणाम;
  संरचना Curl_creader **anchor = &दत्त->req.reader.राशिः;

  यदि(!*anchor) {
    परिणाम = Curl_creader_समुच्चयः_fread(दत्त, दत्त->state.infilesize);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  यावत्(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->अग्र;
  reader->अग्र = *anchor;
  *anchor = reader;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_creader_समुच्चयः(संरचना Curl_easy *दत्त, संरचना Curl_creader *r)
{
  CURLcode परिणाम;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(दत्त);
  परिणाम = do_init_reader_राशिः(दत्त, r);
  यदि(परिणाम)
    Curl_creader_free(दत्त, r);
  प्रत्यागम परिणाम;
}

CURLcode Curl_client_read(संरचना Curl_easy *दत्त, वर्ण *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode परिणाम;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  यदि(!दत्त->req.reader.राशिः) {
    परिणाम = Curl_creader_समुच्चयः_fread(दत्त, दत्त->state.infilesize);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    DEBUGASSERT(दत्त->req.reader.राशिः);
  }
  यदि(!दत्त->req.reader_started) {
    Curl_rlimit_start(&दत्त->प्रगतिः.ul.rlimit, Curl_pgrs_now(दत्त), -1);
    दत्त->req.reader_started = TRUE;
  }

  यदि(Curl_rlimit_सक्रिय(&दत्त->प्रगतिः.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&दत्त->प्रगतिः.ul.rlimit, रिक्त);
    यदि(ul_avail <= 0) {
      परिणाम = CURLE_OK;
      *eos = FALSE;
      गच्छ बहिर्गमनम्;
    }
    यदि(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  परिणाम = Curl_creader_read(दत्त, दत्त->req.reader.राशिः, buf, blen,
                             nread, eos);

बहिर्गमनम्:
  CURL_TRC_READ(दत्त, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णाङ्क)परिणाम, *nread, *eos);
  प्रत्यागम परिणाम;
}

bool Curl_creader_needs_rewind(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = दत्त->req.reader.राशिः;
  यावत्(reader) {
    यदि(reader->crt->needs_rewind(दत्त, reader)) {
      CURL_TRC_READ(दत्त, "client reader needs rewind before next request");
      प्रत्यागम TRUE;
    }
    reader = reader->अग्र;
  }
  प्रत्यागम FALSE;
}

स्थिर CURLcode cr_null_read(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader,
                             वर्ण *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (शून्य)दत्त;
  (शून्य)reader;
  (शून्य)buf;
  (शून्य)blen;
  *pnread = 0;
  *peos = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर curl_off_t cr_null_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                       संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम 0;
}

स्थिर ध्रुव संरचना Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना Curl_creader)
};

CURLcode Curl_creader_समुच्चयः_null(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&r, दत्त, &cr_null, CURL_CR_CLIENT);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  cl_reset_reader(दत्त);
  प्रत्यागम do_init_reader_राशिः(दत्त, r);
}

संरचना cr_buf_ctx {
  संरचना Curl_creader super;
  ध्रुव वर्ण *buf;
  size_t blen;
  size_t सूचकांक;
};

स्थिर CURLcode cr_buf_read(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader,
                            वर्ण *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->सूचकांक;

  यदि(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  अन्यथा {
    यदि(nread > blen)
      nread = blen;
    स्मृतिं_प्रतिलिख(buf, ctx->buf + ctx->सूचकांक, nread);
    *pnread = nread;
    ctx->सूचकांक += nread;
    *peos = (ctx->सूचकांक == ctx->blen);
  }
  CURL_TRC_READ(दत्त, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  प्रत्यागम CURLE_OK;
}

स्थिर bool cr_buf_needs_rewind(संरचना Curl_easy *दत्त,
                                संरचना Curl_creader *reader)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम ctx->सूचकांक > 0;
}

स्थिर CURLcode cr_buf_cntrl(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  विकल्प(opcode) {
  पक्ष CURL_CRCNTRL_REWIND:
    ctx->सूचकांक = 0;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर curl_off_t cr_buf_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                      संरचना Curl_creader *reader)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम (curl_off_t)ctx->blen;
}

स्थिर CURLcode cr_buf_resume_from(संरचना Curl_easy *दत्त,
                                   संरचना Curl_creader *reader,
                                   curl_off_t विस्थापनम्)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (शून्य)दत्त;
  DEBUGASSERT(दत्त->conn);
  /* already started reading? */
  यदि(ctx->सूचकांक)
    प्रत्यागम CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(विस्थापनम्, 0, SIZE_MAX);
  यदि(!boffset)
    प्रत्यागम CURLE_OK;
  यदि(boffset > ctx->blen)
    प्रत्यागम CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव संरचना Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_योग_दीर्घता,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_buf_ctx)
};

CURLcode Curl_creader_समुच्चयः_buf(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *buf, size_t blen)
{
  CURLcode परिणाम;
  संरचना Curl_creader *r;
  संरचना cr_buf_ctx *ctx;

  परिणाम = Curl_creader_create(&r, दत्त, &cr_buf, CURL_CR_CLIENT);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->सूचकांक = 0;

  cl_reset_reader(दत्त);
  परिणाम = do_init_reader_राशिः(दत्त, r);
बहिर्गमनम्:
  CURL_TRC_READ(दत्त, "add buf reader, len=%zu -> %d", blen, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

curl_off_t Curl_creader_योग_दीर्घता(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = दत्त->req.reader.राशिः;
  प्रत्यागम r ? r->crt->योग_दीर्घता(दत्त, r) : -1;
}

curl_off_t Curl_creader_client_दीर्घता(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = दत्त->req.reader.राशिः;
  यावत्(r && r->phase != CURL_CR_CLIENT)
    r = r->अग्र;
  प्रत्यागम r ? r->crt->योग_दीर्घता(दत्त, r) : -1;
}

CURLcode Curl_creader_unpause(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = दत्त->req.reader.राशिः;
  CURLcode परिणाम = CURLE_OK;

  यावत्(reader) {
    परिणाम = reader->crt->cntrl(दत्त, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(दत्त, "unpausing %s -> %d", reader->crt->name, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      भङ्ग;
    reader = reader->अग्र;
  }
  प्रत्यागम परिणाम;
}

bool Curl_creader_is_paused(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = दत्त->req.reader.राशिः;

  यावत्(reader) {
    यदि(reader->crt->is_paused(दत्त, reader))
      प्रत्यागम TRUE;
    reader = reader->अग्र;
  }
  प्रत्यागम FALSE;
}

शून्य Curl_creader_समाप्तम्(संरचना Curl_easy *दत्त, पूर्णाङ्क premature)
{
  संरचना Curl_creader *reader = दत्त->req.reader.राशिः;
  यावत्(reader) {
    reader->crt->समाप्तम्(दत्त, reader, premature);
    reader = reader->अग्र;
  }
}

संरचना Curl_creader *Curl_creader_get_by_type(संरचना Curl_easy *दत्त,
                                              ध्रुव संरचना Curl_crtype *crt)
{
  संरचना Curl_creader *r;
  प्रति(r = दत्त->req.reader.राशिः; r; r = r->अग्र) {
    यदि(r->crt == crt)
      प्रत्यागम r;
  }
  प्रत्यागम रिक्त;
}
