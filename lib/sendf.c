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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাব <linux/tcp.h>
#নতুবা_যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাব <netinet/tcp.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "cw-out.h"
#অন্তর্ভাব "cw-pause.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"

স্থির শূন্য cl_reset_writer(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cwriter *writer = তথ্য->req.writer.স্তূপ;
  যখন(writer) {
    তথ্য->req.writer.স্তূপ = writer->পরবর্তী;
    writer->cwt->do_close(তথ্য, writer);
    curlx_free(writer);
    writer = তথ্য->req.writer.স্তূপ;
  }
  তথ্য->req.writer.paused = FALSE;
}

স্থির শূন্য cl_reset_reader(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = তথ্য->req.reader.স্তূপ;
  তথ্য->req.reader_started = FALSE;
  যখন(reader) {
    তথ্য->req.reader.স্তূপ = reader->পরবর্তী;
    reader->crt->do_close(তথ্য, reader);
    curlx_free(reader);
    reader = তথ্য->req.reader.স্তূপ;
  }
}

শূন্য Curl_client_cleanup(কাঠামো Curl_easy *তথ্য)
{
  cl_reset_reader(তথ্য);
  cl_reset_writer(তথ্য);

  তথ্য->req.bytecount = 0;
  তথ্য->req.headerline = 0;
}

শূন্য Curl_client_reset(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(তথ্য, "client_reset, will rewind reader");
  }
  অন্যথা {
    CURL_TRC_READ(তথ্য, "client_reset, clear readers");
    cl_reset_reader(তথ্য);
  }
  cl_reset_writer(তথ্য);

  তথ্য->req.bytecount = 0;
  তথ্য->req.headerline = 0;
}

