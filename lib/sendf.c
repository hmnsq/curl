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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_LINUX_TCP_H
#समावेश <linux/tcp.h>
#या_अगर defined(HAVE_NETINET_TCP_H)
#समावेश <netinet/tcp.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "transfer.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "cw-out.h"
#समावेश "cw-pause.h"
#समावेश "multiif.h"
#समावेश "progress.h"

अटल व्योम cl_reset_writer(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cwriter *writer = तथ्य->req.writer.ढेर;
  जबतक(writer) {
    तथ्य->req.writer.ढेर = writer->अग्र;
    writer->cwt->do_close(तथ्य, writer);
    curlx_free(writer);
    writer = तथ्य->req.writer.ढेर;
  }
  तथ्य->req.writer.paused = FALSE;
}

अटल व्योम cl_reset_reader(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = तथ्य->req.reader.ढेर;
  तथ्य->req.reader_started = FALSE;
  जबतक(reader) {
    तथ्य->req.reader.ढेर = reader->अग्र;
    reader->crt->do_close(तथ्य, reader);
    curlx_free(reader);
    reader = तथ्य->req.reader.ढेर;
  }
}

व्योम Curl_client_cleanup(काष्ठा Curl_easy *तथ्य)
{
  cl_reset_reader(तथ्य);
  cl_reset_writer(तथ्य);

  तथ्य->req.bytecount = 0;
  तथ्य->req.headerline = 0;
}

व्योम Curl_client_reset(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(तथ्य, "client_reset, will rewind reader");
  }
  अन्यथा {
    CURL_TRC_READ(तथ्य, "client_reset, clear readers");
    cl_reset_reader(तथ्य);
  }
  cl_reset_writer(तथ्य);

  तथ्य->req.bytecount = 0;
  तथ्य->req.headerline = 0;
}

