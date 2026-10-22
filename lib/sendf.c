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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LINUX_TCP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/tcp.h>
#ਜਾਂਜੇ defined(HAVE_NETINET_TCP_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/tcp.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-out.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-pause.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"

ਸਥਿਰ ਖਾਲੀ cl_reset_writer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cwriter *writer = ਅੰਕੜੇ->req.writer.ਢੇਰ;
  ਜਦੋਂ(writer) {
    ਅੰਕੜੇ->req.writer.ਢੇਰ = writer->ਅਗਲਾ;
    writer->cwt->do_close(ਅੰਕੜੇ, writer);
    curlx_free(writer);
    writer = ਅੰਕੜੇ->req.writer.ਢੇਰ;
  }
  ਅੰਕੜੇ->req.writer.paused = FALSE;
}

ਸਥਿਰ ਖਾਲੀ cl_reset_reader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  ਅੰਕੜੇ->req.reader_started = FALSE;
  ਜਦੋਂ(reader) {
    ਅੰਕੜੇ->req.reader.ਢੇਰ = reader->ਅਗਲਾ;
    reader->crt->do_close(ਅੰਕੜੇ, reader);
    curlx_free(reader);
    reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  }
}

ਖਾਲੀ Curl_client_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  cl_reset_reader(ਅੰਕੜੇ);
  cl_reset_writer(ਅੰਕੜੇ);

  ਅੰਕੜੇ->req.bytecount = 0;
  ਅੰਕੜੇ->req.headerline = 0;
}

ਖਾਲੀ Curl_client_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(ਅੰਕੜੇ, "client_reset, will rewind reader");
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_READ(ਅੰਕੜੇ, "client_reset, clear readers");
    cl_reset_reader(ਅੰਕੜੇ);
  }
  cl_reset_writer(ਅੰਕੜੇ);

  ਅੰਕੜੇ->req.bytecount = 0;
  ਅੰਕੜੇ->req.headerline = 0;
}

CURLcode Curl_client_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->req.rewind_read) {
    ਬਣਤਰ Curl_creader *r = ਅੰਕੜੇ->req.reader.ਢੇਰ;
    CURLcode ਨਤੀਜਾ = CURLE_OK;

    CURL_TRC_READ(ਅੰਕੜੇ, "client start, rewind readers");
    ਜਦੋਂ(r) {
      ਨਤੀਜਾ = r->crt->cntrl(ਅੰਕੜੇ, r, CURL_CRCNTRL_REWIND);
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      r = r->ਅਗਲਾ;
    }
    ਅੰਕੜੇ->req.rewind_read = FALSE;
    cl_reset_reader(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

bool Curl_creader_will_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ (bool)ਅੰਕੜੇ->req.rewind_read;
}

ਖਾਲੀ Curl_creader_ਸੈੱਟ_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable)
{
  ਅੰਕੜੇ->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)writer;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer)
{
  ਵਾਪਸ Curl_cwriter_flush(ਅੰਕੜੇ, writer->ਅਗਲਾ);
}

ਖਾਲੀ Curl_cwriter_def_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)writer;
}

ਸਥਿਰ size_t get_max_body_write_len(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਹੱਦਬੰਦੀ)
{
  ਜੇ(ਹੱਦਬੰਦੀ != -1) {
    /* How much more are we allowed to write? */
    ਵਾਪਸ curlx_sotouz_range(ਹੱਦਬੰਦੀ - ਅੰਕੜੇ->req.bytecount, 0, SIZE_MAX);
  }
  ਵਾਪਸ SIZE_MAX;
}

