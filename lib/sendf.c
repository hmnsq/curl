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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ಅಥವಾಆದರೆ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಆದರೆ_ಅಂತ್ಯ

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

ಸ್ಥಿರ ಶೂನ್ಯ cl_reset_writer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cwriter *writer = ದತ್ತಾಂಶ->req.writer.ರಾಶಿ;
  ಆಗ(writer) {
    ದತ್ತಾಂಶ->req.writer.ರಾಶಿ = writer->ಮುಂದಿನ;
    writer->cwt->do_close(ದತ್ತಾಂಶ, writer);
    curlx_free(writer);
    writer = ದತ್ತಾಂಶ->req.writer.ರಾಶಿ;
  }
  ದತ್ತಾಂಶ->req.writer.paused = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cl_reset_reader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  ದತ್ತಾಂಶ->req.reader_started = FALSE;
  ಆಗ(reader) {
    ದತ್ತಾಂಶ->req.reader.ರಾಶಿ = reader->ಮುಂದಿನ;
    reader->crt->do_close(ದತ್ತಾಂಶ, reader);
    curlx_free(reader);
    reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  }
}

ಶೂನ್ಯ Curl_client_cleanup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  cl_reset_reader(ದತ್ತಾಂಶ);
  cl_reset_writer(ದತ್ತಾಂಶ);

  ದತ್ತಾಂಶ->req.bytecount = 0;
  ದತ್ತಾಂಶ->req.headerline = 0;
}

ಶೂನ್ಯ Curl_client_reset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(ದತ್ತಾಂಶ, "client_reset, will rewind reader");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_READ(ದತ್ತಾಂಶ, "client_reset, clear readers");
    cl_reset_reader(ದತ್ತಾಂಶ);
  }
  cl_reset_writer(ದತ್ತಾಂಶ);

  ದತ್ತಾಂಶ->req.bytecount = 0;
  ದತ್ತಾಂಶ->req.headerline = 0;
}

CURLcode Curl_client_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->req.rewind_read) {
    ರಚನೆ Curl_creader *r = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
    CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

    CURL_TRC_READ(ದತ್ತಾಂಶ, "client start, rewind readers");
    ಆಗ(r) {
      ಫಲಿತಾಂಶ = r->crt->cntrl(ದತ್ತಾಂಶ, r, CURL_CRCNTRL_REWIND);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "rewind of client reader '%s' failed: %d",
              r->crt->name, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      r = r->ಮುಂದಿನ;
    }
    ದತ್ತಾಂಶ->req.rewind_read = FALSE;
    cl_reset_reader(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_creader_will_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ (bool)ದತ್ತಾಂಶ->req.rewind_read;
}

ಶೂನ್ಯ Curl_creader_ಗಣ_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable)
{
  ದತ್ತಾಂಶ->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)writer;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cwriter_def_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer)
{
  ಹಿಂತಿರುಗಿ Curl_cwriter_flush(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ);
}

ಶೂನ್ಯ Curl_cwriter_def_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_cwriter *writer)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)writer;
}

ಸ್ಥಿರ size_t get_max_body_write_len(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಎಲ್ಲೆ)
{
  ಆದರೆ(ಎಲ್ಲೆ != -1) {
    /* How much more are we allowed to write? */
    ಹಿಂತಿರುಗಿ curlx_sotouz_range(ಎಲ್ಲೆ - ದತ್ತಾಂಶ->req.bytecount, 0, SIZE_MAX);
  }
  ಹಿಂತಿರುಗಿ SIZE_MAX;
}

