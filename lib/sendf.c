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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#அந்தர்பாவ <linux/tcp.h>
#அல்லதெனில் defined(HAVE_NETINET_TCP_H)
#அந்தர்பாவ <netinet/tcp.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "cw-out.h"
#அந்தர்பாவ "cw-pause.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"

நிலையான வெறுமை cl_reset_writer(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cwriter *writer = data->req.writer.அடுக்கு;
  போது(writer) {
    data->req.writer.அடுக்கு = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.அடுக்கு;
  }
  data->req.writer.paused = FALSE;
}

நிலையான வெறுமை cl_reset_reader(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = data->req.reader.அடுக்கு;
  data->req.reader_started = FALSE;
  போது(reader) {
    data->req.reader.அடுக்கு = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.அடுக்கு;
  }
}

வெறுமை Curl_client_cleanup(கட்டமைப்பு Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

வெறுமை Curl_client_reset(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  இல்லையெனில் {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data->req.rewind_read) {
    கட்டமைப்பு Curl_creader *r = data->req.reader.அடுக்கு;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    போது(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      எனில்(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (முழு)result);
        திரும்பு result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  திரும்பு CURLE_OK;
}

bool Curl_creader_will_rewind(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு (bool)data->req.rewind_read;
}

வெறுமை Curl_creader_set_rewind(கட்டமைப்பு Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_cwriter *writer)
{
  (வெறுமை)data;
  (வெறுமை)writer;
  திரும்பு CURLE_OK;
}

CURLcode Curl_cwriter_def_write(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                மாறிலி எழுத்து *buf, size_t nbytes)
{
  திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_cwriter *writer)
{
  திரும்பு Curl_cwriter_flush(data, writer->next);
}

வெறுமை Curl_cwriter_def_close(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_cwriter *writer)
{
  (வெறுமை)data;
  (வெறுமை)writer;
}

நிலையான size_t get_max_body_write_len(கட்டமைப்பு Curl_easy *data, curl_off_t limit)
{
  எனில்(limit != -1) {
    /* How much more are we allowed to write? */
    திரும்பு curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  திரும்பு SIZE_MAX;
}

கட்டமைப்பு cw_download_ctx {
  கட்டமைப்பு Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
நிலையான CURLcode cw_download_write(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                  மாறிலி எழுத்து *buf, size_t nbytes)
{
  கட்டமைப்பு cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  எனில்(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  எனில்(!(type & CLIENTWRITE_BODY)) {
    எனில்(is_connect && data->set.suppress_connect_headers)
      திரும்பு CURLE_OK;
#ifdef DEBUGBUILD
    எனில்(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      திரும்பு CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned முழு)type, nbytes, (முழு)result);
    திரும்பு result;
  }

  எனில்(!ctx->started_body &&
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

  எனில்(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned முழு)type, nbytes);
    data->req.download_done = TRUE;
    எனில்(data->info.header_size)
      /* if headers have been received, this is fine */
      திரும்பு CURLE_OK;
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  எனில்(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    எனில்(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    எனில்(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    எனில்((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      திரும்பு CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  எனில்(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    எனில்(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  எனில்(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned முழு)type, nbytes, (முழு)result);
    எனில்(result)
      திரும்பு result;
  }

  /* Update stats, write and report progress */
  எனில்(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  எனில்(excess_len) {
    எனில்(!data->req.ignorebody) {
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
  இல்லையெனில் எனில்((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    திரும்பு CURLE_FILESIZE_EXCEEDED;
  }

  திரும்பு CURLE_OK;
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype cw_download = {
  "protocol",
  வெற்று,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  அளவு(கட்டமைப்பு cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
நிலையான CURLcode cw_raw_write(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_cwriter *writer, முழு type,
                             மாறிலி எழுத்து *buf, size_t nbytes)
{
  எனில்(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  திரும்பு Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype cw_raw = {
  "raw",
  வெற்று,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  அளவு(கட்டமைப்பு Curl_cwriter)
};

நிலையான வெறுமை cwriter_add(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு Curl_cwriter *writer)
{
  கட்டமைப்பு Curl_cwriter **anchor = &data->req.writer.அடுக்கு;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  போது(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

நிலையான CURLcode do_init_writer_அடுக்கு(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.அடுக்கு);
  result = Curl_cwriter_create(&data->req.writer.அடுக்கு,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  எனில்(result)
    திரும்பு result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  எனில்(result)
    திரும்பு result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  எனில்(result)
    திரும்பு result;
  cwriter_add(data, writer);

  திரும்பு result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(கட்டமைப்பு Curl_easy *data, முழு type, மாறிலி எழுத்து *buf,
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

  எனில்(!data->req.writer.அடுக்கு) {
    result = do_init_writer_அடுக்கு(data);
    எனில்(result)
      திரும்பு result;
    DEBUGASSERT(data->req.writer.அடுக்கு);
  }

  result = Curl_cwriter_write(data, data->req.writer.அடுக்கு, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned முழு)type, len, (முழு)result);
  திரும்பு result;
}

CURLcode Curl_client_flush(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;

  எனில்(!data->req.writer.அடுக்கு) {
    result = do_init_writer_அடுக்கு(data);
    எனில்(result)
      திரும்பு result;
    DEBUGASSERT(data->req.writer.அடுக்கு);
  }

  result = Curl_cwriter_flush(data, data->req.writer.அடுக்கு);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (முழு)result);
  திரும்பு result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(கட்டமைப்பு Curl_cwriter **pwriter,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  கட்டமைப்பு Curl_cwriter *writer = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  வெறுமை *p;

  DEBUGASSERT(cwt->cwriter_size >= அளவு(கட்டமைப்பு Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  எனில்(!p)
    செல் out;

  writer = (கட்டமைப்பு Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? வெற்று : writer;
  எனில்(result)
    curlx_free(writer);
  திரும்பு result;
}

வெறுமை Curl_cwriter_free(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு Curl_cwriter *writer)
{
  எனில்(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_மொத்தம்(கட்டமைப்பு Curl_easy *data, Curl_cwriter_phase phase)
{
  கட்டமைப்பு Curl_cwriter *w;
  size_t n = 0;

  க்கு(w = data->req.writer.அடுக்கு; w; w = w->next) {
    எனில்(w->phase == phase)
      ++n;
  }
  திரும்பு n;
}

நிலையான CURLcode cwriter_ensure_pause_writer(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  எனில்(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    எனில்(!result)
      cwriter_add(data, writer);
  }
  திரும்பு result;
}

CURLcode Curl_cwriter_add(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_cwriter *writer)
{
  CURLcode result;
  கட்டமைப்பு Curl_cwriter **anchor = &data->req.writer.அடுக்கு;

  எனில்(!*anchor) {
    result = do_init_writer_அடுக்கு(data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    எனில்(result)
      திரும்பு result;
  }

  cwriter_add(data, writer);
  திரும்பு CURLE_OK;
}

கட்டமைப்பு Curl_cwriter *Curl_cwriter_get_by_name(கட்டமைப்பு Curl_easy *data,
                                              மாறிலி எழுத்து *name)
{
  கட்டமைப்பு Curl_cwriter *writer;
  க்கு(writer = data->req.writer.அடுக்கு; writer; writer = writer->next) {
    எனில்(!strcmp(name, writer->cwt->name))
      திரும்பு writer;
  }
  திரும்பு வெற்று;
}

கட்டமைப்பு Curl_cwriter *Curl_cwriter_get_by_type(கட்டமைப்பு Curl_easy *data,
                                              மாறிலி கட்டமைப்பு Curl_cwtype *cwt)
{
  கட்டமைப்பு Curl_cwriter *writer;
  க்கு(writer = data->req.writer.அடுக்கு; writer; writer = writer->next) {
    எனில்(writer->cwt == cwt)
      திரும்பு writer;
  }
  திரும்பு வெற்று;
}

CURLcode Curl_cwriter_unpause(கட்டமைப்பு Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  திரும்பு Curl_cwriter_flush(data, data->req.writer.அடுக்கு);
}

CURLcode Curl_creader_read(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader,
                           எழுத்து *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  எனில்(!reader)
    திரும்பு CURLE_READ_ERROR;
  திரும்பு reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

வெறுமை Curl_creader_clear_eos(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader)
{
  போது(reader) {
    (வெறுமை)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு CURLE_OK;
}

வெறுமை Curl_creader_def_close(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
}

CURLcode Curl_creader_def_read(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creader *reader,
                               எழுத்து *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  எனில்(reader->next)
    திரும்பு reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  இல்லையெனில் {
    *nread = 0;
    *eos = FALSE;
    திரும்பு CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு FALSE;
}

curl_off_t Curl_creader_def_total_length(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு Curl_creader *reader)
{
  திரும்பு reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு Curl_creader *reader,
                                      curl_off_t offset)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  (வெறுமை)offset;
  திரும்பு CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  (வெறுமை)opcode;
  திரும்பு CURLE_OK;
}

bool Curl_creader_def_is_paused(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_creader *reader)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு FALSE;
}

வெறுமை Curl_creader_def_done(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader, முழு premature)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  (வெறுமை)premature;
}

கட்டமைப்பு cr_in_ctx {
  கட்டமைப்பு Curl_creader super;
  curl_read_callback read_cb;
  வெறுமை *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

நிலையான CURLcode cr_in_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  திரும்பு CURLE_OK;
}

/* Real client reader to installed client callbacks. */
நிலையான CURLcode cr_in_read(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader,
                           எழுத்து *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  எனில்(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    திரும்பு ctx->error_result;
  }
  எனில்(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    திரும்பு CURLE_OK;
  }
  /* respect length limitations */
  எனில்(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  எனில்(ctx->read_cb && blen) {
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  தேர்வு(nread) {
  நிலை 0:
    எனில்((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      நிறுத்து;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    நிறுத்து;

  நிலை CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    நிறுத்து;

  நிலை CURL_READFUNC_PAUSE:
    எனில்(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      நிறுத்து;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    நிறுத்து; /* nothing was read */

  இயல்பு:
    எனில்(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      நிறுத்து;
    }
    ctx->read_len += nread;
    எனில்(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    நிறுத்து;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (முழு)result,
                *pnread, *peos);
  திரும்பு result;
}

நிலையான bool cr_in_needs_rewind(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு (bool)ctx->has_used_cb;
}

நிலையான curl_off_t cr_in_total_length(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு ctx->total_len;
}

நிலையான CURLcode cr_in_resume_from(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_creader *reader,
                                  curl_off_t offset)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  முழு seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  எனில்(ctx->read_len)
    திரும்பு CURLE_READ_ERROR;

  எனில்(data->set.seek_func) {
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  எனில்(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    எனில்(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      திரும்பு CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    செய் {
      கட்டமைப்பு Curl_mapi_guard guard;
      எழுத்து scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)அளவு(scratch)) ?
        அளவு(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      எனில்((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        திரும்பு CURLE_READ_ERROR;
      }
    } போது(passed < offset);
  }

  /* now, decrease the size of the read */
  எனில்(ctx->total_len > 0) {
    ctx->total_len -= offset;

    எனில்(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      திரும்பு CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cr_in_rewind(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  எனில்(!ctx->has_used_cb)
    திரும்பு CURLE_OK;

  எனில்(data->set.seek_func) {
    கட்டமைப்பு Curl_mapi_guard guard;
    முழு err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    எனில்(err) {
      failf(data, "seek callback returned error %d", err);
      திரும்பு CURLE_SEND_FAIL_REWIND;
    }
  }
  இல்லையெனில் எனில்(data->set.ioctl_func) {
    கட்டமைப்பு Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (முழு)err);
    எனில்(err) {
      failf(data, "ioctl callback returned error %d", (முழு)err);
      திரும்பு CURLE_SEND_FAIL_REWIND;
    }
  }
  இல்லையெனில் {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    எனில்(data->state.fread_func == (curl_read_callback)fread) {
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
      முழு err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (முழு)err, (முழு)errno);
      எனில்(err != -1)
        /* successful rewind */
        திரும்பு CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    திரும்பு CURLE_SEND_FAIL_REWIND;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cr_in_cntrl(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;

  தேர்வு(opcode) {
  நிலை CURL_CRCNTRL_REWIND:
    திரும்பு cr_in_rewind(data, reader);
  நிலை CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    நிறுத்து;
  நிலை CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான bool cr_in_is_paused(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_in_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு (bool)ctx->is_paused;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_in = {
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
  அளவு(கட்டமைப்பு cr_in_ctx)
};

CURLcode Curl_creader_create(கட்டமைப்பு Curl_creader **preader,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  கட்டமைப்பு Curl_creader *reader = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  வெறுமை *p;

  DEBUGASSERT(crt->creader_size >= அளவு(கட்டமைப்பு Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  எனில்(!p)
    செல் out;

  reader = (கட்டமைப்பு Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? வெற்று : reader;
  எனில்(result)
    curlx_free(reader);
  திரும்பு result;
}

வெறுமை Curl_creader_free(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  எனில்(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

கட்டமைப்பு cr_lc_ctx {
  கட்டமைப்பு Curl_creader super;
  கட்டமைப்பு bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

நிலையான CURLcode cr_lc_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_lc_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cr_lc_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_lc_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
நிலையான CURLcode cr_lc_read(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader,
                           எழுத்து *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  எனில்(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    திரும்பு CURLE_OK;
  }

  எனில்(Curl_bufq_is_empty(&ctx->buf)) {
    எனில்(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      திரும்பு CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    எனில்(result)
      திரும்பு result;
    ctx->read_eos = eos;

    எனில்(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      எனில்(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      எனில்(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      செல் out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    க்கு(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      எனில்((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        தொடர்;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      எனில்(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      எனில்(result)
        திரும்பு result;
      start = i + 1;
    }

    எனில்(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      எனில்(result)
        திரும்பு result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  எனில்(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (முழு)result, *pnread, *peos);
  திரும்பு result;
}

நிலையான curl_off_t cr_lc_total_length(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு -1;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_lc = {
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
  அளவு(கட்டமைப்பு cr_lc_ctx)
};

நிலையான CURLcode cr_lc_add(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = வெற்று;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  எனில்(!result)
    result = Curl_creader_add(data, reader);

  எனில்(result && reader)
    Curl_creader_free(data, reader);
  திரும்பு result;
}

நிலையான CURLcode do_init_reader_அடுக்கு(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.அடுக்கு);

  data->req.reader.அடுக்கு = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  எனில்(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#இல்லையெனில்
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    எனில்(result)
      திரும்பு result;
  }

  திரும்பு result;
}

CURLcode Curl_creader_set_fread(கட்டமைப்பு Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  கட்டமைப்பு Curl_creader *r;
  கட்டமைப்பு cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  எனில்(result || !r)
    செல் out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_அடுக்கு(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (முழு)result);
  திரும்பு result;
}

CURLcode Curl_creader_add(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_creader *reader)
{
  CURLcode result;
  கட்டமைப்பு Curl_creader **anchor = &data->req.reader.அடுக்கு;

  எனில்(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    எனில்(result)
      திரும்பு result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  போது(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  திரும்பு CURLE_OK;
}

CURLcode Curl_creader_set(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_அடுக்கு(data, r);
  எனில்(result)
    Curl_creader_free(data, r);
  திரும்பு result;
}

CURLcode Curl_client_read(கட்டமைப்பு Curl_easy *data, எழுத்து *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  எனில்(!data->req.reader.அடுக்கு) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    எனில்(result)
      திரும்பு result;
    DEBUGASSERT(data->req.reader.அடுக்கு);
  }
  எனில்(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  எனில்(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, வெற்று);
    எனில்(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      செல் out;
    }
    எனில்(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.அடுக்கு, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (முழு)result, *nread, *eos);
  திரும்பு result;
}

bool Curl_creader_needs_rewind(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = data->req.reader.அடுக்கு;
  போது(reader) {
    எனில்(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      திரும்பு TRUE;
    }
    reader = reader->next;
  }
  திரும்பு FALSE;
}

நிலையான CURLcode cr_null_read(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader,
                             எழுத்து *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (வெறுமை)data;
  (வெறுமை)reader;
  (வெறுமை)buf;
  (வெறுமை)blen;
  *pnread = 0;
  *peos = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான curl_off_t cr_null_total_length(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (வெறுமை)data;
  (வெறுமை)reader;
  திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_null = {
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
  அளவு(கட்டமைப்பு Curl_creader)
};

CURLcode Curl_creader_set_null(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  எனில்(result)
    திரும்பு result;

  cl_reset_reader(data);
  திரும்பு do_init_reader_அடுக்கு(data, r);
}

கட்டமைப்பு cr_buf_ctx {
  கட்டமைப்பு Curl_creader super;
  மாறிலி எழுத்து *buf;
  size_t blen;
  size_t index;
};

நிலையான CURLcode cr_buf_read(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_creader *reader,
                            எழுத்து *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  எனில்(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  இல்லையெனில் {
    எனில்(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  திரும்பு CURLE_OK;
}

நிலையான bool cr_buf_needs_rewind(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_buf_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு ctx->index > 0;
}

நிலையான CURLcode cr_buf_cntrl(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  கட்டமைப்பு cr_buf_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  தேர்வு(opcode) {
  நிலை CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான curl_off_t cr_buf_total_length(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_buf_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு (curl_off_t)ctx->blen;
}

நிலையான CURLcode cr_buf_resume_from(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_creader *reader,
                                   curl_off_t offset)
{
  கட்டமைப்பு cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (வெறுமை)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  எனில்(ctx->index)
    திரும்பு CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  எனில்(!boffset)
    திரும்பு CURLE_OK;
  எனில்(boffset > ctx->blen)
    திரும்பு CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  திரும்பு CURLE_OK;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_buf = {
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
  அளவு(கட்டமைப்பு cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *buf, size_t blen)
{
  CURLcode result;
  கட்டமைப்பு Curl_creader *r;
  கட்டமைப்பு cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  எனில்(result)
    செல் out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_அடுக்கு(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (முழு)result);
  திரும்பு result;
}

curl_off_t Curl_creader_total_length(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = data->req.reader.அடுக்கு;
  திரும்பு r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = data->req.reader.அடுக்கு;
  போது(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  திரும்பு r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = data->req.reader.அடுக்கு;
  CURLcode result = CURLE_OK;

  போது(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (முழு)result);
    எனில்(result)
      நிறுத்து;
    reader = reader->next;
  }
  திரும்பு result;
}

bool Curl_creader_is_paused(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = data->req.reader.அடுக்கு;

  போது(reader) {
    எனில்(reader->crt->is_paused(data, reader))
      திரும்பு TRUE;
    reader = reader->next;
  }
  திரும்பு FALSE;
}

வெறுமை Curl_creader_done(கட்டமைப்பு Curl_easy *data, முழு premature)
{
  கட்டமைப்பு Curl_creader *reader = data->req.reader.அடுக்கு;
  போது(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

கட்டமைப்பு Curl_creader *Curl_creader_get_by_type(கட்டமைப்பு Curl_easy *data,
                                              மாறிலி கட்டமைப்பு Curl_crtype *crt)
{
  கட்டமைப்பு Curl_creader *r;
  க்கு(r = data->req.reader.அடுக்கு; r; r = r->next) {
    எனில்(r->crt == crt)
      திரும்பு r;
  }
  திரும்பு வெற்று;
}
