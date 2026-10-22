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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "if2ip.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "ftp.h"
#अन्तर्भाव "ftp-int.h"
#अन्तर्भाव "ftplistparser.h"
#अन्तर्भाव "curl_range.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "parsedate.h" /* for the week day and month names */
#अन्तर्भाव "sockaddr.h" /* required for Curl_sockaddr_storage */
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(ओळ) \
  (ISDIGIT((ओळ)[0]) && ISDIGIT((ओळ)[1]) && ISDIGIT((ओळ)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(ओळ) (STATUSCODE(ओळ) && (' ' == (ओळ)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
स्थिर स्थिरांक अक्षर * स्थिरांक ftp_state_names[] = {
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
स्थिर शून्य ftp_state_खालचा(संरचना Curl_easy *data,
                          संरचना ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , पूर्णांक lineno
#endif
  )
{
  जर(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((शून्य)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#अन्यथा
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_खालचा(x, y, z)
#अन्यथा /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_खालचा(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

स्थिर CURLcode ftp_state_mdtm(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp);
स्थिर CURLcode ftp_state_quote(संरचना Curl_easy *data,
                                संरचना ftp_conn *ftpc,
                                संरचना FTP *ftp,
                                bool init, ftpstate instate);
स्थिर CURLcode ftp_nb_type(संरचना Curl_easy *data,
                            संरचना ftp_conn *ftpc,
                            संरचना FTP *ftp,
                            bool ascii, ftpstate newstate);
स्थिर CURLcode getftpresponse(संरचना Curl_easy *data, माप_प्रकार *nreadp,
                               पूर्णांक *ftpcodep);

स्थिर शून्य freedirs(संरचना ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = रिक्त;
}

स्थिर माप_प्रकार numof_slashes(स्थिरांक अक्षर *str)
{
  स्थिरांक अक्षर *slashPos;
  माप_प्रकार अंक = 0;
  करा {
    slashPos = म_अक्षर(str, '/');
    जर(slashPos) {
      ++अंक;
      str = slashPos + 1;
    }
  } पर्यंत(slashPos);
  परत अंक;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
स्थिर CURLcode ftp_parse_url_path(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp)
{
  स्थिरांक अक्षर *slashPos = रिक्त;
  स्थिरांक अक्षर *fileName = रिक्त;
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *rawPath = रिक्त; /* URL-decoded "raw" path */
  माप_प्रकार pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  जर(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  जर(result) {
    failf(data, "path contains control characters");
    परत result;
  }
  rawPath = ftpc->rawpath;

  निवड(data->set.ftp_filemethod) {
  स्थिती FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    जर((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    थांबा;

  स्थिती FTPFILE_SINGLECWD:
    slashPos = म_खोजप(rawPath, '/');
    जर(slashPos) {
      /* get path before last slash, except for / */
      माप_प्रकार dirlen = slashPos - rawPath;
      जर(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, आकार(ftpc->dirs[0]));
      जर(!ftpc->dirs)
        परत CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (पूर्णांक)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    अन्यथा
      fileName = rawPath; /* filename only (or empty) */
    थांबा;

  पूर्वनिर्धारित: /* allow pretty much anything */
  स्थिती FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    स्थिरांक अक्षर *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    माप_प्रकार dirAlloc = numof_slashes(rawPath);

    जर(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      परत CURLE_URL_MALFORMAT;

    जर(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, आकार(ftpc->dirs[0]));
      जर(!ftpc->dirs)
        परत CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      पर्यंत(dirAlloc--) {
        स्थिरांक अक्षर *spos = म_अक्षर(curPos, '/');
        माप_प्रकार clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        जर(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        जर(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (पूर्णांक)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (पूर्णांक)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    थांबा;
  } /* switch */

  जर(fileName && *fileName)
    ftpc->file = fileName;
  अन्यथा
    ftpc->file = रिक्त; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  जर(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    परत CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  जर((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  अन्यथा { /* newly created FTP connections are already in entry path */
    स्थिरांक अक्षर *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    जर(oldPath) {
      माप_प्रकार n = pathLen;
      जर(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      अन्यथा
        n -= ftpc->file ? म_माप(ftpc->file) : 0;

      जर((म_माप(oldPath) == n) && rawPath && !म_भेदन(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  परत CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
स्थिर पूर्णांक ftp_need_type(संरचना ftp_conn *ftpc,
                         bool ascii_wanted)
{
  परत ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

स्थिर शून्य close_secondarysocket(संरचना Curl_easy *data,
                                  संरचना ftp_conn *ftpc)
{
  (शून्य)ftpc;
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
संरचना ftp_cw_lc_ctx {
  संरचना Curl_cwriter super;
  bool newline_pending;
};

स्थिर CURLcode ftp_cw_lc_write(संरचना Curl_easy *data,
                                संरचना Curl_cwriter *writer, पूर्णांक type,
                                स्थिरांक अक्षर *buf, माप_प्रकार blen)
{
  स्थिर स्थिरांक अक्षर nl = '\n';
  संरचना ftp_cw_lc_ctx *ctx = writer->ctx;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  जर(!ftpc)
    परत CURLE_FAILED_INIT;

  जर(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    परत Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  पर्यंत(blen) {
    /* do not pass EOS when writing parts */
    पूर्णांक chunk_type = (type & ~CLIENTWRITE_EOS);
    स्थिरांक अक्षर *cp;
    माप_प्रकार chunk_len;
    CURLcode result;

    जर(ctx->newline_pending) {
      जर(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        जर(result)
          परत result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = स_प्रथम(buf, '\r', blen);
    जर(!cp)
      थांबा;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    जर(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      जर(result)
        परत result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  जर(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    परत Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  अन्यथा जर(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    जर(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      परत Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    परत Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  परत CURLE_OK;
}

स्थिर स्थिरांक संरचना Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  आकार(संरचना ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
स्थिर CURLcode ftp_check_ctrl_on_data_wait(संरचना Curl_easy *data,
                                            संरचना ftp_conn *ftpc)
{
  संरचना connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  संरचना pingpong *pp = &ftpc->pp;
  माप_प्रकार nread;
  पूर्णांक ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  जर(curlx_dyn_len(&pp->recvbuf)) {
    स्थिरांक अक्षर *l = curlx_dyn_ptr(&pp->recvbuf);
    जर(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (शून्य)getftpresponse(data, &nread, &ftpcode);
      परत CURLE_FTP_ACCEPT_FAILED;
    }
  }

  जर(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  अन्यथा {
    पूर्णांक socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    निवड(socketstate) {
    स्थिती -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      परत CURLE_FTP_ACCEPT_FAILED;
    पूर्वनिर्धारित:
      जर(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      थांबा;
    }
  }

  जर(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    जर(pp->overflow > 3) {
      स्थिरांक अक्षर *r = curlx_dyn_ptr(&pp->recvbuf);
      माप_प्रकार len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      जर((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        जर(!curlx_str_संख्या(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          परत CURLE_OK;
        }
      }
    }

    (शून्य)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    जर(ftpcode / 100 > 3)
      परत CURLE_FTP_ACCEPT_FAILED;

    परत CURLE_WEIRD_SERVER_REPLY;
  }

  परत CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
स्थिर CURLcode ftp_initiate_transfer(संरचना Curl_easy *data,
                                      संरचना ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  जर(result || !connected)
    परत result;

  जर(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  अन्यथा {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  परत CURLE_OK;
}

स्थिर bool ftp_endofresp(संरचना Curl_easy *data, संरचना connectdata *conn,
                          स्थिरांक अक्षर *ओळ, माप_प्रकार len, पूर्णांक *code)
{
  curl_off_t status;
  (शून्य)data;
  (शून्य)conn;

  जर((len > 3) && LASTLINE(ओळ) && !curlx_str_संख्या(&ओळ, &status, 999)) {
    *code = (पूर्णांक)status;
    परत TRUE;
  }

  परत FALSE;
}

स्थिर CURLcode ftp_readresp(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             int8_t sockindex,
                             संरचना pingpong *pp,
                             पूर्णांक *ftpcodep, /* return the ftp-code if done */
                             माप_प्रकार *size) /* size of the response */
{
  पूर्णांक code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  जर(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  जर(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    परत CURLE_OPERATION_TIMEDOUT;
  }

  परत result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
स्थिर CURLcode getftpresponse(संरचना Curl_easy *data,
                               माप_प्रकार *nreadp, /* return number of bytes
                                                  read */
                               पूर्णांक *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  संरचना connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना pingpong *pp = &ftpc->pp;
  माप_प्रकार nread;
  पूर्णांक cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  जर(!ftpc)
    परत CURLE_FAILED_INIT;

  पर्यंत(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    जर(timeout <= 0) {
      failf(data, "FTP response timeout");
      परत CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    जर(timeout < interval_ms)
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

    जर(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    अन्यथा जर(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      पूर्णांक ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      जर(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        परत CURLE_RECV_ERROR;
      }
      अन्यथा जर(ev == 0) {
        result = Curl_pgrsUpdate(data);
        सुरूठेवा; /* continue in our loop for the timeout duration */
      }
    }

    जर(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      जर(result)
        थांबा;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    जर(result)
      थांबा;

    जर(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    अन्यथा
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (पूर्णांक)result, *nreadp, *ftpcodep);

  परत result;
}

स्थिर CURLcode ftp_state_user(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  जर(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  परत result;
}

स्थिर CURLcode ftp_state_pwd(संरचना Curl_easy *data,
                              संरचना ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  जर(!data->id && दो_पर्या("CURL_FTP_PWD_STOP"))
    परत CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  जर(!result)
    ftp_state(data, ftpc, FTP_PWD);

  परत result;
}

/* For the FTP "protocol connect" and "doing" phases only */
स्थिर CURLcode ftp_pollset(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  परत ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
स्थिर CURLcode ftp_domore_pollset(संरचना Curl_easy *data,
                                   संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  जर(!ftpc)
    परत CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  जर(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    परत Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  परत Curl_pp_pollset(data, &ftpc->pp, ps);
}

स्थिर पूर्णांक pathlen(संरचना ftp_conn *ftpc, पूर्णांक अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  परत ftpc->dirs[अंक].len;
}

स्थिर स्थिरांक अक्षर *pathpiece(संरचना ftp_conn *ftpc, पूर्णांक अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  परत &ftpc->rawpath[ftpc->dirs[अंक].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
स्थिर CURLcode ftp_state_cwd(संरचना Curl_easy *data,
                              संरचना ftp_conn *ftpc,
                              संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  जर(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  अन्यथा {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    जर(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      जर(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    अन्यथा {
      जर(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        जर(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      अन्यथा {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  परत result;
}

प्रकारव्याख्या गणना {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
स्थिर CURLcode ftp_port_parse_string(संरचना Curl_easy *data,
                                      संरचना connectdata *conn,
                                      स्थिरांक अक्षर *string_ftpport,
                                      संरचना Curl_sockaddr_storage *ss,
                                      अचिन्हित लघु *port_minp,
                                      अचिन्हित लघु *port_maxp,
                                      स्थिरांक अक्षर **hostp,
                                      अक्षर *hbuf, माप_प्रकार hbuflen)
{
  स्थिरांक अक्षर *ip_end = रिक्त;
  स्थिरांक अक्षर *addr = रिक्त;
  माप_प्रकार addrlen = 0;
  अचिन्हित लघु port_min = 0;
  अचिन्हित लघु port_max = 0;
  अक्षर ipstr[50];
#ifndef USE_IPV6
  (शून्य)conn;
  (शून्य)ss;
#endif

  /* default to nothing */
  *hostp = रिक्त;
  *port_minp = *port_maxp = 0;

  जर(!string_ftpport || (म_माप(string_ftpport) <= 1))
    जा done;

#ifdef USE_IPV6
  जर(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    स्थिरांक अक्षर *ip_start = string_ftpport + 1;
    ip_end = म_अक्षर(ip_start, ']');
    जर(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  अन्यथा
#endif
    जर(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    अन्यथा {
      ip_end = म_अक्षर(string_ftpport, ':');
      addr = string_ftpport;
      जर(ip_end) {
#ifdef USE_IPV6
        संरचना sockaddr_in6 * स्थिरांक sa6 = (शून्य *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        जर(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = म_माप(string_ftpport);
          ip_end = रिक्त; /* this got no port ! */
        }
#endif
      }
      अन्यथा
        /* ipv4|interface */
        addrlen = म_माप(string_ftpport);
    }

  /* parse the port */
  जर(ip_end) {
    स्थिरांक अक्षर *portp = म_अक्षर(ip_end, ':');
    जर(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      जर(!curlx_str_संख्या(&portp, &start, 0xffff)) {
        port_min = (अचिन्हित लघु)start;
        जर(!curlx_str_single(&portp, '-') &&
           !curlx_str_संख्या(&portp, &end, 0xffff))
          port_max = (अचिन्हित लघु)end;
        अन्यथा
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  जर(port_min > port_max)
    port_min = port_max = 0;

  जर(addrlen) {
    स्थिरांक संरचना Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    जर(!remote_addr || (addrlen >= आकार(ipstr)) || (addrlen >= hbuflen))
      परत CURLE_FTP_PORT_FAILED;
    स_नकल(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    निवड(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    स्थिती IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      स_नकल(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      थांबा;
    स्थिती IF2IP_AF_NOT_SUPPORTED:
      परत CURLE_FTP_PORT_FAILED;
    स्थिती IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      थांबा;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  परत CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
स्थिर CURLcode ftp_port_default_host(संरचना Curl_easy *data,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      स्थिरांक अक्षर **hostp,
                                      अक्षर *hbuf, माप_प्रकार hbuflen,
                                      bool *non_localp)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * स्थिरांक sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * स्थिरांक sa6 = (शून्य *)sa;
#endif
  अक्षर buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = आकार(*ss);
  जर(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
    परत CURLE_FTP_PORT_FAILED;
  }
  निवड(sa->sa_family) {
#ifdef USE_IPV6
  स्थिती AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    थांबा;
#endif
  पूर्वनिर्धारित:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    थांबा;
  }
  जर(result)
    परत CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  परत CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
स्थिर CURLcode ftp_port_resolve_host(संरचना Curl_easy *data,
                                      संरचना connectdata *conn,
                                      स्थिरांक अक्षर *host,
                                      संरचना Curl_dns_entry **dns_entryp,
                                      स्थिरांक संरचना Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = रिक्त;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  जर(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  अन्यथा {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  परत result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
स्थिर CURLcode ftp_port_open_socket(संरचना Curl_easy *data,
                                     संरचना connectdata *conn,
                                     स्थिरांक संरचना Curl_addrinfo *res,
                                     स्थिरांक संरचना Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  अक्षर buffer[STRERROR_LEN];
  पूर्णांक sockerr = 0;
  स्थिरांक संरचना Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  साठी(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, रिक्त,
                       Curl_conn_get_transport(data, conn), portsockp);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        परत result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      सुरूठेवा;
    }
    थांबा;
  }
  जर(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, आकार(buffer)));
    परत CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  परत result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
स्थिर CURLcode ftp_port_bind_socket(संरचना Curl_easy *data,
                                     संरचना connectdata *conn,
                                     curl_socket_t portsock,
                                     स्थिरांक संरचना Curl_addrinfo *ai,
                                     संरचना Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     अचिन्हित लघु port_min,
                                     अचिन्हित लघु port_max,
                                     bool non_local)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * स्थिरांक sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * स्थिरांक sa6 = (शून्य *)sa;
#endif
  अक्षर buffer[STRERROR_LEN];
  अचिन्हित लघु port;
  पूर्णांक sockerr;

  स_नकल(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  साठी(port = port_min; port <= port_max;) {
    जर(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    अन्यथा
      sa6->sin6_port = htons(port);
#endif
    जर(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      जर(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, आकार(buffer)));

        *sslen_io = आकार(*ss);
        जर(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
          परत CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        सुरूठेवा;
      }
      जर(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, आकार(buffer)));
        परत CURLE_FTP_PORT_FAILED;
      }
    }
    अन्यथा
      थांबा;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    जर(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      परत CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = आकार(*ss);
  जर(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
    परत CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  परत CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
स्थिर CURLcode ftp_port_listen(संरचना Curl_easy *data, curl_socket_t portsock)
{
  अक्षर buffer[STRERROR_LEN];

  जर(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
    परत CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  परत CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
स्थिर CURLcode ftp_port_send_command(संरचना Curl_easy *data,
                                      संरचना ftp_conn *ftpc,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      स्थिरांक संरचना Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  स्थिर स्थिरांक अक्षर mode[][5] = { "EPRT", "PORT" };
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * स्थिरांक sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * स्थिरांक sa6 = (शून्य *)sa;
#endif
  अक्षर myhost[MAX_IPADR_LEN + 1] = "";
  अचिन्हित लघु port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, आकार(myhost));

#ifdef USE_IPV6
  जर(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  साठी(; fcmd != DONE; fcmd++) {

    जर(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      सुरूठेवा;

    जर((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      सुरूठेवा;

    निवड(sa->sa_family) {
    स्थिती AF_INET:
      port = ntohs(sa4->sin_port);
      थांबा;
#ifdef USE_IPV6
    स्थिती AF_INET6:
      port = ntohs(sa6->sin6_port);
      थांबा;
#endif
    पूर्वनिर्धारित:
      सुरूठेवा; /* might as well skip this */
    }

    जर(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      जर(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        परत result;
      }
      थांबा;
    }
    जर(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      अक्षर target[आकार(myhost) + 20];
      स्थिरांक अक्षर *source = myhost;
      अक्षर *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      पर्यंत(*source) {
        जर(*source == '.')
          *dest = ',';
        अन्यथा
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (पूर्णांक)(port >> 8), (पूर्णांक)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      जर(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        परत result;
      }
      थांबा;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  परत CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
स्थिर CURLcode ftp_state_use_port(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  संरचना connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  संरचना Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  अक्षर hbuf[NI_MAXHOST];
  स्थिरांक अक्षर *host = रिक्त;
  स्थिरांक अक्षर *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  संरचना Curl_dns_entry *dns_entry = रिक्त;
  स्थिरांक संरचना Curl_addrinfo *res = रिक्त;
  स्थिरांक संरचना Curl_addrinfo *ai = रिक्त;
  अचिन्हित लघु port_min = 0;
  अचिन्हित लघु port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, आकार(hbuf));
  जर(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, आकार(hbuf), &non_local);

  /* resolve host string to address list */
  जर(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  जर(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  जर(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  जर(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  जर(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  जर(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  जर(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  जर(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  जर(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  अन्यथा {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    जर(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  जर(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  परत result;
}

स्थिर CURLcode ftp_state_use_pasv(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना connectdata *conn)
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

  स्थिर स्थिरांक अक्षर mode[][5] = { "EPSV", "PASV" };
  पूर्णांक modeoff;

#ifdef PF_INET6
  जर(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  जर(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  परत result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
स्थिर CURLcode ftp_state_prepare_transfer(संरचना Curl_easy *data,
                                           संरचना ftp_conn *ftpc,
                                           संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;

  जर(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  अन्यथा जर(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  अन्यथा {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    जर(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      जर(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      अन्यथा जर(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      अन्यथा
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      जर(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    अन्यथा
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  परत result;
}

स्थिर CURLcode ftp_state_rest(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  जर((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    जर(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  अन्यथा
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  परत result;
}

स्थिर CURLcode ftp_state_size(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  जर((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    जर(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  अन्यथा
    result = ftp_state_rest(data, ftpc, ftp);

  परत result;
}

स्थिर CURLcode ftp_state_list(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
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
  स्थिरांक अक्षर *lstArg = रिक्त;
  पूर्णांक lstArglen = 0;
  अक्षर *cmd;

  जर((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    स्थिरांक अक्षर *rawPath = ftpc->rawpath;
    स्थिरांक अक्षर *slashPos = म_खोजप(rawPath, '/');
    जर(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      माप_प्रकार n = slashPos - rawPath;
      जर(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (पूर्णांक)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  जर(!cmd)
    परत CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  जर(!result)
    ftp_state(data, ftpc, FTP_LIST);

  परत result;
}

स्थिर CURLcode ftp_state_list_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  परत ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

स्थिर CURLcode ftp_state_retr_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  परत ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

स्थिर CURLcode ftp_state_stor_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  परत ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

स्थिर CURLcode ftp_state_type(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  जर(data->req.no_body && ftpc->file &&
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
    जर(result)
      परत result;
  }
  अन्यथा
    result = ftp_state_size(data, ftpc, ftp);

  परत result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
स्थिर CURLcode ftp_state_mdtm(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  जर((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    जर(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  अन्यथा
    result = ftp_state_type(data, ftpc, ftp);

  परत result;
}

/* This is called after the TYPE and possible quote commands have been sent */
स्थिर CURLcode ftp_state_ul_setup(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  जर((data->state.resume_from && !sizechecked) ||
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
    पूर्णांक seekerr = CURL_SEEKFUNC_OK;

    जर(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      जर(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      परत result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    जर(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    जर(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      जर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        परत CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करा {
        अक्षर scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)आकार(scratch)) ?
          आकार(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        माप_प्रकार actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        जर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          परत CURLE_FTP_COULDNT_USE_REST;
        }
      } पर्यंत(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    जर(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      जर(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        परत CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  जर(!result)
    ftp_state(data, ftpc, FTP_STOR);

  परत result;
}

स्थिर CURLcode ftp_state_retr(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  जर(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    परत CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  जर(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    जर(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    अन्यथा {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      जर(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        जर(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          परत CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      अन्यथा {
        जर(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          परत CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    जर(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      परत CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    जर(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  अन्यथा {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    जर(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  परत result;
}

स्थिर CURLcode ftp_state_quote(संरचना Curl_easy *data,
                                संरचना ftp_conn *ftpc,
                                संरचना FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  संरचना curl_slist *item;

  निवड(instate) {
  स्थिती FTP_QUOTE:
  पूर्वनिर्धारित:
    item = data->set.quote;
    थांबा;
  स्थिती FTP_RETR_PREQUOTE:
  स्थिती FTP_STOR_PREQUOTE:
  स्थिती FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    थांबा;
  स्थिती FTP_POSTQUOTE:
    item = data->set.postquote;
    थांबा;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  जर(init)
    ftpc->count1 = 0;
  अन्यथा
    ftpc->count1++;

  जर(item) {
    पूर्णांक i = 0;

    /* Skip count1 items in the linked list */
    पर्यंत((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    जर(item) {
      स्थिरांक अक्षर *cmd = item->data;
      जर(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      अन्यथा
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      जर(result)
        परत result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  जर(!quote) {
    /* No more quote to send, continue to ... */
    निवड(instate) {
    स्थिती FTP_QUOTE:
    पूर्वनिर्धारित:
      result = ftp_state_cwd(data, ftpc, ftp);
      थांबा;
    स्थिती FTP_RETR_PREQUOTE:
      जर(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      अन्यथा {
        जर(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        अन्यथा {
          जर(data->set.ignorecl || data->state.prefer_ascii) {
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
            जर(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          अन्यथा {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            जर(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      थांबा;
    स्थिती FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      थांबा;
    स्थिती FTP_POSTQUOTE:
      थांबा;
    स्थिती FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      थांबा;
    }
  }

  परत result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
स्थिर CURLcode ftp_epsv_disable(संरचना Curl_easy *data,
                                 संरचना ftp_conn *ftpc,
                                 संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;

  जर(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  जर(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  परत result;
}

स्थिर CURLcode ftp_control_addr_dup(संरचना Curl_easy *data, अक्षर **newhostp)
{
  संरचना connectdata *conn = data->conn;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  जर(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  अन्यथा
#endif
  जर(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  अन्यथा {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = रिक्त;
    परत CURLE_FTP_CANT_GET_HOST;
  }
  परत *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर bool match_pasv_6nums(स्थिरांक अक्षर *p,
                             अचिन्हित पूर्णांक *array) /* 6 numbers */
{
  पूर्णांक i;
  साठी(i = 0; i < 6; i++) {
    curl_off_t अंक;
    जर(i) {
      जर(*p != ',')
        परत FALSE;
      p++;
    }
    जर(curlx_str_संख्या(&p, &अंक, 0xff))
      परत FALSE;
    array[i] = (अचिन्हित पूर्णांक)अंक;
  }
  परत TRUE;
}

स्थिर CURLcode ftp_state_pasv_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result;
  स्थिरांक संरचना pingpong *pp = &ftpc->pp;
  अक्षर *newhost = रिक्त;
  अचिन्हित लघु newport = 0;
  स्थिरांक अक्षर *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  जर((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    स्थिरांक अक्षर *ptr = म_अक्षर(str, '(');
    जर(ptr) {
      अक्षर sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      जर(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        स्थिरांक अक्षर *p = &ptr[3];
        curl_off_t अंक;
        जर(curlx_str_संख्या(&p, &अंक, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          परत CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (अचिन्हित लघु)अंक;
        result = ftp_control_addr_dup(data, &newhost);
        जर(result)
          परत result;
      }
      अन्यथा
        ptr = रिक्त;
    }
    जर(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      परत CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  अन्यथा जर((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    अचिन्हित पूर्णांक ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    पर्यंत(*str) {
      जर(match_pasv_6nums(str, ip))
        थांबा;
      str++;
    }

    जर(!*str) {
      failf(data, "Could not interpret the 227-response");
      परत CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    जर(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      जर(result)
        परत result;
    }
    अन्यथा
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    जर(!newhost)
      परत CURLE_OUT_OF_MEMORY;

    newport = (अचिन्हित लघु)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  अन्यथा जर(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    परत ftp_epsv_disable(data, ftpc, conn);
  }
  अन्यथा {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    परत CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  जर(result)
    जा error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  जर(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    जर(result)
      जा error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  जर(result) {
    जर((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    जा error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  जर(data->set.verbose) {
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
  परत result;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ftp_statemach(संरचना Curl_easy *data,
                              संरचना ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  परत result;
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
स्थिर CURLcode ftp_do_more(संरचना Curl_easy *data, domore *more)
{
  संरचना connectdata *conn = data->conn;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  जर(!ftpc || !ftp)
    परत CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  जर(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    जर(result == CURLE_OUT_OF_MEMORY)
      परत result;
    जर(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      जर(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        परत ftp_epsv_disable(data, ftpc, conn);
      }
      परत result;
    }
  }

  जर(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    जर(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    जर(result || !ftpc->wait_data_conn)
      परत result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  जर(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    जर(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      जर(result)
        परत result; /* Failed to accept data connection */

      जर(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        जर(result)
          परत result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      अन्यथा {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        जर(result)
          परत result;
      }
    }
    अन्यथा जर(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      जर(result)
        परत result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    अन्यथा {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      जर(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      जर(result)
        परत result;

      जर((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        जर(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          जर(result)
            परत result;
        }
        /* otherwise fall through */
      }
      अन्यथा {
        जर(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        अन्यथा {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        जर(result)
          परत result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      जर(complete)
        *more = DOMORE_DONE;
    }
    परत result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  जर(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (पूर्णांक)result);
  }

  परत result;
}

/* call this when the DO phase has completed */
स्थिर CURLcode ftp_dophase_done(संरचना Curl_easy *data,
                                 संरचना ftp_conn *ftpc,
                                 संरचना FTP *ftp,
                                 bool connected)
{
  जर(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    जर(result) {
      close_secondarysocket(data, ftpc);
      परत result;
    }
  }

  जर(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  अन्यथा जर(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  परत CURLE_OK;
}

स्थिर CURLcode ftp_state_port_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णांक ftpcode)
{
  संरचना connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  जर(ftpcode / 100 != 2) {
    /* the command failed */

    जर(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    जर(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    अन्यथा
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  अन्यथा {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  परत result;
}

/* return TRUE on error, FALSE on success */
स्थिर bool twodigit(स्थिरांक अक्षर *p, पूर्णांक *val)
{
  जर(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    परत TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  परत FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(स्थिरांक अक्षर *p, पूर्णांक *year, पूर्णांक *month, पूर्णांक *day,
                           पूर्णांक *hour, पूर्णांक *minute, पूर्णांक *second);
UNITTEST bool ftp_213_date(स्थिरांक अक्षर *p, पूर्णांक *year, पूर्णांक *month, पूर्णांक *day,
                           पूर्णांक *hour, पूर्णांक *minute, पूर्णांक *second)
{
  पूर्णांक century;
  जर((म_माप(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    परत FALSE;

  *year += century * 100;
  जर((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    परत FALSE;
  परत TRUE;
}

स्थिर CURLcode client_write_header(संरचना Curl_easy *data,
                                    अक्षर *buf, माप_प्रकार blen)
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
  परत result;
}

स्थिर CURLcode ftp_state_mdtm_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णांक ftpcode)
{
  CURLcode result = CURLE_OK;

  निवड(ftpcode) {
  स्थिती 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    पूर्णांक year, month, day, hour, minute, second;
    संरचना pingpong *pp = &ftpc->pp;
    स्थिरांक अक्षर *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    जर(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      अक्षर timebuf[24];
      curl_msnprintf(timebuf, आकार(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      जर(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#जर defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ढकला
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    जर(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#जर defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic काढा
#endif
      अक्षर headerbuf[128];
      पूर्णांक headerbuflen;
      समय_प्रकार filetime = data->info.filetime;
      संरचना पंचांग buffer;
      स्थिरांक संरचना पंचांग *पंचांग = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      जर(result)
        परत result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, आकार(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday-1 : 6],
                       पंचांग->tm_mday,
                       Curl_month[पंचांग->tm_mon],
                       पंचांग->tm_year + 1900,
                       पंचांग->tm_hour,
                       पंचांग->tm_min,
                       पंचांग->tm_sec);
      result = client_write_header(data, headerbuf, headerbuflen);
      जर(result)
        परत result;
    } /* end of a ridiculous amount of conditionals */
  }
    थांबा;
  पूर्वनिर्धारित:
    infof(data, "unsupported MDTM reply format");
    थांबा;
  स्थिती 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    थांबा;
  }

  जर(data->set.timecondition) {
    जर((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      निवड(data->set.timecondition) {
      स्थिती CURL_TIMECOND_IFMODSINCE:
      पूर्वनिर्धारित:
        जर(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          परत CURLE_OK;
        }
        थांबा;
      स्थिती CURL_TIMECOND_IFUNMODSINCE:
        जर(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          परत CURLE_OK;
        }
        थांबा;
      } /* switch */
    }
    अन्यथा {
      infof(data, "Skipping time comparison");
    }
  }

  जर(!result)
    result = ftp_state_type(data, ftpc, ftp);

  परत result;
}

स्थिर CURLcode ftp_state_type_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  जर(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    परत CURLE_FTP_COULDNT_SET_TYPE;
  }
  जर(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  जर(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  अन्यथा जर(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  अन्यथा जर(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  अन्यथा जर(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  अन्यथा जर(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  परत result;
}

स्थिर CURLcode ftp_state_size_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  स्थिरांक अक्षर *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  माप_प्रकार len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  जर(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    स्थिरांक अक्षर *start = &buf[4];
    स्थिरांक अक्षर *fdigit = स_प्रथम(start, '\r', len - 4);
    जर(fdigit) {
      fdigit--;
      जर(*fdigit == '\n')
        fdigit--;
      पर्यंत(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    अन्यथा
      fdigit = start;
    जर(curlx_str_संख्या(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  अन्यथा जर(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    परत CURLE_REMOTE_FILE_NOT_FOUND;
  }

  जर(instate == FTP_SIZE) {
    जर(filesize != -1) {
      अक्षर clbuf[128];
      पूर्णांक clbuflen = curl_msnprintf(clbuf, आकार(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      जर(result)
        परत result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  अन्यथा जर(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  अन्यथा जर(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  परत result;
}

स्थिर CURLcode ftp_state_rest_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  निवड(instate) {
  स्थिती FTP_REST:
  पूर्वनिर्धारित:
    जर(ftpcode == 350) {
      अक्षर buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, म_माप(buffer));
      जर(result)
        परत result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    थांबा;

  स्थिती FTP_RETR_REST:
    जर(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    अन्यथा {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      जर(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    थांबा;
  }

  परत result;
}

स्थिर CURLcode ftp_state_stor_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode result = CURLE_OK;

  जर(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    परत CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  जर(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    जर(result)
      परत result;

    जर(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      परत ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  परत ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
स्थिर CURLcode ftp_state_get_resp(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   पूर्णांक ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  जर((ftpcode == 150) || (ftpcode == 125)) {

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

    जर((instate != FTP_LIST) &&
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
      माप_प्रकार len = curlx_dyn_len(&ftpc->pp.recvbuf);
      जर(len >= 7) { /* "1 bytes" is 7 characters */
        माप_प्रकार i;
        साठी(i = 0; i < len - 7; i++) {
          curl_off_t what;
          स्थिरांक अक्षर *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          स्थिरांक अक्षर *c = &buf[i];
          जर(!curlx_str_संख्या(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !म_भेदन(c, "bytes", 5)) {
            data->req.size = what;
            थांबा;
          }
        }
      }
    }
    अन्यथा जर(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    जर(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    अन्यथा जर((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    जर(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    जर(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      जर(result)
        परत result;

      जर(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        परत ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    परत ftp_initiate_transfer(data, ftpc);
  }
  अन्यथा {
    जर((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    अन्यथा {
      failf(data, "RETR response: %03d", ftpcode);
      परत instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  परत result;
}

/* after USER, PASS and ACCT */
स्थिर CURLcode ftp_state_loggedin(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  जर(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    जर(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  अन्यथा {
    result = ftp_state_pwd(data, ftpc);
  }
  परत result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
स्थिर bool ftp_has_ctrl(स्थिरांक अक्षर *string)
{
  स्थिरांक अचिन्हित अक्षर *s = (स्थिरांक अचिन्हित अक्षर *)string;
  पर्यंत(*s) {
    जर(*s < 0x20)
      परत TRUE;
    s++;
  }
  परत FALSE;
}

/* for USER and PASS responses */
स्थिर CURLcode ftp_state_user_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  जर((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    जर(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  अन्यथा जर(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  अन्यथा जर(ftpcode == 332) {
    स्थिरांक अक्षर *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    जर(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    अन्यथा जर(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अन्यथा {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      जर(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  अन्यथा {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    स्थिरांक अक्षर *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    जर(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      जर(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      अन्यथा {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        जर(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    अन्यथा {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  परत result;
}

/* for ACCT response */
स्थिर CURLcode ftp_state_acct_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode result = CURLE_OK;
  जर(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  अन्यथा
    result = ftp_state_loggedin(data, ftpc);

  परत result;
}

स्थिर CURLcode ftp_pwd_resp(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             पूर्णांक ftpcode)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode result;

  जर(ftpcode == 257) {
    स्थिरांक अक्षर *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    संरचना dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    पर्यंत(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    जर('\"' == *ptr) {
      /* it started good */
      साठी(ptr++; *ptr; ptr++) {
        जर('\"' == *ptr) {
          जर('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          अन्यथा {
            /* end of path */
            जर(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            थांबा; /* get out of this loop */
          }
        }
        अन्यथा {
          जर(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            परत CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        जर(result) {
          curlx_dyn_free(&out);
          परत result;
        }
      }
    }
    जर(entry_extracted) {
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
      अक्षर *dir = curlx_dyn_ptr(&out);

      जर(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        जर(result) {
          curlx_dyn_free(&out);
          परत result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      जर(!data->state.most_recent_ftp_entrypath)
        परत CURLE_OUT_OF_MEMORY;

      जर(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        परत CURLE_OK;
      }
    }
    अन्यथा {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  परत CURLE_OK;
}

स्थिर स्थिरांक अक्षर * स्थिरांक ftpauth[] = { "SSL", "TLS" };

स्थिर CURLcode ftp_wait_resp(संरचना Curl_easy *data,
                              संरचना connectdata *conn,
                              संरचना ftp_conn *ftpc,
                              पूर्णांक ftpcode)
{
  CURLcode result = CURLE_OK;
  जर(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    जर(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      परत ftp_state_user_resp(data, ftpc, ftpcode);
  }
  अन्यथा जर(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  जर(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    निवड((दीर्घ)data->set.ftpsslauth) {
    स्थिती CURLFTPAUTH_DEFAULT:
    स्थिती CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      थांबा;
    स्थिती CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      थांबा;
    पूर्वनिर्धारित:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (पूर्णांक)data->set.ftpsslauth);
      परत CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    जर(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  अन्यथा
    result = ftp_state_user(data, ftpc, conn);
  परत result;
}

/* an implicit TLS scheme implies TLS on both connections */
स्थिर अचिन्हित अक्षर ftp_use_ssl(संरचना connectdata *conn,
                                 संरचना ftp_conn *ftpc)
{
  जर(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    परत CURLUSESSL_ALL;
  परत ftpc->use_ssl;
}

स्थिर CURLcode ftp_pp_statemachine(संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  CURLcode result;
  पूर्णांक ftpcode;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  संरचना pingpong *pp;
  माप_प्रकार nread = 0;

  जर(!ftpc || !ftp)
    परत CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  जर(pp->sendleft)
    परत Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  जर(result || !ftpcode)
    परत result;

  /* we have now received a full FTP server response */
  निवड(ftpc->state) {
  स्थिती FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    जर(pp->overflow)
      परत CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    जर((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      जर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        जर(result) {
          /* we failed and bail out */
          परत CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      जर(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    अन्यथा जर(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    अन्यथा {
      जर(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      अन्यथा
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    थांबा;

  स्थिती FTP_USER:
  स्थिती FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    जर(!result)
      ftp_state(data, ftpc, FTP_PROT);
    थांबा;

  स्थिती FTP_PROT:
    जर(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    अन्यथा जर(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      परत CURLE_USE_SSL_FAILED;

    जर(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      जर(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    अन्यथा
      result = ftp_state_pwd(data, ftpc);
    थांबा;

  स्थिती FTP_CCC:
    जर(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (अचिन्हित अक्षर)CURLFTPSSL_CCC_ACTIVE));
      जर(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    जर(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    थांबा;

  स्थिती FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_SYST:
    जर(ftpcode == 215) {
      स्थिरांक अक्षर *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      स्थिरांक अक्षर *start;
      अक्षर *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      पर्यंत(*ptr == ' ')
        ptr++;
      साठी(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      जर(!os)
        परत CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      जर(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        जर(result) {
          curlx_free(os);
          परत result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        थांबा;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    अन्यथा {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    थांबा;

  स्थिती FTP_NAMEFMT:
    जर(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      थांबा;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    थांबा;

  स्थिती FTP_QUOTE:
  स्थिती FTP_POSTQUOTE:
  स्थिती FTP_RETR_PREQUOTE:
  स्थिती FTP_STOR_PREQUOTE:
  स्थिती FTP_LIST_PREQUOTE:
    जर((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    अन्यथा
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    थांबा;

  स्थिती FTP_CWD:
    जर(ftpcode / 100 != 2) {
      /* failure to CWD there */
      जर(data->set.ftp_create_missing_dirs &&
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
        जर(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      अन्यथा {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    अन्यथा {
      /* success */
      ftpc->count2 = 0;
      जर(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      अन्यथा {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    थांबा;

  स्थिती FTP_MKD:
    जर((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    अन्यथा {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    थांबा;

  स्थिती FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    थांबा;

  स्थिती FTP_TYPE:
  स्थिती FTP_LIST_TYPE:
  स्थिती FTP_RETR_TYPE:
  स्थिती FTP_STOR_TYPE:
  स्थिती FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    थांबा;

  स्थिती FTP_SIZE:
  स्थिती FTP_RETR_SIZE:
  स्थिती FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    थांबा;

  स्थिती FTP_REST:
  स्थिती FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    थांबा;

  स्थिती FTP_PRET:
    जर(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      परत CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    थांबा;

  स्थिती FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    थांबा;

  स्थिती FTP_LIST:
  स्थिती FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    थांबा;

  स्थिती FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    थांबा;

  स्थिती FTP_QUIT:
  पूर्वनिर्धारित:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    थांबा;
  }

  परत result;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ftp_multi_statemach(संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  परत ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

स्थिर CURLcode ftp_block_statemach(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  पर्यंत(ftpc->state != FTP_STOP) {
    जर(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    जर(result)
      थांबा;
  }

  परत result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
स्थिर CURLcode ftp_connect(संरचना Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना pingpong *pp;

  *done = FALSE; /* default to not done yet */
  जर(!ftpc)
    परत CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  जर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    जर(result)
      परत result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  परत result;
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
स्थिर CURLcode ftp_sendquote(संरचना Curl_easy *data,
                              संरचना ftp_conn *ftpc,
                              संरचना curl_slist *quote)
{
  संरचना curl_slist *item;
  संरचना pingpong *pp = &ftpc->pp;

  item = quote;
  पर्यंत(item) {
    जर(item->data) {
      माप_प्रकार nread;
      स्थिरांक अक्षर *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      पूर्णांक ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      जर(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      जर(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      जर(result)
        परत result;

      जर(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        परत CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  परत CURLE_OK;
}

स्थिर CURLcode ftp_done_status(संरचना Curl_easy *data,
                                संरचना ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  निवड(status) {
  स्थिती CURLE_BAD_DOWNLOAD_RESUME:
  स्थिती CURLE_FTP_WEIRD_PASV_REPLY:
  स्थिती CURLE_FTP_PORT_FAILED:
  स्थिती CURLE_FTP_ACCEPT_FAILED:
  स्थिती CURLE_FTP_ACCEPT_TIMEOUT:
  स्थिती CURLE_FTP_COULDNT_SET_TYPE:
  स्थिती CURLE_FTP_COULDNT_RETR_FILE:
  स्थिती CURLE_PARTIAL_FILE:
  स्थिती CURLE_UPLOAD_FAILED:
  स्थिती CURLE_REMOTE_ACCESS_DENIED:
  स्थिती CURLE_FILESIZE_EXCEEDED:
  स्थिती CURLE_REMOTE_FILE_NOT_FOUND:
  स्थिती CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  स्थिती CURLE_OK: /* does not affect the control connection's status */
    जर(!premature)
      थांबा;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  पूर्वनिर्धारित:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    परत status;      /* use the already set error code */
  }
  परत CURLE_OK;
}

स्थिर शून्य ftp_done_wildcard(संरचना Curl_easy *data, संरचना ftp_conn *ftpc)
{
  जर(data->state.wildcardmatch) {
    जर(data->set.chunk_end && ftpc->file) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

स्थिर शून्य ftp_done_path(संरचना Curl_easy *data, संरचना ftp_conn *ftpc,
                          CURLcode result)
{
  संरचना connectdata *conn = data->conn;
  जर(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  अन्यथा { /* remember working directory for connection reuse */
    स्थिरांक अक्षर *rawPath = ftpc->rawpath;
    जर(rawPath) {
      जर((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      अन्यथा {
        माप_प्रकार pathLen = म_माप(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        जर(!ftpc->cwdfail) {
          जर(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          अन्यथा
            /* file is URL-decoded */
            pathLen -= ftpc->file ? म_माप(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        अन्यथा
          ftpc->prevpath = रिक्त; /* no path */
      }
    }
    जर(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

स्थिर CURLcode ftp_done_secondary_socket(संरचना Curl_easy *data,
                                          संरचना ftp_conn *ftpc,
                                          CURLcode result)
{
  संरचना connectdata *conn = data->conn;
  जर(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    जर(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      जर(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  परत result;
}

स्थिर CURLcode ftp_done_control_reply(संरचना Curl_easy *data,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode result,
                                       bool premature)
{
  संरचना connectdata *conn = data->conn;
  माप_प्रकार nread;
  पूर्णांक ftpcode;

  जर(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    जर(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    जर(result)
      परत result;

    जर(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      परत result;
    }

    जर(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      निवड(ftpcode) {
      स्थिती 226:
      स्थिती 250:
        थांबा;
      स्थिती 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        थांबा;
      पूर्वनिर्धारित:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        थांबा;
      }
    }
  }
  परत result;
}

स्थिर CURLcode ftp_done_check_partial(संरचना Curl_easy *data,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode result,
                                       bool premature)
{
  जर(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  अन्यथा जर(data->state.upload) {
    जर((ftp->transfer == PPTRANSFER_BODY) &&
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
  अन्यथा {
    जर((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    अन्यथा जर(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  परत result;
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
स्थिर CURLcode ftp_done(संरचना Curl_easy *data, CURLcode status,
                         bool premature)
{
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  जर(!ftp || !ftpc)
    परत CURLE_OK;

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
  जर(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (पूर्णांक)result);
  परत result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
स्थिर CURLcode ftp_nb_type(संरचना Curl_easy *data,
                            संरचना ftp_conn *ftpc,
                            संरचना FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  अक्षर want = (अक्षर)(ascii ? 'A' : 'I');

  जर(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    परत ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  जर(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  परत result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
स्थिर CURLcode ftp_perform(
  संरचना Curl_easy *data,
  संरचना ftp_conn *ftpc,
  संरचना FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  जर(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  जर(result)
    परत result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  जर(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  अन्यथा
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  जर(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  परत result;
}

स्थिर शून्य wc_data_dtor(शून्य *ptr)
{
  संरचना ftp_wc *ftpwc = ptr;
  जर(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

स्थिर CURLcode init_wc_data(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  अक्षर *last_slash;
  अक्षर *path = ftp->path;
  संरचना WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  संरचना ftp_wc *ftpwc = रिक्त;

  last_slash = म_खोजप(ftp->path, '/');
  जर(last_slash) {
    last_slash++;
    जर(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      परत ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    जर(!wildcard->pattern)
      परत CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  अन्यथा { /* there is only 'wildcard pattern' or nothing */
    जर(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      जर(!wildcard->pattern)
        परत CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    अन्यथा { /* only list */
      wildcard->state = CURLWC_CLEAN;
      परत ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, आकार(संरचना ftp_wc));
  जर(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  जर(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  जर(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  जर(result) {
    जा fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  जर(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  infof(data, "Wildcard - Parsing started");
  परत CURLE_OK;

fail:
  जर(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = रिक्त;
  परत result;
}

स्थिर CURLcode wc_statemach(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  संरचना WildcardData * स्थिरांक wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  साठी(;;) {
    निवड(wildcard->state) {
    स्थिती CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      जर(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        परत result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      परत result;

    स्थिती CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      संरचना ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      जर(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        सुरूठेवा;
      }
      जर(Curl_llist_मोजणी(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        परत CURLE_REMOTE_FILE_NOT_FOUND;
      }
      सुरूठेवा;
    }

    स्थिती CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      संरचना Curl_llist_गाठ *head = Curl_llist_head(&wildcard->filelist);
      संरचना curl_fileinfo *finfo = Curl_गाठ_elem(head);
      अक्षर *tmp_path;
      अक्षर *enc = curl_easy_escape(रिक्त, finfo->filename, 0);
      जर(!enc)
        परत CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      जर(!tmp_path)
        परत CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      जर(data->set.chunk_bgn) {
        दीर्घ userresponse;
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (पूर्णांक)Curl_llist_मोजणी(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        निवड(userresponse) {
        स्थिती CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          सुरूठेवा;
        स्थिती CURL_CHUNK_BGN_FUNC_FAIL:
          परत CURLE_CHUNK_FAILED;
        }
      }

      जर(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        सुरूठेवा;
      }

      जर(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      जर(result)
        परत result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_गाठ_remove(Curl_llist_head(&wildcard->filelist));

      जर(Curl_llist_मोजणी(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        परत CURLE_OK;
      }
      परत result;
    }

    स्थिती CURLWC_SKIP: {
      जर(data->set.chunk_end) {
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_गाठ_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_मोजणी(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      सुरूठेवा;
    }

    स्थिती CURLWC_CLEAN: {
      संरचना ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      जर(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      परत result;
    }

    स्थिती CURLWC_DONE:
    स्थिती CURLWC_ERROR:
    स्थिती CURLWC_CLEAR:
      जर(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = रिक्त;
      }
      परत result;
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
स्थिर CURLcode ftp_regular_transfer(संरचना Curl_easy *data,
                                     संरचना ftp_conn *ftpc,
                                     संरचना FTP *ftp,
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

  जर(!result) {

    जर(!*dophase_done)
      /* the DO phase has not completed yet */
      परत CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    जर(result)
      परत result;
  }
  अन्यथा
    freedirs(ftpc);

  परत result;
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
स्थिर CURLcode ftp_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  जर(!ftpc || !ftp)
    परत CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    संरचना Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    जर(result)
      परत result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    जर(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      परत result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  जर(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    जर(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      परत CURLE_OK;
    }
    जर(result) /* error, loop or skipping the file */
      परत result;
  }
  अन्यथा { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    जर(result)
      परत result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  परत result;
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
स्थिर CURLcode ftp_quit(संरचना Curl_easy *data,
                         संरचना ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  जर(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    जर(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      परत result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  परत result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode ftp_disconnect(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  जर(!ftpc)
    परत CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  जर(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (शून्य)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  परत CURLE_OK;
}

/* called from multi.c while DOing */
स्थिर CURLcode ftp_doing(संरचना Curl_easy *data,
                          bool *dophase_done)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  जर(!ftpc || !ftp)
    परत CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  जर(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  अन्यथा जर(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  परत result;
}

स्थिर शून्य ftp_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना FTP *ftp = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

स्थिर शून्य ftp_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना ftp_conn *ftpc = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

स्थिर शून्य type_url_check(संरचना Curl_easy *data, संरचना FTP *ftp)
{
  माप_प्रकार len = म_माप(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  जर((len >= 7) && !स_भेद(&ftp->path[len - 7], ";type=", 6)) {
    अक्षर *type = &ftp->path[len - 7];
    अक्षर command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    निवड(command) {
    स्थिती 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      थांबा;

    स्थिती 'D': /* directory mode */
      data->state.list_only = TRUE;
      थांबा;

    स्थिती 'I': /* binary mode */
    पूर्वनिर्धारित:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      थांबा;
    }
  }
}

स्थिर CURLcode ftp_setup_connection(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  संरचना FTP *ftp;
  CURLcode result = CURLE_OK;
  संरचना ftp_conn *ftpc;

  ftp = curlx_calloc(1, आकार(*ftp));
  जर(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    परत CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, आकार(*ftpc));
  जर(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    परत CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  जर(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    जर(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      परत CURLE_OUT_OF_MEMORY;
    }
  }
  जर(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    जर(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      परत CURLE_OUT_OF_MEMORY;
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
               (पूर्णांक)result);
  परत result;
}

bool Curl_ftp_conns_match(संरचना connectdata *needle, संरचना connectdata *conn)
{
  संरचना ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  संरचना ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  जर(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    परत FALSE;

  निवड(nftpc->use_ssl) {
  स्थिती CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    परत (cftpc->use_ssl != CURLUSESSL_NONE);
  पूर्वनिर्धारित:
    परत (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_ftp = {
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