CURLcode Curl_client_start(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->req.rewind_read) {
    काष्ठा Curl_creader *r = तथ्य->req.reader.ढेर;
    CURLcode परिणाम = CURLE_OK;

    CURL_TRC_READ(तथ्य, "client start, rewind readers");
    जबतक(r) {
      परिणाम = r->crt->cntrl(तथ्य, r, CURL_CRCNTRL_REWIND);
      अगर(परिणाम) {
        failf(तथ्य, "rewind of client reader '%s' failed: %d",
              r->crt->name, (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      r = r->अग्र;
    }
    तथ्य->req.rewind_read = FALSE;
    cl_reset_reader(तथ्य);
  }
  वापस CURLE_OK;
}

bool Curl_creader_will_rewind(काष्ठा Curl_easy *तथ्य)
{
  वापस (bool)तथ्य->req.rewind_read;
}

व्योम Curl_creader_set_rewind(काष्ठा Curl_easy *तथ्य, bool enable)
{
  तथ्य->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer)
{
  (व्योम)तथ्य;
  (व्योम)writer;
  वापस CURLE_OK;
}

CURLcode Curl_cwriter_def_write(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer)
{
  वापस Curl_cwriter_flush(तथ्य, writer->अग्र);
}

व्योम Curl_cwriter_def_close(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer)
{
  (व्योम)तथ्य;
  (व्योम)writer;
}

अटल माप_प्रकार get_max_body_write_len(काष्ठा Curl_easy *तथ्य, curl_off_t limit)
{
  अगर(limit != -1) {
    /* How much more are we allowed to write? */
    वापस curlx_sotouz_range(limit - तथ्य->req.bytecount, 0, SIZE_MAX);
  }
  वापस SIZE_MAX;
}

काष्ठा cw_download_ctx {
  काष्ठा Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
अटल CURLcode cw_download_write(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                  स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  काष्ठा cw_download_ctx *ctx = writer->ctx;
  CURLcode परिणाम;
  माप_प्रकार nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  अगर(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || तथ्य->req.upload_done)) {
    Curl_pgrsTime(तथ्य, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  अगर(!(type & CLIENTWRITE_BODY)) {
    अगर(is_connect && तथ्य->set.suppress_connect_headers)
      वापस CURLE_OK;
#अगर_घोषित DEBUGBUILD
    अगर(is_connect && दो_पर्या("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      वापस CURLE_OK;
#पूर्ण_अगर
    परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);
    CURL_TRC_WRITE(तथ्य, "download_write header(type=%x, blen=%zu) -> %d",
                   (अचिन्हित पूर्णांक)type, nbytes, (पूर्णांक)परिणाम);
    वापस परिणाम;
  }

  अगर(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_start(&तथ्य->progress.dl.rlimit, Curl_pgrs_now(तथ्य),
                      तथ्य->req.आकार);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  अगर(तथ्य->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(तथ्य->conn);
    CURL_TRC_WRITE(तथ्य, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (अचिन्हित पूर्णांक)type, nbytes);
    तथ्य->req.download_done = TRUE;
    अगर(तथ्य->info.header_आकार)
      /* if headers have been received, this is fine */
      वापस CURLE_OK;
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  अगर(तथ्य->req.maxdownload != -1) {
    माप_प्रकार wmax = get_max_body_write_len(तथ्य, तथ्य->req.maxdownload);
    अगर(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    अगर(nwrite == wmax) {
      तथ्य->req.download_done = TRUE;
    }

    अगर((type & CLIENTWRITE_EOS) && !तथ्य->req.no_body &&
       (तथ्य->req.आकार > तथ्य->req.bytecount)) {
      failf(तथ्य, "end of response with %" FMT_OFF_T " bytes missing",
            तथ्य->req.आकार - तथ्य->req.bytecount);
      वापस CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  अगर(तथ्य->set.max_filesize && !तथ्य->req.ignorebody) {
    माप_प्रकार wmax = get_max_body_write_len(तथ्य, तथ्य->set.max_filesize);
    अगर(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  अगर(!तथ्य->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nwrite);
    CURL_TRC_WRITE(तथ्य, "download_write body(type=%x, blen=%zu) -> %d",
                   (अचिन्हित पूर्णांक)type, nbytes, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Update stats, write and report progress */
  अगर(nwrite) {
    तथ्य->req.bytecount += nwrite;
    Curl_pgrs_download_inc(तथ्य, nwrite);
  }

  अगर(excess_len) {
    अगर(!तथ्य->req.ignorebody) {
      infof(तथ्य,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, तथ्य->req.आकार, तथ्य->req.maxdownload,
            तथ्य->req.bytecount);
      connclose(तथ्य->conn);
    }
  }
  अन्यथा अगर((nwrite < nbytes) && !तथ्य->req.ignorebody) {
    failf(तथ्य, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          तथ्य->set.max_filesize, तथ्य->req.bytecount);
    वापस CURLE_FILESIZE_EXCEEDED;
  }

  वापस CURLE_OK;
}

अटल स्थिर काष्ठा Curl_cwtype cw_download = {
  "protocol",
  शून्य,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  माप(काष्ठा cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
अटल CURLcode cw_raw_write(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_cwriter *writer, पूर्णांक type,
                             स्थिर अक्षर *buf, माप_प्रकार nbytes)
{
  अगर(type & CLIENTWRITE_BODY && तथ्य->set.verbose && !तथ्य->req.ignorebody) {
    Curl_debug(तथ्य, CURLINFO_तथ्य_IN, buf, nbytes);
  }
  वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, nbytes);
}

अटल स्थिर काष्ठा Curl_cwtype cw_raw = {
  "raw",
  शून्य,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  माप(काष्ठा Curl_cwriter)
};

अटल व्योम cwriter_add(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_cwriter *writer)
{
  काष्ठा Curl_cwriter **anchor = &तथ्य->req.writer.ढेर;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  जबतक(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->अग्र;
  writer->अग्र = *anchor;
  *anchor = writer;
}

अटल CURLcode do_init_writer_ढेर(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cwriter *writer;
  CURLcode परिणाम;

  DEBUGASSERT(!तथ्य->req.writer.ढेर);
  परिणाम = Curl_cwriter_create(&तथ्य->req.writer.ढेर,
                               तथ्य, &Curl_cwt_out, CURL_CW_CLIENT);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_cwriter_create(&writer, तथ्य, &cw_download, CURL_CW_PROTOCOL);
  अगर(परिणाम)
    वापस परिणाम;
  cwriter_add(तथ्य, writer);

  परिणाम = Curl_cwriter_create(&writer, तथ्य, &cw_raw, CURL_CW_RAW);
  अगर(परिणाम)
    वापस परिणाम;
  cwriter_add(तथ्य, writer);

  वापस परिणाम;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(काष्ठा Curl_easy *तथ्य, पूर्णांक type, स्थिर अक्षर *buf,
                           माप_प्रकार len)
{
  CURLcode परिणाम;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_HEADER | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  अगर(!तथ्य->req.writer.ढेर) {
    परिणाम = do_init_writer_ढेर(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    DEBUGASSERT(तथ्य->req.writer.ढेर);
  }

  परिणाम = Curl_cwriter_write(तथ्य, तथ्य->req.writer.ढेर, type, buf, len);
  CURL_TRC_WRITE(तथ्य, "client_write(type=%x, len=%zu) -> %d",
                 (अचिन्हित पूर्णांक)type, len, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

CURLcode Curl_client_flush(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;

  अगर(!तथ्य->req.writer.ढेर) {
    परिणाम = do_init_writer_ढेर(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    DEBUGASSERT(तथ्य->req.writer.ढेर);
  }

  परिणाम = Curl_cwriter_flush(तथ्य, तथ्य->req.writer.ढेर);
  CURL_TRC_WRITE(तथ्य, "client_flush() -> %d", (पूर्णांक)परिणाम);
  वापस परिणाम;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(काष्ठा Curl_cwriter **pwriter,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  काष्ठा Curl_cwriter *writer = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  व्योम *p;

  DEBUGASSERT(cwt->cwriter_आकार >= माप(काष्ठा Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_आकार);
  अगर(!p)
    जाओ out;

  writer = (काष्ठा Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  परिणाम = cwt->do_init(तथ्य, writer);

out:
  *pwriter = परिणाम ? शून्य : writer;
  अगर(परिणाम)
    curlx_free(writer);
  वापस परिणाम;
}

व्योम Curl_cwriter_free(काष्ठा Curl_easy *तथ्य,
                       काष्ठा Curl_cwriter *writer)
{
  अगर(writer) {
    writer->cwt->do_close(तथ्य, writer);
    curlx_free(writer);
  }
}

माप_प्रकार Curl_cwriter_गिनती(काष्ठा Curl_easy *तथ्य, Curl_cwriter_phase phase)
{
  काष्ठा Curl_cwriter *w;
  माप_प्रकार n = 0;

  क्रम(w = तथ्य->req.writer.ढेर; w; w = w->अग्र) {
    अगर(w->phase == phase)
      ++n;
  }
  वापस n;
}

अटल CURLcode cwriter_ensure_pause_writer(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cwriter *writer =
    Curl_cwriter_get_by_type(तथ्य, &Curl_cwt_pause);
  CURLcode परिणाम = CURLE_OK;

  अगर(!writer) {
    परिणाम = Curl_cwriter_create(&writer, तथ्य, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    अगर(!परिणाम)
      cwriter_add(तथ्य, writer);
  }
  वापस परिणाम;
}

CURLcode Curl_cwriter_add(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_cwriter *writer)
{
  CURLcode परिणाम;
  काष्ठा Curl_cwriter **anchor = &तथ्य->req.writer.ढेर;

  अगर(!*anchor) {
    परिणाम = do_init_writer_ढेर(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    परिणाम = cwriter_ensure_pause_writer(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  cwriter_add(तथ्य, writer);
  वापस CURLE_OK;
}

काष्ठा Curl_cwriter *Curl_cwriter_get_by_name(काष्ठा Curl_easy *तथ्य,
                                              स्थिर अक्षर *name)
{
  काष्ठा Curl_cwriter *writer;
  क्रम(writer = तथ्य->req.writer.ढेर; writer; writer = writer->अग्र) {
    अगर(!म_भेद(name, writer->cwt->name))
      वापस writer;
  }
  वापस शून्य;
}

काष्ठा Curl_cwriter *Curl_cwriter_get_by_type(काष्ठा Curl_easy *तथ्य,
                                              स्थिर काष्ठा Curl_cwtype *cwt)
{
  काष्ठा Curl_cwriter *writer;
  क्रम(writer = तथ्य->req.writer.ढेर; writer; writer = writer->अग्र) {
    अगर(writer->cwt == cwt)
      वापस writer;
  }
  वापस शून्य;
}

CURLcode Curl_cwriter_unpause(काष्ठा Curl_easy *तथ्य)
{
  तथ्य->req.writer.paused = FALSE;
  वापस Curl_cwriter_flush(तथ्य, तथ्य->req.writer.ढेर);
}

CURLcode Curl_creader_read(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen, माप_प्रकार *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  अगर(!reader)
    वापस CURLE_READ_ERROR;
  वापस reader->crt->do_read(तथ्य, reader, buf, blen, nread, eos);
}

व्योम Curl_creader_clear_eos(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader)
{
  जबतक(reader) {
    (व्योम)reader->crt->cntrl(तथ्य, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->अग्र;
  }
}

CURLcode Curl_creader_def_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस CURLE_OK;
}

व्योम Curl_creader_def_close(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
}

CURLcode Curl_creader_def_read(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader,
                               अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos)
{
  अगर(reader->अग्र)
    वापस reader->अग्र->crt->do_read(तथ्य, reader->अग्र, buf, blen,
                                      nread, eos);
  अन्यथा {
    *nread = 0;
    *eos = FALSE;
    वापस CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस FALSE;
}

curl_off_t Curl_creader_def_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creader *reader)
{
  वापस reader->अग्र ?
         reader->अग्र->crt->योग_लंबाई(तथ्य, reader->अग्र) : -1;
}

CURLcode Curl_creader_def_resume_from(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creader *reader,
                                      curl_off_t offset)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  (व्योम)offset;
  वापस CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  (व्योम)opcode;
  वापस CURLE_OK;
}

bool Curl_creader_def_is_paused(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस FALSE;
}

व्योम Curl_creader_def_done(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader, पूर्णांक premature)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  (व्योम)premature;
}

काष्ठा cr_in_ctx {
  काष्ठा Curl_creader super;
  curl_read_callback read_cb;
  व्योम *cb_user_तथ्य;
  curl_off_t योग_len;
  curl_off_t read_len;
  CURLcode error_परिणाम;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

अटल CURLcode cr_in_init(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = तथ्य->state.fread_func;
  ctx->cb_user_तथ्य = तथ्य->state.in;
  ctx->योग_len = -1;
  ctx->read_len = 0;
  वापस CURLE_OK;
}

/* Real client reader to installed client callbacks. */
अटल CURLcode cr_in_read(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  अगर(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    वापस ctx->error_परिणाम;
  }
  अगर(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    वापस CURLE_OK;
  }
  /* respect length limitations */
  अगर(ctx->योग_len >= 0) {
    blen = curlx_sotouz_range(ctx->योग_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  अगर(ctx->read_cb && blen) {
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_तथ्य);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  चयन(nread) {
  हाल 0:
    अगर((ctx->योग_len >= 0) && (ctx->read_len < ctx->योग_len)) {
      failf(तथ्य, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->योग_len);
      परिणाम = CURLE_READ_ERROR;
      अवरोध;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    अवरोध;

  हाल CURL_READFUNC_ABORT:
    failf(तथ्य, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_ABORTED_BY_CALLBACK;
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    अवरोध;

  हाल CURL_READFUNC_PAUSE:
    अगर(तथ्य->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(तथ्य, "Read callback asked for PAUSE when not supported");
      परिणाम = CURLE_READ_ERROR;
      अवरोध;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(तथ्य, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    परिणाम = Curl_xfer_pause_send(तथ्य, TRUE);
    अवरोध; /* nothing was read */

  शेष:
    अगर(nread > blen) {
      /* the read function returned a too large value */
      failf(तथ्य, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_परिणाम = CURLE_READ_ERROR;
      परिणाम = CURLE_READ_ERROR;
      अवरोध;
    }
    ctx->read_len += nread;
    अगर(ctx->योग_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->योग_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    अवरोध;
  }
  CURL_TRC_READ(तथ्य, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->योग_len, ctx->read_len, (पूर्णांक)परिणाम,
                *pnread, *peos);
  वापस परिणाम;
}

अटल bool cr_in_needs_rewind(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस (bool)ctx->has_used_cb;
}

अटल curl_off_t cr_in_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_creader *reader)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस ctx->योग_len;
}

अटल CURLcode cr_in_resume_from(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_creader *reader,
                                  curl_off_t offset)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  पूर्णांक seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(तथ्य->conn);
  /* already started reading? */
  अगर(ctx->read_len)
    वापस CURLE_READ_ERROR;

  अगर(तथ्य->set.seek_func) {
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_seek_func);
    seekerr = तथ्य->set.seek_func(तथ्य->set.seek_client, offset, शुरू_से);
    CURL_CBAPI_END(&guard);
  }

  अगर(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    अगर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(तथ्य, "Could not seek stream");
      वापस CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    करो {
      काष्ठा Curl_mapi_guard guard;
      अक्षर scratch[4 * 1024];
      माप_प्रकार readthisamountnow =
        (offset - passed > (curl_off_t)माप(scratch)) ?
        माप(scratch) :
        curlx_sotouz(offset - passed);
      माप_प्रकार actuallyread;

      CURL_CBAPI_START(&guard, तथ्य, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_तथ्य);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      अगर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(तथ्य, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        वापस CURLE_READ_ERROR;
      }
    } जबतक(passed < offset);
  }

  /* now, decrease the size of the read */
  अगर(ctx->योग_len > 0) {
    ctx->योग_len -= offset;

    अगर(ctx->योग_len <= 0) {
      failf(तथ्य, "File already completely uploaded");
      वापस CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  वापस CURLE_OK;
}

अटल CURLcode cr_in_rewind(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  अगर(!ctx->has_used_cb)
    वापस CURLE_OK;

  अगर(तथ्य->set.seek_func) {
    काष्ठा Curl_mapi_guard guard;
    पूर्णांक err;

    CURL_CBAPI_START(&guard, तथ्य, easy_seek_func);
    err = तथ्य->set.seek_func(तथ्य->set.seek_client, 0, शुरू_से);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(तथ्य, "cr_in, rewind via set.seek_func -> %d", err);
    अगर(err) {
      failf(तथ्य, "seek callback returned error %d", err);
      वापस CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा अगर(तथ्य->set.ioctl_func) {
    काष्ठा Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, तथ्य, easy_ioctl_func);
    err = तथ्य->set.ioctl_func(तथ्य, CURLIOCMD_RESTARTREAD,
                               तथ्य->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(तथ्य, "cr_in, rewind via set.ioctl_func -> %d", (पूर्णांक)err);
    अगर(err) {
      failf(तथ्य, "ioctl callback returned error %d", (पूर्णांक)err);
      वापस CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
    अगर(तथ्य->state.fread_func == (curl_read_callback)ख_पढ़ो) {
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
      पूर्णांक err = ख_जाओ(तथ्य->state.in, 0, शुरू_से);
      CURL_TRC_READ(तथ्य, "cr_in, rewind via fseek -> %d(%d)",
                    (पूर्णांक)err, (पूर्णांक)त्रुटि_सं);
      अगर(err != -1)
        /* successful rewind */
        वापस CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(तथ्य, "necessary data rewind was not possible");
    वापस CURLE_SEND_FAIL_REWIND;
  }
  वापस CURLE_OK;
}

अटल CURLcode cr_in_cntrl(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;

  चयन(opcode) {
  हाल CURL_CRCNTRL_REWIND:
    वापस cr_in_rewind(तथ्य, reader);
  हाल CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    अवरोध;
  हाल CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल bool cr_in_is_paused(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader)
{
  काष्ठा cr_in_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस (bool)ctx->is_paused;
}

अटल स्थिर काष्ठा Curl_crtype cr_in = {
  "cr-in",
  cr_in_init,
  cr_in_read,
  Curl_creader_def_close,
  cr_in_needs_rewind,
  cr_in_योग_लंबाई,
  cr_in_resume_from,
  cr_in_cntrl,
  cr_in_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_in_ctx)
};

CURLcode Curl_creader_create(काष्ठा Curl_creader **preader,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  काष्ठा Curl_creader *reader = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  व्योम *p;

  DEBUGASSERT(crt->creader_आकार >= माप(काष्ठा Curl_creader));
  p = curlx_calloc(1, crt->creader_आकार);
  अगर(!p)
    जाओ out;

  reader = (काष्ठा Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  परिणाम = crt->do_init(तथ्य, reader);

out:
  *preader = परिणाम ? शून्य : reader;
  अगर(परिणाम)
    curlx_free(reader);
  वापस परिणाम;
}

व्योम Curl_creader_free(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  अगर(reader) {
    reader->crt->do_close(तथ्य, reader);
    curlx_free(reader);
  }
}

काष्ठा cr_lc_ctx {
  काष्ठा Curl_creader super;
  काष्ठा bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(पूर्व_cr);   /* the last byte was a CR */
};

अटल CURLcode cr_lc_init(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  काष्ठा cr_lc_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  वापस CURLE_OK;
}

अटल व्योम cr_lc_close(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader)
{
  काष्ठा cr_lc_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
अटल CURLcode cr_lc_read(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_lc_ctx *ctx = reader->ctx;
  CURLcode परिणाम;
  माप_प्रकार nread, i, start, n;
  bool eos;

  अगर(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    वापस CURLE_OK;
  }

  अगर(Curl_bufq_is_empty(&ctx->buf)) {
    अगर(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      वापस CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    परिणाम = Curl_creader_read(तथ्य, reader->अग्र, buf, blen, &nread, &eos);
    अगर(परिणाम)
      वापस परिणाम;
    ctx->read_eos = eos;

    अगर(!nread || !स_प्रथम(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      अगर(nread)
        ctx->पूर्व_cr = (buf[nread - 1] == '\r');
      अगर(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      जाओ out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    क्रम(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      अगर((buf[i] != '\n') || ctx->पूर्व_cr) {
        ctx->पूर्व_cr = (buf[i] == '\r');
        जारी;
      }
      ctx->पूर्व_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      अगर(!परिणाम)
        परिणाम = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      अगर(परिणाम)
        वापस परिणाम;
      start = i + 1;
    }

    अगर(start < i) { /* leftover */
      परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  परिणाम = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  अगर(!परिणाम && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(तथ्य, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णांक)परिणाम, *pnread, *peos);
  वापस परिणाम;
}

अटल curl_off_t cr_lc_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस -1;
}

अटल स्थिर काष्ठा Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_lc_ctx)
};

अटल CURLcode cr_lc_add(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = शून्य;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, तथ्य, &cr_lc, CURL_CR_CONTENT_ENCODE);
  अगर(!परिणाम)
    परिणाम = Curl_creader_add(तथ्य, reader);

  अगर(परिणाम && reader)
    Curl_creader_free(तथ्य, reader);
  वापस परिणाम;
}

अटल CURLcode do_init_reader_ढेर(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_creader *r)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!तथ्य->req.reader.ढेर);

  तथ्य->req.reader.ढेर = r;
  clen = r->crt->योग_लंबाई(तथ्य, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  अगर(clen &&
#अगर_घोषित CURL_PREFER_LF_LINEENDS
    (तथ्य->set.crlf || तथ्य->state.prefer_ascii)
#अन्यथा
    तथ्य->set.crlf
#पूर्ण_अगर
    ) {
    परिणाम = cr_lc_add(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस परिणाम;
}

CURLcode Curl_creader_set_fread(काष्ठा Curl_easy *तथ्य, curl_off_t len)
{
  CURLcode परिणाम;
  काष्ठा Curl_creader *r;
  काष्ठा cr_in_ctx *ctx;

  परिणाम = Curl_creader_create(&r, तथ्य, &cr_in, CURL_CR_CLIENT);
  अगर(परिणाम || !r)
    जाओ out;
  ctx = r->ctx;
  ctx->योग_len = len;

  cl_reset_reader(तथ्य);
  परिणाम = do_init_reader_ढेर(तथ्य, r);
out:
  CURL_TRC_READ(तथ्य, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

CURLcode Curl_creader_add(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_creader *reader)
{
  CURLcode परिणाम;
  काष्ठा Curl_creader **anchor = &तथ्य->req.reader.ढेर;

  अगर(!*anchor) {
    परिणाम = Curl_creader_set_fread(तथ्य, तथ्य->state.infilesize);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  जबतक(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->अग्र;
  reader->अग्र = *anchor;
  *anchor = reader;
  वापस CURLE_OK;
}

CURLcode Curl_creader_set(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *r)
{
  CURLcode परिणाम;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(तथ्य);
  परिणाम = do_init_reader_ढेर(तथ्य, r);
  अगर(परिणाम)
    Curl_creader_free(तथ्य, r);
  वापस परिणाम;
}

CURLcode Curl_client_read(काष्ठा Curl_easy *तथ्य, अक्षर *buf, माप_प्रकार blen,
                          माप_प्रकार *nread, bool *eos)
{
  CURLcode परिणाम;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  अगर(!तथ्य->req.reader.ढेर) {
    परिणाम = Curl_creader_set_fread(तथ्य, तथ्य->state.infilesize);
    अगर(परिणाम)
      वापस परिणाम;
    DEBUGASSERT(तथ्य->req.reader.ढेर);
  }
  अगर(!तथ्य->req.reader_started) {
    Curl_rlimit_start(&तथ्य->progress.ul.rlimit, Curl_pgrs_now(तथ्य), -1);
    तथ्य->req.reader_started = TRUE;
  }

  अगर(Curl_rlimit_active(&तथ्य->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&तथ्य->progress.ul.rlimit, शून्य);
    अगर(ul_avail <= 0) {
      परिणाम = CURLE_OK;
      *eos = FALSE;
      जाओ out;
    }
    अगर(ul_avail < (curl_off_t)blen)
      blen = (माप_प्रकार)ul_avail;
  }
  परिणाम = Curl_creader_read(तथ्य, तथ्य->req.reader.ढेर, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(तथ्य, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णांक)परिणाम, *nread, *eos);
  वापस परिणाम;
}

bool Curl_creader_needs_rewind(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = तथ्य->req.reader.ढेर;
  जबतक(reader) {
    अगर(reader->crt->needs_rewind(तथ्य, reader)) {
      CURL_TRC_READ(तथ्य, "client reader needs rewind before next request");
      वापस TRUE;
    }
    reader = reader->अग्र;
  }
  वापस FALSE;
}

अटल CURLcode cr_null_read(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader,
                             अक्षर *buf, माप_प्रकार blen,
                             माप_प्रकार *pnread, bool *peos)
{
  (व्योम)तथ्य;
  (व्योम)reader;
  (व्योम)buf;
  (व्योम)blen;
  *pnread = 0;
  *peos = TRUE;
  वापस CURLE_OK;
}

अटल curl_off_t cr_null_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (व्योम)तथ्य;
  (व्योम)reader;
  वापस 0;
}

अटल स्थिर काष्ठा Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा Curl_creader)
};

CURLcode Curl_creader_set_null(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&r, तथ्य, &cr_null, CURL_CR_CLIENT);
  अगर(परिणाम)
    वापस परिणाम;

  cl_reset_reader(तथ्य);
  वापस do_init_reader_ढेर(तथ्य, r);
}

काष्ठा cr_buf_ctx {
  काष्ठा Curl_creader super;
  स्थिर अक्षर *buf;
  माप_प्रकार blen;
  माप_प्रकार सूचकांक;
};

अटल CURLcode cr_buf_read(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader,
                            अक्षर *buf, माप_प्रकार blen,
                            माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_buf_ctx *ctx = reader->ctx;
  माप_प्रकार nread = ctx->blen - ctx->सूचकांक;

  अगर(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  अन्यथा {
    अगर(nread > blen)
      nread = blen;
    स_नकल(buf, ctx->buf + ctx->सूचकांक, nread);
    *pnread = nread;
    ctx->सूचकांक += nread;
    *peos = (ctx->सूचकांक == ctx->blen);
  }
  CURL_TRC_READ(तथ्य, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  वापस CURLE_OK;
}

अटल bool cr_buf_needs_rewind(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader)
{
  काष्ठा cr_buf_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस ctx->सूचकांक > 0;
}

अटल CURLcode cr_buf_cntrl(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  काष्ठा cr_buf_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  चयन(opcode) {
  हाल CURL_CRCNTRL_REWIND:
    ctx->सूचकांक = 0;
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल curl_off_t cr_buf_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creader *reader)
{
  काष्ठा cr_buf_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस (curl_off_t)ctx->blen;
}

अटल CURLcode cr_buf_resume_from(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_creader *reader,
                                   curl_off_t offset)
{
  काष्ठा cr_buf_ctx *ctx = reader->ctx;
  माप_प्रकार boffset;

  (व्योम)तथ्य;
  DEBUGASSERT(तथ्य->conn);
  /* already started reading? */
  अगर(ctx->सूचकांक)
    वापस CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  अगर(!boffset)
    वापस CURLE_OK;
  अगर(boffset > ctx->blen)
    वापस CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  वापस CURLE_OK;
}

अटल स्थिर काष्ठा Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_योग_लंबाई,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *buf, माप_प्रकार blen)
{
  CURLcode परिणाम;
  काष्ठा Curl_creader *r;
  काष्ठा cr_buf_ctx *ctx;

  परिणाम = Curl_creader_create(&r, तथ्य, &cr_buf, CURL_CR_CLIENT);
  अगर(परिणाम)
    जाओ out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->सूचकांक = 0;

  cl_reset_reader(तथ्य);
  परिणाम = do_init_reader_ढेर(तथ्य, r);
out:
  CURL_TRC_READ(तथ्य, "add buf reader, len=%zu -> %d", blen, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

curl_off_t Curl_creader_योग_लंबाई(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = तथ्य->req.reader.ढेर;
  वापस r ? r->crt->योग_लंबाई(तथ्य, r) : -1;
}

curl_off_t Curl_creader_client_लंबाई(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = तथ्य->req.reader.ढेर;
  जबतक(r && r->phase != CURL_CR_CLIENT)
    r = r->अग्र;
  वापस r ? r->crt->योग_लंबाई(तथ्य, r) : -1;
}

CURLcode Curl_creader_unpause(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = तथ्य->req.reader.ढेर;
  CURLcode परिणाम = CURLE_OK;

  जबतक(reader) {
    परिणाम = reader->crt->cntrl(तथ्य, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(तथ्य, "unpausing %s -> %d", reader->crt->name, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      अवरोध;
    reader = reader->अग्र;
  }
  वापस परिणाम;
}

bool Curl_creader_is_paused(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = तथ्य->req.reader.ढेर;

  जबतक(reader) {
    अगर(reader->crt->is_paused(तथ्य, reader))
      वापस TRUE;
    reader = reader->अग्र;
  }
  वापस FALSE;
}

व्योम Curl_creader_done(काष्ठा Curl_easy *तथ्य, पूर्णांक premature)
{
  काष्ठा Curl_creader *reader = तथ्य->req.reader.ढेर;
  जबतक(reader) {
    reader->crt->done(तथ्य, reader, premature);
    reader = reader->अग्र;
  }
}

काष्ठा Curl_creader *Curl_creader_get_by_type(काष्ठा Curl_easy *तथ्य,
                                              स्थिर काष्ठा Curl_crtype *crt)
{
  काष्ठा Curl_creader *r;
  क्रम(r = तथ्य->req.reader.ढेर; r; r = r->अग्र) {
    अगर(r->crt == crt)
      वापस r;
  }
  वापस शून्य;
}