CURLcode Curl_client_start(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->req.rewind_read) {
    কাঠামো Curl_creader *r = তথ্য->req.reader.স্তূপ;
    CURLcode ফলাফল = CURLE_OK;

    CURL_TRC_READ(তথ্য, "client start, rewind readers");
    যখন(r) {
      ফলাফল = r->crt->cntrl(তথ্য, r, CURL_CRCNTRL_REWIND);
      যদি(ফলাফল) {
        failf(তথ্য, "rewind of client reader '%s' failed: %d",
              r->crt->name, (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      r = r->পরবর্তী;
    }
    তথ্য->req.rewind_read = FALSE;
    cl_reset_reader(তথ্য);
  }
  ফেরত CURLE_OK;
}

bool Curl_creader_will_rewind(কাঠামো Curl_easy *তথ্য)
{
  ফেরত (bool)তথ্য->req.rewind_read;
}

শূন্য Curl_creader_set_rewind(কাঠামো Curl_easy *তথ্য, bool enable)
{
  তথ্য->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_cwriter *writer)
{
  (শূন্য)তথ্য;
  (শূন্য)writer;
  ফেরত CURLE_OK;
}

CURLcode Curl_cwriter_def_write(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer)
{
  ফেরত Curl_cwriter_flush(তথ্য, writer->পরবর্তী);
}

শূন্য Curl_cwriter_def_close(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_cwriter *writer)
{
  (শূন্য)তথ্য;
  (শূন্য)writer;
}

স্থির size_t get_max_body_write_len(কাঠামো Curl_easy *তথ্য, curl_off_t limit)
{
  যদি(limit != -1) {
    /* How much more are we allowed to write? */
    ফেরত curlx_sotouz_range(limit - তথ্য->req.bytecount, 0, SIZE_MAX);
  }
  ফেরত SIZE_MAX;
}

কাঠামো cw_download_ctx {
  কাঠামো Curl_cwriter super;
  BIT(started_response);
  BIT(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
স্থির CURLcode cw_download_write(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                  ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  কাঠামো cw_download_ctx *ctx = writer->ctx;
  CURLcode ফলাফল;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  যদি(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || তথ্য->req.upload_done)) {
    Curl_pgrsTime(তথ্য, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  যদি(!(type & CLIENTWRITE_BODY)) {
    যদি(is_connect && তথ্য->set.suppress_connect_headers)
      ফেরত CURLE_OK;
#ifdef DEBUGBUILD
    যদি(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      ফেরত CURLE_OK;
#endif
    ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);
    CURL_TRC_WRITE(তথ্য, "download_write header(type=%x, blen=%zu) -> %d",
                   (unsigned পূর্ণসংখ্যা)type, nbytes, (পূর্ণসংখ্যা)ফলাফল);
    ফেরত ফলাফল;
  }

  যদি(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_start(&তথ্য->progress.dl.rlimit, Curl_pgrs_now(তথ্য),
                      তথ্য->req.আকার_2);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  যদি(তথ্য->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(তথ্য->conn);
    CURL_TRC_WRITE(তথ্য, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (unsigned পূর্ণসংখ্যা)type, nbytes);
    তথ্য->req.download_done = TRUE;
    যদি(তথ্য->info.header_আকার)
      /* if headers have been received, this is fine */
      ফেরত CURLE_OK;
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  যদি(তথ্য->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(তথ্য, তথ্য->req.maxdownload);
    যদি(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    যদি(nwrite == wmax) {
      তথ্য->req.download_done = TRUE;
    }

    যদি((type & CLIENTWRITE_EOS) && !তথ্য->req.no_body &&
       (তথ্য->req.আকার_2 > তথ্য->req.bytecount)) {
      failf(তথ্য, "end of response with %" FMT_OFF_T " bytes missing",
            তথ্য->req.আকার_2 - তথ্য->req.bytecount);
      ফেরত CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  যদি(তথ্য->set.max_filesize && !তথ্য->req.ignorebody) {
    size_t wmax = get_max_body_write_len(তথ্য, তথ্য->set.max_filesize);
    যদি(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  যদি(!তথ্য->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nwrite);
    CURL_TRC_WRITE(তথ্য, "download_write body(type=%x, blen=%zu) -> %d",
                   (unsigned পূর্ণসংখ্যা)type, nbytes, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Update stats, write and report progress */
  যদি(nwrite) {
    তথ্য->req.bytecount += nwrite;
    Curl_pgrs_download_inc(তথ্য, nwrite);
  }

  যদি(excess_len) {
    যদি(!তথ্য->req.ignorebody) {
      infof(তথ্য,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, তথ্য->req.আকার_2, তথ্য->req.maxdownload,
            তথ্য->req.bytecount);
      connclose(তথ্য->conn);
    }
  }
  অন্যথা যদি((nwrite < nbytes) && !তথ্য->req.ignorebody) {
    failf(তথ্য, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          তথ্য->set.max_filesize, তথ্য->req.bytecount);
    ফেরত CURLE_FILESIZE_EXCEEDED;
  }

  ফেরত CURLE_OK;
}

স্থির ধ্রুবক কাঠামো Curl_cwtype cw_download = {
  "protocol",
  ফাঁকা,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  আকার(কাঠামো cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
স্থির CURLcode cw_raw_write(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                             ধ্রুবক অক্ষর *buf, size_t nbytes)
{
  যদি(type & CLIENTWRITE_BODY && তথ্য->set.verbose && !তথ্য->req.ignorebody) {
    Curl_debug(তথ্য, CURLINFO_তথ্য_IN, buf, nbytes);
  }
  ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, nbytes);
}

স্থির ধ্রুবক কাঠামো Curl_cwtype cw_raw = {
  "raw",
  ফাঁকা,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  আকার(কাঠামো Curl_cwriter)
};

স্থির শূন্য cwriter_add(কাঠামো Curl_easy *তথ্য,
                        কাঠামো Curl_cwriter *writer)
{
  কাঠামো Curl_cwriter **anchor = &তথ্য->req.writer.স্তূপ;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  যখন(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->পরবর্তী;
  writer->পরবর্তী = *anchor;
  *anchor = writer;
}

স্থির CURLcode do_init_writer_স্তূপ(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cwriter *writer;
  CURLcode ফলাফল;

  DEBUGASSERT(!তথ্য->req.writer.স্তূপ);
  ফলাফল = Curl_cwriter_create(&তথ্য->req.writer.স্তূপ,
                               তথ্য, &Curl_cwt_out, CURL_CW_CLIENT);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_cwriter_create(&writer, তথ্য, &cw_download, CURL_CW_PROTOCOL);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  cwriter_add(তথ্য, writer);

  ফলাফল = Curl_cwriter_create(&writer, তথ্য, &cw_raw, CURL_CW_RAW);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  cwriter_add(তথ্য, writer);

  ফেরত ফলাফল;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *buf,
                           size_t len)
{
  CURLcode ফলাফল;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_HEADER | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  যদি(!তথ্য->req.writer.স্তূপ) {
    ফলাফল = do_init_writer_স্তূপ(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    DEBUGASSERT(তথ্য->req.writer.স্তূপ);
  }

  ফলাফল = Curl_cwriter_write(তথ্য, তথ্য->req.writer.স্তূপ, type, buf, len);
  CURL_TRC_WRITE(তথ্য, "client_write(type=%x, len=%zu) -> %d",
                 (unsigned পূর্ণসংখ্যা)type, len, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

CURLcode Curl_client_flush(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;

  যদি(!তথ্য->req.writer.স্তূপ) {
    ফলাফল = do_init_writer_স্তূপ(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    DEBUGASSERT(তথ্য->req.writer.স্তূপ);
  }

  ফলাফল = Curl_cwriter_flush(তথ্য, তথ্য->req.writer.স্তূপ);
  CURL_TRC_WRITE(তথ্য, "client_flush() -> %d", (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(কাঠামো Curl_cwriter **pwriter,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  কাঠামো Curl_cwriter *writer = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  শূন্য *p;

  DEBUGASSERT(cwt->cwriter_আকার >= আকার(কাঠামো Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_আকার);
  যদি(!p)
    যাও out;

  writer = (কাঠামো Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  ফলাফল = cwt->do_init(তথ্য, writer);

out:
  *pwriter = ফলাফল ? ফাঁকা : writer;
  যদি(ফলাফল)
    curlx_free(writer);
  ফেরত ফলাফল;
}

শূন্য Curl_cwriter_free(কাঠামো Curl_easy *তথ্য,
                       কাঠামো Curl_cwriter *writer)
{
  যদি(writer) {
    writer->cwt->do_close(তথ্য, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_গুনতি(কাঠামো Curl_easy *তথ্য, Curl_cwriter_phase phase)
{
  কাঠামো Curl_cwriter *w;
  size_t n = 0;

  জন্য(w = তথ্য->req.writer.স্তূপ; w; w = w->পরবর্তী) {
    যদি(w->phase == phase)
      ++n;
  }
  ফেরত n;
}

স্থির CURLcode cwriter_ensure_pause_writer(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cwriter *writer =
    Curl_cwriter_get_by_type(তথ্য, &Curl_cwt_pause);
  CURLcode ফলাফল = CURLE_OK;

  যদি(!writer) {
    ফলাফল = Curl_cwriter_create(&writer, তথ্য, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    যদি(!ফলাফল)
      cwriter_add(তথ্য, writer);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cwriter_add(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_cwriter *writer)
{
  CURLcode ফলাফল;
  কাঠামো Curl_cwriter **anchor = &তথ্য->req.writer.স্তূপ;

  যদি(!*anchor) {
    ফলাফল = do_init_writer_স্তূপ(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    ফলাফল = cwriter_ensure_pause_writer(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  cwriter_add(তথ্য, writer);
  ফেরত CURLE_OK;
}

কাঠামো Curl_cwriter *Curl_cwriter_get_by_name(কাঠামো Curl_easy *তথ্য,
                                              ধ্রুবক অক্ষর *name)
{
  কাঠামো Curl_cwriter *writer;
  জন্য(writer = তথ্য->req.writer.স্তূপ; writer; writer = writer->পরবর্তী) {
    যদি(!strcmp(name, writer->cwt->name))
      ফেরত writer;
  }
  ফেরত ফাঁকা;
}

কাঠামো Curl_cwriter *Curl_cwriter_get_by_type(কাঠামো Curl_easy *তথ্য,
                                              ধ্রুবক কাঠামো Curl_cwtype *cwt)
{
  কাঠামো Curl_cwriter *writer;
  জন্য(writer = তথ্য->req.writer.স্তূপ; writer; writer = writer->পরবর্তী) {
    যদি(writer->cwt == cwt)
      ফেরত writer;
  }
  ফেরত ফাঁকা;
}

CURLcode Curl_cwriter_unpause(কাঠামো Curl_easy *তথ্য)
{
  তথ্য->req.writer.paused = FALSE;
  ফেরত Curl_cwriter_flush(তথ্য, তথ্য->req.writer.স্তূপ);
}

CURLcode Curl_creader_read(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader,
                           অক্ষর *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  যদি(!reader)
    ফেরত CURLE_READ_ERROR;
  ফেরত reader->crt->do_read(তথ্য, reader, buf, blen, nread, eos);
}

শূন্য Curl_creader_clear_eos(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader)
{
  যখন(reader) {
    (শূন্য)reader->crt->cntrl(তথ্য, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->পরবর্তী;
  }
}

CURLcode Curl_creader_def_init(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত CURLE_OK;
}

শূন্য Curl_creader_def_close(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
}

CURLcode Curl_creader_def_read(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creader *reader,
                               অক্ষর *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  যদি(reader->পরবর্তী)
    ফেরত reader->পরবর্তী->crt->do_read(তথ্য, reader->পরবর্তী, buf, blen,
                                      nread, eos);
  অন্যথা {
    *nread = 0;
    *eos = FALSE;
    ফেরত CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত FALSE;
}

curl_off_t Curl_creader_def_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো Curl_creader *reader)
{
  ফেরত reader->পরবর্তী ?
         reader->পরবর্তী->crt->যোগ_দৈর্ঘ্য(তথ্য, reader->পরবর্তী) : -1;
}

CURLcode Curl_creader_def_resume_from(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_creader *reader,
                                      curl_off_t offset)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  (শূন্য)offset;
  ফেরত CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  (শূন্য)opcode;
  ফেরত CURLE_OK;
}

bool Curl_creader_def_is_paused(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_creader *reader)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত FALSE;
}

শূন্য Curl_creader_def_done(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader, পূর্ণসংখ্যা premature)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  (শূন্য)premature;
}

কাঠামো cr_in_ctx {
  কাঠামো Curl_creader super;
  curl_read_callback read_cb;
  শূন্য *cb_user_তথ্য;
  curl_off_t যোগ_len;
  curl_off_t read_len;
  CURLcode error_ফলাফল;
  BIT(seen_eos);
  BIT(errored);
  BIT(has_used_cb);
  BIT(is_paused);
};

স্থির CURLcode cr_in_init(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  ctx->read_cb = তথ্য->state.fread_func;
  ctx->cb_user_তথ্য = তথ্য->state.in;
  ctx->যোগ_len = -1;
  ctx->read_len = 0;
  ফেরত CURLE_OK;
}

/* Real client reader to installed client callbacks. */
স্থির CURLcode cr_in_read(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader,
                           অক্ষর *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  যদি(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    ফেরত ctx->error_ফলাফল;
  }
  যদি(ctx->seen_eos) {
    *pnread = 0;
    *peos = TRUE;
    ফেরত CURLE_OK;
  }
  /* respect length limitations */
  যদি(ctx->যোগ_len >= 0) {
    blen = curlx_sotouz_range(ctx->যোগ_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  যদি(ctx->read_cb && blen) {
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_cr_in_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_তথ্য);
    CURL_CBAPI_END(&guard);
    ctx->has_used_cb = TRUE;
  }

  নির্বাচন(nread) {
  ক্ষেত্র 0:
    যদি((ctx->যোগ_len >= 0) && (ctx->read_len < ctx->যোগ_len)) {
      failf(তথ্য, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->যোগ_len);
      ফলাফল = CURLE_READ_ERROR;
      বিরতি;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    বিরতি;

  ক্ষেত্র CURL_READFUNC_ABORT:
    failf(তথ্য, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    বিরতি;

  ক্ষেত্র CURL_READFUNC_PAUSE:
    যদি(তথ্য->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(তথ্য, "Read callback asked for PAUSE when not supported");
      ফলাফল = CURLE_READ_ERROR;
      বিরতি;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(তথ্য, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    ফলাফল = Curl_xfer_pause_send(তথ্য, TRUE);
    বিরতি; /* nothing was read */

  পূর্বনির্ধারিত:
    যদি(nread > blen) {
      /* the read function returned a too large value */
      failf(তথ্য, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ফলাফল = CURLE_READ_ERROR;
      ফলাফল = CURLE_READ_ERROR;
      বিরতি;
    }
    ctx->read_len += nread;
    যদি(ctx->যোগ_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->যোগ_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    বিরতি;
  }
  CURL_TRC_READ(তথ্য, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->যোগ_len, ctx->read_len, (পূর্ণসংখ্যা)ফলাফল,
                *pnread, *peos);
  ফেরত ফলাফল;
}

স্থির bool cr_in_needs_rewind(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creader *reader)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত (bool)ctx->has_used_cb;
}

স্থির curl_off_t cr_in_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_creader *reader)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত ctx->যোগ_len;
}

স্থির CURLcode cr_in_resume_from(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_creader *reader,
                                  curl_off_t offset)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  পূর্ণসংখ্যা seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(তথ্য->conn);
  /* already started reading? */
  যদি(ctx->read_len)
    ফেরত CURLE_READ_ERROR;

  যদি(তথ্য->set.seek_func) {
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_seek_func);
    seekerr = তথ্য->set.seek_func(তথ্য->set.seek_client, offset, SEEK_SET);
    CURL_CBAPI_END(&guard);
  }

  যদি(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(তথ্য, "Could not seek stream");
      ফেরত CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    করো {
      কাঠামো Curl_mapi_guard guard;
      অক্ষর scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)আকার(scratch)) ?
        আকার(scratch) :
        curlx_sotouz(offset - passed);
      size_t actuallyread;

      CURL_CBAPI_START(&guard, তথ্য, easy_cr_in_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_তথ্য);
      CURL_CBAPI_END(&guard);

      passed += actuallyread;
      যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(তথ্য, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        ফেরত CURLE_READ_ERROR;
      }
    } যখন(passed < offset);
  }

  /* now, decrease the size of the read */
  যদি(ctx->যোগ_len > 0) {
    ctx->যোগ_len -= offset;

    যদি(ctx->যোগ_len <= 0) {
      failf(তথ্য, "File already completely uploaded");
      ফেরত CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  ফেরত CURLE_OK;
}

স্থির CURLcode cr_in_rewind(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  যদি(!ctx->has_used_cb)
    ফেরত CURLE_OK;

  যদি(তথ্য->set.seek_func) {
    কাঠামো Curl_mapi_guard guard;
    পূর্ণসংখ্যা err;

    CURL_CBAPI_START(&guard, তথ্য, easy_seek_func);
    err = তথ্য->set.seek_func(তথ্য->set.seek_client, 0, SEEK_SET);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(তথ্য, "cr_in, rewind via set.seek_func -> %d", err);
    যদি(err) {
      failf(তথ্য, "seek callback returned error %d", err);
      ফেরত CURLE_SEND_FAIL_REWIND;
    }
  }
  অন্যথা যদি(তথ্য->set.ioctl_func) {
    কাঠামো Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_START(&guard, তথ্য, easy_ioctl_func);
    err = তথ্য->set.ioctl_func(তথ্য, CURLIOCMD_RESTARTREAD,
                               তথ্য->set.ioctl_client);
    CURL_CBAPI_END(&guard);
    CURL_TRC_READ(তথ্য, "cr_in, rewind via set.ioctl_func -> %d", (পূর্ণসংখ্যা)err);
    যদি(err) {
      failf(তথ্য, "ioctl callback returned error %d", (পূর্ণসংখ্যা)err);
      ফেরত CURLE_SEND_FAIL_REWIND;
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
    যদি(তথ্য->state.fread_func == (curl_read_callback)fread) {
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
      পূর্ণসংখ্যা err = fseek(তথ্য->state.in, 0, SEEK_SET);
      CURL_TRC_READ(তথ্য, "cr_in, rewind via fseek -> %d(%d)",
                    (পূর্ণসংখ্যা)err, (পূর্ণসংখ্যা)errno);
      যদি(err != -1)
        /* successful rewind */
        ফেরত CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(তথ্য, "necessary data rewind was not possible");
    ফেরত CURLE_SEND_FAIL_REWIND;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cr_in_cntrl(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;

  নির্বাচন(opcode) {
  ক্ষেত্র CURL_CRCNTRL_REWIND:
    ফেরত cr_in_rewind(তথ্য, reader);
  ক্ষেত্র CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    বিরতি;
  ক্ষেত্র CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির bool cr_in_is_paused(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader)
{
  কাঠামো cr_in_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত (bool)ctx->is_paused;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_in = {
  "cr-in",
  cr_in_init,
  cr_in_read,
  Curl_creader_def_close,
  cr_in_needs_rewind,
  cr_in_যোগ_দৈর্ঘ্য,
  cr_in_resume_from,
  cr_in_cntrl,
  cr_in_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_in_ctx)
};

CURLcode Curl_creader_create(কাঠামো Curl_creader **preader,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  কাঠামো Curl_creader *reader = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  শূন্য *p;

  DEBUGASSERT(crt->creader_আকার >= আকার(কাঠামো Curl_creader));
  p = curlx_calloc(1, crt->creader_আকার);
  যদি(!p)
    যাও out;

  reader = (কাঠামো Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  ফলাফল = crt->do_init(তথ্য, reader);

out:
  *preader = ফলাফল ? ফাঁকা : reader;
  যদি(ফলাফল)
    curlx_free(reader);
  ফেরত ফলাফল;
}

শূন্য Curl_creader_free(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  যদি(reader) {
    reader->crt->do_close(তথ্য, reader);
    curlx_free(reader);
  }
}

কাঠামো cr_lc_ctx {
  কাঠামো Curl_creader super;
  কাঠামো bufq buf;
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(eos);       /* we have returned an EOS */
  BIT(পূর্ব_cr);   /* the last byte was a CR */
};

স্থির CURLcode cr_lc_init(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  কাঠামো cr_lc_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ফেরত CURLE_OK;
}

স্থির শূন্য cr_lc_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  কাঠামো cr_lc_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
স্থির CURLcode cr_lc_read(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader,
                           অক্ষর *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  কাঠামো cr_lc_ctx *ctx = reader->ctx;
  CURLcode ফলাফল;
  size_t nread, i, start, n;
  bool eos;

  যদি(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    ফেরত CURLE_OK;
  }

  যদি(Curl_bufq_is_empty(&ctx->buf)) {
    যদি(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      ফেরত CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    ফলাফল = Curl_creader_read(তথ্য, reader->পরবর্তী, buf, blen, &nread, &eos);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ctx->read_eos = eos;

    যদি(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      যদি(nread)
        ctx->পূর্ব_cr = (buf[nread - 1] == '\r');
      যদি(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      যাও out;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    জন্য(i = start = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      যদি((buf[i] != '\n') || ctx->পূর্ব_cr) {
        ctx->পূর্ব_cr = (buf[i] == '\r');
        চালু;
      }
      ctx->পূর্ব_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      ফলাফল = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      যদি(!ফলাফল)
        ফলাফল = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      start = i + 1;
    }

    যদি(start < i) { /* leftover */
      ফলাফল = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_empty(&ctx->buf));
  *peos = FALSE;
  ফলাফল = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  যদি(!ফলাফল && ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

out:
  CURL_TRC_READ(তথ্য, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণসংখ্যা)ফলাফল, *pnread, *peos);
  ফেরত ফলাফল;
}

স্থির curl_off_t cr_lc_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত -1;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_lc_ctx)
};

স্থির CURLcode cr_lc_add(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = ফাঁকা;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&reader, তথ্য, &cr_lc, CURL_CR_CONTENT_ENCODE);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_add(তথ্য, reader);

  যদি(ফলাফল && reader)
    Curl_creader_free(তথ্য, reader);
  ফেরত ফলাফল;
}

স্থির CURLcode do_init_reader_স্তূপ(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_creader *r)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!তথ্য->req.reader.স্তূপ);

  তথ্য->req.reader.স্তূপ = r;
  clen = r->crt->যোগ_দৈর্ঘ্য(তথ্য, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  যদি(clen &&
#ifdef CURL_PREFER_LF_LINEENDS
    (তথ্য->set.crlf || তথ্য->state.prefer_ascii)
#অন্যথা
    তথ্য->set.crlf
#endif
    ) {
    ফলাফল = cr_lc_add(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত ফলাফল;
}

CURLcode Curl_creader_set_fread(কাঠামো Curl_easy *তথ্য, curl_off_t len)
{
  CURLcode ফলাফল;
  কাঠামো Curl_creader *r;
  কাঠামো cr_in_ctx *ctx;

  ফলাফল = Curl_creader_create(&r, তথ্য, &cr_in, CURL_CR_CLIENT);
  যদি(ফলাফল || !r)
    যাও out;
  ctx = r->ctx;
  ctx->যোগ_len = len;

  cl_reset_reader(তথ্য);
  ফলাফল = do_init_reader_স্তূপ(তথ্য, r);
out:
  CURL_TRC_READ(তথ্য, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

CURLcode Curl_creader_add(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_creader *reader)
{
  CURLcode ফলাফল;
  কাঠামো Curl_creader **anchor = &তথ্য->req.reader.স্তূপ;

  যদি(!*anchor) {
    ফলাফল = Curl_creader_set_fread(তথ্য, তথ্য->state.infilesize);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  যখন(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->পরবর্তী;
  reader->পরবর্তী = *anchor;
  *anchor = reader;
  ফেরত CURLE_OK;
}

CURLcode Curl_creader_set(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *r)
{
  CURLcode ফলাফল;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(তথ্য);
  ফলাফল = do_init_reader_স্তূপ(তথ্য, r);
  যদি(ফলাফল)
    Curl_creader_free(তথ্য, r);
  ফেরত ফলাফল;
}

CURLcode Curl_client_read(কাঠামো Curl_easy *তথ্য, অক্ষর *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode ফলাফল;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  যদি(!তথ্য->req.reader.স্তূপ) {
    ফলাফল = Curl_creader_set_fread(তথ্য, তথ্য->state.infilesize);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    DEBUGASSERT(তথ্য->req.reader.স্তূপ);
  }
  যদি(!তথ্য->req.reader_started) {
    Curl_rlimit_start(&তথ্য->progress.ul.rlimit, Curl_pgrs_now(তথ্য), -1);
    তথ্য->req.reader_started = TRUE;
  }

  যদি(Curl_rlimit_active(&তথ্য->progress.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&তথ্য->progress.ul.rlimit, ফাঁকা);
    যদি(ul_avail <= 0) {
      ফলাফল = CURLE_OK;
      *eos = FALSE;
      যাও out;
    }
    যদি(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  ফলাফল = Curl_creader_read(তথ্য, তথ্য->req.reader.স্তূপ, buf, blen,
                             nread, eos);

out:
  CURL_TRC_READ(তথ্য, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (পূর্ণসংখ্যা)ফলাফল, *nread, *eos);
  ফেরত ফলাফল;
}

bool Curl_creader_needs_rewind(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = তথ্য->req.reader.স্তূপ;
  যখন(reader) {
    যদি(reader->crt->needs_rewind(তথ্য, reader)) {
      CURL_TRC_READ(তথ্য, "client reader needs rewind before next request");
      ফেরত TRUE;
    }
    reader = reader->পরবর্তী;
  }
  ফেরত FALSE;
}

স্থির CURLcode cr_null_read(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader,
                             অক্ষর *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (শূন্য)তথ্য;
  (শূন্য)reader;
  (শূন্য)buf;
  (শূন্য)blen;
  *pnread = 0;
  *peos = TRUE;
  ফেরত CURLE_OK;
}

স্থির curl_off_t cr_null_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত 0;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো Curl_creader)
};

CURLcode Curl_creader_set_null(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&r, তথ্য, &cr_null, CURL_CR_CLIENT);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  cl_reset_reader(তথ্য);
  ফেরত do_init_reader_স্তূপ(তথ্য, r);
}

কাঠামো cr_buf_ctx {
  কাঠামো Curl_creader super;
  ধ্রুবক অক্ষর *buf;
  size_t blen;
  size_t সূচক;
};

স্থির CURLcode cr_buf_read(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader,
                            অক্ষর *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  কাঠামো cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->সূচক;

  যদি(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  অন্যথা {
    যদি(nread > blen)
      nread = blen;
    memcpy(buf, ctx->buf + ctx->সূচক, nread);
    *pnread = nread;
    ctx->সূচক += nread;
    *peos = (ctx->সূচক == ctx->blen);
  }
  CURL_TRC_READ(তথ্য, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  ফেরত CURLE_OK;
}

স্থির bool cr_buf_needs_rewind(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_creader *reader)
{
  কাঠামো cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত ctx->সূচক > 0;
}

স্থির CURLcode cr_buf_cntrl(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  কাঠামো cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  নির্বাচন(opcode) {
  ক্ষেত্র CURL_CRCNTRL_REWIND:
    ctx->সূচক = 0;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির curl_off_t cr_buf_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_creader *reader)
{
  কাঠামো cr_buf_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত (curl_off_t)ctx->blen;
}

স্থির CURLcode cr_buf_resume_from(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_creader *reader,
                                   curl_off_t offset)
{
  কাঠামো cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (শূন্য)তথ্য;
  DEBUGASSERT(তথ্য->conn);
  /* already started reading? */
  যদি(ctx->সূচক)
    ফেরত CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(offset, 0, SIZE_MAX);
  যদি(!boffset)
    ফেরত CURLE_OK;
  যদি(boffset > ctx->blen)
    ফেরত CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  ফেরত CURLE_OK;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_যোগ_দৈর্ঘ্য,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_buf_ctx)
};

CURLcode Curl_creader_set_buf(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *buf, size_t blen)
{
  CURLcode ফলাফল;
  কাঠামো Curl_creader *r;
  কাঠামো cr_buf_ctx *ctx;

  ফলাফল = Curl_creader_create(&r, তথ্য, &cr_buf, CURL_CR_CLIENT);
  যদি(ফলাফল)
    যাও out;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->সূচক = 0;

  cl_reset_reader(তথ্য);
  ফলাফল = do_init_reader_স্তূপ(তথ্য, r);
out:
  CURL_TRC_READ(তথ্য, "add buf reader, len=%zu -> %d", blen, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

curl_off_t Curl_creader_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = তথ্য->req.reader.স্তূপ;
  ফেরত r ? r->crt->যোগ_দৈর্ঘ্য(তথ্য, r) : -1;
}

curl_off_t Curl_creader_client_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = তথ্য->req.reader.স্তূপ;
  যখন(r && r->phase != CURL_CR_CLIENT)
    r = r->পরবর্তী;
  ফেরত r ? r->crt->যোগ_দৈর্ঘ্য(তথ্য, r) : -1;
}

CURLcode Curl_creader_unpause(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = তথ্য->req.reader.স্তূপ;
  CURLcode ফলাফল = CURLE_OK;

  যখন(reader) {
    ফলাফল = reader->crt->cntrl(তথ্য, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(তথ্য, "unpausing %s -> %d", reader->crt->name, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      বিরতি;
    reader = reader->পরবর্তী;
  }
  ফেরত ফলাফল;
}

bool Curl_creader_is_paused(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = তথ্য->req.reader.স্তূপ;

  যখন(reader) {
    যদি(reader->crt->is_paused(তথ্য, reader))
      ফেরত TRUE;
    reader = reader->পরবর্তী;
  }
  ফেরত FALSE;
}

শূন্য Curl_creader_done(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা premature)
{
  কাঠামো Curl_creader *reader = তথ্য->req.reader.স্তূপ;
  যখন(reader) {
    reader->crt->done(তথ্য, reader, premature);
    reader = reader->পরবর্তী;
  }
}

কাঠামো Curl_creader *Curl_creader_get_by_type(কাঠামো Curl_easy *তথ্য,
                                              ধ্রুবক কাঠামো Curl_crtype *crt)
{
  কাঠামো Curl_creader *r;
  জন্য(r = তথ্য->req.reader.স্তূপ; r; r = r->পরবর্তী) {
    যদি(r->crt == crt)
      ফেরত r;
  }
  ফেরত ফাঁকা;
}
