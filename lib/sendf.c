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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#അന്തര്ഭാവ <linux/tcp.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_NETINET_TCP_H)
#അന്തര്ഭാവ <netinet/tcp.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "cw-out.h"
#അന്തര്ഭാവ "cw-pause.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"

സ്ഥിതം ശൂന്യം cl_reset_writer(ഘടന Curl_easy *data)
{
  ഘടന Curl_cwriter *writer = data->req.writer.അട്ടി;
  ആകുമ്പോൾ(writer) {
    data->req.writer.അട്ടി = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.അട്ടി;
  }
  data->req.writer.paused = FALSE;
}

സ്ഥിതം ശൂന്യം cl_reset_reader(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = data->req.reader.അട്ടി;
  data->req.reader_started = FALSE;
  ആകുമ്പോൾ(reader) {
    data->req.reader.അട്ടി = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.അട്ടി;
  }
}

ശൂന്യം Curl_client_cleanup(ഘടന Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

ശൂന്യം Curl_client_reset(ഘടന Curl_easy *data)
{
  എങ്കിൽ(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(ഘടന Curl_easy *data)
{
  എങ്കിൽ(data->req.rewind_read) {
    ഘടന Curl_creader *r = data->req.reader.അട്ടി;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    ആകുമ്പോൾ(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      എങ്കിൽ(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  തിരികെ CURLE_OK;
}

bool Curl_creader_will_rewind(ഘടന Curl_easy *data)
{
  തിരികെ (bool)data->req.rewind_read;
}

ശൂന്യം Curl_creader_set_rewind(ഘടന Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer)
{
  (ശൂന്യം)data;
  (ശൂന്യം)writer;
  തിരികെ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer)
{
  തിരികെ Curl_cwriter_flush(data, writer->next);
}

ശൂന്യം Curl_cwriter_def_close(ഘടന Curl_easy *data,
                            ഘടന Curl_cwriter *writer)
{
  (ശൂന്യം)data;
  (ശൂന്യം)writer;
}

സ്ഥിതം size_t get_max_body_write_len(ഘടന Curl_easy *data, curl_off_t limit)
{
  എങ്കിൽ(limit != -1) {
    /* How much more are we allowed to write? */
    തിരികെ curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  തിരികെ SIZE_MAX;
}

ഘടന cw_download_ctx {
  ഘടന Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
സ്ഥിതം CURLcode cw_download_write(ഘടന Curl_easy *data,
                                  ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                  സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  ഘടന cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  എങ്കിൽ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  എങ്കിൽ(!(type & CLIENTWRITE_BODY)) {
    എങ്കിൽ(is_connect && data->set.suppress_connect_headers)
      തിരികെ CURLE_OK;
#ifdef DEBUGBUILD
    എങ്കിൽ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      തിരികെ CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned പൂർണ്ണസംഖ്യ)type, nbytes, (പൂർണ്ണസംഖ്യ)result);
    തിരികെ result;
  }

  എങ്കിൽ(!ctx->started_body &&
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

  എങ്കിൽ(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned പൂർണ്ണസംഖ്യ)type, nbytes);
    data->req.download_done = TRUE;
    എങ്കിൽ(data->info.header_size)
      /* if headers have been received, this is fine */
      തിരികെ CURLE_OK;
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  എങ്കിൽ(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    എങ്കിൽ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    എങ്കിൽ(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    എങ്കിൽ((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      തിരികെ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  എങ്കിൽ(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    എങ്കിൽ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  എങ്കിൽ(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned പൂർണ്ണസംഖ്യ)type, nbytes, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Update stats, write and report progress */
  എങ്കിൽ(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  എങ്കിൽ(excess_len) {
    എങ്കിൽ(!data->req.ignorebody) {
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
  അല്ലെങ്കിൽ എങ്കിൽ((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    തിരികെ CURLE_FILESIZE_EXCEEDED;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype cw_download = {
  "protocol",
  ശൂന്യ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  വലുപ്പം(ഘടന cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
സ്ഥിതം CURLcode cw_raw_write(ഘടന Curl_easy *data,
                             ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                             സ്ഥിരം അക്ഷരം *buf, size_t nbytes)
{
  എങ്കിൽ(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  തിരികെ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype cw_raw = {
  "raw",
  ശൂന്യ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  വലുപ്പം(ഘടന Curl_cwriter)
};

സ്ഥിതം ശൂന്യം cwriter_add(ഘടന Curl_easy *data,
                        ഘടന Curl_cwriter *writer)
{
  ഘടന Curl_cwriter **anchor = &data->req.writer.അട്ടി;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  ആകുമ്പോൾ(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

സ്ഥിതം CURLcode do_init_writer_അട്ടി(ഘടന Curl_easy *data)
{
  ഘടന Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.അട്ടി);
  result = Curl_cwriter_create(&data->req.writer.അട്ടി,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  എങ്കിൽ(result)
    തിരികെ result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  എങ്കിൽ(result)
    തിരികെ result;
  cwriter_add(data, writer);

  തിരികെ result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *buf,
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

  എങ്കിൽ(!data->req.writer.അട്ടി) {
    result = do_init_writer_അട്ടി(data);
    എങ്കിൽ(result)
      തിരികെ result;
    DEBUGASSERT(data->req.writer.അട്ടി);
  }

  result = Curl_cwriter_write(data, data->req.writer.അട്ടി, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned പൂർണ്ണസംഖ്യ)type, len, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

CURLcode Curl_client_flush(ഘടന Curl_easy *data)
{
  CURLcode result;

  എങ്കിൽ(!data->req.writer.അട്ടി) {
    result = do_init_writer_അട്ടി(data);
    എങ്കിൽ(result)
      തിരികെ result;
    DEBUGASSERT(data->req.writer.അട്ടി);
  }

  result = Curl_cwriter_flush(data, data->req.writer.അട്ടി);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ഘടന Curl_cwriter **pwriter,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ഘടന Curl_cwriter *writer = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ശൂന്യം *p;

  DEBUGASSERT(cwt->cwriter_size >= വലുപ്പം(ഘടന Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  എങ്കിൽ(!p)
    പോകുക out;

  writer = (ഘടന Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? ശൂന്യ : writer;
  എങ്കിൽ(result)
    curlx_free(writer);
  തിരികെ result;
}

ശൂന്യം Curl_cwriter_free(ഘടന Curl_easy *data,
                       ഘടന Curl_cwriter *writer)
{
  എങ്കിൽ(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_എണ്ണം(ഘടന Curl_easy *data, Curl_cwriter_phase phase)
{
  ഘടന Curl_cwriter *w;
  size_t n = 0;

  വേണ്ടി(w = data->req.writer.അട്ടി; w; w = w->next) {
    എങ്കിൽ(w->phase == phase)
      ++n;
  }
  തിരികെ n;
}

സ്ഥിതം CURLcode cwriter_ensure_pause_writer(ഘടന Curl_easy *data)
{
  ഘടന Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    എങ്കിൽ(!result)
      cwriter_add(data, writer);
  }
  തിരികെ result;
}

CURLcode Curl_cwriter_add(ഘടന Curl_easy *data,
                          ഘടന Curl_cwriter *writer)
{
  CURLcode result;
  ഘടന Curl_cwriter **anchor = &data->req.writer.അട്ടി;

  എങ്കിൽ(!*anchor) {
    result = do_init_writer_അട്ടി(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  cwriter_add(data, writer);
  തിരികെ CURLE_OK;
}

ഘടന Curl_cwriter *Curl_cwriter_get_by_name(ഘടന Curl_easy *data,
                                              സ്ഥിരം അക്ഷരം *name)
{
  ഘടന Curl_cwriter *writer;
  വേണ്ടി(writer = data->req.writer.അട്ടി; writer; writer = writer->next) {
    എങ്കിൽ(!strcmp(name, writer->cwt->name))
      തിരികെ writer;
  }
  തിരികെ ശൂന്യ;
}

ഘടന Curl_cwriter *Curl_cwriter_get_by_type(ഘടന Curl_easy *data,
                                              സ്ഥിരം ഘടന Curl_cwtype *cwt)
{
  ഘടന Curl_cwriter *writer;
  വേണ്ടി(writer = data->req.writer.അട്ടി; writer; writer = writer->next) {
    എങ്കിൽ(writer->cwt == cwt)
      തിരികെ writer;
  }
  തിരികെ ശൂന്യ;
}

CURLcode Curl_cwriter_unpause(ഘടന Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  തിരികെ Curl_cwriter_flush(data, data->req.writer.അട്ടി);
}

CURLcode Curl_creader_read(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader,
                           അക്ഷരം *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  എങ്കിൽ(!reader)
    തിരികെ CURLE_READ_ERROR;
  തിരികെ reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

ശൂന്യം Curl_creader_clear_eos(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader)
{
  ആകുമ്പോൾ(reader) {
    (ശൂന്യം)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_creader_def_close(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
}

CURLcode Curl_creader_def_read(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader,
                               അക്ഷരം *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  എങ്കിൽ(reader->next)
    തിരികെ reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  അല്ലെങ്കിൽ {
    *nread = 0;
    *eos = FALSE;
    തിരികെ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ഘടന Curl_easy *data,
                                   ഘടന Curl_creader *reader)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ FALSE;
}

curl_off_t Curl_creader_def_total_length(ഘടന Curl_easy *data,
                                         ഘടന Curl_creader *reader)
{
  തിരികെ reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(ഘടന Curl_easy *data,
                                      ഘടന Curl_creader *reader,
                                      curl_off_t offset)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  (ശൂന്യം)offset;
  തിരികെ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  (ശൂന്യം)opcode;
  തിരികെ CURLE_OK;
}

bool Curl_creader_def_is_paused(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ FALSE;
}

ശൂന്യം Curl_creader_def_done(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader, പൂർണ്ണസംഖ്യ premature)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  (ശൂന്യം)premature;
}

ഘടന cr_in_ctx {
  ഘടന Curl_creader super;
  curl_read_callback read_cb;
  ശൂന്യം *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

സ്ഥിതം CURLcode cr_in_init(ഘടന Curl_easy *data, ഘടന Curl_creader *reader)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  തിരികെ CURLE_OK;
}

/* Real client reader to installed client callbacks. */
സ്ഥിതം CURLcode cr_in_read(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader,
                           അക്ഷരം *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  എങ്കിൽ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    തിരികെ ctx->error_result;
  }
  എങ്കിൽ(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    തിരികെ CURLE_OK;
  }
  /* respect length limitations */
  എങ്കിൽ(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  എങ്കിൽ(ctx->read_cb && blen) {
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  തിരഞ്ഞെടുപ്പ്(nread) {
  അവസ്ഥ 0:
    എങ്കിൽ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      നിർത്തുക;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    നിർത്തുക;

  അവസ്ഥ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    നിർത്തുക;

  അവസ്ഥ CURL_READFUNC_PAUSE:
    എങ്കിൽ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      നിർത്തുക;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    നിർത്തുക; /* nothing was read */

  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      നിർത്തുക;
    }
    ctx->read_len += nread;
    എങ്കിൽ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    നിർത്തുക;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (പൂർണ്ണസംഖ്യ)result,
                *pnread, *peos);
  തിരികെ result;
}

സ്ഥിതം bool cr_in_needs_rewind(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ (bool)ctx->has_used_cb;
}

സ്ഥിതം curl_off_t cr_in_total_length(ഘടന Curl_easy *data,
                                     ഘടന Curl_creader *reader)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ ctx->total_len;
}

സ്ഥിതം CURLcode cr_in_resume_from(ഘടന Curl_easy *data,
                                  ഘടന Curl_creader *reader,
                                  curl_off_t offset)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  പൂർണ്ണസംഖ്യ seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  എങ്കിൽ(ctx->read_len)
    തിരികെ CURLE_READ_ERROR;

  എങ്കിൽ(data->set.seek_func) {
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  എങ്കിൽ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    എങ്കിൽ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      തിരികെ CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    ചെയ്യുക {
      ഘടന Curl_mapi_guard guard;
      അക്ഷരം scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)വലുപ്പം(scratch)) ?
        വലുപ്പം(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      എങ്കിൽ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        തിരികെ CURLE_READ_ERROR;
      }
    } ആകുമ്പോൾ(passed < offset);
  }

  /* now, decrease the size of the read */
  എങ്കിൽ(ctx->total_len > 0) {
    ctx->total_len -= offset;

    എങ്കിൽ(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      തിരികെ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cr_in_rewind(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  എങ്കിൽ(!ctx->has_used_cb)
    തിരികെ CURLE_OK;

  എങ്കിൽ(data->set.seek_func) {
    ഘടന Curl_mapi_guard guard;
    പൂർണ്ണസംഖ്യ err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    എങ്കിൽ(err) {
      failf(data, "seek callback returned error %d", err);
      തിരികെ CURLE_SEND_FAIL_REWIND;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->set.ioctl_func) {
    ഘടന Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (പൂർണ്ണസംഖ്യ)err);
    എങ്കിൽ(err) {
      failf(data, "ioctl callback returned error %d", (പൂർണ്ണസംഖ്യ)err);
      തിരികെ CURLE_SEND_FAIL_REWIND;
    }
  }
  അല്ലെങ്കിൽ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    എങ്കിൽ(data->state.fread_func == (curl_read_callback)fread) {
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
      പൂർണ്ണസംഖ്യ err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (പൂർണ്ണസംഖ്യ)err, (പൂർണ്ണസംഖ്യ)errno);
      എങ്കിൽ(err != -1)
        /* successful rewind */
        തിരികെ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    തിരികെ CURLE_SEND_FAIL_REWIND;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cr_in_cntrl(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;

  തിരഞ്ഞെടുപ്പ്(opcode) {
  അവസ്ഥ CURL_CRCNTRL_REWIND:
    തിരികെ cr_in_rewind(data, reader);
  അവസ്ഥ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    നിർത്തുക;
  അവസ്ഥ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool cr_in_is_paused(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader)
{
  ഘടന cr_in_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ (bool)ctx->is_paused;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_in = {
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
  വലുപ്പം(ഘടന cr_in_ctx)
};

CURLcode Curl_creader_create(ഘടന Curl_creader **preader,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ഘടന Curl_creader *reader = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ശൂന്യം *p;

  DEBUGASSERT(crt->creader_size >= വലുപ്പം(ഘടന Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  എങ്കിൽ(!p)
    പോകുക out;

  reader = (ഘടന Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? ശൂന്യ : reader;
  എങ്കിൽ(result)
    curlx_free(reader);
  തിരികെ result;
}

ശൂന്യം Curl_creader_free(ഘടന Curl_easy *data, ഘടന Curl_creader *reader)
{
  എങ്കിൽ(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

ഘടന cr_lc_ctx {
  ഘടന Curl_creader super;
  ഘടന bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

സ്ഥിതം CURLcode cr_lc_init(ഘടന Curl_easy *data, ഘടന Curl_creader *reader)
{
  ഘടന cr_lc_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cr_lc_close(ഘടന Curl_easy *data, ഘടന Curl_creader *reader)
{
  ഘടന cr_lc_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
സ്ഥിതം CURLcode cr_lc_read(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader,
                           അക്ഷരം *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ഘടന cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  എങ്കിൽ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->buf)) {
    എങ്കിൽ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      തിരികെ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    എങ്കിൽ(result)
      തിരികെ result;
    ctx->read_eos = eos;

    എങ്കിൽ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      എങ്കിൽ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      എങ്കിൽ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      പോകുക out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    വേണ്ടി(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      എങ്കിൽ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        തുടരുക;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      എങ്കിൽ(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      എങ്കിൽ(result)
        തിരികെ result;
      start = i + 1;
    }

    എങ്കിൽ(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  എങ്കിൽ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (പൂർണ്ണസംഖ്യ)result, *pnread, *peos);
  തിരികെ result;
}

സ്ഥിതം curl_off_t cr_lc_total_length(ഘടന Curl_easy *data,
                                     ഘടന Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ -1;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_lc = {
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
  വലുപ്പം(ഘടന cr_lc_ctx)
};

സ്ഥിതം CURLcode cr_lc_add(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = ശൂന്യ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  എങ്കിൽ(!result)
    result = Curl_creader_add(data, reader);

  എങ്കിൽ(result && reader)
    Curl_creader_free(data, reader);
  തിരികെ result;
}

സ്ഥിതം CURLcode do_init_reader_അട്ടി(ഘടന Curl_easy *data,
                                     ഘടന Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.അട്ടി);

  data->req.reader.അട്ടി = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  എങ്കിൽ(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#അല്ലെങ്കിൽ
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ result;
}

CURLcode Curl_creader_set_fread(ഘടന Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  ഘടന Curl_creader *r;
  ഘടന cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  എങ്കിൽ(result || !r)
    പോകുക out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_അട്ടി(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

CURLcode Curl_creader_add(ഘടന Curl_easy *data,
                          ഘടന Curl_creader *reader)
{
  CURLcode result;
  ഘടന Curl_creader **anchor = &data->req.reader.അട്ടി;

  എങ്കിൽ(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ആകുമ്പോൾ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  തിരികെ CURLE_OK;
}

CURLcode Curl_creader_set(ഘടന Curl_easy *data, ഘടന Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_അട്ടി(data, r);
  എങ്കിൽ(result)
    Curl_creader_free(data, r);
  തിരികെ result;
}

CURLcode Curl_client_read(ഘടന Curl_easy *data, അക്ഷരം *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  എങ്കിൽ(!data->req.reader.അട്ടി) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    എങ്കിൽ(result)
      തിരികെ result;
    DEBUGASSERT(data->req.reader.അട്ടി);
  }
  എങ്കിൽ(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  എങ്കിൽ(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, ശൂന്യ);
    എങ്കിൽ(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      പോകുക out;
    }
    എങ്കിൽ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.അട്ടി, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (പൂർണ്ണസംഖ്യ)result, *nread, *eos);
  തിരികെ result;
}

bool Curl_creader_needs_rewind(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = data->req.reader.അട്ടി;
  ആകുമ്പോൾ(reader) {
    എങ്കിൽ(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      തിരികെ TRUE;
    }
    reader = reader->next;
  }
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode cr_null_read(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader,
                             അക്ഷരം *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  (ശൂന്യം)buf;
  (ശൂന്യം)blen;
  *pnread = 0;
  *peos = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം curl_off_t cr_null_total_length(ഘടന Curl_easy *data,
                                       ഘടന Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ശൂന്യം)data;
  (ശൂന്യം)reader;
  തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_null = {
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
  വലുപ്പം(ഘടന Curl_creader)
};

CURLcode Curl_creader_set_null(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  എങ്കിൽ(result)
    തിരികെ result;

  cl_reset_reader(data);
  തിരികെ do_init_reader_അട്ടി(data, r);
}

ഘടന cr_buf_ctx {
  ഘടന Curl_creader super;
  സ്ഥിരം അക്ഷരം *buf;
  size_t blen;
  size_t index;
};

സ്ഥിതം CURLcode cr_buf_read(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader,
                            അക്ഷരം *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ഘടന cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  എങ്കിൽ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool cr_buf_needs_rewind(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader)
{
  ഘടന cr_buf_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ ctx->index > 0;
}

സ്ഥിതം CURLcode cr_buf_cntrl(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ഘടന cr_buf_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരഞ്ഞെടുപ്പ്(opcode) {
  അവസ്ഥ CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം curl_off_t cr_buf_total_length(ഘടന Curl_easy *data,
                                      ഘടന Curl_creader *reader)
{
  ഘടന cr_buf_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ (curl_off_t)ctx->blen;
}

സ്ഥിതം CURLcode cr_buf_resume_from(ഘടന Curl_easy *data,
                                   ഘടന Curl_creader *reader,
                                   curl_off_t offset)
{
  ഘടന cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (ശൂന്യം)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  എങ്കിൽ(ctx->index)
    തിരികെ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  എങ്കിൽ(!boffset)
    തിരികെ CURLE_OK;
  എങ്കിൽ(boffset > ctx->blen)
    തിരികെ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_buf = {
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
  വലുപ്പം(ഘടന cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  CURLcode result;
  ഘടന Curl_creader *r;
  ഘടന cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_അട്ടി(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

curl_off_t Curl_creader_total_length(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = data->req.reader.അട്ടി;
  തിരികെ r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = data->req.reader.അട്ടി;
  ആകുമ്പോൾ(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  തിരികെ r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = data->req.reader.അട്ടി;
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      നിർത്തുക;
    reader = reader->next;
  }
  തിരികെ result;
}

bool Curl_creader_is_paused(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = data->req.reader.അട്ടി;

  ആകുമ്പോൾ(reader) {
    എങ്കിൽ(reader->crt->is_paused(data, reader))
      തിരികെ TRUE;
    reader = reader->next;
  }
  തിരികെ FALSE;
}

ശൂന്യം Curl_creader_done(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ premature)
{
  ഘടന Curl_creader *reader = data->req.reader.അട്ടി;
  ആകുമ്പോൾ(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

ഘടന Curl_creader *Curl_creader_get_by_type(ഘടന Curl_easy *data,
                                              സ്ഥിരം ഘടന Curl_crtype *crt)
{
  ഘടന Curl_creader *r;
  വേണ്ടി(r = data->req.reader.അട്ടി; r; r = r->next) {
    എങ്കിൽ(r->crt == crt)
      തിരികെ r;
  }
  തിരികെ ശൂന്യ;
}
