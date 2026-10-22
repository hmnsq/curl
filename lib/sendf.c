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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LINUX_TCP_H
#అన్తర్భావ <linux/tcp.h>
#లేదావేళ defined(HAVE_NETINET_TCP_H)
#అన్తర్భావ <netinet/tcp.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "cw-out.h"
#అన్తర్భావ "cw-pause.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"

స్థిర శూన్య cl_reset_writer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cwriter *writer = దత్తాంశం->req.writer.దొంతర;
  అయితే(writer) {
    దత్తాంశం->req.writer.దొంతర = writer->తదుపరి;
    writer->cwt->do_close(దత్తాంశం, writer);
    curlx_free(writer);
    writer = దత్తాంశం->req.writer.దొంతర;
  }
  దత్తాంశం->req.writer.paused = FALSE;
}

స్థిర శూన్య cl_reset_reader(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = దత్తాంశం->req.reader.దొంతర;
  దత్తాంశం->req.reader_started = FALSE;
  అయితే(reader) {
    దత్తాంశం->req.reader.దొంతర = reader->తదుపరి;
    reader->crt->do_close(దత్తాంశం, reader);
    curlx_free(reader);
    reader = దత్తాంశం->req.reader.దొంతర;
  }
}

శూన్య Curl_client_cleanup(నిర్మాణం Curl_easy *దత్తాంశం)
{
  cl_reset_reader(దత్తాంశం);
  cl_reset_writer(దత్తాంశం);

  దత్తాంశం->req.bytecount = 0;
  దత్తాంశం->req.headerline = 0;
}

శూన్య Curl_client_reset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->req.rewind_read) {
    /* already requested */
    CURL_TRC_READ(దత్తాంశం, "client_reset, will rewind reader");
  }
  లేకపోతే {
    CURL_TRC_READ(దత్తాంశం, "client_reset, clear readers");
    cl_reset_reader(దత్తాంశం);
  }
  cl_reset_writer(దత్తాంశం);

  దత్తాంశం->req.bytecount = 0;
  దత్తాంశం->req.headerline = 0;
}

CURLcode Curl_client_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->req.rewind_read) {
    నిర్మాణం Curl_creader *r = దత్తాంశం->req.reader.దొంతర;
    CURLcode ఫలితం = CURLE_OK;

    CURL_TRC_READ(దత్తాంశం, "client start, rewind readers");
    అయితే(r) {
      ఫలితం = r->crt->cntrl(దత్తాంశం, r, CURL_CRCNTRL_REWIND);
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "rewind of client reader '%s' failed: %d",
              r->crt->name, (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      r = r->తదుపరి;
    }
    దత్తాంశం->req.rewind_read = FALSE;
    cl_reset_reader(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

bool Curl_creader_will_rewind(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి (bool)దత్తాంశం->req.rewind_read;
}

శూన్య Curl_creader_సమితి_rewind(నిర్మాణం Curl_easy *దత్తాంశం, bool enable)
{
  దత్తాంశం->req.rewind_read = !!enable;
}

CURLcode Curl_cwriter_def_init(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_cwriter *writer)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)writer;
  తిరిగి CURLE_OK;
}

CURLcode Curl_cwriter_def_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);
}

CURLcode Curl_cwriter_def_flush(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer)
{
  తిరిగి Curl_cwriter_flush(దత్తాంశం, writer->తదుపరి);
}

శూన్య Curl_cwriter_def_close(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_cwriter *writer)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)writer;
}

స్థిర size_t get_max_body_write_len(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t హద్దు)
{
  ఒకవేళ(హద్దు != -1) {
    /* How much more are we allowed to write? */
    తిరిగి curlx_sotouz_range(హద్దు - దత్తాంశం->req.bytecount, 0, SIZE_MAX);
  }
  తిరిగి SIZE_MAX;
}

నిర్మాణం cw_download_ctx {
  నిర్మాణం Curl_cwriter super;
  బిట్టు(started_response);
  బిట్టు(started_body);
};

/* Download client writer in phase CURL_CW_PROTOCOL that
 * sees the "real" download body data. */
