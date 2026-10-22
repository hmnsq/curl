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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنهاگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#endif

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

ایستا تهی cl_reset_writer(ساختار Curl_easy *data)
{
  ساختار Curl_cwriter *writer = data->req.writer.پشته;
  وقتی(writer) {
    data->req.writer.پشته = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.پشته;
  }
  data->req.writer.paused = FALSE;
}

ایستا تهی cl_reset_reader(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = data->req.reader.پشته;
  data->req.reader_started = FALSE;
  وقتی(reader) {
    data->req.reader.پشته = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.پشته;
  }
}

تهی Curl_client_cleanup(ساختار Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

تهی Curl_client_reset(ساختار Curl_easy *data)
{
  اگر(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  وگرنه {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(ساختار Curl_easy *data)
{
  اگر(data->req.rewind_read) {
    ساختار Curl_creader *r = data->req.reader.پشته;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    وقتی(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      اگر(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحیح)result);
        بازگشت result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  بازگشت CURLE_OK;
}

bool Curl_creader_will_rewind(ساختار Curl_easy *data)
{
  بازگشت (bool)data->req.rewind_read;
}

تهی Curl_creader_set_rewind(ساختار Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ساختار Curl_easy *data,
                               ساختار Curl_cwriter *writer)
{
  (تهی)data;
  (تهی)writer;
  بازگشت CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer, صحیح type,
                                ثابت نویسه *buf, size_t nbytes)
{
  بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer)
{
  بازگشت Curl_cwriter_flush(data, writer->next);
}

تهی Curl_cwriter_def_close(ساختار Curl_easy *data,
                            ساختار Curl_cwriter *writer)
{
  (تهی)data;
  (تهی)writer;
}

ایستا size_t get_max_body_write_len(ساختار Curl_easy *data, curl_off_t limit)
{
  اگر(limit != -1) {
    /* How much more are we allowed to write? */
    بازگشت curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  بازگشت SIZE_MAX;
}

ساختار cw_download_ctx {
  ساختار Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ایستا CURLcode cw_download_write(ساختار Curl_easy *data,
                                  ساختار Curl_cwriter *writer, صحیح type,
                                  ثابت نویسه *buf, size_t nbytes)
{
  ساختار cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  اگر(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  اگر(!(type & CLIENTWRITE_BODY)) {
    اگر(is_connect && data->set.suppress_connect_headers)
      بازگشت CURLE_OK;
#ifdef DEBUGBUILD
    اگر(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      بازگشت CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned صحیح)type, nbytes, (صحیح)result);
    بازگشت result;
  }

  اگر(!ctx->started_body &&
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

  اگر(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned صحیح)type, nbytes);
    data->req.download_done = TRUE;
    اگر(data->info.header_size)
      /* if headers have been received, this is fine */
      بازگشت CURLE_OK;
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  اگر(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    اگر(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    اگر(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    اگر((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      بازگشت CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  اگر(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    اگر(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  اگر(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned صحیح)type, nbytes, (صحیح)result);
    اگر(result)
      بازگشت result;
  }

  /* Update stats, write and report progress */
  اگر(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  اگر(excess_len) {
    اگر(!data->req.ignorebody) {
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
  وگرنه اگر((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    بازگشت CURLE_FILESIZE_EXCEEDED;
  }

  بازگشت CURLE_OK;
}

ایستا ثابت ساختار Curl_cwtype cw_download = {
  "protocol",
  پوچ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  اندازه(ساختار cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ایستا CURLcode cw_raw_write(ساختار Curl_easy *data,
                             ساختار Curl_cwriter *writer, صحیح type,
                             ثابت نویسه *buf, size_t nbytes)
{
  اگر(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  بازگشت Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

ایستا ثابت ساختار Curl_cwtype cw_raw = {
  "raw",
  پوچ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  اندازه(ساختار Curl_cwriter)
};

ایستا تهی cwriter_add(ساختار Curl_easy *data,
                        ساختار Curl_cwriter *writer)
{
  ساختار Curl_cwriter **anchor = &data->req.writer.پشته;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  وقتی(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

ایستا CURLcode do_init_writer_پشته(ساختار Curl_easy *data)
{
  ساختار Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.پشته);
  result = Curl_cwriter_create(&data->req.writer.پشته,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  اگر(result)
    بازگشت result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  اگر(result)
    بازگشت result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  اگر(result)
    بازگشت result;
  cwriter_add(data, writer);

  بازگشت result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ساختار Curl_easy *data, صحیح type, ثابت نویسه *buf,
                           size_t len)
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

  اگر(!data->req.writer.پشته) {
    result = do_init_writer_پشته(data);
    اگر(result)
      بازگشت result;
    DEBUGASSERT(data->req.writer.پشته);
  }

  result = Curl_cwriter_write(data, data->req.writer.پشته, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned صحیح)type, len, (صحیح)result);
  بازگشت result;
}

CURLcode Curl_client_flush(ساختار Curl_easy *data)
{
  CURLcode result;

  اگر(!data->req.writer.پشته) {
    result = do_init_writer_پشته(data);
    اگر(result)
      بازگشت result;
    DEBUGASSERT(data->req.writer.پشته);
  }

  result = Curl_cwriter_flush(data, data->req.writer.پشته);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (صحیح)result);
  بازگشت result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ساختار Curl_cwriter **pwriter,
                             ساختار Curl_easy *data,
                             ثابت ساختار Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ساختار Curl_cwriter *writer = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  تهی *p;

  DEBUGASSERT(cwt->cwriter_size >= اندازه(ساختار Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  اگر(!p)
    برو out;

  writer = (ساختار Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? پوچ : writer;
  اگر(result)
    curlx_free(writer);
  بازگشت result;
}

تهی Curl_cwriter_free(ساختار Curl_easy *data,
                       ساختار Curl_cwriter *writer)
{
  اگر(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_شمار(ساختار Curl_easy *data, Curl_cwriter_phase phase)
{
  ساختار Curl_cwriter *w;
  size_t n = 0;

  برای(w = data->req.writer.پشته; w; w = w->next) {
    اگر(w->phase == phase)
      ++n;
  }
  بازگشت n;
}

ایستا CURLcode cwriter_ensure_pause_writer(ساختار Curl_easy *data)
{
  ساختار Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  اگر(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    اگر(!result)
      cwriter_add(data, writer);
  }
  بازگشت result;
}

CURLcode Curl_cwriter_add(ساختار Curl_easy *data,
                          ساختار Curl_cwriter *writer)
{
  CURLcode result;
  ساختار Curl_cwriter **anchor = &data->req.writer.پشته;

  اگر(!*anchor) {
    result = do_init_writer_پشته(data);
    اگر(result)
      بازگشت result;
  }

  اگر(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    اگر(result)
      بازگشت result;
  }

  cwriter_add(data, writer);
  بازگشت CURLE_OK;
}

ساختار Curl_cwriter *Curl_cwriter_get_by_name(ساختار Curl_easy *data,
                                              ثابت نویسه *name)
{
  ساختار Curl_cwriter *writer;
  برای(writer = data->req.writer.پشته; writer; writer = writer->next) {
    اگر(!strcmp(name, writer->cwt->name))
      بازگشت writer;
  }
  بازگشت پوچ;
}

ساختار Curl_cwriter *Curl_cwriter_get_by_type(ساختار Curl_easy *data,
                                              ثابت ساختار Curl_cwtype *cwt)
{
  ساختار Curl_cwriter *writer;
  برای(writer = data->req.writer.پشته; writer; writer = writer->next) {
    اگر(writer->cwt == cwt)
      بازگشت writer;
  }
  بازگشت پوچ;
}

CURLcode Curl_cwriter_unpause(ساختار Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  بازگشت Curl_cwriter_flush(data, data->req.writer.پشته);
}

CURLcode Curl_creader_read(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader,
                           نویسه *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  اگر(!reader)
    بازگشت CURLE_READ_ERROR;
  بازگشت reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

تهی Curl_creader_clear_eos(ساختار Curl_easy *data,
                            ساختار Curl_creader *reader)
{
  وقتی(reader) {
    (تهی)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(ساختار Curl_easy *data,
                               ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
  بازگشت CURLE_OK;
}

تهی Curl_creader_def_close(ساختار Curl_easy *data,
                            ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
}

CURLcode Curl_creader_def_read(ساختار Curl_easy *data,
                               ساختار Curl_creader *reader,
                               نویسه *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  اگر(reader->next)
    بازگشت reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  وگرنه {
    *nread = 0;
    *eos = FALSE;
    بازگشت CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ساختار Curl_easy *data,
                                   ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
  بازگشت FALSE;
}

curl_off_t Curl_creader_def_total_length(ساختار Curl_easy *data,
                                         ساختار Curl_creader *reader)
{
  بازگشت reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(ساختار Curl_easy *data,
                                      ساختار Curl_creader *reader,
                                      curl_off_t offset)
{
  (تهی)data;
  (تهی)reader;
  (تهی)offset;
  بازگشت CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ساختار Curl_easy *data,
                                ساختار Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (تهی)data;
  (تهی)reader;
  (تهی)opcode;
  بازگشت CURLE_OK;
}

bool Curl_creader_def_is_paused(ساختار Curl_easy *data,
                                ساختار Curl_creader *reader)
{
  (تهی)data;
  (تهی)reader;
  بازگشت FALSE;
}

تهی Curl_creader_def_done(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader, صحیح premature)
{
  (تهی)data;
  (تهی)reader;
  (تهی)premature;
}

ساختار cr_in_ctx {
  ساختار Curl_creader super;
  curl_read_callback read_cb;
  تهی *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

ایستا CURLcode cr_in_init(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  بازگشت CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ایستا CURLcode cr_in_read(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader,
                           نویسه *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  اگر(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    بازگشت ctx->error_result;
  }
  اگر(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    بازگشت CURLE_OK;
  }
  /* respect length limitations */
  اگر(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  اگر(ctx->read_cb && blen) {
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  انتخاب(nread) {
  حالت 0:
    اگر((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      بشکن;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    بشکن;

  حالت CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    بشکن;

  حالت CURL_READFUNC_PAUSE:
    اگر(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      بشکن;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    بشکن; /* nothing was read */

  پیشفرض:
    اگر(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      بشکن;
    }
    ctx->read_len += nread;
    اگر(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    بشکن;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (صحیح)result,
                *pnread, *peos);
  بازگشت result;
}

ایستا bool cr_in_needs_rewind(ساختار Curl_easy *data,
                               ساختار Curl_creader *reader)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت (bool)ctx->has_used_cb;
}

ایستا curl_off_t cr_in_total_length(ساختار Curl_easy *data,
                                     ساختار Curl_creader *reader)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت ctx->total_len;
}

ایستا CURLcode cr_in_resume_from(ساختار Curl_easy *data,
                                  ساختار Curl_creader *reader,
                                  curl_off_t offset)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  صحیح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  اگر(ctx->read_len)
    بازگشت CURLE_READ_ERROR;

  اگر(data->set.seek_func) {
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  اگر(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      بازگشت CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    انجامبده {
      ساختار Curl_mapi_guard guard;
      نویسه scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)اندازه(scratch)) ?
        اندازه(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        بازگشت CURLE_READ_ERROR;
      }
    } وقتی(passed < offset);
  }

  /* now, decrease the size of the read */
  اگر(ctx->total_len > 0) {
    ctx->total_len -= offset;

    اگر(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      بازگشت CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  بازگشت CURLE_OK;
}

ایستا CURLcode cr_in_rewind(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader)
{
  ساختار cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  اگر(!ctx->has_used_cb)
    بازگشت CURLE_OK;

  اگر(data->set.seek_func) {
    ساختار Curl_mapi_guard guard;
    صحیح err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    اگر(err) {
      failf(data, "seek callback returned error %d", err);
      بازگشت CURLE_SEND_FAIL_REWIND;
    }
  }
  وگرنه اگر(data->set.ioctl_func) {
    ساختار Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (صحیح)err);
    اگر(err) {
      failf(data, "ioctl callback returned error %d", (صحیح)err);
      بازگشت CURLE_SEND_FAIL_REWIND;
    }
  }
  وگرنه {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    اگر(data->state.fread_func == (curl_read_callback)fread) {
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
      صحیح err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (صحیح)err, (صحیح)errno);
      اگر(err != -1)
        /* successful rewind */
        بازگشت CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    بازگشت CURLE_SEND_FAIL_REWIND;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cr_in_cntrl(ساختار Curl_easy *data,
                            ساختار Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ساختار cr_in_ctx *ctx = reader->ctx;

  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND:
    بازگشت cr_in_rewind(data, reader);
  حالت CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    بشکن;
  حالت CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا bool cr_in_is_paused(ساختار Curl_easy *data,
                            ساختار Curl_creader *reader)
{
  ساختار cr_in_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت (bool)ctx->is_paused;
}

ایستا ثابت ساختار Curl_crtype cr_in = {
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
  اندازه(ساختار cr_in_ctx)
};

CURLcode Curl_creader_create(ساختار Curl_creader **preader,
                             ساختار Curl_easy *data,
                             ثابت ساختار Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ساختار Curl_creader *reader = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  تهی *p;

  DEBUGASSERT(crt->creader_size >= اندازه(ساختار Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  اگر(!p)
    برو out;

  reader = (ساختار Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? پوچ : reader;
  اگر(result)
    curlx_free(reader);
  بازگشت result;
}

تهی Curl_creader_free(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  اگر(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

ساختار cr_lc_ctx {
  ساختار Curl_creader super;
  ساختار bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

ایستا CURLcode cr_lc_init(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  ساختار cr_lc_ctx *ctx = reader->ctx;
  (تهی)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  بازگشت CURLE_OK;
}

ایستا تهی cr_lc_close(ساختار Curl_easy *data, ساختار Curl_creader *reader)
{
  ساختار cr_lc_ctx *ctx = reader->ctx;
  (تهی)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ایستا CURLcode cr_lc_read(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader,
                           نویسه *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ساختار cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  اگر(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    بازگشت CURLE_OK;
  }

  اگر(Curl_bufq_is_empty(&ctx->buf)) {
    اگر(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      بازگشت CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    اگر(result)
      بازگشت result;
    ctx->read_eos = eos;

    اگر(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      اگر(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      اگر(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      برو out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    برای(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      اگر((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ادامه;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      اگر(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      اگر(result)
        بازگشت result;
      start = i + 1;
    }

    اگر(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      اگر(result)
        بازگشت result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  اگر(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)result, *pnread, *peos);
  بازگشت result;
}

ایستا curl_off_t cr_lc_total_length(ساختار Curl_easy *data,
                                     ساختار Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (تهی)data;
  (تهی)reader;
  بازگشت -1;
}

ایستا ثابت ساختار Curl_crtype cr_lc = {
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
  اندازه(ساختار cr_lc_ctx)
};

ایستا CURLcode cr_lc_add(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = پوچ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  اگر(!result)
    result = Curl_creader_add(data, reader);

  اگر(result && reader)
    Curl_creader_free(data, reader);
  بازگشت result;
}

ایستا CURLcode do_init_reader_پشته(ساختار Curl_easy *data,
                                     ساختار Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.پشته);

  data->req.reader.پشته = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  اگر(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#وگرنه
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    اگر(result)
      بازگشت result;
  }

  بازگشت result;
}

CURLcode Curl_creader_set_fread(ساختار Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  ساختار Curl_creader *r;
  ساختار cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  اگر(result || !r)
    برو out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_پشته(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحیح)result);
  بازگشت result;
}

CURLcode Curl_creader_add(ساختار Curl_easy *data,
                          ساختار Curl_creader *reader)
{
  CURLcode result;
  ساختار Curl_creader **anchor = &data->req.reader.پشته;

  اگر(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    اگر(result)
      بازگشت result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  وقتی(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  بازگشت CURLE_OK;
}

CURLcode Curl_creader_set(ساختار Curl_easy *data, ساختار Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_پشته(data, r);
  اگر(result)
    Curl_creader_free(data, r);
  بازگشت result;
}

CURLcode Curl_client_read(ساختار Curl_easy *data, نویسه *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  اگر(!data->req.reader.پشته) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    اگر(result)
      بازگشت result;
    DEBUGASSERT(data->req.reader.پشته);
  }
  اگر(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  اگر(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, پوچ);
    اگر(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      برو out;
    }
    اگر(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.پشته, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحیح)result, *nread, *eos);
  بازگشت result;
}

bool Curl_creader_needs_rewind(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = data->req.reader.پشته;
  وقتی(reader) {
    اگر(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      بازگشت TRUE;
    }
    reader = reader->next;
  }
  بازگشت FALSE;
}

ایستا CURLcode cr_null_read(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader,
                             نویسه *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (تهی)data;
  (تهی)reader;
  (تهی)buf;
  (تهی)blen;
  *pnread = 0;
  *peos = TRUE;
  بازگشت CURLE_OK;
}

ایستا curl_off_t cr_null_total_length(ساختار Curl_easy *data,
                                       ساختار Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (تهی)data;
  (تهی)reader;
  بازگشت 0;
}

ایستا ثابت ساختار Curl_crtype cr_null = {
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
  اندازه(ساختار Curl_creader)
};

CURLcode Curl_creader_set_null(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  اگر(result)
    بازگشت result;

  cl_reset_reader(data);
  بازگشت do_init_reader_پشته(data, r);
}

ساختار cr_buf_ctx {
  ساختار Curl_creader super;
  ثابت نویسه *buf;
  size_t blen;
  size_t index;
};

ایستا CURLcode cr_buf_read(ساختار Curl_easy *data,
                            ساختار Curl_creader *reader,
                            نویسه *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ساختار cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  اگر(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  وگرنه {
    اگر(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  بازگشت CURLE_OK;
}

ایستا bool cr_buf_needs_rewind(ساختار Curl_easy *data,
                                ساختار Curl_creader *reader)
{
  ساختار cr_buf_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت ctx->index > 0;
}

ایستا CURLcode cr_buf_cntrl(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ساختار cr_buf_ctx *ctx = reader->ctx;
  (تهی)data;
  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا curl_off_t cr_buf_total_length(ساختار Curl_easy *data,
                                      ساختار Curl_creader *reader)
{
  ساختار cr_buf_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت (curl_off_t)ctx->blen;
}

ایستا CURLcode cr_buf_resume_from(ساختار Curl_easy *data,
                                   ساختار Curl_creader *reader,
                                   curl_off_t offset)
{
  ساختار cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (تهی)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  اگر(ctx->index)
    بازگشت CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  اگر(!boffset)
    بازگشت CURLE_OK;
  اگر(boffset > ctx->blen)
    بازگشت CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  بازگشت CURLE_OK;
}

ایستا ثابت ساختار Curl_crtype cr_buf = {
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
  اندازه(ساختار cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(ساختار Curl_easy *data,
                              ثابت نویسه *buf, size_t blen)
{
  CURLcode result;
  ساختار Curl_creader *r;
  ساختار cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  اگر(result)
    برو out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_پشته(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (صحیح)result);
  بازگشت result;
}

curl_off_t Curl_creader_total_length(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = data->req.reader.پشته;
  بازگشت r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = data->req.reader.پشته;
  وقتی(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  بازگشت r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = data->req.reader.پشته;
  CURLcode result = CURLE_OK;

  وقتی(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (صحیح)result);
    اگر(result)
      بشکن;
    reader = reader->next;
  }
  بازگشت result;
}

bool Curl_creader_is_paused(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = data->req.reader.پشته;

  وقتی(reader) {
    اگر(reader->crt->is_paused(data, reader))
      بازگشت TRUE;
    reader = reader->next;
  }
  بازگشت FALSE;
}

تهی Curl_creader_done(ساختار Curl_easy *data, صحیح premature)
{
  ساختار Curl_creader *reader = data->req.reader.پشته;
  وقتی(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

ساختار Curl_creader *Curl_creader_get_by_type(ساختار Curl_easy *data,
                                              ثابت ساختار Curl_crtype *crt)
{
  ساختار Curl_creader *r;
  برای(r = data->req.reader.پشته; r; r = r->next) {
    اگر(r->crt == crt)
      بازگشت r;
  }
  بازگشت پوچ;
}
