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
  ᱡᱚᱠᱷᱚᱱ(writer) {
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
  ᱡᱚᱠᱷᱚᱱ(reader) {
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
  ᱡᱩᱫᱤ(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  ᱵᱟᱝ {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(sa_nracanaa Curl_easy *data)
{
  ᱡᱩᱫᱤ(data->req.rewind_read) {
    sa_nracanaa Curl_creader *r = data->req.reader.stack;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    ᱡᱚᱠᱷᱚᱱ(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      ᱡᱩᱫᱤ(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (puur_naa_nka)result);
        ᱨᱩᱣᱟᱹᱲ result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

bool Curl_creader_will_rewind(sa_nracanaa Curl_easy *data)
{
  ᱨᱩᱣᱟᱹᱲ (bool)data->req.rewind_read;
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
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer, puur_naa_nka type,
                                const var_na *buf, size_t nbytes)
{
  ᱨᱩᱣᱟᱹᱲ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer)
{
  ᱨᱩᱣᱟᱹᱲ Curl_cwriter_flush(data, writer->next);
}

void Curl_cwriter_def_close(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_cwriter *writer)
{
  (void)data;
  (void)writer;
}

static size_t get_max_body_write_len(sa_nracanaa Curl_easy *data, curl_off_t limit)
{
  ᱡᱩᱫᱤ(limit != -1) {
    /* How much more are we allowed to write? */
    ᱨᱩᱣᱟᱹᱲ curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  ᱨᱩᱣᱟᱹᱲ SIZE_MAX;
}

sa_nracanaa cw_download_ctx {
  sa_nracanaa Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
static CURLcode cw_download_write(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_cwriter *writer, puur_naa_nka type,
                                  const var_na *buf, size_t nbytes)
{
  sa_nracanaa cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ᱡᱩᱫᱤ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ᱡᱩᱫᱤ(!(type & CLIENTWRITE_BODY)) {
    ᱡᱩᱫᱤ(is_connect && data->set.suppress_connect_headers)
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
#ifdef DEBUGBUILD
    ᱡᱩᱫᱤ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned puur_naa_nka)type, nbytes, (puur_naa_nka)result);
    ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱡᱩᱫᱤ(!ctx->started_body &&
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

  ᱡᱩᱫᱤ(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned puur_naa_nka)type, nbytes);
    data->req.download_done = TRUE;
    ᱡᱩᱫᱤ(data->info.header_size)
      /* if headers have been received, this is fine */
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    ᱨᱩᱣᱟᱹᱲ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ᱡᱩᱫᱤ(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    ᱡᱩᱫᱤ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ᱡᱩᱫᱤ(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    ᱡᱩᱫᱤ((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      ᱨᱩᱣᱟᱹᱲ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ᱡᱩᱫᱤ(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    ᱡᱩᱫᱤ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ᱡᱩᱫᱤ(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned puur_naa_nka)type, nbytes, (puur_naa_nka)result);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  /* Update stats, write and report progress */
  ᱡᱩᱫᱤ(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  ᱡᱩᱫᱤ(excess_len) {
    ᱡᱩᱫᱤ(!data->req.ignorebody) {
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
  ᱵᱟᱝ ᱡᱩᱫᱤ((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    ᱨᱩᱣᱟᱹᱲ CURLE_FILESIZE_EXCEEDED;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
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
                             sa_nracanaa Curl_cwriter *writer, puur_naa_nka type,
                             const var_na *buf, size_t nbytes)
{
  ᱡᱩᱫᱤ(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  ᱨᱩᱣᱟᱹᱲ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
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
  ᱡᱚᱠᱷᱚᱱ(*anchor && (*anchor)->phase < writer->phase)
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
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;
  cwriter_add(data, writer);

  ᱨᱩᱣᱟᱹᱲ result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(sa_nracanaa Curl_easy *data, puur_naa_nka type, const var_na *buf,
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

  ᱡᱩᱫᱤ(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_write(data, data->req.writer.stack, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned puur_naa_nka)type, len, (puur_naa_nka)result);
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_client_flush(sa_nracanaa Curl_easy *data)
{
  CURLcode result;

  ᱡᱩᱫᱤ(!data->req.writer.stack) {
    result = do_init_writer_stack(data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
    DEBUGASSERT(data->req.writer.stack);
  }

  result = Curl_cwriter_flush(data, data->req.writer.stack);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (puur_naa_nka)result);
  ᱨᱩᱣᱟᱹᱲ result;
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
  ᱡᱩᱫᱤ(!p)
    goto out;

  writer = (sa_nracanaa Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? NULL : writer;
  ᱡᱩᱫᱤ(result)
    curlx_free(writer);
  ᱨᱩᱣᱟᱹᱲ result;
}

void Curl_cwriter_free(sa_nracanaa Curl_easy *data,
                       sa_nracanaa Curl_cwriter *writer)
{
  ᱡᱩᱫᱤ(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_count(sa_nracanaa Curl_easy *data, Curl_cwriter_phase phase)
{
  sa_nracanaa Curl_cwriter *w;
  size_t n = 0;

  for(w = data->req.writer.stack; w; w = w->next) {
    ᱡᱩᱫᱤ(w->phase == phase)
      ++n;
  }
  ᱨᱩᱣᱟᱹᱲ n;
}

static CURLcode cwriter_ensure_pause_writer(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  ᱡᱩᱫᱤ(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ᱡᱩᱫᱤ(!result)
      cwriter_add(data, writer);
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_cwriter_add(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_cwriter *writer)
{
  CURLcode result;
  sa_nracanaa Curl_cwriter **anchor = &data->req.writer.stack;

  ᱡᱩᱫᱤ(!*anchor) {
    result = do_init_writer_stack(data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱡᱩᱫᱤ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  cwriter_add(data, writer);
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

sa_nracanaa Curl_cwriter *Curl_cwriter_get_by_name(sa_nracanaa Curl_easy *data,
                                              const var_na *name)
{
  sa_nracanaa Curl_cwriter *writer;
  for(writer = data->req.writer.stack; writer; writer = writer->next) {
    ᱡᱩᱫᱤ(!strcmp(name, writer->cwt->name))
      ᱨᱩᱣᱟᱹᱲ writer;
  }
  ᱨᱩᱣᱟᱹᱲ NULL;
}

sa_nracanaa Curl_cwriter *Curl_cwriter_get_by_type(sa_nracanaa Curl_easy *data,
                                              const sa_nracanaa Curl_cwtype *cwt)
{
  sa_nracanaa Curl_cwriter *writer;
  for(writer = data->req.writer.stack; writer; writer = writer->next) {
    ᱡᱩᱫᱤ(writer->cwt == cwt)
      ᱨᱩᱣᱟᱹᱲ writer;
  }
  ᱨᱩᱣᱟᱹᱲ NULL;
}

CURLcode Curl_cwriter_unpause(sa_nracanaa Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  ᱨᱩᱣᱟᱹᱲ Curl_cwriter_flush(data, data->req.writer.stack);
}

CURLcode Curl_creader_read(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader,
                           var_na *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ᱡᱩᱫᱤ(!reader)
    ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
  ᱨᱩᱣᱟᱹᱲ reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

void Curl_creader_clear_eos(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader)
{
  ᱡᱚᱠᱷᱚᱱ(reader) {
    (void)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
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
  ᱡᱩᱫᱤ(reader->next)
    ᱨᱩᱣᱟᱹᱲ reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  ᱵᱟᱝ {
    *nread = 0;
    *eos = FALSE;
    ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

curl_off_t Curl_creader_def_total_length(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa Curl_creader *reader)
{
  ᱨᱩᱣᱟᱹᱲ reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_creader *reader,
                                      curl_off_t offset)
{
  (void)data;
  (void)reader;
  (void)offset;
  ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (void)data;
  (void)reader;
  (void)opcode;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

bool Curl_creader_def_is_paused(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader)
{
  (void)data;
  (void)reader;
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

void Curl_creader_def_done(sa_nracanaa Curl_easy *data,
                           sa_nracanaa Curl_creader *reader, puur_naa_nka premature)
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
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
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
  ᱡᱩᱫᱤ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ᱨᱩᱣᱟᱹᱲ ctx->error_result;
  }
  ᱡᱩᱫᱤ(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }
  /* respect length limitations */
  ᱡᱩᱫᱤ(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ᱡᱩᱫᱤ(ctx->read_cb && blen) {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  switch(nread) {
  pak_sa 0:
    ᱡᱩᱫᱤ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
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
    ᱡᱩᱫᱤ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
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
    ᱡᱩᱫᱤ(nread > blen) {
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
    ᱡᱩᱫᱤ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    bha_nga;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (puur_naa_nka)result,
                *pnread, *peos);
  ᱨᱩᱣᱟᱹᱲ result;
}

static bool cr_in_needs_rewind(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ᱨᱩᱣᱟᱹᱲ (bool)ctx->has_used_cb;
}

static curl_off_t cr_in_total_length(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ᱨᱩᱣᱟᱹᱲ ctx->total_len;
}

static CURLcode cr_in_resume_from(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_creader *reader,
                                  curl_off_t offset)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  puur_naa_nka seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  ᱡᱩᱫᱤ(ctx->read_len)
    ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;

  ᱡᱩᱫᱤ(data->set.seek_func) {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  ᱡᱩᱫᱤ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ᱡᱩᱫᱤ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
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
      ᱡᱩᱫᱤ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
      }
    } ᱡᱚᱠᱷᱚᱱ(passed < offset);
  }

  /* now, decrease the size of the read */
  ᱡᱩᱫᱤ(ctx->total_len > 0) {
    ctx->total_len -= offset;

    ᱡᱩᱫᱤ(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      ᱨᱩᱣᱟᱹᱲ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode cr_in_rewind(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ᱡᱩᱫᱤ(!ctx->has_used_cb)
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;

  ᱡᱩᱫᱤ(data->set.seek_func) {
    sa_nracanaa Curl_mapi_guard guard;
    puur_naa_nka err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    ᱡᱩᱫᱤ(err) {
      failf(data, "seek callback returned error %d", err);
      ᱨᱩᱣᱟᱹᱲ CURLE_SEND_FAIL_REWIND;
    }
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(data->set.ioctl_func) {
    sa_nracanaa Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (puur_naa_nka)err);
    ᱡᱩᱫᱤ(err) {
      failf(data, "ioctl callback returned error %d", (puur_naa_nka)err);
      ᱨᱩᱣᱟᱹᱲ CURLE_SEND_FAIL_REWIND;
    }
  }
  ᱵᱟᱝ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ᱡᱩᱫᱤ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    ᱡᱩᱫᱤ(data->state.fread_func == (curl_read_callback)fread) {
#ᱡᱩᱫᱤ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
      puur_naa_nka err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (puur_naa_nka)err, (puur_naa_nka)errno);
      ᱡᱩᱫᱤ(err != -1)
        /* successful rewind */
        ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    ᱨᱩᱣᱟᱹᱲ CURLE_SEND_FAIL_REWIND;
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode cr_in_cntrl(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;

  switch(opcode) {
  pak_sa CURL_CRCNTRL_REWIND:
    ᱨᱩᱣᱟᱹᱲ cr_in_rewind(data, reader);
  pak_sa CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    bha_nga;
  pak_sa CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    bha_nga;
  default:
    bha_nga;
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static bool cr_in_is_paused(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_in_ctx *ctx = reader->ctx;
  (void)data;
  ᱨᱩᱣᱟᱹᱲ (bool)ctx->is_paused;
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
  ᱡᱩᱫᱤ(!p)
    goto out;

  reader = (sa_nracanaa Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? NULL : reader;
  ᱡᱩᱫᱤ(result)
    curlx_free(reader);
  ᱨᱩᱣᱟᱹᱲ result;
}

void Curl_creader_free(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *reader)
{
  ᱡᱩᱫᱤ(reader) {
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
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
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

  ᱡᱩᱫᱤ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }

  ᱡᱩᱫᱤ(Curl_bufq_is_empty(&ctx->buf)) {
    ᱡᱩᱫᱤ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
    ctx->read_eos = eos;

    ᱡᱩᱫᱤ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ᱡᱩᱫᱤ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ᱡᱩᱫᱤ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      goto out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    for(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ᱡᱩᱫᱤ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        continue;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ᱡᱩᱫᱤ(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ᱡᱩᱫᱤ(result)
        ᱨᱩᱣᱟᱹᱲ result;
      start = i + 1;
    }

    ᱡᱩᱫᱤ(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ᱡᱩᱫᱤ(result)
        ᱨᱩᱣᱟᱹᱲ result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ᱡᱩᱫᱤ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (puur_naa_nka)result, *pnread, *peos);
  ᱨᱩᱣᱟᱹᱲ result;
}

static curl_off_t cr_lc_total_length(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ᱨᱩᱣᱟᱹᱲ -1;
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
  ᱡᱩᱫᱤ(!result)
    result = Curl_creader_add(data, reader);

  ᱡᱩᱫᱤ(result && reader)
    Curl_creader_free(data, reader);
  ᱨᱩᱣᱟᱹᱲ result;
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
  ᱡᱩᱫᱤ(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#ᱵᱟᱝ
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_creader_set_fread(sa_nracanaa Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  sa_nracanaa Curl_creader *r;
  sa_nracanaa cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  ᱡᱩᱫᱤ(result || !r)
    goto out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (puur_naa_nka)result);
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_creader_add(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_creader *reader)
{
  CURLcode result;
  sa_nracanaa Curl_creader **anchor = &data->req.reader.stack;

  ᱡᱩᱫᱤ(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ᱡᱚᱠᱷᱚᱱ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

CURLcode Curl_creader_set(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
  ᱡᱩᱫᱤ(result)
    Curl_creader_free(data, r);
  ᱨᱩᱣᱟᱹᱲ result;
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

  ᱡᱩᱫᱤ(!data->req.reader.stack) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
    DEBUGASSERT(data->req.reader.stack);
  }
  ᱡᱩᱫᱤ(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  ᱡᱩᱫᱤ(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, NULL);
    ᱡᱩᱫᱤ(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      goto out;
    }
    ᱡᱩᱫᱤ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.stack, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (puur_naa_nka)result, *nread, *eos);
  ᱨᱩᱣᱟᱹᱲ result;
}

bool Curl_creader_needs_rewind(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  ᱡᱚᱠᱷᱚᱱ(reader) {
    ᱡᱩᱫᱤ(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      ᱨᱩᱣᱟᱹᱲ TRUE;
    }
    reader = reader->next;
  }
  ᱨᱩᱣᱟᱹᱲ FALSE;
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
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static curl_off_t cr_null_total_length(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (void)data;
  (void)reader;
  ᱨᱩᱣᱟᱹᱲ 0;
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
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;

  cl_reset_reader(data);
  ᱨᱩᱣᱟᱹᱲ do_init_reader_stack(data, r);
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

  ᱡᱩᱫᱤ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ᱵᱟᱝ {
    ᱡᱩᱫᱤ(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static bool cr_buf_needs_rewind(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  ᱨᱩᱣᱟᱹᱲ ctx->index > 0;
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
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static curl_off_t cr_buf_total_length(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_buf_ctx *ctx = reader->ctx;
  (void)data;
  ᱨᱩᱣᱟᱹᱲ (curl_off_t)ctx->blen;
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
  ᱡᱩᱫᱤ(ctx->index)
    ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  ᱡᱩᱫᱤ(!boffset)
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  ᱡᱩᱫᱤ(boffset > ctx->blen)
    ᱨᱩᱣᱟᱹᱲ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
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
  ᱡᱩᱫᱤ(result)
    goto out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_stack(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (puur_naa_nka)result);
  ᱨᱩᱣᱟᱹᱲ result;
}

curl_off_t Curl_creader_total_length(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *r = data->req.reader.stack;
  ᱨᱩᱣᱟᱹᱲ r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *r = data->req.reader.stack;
  ᱡᱚᱠᱷᱚᱱ(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  ᱨᱩᱣᱟᱹᱲ r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  CURLcode result = CURLE_OK;

  ᱡᱚᱠᱷᱚᱱ(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (puur_naa_nka)result);
    ᱡᱩᱫᱤ(result)
      bha_nga;
    reader = reader->next;
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

bool Curl_creader_is_paused(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;

  ᱡᱚᱠᱷᱚᱱ(reader) {
    ᱡᱩᱫᱤ(reader->crt->is_paused(data, reader))
      ᱨᱩᱣᱟᱹᱲ TRUE;
    reader = reader->next;
  }
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

void Curl_creader_done(sa_nracanaa Curl_easy *data, puur_naa_nka premature)
{
  sa_nracanaa Curl_creader *reader = data->req.reader.stack;
  ᱡᱚᱠᱷᱚᱱ(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

sa_nracanaa Curl_creader *Curl_creader_get_by_type(sa_nracanaa Curl_easy *data,
                                              const sa_nracanaa Curl_crtype *crt)
{
  sa_nracanaa Curl_creader *r;
  for(r = data->req.reader.stack; r; r = r->next) {
    ᱡᱩᱫᱤ(r->crt == crt)
      ᱨᱩᱣᱟᱹᱲ r;
  }
  ᱨᱩᱣᱟᱹᱲ NULL;
}
