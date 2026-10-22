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
#شامل "curl_setup.h"
#شامل "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل "sendf.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "if2ip.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "escape.h"
#شامل "ftp.h"
#شامل "ftp-int.h"
#شامل "ftplistparser.h"
#شامل "curl_range.h"
#شامل "strcase.h"
#شامل "vtls/vtls.h"
#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "connect.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/inet_pton.h"
#شامل "select.h"
#شامل "parsedate.h" /* for the week day and month names */
#شامل "sockaddr.h" /* required for Curl_sockaddr_storage */
#شامل "multiif.h"
#شامل "url.h"
#شامل "http_proxy.h"
#شامل "vdns/hostip.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"
#شامل "curl_ctype.h"

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
ایستا ثابت نویسه * ثابت ftp_state_names[] = {
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
ایستا تهی ftp_state_کم(ساختار Curl_easy *data,
                          ساختار ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , صحیح lineno
#endif
  )
{
  اگر(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((تهی)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#وگرنه
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_کم(x, y, z)
#وگرنه /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_کم(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

ایستا CURLcode ftp_state_mdtm(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp);
ایستا CURLcode ftp_state_quote(ساختار Curl_easy *data,
                                ساختار ftp_conn *ftpc,
                                ساختار FTP *ftp,
                                bool init, ftpstate instate);
ایستا CURLcode ftp_nb_type(ساختار Curl_easy *data,
                            ساختار ftp_conn *ftpc,
                            ساختار FTP *ftp,
                            bool ascii, ftpstate newstate);
ایستا CURLcode getftpresponse(ساختار Curl_easy *data, size_t *nreadp,
                               صحیح *ftpcodep);

ایستا تهی freedirs(ساختار ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = پوچ;
}

ایستا size_t numof_slashes(ثابت نویسه *str)
{
  ثابت نویسه *slashPos;
  size_t رقم = 0;
  انجامبده {
    slashPos = strchr(str, '/');
    اگر(slashPos) {
      ++رقم;
      str = slashPos + 1;
    }
  } وقتی(slashPos);
  بازگشت رقم;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ایستا CURLcode ftp_parse_url_path(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc,
                                   ساختار FTP *ftp)
{
  ثابت نویسه *slashPos = پوچ;
  ثابت نویسه *fileName = پوچ;
  CURLcode result = CURLE_OK;
  ثابت نویسه *rawPath = پوچ; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  اگر(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  اگر(result) {
    failf(data, "path contains control characters");
    بازگشت result;
  }
  rawPath = ftpc->rawpath;

  انتخاب(data->set.ftp_filemethod) {
  حالت FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    اگر((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    بشکن;

  حالت FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    اگر(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      اگر(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, اندازه(ftpc->dirs[0]));
      اگر(!ftpc->dirs)
        بازگشت CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (صحیح)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    وگرنه
      fileName = rawPath; /* filename only (or empty) */
    بشکن;

  پیشفرض: /* allow pretty much anything */
  حالت FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ثابت نویسه *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    اگر(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      بازگشت CURLE_URL_MALFORMAT;

    اگر(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, اندازه(ftpc->dirs[0]));
      اگر(!ftpc->dirs)
        بازگشت CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      وقتی(dirAlloc--) {
        ثابت نویسه *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        اگر(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        اگر(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (صحیح)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (صحیح)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    بشکن;
  } /* switch */

  اگر(fileName && *fileName)
    ftpc->file = fileName;
  وگرنه
    ftpc->file = پوچ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  اگر(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    بازگشت CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  اگر((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  وگرنه { /* newly created FTP connections are already in entry path */
    ثابت نویسه *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    اگر(oldPath) {
      size_t n = pathLen;
      اگر(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      وگرنه
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      اگر((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  بازگشت CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ایستا صحیح ftp_need_type(ساختار ftp_conn *ftpc,
                         bool ascii_wanted)
{
  بازگشت ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ایستا تهی close_secondarysocket(ساختار Curl_easy *data,
                                  ساختار ftp_conn *ftpc)
{
  (تهی)ftpc;
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
ساختار ftp_cw_lc_ctx {
  ساختار Curl_cwriter super;
  bool newline_pending;
};

ایستا CURLcode ftp_cw_lc_write(ساختار Curl_easy *data,
                                ساختار Curl_cwriter *writer, صحیح type,
                                ثابت نویسه *buf, size_t blen)
{
  ایستا ثابت نویسه nl = '\n';
  ساختار ftp_cw_lc_ctx *ctx = writer->ctx;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    بازگشت CURLE_FAILED_INIT;

  اگر(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  وقتی(blen) {
    /* do not pass EOS when writing parts */
    صحیح chunk_type = (type & ~CLIENTWRITE_EOS);
    ثابت نویسه *cp;
    size_t chunk_len;
    CURLcode result;

    اگر(ctx->newline_pending) {
      اگر(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        اگر(result)
          بازگشت result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    اگر(!cp)
      بشکن;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    اگر(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      اگر(result)
        بازگشت result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  اگر(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  وگرنه اگر(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    اگر(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      بازگشت Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    بازگشت Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  بازگشت CURLE_OK;
}

ایستا ثابت ساختار Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  پوچ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  اندازه(ساختار ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ایستا CURLcode ftp_check_ctrl_on_data_wait(ساختار Curl_easy *data,
                                            ساختار ftp_conn *ftpc)
{
  ساختار connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ساختار pingpong *pp = &ftpc->pp;
  size_t nread;
  صحیح ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  اگر(curlx_dyn_len(&pp->recvbuf)) {
    ثابت نویسه *l = curlx_dyn_ptr(&pp->recvbuf);
    اگر(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (تهی)getftpresponse(data, &nread, &ftpcode);
      بازگشت CURLE_FTP_ACCEPT_FAILED;
    }
  }

  اگر(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  وگرنه {
    صحیح socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    انتخاب(socketstate) {
    حالت -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      بازگشت CURLE_FTP_ACCEPT_FAILED;
    پیشفرض:
      اگر(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      بشکن;
    }
  }

  اگر(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    اگر(pp->overflow > 3) {
      ثابت نویسه *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      اگر((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        اگر(!curlx_str_عدد(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          بازگشت CURLE_OK;
        }
      }
    }

    (تهی)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    اگر(ftpcode / 100 > 3)
      بازگشت CURLE_FTP_ACCEPT_FAILED;

    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  بازگشت CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ایستا CURLcode ftp_initiate_transfer(ساختار Curl_easy *data,
                                      ساختار ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  اگر(result || !connected)
    بازگشت result;

  اگر(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  وگرنه {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  بازگشت CURLE_OK;
}

ایستا bool ftp_endofresp(ساختار Curl_easy *data, ساختار connectdata *conn,
                          ثابت نویسه *سطر, size_t len, صحیح *code)
{
  curl_off_t status;
  (تهی)data;
  (تهی)conn;

  اگر((len > 3) && LASTLINE(سطر) && !curlx_str_عدد(&سطر, &status, 999)) {
    *code = (صحیح)status;
    بازگشت TRUE;
  }

  بازگشت FALSE;
}

ایستا CURLcode ftp_readresp(ساختار Curl_easy *data,
                             ساختار ftp_conn *ftpc,
                             int8_t sockindex,
                             ساختار pingpong *pp,
                             صحیح *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  صحیح code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  اگر(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  اگر(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }

  بازگشت result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ایستا CURLcode getftpresponse(ساختار Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               صحیح *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ساختار connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ساختار pingpong *pp = &ftpc->pp;
  size_t nread;
  صحیح cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  اگر(!ftpc)
    بازگشت CURLE_FAILED_INIT;

  وقتی(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    اگر(timeout <= 0) {
      failf(data, "FTP response timeout");
      بازگشت CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    اگر(timeout < interval_ms)
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

    اگر(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    وگرنه اگر(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      صحیح ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      اگر(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        بازگشت CURLE_RECV_ERROR;
      }
      وگرنه اگر(ev == 0) {
        result = Curl_pgrsUpdate(data);
        ادامه; /* continue in our loop for the timeout duration */
      }
    }

    اگر(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      اگر(result)
        بشکن;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    اگر(result)
      بشکن;

    اگر(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    وگرنه
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (صحیح)result, *nreadp, *ftpcodep);

  بازگشت result;
}

ایستا CURLcode ftp_state_user(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  اگر(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  بازگشت result;
}

ایستا CURLcode ftp_state_pwd(ساختار Curl_easy *data,
                              ساختار ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  اگر(!data->id && getenv("CURL_FTP_PWD_STOP"))
    بازگشت CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  اگر(!result)
    ftp_state(data, ftpc, FTP_PWD);

  بازگشت result;
}

/* For the FTP "protocol connect" and "doing" phases only */
ایستا CURLcode ftp_pollset(ساختار Curl_easy *data,
                            ساختار easy_pollset *ps)
{
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بازگشت ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ایستا CURLcode ftp_domore_pollset(ساختار Curl_easy *data,
                                   ساختار easy_pollset *ps)
{
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    بازگشت CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  اگر(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    بازگشت Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  بازگشت Curl_pp_pollset(data, &ftpc->pp, ps);
}

ایستا صحیح pathlen(ساختار ftp_conn *ftpc, صحیح رقم)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > رقم);
  بازگشت ftpc->dirs[رقم].len;
}

ایستا ثابت نویسه *pathpiece(ساختار ftp_conn *ftpc, صحیح رقم)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > رقم);
  بازگشت &ftpc->rawpath[ftpc->dirs[رقم].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ایستا CURLcode ftp_state_cwd(ساختار Curl_easy *data,
                              ساختار ftp_conn *ftpc,
                              ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;

  اگر(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  وگرنه {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    اگر(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      اگر(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    وگرنه {
      اگر(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        اگر(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      وگرنه {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  بازگشت result;
}

تعریفنوع شمارش {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ایستا CURLcode ftp_port_parse_string(ساختار Curl_easy *data,
                                      ساختار connectdata *conn,
                                      ثابت نویسه *string_ftpport,
                                      ساختار Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ثابت نویسه **hostp,
                                      نویسه *hbuf, size_t hbuflen)
{
  ثابت نویسه *ip_end = پوچ;
  ثابت نویسه *addr = پوچ;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  نویسه ipstr[50];
#ifndef USE_IPV6
  (تهی)conn;
  (تهی)ss;
#endif

  /* default to nothing */
  *hostp = پوچ;
  *port_minp = *port_maxp = 0;

  اگر(!string_ftpport || (strlen(string_ftpport) <= 1))
    برو done;

#ifdef USE_IPV6
  اگر(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ثابت نویسه *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    اگر(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  وگرنه
#endif
    اگر(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    وگرنه {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      اگر(ip_end) {
#ifdef USE_IPV6
        ساختار sockaddr_in6 * ثابت sa6 = (تهی *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        اگر(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = پوچ; /* this got no port ! */
        }
#endif
      }
      وگرنه
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  اگر(ip_end) {
    ثابت نویسه *portp = strchr(ip_end, ':');
    اگر(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      اگر(!curlx_str_عدد(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        اگر(!curlx_str_single(&portp, '-') &&
           !curlx_str_عدد(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        وگرنه
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  اگر(port_min > port_max)
    port_min = port_max = 0;

  اگر(addrlen) {
    ثابت ساختار Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    اگر(!remote_addr || (addrlen >= اندازه(ipstr)) || (addrlen >= hbuflen))
      بازگشت CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    انتخاب(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    حالت IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      بشکن;
    حالت IF2IP_AF_NOT_SUPPORTED:
      بازگشت CURLE_FTP_PORT_FAILED;
    حالت IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      بشکن;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  بازگشت CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ایستا CURLcode ftp_port_default_host(ساختار Curl_easy *data,
                                      ساختار connectdata *conn,
                                      ساختار Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ثابت نویسه **hostp,
                                      نویسه *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ساختار sockaddr *sa = (ساختار sockaddr *)ss;
  ساختار sockaddr_in * ثابت sa4 = (تهی *)sa;
#ifdef USE_IPV6
  ساختار sockaddr_in6 * ثابت sa6 = (تهی *)sa;
#endif
  نویسه buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = اندازه(*ss);
  اگر(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
    بازگشت CURLE_FTP_PORT_FAILED;
  }
  انتخاب(sa->sa_family) {
#ifdef USE_IPV6
  حالت AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    بشکن;
#endif
  پیشفرض:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    بشکن;
  }
  اگر(result)
    بازگشت CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  بازگشت CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ایستا CURLcode ftp_port_resolve_host(ساختار Curl_easy *data,
                                      ساختار connectdata *conn,
                                      ثابت نویسه *host,
                                      ساختار Curl_dns_entry **dns_entryp,
                                      ثابت ساختار Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = پوچ;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  اگر(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  وگرنه {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  بازگشت result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ایستا CURLcode ftp_port_open_socket(ساختار Curl_easy *data,
                                     ساختار connectdata *conn,
                                     ثابت ساختار Curl_addrinfo *res,
                                     ثابت ساختار Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  نویسه buffer[STRERROR_LEN];
  صحیح sockerr = 0;
  ثابت ساختار Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  برای(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, پوچ,
                       Curl_conn_get_transport(data, conn), portsockp);
    اگر(result) {
      اگر(result == CURLE_OUT_OF_MEMORY)
        بازگشت result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      ادامه;
    }
    بشکن;
  }
  اگر(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, اندازه(buffer)));
    بازگشت CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  بازگشت result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ایستا CURLcode ftp_port_bind_socket(ساختار Curl_easy *data,
                                     ساختار connectdata *conn,
                                     curl_socket_t portsock,
                                     ثابت ساختار Curl_addrinfo *ai,
                                     ساختار Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  ساختار sockaddr *sa = (ساختار sockaddr *)ss;
  ساختار sockaddr_in * ثابت sa4 = (تهی *)sa;
#ifdef USE_IPV6
  ساختار sockaddr_in6 * ثابت sa6 = (تهی *)sa;
#endif
  نویسه buffer[STRERROR_LEN];
  unsigned short port;
  صحیح sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  برای(port = port_min; port <= port_max;) {
    اگر(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    وگرنه
      sa6->sin6_port = htons(port);
#endif
    اگر(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      اگر(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, اندازه(buffer)));

        *sslen_io = اندازه(*ss);
        اگر(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
          بازگشت CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        ادامه;
      }
      اگر(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, اندازه(buffer)));
        بازگشت CURLE_FTP_PORT_FAILED;
      }
    }
    وگرنه
      بشکن;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    اگر(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      بازگشت CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = اندازه(*ss);
  اگر(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
    بازگشت CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  بازگشت CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ایستا CURLcode ftp_port_listen(ساختار Curl_easy *data, curl_socket_t portsock)
{
  نویسه buffer[STRERROR_LEN];

  اگر(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
    بازگشت CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  بازگشت CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ایستا CURLcode ftp_port_send_command(ساختار Curl_easy *data,
                                      ساختار ftp_conn *ftpc,
                                      ساختار connectdata *conn,
                                      ساختار Curl_sockaddr_storage *ss,
                                      ثابت ساختار Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ایستا ثابت نویسه mode[][5] = { "EPRT", "PORT" };
  ساختار sockaddr *sa = (ساختار sockaddr *)ss;
  ساختار sockaddr_in * ثابت sa4 = (تهی *)sa;
#ifdef USE_IPV6
  ساختار sockaddr_in6 * ثابت sa6 = (تهی *)sa;
#endif
  نویسه myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, اندازه(myhost));

#ifdef USE_IPV6
  اگر(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  برای(; fcmd != DONE; fcmd++) {

    اگر(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ادامه;

    اگر((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ادامه;

    انتخاب(sa->sa_family) {
    حالت AF_INET:
      port = ntohs(sa4->sin_port);
      بشکن;
#ifdef USE_IPV6
    حالت AF_INET6:
      port = ntohs(sa6->sin6_port);
      بشکن;
#endif
    پیشفرض:
      ادامه; /* might as well skip this */
    }

    اگر(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      اگر(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        بازگشت result;
      }
      بشکن;
    }
    اگر(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      نویسه target[اندازه(myhost) + 20];
      ثابت نویسه *source = myhost;
      نویسه *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      وقتی(*source) {
        اگر(*source == '.')
          *dest = ',';
        وگرنه
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (صحیح)(port >> 8), (صحیح)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      اگر(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        بازگشت result;
      }
      بشکن;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  بازگشت CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ایستا CURLcode ftp_state_use_port(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  ساختار connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ساختار Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  نویسه hbuf[NI_MAXHOST];
  ثابت نویسه *host = پوچ;
  ثابت نویسه *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  ساختار Curl_dns_entry *dns_entry = پوچ;
  ثابت ساختار Curl_addrinfo *res = پوچ;
  ثابت ساختار Curl_addrinfo *ai = پوچ;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, اندازه(hbuf));
  اگر(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, اندازه(hbuf), &non_local);

  /* resolve host string to address list */
  اگر(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  اگر(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  اگر(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  اگر(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  اگر(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  اگر(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  اگر(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  اگر(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  اگر(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  وگرنه {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    اگر(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  اگر(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  بازگشت result;
}

ایستا CURLcode ftp_state_use_pasv(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc,
                                   ساختار connectdata *conn)
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

  ایستا ثابت نویسه mode[][5] = { "EPSV", "PASV" };
  صحیح modeoff;

#ifdef PF_INET6
  اگر(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  اگر(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  بازگشت result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ایستا CURLcode ftp_state_prepare_transfer(ساختار Curl_easy *data,
                                           ساختار ftp_conn *ftpc,
                                           ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;

  اگر(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  وگرنه اگر(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  وگرنه {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    اگر(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      اگر(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      وگرنه اگر(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      وگرنه
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      اگر(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    وگرنه
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  بازگشت result;
}

ایستا CURLcode ftp_state_rest(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;

  اگر((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    اگر(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  وگرنه
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  بازگشت result;
}

ایستا CURLcode ftp_state_size(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;

  اگر((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    اگر(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  وگرنه
    result = ftp_state_rest(data, ftpc, ftp);

  بازگشت result;
}

ایستا CURLcode ftp_state_list(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp)
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
  ثابت نویسه *lstArg = پوچ;
  صحیح lstArglen = 0;
  نویسه *cmd;

  اگر((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ثابت نویسه *rawPath = ftpc->rawpath;
    ثابت نویسه *slashPos = strrchr(rawPath, '/');
    اگر(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      اگر(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (صحیح)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  اگر(!cmd)
    بازگشت CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  اگر(!result)
    ftp_state(data, ftpc, FTP_LIST);

  بازگشت result;
}

ایستا CURLcode ftp_state_list_prequote(ساختار Curl_easy *data,
                                        ساختار ftp_conn *ftpc,
                                        ساختار FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  بازگشت ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ایستا CURLcode ftp_state_retr_prequote(ساختار Curl_easy *data,
                                        ساختار ftp_conn *ftpc,
                                        ساختار FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  بازگشت ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ایستا CURLcode ftp_state_stor_prequote(ساختار Curl_easy *data,
                                        ساختار ftp_conn *ftpc,
                                        ساختار FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  بازگشت ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ایستا CURLcode ftp_state_type(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  اگر(data->req.no_body && ftpc->file &&
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
    اگر(result)
      بازگشت result;
  }
  وگرنه
    result = ftp_state_size(data, ftpc, ftp);

  بازگشت result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ایستا CURLcode ftp_state_mdtm(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  اگر((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    اگر(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  وگرنه
    result = ftp_state_type(data, ftpc, ftp);

  بازگشت result;
}

/* This is called after the TYPE and possible quote commands have been sent */
ایستا CURLcode ftp_state_ul_setup(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc,
                                   ساختار FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  اگر((data->state.resume_from && !sizechecked) ||
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
    صحیح seekerr = CURL_SEEKFUNC_OK;

    اگر(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      اگر(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      بازگشت result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    اگر(data->set.seek_func) {
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    اگر(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        بازگشت CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      انجامبده {
        نویسه scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)اندازه(scratch)) ?
          اندازه(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          بازگشت CURLE_FTP_COULDNT_USE_REST;
        }
      } وقتی(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    اگر(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      اگر(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        بازگشت CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  اگر(!result)
    ftp_state(data, ftpc, FTP_STOR);

  بازگشت result;
}

ایستا CURLcode ftp_state_retr(ساختار Curl_easy *data,
                               ساختار ftp_conn *ftpc,
                               ساختار FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  اگر(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    بازگشت CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  اگر(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    اگر(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    وگرنه {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      اگر(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        اگر(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          بازگشت CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      وگرنه {
        اگر(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          بازگشت CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    اگر(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      بازگشت CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    اگر(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  وگرنه {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    اگر(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  بازگشت result;
}

ایستا CURLcode ftp_state_quote(ساختار Curl_easy *data,
                                ساختار ftp_conn *ftpc,
                                ساختار FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  ساختار curl_slist *item;

  انتخاب(instate) {
  حالت FTP_QUOTE:
  پیشفرض:
    item = data->set.quote;
    بشکن;
  حالت FTP_RETR_PREQUOTE:
  حالت FTP_STOR_PREQUOTE:
  حالت FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    بشکن;
  حالت FTP_POSTQUOTE:
    item = data->set.postquote;
    بشکن;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  اگر(init)
    ftpc->count1 = 0;
  وگرنه
    ftpc->count1++;

  اگر(item) {
    صحیح i = 0;

    /* Skip count1 items in the linked list */
    وقتی((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    اگر(item) {
      ثابت نویسه *cmd = item->data;
      اگر(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      وگرنه
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      اگر(result)
        بازگشت result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  اگر(!quote) {
    /* No more quote to send, continue to ... */
    انتخاب(instate) {
    حالت FTP_QUOTE:
    پیشفرض:
      result = ftp_state_cwd(data, ftpc, ftp);
      بشکن;
    حالت FTP_RETR_PREQUOTE:
      اگر(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      وگرنه {
        اگر(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        وگرنه {
          اگر(data->set.ignorecl || data->state.prefer_ascii) {
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
            اگر(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          وگرنه {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            اگر(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      بشکن;
    حالت FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      بشکن;
    حالت FTP_POSTQUOTE:
      بشکن;
    حالت FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      بشکن;
    }
  }

  بازگشت result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ایستا CURLcode ftp_epsv_disable(ساختار Curl_easy *data,
                                 ساختار ftp_conn *ftpc,
                                 ساختار connectdata *conn)
{
  CURLcode result = CURLE_OK;

  اگر(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  اگر(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  بازگشت result;
}

ایستا CURLcode ftp_control_addr_dup(ساختار Curl_easy *data, نویسه **newhostp)
{
  ساختار connectdata *conn = data->conn;
  ساختار ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  اگر(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  وگرنه
#endif
  اگر(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  وگرنه {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = پوچ;
    بازگشت CURLE_FTP_CANT_GET_HOST;
  }
  بازگشت *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ایستا bool match_pasv_6nums(ثابت نویسه *p,
                             unsigned صحیح *array) /* 6 numbers */
{
  صحیح i;
  برای(i = 0; i < 6; i++) {
    curl_off_t رقم;
    اگر(i) {
      اگر(*p != ',')
        بازگشت FALSE;
      p++;
    }
    اگر(curlx_str_عدد(&p, &رقم, 0xff))
      بازگشت FALSE;
    array[i] = (unsigned صحیح)رقم;
  }
  بازگشت TRUE;
}

ایستا CURLcode ftp_state_pasv_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result;
  ثابت ساختار pingpong *pp = &ftpc->pp;
  نویسه *newhost = پوچ;
  unsigned short newport = 0;
  ثابت نویسه *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  اگر((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ثابت نویسه *ptr = strchr(str, '(');
    اگر(ptr) {
      نویسه sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      اگر(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ثابت نویسه *p = &ptr[3];
        curl_off_t رقم;
        اگر(curlx_str_عدد(&p, &رقم, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          بازگشت CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)رقم;
        result = ftp_control_addr_dup(data, &newhost);
        اگر(result)
          بازگشت result;
      }
      وگرنه
        ptr = پوچ;
    }
    اگر(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      بازگشت CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  وگرنه اگر((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned صحیح ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    وقتی(*str) {
      اگر(match_pasv_6nums(str, ip))
        بشکن;
      str++;
    }

    اگر(!*str) {
      failf(data, "Could not interpret the 227-response");
      بازگشت CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    اگر(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      اگر(result)
        بازگشت result;
    }
    وگرنه
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    اگر(!newhost)
      بازگشت CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  وگرنه اگر(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    بازگشت ftp_epsv_disable(data, ftpc, conn);
  }
  وگرنه {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    بازگشت CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  اگر(result)
    برو error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  اگر(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    اگر(result)
      برو error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  اگر(result) {
    اگر((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    برو error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  اگر(data->set.verbose) {
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
  بازگشت result;
}

/* called repeatedly until done from multi.c */
ایستا CURLcode ftp_statemach(ساختار Curl_easy *data,
                              ساختار ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  بازگشت result;
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
ایستا CURLcode ftp_do_more(ساختار Curl_easy *data, domore *more)
{
  ساختار connectdata *conn = data->conn;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ساختار FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  اگر(!ftpc || !ftp)
    بازگشت CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  اگر(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    اگر(result == CURLE_OUT_OF_MEMORY)
      بازگشت result;
    اگر(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      اگر(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        بازگشت ftp_epsv_disable(data, ftpc, conn);
      }
      بازگشت result;
    }
  }

  اگر(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    اگر(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    اگر(result || !ftpc->wait_data_conn)
      بازگشت result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  اگر(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    اگر(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      اگر(result)
        بازگشت result; /* Failed to accept data connection */

      اگر(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        اگر(result)
          بازگشت result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      وگرنه {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        اگر(result)
          بازگشت result;
      }
    }
    وگرنه اگر(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      اگر(result)
        بازگشت result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    وگرنه {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      اگر(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      اگر(result)
        بازگشت result;

      اگر((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        اگر(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          اگر(result)
            بازگشت result;
        }
        /* otherwise fall through */
      }
      وگرنه {
        اگر(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        وگرنه {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        اگر(result)
          بازگشت result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      اگر(complete)
        *more = DOMORE_DONE;
    }
    بازگشت result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  اگر(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (صحیح)result);
  }

  بازگشت result;
}

/* call this when the DO phase has completed */
ایستا CURLcode ftp_dophase_done(ساختار Curl_easy *data,
                                 ساختار ftp_conn *ftpc,
                                 ساختار FTP *ftp,
                                 bool connected)
{
  اگر(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    اگر(result) {
      close_secondarysocket(data, ftpc);
      بازگشت result;
    }
  }

  اگر(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  وگرنه اگر(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  بازگشت CURLE_OK;
}

ایستا CURLcode ftp_state_port_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    ساختار FTP *ftp,
                                    صحیح ftpcode)
{
  ساختار connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  اگر(ftpcode / 100 != 2) {
    /* the command failed */

    اگر(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    اگر(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    وگرنه
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  وگرنه {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  بازگشت result;
}

/* return TRUE on error, FALSE on success */
ایستا bool twodigit(ثابت نویسه *p, صحیح *val)
{
  اگر(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    بازگشت TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  بازگشت FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ثابت نویسه *p, صحیح *year, صحیح *month, صحیح *day,
                           صحیح *hour, صحیح *minute, صحیح *second);
UNITTEST bool ftp_213_date(ثابت نویسه *p, صحیح *year, صحیح *month, صحیح *day,
                           صحیح *hour, صحیح *minute, صحیح *second)
{
  صحیح century;
  اگر((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    بازگشت FALSE;

  *year += century * 100;
  اگر((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    بازگشت FALSE;
  بازگشت TRUE;
}

ایستا CURLcode client_write_header(ساختار Curl_easy *data,
                                    نویسه *buf, size_t blen)
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
  بازگشت result;
}

ایستا CURLcode ftp_state_mdtm_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    ساختار FTP *ftp,
                                    صحیح ftpcode)
{
  CURLcode result = CURLE_OK;

  انتخاب(ftpcode) {
  حالت 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    صحیح year, month, day, hour, minute, second;
    ساختار pingpong *pp = &ftpc->pp;
    ثابت نویسه *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    اگر(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      نویسه timebuf[24];
      curl_msnprintf(timebuf, اندازه(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      اگر(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#اگر defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic افزودن
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    اگر(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#اگر defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic برداشت
#endif
      نویسه headerbuf[128];
      صحیح headerbuflen;
      time_t filetime = data->info.filetime;
      ساختار tm buffer;
      ثابت ساختار tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      اگر(result)
        بازگشت result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, اندازه(headerbuf),
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
      اگر(result)
        بازگشت result;
    } /* end of a ridiculous amount of conditionals */
  }
    بشکن;
  پیشفرض:
    infof(data, "unsupported MDTM reply format");
    بشکن;
  حالت 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    بشکن;
  }

  اگر(data->set.timecondition) {
    اگر((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      انتخاب(data->set.timecondition) {
      حالت CURL_TIMECOND_IFMODSINCE:
      پیشفرض:
        اگر(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          بازگشت CURLE_OK;
        }
        بشکن;
      حالت CURL_TIMECOND_IFUNMODSINCE:
        اگر(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          بازگشت CURLE_OK;
        }
        بشکن;
      } /* switch */
    }
    وگرنه {
      infof(data, "Skipping time comparison");
    }
  }

  اگر(!result)
    result = ftp_state_type(data, ftpc, ftp);

  بازگشت result;
}

ایستا CURLcode ftp_state_type_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    ساختار FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  اگر(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    بازگشت CURLE_FTP_COULDNT_SET_TYPE;
  }
  اگر(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  اگر(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  وگرنه اگر(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  وگرنه اگر(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  وگرنه اگر(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  وگرنه اگر(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  بازگشت result;
}

ایستا CURLcode ftp_state_size_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    ساختار FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ثابت نویسه *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  اگر(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ثابت نویسه *start = &buf[4];
    ثابت نویسه *fdigit = memchr(start, '\r', len - 4);
    اگر(fdigit) {
      fdigit--;
      اگر(*fdigit == '\n')
        fdigit--;
      وقتی(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    وگرنه
      fdigit = start;
    اگر(curlx_str_عدد(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  وگرنه اگر(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    بازگشت CURLE_REMOTE_FILE_NOT_FOUND;
  }

  اگر(instate == FTP_SIZE) {
    اگر(filesize != -1) {
      نویسه clbuf[128];
      صحیح clbuflen = curl_msnprintf(clbuf, اندازه(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      اگر(result)
        بازگشت result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  وگرنه اگر(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  وگرنه اگر(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  بازگشت result;
}

ایستا CURLcode ftp_state_rest_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    ساختار FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  انتخاب(instate) {
  حالت FTP_REST:
  پیشفرض:
    اگر(ftpcode == 350) {
      نویسه buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      اگر(result)
        بازگشت result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    بشکن;

  حالت FTP_RETR_REST:
    اگر(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    وگرنه {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      اگر(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    بشکن;
  }

  بازگشت result;
}

ایستا CURLcode ftp_state_stor_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode result = CURLE_OK;

  اگر(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    بازگشت CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  اگر(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    اگر(result)
      بازگشت result;

    اگر(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      بازگشت ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  بازگشت ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
ایستا CURLcode ftp_state_get_resp(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc,
                                   ساختار FTP *ftp,
                                   صحیح ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  اگر((ftpcode == 150) || (ftpcode == 125)) {

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

    اگر((instate != FTP_LIST) &&
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
      اگر(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        برای(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ثابت نویسه *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ثابت نویسه *c = &buf[i];
          اگر(!curlx_str_عدد(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            بشکن;
          }
        }
      }
    }
    وگرنه اگر(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    اگر(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    وگرنه اگر((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    اگر(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    اگر(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      اگر(result)
        بازگشت result;

      اگر(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        بازگشت ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    بازگشت ftp_initiate_transfer(data, ftpc);
  }
  وگرنه {
    اگر((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    وگرنه {
      failf(data, "RETR response: %03d", ftpcode);
      بازگشت instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  بازگشت result;
}

/* after USER, PASS and ACCT */
ایستا CURLcode ftp_state_loggedin(ساختار Curl_easy *data,
                                   ساختار ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  اگر(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    اگر(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  وگرنه {
    result = ftp_state_pwd(data, ftpc);
  }
  بازگشت result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ایستا bool ftp_has_ctrl(ثابت نویسه *string)
{
  ثابت unsigned نویسه *s = (ثابت unsigned نویسه *)string;
  وقتی(*s) {
    اگر(*s < 0x20)
      بازگشت TRUE;
    s++;
  }
  بازگشت FALSE;
}

/* for USER and PASS responses */
ایستا CURLcode ftp_state_user_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  اگر((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    اگر(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  وگرنه اگر(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  وگرنه اگر(ftpcode == 332) {
    ثابت نویسه *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    اگر(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    وگرنه اگر(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    وگرنه {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      اگر(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  وگرنه {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ثابت نویسه *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    اگر(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      اگر(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      وگرنه {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        اگر(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    وگرنه {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  بازگشت result;
}

/* for ACCT response */
ایستا CURLcode ftp_state_acct_resp(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode result = CURLE_OK;
  اگر(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  وگرنه
    result = ftp_state_loggedin(data, ftpc);

  بازگشت result;
}

ایستا CURLcode ftp_pwd_resp(ساختار Curl_easy *data,
                             ساختار ftp_conn *ftpc,
                             صحیح ftpcode)
{
  ساختار pingpong *pp = &ftpc->pp;
  CURLcode result;

  اگر(ftpcode == 257) {
    ثابت نویسه *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    ساختار dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    وقتی(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    اگر('\"' == *ptr) {
      /* it started good */
      برای(ptr++; *ptr; ptr++) {
        اگر('\"' == *ptr) {
          اگر('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          وگرنه {
            /* end of path */
            اگر(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            بشکن; /* get out of this loop */
          }
        }
        وگرنه {
          اگر(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            بازگشت CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        اگر(result) {
          curlx_dyn_free(&out);
          بازگشت result;
        }
      }
    }
    اگر(entry_extracted) {
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
      نویسه *dir = curlx_dyn_ptr(&out);

      اگر(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        اگر(result) {
          curlx_dyn_free(&out);
          بازگشت result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      اگر(!data->state.most_recent_ftp_entrypath)
        بازگشت CURLE_OUT_OF_MEMORY;

      اگر(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        بازگشت CURLE_OK;
      }
    }
    وگرنه {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  بازگشت CURLE_OK;
}

ایستا ثابت نویسه * ثابت ftpauth[] = { "SSL", "TLS" };

ایستا CURLcode ftp_wait_resp(ساختار Curl_easy *data,
                              ساختار connectdata *conn,
                              ساختار ftp_conn *ftpc,
                              صحیح ftpcode)
{
  CURLcode result = CURLE_OK;
  اگر(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    اگر(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      بازگشت ftp_state_user_resp(data, ftpc, ftpcode);
  }
  وگرنه اگر(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  اگر(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    انتخاب((long)data->set.ftpsslauth) {
    حالت CURLFTPAUTH_DEFAULT:
    حالت CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      بشکن;
    حالت CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      بشکن;
    پیشفرض:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (صحیح)data->set.ftpsslauth);
      بازگشت CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    اگر(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  وگرنه
    result = ftp_state_user(data, ftpc, conn);
  بازگشت result;
}

/* an implicit TLS scheme implies TLS on both connections */
ایستا unsigned نویسه ftp_use_ssl(ساختار connectdata *conn,
                                 ساختار ftp_conn *ftpc)
{
  اگر(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    بازگشت CURLUSESSL_ALL;
  بازگشت ftpc->use_ssl;
}

ایستا CURLcode ftp_pp_statemachine(ساختار Curl_easy *data,
                                    ساختار connectdata *conn)
{
  CURLcode result;
  صحیح ftpcode;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ساختار FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ساختار pingpong *pp;
  size_t nread = 0;

  اگر(!ftpc || !ftp)
    بازگشت CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  اگر(pp->sendleft)
    بازگشت Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  اگر(result || !ftpcode)
    بازگشت result;

  /* we have now received a full FTP server response */
  انتخاب(ftpc->state) {
  حالت FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    بشکن;

  حالت FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    اگر(pp->overflow)
      بازگشت CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    اگر((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        اگر(result) {
          /* we failed and bail out */
          بازگشت CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      اگر(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    وگرنه اگر(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    وگرنه {
      اگر(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      وگرنه
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    بشکن;

  حالت FTP_USER:
  حالت FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    بشکن;

  حالت FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    بشکن;

  حالت FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    اگر(!result)
      ftp_state(data, ftpc, FTP_PROT);
    بشکن;

  حالت FTP_PROT:
    اگر(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    وگرنه اگر(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      بازگشت CURLE_USE_SSL_FAILED;

    اگر(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      اگر(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    وگرنه
      result = ftp_state_pwd(data, ftpc);
    بشکن;

  حالت FTP_CCC:
    اگر(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned نویسه)CURLFTPSSL_CCC_ACTIVE));
      اگر(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    اگر(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    بشکن;

  حالت FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    بشکن;

  حالت FTP_SYST:
    اگر(ftpcode == 215) {
      ثابت نویسه *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ثابت نویسه *start;
      نویسه *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      وقتی(*ptr == ' ')
        ptr++;
      برای(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      اگر(!os)
        بازگشت CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      اگر(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        اگر(result) {
          curlx_free(os);
          بازگشت result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        بشکن;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    وگرنه {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    بشکن;

  حالت FTP_NAMEFMT:
    اگر(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      بشکن;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    بشکن;

  حالت FTP_QUOTE:
  حالت FTP_POSTQUOTE:
  حالت FTP_RETR_PREQUOTE:
  حالت FTP_STOR_PREQUOTE:
  حالت FTP_LIST_PREQUOTE:
    اگر((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    وگرنه
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    بشکن;

  حالت FTP_CWD:
    اگر(ftpcode / 100 != 2) {
      /* failure to CWD there */
      اگر(data->set.ftp_create_missing_dirs &&
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
        اگر(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      وگرنه {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    وگرنه {
      /* success */
      ftpc->count2 = 0;
      اگر(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      وگرنه {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    بشکن;

  حالت FTP_MKD:
    اگر((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    وگرنه {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    بشکن;

  حالت FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    بشکن;

  حالت FTP_TYPE:
  حالت FTP_LIST_TYPE:
  حالت FTP_RETR_TYPE:
  حالت FTP_STOR_TYPE:
  حالت FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    بشکن;

  حالت FTP_SIZE:
  حالت FTP_RETR_SIZE:
  حالت FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    بشکن;

  حالت FTP_REST:
  حالت FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    بشکن;

  حالت FTP_PRET:
    اگر(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      بازگشت CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    بشکن;

  حالت FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    بشکن;

  حالت FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    بشکن;

  حالت FTP_LIST:
  حالت FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    بشکن;

  حالت FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    بشکن;

  حالت FTP_QUIT:
  پیشفرض:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    بشکن;
  }

  بازگشت result;
}

/* called repeatedly until done from multi.c */
ایستا CURLcode ftp_multi_statemach(ساختار Curl_easy *data,
                                    bool *done)
{
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  بازگشت ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

ایستا CURLcode ftp_block_statemach(ساختار Curl_easy *data,
                                    ساختار ftp_conn *ftpc)
{
  ساختار pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  وقتی(ftpc->state != FTP_STOP) {
    اگر(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    اگر(result)
      بشکن;
  }

  بازگشت result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ایستا CURLcode ftp_connect(ساختار Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  ساختار connectdata *conn = data->conn;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ساختار pingpong *pp;

  *done = FALSE; /* default to not done yet */
  اگر(!ftpc)
    بازگشت CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    اگر(result)
      بازگشت result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  بازگشت result;
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
ایستا CURLcode ftp_sendquote(ساختار Curl_easy *data,
                              ساختار ftp_conn *ftpc,
                              ساختار curl_slist *quote)
{
  ساختار curl_slist *item;
  ساختار pingpong *pp = &ftpc->pp;

  item = quote;
  وقتی(item) {
    اگر(item->data) {
      size_t nread;
      ثابت نویسه *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      صحیح ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      اگر(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      اگر(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      اگر(result)
        بازگشت result;

      اگر(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        بازگشت CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode ftp_done_status(ساختار Curl_easy *data,
                                ساختار ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  انتخاب(status) {
  حالت CURLE_BAD_DOWNLOAD_RESUME:
  حالت CURLE_FTP_WEIRD_PASV_REPLY:
  حالت CURLE_FTP_PORT_FAILED:
  حالت CURLE_FTP_ACCEPT_FAILED:
  حالت CURLE_FTP_ACCEPT_TIMEOUT:
  حالت CURLE_FTP_COULDNT_SET_TYPE:
  حالت CURLE_FTP_COULDNT_RETR_FILE:
  حالت CURLE_PARTIAL_FILE:
  حالت CURLE_UPLOAD_FAILED:
  حالت CURLE_REMOTE_ACCESS_DENIED:
  حالت CURLE_FILESIZE_EXCEEDED:
  حالت CURLE_REMOTE_FILE_NOT_FOUND:
  حالت CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  حالت CURLE_OK: /* does not affect the control connection's status */
    اگر(!premature)
      بشکن;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  پیشفرض:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    بازگشت status;      /* use the already set error code */
  }
  بازگشت CURLE_OK;
}

ایستا تهی ftp_done_wildcard(ساختار Curl_easy *data, ساختار ftp_conn *ftpc)
{
  اگر(data->state.wildcardmatch) {
    اگر(data->set.chunk_end && ftpc->file) {
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

ایستا تهی ftp_done_path(ساختار Curl_easy *data, ساختار ftp_conn *ftpc,
                          CURLcode result)
{
  ساختار connectdata *conn = data->conn;
  اگر(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  وگرنه { /* remember working directory for connection reuse */
    ثابت نویسه *rawPath = ftpc->rawpath;
    اگر(rawPath) {
      اگر((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      وگرنه {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        اگر(!ftpc->cwdfail) {
          اگر(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          وگرنه
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        وگرنه
          ftpc->prevpath = پوچ; /* no path */
      }
    }
    اگر(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ایستا CURLcode ftp_done_secondary_socket(ساختار Curl_easy *data,
                                          ساختار ftp_conn *ftpc,
                                          CURLcode result)
{
  ساختار connectdata *conn = data->conn;
  اگر(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    اگر(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      اگر(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  بازگشت result;
}

ایستا CURLcode ftp_done_control_reply(ساختار Curl_easy *data,
                                       ساختار ftp_conn *ftpc,
                                       ساختار FTP *ftp, CURLcode result,
                                       bool premature)
{
  ساختار connectdata *conn = data->conn;
  size_t nread;
  صحیح ftpcode;

  اگر(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    اگر(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    اگر(result)
      بازگشت result;

    اگر(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      بازگشت result;
    }

    اگر(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      انتخاب(ftpcode) {
      حالت 226:
      حالت 250:
        بشکن;
      حالت 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        بشکن;
      پیشفرض:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        بشکن;
      }
    }
  }
  بازگشت result;
}

ایستا CURLcode ftp_done_check_partial(ساختار Curl_easy *data,
                                       ساختار ftp_conn *ftpc,
                                       ساختار FTP *ftp, CURLcode result,
                                       bool premature)
{
  اگر(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  وگرنه اگر(data->state.upload) {
    اگر((ftp->transfer == PPTRANSFER_BODY) &&
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
  وگرنه {
    اگر((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    وگرنه اگر(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  بازگشت result;
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
ایستا CURLcode ftp_done(ساختار Curl_easy *data, CURLcode status,
                         bool premature)
{
  ساختار FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  اگر(!ftp || !ftpc)
    بازگشت CURLE_OK;

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
  اگر(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (صحیح)result);
  بازگشت result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ایستا CURLcode ftp_nb_type(ساختار Curl_easy *data,
                            ساختار ftp_conn *ftpc,
                            ساختار FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  نویسه want = (نویسه)(ascii ? 'A' : 'I');

  اگر(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    بازگشت ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  اگر(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  بازگشت result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ایستا CURLcode ftp_perform(
  ساختار Curl_easy *data,
  ساختار ftp_conn *ftpc,
  ساختار FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  اگر(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  اگر(result)
    بازگشت result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  اگر(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  وگرنه
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  اگر(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  بازگشت result;
}

ایستا تهی wc_data_dtor(تهی *ptr)
{
  ساختار ftp_wc *ftpwc = ptr;
  اگر(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ایستا CURLcode init_wc_data(ساختار Curl_easy *data,
                             ساختار ftp_conn *ftpc,
                             ساختار FTP *ftp)
{
  نویسه *last_slash;
  نویسه *path = ftp->path;
  ساختار WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  ساختار ftp_wc *ftpwc = پوچ;

  last_slash = strrchr(ftp->path, '/');
  اگر(last_slash) {
    last_slash++;
    اگر(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      بازگشت ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    اگر(!wildcard->pattern)
      بازگشت CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  وگرنه { /* there is only 'wildcard pattern' or nothing */
    اگر(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      اگر(!wildcard->pattern)
        بازگشت CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    وگرنه { /* only list */
      wildcard->state = CURLWC_CLEAN;
      بازگشت ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, اندازه(ساختار ftp_wc));
  اگر(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  اگر(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  اگر(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  اگر(result) {
    برو fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  اگر(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  infof(data, "Wildcard - Parsing started");
  بازگشت CURLE_OK;

fail:
  اگر(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = پوچ;
  بازگشت result;
}

ایستا CURLcode wc_statemach(ساختار Curl_easy *data,
                             ساختار ftp_conn *ftpc,
                             ساختار FTP *ftp)
{
  ساختار WildcardData * ثابت wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  برای(;;) {
    انتخاب(wildcard->state) {
    حالت CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      اگر(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        بازگشت result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      بازگشت result;

    حالت CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ساختار ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      اگر(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ادامه;
      }
      اگر(Curl_llist_شمار(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        بازگشت CURLE_REMOTE_FILE_NOT_FOUND;
      }
      ادامه;
    }

    حالت CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ساختار Curl_llist_گره *head = Curl_llist_head(&wildcard->filelist);
      ساختار curl_fileinfo *finfo = Curl_گره_elem(head);
      نویسه *tmp_path;
      نویسه *enc = curl_easy_escape(پوچ, finfo->filename, 0);
      اگر(!enc)
        بازگشت CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      اگر(!tmp_path)
        بازگشت CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      اگر(data->set.chunk_bgn) {
        long userresponse;
        ساختار Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (صحیح)Curl_llist_شمار(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        انتخاب(userresponse) {
        حالت CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ادامه;
        حالت CURL_CHUNK_BGN_FUNC_FAIL:
          بازگشت CURLE_CHUNK_FAILED;
        }
      }

      اگر(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ادامه;
      }

      اگر(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      اگر(result)
        بازگشت result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_گره_remove(Curl_llist_head(&wildcard->filelist));

      اگر(Curl_llist_شمار(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        بازگشت CURLE_OK;
      }
      بازگشت result;
    }

    حالت CURLWC_SKIP: {
      اگر(data->set.chunk_end) {
        ساختار Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_گره_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_شمار(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ادامه;
    }

    حالت CURLWC_CLEAN: {
      ساختار ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      اگر(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      بازگشت result;
    }

    حالت CURLWC_DONE:
    حالت CURLWC_ERROR:
    حالت CURLWC_CLEAR:
      اگر(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = پوچ;
      }
      بازگشت result;
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
ایستا CURLcode ftp_regular_transfer(ساختار Curl_easy *data,
                                     ساختار ftp_conn *ftpc,
                                     ساختار FTP *ftp,
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

  اگر(!result) {

    اگر(!*dophase_done)
      /* the DO phase has not completed yet */
      بازگشت CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    اگر(result)
      بازگشت result;
  }
  وگرنه
    freedirs(ftpc);

  بازگشت result;
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
ایستا CURLcode ftp_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ساختار FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  اگر(!ftpc || !ftp)
    بازگشت CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ساختار Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    اگر(result)
      بازگشت result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    اگر(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      بازگشت result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  اگر(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    اگر(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      بازگشت CURLE_OK;
    }
    اگر(result) /* error, loop or skipping the file */
      بازگشت result;
  }
  وگرنه { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    اگر(result)
      بازگشت result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  بازگشت result;
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
ایستا CURLcode ftp_quit(ساختار Curl_easy *data,
                         ساختار ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  اگر(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    اگر(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      بازگشت result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ایستا CURLcode ftp_disconnect(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               bool dead_connection)
{
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    بازگشت CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  اگر(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (تهی)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  بازگشت CURLE_OK;
}

/* called from multi.c while DOing */
ایستا CURLcode ftp_doing(ساختار Curl_easy *data,
                          bool *dophase_done)
{
  ساختار ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ساختار FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  اگر(!ftpc || !ftp)
    بازگشت CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  اگر(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  وگرنه اگر(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  بازگشت result;
}

ایستا تهی ftp_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار FTP *ftp = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ایستا تهی ftp_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار ftp_conn *ftpc = entry;
  (تهی)کلید;
  (تهی)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ایستا تهی type_url_check(ساختار Curl_easy *data, ساختار FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  اگر((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    نویسه *type = &ftp->path[len - 7];
    نویسه command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    انتخاب(command) {
    حالت 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      بشکن;

    حالت 'D': /* directory mode */
      data->state.list_only = TRUE;
      بشکن;

    حالت 'I': /* binary mode */
    پیشفرض:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      بشکن;
    }
  }
}

ایستا CURLcode ftp_setup_connection(ساختار Curl_easy *data,
                                     ساختار connectdata *conn)
{
  ساختار FTP *ftp;
  CURLcode result = CURLE_OK;
  ساختار ftp_conn *ftpc;

  ftp = curlx_calloc(1, اندازه(*ftp));
  اگر(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, اندازه(*ftpc));
  اگر(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  اگر(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    اگر(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  اگر(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    اگر(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      بازگشت CURLE_OUT_OF_MEMORY;
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
               (صحیح)result);
  بازگشت result;
}

bool Curl_ftp_conns_match(ساختار connectdata *needle, ساختار connectdata *conn)
{
  ساختار ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ساختار ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  اگر(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    بازگشت FALSE;

  انتخاب(nftpc->use_ssl) {
  حالت CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    بازگشت (cftpc->use_ssl != CURLUSESSL_NONE);
  پیشفرض:
    بازگشت (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ثابت ساختار Curl_protocol Curl_protocol_ftp = {
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