స్థిర CURLcode cw_download_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                  స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  నిర్మాణం cw_download_ctx *ctx = writer->ctx;
  CURLcode ఫలితం;
  size_t nwrite, excess_len = 0;
  bool is_connect = !!(type & CLIENTWRITE_CONNECT);

  ఒకవేళ(!ctx->started_response &&
     !(type & CLIENTWRITE_CONNECT) &&
     (!(type & CLIENTWRITE_INFO) || దత్తాంశం->req.upload_పూర్తి)) {
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_STARTTRANSFER);
    ctx->started_response = TRUE;
  }

  ఒకవేళ(!(type & CLIENTWRITE_BODY)) {
    ఒకవేళ(is_connect && దత్తాంశం->సమితి.suppress_connect_శీర్షికలు)
      తిరిగి CURLE_OK;
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(is_connect && getenv("CURL_DBG_SUPPRESS_CONNECT_HDS"))
      తిరిగి CURLE_OK;
#అయితే_ముగింపు
    ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);
    CURL_TRC_WRITE(దత్తాంశం, "download_write header(type=%x, blen=%zu) -> %d",
                   (గుర్తులేని పూర్ణాంకం)type, nbytes, (పూర్ణాంకం)ఫలితం);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(!ctx->started_body &&
     !(type & (CLIENTWRITE_INFO | CLIENTWRITE_CONNECT))) {
    Curl_rlimit_ప్రారంభం(&దత్తాంశం->పురోగతి.dl.rlimit, Curl_pgrs_now(దత్తాంశం),
                      దత్తాంశం->req.కొలత);
    ctx->started_body = TRUE;
  }

  /* Here, we deal with REAL BODY bytes. All filtering and transfer
   * encodings have been applied and only the true content, e.g. BODY,
   * bytes are passed here.
   * This allows us to check sizes, update stats, etc. independent
   * from the protocol in play. */

  ఒకవేళ(దత్తాంశం->req.no_body && nbytes > 0) {
    /* BODY arrives although we want none, bail out */
    streamclose(దత్తాంశం->conn);
    CURL_TRC_WRITE(దత్తాంశం, "download_write body(type=%x, blen=%zu), "
                   "did not want a BODY", (గుర్తులేని పూర్ణాంకం)type, nbytes);
    దత్తాంశం->req.download_పూర్తి = TRUE;
    ఒకవేళ(దత్తాంశం->info.శీర్షిక_కొలత)
      /* if headers have been received, this is fine */
      తిరిగి CURLE_OK;
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  /* Determine if we see any bytes in excess to what is allowed.
   * We write the allowed bytes and handle excess further below.
   * This gives deterministic BODY writes on varying buffer receive
   * lengths. */
  nwrite = nbytes;
  ఒకవేళ(దత్తాంశం->req.maxdownload != -1) {
    size_t wmax = get_max_body_write_len(దత్తాంశం, దత్తాంశం->req.maxdownload);
    ఒకవేళ(nwrite > wmax) {
      excess_len = nbytes - wmax;
      nwrite = wmax;
    }

    ఒకవేళ(nwrite == wmax) {
      దత్తాంశం->req.download_పూర్తి = TRUE;
    }

    ఒకవేళ((type & CLIENTWRITE_EOS) && !దత్తాంశం->req.no_body &&
       (దత్తాంశం->req.కొలత > దత్తాంశం->req.bytecount)) {
      failf(దత్తాంశం, "end of response with %" FMT_OFF_T " bytes missing",
            దత్తాంశం->req.కొలత - దత్తాంశం->req.bytecount);
      తిరిగి CURLE_PARTIAL_FILE;
    }
  }

  /* Error on too large filesize is handled below, after writing
   * the permitted bytes */
  ఒకవేళ(దత్తాంశం->సమితి.max_filesize && !దత్తాంశం->req.ignorebody) {
    size_t wmax = get_max_body_write_len(దత్తాంశం, దత్తాంశం->సమితి.max_filesize);
    ఒకవేళ(nwrite > wmax) {
      nwrite = wmax;
    }
  }

  ఒకవేళ(!దత్తాంశం->req.ignorebody && (nwrite || (type & CLIENTWRITE_EOS))) {
    ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nwrite);
    CURL_TRC_WRITE(దత్తాంశం, "download_write body(type=%x, blen=%zu) -> %d",
                   (గుర్తులేని పూర్ణాంకం)type, nbytes, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Update stats, write and report progress */
  ఒకవేళ(nwrite) {
    దత్తాంశం->req.bytecount += nwrite;
    Curl_pgrs_download_inc(దత్తాంశం, nwrite);
  }

  ఒకవేళ(excess_len) {
    ఒకవేళ(!దత్తాంశం->req.ignorebody) {
      infof(దత్తాంశం,
            "Excess found writing body:"
            " excess = %zu"
            ", size = %" FMT_OFF_T
            ", maxdownload = %" FMT_OFF_T
            ", bytecount = %" FMT_OFF_T,
            excess_len, దత్తాంశం->req.కొలత, దత్తాంశం->req.maxdownload,
            దత్తాంశం->req.bytecount);
      connclose(దత్తాంశం->conn);
    }
  }
  లేకపోతే ఒకవేళ((nwrite < nbytes) && !దత్తాంశం->req.ignorebody) {
    failf(దత్తాంశం, "Exceeded the maximum allowed file size "
          "(%" FMT_OFF_T ") with %" FMT_OFF_T " bytes",
          దత్తాంశం->సమితి.max_filesize, దత్తాంశం->req.bytecount);
    తిరిగి CURLE_FILESIZE_EXCEEDED;
  }

  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype cw_download = {
  "protocol",
  ఖాళీ,
  0,
  Curl_cwriter_def_init,
  cw_download_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  పరిమాణం(నిర్మాణం cw_download_ctx)
};

/* RAW client writer in phase CURL_CW_RAW that
 * enabled tracing of raw data. */
స్థిర CURLcode cw_raw_write(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                             స్థిరరాశి అక్షరం *buf, size_t nbytes)
{
  ఒకవేళ(type & CLIENTWRITE_BODY && దత్తాంశం->సమితి.verbose && !దత్తాంశం->req.ignorebody) {
    Curl_debug(దత్తాంశం, CURLINFO_దత్తాంశం_లోపల, buf, nbytes);
  }
  తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, nbytes);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype cw_raw = {
  "raw",
  ఖాళీ,
  0,
  Curl_cwriter_def_init,
  cw_raw_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  పరిమాణం(నిర్మాణం Curl_cwriter)
};

స్థిర శూన్య cwriter_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_cwriter *writer)
{
  నిర్మాణం Curl_cwriter **anchor = &దత్తాంశం->req.writer.దొంతర;

  /* Insert the writer as first in its phase.
   * Skip existing writers of lower phases. */
  అయితే(*anchor && (*anchor)->phase < writer->phase)
    anchor = &(*anchor)->తదుపరి;
  writer->తదుపరి = *anchor;
  *anchor = writer;
}

స్థిర CURLcode do_init_writer_దొంతర(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cwriter *writer;
  CURLcode ఫలితం;

  DEBUGASSERT(!దత్తాంశం->req.writer.దొంతర);
  ఫలితం = Curl_cwriter_create(&దత్తాంశం->req.writer.దొంతర,
                               దత్తాంశం, &Curl_cwt_బహిర్గతం, CURL_CW_CLIENT);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_cwriter_create(&writer, దత్తాంశం, &cw_download, CURL_CW_PROTOCOL);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  cwriter_కూడు(దత్తాంశం, writer);

  ఫలితం = Curl_cwriter_create(&writer, దత్తాంశం, &cw_raw, CURL_CW_RAW);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  cwriter_కూడు(దత్తాంశం, writer);

  తిరిగి ఫలితం;
}

/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం type, స్థిరరాశి అక్షరం *buf,
                           size_t len)
{
  CURLcode ఫలితం;

  /* it is one of those, at least */
  DEBUGASSERT(type &
              (CLIENTWRITE_BODY | CLIENTWRITE_శీర్షిక | CLIENTWRITE_INFO));
  /* BODY is only BODY (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_BODY) ||
              ((type & ~(CLIENTWRITE_BODY | CLIENTWRITE_EOS)) == 0));
  /* INFO is only INFO (with optional EOS) */
  DEBUGASSERT(!(type & CLIENTWRITE_INFO) ||
              ((type & ~(CLIENTWRITE_INFO | CLIENTWRITE_EOS)) == 0));

  ఒకవేళ(!దత్తాంశం->req.writer.దొంతర) {
    ఫలితం = do_init_writer_దొంతర(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    DEBUGASSERT(దత్తాంశం->req.writer.దొంతర);
  }

  ఫలితం = Curl_cwriter_write(దత్తాంశం, దత్తాంశం->req.writer.దొంతర, type, buf, len);
  CURL_TRC_WRITE(దత్తాంశం, "client_write(type=%x, len=%zu) -> %d",
                 (గుర్తులేని పూర్ణాంకం)type, len, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

CURLcode Curl_client_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;

  ఒకవేళ(!దత్తాంశం->req.writer.దొంతర) {
    ఫలితం = do_init_writer_దొంతర(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    DEBUGASSERT(దత్తాంశం->req.writer.దొంతర);
  }

  ఫలితం = Curl_cwriter_flush(దత్తాంశం, దత్తాంశం->req.writer.దొంతర);
  CURL_TRC_WRITE(దత్తాంశం, "client_flush() -> %d", (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

/* Create an unencoding writer stage using the given handler. */
CURLcode Curl_cwriter_create(నిర్మాణం Curl_cwriter **pwriter,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం Curl_cwtype *cwt,
                             Curl_cwriter_phase phase)
{
  నిర్మాణం Curl_cwriter *writer = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  శూన్య *p;

  DEBUGASSERT(cwt->cwriter_కొలత >= పరిమాణం(నిర్మాణం Curl_cwriter));
  p = curlx_calloc(1, cwt->cwriter_కొలత);
  ఒకవేళ(!p)
    వెళ్ళు బహిర్గతం;

  writer = (నిర్మాణం Curl_cwriter *)p;
  writer->cwt = cwt;
  writer->ctx = p;
  writer->phase = phase;
  ఫలితం = cwt->do_init(దత్తాంశం, writer);

బహిర్గతం:
  *pwriter = ఫలితం ? ఖాళీ : writer;
  ఒకవేళ(ఫలితం)
    curlx_free(writer);
  తిరిగి ఫలితం;
}

శూన్య Curl_cwriter_free(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం Curl_cwriter *writer)
{
  ఒకవేళ(writer) {
    writer->cwt->do_close(దత్తాంశం, writer);
    curlx_free(writer);
  }
}

size_t Curl_cwriter_లెక్క(నిర్మాణం Curl_easy *దత్తాంశం, Curl_cwriter_phase phase)
{
  నిర్మాణం Curl_cwriter *w;
  size_t n = 0;

  కొరకు(w = దత్తాంశం->req.writer.దొంతర; w; w = w->తదుపరి) {
    ఒకవేళ(w->phase == phase)
      ++n;
  }
  తిరిగి n;
}

స్థిర CURLcode cwriter_ensure_pause_writer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cwriter *writer =
    Curl_cwriter_get_by_type(దత్తాంశం, &Curl_cwt_pause);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!writer) {
    ఫలితం = Curl_cwriter_create(&writer, దత్తాంశం, &Curl_cwt_pause,
                                 CURL_CW_BEFORE_DECODE);
    ఒకవేళ(!ఫలితం)
      cwriter_కూడు(దత్తాంశం, writer);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cwriter_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_cwriter *writer)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_cwriter **anchor = &దత్తాంశం->req.writer.దొంతర;

  ఒకవేళ(!*anchor) {
    ఫలితం = do_init_writer_దొంతర(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(writer->cwt->flags & CURL_CW_FLAG_BLOWUP) {
    /* On adding a writer that may blow up write sizes, e.g. zip bombs,
     * add the pause writer. Do this first as any failure will make the
     * caller destroy the writer again. */
    ఫలితం = cwriter_ensure_pause_writer(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  cwriter_కూడు(దత్తాంశం, writer);
  తిరిగి CURLE_OK;
}

నిర్మాణం Curl_cwriter *Curl_cwriter_get_by_name(నిర్మాణం Curl_easy *దత్తాంశం,
                                              స్థిరరాశి అక్షరం *name)
{
  నిర్మాణం Curl_cwriter *writer;
  కొరకు(writer = దత్తాంశం->req.writer.దొంతర; writer; writer = writer->తదుపరి) {
    ఒకవేళ(!strcmp(name, writer->cwt->name))
      తిరిగి writer;
  }
  తిరిగి ఖాళీ;
}

నిర్మాణం Curl_cwriter *Curl_cwriter_get_by_type(నిర్మాణం Curl_easy *దత్తాంశం,
                                              స్థిరరాశి నిర్మాణం Curl_cwtype *cwt)
{
  నిర్మాణం Curl_cwriter *writer;
  కొరకు(writer = దత్తాంశం->req.writer.దొంతర; writer; writer = writer->తదుపరి) {
    ఒకవేళ(writer->cwt == cwt)
      తిరిగి writer;
  }
  తిరిగి ఖాళీ;
}

CURLcode Curl_cwriter_unpause(నిర్మాణం Curl_easy *దత్తాంశం)
{
  దత్తాంశం->req.writer.paused = FALSE;
  తిరిగి Curl_cwriter_flush(దత్తాంశం, దత్తాంశం->req.writer.దొంతర);
}

CURLcode Curl_creader_read(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader,
                           అక్షరం *buf, size_t blen, size_t *nread, bool *eos)
{
  *nread = 0;
  *eos = FALSE;
  ఒకవేళ(!reader)
    తిరిగి CURLE_READ_ERROR;
  తిరిగి reader->crt->do_read(దత్తాంశం, reader, buf, blen, nread, eos);
}

శూన్య Curl_creader_clear_eos(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader)
{
  అయితే(reader) {
    (శూన్య)reader->crt->cntrl(దత్తాంశం, reader, CURL_CRCNTRL_CLEAR_EOS);
    reader = reader->తదుపరి;
  }
}

CURLcode Curl_creader_def_init(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి CURLE_OK;
}

శూన్య Curl_creader_def_close(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
}

CURLcode Curl_creader_def_read(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creader *reader,
                               అక్షరం *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ఒకవేళ(reader->తదుపరి)
    తిరిగి reader->తదుపరి->crt->do_read(దత్తాంశం, reader->తదుపరి, buf, blen,
                                      nread, eos);
  లేకపోతే {
    *nread = 0;
    *eos = FALSE;
    తిరిగి CURLE_READ_ERROR;
  }
}

bool Curl_creader_def_needs_rewind(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి FALSE;
}

curl_off_t Curl_creader_def_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_creader *reader)
{
  తిరిగి reader->తదుపరి ?
         reader->తదుపరి->crt->మొత్తం_నిడివి(దత్తాంశం, reader->తదుపరి) : -1;
}

CURLcode Curl_creader_def_resume_from(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_creader *reader,
                                      curl_off_t స్థానభ్రంశం)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  (శూన్య)స్థానభ్రంశం;
  తిరిగి CURLE_READ_ERROR;
}

CURLcode Curl_creader_def_cntrl(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_creader *reader,
                                Curl_creader_cntrl opcode)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  (శూన్య)opcode;
  తిరిగి CURLE_OK;
}

bool Curl_creader_def_is_paused(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_creader *reader)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి FALSE;
}

శూన్య Curl_creader_def_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader, పూర్ణాంకం premature)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  (శూన్య)premature;
}

నిర్మాణం cr_లోపల_ctx {
  నిర్మాణం Curl_creader super;
  curl_read_callback read_cb;
  శూన్య *cb_user_దత్తాంశం;
  curl_off_t మొత్తం_len;
  curl_off_t read_len;
  CURLcode error_ఫలితం;
  బిట్టు(చూసిన_eos);
  బిట్టు(errored);
  బిట్టు(has_వాడిన_cb);
  బిట్టు(is_paused);
};

స్థిర CURLcode cr_లోపల_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  ctx->read_cb = దత్తాంశం->state.fread_func;
  ctx->cb_user_దత్తాంశం = దత్తాంశం->state.లోపల;
  ctx->మొత్తం_len = -1;
  ctx->read_len = 0;
  తిరిగి CURLE_OK;
}

/* Real client reader to installed client callbacks. */
స్థిర CURLcode cr_లోపల_read(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader,
                           అక్షరం *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;

  ctx->is_paused = FALSE;

  /* Once we have errored, we will return the same error forever */
  ఒకవేళ(ctx->errored) {
    *pnread = 0;
    *peos = FALSE;
    తిరిగి ctx->error_ఫలితం;
  }
  ఒకవేళ(ctx->చూసిన_eos) {
    *pnread = 0;
    *peos = TRUE;
    తిరిగి CURLE_OK;
  }
  /* respect length limitations */
  ఒకవేళ(ctx->మొత్తం_len >= 0) {
    blen = curlx_sotouz_range(ctx->మొత్తం_len - ctx->read_len, 0, blen);
  }
  nread = 0;
  ఒకవేళ(ctx->read_cb && blen) {
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_cr_లోపల_read);
    nread = ctx->read_cb(buf, 1, blen, ctx->cb_user_దత్తాంశం);
    CURL_CBAPI_చివర(&guard);
    ctx->has_వాడిన_cb = TRUE;
  }

  ఎంపిక(nread) {
  స్థితి 0:
    ఒకవేళ((ctx->మొత్తం_len >= 0) && (ctx->read_len < ctx->మొత్తం_len)) {
      failf(దత్తాంశం, "client read function EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T " of needed bytes read",
            ctx->read_len, ctx->మొత్తం_len);
      ఫలితం = CURLE_READ_ERROR;
      ఆపు;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->చూసిన_eos = TRUE;
    ఆపు;

  స్థితి CURL_READFUNC_ABORT:
    failf(దత్తాంశం, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    ఆపు;

  స్థితి CURL_READFUNC_PAUSE:
    ఒకవేళ(దత్తాంశం->conn->scheme->flags & PROTOPT_NONETWORK) {
      /* protocols that work without network cannot be paused. This is
         actually only file:// now, and it cannot pause since the transfer
         is not done using the "normal" procedure. */
      failf(దత్తాంశం, "Read callback asked for PAUSE when not supported");
      ఫలితం = CURLE_READ_ERROR;
      ఆపు;
    }
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(దత్తాంశం, "cr_in_read, callback returned CURL_READFUNC_PAUSE");
    ctx->is_paused = TRUE;
    *pnread = 0;
    *peos = FALSE;
    ఫలితం = Curl_xfer_pause_send(దత్తాంశం, TRUE);
    ఆపు; /* nothing was read */

  అప్రమేయ:
    ఒకవేళ(nread > blen) {
      /* the read function returned a too large value */
      failf(దత్తాంశం, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ఫలితం = CURLE_READ_ERROR;
      ఫలితం = CURLE_READ_ERROR;
      ఆపు;
    }
    ctx->read_len += nread;
    ఒకవేళ(ctx->మొత్తం_len >= 0)
      ctx->చూసిన_eos = (ctx->read_len >= ctx->మొత్తం_len);
    *pnread = nread;
    *peos = (bool)ctx->చూసిన_eos;
    ఆపు;
  }
  CURL_TRC_READ(దత్తాంశం, "cr_in_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, nread=%zu, eos=%d",
                blen, ctx->మొత్తం_len, ctx->read_len, (పూర్ణాంకం)ఫలితం,
                *pnread, *peos);
  తిరిగి ఫలితం;
}

స్థిర bool cr_లోపల_needs_rewind(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి (bool)ctx->has_వాడిన_cb;
}

స్థిర curl_off_t cr_లోపల_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి ctx->మొత్తం_len;
}

స్థిర CURLcode cr_లోపల_resume_from(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_creader *reader,
                                  curl_off_t స్థానభ్రంశం)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  పూర్ణాంకం seekerr = CURL_SEEKFUNC_CANTSEEK;

  DEBUGASSERT(దత్తాంశం->conn);
  /* already started reading? */
  ఒకవేళ(ctx->read_len)
    తిరిగి CURLE_READ_ERROR;

  ఒకవేళ(దత్తాంశం->సమితి.seek_func) {
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_seek_func);
    seekerr = దత్తాంశం->సమితి.seek_func(దత్తాంశం->సమితి.seek_client, స్థానభ్రంశం, SEEK_SET);
    CURL_CBAPI_చివర(&guard);
  }

  ఒకవేళ(seekerr != CURL_SEEKFUNC_OK) {
    curl_off_t passed = 0;

    ఒకవేళ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
      failf(దత్తాంశం, "Could not seek stream");
      తిరిగి CURLE_READ_ERROR;
    }
    /* when seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
    చేయి {
      నిర్మాణం Curl_mapi_guard guard;
      అక్షరం scratch[4 * 1024];
      size_t readthisamountnow =
        (స్థానభ్రంశం - passed > (curl_off_t)పరిమాణం(scratch)) ?
        పరిమాణం(scratch) :
        curlx_sotouz(స్థానభ్రంశం - passed);
      size_t actuallyread;

      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_cr_లోపల_resume_from);
      actuallyread = ctx->read_cb(scratch, 1, readthisamountnow,
                                  ctx->cb_user_దత్తాంశం);
      CURL_CBAPI_చివర(&guard);

      passed += actuallyread;
      ఒకవేళ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(దత్తాంశం, "Could only read %" FMT_OFF_T " bytes from the input",
              passed);
        తిరిగి CURLE_READ_ERROR;
      }
    } అయితే(passed < స్థానభ్రంశం);
  }

  /* now, decrease the size of the read */
  ఒకవేళ(ctx->మొత్తం_len > 0) {
    ctx->మొత్తం_len -= స్థానభ్రంశం;

    ఒకవేళ(ctx->మొత్తం_len <= 0) {
      failf(దత్తాంశం, "File already completely uploaded");
      తిరిగి CURLE_PARTIAL_FILE;
    }
  }
  /* we have passed, proceed as normal */
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cr_లోపల_rewind(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;

  /* If we never invoked the callback, there is noting to rewind */
  ఒకవేళ(!ctx->has_వాడిన_cb)
    తిరిగి CURLE_OK;

  ఒకవేళ(దత్తాంశం->సమితి.seek_func) {
    నిర్మాణం Curl_mapi_guard guard;
    పూర్ణాంకం err;

    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_seek_func);
    err = దత్తాంశం->సమితి.seek_func(దత్తాంశం->సమితి.seek_client, 0, SEEK_SET);
    CURL_CBAPI_చివర(&guard);
    CURL_TRC_READ(దత్తాంశం, "cr_in, rewind via set.seek_func -> %d", err);
    ఒకవేళ(err) {
      failf(దత్తాంశం, "seek callback returned error %d", err);
      తిరిగి CURLE_SEND_FAIL_REWIND;
    }
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.ioctl_func) {
    నిర్మాణం Curl_mapi_guard guard;
    curlioerr err;

    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_ioctl_func);
    err = దత్తాంశం->సమితి.ioctl_func(దత్తాంశం, CURLIOCMD_RESTARTREAD,
                               దత్తాంశం->సమితి.ioctl_client);
    CURL_CBAPI_చివర(&guard);
    CURL_TRC_READ(దత్తాంశం, "cr_in, rewind via set.ioctl_func -> %d", (పూర్ణాంకం)err);
    ఒకవేళ(err) {
      failf(దత్తాంశం, "ioctl callback returned error %d", (పూర్ణాంకం)err);
      తిరిగి CURLE_SEND_FAIL_REWIND;
    }
  }
  లేకపోతే {
    /* If no CURLOPT_READFUNCTION is used, we know that we operate on a
       given FILE * stream and we can actually attempt to rewind that
       ourselves with fseek() */
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
    ఒకవేళ(దత్తాంశం->state.fread_func == (curl_read_callback)fread) {
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
      పూర్ణాంకం err = fseek(దత్తాంశం->state.లోపల, 0, SEEK_SET);
      CURL_TRC_READ(దత్తాంశం, "cr_in, rewind via fseek -> %d(%d)",
                    (పూర్ణాంకం)err, (పూర్ణాంకం)errno);
      ఒకవేళ(err != -1)
        /* successful rewind */
        తిరిగి CURLE_OK;
    }

    /* no callback set or failure above, makes us fail at once */
    failf(దత్తాంశం, "necessary data rewind was not possible");
    తిరిగి CURLE_SEND_FAIL_REWIND;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cr_లోపల_cntrl(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader,
                            Curl_creader_cntrl opcode)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;

  ఎంపిక(opcode) {
  స్థితి CURL_CRCNTRL_REWIND:
    తిరిగి cr_లోపల_rewind(దత్తాంశం, reader);
  స్థితి CURL_CRCNTRL_UNPAUSE:
    ctx->is_paused = FALSE;
    ఆపు;
  స్థితి CURL_CRCNTRL_CLEAR_EOS:
    ctx->చూసిన_eos = FALSE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర bool cr_లోపల_is_paused(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_లోపల_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి (bool)ctx->is_paused;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_లోపల = {
  "cr-in",
  cr_లోపల_init,
  cr_లోపల_read,
  Curl_creader_def_close,
  cr_లోపల_needs_rewind,
  cr_లోపల_మొత్తం_నిడివి,
  cr_లోపల_resume_from,
  cr_లోపల_cntrl,
  cr_లోపల_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_లోపల_ctx)
};

CURLcode Curl_creader_create(నిర్మాణం Curl_creader **preader,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం Curl_crtype *crt,
                             Curl_creader_phase phase)
{
  నిర్మాణం Curl_creader *reader = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  శూన్య *p;

  DEBUGASSERT(crt->creader_కొలత >= పరిమాణం(నిర్మాణం Curl_creader));
  p = curlx_calloc(1, crt->creader_కొలత);
  ఒకవేళ(!p)
    వెళ్ళు బహిర్గతం;

  reader = (నిర్మాణం Curl_creader *)p;
  reader->crt = crt;
  reader->ctx = p;
  reader->phase = phase;
  ఫలితం = crt->do_init(దత్తాంశం, reader);

బహిర్గతం:
  *preader = ఫలితం ? ఖాళీ : reader;
  ఒకవేళ(ఫలితం)
    curlx_free(reader);
  తిరిగి ఫలితం;
}

శూన్య Curl_creader_free(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  ఒకవేళ(reader) {
    reader->crt->do_close(దత్తాంశం, reader);
    curlx_free(reader);
  }
}

నిర్మాణం cr_lc_ctx {
  నిర్మాణం Curl_creader super;
  నిర్మాణం bufq buf;
  బిట్టు(read_eos);  /* we read an EOS from the next reader */
  బిట్టు(eos);       /* we have returned an EOS */
  బిట్టు(prev_cr);   /* the last byte was a CR */
};

స్థిర CURLcode cr_lc_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_lc_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_హద్దు);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cr_lc_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_lc_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_free(&ctx->buf);
}

