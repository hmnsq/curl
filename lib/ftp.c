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
#define STATUSCODE(पंक्ति) \
  (ISDIGIT((पंक्ति)[0]) && ISDIGIT((पंक्ति)[1]) && ISDIGIT((पंक्ति)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(पंक्ति) (STATUSCODE(पंक्ति) && (' ' == (पंक्ति)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
स्थिर ध्रुव वर्ण * ध्रुव ftp_state_names[] = {
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
स्थिर शून्य ftp_state_निचला(संरचना Curl_easy *data,
                          संरचना ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , पूर्णाङ्क lineno
#endif
  )
{
  यदि(ftpc->state != newstate) {
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
#define ftp_state(x, y, z) ftp_state_निचला(x, y, z)
#अन्यथा /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_निचला(x, y, z, __LINE__)
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
स्थिर CURLcode getftpresponse(संरचना Curl_easy *data, size_t *nreadp,
                               पूर्णाङ्क *ftpcodep);

स्थिर शून्य freedirs(संरचना ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = रिक्त;
}

स्थिर size_t numof_slashes(ध्रुव वर्ण *str)
{
  ध्रुव वर्ण *slashPos;
  size_t अंक = 0;
  कुरु {
    slashPos = strchr(str, '/');
    यदि(slashPos) {
      ++अंक;
      str = slashPos + 1;
    }
  } यावत्(slashPos);
  प्रत्यागम अंक;
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
  ध्रुव वर्ण *slashPos = रिक्त;
  ध्रुव वर्ण *fileName = रिक्त;
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *rawPath = रिक्त; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  यदि(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  यदि(result) {
    failf(data, "path contains control characters");
    प्रत्यागम result;
  }
  rawPath = ftpc->rawpath;

  विकल्प(data->set.ftp_filemethod) {
  पक्ष FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    यदि((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    भङ्ग;

  पक्ष FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    यदि(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      यदि(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, परिमाण(ftpc->dirs[0]));
      यदि(!ftpc->dirs)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (पूर्णाङ्क)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    अन्यथा
      fileName = rawPath; /* filename only (or empty) */
    भङ्ग;

  स्वभाव: /* allow pretty much anything */
  पक्ष FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ध्रुव वर्ण *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    यदि(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      प्रत्यागम CURLE_URL_MALFORMAT;

    यदि(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, परिमाण(ftpc->dirs[0]));
      यदि(!ftpc->dirs)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      यावत्(dirAlloc--) {
        ध्रुव वर्ण *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        यदि(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        यदि(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (पूर्णाङ्क)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (पूर्णाङ्क)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    भङ्ग;
  } /* switch */

  यदि(fileName && *fileName)
    ftpc->file = fileName;
  अन्यथा
    ftpc->file = रिक्त; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  यदि(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  यदि((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  अन्यथा { /* newly created FTP connections are already in entry path */
    ध्रुव वर्ण *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    यदि(oldPath) {
      size_t n = pathLen;
      यदि(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      अन्यथा
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      यदि((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
स्थिर पूर्णाङ्क ftp_need_type(संरचना ftp_conn *ftpc,
                         bool ascii_wanted)
{
  प्रत्यागम ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
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
                                संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                ध्रुव वर्ण *buf, size_t blen)
{
  स्थिर ध्रुव वर्ण nl = '\n';
  संरचना ftp_cw_lc_ctx *ctx = writer->ctx;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  यावत्(blen) {
    /* do not pass EOS when writing parts */
    पूर्णाङ्क chunk_type = (type & ~CLIENTWRITE_EOS);
    ध्रुव वर्ण *cp;
    size_t chunk_len;
    CURLcode result;

    यदि(ctx->newline_pending) {
      यदि(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        यदि(result)
          प्रत्यागम result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    यदि(!cp)
      भङ्ग;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    यदि(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      यदि(result)
        प्रत्यागम result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  यदि(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  अन्यथा यदि(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    यदि(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      प्रत्यागम Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव संरचना Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  परिमाण(संरचना ftp_cw_lc_ctx)
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
  size_t nread;
  पूर्णाङ्क ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  यदि(curlx_dyn_len(&pp->recvbuf)) {
    ध्रुव वर्ण *l = curlx_dyn_ptr(&pp->recvbuf);
    यदि(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (शून्य)getftpresponse(data, &nread, &ftpcode);
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
    }
  }

  यदि(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  अन्यथा {
    पूर्णाङ्क socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    विकल्प(socketstate) {
    पक्ष -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
    स्वभाव:
      यदि(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      भङ्ग;
    }
  }

  यदि(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    यदि(pp->overflow > 3) {
      ध्रुव वर्ण *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      यदि((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        यदि(!curlx_str_संख्या(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          प्रत्यागम CURLE_OK;
        }
      }
    }

    (शून्य)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    यदि(ftpcode / 100 > 3)
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;

    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  प्रत्यागम CURLE_OK;
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
  यदि(result || !connected)
    प्रत्यागम result;

  यदि(data->state.upload) {
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

  प्रत्यागम CURLE_OK;
}

स्थिर bool ftp_endofresp(संरचना Curl_easy *data, संरचना connectdata *conn,
                          ध्रुव वर्ण *पंक्ति, size_t len, पूर्णाङ्क *code)
{
  curl_off_t status;
  (शून्य)data;
  (शून्य)conn;

  यदि((len > 3) && LASTLINE(पंक्ति) && !curlx_str_संख्या(&पंक्ति, &status, 999)) {
    *code = (पूर्णाङ्क)status;
    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

स्थिर CURLcode ftp_readresp(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             int8_t sockindex,
                             संरचना pingpong *pp,
                             पूर्णाङ्क *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  पूर्णाङ्क code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  यदि(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  यदि(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }

  प्रत्यागम result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
स्थिर CURLcode getftpresponse(संरचना Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               पूर्णाङ्क *ftpcodep) /* return the ftp-code */
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
  size_t nread;
  पूर्णाङ्क cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    यदि(timeout <= 0) {
      failf(data, "FTP response timeout");
      प्रत्यागम CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    यदि(timeout < interval_ms)
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

    यदि(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    अन्यथा यदि(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      पूर्णाङ्क ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      यदि(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      अन्यथा यदि(ev == 0) {
        result = Curl_pgrsUpdate(data);
        अनुवर्त; /* continue in our loop for the timeout duration */
      }
    }

    यदि(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      यदि(result)
        भङ्ग;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    यदि(result)
      भङ्ग;

    यदि(!nread && curlx_dyn_len(&pp->recvbuf))
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
               (पूर्णाङ्क)result, *nreadp, *ftpcodep);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_user(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  यदि(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_pwd(संरचना Curl_easy *data,
                              संरचना ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  यदि(!data->id && getenv("CURL_FTP_PWD_STOP"))
    प्रत्यागम CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  यदि(!result)
    ftp_state(data, ftpc, FTP_PWD);

  प्रत्यागम result;
}

/* For the FTP "protocol connect" and "doing" phases only */
स्थिर CURLcode ftp_pollset(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  प्रत्यागम ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
स्थिर CURLcode ftp_domore_pollset(संरचना Curl_easy *data,
                                   संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  यदि(!ftpc)
    प्रत्यागम CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  यदि(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    प्रत्यागम Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  प्रत्यागम Curl_pp_pollset(data, &ftpc->pp, ps);
}

स्थिर पूर्णाङ्क pathlen(संरचना ftp_conn *ftpc, पूर्णाङ्क अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  प्रत्यागम ftpc->dirs[अंक].len;
}

स्थिर ध्रुव वर्ण *pathpiece(संरचना ftp_conn *ftpc, पूर्णाङ्क अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  प्रत्यागम &ftpc->rawpath[ftpc->dirs[अंक].start];
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

  यदि(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  अन्यथा {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    यदि(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      यदि(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    अन्यथा {
      यदि(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        यदि(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      अन्यथा {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  प्रत्यागम result;
}

प्रकारनिर्देश गणना {
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
                                      ध्रुव वर्ण *string_ftpport,
                                      संरचना Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ध्रुव वर्ण **hostp,
                                      वर्ण *hbuf, size_t hbuflen)
{
  ध्रुव वर्ण *ip_end = रिक्त;
  ध्रुव वर्ण *addr = रिक्त;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  वर्ण ipstr[50];
#ifndef USE_IPV6
  (शून्य)conn;
  (शून्य)ss;
#endif

  /* default to nothing */
  *hostp = रिक्त;
  *port_minp = *port_maxp = 0;

  यदि(!string_ftpport || (strlen(string_ftpport) <= 1))
    गच्छ done;

#ifdef USE_IPV6
  यदि(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ध्रुव वर्ण *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    यदि(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  अन्यथा
#endif
    यदि(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    अन्यथा {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      यदि(ip_end) {
#ifdef USE_IPV6
        संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        यदि(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = रिक्त; /* this got no port ! */
        }
#endif
      }
      अन्यथा
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  यदि(ip_end) {
    ध्रुव वर्ण *portp = strchr(ip_end, ':');
    यदि(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      यदि(!curlx_str_संख्या(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        यदि(!curlx_str_single(&portp, '-') &&
           !curlx_str_संख्या(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        अन्यथा
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  यदि(port_min > port_max)
    port_min = port_max = 0;

  यदि(addrlen) {
    ध्रुव संरचना Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    यदि(!remote_addr || (addrlen >= परिमाण(ipstr)) || (addrlen >= hbuflen))
      प्रत्यागम CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    विकल्प(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    पक्ष IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      भङ्ग;
    पक्ष IF2IP_AF_NOT_SUPPORTED:
      प्रत्यागम CURLE_FTP_PORT_FAILED;
    पक्ष IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      भङ्ग;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  प्रत्यागम CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
स्थिर CURLcode ftp_port_default_host(संरचना Curl_easy *data,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ध्रुव वर्ण **hostp,
                                      वर्ण *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#endif
  वर्ण buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = परिमाण(*ss);
  यदि(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, परिमाण(buffer)));
    प्रत्यागम CURLE_FTP_PORT_FAILED;
  }
  विकल्प(sa->sa_family) {
#ifdef USE_IPV6
  पक्ष AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    भङ्ग;
#endif
  स्वभाव:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    भङ्ग;
  }
  यदि(result)
    प्रत्यागम CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  प्रत्यागम CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
स्थिर CURLcode ftp_port_resolve_host(संरचना Curl_easy *data,
                                      संरचना connectdata *conn,
                                      ध्रुव वर्ण *host,
                                      संरचना Curl_dns_entry **dns_entryp,
                                      ध्रुव संरचना Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = रिक्त;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  यदि(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  अन्यथा {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  प्रत्यागम result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
स्थिर CURLcode ftp_port_open_socket(संरचना Curl_easy *data,
                                     संरचना connectdata *conn,
                                     ध्रुव संरचना Curl_addrinfo *res,
                                     ध्रुव संरचना Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  वर्ण buffer[STRERROR_LEN];
  पूर्णाङ्क sockerr = 0;
  ध्रुव संरचना Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  प्रति(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, रिक्त,
                       Curl_conn_get_transport(data, conn), portsockp);
    यदि(result) {
      यदि(result == CURLE_OUT_OF_MEMORY)
        प्रत्यागम result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      अनुवर्त;
    }
    भङ्ग;
  }
  यदि(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, परिमाण(buffer)));
    प्रत्यागम CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  प्रत्यागम result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
स्थिर CURLcode ftp_port_bind_socket(संरचना Curl_easy *data,
                                     संरचना connectdata *conn,
                                     curl_socket_t portsock,
                                     ध्रुव संरचना Curl_addrinfo *ai,
                                     संरचना Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#endif
  वर्ण buffer[STRERROR_LEN];
  unsigned short port;
  पूर्णाङ्क sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  प्रति(port = port_min; port <= port_max;) {
    यदि(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    अन्यथा
      sa6->sin6_port = htons(port);
#endif
    यदि(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      यदि(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, परिमाण(buffer)));

        *sslen_io = परिमाण(*ss);
        यदि(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, परिमाण(buffer)));
          प्रत्यागम CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        अनुवर्त;
      }
      यदि(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, परिमाण(buffer)));
        प्रत्यागम CURLE_FTP_PORT_FAILED;
      }
    }
    अन्यथा
      भङ्ग;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    यदि(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      प्रत्यागम CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = परिमाण(*ss);
  यदि(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, परिमाण(buffer)));
    प्रत्यागम CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  प्रत्यागम CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
स्थिर CURLcode ftp_port_listen(संरचना Curl_easy *data, curl_socket_t portsock)
{
  वर्ण buffer[STRERROR_LEN];

  यदि(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, परिमाण(buffer)));
    प्रत्यागम CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  प्रत्यागम CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
स्थिर CURLcode ftp_port_send_command(संरचना Curl_easy *data,
                                      संरचना ftp_conn *ftpc,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      ध्रुव संरचना Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  स्थिर ध्रुव वर्ण mode[][5] = { "EPRT", "PORT" };
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#endif
  वर्ण myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, परिमाण(myhost));

#ifdef USE_IPV6
  यदि(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  प्रति(; fcmd != DONE; fcmd++) {

    यदि(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      अनुवर्त;

    यदि((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      अनुवर्त;

    विकल्प(sa->sa_family) {
    पक्ष AF_INET:
      port = ntohs(sa4->sin_port);
      भङ्ग;
#ifdef USE_IPV6
    पक्ष AF_INET6:
      port = ntohs(sa6->sin6_port);
      भङ्ग;
#endif
    स्वभाव:
      अनुवर्त; /* might as well skip this */
    }

    यदि(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      यदि(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        प्रत्यागम result;
      }
      भङ्ग;
    }
    यदि(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      वर्ण target[परिमाण(myhost) + 20];
      ध्रुव वर्ण *source = myhost;
      वर्ण *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      यावत्(*source) {
        यदि(*source == '.')
          *dest = ',';
        अन्यथा
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (पूर्णाङ्क)(port >> 8), (पूर्णाङ्क)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      यदि(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        प्रत्यागम result;
      }
      भङ्ग;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  प्रत्यागम CURLE_OK;
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
  वर्ण hbuf[NI_MAXHOST];
  ध्रुव वर्ण *host = रिक्त;
  ध्रुव वर्ण *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  संरचना Curl_dns_entry *dns_entry = रिक्त;
  ध्रुव संरचना Curl_addrinfo *res = रिक्त;
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, परिमाण(hbuf));
  यदि(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, परिमाण(hbuf), &non_local);

  /* resolve host string to address list */
  यदि(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  यदि(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  यदि(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  यदि(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  यदि(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  यदि(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  यदि(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  यदि(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  यदि(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  अन्यथा {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    यदि(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  यदि(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  प्रत्यागम result;
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

  स्थिर ध्रुव वर्ण mode[][5] = { "EPSV", "PASV" };
  पूर्णाङ्क modeoff;

#ifdef PF_INET6
  यदि(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  यदि(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  प्रत्यागम result;
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

  यदि(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  अन्यथा यदि(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  अन्यथा {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    यदि(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      यदि(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      अन्यथा यदि(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      अन्यथा
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      यदि(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    अन्यथा
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_rest(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  यदि((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    यदि(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  अन्यथा
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_size(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  यदि((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    यदि(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  अन्यथा
    result = ftp_state_rest(data, ftpc, ftp);

  प्रत्यागम result;
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
  ध्रुव वर्ण *lstArg = रिक्त;
  पूर्णाङ्क lstArglen = 0;
  वर्ण *cmd;

  यदि((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ध्रुव वर्ण *rawPath = ftpc->rawpath;
    ध्रुव वर्ण *slashPos = strrchr(rawPath, '/');
    यदि(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      यदि(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (पूर्णाङ्क)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  यदि(!cmd)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  यदि(!result)
    ftp_state(data, ftpc, FTP_LIST);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_list_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

स्थिर CURLcode ftp_state_retr_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

स्थिर CURLcode ftp_state_stor_prequote(संरचना Curl_easy *data,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

स्थिर CURLcode ftp_state_type(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  यदि(data->req.no_body && ftpc->file &&
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
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा
    result = ftp_state_size(data, ftpc, ftp);

  प्रत्यागम result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
स्थिर CURLcode ftp_state_mdtm(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  यदि((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    यदि(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  अन्यथा
    result = ftp_state_type(data, ftpc, ftp);

  प्रत्यागम result;
}

/* This is called after the TYPE and possible quote commands have been sent */
स्थिर CURLcode ftp_state_ul_setup(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  यदि((data->state.resume_from && !sizechecked) ||
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
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;

    यदि(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      यदि(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      प्रत्यागम result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    यदि(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    यदि(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      कुरु {
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
        }
      } यावत्(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    यदि(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      यदि(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        प्रत्यागम CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  यदि(!result)
    ftp_state(data, ftpc, FTP_STOR);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_retr(संरचना Curl_easy *data,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  यदि(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    प्रत्यागम CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  यदि(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    यदि(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    अन्यथा {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      यदि(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        यदि(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      अन्यथा {
        यदि(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    यदि(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      प्रत्यागम CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    यदि(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  अन्यथा {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    यदि(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  प्रत्यागम result;
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

  विकल्प(instate) {
  पक्ष FTP_QUOTE:
  स्वभाव:
    item = data->set.quote;
    भङ्ग;
  पक्ष FTP_RETR_PREQUOTE:
  पक्ष FTP_STOR_PREQUOTE:
  पक्ष FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    भङ्ग;
  पक्ष FTP_POSTQUOTE:
    item = data->set.postquote;
    भङ्ग;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  यदि(init)
    ftpc->count1 = 0;
  अन्यथा
    ftpc->count1++;

  यदि(item) {
    पूर्णाङ्क i = 0;

    /* Skip count1 items in the linked list */
    यावत्((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    यदि(item) {
      ध्रुव वर्ण *cmd = item->data;
      यदि(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      अन्यथा
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      यदि(result)
        प्रत्यागम result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  यदि(!quote) {
    /* No more quote to send, continue to ... */
    विकल्प(instate) {
    पक्ष FTP_QUOTE:
    स्वभाव:
      result = ftp_state_cwd(data, ftpc, ftp);
      भङ्ग;
    पक्ष FTP_RETR_PREQUOTE:
      यदि(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      अन्यथा {
        यदि(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        अन्यथा {
          यदि(data->set.ignorecl || data->state.prefer_ascii) {
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
            यदि(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          अन्यथा {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            यदि(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      भङ्ग;
    पक्ष FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      भङ्ग;
    पक्ष FTP_POSTQUOTE:
      भङ्ग;
    पक्ष FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      भङ्ग;
    }
  }

  प्रत्यागम result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
स्थिर CURLcode ftp_epsv_disable(संरचना Curl_easy *data,
                                 संरचना ftp_conn *ftpc,
                                 संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;

  यदि(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  यदि(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ftp_control_addr_dup(संरचना Curl_easy *data, वर्ण **newhostp)
{
  संरचना connectdata *conn = data->conn;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  यदि(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  अन्यथा
#endif
  यदि(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  अन्यथा {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = रिक्त;
    प्रत्यागम CURLE_FTP_CANT_GET_HOST;
  }
  प्रत्यागम *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर bool match_pasv_6nums(ध्रुव वर्ण *p,
                             unsigned पूर्णाङ्क *array) /* 6 numbers */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 6; i++) {
    curl_off_t अंक;
    यदि(i) {
      यदि(*p != ',')
        प्रत्यागम FALSE;
      p++;
    }
    यदि(curlx_str_संख्या(&p, &अंक, 0xff))
      प्रत्यागम FALSE;
    array[i] = (unsigned पूर्णाङ्क)अंक;
  }
  प्रत्यागम TRUE;
}

स्थिर CURLcode ftp_state_pasv_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result;
  ध्रुव संरचना pingpong *pp = &ftpc->pp;
  वर्ण *newhost = रिक्त;
  unsigned short newport = 0;
  ध्रुव वर्ण *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  यदि((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ध्रुव वर्ण *ptr = strchr(str, '(');
    यदि(ptr) {
      वर्ण sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      यदि(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ध्रुव वर्ण *p = &ptr[3];
        curl_off_t अंक;
        यदि(curlx_str_संख्या(&p, &अंक, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          प्रत्यागम CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)अंक;
        result = ftp_control_addr_dup(data, &newhost);
        यदि(result)
          प्रत्यागम result;
      }
      अन्यथा
        ptr = रिक्त;
    }
    यदि(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      प्रत्यागम CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  अन्यथा यदि((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned पूर्णाङ्क ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    यावत्(*str) {
      यदि(match_pasv_6nums(str, ip))
        भङ्ग;
      str++;
    }

    यदि(!*str) {
      failf(data, "Could not interpret the 227-response");
      प्रत्यागम CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    यदि(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    यदि(!newhost)
      प्रत्यागम CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  अन्यथा यदि(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    प्रत्यागम ftp_epsv_disable(data, ftpc, conn);
  }
  अन्यथा {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    प्रत्यागम CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  यदि(result)
    गच्छ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  यदि(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    यदि(result)
      गच्छ error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  यदि(result) {
    यदि((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    गच्छ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  यदि(data->set.verbose) {
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
  प्रत्यागम result;
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

  प्रत्यागम result;
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

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  यदि(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    यदि(result == CURLE_OUT_OF_MEMORY)
      प्रत्यागम result;
    यदि(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      यदि(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        प्रत्यागम ftp_epsv_disable(data, ftpc, conn);
      }
      प्रत्यागम result;
    }
  }

  यदि(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    यदि(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    यदि(result || !ftpc->wait_data_conn)
      प्रत्यागम result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  यदि(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    यदि(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      यदि(result)
        प्रत्यागम result; /* Failed to accept data connection */

      यदि(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        यदि(result)
          प्रत्यागम result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      अन्यथा {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        यदि(result)
          प्रत्यागम result;
      }
    }
    अन्यथा यदि(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      यदि(result)
        प्रत्यागम result;

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

      यदि(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      यदि(result)
        प्रत्यागम result;

      यदि((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        यदि(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          यदि(result)
            प्रत्यागम result;
        }
        /* otherwise fall through */
      }
      अन्यथा {
        यदि(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        अन्यथा {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        यदि(result)
          प्रत्यागम result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      यदि(complete)
        *more = DOMORE_DONE;
    }
    प्रत्यागम result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  यदि(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (पूर्णाङ्क)result);
  }

  प्रत्यागम result;
}

/* call this when the DO phase has completed */
स्थिर CURLcode ftp_dophase_done(संरचना Curl_easy *data,
                                 संरचना ftp_conn *ftpc,
                                 संरचना FTP *ftp,
                                 bool connected)
{
  यदि(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    यदि(result) {
      close_secondarysocket(data, ftpc);
      प्रत्यागम result;
    }
  }

  यदि(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  अन्यथा यदि(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ftp_state_port_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode)
{
  संरचना connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  यदि(ftpcode / 100 != 2) {
    /* the command failed */

    यदि(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    यदि(fcmd == DONE) {
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

  प्रत्यागम result;
}

/* return TRUE on error, FALSE on success */
स्थिर bool twodigit(ध्रुव वर्ण *p, पूर्णाङ्क *val)
{
  यदि(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    प्रत्यागम TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  प्रत्यागम FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ध्रुव वर्ण *p, पूर्णाङ्क *year, पूर्णाङ्क *month, पूर्णाङ्क *day,
                           पूर्णाङ्क *hour, पूर्णाङ्क *minute, पूर्णाङ्क *second);
UNITTEST bool ftp_213_date(ध्रुव वर्ण *p, पूर्णाङ्क *year, पूर्णाङ्क *month, पूर्णाङ्क *day,
                           पूर्णाङ्क *hour, पूर्णाङ्क *minute, पूर्णाङ्क *second)
{
  पूर्णाङ्क century;
  यदि((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    प्रत्यागम FALSE;

  *year += century * 100;
  यदि((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

स्थिर CURLcode client_write_header(संरचना Curl_easy *data,
                                    वर्ण *buf, size_t blen)
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
  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_mdtm_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode result = CURLE_OK;

  विकल्प(ftpcode) {
  पक्ष 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    पूर्णाङ्क year, month, day, hour, minute, second;
    संरचना pingpong *pp = &ftpc->pp;
    ध्रुव वर्ण *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    यदि(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      वर्ण timebuf[24];
      curl_msnprintf(timebuf, परिमाण(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      यदि(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#यदि defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic धकेलो
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    यदि(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#यदि defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic निकालो
#endif
      वर्ण headerbuf[128];
      पूर्णाङ्क headerbuflen;
      time_t filetime = data->info.filetime;
      संरचना tm buffer;
      ध्रुव संरचना tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      यदि(result)
        प्रत्यागम result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, परिमाण(headerbuf),
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
      यदि(result)
        प्रत्यागम result;
    } /* end of a ridiculous amount of conditionals */
  }
    भङ्ग;
  स्वभाव:
    infof(data, "unsupported MDTM reply format");
    भङ्ग;
  पक्ष 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    भङ्ग;
  }

  यदि(data->set.timecondition) {
    यदि((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      विकल्प(data->set.timecondition) {
      पक्ष CURL_TIMECOND_IFMODSINCE:
      स्वभाव:
        यदि(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          प्रत्यागम CURLE_OK;
        }
        भङ्ग;
      पक्ष CURL_TIMECOND_IFUNMODSINCE:
        यदि(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          प्रत्यागम CURLE_OK;
        }
        भङ्ग;
      } /* switch */
    }
    अन्यथा {
      infof(data, "Skipping time comparison");
    }
  }

  यदि(!result)
    result = ftp_state_type(data, ftpc, ftp);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_type_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  यदि(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    प्रत्यागम CURLE_FTP_COULDNT_SET_TYPE;
  }
  यदि(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  यदि(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  अन्यथा यदि(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  अन्यथा यदि(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  अन्यथा यदि(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  अन्यथा यदि(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_size_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ध्रुव वर्ण *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  यदि(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ध्रुव वर्ण *start = &buf[4];
    ध्रुव वर्ण *fdigit = memchr(start, '\r', len - 4);
    यदि(fdigit) {
      fdigit--;
      यदि(*fdigit == '\n')
        fdigit--;
      यावत्(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    अन्यथा
      fdigit = start;
    यदि(curlx_str_संख्या(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  अन्यथा यदि(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    प्रत्यागम CURLE_REMOTE_FILE_NOT_FOUND;
  }

  यदि(instate == FTP_SIZE) {
    यदि(filesize != -1) {
      वर्ण clbuf[128];
      पूर्णाङ्क clbuflen = curl_msnprintf(clbuf, परिमाण(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      यदि(result)
        प्रत्यागम result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  अन्यथा यदि(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  अन्यथा यदि(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_rest_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  विकल्प(instate) {
  पक्ष FTP_REST:
  स्वभाव:
    यदि(ftpcode == 350) {
      वर्ण buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      यदि(result)
        प्रत्यागम result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    भङ्ग;

  पक्ष FTP_RETR_REST:
    यदि(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    अन्यथा {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      यदि(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    भङ्ग;
  }

  प्रत्यागम result;
}

स्थिर CURLcode ftp_state_stor_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode result = CURLE_OK;

  यदि(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    प्रत्यागम CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  यदि(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    यदि(result)
      प्रत्यागम result;

    यदि(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      प्रत्यागम ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  प्रत्यागम ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
स्थिर CURLcode ftp_state_get_resp(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   पूर्णाङ्क ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  यदि((ftpcode == 150) || (ftpcode == 125)) {

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

    यदि((instate != FTP_LIST) &&
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
      यदि(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        प्रति(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ध्रुव वर्ण *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ध्रुव वर्ण *c = &buf[i];
          यदि(!curlx_str_संख्या(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            भङ्ग;
          }
        }
      }
    }
    अन्यथा यदि(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    यदि(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    अन्यथा यदि((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    यदि(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    यदि(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      यदि(result)
        प्रत्यागम result;

      यदि(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        प्रत्यागम ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    प्रत्यागम ftp_initiate_transfer(data, ftpc);
  }
  अन्यथा {
    यदि((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    अन्यथा {
      failf(data, "RETR response: %03d", ftpcode);
      प्रत्यागम instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  प्रत्यागम result;
}

/* after USER, PASS and ACCT */
स्थिर CURLcode ftp_state_loggedin(संरचना Curl_easy *data,
                                   संरचना ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  यदि(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    यदि(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  अन्यथा {
    result = ftp_state_pwd(data, ftpc);
  }
  प्रत्यागम result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
स्थिर bool ftp_has_ctrl(ध्रुव वर्ण *string)
{
  ध्रुव unsigned वर्ण *s = (ध्रुव unsigned वर्ण *)string;
  यावत्(*s) {
    यदि(*s < 0x20)
      प्रत्यागम TRUE;
    s++;
  }
  प्रत्यागम FALSE;
}

/* for USER and PASS responses */
स्थिर CURLcode ftp_state_user_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  यदि((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    यदि(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  अन्यथा यदि(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  अन्यथा यदि(ftpcode == 332) {
    ध्रुव वर्ण *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    यदि(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    अन्यथा यदि(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अन्यथा {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      यदि(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  अन्यथा {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ध्रुव वर्ण *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    यदि(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      यदि(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      अन्यथा {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        यदि(!result) {
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
  प्रत्यागम result;
}

/* for ACCT response */
स्थिर CURLcode ftp_state_acct_resp(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode result = CURLE_OK;
  यदि(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  अन्यथा
    result = ftp_state_loggedin(data, ftpc);

  प्रत्यागम result;
}

स्थिर CURLcode ftp_pwd_resp(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             पूर्णाङ्क ftpcode)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode result;

  यदि(ftpcode == 257) {
    ध्रुव वर्ण *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
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
    यावत्(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    यदि('\"' == *ptr) {
      /* it started good */
      प्रति(ptr++; *ptr; ptr++) {
        यदि('\"' == *ptr) {
          यदि('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          अन्यथा {
            /* end of path */
            यदि(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            भङ्ग; /* get out of this loop */
          }
        }
        अन्यथा {
          यदि(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        यदि(result) {
          curlx_dyn_free(&out);
          प्रत्यागम result;
        }
      }
    }
    यदि(entry_extracted) {
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
      वर्ण *dir = curlx_dyn_ptr(&out);

      यदि(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        यदि(result) {
          curlx_dyn_free(&out);
          प्रत्यागम result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      यदि(!data->state.most_recent_ftp_entrypath)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      यदि(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        प्रत्यागम CURLE_OK;
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
  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव वर्ण * ध्रुव ftpauth[] = { "SSL", "TLS" };

स्थिर CURLcode ftp_wait_resp(संरचना Curl_easy *data,
                              संरचना connectdata *conn,
                              संरचना ftp_conn *ftpc,
                              पूर्णाङ्क ftpcode)
{
  CURLcode result = CURLE_OK;
  यदि(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    यदि(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      प्रत्यागम ftp_state_user_resp(data, ftpc, ftpcode);
  }
  अन्यथा यदि(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  यदि(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    विकल्प((long)data->set.ftpsslauth) {
    पक्ष CURLFTPAUTH_DEFAULT:
    पक्ष CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      भङ्ग;
    पक्ष CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      भङ्ग;
    स्वभाव:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (पूर्णाङ्क)data->set.ftpsslauth);
      प्रत्यागम CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    यदि(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  अन्यथा
    result = ftp_state_user(data, ftpc, conn);
  प्रत्यागम result;
}

/* an implicit TLS scheme implies TLS on both connections */
स्थिर unsigned वर्ण ftp_use_ssl(संरचना connectdata *conn,
                                 संरचना ftp_conn *ftpc)
{
  यदि(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    प्रत्यागम CURLUSESSL_ALL;
  प्रत्यागम ftpc->use_ssl;
}

स्थिर CURLcode ftp_pp_statemachine(संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  CURLcode result;
  पूर्णाङ्क ftpcode;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  संरचना pingpong *pp;
  size_t nread = 0;

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  यदि(pp->sendleft)
    प्रत्यागम Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  यदि(result || !ftpcode)
    प्रत्यागम result;

  /* we have now received a full FTP server response */
  विकल्प(ftpc->state) {
  पक्ष FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    यदि(pp->overflow)
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    यदि((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        यदि(result) {
          /* we failed and bail out */
          प्रत्यागम CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      यदि(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    अन्यथा यदि(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    अन्यथा {
      यदि(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      अन्यथा
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    भङ्ग;

  पक्ष FTP_USER:
  पक्ष FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    यदि(!result)
      ftp_state(data, ftpc, FTP_PROT);
    भङ्ग;

  पक्ष FTP_PROT:
    यदि(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    अन्यथा यदि(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      प्रत्यागम CURLE_USE_SSL_FAILED;

    यदि(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      यदि(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    अन्यथा
      result = ftp_state_pwd(data, ftpc);
    भङ्ग;

  पक्ष FTP_CCC:
    यदि(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned वर्ण)CURLFTPSSL_CCC_ACTIVE));
      यदि(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    यदि(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    भङ्ग;

  पक्ष FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_SYST:
    यदि(ftpcode == 215) {
      ध्रुव वर्ण *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ध्रुव वर्ण *start;
      वर्ण *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      यावत्(*ptr == ' ')
        ptr++;
      प्रति(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      यदि(!os)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      यदि(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        यदि(result) {
          curlx_free(os);
          प्रत्यागम result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        भङ्ग;
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
    भङ्ग;

  पक्ष FTP_NAMEFMT:
    यदि(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      भङ्ग;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    भङ्ग;

  पक्ष FTP_QUOTE:
  पक्ष FTP_POSTQUOTE:
  पक्ष FTP_RETR_PREQUOTE:
  पक्ष FTP_STOR_PREQUOTE:
  पक्ष FTP_LIST_PREQUOTE:
    यदि((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    अन्यथा
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    भङ्ग;

  पक्ष FTP_CWD:
    यदि(ftpcode / 100 != 2) {
      /* failure to CWD there */
      यदि(data->set.ftp_create_missing_dirs &&
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
        यदि(!result)
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
      यदि(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      अन्यथा {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    भङ्ग;

  पक्ष FTP_MKD:
    यदि((ftpcode / 100 != 2) && !ftpc->count3--) {
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
    भङ्ग;

  पक्ष FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    भङ्ग;

  पक्ष FTP_TYPE:
  पक्ष FTP_LIST_TYPE:
  पक्ष FTP_RETR_TYPE:
  पक्ष FTP_STOR_TYPE:
  पक्ष FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_SIZE:
  पक्ष FTP_RETR_SIZE:
  पक्ष FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_REST:
  पक्ष FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_PRET:
    यदि(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      प्रत्यागम CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    भङ्ग;

  पक्ष FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    भङ्ग;

  पक्ष FTP_LIST:
  पक्ष FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_QUIT:
  स्वभाव:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    भङ्ग;
  }

  प्रत्यागम result;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ftp_multi_statemach(संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  प्रत्यागम ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

स्थिर CURLcode ftp_block_statemach(संरचना Curl_easy *data,
                                    संरचना ftp_conn *ftpc)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  यावत्(ftpc->state != FTP_STOP) {
    यदि(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    यदि(result)
      भङ्ग;
  }

  प्रत्यागम result;
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
  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    यदि(result)
      प्रत्यागम result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  प्रत्यागम result;
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
  यावत्(item) {
    यदि(item->data) {
      size_t nread;
      ध्रुव वर्ण *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      पूर्णाङ्क ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      यदि(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      यदि(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      यदि(result)
        प्रत्यागम result;

      यदि(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        प्रत्यागम CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ftp_done_status(संरचना Curl_easy *data,
                                संरचना ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  विकल्प(status) {
  पक्ष CURLE_BAD_DOWNLOAD_RESUME:
  पक्ष CURLE_FTP_WEIRD_PASV_REPLY:
  पक्ष CURLE_FTP_PORT_FAILED:
  पक्ष CURLE_FTP_ACCEPT_FAILED:
  पक्ष CURLE_FTP_ACCEPT_TIMEOUT:
  पक्ष CURLE_FTP_COULDNT_SET_TYPE:
  पक्ष CURLE_FTP_COULDNT_RETR_FILE:
  पक्ष CURLE_PARTIAL_FILE:
  पक्ष CURLE_UPLOAD_FAILED:
  पक्ष CURLE_REMOTE_ACCESS_DENIED:
  पक्ष CURLE_FILESIZE_EXCEEDED:
  पक्ष CURLE_REMOTE_FILE_NOT_FOUND:
  पक्ष CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  पक्ष CURLE_OK: /* does not affect the control connection's status */
    यदि(!premature)
      भङ्ग;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  स्वभाव:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    प्रत्यागम status;      /* use the already set error code */
  }
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ftp_done_wildcard(संरचना Curl_easy *data, संरचना ftp_conn *ftpc)
{
  यदि(data->state.wildcardmatch) {
    यदि(data->set.chunk_end && ftpc->file) {
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
  यदि(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  अन्यथा { /* remember working directory for connection reuse */
    ध्रुव वर्ण *rawPath = ftpc->rawpath;
    यदि(rawPath) {
      यदि((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      अन्यथा {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        यदि(!ftpc->cwdfail) {
          यदि(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          अन्यथा
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        अन्यथा
          ftpc->prevpath = रिक्त; /* no path */
      }
    }
    यदि(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

स्थिर CURLcode ftp_done_secondary_socket(संरचना Curl_easy *data,
                                          संरचना ftp_conn *ftpc,
                                          CURLcode result)
{
  संरचना connectdata *conn = data->conn;
  यदि(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    यदि(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      यदि(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ftp_done_control_reply(संरचना Curl_easy *data,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode result,
                                       bool premature)
{
  संरचना connectdata *conn = data->conn;
  size_t nread;
  पूर्णाङ्क ftpcode;

  यदि(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    यदि(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    यदि(result)
      प्रत्यागम result;

    यदि(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      प्रत्यागम result;
    }

    यदि(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      विकल्प(ftpcode) {
      पक्ष 226:
      पक्ष 250:
        भङ्ग;
      पक्ष 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        भङ्ग;
      स्वभाव:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        भङ्ग;
      }
    }
  }
  प्रत्यागम result;
}

स्थिर CURLcode ftp_done_check_partial(संरचना Curl_easy *data,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode result,
                                       bool premature)
{
  यदि(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  अन्यथा यदि(data->state.upload) {
    यदि((ftp->transfer == PPTRANSFER_BODY) &&
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
    यदि((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    अन्यथा यदि(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  प्रत्यागम result;
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

  यदि(!ftp || !ftpc)
    प्रत्यागम CURLE_OK;

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
  यदि(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (पूर्णाङ्क)result);
  प्रत्यागम result;
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
  वर्ण want = (वर्ण)(ascii ? 'A' : 'I');

  यदि(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    प्रत्यागम ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  यदि(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  प्रत्यागम result;
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

  यदि(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  यदि(result)
    प्रत्यागम result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  यदि(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  अन्यथा
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  यदि(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  प्रत्यागम result;
}

स्थिर शून्य wc_data_dtor(शून्य *ptr)
{
  संरचना ftp_wc *ftpwc = ptr;
  यदि(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

स्थिर CURLcode init_wc_data(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  वर्ण *last_slash;
  वर्ण *path = ftp->path;
  संरचना WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  संरचना ftp_wc *ftpwc = रिक्त;

  last_slash = strrchr(ftp->path, '/');
  यदि(last_slash) {
    last_slash++;
    यदि(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      प्रत्यागम ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    यदि(!wildcard->pattern)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  अन्यथा { /* there is only 'wildcard pattern' or nothing */
    यदि(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      यदि(!wildcard->pattern)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    अन्यथा { /* only list */
      wildcard->state = CURLWC_CLEAN;
      प्रत्यागम ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, परिमाण(संरचना ftp_wc));
  यदि(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  यदि(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  यदि(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  यदि(result) {
    गच्छ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  यदि(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  infof(data, "Wildcard - Parsing started");
  प्रत्यागम CURLE_OK;

fail:
  यदि(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = रिक्त;
  प्रत्यागम result;
}

स्थिर CURLcode wc_statemach(संरचना Curl_easy *data,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  संरचना WildcardData * ध्रुव wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  प्रति(;;) {
    विकल्प(wildcard->state) {
    पक्ष CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      यदि(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        प्रत्यागम result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      प्रत्यागम result;

    पक्ष CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      संरचना ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      यदि(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        अनुवर्त;
      }
      यदि(Curl_llist_गिनती(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        प्रत्यागम CURLE_REMOTE_FILE_NOT_FOUND;
      }
      अनुवर्त;
    }

    पक्ष CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      संरचना Curl_llist_गाँठ *head = Curl_llist_head(&wildcard->filelist);
      संरचना curl_fileinfo *finfo = Curl_गाँठ_elem(head);
      वर्ण *tmp_path;
      वर्ण *enc = curl_easy_escape(रिक्त, finfo->filename, 0);
      यदि(!enc)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      यदि(!tmp_path)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      यदि(data->set.chunk_bgn) {
        long userresponse;
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (पूर्णाङ्क)Curl_llist_गिनती(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        विकल्प(userresponse) {
        पक्ष CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          अनुवर्त;
        पक्ष CURL_CHUNK_BGN_FUNC_FAIL:
          प्रत्यागम CURLE_CHUNK_FAILED;
        }
      }

      यदि(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        अनुवर्त;
      }

      यदि(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      यदि(result)
        प्रत्यागम result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_गाँठ_remove(Curl_llist_head(&wildcard->filelist));

      यदि(Curl_llist_गिनती(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        प्रत्यागम CURLE_OK;
      }
      प्रत्यागम result;
    }

    पक्ष CURLWC_SKIP: {
      यदि(data->set.chunk_end) {
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_गाँठ_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_गिनती(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      अनुवर्त;
    }

    पक्ष CURLWC_CLEAN: {
      संरचना ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      यदि(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      प्रत्यागम result;
    }

    पक्ष CURLWC_DONE:
    पक्ष CURLWC_ERROR:
    पक्ष CURLWC_CLEAR:
      यदि(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = रिक्त;
      }
      प्रत्यागम result;
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

  यदि(!result) {

    यदि(!*dophase_done)
      /* the DO phase has not completed yet */
      प्रत्यागम CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा
    freedirs(ftpc);

  प्रत्यागम result;
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
  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    संरचना Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    यदि(result)
      प्रत्यागम result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    यदि(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      प्रत्यागम result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  यदि(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    यदि(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      प्रत्यागम CURLE_OK;
    }
    यदि(result) /* error, loop or skipping the file */
      प्रत्यागम result;
  }
  अन्यथा { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    यदि(result)
      प्रत्यागम result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  प्रत्यागम result;
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

  यदि(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    यदि(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      प्रत्यागम result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  प्रत्यागम result;
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

  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  यदि(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (शून्य)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  प्रत्यागम CURLE_OK;
}

/* called from multi.c while DOing */
स्थिर CURLcode ftp_doing(संरचना Curl_easy *data,
                          bool *dophase_done)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  यदि(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  अन्यथा यदि(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  प्रत्यागम result;
}

स्थिर शून्य ftp_easy_dtor(ध्रुव शून्य *कुंजी, size_t klen, शून्य *entry)
{
  संरचना FTP *ftp = entry;
  (शून्य)कुंजी;
  (शून्य)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

स्थिर शून्य ftp_conn_dtor(ध्रुव शून्य *कुंजी, size_t klen, शून्य *entry)
{
  संरचना ftp_conn *ftpc = entry;
  (शून्य)कुंजी;
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
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  यदि((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    वर्ण *type = &ftp->path[len - 7];
    वर्ण command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    विकल्प(command) {
    पक्ष 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      भङ्ग;

    पक्ष 'D': /* directory mode */
      data->state.list_only = TRUE;
      भङ्ग;

    पक्ष 'I': /* binary mode */
    स्वभाव:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      भङ्ग;
    }
  }
}

स्थिर CURLcode ftp_setup_connection(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  संरचना FTP *ftp;
  CURLcode result = CURLE_OK;
  संरचना ftp_conn *ftpc;

  ftp = curlx_calloc(1, परिमाण(*ftp));
  यदि(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, परिमाण(*ftpc));
  यदि(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  यदि(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    यदि(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }
  यदि(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    यदि(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
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
               (पूर्णाङ्क)result);
  प्रत्यागम result;
}

bool Curl_ftp_conns_match(संरचना connectdata *needle, संरचना connectdata *conn)
{
  संरचना ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  संरचना ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  यदि(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    प्रत्यागम FALSE;

  विकल्प(nftpc->use_ssl) {
  पक्ष CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    प्रत्यागम (cftpc->use_ssl != CURLUSESSL_NONE);
  स्वभाव:
    प्रत्यागम (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_ftp = {
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
