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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাৱ <linux/tcp.h>
#নোচেদ্যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাৱ <netinet/tcp.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "cw-out.h"
#অন্তর্ভাৱ "cw-pause.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"

স্থির শূন্য cl_reset_writer(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cwriter *writer = data->req.writer.স্তূপ;
  যেতিয়ালৈ(writer) {
    data->req.writer.স্তূপ = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.স্তূপ;
  }
  data->req.writer.paused = FALSE;
}

স্থির শূন্য cl_reset_reader(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = data->req.reader.স্তূপ;
  data->req.reader_started = FALSE;
  যেতিয়ালৈ(reader) {
    data->req.reader.স্তূপ = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.স্তূপ;
  }
}

শূন্য Curl_client_cleanup(সংরচনা Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

শূন্য Curl_client_reset(সংরচনা Curl_easy *data)
{
  যদি(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  অন্যথা {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(সংরচনা Curl_easy *data)
{
  যদি(data->req.rewind_read) {
    সংরচনা Curl_creader *r = data->req.reader.স্তূপ;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    যেতিয়ালৈ(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      যদি(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  ঘূৰাই CURLE_OK;
}

bool Curl_creader_will_rewind(সংরচনা Curl_easy *data)
{
  ঘূৰাই (bool)data->req.rewind_read;
}

শূন্য Curl_creader_set_rewind(সংরচনা Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(সংরচনা Curl_easy *data,
                               সংরচনা Curl_cwriter *writer)
{
  (শূন্য)data;
  (শূন্য)writer;
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_cwriter_def_write(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer)
{
  ঘূৰাই Curl_cwriter_flush(data, writer->next);
}

শূন্য Curl_cwriter_def_close(সংরচনা Curl_easy *data,
                            সংরচনা Curl_cwriter *writer)
{
  (শূন্য)data;
  (শূন্য)writer;
}

স্থির size_t get_max_body_write_len(সংরচনা Curl_easy *data, curl_off_t limit)
{
  যদি(limit != -1) {
    /* How much more are we allowed to write? */
    ঘূৰাই curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  ঘূৰাই SIZE_MAX;
}

সংরচনা cw_download_ctx {
  সংরচনা Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
স্থির CURLcode cw_download_write(সংরচনা Curl_easy *data,
                                  সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                  ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  সংরচনা cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  যদি(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  যদি(!(type & CLIENTWRITE_BODY)) {
    যদি(is_connect && data->set.suppress_connect_headers)
      ঘূৰাই CURLE_OK;
#ifdef DEBUGBUILD
    যদি(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ঘূৰাই CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned পূর্ণাঙ্ক)type, nbytes, (পূর্ণাঙ্ক)result);
    ঘূৰাই result;
  }

  যদি(!ctx->started_body &&
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

  যদি(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned পূর্ণাঙ্ক)type, nbytes);
    data->req.download_done = TRUE;
    যদি(data->info.header_size)
      /* if headers have been received, this is fine */
      ঘূৰাই CURLE_OK;
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  যদি(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    যদি(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    যদি(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    যদি((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      ঘূৰাই CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  যদি(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    যদি(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  যদি(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned পূর্ণাঙ্ক)type, nbytes, (পূর্ণাঙ্ক)result);
    যদি(result)
      ঘূৰাই result;
  }

  /* Update stats, write and report progress */
  যদি(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  যদি(excess_len) {
    যদি(!data->req.ignorebody) {
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
  অন্যথা যদি((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    ঘূৰাই CURLE_FILESIZE_EXCEEDED;
  }

  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype cw_download = {
  "protocol",
  রিক্ত,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  পরিমাণ(সংরচনা cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
স্থির CURLcode cw_raw_write(সংরচনা Curl_easy *data,
                             সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                             ধ্রুৱ ৱর্ণ *buf, size_t nbytes)
{
  যদি(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype cw_raw = {
  "raw",
  রিক্ত,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  পরিমাণ(সংরচনা Curl_cwriter)
};

স্থির শূন্য cwriter_add(সংরচনা Curl_easy *data,
                        সংরচনা Curl_cwriter *writer)
{
  সংরচনা Curl_cwriter **anchor = &data->req.writer.স্তূপ;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  যেতিয়ালৈ(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

স্থির CURLcode do_init_writer_স্তূপ(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.স্তূপ);
  result = Curl_cwriter_create(&data->req.writer.স্তূপ,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  যদি(result)
    ঘূৰাই result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  যদি(result)
    ঘূৰাই result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  যদি(result)
    ঘূৰাই result;
  cwriter_add(data, writer);

  ঘূৰাই result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(সংরচনা Curl_easy *data, পূর্ণাঙ্ক type, ধ্রুৱ ৱর্ণ *buf,
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

  যদি(!data->req.writer.স্তূপ) {
    result = do_init_writer_স্তূপ(data);
    যদি(result)
      ঘূৰাই result;
    DEBUGASSERT(data->req.writer.স্তূপ);
  }

  result = Curl_cwriter_write(data, data->req.writer.স্তূপ, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned পূর্ণাঙ্ক)type, len, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

CURLcode Curl_client_flush(সংরচনা Curl_easy *data)
{
  CURLcode result;

  যদি(!data->req.writer.স্তূপ) {
    result = do_init_writer_স্তূপ(data);
    যদি(result)
      ঘূৰাই result;
    DEBUGASSERT(data->req.writer.স্তূপ);
  }

  result = Curl_cwriter_flush(data, data->req.writer.স্তূপ);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(সংরচনা Curl_cwriter **pwriter,
                             সংরচনা Curl_easy *data,
                             ধ্রুৱ সংরচনা Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  সংরচনা Curl_cwriter *writer = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  শূন্য *p;

  DEBUGASSERT(cwt->cwriter_size >= পরিমাণ(সংরচনা Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  যদি(!p)
    গচ্ছ out;

  writer = (সংরচনা Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? রিক্ত : writer;
  যদি(result)
    curlx_free(writer);
  ঘূৰাই result;
}

শূন্য Curl_cwriter_free(সংরচনা Curl_easy *data,
                       সংরচনা Curl_cwriter *writer)
{
  যদি(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_গুনতি(সংরচনা Curl_easy *data, Curl_cwriter_phase phase)
{
  সংরচনা Curl_cwriter *w;
  size_t n = 0;

  বাবে(w = data->req.writer.স্তূপ; w; w = w->next) {
    যদি(w->phase == phase)
      ++n;
  }
  ঘূৰাই n;
}

স্থির CURLcode cwriter_ensure_pause_writer(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  যদি(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    যদি(!result)
      cwriter_add(data, writer);
  }
  ঘূৰাই result;
}

CURLcode Curl_cwriter_add(সংরচনা Curl_easy *data,
                          সংরচনা Curl_cwriter *writer)
{
  CURLcode result;
  সংরচনা Curl_cwriter **anchor = &data->req.writer.স্তূপ;

  যদি(!*anchor) {
    result = do_init_writer_স্তূপ(data);
    যদি(result)
      ঘূৰাই result;
  }

  যদি(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    যদি(result)
      ঘূৰাই result;
  }

  cwriter_add(data, writer);
  ঘূৰাই CURLE_OK;
}

সংরচনা Curl_cwriter *Curl_cwriter_get_by_name(সংরচনা Curl_easy *data,
                                              ধ্রুৱ ৱর্ণ *name)
{
  সংরচনা Curl_cwriter *writer;
  বাবে(writer = data->req.writer.স্তূপ; writer; writer = writer->next) {
    যদি(!strcmp(name, writer->cwt->name))
      ঘূৰাই writer;
  }
  ঘূৰাই রিক্ত;
}

সংরচনা Curl_cwriter *Curl_cwriter_get_by_type(সংরচনা Curl_easy *data,
                                              ধ্রুৱ সংরচনা Curl_cwtype *cwt)
{
  সংরচনা Curl_cwriter *writer;
  বাবে(writer = data->req.writer.স্তূপ; writer; writer = writer->next) {
    যদি(writer->cwt == cwt)
      ঘূৰাই writer;
  }
  ঘূৰাই রিক্ত;
}

CURLcode Curl_cwriter_unpause(সংরচনা Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  ঘূৰাই Curl_cwriter_flush(data, data->req.writer.স্তূপ);
}

CURLcode Curl_creader_read(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader,
                           ৱর্ণ *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  যদি(!reader)
    ঘূৰাই CURLE_READ_ERROR;
  ঘূৰাই reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

শূন্য Curl_creader_clear_eos(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader)
{
  যেতিয়ালৈ(reader) {
    (শূন্য)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_creader_def_close(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
}

CURLcode Curl_creader_def_read(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creader *reader,
                               ৱর্ণ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  যদি(reader->next)
    ঘূৰাই reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  অন্যথা {
    *nread = 0;
    *eos = FALSE;
    ঘূৰাই CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(সংরচনা Curl_easy *data,
                                   সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই FALSE;
}

curl_off_t Curl_creader_def_total_length(সংরচনা Curl_easy *data,
                                         সংরচনা Curl_creader *reader)
{
  ঘূৰাই reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(সংরচনা Curl_easy *data,
                                      সংরচনা Curl_creader *reader,
                                      curl_off_t offset)
{
  (শূন্য)data;
  (শূন্য)reader;
  (শূন্য)offset;
  ঘূৰাই CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(সংরচনা Curl_easy *data,
                                সংরচনা Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (শূন্য)data;
  (শূন্য)reader;
  (শূন্য)opcode;
  ঘূৰাই CURLE_OK;
}

bool Curl_creader_def_is_paused(সংরচনা Curl_easy *data,
                                সংরচনা Curl_creader *reader)
{
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই FALSE;
}

শূন্য Curl_creader_def_done(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader, পূর্ণাঙ্ক premature)
{
  (শূন্য)data;
  (শূন্য)reader;
  (শূন্য)premature;
}

সংরচনা cr_in_ctx {
  সংরচনা Curl_creader super;
  curl_read_callback read_cb;
  শূন্য *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

স্থির CURLcode cr_in_init(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  ঘূৰাই CURLE_OK;
}

/* Real client reader to installed client callbacks. */
স্থির CURLcode cr_in_read(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader,
                           ৱর্ণ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  যদি(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ঘূৰাই ctx->error_result;
  }
  যদি(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ঘূৰাই CURLE_OK;
  }
  /* respect length limitations */
  যদি(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  যদি(ctx->read_cb && blen) {
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  ৱিকল্প(nread) {
  পক্ষ 0:
    যদি((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      ভঙ্গ;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ভঙ্গ;

  পক্ষ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ভঙ্গ;

  পক্ষ CURL_READFUNC_PAUSE:
    যদি(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      ভঙ্গ;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ভঙ্গ; /* nothing was read */

  স্ৱভাৱ:
    যদি(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      ভঙ্গ;
    }
    ctx->read_len += nread;
    যদি(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ভঙ্গ;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (পূর্ণাঙ্ক)result,
                *pnread, *peos);
  ঘূৰাই result;
}

স্থির bool cr_in_needs_rewind(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creader *reader)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই (bool)ctx->has_used_cb;
}

স্থির curl_off_t cr_in_total_length(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_creader *reader)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই ctx->total_len;
}

স্থির CURLcode cr_in_resume_from(সংরচনা Curl_easy *data,
                                  সংরচনা Curl_creader *reader,
                                  curl_off_t offset)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  পূর্ণাঙ্ক seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  যদি(ctx->read_len)
    ঘূৰাই CURLE_READ_ERROR;

  যদি(data->set.seek_func) {
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  যদি(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      ঘূৰাই CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    কৰা {
      সংরচনা Curl_mapi_guard guard;
      ৱর্ণ scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)পরিমাণ(scratch)) ?
        পরিমাণ(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ঘূৰাই CURLE_READ_ERROR;
      }
    } যেতিয়ালৈ(passed < offset);
  }

  /* now, decrease the size of the read */
  যদি(ctx->total_len > 0) {
    ctx->total_len -= offset;

    যদি(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      ঘূৰাই CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cr_in_rewind(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  যদি(!ctx->has_used_cb)
    ঘূৰাই CURLE_OK;

  যদি(data->set.seek_func) {
    সংরচনা Curl_mapi_guard guard;
    পূর্ণাঙ্ক err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    যদি(err) {
      failf(data, "seek callback returned error %d", err);
      ঘূৰাই CURLE_SEND_FAIL_REWIND;
    }
  }
  অন্যথা যদি(data->set.ioctl_func) {
    সংরচনা Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (পূর্ণাঙ্ক)err);
    যদি(err) {
      failf(data, "ioctl callback returned error %d", (পূর্ণাঙ্ক)err);
      ঘূৰাই CURLE_SEND_FAIL_REWIND;
    }
  }
  অন্যথা {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    যদি(data->state.fread_func == (curl_read_callback)fread) {
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বেৰ_কৰো
#endif
      পূর্ণাঙ্ক err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (পূর্ণাঙ্ক)err, (পূর্ণাঙ্ক)errno);
      যদি(err != -1)
        /* successful rewind */
        ঘূৰাই CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    ঘূৰাই CURLE_SEND_FAIL_REWIND;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cr_in_cntrl(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;

  ৱিকল্প(opcode) {
  পক্ষ CURL_CRCNTRL_REWIND:
    ঘূৰাই cr_in_rewind(data, reader);
  পক্ষ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ভঙ্গ;
  পক্ষ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির bool cr_in_is_paused(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader)
{
  সংরচনা cr_in_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই (bool)ctx->is_paused;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_in = {
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
  পরিমাণ(সংরচনা cr_in_ctx)
};

CURLcode Curl_creader_create(সংরচনা Curl_creader **preader,
                             সংরচনা Curl_easy *data,
                             ধ্রুৱ সংরচনা Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  সংরচনা Curl_creader *reader = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  শূন্য *p;

  DEBUGASSERT(crt->creader_size >= পরিমাণ(সংরচনা Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  যদি(!p)
    গচ্ছ out;

  reader = (সংরচনা Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? রিক্ত : reader;
  যদি(result)
    curlx_free(reader);
  ঘূৰাই result;
}

শূন্য Curl_creader_free(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  যদি(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

সংরচনা cr_lc_ctx {
  সংরচনা Curl_creader super;
  সংরচনা bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

স্থির CURLcode cr_lc_init(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  সংরচনা cr_lc_ctx *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cr_lc_close(সংরচনা Curl_easy *data, সংরচনা Curl_creader *reader)
{
  সংরচনা cr_lc_ctx *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
স্থির CURLcode cr_lc_read(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader,
                           ৱর্ণ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  সংরচনা cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  যদি(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ঘূৰাই CURLE_OK;
  }

  যদি(Curl_bufq_is_empty(&ctx->buf)) {
    যদি(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ঘূৰাই CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    যদি(result)
      ঘূৰাই result;
    ctx->read_eos = eos;

    যদি(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      যদি(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      যদি(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      গচ্ছ out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    বাবে(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      যদি((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        অনুৱর্ত;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      যদি(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      যদি(result)
        ঘূৰাই result;
      start = i + 1;
    }

    যদি(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      যদি(result)
        ঘূৰাই result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  যদি(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণাঙ্ক)result, *pnread, *peos);
  ঘূৰাই result;
}

স্থির curl_off_t cr_lc_total_length(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই -1;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_lc = {
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
  পরিমাণ(সংরচনা cr_lc_ctx)
};

স্থির CURLcode cr_lc_add(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = রিক্ত;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  যদি(!result)
    result = Curl_creader_add(data, reader);

  যদি(result && reader)
    Curl_creader_free(data, reader);
  ঘূৰাই result;
}

স্থির CURLcode do_init_reader_স্তূপ(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.স্তূপ);

  data->req.reader.স্তূপ = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  যদি(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#অন্যথা
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    যদি(result)
      ঘূৰাই result;
  }

  ঘূৰাই result;
}

CURLcode Curl_creader_set_fread(সংরচনা Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  সংরচনা Curl_creader *r;
  সংরচনা cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  যদি(result || !r)
    গচ্ছ out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_স্তূপ(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

CURLcode Curl_creader_add(সংরচনা Curl_easy *data,
                          সংরচনা Curl_creader *reader)
{
  CURLcode result;
  সংরচনা Curl_creader **anchor = &data->req.reader.স্তূপ;

  যদি(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    যদি(result)
      ঘূৰাই result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  যেতিয়ালৈ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_creader_set(সংরচনা Curl_easy *data, সংরচনা Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_স্তূপ(data, r);
  যদি(result)
    Curl_creader_free(data, r);
  ঘূৰাই result;
}

CURLcode Curl_client_read(সংরচনা Curl_easy *data, ৱর্ণ *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  যদি(!data->req.reader.স্তূপ) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    যদি(result)
      ঘূৰাই result;
    DEBUGASSERT(data->req.reader.স্তূপ);
  }
  যদি(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  যদি(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, রিক্ত);
    যদি(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      গচ্ছ out;
    }
    যদি(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.স্তূপ, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণাঙ্ক)result, *nread, *eos);
  ঘূৰাই result;
}

bool Curl_creader_needs_rewind(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = data->req.reader.স্তূপ;
  যেতিয়ালৈ(reader) {
    যদি(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      ঘূৰাই TRUE;
    }
    reader = reader->next;
  }
  ঘূৰাই FALSE;
}

স্থির CURLcode cr_null_read(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader,
                             ৱর্ণ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (শূন্য)data;
  (শূন্য)reader;
  (শূন্য)buf;
  (শূন্য)blen;
  *pnread = 0;
  *peos = TRUE;
  ঘূৰাই CURLE_OK;
}

স্থির curl_off_t cr_null_total_length(সংরচনা Curl_easy *data,
                                       সংরচনা Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)data;
  (শূন্য)reader;
  ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_null = {
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
  পরিমাণ(সংরচনা Curl_creader)
};

CURLcode Curl_creader_set_null(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  যদি(result)
    ঘূৰাই result;

  cl_reset_reader(data);
  ঘূৰাই do_init_reader_স্তূপ(data, r);
}

সংরচনা cr_buf_ctx {
  সংরচনা Curl_creader super;
  ধ্রুৱ ৱর্ণ *buf;
  size_t blen;
  size_t index;
};

স্থির CURLcode cr_buf_read(সংরচনা Curl_easy *data,
                            সংরচনা Curl_creader *reader,
                            ৱর্ণ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  সংরচনা cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  যদি(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  অন্যথা {
    যদি(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ঘূৰাই CURLE_OK;
}

স্থির bool cr_buf_needs_rewind(সংরচনা Curl_easy *data,
                                সংরচনা Curl_creader *reader)
{
  সংরচনা cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই ctx->index > 0;
}

স্থির CURLcode cr_buf_cntrl(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  সংরচনা cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ৱিকল্প(opcode) {
  পক্ষ CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির curl_off_t cr_buf_total_length(সংরচনা Curl_easy *data,
                                      সংরচনা Curl_creader *reader)
{
  সংরচনা cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই (curl_off_t)ctx->blen;
}

স্থির CURLcode cr_buf_resume_from(সংরচনা Curl_easy *data,
                                   সংরচনা Curl_creader *reader,
                                   curl_off_t offset)
{
  সংরচনা cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (শূন্য)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  যদি(ctx->index)
    ঘূৰাই CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  যদি(!boffset)
    ঘূৰাই CURLE_OK;
  যদি(boffset > ctx->blen)
    ঘূৰাই CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_buf = {
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
  পরিমাণ(সংরচনা cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(সংরচনা Curl_easy *data,
                              ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  CURLcode result;
  সংরচনা Curl_creader *r;
  সংরচনা cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  যদি(result)
    গচ্ছ out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_স্তূপ(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

curl_off_t Curl_creader_total_length(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = data->req.reader.স্তূপ;
  ঘূৰাই r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = data->req.reader.স্তূপ;
  যেতিয়ালৈ(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  ঘূৰাই r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = data->req.reader.স্তূপ;
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (পূর্ণাঙ্ক)result);
    যদি(result)
      ভঙ্গ;
    reader = reader->next;
  }
  ঘূৰাই result;
}

bool Curl_creader_is_paused(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = data->req.reader.স্তূপ;

  যেতিয়ালৈ(reader) {
    যদি(reader->crt->is_paused(data, reader))
      ঘূৰাই TRUE;
    reader = reader->next;
  }
  ঘূৰাই FALSE;
}

শূন্য Curl_creader_done(সংরচনা Curl_easy *data, পূর্ণাঙ্ক premature)
{
  সংরচনা Curl_creader *reader = data->req.reader.স্তূপ;
  যেতিয়ালৈ(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

সংরচনা Curl_creader *Curl_creader_get_by_type(সংরচনা Curl_easy *data,
                                              ধ্রুৱ সংরচনা Curl_crtype *crt)
{
  সংরচনা Curl_creader *r;
  বাবে(r = data->req.reader.স্তূপ; r; r = r->next) {
    যদি(r->crt == crt)
      ঘূৰাই r;
  }
  ঘূৰাই রিক্ত;
}