/* client reader doing line end conversions. */
స్థిర CURLcode cr_lc_read(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader,
                           అక్షరం *buf, size_t blen,
                           size_t *pnread, bool *peos)
{
  నిర్మాణం cr_lc_ctx *ctx = reader->ctx;
  CURLcode ఫలితం;
  size_t nread, i, ప్రారంభం, n;
  bool eos;

  ఒకవేళ(ctx->eos) {
    *pnread = 0;
    *peos = TRUE;
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->buf)) {
    ఒకవేళ(ctx->read_eos) {
      ctx->eos = TRUE;
      *pnread = 0;
      *peos = TRUE;
      తిరిగి CURLE_OK;
    }
    /* Still getting data form the next reader, ctx->buf is empty */
    ఫలితం = Curl_creader_read(దత్తాంశం, reader->తదుపరి, buf, blen, &nread, &eos);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ctx->read_eos = eos;

    ఒకవేళ(!nread || !memchr(buf, '\n', nread)) {
      /* nothing to convert, return this right away */
      ఒకవేళ(nread)
        ctx->prev_cr = (buf[nread - 1] == '\r');
      ఒకవేళ(ctx->read_eos)
        ctx->eos = TRUE;
      *pnread = nread;
      *peos = (bool)ctx->eos;
      వెళ్ళు బహిర్గతం;
    }

    /* at least one \n might need conversion to '\r\n', place into ctx->buf */
    కొరకు(i = ప్రారంభం = 0; i < nread; ++i) {
      /* if this byte is not an LF character, or if the preceding character is
         a CR (meaning this already is a CRLF pair), go to next */
      ఒకవేళ((buf[i] != '\n') || ctx->prev_cr) {
        ctx->prev_cr = (buf[i] == '\r');
        కొనసాగు;
      }
      ctx->prev_cr = FALSE;
      /* on a soft limit bufq, we do not need to check length */
      ఫలితం = Curl_bufq_cwrite(&ctx->buf, buf + ప్రారంభం, i - ప్రారంభం, &n);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_bufq_cwrite(&ctx->buf, STRCONST("\r\n"), &n);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ప్రారంభం = i + 1;
    }

    ఒకవేళ(ప్రారంభం < i) { /* leftover */
      ఫలితం = Curl_bufq_cwrite(&ctx->buf, buf + ప్రారంభం, i - ప్రారంభం, &n);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  DEBUGASSERT(!Curl_bufq_is_ఖాళీలు(&ctx->buf));
  *peos = FALSE;
  ఫలితం = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  ఒకవేళ(!ఫలితం && ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ctx->buf)) {
    /* no more data, read all, done. */
    ctx->eos = TRUE;
    *peos = TRUE;
  }

బహిర్గతం:
  CURL_TRC_READ(దత్తాంశం, "cr_lc_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (పూర్ణాంకం)ఫలితం, *pnread, *peos);
  తిరిగి ఫలితం;
}

స్థిర curl_off_t cr_lc_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి -1;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_lc = {
  "cr-lineconv",
  cr_lc_init,
  cr_lc_read,
  cr_lc_close,
  Curl_creader_def_needs_rewind,
  cr_lc_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_lc_ctx)
};

