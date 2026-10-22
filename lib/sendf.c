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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ଅନ୍ତର୍ଭାଵ <linux/tcp.h>
#ନଚେତଯଦି defined(HAVE_NETINET_TCP_H)
#ଅନ୍ତର୍ଭାଵ <netinet/tcp.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "cw-out.h"
#ଅନ୍ତର୍ଭାଵ "cw-pause.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"

ସ୍ଥିର ଶୂନ୍ୟ cl_reset_writer(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cwriter *writer = data->req.writer.ଗଦା;
  ଯେତେବେଳେ(writer) {
    data->req.writer.ଗଦା = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.ଗଦା;
  }
  data->req.writer.paused = FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ cl_reset_reader(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = data->req.reader.ଗଦା;
  data->req.reader_started = FALSE;
  ଯେତେବେଳେ(reader) {
    data->req.reader.ଗଦା = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.ଗଦା;
  }
}

ଶୂନ୍ୟ Curl_client_cleanup(ସଂରଚନା Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

ଶୂନ୍ୟ Curl_client_reset(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  ଅନ୍ୟଥା {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->req.rewind_read) {
    ସଂରଚନା Curl_creader *r = data->req.reader.ଗଦା;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    ଯେତେବେଳେ(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      ଯଦି(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_creader_will_rewind(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ (bool)data->req.rewind_read;
}

ଶୂନ୍ୟ Curl_creader_set_rewind(ସଂରଚନା Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_cwriter *writer)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)writer;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer)
{
  ଫେରନ୍ତୁ Curl_cwriter_flush(data, writer->next);
}

ଶୂନ୍ୟ Curl_cwriter_def_close(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_cwriter *writer)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)writer;
}

ସ୍ଥିର size_t get_max_body_write_len(ସଂରଚନା Curl_easy *data, curl_off_t limit)
{
  ଯଦି(limit != -1) {
    /* How much more are we allowed to write? */
    ଫେରନ୍ତୁ curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  ଫେରନ୍ତୁ SIZE_MAX;
}

ସଂରଚନା cw_download_ctx {
  ସଂରଚନା Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ସ୍ଥିର CURLcode cw_download_write(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                  ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ସଂରଚନା cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ଯଦି(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ଯଦି(!(type & CLIENTWRITE_BODY)) {
    ଯଦି(is_connect && data->set.suppress_connect_headers)
      ଫେରନ୍ତୁ CURLE_OK;
#ifdef DEBUGBUILD
    ଯଦି(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ଫେରନ୍ତୁ CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type, nbytes, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(!ctx->started_body &&
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

  ଯଦି(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type, nbytes);
    data->req.download_done = TRUE;
    ଯଦି(data->info.header_size)
      /* if headers have been received, this is fine */
      ଫେରନ୍ତୁ CURLE_OK;
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ଯଦି(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    ଯଦି(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ଯଦି(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    ଯଦି((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      ଫେରନ୍ତୁ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ଯଦି(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    ଯଦି(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ଯଦି(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type, nbytes, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Update stats, write and report progress */
  ଯଦି(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  ଯଦି(excess_len) {
    ଯଦି(!data->req.ignorebody) {
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
  ଅନ୍ୟଥା ଯଦି((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    ଫେରନ୍ତୁ CURLE_FILESIZE_EXCEEDED;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype cw_download = {
  "protocol",
  ଖାଲି,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ଆକାର(ସଂରଚନା cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ସ୍ଥିର CURLcode cw_raw_write(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                             ଅଚଳ ଅକ୍ଷର *buf, size_t nbytes)
{
  ଯଦି(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype cw_raw = {
  "raw",
  ଖାଲି,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ଆକାର(ସଂରଚନା Curl_cwriter)
};

ସ୍ଥିର ଶୂନ୍ୟ cwriter_add(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା Curl_cwriter *writer)
{
  ସଂରଚନା Curl_cwriter **anchor = &data->req.writer.ଗଦା;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  ଯେତେବେଳେ(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

ସ୍ଥିର CURLcode do_init_writer_ଗଦା(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.ଗଦା);
  result = Curl_cwriter_create(&data->req.writer.ଗଦା,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  cwriter_add(data, writer);

  ଫେରନ୍ତୁ result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *buf,
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

  ଯଦି(!data->req.writer.ଗଦା) {
    result = do_init_writer_ଗଦା(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    DEBUGASSERT(data->req.writer.ଗଦା);
  }

  result = Curl_cwriter_write(data, data->req.writer.ଗଦା, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_client_flush(ସଂରଚନା Curl_easy *data)
{
  CURLcode result;

  ଯଦି(!data->req.writer.ଗଦା) {
    result = do_init_writer_ଗଦା(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    DEBUGASSERT(data->req.writer.ଗଦା);
  }

  result = Curl_cwriter_flush(data, data->req.writer.ଗଦା);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ସଂରଚନା Curl_cwriter **pwriter,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ସଂରଚନା Curl_cwriter *writer = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ଶୂନ୍ୟ *p;

  DEBUGASSERT(cwt->cwriter_size >= ଆକାର(ସଂରଚନା Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  ଯଦି(!p)
    ଯାଅ out;

  writer = (ସଂରଚନା Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? ଖାଲି : writer;
  ଯଦି(result)
    curlx_free(writer);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_cwriter_free(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା Curl_cwriter *writer)
{
  ଯଦି(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ଗଣନା(ସଂରଚନା Curl_easy *data, Curl_cwriter_phase phase)
{
  ସଂରଚନା Curl_cwriter *w;
  size_t n = 0;

  ପାଇଁ(w = data->req.writer.ଗଦା; w; w = w->next) {
    ଯଦି(w->phase == phase)
      ++n;
  }
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର CURLcode cwriter_ensure_pause_writer(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  ଯଦି(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ଯଦି(!result)
      cwriter_add(data, writer);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cwriter_add(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_cwriter *writer)
{
  CURLcode result;
  ସଂରଚନା Curl_cwriter **anchor = &data->req.writer.ଗଦା;

  ଯଦି(!*anchor) {
    result = do_init_writer_ଗଦା(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  cwriter_add(data, writer);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା Curl_cwriter *Curl_cwriter_get_by_name(ସଂରଚନା Curl_easy *data,
                                              ଅଚଳ ଅକ୍ଷର *name)
{
  ସଂରଚନା Curl_cwriter *writer;
  ପାଇଁ(writer = data->req.writer.ଗଦା; writer; writer = writer->next) {
    ଯଦି(!strcmp(name, writer->cwt->name))
      ଫେରନ୍ତୁ writer;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସଂରଚନା Curl_cwriter *Curl_cwriter_get_by_type(ସଂରଚନା Curl_easy *data,
                                              ଅଚଳ ସଂରଚନା Curl_cwtype *cwt)
{
  ସଂରଚନା Curl_cwriter *writer;
  ପାଇଁ(writer = data->req.writer.ଗଦା; writer; writer = writer->next) {
    ଯଦି(writer->cwt == cwt)
      ଫେରନ୍ତୁ writer;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

CURLcode Curl_cwriter_unpause(ସଂରଚନା Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  ଫେରନ୍ତୁ Curl_cwriter_flush(data, data->req.writer.ଗଦା);
}

CURLcode Curl_creader_read(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader,
                           ଅକ୍ଷର *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ଯଦି(!reader)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  ଫେରନ୍ତୁ reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

ଶୂନ୍ୟ Curl_creader_clear_eos(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader)
{
  ଯେତେବେଳେ(reader) {
    (ଶୂନ୍ୟ)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_creader_def_close(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
}

CURLcode Curl_creader_def_read(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creader *reader,
                               ଅକ୍ଷର *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ଯଦି(reader->next)
    ଫେରନ୍ତୁ reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  ଅନ୍ୟଥା {
    *nread = 0;
    *eos = FALSE;
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ FALSE;
}

curl_off_t Curl_creader_def_total_length(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା Curl_creader *reader)
{
  ଫେରନ୍ତୁ reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା Curl_creader *reader,
                                      curl_off_t offset)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  (ଶୂନ୍ୟ)offset;
  ଫେରନ୍ତୁ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  (ଶୂନ୍ୟ)opcode;
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_creader_def_is_paused(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_creader *reader)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_creader_def_done(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader, ପୂର୍ଣ୍ଣସଂଖ୍ୟା premature)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  (ଶୂନ୍ୟ)premature;
}

ସଂରଚନା cr_in_ctx {
  ସଂରଚନା Curl_creader super;
  curl_read_callback read_cb;
  ଶୂନ୍ୟ *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

ସ୍ଥିର CURLcode cr_in_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ସ୍ଥିର CURLcode cr_in_read(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader,
                           ଅକ୍ଷର *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  ଯଦି(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ଫେରନ୍ତୁ ctx->error_result;
  }
  ଯଦି(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  /* respect length limitations */
  ଯଦି(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ଯଦି(ctx->read_cb && blen) {
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  ବାଛ(nread) {
  ସ୍ଥିତି 0:
    ଯଦି((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      ଭାଙ୍ଗ;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_READFUNC_PAUSE:
    ଯଦି(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      ଭାଙ୍ଗ;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ଭାଙ୍ଗ; /* nothing was read */

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      ଭାଙ୍ଗ;
    }
    ctx->read_len += nread;
    ଯଦି(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ଭାଙ୍ଗ;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                *pnread, *peos);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cr_in_needs_rewind(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ (bool)ctx->has_used_cb;
}

ସ୍ଥିର curl_off_t cr_in_total_length(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ctx->total_len;
}

ସ୍ଥିର CURLcode cr_in_resume_from(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_creader *reader,
                                  curl_off_t offset)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  ଯଦି(ctx->read_len)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;

  ଯଦି(data->set.seek_func) {
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  ଯଦି(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ଯଦି(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      ଫେରନ୍ତୁ CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    କର {
      ସଂରଚନା Curl_mapi_guard guard;
      ଅକ୍ଷର scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)ଆକାର(scratch)) ?
        ଆକାର(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      ଯଦି((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ଫେରନ୍ତୁ CURLE_READ_ERROR;
      }
    } ଯେତେବେଳେ(passed < offset);
  }

  /* now, decrease the size of the read */
  ଯଦି(ctx->total_len > 0) {
    ctx->total_len -= offset;

    ଯଦି(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      ଫେରନ୍ତୁ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cr_in_rewind(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ଯଦି(!ctx->has_used_cb)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(data->set.seek_func) {
    ସଂରଚନା Curl_mapi_guard guard;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    ଯଦି(err) {
      failf(data, "seek callback returned error %d", err);
      ଫେରନ୍ତୁ CURLE_SEND_FAIL_REWIND;
    }
  }
  ଅନ୍ୟଥା ଯଦି(data->set.ioctl_func) {
    ସଂରଚନା Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)err);
    ଯଦି(err) {
      failf(data, "ioctl callback returned error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)err);
      ଫେରନ୍ତୁ CURLE_SEND_FAIL_REWIND;
    }
  }
  ଅନ୍ୟଥା {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    ଯଦି(data->state.fread_func == (curl_read_callback)fread) {
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)err, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)errno);
      ଯଦି(err != -1)
        /* successful rewind */
        ଫେରନ୍ତୁ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    ଫେରନ୍ତୁ CURLE_SEND_FAIL_REWIND;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cr_in_cntrl(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;

  ବାଛ(opcode) {
  ସ୍ଥିତି CURL_CRCNTRL_REWIND:
    ଫେରନ୍ତୁ cr_in_rewind(data, reader);
  ସ୍ଥିତି CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool cr_in_is_paused(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_in_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ (bool)ctx->is_paused;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_in = {
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
  ଆକାର(ସଂରଚନା cr_in_ctx)
};

CURLcode Curl_creader_create(ସଂରଚନା Curl_creader **preader,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ସଂରଚନା Curl_creader *reader = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ଶୂନ୍ୟ *p;

  DEBUGASSERT(crt->creader_size >= ଆକାର(ସଂରଚନା Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  ଯଦି(!p)
    ଯାଅ out;

  reader = (ସଂରଚନା Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? ଖାଲି : reader;
  ଯଦି(result)
    curlx_free(reader);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_creader_free(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  ଯଦି(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

ସଂରଚନା cr_lc_ctx {
  ସଂରଚନା Curl_creader super;
  ସଂରଚନା bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

ସ୍ଥିର CURLcode cr_lc_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_lc_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_lc_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_lc_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ସ୍ଥିର CURLcode cr_lc_read(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader,
                           ଅକ୍ଷର *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  ଯଦି(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(Curl_bufq_is_empty(&ctx->buf)) {
    ଯଦି(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ctx->read_eos = eos;

    ଯଦି(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ଯଦି(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ଯଦି(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ଯାଅ out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    ପାଇଁ(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ଯଦି((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ଜାରି;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ଯଦି(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      start = i + 1;
    }

    ଯଦି(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ଯଦି(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread, *peos);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର curl_off_t cr_lc_total_length(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_lc = {
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
  ଆକାର(ସଂରଚନା cr_lc_ctx)
};

ସ୍ଥିର CURLcode cr_lc_add(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = ଖାଲି;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  ଯଦି(!result)
    result = Curl_creader_add(data, reader);

  ଯଦି(result && reader)
    Curl_creader_free(data, reader);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode do_init_reader_ଗଦା(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.ଗଦା);

  data->req.reader.ଗଦା = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  ଯଦି(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#ଅନ୍ୟଥା
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_creader_set_fread(ସଂରଚନା Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  ସଂରଚନା Curl_creader *r;
  ସଂରଚନା cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  ଯଦି(result || !r)
    ଯାଅ out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_ଗଦା(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_creader_add(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_creader *reader)
{
  CURLcode result;
  ସଂରଚନା Curl_creader **anchor = &data->req.reader.ଗଦା;

  ଯଦି(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ଯେତେବେଳେ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_creader_set(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_ଗଦା(data, r);
  ଯଦି(result)
    Curl_creader_free(data, r);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_client_read(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  ଯଦି(!data->req.reader.ଗଦା) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    DEBUGASSERT(data->req.reader.ଗଦା);
  }
  ଯଦି(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  ଯଦି(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, ଖାଲି);
    ଯଦି(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      ଯାଅ out;
    }
    ଯଦି(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.ଗଦା, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *nread, *eos);
  ଫେରନ୍ତୁ result;
}

bool Curl_creader_needs_rewind(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = data->req.reader.ଗଦା;
  ଯେତେବେଳେ(reader) {
    ଯଦି(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      ଫେରନ୍ତୁ TRUE;
    }
    reader = reader->next;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode cr_null_read(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader,
                             ଅକ୍ଷର *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)blen;
  *pnread = 0;
  *peos = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର curl_off_t cr_null_total_length(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_null = {
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
  ଆକାର(ସଂରଚନା Curl_creader)
};

CURLcode Curl_creader_set_null(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  cl_reset_reader(data);
  ଫେରନ୍ତୁ do_init_reader_ଗଦା(data, r);
}

ସଂରଚନା cr_buf_ctx {
  ସଂରଚନା Curl_creader super;
  ଅଚଳ ଅକ୍ଷର *buf;
  size_t blen;
  size_t index;
};

ସ୍ଥିର CURLcode cr_buf_read(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader,
                            ଅକ୍ଷର *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  ଯଦି(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ଅନ୍ୟଥା {
    ଯଦି(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool cr_buf_needs_rewind(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_buf_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ctx->index > 0;
}

ସ୍ଥିର CURLcode cr_buf_cntrl(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ସଂରଚନା cr_buf_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ବାଛ(opcode) {
  ସ୍ଥିତି CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର curl_off_t cr_buf_total_length(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_buf_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ (curl_off_t)ctx->blen;
}

ସ୍ଥିର CURLcode cr_buf_resume_from(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_creader *reader,
                                   curl_off_t offset)
{
  ସଂରଚନା cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  ଯଦି(ctx->index)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  ଯଦି(!boffset)
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(boffset > ctx->blen)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_buf = {
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
  ଆକାର(ସଂରଚନା cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  CURLcode result;
  ସଂରଚନା Curl_creader *r;
  ସଂରଚନା cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  ଯଦି(result)
    ଯାଅ out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_ଗଦା(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

curl_off_t Curl_creader_total_length(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = data->req.reader.ଗଦା;
  ଫେରନ୍ତୁ r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = data->req.reader.ଗଦା;
  ଯେତେବେଳେ(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  ଫେରନ୍ତୁ r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = data->req.reader.ଗଦା;
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    reader = reader->next;
  }
  ଫେରନ୍ତୁ result;
}

bool Curl_creader_is_paused(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = data->req.reader.ଗଦା;

  ଯେତେବେଳେ(reader) {
    ଯଦି(reader->crt->is_paused(data, reader))
      ଫେରନ୍ତୁ TRUE;
    reader = reader->next;
  }
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_creader_done(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା premature)
{
  ସଂରଚନା Curl_creader *reader = data->req.reader.ଗଦା;
  ଯେତେବେଳେ(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

ସଂରଚନା Curl_creader *Curl_creader_get_by_type(ସଂରଚନା Curl_easy *data,
                                              ଅଚଳ ସଂରଚନା Curl_crtype *crt)
{
  ସଂରଚନା Curl_creader *r;
  ପାଇଁ(r = data->req.reader.ଗଦା; r; r = r->next) {
    ଯଦି(r->crt == crt)
      ଫେରନ୍ତୁ r;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}
