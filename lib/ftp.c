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
#ضمن "curl_setup.h"
#ضمن "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "sendf.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "if2ip.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "escape.h"
#ضمن "ftp.h"
#ضمن "ftp-int.h"
#ضمن "ftplistparser.h"
#ضمن "curl_range.h"
#ضمن "strcase.h"
#ضمن "vtls/vtls.h"
#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "connect.h"
#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/inet_pton.h"
#ضمن "select.h"
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "sockaddr.h" /* required for Curl_sockaddr_storage */
#ضمن "multiif.h"
#ضمن "url.h"
#ضمن "http_proxy.h"
#ضمن "vdns/hostip.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"
#ضمن "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(سطر) \
  (ISDIGIT((سطر)[0]) && ISDIGIT((سطر)[1]) && ISDIGIT((سطر)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(سطر) (STATUSCODE(سطر) && (' ' == (سطر)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
ساكن ثابت حرف * ثابت ftp_state_names[] = {
  "STOP",
  "WAIT220",
  "AUTH",
  "USER",
  "PASS",
  "ACCT",
  "PBSZ",
  "PROT",
  "CCC",
  "PWD",
  "SYST",
  "NAMEFMT",
  "QUOTE",
  "RETR_PREQUOTE",
  "STOR_PREQUOTE",
  "LIST_PREQUOTE",
  "POSTQUOTE",
  "CWD",
  "MKD",
  "MDTM",
  "TYPE",
  "LIST_TYPE",
  "RETR_LIST_TYPE",
  "RETR_TYPE",
  "STOR_TYPE",
  "SIZE",
  "RETR_SIZE",
  "STOR_SIZE",
  "REST",
  "RETR_REST",
  "PORT",
  "PRET",
  "PASV",
  "LIST",
  "RETR",
  "STOR",
  "QUIT"
};
#define FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_names[(ftpc)->state] : "???")

#endif /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ساكن فراغ ftp_state_أدنى(بنية Curl_easy *data,
                          بنية ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , صحيح lineno
#endif
  )
{
  إذا(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((فراغ)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#وإلا
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_أدنى(x, y, z)
#وإلا /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_أدنى(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp);
ساكن CURLcode ftp_state_quote(بنية Curl_easy *data,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init, ftpstate instate);
ساكن CURLcode ftp_nb_type(بنية Curl_easy *data,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate);
ساكن CURLcode getftpresponse(بنية Curl_easy *data, size_t *nreadp,
                               صحيح *ftpcodep);

ساكن فراغ freedirs(بنية ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = عدم;
}

ساكن size_t numof_slashes(ثابت حرف *str)
{
  ثابت حرف *slashPos;
  size_t رقم = 0;
  افعل {
    slashPos = strchr(str, '/');
    إذا(slashPos) {
      ++رقم;
      str = slashPos + 1;
    }
  } طالما(slashPos);
  أرجع رقم;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode ftp_parse_url_path(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp)
{
  ثابت حرف *slashPos = عدم;
  ثابت حرف *fileName = عدم;
  CURLcode result = CURLE_OK;
  ثابت حرف *rawPath = عدم; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  إذا(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  إذا(result) {
    failf(data, "path contains control characters");
    أرجع result;
  }
  rawPath = ftpc->rawpath;

  بدل(data->set.ftp_filemethod) {
  حالة FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    إذا((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    اكسر;

  حالة FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    إذا(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      إذا(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (صحيح)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    وإلا
      fileName = rawPath; /* filename only (or empty) */
    اكسر;

  افتراضي: /* allow pretty much anything */
  حالة FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ثابت حرف *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    إذا(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      أرجع CURLE_URL_MALFORMAT;

    إذا(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      طالما(dirAlloc--) {
        ثابت حرف *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        إذا(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        إذا(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (صحيح)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (صحيح)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    اكسر;
  } /* switch */

  إذا(fileName && *fileName)
    ftpc->file = fileName;
  وإلا
    ftpc->file = عدم; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  إذا(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    أرجع CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  إذا((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  وإلا { /* newly created FTP connections are already in entry path */
    ثابت حرف *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    إذا(oldPath) {
      size_t n = pathLen;
      إذا(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      وإلا
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      إذا((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ساكن صحيح ftp_need_type(بنية ftp_conn *ftpc,
                         bool ascii_wanted)
{
  أرجع ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ساكن فراغ close_secondarysocket(بنية Curl_easy *data,
                                  بنية ftp_conn *ftpc)
{
  (فراغ)ftpc;
  CURL_TRC_FTP(data, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(data, data->conn, SECONDARYSOCKET);
}

#ifdef CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
بنية ftp_cw_lc_ctx {
  بنية Curl_cwriter super;
  bool newline_pending;
};

ساكن CURLcode ftp_cw_lc_write(بنية Curl_easy *data,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t blen)
{
  ساكن ثابت حرف nl = '\n';
  بنية ftp_cw_lc_ctx *ctx = writer->ctx;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  إذا(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    أرجع Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  طالما(blen) {
    /* do not pass EOS when writing parts */
    صحيح chunk_type = (type & ~CLIENTWRITE_EOS);
    ثابت حرف *cp;
    size_t chunk_len;
    CURLcode result;

    إذا(ctx->newline_pending) {
      إذا(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        إذا(result)
          أرجع result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    إذا(!cp)
      اكسر;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    إذا(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      إذا(result)
        أرجع result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  إذا(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    أرجع Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  وإلا إذا(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    إذا(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      أرجع Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    أرجع Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  أرجع CURLE_OK;
}

ساكن ثابت بنية Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  عدم,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ساكن CURLcode ftp_check_ctrl_on_data_wait(بنية Curl_easy *data,
                                            بنية ftp_conn *ftpc)
{
  بنية connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  بنية pingpong *pp = &ftpc->pp;
  size_t nread;
  صحيح ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  إذا(curlx_dyn_len(&pp->recvbuf)) {
    ثابت حرف *l = curlx_dyn_ptr(&pp->recvbuf);
    إذا(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (فراغ)getftpresponse(data, &nread, &ftpcode);
      أرجع CURLE_FTP_ACCEPT_FAILED;
    }
  }

  إذا(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  وإلا {
    صحيح socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    بدل(socketstate) {
    حالة -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      أرجع CURLE_FTP_ACCEPT_FAILED;
    افتراضي:
      إذا(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      اكسر;
    }
  }

  إذا(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    إذا(pp->overflow > 3) {
      ثابت حرف *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      إذا((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        إذا(!curlx_str_رقم(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          أرجع CURLE_OK;
        }
      }
    }

    (فراغ)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    إذا(ftpcode / 100 > 3)
      أرجع CURLE_FTP_ACCEPT_FAILED;

    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ساكن CURLcode ftp_initiate_transfer(بنية Curl_easy *data,
                                      بنية ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  إذا(result || !connected)
    أرجع result;

  إذا(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  وإلا {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  أرجع CURLE_OK;
}

ساكن bool ftp_endofresp(بنية Curl_easy *data, بنية connectdata *conn,
                          ثابت حرف *سطر, size_t len, صحيح *code)
{
  curl_off_t status;
  (فراغ)data;
  (فراغ)conn;

  إذا((len > 3) && LASTLINE(سطر) && !curlx_str_رقم(&سطر, &status, 999)) {
    *code = (صحيح)status;
    أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن CURLcode ftp_readresp(بنية Curl_easy *data,
                             بنية ftp_conn *ftpc,
                             int8_t sockindex,
                             بنية pingpong *pp,
                             صحيح *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  صحيح code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  إذا(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  إذا(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  أرجع result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ساكن CURLcode getftpresponse(بنية Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               صحيح *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  بنية connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp = &ftpc->pp;
  size_t nread;
  صحيح cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  طالما(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    إذا(timeout <= 0) {
      failf(data, "FTP response timeout");
      أرجع CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    إذا(timeout < interval_ms)
      interval_ms = timeout;

    /*
     * Since this function is blocking, we need to wait here for input on the
     * connection and only then we call the response reading function. We do
     * timeout at least every second to make the timeout check run.
     *
     * A caution here is that the ftp_readresp() function has a cache that may
     * contain pieces of a response from the previous invoke and we need to
     * make sure we do not wait for input while there is unhandled data in
     * that cache. Also, if the cache is there, we call ftp_readresp() and
     * the cache was not good enough to continue we must not busy-loop around
     * this function.
     *
     */

    إذا(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    وإلا إذا(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      صحيح ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      إذا(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }
      وإلا إذا(ev == 0) {
        result = Curl_pgrsUpdate(data);
        استمر; /* continue in our loop for the timeout duration */
      }
    }

    إذا(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      إذا(result)
        اكسر;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    إذا(result)
      اكسر;

    إذا(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    وإلا
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (صحيح)result, *nreadp, *ftpcodep);

  أرجع result;
}

ساكن CURLcode ftp_state_user(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  إذا(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  أرجع result;
}

ساكن CURLcode ftp_state_pwd(بنية Curl_easy *data,
                              بنية ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  إذا(!data->id && getenv("CURL_FTP_PWD_STOP"))
    أرجع CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  إذا(!result)
    ftp_state(data, ftpc, FTP_PWD);

  أرجع result;
}

/* For the FTP "protocol connect" and "doing" phases only */
ساكن CURLcode ftp_pollset(بنية Curl_easy *data,
                            بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ساكن CURLcode ftp_domore_pollset(بنية Curl_easy *data,
                                   بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  إذا(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    أرجع Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  أرجع Curl_pp_pollset(data, &ftpc->pp, ps);
}

ساكن صحيح pathlen(بنية ftp_conn *ftpc, صحيح رقم)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > رقم);
  أرجع ftpc->dirs[رقم].len;
}

ساكن ثابت حرف *pathpiece(بنية ftp_conn *ftpc, صحيح رقم)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > رقم);
  أرجع &ftpc->rawpath[ftpc->dirs[رقم].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ساكن CURLcode ftp_state_cwd(بنية Curl_easy *data,
                              بنية ftp_conn *ftpc,
                              بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  إذا(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  وإلا {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    إذا(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      إذا(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    وإلا {
      إذا(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        إذا(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      وإلا {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  أرجع result;
}

تعريفنوع تعداد {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ساكن CURLcode ftp_port_parse_string(بنية Curl_easy *data,
                                      بنية connectdata *conn,
                                      ثابت حرف *string_ftpport,
                                      بنية Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen)
{
  ثابت حرف *ip_end = عدم;
  ثابت حرف *addr = عدم;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  حرف ipstr[50];
#ifndef USE_IPV6
  (فراغ)conn;
  (فراغ)ss;
#endif

  /* default to nothing */
  *hostp = عدم;
  *port_minp = *port_maxp = 0;

  إذا(!string_ftpport || (strlen(string_ftpport) <= 1))
    اذهب done;

#ifdef USE_IPV6
  إذا(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ثابت حرف *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    إذا(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  وإلا
#endif
    إذا(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    وإلا {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      إذا(ip_end) {
#ifdef USE_IPV6
        بنية sockaddr_in6 * ثابت sa6 = (فراغ *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        إذا(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = عدم; /* this got no port ! */
        }
#endif
      }
      وإلا
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  إذا(ip_end) {
    ثابت حرف *portp = strchr(ip_end, ':');
    إذا(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      إذا(!curlx_str_رقم(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        إذا(!curlx_str_single(&portp, '-') &&
           !curlx_str_رقم(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        وإلا
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  إذا(port_min > port_max)
    port_min = port_max = 0;

  إذا(addrlen) {
    ثابت بنية Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    إذا(!remote_addr || (addrlen >= حجم(ipstr)) || (addrlen >= hbuflen))
      أرجع CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    بدل(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    حالة IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      اكسر;
    حالة IF2IP_AF_NOT_SUPPORTED:
      أرجع CURLE_FTP_PORT_FAILED;
    حالة IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      اكسر;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  أرجع CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ساكن CURLcode ftp_port_default_host(بنية Curl_easy *data,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#ifdef USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#endif
  حرف buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = حجم(*ss);
  إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
    أرجع CURLE_FTP_PORT_FAILED;
  }
  بدل(sa->sa_family) {
#ifdef USE_IPV6
  حالة AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    اكسر;
#endif
  افتراضي:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    اكسر;
  }
  إذا(result)
    أرجع CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  أرجع CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ساكن CURLcode ftp_port_resolve_host(بنية Curl_easy *data,
                                      بنية connectdata *conn,
                                      ثابت حرف *host,
                                      بنية Curl_dns_entry **dns_entryp,
                                      ثابت بنية Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = عدم;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  إذا(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  وإلا {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  أرجع result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ساكن CURLcode ftp_port_open_socket(بنية Curl_easy *data,
                                     بنية connectdata *conn,
                                     ثابت بنية Curl_addrinfo *res,
                                     ثابت بنية Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  حرف buffer[STRERROR_LEN];
  صحيح sockerr = 0;
  ثابت بنية Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  لكل(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, عدم,
                       Curl_conn_get_transport(data, conn), portsockp);
    إذا(result) {
      إذا(result == CURLE_OUT_OF_MEMORY)
        أرجع result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      استمر;
    }
    اكسر;
  }
  إذا(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, حجم(buffer)));
    أرجع CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  أرجع result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ساكن CURLcode ftp_port_bind_socket(بنية Curl_easy *data,
                                     بنية connectdata *conn,
                                     curl_socket_t portsock,
                                     ثابت بنية Curl_addrinfo *ai,
                                     بنية Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#ifdef USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#endif
  حرف buffer[STRERROR_LEN];
  unsigned short port;
  صحيح sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  لكل(port = port_min; port <= port_max;) {
    إذا(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    وإلا
      sa6->sin6_port = htons(port);
#endif
    إذا(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      إذا(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, حجم(buffer)));

        *sslen_io = حجم(*ss);
        إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
          أرجع CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        استمر;
      }
      إذا(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, حجم(buffer)));
        أرجع CURLE_FTP_PORT_FAILED;
      }
    }
    وإلا
      اكسر;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    إذا(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      أرجع CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = حجم(*ss);
  إذا(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
    أرجع CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  أرجع CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ساكن CURLcode ftp_port_listen(بنية Curl_easy *data, curl_socket_t portsock)
{
  حرف buffer[STRERROR_LEN];

  إذا(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
    أرجع CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  أرجع CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ساكن CURLcode ftp_port_send_command(بنية Curl_easy *data,
                                      بنية ftp_conn *ftpc,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      ثابت بنية Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ساكن ثابت حرف mode[][5] = { "EPRT", "PORT" };
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#ifdef USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#endif
  حرف myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, حجم(myhost));

#ifdef USE_IPV6
  إذا(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  لكل(; fcmd != DONE; fcmd++) {

    إذا(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      استمر;

    إذا((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      استمر;

    بدل(sa->sa_family) {
    حالة AF_INET:
      port = ntohs(sa4->sin_port);
      اكسر;
#ifdef USE_IPV6
    حالة AF_INET6:
      port = ntohs(sa6->sin6_port);
      اكسر;
#endif
    افتراضي:
      استمر; /* might as well skip this */
    }

    إذا(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      إذا(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        أرجع result;
      }
      اكسر;
    }
    إذا(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      حرف target[حجم(myhost) + 20];
      ثابت حرف *source = myhost;
      حرف *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      طالما(*source) {
        إذا(*source == '.')
          *dest = ',';
        وإلا
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (صحيح)(port >> 8), (صحيح)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      إذا(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        أرجع result;
      }
      اكسر;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  أرجع CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ساكن CURLcode ftp_state_use_port(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  بنية connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  بنية Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  حرف hbuf[NI_MAXHOST];
  ثابت حرف *host = عدم;
  ثابت حرف *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  بنية Curl_dns_entry *dns_entry = عدم;
  ثابت بنية Curl_addrinfo *res = عدم;
  ثابت بنية Curl_addrinfo *ai = عدم;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, حجم(hbuf));
  إذا(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, حجم(hbuf), &non_local);

  /* resolve host string to address list */
  إذا(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  إذا(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  إذا(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  إذا(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  إذا(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  إذا(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  إذا(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  إذا(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  إذا(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  وإلا {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    إذا(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      result = Curl_ssl_cfilter_add(
        data, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->bits.do_more = FALSE;
    Curl_expire(data, (data->set.accepttimeout > 0) ?
                data->set.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  إذا(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  أرجع result;
}

ساكن CURLcode ftp_state_use_pasv(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc,
                                   بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  ساكن ثابت حرف mode[][5] = { "EPSV", "PASV" };
  صحيح modeoff;

#ifdef PF_INET6
  إذا(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  إذا(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  أرجع result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ساكن CURLcode ftp_state_prepare_transfer(بنية Curl_easy *data,
                                           بنية ftp_conn *ftpc,
                                           بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  إذا(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  وإلا إذا(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  وإلا {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    إذا(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      إذا(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      وإلا إذا(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      وإلا
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      إذا(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    وإلا
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  أرجع result;
}

ساكن CURLcode ftp_state_rest(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  إذا((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    إذا(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  وإلا
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  أرجع result;
}

ساكن CURLcode ftp_state_size(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  إذا((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    إذا(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  وإلا
    result = ftp_state_rest(data, ftpc, ftp);

  أرجع result;
}

ساكن CURLcode ftp_state_list(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  ثابت حرف *lstArg = عدم;
  صحيح lstArglen = 0;
  حرف *cmd;

  إذا((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ثابت حرف *rawPath = ftpc->rawpath;
    ثابت حرف *slashPos = strrchr(rawPath, '/');
    إذا(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      إذا(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (صحيح)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  إذا(!cmd)
    أرجع CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  إذا(!result)
    ftp_state(data, ftpc, FTP_LIST);

  أرجع result;
}

ساكن CURLcode ftp_state_list_prequote(بنية Curl_easy *data,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ساكن CURLcode ftp_state_retr_prequote(بنية Curl_easy *data,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ساكن CURLcode ftp_state_stor_prequote(بنية Curl_easy *data,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ساكن CURLcode ftp_state_type(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  إذا(data->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)data->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                         FTP_TYPE);
    إذا(result)
      أرجع result;
  }
  وإلا
    result = ftp_state_size(data, ftpc, ftp);

  أرجع result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  إذا((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    إذا(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  وإلا
    result = ftp_state_type(data, ftpc, ftp);

  أرجع result;
}

/* This is called after the TYPE and possible quote commands have been sent */
ساكن CURLcode ftp_state_ul_setup(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  إذا((data->state.resume_from && !sizechecked) ||
     ((data->state.resume_from > 0) && sizechecked)) {
    /* we are about to continue the uploading of a file
       1. get already existing file's size. We use the SIZE command for this
          which may not exist in the server!  The SIZE command is not in
          RFC959.

       2. This used to set REST, but since we can do append, we issue no
          another ftp command. Skip the source file offset and APPEND the rest
          on the file instead

       3. pass file-size number of bytes in the source file
       4. lower the infilesize counter */
    /* => transfer as usual */
    صحيح seekerr = CURL_SEEKFUNC_OK;

    إذا(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      إذا(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      أرجع result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    إذا(data->set.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)حجم(scratch)) ?
          حجم(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    إذا(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      إذا(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        أرجع CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  إذا(!result)
    ftp_state(data, ftpc, FTP_STOR);

  أرجع result;
}

ساكن CURLcode ftp_state_retr(بنية Curl_easy *data,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  إذا(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  إذا(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    إذا(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    وإلا {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      إذا(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        إذا(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      وإلا {
        إذا(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    إذا(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      أرجع CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    إذا(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  وإلا {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    إذا(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  أرجع result;
}

ساكن CURLcode ftp_state_quote(بنية Curl_easy *data,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  بنية curl_slist *item;

  بدل(instate) {
  حالة FTP_QUOTE:
  افتراضي:
    item = data->set.quote;
    اكسر;
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    اكسر;
  حالة FTP_POSTQUOTE:
    item = data->set.postquote;
    اكسر;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  إذا(init)
    ftpc->count1 = 0;
  وإلا
    ftpc->count1++;

  إذا(item) {
    صحيح i = 0;

    /* Skip count1 items in the linked list */
    طالما((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    إذا(item) {
      ثابت حرف *cmd = item->data;
      إذا(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      وإلا
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      إذا(result)
        أرجع result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  إذا(!quote) {
    /* No more quote to send, continue to ... */
    بدل(instate) {
    حالة FTP_QUOTE:
    افتراضي:
      result = ftp_state_cwd(data, ftpc, ftp);
      اكسر;
    حالة FTP_RETR_PREQUOTE:
      إذا(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      وإلا {
        إذا(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        وإلا {
          إذا(data->set.ignorecl || data->state.prefer_ascii) {
            /* 'ignorecl' is used to support download of growing files. It
               prevents the state machine from requesting the file size from
               the server. With an unknown file size the download continues
               until the server terminates it, otherwise the client stops if
               the received byte count exceeds the reported file size. Set
               option CURLOPT_IGNORE_CONTENT_LENGTH to 1 to enable this
               behavior.

               In addition: asking for the size for 'TYPE A' transfers is not
               constructive since servers do not report the converted size.
               Thus, skip it. */
            result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
            إذا(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          وإلا {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            إذا(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      اكسر;
    حالة FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      اكسر;
    حالة FTP_POSTQUOTE:
      اكسر;
    حالة FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      اكسر;
    }
  }

  أرجع result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ساكن CURLcode ftp_epsv_disable(بنية Curl_easy *data,
                                 بنية ftp_conn *ftpc,
                                 بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;

  إذا(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  إذا(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  أرجع result;
}

ساكن CURLcode ftp_control_addr_dup(بنية Curl_easy *data, حرف **newhostp)
{
  بنية connectdata *conn = data->conn;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  إذا(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  وإلا
#endif
  إذا(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  وإلا {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = عدم;
    أرجع CURLE_FTP_CANT_GET_HOST;
  }
  أرجع *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ساكن bool match_pasv_6nums(ثابت حرف *p,
                             unsigned صحيح *array) /* 6 numbers */
{
  صحيح i;
  لكل(i = 0; i < 6; i++) {
    curl_off_t رقم;
    إذا(i) {
      إذا(*p != ',')
        أرجع FALSE;
      p++;
    }
    إذا(curlx_str_رقم(&p, &رقم, 0xff))
      أرجع FALSE;
    array[i] = (unsigned صحيح)رقم;
  }
  أرجع TRUE;
}

ساكن CURLcode ftp_state_pasv_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = data->conn;
  CURLcode result;
  ثابت بنية pingpong *pp = &ftpc->pp;
  حرف *newhost = عدم;
  unsigned short newport = 0;
  ثابت حرف *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  إذا((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ثابت حرف *ptr = strchr(str, '(');
    إذا(ptr) {
      حرف sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      إذا(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ثابت حرف *p = &ptr[3];
        curl_off_t رقم;
        إذا(curlx_str_رقم(&p, &رقم, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          أرجع CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)رقم;
        result = ftp_control_addr_dup(data, &newhost);
        إذا(result)
          أرجع result;
      }
      وإلا
        ptr = عدم;
    }
    إذا(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      أرجع CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  وإلا إذا((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned صحيح ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    طالما(*str) {
      إذا(match_pasv_6nums(str, ip))
        اكسر;
      str++;
    }

    إذا(!*str) {
      failf(data, "Could not interpret the 227-response");
      أرجع CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    إذا(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      إذا(result)
        أرجع result;
    }
    وإلا
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    إذا(!newhost)
      أرجع CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  وإلا إذا(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    أرجع ftp_epsv_disable(data, ftpc, conn);
  }
  وإلا {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    أرجع CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  إذا(result)
    اذهب error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  إذا(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    إذا(result)
      اذهب error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  إذا(result) {
    إذا((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    اذهب error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  إذا(data->set.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(data, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->port);
  }
#endif

  conn->bits.do_more = TRUE;
  ftp_state(data, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  أرجع result;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_statemach(بنية Curl_easy *data,
                              بنية ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  أرجع result;
}

/*
 * ftp_do_more()
 *
 * This function shall be called when the second FTP (data) connection is
 * connected.
 *
 * 'more' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (which is for when PASV is being sent to retry a failed EPSV).
 */
ساكن CURLcode ftp_do_more(بنية Curl_easy *data, domore *more)
{
  بنية connectdata *conn = data->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  إذا(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    إذا(result == CURLE_OUT_OF_MEMORY)
      أرجع result;
    إذا(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      إذا(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        أرجع ftp_epsv_disable(data, ftpc, conn);
      }
      أرجع result;
    }
  }

  إذا(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    إذا(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    إذا(result || !ftpc->wait_data_conn)
      أرجع result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  إذا(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    إذا(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      إذا(result)
        أرجع result; /* Failed to accept data connection */

      إذا(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        إذا(result)
          أرجع result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      وإلا {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        إذا(result)
          أرجع result;
      }
    }
    وإلا إذا(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      إذا(result)
        أرجع result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    وإلا {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      إذا(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      إذا(result)
        أرجع result;

      إذا((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        إذا(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          إذا(result)
            أرجع result;
        }
        /* otherwise fall through */
      }
      وإلا {
        إذا(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        وإلا {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        إذا(result)
          أرجع result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      إذا(complete)
        *more = DOMORE_DONE;
    }
    أرجع result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  إذا(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (صحيح)result);
  }

  أرجع result;
}

/* call this when the DO phase has completed */
ساكن CURLcode ftp_dophase_done(بنية Curl_easy *data,
                                 بنية ftp_conn *ftpc,
                                 بنية FTP *ftp,
                                 bool connected)
{
  إذا(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    إذا(result) {
      close_secondarysocket(data, ftpc);
      أرجع result;
    }
  }

  إذا(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  وإلا إذا(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_state_port_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  إذا(ftpcode / 100 != 2) {
    /* the command failed */

    إذا(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    إذا(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    وإلا
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  وإلا {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  أرجع result;
}

/* return TRUE on error, FALSE on success */
ساكن bool twodigit(ثابت حرف *p, صحيح *val)
{
  إذا(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    أرجع TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  أرجع FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ثابت حرف *p, صحيح *year, صحيح *month, صحيح *day,
                           صحيح *hour, صحيح *minute, صحيح *second);
UNITTEST bool ftp_213_date(ثابت حرف *p, صحيح *year, صحيح *month, صحيح *day,
                           صحيح *hour, صحيح *minute, صحيح *second)
{
  صحيح century;
  إذا((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    أرجع FALSE;

  *year += century * 100;
  إذا((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode client_write_header(بنية Curl_easy *data,
                                    حرف *buf, size_t blen)
{
  /* Some replies from an FTP server are written to the client
   * as CLIENTWRITE_HEADER, formatted as if they came from a
   * HTTP conversation.
   * In all protocols, CLIENTWRITE_HEADER data is only passed to
   * the body write callback when data->set.include_header is set
   * via CURLOPT_HEADER.
   * For historic reasons, FTP never played this game and expects
   * all its headers to do that always. Set that flag during the
   * call to Curl_client_write() so it does the right thing.
   *
   * Notice that we cannot enable this flag for FTP in general,
   * as an FTP transfer might involve an HTTP proxy connection and
   * headers from CONNECT should not automatically be part of the
   * output. */
  CURLcode result;
  bool save = (bool)data->set.include_header;
  data->set.include_header = TRUE;
  result = Curl_client_write(data, CLIENTWRITE_HEADER, buf, blen);
  data->set.include_header = save;
  أرجع result;
}

ساكن CURLcode ftp_state_mdtm_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  CURLcode result = CURLE_OK;

  بدل(ftpcode) {
  حالة 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    صحيح year, month, day, hour, minute, second;
    بنية pingpong *pp = &ftpc->pp;
    ثابت حرف *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    إذا(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      حرف timebuf[24];
      curl_msnprintf(timebuf, حجم(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      إذا(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ادفع
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    إذا(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic اسحب
#endif
      حرف headerbuf[128];
      صحيح headerbuflen;
      time_t filetime = data->info.filetime;
      بنية tm buffer;
      ثابت بنية tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      إذا(result)
        أرجع result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, حجم(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                       tm->tm_mday,
                       Curl_month[tm->tm_mon],
                       tm->tm_year + 1900,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec);
      result = client_write_header(data, headerbuf, headerbuflen);
      إذا(result)
        أرجع result;
    } /* end of a ridiculous amount of conditionals */
  }
    اكسر;
  افتراضي:
    infof(data, "unsupported MDTM reply format");
    اكسر;
  حالة 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    اكسر;
  }

  إذا(data->set.timecondition) {
    إذا((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      بدل(data->set.timecondition) {
      حالة CURL_TIMECOND_IFMODSINCE:
      افتراضي:
        إذا(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      حالة CURL_TIMECOND_IFUNMODSINCE:
        إذا(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      } /* switch */
    }
    وإلا {
      infof(data, "Skipping time comparison");
    }
  }

  إذا(!result)
    result = ftp_state_type(data, ftpc, ftp);

  أرجع result;
}

ساكن CURLcode ftp_state_type_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  إذا(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    أرجع CURLE_FTP_COULDNT_SET_TYPE;
  }
  إذا(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  إذا(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  وإلا إذا(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  وإلا إذا(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  أرجع result;
}

ساكن CURLcode ftp_state_size_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ثابت حرف *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  إذا(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ثابت حرف *start = &buf[4];
    ثابت حرف *fdigit = memchr(start, '\r', len - 4);
    إذا(fdigit) {
      fdigit--;
      إذا(*fdigit == '\n')
        fdigit--;
      طالما(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    وإلا
      fdigit = start;
    إذا(curlx_str_رقم(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  وإلا إذا(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    أرجع CURLE_REMOTE_FILE_NOT_FOUND;
  }

  إذا(instate == FTP_SIZE) {
    إذا(filesize != -1) {
      حرف clbuf[128];
      صحيح clbuflen = curl_msnprintf(clbuf, حجم(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      إذا(result)
        أرجع result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  وإلا إذا(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  وإلا إذا(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  أرجع result;
}

ساكن CURLcode ftp_state_rest_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  بدل(instate) {
  حالة FTP_REST:
  افتراضي:
    إذا(ftpcode == 350) {
      حرف buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      إذا(result)
        أرجع result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    اكسر;

  حالة FTP_RETR_REST:
    إذا(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    وإلا {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      إذا(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    اكسر;
  }

  أرجع result;
}

ساكن CURLcode ftp_state_stor_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode result = CURLE_OK;

  إذا(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    أرجع CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  إذا(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    إذا(result)
      أرجع result;

    إذا(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      أرجع ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  أرجع ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
ساكن CURLcode ftp_state_get_resp(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   صحيح ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  إذا((ftpcode == 150) || (ftpcode == 125)) {

    /*
      A;
      150 Opening BINARY mode data connection for /etc/passwd (2241
      bytes).  (ok, the file is being transferred)

      B:
      150 Opening ASCII mode data connection for /bin/ls

      C:
      150 ASCII data connection for /bin/ls (137.167.104.91,37445) (0 bytes).

      D:
      150 Opening ASCII mode data connection for [file] (0.0.0.0,0) (545 bytes)

      E:
      125 Data connection already open; Transfer starting. */

    data->req.size = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    إذا((instate != FTP_LIST) &&
       !data->state.prefer_ascii &&
       !data->set.ignorecl &&
       (ftp->downloadsize < 1)) {
      /*
       * It seems directory listings either do not show the size or often uses
       * size 0 anyway. ASCII transfers may cause that the transferred amount
       * of data is not the same as this line tells, why using this number in
       * those cases only confuses us.
       *
       * Example D above makes this parsing a little tricky */
      size_t len = curlx_dyn_len(&ftpc->pp.recvbuf);
      إذا(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        لكل(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ثابت حرف *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ثابت حرف *c = &buf[i];
          إذا(!curlx_str_رقم(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            اكسر;
          }
        }
      }
    }
    وإلا إذا(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    إذا(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    وإلا إذا((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    إذا(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    إذا(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      إذا(result)
        أرجع result;

      إذا(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        أرجع ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    أرجع ftp_initiate_transfer(data, ftpc);
  }
  وإلا {
    إذا((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    وإلا {
      failf(data, "RETR response: %03d", ftpcode);
      أرجع instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  أرجع result;
}

/* after USER, PASS and ACCT */
ساكن CURLcode ftp_state_loggedin(بنية Curl_easy *data,
                                   بنية ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  إذا(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PBSZ = PROTECTION BUFFER SIZE.

    The 'draft-murray-auth-ftp-ssl' (draft 12, page 7) says:

    Specifically, the PROT command MUST be preceded by a PBSZ
    command and a PBSZ command MUST be preceded by a successful
    security data exchange (the TLS negotiation in this case)

    ... (and on page 8):

    Thus the PBSZ command must still be issued, but must have a
    parameter of '0' to indicate that no buffering is taking place
    and the data connection should not be encapsulated.
    */
    result = Curl_pp_sendf(data, &ftpc->pp, "PBSZ %d", 0);
    إذا(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  وإلا {
    result = ftp_state_pwd(data, ftpc);
  }
  أرجع result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ساكن bool ftp_has_ctrl(ثابت حرف *string)
{
  ثابت unsigned حرف *s = (ثابت unsigned حرف *)string;
  طالما(*s) {
    إذا(*s < 0x20)
      أرجع TRUE;
    s++;
  }
  أرجع FALSE;
}

/* for USER and PASS responses */
ساكن CURLcode ftp_state_user_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  إذا((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    إذا(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  وإلا إذا(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  وإلا إذا(ftpcode == 332) {
    ثابت حرف *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    إذا(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    وإلا إذا(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    وإلا {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      إذا(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  وإلا {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ثابت حرف *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    إذا(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      إذا(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      وإلا {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        إذا(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    وإلا {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  أرجع result;
}

/* for ACCT response */
ساكن CURLcode ftp_state_acct_resp(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode result = CURLE_OK;
  إذا(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  وإلا
    result = ftp_state_loggedin(data, ftpc);

  أرجع result;
}

ساكن CURLcode ftp_pwd_resp(بنية Curl_easy *data,
                             بنية ftp_conn *ftpc,
                             صحيح ftpcode)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode result;

  إذا(ftpcode == 257) {
    ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    بنية dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    طالما(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    إذا('\"' == *ptr) {
      /* it started good */
      لكل(ptr++; *ptr; ptr++) {
        إذا('\"' == *ptr) {
          إذا('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          وإلا {
            /* end of path */
            إذا(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            اكسر; /* get out of this loop */
          }
        }
        وإلا {
          إذا(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            أرجع CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        إذا(result) {
          curlx_dyn_free(&out);
          أرجع result;
        }
      }
    }
    إذا(entry_extracted) {
      /* If the path name does not look like an absolute path (i.e.: it
         does not start with a '/'), we probably need some server-dependent
         adjustments. For example, this is the case when connecting to
         an OS400 FTP server: this server supports two name syntaxes,
         the default one being incompatible with standard paths. In
         addition, this server switches automatically to the regular path
         syntax when one is encountered in a command: this results in
         having an entrypath in the wrong syntax when later used in CWD.
         The method used here is to check the server OS: we do it only
         if the path name looks strange to minimize overhead on other
         systems. */
      حرف *dir = curlx_dyn_ptr(&out);

      إذا(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        إذا(result) {
          curlx_dyn_free(&out);
          أرجع result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      إذا(!data->state.most_recent_ftp_entrypath)
        أرجع CURLE_OUT_OF_MEMORY;

      إذا(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        أرجع CURLE_OK;
      }
    }
    وإلا {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  أرجع CURLE_OK;
}

ساكن ثابت حرف * ثابت ftpauth[] = { "SSL", "TLS" };

ساكن CURLcode ftp_wait_resp(بنية Curl_easy *data,
                              بنية connectdata *conn,
                              بنية ftp_conn *ftpc,
                              صحيح ftpcode)
{
  CURLcode result = CURLE_OK;
  إذا(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    إذا(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      أرجع ftp_state_user_resp(data, ftpc, ftpcode);
  }
  وإلا إذا(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  إذا(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    بدل((long)data->set.ftpsslauth) {
    حالة CURLFTPAUTH_DEFAULT:
    حالة CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      اكسر;
    حالة CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      اكسر;
    افتراضي:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (صحيح)data->set.ftpsslauth);
      أرجع CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    إذا(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  وإلا
    result = ftp_state_user(data, ftpc, conn);
  أرجع result;
}

/* an implicit TLS scheme implies TLS on both connections */
ساكن unsigned حرف ftp_use_ssl(بنية connectdata *conn,
                                 بنية ftp_conn *ftpc)
{
  إذا(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    أرجع CURLUSESSL_ALL;
  أرجع ftpc->use_ssl;
}

ساكن CURLcode ftp_pp_statemachine(بنية Curl_easy *data,
                                    بنية connectdata *conn)
{
  CURLcode result;
  صحيح ftpcode;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  إذا(result || !ftpcode)
    أرجع result;

  /* we have now received a full FTP server response */
  بدل(ftpc->state) {
  حالة FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    اكسر;

  حالة FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    إذا(pp->overflow)
      أرجع CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    إذا((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        إذا(result) {
          /* we failed and bail out */
          أرجع CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      إذا(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    وإلا إذا(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    وإلا {
      إذا(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      وإلا
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    اكسر;

  حالة FTP_USER:
  حالة FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    اكسر;

  حالة FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    اكسر;

  حالة FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    إذا(!result)
      ftp_state(data, ftpc, FTP_PROT);
    اكسر;

  حالة FTP_PROT:
    إذا(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    وإلا إذا(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      أرجع CURLE_USE_SSL_FAILED;

    إذا(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      إذا(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    وإلا
      result = ftp_state_pwd(data, ftpc);
    اكسر;

  حالة FTP_CCC:
    إذا(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned حرف)CURLFTPSSL_CCC_ACTIVE));
      إذا(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    إذا(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    اكسر;

  حالة FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    اكسر;

  حالة FTP_SYST:
    إذا(ftpcode == 215) {
      ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ثابت حرف *start;
      حرف *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      طالما(*ptr == ' ')
        ptr++;
      لكل(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      إذا(!os)
        أرجع CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      إذا(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        إذا(result) {
          curlx_free(os);
          أرجع result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        اكسر;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    وإلا {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_NAMEFMT:
    إذا(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      اكسر;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_QUOTE:
  حالة FTP_POSTQUOTE:
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    إذا((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    وإلا
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    اكسر;

  حالة FTP_CWD:
    إذا(ftpcode / 100 != 2) {
      /* failure to CWD there */
      إذا(data->set.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (data->set.ftp_create_missing_dirs == 2) ? 1 : 0;

        result = Curl_pp_sendf(data, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        إذا(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      وإلا {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    وإلا {
      /* success */
      ftpc->count2 = 0;
      إذا(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      وإلا {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    اكسر;

  حالة FTP_MKD:
    إذا((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    وإلا {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    اكسر;

  حالة FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_TYPE:
  حالة FTP_LIST_TYPE:
  حالة FTP_RETR_TYPE:
  حالة FTP_STOR_TYPE:
  حالة FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_SIZE:
  حالة FTP_RETR_SIZE:
  حالة FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_REST:
  حالة FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_PRET:
    إذا(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      أرجع CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    اكسر;

  حالة FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    اكسر;

  حالة FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_LIST:
  حالة FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    اكسر;

  حالة FTP_QUIT:
  افتراضي:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    اكسر;
  }

  أرجع result;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_multi_statemach(بنية Curl_easy *data,
                                    bool *done)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

ساكن CURLcode ftp_block_statemach(بنية Curl_easy *data,
                                    بنية ftp_conn *ftpc)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  طالما(ftpc->state != FTP_STOP) {
    إذا(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    إذا(result)
      اكسر;
  }

  أرجع result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ساكن CURLcode ftp_connect(بنية Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  بنية connectdata *conn = data->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp;

  *done = FALSE; /* default to not done yet */
  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    إذا(result)
      أرجع result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  أرجع result;
}

/***********************************************************************
 *
 * ftp_sendquote()
 *
 * Where a 'quote' means a list of custom commands to send to the server.
 * The quote list is passed as an argument.
 *
 * BLOCKING
 */
ساكن CURLcode ftp_sendquote(بنية Curl_easy *data,
                              بنية ftp_conn *ftpc,
                              بنية curl_slist *quote)
{
  بنية curl_slist *item;
  بنية pingpong *pp = &ftpc->pp;

  item = quote;
  طالما(item) {
    إذا(item->data) {
      size_t nread;
      ثابت حرف *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      صحيح ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      إذا(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      إذا(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      إذا(result)
        أرجع result;

      إذا(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        أرجع CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_done_status(بنية Curl_easy *data,
                                بنية ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  بدل(status) {
  حالة CURLE_BAD_DOWNLOAD_RESUME:
  حالة CURLE_FTP_WEIRD_PASV_REPLY:
  حالة CURLE_FTP_PORT_FAILED:
  حالة CURLE_FTP_ACCEPT_FAILED:
  حالة CURLE_FTP_ACCEPT_TIMEOUT:
  حالة CURLE_FTP_COULDNT_SET_TYPE:
  حالة CURLE_FTP_COULDNT_RETR_FILE:
  حالة CURLE_PARTIAL_FILE:
  حالة CURLE_UPLOAD_FAILED:
  حالة CURLE_REMOTE_ACCESS_DENIED:
  حالة CURLE_FILESIZE_EXCEEDED:
  حالة CURLE_REMOTE_FILE_NOT_FOUND:
  حالة CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  حالة CURLE_OK: /* does not affect the control connection's status */
    إذا(!premature)
      اكسر;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  افتراضي:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    أرجع status;      /* use the already set error code */
  }
  أرجع CURLE_OK;
}

ساكن فراغ ftp_done_wildcard(بنية Curl_easy *data, بنية ftp_conn *ftpc)
{
  إذا(data->state.wildcardmatch) {
    إذا(data->set.chunk_end && ftpc->file) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

ساكن فراغ ftp_done_path(بنية Curl_easy *data, بنية ftp_conn *ftpc,
                          CURLcode result)
{
  بنية connectdata *conn = data->conn;
  إذا(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  وإلا { /* remember working directory for connection reuse */
    ثابت حرف *rawPath = ftpc->rawpath;
    إذا(rawPath) {
      إذا((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      وإلا {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        إذا(!ftpc->cwdfail) {
          إذا(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          وإلا
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        وإلا
          ftpc->prevpath = عدم; /* no path */
      }
    }
    إذا(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ساكن CURLcode ftp_done_secondary_socket(بنية Curl_easy *data,
                                          بنية ftp_conn *ftpc,
                                          CURLcode result)
{
  بنية connectdata *conn = data->conn;
  إذا(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    إذا(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      إذا(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  أرجع result;
}

ساكن CURLcode ftp_done_control_reply(بنية Curl_easy *data,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode result,
                                       bool premature)
{
  بنية connectdata *conn = data->conn;
  size_t nread;
  صحيح ftpcode;

  إذا(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    إذا(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    إذا(result)
      أرجع result;

    إذا(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      أرجع result;
    }

    إذا(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      بدل(ftpcode) {
      حالة 226:
      حالة 250:
        اكسر;
      حالة 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        اكسر;
      افتراضي:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        اكسر;
      }
    }
  }
  أرجع result;
}

ساكن CURLcode ftp_done_check_partial(بنية Curl_easy *data,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode result,
                                       bool premature)
{
  إذا(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  وإلا إذا(data->state.upload) {
    إذا((ftp->transfer == PPTRANSFER_BODY) &&
       (data->state.infilesize != -1) && /* upload with known size */
       ((!data->set.crlf && !data->state.prefer_ascii && /* no conversion */
         (data->state.infilesize != data->req.writebytecount)) ||
        ((data->set.crlf || data->state.prefer_ascii) && /* maybe CRLF conv */
         (data->state.infilesize > data->req.writebytecount))
       )) {
      failf(data, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            data->req.writebytecount, data->state.infilesize);
      result = CURLE_PARTIAL_FILE;
    }
  }
  وإلا {
    إذا((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    وإلا إذا(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  أرجع result;
}

/***********************************************************************
 *
 * ftp_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
ساكن CURLcode ftp_done(بنية Curl_easy *data, CURLcode status,
                         bool premature)
{
  بنية FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  إذا(!ftp || !ftpc)
    أرجع CURLE_OK;

  result = ftp_done_status(data, ftpc, status, premature);

  ftp_done_wildcard(data, ftpc);
  ftp_done_path(data, ftpc, result);
  result = ftp_done_secondary_socket(data, ftpc, result);
  result = ftp_done_control_reply(data, ftpc, ftp, result, premature);
  result = ftp_done_check_partial(data, ftpc, ftp, result, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  إذا(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (صحيح)result);
  أرجع result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ساكن CURLcode ftp_nb_type(بنية Curl_easy *data,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  حرف want = (حرف)(ascii ? 'A' : 'I');

  إذا(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    أرجع ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  إذا(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  أرجع result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ساكن CURLcode ftp_perform(
  بنية Curl_easy *data,
  بنية ftp_conn *ftpc,
  بنية FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  إذا(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  إذا(result)
    أرجع result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  إذا(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  وإلا
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  إذا(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  أرجع result;
}

ساكن فراغ wc_data_dtor(فراغ *ptr)
{
  بنية ftp_wc *ftpwc = ptr;
  إذا(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ساكن CURLcode init_wc_data(بنية Curl_easy *data,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  حرف *last_slash;
  حرف *path = ftp->path;
  بنية WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  بنية ftp_wc *ftpwc = عدم;

  last_slash = strrchr(ftp->path, '/');
  إذا(last_slash) {
    last_slash++;
    إذا(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    إذا(!wildcard->pattern)
      أرجع CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  وإلا { /* there is only 'wildcard pattern' or nothing */
    إذا(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      إذا(!wildcard->pattern)
        أرجع CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    وإلا { /* only list */
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, حجم(بنية ftp_wc));
  إذا(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  إذا(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  إذا(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  إذا(result) {
    اذهب fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  إذا(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  infof(data, "Wildcard - Parsing started");
  أرجع CURLE_OK;

fail:
  إذا(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = عدم;
  أرجع result;
}

ساكن CURLcode wc_statemach(بنية Curl_easy *data,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  بنية WildcardData * ثابت wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  لكل(;;) {
    بدل(wildcard->state) {
    حالة CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      إذا(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        أرجع result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      أرجع result;

    حالة CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      بنية ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      إذا(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        استمر;
      }
      إذا(Curl_llist_عدد(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        أرجع CURLE_REMOTE_FILE_NOT_FOUND;
      }
      استمر;
    }

    حالة CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      بنية Curl_llist_عقدة *head = Curl_llist_head(&wildcard->filelist);
      بنية curl_fileinfo *finfo = Curl_عقدة_elem(head);
      حرف *tmp_path;
      حرف *enc = curl_easy_escape(عدم, finfo->filename, 0);
      إذا(!enc)
        أرجع CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      إذا(!tmp_path)
        أرجع CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      إذا(data->set.chunk_bgn) {
        long userresponse;
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (صحيح)Curl_llist_عدد(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        بدل(userresponse) {
        حالة CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          استمر;
        حالة CURL_CHUNK_BGN_FUNC_FAIL:
          أرجع CURLE_CHUNK_FAILED;
        }
      }

      إذا(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        استمر;
      }

      إذا(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      إذا(result)
        أرجع result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_عقدة_remove(Curl_llist_head(&wildcard->filelist));

      إذا(Curl_llist_عدد(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        أرجع CURLE_OK;
      }
      أرجع result;
    }

    حالة CURLWC_SKIP: {
      إذا(data->set.chunk_end) {
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_عقدة_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_عدد(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      استمر;
    }

    حالة CURLWC_CLEAN: {
      بنية ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      إذا(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      أرجع result;
    }

    حالة CURLWC_DONE:
    حالة CURLWC_ERROR:
    حالة CURLWC_CLEAR:
      إذا(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = عدم;
      }
      أرجع result;
    }
  }
  /* UNREACHABLE */
}

/***********************************************************************
 *
 * ftp_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 *
 * ftp->ctl_valid starts out as FALSE, and gets set to TRUE if we reach the
 * ftp_done() function without finding any major problem.
 */
ساكن CURLcode ftp_regular_transfer(بنية Curl_easy *data,
                                     بنية ftp_conn *ftpc,
                                     بنية FTP *ftp,
                                     bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  data->req.size = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(data);

  ftpc->ctl_valid = TRUE; /* starts good */

  result = ftp_perform(data, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_done); /* all commands in the DO-phase done? */

  إذا(!result) {

    إذا(!*dophase_done)
      /* the DO phase has not completed yet */
      أرجع CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    إذا(result)
      أرجع result;
  }
  وإلا
    freedirs(ftpc);

  أرجع result;
}

/***********************************************************************
 *
 * ftp_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (ftp_perform).
 *
 * The input argument is already checked for validity.
 */
ساكن CURLcode ftp_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    بنية Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    إذا(result)
      أرجع result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    إذا(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      أرجع result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  إذا(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    إذا(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      أرجع CURLE_OK;
    }
    إذا(result) /* error, loop or skipping the file */
      أرجع result;
  }
  وإلا { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    إذا(result)
      أرجع result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  أرجع result;
}

/***********************************************************************
 *
 * ftp_quit()
 *
 * This should be called before calling sclose() on an ftp control connection
 * (not data connections). We should then wait for the response from the
 * server before returning. The calling code should then try to close the
 * connection.
 *
 */
ساكن CURLcode ftp_quit(بنية Curl_easy *data,
                         بنية ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  إذا(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    إذا(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      أرجع result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  أرجع result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode ftp_disconnect(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  إذا(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (فراغ)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  أرجع CURLE_OK;
}

/* called from multi.c while DOing */
ساكن CURLcode ftp_doing(بنية Curl_easy *data,
                          bool *dophase_done)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  إذا(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  وإلا إذا(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  أرجع result;
}

ساكن فراغ ftp_easy_dtor(ثابت فراغ *مفتاح, size_t klen, فراغ *entry)
{
  بنية FTP *ftp = entry;
  (فراغ)مفتاح;
  (فراغ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ساكن فراغ ftp_conn_dtor(ثابت فراغ *مفتاح, size_t klen, فراغ *entry)
{
  بنية ftp_conn *ftpc = entry;
  (فراغ)مفتاح;
  (فراغ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ساكن فراغ type_url_check(بنية Curl_easy *data, بنية FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  إذا((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    حرف *type = &ftp->path[len - 7];
    حرف command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    بدل(command) {
    حالة 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      اكسر;

    حالة 'D': /* directory mode */
      data->state.list_only = TRUE;
      اكسر;

    حالة 'I': /* binary mode */
    افتراضي:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      اكسر;
    }
  }
}

ساكن CURLcode ftp_setup_connection(بنية Curl_easy *data,
                                     بنية connectdata *conn)
{
  بنية FTP *ftp;
  CURLcode result = CURLE_OK;
  بنية ftp_conn *ftpc;

  ftp = curlx_calloc(1, حجم(*ftp));
  إذا(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, حجم(*ftpc));
  إذا(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  إذا(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    إذا(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }
  إذا(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    إذا(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }

  ftp->path = &data->state.up.path[1]; /* do not include the initial slash */

  type_url_check(data, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->known_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = data->set.use_ssl;
  ftpc->ccc = data->set.ftp_ccc;

  CURL_TRC_FTP(data, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (صحيح)result);
  أرجع result;
}

bool Curl_ftp_conns_match(بنية connectdata *needle, بنية connectdata *conn)
{
  بنية ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  بنية ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  إذا(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    أرجع FALSE;

  بدل(nftpc->use_ssl) {
  حالة CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    أرجع (cftpc->use_ssl != CURLUSESSL_NONE);
  افتراضي:
    أرجع (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_ftp = {
  ftp_setup_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_done,                        /* done */
  ftp_do_more,                     /* do_more */
  ftp_connect,                     /* connect_it */
  ftp_multi_statemach,             /* connecting */
  ftp_doing,                       /* doing */
  ftp_pollset,                     /* proto_pollset */
  ftp_pollset,                     /* doing_pollset */
  ftp_domore_pollset,              /* domore_pollset */
  ZERO_NULL,                       /* perform_pollset */
  ftp_disconnect,                  /* disconnect */
  ZERO_NULL,                       /* write_resp */
  ZERO_NULL,                       /* write_resp_hd */
  ZERO_NULL,                       /* connection_is_dead */
  ZERO_NULL,                       /* attach connection */
  ZERO_NULL,                       /* follow */
};

#endif /* CURL_DISABLE_FTP */
