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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#include <linux/tcp.h>
#elif defined(HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif

#include "urldata.h"
#include "sendf.h"
#include "curl_trc.h"
#include "transfer.h"
#include "cfilters.h"
#include "connect.h"
#include "cw-out.h"
#include "cw-pause.h"
#include "multiif.h"
#include "progress.h"

static void cl_reset_writer(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *writer = data->req.writer.stack;
  སྐབས(writer) {
    data->req.writer.stack = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.stack;
  }
  data->req.writer.paused = FALSE;
}

static void cl_reset_reader(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  data->req.reader_started = FALSE;
  སྐབས(reader) {
    data->req.reader.stack = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.stack;
  }
}

void Curl_client_cleanup(sa_nracanaa Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

void Curl_client_reset(sa_nracanaa Curl_easy *data)
{
  གལ་ཏེ(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  དེ་མིན {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(sa_nracanaa Curl_easy *data)
{
  གལ་ཏེ(data->req.rewind_read) {
    sa_nracanaa Curl_creader *r = data->req.reader.stack;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    སྐབས(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      གལ་ཏེ(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ཧྲིལ)result);
        ཕྱིར result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  ཕྱིར CURLE_OK;
}

bool Curl_creader_will_rewind(sa_nracanaa Curl_easy *data)
{
  ཕྱིར (bool)data->req.rewind_read;
}

void Curl_creader_set_rewind(sa_nracanaa Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
  ཕྱིར CURLE_OK;
}

CURLcode Curl_cwriter_def_write(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer, ཧྲིལ type,
                                const var_na *buf, size_t nbytes)
{
  ཕྱིར Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer)
{
  ཕྱིར Curl_cwriter_flush(data, writer->next);
}

void Curl_cwriter_def_close(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
}

static size_t get_max_body_write_len(sa_nracanaa Curl_easy *data, curl_off_t limit)
{
  གལ་ཏེ(limit != -1) {
    /* How much more are we allowed to write? */
    ཕྱིར curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  ཕྱིར SIZE_MAX;
}

sa_nracanaa cw_download_ctx {
  sa_nracanaa Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
static CURLcode cw_download_write(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_cwriter *writer, ཧྲིལ type,
                                  const var_na *buf, size_t nbytes)
{
  sa_nracanaa cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  གལ་ཏེ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  གལ་ཏེ(!(type & CLIENTWRITE_BODY)) {
    གལ་ཏེ(is_connect && data->set.suppress_connect_headers)
      ཕྱིར CURLE_OK;
#ifdef DEBUGBUILD
    གལ་ཏེ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ཕྱིར CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned ཧྲིལ)type, nbytes, (ཧྲིལ)result);
    ཕྱིར result;
  }

  གལ་ཏེ(!ctx->started_body &&
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

  གལ་ཏེ(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned ཧྲིལ)type, nbytes);
    data->req.download_done = TRUE;
    གལ་ཏེ(data->info.header_size)
      /* if headers have been received, this is fine */
      ཕྱིར CURLE_OK;
    ཕྱིར CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  གལ་ཏེ(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    གལ་ཏེ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    གལ་ཏེ(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    གལ་ཏེ((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      ཕྱིར CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  གལ་ཏེ(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    གལ་ཏེ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  གལ་ཏེ(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned ཧྲིལ)type, nbytes, (ཧྲིལ)result);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  /* Update stats, write and report progress */
  གལ་ཏེ(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  གལ་ཏེ(excess_len) {
    གལ་ཏེ(!data->req.ignorebody) {
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
  དེ་མིན གལ་ཏེ((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    ཕྱིར CURLE_FILESIZE_EXCEEDED;
  }

  ཕྱིར CURLE_OK;
}

static const sa_nracanaa Curl_cwtype cw_download = {
  "protocol",
  NULL,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  parimaa_na(sa_nracanaa cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
static CURLcode cw_raw_write(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_cwriter *writer, ཧྲིལ type,
                             const var_na *buf, size_t nbytes)
{
  གལ་ཏེ(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  ཕྱིར Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

static const sa_nracanaa Curl_cwtype cw_raw = {
  "raw",
  NULL,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  parimaa_na(sa_nracanaa Curl_cwriter)
};

static void cwriter_add(sa_nracanaa Curl_easy *data,
                        sa_nracanaa Curl_cwriter *writer)
{
  sa_nracanaa Curl_cwriter **anchor = &data->req.writer.stack;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  སྐབས(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

static CURLcode do_init_writer_stack(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.stack);
  result = Curl_cwriter_create(&data->req.writer.stack,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  གལ་ཏེ(result)
    ཕྱིར result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  གལ་ཏེ(result)
    ཕྱིར result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  གལ་ཏེ(result)
    ཕྱིར result;
  cwriter_add(data, writer);

  ཕྱིར result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(sa_nracanaa Curl_easy *data, ཧྲིལ type, const var_na *buf,
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

  གལ་ཏེ(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    གལ་ཏེ(result)
      ཕྱིར result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_write(data, data->req.writer.stack, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned ཧྲིལ)type, len, (ཧྲིལ)result);
  ཕྱིར result;
}

CURLcode Curl_client_flush(sa_nracanaa Curl_easy *data)
{
  CURLcode result;

  གལ་ཏེ(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    གལ་ཏེ(result)
      ཕྱིར result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_flush(data, data->req.writer.stack);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (ཧྲིལ)result);
  ཕྱིར result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(sa_nracanaa Curl_cwriter **pwriter,
                             sa_nracanaa Curl_easy *data,
                             const sa_nracanaa Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  sa_nracanaa Curl_cwriter *writer = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  void *p;

  DEBUGASSERT(cwt->cwriter_size >= parimaa_na(sa_nracanaa Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  གལ་ཏེ(!p)
    goto out;

  writer = (sa_nracanaa Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? NULL : writer;
  གལ་ཏེ(result)
    curlx_free(writer);
  ཕྱིར result;
}

void Curl_cwriter_free(sa_nracanaa Curl_easy *data,
                       sa_nracanaa Curl_cwriter *writer)
{
  གལ་ཏེ(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_count(sa_nracanaa Curl_easy *data, Curl_cwriter_phase phase)
{
  sa_nracanaa Curl_cwriter *w;
  size_t n = 0;

  ཆེད་དུ(w = data->req.writer.stack; w; w = w->next) {
    གལ་ཏེ(w->phase == phase)
      ++n;
  }
  ཕྱིར n;
}

static CURLcode cwriter_ensure_pause_writer(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  གལ་ཏེ(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    གལ་ཏེ(!result)
      cwriter_add(data, writer);
  }
  ཕྱིར result;
}

CURLcode Curl_cwriter_add(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_cwriter *writer)
{
  CURLcode result;
  sa_nracanaa Curl_cwriter **anchor = &data->req.writer.stack;

  གལ་ཏེ(!*anchor) {
    result = do_init_writer_stack(data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  གལ་ཏེ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  cwriter_add(data, writer);
  ཕྱིར CURLE_OK;
}

sa_nracanaa Curl_cwriter *Curl_cwriter_get_by_name(sa_nracanaa Curl_easy *data,
                                              const var_na *name)
{
  sa_nracanaa Curl_cwriter *writer;
  ཆེད་དུ(writer = data->req.writer.stack; writer; writer = writer->next) {
    གལ་ཏེ(!strcmp(name, writer->cwt->name))
      ཕྱིར writer;
  }
  ཕྱིར NULL;
}

sa_nracanaa Curl_cwriter *Curl_cwriter_get_by_type(sa_nracanaa Curl_easy *data,
                                              const sa_nracanaa Curl_cwtype *cwt)
{
  sa_nracanaa Curl_cwriter *writer;
  ཆེད་དུ(writer = data->req.writer.stack; writer; writer = writer->next) {
    གལ་ཏེ(writer->cwt == cwt)
      ཕྱིར writer;
  }
  ཕྱིར NULL;
}

CURLcode Curl_cwriter_unpause(sa_nracanaa Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  ཕྱིར Curl_cwriter_flush(data, data->req.writer.stack);
}

CURLcode Curl_creader_read(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader,
                           var_na *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  གལ་ཏེ(!reader)
    ཕྱིར CURLE_READ_ERROR;
  ཕྱིར reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

void Curl_creader_clear_eos(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader)
{
  སྐབས(reader) {
    (void)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ཕྱིར CURLE_OK;
}

void Curl_creader_def_close(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
}

CURLcode Curl_creader_def_read(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader,
                               var_na *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  གལ་ཏེ(reader->next)
    ཕྱིར reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  དེ་མིན {
    *nread = 0;
    *eos = FALSE;
    ཕྱིར CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ཕྱིར FALSE;
}

curl_off_t Curl_creader_def_total_length(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa Curl_creader *reader)
{
  ཕྱིར reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_creader *reader,
                                      curl_off_t offset)
{
  (void)data;
  (void)reader;
  (void)offset;
  ཕྱིར CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (void)data;
  (void)reader;
  (void)opcode;
  ཕྱིར CURLE_OK;
}

bool Curl_creader_def_is_paused(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ཕྱིར FALSE;
}

void Curl_creader_def_done(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader, ཧྲིལ premature)
{
  (void)data;
  (void)reader;
  (void)premature;
}

sa_nracanaa cr_in_ctx {
  sa_nracanaa Curl_creader super;
  curl_read_callback read_cb;
  void *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

static CURLcode cr_in_init(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  ཕྱིར CURLE_OK;
}

/* Real client reader to installed client callbacks. */
static CURLcode cr_in_read(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader,
                           var_na *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  གལ་ཏེ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ཕྱིར ctx->error_result;
  }
  གལ་ཏེ(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ཕྱིར CURLE_OK;
  }
  /* respect length limitations */
  གལ་ཏེ(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  གལ་ཏེ(ctx->read_cb && blen) {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  switch(nread) {
  pak_sa 0:
    གལ་ཏེ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      bha_nga;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    bha_nga;

  pak_sa CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    bha_nga;

  pak_sa CURL_READFUNC_PAUSE:
    གལ་ཏེ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      bha_nga;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    bha_nga; /* nothing was read */

  default:
    གལ་ཏེ(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      bha_nga;
    }
    ctx->read_len += nread;
    གལ་ཏེ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    bha_nga;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (ཧྲིལ)result,
                *pnread, *peos);
  ཕྱིར result;
}

static bool cr_in_needs_rewind(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ཕྱིར (bool)ctx->has_used_cb;
}

static curl_off_t cr_in_total_length(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ཕྱིར ctx->total_len;
}

static CURLcode cr_in_resume_from(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_creader *reader,
                                  curl_off_t offset)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  ཧྲིལ seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  གལ་ཏེ(ctx->read_len)
    ཕྱིར CURLE_READ_ERROR;

  གལ་ཏེ(data->set.seek_func) {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  གལ་ཏེ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    གལ་ཏེ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      ཕྱིར CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    do {
      sa_nracanaa Curl_mapi_guard guard;
      var_na scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)parimaa_na(scratch)) ?
        parimaa_na(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      གལ་ཏེ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ཕྱིར CURLE_READ_ERROR;
      }
    } སྐབས(passed < offset);
  }

  /* now, decrease the size of the read */
  གལ་ཏེ(ctx->total_len > 0) {
    ctx->total_len -= offset;

    གལ་ཏེ(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      ཕྱིར CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ཕྱིར CURLE_OK;
}

static CURLcode cr_in_rewind(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  གལ་ཏེ(!ctx->has_used_cb)
    ཕྱིར CURLE_OK;

  གལ་ཏེ(data->set.seek_func) {
    sa_nracanaa Curl_mapi_guard guard;
    ཧྲིལ err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    གལ་ཏེ(err) {
      failf(data, "seek callback returned error %d", err);
      ཕྱིར CURLE_SEND_FAIL_REWIND;
    }
  }
  དེ་མིན གལ་ཏེ(data->set.ioctl_func) {
    sa_nracanaa Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (ཧྲིལ)err);
    གལ་ཏེ(err) {
      failf(data, "ioctl callback returned error %d", (ཧྲིལ)err);
      ཕྱིར CURLE_SEND_FAIL_REWIND;
    }
  }
  དེ་མིན {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    གལ་ཏེ(data->state.fread_func == (curl_read_callback)fread) {
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
      ཧྲིལ err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (ཧྲིལ)err, (ཧྲིལ)errno);
      གལ་ཏེ(err != -1)
        /* successful rewind */
        ཕྱིར CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    ཕྱིར CURLE_SEND_FAIL_REWIND;
  }
  ཕྱིར CURLE_OK;
}

static CURLcode cr_in_cntrl(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;

  switch(opcode) {
  pak_sa CURL_CRCNTRL_REWIND:
    ཕྱིར cr_in_rewind(data, reader);
  pak_sa CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    bha_nga;
  pak_sa CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    bha_nga;
  default:
    bha_nga;
  }
  ཕྱིར CURLE_OK;
}

static bool cr_in_is_paused(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ཕྱིར (bool)ctx->is_paused;
}

static const sa_nracanaa Curl_crtype cr_in = {
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
  parimaa_na(sa_nracanaa cr_in_ctx)
};

CURLcode Curl_creader_create(sa_nracanaa Curl_creader **preader,
                             sa_nracanaa Curl_easy *data,
                             const sa_nracanaa Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  sa_nracanaa Curl_creader *reader = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  void *p;

  DEBUGASSERT(crt->creader_size >= parimaa_na(sa_nracanaa Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  གལ་ཏེ(!p)
    goto out;

  reader = (sa_nracanaa Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? NULL : reader;
  གལ་ཏེ(result)
    curlx_free(reader);
  ཕྱིར result;
}

void Curl_creader_free(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  གལ་ཏེ(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

sa_nracanaa cr_lc_ctx {
  sa_nracanaa Curl_creader super;
  sa_nracanaa bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

static CURLcode cr_lc_init(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_lc_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ཕྱིར CURLE_OK;
}

static void cr_lc_close(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_lc_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
static CURLcode cr_lc_read(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader,
                           var_na *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  sa_nracanaa cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  གལ་ཏེ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ཕྱིར CURLE_OK;
  }

  གལ་ཏེ(Curl_bufq_is_empty(&ctx->buf)) {
    གལ་ཏེ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ཕྱིར CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    གལ་ཏེ(result)
      ཕྱིར result;
    ctx->read_eos = eos;

    གལ་ཏེ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      གལ་ཏེ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      གལ་ཏེ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      goto out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    ཆེད་དུ(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      གལ་ཏེ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        continue;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      གལ་ཏེ(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      གལ་ཏེ(result)
        ཕྱིར result;
      start = i + 1;
    }

    གལ་ཏེ(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      གལ་ཏེ(result)
        ཕྱིར result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  གལ་ཏེ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ཧྲིལ)result, *pnread, *peos);
  ཕྱིར result;
}

static curl_off_t cr_lc_total_length(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ཕྱིར -1;
}

static const sa_nracanaa Curl_crtype cr_lc = {
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
  parimaa_na(sa_nracanaa cr_lc_ctx)
};

static CURLcode cr_lc_add(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  གལ་ཏེ(!result)
    result = Curl_creader_add(data, reader);

  གལ་ཏེ(result && reader)
    Curl_creader_free(data, reader);
  ཕྱིར result;
}

static CURLcode do_init_reader_stack(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *r)
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
  གལ་ཏེ(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#དེ་མིན
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  ཕྱིར result;
}

CURLcode Curl_creader_set_fread(sa_nracanaa Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  sa_nracanaa Curl_creader *r;
  sa_nracanaa cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  གལ་ཏེ(result || !r)
    goto out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ཧྲིལ)result);
  ཕྱིར result;
}

CURLcode Curl_creader_add(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_creader *reader)
{
  CURLcode result;
  sa_nracanaa Curl_creader **anchor = &data->req.reader.stack;

  གལ་ཏེ(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  སྐབས(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  ཕྱིར CURLE_OK;
}

CURLcode Curl_creader_set(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
  གལ་ཏེ(result)
    Curl_creader_free(data, r);
  ཕྱིར result;
}

CURLcode Curl_client_read(sa_nracanaa Curl_easy *data, var_na *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  གལ་ཏེ(!data->req.reader.stack) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    གལ་ཏེ(result)
      ཕྱིར result;
    DEBUGASSERT(data->req.reader.stack);
  }
  གལ་ཏེ(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  གལ་ཏེ(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, NULL);
    གལ་ཏེ(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      goto out;
    }
    གལ་ཏེ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.stack, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ཧྲིལ)result, *nread, *eos);
  ཕྱིར result;
}

bool Curl_creader_needs_rewind(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  སྐབས(reader) {
    གལ་ཏེ(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      ཕྱིར TRUE;
    }
    reader = reader->next;
  }
  ཕྱིར FALSE;
}

static CURLcode cr_null_read(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader,
                             var_na *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (void)data;
  (void)reader;
  (void)buf;
  (void)blen;
  *pnread = 0;
  *peos = TRUE;
  ཕྱིར CURLE_OK;
}

static curl_off_t cr_null_total_length(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ཕྱིར 0;
}

static const sa_nracanaa Curl_crtype cr_null = {
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
  parimaa_na(sa_nracanaa Curl_creader)
};

CURLcode Curl_creader_set_null(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  གལ་ཏེ(result)
    ཕྱིར result;

  cl_reset_reader(data);
  ཕྱིར do_init_reader_stack(data, r);
}

sa_nracanaa cr_buf_ctx {
  sa_nracanaa Curl_creader super;
  const var_na *buf;
  size_t blen;
  size_t index;
};

static CURLcode cr_buf_read(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader,
                            var_na *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  གལ་ཏེ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  དེ་མིན {
    གལ་ཏེ(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ཕྱིར CURLE_OK;
}

static bool cr_buf_needs_rewind(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  ཕྱིར ctx->index > 0;
}

static CURLcode cr_buf_cntrl(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  switch(opcode) {
  pak_sa CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    bha_nga;
  default:
    bha_nga;
  }
  ཕྱིར CURLE_OK;
}

static curl_off_t cr_buf_total_length(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  ཕྱིར (curl_off_t)ctx->blen;
}

static CURLcode cr_buf_resume_from(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_creader *reader,
                                   curl_off_t offset)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (void)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  གལ་ཏེ(ctx->index)
    ཕྱིར CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  གལ་ཏེ(!boffset)
    ཕྱིར CURLE_OK;
  གལ་ཏེ(boffset > ctx->blen)
    ཕྱིར CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ཕྱིར CURLE_OK;
}

static const sa_nracanaa Curl_crtype cr_buf = {
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
  parimaa_na(sa_nracanaa cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(sa_nracanaa Curl_easy *data,
                              const var_na *buf, size_t blen)
{
  CURLcode result;
  sa_nracanaa Curl_creader *r;
  sa_nracanaa cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  གལ་ཏེ(result)
    goto out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (ཧྲིལ)result);
  ཕྱིར result;
}

curl_off_t Curl_creader_total_length(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *r = data->req.reader.stack;
  ཕྱིར r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *r = data->req.reader.stack;
  སྐབས(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  ཕྱིར r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  CURLcode result = CURLE_OK;

  སྐབས(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (ཧྲིལ)result);
    གལ་ཏེ(result)
      bha_nga;
    reader = reader->next;
  }
  ཕྱིར result;
}

bool Curl_creader_is_paused(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;

  སྐབས(reader) {
    གལ་ཏེ(reader->crt->is_paused(data, reader))
      ཕྱིར TRUE;
    reader = reader->next;
  }
  ཕྱིར FALSE;
}

void Curl_creader_done(sa_nracanaa Curl_easy *data, ཧྲིལ premature)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  སྐབས(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

sa_nracanaa Curl_creader *Curl_creader_get_by_type(sa_nracanaa Curl_easy *data,
                                              const sa_nracanaa Curl_crtype *crt)
{
  sa_nracanaa Curl_creader *r;
  ཆེད་དུ(r = data->req.reader.stack; r; r = r->next) {
    གལ་ཏེ(r->crt == crt)
      ཕྱིར r;
  }
  ཕྱིར NULL;
}
