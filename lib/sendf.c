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

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#किंवाजर defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#endif

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

स्थिर शून्य cl_reset_writer(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *writer = data->req.writer.चळत;
  पर्यंत(writer) {
    data->req.writer.चळत = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.चळत;
  }
  data->req.writer.paused = FALSE;
}

स्थिर शून्य cl_reset_reader(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = data->req.reader.चळत;
  data->req.reader_started = FALSE;
  पर्यंत(reader) {
    data->req.reader.चळत = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.चळत;
  }
}

शून्य Curl_client_cleanup(संरचना Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

शून्य Curl_client_reset(संरचना Curl_easy *data)
{
  जर(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  अन्यथा {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(संरचना Curl_easy *data)
{
  जर(data->req.rewind_read) {
    संरचना Curl_creader *r = data->req.reader.चळत;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    पर्यंत(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      जर(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (पूर्णांक)result);
        परत result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  परत CURLE_OK;
}

bool Curl_creader_will_rewind(संरचना Curl_easy *data)
{
  परत (bool)data->req.rewind_read;
}

शून्य Curl_creader_set_rewind(संरचना Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(संरचना Curl_easy *data,
                               संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
  परत CURLE_OK;
}

CURLcode Curl_cwriter_def_write(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer, पूर्णांक type,
                                स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer)
{
  परत Curl_cwriter_flush(data, writer->next);
}

शून्य Curl_cwriter_def_close(संरचना Curl_easy *data,
                            संरचना Curl_cwriter *writer)
{
  (शून्य)data;
  (शून्य)writer;
}

स्थिर माप_प्रकार get_max_body_write_len(संरचना Curl_easy *data, curl_off_t limit)
{
  जर(limit != -1) {
    /* How much more are we allowed to write? */
    परत curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  परत SIZE_MAX;
}

संरचना cw_download_ctx {
  संरचना Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
स्थिर CURLcode cw_download_write(संरचना Curl_easy *data,
                                  संरचना Curl_cwriter *writer, पूर्णांक type,
                                  स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  संरचना cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  माप_प्रकार nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  जर(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  जर(!(type & CLIENTWRITE_BODY)) {
    जर(is_connect && data->set.suppress_connect_headers)
      परत CURLE_OK;
#ifdef DEBUGBUILD
    जर(is_connect && दो_पर्या("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      परत CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (अचिन्हित पूर्णांक)type, nbytes, (पूर्णांक)result);
    परत result;
  }

  जर(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_start(&data->progress.dl.rlimit, Curl_pgrs_now(data),
                      data->req.size);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  जर(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (अचिन्हित पूर्णांक)type, nbytes);
    data->req.download_done = TRUE;
    जर(data->info.header_size)
      /* if headers have been received, this is fine */
      परत CURLE_OK;
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  जर(data->req.maxdownload != -1) {
    माप_प्रकार wmax = get_max_body_write_len(data, data->req.maxdownload);
    जर(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    जर(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    जर((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      परत CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  जर(data->set.max_filesize && !data->req.ignorebody) {
    माप_प्रकार wmax = get_max_body_write_len(data, data->set.max_filesize);
    जर(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  जर(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (अचिन्हित पूर्णांक)type, nbytes, (पूर्णांक)result);
    जर(result)
      परत result;
  }

  /* Update stats, write and report progress */
  जर(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  जर(excess_len) {
    जर(!data->req.ignorebody) {
      infof(data,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, data->req.size, data->req.maxdownload,
            data->req.bytecount);
      connclose(data->conn);
    }
  }
  अन्यथा जर((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    परत CURLE_FILESIZE_EXCEEDED;
  }

  परत CURLE_OK;
}

स्थिर स्थिरांक संरचना Curl_cwtype cw_download = {
  "protocol",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  आकार(संरचना cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
स्थिर CURLcode cw_raw_write(संरचना Curl_easy *data,
                             संरचना Curl_cwriter *writer, पूर्णांक type,
                             स्थिरांक अक्षर *buf, माप_प्रकार nbytes)
{
  जर(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  परत Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

स्थिर स्थिरांक संरचना Curl_cwtype cw_raw = {
  "raw",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  आकार(संरचना Curl_cwriter)
};

स्थिर शून्य cwriter_add(संरचना Curl_easy *data,
                        संरचना Curl_cwriter *writer)
{
  संरचना Curl_cwriter **anchor = &data->req.writer.चळत;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  पर्यंत(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

स्थिर CURLcode do_init_writer_चळत(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.चळत);
  result = Curl_cwriter_create(&data->req.writer.चळत,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  जर(result)
    परत result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  जर(result)
    परत result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  जर(result)
    परत result;
  cwriter_add(data, writer);

  परत result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(संरचना Curl_easy *data, पूर्णांक type, स्थिरांक अक्षर *buf,
                           माप_प्रकार len)
{
  CURLcode result;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_HEADER | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  जर(!data->req.writer.चळत) {
    result = do_init_writer_चळत(data);
    जर(result)
      परत result;
    DEBUGASSERT(data->req.writer.चळत);
  }

  result = Curl_cwriter_write(data, data->req.writer.चळत, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (अचिन्हित पूर्णांक)type, len, (पूर्णांक)result);
  परत result;
}

CURLcode Curl_client_flush(संरचना Curl_easy *data)
{
  CURLcode result;

  जर(!data->req.writer.चळत) {
    result = do_init_writer_चळत(data);
    जर(result)
      परत result;
    DEBUGASSERT(data->req.writer.चळत);
  }

  result = Curl_cwriter_flush(data, data->req.writer.चळत);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (पूर्णांक)result);
  परत result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(संरचना Curl_cwriter **pwriter,
                             संरचना Curl_easy *data,
                             स्थिरांक संरचना Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  संरचना Curl_cwriter *writer = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  शून्य *p;

  DEBUGASSERT(cwt->cwriter_size >= आकार(संरचना Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  जर(!p)
    जा out;

  writer = (संरचना Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? रिक्त : writer;
  जर(result)
    curlx_free(writer);
  परत result;
}

शून्य Curl_cwriter_free(संरचना Curl_easy *data,
                       संरचना Curl_cwriter *writer)
{
  जर(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

माप_प्रकार Curl_cwriter_मोजणी(संरचना Curl_easy *data, Curl_cwriter_phase phase)
{
  संरचना Curl_cwriter *w;
  माप_प्रकार n = 0;

  साठी(w = data->req.writer.चळत; w; w = w->next) {
    जर(w->phase == phase)
      ++n;
  }
  परत n;
}

स्थिर CURLcode cwriter_ensure_pause_writer(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  जर(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    जर(!result)
      cwriter_add(data, writer);
  }
  परत result;
}

CURLcode Curl_cwriter_add(संरचना Curl_easy *data,
                          संरचना Curl_cwriter *writer)
{
  CURLcode result;
  संरचना Curl_cwriter **anchor = &data->req.writer.चळत;

  जर(!*anchor) {
    result = do_init_writer_चळत(data);
    जर(result)
      परत result;
  }

  जर(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    जर(result)
      परत result;
  }

  cwriter_add(data, writer);
  परत CURLE_OK;
}

संरचना Curl_cwriter *Curl_cwriter_get_by_name(संरचना Curl_easy *data,
                                              स्थिरांक अक्षर *name)
{
  संरचना Curl_cwriter *writer;
  साठी(writer = data->req.writer.चळत; writer; writer = writer->next) {
    जर(!म_भेद(name, writer->cwt->name))
      परत writer;
  }
  परत रिक्त;
}

संरचना Curl_cwriter *Curl_cwriter_get_by_type(संरचना Curl_easy *data,
                                              स्थिरांक संरचना Curl_cwtype *cwt)
{
  संरचना Curl_cwriter *writer;
  साठी(writer = data->req.writer.चळत; writer; writer = writer->next) {
    जर(writer->cwt == cwt)
      परत writer;
  }
  परत रिक्त;
}

CURLcode Curl_cwriter_unpause(संरचना Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  परत Curl_cwriter_flush(data, data->req.writer.चळत);
}

CURLcode Curl_creader_read(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen, माप_प्रकार *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  जर(!reader)
    परत CURLE_READ_ERROR;
  परत reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

शून्य Curl_creader_clear_eos(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader)
{
  पर्यंत(reader) {
    (शून्य)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
  परत CURLE_OK;
}

शून्य Curl_creader_def_close(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
}

CURLcode Curl_creader_def_read(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader,
                               अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos)
{
  जर(reader->next)
    परत reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  अन्यथा {
    *nread = 0;
    *eos = FALSE;
    परत CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(संरचना Curl_easy *data,
                                   संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
  परत FALSE;
}

curl_off_t Curl_creader_def_total_length(संरचना Curl_easy *data,
                                         संरचना Curl_creader *reader)
{
  परत reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(संरचना Curl_easy *data,
                                      संरचना Curl_creader *reader,
                                      curl_off_t offset)
{
  (शून्य)data;
  (शून्य)reader;
  (शून्य)offset;
  परत CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (शून्य)data;
  (शून्य)reader;
  (शून्य)opcode;
  परत CURLE_OK;
}

bool Curl_creader_def_is_paused(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader)
{
  (शून्य)data;
  (शून्य)reader;
  परत FALSE;
}

शून्य Curl_creader_def_done(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader, पूर्णांक premature)
{
  (शून्य)data;
  (शून्य)reader;
  (शून्य)premature;
}

संरचना cr_in_ctx {
  संरचना Curl_creader super;
  curl_read_callback read_cb;
  शून्य *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

स्थिर CURLcode cr_in_init(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  परत CURLE_OK;
}

/* Real client reader to installed client callbacks. */
स्थिर CURLcode cr_in_read(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  जर(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    परत ctx->error_result;
  }
  जर(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    परत CURLE_OK;
  }
  /* respect length limitations */
  जर(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  जर(ctx->read_cb && blen) {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  निवड(nread) {
  स्थिती 0:
    जर((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      थांबा;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    थांबा;

  स्थिती CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    थांबा;

  स्थिती CURL_READFUNC_PAUSE:
    जर(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      थांबा;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    थांबा; /* nothing was read */

  पूर्वनिर्धारित:
    जर(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      थांबा;
    }
    ctx->read_len += nread;
    जर(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    थांबा;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (पूर्णांक)result,
                *pnread, *peos);
  परत result;
}

स्थिर bool cr_in_needs_rewind(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत (bool)ctx->has_used_cb;
}

स्थिर curl_off_t cr_in_total_length(संरचना Curl_easy *data,
                                     संरचना Curl_creader *reader)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत ctx->total_len;
}

स्थिर CURLcode cr_in_resume_from(संरचना Curl_easy *data,
                                  संरचना Curl_creader *reader,
                                  curl_off_t offset)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  पूर्णांक seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  जर(ctx->read_len)
    परत CURLE_READ_ERROR;

  जर(data->set.seek_func) {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, शुरू_से);
    CURL_CBAPI_END(&guard);
  }

  जर(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    जर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      परत CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    करा {
      संरचना Curl_mapi_guard guard;
      अक्षर scratch[4 * 1024];
      माप_प्रकार readthisamountnow =
        (offset - passed > (curl_off_t)आकार(scratch)) ?
        आकार(scratch) :
        curlx_sotouz(offset - passed);
      माप_प्रकार actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      जर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        परत CURLE_READ_ERROR;
      }
    } पर्यंत(passed < offset);
  }

  /* now, decrease the size of the read */
  जर(ctx->total_len > 0) {
    ctx->total_len -= offset;

    जर(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      परत CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  परत CURLE_OK;
}

स्थिर CURLcode cr_in_rewind(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader)
{
  संरचना cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  जर(!ctx->has_used_cb)
    परत CURLE_OK;

  जर(data->set.seek_func) {
    संरचना Curl_mapi_guard guard;
    पूर्णांक err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, शुरू_से);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    जर(err) {
      failf(data, "seek callback returned error %d", err);
      परत CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा जर(data->set.ioctl_func) {
    संरचना Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (पूर्णांक)err);
    जर(err) {
      failf(data, "ioctl callback returned error %d", (पूर्णांक)err);
      परत CURLE_SEND_FAIL_REWIND;
    }
  }
  अन्यथा {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    जर(data->state.fread_func == (curl_read_callback)ख_पढ़ो) {
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic काढा
#endif
      पूर्णांक err = ख_जाओ(data->state.in, 0, शुरू_से);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (पूर्णांक)err, (पूर्णांक)त्रुटि_सं);
      जर(err != -1)
        /* successful rewind */
        परत CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    परत CURLE_SEND_FAIL_REWIND;
  }
  परत CURLE_OK;
}

स्थिर CURLcode cr_in_cntrl(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  संरचना cr_in_ctx *ctx = reader->ctx;

  निवड(opcode) {
  स्थिती CURL_CRCNTRL_REWIND:
    परत cr_in_rewind(data, reader);
  स्थिती CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    थांबा;
  स्थिती CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत CURLE_OK;
}

स्थिर bool cr_in_is_paused(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader)
{
  संरचना cr_in_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत (bool)ctx->is_paused;
}

स्थिर स्थिरांक संरचना Curl_crtype cr_in = {
  "cr-in",
  cr_in_init,
  cr_in_read,
  Curl_creader_def_close,
  cr_in_needs_rewind,
  cr_in_total_length,
  cr_in_resume_from,
  cr_in_cntrl,
  cr_in_is_paused,
  Curl_creader_def_done,
  आकार(संरचना cr_in_ctx)
};

CURLcode Curl_creader_create(संरचना Curl_creader **preader,
                             संरचना Curl_easy *data,
                             स्थिरांक संरचना Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  शून्य *p;

  DEBUGASSERT(crt->creader_size >= आकार(संरचना Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  जर(!p)
    जा out;

  reader = (संरचना Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? रिक्त : reader;
  जर(result)
    curlx_free(reader);
  परत result;
}

शून्य Curl_creader_free(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  जर(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

संरचना cr_lc_ctx {
  संरचना Curl_creader super;
  संरचना bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

स्थिर CURLcode cr_lc_init(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  परत CURLE_OK;
}

स्थिर शून्य cr_lc_close(संरचना Curl_easy *data, संरचना Curl_creader *reader)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
स्थिर CURLcode cr_lc_read(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  माप_प्रकार nread, i, start, n;
  bool eos;

  जर(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    परत CURLE_OK;
  }

  जर(Curl_bufq_is_empty(&ctx->buf)) {
    जर(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      परत CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    जर(result)
      परत result;
    ctx->read_eos = eos;

    जर(!nread || !स_प्रथम(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      जर(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      जर(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      जा out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    साठी(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      जर((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        सुरूठेवा;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      जर(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      जर(result)
        परत result;
      start = i + 1;
    }

    जर(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      जर(result)
        परत result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  जर(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णांक)result, *pnread, *peos);
  परत result;
}

स्थिर curl_off_t cr_lc_total_length(संरचना Curl_easy *data,
                                     संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  परत -1;
}

स्थिर स्थिरांक संरचना Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  आकार(संरचना cr_lc_ctx)
};

स्थिर CURLcode cr_lc_add(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  जर(!result)
    result = Curl_creader_add(data, reader);

  जर(result && reader)
    Curl_creader_free(data, reader);
  परत result;
}

स्थिर CURLcode do_init_reader_चळत(संरचना Curl_easy *data,
                                     संरचना Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.चळत);

  data->req.reader.चळत = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  जर(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#अन्यथा
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    जर(result)
      परत result;
  }

  परत result;
}

CURLcode Curl_creader_set_fread(संरचना Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  संरचना Curl_creader *r;
  संरचना cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  जर(result || !r)
    जा out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_चळत(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (पूर्णांक)result);
  परत result;
}

CURLcode Curl_creader_add(संरचना Curl_easy *data,
                          संरचना Curl_creader *reader)
{
  CURLcode result;
  संरचना Curl_creader **anchor = &data->req.reader.चळत;

  जर(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    जर(result)
      परत result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  पर्यंत(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  परत CURLE_OK;
}

CURLcode Curl_creader_set(संरचना Curl_easy *data, संरचना Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_चळत(data, r);
  जर(result)
    Curl_creader_free(data, r);
  परत result;
}

CURLcode Curl_client_read(संरचना Curl_easy *data, अक्षर *buf, माप_प्रकार blen,
                          माप_प्रकार *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  जर(!data->req.reader.चळत) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    जर(result)
      परत result;
    DEBUGASSERT(data->req.reader.चळत);
  }
  जर(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  जर(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, रिक्त);
    जर(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      जा out;
    }
    जर(ul_avail < (curl_off_t)blen)
      blen = (माप_प्रकार)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.चळत, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (पूर्णांक)result, *nread, *eos);
  परत result;
}

bool Curl_creader_needs_rewind(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = data->req.reader.चळत;
  पर्यंत(reader) {
    जर(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      परत TRUE;
    }
    reader = reader->next;
  }
  परत FALSE;
}

स्थिर CURLcode cr_null_read(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader,
                             अक्षर *buf, माप_प्रकार blen,
                             माप_प्रकार *pnread, bool *peos)
{
  (शून्य)data;
  (शून्य)reader;
  (शून्य)buf;
  (शून्य)blen;
  *pnread = 0;
  *peos = TRUE;
  परत CURLE_OK;
}

स्थिर curl_off_t cr_null_total_length(संरचना Curl_easy *data,
                                       संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)data;
  (शून्य)reader;
  परत 0;
}

स्थिर स्थिरांक संरचना Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  आकार(संरचना Curl_creader)
};

CURLcode Curl_creader_set_null(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  जर(result)
    परत result;

  cl_reset_reader(data);
  परत do_init_reader_चळत(data, r);
}

संरचना cr_buf_ctx {
  संरचना Curl_creader super;
  स्थिरांक अक्षर *buf;
  माप_प्रकार blen;
  माप_प्रकार index;
};

स्थिर CURLcode cr_buf_read(संरचना Curl_easy *data,
                            संरचना Curl_creader *reader,
                            अक्षर *buf, माप_प्रकार blen,
                            माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  माप_प्रकार nread = ctx->blen - ctx->index;

  जर(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  अन्यथा {
    जर(nread > blen)
      nread = blen;
    स_नकल(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  परत CURLE_OK;
}

स्थिर bool cr_buf_needs_rewind(संरचना Curl_easy *data,
                                संरचना Curl_creader *reader)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत ctx->index > 0;
}

स्थिर CURLcode cr_buf_cntrl(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)data;
  निवड(opcode) {
  स्थिती CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत CURLE_OK;
}

स्थिर curl_off_t cr_buf_total_length(संरचना Curl_easy *data,
                                      संरचना Curl_creader *reader)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत (curl_off_t)ctx->blen;
}

स्थिर CURLcode cr_buf_resume_from(संरचना Curl_easy *data,
                                   संरचना Curl_creader *reader,
                                   curl_off_t offset)
{
  संरचना cr_buf_ctx *ctx = reader->ctx;
  माप_प्रकार boffset;

  (शून्य)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  जर(ctx->index)
    परत CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  जर(!boffset)
    परत CURLE_OK;
  जर(boffset > ctx->blen)
    परत CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  परत CURLE_OK;
}

स्थिर स्थिरांक संरचना Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_total_length,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  आकार(संरचना cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *buf, माप_प्रकार blen)
{
  CURLcode result;
  संरचना Curl_creader *r;
  संरचना cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  जर(result)
    जा out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_चळत(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (पूर्णांक)result);
  परत result;
}

curl_off_t Curl_creader_total_length(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = data->req.reader.चळत;
  परत r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = data->req.reader.चळत;
  पर्यंत(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  परत r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = data->req.reader.चळत;
  CURLcode result = CURLE_OK;

  पर्यंत(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (पूर्णांक)result);
    जर(result)
      थांबा;
    reader = reader->next;
  }
  परत result;
}

bool Curl_creader_is_paused(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = data->req.reader.चळत;

  पर्यंत(reader) {
    जर(reader->crt->is_paused(data, reader))
      परत TRUE;
    reader = reader->next;
  }
  परत FALSE;
}

शून्य Curl_creader_done(संरचना Curl_easy *data, पूर्णांक premature)
{
  संरचना Curl_creader *reader = data->req.reader.चळत;
  पर्यंत(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

संरचना Curl_creader *Curl_creader_get_by_type(संरचना Curl_easy *data,
                                              स्थिरांक संरचना Curl_crtype *crt)
{
  संरचना Curl_creader *r;
  साठी(r = data->req.reader.चळत; r; r = r->next) {
    जर(r->crt == crt)
      परत r;
  }
  परत रिक्त;
}
