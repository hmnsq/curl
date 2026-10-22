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
#समावेश "curl_setup.h"
#समावेश "urldata.h"

#अगर_अघोषित CURL_DISABLE_FTP

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "sendf.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "if2ip.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "escape.h"
#समावेश "ftp.h"
#समावेश "ftp-int.h"
#समावेश "ftplistparser.h"
#समावेश "curl_range.h"
#समावेश "strcase.h"
#समावेश "vtls/vtls.h"
#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "connect.h"
#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/inet_pton.h"
#समावेश "select.h"
#समावेश "parsedate.h" /* for the week day and month names */
#समावेश "sockaddr.h" /* required for Curl_sockaddr_storage */
#समावेश "multiif.h"
#समावेश "url.h"
#समावेश "http_proxy.h"
#समावेश "vdns/hostip.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"
#समावेश "curl_ctype.h"

#अगर_अघोषित NI_MAXHOST
#घोषणा NI_MAXHOST 1025
#पूर्ण_अगर
#अगर_अघोषित INET_ADDRSTRLEN
#घोषणा INET_ADDRSTRLEN 16
#पूर्ण_अगर

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#घोषणा STATUSCODE(पंक्ति) \
  (ISDIGIT((पंक्ति)[0]) && ISDIGIT((पंक्ति)[1]) && ISDIGIT((पंक्ति)[2]))

/* macro to check for the last line in an FTP server response */
#घोषणा LASTLINE(पंक्ति) (STATUSCODE(पंक्ति) && (' ' == (पंक्ति)[3]))

