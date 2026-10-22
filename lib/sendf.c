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

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#endif

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

ساكن فراغ cl_reset_writer(بنية Curl_easy *data)
{
  بنية Curl_cwriter *writer = data->req.writer.stack;
  طالما(writer) {
    data->req.writer.stack = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.stack;
  }
  data->req.writer.paused = FALSE;
}

ساكن فراغ cl_reset_reader(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = data->req.reader.stack;
  data->req.reader_started = FALSE;
  طالما(reader) {
    data->req.reader.stack = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.stack;
  }
}

فراغ Curl_client_cleanup(بنية Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

فراغ Curl_client_reset(بنية Curl_easy *data)
{
  𐤀𐤌(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  وإلا {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(بنية Curl_easy *data)
{
  𐤀𐤌(data->req.rewind_read) {
    بنية Curl_creader *r = data->req.reader.stack;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    طالما(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      𐤀𐤌(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (صحيح)result);
        أرجع result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  أرجع CURLE_OK;
}

bool Curl_creader_will_rewind(بنية Curl_easy *data)
{
  أرجع (bool)data->req.rewind_read;
}

فراغ Curl_creader_set_rewind(بنية Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(بنية Curl_easy *data,
                               بنية Curl_cwriter *writer)
{
  (فراغ)data;
  (فراغ)writer;
  أرجع CURLE_OK;
}

CURLcode Curl_cwriter_def_write(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t nbytes)
{
  أرجع Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer)
{
  أرجع Curl_cwriter_flush(data, writer->next);
}

فراغ Curl_cwriter_def_close(بنية Curl_easy *data,
                            بنية Curl_cwriter *writer)
{
  (فراغ)data;
  (فراغ)writer;
}

ساكن size_t get_max_body_write_len(بنية Curl_easy *data, curl_off_t limit)
{
  𐤀𐤌(limit != -1) {
    /* How much more are we allowed to write? */
    أرجع curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  أرجع SIZE_MAX;
}

بنية cw_download_ctx {
  بنية Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ساكن CURLcode cw_download_write(بنية Curl_easy *data,
                                  بنية Curl_cwriter *writer, صحيح type,
                                  ثابت حرف *buf, size_t nbytes)
{
  بنية cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  𐤀𐤌(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  𐤀𐤌(!(type & CLIENTWRITE_BODY)) {
    𐤀𐤌(is_connect && data->set.suppress_connect_headers)
      أرجع CURLE_OK;
#ifdef DEBUGBUILD
    𐤀𐤌(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      أرجع CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned صحيح)type, nbytes, (صحيح)result);
    أرجع result;
  }

  𐤀𐤌(!ctx->started_body &&
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

  𐤀𐤌(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned صحيح)type, nbytes);
    data->req.download_done = TRUE;
    𐤀𐤌(data->info.header_size)
      /* if headers have been received, this is fine */
      أرجع CURLE_OK;
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  𐤀𐤌(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    𐤀𐤌(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    𐤀𐤌(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    𐤀𐤌((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      أرجع CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  𐤀𐤌(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    𐤀𐤌(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  𐤀𐤌(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned صحيح)type, nbytes, (صحيح)result);
    𐤀𐤌(result)
      أرجع result;
  }

  /* Update stats, write and report progress */
  𐤀𐤌(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  𐤀𐤌(excess_len) {
    𐤀𐤌(!data->req.ignorebody) {
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
  وإلا 𐤀𐤌((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
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
ساكن CURLcode cw_raw_write(بنية Curl_easy *data,
                             بنية Curl_cwriter *writer, صحيح type,
                             ثابت حرف *buf, size_t nbytes)
{
  𐤀𐤌(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  أرجع Curl_cwriter_write(data, writer->next, type, buf, nbytes);
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

ساكن فراغ cwriter_add(بنية Curl_easy *data,
                        بنية Curl_cwriter *writer)
{
  بنية Curl_cwriter **anchor = &data->req.writer.stack;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  طالما(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

ساكن CURLcode do_init_writer_stack(بنية Curl_easy *data)
{
  بنية Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.stack);
  result = Curl_cwriter_create(&data->req.writer.stack,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  𐤀𐤌(result)
    أرجع result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  𐤀𐤌(result)
    أرجع result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  𐤀𐤌(result)
    أرجع result;
  cwriter_add(data, writer);

  أرجع result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(بنية Curl_easy *data, صحيح type, ثابت حرف *buf,
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

  𐤀𐤌(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    𐤀𐤌(result)
      أرجع result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_write(data, data->req.writer.stack, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned صحيح)type, len, (صحيح)result);
  أرجع result;
}

CURLcode Curl_client_flush(بنية Curl_easy *data)
{
  CURLcode result;

  𐤀𐤌(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    𐤀𐤌(result)
      أرجع result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_flush(data, data->req.writer.stack);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (صحيح)result);
  أرجع result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(بنية Curl_cwriter **pwriter,
                             بنية Curl_easy *data,
                             ثابت بنية Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *writer = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  فراغ *p;

  DEBUGASSERT(cwt->cwriter_size >= حجم(بنية Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  𐤀𐤌(!p)
    اذهب out;

  writer = (بنية Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? عدم : writer;
  𐤀𐤌(result)
    curlx_free(writer);
  أرجع result;
}

فراغ Curl_cwriter_free(بنية Curl_easy *data,
                       بنية Curl_cwriter *writer)
{
  𐤀𐤌(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_count(بنية Curl_easy *data, Curl_cwriter_phase phase)
{
  بنية Curl_cwriter *w;
  size_t n = 0;

  لكل(w = data->req.writer.stack; w; w = w->next) {
    𐤀𐤌(w->phase == phase)
      ++n;
  }
  أرجع n;
}

ساكن CURLcode cwriter_ensure_pause_writer(بنية Curl_easy *data)
{
  بنية Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  𐤀𐤌(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    𐤀𐤌(!result)
      cwriter_add(data, writer);
  }
  أرجع result;
}

CURLcode Curl_cwriter_add(بنية Curl_easy *data,
                          بنية Curl_cwriter *writer)
{
  CURLcode result;
  بنية Curl_cwriter **anchor = &data->req.writer.stack;

  𐤀𐤌(!*anchor) {
    result = do_init_writer_stack(data);
    𐤀𐤌(result)
      أرجع result;
  }

  𐤀𐤌(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    𐤀𐤌(result)
      أرجع result;
  }

  cwriter_add(data, writer);
  أرجع CURLE_OK;
}

بنية Curl_cwriter *Curl_cwriter_get_by_name(بنية Curl_easy *data,
                                              ثابت حرف *name)
{
  بنية Curl_cwriter *writer;
  لكل(writer = data->req.writer.stack; writer; writer = writer->next) {
    𐤀𐤌(!strcmp(name, writer->cwt->name))
      أرجع writer;
  }
  أرجع عدم;
}

بنية Curl_cwriter *Curl_cwriter_get_by_type(بنية Curl_easy *data,
                                              ثابت بنية Curl_cwtype *cwt)
{
  بنية Curl_cwriter *writer;
  لكل(writer = data->req.writer.stack; writer; writer = writer->next) {
    𐤀𐤌(writer->cwt == cwt)
      أرجع writer;
  }
  أرجع عدم;
}

CURLcode Curl_cwriter_unpause(بنية Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  أرجع Curl_cwriter_flush(data, data->req.writer.stack);
}

CURLcode Curl_creader_read(بنية Curl_easy *data,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  𐤀𐤌(!reader)
    أرجع CURLE_READ_ERROR;
  أرجع reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

فراغ Curl_creader_clear_eos(بنية Curl_easy *data,
                            بنية Curl_creader *reader)
{
  طالما(reader) {
    (فراغ)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(بنية Curl_easy *data,
                               بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
  أرجع CURLE_OK;
}

فراغ Curl_creader_def_close(بنية Curl_easy *data,
                            بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
}

CURLcode Curl_creader_def_read(بنية Curl_easy *data,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  𐤀𐤌(reader->next)
    أرجع reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  وإلا {
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(بنية Curl_easy *data,
                                   بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
  أرجع FALSE;
}

curl_off_t Curl_creader_def_total_length(بنية Curl_easy *data,
                                         بنية Curl_creader *reader)
{
  أرجع reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(بنية Curl_easy *data,
                                      بنية Curl_creader *reader,
                                      curl_off_t offset)
{
  (فراغ)data;
  (فراغ)reader;
  (فراغ)offset;
  أرجع CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(بنية Curl_easy *data,
                                بنية Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (فراغ)data;
  (فراغ)reader;
  (فراغ)opcode;
  أرجع CURLE_OK;
}

bool Curl_creader_def_is_paused(بنية Curl_easy *data,
                                بنية Curl_creader *reader)
{
  (فراغ)data;
  (فراغ)reader;
  أرجع FALSE;
}

فراغ Curl_creader_def_done(بنية Curl_easy *data,
                           بنية Curl_creader *reader, صحيح premature)
{
  (فراغ)data;
  (فراغ)reader;
  (فراغ)premature;
}

بنية cr_in_ctx {
  بنية Curl_creader super;
  curl_read_callback read_cb;
  فراغ *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

ساكن CURLcode cr_in_init(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  أرجع CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_in_read(بنية Curl_easy *data,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  𐤀𐤌(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_result;
  }
  𐤀𐤌(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  𐤀𐤌(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  𐤀𐤌(ctx->read_cb && blen) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  بدل(nread) {
  حالة 0:
    𐤀𐤌((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      اكسر;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    𐤀𐤌(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      اكسر;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    اكسر; /* nothing was read */

  افتراضي:
    𐤀𐤌(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      اكسر;
    }
    ctx->read_len += nread;
    𐤀𐤌(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    اكسر;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (صحيح)result,
                *pnread, *peos);
  أرجع result;
}

ساكن bool cr_in_needs_rewind(بنية Curl_easy *data,
                               بنية Curl_creader *reader)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  (فراغ)data;
  أرجع (bool)ctx->has_used_cb;
}

ساكن curl_off_t cr_in_total_length(بنية Curl_easy *data,
                                     بنية Curl_creader *reader)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  (فراغ)data;
  أرجع ctx->total_len;
}

ساكن CURLcode cr_in_resume_from(بنية Curl_easy *data,
                                  بنية Curl_creader *reader,
                                  curl_off_t offset)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  صحيح seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  𐤀𐤌(ctx->read_len)
    أرجع CURLE_READ_ERROR;

  𐤀𐤌(data->set.seek_func) {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  𐤀𐤌(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    𐤀𐤌(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      أرجع CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    𐤐𐤏𐤋 {
      بنية Curl_mapi_guard guard;
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      𐤀𐤌((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < offset);
  }

  /* now, decrease the size of the read */
  𐤀𐤌(ctx->total_len > 0) {
    ctx->total_len -= offset;

    𐤀𐤌(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      أرجع CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  أرجع CURLE_OK;
}

ساكن CURLcode cr_in_rewind(بنية Curl_easy *data,
                             بنية Curl_creader *reader)
{
  بنية cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  𐤀𐤌(!ctx->has_used_cb)
    أرجع CURLE_OK;

  𐤀𐤌(data->set.seek_func) {
    بنية Curl_mapi_guard guard;
    صحيح err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    𐤀𐤌(err) {
      failf(data, "seek callback returned error %d", err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا 𐤀𐤌(data->set.ioctl_func) {
    بنية Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (صحيح)err);
    𐤀𐤌(err) {
      failf(data, "ioctl callback returned error %d", (صحيح)err);
      أرجع CURLE_SEND_FAIL_REWIND;
    }
  }
  وإلا {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#𐤀𐤌 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    𐤀𐤌(data->state.fread_func == (curl_read_callback)fread) {
#𐤀𐤌 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
      صحيح err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (صحيح)err, (صحيح)errno);
      𐤀𐤌(err != -1)
        /* successful rewind */
        أرجع CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    أرجع CURLE_SEND_FAIL_REWIND;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cr_in_cntrl(بنية Curl_easy *data,
                            بنية Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  بنية cr_in_ctx *ctx = reader->ctx;

  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    أرجع cr_in_rewind(data, reader);
  حالة CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_in_is_paused(بنية Curl_easy *data,
                            بنية Curl_creader *reader)
{
  بنية cr_in_ctx *ctx = reader->ctx;
  (فراغ)data;
  أرجع (bool)ctx->is_paused;
}

ساكن ثابت بنية Curl_crtype cr_in = {
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
  حجم(بنية cr_in_ctx)
};

CURLcode Curl_creader_create(بنية Curl_creader **preader,
                             بنية Curl_easy *data,
                             ثابت بنية Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  بنية Curl_creader *reader = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  فراغ *p;

  DEBUGASSERT(crt->creader_size >= حجم(بنية Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  𐤀𐤌(!p)
    اذهب out;

  reader = (بنية Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? عدم : reader;
  𐤀𐤌(result)
    curlx_free(reader);
  أرجع result;
}

فراغ Curl_creader_free(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  𐤀𐤌(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

بنية cr_lc_ctx {
  بنية Curl_creader super;
  بنية bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

ساكن CURLcode cr_lc_init(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  أرجع CURLE_OK;
}

ساكن فراغ cr_lc_close(بنية Curl_easy *data, بنية Curl_creader *reader)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  (فراغ)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ساكن CURLcode cr_lc_read(بنية Curl_easy *data,
                           بنية Curl_creader *reader,
                           حرف *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  بنية cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  𐤀𐤌(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }

  𐤀𐤌(Curl_bufq_is_empty(&ctx->buf)) {
    𐤀𐤌(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      أرجع CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    𐤀𐤌(result)
      أرجع result;
    ctx->read_eos = eos;

    𐤀𐤌(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      𐤀𐤌(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      𐤀𐤌(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      اذهب out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    لكل(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      𐤀𐤌((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        استمر;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      𐤀𐤌(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      𐤀𐤌(result)
        أرجع result;
      start = i + 1;
    }

    𐤀𐤌(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      𐤀𐤌(result)
        أرجع result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  𐤀𐤌(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)result, *pnread, *peos);
  أرجع result;
}

ساكن curl_off_t cr_lc_total_length(بنية Curl_easy *data,
                                     بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)data;
  (فراغ)reader;
  أرجع -1;
}

ساكن ثابت بنية Curl_crtype cr_lc = {
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
  حجم(بنية cr_lc_ctx)
};

ساكن CURLcode cr_lc_add(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = عدم;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  𐤀𐤌(!result)
    result = Curl_creader_add(data, reader);

  𐤀𐤌(result && reader)
    Curl_creader_free(data, reader);
  أرجع result;
}

ساكن CURLcode do_init_reader_stack(بنية Curl_easy *data,
                                     بنية Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.stack);

  data->req.reader.stack = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  𐤀𐤌(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#وإلا
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    𐤀𐤌(result)
      أرجع result;
  }

  أرجع result;
}

CURLcode Curl_creader_set_fread(بنية Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  بنية Curl_creader *r;
  بنية cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  𐤀𐤌(result || !r)
    اذهب out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (صحيح)result);
  أرجع result;
}

CURLcode Curl_creader_add(بنية Curl_easy *data,
                          بنية Curl_creader *reader)
{
  CURLcode result;
  بنية Curl_creader **anchor = &data->req.reader.stack;

  𐤀𐤌(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    𐤀𐤌(result)
      أرجع result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  طالما(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  أرجع CURLE_OK;
}

CURLcode Curl_creader_set(بنية Curl_easy *data, بنية Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
  𐤀𐤌(result)
    Curl_creader_free(data, r);
  أرجع result;
}

CURLcode Curl_client_read(بنية Curl_easy *data, حرف *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  𐤀𐤌(!data->req.reader.stack) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    𐤀𐤌(result)
      أرجع result;
    DEBUGASSERT(data->req.reader.stack);
  }
  𐤀𐤌(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  𐤀𐤌(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, عدم);
    𐤀𐤌(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      اذهب out;
    }
    𐤀𐤌(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.stack, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (صحيح)result, *nread, *eos);
  أرجع result;
}

bool Curl_creader_needs_rewind(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = data->req.reader.stack;
  طالما(reader) {
    𐤀𐤌(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      أرجع TRUE;
    }
    reader = reader->next;
  }
  أرجع FALSE;
}

ساكن CURLcode cr_null_read(بنية Curl_easy *data,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (فراغ)data;
  (فراغ)reader;
  (فراغ)buf;
  (فراغ)blen;
  *pnread = 0;
  *peos = TRUE;
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_null_total_length(بنية Curl_easy *data,
                                       بنية Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (فراغ)data;
  (فراغ)reader;
  أرجع 0;
}

ساكن ثابت بنية Curl_crtype cr_null = {
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
  حجم(بنية Curl_creader)
};

CURLcode Curl_creader_set_null(بنية Curl_easy *data)
{
  بنية Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  𐤀𐤌(result)
    أرجع result;

  cl_reset_reader(data);
  أرجع do_init_reader_stack(data, r);
}

بنية cr_buf_ctx {
  بنية Curl_creader super;
  ثابت حرف *buf;
  size_t blen;
  size_t index;
};

ساكن CURLcode cr_buf_read(بنية Curl_easy *data,
                            بنية Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  𐤀𐤌(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  وإلا {
    𐤀𐤌(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  أرجع CURLE_OK;
}

ساكن bool cr_buf_needs_rewind(بنية Curl_easy *data,
                                بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)data;
  أرجع ctx->index > 0;
}

ساكن CURLcode cr_buf_cntrl(بنية Curl_easy *data,
                             بنية Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)data;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن curl_off_t cr_buf_total_length(بنية Curl_easy *data,
                                      بنية Curl_creader *reader)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  (فراغ)data;
  أرجع (curl_off_t)ctx->blen;
}

ساكن CURLcode cr_buf_resume_from(بنية Curl_easy *data,
                                   بنية Curl_creader *reader,
                                   curl_off_t offset)
{
  بنية cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (فراغ)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  𐤀𐤌(ctx->index)
    أرجع CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  𐤀𐤌(!boffset)
    أرجع CURLE_OK;
  𐤀𐤌(boffset > ctx->blen)
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
  cr_buf_total_length,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  حجم(بنية cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(بنية Curl_easy *data,
                              ثابت حرف *buf, size_t blen)
{
  CURLcode result;
  بنية Curl_creader *r;
  بنية cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  𐤀𐤌(result)
    اذهب out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (صحيح)result);
  أرجع result;
}

curl_off_t Curl_creader_total_length(بنية Curl_easy *data)
{
  بنية Curl_creader *r = data->req.reader.stack;
  أرجع r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(بنية Curl_easy *data)
{
  بنية Curl_creader *r = data->req.reader.stack;
  طالما(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  أرجع r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = data->req.reader.stack;
  CURLcode result = CURLE_OK;

  طالما(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (صحيح)result);
    𐤀𐤌(result)
      اكسر;
    reader = reader->next;
  }
  أرجع result;
}

bool Curl_creader_is_paused(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = data->req.reader.stack;

  طالما(reader) {
    𐤀𐤌(reader->crt->is_paused(data, reader))
      أرجع TRUE;
    reader = reader->next;
  }
  أرجع FALSE;
}

فراغ Curl_creader_done(بنية Curl_easy *data, صحيح premature)
{
  بنية Curl_creader *reader = data->req.reader.stack;
  طالما(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

بنية Curl_creader *Curl_creader_get_by_type(بنية Curl_easy *data,
                                              ثابت بنية Curl_crtype *crt)
{
  بنية Curl_creader *r;
  لكل(r = data->req.reader.stack; r; r = r->next) {
    𐤀𐤌(r->crt == crt)
      أرجع r;
  }
  أرجع عدم;
}