ਬਣਤਰ cw_download_ctx {
  ਬਣਤਰ Curl_cwriter super;
  ਦੋਆਧਾਰੀ_ਅੰਕ(started_response);
  ਦੋਆਧਾਰੀ_ਅੰਕ(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ਸਥਿਰ CURLcode cw_download_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                  ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਬਣਤਰ cw_download_ctx *ctx = writer->ctx;
  CURLcode ਨਤੀਜਾ;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ਜੇ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || ਅੰਕੜੇ->req.upload_ਪੂਰਾ)) {
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ਜੇ(!(type & CLIENTWRITE_BODY)) {
    ਜੇ(is_connect && ਅੰਕੜੇ->ਸੈੱਟ.suppress_connect_ਸਿਰਲੇਖਾਂ)
      ਵਾਪਸ CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);
    CURL_TRC_WRITE(ਅੰਕੜੇ, "download_write header(type=%x, blen=%zu) -> %d",
                   (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type, nbytes, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_ਸ਼ੁਰੂ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, Curl_pgrs_now(ਅੰਕੜੇ),
                      ਅੰਕੜੇ->req.ਮਾਪ);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  ਜੇ(ਅੰਕੜੇ->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(ਅੰਕੜੇ->conn);
    CURL_TRC_WRITE(ਅੰਕੜੇ, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type, nbytes);
    ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
    ਜੇ(ਅੰਕੜੇ->info.ਸਿਰਲੇਖ_ਮਾਪ)
      /* if headers have been received, this is fine */
      ਵਾਪਸ CURLE_OK;
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ਜੇ(ਅੰਕੜੇ->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(ਅੰਕੜੇ, ਅੰਕੜੇ->req.maxdownload);
    ਜੇ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ਜੇ(nwrite == wmax) {
      ਅੰਕੜੇ->req.download_ਪੂਰਾ = TRUE;
    }

    ਜੇ((type & CLIENTWRITE_EOS) && !ਅੰਕੜੇ->req.no_body &&
       (ਅੰਕੜੇ->req.ਮਾਪ > ਅੰਕੜੇ->req.bytecount)) {
      failf(ਅੰਕੜੇ, "end of response with %" FMT_OFF_T " bytes missing",
            ਅੰਕੜੇ->req.ਮਾਪ - ਅੰਕੜੇ->req.bytecount);
      ਵਾਪਸ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize && !ਅੰਕੜੇ->req.ignorebody) {
    size_t wmax = get_max_body_write_len(ਅੰਕੜੇ, ਅੰਕੜੇ->ਸੈੱਟ.max_filesize);
    ਜੇ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ਜੇ(!ਅੰਕੜੇ->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nwrite);
    CURL_TRC_WRITE(ਅੰਕੜੇ, "download_write body(type=%x, blen=%zu) -> %d",
                   (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type, nbytes, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Update stats, write and report progress */
  ਜੇ(nwrite) {
    ਅੰਕੜੇ->req.bytecount += nwrite;
    Curl_pgrs_download_inc(ਅੰਕੜੇ, nwrite);
  }

  ਜੇ(excess_len) {
    ਜੇ(!ਅੰਕੜੇ->req.ignorebody) {
      infof(ਅੰਕੜੇ,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, ਅੰਕੜੇ->req.ਮਾਪ, ਅੰਕੜੇ->req.maxdownload,
            ਅੰਕੜੇ->req.bytecount);
      connclose(ਅੰਕੜੇ->conn);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ((nwrite < nbytes) && !ਅੰਕੜੇ->req.ignorebody) {
    failf(ਅੰਕੜੇ, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          ਅੰਕੜੇ->ਸੈੱਟ.max_filesize, ਅੰਕੜੇ->req.bytecount);
    ਵਾਪਸ CURLE_FILESIZE_EXCEEDED;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype cw_download = {
  "protocol",
  ਨਿਰਾ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ਆਕਾਰ(ਬਣਤਰ cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ਸਥਿਰ CURLcode cw_raw_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                             ਅਚਲ ਅੱਖਰ *buf, size_t nbytes)
{
  ਜੇ(type & CLIENTWRITE_BODY && ਅੰਕੜੇ->ਸੈੱਟ.verbose && !ਅੰਕੜੇ->req.ignorebody) {
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਅੰਕੜੇ_ਵਿੱਚ, buf, nbytes);
  }
  ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, nbytes);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype cw_raw = {
  "raw",
  ਨਿਰਾ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ਆਕਾਰ(ਬਣਤਰ Curl_cwriter)
};

ਸਥਿਰ ਖਾਲੀ cwriter_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_cwriter *writer)
{
  ਬਣਤਰ Curl_cwriter **anchor = &ਅੰਕੜੇ->req.writer.ਢੇਰ;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  ਜਦੋਂ(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->ਅਗਲਾ;
  writer->ਅਗਲਾ = *anchor;
  *anchor = writer;
}

ਸਥਿਰ CURLcode do_init_writer_ਢੇਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cwriter *writer;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(!ਅੰਕੜੇ->req.writer.ਢੇਰ);
  ਨਤੀਜਾ = Curl_cwriter_create(&ਅੰਕੜੇ->req.writer.ਢੇਰ,
                               ਅੰਕੜੇ, &Curl_cwt_ਬਾਹਰਲਾ, CURL_CW_CLIENT);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_cwriter_create(&writer, ਅੰਕੜੇ, &cw_download, CURL_CW_PROTOCOL);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);

  ਨਤੀਜਾ = Curl_cwriter_create(&writer, ਅੰਕੜੇ, &cw_raw, CURL_CW_RAW);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *buf,
                           size_t len)
{
  CURLcode ਨਤੀਜਾ;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_ਸਿਰਲੇਖ | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  ਜੇ(!ਅੰਕੜੇ->req.writer.ਢੇਰ) {
    ਨਤੀਜਾ = do_init_writer_ਢੇਰ(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(ਅੰਕੜੇ->req.writer.ਢੇਰ);
  }

  ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, ਅੰਕੜੇ->req.writer.ਢੇਰ, type, buf, len);
  CURL_TRC_WRITE(ਅੰਕੜੇ, "client_write(type=%x, len=%zu) -> %d",
                 (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type, len, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_client_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!ਅੰਕੜੇ->req.writer.ਢੇਰ) {
    ਨਤੀਜਾ = do_init_writer_ਢੇਰ(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(ਅੰਕੜੇ->req.writer.ਢੇਰ);
  }

  ਨਤੀਜਾ = Curl_cwriter_flush(ਅੰਕੜੇ, ਅੰਕੜੇ->req.writer.ਢੇਰ);
  CURL_TRC_WRITE(ਅੰਕੜੇ, "client_flush() -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ਬਣਤਰ Curl_cwriter **pwriter,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ਬਣਤਰ Curl_cwriter *writer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਖਾਲੀ *p;

  DEBUGASSERT(cwt->cwriter_ਮਾਪ >= ਆਕਾਰ(ਬਣਤਰ Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_ਮਾਪ);
  ਜੇ(!p)
    ਜਾਓ ਬਾਹਰਲਾ;

  writer = (ਬਣਤਰ Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  ਨਤੀਜਾ = cwt->do_init(ਅੰਕੜੇ, writer);

ਬਾਹਰਲਾ:
  *pwriter = ਨਤੀਜਾ ? ਨਿਰਾ : writer;
  ਜੇ(ਨਤੀਜਾ)
    curlx_free(writer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_cwriter_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ Curl_cwriter *writer)
{
  ਜੇ(writer) {
    writer->cwt->do_close(ਅੰਕੜੇ, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ਗਿਣਤੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Curl_cwriter_phase phase)
{
  ਬਣਤਰ Curl_cwriter *w;
  size_t n = 0;

  ਲਈ(w = ਅੰਕੜੇ->req.writer.ਢੇਰ; w; w = w->ਅਗਲਾ) {
    ਜੇ(w->phase == phase)
      ++n;
  }
  ਵਾਪਸ n;
}

ਸਥਿਰ CURLcode cwriter_ensure_pause_writer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cwriter *writer =
    Curl_cwriter_get_by_type(ਅੰਕੜੇ, &Curl_cwt_pause);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!writer) {
    ਨਤੀਜਾ = Curl_cwriter_create(&writer, ਅੰਕੜੇ, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ਜੇ(!ਨਤੀਜਾ)
      cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cwriter_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_cwriter *writer)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_cwriter **anchor = &ਅੰਕੜੇ->req.writer.ਢੇਰ;

  ਜੇ(!*anchor) {
    ਨਤੀਜਾ = do_init_writer_ਢੇਰ(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    ਨਤੀਜਾ = cwriter_ensure_pause_writer(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ Curl_cwriter *Curl_cwriter_get_by_name(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਅੱਖਰ *name)
{
  ਬਣਤਰ Curl_cwriter *writer;
  ਲਈ(writer = ਅੰਕੜੇ->req.writer.ਢੇਰ; writer; writer = writer->ਅਗਲਾ) {
    ਜੇ(!strcmp(name, writer->cwt->name))
      ਵਾਪਸ writer;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਬਣਤਰ Curl_cwriter *Curl_cwriter_get_by_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt)
{
  ਬਣਤਰ Curl_cwriter *writer;
  ਲਈ(writer = ਅੰਕੜੇ->req.writer.ਢੇਰ; writer; writer = writer->ਅਗਲਾ) {
    ਜੇ(writer->cwt == cwt)
      ਵਾਪਸ writer;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode Curl_cwriter_unpause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅੰਕੜੇ->req.writer.paused = FALSE;
  ਵਾਪਸ Curl_cwriter_flush(ਅੰਕੜੇ, ਅੰਕੜੇ->req.writer.ਢੇਰ);
}

CURLcode Curl_creader_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader,
                           ਅੱਖਰ *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ਜੇ(!reader)
    ਵਾਪਸ CURLE_READ_ERROR;
  ਵਾਪਸ reader->crt->do_read(ਅੰਕੜੇ, reader, buf, blen, nread, eos);
}

ਖਾਲੀ Curl_creader_clear_eos(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader)
{
  ਜਦੋਂ(reader) {
    (ਖਾਲੀ)reader->crt->cntrl(ਅੰਕੜੇ, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->ਅਗਲਾ;
  }
}

CURLcode Curl_creader_def_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_creader_def_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
}

CURLcode Curl_creader_def_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader,
                               ਅੱਖਰ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ਜੇ(reader->ਅਗਲਾ)
    ਵਾਪਸ reader->ਅਗਲਾ->crt->do_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen,
                                      nread, eos);
  ਨਹੀਂਤਾਂ {
    *nread = 0;
    *eos = FALSE;
    ਵਾਪਸ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ FALSE;
}

curl_off_t Curl_creader_def_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creader *reader)
{
  ਵਾਪਸ reader->ਅਗਲਾ ?
         reader->ਅਗਲਾ->crt->ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ, reader->ਅਗਲਾ) : -1;
}

CURLcode Curl_creader_def_resume_from(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creader *reader,
                                      curl_off_t ਵਿੱਥ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  (ਖਾਲੀ)ਵਿੱਥ;
  ਵਾਪਸ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  (ਖਾਲੀ)opcode;
  ਵਾਪਸ CURLE_OK;
}

bool Curl_creader_def_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_creader_def_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader, ਪੂਰਨ premature)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  (ਖਾਲੀ)premature;
}

ਬਣਤਰ cr_ਵਿੱਚ_ctx {
  ਬਣਤਰ Curl_creader super;
  curl_read_callback read_cb;
  ਖਾਲੀ *cb_user_ਅੰਕੜੇ;
  curl_off_t ਕੁੱਲ_len;
  curl_off_t read_len;
  CURLcode error_ਨਤੀਜਾ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵੇਖਿਆ_eos);
  ਦੋਆਧਾਰੀ_ਅੰਕ(errored);
  ਦੋਆਧਾਰੀ_ਅੰਕ(has_ਵਰਤਿਆ_cb);
  ਦੋਆਧਾਰੀ_ਅੰਕ(is_paused);
};

ਸਥਿਰ CURLcode cr_ਵਿੱਚ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  ctx->read_cb = ਅੰਕੜੇ->state.fread_func;
  ctx->cb_user_ਅੰਕੜੇ = ਅੰਕੜੇ->state.ਵਿੱਚ;
  ctx->ਕੁੱਲ_len = -1;
  ctx->read_len = 0;
  ਵਾਪਸ CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ਸਥਿਰ CURLcode cr_ਵਿੱਚ_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader,
                           ਅੱਖਰ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  ਜੇ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ਵਾਪਸ ctx->error_ਨਤੀਜਾ;
  }
  ਜੇ(ctx->ਵੇਖਿਆ_eos) {
    *pnread = 0;
    *peos = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  /* respect length limitations */
  ਜੇ(ctx->ਕੁੱਲ_len >= 0) {
    blen = curlx_sotouz_range(ctx->ਕੁੱਲ_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ਜੇ(ctx->read_cb && blen) {
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_cr_ਵਿੱਚ_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_ਅੰਕੜੇ);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ctx->has_ਵਰਤਿਆ_cb = TRUE;
  }

  ਚੋਣ(nread) {
  ਹਾਲਤ 0:
    ਜੇ((ctx->ਕੁੱਲ_len >= 0) && (ctx->read_len < ctx->ਕੁੱਲ_len)) {
      failf(ਅੰਕੜੇ, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->ਕੁੱਲ_len);
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਰੋਕੋ;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ਵੇਖਿਆ_eos = TRUE;
    ਰੋਕੋ;

  ਹਾਲਤ CURL_READFUNC_ABORT:
    failf(ਅੰਕੜੇ, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    ਰੋਕੋ;

  ਹਾਲਤ CURL_READFUNC_PAUSE:
    ਜੇ(ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(ਅੰਕੜੇ, "Read callback asked for PAUSE when not supported");
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਰੋਕੋ;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    ਨਤੀਜਾ = Curl_xfer_pause_send(ਅੰਕੜੇ, TRUE);
    ਰੋਕੋ; /* nothing was read */

  ਮੂਲ:
    ਜੇ(nread > blen) {
      /* the read function returned a too large value */
      failf(ਅੰਕੜੇ, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਰੋਕੋ;
    }
    ctx->read_len += nread;
    ਜੇ(ctx->ਕੁੱਲ_len >= 0)
      ctx->ਵੇਖਿਆ_eos = (ctx->read_len >= ctx->ਕੁੱਲ_len);
    *pnread = nread;
    *peos = (bool)ctx->ਵੇਖਿਆ_eos;
    ਰੋਕੋ;
  }
  CURL_TRC_READ(ਅੰਕੜੇ, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->ਕੁੱਲ_len, ctx->read_len, (ਪੂਰਨ)ਨਤੀਜਾ,
                *pnread, *peos);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cr_ਵਿੱਚ_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ (bool)ctx->has_ਵਰਤਿਆ_cb;
}

ਸਥਿਰ curl_off_t cr_ਵਿੱਚ_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ctx->ਕੁੱਲ_len;
}

ਸਥਿਰ CURLcode cr_ਵਿੱਚ_resume_from(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_creader *reader,
                                  curl_off_t ਵਿੱਥ)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  ਪੂਰਨ seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(ਅੰਕੜੇ->conn);
  /* already started reading? */
  ਜੇ(ctx->read_len)
    ਵਾਪਸ CURLE_READ_ERROR;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.seek_func) {
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_seek_func);
    seekerr = ਅੰਕੜੇ->ਸੈੱਟ.seek_func(ਅੰਕੜੇ->ਸੈੱਟ.seek_client, ਵਿੱਥ, SEEK_SET);
    CURL_CBAPI_ਸਿਰਾ(&guard);
  }

  ਜੇ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ਜੇ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(ਅੰਕੜੇ, "Could not seek stream");
      ਵਾਪਸ CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    ਕਰੋ {
      ਬਣਤਰ Curl_mapi_guard guard;
      ਅੱਖਰ scratch[4 * 1024];
      size_t readthisamountnow =
        (ਵਿੱਥ - passed > (curl_off_t)ਆਕਾਰ(scratch)) ?
        ਆਕਾਰ(scratch) :
        curlx_sotouz(ਵਿੱਥ - passed);
      size_t actuallyread;

      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_cr_ਵਿੱਚ_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_ਅੰਕੜੇ);
      CURL_CBAPI_ਸਿਰਾ(&guard);

      passed += actuallyread;
      ਜੇ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ਅੰਕੜੇ, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ਵਾਪਸ CURLE_READ_ERROR;
      }
    } ਜਦੋਂ(passed < ਵਿੱਥ);
  }

  /* now, decrease the size of the read */
  ਜੇ(ctx->ਕੁੱਲ_len > 0) {
    ctx->ਕੁੱਲ_len -= ਵਿੱਥ;

    ਜੇ(ctx->ਕੁੱਲ_len <= 0) {
      failf(ਅੰਕੜੇ, "File already completely uploaded");
      ਵਾਪਸ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cr_ਵਿੱਚ_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ਜੇ(!ctx->has_ਵਰਤਿਆ_cb)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.seek_func) {
    ਬਣਤਰ Curl_mapi_guard guard;
    ਪੂਰਨ err;

    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_seek_func);
    err = ਅੰਕੜੇ->ਸੈੱਟ.seek_func(ਅੰਕੜੇ->ਸੈੱਟ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_in, rewind via set.seek_func -> %d", err);
    ਜੇ(err) {
      failf(ਅੰਕੜੇ, "seek callback returned error %d", err);
      ਵਾਪਸ CURLE_SEND_FAIL_REWIND;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ioctl_func) {
    ਬਣਤਰ Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_ioctl_func);
    err = ਅੰਕੜੇ->ਸੈੱਟ.ioctl_func(ਅੰਕੜੇ, CURLIOCMD_RESTARTREAD,
                               ਅੰਕੜੇ->ਸੈੱਟ.ioctl_client);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_in, rewind via set.ioctl_func -> %d", (ਪੂਰਨ)err);
    ਜੇ(err) {
      failf(ਅੰਕੜੇ, "ioctl callback returned error %d", (ਪੂਰਨ)err);
      ਵਾਪਸ CURLE_SEND_FAIL_REWIND;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
    ਜੇ(ਅੰਕੜੇ->state.fread_func == (curl_read_callback)fread) {
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
      ਪੂਰਨ err = fseek(ਅੰਕੜੇ->state.ਵਿੱਚ, 0, SEEK_SET);
      CURL_TRC_READ(ਅੰਕੜੇ, "cr_in, rewind via fseek -> %d(%d)",
                    (ਪੂਰਨ)err, (ਪੂਰਨ)errno);
      ਜੇ(err != -1)
        /* successful rewind */
        ਵਾਪਸ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(ਅੰਕੜੇ, "necessary data rewind was not possible");
    ਵਾਪਸ CURLE_SEND_FAIL_REWIND;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cr_ਵਿੱਚ_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;

  ਚੋਣ(opcode) {
  ਹਾਲਤ CURL_CRCNTRL_REWIND:
    ਵਾਪਸ cr_ਵਿੱਚ_rewind(ਅੰਕੜੇ, reader);
  ਹਾਲਤ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_CRCNTRL_CLEAR_EOS:
    ctx->ਵੇਖਿਆ_eos = FALSE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool cr_ਵਿੱਚ_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ (bool)ctx->is_paused;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_ਵਿੱਚ = {
  "cr-in",
  cr_ਵਿੱਚ_init,
  cr_ਵਿੱਚ_read,
  Curl_creader_def_close,
  cr_ਵਿੱਚ_needs_rewind,
  cr_ਵਿੱਚ_ਕੁੱਲ_ਲੰਮਾਈ,
  cr_ਵਿੱਚ_resume_from,
  cr_ਵਿੱਚ_cntrl,
  cr_ਵਿੱਚ_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_ਵਿੱਚ_ctx)
};

CURLcode Curl_creader_create(ਬਣਤਰ Curl_creader **preader,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ਬਣਤਰ Curl_creader *reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਖਾਲੀ *p;

  DEBUGASSERT(crt->creader_ਮਾਪ >= ਆਕਾਰ(ਬਣਤਰ Curl_creader));
  p = curlx_calloc(1, crt->creader_ਮਾਪ);
  ਜੇ(!p)
    ਜਾਓ ਬਾਹਰਲਾ;

  reader = (ਬਣਤਰ Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  ਨਤੀਜਾ = crt->do_init(ਅੰਕੜੇ, reader);

ਬਾਹਰਲਾ:
  *preader = ਨਤੀਜਾ ? ਨਿਰਾ : reader;
  ਜੇ(ਨਤੀਜਾ)
    curlx_free(reader);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_creader_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  ਜੇ(reader) {
    reader->crt->do_close(ਅੰਕੜੇ, reader);
    curlx_free(reader);
  }
}

ਬਣਤਰ cr_lc_ctx {
  ਬਣਤਰ Curl_creader super;
  ਬਣਤਰ bufq buf;
  ਦੋਆਧਾਰੀ_ਅੰਕ(read_eos);  /* we read an EOS from the next reader */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos);       /* we have returned an EOS */
  ਦੋਆਧਾਰੀ_ਅੰਕ(prev_cr);   /* the last byte was a CR */
};

ਸਥਿਰ CURLcode cr_lc_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_lc_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cr_lc_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_lc_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ਸਥਿਰ CURLcode cr_lc_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader,
                           ਅੱਖਰ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_lc_ctx *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ;
  size_t nread, i, ਸ਼ੁਰੂ, n;
  bool eos;

  ਜੇ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf)) {
    ਜੇ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    ਨਤੀਜਾ = Curl_creader_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen, &nread, &eos);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ctx->read_eos = eos;

    ਜੇ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ਜੇ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ਜੇ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    ਲਈ(i = ਸ਼ੁਰੂ = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ਜੇ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ਜਾਰੀ;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, buf + ਸ਼ੁਰੂ, i - ਸ਼ੁਰੂ, &n);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਸ਼ੁਰੂ = i + 1;
    }

    ਜੇ(ਸ਼ੁਰੂ < i) { /* leftover */
      ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, buf + ਸ਼ੁਰੂ, i - ਸ਼ੁਰੂ, &n);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf));
  *peos = FALSE;
  ਨਤੀਜਾ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ਜੇ(!ਨਤੀਜਾ && ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ਬਾਹਰਲਾ:
  CURL_TRC_READ(ਅੰਕੜੇ, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread, *peos);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ curl_off_t cr_lc_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ -1;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_lc_ctx)
};

ਸਥਿਰ CURLcode cr_lc_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&reader, ਅੰਕੜੇ, &cr_lc, CURL_CR_CONTENT_ENCODE);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਜੋੜੋ(ਅੰਕੜੇ, reader);

  ਜੇ(ਨਤੀਜਾ && reader)
    Curl_creader_free(ਅੰਕੜੇ, reader);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode do_init_reader_ਢੇਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_creader *r)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!ਅੰਕੜੇ->req.reader.ਢੇਰ);

  ਅੰਕੜੇ->req.reader.ਢੇਰ = r;
  clen = r->crt->ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  ਜੇ(clen &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_PREFER_LF_LINEENDS
    (ਅੰਕੜੇ->ਸੈੱਟ.crlf || ਅੰਕੜੇ->state.prefer_ascii)
#ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->ਸੈੱਟ.crlf
#ਜੇ_ਅੰਤ
    ) {
    ਨਤੀਜਾ = cr_lc_ਜੋੜੋ(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_creader_ਸੈੱਟ_fread(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t len)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_creader *r;
  ਬਣਤਰ cr_ਵਿੱਚ_ctx *ctx;

  ਨਤੀਜਾ = Curl_creader_create(&r, ਅੰਕੜੇ, &cr_ਵਿੱਚ, CURL_CR_CLIENT);
  ਜੇ(ਨਤੀਜਾ || !r)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = r->ctx;
  ctx->ਕੁੱਲ_len = len;

  cl_reset_reader(ਅੰਕੜੇ);
  ਨਤੀਜਾ = do_init_reader_ਢੇਰ(ਅੰਕੜੇ, r);
ਬਾਹਰਲਾ:
  CURL_TRC_READ(ਅੰਕੜੇ, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_creader_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_creader *reader)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_creader **anchor = &ਅੰਕੜੇ->req.reader.ਢੇਰ;

  ਜੇ(!*anchor) {
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ਜਦੋਂ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->ਅਗਲਾ;
  reader->ਅਗਲਾ = *anchor;
  *anchor = reader;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_creader_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *r)
{
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(ਅੰਕੜੇ);
  ਨਤੀਜਾ = do_init_reader_ਢੇਰ(ਅੰਕੜੇ, r);
  ਜੇ(ਨਤੀਜਾ)
    Curl_creader_free(ਅੰਕੜੇ, r);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_client_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  ਜੇ(!ਅੰਕੜੇ->req.reader.ਢੇਰ) {
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(ਅੰਕੜੇ->req.reader.ਢੇਰ);
  }
  ਜੇ(!ਅੰਕੜੇ->req.reader_started) {
    Curl_rlimit_ਸ਼ੁਰੂ(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, Curl_pgrs_now(ਅੰਕੜੇ), -1);
    ਅੰਕੜੇ->req.reader_started = TRUE;
  }

  ਜੇ(Curl_rlimit_ਸਰਗਰਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, ਨਿਰਾ);
    ਜੇ(ul_avail <= 0) {
      ਨਤੀਜਾ = CURLE_OK;
      *eos = FALSE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  ਨਤੀਜਾ = Curl_creader_read(ਅੰਕੜੇ, ਅੰਕੜੇ->req.reader.ਢੇਰ, buf, blen,
                             nread, eos);

ਬਾਹਰਲਾ:
  CURL_TRC_READ(ਅੰਕੜੇ, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ਪੂਰਨ)ਨਤੀਜਾ, *nread, *eos);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_creader_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  ਜਦੋਂ(reader) {
    ਜੇ(reader->crt->needs_rewind(ਅੰਕੜੇ, reader)) {
      CURL_TRC_READ(ਅੰਕੜੇ, "client reader needs rewind before next request");
      ਵਾਪਸ TRUE;
    }
    reader = reader->ਅਗਲਾ;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode cr_null_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader,
                             ਅੱਖਰ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)blen;
  *pnread = 0;
  *peos = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ curl_off_t cr_null_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ Curl_creader)
};

CURLcode Curl_creader_ਸੈੱਟ_null(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&r, ਅੰਕੜੇ, &cr_null, CURL_CR_CLIENT);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  cl_reset_reader(ਅੰਕੜੇ);
  ਵਾਪਸ do_init_reader_ਢੇਰ(ਅੰਕੜੇ, r);
}

ਬਣਤਰ cr_buf_ctx {
  ਬਣਤਰ Curl_creader super;
  ਅਚਲ ਅੱਖਰ *buf;
  size_t blen;
  size_t ਸੂਚਕਾਂਕ;
};

ਸਥਿਰ CURLcode cr_buf_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader,
                            ਅੱਖਰ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->ਸੂਚਕਾਂਕ;

  ਜੇ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(nread > blen)
      nread = blen;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, ctx->buf + ctx->ਸੂਚਕਾਂਕ, nread);
    *pnread = nread;
    ctx->ਸੂਚਕਾਂਕ += nread;
    *peos = (ctx->ਸੂਚਕਾਂਕ == ctx->blen);
  }
  CURL_TRC_READ(ਅੰਕੜੇ, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool cr_buf_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_buf_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ctx->ਸੂਚਕਾਂਕ > 0;
}

ਸਥਿਰ CURLcode cr_buf_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ਬਣਤਰ cr_buf_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਚੋਣ(opcode) {
  ਹਾਲਤ CURL_CRCNTRL_REWIND:
    ctx->ਸੂਚਕਾਂਕ = 0;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ curl_off_t cr_buf_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_buf_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ (curl_off_t)ctx->blen;
}

ਸਥਿਰ CURLcode cr_buf_resume_from(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_creader *reader,
                                   curl_off_t ਵਿੱਥ)
{
  ਬਣਤਰ cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  /* already started reading? */
  ਜੇ(ctx->ਸੂਚਕਾਂਕ)
    ਵਾਪਸ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(ਵਿੱਥ, 0, SIZE_MAX);
  ਜੇ(!boffset)
    ਵਾਪਸ CURLE_OK;
  ਜੇ(boffset > ctx->blen)
    ਵਾਪਸ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_ਕੁੱਲ_ਲੰਮਾਈ,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_buf_ctx)
};

CURLcode Curl_creader_ਸੈੱਟ_buf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_creader *r;
  ਬਣਤਰ cr_buf_ctx *ctx;

  ਨਤੀਜਾ = Curl_creader_create(&r, ਅੰਕੜੇ, &cr_buf, CURL_CR_CLIENT);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->ਸੂਚਕਾਂਕ = 0;

  cl_reset_reader(ਅੰਕੜੇ);
  ਨਤੀਜਾ = do_init_reader_ਢੇਰ(ਅੰਕੜੇ, r);
ਬਾਹਰਲਾ:
  CURL_TRC_READ(ਅੰਕੜੇ, "add buf reader, len=%zu -> %d", blen, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

curl_off_t Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  ਵਾਪਸ r ? r->crt->ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ, r) : -1;
}

curl_off_t Curl_creader_client_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  ਜਦੋਂ(r && r->phase != CURL_CR_CLIENT)
    r = r->ਅਗਲਾ;
  ਵਾਪਸ r ? r->crt->ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ, r) : -1;
}

CURLcode Curl_creader_unpause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(reader) {
    ਨਤੀਜਾ = reader->crt->cntrl(ਅੰਕੜੇ, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(ਅੰਕੜੇ, "unpausing %s -> %d", reader->crt->name, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    reader = reader->ਅਗਲਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_creader_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;

  ਜਦੋਂ(reader) {
    ਜੇ(reader->crt->is_paused(ਅੰਕੜੇ, reader))
      ਵਾਪਸ TRUE;
    reader = reader->ਅਗਲਾ;
  }
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_creader_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ premature)
{
  ਬਣਤਰ Curl_creader *reader = ਅੰਕੜੇ->req.reader.ਢੇਰ;
  ਜਦੋਂ(reader) {
    reader->crt->ਪੂਰਾ(ਅੰਕੜੇ, reader, premature);
    reader = reader->ਅਗਲਾ;
  }
}

ਬਣਤਰ Curl_creader *Curl_creader_get_by_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਬਣਤਰ Curl_crtype *crt)
{
  ਬਣਤਰ Curl_creader *r;
  ਲਈ(r = ਅੰਕੜੇ->req.reader.ਢੇਰ; r; r = r->ਅਗਲਾ) {
    ਜੇ(r->crt == crt)
      ਵਾਪਸ r;
  }
  ਵਾਪਸ ਨਿਰਾ;
}