#अगर_घोषित CURLVERBOSE
/* for tracing purposes */
अटल स्थिर अक्षर * स्थिर ftp_state_names[] = {
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
#घोषणा FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_names[(ftpc)->state] : "???")

#पूर्ण_अगर /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
अटल व्योम ftp_state_निचला(काष्ठा Curl_easy *तथ्य,
                          काष्ठा ftp_conn *ftpc,
                          ftpstate newstate
#अगर_घोषित DEBUGBUILD
                          , पूर्णांक lineno
#पूर्ण_अगर
  )
{
  अगर(ftpc->state != newstate) {
#अगर_घोषित DEBUGBUILD
    NOVERBOSE((व्योम)lineno);
    CURL_TRC_FTP(तथ्य, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#अन्यथा
    CURL_TRC_FTP(तथ्य, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#पूर्ण_अगर
  }
  ftpc->state = newstate;
}

/* Local API functions */
#अगर_अघोषित DEBUGBUILD
#घोषणा ftp_state(x, y, z) ftp_state_निचला(x, y, z)
#अन्यथा /* !DEBUGBUILD */
#घोषणा ftp_state(x, y, z) ftp_state_निचला(x, y, z, __LINE__)
#पूर्ण_अगर /* DEBUGBUILD */

अटल CURLcode ftp_state_mdtm(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp);
अटल CURLcode ftp_state_quote(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ftp_conn *ftpc,
                                काष्ठा FTP *ftp,
                                bool init, ftpstate instate);
अटल CURLcode ftp_nb_type(काष्ठा Curl_easy *तथ्य,
                            काष्ठा ftp_conn *ftpc,
                            काष्ठा FTP *ftp,
                            bool ascii, ftpstate newstate);
अटल CURLcode getftpresponse(काष्ठा Curl_easy *तथ्य, माप_प्रकार *nreadp,
                               पूर्णांक *ftpcodep);

अटल व्योम freedirs(काष्ठा ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = शून्य;
}

अटल माप_प्रकार numof_slashes(स्थिर अक्षर *str)
{
  स्थिर अक्षर *slashPos;
  माप_प्रकार अंक = 0;
  करो {
    slashPos = म_अक्षर(str, '/');
    अगर(slashPos) {
      ++अंक;
      str = slashPos + 1;
    }
  } जबतक(slashPos);
  वापस अंक;
}

#घोषणा FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
अटल CURLcode ftp_parse_url_path(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc,
                                   काष्ठा FTP *ftp)
{
  स्थिर अक्षर *slashPos = शून्य;
  स्थिर अक्षर *fileName = शून्य;
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *rawPath = शून्य; /* URL-decoded "raw" path */
  माप_प्रकार pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  अगर(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  परिणाम = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  अगर(परिणाम) {
    failf(तथ्य, "path contains control characters");
    वापस परिणाम;
  }
  rawPath = ftpc->rawpath;

  चयन(तथ्य->set.ftp_filemethod) {
  हाल FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    अगर((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    अवरोध;

  हाल FTPFILE_SINGLECWD:
    slashPos = म_खोजप(rawPath, '/');
    अगर(slashPos) {
      /* get path before last slash, except for / */
      माप_प्रकार dirlen = slashPos - rawPath;
      अगर(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, माप(ftpc->dirs[0]));
      अगर(!ftpc->dirs)
        वापस CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (पूर्णांक)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    अन्यथा
      fileName = rawPath; /* filename only (or empty) */
    अवरोध;

  शेष: /* allow pretty much anything */
  हाल FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    स्थिर अक्षर *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    माप_प्रकार dirAlloc = numof_slashes(rawPath);

    अगर(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      वापस CURLE_URL_MALFORMAT;

    अगर(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, माप(ftpc->dirs[0]));
      अगर(!ftpc->dirs)
        वापस CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      जबतक(dirAlloc--) {
        स्थिर अक्षर *spos = म_अक्षर(curPos, '/');
        माप_प्रकार clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        अगर(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        अगर(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (पूर्णांक)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (पूर्णांक)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    अवरोध;
  } /* switch */

  अगर(fileName && *fileName)
    ftpc->file = fileName;
  अन्यथा
    ftpc->file = शून्य; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  अगर(तथ्य->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(तथ्य, "Uploading to a URL without a filename");
    वापस CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  अगर((तथ्य->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  अन्यथा { /* newly created FTP connections are already in entry path */
    स्थिर अक्षर *oldPath = तथ्य->conn->bits.reuse ? ftpc->prevpath : "";
    अगर(oldPath) {
      माप_प्रकार n = pathLen;
      अगर(तथ्य->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      अन्यथा
        n -= ftpc->file ? म_माप(ftpc->file) : 0;

      अगर((म_माप(oldPath) == n) && rawPath && !म_भेदन(rawPath, oldPath, n)) {
        infof(तथ्य, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
अटल पूर्णांक ftp_need_type(काष्ठा ftp_conn *ftpc,
                         bool ascii_wanted)
{
  वापस ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

अटल व्योम close_secondarysocket(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ftp_conn *ftpc)
{
  (व्योम)ftpc;
  CURL_TRC_FTP(तथ्य, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(तथ्य, तथ्य->conn, SECONDARYSOCKET);
}

#अगर_घोषित CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
काष्ठा ftp_cw_lc_ctx {
  काष्ठा Curl_cwriter super;
  bool newline_pending;
};

अटल CURLcode ftp_cw_lc_write(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                स्थिर अक्षर *buf, माप_प्रकार blen)
{
  अटल स्थिर अक्षर nl = '\n';
  काष्ठा ftp_cw_lc_ctx *ctx = writer->ctx;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);

  अगर(!ftpc)
    वापस CURLE_FAILED_INIT;

  अगर(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  जबतक(blen) {
    /* do not pass EOS when writing parts */
    पूर्णांक chunk_type = (type & ~CLIENTWRITE_EOS);
    स्थिर अक्षर *cp;
    माप_प्रकार chunk_len;
    CURLcode परिणाम;

    अगर(ctx->newline_pending) {
      अगर(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, chunk_type, &nl, 1);
        अगर(परिणाम)
          वापस परिणाम;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = स_प्रथम(buf, '\r', blen);
    अगर(!cp)
      अवरोध;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    अगर(chunk_len) {
      परिणाम = Curl_cwriter_write(तथ्य, writer->अग्र, chunk_type,
                                  buf, chunk_len);
      अगर(परिणाम)
        वापस परिणाम;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  अगर(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, blen);
  }
  अन्यथा अगर(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    अगर(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, 0);
  }
  वापस CURLE_OK;
}

अटल स्थिर काष्ठा Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  शून्य,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  माप(काष्ठा ftp_cw_lc_ctx)
};

#पूर्ण_अगर /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
अटल CURLcode ftp_check_ctrl_on_तथ्य_wait(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ftp_conn *ftpc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  काष्ठा pingpong *pp = &ftpc->pp;
  माप_प्रकार nread;
  पूर्णांक ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  अगर(curlx_dyn_len(&pp->recvbuf)) {
    स्थिर अक्षर *l = curlx_dyn_ptr(&pp->recvbuf);
    अगर(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(तथ्य, "There is negative response in cache while serv connect");
      (व्योम)getftpresponse(तथ्य, &nread, &ftpcode);
      वापस CURLE_FTP_ACCEPT_FAILED;
    }
  }

  अगर(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  अन्यथा {
    पूर्णांक socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    चयन(socketstate) {
    हाल -1: /* error */
      /* let's die here */
      failf(तथ्य, "Error while waiting for server connect");
      वापस CURLE_FTP_ACCEPT_FAILED;
    शेष:
      अगर(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      अवरोध;
    }
  }

  अगर(response) {
    infof(तथ्य, "Ctrl conn has data while waiting for data conn");
    अगर(pp->overflow > 3) {
      स्थिर अक्षर *r = curlx_dyn_ptr(&pp->recvbuf);
      माप_प्रकार len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      अगर((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        अगर(!curlx_str_संख्या(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(तथ्य, "Got 226 before data activity");
          वापस CURLE_OK;
        }
      }
    }

    (व्योम)getftpresponse(तथ्य, &nread, &ftpcode);

    infof(तथ्य, "FTP code: %03d", ftpcode);

    अगर(ftpcode / 100 > 3)
      वापस CURLE_FTP_ACCEPT_FAILED;

    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
अटल CURLcode ftp_initiate_transfer(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(तथ्य, "ftp_initiate_transfer()");
  परिणाम = Curl_conn_connect(तथ्य, SECONDARYSOCKET, TRUE, &connected);
  अगर(परिणाम || !connected)
    वापस परिणाम;

  अगर(तथ्य->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(तथ्य, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(तथ्य, TRUE, TRUE);
  }
  अन्यथा {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(तथ्य, SECONDARYSOCKET, तथ्य->req.आकार);
    Curl_xfer_set_shutdown(तथ्य, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(तथ्य, ftpc, FTP_STOP);

  वापस CURLE_OK;
}

अटल bool ftp_endofresp(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                          स्थिर अक्षर *पंक्ति, माप_प्रकार len, पूर्णांक *code)
{
  curl_off_t status;
  (व्योम)तथ्य;
  (व्योम)conn;

  अगर((len > 3) && LASTLINE(पंक्ति) && !curlx_str_संख्या(&पंक्ति, &status, 999)) {
    *code = (पूर्णांक)status;
    वापस TRUE;
  }

  वापस FALSE;
}

अटल CURLcode ftp_readresp(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ftp_conn *ftpc,
                             int8_t sockindex,
                             काष्ठा pingpong *pp,
                             पूर्णांक *ftpcodep, /* return the ftp-code if done */
                             माप_प्रकार *आकार) /* size of the response */
{
  पूर्णांक code;
  CURLcode परिणाम = Curl_pp_readresp(तथ्य, sockindex, pp, &code, आकार);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  अगर(!ftpc->shutdown)
    तथ्य->info.httpcode = code;

  *ftpcodep = code;

  अगर(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(तथ्य, "We got a 421 - timeout");
    ftp_state(तथ्य, ftpc, FTP_STOP);
    वापस CURLE_OPERATION_TIMEDOUT;
  }

  वापस परिणाम;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
अटल CURLcode getftpresponse(काष्ठा Curl_easy *तथ्य,
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

  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  काष्ठा pingpong *pp = &ftpc->pp;
  माप_प्रकार nread;
  पूर्णांक cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(तथ्य, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  अगर(!ftpc)
    वापस CURLE_FAILED_INIT;

  जबतक(!*ftpcodep && !परिणाम) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(तथ्य, pp);
    timediff_t interval_ms;

    अगर(timeout <= 0) {
      failf(तथ्य, "FTP response timeout");
      वापस CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    अगर(timeout < interval_ms)
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

    अगर(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    अन्यथा अगर(!Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(तथ्य, pp) ?
        sockfd : CURL_SOCKET_BAD;
      पूर्णांक ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      अगर(ev < 0) {
        failf(तथ्य, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        वापस CURLE_RECV_ERROR;
      }
      अन्यथा अगर(ev == 0) {
        परिणाम = Curl_pgrsUpdate(तथ्य);
        जारी; /* continue in our loop for the timeout duration */
      }
    }

    अगर(Curl_pp_needs_flush(तथ्य, pp)) {
      परिणाम = Curl_pp_flushsend(तथ्य, pp);
      अगर(परिणाम)
        अवरोध;
    }

    परिणाम = ftp_readresp(तथ्य, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    अगर(परिणाम)
      अवरोध;

    अगर(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
तथ्य*/
      cache_skip++;
    अन्यथा
      /* when we got data or there is no cache left, we reset the cache skip
गणक*/
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(तथ्य, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (पूर्णांक)परिणाम, *nreadp, *ftpcodep);

  वापस परिणाम;
}

अटल CURLcode ftp_state_user(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा connectdata *conn)
{
  CURLcode परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  अगर(!परिणाम) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(तथ्य, ftpc, FTP_USER);
  }
  वापस परिणाम;
}

अटल CURLcode ftp_state_pwd(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ftp_conn *ftpc)
{
  CURLcode परिणाम;
#अगर_घोषित DEBUGBUILD
  अगर(!तथ्य->id && दो_पर्या("CURL_FTP_PWD_STOP"))
    वापस CURLE_OK;
#पूर्ण_अगर
  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "PWD");
  अगर(!परिणाम)
    ftp_state(तथ्य, ftpc, FTP_PWD);

  वापस परिणाम;
}

/* For the FTP "protocol connect" and "doing" phases only */
अटल CURLcode ftp_pollset(काष्ठा Curl_easy *तथ्य,
                            काष्ठा easy_pollset *ps)
{
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  वापस ftpc ? Curl_pp_pollset(तथ्य, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
अटल CURLcode ftp_domore_pollset(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा easy_pollset *ps)
{
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);

  अगर(!ftpc)
    वापस CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(तथ्य, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  अगर(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(तथ्य->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (तथ्य->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(तथ्य->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    वापस Curl_pollset_add_in(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
  }
  वापस Curl_pp_pollset(तथ्य, &ftpc->pp, ps);
}

अटल पूर्णांक pathlen(काष्ठा ftp_conn *ftpc, पूर्णांक अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  वापस ftpc->dirs[अंक].len;
}

अटल स्थिर अक्षर *pathpiece(काष्ठा ftp_conn *ftpc, पूर्णांक अंक)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अंक);
  वापस &ftpc->rawpath[ftpc->dirs[अंक].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
अटल CURLcode ftp_state_cwd(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ftp_conn *ftpc,
                              काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(ftpc->cwddone)
    /* already done and fine */
    परिणाम = ftp_state_mdtm(तथ्य, ftpc, ftp);
  अन्यथा {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((तथ्य->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    अगर(तथ्य->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "CWD %s", ftpc->entrypath);
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_CWD);
    }
    अन्यथा {
      अगर(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        अगर(!परिणाम)
          ftp_state(तथ्य, ftpc, FTP_CWD);
      }
      अन्यथा {
        /* No CWD necessary */
        परिणाम = ftp_state_mdtm(तथ्य, ftpc, ftp);
      }
    }
  }
  वापस परिणाम;
}

प्रकार क्रमागत {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
अटल CURLcode ftp_port_parse_string(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn,
                                      स्थिर अक्षर *string_ftpport,
                                      काष्ठा Curl_sockaddr_storage *ss,
                                      अचिन्हित लघु *port_minp,
                                      अचिन्हित लघु *port_maxp,
                                      स्थिर अक्षर **hostp,
                                      अक्षर *hbuf, माप_प्रकार hbuflen)
{
  स्थिर अक्षर *ip_end = शून्य;
  स्थिर अक्षर *addr = शून्य;
  माप_प्रकार addrlen = 0;
  अचिन्हित लघु port_min = 0;
  अचिन्हित लघु port_max = 0;
  अक्षर ipstr[50];
#अगर_अघोषित USE_IPV6
  (व्योम)conn;
  (व्योम)ss;
#पूर्ण_अगर

  /* default to nothing */
  *hostp = शून्य;
  *port_minp = *port_maxp = 0;

  अगर(!string_ftpport || (म_माप(string_ftpport) <= 1))
    जाओ done;

#अगर_घोषित USE_IPV6
  अगर(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    स्थिर अक्षर *ip_start = string_ftpport + 1;
    ip_end = म_अक्षर(ip_start, ']');
    अगर(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  अन्यथा
#पूर्ण_अगर
    अगर(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    अन्यथा {
      ip_end = म_अक्षर(string_ftpport, ':');
      addr = string_ftpport;
      अगर(ip_end) {
#अगर_घोषित USE_IPV6
        काष्ठा sockaddr_in6 * स्थिर sa6 = (व्योम *)ss;
#पूर्ण_अगर
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#अगर_घोषित USE_IPV6
        अगर(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = म_माप(string_ftpport);
          ip_end = शून्य; /* this got no port ! */
        }
#पूर्ण_अगर
      }
      अन्यथा
        /* ipv4|interface */
        addrlen = म_माप(string_ftpport);
    }

  /* parse the port */
  अगर(ip_end) {
    स्थिर अक्षर *portp = म_अक्षर(ip_end, ':');
    अगर(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      अगर(!curlx_str_संख्या(&portp, &start, 0xffff)) {
        port_min = (अचिन्हित लघु)start;
        अगर(!curlx_str_single(&portp, '-') &&
           !curlx_str_संख्या(&portp, &end, 0xffff))
          port_max = (अचिन्हित लघु)end;
        अन्यथा
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  अगर(port_min > port_max)
    port_min = port_max = 0;

  अगर(addrlen) {
    स्थिर काष्ठा Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(तथ्य, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    अगर(!remote_addr || (addrlen >= माप(ipstr)) || (addrlen >= hbuflen))
      वापस CURLE_FTP_PORT_FAILED;
    स_नकल(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    चयन(Curl_if2ip(remote_addr->family,
#अगर_घोषित USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#पूर्ण_अगर
                      ipstr, hbuf, hbuflen)) {
    हाल IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      स_नकल(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      अवरोध;
    हाल IF2IP_AF_NOT_SUPPORTED:
      वापस CURLE_FTP_PORT_FAILED;
    हाल IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      अवरोध;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  वापस CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
अटल CURLcode ftp_port_default_host(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn,
                                      काष्ठा Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      स्थिर अक्षर **hostp,
                                      अक्षर *hbuf, माप_प्रकार hbuflen,
                                      bool *non_localp)
{
  काष्ठा sockaddr *sa = (काष्ठा sockaddr *)ss;
  काष्ठा sockaddr_in * स्थिर sa4 = (व्योम *)sa;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 * स्थिर sa6 = (व्योम *)sa;
#पूर्ण_अगर
  अक्षर बफर[STRERROR_LEN];
  CURLcode परिणाम;

  *sslenp = माप(*ss);
  अगर(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(तथ्य, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
    वापस CURLE_FTP_PORT_FAILED;
  }
  चयन(sa->sa_family) {
#अगर_घोषित USE_IPV6
  हाल AF_INET6:
    परिणाम = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    अवरोध;
#पूर्ण_अगर
  शेष:
    परिणाम = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    अवरोध;
  }
  अगर(परिणाम)
    वापस CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  वापस CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
अटल CURLcode ftp_port_resolve_host(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn,
                                      स्थिर अक्षर *host,
                                      काष्ठा Curl_dns_entry **dns_entryp,
                                      स्थिर काष्ठा Curl_addrinfo **resp)
{
  CURLcode परिणाम;

  *resp = शून्य;
  परिणाम = Curl_resolv_blocking(
    तथ्य, Curl_resolv_dns_queries(तथ्य, conn->ip_version),
    host, 0, Curl_conn_get_transport(तथ्य, conn), dns_entryp);
  अगर(परिणाम)
    failf(तथ्य, "failed to resolve the address provided to PORT: %s", host);
  अन्यथा {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  वापस परिणाम;
}

/*
 * Open a TCP socket for the resolved address family.
 */
अटल CURLcode ftp_port_open_socket(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn,
                                     स्थिर काष्ठा Curl_addrinfo *res,
                                     स्थिर काष्ठा Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  अक्षर बफर[STRERROR_LEN];
  पूर्णांक sockerr = 0;
  स्थिर काष्ठा Curl_addrinfo *ai;
  CURLcode परिणाम = CURLE_FTP_PORT_FAILED;

  क्रम(ai = res; ai; ai = ai->ai_अग्र) {
    परिणाम =
      Curl_socket_open(तथ्य, ai, शून्य,
                       Curl_conn_get_transport(तथ्य, conn), portsockp);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        वापस परिणाम;
      परिणाम = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      जारी;
    }
    अवरोध;
  }
  अगर(!ai) {
    failf(तथ्य, "socket failure: %s",
          curlx_strerror(sockerr, बफर, माप(बफर)));
    वापस CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  वापस परिणाम;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
अटल CURLcode ftp_port_bind_socket(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn,
                                     curl_socket_t portsock,
                                     स्थिर काष्ठा Curl_addrinfo *ai,
                                     काष्ठा Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     अचिन्हित लघु port_min,
                                     अचिन्हित लघु port_max,
                                     bool non_local)
{
  काष्ठा sockaddr *sa = (काष्ठा sockaddr *)ss;
  काष्ठा sockaddr_in * स्थिर sa4 = (व्योम *)sa;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 * स्थिर sa6 = (व्योम *)sa;
#पूर्ण_अगर
  अक्षर बफर[STRERROR_LEN];
  अचिन्हित लघु port;
  पूर्णांक sockerr;

  स_नकल(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  क्रम(port = port_min; port <= port_max;) {
    अगर(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#अगर_घोषित USE_IPV6
    अन्यथा
      sa6->sin6_port = htons(port);
#पूर्ण_अगर
    अगर(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      अगर(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(तथ्य, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, बफर, माप(बफर)));

        *sslen_io = माप(*ss);
        अगर(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(तथ्य, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
          वापस CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        जारी;
      }
      अगर(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(तथ्य, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, बफर, माप(बफर)));
        वापस CURLE_FTP_PORT_FAILED;
      }
    }
    अन्यथा
      अवरोध;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    अगर(port == port_max) {
      failf(तथ्य, "bind() failed, ran out of ports");
      वापस CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = माप(*ss);
  अगर(getsockname(portsock, sa, sslen_io)) {
    failf(तथ्य, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
    वापस CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(तथ्य, "ftp_port_bind_socket(), socket bound to port %d", port);
  वापस CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
अटल CURLcode ftp_port_listen(काष्ठा Curl_easy *तथ्य, curl_socket_t portsock)
{
  अक्षर बफर[STRERROR_LEN];

  अगर(listen(portsock, 1)) {
    failf(तथ्य, "socket failure: %s",
          curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
    वापस CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(तथ्य, "ftp_port_listen(), listening on port");
  वापस CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
अटल CURLcode ftp_port_send_command(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ftp_conn *ftpc,
                                      काष्ठा connectdata *conn,
                                      काष्ठा Curl_sockaddr_storage *ss,
                                      स्थिर काष्ठा Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  अटल स्थिर अक्षर mode[][5] = { "EPRT", "PORT" };
  काष्ठा sockaddr *sa = (काष्ठा sockaddr *)ss;
  काष्ठा sockaddr_in * स्थिर sa4 = (व्योम *)sa;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 * स्थिर sa6 = (व्योम *)sa;
#पूर्ण_अगर
  अक्षर myhost[MAX_IPADR_LEN + 1] = "";
  अचिन्हित लघु port;
  CURLcode परिणाम;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, माप(myhost));

#अगर_घोषित USE_IPV6
  अगर(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#पूर्ण_अगर

  क्रम(; fcmd != DONE; fcmd++) {

    अगर(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      जारी;

    अगर((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      जारी;

    चयन(sa->sa_family) {
    हाल AF_INET:
      port = ntohs(sa4->sin_port);
      अवरोध;
#अगर_घोषित USE_IPV6
    हाल AF_INET6:
      port = ntohs(sa6->sin6_port);
      अवरोध;
#पूर्ण_अगर
    शेष:
      जारी; /* might as well skip this */
    }

    अगर(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      अगर(परिणाम) {
        failf(तथ्य, "Failure sending EPRT command: %s",
              curl_easy_strerror(परिणाम));
        वापस परिणाम;
      }
      अवरोध;
    }
    अगर(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      अक्षर target[माप(myhost) + 20];
      स्थिर अक्षर *source = myhost;
      अक्षर *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      जबतक(*source) {
        अगर(*source == '.')
          *dest = ',';
        अन्यथा
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (पूर्णांक)(port >> 8), (पूर्णांक)(port & 0xff));

      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s %s", mode[fcmd], target);
      अगर(परिणाम) {
        failf(तथ्य, "Failure sending PORT command: %s",
              curl_easy_strerror(परिणाम));
        वापस परिणाम;
      }
      अवरोध;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(तथ्य, ftpc, FTP_PORT);
  वापस CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
अटल CURLcode ftp_state_use_port(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode परिणाम = CURLE_FTP_PORT_FAILED;
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  काष्ठा Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  अक्षर hbuf[NI_MAXHOST];
  स्थिर अक्षर *host = शून्य;
  स्थिर अक्षर *string_ftpport = CURL_EASY_STR(तथ्य, STRING_FTPPORT);
  काष्ठा Curl_dns_entry *dns_entry = शून्य;
  स्थिर काष्ठा Curl_addrinfo *res = शून्य;
  स्थिर काष्ठा Curl_addrinfo *ai = शून्य;
  अचिन्हित लघु port_min = 0;
  अचिन्हित लघु port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  परिणाम = ftp_port_parse_string(तथ्य, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, माप(hbuf));
  अगर(!परिणाम && !host)
    /* if no host was specified, use the control connection's local IP */
    परिणाम = ftp_port_default_host(तथ्य, conn, &ss, &sslen, &host,
                                   hbuf, माप(hbuf), &non_local);

  /* resolve host string to address list */
  अगर(!परिणाम)
    परिणाम = ftp_port_resolve_host(तथ्य, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  अगर(!परिणाम)
    परिणाम = ftp_port_open_socket(तथ्य, conn, res, &ai, &portsock);
  अगर(!परिणाम) {
    CURL_TRC_FTP(तथ्य, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    परिणाम = ftp_port_bind_socket(तथ्य, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  अगर(!परिणाम)
    परिणाम = ftp_port_listen(तथ्य, portsock);

  /* send the PORT / EPRT command */
  अगर(!परिणाम)
    परिणाम = ftp_port_send_command(तथ्य, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  अगर(!परिणाम)
    परिणाम = Curl_conn_tcp_listen_set(तथ्य, conn, SECONDARYSOCKET, &portsock);

  अगर(!परिणाम)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  अगर(dns_entry)
    Curl_dns_entry_unlink(तथ्य, &dns_entry);
  अगर(परिणाम) {
    ftp_state(तथ्य, ftpc, FTP_STOP);
  }
  अन्यथा {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    अगर(conn->bits.ftp_use_तथ्य_ssl && तथ्य->set.ftp_use_port &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      परिणाम = Curl_ssl_cfilter_add(
        तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->bits.do_more = FALSE;
    Curl_expire(तथ्य, (तथ्य->set.accepttimeout > 0) ?
                तथ्य->set.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  अगर(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(तथ्य, conn, portsock);
  वापस परिणाम;
}

अटल CURLcode ftp_state_use_pasv(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc,
                                   काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  अटल स्थिर अक्षर mode[][5] = { "EPSV", "PASV" };
  पूर्णांक modeoff;

#अगर_घोषित PF_INET6
  अगर(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#पूर्ण_अगर

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", mode[modeoff]);
  अगर(!परिणाम) {
    ftpc->count1 = modeoff;
    ftp_state(तथ्य, ftpc, FTP_PASV);
    infof(तथ्य, "Connect data stream passively");
  }
  वापस परिणाम;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
अटल CURLcode ftp_state_prepare_transfer(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ftp_conn *ftpc,
                                           काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  अगर(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(तथ्य, ftpc, FTP_RETR_PREQUOTE);
    परिणाम = ftp_state_quote(तथ्य, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  अन्यथा अगर(तथ्य->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    परिणाम = ftp_state_use_port(तथ्य, ftpc, EPRT);
  }
  अन्यथा {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    अगर(तथ्य->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      अगर(!ftpc->file)
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) :
                               (तथ्य->state.list_only ? "NLST" : "LIST"));
      अन्यथा अगर(तथ्य->state.upload)
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "PRET STOR %s", ftpc->file);
      अन्यथा
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "PRET RETR %s", ftpc->file);
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_PRET);
    }
    अन्यथा
      परिणाम = ftp_state_use_pasv(तथ्य, ftpc, conn);
  }
  वापस परिणाम;
}

अटल CURLcode ftp_state_rest(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  अगर((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "REST %d", 0);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_REST);
  }
  अन्यथा
    परिणाम = ftp_state_prepare_transfer(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

अटल CURLcode ftp_state_आकार(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  अगर((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "SIZE %s", ftpc->file);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_आकार);
  }
  अन्यथा
    परिणाम = ftp_state_rest(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

अटल CURLcode ftp_state_list(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  स्थिर अक्षर *lstArg = शून्य;
  पूर्णांक lstArglen = 0;
  अक्षर *cmd;

  अगर((तथ्य->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    स्थिर अक्षर *rawPath = ftpc->rawpath;
    स्थिर अक्षर *slashPos = म_खोजप(rawPath, '/');
    अगर(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      माप_प्रकार n = slashPos - rawPath;
      अगर(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (पूर्णांक)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) :
                      (तथ्य->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  अगर(!cmd)
    वापस CURLE_OUT_OF_MEMORY;

  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  अगर(!परिणाम)
    ftp_state(तथ्य, ftpc, FTP_LIST);

  वापस परिणाम;
}

अटल CURLcode ftp_state_list_prequote(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ftp_conn *ftpc,
                                        काष्ठा FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  वापस ftp_state_quote(तथ्य, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

अटल CURLcode ftp_state_retr_prequote(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ftp_conn *ftpc,
                                        काष्ठा FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  वापस ftp_state_quote(तथ्य, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

अटल CURLcode ftp_state_stor_prequote(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ftp_conn *ftpc,
                                        काष्ठा FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  वापस ftp_state_quote(तथ्य, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

अटल CURLcode ftp_state_type(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  अगर(तथ्य->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)तथ्य->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    परिणाम = ftp_nb_type(तथ्य, ftpc, ftp, (bool)तथ्य->state.prefer_ascii,
                         FTP_TYPE);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
    परिणाम = ftp_state_आकार(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
अटल CURLcode ftp_state_mdtm(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  अगर((तथ्य->set.get_filetime || तथ्य->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "MDTM %s", ftpc->file);

    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_MDTM);
  }
  अन्यथा
    परिणाम = ftp_state_type(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

/* This is called after the TYPE and possible quote commands have been sent */
अटल CURLcode ftp_state_ul_setup(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc,
                                   काष्ठा FTP *ftp,
                                   bool sizechecked)
{
  CURLcode परिणाम = CURLE_OK;
  curl_bit append = तथ्य->set.remote_append;

  अगर((तथ्य->state.resume_from && !sizechecked) ||
     ((तथ्य->state.resume_from > 0) && sizechecked)) {
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

    अगर(तथ्य->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "SIZE %s", ftpc->file);
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_STOR_आकार);
      वापस परिणाम;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    अगर(तथ्य->set.seek_func) {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_seek_func);
      seekerr = तथ्य->set.seek_func(तथ्य->set.seek_client,
                                    तथ्य->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    अगर(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      अगर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(तथ्य, "Could not seek stream");
        वापस CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करो {
        अक्षर scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (तथ्य->state.resume_from - passed > (curl_off_t)माप(scratch)) ?
          माप(scratch) :
          curlx_sotouz(तथ्य->state.resume_from - passed);

        माप_प्रकार actuallyread =
          तथ्य->state.fread_func(scratch, 1, readthisamountnow,
                                 तथ्य->state.in);

        passed += actuallyread;
        अगर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(तथ्य, "Failed to read data");
          वापस CURLE_FTP_COULDNT_USE_REST;
        }
      } जबतक(passed < तथ्य->state.resume_from);
    }
    /* now, decrease the size of the read */
    अगर(तथ्य->state.infilesize > 0) {
      तथ्य->state.infilesize -= तथ्य->state.resume_from;

      अगर(तथ्य->state.infilesize <= 0) {
        infof(तथ्य, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(तथ्य);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(तथ्य, ftpc, FTP_STOP);
        वापस CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  अगर(!परिणाम)
    ftp_state(तथ्य, ftpc, FTP_STOR);

  वापस परिणाम;
}

अटल CURLcode ftp_state_retr(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ftp_conn *ftpc,
                               काष्ठा FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_FTP(तथ्य, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  अगर(तथ्य->set.max_filesize && (filesize > तथ्य->set.max_filesize)) {
    failf(तथ्य, "Maximum file size exceeded");
    वापस CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  अगर(तथ्य->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    अगर(filesize == -1) {
      infof(तथ्य, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    अन्यथा {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      अगर(तथ्य->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        अगर(filesize < -तथ्य->state.resume_from) {
          failf(तथ्य, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                तथ्य->state.resume_from, filesize);
          वापस CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -तथ्य->state.resume_from;
        /* download from where? */
        तथ्य->state.resume_from = filesize - ftp->downloadsize;
      }
      अन्यथा {
        अगर(filesize < तथ्य->state.resume_from) {
          failf(तथ्य, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                तथ्य->state.resume_from, filesize);
          वापस CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - तथ्य->state.resume_from;
      }
    }

    अगर(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(तथ्य);
      infof(तथ्य, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(तथ्य, ftpc, FTP_STOP);
      वापस CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(तथ्य, "Instructs server to resume from offset %" FMT_OFF_T,
          तथ्य->state.resume_from);

    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "REST %" FMT_OFF_T,
                           तथ्य->state.resume_from);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_RETR_REST);
  }
  अन्यथा {
    /* no resume */
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "RETR %s", ftpc->file);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_RETR);
  }

  वापस परिणाम;
}

अटल CURLcode ftp_state_quote(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ftp_conn *ftpc,
                                काष्ठा FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  bool quote = FALSE;
  काष्ठा curl_slist *item;

  चयन(instate) {
  हाल FTP_QUOTE:
  शेष:
    item = तथ्य->set.quote;
    अवरोध;
  हाल FTP_RETR_PREQUOTE:
  हाल FTP_STOR_PREQUOTE:
  हाल FTP_LIST_PREQUOTE:
    item = तथ्य->set.prequote;
    अवरोध;
  हाल FTP_POSTQUOTE:
    item = तथ्य->set.postquote;
    अवरोध;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  अगर(init)
    ftpc->count1 = 0;
  अन्यथा
    ftpc->count1++;

  अगर(item) {
    पूर्णांक i = 0;

    /* Skip count1 items in the linked list */
    जबतक((i < ftpc->count1) && item) {
      item = item->अग्र;
      i++;
    }
    अगर(item) {
      स्थिर अक्षर *cmd = item->तथ्य;
      अगर(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      अन्यथा
        ftpc->count2 = 0; /* failure means cancel operation */

      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", cmd);
      अगर(परिणाम)
        वापस परिणाम;
      ftp_state(तथ्य, ftpc, instate);
      quote = TRUE;
    }
  }

  अगर(!quote) {
    /* No more quote to send, continue to ... */
    चयन(instate) {
    हाल FTP_QUOTE:
    शेष:
      परिणाम = ftp_state_cwd(तथ्य, ftpc, ftp);
      अवरोध;
    हाल FTP_RETR_PREQUOTE:
      अगर(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(तथ्य, ftpc, FTP_STOP);
      अन्यथा {
        अगर(ftpc->known_filesize != -1) {
          Curl_pgrs_Set_Download_आकार(तथ्य, ftpc->known_filesize);
          परिणाम = ftp_state_retr(तथ्य, ftpc, ftp, ftpc->known_filesize);
        }
        अन्यथा {
          अगर(तथ्य->set.ignorecl || तथ्य->state.prefer_ascii) {
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
            परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "RETR %s", ftpc->file);
            अगर(!परिणाम)
              ftp_state(तथ्य, ftpc, FTP_RETR);
          }
          अन्यथा {
            परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "SIZE %s", ftpc->file);
            अगर(!परिणाम)
              ftp_state(तथ्य, ftpc, FTP_RETR_आकार);
          }
        }
      }
      अवरोध;
    हाल FTP_STOR_PREQUOTE:
      परिणाम = ftp_state_ul_setup(तथ्य, ftpc, ftp, FALSE);
      अवरोध;
    हाल FTP_POSTQUOTE:
      अवरोध;
    हाल FTP_LIST_PREQUOTE:
      ftp_state(तथ्य, ftpc, FTP_LIST_TYPE);
      परिणाम = ftp_state_list(तथ्य, ftpc, ftp);
      अवरोध;
    }
  }

  वापस परिणाम;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
अटल CURLcode ftp_epsv_disable(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ftp_conn *ftpc,
                                 काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(तथ्य, "Failed EPSV attempt, exiting");
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  infof(तथ्य, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(तथ्य, ftpc);
  तथ्य->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "PASV");
  अगर(!परिणाम) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(तथ्य, ftpc, FTP_PASV);
  }
  वापस परिणाम;
}

अटल CURLcode ftp_control_addr_dup(काष्ठा Curl_easy *तथ्य, अक्षर **newhostp)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  अन्यथा
#पूर्ण_अगर
  अगर(!Curl_conn_get_ip_info(तथ्य, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  अन्यथा {
    /* failed to get the remote_ip of the DATA connection */
    failf(तथ्य, "unable to get peername of DATA connection");
    *newhostp = शून्य;
    वापस CURLE_FTP_CANT_GET_HOST;
  }
  वापस *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

अटल bool match_pasv_6nums(स्थिर अक्षर *p,
                             अचिन्हित पूर्णांक *array) /* 6 numbers */
{
  पूर्णांक i;
  क्रम(i = 0; i < 6; i++) {
    curl_off_t अंक;
    अगर(i) {
      अगर(*p != ',')
        वापस FALSE;
      p++;
    }
    अगर(curlx_str_संख्या(&p, &अंक, 0xff))
      वापस FALSE;
    array[i] = (अचिन्हित पूर्णांक)अंक;
  }
  वापस TRUE;
}

अटल CURLcode ftp_state_pasv_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम;
  स्थिर काष्ठा pingpong *pp = &ftpc->pp;
  अक्षर *newhost = शून्य;
  अचिन्हित लघु newport = 0;
  स्थिर अक्षर *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  अगर((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    स्थिर अक्षर *ptr = म_अक्षर(str, '(');
    अगर(ptr) {
      अक्षर sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      अगर(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        स्थिर अक्षर *p = &ptr[3];
        curl_off_t अंक;
        अगर(curlx_str_संख्या(&p, &अंक, 0xffff) || (*p != sep)) {
          failf(तथ्य, "Illegal port number in EPSV reply");
          वापस CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (अचिन्हित लघु)अंक;
        परिणाम = ftp_control_addr_dup(तथ्य, &newhost);
        अगर(परिणाम)
          वापस परिणाम;
      }
      अन्यथा
        ptr = शून्य;
    }
    अगर(!ptr) {
      failf(तथ्य, "Weirdly formatted EPSV reply");
      वापस CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  अन्यथा अगर((ftpc->count1 == 1) &&
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
    जबतक(*str) {
      अगर(match_pasv_6nums(str, ip))
        अवरोध;
      str++;
    }

    अगर(!*str) {
      failf(तथ्य, "Could not interpret the 227-response");
      वापस CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    अगर(तथ्य->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(तथ्य, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      परिणाम = ftp_control_addr_dup(तथ्य, &newhost);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    अगर(!newhost)
      वापस CURLE_OUT_OF_MEMORY;

    newport = (अचिन्हित लघु)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  अन्यथा अगर(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    वापस ftp_epsv_disable(तथ्य, ftpc, conn);
  }
  अन्यथा {
    failf(तथ्य, "Bad PASV/EPSV response: %03d", ftpcode);
    वापस CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  परिणाम = Curl_peer_create(तथ्य, conn->scheme, newhost, newport,
                            &conn->origin2);
  अगर(परिणाम)
    जाओ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  अगर(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    परिणाम = Curl_peer_create(तथ्य, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    अगर(परिणाम)
      जाओ error;
  }

  परिणाम = Curl_conn_setup(तथ्य, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_तथ्य_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  अगर(परिणाम) {
    अगर((परिणाम != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      परिणाम = ftp_epsv_disable(तथ्य, ftpc, conn);
    }
    जाओ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#अगर_घोषित CURLVERBOSE
  अगर(तथ्य->set.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(तथ्य, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->port);
  }
#पूर्ण_अगर

  conn->bits.do_more = TRUE;
  ftp_state(तथ्य, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  वापस परिणाम;
}

/* called repeatedly until done from multi.c */
अटल CURLcode ftp_statemach(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ftp_conn *ftpc,
                              bool *done)
{
  CURLcode परिणाम = Curl_pp_statemach(तथ्य, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  वापस परिणाम;
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
अटल CURLcode ftp_do_more(काष्ठा Curl_easy *तथ्य, domore *more)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  काष्ठा FTP *ftp = Curl_meta_get(तथ्य, CURL_META_FTP_EASY);
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  अगर(!ftpc || !ftp)
    वापस CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  अगर(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(तथ्य, SECONDARYSOCKET);
    परिणाम = Curl_conn_connect(तथ्य, SECONDARYSOCKET, FALSE, &connected);
    अगर(परिणाम == CURLE_OUT_OF_MEMORY)
      वापस परिणाम;
    अगर(परिणाम || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(तथ्य, SECONDARYSOCKET))) {
      अगर(परिणाम && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        वापस ftp_epsv_disable(तथ्य, ftpc, conn);
      }
      वापस परिणाम;
    }
  }

  अगर(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    परिणाम = ftp_statemach(तथ्य, ftpc, &complete);

    अगर(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    अगर(परिणाम || !ftpc->wait_तथ्य_conn)
      वापस परिणाम;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  अगर(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    अगर(ftpc->wait_तथ्य_conn) {
      bool serv_conned;

      परिणाम = Curl_conn_connect(तथ्य, SECONDARYSOCKET, FALSE, &serv_conned);
      अगर(परिणाम)
        वापस परिणाम; /* Failed to accept data connection */

      अगर(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_तथ्य_conn = FALSE;
        परिणाम = ftp_initiate_transfer(तथ्य, ftpc);

        अगर(परिणाम)
          वापस परिणाम;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      अन्यथा {
        परिणाम = ftp_check_ctrl_on_तथ्य_wait(तथ्य, ftpc);
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
    अन्यथा अगर(तथ्य->state.upload) {
      परिणाम = ftp_nb_type(तथ्य, ftpc, ftp, (bool)तथ्य->state.prefer_ascii,
                           FTP_STOR_TYPE);
      अगर(परिणाम)
        वापस परिणाम;

      परिणाम = ftp_statemach(तथ्य, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_तथ्य_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    अन्यथा {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      परिणाम = Curl_range(तथ्य);

      अगर(परिणाम == CURLE_OK && तथ्य->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      अगर(परिणाम)
        वापस परिणाम;

      अगर((तथ्य->state.list_only || !ftpc->file) &&
         !तथ्य->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        अगर(ftp->transfer == PPTRANSFER_BODY) {
          परिणाम = ftp_nb_type(तथ्य, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          अगर(परिणाम)
            वापस परिणाम;
        }
        /* otherwise fall through */
      }
      अन्यथा {
        अगर(तथ्य->set.prequote && !ftpc->file) {
          परिणाम = ftp_nb_type(तथ्य, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        अन्यथा {
          परिणाम = ftp_nb_type(तथ्य, ftpc, ftp, (bool)तथ्य->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        अगर(परिणाम)
          वापस परिणाम;
      }

      परिणाम = ftp_statemach(तथ्य, ftpc, &complete);
      अगर(complete)
        *more = DOMORE_DONE;
    }
    वापस परिणाम;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(तथ्य);

  अगर(!ftpc->wait_तथ्य_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(तथ्य, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (पूर्णांक)परिणाम);
  }

  वापस परिणाम;
}

/* call this when the DO phase has completed */
अटल CURLcode ftp_dophase_done(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ftp_conn *ftpc,
                                 काष्ठा FTP *ftp,
                                 bool connected)
{
  अगर(connected) {
    domore completed;
    CURLcode परिणाम = ftp_do_more(तथ्य, &completed);

    अगर(परिणाम) {
      close_secondarysocket(तथ्य, ftpc);
      वापस परिणाम;
    }
  }

  अगर(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(तथ्य);
  अन्यथा अगर(!connected)
    /* since we did not connect now, we want do_more to get called */
    तथ्य->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  वापस CURLE_OK;
}

अटल CURLcode ftp_state_port_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    काष्ठा FTP *ftp,
                                    पूर्णांक ftpcode)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode परिणाम = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  अगर(ftpcode / 100 != 2) {
    /* the command failed */

    अगर(EPRT == fcmd) {
      infof(तथ्य, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    अगर(fcmd == DONE) {
      failf(तथ्य, "Failed to do PORT");
      परिणाम = CURLE_FTP_PORT_FAILED;
    }
    अन्यथा
      /* try next */
      परिणाम = ftp_state_use_port(तथ्य, ftpc, fcmd);
  }
  अन्यथा {
    infof(तथ्य, "Connect data stream actively");
    ftp_state(तथ्य, ftpc, FTP_STOP); /* end of DO phase */
    परिणाम = ftp_dophase_done(तथ्य, ftpc, ftp, FALSE);
  }

  वापस परिणाम;
}

/* return TRUE on error, FALSE on success */
अटल bool twodigit(स्थिर अक्षर *p, पूर्णांक *val)
{
  अगर(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    वापस TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  वापस FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(स्थिर अक्षर *p, पूर्णांक *year, पूर्णांक *month, पूर्णांक *day,
                           पूर्णांक *hour, पूर्णांक *minute, पूर्णांक *second);
UNITTEST bool ftp_213_date(स्थिर अक्षर *p, पूर्णांक *year, पूर्णांक *month, पूर्णांक *day,
                           पूर्णांक *hour, पूर्णांक *minute, पूर्णांक *second)
{
  पूर्णांक century;
  अगर((म_माप(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    वापस FALSE;

  *year += century * 100;
  अगर((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    वापस FALSE;
  वापस TRUE;
}

अटल CURLcode client_write_header(काष्ठा Curl_easy *तथ्य,
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
  CURLcode परिणाम;
  bool save = (bool)तथ्य->set.include_header;
  तथ्य->set.include_header = TRUE;
  परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, buf, blen);
  तथ्य->set.include_header = save;
  वापस परिणाम;
}

अटल CURLcode ftp_state_mdtm_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    काष्ठा FTP *ftp,
                                    पूर्णांक ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

  चयन(ftpcode) {
  हाल 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    पूर्णांक year, month, day, hour, minute, second;
    काष्ठा pingpong *pp = &ftpc->pp;
    स्थिर अक्षर *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    अगर(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      अक्षर timebuf[24];
      curl_msnprintf(timebuf, माप(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      अगर(!Curl_getdate_capped(timebuf, &तथ्य->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#अगर defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#आशय GCC diagnostic धकेलो
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#आशय GCC diagnostic ignored "-Wtype-limits"
#पूर्ण_अगर
    अगर(तथ्य->req.no_body && ftpc->file &&
       तथ्य->set.get_filetime && showtime) {
#अगर defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
      अक्षर headerbuf[128];
      पूर्णांक headerbuflen;
      समय_प्रकार filetime = तथ्य->info.filetime;
      काष्ठा पंचांग बफर;
      स्थिर काष्ठा पंचांग *पंचांग = &बफर;

      परिणाम = curlx_gmtime(filetime, &बफर);
      अगर(परिणाम)
        वापस परिणाम;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, माप(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday-1 : 6],
                       पंचांग->tm_mday,
                       Curl_month[पंचांग->tm_mon],
                       पंचांग->tm_year + 1900,
                       पंचांग->tm_hour,
                       पंचांग->tm_min,
                       पंचांग->tm_sec);
      परिणाम = client_write_header(तथ्य, headerbuf, headerbuflen);
      अगर(परिणाम)
        वापस परिणाम;
    } /* end of a ridiculous amount of conditionals */
  }
    अवरोध;
  शेष:
    infof(तथ्य, "unsupported MDTM reply format");
    अवरोध;
  हाल 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(तथ्य, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    अवरोध;
  }

  अगर(तथ्य->set.timecondition) {
    अगर((तथ्य->info.filetime > 0) && (तथ्य->set.timevalue > 0)) {
      चयन(तथ्य->set.timecondition) {
      हाल CURL_TIMECOND_IFMODSINCE:
      शेष:
        अगर(तथ्य->info.filetime <= तथ्य->set.timevalue) {
          infof(तथ्य, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          तथ्य->info.timecond = TRUE;
          ftp_state(तथ्य, ftpc, FTP_STOP);
          वापस CURLE_OK;
        }
        अवरोध;
      हाल CURL_TIMECOND_IFUNMODSINCE:
        अगर(तथ्य->info.filetime > तथ्य->set.timevalue) {
          infof(तथ्य, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          तथ्य->info.timecond = TRUE;
          ftp_state(तथ्य, ftpc, FTP_STOP);
          वापस CURLE_OK;
        }
        अवरोध;
      } /* switch */
    }
    अन्यथा {
      infof(तथ्य, "Skipping time comparison");
    }
  }

  अगर(!परिणाम)
    परिणाम = ftp_state_type(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

अटल CURLcode ftp_state_type_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    काष्ठा FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(तथ्य, "Could not set desired mode");
    वापस CURLE_FTP_COULDNT_SET_TYPE;
  }
  अगर(ftpcode != 200)
    infof(तथ्य, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  अगर(instate == FTP_TYPE)
    परिणाम = ftp_state_आकार(तथ्य, ftpc, ftp);
  अन्यथा अगर(instate == FTP_LIST_TYPE)
    परिणाम = ftp_state_list(तथ्य, ftpc, ftp);
  अन्यथा अगर(instate == FTP_RETR_TYPE)
    परिणाम = ftp_state_retr_prequote(तथ्य, ftpc, ftp);
  अन्यथा अगर(instate == FTP_STOR_TYPE)
    परिणाम = ftp_state_stor_prequote(तथ्य, ftpc, ftp);
  अन्यथा अगर(instate == FTP_RETR_LIST_TYPE)
    परिणाम = ftp_state_list_prequote(तथ्य, ftpc, ftp);

  वापस परिणाम;
}

अटल CURLcode ftp_state_आकार_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    काष्ठा FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t filesize = -1;
  स्थिर अक्षर *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  माप_प्रकार len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  अगर(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    स्थिर अक्षर *start = &buf[4];
    स्थिर अक्षर *fdigit = स_प्रथम(start, '\r', len - 4);
    अगर(fdigit) {
      fdigit--;
      अगर(*fdigit == '\n')
        fdigit--;
      जबतक(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    अन्यथा
      fdigit = start;
    अगर(curlx_str_संख्या(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  अन्यथा अगर(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_आकार) {
    failf(तथ्य, "The file does not exist");
    वापस CURLE_REMOTE_FILE_NOT_FOUND;
  }

  अगर(instate == FTP_आकार) {
    अगर(filesize != -1) {
      अक्षर clbuf[128];
      पूर्णांक clbuflen = curl_msnprintf(clbuf, माप(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      परिणाम = client_write_header(तथ्य, clbuf, clbuflen);
      अगर(परिणाम)
        वापस परिणाम;
    }
    Curl_pgrs_Set_Download_आकार(तथ्य, filesize);
    परिणाम = ftp_state_rest(तथ्य, ftpc, ftp);
  }
  अन्यथा अगर(instate == FTP_RETR_आकार) {
    Curl_pgrs_Set_Download_आकार(तथ्य, filesize);
    परिणाम = ftp_state_retr(तथ्य, ftpc, ftp, filesize);
  }
  अन्यथा अगर(instate == FTP_STOR_आकार) {
    तथ्य->state.resume_from = filesize;
    परिणाम = ftp_state_ul_setup(तथ्य, ftpc, ftp, TRUE);
  }

  वापस परिणाम;
}

अटल CURLcode ftp_state_rest_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    काष्ठा FTP *ftp,
                                    पूर्णांक ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  चयन(instate) {
  हाल FTP_REST:
  शेष:
    अगर(ftpcode == 350) {
      अक्षर बफर[24] = { "Accept-ranges: bytes\r\n" };
      परिणाम = client_write_header(तथ्य, बफर, म_माप(बफर));
      अगर(परिणाम)
        वापस परिणाम;
    }
    परिणाम = ftp_state_prepare_transfer(तथ्य, ftpc, ftp);
    अवरोध;

  हाल FTP_RETR_REST:
    अगर(ftpcode != 350) {
      failf(तथ्य, "Could not use REST");
      परिणाम = CURLE_FTP_COULDNT_USE_REST;
    }
    अन्यथा {
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "RETR %s", ftpc->file);
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_RETR);
    }
    अवरोध;
  }

  वापस परिणाम;
}

अटल CURLcode ftp_state_stor_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(ftpcode >= 400) {
    failf(तथ्य, "Failed FTP upload: %0d", ftpcode);
    ftp_state(तथ्य, ftpc, FTP_STOP);
    वापस CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  अगर(तथ्य->set.ftp_use_port) {
    bool connected;

    ftp_state(तथ्य, ftpc, FTP_STOP); /* no longer in STOR state */

    परिणाम = Curl_conn_connect(तथ्य, SECONDARYSOCKET, FALSE, &connected);
    अगर(परिणाम)
      वापस परिणाम;

    अगर(!connected) {
      infof(तथ्य, "Data conn was not available immediately");
      ftpc->wait_तथ्य_conn = TRUE;
      वापस ftp_check_ctrl_on_तथ्य_wait(तथ्य, ftpc);
    }
    ftpc->wait_तथ्य_conn = FALSE;
  }
  वापस ftp_initiate_transfer(तथ्य, ftpc);
}

/* for LIST and RETR responses */
अटल CURLcode ftp_state_get_resp(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc,
                                   काष्ठा FTP *ftp,
                                   पूर्णांक ftpcode,
                                   ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  अगर((ftpcode == 150) || (ftpcode == 125)) {

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

    तथ्य->req.आकार = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    अगर((instate != FTP_LIST) &&
       !तथ्य->state.prefer_ascii &&
       !तथ्य->set.ignorecl &&
       (ftp->downloadsize < 1)) {
      /*
       * It seems directory listings either do not show the size or often uses
       * size 0 anyway. ASCII transfers may cause that the transferred amount
       * of data is not the same as this line tells, why using this number in
       * those cases only confuses us.
       *
       * Example D above makes this parsing a little tricky */
      माप_प्रकार len = curlx_dyn_len(&ftpc->pp.recvbuf);
      अगर(len >= 7) { /* "1 bytes" is 7 characters */
        माप_प्रकार i;
        क्रम(i = 0; i < len - 7; i++) {
          curl_off_t what;
          स्थिर अक्षर *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          स्थिर अक्षर *c = &buf[i];
          अगर(!curlx_str_संख्या(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !म_भेदन(c, "bytes", 5)) {
            तथ्य->req.आकार = what;
            अवरोध;
          }
        }
      }
    }
    अन्यथा अगर(ftp->downloadsize > -1)
      तथ्य->req.आकार = ftp->downloadsize;

    अगर(तथ्य->req.आकार > तथ्य->req.maxdownload && तथ्य->req.maxdownload > 0)
      तथ्य->req.आकार = तथ्य->req.maxdownload;
    अन्यथा अगर((instate != FTP_LIST) && तथ्य->state.prefer_ascii)
      तथ्य->req.आकार = -1; /* for servers that understate ASCII mode file
आकार*/

    infof(तथ्य, "Maxdownload = %" FMT_OFF_T, तथ्य->req.maxdownload);

    अगर(instate != FTP_LIST)
      infof(तथ्य, "Getting file with size: %" FMT_OFF_T, तथ्य->req.आकार);

    अगर(तथ्य->set.ftp_use_port) {
      bool connected;

      परिणाम = Curl_conn_connect(तथ्य, SECONDARYSOCKET, FALSE, &connected);
      अगर(परिणाम)
        वापस परिणाम;

      अगर(!connected) {
        infof(तथ्य, "Data conn was not available immediately");
        ftp_state(तथ्य, ftpc, FTP_STOP);
        ftpc->wait_तथ्य_conn = TRUE;
        वापस ftp_check_ctrl_on_तथ्य_wait(तथ्य, ftpc);
      }
      ftpc->wait_तथ्य_conn = FALSE;
    }
    वापस ftp_initiate_transfer(तथ्य, ftpc);
  }
  अन्यथा {
    अगर((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(तथ्य, ftpc, FTP_STOP); /* this phase is over */
    }
    अन्यथा {
      failf(तथ्य, "RETR response: %03d", ftpcode);
      वापस instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  वापस परिणाम;
}

/* after USER, PASS and ACCT */
अटल CURLcode ftp_state_loggedin(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET)) {
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
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "PBSZ %d", 0);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_PBSZ);
  }
  अन्यथा {
    परिणाम = ftp_state_pwd(तथ्य, ftpc);
  }
  वापस परिणाम;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
अटल bool ftp_has_ctrl(स्थिर अक्षर *string)
{
  स्थिर अचिन्हित अक्षर *s = (स्थिर अचिन्हित अक्षर *)string;
  जबतक(*s) {
    अगर(*s < 0x20)
      वापस TRUE;
    s++;
  }
  वापस FALSE;
}

/* for USER and PASS responses */
अटल CURLcode ftp_state_user_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  अगर((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(तथ्य->conn->creds));
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_PASS);
  }
  अन्यथा अगर(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    परिणाम = ftp_state_loggedin(तथ्य, ftpc);
  }
  अन्यथा अगर(ftpcode == 332) {
    स्थिर अक्षर *account = CURL_EASY_STR(तथ्य, STRING_FTP_ACCOUNT);
    अगर(!account) {
      failf(तथ्य, "ACCT requested but none available");
      परिणाम = CURLE_LOGIN_DENIED;
    }
    अन्यथा अगर(ftp_has_ctrl(account)) {
      failf(तथ्य, "Control byte in FTP account");
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अन्यथा {
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "ACCT %s", account);
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_ACCT);
    }
  }
  अन्यथा {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    स्थिर अक्षर *alt = CURL_EASY_STR(तथ्य, STRING_FTP_ALTERNATIVE_TO_USER);
    अगर(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      अगर(ftp_has_ctrl(alt)) {
        failf(तथ्य, "Control byte in FTP alternative-to-user command");
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      अन्यथा {
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", alt);
        अगर(!परिणाम) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(तथ्य, ftpc, FTP_USER);
        }
      }
    }
    अन्यथा {
      failf(तथ्य, "Access denied: %03d", ftpcode);
      परिणाम = CURLE_LOGIN_DENIED;
    }
  }
  वापस परिणाम;
}

/* for ACCT response */
अटल CURLcode ftp_state_acct_resp(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc,
                                    पूर्णांक ftpcode)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(ftpcode != 230) {
    failf(तथ्य, "ACCT rejected by server: %03d", ftpcode);
    परिणाम = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  अन्यथा
    परिणाम = ftp_state_loggedin(तथ्य, ftpc);

  वापस परिणाम;
}

अटल CURLcode ftp_pwd_resp(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ftp_conn *ftpc,
                             पूर्णांक ftpcode)
{
  काष्ठा pingpong *pp = &ftpc->pp;
  CURLcode परिणाम;

  अगर(ftpcode == 257) {
    स्थिर अक्षर *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    काष्ठा dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    जबतक(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    अगर('\"' == *ptr) {
      /* it started good */
      क्रम(ptr++; *ptr; ptr++) {
        अगर('\"' == *ptr) {
          अगर('\"' == ptr[1]) {
            /* "quote-doubling" */
            परिणाम = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          अन्यथा {
            /* end of path */
            अगर(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            अवरोध; /* get out of this loop */
          }
        }
        अन्यथा {
          अगर(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            वापस CURLE_WEIRD_SERVER_REPLY;
          }
          परिणाम = curlx_dyn_addn(&out, ptr, 1);
        }
        अगर(परिणाम) {
          curlx_dyn_free(&out);
          वापस परिणाम;
        }
      }
    }
    अगर(entry_extracted) {
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

      अगर(!ftpc->server_os && dir[0] != '/') {
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "SYST");
        अगर(परिणाम) {
          curlx_dyn_free(&out);
          वापस परिणाम;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(तथ्य, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(तथ्य->state.most_recent_ftp_entrypath);
      तथ्य->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      अगर(!तथ्य->state.most_recent_ftp_entrypath)
        वापस CURLE_OUT_OF_MEMORY;

      अगर(!ftpc->server_os && dir[0] != '/') {
        ftp_state(तथ्य, ftpc, FTP_SYST);
        वापस CURLE_OK;
      }
    }
    अन्यथा {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(तथ्य, "Failed to figure out path");
    }
  }
  ftp_state(तथ्य, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(तथ्य, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  वापस CURLE_OK;
}

अटल स्थिर अक्षर * स्थिर ftpauth[] = { "SSL", "TLS" };

अटल CURLcode ftp_wait_resp(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn,
                              काष्ठा ftp_conn *ftpc,
                              पूर्णांक ftpcode)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    अगर(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      वापस ftp_state_user_resp(तथ्य, ftpc, ftpcode);
  }
  अन्यथा अगर(ftpcode != 220) {
    failf(तथ्य, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  अगर(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    चयन((दीर्घ)तथ्य->set.ftpsslauth) {
    हाल CURLFTPAUTH_DEFAULT:
    हाल CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      अवरोध;
    हाल CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      अवरोध;
    शेष:
      failf(तथ्य, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (पूर्णांक)तथ्य->set.ftpsslauth);
      वापस CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_AUTH);
  }
  अन्यथा
    परिणाम = ftp_state_user(तथ्य, ftpc, conn);
  वापस परिणाम;
}

/* an implicit TLS scheme implies TLS on both connections */
अटल अचिन्हित अक्षर ftp_use_ssl(काष्ठा connectdata *conn,
                                 काष्ठा ftp_conn *ftpc)
{
  अगर(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    वापस CURLUSESSL_ALL;
  वापस ftpc->use_ssl;
}

अटल CURLcode ftp_pp_statemachine(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn)
{
  CURLcode परिणाम;
  पूर्णांक ftpcode;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  काष्ठा FTP *ftp = Curl_meta_get(तथ्य, CURL_META_FTP_EASY);
  काष्ठा pingpong *pp;
  माप_प्रकार nread = 0;

  अगर(!ftpc || !ftp)
    वापस CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  अगर(pp->sendleft)
    वापस Curl_pp_flushsend(तथ्य, pp);

  परिणाम = ftp_readresp(तथ्य, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  अगर(परिणाम || !ftpcode)
    वापस परिणाम;

  /* we have now received a full FTP server response */
  चयन(ftpc->state) {
  हाल FTP_WAIT220:
    परिणाम = ftp_wait_resp(तथ्य, conn, ftpc, ftpcode);
    अवरोध;

  हाल FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    अगर(pp->overflow)
      वापस CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    अगर((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      अगर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        परिणाम = Curl_ssl_cfilter_add(
          तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        अगर(परिणाम) {
          /* we failed and bail out */
          वापस CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, TRUE, &done);
      अगर(!परिणाम) {
        conn->bits.ftp_use_तथ्य_ssl = FALSE; /* clear-text data */
        परिणाम = ftp_state_user(तथ्य, ftpc, conn);
      }
    }
    अन्यथा अगर(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    अन्यथा {
      अगर(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        परिणाम = CURLE_USE_SSL_FAILED;
      अन्यथा
        /* ignore the failure and continue */
        परिणाम = ftp_state_user(तथ्य, ftpc, conn);
    }
    अवरोध;

  हाल FTP_USER:
  हाल FTP_PASS:
    परिणाम = ftp_state_user_resp(तथ्य, ftpc, ftpcode);
    अवरोध;

  हाल FTP_ACCT:
    परिणाम = ftp_state_acct_resp(तथ्य, ftpc, ftpcode);
    अवरोध;

  हाल FTP_PBSZ:
    परिणाम =
      Curl_pp_sendf(तथ्य, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    अगर(!परिणाम)
      ftp_state(तथ्य, ftpc, FTP_PROT);
    अवरोध;

  हाल FTP_PROT:
    अगर(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_तथ्य_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    अन्यथा अगर(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      वापस CURLE_USE_SSL_FAILED;

    अगर(तथ्य->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "CCC");
      अगर(!परिणाम)
        ftp_state(तथ्य, ftpc, FTP_CCC);
    }
    अन्यथा
      परिणाम = ftp_state_pwd(तथ्य, ftpc);
    अवरोध;

  हाल FTP_CCC:
    अगर(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      परिणाम = Curl_ssl_cfilter_remove(तथ्य, FIRSTSOCKET,
                                       (तथ्य->set.ftp_ccc ==
                                        (अचिन्हित अक्षर)CURLFTPSSL_CCC_ACTIVE));
      अगर(परिणाम)
        failf(तथ्य, "Failed to clear the command channel (CCC)");
    }
    अगर(!परिणाम)
      /* Then continue as normal */
      परिणाम = ftp_state_pwd(तथ्य, ftpc);
    अवरोध;

  हाल FTP_PWD:
    परिणाम = ftp_pwd_resp(तथ्य, ftpc, ftpcode);
    अवरोध;

  हाल FTP_SYST:
    अगर(ftpcode == 215) {
      स्थिर अक्षर *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      स्थिर अक्षर *start;
      अक्षर *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      जबतक(*ptr == ' ')
        ptr++;
      क्रम(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      अगर(!os)
        वापस CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      अगर(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        अगर(परिणाम) {
          curlx_free(os);
          वापस परिणाम;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(तथ्य, ftpc, FTP_NAMEFMT);
        अवरोध;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    अन्यथा {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(तथ्य, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(तथ्य, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    अवरोध;

  हाल FTP_NAMEFMT:
    अगर(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(तथ्य, ftpc);
      अवरोध;
    }

    ftp_state(तथ्य, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(तथ्य, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    अवरोध;

  हाल FTP_QUOTE:
  हाल FTP_POSTQUOTE:
  हाल FTP_RETR_PREQUOTE:
  हाल FTP_STOR_PREQUOTE:
  हाल FTP_LIST_PREQUOTE:
    अगर((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(तथ्य, "QUOT command failed with %03d", ftpcode);
      परिणाम = CURLE_QUOTE_ERROR;
    }
    अन्यथा
      परिणाम = ftp_state_quote(तथ्य, ftpc, ftp, FALSE, ftpc->state);
    अवरोध;

  हाल FTP_CWD:
    अगर(ftpcode / 100 != 2) {
      /* failure to CWD there */
      अगर(तथ्य->set.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (तथ्य->set.ftp_create_missing_dirs == 2) ? 1 : 0;

        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        अगर(!परिणाम)
          ftp_state(तथ्य, ftpc, FTP_MKD);
      }
      अन्यथा {
        /* return failure */
        failf(तथ्य, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        परिणाम = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    अन्यथा {
      /* success */
      ftpc->count2 = 0;
      अगर(ftpc->cwdcount >= ftpc->dirdepth)
        परिणाम = ftp_state_mdtm(तथ्य, ftpc, ftp);
      अन्यथा {
        ftpc->cwdcount++;
        /* send next CWD */
        परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    अवरोध;

  हाल FTP_MKD:
    अगर((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(तथ्य, "Failed to MKD dir: %03d", ftpcode);
      परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    }
    अन्यथा {
      ftp_state(तथ्य, ftpc, FTP_CWD);
      /* send CWD */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    अवरोध;

  हाल FTP_MDTM:
    परिणाम = ftp_state_mdtm_resp(तथ्य, ftpc, ftp, ftpcode);
    अवरोध;

  हाल FTP_TYPE:
  हाल FTP_LIST_TYPE:
  हाल FTP_RETR_TYPE:
  हाल FTP_STOR_TYPE:
  हाल FTP_RETR_LIST_TYPE:
    परिणाम = ftp_state_type_resp(तथ्य, ftpc, ftp, ftpcode, ftpc->state);
    अवरोध;

  हाल FTP_आकार:
  हाल FTP_RETR_आकार:
  हाल FTP_STOR_आकार:
    परिणाम = ftp_state_आकार_resp(तथ्य, ftpc, ftp, ftpcode, ftpc->state);
    अवरोध;

  हाल FTP_REST:
  हाल FTP_RETR_REST:
    परिणाम = ftp_state_rest_resp(तथ्य, ftpc, ftp, ftpcode, ftpc->state);
    अवरोध;

  हाल FTP_PRET:
    अगर(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(तथ्य, "PRET command not accepted: %03d", ftpcode);
      वापस CURLE_FTP_PRET_FAILED;
    }
    परिणाम = ftp_state_use_pasv(तथ्य, ftpc, conn);
    अवरोध;

  हाल FTP_PASV:
    परिणाम = ftp_state_pasv_resp(तथ्य, ftpc, ftpcode);
    अवरोध;

  हाल FTP_PORT:
    परिणाम = ftp_state_port_resp(तथ्य, ftpc, ftp, ftpcode);
    अवरोध;

  हाल FTP_LIST:
  हाल FTP_RETR:
    परिणाम = ftp_state_get_resp(तथ्य, ftpc, ftp, ftpcode, ftpc->state);
    अवरोध;

  हाल FTP_STOR:
    परिणाम = ftp_state_stor_resp(तथ्य, ftpc, ftpcode);
    अवरोध;

  हाल FTP_QUIT:
  शेष:
    /* internal error */
    ftp_state(तथ्य, ftpc, FTP_STOP);
    अवरोध;
  }

  वापस परिणाम;
}

/* called repeatedly until done from multi.c */
अटल CURLcode ftp_multi_statemach(काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  वापस ftpc ? ftp_statemach(तथ्य, ftpc, done) : CURLE_FAILED_INIT;
}

अटल CURLcode ftp_block_statemach(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_conn *ftpc)
{
  काष्ठा pingpong *pp = &ftpc->pp;
  CURLcode परिणाम = CURLE_OK;

  जबतक(ftpc->state != FTP_STOP) {
    अगर(ftpc->shutdown)
      CURL_TRC_FTP(तथ्य, "in shutdown, waiting for server response");
    परिणाम = Curl_pp_statemach(तथ्य, pp, TRUE, TRUE /* disconnecting */);
    अगर(परिणाम)
      अवरोध;
  }

  वापस परिणाम;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
अटल CURLcode ftp_connect(काष्ठा Curl_easy *तथ्य,
                            bool *done) /* see description above */
{
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  काष्ठा pingpong *pp;

  *done = FALSE; /* default to not done yet */
  अगर(!ftpc)
    वापस CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  अगर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, TRUE, done);
    अगर(परिणाम)
      वापस परिणाम;
  }

  Curl_pp_init(pp, Curl_pgrs_now(तथ्य)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(तथ्य, ftpc, FTP_WAIT220);

  परिणाम = ftp_statemach(तथ्य, ftpc, done);

  वापस परिणाम;
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
अटल CURLcode ftp_sendquote(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ftp_conn *ftpc,
                              काष्ठा curl_slist *quote)
{
  काष्ठा curl_slist *item;
  काष्ठा pingpong *pp = &ftpc->pp;

  item = quote;
  जबतक(item) {
    अगर(item->तथ्य) {
      माप_प्रकार nread;
      स्थिर अक्षर *cmd = item->तथ्य;
      bool acceptfail = FALSE;
      CURLcode परिणाम;
      पूर्णांक ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      अगर(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", cmd);
      अगर(!परिणाम) {
        pp->response = *Curl_pgrs_now(तथ्य); /* timeout relative now */
        परिणाम = getftpresponse(तथ्य, &nread, &ftpcode);
      }
      अगर(परिणाम)
        वापस परिणाम;

      अगर(!acceptfail && (ftpcode >= 400)) {
        failf(तथ्य, "QUOT string not accepted: %s", cmd);
        वापस CURLE_QUOTE_ERROR;
      }
    }

    item = item->अग्र;
  }

  वापस CURLE_OK;
}

अटल CURLcode ftp_done_status(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  चयन(status) {
  हाल CURLE_BAD_DOWNLOAD_RESUME:
  हाल CURLE_FTP_WEIRD_PASV_REPLY:
  हाल CURLE_FTP_PORT_FAILED:
  हाल CURLE_FTP_ACCEPT_FAILED:
  हाल CURLE_FTP_ACCEPT_TIMEOUT:
  हाल CURLE_FTP_COULDNT_SET_TYPE:
  हाल CURLE_FTP_COULDNT_RETR_FILE:
  हाल CURLE_PARTIAL_FILE:
  हाल CURLE_UPLOAD_FAILED:
  हाल CURLE_REMOTE_ACCESS_DENIED:
  हाल CURLE_FILESIZE_EXCEEDED:
  हाल CURLE_REMOTE_FILE_NOT_FOUND:
  हाल CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  हाल CURLE_OK: /* does not affect the control connection's status */
    अगर(!premature)
      अवरोध;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  शेष:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(तथ्य, "FTP ended with bad error code");
    connclose(तथ्य->conn);
    वापस status;      /* use the already set error code */
  }
  वापस CURLE_OK;
}

अटल व्योम ftp_done_wildcard(काष्ठा Curl_easy *तथ्य, काष्ठा ftp_conn *ftpc)
{
  अगर(तथ्य->state.wildcardmatch) {
    अगर(तथ्य->set.chunk_end && ftpc->file) {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_chunk_end);
      तथ्य->set.chunk_end(तथ्य->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

अटल व्योम ftp_done_path(काष्ठा Curl_easy *तथ्य, काष्ठा ftp_conn *ftpc,
                          CURLcode परिणाम)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(परिणाम) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  अन्यथा { /* remember working directory for connection reuse */
    स्थिर अक्षर *rawPath = ftpc->rawpath;
    अगर(rawPath) {
      अगर((तथ्य->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      अन्यथा {
        माप_प्रकार pathLen = म_माप(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        अगर(!ftpc->cwdfail) {
          अगर(तथ्य->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          अन्यथा
            /* file is URL-decoded */
            pathLen -= ftpc->file ? म_माप(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        अन्यथा
          ftpc->prevpath = शून्य; /* no path */
      }
    }
    अगर(ftpc->prevpath)
      infof(तथ्य, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

अटल CURLcode ftp_done_secondary_socket(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ftp_conn *ftpc,
                                          CURLcode परिणाम)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    अगर(!परिणाम && ftpc->dont_check && तथ्य->req.maxdownload > 0) {
      /* partial download completed */
      परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "ABOR");
      अगर(परिणाम) {
        failf(तथ्य, "Failure sending ABOR command: %s",
              curl_easy_strerror(परिणाम));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(तथ्य, ftpc);
  }
  वापस परिणाम;
}

अटल CURLcode ftp_done_control_reply(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ftp_conn *ftpc,
                                       काष्ठा FTP *ftp, CURLcode परिणाम,
                                       bool premature)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  माप_प्रकार nread;
  पूर्णांक ftpcode;

  अगर(!परिणाम && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(तथ्य); /* timeout relative now */
    परिणाम = getftpresponse(तथ्य, &nread, &ftpcode);

    अगर(!nread && (परिणाम == CURLE_OPERATION_TIMEDOUT)) {
      failf(तथ्य, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    अगर(परिणाम)
      वापस परिणाम;

    अगर(ftpc->dont_check && तथ्य->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(तथ्य, "partial download completed, closing connection");
      connclose(conn);
      वापस परिणाम;
    }

    अगर(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      चयन(ftpcode) {
      हाल 226:
      हाल 250:
        अवरोध;
      हाल 552:
        failf(तथ्य, "Exceeded storage allocation");
        परिणाम = CURLE_REMOTE_DISK_FULL;
        अवरोध;
      शेष:
        failf(तथ्य, "server did not report OK, got %d", ftpcode);
        परिणाम = CURLE_PARTIAL_FILE;
        अवरोध;
      }
    }
  }
  वापस परिणाम;
}

अटल CURLcode ftp_done_check_partial(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ftp_conn *ftpc,
                                       काष्ठा FTP *ftp, CURLcode परिणाम,
                                       bool premature)
{
  अगर(परिणाम || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  अन्यथा अगर(तथ्य->state.upload) {
    अगर((ftp->transfer == PPTRANSFER_BODY) &&
       (तथ्य->state.infilesize != -1) && /* upload with known size */
       ((!तथ्य->set.crlf && !तथ्य->state.prefer_ascii && /* no conversion */
         (तथ्य->state.infilesize != तथ्य->req.writebytecount)) ||
        ((तथ्य->set.crlf || तथ्य->state.prefer_ascii) && /* maybe CRLF conv */
         (तथ्य->state.infilesize > तथ्य->req.writebytecount))
       )) {
      failf(तथ्य, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            तथ्य->req.writebytecount, तथ्य->state.infilesize);
      परिणाम = CURLE_PARTIAL_FILE;
    }
  }
  अन्यथा {
    अगर((तथ्य->req.आकार != -1) &&
       (तथ्य->req.आकार != तथ्य->req.bytecount) &&
       (तथ्य->req.maxdownload != तथ्य->req.bytecount)) {
      failf(तथ्य, "Received only partial file: %" FMT_OFF_T " bytes",
            तथ्य->req.bytecount);
      परिणाम = CURLE_PARTIAL_FILE;
    }
    अन्यथा अगर(!ftpc->dont_check &&
            !तथ्य->req.bytecount &&
            (तथ्य->req.आकार > 0)) {
      failf(तथ्य, "No data was received");
      परिणाम = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  वापस परिणाम;
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
अटल CURLcode ftp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                         bool premature)
{
  काष्ठा FTP *ftp = Curl_meta_get(तथ्य, CURL_META_FTP_EASY);
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  CURLcode परिणाम;

  अगर(!ftp || !ftpc)
    वापस CURLE_OK;

  परिणाम = ftp_done_status(तथ्य, ftpc, status, premature);

  ftp_done_wildcard(तथ्य, ftpc);
  ftp_done_path(तथ्य, ftpc, परिणाम);
  परिणाम = ftp_done_secondary_socket(तथ्य, ftpc, परिणाम);
  परिणाम = ftp_done_control_reply(तथ्य, ftpc, ftp, परिणाम, premature);
  परिणाम = ftp_done_check_partial(तथ्य, ftpc, ftp, परिणाम, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  अगर(!status && !परिणाम && !premature && तथ्य->set.postquote)
    परिणाम = ftp_sendquote(तथ्य, ftpc, तथ्य->set.postquote);
  CURL_TRC_FTP(तथ्य, "[%s] done, result=%d", FTP_CSTATE(ftpc), (पूर्णांक)परिणाम);
  वापस परिणाम;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
अटल CURLcode ftp_nb_type(काष्ठा Curl_easy *तथ्य,
                            काष्ठा ftp_conn *ftpc,
                            काष्ठा FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode परिणाम;
  अक्षर want = (अक्षर)(ascii ? 'A' : 'I');

  अगर(ftpc->transfertype == want) {
    ftp_state(तथ्य, ftpc, newstate);
    वापस ftp_state_type_resp(तथ्य, ftpc, ftp, 200, newstate);
  }

  परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "TYPE %c", want);
  अगर(!परिणाम) {
    ftp_state(तथ्य, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  वापस परिणाम;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
अटल CURLcode ftp_perform(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा ftp_conn *ftpc,
  काष्ठा FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_FTP(तथ्य, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  अगर(तथ्य->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  परिणाम = ftp_state_quote(तथ्य, ftpc, ftp, TRUE, FTP_QUOTE);
  अगर(परिणाम)
    वापस परिणाम;

  /* run the state-machine */
  परिणाम = ftp_statemach(तथ्य, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, SECONDARYSOCKET);

  अगर(*connected)
    infof(तथ्य, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  अन्यथा
    CURL_TRC_FTP(तथ्य, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  अगर(*dophase_done)
    CURL_TRC_FTP(तथ्य, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  वापस परिणाम;
}

अटल व्योम wc_तथ्य_dtor(व्योम *ptr)
{
  काष्ठा ftp_wc *ftpwc = ptr;
  अगर(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_तथ्य_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

अटल CURLcode init_wc_तथ्य(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ftp_conn *ftpc,
                             काष्ठा FTP *ftp)
{
  अक्षर *last_slash;
  अक्षर *path = ftp->path;
  काष्ठा Wildcard_तथ्य *wildcard = तथ्य->wildcard;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ftp_wc *ftpwc = शून्य;

  last_slash = म_खोजप(ftp->path, '/');
  अगर(last_slash) {
    last_slash++;
    अगर(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      वापस ftp_parse_url_path(तथ्य, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    अगर(!wildcard->pattern)
      वापस CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  अन्यथा { /* there is only 'wildcard pattern' or nothing */
    अगर(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      अगर(!wildcard->pattern)
        वापस CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    अन्यथा { /* only list */
      wildcard->state = CURLWC_CLEAN;
      वापस ftp_parse_url_path(तथ्य, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, माप(काष्ठा ftp_wc));
  अगर(!ftpwc) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_तथ्य_alloc();
  अगर(!ftpwc->parser) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_तथ्य_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  अगर(तथ्य->set.ftp_filemethod == FTPFILE_NOCWD)
    तथ्य->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  परिणाम = ftp_parse_url_path(तथ्य, ftpc, ftp);
  अगर(परिणाम) {
    जाओ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  अगर(!wildcard->path) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  infof(तथ्य, "Wildcard - Parsing started");
  वापस CURLE_OK;

fail:
  अगर(ftpwc) {
    Curl_ftp_parselist_तथ्य_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = शून्य;
  वापस परिणाम;
}

अटल CURLcode wc_statemach(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ftp_conn *ftpc,
                             काष्ठा FTP *ftp)
{
  काष्ठा Wildcard_तथ्य * स्थिर wildcard = तथ्य->wildcard;
  CURLcode परिणाम = CURLE_OK;

  क्रम(;;) {
    चयन(wildcard->state) {
    हाल CURLWC_INIT:
      परिणाम = init_wc_तथ्य(तथ्य, ftpc, ftp);
      अगर(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        वापस परिणाम;
      wildcard->state = परिणाम ? CURLWC_ERROR : CURLWC_MATCHING;
      वापस परिणाम;

    हाल CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      काष्ठा ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      अगर(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        जारी;
      }
      अगर(Curl_llist_गिनती(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        वापस CURLE_REMOTE_FILE_NOT_FOUND;
      }
      जारी;
    }

    हाल CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      काष्ठा Curl_llist_गाँठ *शीर्ष = Curl_llist_शीर्ष(&wildcard->filelist);
      काष्ठा curl_fileinfo *finfo = Curl_गाँठ_elem(शीर्ष);
      अक्षर *tmp_path;
      अक्षर *enc = curl_easy_escape(शून्य, finfo->filename, 0);
      अगर(!enc)
        वापस CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      अगर(!tmp_path)
        वापस CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(तथ्य, "Wildcard - START of \"%s\"", finfo->filename);
      अगर(तथ्य->set.chunk_bgn) {
        दीर्घ userresponse;
        काष्ठा Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, तथ्य, easy_chunk_bgn);
        userresponse = तथ्य->set.chunk_bgn(
          finfo, तथ्य->set.wildcardptr,
          (पूर्णांक)Curl_llist_गिनती(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        चयन(userresponse) {
        हाल CURL_CHUNK_BGN_FUNC_SKIP:
          infof(तथ्य, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          जारी;
        हाल CURL_CHUNK_BGN_FUNC_FAIL:
          वापस CURLE_CHUNK_FAILED;
        }
      }

      अगर(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        जारी;
      }

      अगर(finfo->flags & CURLFINFOFLAG_KNOWN_आकार)
        ftpc->known_filesize = finfo->आकार;

      परिणाम = ftp_parse_url_path(तथ्य, ftpc, ftp);
      अगर(परिणाम)
        वापस परिणाम;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_गाँठ_remove(Curl_llist_शीर्ष(&wildcard->filelist));

      अगर(Curl_llist_गिनती(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        वापस CURLE_OK;
      }
      वापस परिणाम;
    }

    हाल CURLWC_SKIP: {
      अगर(तथ्य->set.chunk_end) {
        काष्ठा Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, तथ्य, easy_chunk_end);
        तथ्य->set.chunk_end(तथ्य->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_गाँठ_remove(Curl_llist_शीर्ष(&wildcard->filelist));
      wildcard->state = (Curl_llist_गिनती(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      जारी;
    }

    हाल CURLWC_CLEAN: {
      काष्ठा ftp_wc *ftpwc = wildcard->ftpwc;
      परिणाम = CURLE_OK;
      अगर(ftpwc)
        परिणाम = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = परिणाम ? CURLWC_ERROR : CURLWC_DONE;
      वापस परिणाम;
    }

    हाल CURLWC_DONE:
    हाल CURLWC_ERROR:
    हाल CURLWC_CLEAR:
      अगर(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = शून्य;
      }
      वापस परिणाम;
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
अटल CURLcode ftp_regular_transfer(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ftp_conn *ftpc,
                                     काष्ठा FTP *ftp,
                                     bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;
  तथ्य->req.आकार = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(तथ्य);

  ftpc->ctl_valid = TRUE; /* starts good */

  परिणाम = ftp_perform(तथ्य, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_done); /* all commands in the DO-phase done? */

  अगर(!परिणाम) {

    अगर(!*dophase_done)
      /* the DO phase has not completed yet */
      वापस CURLE_OK;

    परिणाम = ftp_dophase_done(तथ्य, ftpc, ftp, connected);

    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
    freedirs(ftpc);

  वापस परिणाम;
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
अटल CURLcode ftp_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  काष्ठा FTP *ftp = Curl_meta_get(तथ्य, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  अगर(!ftpc || !ftp)
    वापस CURLE_FAILED_INIT;
  ftpc->wait_तथ्य_conn = FALSE; /* default to no such wait */

#अगर_घोषित CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    काष्ठा Curl_cwriter *ftp_lc_writer;

    परिणाम = Curl_cwriter_create(&ftp_lc_writer, तथ्य, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    अगर(परिणाम)
      वापस परिणाम;

    परिणाम = Curl_cwriter_add(तथ्य, ftp_lc_writer);
    अगर(परिणाम) {
      Curl_cwriter_free(तथ्य, ftp_lc_writer);
      वापस परिणाम;
    }
  }
#पूर्ण_अगर /* CURL_PREFER_LF_LINEENDS */

  अगर(तथ्य->state.wildcardmatch) {
    परिणाम = wc_statemach(तथ्य, ftpc, ftp);
    अगर(तथ्य->wildcard->state == CURLWC_SKIP ||
       तथ्य->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      वापस CURLE_OK;
    }
    अगर(परिणाम) /* error, loop or skipping the file */
      वापस परिणाम;
  }
  अन्यथा { /* no wildcard FSM needed */
    परिणाम = ftp_parse_url_path(तथ्य, ftpc, ftp);
    अगर(परिणाम)
      वापस परिणाम;
  }

  परिणाम = ftp_regular_transfer(तथ्य, ftpc, ftp, done);

  वापस परिणाम;
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
अटल CURLcode ftp_quit(काष्ठा Curl_easy *तथ्य,
                         काष्ठा ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(ftpc->ctl_valid) {
    CURL_TRC_FTP(तथ्य, "sending QUIT to close session");
    परिणाम = Curl_pp_sendf(तथ्य, &ftpc->pp, "%s", "QUIT");
    अगर(परिणाम) {
      failf(तथ्य, "Failure sending QUIT command: %s",
            curl_easy_strerror(परिणाम));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(तथ्य->conn); /* mark for closure */
      ftp_state(तथ्य, ftpc, FTP_STOP);
      वापस परिणाम;
    }

    ftp_state(तथ्य, ftpc, FTP_QUIT);

    परिणाम = ftp_block_statemach(तथ्य, ftpc);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
अटल CURLcode ftp_disconnect(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               bool dead_connection)
{
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  अगर(!ftpc)
    वापस CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  अगर(dead_connection || Curl_pp_needs_flush(तथ्य, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (व्योम)ftp_quit(तथ्य, ftpc); /* ignore errors on the QUIT */
  वापस CURLE_OK;
}

/* called from multi.c while DOing */
अटल CURLcode ftp_doing(काष्ठा Curl_easy *तथ्य,
                          bool *dophase_done)
{
  काष्ठा ftp_conn *ftpc = Curl_conn_meta_get(तथ्य->conn, CURL_META_FTP_CONN);
  काष्ठा FTP *ftp = Curl_meta_get(तथ्य, CURL_META_FTP_EASY);
  CURLcode परिणाम;

  अगर(!ftpc || !ftp)
    वापस CURLE_FAILED_INIT;
  परिणाम = ftp_statemach(तथ्य, ftpc, dophase_done);

  अगर(परिणाम)
    CURL_TRC_FTP(तथ्य, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  अन्यथा अगर(*dophase_done) {
    परिणाम = ftp_dophase_done(तथ्य, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(तथ्य, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  वापस परिणाम;
}

अटल व्योम ftp_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा FTP *ftp = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

अटल व्योम ftp_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ftp_conn *ftpc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

अटल व्योम type_url_check(काष्ठा Curl_easy *तथ्य, काष्ठा FTP *ftp)
{
  माप_प्रकार len = म_माप(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  अगर((len >= 7) && !स_भेद(&ftp->path[len - 7], ";type=", 6)) {
    अक्षर *type = &ftp->path[len - 7];
    अक्षर command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    चयन(command) {
    हाल 'A': /* ASCII mode */
      तथ्य->state.prefer_ascii = TRUE;
      अवरोध;

    हाल 'D': /* directory mode */
      तथ्य->state.list_only = TRUE;
      अवरोध;

    हाल 'I': /* binary mode */
    शेष:
      /* switch off ASCII */
      तथ्य->state.prefer_ascii = FALSE;
      अवरोध;
    }
  }
}

अटल CURLcode ftp_setup_connection(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  काष्ठा FTP *ftp;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ftp_conn *ftpc;

  ftp = curlx_calloc(1, माप(*ftp));
  अगर(!ftp ||
     Curl_meta_set(तथ्य, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, माप(*ftpc));
  अगर(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  अगर(CURL_EASY_STR(तथ्य, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(तथ्य, STRING_FTP_ACCOUNT));
    अगर(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }
  अगर(CURL_EASY_STR(तथ्य, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(तथ्य, STRING_FTP_ALTERNATIVE_TO_USER));
    अगर(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  ftp->path = &तथ्य->state.up.path[1]; /* do not include the initial slash */

  type_url_check(तथ्य, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->known_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = तथ्य->set.use_ssl;
  ftpc->ccc = तथ्य->set.ftp_ccc;

  CURL_TRC_FTP(तथ्य, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (पूर्णांक)परिणाम);
  वापस परिणाम;
}

bool Curl_ftp_conns_match(काष्ठा connectdata *needle, काष्ठा connectdata *conn)
{
  काष्ठा ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  काष्ठा ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  अगर(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    वापस FALSE;

  चयन(nftpc->use_ssl) {
  हाल CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    वापस (cftpc->use_ssl != CURLUSESSL_NONE);
  शेष:
    वापस (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_ftp = {
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

#पूर्ण_अगर /* CURL_DISABLE_FTP */