స్థిర CURLcode cr_lc_కూడు(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = ఖాళీ;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&reader, దత్తాంశం, &cr_lc, CURL_CR_CONTENT_ENCODE);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_కూడు(దత్తాంశం, reader);

  ఒకవేళ(ఫలితం && reader)
    Curl_creader_free(దత్తాంశం, reader);
  తిరిగి ఫలితం;
}

స్థిర CURLcode do_init_reader_దొంతర(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_creader *r)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_off_t clen;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);
  DEBUGASSERT(!దత్తాంశం->req.reader.దొంతర);

  దత్తాంశం->req.reader.దొంతర = r;
  clen = r->crt->మొత్తం_నిడివి(దత్తాంశం, r);
  /* if we do not have 0 length init, and CRLF conversion is wanted,
   * add the reader for it */
  ఒకవేళ(clen &&
#నిర్వచితమైతే CURL_PREFER_LF_LINEENDS
    (దత్తాంశం->సమితి.crlf || దత్తాంశం->state.prefer_ascii)
#లేకపోతే
    దత్తాంశం->సమితి.crlf
#అయితే_ముగింపు
    ) {
    ఫలితం = cr_lc_కూడు(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_creader_సమితి_fread(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t len)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_creader *r;
  నిర్మాణం cr_లోపల_ctx *ctx;

  ఫలితం = Curl_creader_create(&r, దత్తాంశం, &cr_లోపల, CURL_CR_CLIENT);
  ఒకవేళ(ఫలితం || !r)
    వెళ్ళు బహిర్గతం;
  ctx = r->ctx;
  ctx->మొత్తం_len = len;

  cl_reset_reader(దత్తాంశం);
  ఫలితం = do_init_reader_దొంతర(దత్తాంశం, r);
బహిర్గతం:
  CURL_TRC_READ(దత్తాంశం, "add fread reader, len=%" FMT_OFF_T " -> %d",
                len, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

CURLcode Curl_creader_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_creader *reader)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_creader **anchor = &దత్తాంశం->req.reader.దొంతర;

  ఒకవేళ(!*anchor) {
    ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, దత్తాంశం->state.infilesize);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Insert the writer as first in its phase.
   * Skip existing readers of lower phases. */
  అయితే(*anchor && (*anchor)->phase < reader->phase)
    anchor = &(*anchor)->తదుపరి;
  reader->తదుపరి = *anchor;
  *anchor = reader;
  తిరిగి CURLE_OK;
}

CURLcode Curl_creader_సమితి(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *r)
{
  CURLcode ఫలితం;

  DEBUGASSERT(r);
  DEBUGASSERT(r->crt);
  DEBUGASSERT(r->phase == CURL_CR_CLIENT);

  cl_reset_reader(దత్తాంశం);
  ఫలితం = do_init_reader_దొంతర(దత్తాంశం, r);
  ఒకవేళ(ఫలితం)
    Curl_creader_free(దత్తాంశం, r);
  తిరిగి ఫలితం;
}

CURLcode Curl_client_read(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf, size_t blen,
                          size_t *nread, bool *eos)
{
  CURLcode ఫలితం;

  DEBUGASSERT(buf);
  DEBUGASSERT(blen);
  DEBUGASSERT(nread);
  DEBUGASSERT(eos);
  *nread = 0;

  ఒకవేళ(!దత్తాంశం->req.reader.దొంతర) {
    ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, దత్తాంశం->state.infilesize);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    DEBUGASSERT(దత్తాంశం->req.reader.దొంతర);
  }
  ఒకవేళ(!దత్తాంశం->req.reader_started) {
    Curl_rlimit_ప్రారంభం(&దత్తాంశం->పురోగతి.ul.rlimit, Curl_pgrs_now(దత్తాంశం), -1);
    దత్తాంశం->req.reader_started = TRUE;
  }

  ఒకవేళ(Curl_rlimit_చురుకైన(&దత్తాంశం->పురోగతి.ul.rlimit)) {
    curl_off_t ul_avail = Curl_rlimit_avail(&దత్తాంశం->పురోగతి.ul.rlimit, ఖాళీ);
    ఒకవేళ(ul_avail <= 0) {
      ఫలితం = CURLE_OK;
      *eos = FALSE;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(ul_avail < (curl_off_t)blen)
      blen = (size_t)ul_avail;
  }
  ఫలితం = Curl_creader_read(దత్తాంశం, దత్తాంశం->req.reader.దొంతర, buf, blen,
                             nread, eos);

బహిర్గతం:
  CURL_TRC_READ(దత్తాంశం, "client_read(len=%zu) -> %d, nread=%zu, eos=%d",
                blen, (పూర్ణాంకం)ఫలితం, *nread, *eos);
  తిరిగి ఫలితం;
}

bool Curl_creader_needs_rewind(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = దత్తాంశం->req.reader.దొంతర;
  అయితే(reader) {
    ఒకవేళ(reader->crt->needs_rewind(దత్తాంశం, reader)) {
      CURL_TRC_READ(దత్తాంశం, "client reader needs rewind before next request");
      తిరిగి TRUE;
    }
    reader = reader->తదుపరి;
  }
  తిరిగి FALSE;
}

స్థిర CURLcode cr_null_read(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader,
                             అక్షరం *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  (శూన్య)buf;
  (శూన్య)blen;
  *pnread = 0;
  *peos = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర curl_off_t cr_null_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_null = {
  "cr-null",
  Curl_creader_def_init,
  cr_null_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  cr_null_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం Curl_creader)
};

CURLcode Curl_creader_సమితి_null(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&r, దత్తాంశం, &cr_null, CURL_CR_CLIENT);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  cl_reset_reader(దత్తాంశం);
  తిరిగి do_init_reader_దొంతర(దత్తాంశం, r);
}

