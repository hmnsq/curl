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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "cw-out.h"
#ಅನ್ತರ್ಭಾವ "cw-pause.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"

ಸ್ಥಿರ ಶೂನ್ಯ cl_reset_writer(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cwriter *writer = data->req.writer.ರಾಶಿ;
  ಯಾವತ್(writer) {
    data->req.writer.ರಾಶಿ = writer->next;
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
    writer = data->req.writer.ರಾಶಿ;
  }
  data->req.writer.paused = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cl_reset_reader(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = data->req.reader.ರಾಶಿ;
  data->req.reader_started = FALSE;
  ಯಾವತ್(reader) {
    data->req.reader.ರಾಶಿ = reader->next;
    reader->crt->do_close(data, reader);
    curlx_free(reader);
    reader = data->req.reader.ರಾಶಿ;
  }
}

ಶೂನ್ಯ Curl_client_cleanup(ಸಂರಚನಾ Curl_easy *data)
{
  cl_reset_reader(data);
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

ಶೂನ್ಯ Curl_client_reset(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(data, "client_reset, will rewind reader");
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_READ(data, "client_reset, clear readers");
    cl_reset_reader(data);
  }
  cl_reset_writer(data);

  data->req.bytecount = 0;
  data->req.headerline = 0;
}

CURLcode Curl_client_start(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->req.rewind_read) {
    ಸಂರಚನಾ Curl_creader *r = data->req.reader.ರಾಶಿ;
    CURLcode result = CURLE_OK;

    CURL_TRC_READ(data, "client start, rewind readers");
    ಯಾವತ್(r) {
      result = r->crt->cntrl(data, r, CURL_CRCNTRL_REWIND);
      ಯದಿ(result) {
        failf(data, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      r = r->next;
    }
    data->req.rewind_read = FALSE;
    cl_reset_reader(data);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

bool Curl_creader_will_rewind(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ (bool)data->req.rewind_read;
}

ಶೂನ್ಯ Curl_creader_set_rewind(ಸಂರಚನಾ Curl_easy *data, bool enable)
{
  data->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)writer;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಪ್ರತ್ಯಾಗಮ Curl_cwriter_flush(data, writer->next);
}

ಶೂನ್ಯ Curl_cwriter_def_close(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)writer;
}

ಸ್ಥಿರ size_t get_max_body_write_len(ಸಂರಚನಾ Curl_easy *data, curl_off_t limit)
{
  ಯದಿ(limit != -1) {
    /* How much more are we allowed to write? */
    ಪ್ರತ್ಯಾಗಮ curlx_sotouz_range(limit - data->req.bytecount, 0, SIZE_MAX);
  }
  ಪ್ರತ್ಯಾಗಮ SIZE_MAX;
}

ಸಂರಚನಾ cw_download_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ಸ್ಥಿರ CURLcode cw_download_write(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                  ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಸಂರಚನಾ cw_download_ctx *ctx = writer->ctx;
  CURLcode result;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ಯದಿ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || data->req.upload_done)) {
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ಯದಿ(!(type & CLIENTWRITE_BODY)) {
    ಯದಿ(is_connect && data->set.suppress_connect_headers)
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#ifdef DEBUGBUILD
    ಯದಿ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
    result = Curl_cwriter_write(data, writer->next, type, buf, nbytes);
    CURL_TRC_WRITE(data, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned ಪೂರ್ಣಾಙ್ಕ)type, nbytes, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(!ctx->started_body &&
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

  ಯದಿ(data->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(data->conn);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned ಪೂರ್ಣಾಙ್ಕ)type, nbytes);
    data->req.download_done = TRUE;
    ಯದಿ(data->info.header_size)
      /* if headers have been received, this is fine */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ಯದಿ(data->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(data, data->req.maxdownload);
    ಯದಿ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ಯದಿ(nwrite == wmax) {
      data->req.download_done = TRUE;
    }

    ಯದಿ((type & CLIENTWRITE_EOS) && !data->req.no_body &&
       (data->req.size > data->req.bytecount)) {
      failf(data, "end of response with %" FMT_OFF_T " bytes missing",
            data->req.size - data->req.bytecount);
      ಪ್ರತ್ಯಾಗಮ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ಯದಿ(data->set.max_filesize && !data->req.ignorebody) {
    size_t wmax = get_max_body_write_len(data, data->set.max_filesize);
    ಯದಿ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ಯದಿ(!data->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    result = Curl_cwriter_write(data, writer->next, type, buf, nwrite);
    CURL_TRC_WRITE(data, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned ಪೂರ್ಣಾಙ್ಕ)type, nbytes, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Update stats, write and report progress */
  ಯದಿ(nwrite) {
    data->req.bytecount += nwrite;
    Curl_pgrs_download_inc(data, nwrite);
  }

  ಯದಿ(excess_len) {
    ಯದಿ(!data->req.ignorebody) {
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
  ಅನ್ಯಥಾ ಯದಿ((nwrite < nbytes) && !data->req.ignorebody) {
    failf(data, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          data->set.max_filesize, data->req.bytecount);
    ಪ್ರತ್ಯಾಗಮ CURLE_FILESIZE_EXCEEDED;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype cw_download = {
  "protocol",
  ರಿಕ್ತ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ಸ್ಥಿರ CURLcode cw_raw_write(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                             ಧ್ರುವ ವರ್ಣ *buf, size_t nbytes)
{
  ಯದಿ(type & CLIENTWRITE_BODY && data->set.verbose && !data->req.ignorebody) {
    Curl_debug(data, CURLINFO_DATA_IN, buf, nbytes);
  }
  ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, nbytes);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype cw_raw = {
  "raw",
  ರಿಕ್ತ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_cwriter)
};

ಸ್ಥಿರ ಶೂನ್ಯ cwriter_add(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಸಂರಚನಾ Curl_cwriter **anchor = &data->req.writer.ರಾಶಿ;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  ಯಾವತ್(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->next;
  writer->next = *anchor;
  *anchor = writer;
}

ಸ್ಥಿರ CURLcode do_init_writer_ರಾಶಿ(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cwriter *writer;
  CURLcode result;

  DEBUGASSERT(!data->req.writer.ರಾಶಿ);
  result = Curl_cwriter_create(&data->req.writer.ರಾಶಿ,
                               data, &Curl_cwt_out, CURL_CW_CLIENT);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_cwriter_create(&writer, data, &cw_download, CURL_CW_PROTOCOL);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  cwriter_add(data, writer);

  result = Curl_cwriter_create(&writer, data, &cw_raw, CURL_CW_RAW);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  cwriter_add(data, writer);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *buf,
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

  ಯದಿ(!data->req.writer.ರಾಶಿ) {
    result = do_init_writer_ರಾಶಿ(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    DEBUGASSERT(data->req.writer.ರಾಶಿ);
  }

  result = Curl_cwriter_write(data, data->req.writer.ರಾಶಿ, type, buf, len);
  CURL_TRC_WRITE(data, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned ಪೂರ್ಣಾಙ್ಕ)type, len, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_client_flush(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;

  ಯದಿ(!data->req.writer.ರಾಶಿ) {
    result = do_init_writer_ರಾಶಿ(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    DEBUGASSERT(data->req.writer.ರಾಶಿ);
  }

  result = Curl_cwriter_flush(data, data->req.writer.ರಾಶಿ);
  CURL_TRC_WRITE(data, "client_flush() -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ಸಂರಚನಾ Curl_cwriter **pwriter,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ಸಂರಚನಾ Curl_cwriter *writer = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಶೂನ್ಯ *p;

  DEBUGASSERT(cwt->cwriter_size >= ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_size);
  ಯದಿ(!p)
    ಗಚ್ಛ out;

  writer = (ಸಂರಚನಾ Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  result = cwt->do_init(data, writer);

out:
  *pwriter = result ? ರಿಕ್ತ : writer;
  ಯದಿ(result)
    curlx_free(writer);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_cwriter_free(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ Curl_cwriter *writer)
{
  ಯದಿ(writer) {
    writer->cwt->do_close(data, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ಎಣಿಕೆ(ಸಂರಚನಾ Curl_easy *data, Curl_cwriter_phase phase)
{
  ಸಂರಚನಾ Curl_cwriter *w;
  size_t n = 0;

  ಪ್ರತಿ(w = data->req.writer.ರಾಶಿ; w; w = w->next) {
    ಯದಿ(w->phase == phase)
      ++n;
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ CURLcode cwriter_ensure_pause_writer(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cwriter *writer =
    Curl_cwriter_get_by_type(data, &Curl_cwt_pause);
  CURLcode result = CURLE_OK;

  ಯದಿ(!writer) {
    result = Curl_cwriter_create(&writer, data, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ಯದಿ(!result)
      cwriter_add(data, writer);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cwriter_add(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_cwriter *writer)
{
  CURLcode result;
  ಸಂರಚನಾ Curl_cwriter **anchor = &data->req.writer.ರಾಶಿ;

  ಯದಿ(!*anchor) {
    result = do_init_writer_ರಾಶಿ(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    result = cwriter_ensure_pause_writer(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  cwriter_add(data, writer);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ Curl_cwriter *Curl_cwriter_get_by_name(ಸಂರಚನಾ Curl_easy *data,
                                              ಧ್ರುವ ವರ್ಣ *name)
{
  ಸಂರಚನಾ Curl_cwriter *writer;
  ಪ್ರತಿ(writer = data->req.writer.ರಾಶಿ; writer; writer = writer->next) {
    ಯದಿ(!strcmp(name, writer->cwt->name))
      ಪ್ರತ್ಯಾಗಮ writer;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸಂರಚನಾ Curl_cwriter *Curl_cwriter_get_by_type(ಸಂರಚನಾ Curl_easy *data,
                                              ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype *cwt)
{
  ಸಂರಚನಾ Curl_cwriter *writer;
  ಪ್ರತಿ(writer = data->req.writer.ರಾಶಿ; writer; writer = writer->next) {
    ಯದಿ(writer->cwt == cwt)
      ಪ್ರತ್ಯಾಗಮ writer;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURLcode Curl_cwriter_unpause(ಸಂರಚನಾ Curl_easy *data)
{
  data->req.writer.paused = FALSE;
  ಪ್ರತ್ಯಾಗಮ Curl_cwriter_flush(data, data->req.writer.ರಾಶಿ);
}

CURLcode Curl_creader_read(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader,
                           ವರ್ಣ *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ಯದಿ(!reader)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  ಪ್ರತ್ಯಾಗಮ reader->crt->do_read(data, reader, buf, blen, nread, eos);
}

ಶೂನ್ಯ Curl_creader_clear_eos(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader)
{
  ಯಾವತ್(reader) {
    (ಶೂನ್ಯ)reader->crt->cntrl(data, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->next;
  }
}

CURLcode Curl_creader_def_init(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_creader_def_close(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
}

CURLcode Curl_creader_def_read(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creader *reader,
                               ವರ್ಣ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ಯದಿ(reader->next)
    ಪ್ರತ್ಯಾಗಮ reader->next->crt->do_read(data, reader->next, buf, blen,
                                      nread, eos);
  ಅನ್ಯಥಾ {
    *nread = 0;
    *eos = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

curl_off_t Curl_creader_def_total_length(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ Curl_creader *reader)
{
  ಪ್ರತ್ಯಾಗಮ reader->next ?
         reader->next->crt->total_length(data, reader->next) : -1;
}

CURLcode Curl_creader_def_resume_from(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ Curl_creader *reader,
                                      curl_off_t offset)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)offset;
  ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)opcode;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

bool Curl_creader_def_is_paused(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_creader *reader)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_creader_def_done(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader, ಪೂರ್ಣಾಙ್ಕ premature)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)premature;
}

ಸಂರಚನಾ cr_in_ctx {
  ಸಂರಚನಾ Curl_creader super;
  curl_read_callback read_cb;
  ಶೂನ್ಯ *cb_user_data;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

ಸ್ಥಿರ CURLcode cr_in_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = data->state.fread_func;
  ctx->cb_user_data = data->state.in;
  ctx->total_len = -1;
  ctx->read_len = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ಸ್ಥಿರ CURLcode cr_in_read(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader,
                           ವರ್ಣ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  ಯದಿ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ಪ್ರತ್ಯಾಗಮ ctx->error_result;
  }
  ಯದಿ(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  /* respect length limitations */
  ಯದಿ(ctx->total_len >= 0) {
    blen = curlx_sotouz_range(ctx->total_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ಯದಿ(ctx->read_cb && blen) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_data);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  ವಿಕಲ್ಪ(nread) {
  ಪಕ್ಷ 0:
    ಯದಿ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->total_len);
      result = CURLE_READ_ERROR;
      ಭಙ್ಗ;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_READFUNC_PAUSE:
    ಯದಿ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(data, "Read callback asked for PAUSE when not supported");
      result = CURLE_READ_ERROR;
      ಭಙ್ಗ;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ಭಙ್ಗ; /* nothing was read */

  ಸ್ವಭಾವ:
    ಯದಿ(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      result = CURLE_READ_ERROR;
      ಭಙ್ಗ;
    }
    ctx->read_len += nread;
    ಯದಿ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ಭಙ್ಗ;
  }
  CURL_TRC_READ(data, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->total_len, ctx->read_len, (ಪೂರ್ಣಾಙ್ಕ)result,
                *pnread, *peos);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cr_in_needs_rewind(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ (bool)ctx->has_used_cb;
}

ಸ್ಥಿರ curl_off_t cr_in_total_length(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ctx->total_len;
}

ಸ್ಥಿರ CURLcode cr_in_resume_from(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_creader *reader,
                                  curl_off_t offset)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  ಪೂರ್ಣಾಙ್ಕ seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(data->conn);
  /* already started reading? */
  ಯದಿ(ctx->read_len)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;

  ಯದಿ(data->set.seek_func) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_seek_func);
    seekerr = data->set.seek_func(data->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  ಯದಿ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ಯದಿ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(data, "Could not seek stream");
      ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    ಕುರು {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      ವರ್ಣ scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)ಪರಿಮಾಣ(scratch)) ?
        ಪರಿಮಾಣ(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, data, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_data);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      ಯದಿ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
      }
    } ಯಾವತ್(passed < offset);
  }

  /* now, decrease the size of the read */
  ಯದಿ(ctx->total_len > 0) {
    ctx->total_len -= offset;

    ಯದಿ(ctx->total_len <= 0) {
      failf(data, "File already completely uploaded");
      ಪ್ರತ್ಯಾಗಮ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_in_rewind(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ಯದಿ(!ctx->has_used_cb)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(data->set.seek_func) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಙ್ಕ err;

    CURL_CBAPI_START(&guard, data, easy_seek_func);
    err = data->set.seek_func(data->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.seek_func -> %d", err);
    ಯದಿ(err) {
      failf(data, "seek callback returned error %d", err);
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_FAIL_REWIND;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(data->set.ioctl_func) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, data, easy_ioctl_func);
    err = data->set.ioctl_func(data, CURLIOCMD_RESTARTREAD,
                               data->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(data, "cr_in, rewind via set.ioctl_func -> %d", (ಪೂರ್ಣಾಙ್ಕ)err);
    ಯದಿ(err) {
      failf(data, "ioctl callback returned error %d", (ಪೂರ್ಣಾಙ್ಕ)err);
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_FAIL_REWIND;
    }
  }
  ಅನ್ಯಥಾ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    ಯದಿ(data->state.fread_func == (curl_read_callback)fread) {
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
      ಪೂರ್ಣಾಙ್ಕ err = fseek(data->state.in, 0, SEEK_SET);
      CURL_TRC_READ(data, "cr_in, rewind via fseek -> %d(%d)",
                    (ಪೂರ್ಣಾಙ್ಕ)err, (ಪೂರ್ಣಾಙ್ಕ)errno);
      ಯದಿ(err != -1)
        /* successful rewind */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(data, "necessary data rewind was not possible");
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_FAIL_REWIND;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_in_cntrl(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;

  ವಿಕಲ್ಪ(opcode) {
  ಪಕ್ಷ CURL_CRCNTRL_REWIND:
    ಪ್ರತ್ಯಾಗಮ cr_in_rewind(data, reader);
  ಪಕ್ಷ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool cr_in_is_paused(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_in_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ (bool)ctx->is_paused;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_in = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_in_ctx)
};

CURLcode Curl_creader_create(ಸಂರಚನಾ Curl_creader **preader,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ಸಂರಚನಾ Curl_creader *reader = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಶೂನ್ಯ *p;

  DEBUGASSERT(crt->creader_size >= ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_creader));
  p = curlx_calloc(1, crt->creader_size);
  ಯದಿ(!p)
    ಗಚ್ಛ out;

  reader = (ಸಂರಚನಾ Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  result = crt->do_init(data, reader);

out:
  *preader = result ? ರಿಕ್ತ : reader;
  ಯದಿ(result)
    curlx_free(reader);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_creader_free(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  ಯದಿ(reader) {
    reader->crt->do_close(data, reader);
    curlx_free(reader);
  }
}

ಸಂರಚನಾ cr_lc_ctx {
  ಸಂರಚನಾ Curl_creader super;
  ಸಂರಚನಾ bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(prev_cr);   /* the last byte was a CR */
};

ಸ್ಥಿರ CURLcode cr_lc_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_lc_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_lc_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_lc_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ಸ್ಥಿರ CURLcode cr_lc_read(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader,
                           ವರ್ಣ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_lc_ctx *ctx = reader->ctx;
  CURLcode result;
  size_t nread, i, start, n;
  bool eos;

  ಯದಿ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(Curl_bufq_is_empty(&ctx->buf)) {
    ಯದಿ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ctx->read_eos = eos;

    ಯದಿ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ಯದಿ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ಯದಿ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ಗಚ್ಛ out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    ಪ್ರತಿ(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ಯದಿ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ಅನುವರ್ತ;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ಯದಿ(!result)
        result = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      start = i + 1;
    }

    ಯದಿ(start < i) { /* leftover */
      result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ಯದಿ(!result && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(data, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread, *peos);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ curl_off_t cr_lc_total_length(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_lc = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_lc_ctx)
};

ಸ್ಥಿರ CURLcode cr_lc_add(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = ರಿಕ್ತ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_lc, CURL_CR_CONTENT_ENCODE);
  ಯದಿ(!result)
    result = Curl_creader_add(data, reader);

  ಯದಿ(result && reader)
    Curl_creader_free(data, reader);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode do_init_reader_ರಾಶಿ(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_creader *r)
{
  CURLcode result = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!data->req.reader.ರಾಶಿ);

  data->req.reader.ರಾಶಿ = r;
  clen = r->crt->total_length(data, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  ಯದಿ(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (data->set.crlf || data->state.prefer_ascii)
#ಅನ್ಯಥಾ
    data->set.crlf
#endif
    ) {
    result = cr_lc_add(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_creader_set_fread(ಸಂರಚನಾ Curl_easy *data, curl_off_t len)
{
  CURLcode result;
  ಸಂರಚನಾ Curl_creader *r;
  ಸಂರಚನಾ cr_in_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_in, CURL_CR_CLIENT);
  ಯದಿ(result || !r)
    ಗಚ್ಛ out;
  ctx = r->ctx;
  ctx->total_len = len;

  cl_reset_reader(data);
  result = do_init_reader_ರಾಶಿ(data, r);
out:
  CURL_TRC_READ(data, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_creader_add(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_creader *reader)
{
  CURLcode result;
  ಸಂರಚನಾ Curl_creader **anchor = &data->req.reader.ರಾಶಿ;

  ಯದಿ(!*anchor) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ಯಾವತ್(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->next;
  reader->next = *anchor;
  *anchor = reader;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_creader_set(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *r)
{
  CURLcode result;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(data);
  result = do_init_reader_ರಾಶಿ(data, r);
  ಯದಿ(result)
    Curl_creader_free(data, r);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_client_read(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode result;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  ಯದಿ(!data->req.reader.ರಾಶಿ) {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    DEBUGASSERT(data->req.reader.ರಾಶಿ);
  }
  ಯದಿ(!data->req.reader_started) {
    Curl_rlimit_start(&data->progress.ul.rlimit, Curl_pgrs_now(data), -1);
    data->req.reader_started = TRUE;
  }

  ಯದಿ(Curl_rlimit_active(&data->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&data->progress.ul.rlimit, ರಿಕ್ತ);
    ಯದಿ(ul_avail <= 0) {
      result = CURLE_OK;
      *eos = FALSE;
      ಗಚ್ಛ out;
    }
    ಯದಿ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  result = Curl_creader_read(data, data->req.reader.ರಾಶಿ, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(data, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಙ್ಕ)result, *nread, *eos);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_creader_needs_rewind(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = data->req.reader.ರಾಶಿ;
  ಯಾವತ್(reader) {
    ಯದಿ(reader->crt->needs_rewind(data, reader)) {
      CURL_TRC_READ(data, "client reader needs rewind before next request");
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    reader = reader->next;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode cr_null_read(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader,
                             ವರ್ಣ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  *pnread = 0;
  *peos = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_null_total_length(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_null = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_creader)
};

CURLcode Curl_creader_set_null(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_null, CURL_CR_CLIENT);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  cl_reset_reader(data);
  ಪ್ರತ್ಯಾಗಮ do_init_reader_ರಾಶಿ(data, r);
}

ಸಂರಚನಾ cr_buf_ctx {
  ಸಂರಚನಾ Curl_creader super;
  ಧ್ರುವ ವರ್ಣ *buf;
  size_t blen;
  size_t index;
};

ಸ್ಥಿರ CURLcode cr_buf_read(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader,
                            ವರ್ಣ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->index;

  ಯದಿ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->index, nread);
    *pnread = nread;
    ctx->index += nread;
    *peos = (ctx->index == ctx->blen);
  }
  CURL_TRC_READ(data, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool cr_buf_needs_rewind(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ctx->index > 0;
}

ಸ್ಥಿರ CURLcode cr_buf_cntrl(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ಸಂರಚನಾ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ವಿಕಲ್ಪ(opcode) {
  ಪಕ್ಷ CURL_CRCNTRL_REWIND:
    ctx->index = 0;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_buf_total_length(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ (curl_off_t)ctx->blen;
}

ಸ್ಥಿರ CURLcode cr_buf_resume_from(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_creader *reader,
                                   curl_off_t offset)
{
  ಸಂರಚನಾ cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(data->conn);
  /* already started reading? */
  ಯದಿ(ctx->index)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  ಯದಿ(!boffset)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(boffset > ctx->blen)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_buf = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  CURLcode result;
  ಸಂರಚನಾ Curl_creader *r;
  ಸಂರಚನಾ cr_buf_ctx *ctx;

  result = Curl_creader_create(&r, data, &cr_buf, CURL_CR_CLIENT);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->index = 0;

  cl_reset_reader(data);
  result = do_init_reader_ರಾಶಿ(data, r);
out:
  CURL_TRC_READ(data, "add buf reader, len=%zu -> %d", blen, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

curl_off_t Curl_creader_total_length(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = data->req.reader.ರಾಶಿ;
  ಪ್ರತ್ಯಾಗಮ r ? r->crt->total_length(data, r) : -1;
}

curl_off_t Curl_creader_client_length(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = data->req.reader.ರಾಶಿ;
  ಯಾವತ್(r && r->phase != CURL_CR_CLIENT)
    r = r->next;
  ಪ್ರತ್ಯಾಗಮ r ? r->crt->total_length(data, r) : -1;
}

CURLcode Curl_creader_unpause(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = data->req.reader.ರಾಶಿ;
  CURLcode result = CURLE_OK;

  ಯಾವತ್(reader) {
    result = reader->crt->cntrl(data, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(data, "unpausing %s -> %d", reader->crt->name, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಭಙ್ಗ;
    reader = reader->next;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_creader_is_paused(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = data->req.reader.ರಾಶಿ;

  ಯಾವತ್(reader) {
    ಯದಿ(reader->crt->is_paused(data, reader))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    reader = reader->next;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_creader_done(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ premature)
{
  ಸಂರಚನಾ Curl_creader *reader = data->req.reader.ರಾಶಿ;
  ಯಾವತ್(reader) {
    reader->crt->done(data, reader, premature);
    reader = reader->next;
  }
}

ಸಂರಚನಾ Curl_creader *Curl_creader_get_by_type(ಸಂರಚನಾ Curl_easy *data,
                                              ಧ್ರುವ ಸಂರಚನಾ Curl_crtype *crt)
{
  ಸಂರಚನಾ Curl_creader *r;
  ಪ್ರತಿ(r = data->req.reader.ರಾಶಿ; r; r = r->next) {
    ಯದಿ(r->crt == crt)
      ಪ್ರತ್ಯಾಗಮ r;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
