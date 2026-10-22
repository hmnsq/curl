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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#અન્તર્ભાવ <linux/tcp.h>
#અથવાજો defined(HAVE_NETINET_TCP_H)
#અન્તર્ભાવ <netinet/tcp.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "cw-out.h"
#અન્તર્ભાવ "cw-pause.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"

સ્થિર શૂન્ય cl_reset_writer(સંરચના Curl_easy *data)
{
  સંરચના Curl_cwriter *writer = data->req.writer.ઢગલો;
  જ્યારે(writer) {
    data->req.writer.ઢગલો = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.ઢગલો;
  }
  data->req.writer.paused = FALSE;
}

સ્થિર શૂન્ય cl_reset_reader(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = data->req.reader.ઢગલો;
  data->req.reader_started = FALSE;
  જ્યારે(reader) {
    data->req.reader.ઢગલો = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.ઢગલો;
  }
}

શૂન્ય Curl_client_cleanup(સંરચના Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

શૂન્ય Curl_client_reset(સંરચના Curl_easy *data)
{
  જો(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  નહીંતર {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(સંરચના Curl_easy *data)
{
  જો(data->req.rewind_read) {
    સંરચના Curl_creader *r = data->req.reader.ઢગલો;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    જ્યારે(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      જો(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (પૂર્ણાંક)result);
        પાછા result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  પાછા CURLE_OK;
}

bool Curl_creader_will_rewind(સંરચના Curl_easy *data)
{
  પાછા (bool)data->req.rewind_read;
}

શૂન્ય Curl_creader_set_rewind(સંરચના Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(સંરચના Curl_easy *data,
                               સંરચના Curl_cwriter *writer)
{
  (શૂન્ય)data;
  (શૂન્ય)writer;
  પાછા CURLE_OK;
}

CURLcode Curl_cwriter_def_write(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                અચલ અક્ષર *buf, size_t nbytes)
{
  પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer)
{
  પાછા Curl_cwriter_flush(data, writer->next);
}

શૂન્ય Curl_cwriter_def_close(સંરચના Curl_easy *data,
                            સંરચના Curl_cwriter *writer)
{
  (શૂન્ય)data;
  (શૂન્ય)writer;
}

સ્થિર size_t get_max_body_write_len(સંરચના Curl_easy *data, curl_off_t limit)
{
  જો(limit != -1) {
    /* How much more are we allowed to write? */
    પાછા curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  પાછા SIZE_MAX;
}

સંરચના cw_download_ctx {
  સંરચના Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
સ્થિર CURLcode cw_download_write(સંરચના Curl_easy *data,
                                  સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                  અચલ અક્ષર *buf, size_t nbytes)
{
  સંરચના cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  જો(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  જો(!(type & CLIENTWRITE_BODY)) {
    જો(is_connect && data->set.suppress_connect_headers)
      પાછા CURLE_OK;
#ifdef DEBUGBUILD
    જો(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      પાછા CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned પૂર્ણાંક)type, nbytes, (પૂર્ણાંક)result);
    પાછા result;
  }

  જો(!ctx->started_body &&
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

  જો(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned પૂર્ણાંક)type, nbytes);
    data->req.download_done = TRUE;
    જો(data->info.header_size)
      /* if headers have been received, this is fine */
      પાછા CURLE_OK;
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  જો(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    જો(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    જો(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    જો((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      પાછા CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  જો(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    જો(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  જો(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned પૂર્ણાંક)type, nbytes, (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
  }

  /* Update stats, write and report progress */
  જો(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  જો(excess_len) {
    જો(!data->req.ignorebody) {
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
  નહીંતર જો((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    પાછા CURLE_FILESIZE_EXCEEDED;
  }

  પાછા CURLE_OK;
}

સ્થિર અચલ સંરચના Curl_cwtype cw_download = {
  "protocol",
  ખાલી,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  કદ(સંરચના cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
સ્થિર CURLcode cw_raw_write(સંરચના Curl_easy *data,
                             સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                             અચલ અક્ષર *buf, size_t nbytes)
{
  જો(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  પાછા Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

સ્થિર અચલ સંરચના Curl_cwtype cw_raw = {
  "raw",
  ખાલી,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  કદ(સંરચના Curl_cwriter)
};

સ્થિર શૂન્ય cwriter_add(સંરચના Curl_easy *data,
                        સંરચના Curl_cwriter *writer)
{
  સંરચના Curl_cwriter **anchor = &data->req.writer.ઢગલો;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  જ્યારે(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

સ્થિર CURLcode do_init_writer_ઢગલો(સંરચના Curl_easy *data)
{
  સંરચના Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.ઢગલો);
  result = Curl_cwriter_create(&data->req.writer.ઢગલો,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  જો(result)
    પાછા result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  જો(result)
    પાછા result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  જો(result)
    પાછા result;
  cwriter_add(data, writer);

  પાછા result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(સંરચના Curl_easy *data, પૂર્ણાંક type, અચલ અક્ષર *buf,
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

  જો(!data->req.writer.ઢગલો) {
    result = do_init_writer_ઢગલો(data);
    જો(result)
      પાછા result;
    DEBUGASSERT(data->req.writer.ઢગલો);
  }

  result = Curl_cwriter_write(data, data->req.writer.ઢગલો, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned પૂર્ણાંક)type, len, (પૂર્ણાંક)result);
  પાછા result;
}

CURLcode Curl_client_flush(સંરચના Curl_easy *data)
{
  CURLcode result;

  જો(!data->req.writer.ઢગલો) {
    result = do_init_writer_ઢગલો(data);
    જો(result)
      પાછા result;
    DEBUGASSERT(data->req.writer.ઢગલો);
  }

  result = Curl_cwriter_flush(data, data->req.writer.ઢગલો);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (પૂર્ણાંક)result);
  પાછા result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(સંરચના Curl_cwriter **pwriter,
                             સંરચના Curl_easy *data,
                             અચલ સંરચના Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  સંરચના Curl_cwriter *writer = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  શૂન્ય *p;

  DEBUGASSERT(cwt->cwriter_size >= કદ(સંરચના Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  જો(!p)
    જાઓ out;

  writer = (સંરચના Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? ખાલી : writer;
  જો(result)
    curlx_free(writer);
  પાછા result;
}

શૂન્ય Curl_cwriter_free(સંરચના Curl_easy *data,
                       સંરચના Curl_cwriter *writer)
{
  જો(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ગણતરી(સંરચના Curl_easy *data, Curl_cwriter_phase phase)
{
  સંરચના Curl_cwriter *w;
  size_t n = 0;

  માટે(w = data->req.writer.ઢગલો; w; w = w->next) {
    જો(w->phase == phase)
      ++n;
  }
  પાછા n;
}

સ્થિર CURLcode cwriter_ensure_pause_writer(સંરચના Curl_easy *data)
{
  સંરચના Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  જો(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    જો(!result)
      cwriter_add(data, writer);
  }
  પાછા result;
}

CURLcode Curl_cwriter_add(સંરચના Curl_easy *data,
                          સંરચના Curl_cwriter *writer)
{
  CURLcode result;
  સંરચના Curl_cwriter **anchor = &data->req.writer.ઢગલો;

  જો(!*anchor) {
    result = do_init_writer_ઢગલો(data);
    જો(result)
      પાછા result;
  }

  જો(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    જો(result)
      પાછા result;
  }

  cwriter_add(data, writer);
  પાછા CURLE_OK;
}

સંરચના Curl_cwriter *Curl_cwriter_get_by_name(સંરચના Curl_easy *data,
                                              અચલ અક્ષર *name)
{
  સંરચના Curl_cwriter *writer;
  માટે(writer = data->req.writer.ઢગલો; writer; writer = writer->next) {
    જો(!strcmp(name, writer->cwt->name))
      પાછા writer;
  }
  પાછા ખાલી;
}

સંરચના Curl_cwriter *Curl_cwriter_get_by_type(સંરચના Curl_easy *data,
                                              અચલ સંરચના Curl_cwtype *cwt)
{
  સંરચના Curl_cwriter *writer;
  માટે(writer = data->req.writer.ઢગલો; writer; writer = writer->next) {
    જો(writer->cwt == cwt)
      પાછા writer;
  }
  પાછા ખાલી;
}

CURLcode Curl_cwriter_unpause(સંરચના Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  પાછા Curl_cwriter_flush(data, data->req.writer.ઢગલો);
}

CURLcode Curl_creader_read(સંરચના Curl_easy *data,
                           સંરચના Curl_creader *reader,
                           અક્ષર *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  જો(!reader)
    પાછા CURLE_READ_ERROR;
  પાછા reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

શૂન્ય Curl_creader_clear_eos(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader)
{
  જ્યારે(reader) {
    (શૂન્ય)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(સંરચના Curl_easy *data,
                               સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા CURLE_OK;
}

શૂન્ય Curl_creader_def_close(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
}

CURLcode Curl_creader_def_read(સંરચના Curl_easy *data,
                               સંરચના Curl_creader *reader,
                               અક્ષર *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  જો(reader->next)
    પાછા reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  નહીંતર {
    *nread = 0;
    *eos = FALSE;
    પાછા CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(સંરચના Curl_easy *data,
                                   સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા FALSE;
}

curl_off_t Curl_creader_def_total_length(સંરચના Curl_easy *data,
                                         સંરચના Curl_creader *reader)
{
  પાછા reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(સંરચના Curl_easy *data,
                                      સંરચના Curl_creader *reader,
                                      curl_off_t offset)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  (શૂન્ય)offset;
  પાછા CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(સંરચના Curl_easy *data,
                                સંરચના Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  (શૂન્ય)opcode;
  પાછા CURLE_OK;
}

bool Curl_creader_def_is_paused(સંરચના Curl_easy *data,
                                સંરચના Curl_creader *reader)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા FALSE;
}

શૂન્ય Curl_creader_def_done(સંરચના Curl_easy *data,
                           સંરચના Curl_creader *reader, પૂર્ણાંક premature)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  (શૂન્ય)premature;
}

સંરચના cr_in_ctx {
  સંરચના Curl_creader super;
  curl_read_callback read_cb;
  શૂન્ય *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

સ્થિર CURLcode cr_in_init(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  પાછા CURLE_OK;
}

/* Real client reader to installed client callbacks. */
સ્થિર CURLcode cr_in_read(સંરચના Curl_easy *data,
                           સંરચના Curl_creader *reader,
                           અક્ષર *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  જો(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    પાછા ctx->error_result;
  }
  જો(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    પાછા CURLE_OK;
  }
  /* respect length limitations */
  જો(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  જો(ctx->read_cb && blen) {
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  પસંદગી(nread) {
  સ્થિતિ 0:
    જો((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      તોડો;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    તોડો;

  સ્થિતિ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    તોડો;

  સ્થિતિ CURL_READFUNC_PAUSE:
    જો(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      તોડો;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    તોડો; /* nothing was read */

  મૂળભૂત:
    જો(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      તોડો;
    }
    ctx->read_len += nread;
    જો(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    તોડો;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (પૂર્ણાંક)result,
                *pnread, *peos);
  પાછા result;
}

સ્થિર bool cr_in_needs_rewind(સંરચના Curl_easy *data,
                               સંરચના Curl_creader *reader)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા (bool)ctx->has_used_cb;
}

સ્થિર curl_off_t cr_in_total_length(સંરચના Curl_easy *data,
                                     સંરચના Curl_creader *reader)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા ctx->total_len;
}

સ્થિર CURLcode cr_in_resume_from(સંરચના Curl_easy *data,
                                  સંરચના Curl_creader *reader,
                                  curl_off_t offset)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  પૂર્ણાંક seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  જો(ctx->read_len)
    પાછા CURLE_READ_ERROR;

  જો(data->set.seek_func) {
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  જો(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    જો(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      પાછા CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    કરો {
      સંરચના Curl_mapi_guard guard;
      અક્ષર scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)કદ(scratch)) ?
        કદ(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      જો((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        પાછા CURLE_READ_ERROR;
      }
    } જ્યારે(passed < offset);
  }

  /* now, decrease the size of the read */
  જો(ctx->total_len > 0) {
    ctx->total_len -= offset;

    જો(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      પાછા CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  પાછા CURLE_OK;
}

સ્થિર CURLcode cr_in_rewind(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  જો(!ctx->has_used_cb)
    પાછા CURLE_OK;

  જો(data->set.seek_func) {
    સંરચના Curl_mapi_guard guard;
    પૂર્ણાંક err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    જો(err) {
      failf(data, "seek callback returned error %d", err);
      પાછા CURLE_SEND_FAIL_REWIND;
    }
  }
  નહીંતર જો(data->set.ioctl_func) {
    સંરચના Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (પૂર્ણાંક)err);
    જો(err) {
      failf(data, "ioctl callback returned error %d", (પૂર્ણાંક)err);
      પાછા CURLE_SEND_FAIL_REWIND;
    }
  }
  નહીંતર {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    જો(data->state.fread_func == (curl_read_callback)fread) {
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
      પૂર્ણાંક err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (પૂર્ણાંક)err, (પૂર્ણાંક)errno);
      જો(err != -1)
        /* successful rewind */
        પાછા CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    પાછા CURLE_SEND_FAIL_REWIND;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cr_in_cntrl(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;

  પસંદગી(opcode) {
  સ્થિતિ CURL_CRCNTRL_REWIND:
    પાછા cr_in_rewind(data, reader);
  સ્થિતિ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    તોડો;
  સ્થિતિ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર bool cr_in_is_paused(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader)
{
  સંરચના cr_in_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા (bool)ctx->is_paused;
}

સ્થિર અચલ સંરચના Curl_crtype cr_in = {
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
  કદ(સંરચના cr_in_ctx)
};

CURLcode Curl_creader_create(સંરચના Curl_creader **preader,
                             સંરચના Curl_easy *data,
                             અચલ સંરચના Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  સંરચના Curl_creader *reader = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  શૂન્ય *p;

  DEBUGASSERT(crt->creader_size >= કદ(સંરચના Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  જો(!p)
    જાઓ out;

  reader = (સંરચના Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? ખાલી : reader;
  જો(result)
    curlx_free(reader);
  પાછા result;
}

શૂન્ય Curl_creader_free(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  જો(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

સંરચના cr_lc_ctx {
  સંરચના Curl_creader super;
  સંરચના bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

સ્થિર CURLcode cr_lc_init(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  સંરચના cr_lc_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cr_lc_close(સંરચના Curl_easy *data, સંરચના Curl_creader *reader)
{
  સંરચના cr_lc_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
સ્થિર CURLcode cr_lc_read(સંરચના Curl_easy *data,
                           સંરચના Curl_creader *reader,
                           અક્ષર *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  સંરચના cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  જો(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    પાછા CURLE_OK;
  }

  જો(Curl_bufq_is_empty(&ctx->buf)) {
    જો(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      પાછા CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    જો(result)
      પાછા result;
    ctx->read_eos = eos;

    જો(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      જો(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      જો(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      જાઓ out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    માટે(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      જો((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ચાલુ;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      જો(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      જો(result)
        પાછા result;
      start = i + 1;
    }

    જો(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      જો(result)
        પાછા result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  જો(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (પૂર્ણાંક)result, *pnread, *peos);
  પાછા result;
}

સ્થિર curl_off_t cr_lc_total_length(સંરચના Curl_easy *data,
                                     સંરચના Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા -1;
}

સ્થિર અચલ સંરચના Curl_crtype cr_lc = {
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
  કદ(સંરચના cr_lc_ctx)
};

સ્થિર CURLcode cr_lc_add(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = ખાલી;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  જો(!result)
    result = Curl_creader_add(data, reader);

  જો(result && reader)
    Curl_creader_free(data, reader);
  પાછા result;
}

સ્થિર CURLcode do_init_reader_ઢગલો(સંરચના Curl_easy *data,
                                     સંરચના Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.ઢગલો);

  data->req.reader.ઢગલો = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  જો(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#નહીંતર
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    જો(result)
      પાછા result;
  }

  પાછા result;
}

CURLcode Curl_creader_set_fread(સંરચના Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  સંરચના Curl_creader *r;
  સંરચના cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  જો(result || !r)
    જાઓ out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_ઢગલો(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (પૂર્ણાંક)result);
  પાછા result;
}

CURLcode Curl_creader_add(સંરચના Curl_easy *data,
                          સંરચના Curl_creader *reader)
{
  CURLcode result;
  સંરચના Curl_creader **anchor = &data->req.reader.ઢગલો;

  જો(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    જો(result)
      પાછા result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  જ્યારે(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  પાછા CURLE_OK;
}

CURLcode Curl_creader_set(સંરચના Curl_easy *data, સંરચના Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_ઢગલો(data, r);
  જો(result)
    Curl_creader_free(data, r);
  પાછા result;
}

CURLcode Curl_client_read(સંરચના Curl_easy *data, અક્ષર *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  જો(!data->req.reader.ઢગલો) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    જો(result)
      પાછા result;
    DEBUGASSERT(data->req.reader.ઢગલો);
  }
  જો(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  જો(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, ખાલી);
    જો(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      જાઓ out;
    }
    જો(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.ઢગલો, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (પૂર્ણાંક)result, *nread, *eos);
  પાછા result;
}

bool Curl_creader_needs_rewind(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = data->req.reader.ઢગલો;
  જ્યારે(reader) {
    જો(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      પાછા TRUE;
    }
    reader = reader->next;
  }
  પાછા FALSE;
}

સ્થિર CURLcode cr_null_read(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader,
                             અક્ષર *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (શૂન્ય)data;
  (શૂન્ય)reader;
  (શૂન્ય)buf;
  (શૂન્ય)blen;
  *pnread = 0;
  *peos = TRUE;
  પાછા CURLE_OK;
}

સ્થિર curl_off_t cr_null_total_length(સંરચના Curl_easy *data,
                                       સંરચના Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (શૂન્ય)data;
  (શૂન્ય)reader;
  પાછા 0;
}

સ્થિર અચલ સંરચના Curl_crtype cr_null = {
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
  કદ(સંરચના Curl_creader)
};

CURLcode Curl_creader_set_null(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  જો(result)
    પાછા result;

  cl_reset_reader(data);
  પાછા do_init_reader_ઢગલો(data, r);
}

સંરચના cr_buf_ctx {
  સંરચના Curl_creader super;
  અચલ અક્ષર *buf;
  size_t blen;
  size_t index;
};

સ્થિર CURLcode cr_buf_read(સંરચના Curl_easy *data,
                            સંરચના Curl_creader *reader,
                            અક્ષર *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  સંરચના cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  જો(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  નહીંતર {
    જો(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  પાછા CURLE_OK;
}

સ્થિર bool cr_buf_needs_rewind(સંરચના Curl_easy *data,
                                સંરચના Curl_creader *reader)
{
  સંરચના cr_buf_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા ctx->index > 0;
}

સ્થિર CURLcode cr_buf_cntrl(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  સંરચના cr_buf_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પસંદગી(opcode) {
  સ્થિતિ CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર curl_off_t cr_buf_total_length(સંરચના Curl_easy *data,
                                      સંરચના Curl_creader *reader)
{
  સંરચના cr_buf_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા (curl_off_t)ctx->blen;
}

સ્થિર CURLcode cr_buf_resume_from(સંરચના Curl_easy *data,
                                   સંરચના Curl_creader *reader,
                                   curl_off_t offset)
{
  સંરચના cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (શૂન્ય)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  જો(ctx->index)
    પાછા CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  જો(!boffset)
    પાછા CURLE_OK;
  જો(boffset > ctx->blen)
    પાછા CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  પાછા CURLE_OK;
}

સ્થિર અચલ સંરચના Curl_crtype cr_buf = {
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
  કદ(સંરચના cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(સંરચના Curl_easy *data,
                              અચલ અક્ષર *buf, size_t blen)
{
  CURLcode result;
  સંરચના Curl_creader *r;
  સંરચના cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  જો(result)
    જાઓ out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_ઢગલો(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (પૂર્ણાંક)result);
  પાછા result;
}

curl_off_t Curl_creader_total_length(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *r = data->req.reader.ઢગલો;
  પાછા r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *r = data->req.reader.ઢગલો;
  જ્યારે(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  પાછા r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = data->req.reader.ઢગલો;
  CURLcode result = CURLE_OK;

  જ્યારે(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (પૂર્ણાંક)result);
    જો(result)
      તોડો;
    reader = reader->next;
  }
  પાછા result;
}

bool Curl_creader_is_paused(સંરચના Curl_easy *data)
{
  સંરચના Curl_creader *reader = data->req.reader.ઢગલો;

  જ્યારે(reader) {
    જો(reader->crt->is_paused(data, reader))
      પાછા TRUE;
    reader = reader->next;
  }
  પાછા FALSE;
}

શૂન્ય Curl_creader_done(સંરચના Curl_easy *data, પૂર્ણાંક premature)
{
  સંરચના Curl_creader *reader = data->req.reader.ઢગલો;
  જ્યારે(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

સંરચના Curl_creader *Curl_creader_get_by_type(સંરચના Curl_easy *data,
                                              અચલ સંરચના Curl_crtype *crt)
{
  સંરચના Curl_creader *r;
  માટે(r = data->req.reader.ઢગલો; r; r = r->next) {
    જો(r->crt == crt)
      પાછા r;
  }
  પાછા ખાલી;
}