నిర్మాణం cr_buf_ctx {
  నిర్మాణం Curl_creader super;
  స్థిరరాశి అక్షరం *buf;
  size_t blen;
  size_t సూచీ;
};

స్థిర CURLcode cr_buf_read(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader,
                            అక్షరం *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  నిర్మాణం cr_buf_ctx *ctx = reader->ctx;
  size_t nread = ctx->blen - ctx->సూచీ;

  ఒకవేళ(!nread || !ctx->buf) {
    *pnread = 0;
    *peos = TRUE;
  }
  లేకపోతే {
    ఒకవేళ(nread > blen)
      nread = blen;
    జ్ఞాపకం_నకలు(buf, ctx->buf + ctx->సూచీ, nread);
    *pnread = nread;
    ctx->సూచీ += nread;
    *peos = (ctx->సూచీ == ctx->blen);
  }
  CURL_TRC_READ(దత్తాంశం, "cr_buf_read(len=%zu) -> 0, nread=%zu, eos=%d",
                blen, *pnread, *peos);
  తిరిగి CURLE_OK;
}

స్థిర bool cr_buf_needs_rewind(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_buf_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి ctx->సూచీ > 0;
}

స్థిర CURLcode cr_buf_cntrl(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader,
                             Curl_creader_cntrl opcode)
{
  నిర్మాణం cr_buf_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  ఎంపిక(opcode) {
  స్థితి CURL_CRCNTRL_REWIND:
    ctx->సూచీ = 0;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర curl_off_t cr_buf_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_buf_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి (curl_off_t)ctx->blen;
}

స్థిర CURLcode cr_buf_resume_from(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_creader *reader,
                                   curl_off_t స్థానభ్రంశం)
{
  నిర్మాణం cr_buf_ctx *ctx = reader->ctx;
  size_t boffset;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(దత్తాంశం->conn);
  /* already started reading? */
  ఒకవేళ(ctx->సూచీ)
    తిరిగి CURLE_READ_ERROR;
  boffset = curlx_sotouz_range(స్థానభ్రంశం, 0, SIZE_MAX);
  ఒకవేళ(!boffset)
    తిరిగి CURLE_OK;
  ఒకవేళ(boffset > ctx->blen)
    తిరిగి CURLE_READ_ERROR;

  ctx->buf += boffset;
  ctx->blen -= boffset;
  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_buf = {
  "cr-buf",
  Curl_creader_def_init,
  cr_buf_read,
  Curl_creader_def_close,
  cr_buf_needs_rewind,
  cr_buf_మొత్తం_నిడివి,
  cr_buf_resume_from,
  cr_buf_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_buf_ctx)
};

CURLcode Curl_creader_సమితి_buf(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *buf, size_t blen)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_creader *r;
  నిర్మాణం cr_buf_ctx *ctx;

  ఫలితం = Curl_creader_create(&r, దత్తాంశం, &cr_buf, CURL_CR_CLIENT);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = r->ctx;
  ctx->buf = buf;
  ctx->blen = blen;
  ctx->సూచీ = 0;

  cl_reset_reader(దత్తాంశం);
  ఫలితం = do_init_reader_దొంతర(దత్తాంశం, r);
బహిర్గతం:
  CURL_TRC_READ(దత్తాంశం, "add buf reader, len=%zu -> %d", blen, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

curl_off_t Curl_creader_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = దత్తాంశం->req.reader.దొంతర;
  తిరిగి r ? r->crt->మొత్తం_నిడివి(దత్తాంశం, r) : -1;
}

curl_off_t Curl_creader_client_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = దత్తాంశం->req.reader.దొంతర;
  అయితే(r && r->phase != CURL_CR_CLIENT)
    r = r->తదుపరి;
  తిరిగి r ? r->crt->మొత్తం_నిడివి(దత్తాంశం, r) : -1;
}