ರಚನೆ cw_download_ctx {
  ರಚನೆ Curl_cwriter super;
  ಬಿಟ್(started_response);
  ಬಿಟ್(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
ಸ್ಥಿರ CURLcode cw_download_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ರಚನೆ cw_download_ctx *ctx = writer->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ಆದರೆ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ)) {
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ಆದರೆ(!(type & CLIENTWRITE_BODY)) {
    ಆದರೆ(is_connect && ದತ್ತಾಂಶ->ಗಣ.suppress_connect_ಶೀರ್ಷಿಕೆಗಳು)
      ಹಿಂತಿರುಗಿ CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "download_write header(type=%x, blen=%zu) -> %d",
                   (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type, nbytes, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_ಆರಂಭ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, Curl_pgrs_now(ದತ್ತಾಂಶ),
                      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(ದತ್ತಾಂಶ->conn);
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type, nbytes);
    ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
    ಆದರೆ(ದತ್ತಾಂಶ->info.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)
      /* if headers have been received, this is fine */
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ಆದರೆ(ದತ್ತಾಂಶ->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.maxdownload);
    ಆದರೆ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ಆದರೆ(nwrite == wmax) {
      ದತ್ತಾಂಶ->req.download_ಮುಗಿದ = TRUE;
    }

    ಆದರೆ((type & CLIENTWRITE_EOS) && !ದತ್ತಾಂಶ->req.no_body &&
       (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 > ದತ್ತಾಂಶ->req.bytecount)) {
      failf(ದತ್ತಾಂಶ, "end of response with %" FMT_OFF_T " bytes missing",
            ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->req.bytecount);
      ಹಿಂತಿರುಗಿ CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize && !ದತ್ತಾಂಶ->req.ignorebody) {
    size_t wmax = get_max_body_write_len(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->ಗಣ.max_filesize);
    ಆದರೆ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nwrite);
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "download_write body(type=%x, blen=%zu) -> %d",
                   (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type, nbytes, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Update stats, write and report progress */
  ಆದರೆ(nwrite) {
    ದತ್ತಾಂಶ->req.bytecount += nwrite;
    Curl_pgrs_download_inc(ದತ್ತಾಂಶ, nwrite);
  }

  ಆದರೆ(excess_len) {
    ಆದರೆ(!ದತ್ತಾಂಶ->req.ignorebody) {
      infof(ದತ್ತಾಂಶ,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2, ದತ್ತಾಂಶ->req.maxdownload,
            ದತ್ತಾಂಶ->req.bytecount);
      connclose(ದತ್ತಾಂಶ->conn);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((nwrite < nbytes) && !ದತ್ತಾಂಶ->req.ignorebody) {
    failf(ದತ್ತಾಂಶ, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          ದತ್ತಾಂಶ->ಗಣ.max_filesize, ದತ್ತಾಂಶ->req.bytecount);
    ಹಿಂತಿರುಗಿ CURLE_FILESIZE_EXCEEDED;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype cw_download = {
  "protocol",
  ಖಾಲಿ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಗಾತ್ರ(ರಚನೆ cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
ಸ್ಥಿರ CURLcode cw_raw_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t nbytes)
{
  ಆದರೆ(type & CLIENTWRITE_BODY && ದತ್ತಾಂಶ->ಗಣ.verbose && !ದತ್ತಾಂಶ->req.ignorebody) {
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ದತ್ತಾಂಶ_ಒಳ, buf, nbytes);
  }
  ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, nbytes);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype cw_raw = {
  "raw",
  ಖಾಲಿ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಗಾತ್ರ(ರಚನೆ Curl_cwriter)
};

ಸ್ಥಿರ ಶೂನ್ಯ cwriter_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_cwriter *writer)
{
  ರಚನೆ Curl_cwriter **anchor = &ದತ್ತಾಂಶ->req.writer.ರಾಶಿ;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  ಆಗ(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->ಮುಂದಿನ;
  writer->ಮುಂದಿನ = *anchor;
  *anchor = writer;
}

ಸ್ಥಿರ CURLcode do_init_writer_ರಾಶಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cwriter *writer;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(!ದತ್ತಾಂಶ->req.writer.ರಾಶಿ);
  ಫಲಿತಾಂಶ = Curl_cwriter_create(&ದತ್ತಾಂಶ->req.writer.ರಾಶಿ,
                               ದತ್ತಾಂಶ, &Curl_cwt_ಹೊರಹೋಗು, CURL_CW_CLIENT);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_cwriter_create(&writer, ದತ್ತಾಂಶ, &cw_download, CURL_CW_PROTOCOL);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);

  ಫಲಿತಾಂಶ = Curl_cwriter_create(&writer, ದತ್ತಾಂಶ, &cw_raw, CURL_CW_RAW);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                           size_t len)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_ಶೀರ್ಷಿಕೆ | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  ಆದರೆ(!ದತ್ತಾಂಶ->req.writer.ರಾಶಿ) {
    ಫಲಿತಾಂಶ = do_init_writer_ರಾಶಿ(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(ದತ್ತಾಂಶ->req.writer.ರಾಶಿ);
  }

  ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.writer.ರಾಶಿ, type, buf, len);
  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "client_write(type=%x, len=%zu) -> %d",
                 (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_client_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ->req.writer.ರಾಶಿ) {
    ಫಲಿತಾಂಶ = do_init_writer_ರಾಶಿ(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(ದತ್ತಾಂಶ->req.writer.ರಾಶಿ);
  }

  ಫಲಿತಾಂಶ = Curl_cwriter_flush(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.writer.ರಾಶಿ);
  CURL_TRC_WRITE(ದತ್ತಾಂಶ, "client_flush() -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(ರಚನೆ Curl_cwriter **pwriter,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  ರಚನೆ Curl_cwriter *writer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಶೂನ್ಯ *p;

  DEBUGASSERT(cwt->cwriter_ಗಾತ್ರ >= ಗಾತ್ರ(ರಚನೆ Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_ಗಾತ್ರ);
  ಆದರೆ(!p)
    ಹೋಗು ಹೊರಹೋಗು;

  writer = (ರಚನೆ Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  ಫಲಿತಾಂಶ = cwt->do_init(ದತ್ತಾಂಶ, writer);

ಹೊರಹೋಗು:
  *pwriter = ಫಲಿತಾಂಶ ? ಖಾಲಿ : writer;
  ಆದರೆ(ಫಲಿತಾಂಶ)
    curlx_free(writer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_cwriter_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ Curl_cwriter *writer)
{
  ಆದರೆ(writer) {
    writer->cwt->do_close(ದತ್ತಾಂಶ, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_ಎಣಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, Curl_cwriter_phase phase)
{
  ರಚನೆ Curl_cwriter *w;
  size_t n = 0;

  ಗಾಗಿ(w = ದತ್ತಾಂಶ->req.writer.ರಾಶಿ; w; w = w->ಮುಂದಿನ) {
    ಆದರೆ(w->phase == phase)
      ++n;
  }
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ CURLcode cwriter_ensure_pause_writer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cwriter *writer =
    Curl_cwriter_get_by_type(ದತ್ತಾಂಶ, &Curl_cwt_pause);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!writer) {
    ಫಲಿತಾಂಶ = Curl_cwriter_create(&writer, ದತ್ತಾಂಶ, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cwriter_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_cwriter *writer)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_cwriter **anchor = &ದತ್ತಾಂಶ->req.writer.ರಾಶಿ;

  ಆದರೆ(!*anchor) {
    ಫಲಿತಾಂಶ = do_init_writer_ರಾಶಿ(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    ಫಲಿತಾಂಶ = cwriter_ensure_pause_writer(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ Curl_cwriter *Curl_cwriter_get_by_name(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ರಚನೆ Curl_cwriter *writer;
  ಗಾಗಿ(writer = ದತ್ತಾಂಶ->req.writer.ರಾಶಿ; writer; writer = writer->ಮುಂದಿನ) {
    ಆದರೆ(!strcmp(name, writer->cwt->name))
      ಹಿಂತಿರುಗಿ writer;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ರಚನೆ Curl_cwriter *Curl_cwriter_get_by_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype *cwt)
{
  ರಚನೆ Curl_cwriter *writer;
  ಗಾಗಿ(writer = ದತ್ತಾಂಶ->req.writer.ರಾಶಿ; writer; writer = writer->ಮುಂದಿನ) {
    ಆದರೆ(writer->cwt == cwt)
      ಹಿಂತಿರುಗಿ writer;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode Curl_cwriter_unpause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ದತ್ತಾಂಶ->req.writer.paused = FALSE;
  ಹಿಂತಿರುಗಿ Curl_cwriter_flush(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.writer.ರಾಶಿ);
}

CURLcode Curl_creader_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader,
                           ಅಕ್ಷರ *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ಆದರೆ(!reader)
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
  ಹಿಂತಿರುಗಿ reader->crt->do_read(ದತ್ತಾಂಶ, reader, buf, blen, nread, eos);
}

ಶೂನ್ಯ Curl_creader_clear_eos(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader)
{
  ಆಗ(reader) {
    (ಶೂನ್ಯ)reader->crt->cntrl(ದತ್ತಾಂಶ, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->ಮುಂದಿನ;
  }
}

CURLcode Curl_creader_def_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_creader_def_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
}

CURLcode Curl_creader_def_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creader *reader,
                               ಅಕ್ಷರ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ಆದರೆ(reader->ಮುಂದಿನ)
    ಹಿಂತಿರುಗಿ reader->ಮುಂದಿನ->crt->do_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen,
                                      nread, eos);
  ಇಲ್ಲದಿದ್ದರೆ {
    *nread = 0;
    *eos = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ FALSE;
}

curl_off_t Curl_creader_def_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_creader *reader)
{
  ಹಿಂತಿರುಗಿ reader->ಮುಂದಿನ ?
         reader->ಮುಂದಿನ->crt->ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ) : -1;
}

CURLcode Curl_creader_def_resume_from(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creader *reader,
                                      curl_off_t ಸ್ಥಾನಾಂತರ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)ಸ್ಥಾನಾಂತರ;
  ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)opcode;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_creader_def_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_creader *reader)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_creader_def_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader, ಪೂರ್ಣಾಂಕ premature)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)premature;
}

ರಚನೆ cr_ಒಳ_ctx {
  ರಚನೆ Curl_creader super;
  curl_read_callback read_cb;
  ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ;
  curl_off_t ಒಟ್ಟು_len;
  curl_off_t read_len;
  CURLcode error_ಫಲಿತಾಂಶ;
  ಬಿಟ್(ನೋಡಿದ_eos);
  ಬಿಟ್(errored);
  ಬಿಟ್(has_ಬಳಸಿದ_cb);
  ಬಿಟ್(is_paused);
};

ಸ್ಥಿರ CURLcode cr_ಒಳ_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  ctx->read_cb = ದತ್ತಾಂಶ->state.fread_func;
  ctx->cb_user_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ->state.ಒಳ;
  ctx->ಒಟ್ಟು_len = -1;
  ctx->read_len = 0;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Real client reader to installed client callbacks. */
ಸ್ಥಿರ CURLcode cr_ಒಳ_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader,
                           ಅಕ್ಷರ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  ಆದರೆ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ಹಿಂತಿರುಗಿ ctx->error_ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(ctx->ನೋಡಿದ_eos) {
    *pnread = 0;
    *peos = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  /* respect length limitations */
  ಆದರೆ(ctx->ಒಟ್ಟು_len >= 0) {
    blen = curlx_sotouz_range(ctx->ಒಟ್ಟು_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ಆದರೆ(ctx->read_cb && blen) {
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_cr_ಒಳ_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_ದತ್ತಾಂಶ);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ctx->has_ಬಳಸಿದ_cb = TRUE;
  }

  ಆಯ್ಕೆ(nread) {
  ಸ್ಥಿತಿ 0:
    ಆದರೆ((ctx->ಒಟ್ಟು_len >= 0) && (ctx->read_len < ctx->ಒಟ್ಟು_len)) {
      failf(ದತ್ತಾಂಶ, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->ಒಟ್ಟು_len);
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ನೋಡಿದ_eos = TRUE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
    failf(ದತ್ತಾಂಶ, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
    ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(ದತ್ತಾಂಶ, "Read callback asked for PAUSE when not supported");
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    ಫಲಿತಾಂಶ = Curl_xfer_pause_send(ದತ್ತಾಂಶ, TRUE);
    ನಿಲ್ಲಿಸು; /* nothing was read */

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(nread > blen) {
      /* the read function returned a too large value */
      failf(ದತ್ತಾಂಶ, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    ctx->read_len += nread;
    ಆದರೆ(ctx->ಒಟ್ಟು_len >= 0)
      ctx->ನೋಡಿದ_eos = (ctx->read_len >= ctx->ಒಟ್ಟು_len);
    *pnread = nread;
    *peos = (bool)ctx->ನೋಡಿದ_eos;
    ನಿಲ್ಲಿಸು;
  }
  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->ಒಟ್ಟು_len, ctx->read_len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                *pnread, *peos);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cr_ಒಳ_needs_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ (bool)ctx->has_ಬಳಸಿದ_cb;
}

ಸ್ಥಿರ curl_off_t cr_ಒಳ_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ctx->ಒಟ್ಟು_len;
}

ಸ್ಥಿರ CURLcode cr_ಒಳ_resume_from(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_creader *reader,
                                  curl_off_t ಸ್ಥಾನಾಂತರ)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  ಪೂರ್ಣಾಂಕ seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  /* already started reading? */
  ಆದರೆ(ctx->read_len)
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.seek_func) {
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_seek_func);
    seekerr = ದತ್ತಾಂಶ->ಗಣ.seek_func(ದತ್ತಾಂಶ->ಗಣ.seek_client, ಸ್ಥಾನಾಂತರ, SEEK_SET);
    CURL_CBAPI_ಕೊನೆ(&guard);
  }

  ಆದರೆ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ಆದರೆ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(ದತ್ತಾಂಶ, "Could not seek stream");
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    ಮಾಡು {
      ರಚನೆ Curl_mapi_guard guard;
      ಅಕ್ಷರ scratch[4 * 1024];
      size_t readthisamountnow =
        (ಸ್ಥಾನಾಂತರ - passed > (curl_off_t)ಗಾತ್ರ(scratch)) ?
        ಗಾತ್ರ(scratch) :
        curlx_sotouz(ಸ್ಥಾನಾಂತರ - passed);
      size_t actuallyread;

      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_cr_ಒಳ_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_ದತ್ತಾಂಶ);
      CURL_CBAPI_ಕೊನೆ(&guard);

      passed += actuallyread;
      ಆದರೆ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ದತ್ತಾಂಶ, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
      }
    } ಆಗ(passed < ಸ್ಥಾನಾಂತರ);
  }

  /* now, decrease the size of the read */
  ಆದರೆ(ctx->ಒಟ್ಟು_len > 0) {
    ctx->ಒಟ್ಟು_len -= ಸ್ಥಾನಾಂತರ;

    ಆದರೆ(ctx->ಒಟ್ಟು_len <= 0) {
      failf(ದತ್ತಾಂಶ, "File already completely uploaded");
      ಹಿಂತಿರುಗಿ CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_ಒಳ_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ಆದರೆ(!ctx->has_ಬಳಸಿದ_cb)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.seek_func) {
    ರಚನೆ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಂಕ err;

    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_seek_func);
    err = ದತ್ತಾಂಶ->ಗಣ.seek_func(ದತ್ತಾಂಶ->ಗಣ.seek_client, 0, SEEK_SET);
    CURL_CBAPI_ಕೊನೆ(&guard);
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_in, rewind via set.seek_func -> %d", err);
    ಆದರೆ(err) {
      failf(ದತ್ತಾಂಶ, "seek callback returned error %d", err);
      ಹಿಂತಿರುಗಿ CURLE_SEND_FAIL_REWIND;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ioctl_func) {
    ರಚನೆ Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_ioctl_func);
    err = ದತ್ತಾಂಶ->ಗಣ.ioctl_func(ದತ್ತಾಂಶ, CURLIOCMD_RESTARTREAD,
                               ದತ್ತಾಂಶ->ಗಣ.ioctl_client);
    CURL_CBAPI_ಕೊನೆ(&guard);
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_in, rewind via set.ioctl_func -> %d", (ಪೂರ್ಣಾಂಕ)err);
    ಆದರೆ(err) {
      failf(ದತ್ತಾಂಶ, "ioctl callback returned error %d", (ಪೂರ್ಣಾಂಕ)err);
      ಹಿಂತಿರುಗಿ CURLE_SEND_FAIL_REWIND;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ದತ್ತಾಂಶ->state.fread_func == (curl_read_callback)fread) {
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಪೂರ್ಣಾಂಕ err = fseek(ದತ್ತಾಂಶ->state.ಒಳ, 0, SEEK_SET);
      CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_in, rewind via fseek -> %d(%d)",
                    (ಪೂರ್ಣಾಂಕ)err, (ಪೂರ್ಣಾಂಕ)errno);
      ಆದರೆ(err != -1)
        /* successful rewind */
        ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(ದತ್ತಾಂಶ, "necessary data rewind was not possible");
    ಹಿಂತಿರುಗಿ CURLE_SEND_FAIL_REWIND;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_ಒಳ_cntrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;

  ಆಯ್ಕೆ(opcode) {
  ಸ್ಥಿತಿ CURL_CRCNTRL_REWIND:
    ಹಿಂತಿರುಗಿ cr_ಒಳ_rewind(ದತ್ತಾಂಶ, reader);
  ಸ್ಥಿತಿ CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_CRCNTRL_CLEAR_EOS:
    ctx->ನೋಡಿದ_eos = FALSE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool cr_ಒಳ_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_ಒಳ_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ (bool)ctx->is_paused;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_ಒಳ = {
  "cr-in",
  cr_ಒಳ_init,
  cr_ಒಳ_read,
  Curl_creader_def_close,
  cr_ಒಳ_needs_rewind,
  cr_ಒಳ_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  cr_ಒಳ_resume_from,
  cr_ಒಳ_cntrl,
  cr_ಒಳ_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_ಒಳ_ctx)
};

CURLcode Curl_creader_create(ರಚನೆ Curl_creader **preader,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  ರಚನೆ Curl_creader *reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಶೂನ್ಯ *p;

  DEBUGASSERT(crt->creader_ಗಾತ್ರ >= ಗಾತ್ರ(ರಚನೆ Curl_creader));
  p = curlx_calloc(1, crt->creader_ಗಾತ್ರ);
  ಆದರೆ(!p)
    ಹೋಗು ಹೊರಹೋಗು;

  reader = (ರಚನೆ Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  ಫಲಿತಾಂಶ = crt->do_init(ದತ್ತಾಂಶ, reader);

ಹೊರಹೋಗು:
  *preader = ಫಲಿತಾಂಶ ? ಖಾಲಿ : reader;
  ಆದರೆ(ಫಲಿತಾಂಶ)
    curlx_free(reader);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_creader_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  ಆದರೆ(reader) {
    reader->crt->do_close(ದತ್ತಾಂಶ, reader);
    curlx_free(reader);
  }
}

ರಚನೆ cr_lc_ctx {
  ರಚನೆ Curl_creader super;
  ರಚನೆ bufq buf;
  ಬಿಟ್(read_eos);  /* we read an EOS from the next reader */
  ಬಿಟ್(eos);       /* we have returned an EOS */
  ಬಿಟ್(prev_cr);   /* the last byte was a CR */
};

ಸ್ಥಿರ CURLcode cr_lc_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_lc_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_lc_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_lc_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
ಸ್ಥಿರ CURLcode cr_lc_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader,
                           ಅಕ್ಷರ *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  ರಚನೆ cr_lc_ctx *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  size_t nread, i, ಆರಂಭ, n;
  bool eos;

  ಆದರೆ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf)) {
    ಆದರೆ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    ಫಲಿತಾಂಶ = Curl_creader_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen, &nread, &eos);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ctx->read_eos = eos;

    ಆದರೆ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ಆದರೆ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ಆದರೆ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    ಗಾಗಿ(i = ಆರಂಭ = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ಆದರೆ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        ಮುಂದುವರಿ;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, buf + ಆರಂಭ, i - ಆರಂಭ, &n);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಆರಂಭ = i + 1;
    }

    ಆದರೆ(ಆರಂಭ < i) { /* leftover */
      ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, buf + ಆರಂಭ, i - ಆರಂಭ, &n);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf));
  *peos = FALSE;
  ಫಲಿತಾಂಶ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ಆದರೆ(!ಫಲಿತಾಂಶ && ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

ಹೊರಹೋಗು:
  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread, *peos);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ curl_off_t cr_lc_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_lc_ctx)
};

