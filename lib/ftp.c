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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "if2ip.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "ftp.h"
#અન્તર્ભાવ "ftp-int.h"
#અન્તર્ભાવ "ftplistparser.h"
#અન્તર્ભાવ "curl_range.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "parsedate.h" /* for the week day and month names */
#અન્તર્ભાવ "sockaddr.h" /* required for Curl_sockaddr_storage */
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(લીટી) \
  (ISDIGIT((લીટી)[0]) && ISDIGIT((લીટી)[1]) && ISDIGIT((લીટી)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(લીટી) (STATUSCODE(લીટી) && (' ' == (લીટી)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
સ્થિર અચલ અક્ષર * અચલ ftp_state_names[] = {
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
સ્થિર શૂન્ય ftp_state_નીચલું(સંરચના Curl_easy *data,
                          સંરચના ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , પૂર્ણાંક lineno
#endif
  )
{
  જો(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((શૂન્ય)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#નહીંતર
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_નીચલું(x, y, z)
#નહીંતર /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_નીચલું(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

સ્થિર CURLcode ftp_state_mdtm(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp);
સ્થિર CURLcode ftp_state_quote(સંરચના Curl_easy *data,
                                સંરચના ftp_conn *ftpc,
                                સંરચના FTP *ftp,
                                bool init, ftpstate instate);
સ્થિર CURLcode ftp_nb_type(સંરચના Curl_easy *data,
                            સંરચના ftp_conn *ftpc,
                            સંરચના FTP *ftp,
                            bool ascii, ftpstate newstate);
સ્થિર CURLcode getftpresponse(સંરચના Curl_easy *data, size_t *nreadp,
                               પૂર્ણાંક *ftpcodep);

સ્થિર શૂન્ય freedirs(સંરચના ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ખાલી;
}

સ્થિર size_t numof_slashes(અચલ અક્ષર *str)
{
  અચલ અક્ષર *slashPos;
  size_t અંક = 0;
  કરો {
    slashPos = strchr(str, '/');
    જો(slashPos) {
      ++અંક;
      str = slashPos + 1;
    }
  } જ્યારે(slashPos);
  પાછા અંક;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
સ્થિર CURLcode ftp_parse_url_path(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc,
                                   સંરચના FTP *ftp)
{
  અચલ અક્ષર *slashPos = ખાલી;
  અચલ અક્ષર *fileName = ખાલી;
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *rawPath = ખાલી; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  જો(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  જો(result) {
    failf(data, "path contains control characters");
    પાછા result;
  }
  rawPath = ftpc->rawpath;

  પસંદગી(data->set.ftp_filemethod) {
  સ્થિતિ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    જો((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    તોડો;

  સ્થિતિ FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    જો(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      જો(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, કદ(ftpc->dirs[0]));
      જો(!ftpc->dirs)
        પાછા CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (પૂર્ણાંક)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    નહીંતર
      fileName = rawPath; /* filename only (or empty) */
    તોડો;

  મૂળભૂત: /* allow pretty much anything */
  સ્થિતિ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    અચલ અક્ષર *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    જો(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      પાછા CURLE_URL_MALFORMAT;

    જો(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, કદ(ftpc->dirs[0]));
      જો(!ftpc->dirs)
        પાછા CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      જ્યારે(dirAlloc--) {
        અચલ અક્ષર *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        જો(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        જો(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (પૂર્ણાંક)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (પૂર્ણાંક)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    તોડો;
  } /* switch */

  જો(fileName && *fileName)
    ftpc->file = fileName;
  નહીંતર
    ftpc->file = ખાલી; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  જો(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    પાછા CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  જો((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  નહીંતર { /* newly created FTP connections are already in entry path */
    અચલ અક્ષર *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    જો(oldPath) {
      size_t n = pathLen;
      જો(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      નહીંતર
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      જો((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  પાછા CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
સ્થિર પૂર્ણાંક ftp_need_type(સંરચના ftp_conn *ftpc,
                         bool ascii_wanted)
{
  પાછા ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

સ્થિર શૂન્ય close_secondarysocket(સંરચના Curl_easy *data,
                                  સંરચના ftp_conn *ftpc)
{
  (શૂન્ય)ftpc;
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
સંરચના ftp_cw_lc_ctx {
  સંરચના Curl_cwriter super;
  bool newline_pending;
};

સ્થિર CURLcode ftp_cw_lc_write(સંરચના Curl_easy *data,
                                સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                અચલ અક્ષર *buf, size_t blen)
{
  સ્થિર અચલ અક્ષર nl = '\n';
  સંરચના ftp_cw_lc_ctx *ctx = writer->ctx;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  જો(!ftpc)
    પાછા CURLE_FAILED_INIT;

  જો(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    પાછા Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  જ્યારે(blen) {
    /* do not pass EOS when writing parts */
    પૂર્ણાંક chunk_type = (type & ~CLIENTWRITE_EOS);
    અચલ અક્ષર *cp;
    size_t chunk_len;
    CURLcode result;

    જો(ctx->newline_pending) {
      જો(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        જો(result)
          પાછા result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    જો(!cp)
      તોડો;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    જો(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      જો(result)
        પાછા result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  જો(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    પાછા Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  નહીંતર જો(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    જો(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      પાછા Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    પાછા Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  પાછા CURLE_OK;
}

સ્થિર અચલ સંરચના Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ખાલી,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  કદ(સંરચના ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
સ્થિર CURLcode ftp_check_ctrl_on_data_wait(સંરચના Curl_easy *data,
                                            સંરચના ftp_conn *ftpc)
{
  સંરચના connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  સંરચના pingpong *pp = &ftpc->pp;
  size_t nread;
  પૂર્ણાંક ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  જો(curlx_dyn_len(&pp->recvbuf)) {
    અચલ અક્ષર *l = curlx_dyn_ptr(&pp->recvbuf);
    જો(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (શૂન્ય)getftpresponse(data, &nread, &ftpcode);
      પાછા CURLE_FTP_ACCEPT_FAILED;
    }
  }

  જો(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  નહીંતર {
    પૂર્ણાંક socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    પસંદગી(socketstate) {
    સ્થિતિ -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      પાછા CURLE_FTP_ACCEPT_FAILED;
    મૂળભૂત:
      જો(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      તોડો;
    }
  }

  જો(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    જો(pp->overflow > 3) {
      અચલ અક્ષર *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      જો((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        જો(!curlx_str_સંખ્યા(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          પાછા CURLE_OK;
        }
      }
    }

    (શૂન્ય)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    જો(ftpcode / 100 > 3)
      પાછા CURLE_FTP_ACCEPT_FAILED;

    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  પાછા CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
સ્થિર CURLcode ftp_initiate_transfer(સંરચના Curl_easy *data,
                                      સંરચના ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  જો(result || !connected)
    પાછા result;

  જો(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  નહીંતર {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  પાછા CURLE_OK;
}

સ્થિર bool ftp_endofresp(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                          અચલ અક્ષર *લીટી, size_t len, પૂર્ણાંક *code)
{
  curl_off_t status;
  (શૂન્ય)data;
  (શૂન્ય)conn;

  જો((len > 3) && LASTLINE(લીટી) && !curlx_str_સંખ્યા(&લીટી, &status, 999)) {
    *code = (પૂર્ણાંક)status;
    પાછા TRUE;
  }

  પાછા FALSE;
}

સ્થિર CURLcode ftp_readresp(સંરચના Curl_easy *data,
                             સંરચના ftp_conn *ftpc,
                             int8_t sockindex,
                             સંરચના pingpong *pp,
                             પૂર્ણાંક *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  પૂર્ણાંક code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  જો(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  જો(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    પાછા CURLE_OPERATION_TIMEDOUT;
  }

  પાછા result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
સ્થિર CURLcode getftpresponse(સંરચના Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               પૂર્ણાંક *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  સંરચના connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  સંરચના pingpong *pp = &ftpc->pp;
  size_t nread;
  પૂર્ણાંક cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  જો(!ftpc)
    પાછા CURLE_FAILED_INIT;

  જ્યારે(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    જો(timeout <= 0) {
      failf(data, "FTP response timeout");
      પાછા CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    જો(timeout < interval_ms)
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

    જો(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    નહીંતર જો(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      પૂર્ણાંક ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      જો(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        પાછા CURLE_RECV_ERROR;
      }
      નહીંતર જો(ev == 0) {
        result = Curl_pgrsUpdate(data);
        ચાલુ; /* continue in our loop for the timeout duration */
      }
    }

    જો(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      જો(result)
        તોડો;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    જો(result)
      તોડો;

    જો(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    નહીંતર
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (પૂર્ણાંક)result, *nreadp, *ftpcodep);

  પાછા result;
}

સ્થિર CURLcode ftp_state_user(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  જો(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  પાછા result;
}

સ્થિર CURLcode ftp_state_pwd(સંરચના Curl_easy *data,
                              સંરચના ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  જો(!data->id && getenv("CURL_FTP_PWD_STOP"))
    પાછા CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  જો(!result)
    ftp_state(data, ftpc, FTP_PWD);

  પાછા result;
}

/* For the FTP "protocol connect" and "doing" phases only */
સ્થિર CURLcode ftp_pollset(સંરચના Curl_easy *data,
                            સંરચના easy_pollset *ps)
{
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  પાછા ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
સ્થિર CURLcode ftp_domore_pollset(સંરચના Curl_easy *data,
                                   સંરચના easy_pollset *ps)
{
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  જો(!ftpc)
    પાછા CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  જો(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    પાછા Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  પાછા Curl_pp_pollset(data, &ftpc->pp, ps);
}

સ્થિર પૂર્ણાંક pathlen(સંરચના ftp_conn *ftpc, પૂર્ણાંક અંક)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > અંક);
  પાછા ftpc->dirs[અંક].len;
}

સ્થિર અચલ અક્ષર *pathpiece(સંરચના ftp_conn *ftpc, પૂર્ણાંક અંક)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > અંક);
  પાછા &ftpc->rawpath[ftpc->dirs[અંક].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
સ્થિર CURLcode ftp_state_cwd(સંરચના Curl_easy *data,
                              સંરચના ftp_conn *ftpc,
                              સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;

  જો(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  નહીંતર {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    જો(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      જો(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    નહીંતર {
      જો(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        જો(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      નહીંતર {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  પાછા result;
}

પ્રકારવ્યાખ્યા ગણના {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
સ્થિર CURLcode ftp_port_parse_string(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn,
                                      અચલ અક્ષર *string_ftpport,
                                      સંરચના Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      અચલ અક્ષર **hostp,
                                      અક્ષર *hbuf, size_t hbuflen)
{
  અચલ અક્ષર *ip_end = ખાલી;
  અચલ અક્ષર *addr = ખાલી;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  અક્ષર ipstr[50];
#ifndef USE_IPV6
  (શૂન્ય)conn;
  (શૂન્ય)ss;
#endif

  /* default to nothing */
  *hostp = ખાલી;
  *port_minp = *port_maxp = 0;

  જો(!string_ftpport || (strlen(string_ftpport) <= 1))
    જાઓ done;

#ifdef USE_IPV6
  જો(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    અચલ અક્ષર *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    જો(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  નહીંતર
#endif
    જો(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    નહીંતર {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      જો(ip_end) {
#ifdef USE_IPV6
        સંરચના sockaddr_in6 * અચલ sa6 = (શૂન્ય *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        જો(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = ખાલી; /* this got no port ! */
        }
#endif
      }
      નહીંતર
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  જો(ip_end) {
    અચલ અક્ષર *portp = strchr(ip_end, ':');
    જો(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      જો(!curlx_str_સંખ્યા(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        જો(!curlx_str_single(&portp, '-') &&
           !curlx_str_સંખ્યા(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        નહીંતર
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  જો(port_min > port_max)
    port_min = port_max = 0;

  જો(addrlen) {
    અચલ સંરચના Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    જો(!remote_addr || (addrlen >= કદ(ipstr)) || (addrlen >= hbuflen))
      પાછા CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    પસંદગી(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    સ્થિતિ IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      તોડો;
    સ્થિતિ IF2IP_AF_NOT_SUPPORTED:
      પાછા CURLE_FTP_PORT_FAILED;
    સ્થિતિ IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      તોડો;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  પાછા CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
સ્થિર CURLcode ftp_port_default_host(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn,
                                      સંરચના Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      અચલ અક્ષર **hostp,
                                      અક્ષર *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  સંરચના sockaddr *sa = (સંરચના sockaddr *)ss;
  સંરચના sockaddr_in * અચલ sa4 = (શૂન્ય *)sa;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 * અચલ sa6 = (શૂન્ય *)sa;
#endif
  અક્ષર buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = કદ(*ss);
  જો(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
    પાછા CURLE_FTP_PORT_FAILED;
  }
  પસંદગી(sa->sa_family) {
#ifdef USE_IPV6
  સ્થિતિ AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    તોડો;
#endif
  મૂળભૂત:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    તોડો;
  }
  જો(result)
    પાછા CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  પાછા CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
સ્થિર CURLcode ftp_port_resolve_host(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn,
                                      અચલ અક્ષર *host,
                                      સંરચના Curl_dns_entry **dns_entryp,
                                      અચલ સંરચના Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = ખાલી;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  જો(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  નહીંતર {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  પાછા result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
સ્થિર CURLcode ftp_port_open_socket(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn,
                                     અચલ સંરચના Curl_addrinfo *res,
                                     અચલ સંરચના Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  અક્ષર buffer[STRERROR_LEN];
  પૂર્ણાંક sockerr = 0;
  અચલ સંરચના Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  માટે(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, ખાલી,
                       Curl_conn_get_transport(data, conn), portsockp);
    જો(result) {
      જો(result == CURLE_OUT_OF_MEMORY)
        પાછા result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      ચાલુ;
    }
    તોડો;
  }
  જો(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, કદ(buffer)));
    પાછા CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  પાછા result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
સ્થિર CURLcode ftp_port_bind_socket(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn,
                                     curl_socket_t portsock,
                                     અચલ સંરચના Curl_addrinfo *ai,
                                     સંરચના Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  સંરચના sockaddr *sa = (સંરચના sockaddr *)ss;
  સંરચના sockaddr_in * અચલ sa4 = (શૂન્ય *)sa;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 * અચલ sa6 = (શૂન્ય *)sa;
#endif
  અક્ષર buffer[STRERROR_LEN];
  unsigned short port;
  પૂર્ણાંક sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  માટે(port = port_min; port <= port_max;) {
    જો(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    નહીંતર
      sa6->sin6_port = htons(port);
#endif
    જો(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      જો(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, કદ(buffer)));

        *sslen_io = કદ(*ss);
        જો(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
          પાછા CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        ચાલુ;
      }
      જો(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, કદ(buffer)));
        પાછા CURLE_FTP_PORT_FAILED;
      }
    }
    નહીંતર
      તોડો;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    જો(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      પાછા CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = કદ(*ss);
  જો(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
    પાછા CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  પાછા CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
સ્થિર CURLcode ftp_port_listen(સંરચના Curl_easy *data, curl_socket_t portsock)
{
  અક્ષર buffer[STRERROR_LEN];

  જો(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
    પાછા CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  પાછા CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
સ્થિર CURLcode ftp_port_send_command(સંરચના Curl_easy *data,
                                      સંરચના ftp_conn *ftpc,
                                      સંરચના connectdata *conn,
                                      સંરચના Curl_sockaddr_storage *ss,
                                      અચલ સંરચના Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  સ્થિર અચલ અક્ષર mode[][5] = { "EPRT", "PORT" };
  સંરચના sockaddr *sa = (સંરચના sockaddr *)ss;
  સંરચના sockaddr_in * અચલ sa4 = (શૂન્ય *)sa;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 * અચલ sa6 = (શૂન્ય *)sa;
#endif
  અક્ષર myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, કદ(myhost));

#ifdef USE_IPV6
  જો(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  માટે(; fcmd != DONE; fcmd++) {

    જો(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ચાલુ;

    જો((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ચાલુ;

    પસંદગી(sa->sa_family) {
    સ્થિતિ AF_INET:
      port = ntohs(sa4->sin_port);
      તોડો;
#ifdef USE_IPV6
    સ્થિતિ AF_INET6:
      port = ntohs(sa6->sin6_port);
      તોડો;
#endif
    મૂળભૂત:
      ચાલુ; /* might as well skip this */
    }

    જો(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      જો(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        પાછા result;
      }
      તોડો;
    }
    જો(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      અક્ષર target[કદ(myhost) + 20];
      અચલ અક્ષર *source = myhost;
      અક્ષર *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      જ્યારે(*source) {
        જો(*source == '.')
          *dest = ',';
        નહીંતર
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (પૂર્ણાંક)(port >> 8), (પૂર્ણાંક)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      જો(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        પાછા result;
      }
      તોડો;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  પાછા CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
સ્થિર CURLcode ftp_state_use_port(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  સંરચના connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  સંરચના Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  અક્ષર hbuf[NI_MAXHOST];
  અચલ અક્ષર *host = ખાલી;
  અચલ અક્ષર *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  સંરચના Curl_dns_entry *dns_entry = ખાલી;
  અચલ સંરચના Curl_addrinfo *res = ખાલી;
  અચલ સંરચના Curl_addrinfo *ai = ખાલી;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, કદ(hbuf));
  જો(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, કદ(hbuf), &non_local);

  /* resolve host string to address list */
  જો(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  જો(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  જો(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  જો(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  જો(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  જો(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  જો(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  જો(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  જો(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  નહીંતર {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    જો(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  જો(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  પાછા result;
}

સ્થિર CURLcode ftp_state_use_pasv(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc,
                                   સંરચના connectdata *conn)
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

  સ્થિર અચલ અક્ષર mode[][5] = { "EPSV", "PASV" };
  પૂર્ણાંક modeoff;

#ifdef PF_INET6
  જો(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  જો(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  પાછા result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
સ્થિર CURLcode ftp_state_prepare_transfer(સંરચના Curl_easy *data,
                                           સંરચના ftp_conn *ftpc,
                                           સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;

  જો(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  નહીંતર જો(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  નહીંતર {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    જો(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      જો(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      નહીંતર જો(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      નહીંતર
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      જો(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    નહીંતર
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  પાછા result;
}

સ્થિર CURLcode ftp_state_rest(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;

  જો((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    જો(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  નહીંતર
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  પાછા result;
}

સ્થિર CURLcode ftp_state_size(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;

  જો((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    જો(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  નહીંતર
    result = ftp_state_rest(data, ftpc, ftp);

  પાછા result;
}

સ્થિર CURLcode ftp_state_list(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp)
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
  અચલ અક્ષર *lstArg = ખાલી;
  પૂર્ણાંક lstArglen = 0;
  અક્ષર *cmd;

  જો((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    અચલ અક્ષર *rawPath = ftpc->rawpath;
    અચલ અક્ષર *slashPos = strrchr(rawPath, '/');
    જો(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      જો(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (પૂર્ણાંક)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  જો(!cmd)
    પાછા CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  જો(!result)
    ftp_state(data, ftpc, FTP_LIST);

  પાછા result;
}

સ્થિર CURLcode ftp_state_list_prequote(સંરચના Curl_easy *data,
                                        સંરચના ftp_conn *ftpc,
                                        સંરચના FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  પાછા ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

સ્થિર CURLcode ftp_state_retr_prequote(સંરચના Curl_easy *data,
                                        સંરચના ftp_conn *ftpc,
                                        સંરચના FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  પાછા ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

સ્થિર CURLcode ftp_state_stor_prequote(સંરચના Curl_easy *data,
                                        સંરચના ftp_conn *ftpc,
                                        સંરચના FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  પાછા ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

સ્થિર CURLcode ftp_state_type(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  જો(data->req.no_body && ftpc->file &&
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
    જો(result)
      પાછા result;
  }
  નહીંતર
    result = ftp_state_size(data, ftpc, ftp);

  પાછા result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
સ્થિર CURLcode ftp_state_mdtm(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  જો((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    જો(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  નહીંતર
    result = ftp_state_type(data, ftpc, ftp);

  પાછા result;
}

/* This is called after the TYPE and possible quote commands have been sent */
સ્થિર CURLcode ftp_state_ul_setup(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc,
                                   સંરચના FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  જો((data->state.resume_from && !sizechecked) ||
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
    પૂર્ણાંક seekerr = CURL_SEEKFUNC_OK;

    જો(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      જો(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      પાછા result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    જો(data->set.seek_func) {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    જો(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      જો(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        પાછા CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      કરો {
        અક્ષર scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)કદ(scratch)) ?
          કદ(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        જો((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          પાછા CURLE_FTP_COULDNT_USE_REST;
        }
      } જ્યારે(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    જો(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      જો(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        પાછા CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  જો(!result)
    ftp_state(data, ftpc, FTP_STOR);

  પાછા result;
}

સ્થિર CURLcode ftp_state_retr(સંરચના Curl_easy *data,
                               સંરચના ftp_conn *ftpc,
                               સંરચના FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  જો(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    પાછા CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  જો(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    જો(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    નહીંતર {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      જો(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        જો(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          પાછા CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      નહીંતર {
        જો(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          પાછા CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    જો(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      પાછા CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    જો(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  નહીંતર {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    જો(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  પાછા result;
}

સ્થિર CURLcode ftp_state_quote(સંરચના Curl_easy *data,
                                સંરચના ftp_conn *ftpc,
                                સંરચના FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  સંરચના curl_slist *item;

  પસંદગી(instate) {
  સ્થિતિ FTP_QUOTE:
  મૂળભૂત:
    item = data->set.quote;
    તોડો;
  સ્થિતિ FTP_RETR_PREQUOTE:
  સ્થિતિ FTP_STOR_PREQUOTE:
  સ્થિતિ FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    તોડો;
  સ્થિતિ FTP_POSTQUOTE:
    item = data->set.postquote;
    તોડો;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  જો(init)
    ftpc->count1 = 0;
  નહીંતર
    ftpc->count1++;

  જો(item) {
    પૂર્ણાંક i = 0;

    /* Skip count1 items in the linked list */
    જ્યારે((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    જો(item) {
      અચલ અક્ષર *cmd = item->data;
      જો(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      નહીંતર
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      જો(result)
        પાછા result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  જો(!quote) {
    /* No more quote to send, continue to ... */
    પસંદગી(instate) {
    સ્થિતિ FTP_QUOTE:
    મૂળભૂત:
      result = ftp_state_cwd(data, ftpc, ftp);
      તોડો;
    સ્થિતિ FTP_RETR_PREQUOTE:
      જો(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      નહીંતર {
        જો(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        નહીંતર {
          જો(data->set.ignorecl || data->state.prefer_ascii) {
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
            જો(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          નહીંતર {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            જો(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      તોડો;
    સ્થિતિ FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      તોડો;
    સ્થિતિ FTP_POSTQUOTE:
      તોડો;
    સ્થિતિ FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      તોડો;
    }
  }

  પાછા result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
સ્થિર CURLcode ftp_epsv_disable(સંરચના Curl_easy *data,
                                 સંરચના ftp_conn *ftpc,
                                 સંરચના connectdata *conn)
{
  CURLcode result = CURLE_OK;

  જો(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  જો(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  પાછા result;
}

સ્થિર CURLcode ftp_control_addr_dup(સંરચના Curl_easy *data, અક્ષર **newhostp)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  જો(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  નહીંતર
#endif
  જો(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  નહીંતર {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = ખાલી;
    પાછા CURLE_FTP_CANT_GET_HOST;
  }
  પાછા *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

સ્થિર bool match_pasv_6nums(અચલ અક્ષર *p,
                             unsigned પૂર્ણાંક *array) /* 6 numbers */
{
  પૂર્ણાંક i;
  માટે(i = 0; i < 6; i++) {
    curl_off_t અંક;
    જો(i) {
      જો(*p != ',')
        પાછા FALSE;
      p++;
    }
    જો(curlx_str_સંખ્યા(&p, &અંક, 0xff))
      પાછા FALSE;
    array[i] = (unsigned પૂર્ણાંક)અંક;
  }
  પાછા TRUE;
}

સ્થિર CURLcode ftp_state_pasv_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    પૂર્ણાંક ftpcode)
{
  સંરચના connectdata *conn = data->conn;
  CURLcode result;
  અચલ સંરચના pingpong *pp = &ftpc->pp;
  અક્ષર *newhost = ખાલી;
  unsigned short newport = 0;
  અચલ અક્ષર *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  જો((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    અચલ અક્ષર *ptr = strchr(str, '(');
    જો(ptr) {
      અક્ષર sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      જો(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        અચલ અક્ષર *p = &ptr[3];
        curl_off_t અંક;
        જો(curlx_str_સંખ્યા(&p, &અંક, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          પાછા CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)અંક;
        result = ftp_control_addr_dup(data, &newhost);
        જો(result)
          પાછા result;
      }
      નહીંતર
        ptr = ખાલી;
    }
    જો(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      પાછા CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  નહીંતર જો((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned પૂર્ણાંક ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    જ્યારે(*str) {
      જો(match_pasv_6nums(str, ip))
        તોડો;
      str++;
    }

    જો(!*str) {
      failf(data, "Could not interpret the 227-response");
      પાછા CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    જો(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      જો(result)
        પાછા result;
    }
    નહીંતર
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    જો(!newhost)
      પાછા CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  નહીંતર જો(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    પાછા ftp_epsv_disable(data, ftpc, conn);
  }
  નહીંતર {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    પાછા CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  જો(result)
    જાઓ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  જો(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    જો(result)
      જાઓ error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  જો(result) {
    જો((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    જાઓ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  જો(data->set.verbose) {
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
  પાછા result;
}

/* called repeatedly until done from multi.c */
સ્થિર CURLcode ftp_statemach(સંરચના Curl_easy *data,
                              સંરચના ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  પાછા result;
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
સ્થિર CURLcode ftp_do_more(સંરચના Curl_easy *data, domore *more)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  સંરચના FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  જો(!ftpc || !ftp)
    પાછા CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  જો(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    જો(result == CURLE_OUT_OF_MEMORY)
      પાછા result;
    જો(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      જો(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        પાછા ftp_epsv_disable(data, ftpc, conn);
      }
      પાછા result;
    }
  }

  જો(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    જો(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    જો(result || !ftpc->wait_data_conn)
      પાછા result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  જો(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    જો(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      જો(result)
        પાછા result; /* Failed to accept data connection */

      જો(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        જો(result)
          પાછા result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      નહીંતર {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        જો(result)
          પાછા result;
      }
    }
    નહીંતર જો(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      જો(result)
        પાછા result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    નહીંતર {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      જો(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      જો(result)
        પાછા result;

      જો((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        જો(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          જો(result)
            પાછા result;
        }
        /* otherwise fall through */
      }
      નહીંતર {
        જો(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        નહીંતર {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        જો(result)
          પાછા result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      જો(complete)
        *more = DOMORE_DONE;
    }
    પાછા result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  જો(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (પૂર્ણાંક)result);
  }

  પાછા result;
}

/* call this when the DO phase has completed */
સ્થિર CURLcode ftp_dophase_done(સંરચના Curl_easy *data,
                                 સંરચના ftp_conn *ftpc,
                                 સંરચના FTP *ftp,
                                 bool connected)
{
  જો(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    જો(result) {
      close_secondarysocket(data, ftpc);
      પાછા result;
    }
  }

  જો(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  નહીંતર જો(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  પાછા CURLE_OK;
}

સ્થિર CURLcode ftp_state_port_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    સંરચના FTP *ftp,
                                    પૂર્ણાંક ftpcode)
{
  સંરચના connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  જો(ftpcode / 100 != 2) {
    /* the command failed */

    જો(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    જો(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    નહીંતર
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  નહીંતર {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  પાછા result;
}

/* return TRUE on error, FALSE on success */
સ્થિર bool twodigit(અચલ અક્ષર *p, પૂર્ણાંક *val)
{
  જો(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    પાછા TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  પાછા FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(અચલ અક્ષર *p, પૂર્ણાંક *year, પૂર્ણાંક *month, પૂર્ણાંક *day,
                           પૂર્ણાંક *hour, પૂર્ણાંક *minute, પૂર્ણાંક *second);
UNITTEST bool ftp_213_date(અચલ અક્ષર *p, પૂર્ણાંક *year, પૂર્ણાંક *month, પૂર્ણાંક *day,
                           પૂર્ણાંક *hour, પૂર્ણાંક *minute, પૂર્ણાંક *second)
{
  પૂર્ણાંક century;
  જો((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    પાછા FALSE;

  *year += century * 100;
  જો((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    પાછા FALSE;
  પાછા TRUE;
}

સ્થિર CURLcode client_write_header(સંરચના Curl_easy *data,
                                    અક્ષર *buf, size_t blen)
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
  પાછા result;
}

સ્થિર CURLcode ftp_state_mdtm_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    સંરચના FTP *ftp,
                                    પૂર્ણાંક ftpcode)
{
  CURLcode result = CURLE_OK;

  પસંદગી(ftpcode) {
  સ્થિતિ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    પૂર્ણાંક year, month, day, hour, minute, second;
    સંરચના pingpong *pp = &ftpc->pp;
    અચલ અક્ષર *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    જો(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      અક્ષર timebuf[24];
      curl_msnprintf(timebuf, કદ(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      જો(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#જો defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ધકેલો
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    જો(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#જો defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic કાઢો
#endif
      અક્ષર headerbuf[128];
      પૂર્ણાંક headerbuflen;
      time_t filetime = data->info.filetime;
      સંરચના tm buffer;
      અચલ સંરચના tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      જો(result)
        પાછા result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, કદ(headerbuf),
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
      જો(result)
        પાછા result;
    } /* end of a ridiculous amount of conditionals */
  }
    તોડો;
  મૂળભૂત:
    infof(data, "unsupported MDTM reply format");
    તોડો;
  સ્થિતિ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    તોડો;
  }

  જો(data->set.timecondition) {
    જો((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      પસંદગી(data->set.timecondition) {
      સ્થિતિ CURL_TIMECOND_IFMODSINCE:
      મૂળભૂત:
        જો(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          પાછા CURLE_OK;
        }
        તોડો;
      સ્થિતિ CURL_TIMECOND_IFUNMODSINCE:
        જો(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          પાછા CURLE_OK;
        }
        તોડો;
      } /* switch */
    }
    નહીંતર {
      infof(data, "Skipping time comparison");
    }
  }

  જો(!result)
    result = ftp_state_type(data, ftpc, ftp);

  પાછા result;
}

સ્થિર CURLcode ftp_state_type_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    સંરચના FTP *ftp,
                                    પૂર્ણાંક ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  જો(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    પાછા CURLE_FTP_COULDNT_SET_TYPE;
  }
  જો(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  જો(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  નહીંતર જો(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  નહીંતર જો(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  નહીંતર જો(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  નહીંતર જો(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  પાછા result;
}

સ્થિર CURLcode ftp_state_size_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    સંરચના FTP *ftp,
                                    પૂર્ણાંક ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  અચલ અક્ષર *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  જો(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    અચલ અક્ષર *start = &buf[4];
    અચલ અક્ષર *fdigit = memchr(start, '\r', len - 4);
    જો(fdigit) {
      fdigit--;
      જો(*fdigit == '\n')
        fdigit--;
      જ્યારે(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    નહીંતર
      fdigit = start;
    જો(curlx_str_સંખ્યા(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  નહીંતર જો(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    પાછા CURLE_REMOTE_FILE_NOT_FOUND;
  }

  જો(instate == FTP_SIZE) {
    જો(filesize != -1) {
      અક્ષર clbuf[128];
      પૂર્ણાંક clbuflen = curl_msnprintf(clbuf, કદ(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      જો(result)
        પાછા result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  નહીંતર જો(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  નહીંતર જો(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  પાછા result;
}

સ્થિર CURLcode ftp_state_rest_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    સંરચના FTP *ftp,
                                    પૂર્ણાંક ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  પસંદગી(instate) {
  સ્થિતિ FTP_REST:
  મૂળભૂત:
    જો(ftpcode == 350) {
      અક્ષર buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      જો(result)
        પાછા result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    તોડો;

  સ્થિતિ FTP_RETR_REST:
    જો(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    નહીંતર {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      જો(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    તોડો;
  }

  પાછા result;
}

સ્થિર CURLcode ftp_state_stor_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    પૂર્ણાંક ftpcode)
{
  CURLcode result = CURLE_OK;

  જો(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    પાછા CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  જો(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    જો(result)
      પાછા result;

    જો(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      પાછા ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  પાછા ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
સ્થિર CURLcode ftp_state_get_resp(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc,
                                   સંરચના FTP *ftp,
                                   પૂર્ણાંક ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  જો((ftpcode == 150) || (ftpcode == 125)) {

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

    જો((instate != FTP_LIST) &&
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
      જો(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        માટે(i = 0; i < len - 7; i++) {
          curl_off_t what;
          અચલ અક્ષર *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          અચલ અક્ષર *c = &buf[i];
          જો(!curlx_str_સંખ્યા(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            તોડો;
          }
        }
      }
    }
    નહીંતર જો(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    જો(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    નહીંતર જો((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    જો(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    જો(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      જો(result)
        પાછા result;

      જો(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        પાછા ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    પાછા ftp_initiate_transfer(data, ftpc);
  }
  નહીંતર {
    જો((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    નહીંતર {
      failf(data, "RETR response: %03d", ftpcode);
      પાછા instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  પાછા result;
}

/* after USER, PASS and ACCT */
સ્થિર CURLcode ftp_state_loggedin(સંરચના Curl_easy *data,
                                   સંરચના ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  જો(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    જો(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  નહીંતર {
    result = ftp_state_pwd(data, ftpc);
  }
  પાછા result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
સ્થિર bool ftp_has_ctrl(અચલ અક્ષર *string)
{
  અચલ unsigned અક્ષર *s = (અચલ unsigned અક્ષર *)string;
  જ્યારે(*s) {
    જો(*s < 0x20)
      પાછા TRUE;
    s++;
  }
  પાછા FALSE;
}

/* for USER and PASS responses */
સ્થિર CURLcode ftp_state_user_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    પૂર્ણાંક ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  જો((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    જો(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  નહીંતર જો(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  નહીંતર જો(ftpcode == 332) {
    અચલ અક્ષર *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    જો(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    નહીંતર જો(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    નહીંતર {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      જો(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  નહીંતર {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    અચલ અક્ષર *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    જો(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      જો(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      નહીંતર {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        જો(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    નહીંતર {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  પાછા result;
}

/* for ACCT response */
સ્થિર CURLcode ftp_state_acct_resp(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc,
                                    પૂર્ણાંક ftpcode)
{
  CURLcode result = CURLE_OK;
  જો(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  નહીંતર
    result = ftp_state_loggedin(data, ftpc);

  પાછા result;
}

સ્થિર CURLcode ftp_pwd_resp(સંરચના Curl_easy *data,
                             સંરચના ftp_conn *ftpc,
                             પૂર્ણાંક ftpcode)
{
  સંરચના pingpong *pp = &ftpc->pp;
  CURLcode result;

  જો(ftpcode == 257) {
    અચલ અક્ષર *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    સંરચના dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    જ્યારે(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    જો('\"' == *ptr) {
      /* it started good */
      માટે(ptr++; *ptr; ptr++) {
        જો('\"' == *ptr) {
          જો('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          નહીંતર {
            /* end of path */
            જો(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            તોડો; /* get out of this loop */
          }
        }
        નહીંતર {
          જો(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            પાછા CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        જો(result) {
          curlx_dyn_free(&out);
          પાછા result;
        }
      }
    }
    જો(entry_extracted) {
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
      અક્ષર *dir = curlx_dyn_ptr(&out);

      જો(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        જો(result) {
          curlx_dyn_free(&out);
          પાછા result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      જો(!data->state.most_recent_ftp_entrypath)
        પાછા CURLE_OUT_OF_MEMORY;

      જો(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        પાછા CURLE_OK;
      }
    }
    નહીંતર {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  પાછા CURLE_OK;
}

સ્થિર અચલ અક્ષર * અચલ ftpauth[] = { "SSL", "TLS" };

સ્થિર CURLcode ftp_wait_resp(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn,
                              સંરચના ftp_conn *ftpc,
                              પૂર્ણાંક ftpcode)
{
  CURLcode result = CURLE_OK;
  જો(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    જો(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      પાછા ftp_state_user_resp(data, ftpc, ftpcode);
  }
  નહીંતર જો(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  જો(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    પસંદગી((long)data->set.ftpsslauth) {
    સ્થિતિ CURLFTPAUTH_DEFAULT:
    સ્થિતિ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      તોડો;
    સ્થિતિ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      તોડો;
    મૂળભૂત:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (પૂર્ણાંક)data->set.ftpsslauth);
      પાછા CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    જો(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  નહીંતર
    result = ftp_state_user(data, ftpc, conn);
  પાછા result;
}

/* an implicit TLS scheme implies TLS on both connections */
સ્થિર unsigned અક્ષર ftp_use_ssl(સંરચના connectdata *conn,
                                 સંરચના ftp_conn *ftpc)
{
  જો(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    પાછા CURLUSESSL_ALL;
  પાછા ftpc->use_ssl;
}

સ્થિર CURLcode ftp_pp_statemachine(સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn)
{
  CURLcode result;
  પૂર્ણાંક ftpcode;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  સંરચના FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  સંરચના pingpong *pp;
  size_t nread = 0;

  જો(!ftpc || !ftp)
    પાછા CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  જો(pp->sendleft)
    પાછા Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  જો(result || !ftpcode)
    પાછા result;

  /* we have now received a full FTP server response */
  પસંદગી(ftpc->state) {
  સ્થિતિ FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    જો(pp->overflow)
      પાછા CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    જો((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      જો(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        જો(result) {
          /* we failed and bail out */
          પાછા CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      જો(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    નહીંતર જો(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    નહીંતર {
      જો(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      નહીંતર
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    તોડો;

  સ્થિતિ FTP_USER:
  સ્થિતિ FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    જો(!result)
      ftp_state(data, ftpc, FTP_PROT);
    તોડો;

  સ્થિતિ FTP_PROT:
    જો(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    નહીંતર જો(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      પાછા CURLE_USE_SSL_FAILED;

    જો(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      જો(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    નહીંતર
      result = ftp_state_pwd(data, ftpc);
    તોડો;

  સ્થિતિ FTP_CCC:
    જો(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned અક્ષર)CURLFTPSSL_CCC_ACTIVE));
      જો(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    જો(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    તોડો;

  સ્થિતિ FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_SYST:
    જો(ftpcode == 215) {
      અચલ અક્ષર *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      અચલ અક્ષર *start;
      અક્ષર *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      જ્યારે(*ptr == ' ')
        ptr++;
      માટે(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      જો(!os)
        પાછા CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      જો(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        જો(result) {
          curlx_free(os);
          પાછા result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        તોડો;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    નહીંતર {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    તોડો;

  સ્થિતિ FTP_NAMEFMT:
    જો(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      તોડો;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    તોડો;

  સ્થિતિ FTP_QUOTE:
  સ્થિતિ FTP_POSTQUOTE:
  સ્થિતિ FTP_RETR_PREQUOTE:
  સ્થિતિ FTP_STOR_PREQUOTE:
  સ્થિતિ FTP_LIST_PREQUOTE:
    જો((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    નહીંતર
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    તોડો;

  સ્થિતિ FTP_CWD:
    જો(ftpcode / 100 != 2) {
      /* failure to CWD there */
      જો(data->set.ftp_create_missing_dirs &&
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
        જો(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      નહીંતર {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    નહીંતર {
      /* success */
      ftpc->count2 = 0;
      જો(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      નહીંતર {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    તોડો;

  સ્થિતિ FTP_MKD:
    જો((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    નહીંતર {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    તોડો;

  સ્થિતિ FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    તોડો;

  સ્થિતિ FTP_TYPE:
  સ્થિતિ FTP_LIST_TYPE:
  સ્થિતિ FTP_RETR_TYPE:
  સ્થિતિ FTP_STOR_TYPE:
  સ્થિતિ FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    તોડો;

  સ્થિતિ FTP_SIZE:
  સ્થિતિ FTP_RETR_SIZE:
  સ્થિતિ FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    તોડો;

  સ્થિતિ FTP_REST:
  સ્થિતિ FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    તોડો;

  સ્થિતિ FTP_PRET:
    જો(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      પાછા CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    તોડો;

  સ્થિતિ FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    તોડો;

  સ્થિતિ FTP_LIST:
  સ્થિતિ FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    તોડો;

  સ્થિતિ FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    તોડો;

  સ્થિતિ FTP_QUIT:
  મૂળભૂત:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    તોડો;
  }

  પાછા result;
}

/* called repeatedly until done from multi.c */
સ્થિર CURLcode ftp_multi_statemach(સંરચના Curl_easy *data,
                                    bool *done)
{
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  પાછા ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

સ્થિર CURLcode ftp_block_statemach(સંરચના Curl_easy *data,
                                    સંરચના ftp_conn *ftpc)
{
  સંરચના pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  જ્યારે(ftpc->state != FTP_STOP) {
    જો(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    જો(result)
      તોડો;
  }

  પાછા result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
સ્થિર CURLcode ftp_connect(સંરચના Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  સંરચના connectdata *conn = data->conn;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  સંરચના pingpong *pp;

  *done = FALSE; /* default to not done yet */
  જો(!ftpc)
    પાછા CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  જો(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    જો(result)
      પાછા result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  પાછા result;
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
સ્થિર CURLcode ftp_sendquote(સંરચના Curl_easy *data,
                              સંરચના ftp_conn *ftpc,
                              સંરચના curl_slist *quote)
{
  સંરચના curl_slist *item;
  સંરચના pingpong *pp = &ftpc->pp;

  item = quote;
  જ્યારે(item) {
    જો(item->data) {
      size_t nread;
      અચલ અક્ષર *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      પૂર્ણાંક ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      જો(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      જો(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      જો(result)
        પાછા result;

      જો(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        પાછા CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode ftp_done_status(સંરચના Curl_easy *data,
                                સંરચના ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  પસંદગી(status) {
  સ્થિતિ CURLE_BAD_DOWNLOAD_RESUME:
  સ્થિતિ CURLE_FTP_WEIRD_PASV_REPLY:
  સ્થિતિ CURLE_FTP_PORT_FAILED:
  સ્થિતિ CURLE_FTP_ACCEPT_FAILED:
  સ્થિતિ CURLE_FTP_ACCEPT_TIMEOUT:
  સ્થિતિ CURLE_FTP_COULDNT_SET_TYPE:
  સ્થિતિ CURLE_FTP_COULDNT_RETR_FILE:
  સ્થિતિ CURLE_PARTIAL_FILE:
  સ્થિતિ CURLE_UPLOAD_FAILED:
  સ્થિતિ CURLE_REMOTE_ACCESS_DENIED:
  સ્થિતિ CURLE_FILESIZE_EXCEEDED:
  સ્થિતિ CURLE_REMOTE_FILE_NOT_FOUND:
  સ્થિતિ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  સ્થિતિ CURLE_OK: /* does not affect the control connection's status */
    જો(!premature)
      તોડો;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  મૂળભૂત:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    પાછા status;      /* use the already set error code */
  }
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય ftp_done_wildcard(સંરચના Curl_easy *data, સંરચના ftp_conn *ftpc)
{
  જો(data->state.wildcardmatch) {
    જો(data->set.chunk_end && ftpc->file) {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

સ્થિર શૂન્ય ftp_done_path(સંરચના Curl_easy *data, સંરચના ftp_conn *ftpc,
                          CURLcode result)
{
  સંરચના connectdata *conn = data->conn;
  જો(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  નહીંતર { /* remember working directory for connection reuse */
    અચલ અક્ષર *rawPath = ftpc->rawpath;
    જો(rawPath) {
      જો((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      નહીંતર {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        જો(!ftpc->cwdfail) {
          જો(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          નહીંતર
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        નહીંતર
          ftpc->prevpath = ખાલી; /* no path */
      }
    }
    જો(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

સ્થિર CURLcode ftp_done_secondary_socket(સંરચના Curl_easy *data,
                                          સંરચના ftp_conn *ftpc,
                                          CURLcode result)
{
  સંરચના connectdata *conn = data->conn;
  જો(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    જો(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      જો(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  પાછા result;
}

સ્થિર CURLcode ftp_done_control_reply(સંરચના Curl_easy *data,
                                       સંરચના ftp_conn *ftpc,
                                       સંરચના FTP *ftp, CURLcode result,
                                       bool premature)
{
  સંરચના connectdata *conn = data->conn;
  size_t nread;
  પૂર્ણાંક ftpcode;

  જો(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    જો(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    જો(result)
      પાછા result;

    જો(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      પાછા result;
    }

    જો(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      પસંદગી(ftpcode) {
      સ્થિતિ 226:
      સ્થિતિ 250:
        તોડો;
      સ્થિતિ 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        તોડો;
      મૂળભૂત:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        તોડો;
      }
    }
  }
  પાછા result;
}

સ્થિર CURLcode ftp_done_check_partial(સંરચના Curl_easy *data,
                                       સંરચના ftp_conn *ftpc,
                                       સંરચના FTP *ftp, CURLcode result,
                                       bool premature)
{
  જો(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  નહીંતર જો(data->state.upload) {
    જો((ftp->transfer == PPTRANSFER_BODY) &&
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
  નહીંતર {
    જો((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    નહીંતર જો(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  પાછા result;
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
સ્થિર CURLcode ftp_done(સંરચના Curl_easy *data, CURLcode status,
                         bool premature)
{
  સંરચના FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  જો(!ftp || !ftpc)
    પાછા CURLE_OK;

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
  જો(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (પૂર્ણાંક)result);
  પાછા result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
સ્થિર CURLcode ftp_nb_type(સંરચના Curl_easy *data,
                            સંરચના ftp_conn *ftpc,
                            સંરચના FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  અક્ષર want = (અક્ષર)(ascii ? 'A' : 'I');

  જો(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    પાછા ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  જો(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  પાછા result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
સ્થિર CURLcode ftp_perform(
  સંરચના Curl_easy *data,
  સંરચના ftp_conn *ftpc,
  સંરચના FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  જો(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  જો(result)
    પાછા result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  જો(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  નહીંતર
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  જો(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  પાછા result;
}

સ્થિર શૂન્ય wc_data_dtor(શૂન્ય *ptr)
{
  સંરચના ftp_wc *ftpwc = ptr;
  જો(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

સ્થિર CURLcode init_wc_data(સંરચના Curl_easy *data,
                             સંરચના ftp_conn *ftpc,
                             સંરચના FTP *ftp)
{
  અક્ષર *last_slash;
  અક્ષર *path = ftp->path;
  સંરચના WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  સંરચના ftp_wc *ftpwc = ખાલી;

  last_slash = strrchr(ftp->path, '/');
  જો(last_slash) {
    last_slash++;
    જો(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      પાછા ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    જો(!wildcard->pattern)
      પાછા CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  નહીંતર { /* there is only 'wildcard pattern' or nothing */
    જો(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      જો(!wildcard->pattern)
        પાછા CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    નહીંતર { /* only list */
      wildcard->state = CURLWC_CLEAN;
      પાછા ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, કદ(સંરચના ftp_wc));
  જો(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  જો(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  જો(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  જો(result) {
    જાઓ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  જો(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  infof(data, "Wildcard - Parsing started");
  પાછા CURLE_OK;

fail:
  જો(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ખાલી;
  પાછા result;
}

સ્થિર CURLcode wc_statemach(સંરચના Curl_easy *data,
                             સંરચના ftp_conn *ftpc,
                             સંરચના FTP *ftp)
{
  સંરચના WildcardData * અચલ wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  માટે(;;) {
    પસંદગી(wildcard->state) {
    સ્થિતિ CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      જો(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        પાછા result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      પાછા result;

    સ્થિતિ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      સંરચના ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      જો(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ચાલુ;
      }
      જો(Curl_llist_ગણતરી(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        પાછા CURLE_REMOTE_FILE_NOT_FOUND;
      }
      ચાલુ;
    }

    સ્થિતિ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      સંરચના Curl_llist_ગાંઠ *head = Curl_llist_head(&wildcard->filelist);
      સંરચના curl_fileinfo *finfo = Curl_ગાંઠ_elem(head);
      અક્ષર *tmp_path;
      અક્ષર *enc = curl_easy_escape(ખાલી, finfo->filename, 0);
      જો(!enc)
        પાછા CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      જો(!tmp_path)
        પાછા CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      જો(data->set.chunk_bgn) {
        long userresponse;
        સંરચના Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (પૂર્ણાંક)Curl_llist_ગણતરી(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        પસંદગી(userresponse) {
        સ્થિતિ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ચાલુ;
        સ્થિતિ CURL_CHUNK_BGN_FUNC_FAIL:
          પાછા CURLE_CHUNK_FAILED;
        }
      }

      જો(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ચાલુ;
      }

      જો(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      જો(result)
        પાછા result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ગાંઠ_remove(Curl_llist_head(&wildcard->filelist));

      જો(Curl_llist_ગણતરી(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        પાછા CURLE_OK;
      }
      પાછા result;
    }

    સ્થિતિ CURLWC_SKIP: {
      જો(data->set.chunk_end) {
        સંરચના Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_ગાંઠ_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_ગણતરી(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ચાલુ;
    }

    સ્થિતિ CURLWC_CLEAN: {
      સંરચના ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      જો(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      પાછા result;
    }

    સ્થિતિ CURLWC_DONE:
    સ્થિતિ CURLWC_ERROR:
    સ્થિતિ CURLWC_CLEAR:
      જો(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ખાલી;
      }
      પાછા result;
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
સ્થિર CURLcode ftp_regular_transfer(સંરચના Curl_easy *data,
                                     સંરચના ftp_conn *ftpc,
                                     સંરચના FTP *ftp,
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

  જો(!result) {

    જો(!*dophase_done)
      /* the DO phase has not completed yet */
      પાછા CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    જો(result)
      પાછા result;
  }
  નહીંતર
    freedirs(ftpc);

  પાછા result;
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
સ્થિર CURLcode ftp_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  સંરચના FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  જો(!ftpc || !ftp)
    પાછા CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    સંરચના Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    જો(result)
      પાછા result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    જો(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      પાછા result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  જો(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    જો(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      પાછા CURLE_OK;
    }
    જો(result) /* error, loop or skipping the file */
      પાછા result;
  }
  નહીંતર { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    જો(result)
      પાછા result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  પાછા result;
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
સ્થિર CURLcode ftp_quit(સંરચના Curl_easy *data,
                         સંરચના ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  જો(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    જો(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      પાછા result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  પાછા result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
સ્થિર CURLcode ftp_disconnect(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               bool dead_connection)
{
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  જો(!ftpc)
    પાછા CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  જો(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (શૂન્ય)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  પાછા CURLE_OK;
}

/* called from multi.c while DOing */
સ્થિર CURLcode ftp_doing(સંરચના Curl_easy *data,
                          bool *dophase_done)
{
  સંરચના ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  સંરચના FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  જો(!ftpc || !ftp)
    પાછા CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  જો(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  નહીંતર જો(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  પાછા result;
}

સ્થિર શૂન્ય ftp_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના FTP *ftp = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

સ્થિર શૂન્ય ftp_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ftp_conn *ftpc = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

સ્થિર શૂન્ય type_url_check(સંરચના Curl_easy *data, સંરચના FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  જો((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    અક્ષર *type = &ftp->path[len - 7];
    અક્ષર command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    પસંદગી(command) {
    સ્થિતિ 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      તોડો;

    સ્થિતિ 'D': /* directory mode */
      data->state.list_only = TRUE;
      તોડો;

    સ્થિતિ 'I': /* binary mode */
    મૂળભૂત:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      તોડો;
    }
  }
}

સ્થિર CURLcode ftp_setup_connection(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn)
{
  સંરચના FTP *ftp;
  CURLcode result = CURLE_OK;
  સંરચના ftp_conn *ftpc;

  ftp = curlx_calloc(1, કદ(*ftp));
  જો(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, કદ(*ftpc));
  જો(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  જો(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    જો(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      પાછા CURLE_OUT_OF_MEMORY;
    }
  }
  જો(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    જો(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      પાછા CURLE_OUT_OF_MEMORY;
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
               (પૂર્ણાંક)result);
  પાછા result;
}

bool Curl_ftp_conns_match(સંરચના connectdata *needle, સંરચના connectdata *conn)
{
  સંરચના ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  સંરચના ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  જો(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    પાછા FALSE;

  પસંદગી(nftpc->use_ssl) {
  સ્થિતિ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    પાછા (cftpc->use_ssl != CURLUSESSL_NONE);
  મૂળભૂત:
    પાછા (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
અચલ સંરચના Curl_protocol Curl_protocol_ftp = {
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