CURLcode Curl_creader_unpause(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = దత్తాంశం->req.reader.దొంతర;
  CURLcode ఫలితం = CURLE_OK;

  అయితే(reader) {
    ఫలితం = reader->crt->cntrl(దత్తాంశం, reader, CURL_CRCNTRL_UNPAUSE);
    CURL_TRC_READ(దత్తాంశం, "unpausing %s -> %d", reader->crt->name, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      ఆపు;
    reader = reader->తదుపరి;
  }
  తిరిగి ఫలితం;
}

bool Curl_creader_is_paused(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = దత్తాంశం->req.reader.దొంతర;

  అయితే(reader) {
    ఒకవేళ(reader->crt->is_paused(దత్తాంశం, reader))
      తిరిగి TRUE;
    reader = reader->తదుపరి;
  }
  తిరిగి FALSE;
}

శూన్య Curl_creader_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం premature)
{
  నిర్మాణం Curl_creader *reader = దత్తాంశం->req.reader.దొంతర;
  అయితే(reader) {
    reader->crt->పూర్తి(దత్తాంశం, reader, premature);
    reader = reader->తదుపరి;
  }
}

నిర్మాణం Curl_creader *Curl_creader_get_by_type(నిర్మాణం Curl_easy *దత్తాంశం,
                                              స్థిరరాశి నిర్మాణం Curl_crtype *crt)
{
  నిర్మాణం Curl_creader *r;
  కొరకు(r = దత్తాంశం->req.reader.దొంతర; r; r = r->తదుపరి) {
    ఒకవేళ(r->crt == crt)
      తిరిగి r;
  }
  తిరిగి ఖాళీ;
}