ಸ್ಥಿರ CURLcode cr_lc_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&reader, ದತ್ತಾಂಶ, &cr_lc, CURL_CR_CONTENT_ENCODE);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಕೂಡಿಸು(ದತ್ತಾಂಶ, reader);

  ಆದರೆ(ಫಲಿತಾಂಶ && reader)
    Curl_creader_free(ದತ್ತಾಂಶ, reader);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode do_init_reader_ರಾಶಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_creader *r)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!ದತ್ತಾಂಶ->req.reader.ರಾಶಿ);

  ದತ್ತಾಂಶ->req.reader.ರಾಶಿ = r;
  clen = r->crt->ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  ಆದರೆ(clen &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_PREFER_LF_LINEENDS
    (ದತ್ತಾಂಶ->ಗಣ.crlf || ದತ್ತಾಂಶ->state.prefer_ascii)
#ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->ಗಣ.crlf
#ಆದರೆ_ಅಂತ್ಯ
    ) {
    ಫಲಿತಾಂಶ = cr_lc_ಕೂಡಿಸು(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_creader_ಗಣ_fread(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t len)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_creader *r;
  ರಚನೆ cr_ಒಳ_ctx *ctx;

  ಫಲಿತಾಂಶ = Curl_creader_create(&r, ದತ್ತಾಂಶ, &cr_ಒಳ, CURL_CR_CLIENT);
  ಆದರೆ(ಫಲಿತಾಂಶ || !r)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = r->ctx;
  ctx->ಒಟ್ಟು_len = len;

  cl_reset_reader(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = do_init_reader_ರಾಶಿ(ದತ್ತಾಂಶ, r);
ಹೊರಹೋಗು:
  CURL_TRC_READ(ದತ್ತಾಂಶ, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_creader_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_creader *reader)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_creader **anchor = &ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;

  ಆದರೆ(!*anchor) {
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  ಆಗ(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->ಮುಂದಿನ;
  reader->ಮುಂದಿನ = *anchor;
  *anchor = reader;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_creader_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *r)
{
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = do_init_reader_ರಾಶಿ(ದತ್ತಾಂಶ, r);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    Curl_creader_free(ದತ್ತಾಂಶ, r);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_client_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  ಆದರೆ(!ದತ್ತಾಂಶ->req.reader.ರಾಶಿ) {
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(ದತ್ತಾಂಶ->req.reader.ರಾಶಿ);
  }
  ಆದರೆ(!ದತ್ತಾಂಶ->req.reader_started) {
    Curl_rlimit_ಆರಂಭ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, Curl_pgrs_now(ದತ್ತಾಂಶ), -1);
    ದತ್ತಾಂಶ->req.reader_started = TRUE;
  }

  ಆದರೆ(Curl_rlimit_ಸಕ್ರಿಯ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, ಖಾಲಿ);
    ಆದರೆ(ul_avail <= 0) {
      ಫಲಿತಾಂಶ = CURLE_OK;
      *eos = FALSE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  ಫಲಿತಾಂಶ = Curl_creader_read(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.reader.ರಾಶಿ, buf, blen,
                             nread, eos);

ಹೊರಹೋಗು:
  CURL_TRC_READ(ದತ್ತಾಂಶ, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *nread, *eos);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_creader_needs_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  ಆಗ(reader) {
    ಆದರೆ(reader->crt->needs_rewind(ದತ್ತಾಂಶ, reader)) {
      CURL_TRC_READ(ದತ್ತಾಂಶ, "client reader needs rewind before next request");
      ಹಿಂತಿರುಗಿ TRUE;
    }
    reader = reader->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode cr_null_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader,
                             ಅಕ್ಷರ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  *pnread = 0;
  *peos = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_null_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ Curl_creader)
};

CURLcode Curl_creader_ಗಣ_null(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&r, ದತ್ತಾಂಶ, &cr_null, CURL_CR_CLIENT);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  cl_reset_reader(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ do_init_reader_ರಾಶಿ(ದತ್ತಾಂಶ, r);
}

ರಚನೆ cr_buf_ctx {
  ರಚನೆ Curl_creader super;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf;
  size_t blen;
  size_t ಸೂಚ್ಯಂಕ;
};

ಸ್ಥಿರ CURLcode cr_buf_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader,
                            ಅಕ್ಷರ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ರಚನೆ cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->ಸೂಚ್ಯಂಕ;

  ಆದರೆ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(nread > blen)
      nread = blen;
    ಸ್ಮರಣೆ_ನಕಲು(buf, ctx->buf + ctx->ಸೂಚ್ಯಂಕ, nread);
    *pnread = nread;
    ctx->ಸೂಚ್ಯಂಕ += nread;
    *peos = (ctx->ಸೂಚ್ಯಂಕ == ctx->blen);
  }
  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool cr_buf_needs_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ctx->ಸೂಚ್ಯಂಕ > 0;
}

ಸ್ಥಿರ CURLcode cr_buf_cntrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  ರಚನೆ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆಯ್ಕೆ(opcode) {
  ಸ್ಥಿತಿ CURL_CRCNTRL_REWIND:
    ctx->ಸೂಚ್ಯಂಕ = 0;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ curl_off_t cr_buf_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_buf_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ (curl_off_t)ctx->blen;
}

ಸ್ಥಿರ CURLcode cr_buf_resume_from(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_creader *reader,
                                   curl_off_t ಸ್ಥಾನಾಂತರ)
{
  ರಚನೆ cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  /* already started reading? */
  ಆದರೆ(ctx->ಸೂಚ್ಯಂಕ)
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(ಸ್ಥಾನಾಂತರ, 0, SIZE_MAX);
  ಆದರೆ(!boffset)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(boffset > ctx->blen)
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_buf_ctx)
};

CURLcode Curl_creader_ಗಣ_buf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_creader *r;
  ರಚನೆ cr_buf_ctx *ctx;

  ಫಲಿತಾಂಶ = Curl_creader_create(&r, ದತ್ತಾಂಶ, &cr_buf, CURL_CR_CLIENT);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->ಸೂಚ್ಯಂಕ = 0;

  cl_reset_reader(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = do_init_reader_ರಾಶಿ(ದತ್ತಾಂಶ, r);
ಹೊರಹೋಗು:
  CURL_TRC_READ(ದತ್ತಾಂಶ, "add buf reader, len=%zu -> %d", blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

curl_off_t Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  ಹಿಂತಿರುಗಿ r ? r->crt->ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ, r) : -1;
}

curl_off_t Curl_creader_client_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  ಆಗ(r && r->phase != CURL_CR_CLIENT)
    r = r->ಮುಂದಿನ;
  ಹಿಂತಿರುಗಿ r ? r->crt->ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ, r) : -1;
}

CURLcode Curl_creader_unpause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(reader) {
    ಫಲಿತಾಂಶ = reader->crt->cntrl(ದತ್ತಾಂಶ, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(ದತ್ತಾಂಶ, "unpausing %s -> %d", reader->crt->name, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    reader = reader->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_creader_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;

  ಆಗ(reader) {
    ಆದರೆ(reader->crt->is_paused(ದತ್ತಾಂಶ, reader))
      ಹಿಂತಿರುಗಿ TRUE;
    reader = reader->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_creader_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ premature)
{
  ರಚನೆ Curl_creader *reader = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ;
  ಆಗ(reader) {
    reader->crt->ಮುಗಿದ(ದತ್ತಾಂಶ, reader, premature);
    reader = reader->ಮುಂದಿನ;
  }
}

ರಚನೆ Curl_creader *Curl_creader_get_by_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype *crt)
{
  ರಚನೆ Curl_creader *r;
  ಗಾಗಿ(r = ದತ್ತಾಂಶ->req.reader.ರಾಶಿ; r; r = r->ಮುಂದಿನ) {
    ಆದರೆ(r->crt == crt)
      ಹಿಂತಿರುಗಿ r;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
