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

static void cl_reset_writer(struct Curl_easy *data)
{
  struct Curl_cwriter *writer = data->req.writer.stack;
  поки(writer) {
    data->req.writer.stack = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.stack;
  }
  data->req.writer.paused = FALSE;
}

static void cl_reset_reader(struct Curl_easy *data)
{
  struct Curl_creader *reader = data->req.reader.stack;
  data->req.reader_started = FALSE;
  поки(reader) {
    data->req.reader.stack = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.stack;
  }
}

void Curl_client_cleanup(struct Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

void Curl_client_reset(struct Curl_easy *data)
{
  якщо(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  інакше {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(struct Curl_easy *data)
{
  якщо(data->req.rewind_read) {
    struct Curl_creader *r = data->req.reader.stack;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    поки(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      якщо(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ціле)result);
        повернути result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  повернути CURLE_OK;
}

bool Curl_creader_will_rewind(struct Curl_easy *data)
{
  повернути (bool)data->req.rewind_read;
}

void Curl_creader_set_rewind(struct Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(struct Curl_easy *data,
                               struct Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
  повернути CURLE_OK;
}

CURLcode Curl_cwriter_def_write(struct Curl_easy *data,
                                struct Curl_cwriter *writer, ціле type,
                                const char *buf, size_t nbytes)
{
  повернути Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(struct Curl_easy *data,
                                struct Curl_cwriter *writer)
{
  повернути Curl_cwriter_flush(data, writer->next);
}

void Curl_cwriter_def_close(struct Curl_easy *data,
                            struct Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
}

static size_t get_max_body_write_len(struct Curl_easy *data, curl_off_t limit)
{
  якщо(limit != -1) {
    /* How much more are we allowed to write? */
    повернути curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  повернути SIZE_MAX;
}

struct cw_download_ctx {
  struct Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
static CURLcode cw_download_write(struct Curl_easy *data,
                                  struct Curl_cwriter *writer, ціле type,
                                  const char *buf, size_t nbytes)
{
  struct cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  якщо(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  якщо(!(type & CLIENTWRITE_BODY)) {
    якщо(is_connect && data->set.suppress_connect_headers)
      повернути CURLE_OK;
#ifdef DEBUGBUILD
    якщо(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      повернути CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned ціле)type, nbytes, (ціле)result);
    повернути result;
  }

  якщо(!ctx->started_body &&
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

  якщо(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned ціле)type, nbytes);
    data->req.download_done = TRUE;
    якщо(data->info.header_size)
      /* if headers have been received, this is fine */
      повернути CURLE_OK;
    повернути CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  якщо(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    якщо(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    якщо(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    якщо((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      повернути CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  якщо(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    якщо(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  якщо(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned ціле)type, nbytes, (ціле)result);
    якщо(result)
      повернути result;
  }

  /* Update stats, write and report progress */
  якщо(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  якщо(excess_len) {
    якщо(!data->req.ignorebody) {
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
  інакше якщо((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    повернути CURLE_FILESIZE_EXCEEDED;
  }

  повернути CURLE_OK;
}

static const struct Curl_cwtype cw_download = {
  "protocol",
  NULL,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  sizeof(struct cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
static CURLcode cw_raw_write(struct Curl_easy *data,
                             struct Curl_cwriter *writer, ціле type,
                             const char *buf, size_t nbytes)
{
  якщо(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  повернути Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

static const struct Curl_cwtype cw_raw = {
  "raw",
  NULL,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  sizeof(struct Curl_cwriter)
};

static void cwriter_add(struct Curl_easy *data,
                        struct Curl_cwriter *writer)
{
  struct Curl_cwriter **anchor = &data->req.writer.stack;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  поки(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

static CURLcode do_init_writer_stack(struct Curl_easy *data)
{
  struct Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.stack);
  result = Curl_cwriter_create(&data->req.writer.stack,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  якщо(result)
    повернути result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  якщо(result)
    повернути result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  якщо(result)
    повернути result;
  cwriter_add(data, writer);

  повернути result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(struct Curl_easy *data, ціле type, const char *buf,
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

  якщо(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    якщо(result)
      повернути result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_write(data, data->req.writer.stack, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned ціле)type, len, (ціле)result);
  повернути result;
}

CURLcode Curl_client_flush(struct Curl_easy *data)
{
  CURLcode result;

  якщо(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    якщо(result)
      повернути result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_flush(data, data->req.writer.stack);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (ціле)result);
  повернути result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(struct Curl_cwriter **pwriter,
                             struct Curl_easy *data,
                             const struct Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  struct Curl_cwriter *writer = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  void *p;

  DEBUGASSERT(cwt->cwriter_size >= sizeof(struct Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  якщо(!p)
    goto out;

  writer = (struct Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? NULL : writer;
  якщо(result)
    curlx_free(writer);
  повернути result;
}

void Curl_cwriter_free(struct Curl_easy *data,
                       struct Curl_cwriter *writer)
{
  якщо(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_count(struct Curl_easy *data, Curl_cwriter_phase phase)
{
  struct Curl_cwriter *w;
  size_t n = 0;

  для(w = data->req.writer.stack; w; w = w->next) {
    якщо(w->phase == phase)
      ++n;
  }
  повернути n;
}

static CURLcode cwriter_ensure_pause_writer(struct Curl_easy *data)
{
  struct Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  якщо(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    якщо(!result)
      cwriter_add(data, writer);
  }
  повернути result;
}

CURLcode Curl_cwriter_add(struct Curl_easy *data,
                          struct Curl_cwriter *writer)
{
  CURLcode result;
  struct Curl_cwriter **anchor = &data->req.writer.stack;

  якщо(!*anchor) {
    result = do_init_writer_stack(data);
    якщо(result)
      повернути result;
  }

  якщо(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    якщо(result)
      повернути result;
  }

  cwriter_add(data, writer);
  повернути CURLE_OK;
}

struct Curl_cwriter *Curl_cwriter_get_by_name(struct Curl_easy *data,
                                              const char *name)
{
  struct Curl_cwriter *writer;
  для(writer = data->req.writer.stack; writer; writer = writer->next) {
    якщо(!strcmp(name, writer->cwt->name))
      повернути writer;
  }
  повернути NULL;
}

struct Curl_cwriter *Curl_cwriter_get_by_type(struct Curl_easy *data,
                                              const struct Curl_cwtype *cwt)
{
  struct Curl_cwriter *writer;
  для(writer = data->req.writer.stack; writer; writer = writer->next) {
    якщо(writer->cwt == cwt)
      повернути writer;
  }
  повернути NULL;
}

CURLcode Curl_cwriter_unpause(struct Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  повернути Curl_cwriter_flush(data, data->req.writer.stack);
}

CURLcode Curl_creader_read(struct Curl_easy *data,
                           struct Curl_creader *reader,
                           char *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  якщо(!reader)
    повернути CURLE_READ_ERROR;
  повернути reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

void Curl_creader_clear_eos(struct Curl_easy *data,
                            struct Curl_creader *reader)
{
  поки(reader) {
    (void)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(struct Curl_easy *data,
                               struct Curl_creader *reader)
{
  (void)data;
  (void)reader;
  повернути CURLE_OK;
}

void Curl_creader_def_close(struct Curl_easy *data,
                            struct Curl_creader *reader)
{
  (void)data;
  (void)reader;
}

CURLcode Curl_creader_def_read(struct Curl_easy *data,
                               struct Curl_creader *reader,
                               char *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  якщо(reader->next)
    повернути reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  інакше {
    *nread = 0;
    *eos = FALSE;
    повернути CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(struct Curl_easy *data,
                                   struct Curl_creader *reader)
{
  (void)data;
  (void)reader;
  повернути FALSE;
}

curl_off_t Curl_creader_def_total_length(struct Curl_easy *data,
                                         struct Curl_creader *reader)
{
  повернути reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(struct Curl_easy *data,
                                      struct Curl_creader *reader,
                                      curl_off_t offset)
{
  (void)data;
  (void)reader;
  (void)offset;
  повернути CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(struct Curl_easy *data,
                                struct Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (void)data;
  (void)reader;
  (void)opcode;
  повернути CURLE_OK;
}

bool Curl_creader_def_is_paused(struct Curl_easy *data,
                                struct Curl_creader *reader)
{
  (void)data;
  (void)reader;
  повернути FALSE;
}

void Curl_creader_def_done(struct Curl_easy *data,
                           struct Curl_creader *reader, ціле premature)
{
  (void)data;
  (void)reader;
  (void)premature;
}

struct cr_in_ctx {
  struct Curl_creader super;
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

static CURLcode cr_in_init(struct Curl_easy *data, struct Curl_creader *reader)
{
  struct cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  повернути CURLE_OK;
}

/* Real client reader to installed client callbacks. */
static CURLcode cr_in_read(struct Curl_easy *data,
                           struct Curl_creader *reader,
                           char *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  struct cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  якщо(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    повернути ctx->error_result;
  }
  якщо(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    повернути CURLE_OK;
  }
  /* respect length limitations */
  якщо(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  якщо(ctx->read_cb && blen) {
    struct Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  switch(nread) {
  case 0:
    якщо((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      break;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    break;

  case CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    break;

  case CURL_READFUNC_PAUSE:
    якщо(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      break;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    break; /* nothing was read */

  default:
    якщо(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      break;
    }
    ctx->read_len += nread;
    якщо(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    break;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (ціле)result,
                *pnread, *peos);
  повернути result;
}

static bool cr_in_needs_rewind(struct Curl_easy *data,
                               struct Curl_creader *reader)
{
  struct cr_in_ctx *ctx = reader->ctx;
  (void)data;
  повернути (bool)ctx->has_used_cb;
}

static curl_off_t cr_in_total_length(struct Curl_easy *data,
                                     struct Curl_creader *reader)
{
  struct cr_in_ctx *ctx = reader->ctx;
  (void)data;
  повернути ctx->total_len;
}

static CURLcode cr_in_resume_from(struct Curl_easy *data,
                                  struct Curl_creader *reader,
                                  curl_off_t offset)
{
  struct cr_in_ctx *ctx = reader->ctx;
  ціле seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  якщо(ctx->read_len)
    повернути CURLE_READ_ERROR;

  якщо(data->set.seek_func) {
    struct Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  якщо(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    якщо(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      повернути CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    do {
      struct Curl_mapi_guard guard;
      char scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)sizeof(scratch)) ?
        sizeof(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      якщо((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        повернути CURLE_READ_ERROR;
      }
    } поки(passed < offset);
  }

  /* now, decrease the size of the read */
  якщо(ctx->total_len > 0) {
    ctx->total_len -= offset;

    якщо(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      повернути CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  повернути CURLE_OK;
}

static CURLcode cr_in_rewind(struct Curl_easy *data,
                             struct Curl_creader *reader)
{
  struct cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  якщо(!ctx->has_used_cb)
    повернути CURLE_OK;

  якщо(data->set.seek_func) {
    struct Curl_mapi_guard guard;
    ціле err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    якщо(err) {
      failf(data, "seek callback returned error %d", err);
      повернути CURLE_SEND_FAIL_REWIND;
    }
  }
  інакше якщо(data->set.ioctl_func) {
    struct Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (ціле)err);
    якщо(err) {
      failf(data, "ioctl callback returned error %d", (ціле)err);
      повернути CURLE_SEND_FAIL_REWIND;
    }
  }
  інакше {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#якщо defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    якщо(data->state.fread_func == (curl_read_callback)fread) {
#якщо defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
      ціле err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (ціле)err, (ціле)errno);
      якщо(err != -1)
        /* successful rewind */
        повернути CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    повернути CURLE_SEND_FAIL_REWIND;
  }
  повернути CURLE_OK;
}

static CURLcode cr_in_cntrl(struct Curl_easy *data,
                            struct Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  struct cr_in_ctx *ctx = reader->ctx;

  switch(opcode) {
  case CURL_CRCNTRL_REWIND:
    повернути cr_in_rewind(data, reader);
  case CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    break;
  case CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    break;
  default:
    break;
  }
  повернути CURLE_OK;
}

static bool cr_in_is_paused(struct Curl_easy *data,
                            struct Curl_creader *reader)
{
  struct cr_in_ctx *ctx = reader->ctx;
  (void)data;
  повернути (bool)ctx->is_paused;
}

static const struct Curl_crtype cr_in = {
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
  sizeof(struct cr_in_ctx)
};

CURLcode Curl_creader_create(struct Curl_creader **preader,
                             struct Curl_easy *data,
                             const struct Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  struct Curl_creader *reader = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  void *p;

  DEBUGASSERT(crt->creader_size >= sizeof(struct Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  якщо(!p)
    goto out;

  reader = (struct Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? NULL : reader;
  якщо(result)
    curlx_free(reader);
  повернути result;
}

void Curl_creader_free(struct Curl_easy *data, struct Curl_creader *reader)
{
  якщо(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

struct cr_lc_ctx {
  struct Curl_creader super;
  struct bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

static CURLcode cr_lc_init(struct Curl_easy *data, struct Curl_creader *reader)
{
  struct cr_lc_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  повернути CURLE_OK;
}

static void cr_lc_close(struct Curl_easy *data, struct Curl_creader *reader)
{
  struct cr_lc_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
static CURLcode cr_lc_read(struct Curl_easy *data,
                           struct Curl_creader *reader,
                           char *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  struct cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  якщо(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    повернути CURLE_OK;
  }

  якщо(Curl_bufq_is_empty(&ctx->buf)) {
    якщо(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      повернути CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    якщо(result)
      повернути result;
    ctx->read_eos = eos;

    якщо(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      якщо(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      якщо(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      goto out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    для(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      якщо((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        continue;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      якщо(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      якщо(result)
        повернути result;
      start = i + 1;
    }

    якщо(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      якщо(result)
        повернути result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  якщо(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ціле)result, *pnread, *peos);
  повернути result;
}

static curl_off_t cr_lc_total_length(struct Curl_easy *data,
                                     struct Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  повернути -1;
}

static const struct Curl_crtype cr_lc = {
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
  sizeof(struct cr_lc_ctx)
};

static CURLcode cr_lc_add(struct Curl_easy *data)
{
  struct Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  якщо(!result)
    result = Curl_creader_add(data, reader);

  якщо(result && reader)
    Curl_creader_free(data, reader);
  повернути result;
}

static CURLcode do_init_reader_stack(struct Curl_easy *data,
                                     struct Curl_creader *r)
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
  якщо(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#інакше
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    якщо(result)
      повернути result;
  }

  повернути result;
}

CURLcode Curl_creader_set_fread(struct Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  struct Curl_creader *r;
  struct cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  якщо(result || !r)
    goto out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ціле)result);
  повернути result;
}

CURLcode Curl_creader_add(struct Curl_easy *data,
                          struct Curl_creader *reader)
{
  CURLcode result;
  struct Curl_creader **anchor = &data->req.reader.stack;

  якщо(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    якщо(result)
      повернути result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  поки(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  повернути CURLE_OK;
}

CURLcode Curl_creader_set(struct Curl_easy *data, struct Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
  якщо(result)
    Curl_creader_free(data, r);
  повернути result;
}

CURLcode Curl_client_read(struct Curl_easy *data, char *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  якщо(!data->req.reader.stack) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    якщо(result)
      повернути result;
    DEBUGASSERT(data->req.reader.stack);
  }
  якщо(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  якщо(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, NULL);
    якщо(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      goto out;
    }
    якщо(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.stack, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ціле)result, *nread, *eos);
  повернути result;
}

bool Curl_creader_needs_rewind(struct Curl_easy *data)
{
  struct Curl_creader *reader = data->req.reader.stack;
  поки(reader) {
    якщо(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      повернути TRUE;
    }
    reader = reader->next;
  }
  повернути FALSE;
}

static CURLcode cr_null_read(struct Curl_easy *data,
                             struct Curl_creader *reader,
                             char *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (void)data;
  (void)reader;
  (void)buf;
  (void)blen;
  *pnread = 0;
  *peos = TRUE;
  повернути CURLE_OK;
}

static curl_off_t cr_null_total_length(struct Curl_easy *data,
                                       struct Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  повернути 0;
}

static const struct Curl_crtype cr_null = {
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
  sizeof(struct Curl_creader)
};

CURLcode Curl_creader_set_null(struct Curl_easy *data)
{
  struct Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  якщо(result)
    повернути result;

  cl_reset_reader(data);
  повернути do_init_reader_stack(data, r);
}

struct cr_buf_ctx {
  struct Curl_creader super;
  const char *buf;
  size_t blen;
  size_t index;
};

static CURLcode cr_buf_read(struct Curl_easy *data,
                            struct Curl_creader *reader,
                            char *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  struct cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  якщо(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  інакше {
    якщо(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  повернути CURLE_OK;
}

static bool cr_buf_needs_rewind(struct Curl_easy *data,
                                struct Curl_creader *reader)
{
  struct cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  повернути ctx->index > 0;
}

static CURLcode cr_buf_cntrl(struct Curl_easy *data,
                             struct Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  struct cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  switch(opcode) {
  case CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    break;
  default:
    break;
  }
  повернути CURLE_OK;
}

static curl_off_t cr_buf_total_length(struct Curl_easy *data,
                                      struct Curl_creader *reader)
{
  struct cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  повернути (curl_off_t)ctx->blen;
}

static CURLcode cr_buf_resume_from(struct Curl_easy *data,
                                   struct Curl_creader *reader,
                                   curl_off_t offset)
{
  struct cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (void)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  якщо(ctx->index)
    повернути CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  якщо(!boffset)
    повернути CURLE_OK;
  якщо(boffset > ctx->blen)
    повернути CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  повернути CURLE_OK;
}

static const struct Curl_crtype cr_buf = {
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
  sizeof(struct cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(struct Curl_easy *data,
                              const char *buf, size_t blen)
{
  CURLcode result;
  struct Curl_creader *r;
  struct cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  якщо(result)
    goto out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (ціле)result);
  повернути result;
}

curl_off_t Curl_creader_total_length(struct Curl_easy *data)
{
  struct Curl_creader *r = data->req.reader.stack;
  повернути r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(struct Curl_easy *data)
{
  struct Curl_creader *r = data->req.reader.stack;
  поки(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  повернути r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(struct Curl_easy *data)
{
  struct Curl_creader *reader = data->req.reader.stack;
  CURLcode result = CURLE_OK;

  поки(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (ціле)result);
    якщо(result)
      break;
    reader = reader->next;
  }
  повернути result;
}

bool Curl_creader_is_paused(struct Curl_easy *data)
{
  struct Curl_creader *reader = data->req.reader.stack;

  поки(reader) {
    якщо(reader->crt->is_paused(data, reader))
      повернути TRUE;
    reader = reader->next;
  }
  повернути FALSE;
}

void Curl_creader_done(struct Curl_easy *data, ціле premature)
{
  struct Curl_creader *reader = data->req.reader.stack;
  поки(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

struct Curl_creader *Curl_creader_get_by_type(struct Curl_easy *data,
                                              const struct Curl_crtype *crt)
{
  struct Curl_creader *r;
  для(r = data->req.reader.stack; r; r = r->next) {
    якщо(r->crt == crt)
      повернути r;
  }
  повернути NULL;
}
