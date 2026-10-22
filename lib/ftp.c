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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "if2ip.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "ftp.h"
#অন্তর্ভাৱ "ftp-int.h"
#অন্তর্ভাৱ "ftplistparser.h"
#অন্তর্ভাৱ "curl_range.h"
#অন্তর্ভাৱ "strcase.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "cf-socket.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "curlx/inet_ntop.h"
#অন্তর্ভাৱ "curlx/inet_pton.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "parsedate.h" /* for the week day and month names */
#অন্তর্ভাৱ "sockaddr.h" /* required for Curl_sockaddr_storage */
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/strerr.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(সাৰি) \
  (ISDIGIT((সাৰি)[0]) && ISDIGIT((সাৰি)[1]) && ISDIGIT((সাৰি)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(সাৰি) (STATUSCODE(সাৰি) && (' ' == (সাৰি)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ ftp_state_names[] = {
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
স্থির শূন্য ftp_state_নিচেৰ(সংরচনা Curl_easy *data,
                          সংরচনা ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , পূর্ণাঙ্ক lineno
#endif
  )
{
  যদি(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((শূন্য)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#অন্যথা
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_নিচেৰ(x, y, z)
#অন্যথা /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_নিচেৰ(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

স্থির CURLcode ftp_state_mdtm(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp);
স্থির CURLcode ftp_state_quote(সংরচনা Curl_easy *data,
                                সংরচনা ftp_conn *ftpc,
                                সংরচনা FTP *ftp,
                                bool init, ftpstate instate);
স্থির CURLcode ftp_nb_type(সংরচনা Curl_easy *data,
                            সংরচনা ftp_conn *ftpc,
                            সংরচনা FTP *ftp,
                            bool ascii, ftpstate newstate);
স্থির CURLcode getftpresponse(সংরচনা Curl_easy *data, size_t *nreadp,
                               পূর্ণাঙ্ক *ftpcodep);

স্থির শূন্য freedirs(সংরচনা ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = রিক্ত;
}

স্থির size_t numof_slashes(ধ্রুৱ ৱর্ণ *str)
{
  ধ্রুৱ ৱর্ণ *slashPos;
  size_t অঙ্ক = 0;
  কৰা {
    slashPos = strchr(str, '/');
    যদি(slashPos) {
      ++অঙ্ক;
      str = slashPos + 1;
    }
  } যেতিয়ালৈ(slashPos);
  ঘূৰাই অঙ্ক;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
স্থির CURLcode ftp_parse_url_path(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc,
                                   সংরচনা FTP *ftp)
{
  ধ্রুৱ ৱর্ণ *slashPos = রিক্ত;
  ধ্রুৱ ৱর্ণ *fileName = রিক্ত;
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *rawPath = রিক্ত; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  যদি(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  যদি(result) {
    failf(data, "path contains control characters");
    ঘূৰাই result;
  }
  rawPath = ftpc->rawpath;

  ৱিকল্প(data->set.ftp_filemethod) {
  পক্ষ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    যদি((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ভঙ্গ;

  পক্ষ FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    যদি(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      যদি(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, পরিমাণ(ftpc->dirs[0]));
      যদি(!ftpc->dirs)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (পূর্ণাঙ্ক)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    অন্যথা
      fileName = rawPath; /* filename only (or empty) */
    ভঙ্গ;

  স্ৱভাৱ: /* allow pretty much anything */
  পক্ষ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ধ্রুৱ ৱর্ণ *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    যদি(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      ঘূৰাই CURLE_URL_MALFORMAT;

    যদি(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, পরিমাণ(ftpc->dirs[0]));
      যদি(!ftpc->dirs)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      যেতিয়ালৈ(dirAlloc--) {
        ধ্রুৱ ৱর্ণ *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        যদি(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        যদি(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (পূর্ণাঙ্ক)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (পূর্ণাঙ্ক)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ভঙ্গ;
  } /* switch */

  যদি(fileName && *fileName)
    ftpc->file = fileName;
  অন্যথা
    ftpc->file = রিক্ত; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  যদি(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  যদি((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  অন্যথা { /* newly created FTP connections are already in entry path */
    ধ্রুৱ ৱর্ণ *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    যদি(oldPath) {
      size_t n = pathLen;
      যদি(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      অন্যথা
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      যদি((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ঘূৰাই CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
স্থির পূর্ণাঙ্ক ftp_need_type(সংরচনা ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ঘূৰাই ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

স্থির শূন্য close_secondarysocket(সংরচনা Curl_easy *data,
                                  সংরচনা ftp_conn *ftpc)
{
  (শূন্য)ftpc;
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
সংরচনা ftp_cw_lc_ctx {
  সংরচনা Curl_cwriter super;
  bool newline_pending;
};

স্থির CURLcode ftp_cw_lc_write(সংরচনা Curl_easy *data,
                                সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                ধ্রুৱ ৱর্ণ *buf, size_t blen)
{
  স্থির ধ্রুৱ ৱর্ণ nl = '\n';
  সংরচনা ftp_cw_lc_ctx *ctx = writer->ctx;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  যেতিয়ালৈ(blen) {
    /* do not pass EOS when writing parts */
    পূর্ণাঙ্ক chunk_type = (type & ~CLIENTWRITE_EOS);
    ধ্রুৱ ৱর্ণ *cp;
    size_t chunk_len;
    CURLcode result;

    যদি(ctx->newline_pending) {
      যদি(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        যদি(result)
          ঘূৰাই result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    যদি(!cp)
      ভঙ্গ;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    যদি(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      যদি(result)
        ঘূৰাই result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  যদি(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  অন্যথা যদি(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    যদি(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ঘূৰাই Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ঘূৰাই Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ সংরচনা Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  রিক্ত,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  পরিমাণ(সংরচনা ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
স্থির CURLcode ftp_check_ctrl_on_data_wait(সংরচনা Curl_easy *data,
                                            সংরচনা ftp_conn *ftpc)
{
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  সংরচনা pingpong *pp = &ftpc->pp;
  size_t nread;
  পূর্ণাঙ্ক ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  যদি(curlx_dyn_len(&pp->recvbuf)) {
    ধ্রুৱ ৱর্ণ *l = curlx_dyn_ptr(&pp->recvbuf);
    যদি(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (শূন্য)getftpresponse(data, &nread, &ftpcode);
      ঘূৰাই CURLE_FTP_ACCEPT_FAILED;
    }
  }

  যদি(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  অন্যথা {
    পূর্ণাঙ্ক socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ৱিকল্প(socketstate) {
    পক্ষ -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      ঘূৰাই CURLE_FTP_ACCEPT_FAILED;
    স্ৱভাৱ:
      যদি(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      ভঙ্গ;
    }
  }

  যদি(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    যদি(pp->overflow > 3) {
      ধ্রুৱ ৱর্ণ *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      যদি((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        যদি(!curlx_str_সংখ্যা(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          ঘূৰাই CURLE_OK;
        }
      }
    }

    (শূন্য)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    যদি(ftpcode / 100 > 3)
      ঘূৰাই CURLE_FTP_ACCEPT_FAILED;

    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  ঘূৰাই CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
স্থির CURLcode ftp_initiate_transfer(সংরচনা Curl_easy *data,
                                      সংরচনা ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  যদি(result || !connected)
    ঘূৰাই result;

  যদি(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  অন্যথা {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  ঘূৰাই CURLE_OK;
}

স্থির bool ftp_endofresp(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                          ধ্রুৱ ৱর্ণ *সাৰি, size_t len, পূর্ণাঙ্ক *code)
{
  curl_off_t status;
  (শূন্য)data;
  (শূন্য)conn;

  যদি((len > 3) && LASTLINE(সাৰি) && !curlx_str_সংখ্যা(&সাৰি, &status, 999)) {
    *code = (পূর্ণাঙ্ক)status;
    ঘূৰাই TRUE;
  }

  ঘূৰাই FALSE;
}

স্থির CURLcode ftp_readresp(সংরচনা Curl_easy *data,
                             সংরচনা ftp_conn *ftpc,
                             int8_t sockindex,
                             সংরচনা pingpong *pp,
                             পূর্ণাঙ্ক *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  পূর্ণাঙ্ক code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  যদি(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  যদি(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;
  }

  ঘূৰাই result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
স্থির CURLcode getftpresponse(সংরচনা Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               পূর্ণাঙ্ক *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  সংরচনা connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  সংরচনা pingpong *pp = &ftpc->pp;
  size_t nread;
  পূর্ণাঙ্ক cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  যদি(!ftpc)
    ঘূৰাই CURLE_FAILED_INIT;

  যেতিয়ালৈ(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    যদি(timeout <= 0) {
      failf(data, "FTP response timeout");
      ঘূৰাই CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    যদি(timeout < interval_ms)
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

    যদি(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    অন্যথা যদি(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      পূর্ণাঙ্ক ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      যদি(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ঘূৰাই CURLE_RECV_ERROR;
      }
      অন্যথা যদি(ev == 0) {
        result = Curl_pgrsUpdate(data);
        অনুৱর্ত; /* continue in our loop for the timeout duration */
      }
    }

    যদি(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      যদি(result)
        ভঙ্গ;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    যদি(result)
      ভঙ্গ;

    যদি(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    অন্যথা
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (পূর্ণাঙ্ক)result, *nreadp, *ftpcodep);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_user(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  যদি(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  ঘূৰাই result;
}

স্থির CURLcode ftp_state_pwd(সংরচনা Curl_easy *data,
                              সংরচনা ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  যদি(!data->id && getenv("CURL_FTP_PWD_STOP"))
    ঘূৰাই CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  যদি(!result)
    ftp_state(data, ftpc, FTP_PWD);

  ঘূৰাই result;
}

/* For the FTP "protocol connect" and "doing" phases only */
স্থির CURLcode ftp_pollset(সংরচনা Curl_easy *data,
                            সংরচনা easy_pollset *ps)
{
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ঘূৰাই ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
স্থির CURLcode ftp_domore_pollset(সংরচনা Curl_easy *data,
                                   সংরচনা easy_pollset *ps)
{
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ঘূৰাই CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  যদি(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ঘূৰাই Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  ঘূৰাই Curl_pp_pollset(data, &ftpc->pp, ps);
}

স্থির পূর্ণাঙ্ক pathlen(সংরচনা ftp_conn *ftpc, পূর্ণাঙ্ক অঙ্ক)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > অঙ্ক);
  ঘূৰাই ftpc->dirs[অঙ্ক].len;
}

স্থির ধ্রুৱ ৱর্ণ *pathpiece(সংরচনা ftp_conn *ftpc, পূর্ণাঙ্ক অঙ্ক)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > অঙ্ক);
  ঘূৰাই &ftpc->rawpath[ftpc->dirs[অঙ্ক].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
স্থির CURLcode ftp_state_cwd(সংরচনা Curl_easy *data,
                              সংরচনা ftp_conn *ftpc,
                              সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;

  যদি(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  অন্যথা {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    যদি(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      যদি(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    অন্যথা {
      যদি(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        যদি(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      অন্যথা {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  ঘূৰাই result;
}

প্রকারনির্দেশ গণনা {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
স্থির CURLcode ftp_port_parse_string(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn,
                                      ধ্রুৱ ৱর্ণ *string_ftpport,
                                      সংরচনা Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ধ্রুৱ ৱর্ণ **hostp,
                                      ৱর্ণ *hbuf, size_t hbuflen)
{
  ধ্রুৱ ৱর্ণ *ip_end = রিক্ত;
  ধ্রুৱ ৱর্ণ *addr = রিক্ত;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  ৱর্ণ ipstr[50];
#ifndef USE_IPV6
  (শূন্য)conn;
  (শূন্য)ss;
#endif

  /* default to nothing */
  *hostp = রিক্ত;
  *port_minp = *port_maxp = 0;

  যদি(!string_ftpport || (strlen(string_ftpport) <= 1))
    গচ্ছ done;

#ifdef USE_IPV6
  যদি(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ধ্রুৱ ৱর্ণ *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    যদি(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  অন্যথা
#endif
    যদি(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    অন্যথা {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      যদি(ip_end) {
#ifdef USE_IPV6
        সংরচনা sockaddr_in6 * ধ্রুৱ sa6 = (শূন্য *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        যদি(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = রিক্ত; /* this got no port ! */
        }
#endif
      }
      অন্যথা
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  যদি(ip_end) {
    ধ্রুৱ ৱর্ণ *portp = strchr(ip_end, ':');
    যদি(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      যদি(!curlx_str_সংখ্যা(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        যদি(!curlx_str_single(&portp, '-') &&
           !curlx_str_সংখ্যা(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        অন্যথা
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  যদি(port_min > port_max)
    port_min = port_max = 0;

  যদি(addrlen) {
    ধ্রুৱ সংরচনা Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    যদি(!remote_addr || (addrlen >= পরিমাণ(ipstr)) || (addrlen >= hbuflen))
      ঘূৰাই CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ৱিকল্প(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    পক্ষ IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ভঙ্গ;
    পক্ষ IF2IP_AF_NOT_SUPPORTED:
      ঘূৰাই CURLE_FTP_PORT_FAILED;
    পক্ষ IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      ভঙ্গ;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  ঘূৰাই CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
স্থির CURLcode ftp_port_default_host(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn,
                                      সংরচনা Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ধ্রুৱ ৱর্ণ **hostp,
                                      ৱর্ণ *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  সংরচনা sockaddr *sa = (সংরচনা sockaddr *)ss;
  সংরচনা sockaddr_in * ধ্রুৱ sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 * ধ্রুৱ sa6 = (শূন্য *)sa;
#endif
  ৱর্ণ buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = পরিমাণ(*ss);
  যদি(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
    ঘূৰাই CURLE_FTP_PORT_FAILED;
  }
  ৱিকল্প(sa->sa_family) {
#ifdef USE_IPV6
  পক্ষ AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ভঙ্গ;
#endif
  স্ৱভাৱ:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ভঙ্গ;
  }
  যদি(result)
    ঘূৰাই CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ঘূৰাই CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
স্থির CURLcode ftp_port_resolve_host(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn,
                                      ধ্রুৱ ৱর্ণ *host,
                                      সংরচনা Curl_dns_entry **dns_entryp,
                                      ধ্রুৱ সংরচনা Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = রিক্ত;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  যদি(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  অন্যথা {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ঘূৰাই result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
স্থির CURLcode ftp_port_open_socket(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn,
                                     ধ্রুৱ সংরচনা Curl_addrinfo *res,
                                     ধ্রুৱ সংরচনা Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  ৱর্ণ buffer[STRERROR_LEN];
  পূর্ণাঙ্ক sockerr = 0;
  ধ্রুৱ সংরচনা Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  বাবে(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, রিক্ত,
                       Curl_conn_get_transport(data, conn), portsockp);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        ঘূৰাই result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      অনুৱর্ত;
    }
    ভঙ্গ;
  }
  যদি(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, পরিমাণ(buffer)));
    ঘূৰাই CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  ঘূৰাই result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
স্থির CURLcode ftp_port_bind_socket(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn,
                                     curl_socket_t portsock,
                                     ধ্রুৱ সংরচনা Curl_addrinfo *ai,
                                     সংরচনা Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  সংরচনা sockaddr *sa = (সংরচনা sockaddr *)ss;
  সংরচনা sockaddr_in * ধ্রুৱ sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 * ধ্রুৱ sa6 = (শূন্য *)sa;
#endif
  ৱর্ণ buffer[STRERROR_LEN];
  unsigned short port;
  পূর্ণাঙ্ক sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  বাবে(port = port_min; port <= port_max;) {
    যদি(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    অন্যথা
      sa6->sin6_port = htons(port);
#endif
    যদি(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      যদি(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, পরিমাণ(buffer)));

        *sslen_io = পরিমাণ(*ss);
        যদি(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
          ঘূৰাই CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        অনুৱর্ত;
      }
      যদি(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, পরিমাণ(buffer)));
        ঘূৰাই CURLE_FTP_PORT_FAILED;
      }
    }
    অন্যথা
      ভঙ্গ;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    যদি(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      ঘূৰাই CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = পরিমাণ(*ss);
  যদি(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
    ঘূৰাই CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  ঘূৰাই CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
স্থির CURLcode ftp_port_listen(সংরচনা Curl_easy *data, curl_socket_t portsock)
{
  ৱর্ণ buffer[STRERROR_LEN];

  যদি(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, পরিমাণ(buffer)));
    ঘূৰাই CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  ঘূৰাই CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
স্থির CURLcode ftp_port_send_command(সংরচনা Curl_easy *data,
                                      সংরচনা ftp_conn *ftpc,
                                      সংরচনা connectdata *conn,
                                      সংরচনা Curl_sockaddr_storage *ss,
                                      ধ্রুৱ সংরচনা Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  স্থির ধ্রুৱ ৱর্ণ mode[][5] = { "EPRT", "PORT" };
  সংরচনা sockaddr *sa = (সংরচনা sockaddr *)ss;
  সংরচনা sockaddr_in * ধ্রুৱ sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 * ধ্রুৱ sa6 = (শূন্য *)sa;
#endif
  ৱর্ণ myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, পরিমাণ(myhost));

#ifdef USE_IPV6
  যদি(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  বাবে(; fcmd != DONE; fcmd++) {

    যদি(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      অনুৱর্ত;

    যদি((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      অনুৱর্ত;

    ৱিকল্প(sa->sa_family) {
    পক্ষ AF_INET:
      port = ntohs(sa4->sin_port);
      ভঙ্গ;
#ifdef USE_IPV6
    পক্ষ AF_INET6:
      port = ntohs(sa6->sin6_port);
      ভঙ্গ;
#endif
    স্ৱভাৱ:
      অনুৱর্ত; /* might as well skip this */
    }

    যদি(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      যদি(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        ঘূৰাই result;
      }
      ভঙ্গ;
    }
    যদি(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      ৱর্ণ target[পরিমাণ(myhost) + 20];
      ধ্রুৱ ৱর্ণ *source = myhost;
      ৱর্ণ *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      যেতিয়ালৈ(*source) {
        যদি(*source == '.')
          *dest = ',';
        অন্যথা
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (পূর্ণাঙ্ক)(port >> 8), (পূর্ণাঙ্ক)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      যদি(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        ঘূৰাই result;
      }
      ভঙ্গ;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  ঘূৰাই CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
স্থির CURLcode ftp_state_use_port(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  সংরচনা Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  ৱর্ণ hbuf[NI_MAXHOST];
  ধ্রুৱ ৱর্ণ *host = রিক্ত;
  ধ্রুৱ ৱর্ণ *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  সংরচনা Curl_dns_entry *dns_entry = রিক্ত;
  ধ্রুৱ সংরচনা Curl_addrinfo *res = রিক্ত;
  ধ্রুৱ সংরচনা Curl_addrinfo *ai = রিক্ত;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, পরিমাণ(hbuf));
  যদি(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, পরিমাণ(hbuf), &non_local);

  /* resolve host string to address list */
  যদি(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  যদি(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  যদি(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  যদি(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  যদি(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  যদি(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  যদি(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  যদি(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  যদি(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  অন্যথা {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    যদি(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  যদি(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  ঘূৰাই result;
}

স্থির CURLcode ftp_state_use_pasv(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc,
                                   সংরচনা connectdata *conn)
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

  স্থির ধ্রুৱ ৱর্ণ mode[][5] = { "EPSV", "PASV" };
  পূর্ণাঙ্ক modeoff;

#ifdef PF_INET6
  যদি(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  যদি(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  ঘূৰাই result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
স্থির CURLcode ftp_state_prepare_transfer(সংরচনা Curl_easy *data,
                                           সংরচনা ftp_conn *ftpc,
                                           সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  যদি(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  অন্যথা যদি(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  অন্যথা {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    যদি(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      যদি(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      অন্যথা যদি(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      অন্যথা
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      যদি(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    অন্যথা
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  ঘূৰাই result;
}

স্থির CURLcode ftp_state_rest(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;

  যদি((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    যদি(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  অন্যথা
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_size(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;

  যদি((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    যদি(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  অন্যথা
    result = ftp_state_rest(data, ftpc, ftp);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_list(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp)
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
  ধ্রুৱ ৱর্ণ *lstArg = রিক্ত;
  পূর্ণাঙ্ক lstArglen = 0;
  ৱর্ণ *cmd;

  যদি((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ধ্রুৱ ৱর্ণ *rawPath = ftpc->rawpath;
    ধ্রুৱ ৱর্ণ *slashPos = strrchr(rawPath, '/');
    যদি(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      যদি(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (পূর্ণাঙ্ক)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  যদি(!cmd)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  যদি(!result)
    ftp_state(data, ftpc, FTP_LIST);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_list_prequote(সংরচনা Curl_easy *data,
                                        সংরচনা ftp_conn *ftpc,
                                        সংরচনা FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ঘূৰাই ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

স্থির CURLcode ftp_state_retr_prequote(সংরচনা Curl_easy *data,
                                        সংরচনা ftp_conn *ftpc,
                                        সংরচনা FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ঘূৰাই ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

স্থির CURLcode ftp_state_stor_prequote(সংরচনা Curl_easy *data,
                                        সংরচনা ftp_conn *ftpc,
                                        সংরচনা FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ঘূৰাই ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

স্থির CURLcode ftp_state_type(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  যদি(data->req.no_body && ftpc->file &&
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
    যদি(result)
      ঘূৰাই result;
  }
  অন্যথা
    result = ftp_state_size(data, ftpc, ftp);

  ঘূৰাই result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
স্থির CURLcode ftp_state_mdtm(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  যদি((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    যদি(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  অন্যথা
    result = ftp_state_type(data, ftpc, ftp);

  ঘূৰাই result;
}

/* This is called after the TYPE and possible quote commands have been sent */
স্থির CURLcode ftp_state_ul_setup(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc,
                                   সংরচনা FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  যদি((data->state.resume_from && !sizechecked) ||
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
    পূর্ণাঙ্ক seekerr = CURL_SEEKFUNC_OK;

    যদি(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      যদি(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      ঘূৰাই result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    যদি(data->set.seek_func) {
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    যদি(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ঘূৰাই CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      কৰা {
        ৱর্ণ scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)পরিমাণ(scratch)) ?
          পরিমাণ(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ঘূৰাই CURLE_FTP_COULDNT_USE_REST;
        }
      } যেতিয়ালৈ(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    যদি(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      যদি(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        ঘূৰাই CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  যদি(!result)
    ftp_state(data, ftpc, FTP_STOR);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_retr(সংরচনা Curl_easy *data,
                               সংরচনা ftp_conn *ftpc,
                               সংরচনা FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  যদি(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    ঘূৰাই CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  যদি(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    যদি(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    অন্যথা {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      যদি(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        যদি(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ঘূৰাই CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      অন্যথা {
        যদি(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ঘূৰাই CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    যদি(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      ঘূৰাই CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    যদি(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  অন্যথা {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    যদি(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_quote(সংরচনা Curl_easy *data,
                                সংরচনা ftp_conn *ftpc,
                                সংরচনা FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  সংরচনা curl_slist *item;

  ৱিকল্প(instate) {
  পক্ষ FTP_QUOTE:
  স্ৱভাৱ:
    item = data->set.quote;
    ভঙ্গ;
  পক্ষ FTP_RETR_PREQUOTE:
  পক্ষ FTP_STOR_PREQUOTE:
  পক্ষ FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    ভঙ্গ;
  পক্ষ FTP_POSTQUOTE:
    item = data->set.postquote;
    ভঙ্গ;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  যদি(init)
    ftpc->count1 = 0;
  অন্যথা
    ftpc->count1++;

  যদি(item) {
    পূর্ণাঙ্ক i = 0;

    /* Skip count1 items in the linked list */
    যেতিয়ালৈ((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    যদি(item) {
      ধ্রুৱ ৱর্ণ *cmd = item->data;
      যদি(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      অন্যথা
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      যদি(result)
        ঘূৰাই result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  যদি(!quote) {
    /* No more quote to send, continue to ... */
    ৱিকল্প(instate) {
    পক্ষ FTP_QUOTE:
    স্ৱভাৱ:
      result = ftp_state_cwd(data, ftpc, ftp);
      ভঙ্গ;
    পক্ষ FTP_RETR_PREQUOTE:
      যদি(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      অন্যথা {
        যদি(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        অন্যথা {
          যদি(data->set.ignorecl || data->state.prefer_ascii) {
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
            যদি(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          অন্যথা {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            যদি(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      ভঙ্গ;
    পক্ষ FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      ভঙ্গ;
    পক্ষ FTP_POSTQUOTE:
      ভঙ্গ;
    পক্ষ FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      ভঙ্গ;
    }
  }

  ঘূৰাই result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
স্থির CURLcode ftp_epsv_disable(সংরচনা Curl_easy *data,
                                 সংরচনা ftp_conn *ftpc,
                                 সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;

  যদি(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  যদি(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  ঘূৰাই result;
}

স্থির CURLcode ftp_control_addr_dup(সংরচনা Curl_easy *data, ৱর্ণ **newhostp)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  যদি(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  অন্যথা
#endif
  যদি(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  অন্যথা {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = রিক্ত;
    ঘূৰাই CURLE_FTP_CANT_GET_HOST;
  }
  ঘূৰাই *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির bool match_pasv_6nums(ধ্রুৱ ৱর্ণ *p,
                             unsigned পূর্ণাঙ্ক *array) /* 6 numbers */
{
  পূর্ণাঙ্ক i;
  বাবে(i = 0; i < 6; i++) {
    curl_off_t অঙ্ক;
    যদি(i) {
      যদি(*p != ',')
        ঘূৰাই FALSE;
      p++;
    }
    যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, 0xff))
      ঘূৰাই FALSE;
    array[i] = (unsigned পূর্ণাঙ্ক)অঙ্ক;
  }
  ঘূৰাই TRUE;
}

স্থির CURLcode ftp_state_pasv_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    পূর্ণাঙ্ক ftpcode)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result;
  ধ্রুৱ সংরচনা pingpong *pp = &ftpc->pp;
  ৱর্ণ *newhost = রিক্ত;
  unsigned short newport = 0;
  ধ্রুৱ ৱর্ণ *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  যদি((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ধ্রুৱ ৱর্ণ *ptr = strchr(str, '(');
    যদি(ptr) {
      ৱর্ণ sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      যদি(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ধ্রুৱ ৱর্ণ *p = &ptr[3];
        curl_off_t অঙ্ক;
        যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          ঘূৰাই CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)অঙ্ক;
        result = ftp_control_addr_dup(data, &newhost);
        যদি(result)
          ঘূৰাই result;
      }
      অন্যথা
        ptr = রিক্ত;
    }
    যদি(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      ঘূৰাই CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  অন্যথা যদি((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned পূর্ণাঙ্ক ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    যেতিয়ালৈ(*str) {
      যদি(match_pasv_6nums(str, ip))
        ভঙ্গ;
      str++;
    }

    যদি(!*str) {
      failf(data, "Could not interpret the 227-response");
      ঘূৰাই CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    যদি(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      যদি(result)
        ঘূৰাই result;
    }
    অন্যথা
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    যদি(!newhost)
      ঘূৰাই CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  অন্যথা যদি(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ঘূৰাই ftp_epsv_disable(data, ftpc, conn);
  }
  অন্যথা {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    ঘূৰাই CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  যদি(result)
    গচ্ছ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  যদি(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    যদি(result)
      গচ্ছ error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  যদি(result) {
    যদি((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    গচ্ছ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  যদি(data->set.verbose) {
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
  ঘূৰাই result;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode ftp_statemach(সংরচনা Curl_easy *data,
                              সংরচনা ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  ঘূৰাই result;
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
স্থির CURLcode ftp_do_more(সংরচনা Curl_easy *data, domore *more)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  সংরচনা FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  যদি(!ftpc || !ftp)
    ঘূৰাই CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  যদি(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    যদি(result == CURLE_OUT_OF_MEMORY)
      ঘূৰাই result;
    যদি(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      যদি(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ঘূৰাই ftp_epsv_disable(data, ftpc, conn);
      }
      ঘূৰাই result;
    }
  }

  যদি(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    যদি(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    যদি(result || !ftpc->wait_data_conn)
      ঘূৰাই result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  যদি(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    যদি(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      যদি(result)
        ঘূৰাই result; /* Failed to accept data connection */

      যদি(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        যদি(result)
          ঘূৰাই result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      অন্যথা {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        যদি(result)
          ঘূৰাই result;
      }
    }
    অন্যথা যদি(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      যদি(result)
        ঘূৰাই result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    অন্যথা {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      যদি(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      যদি(result)
        ঘূৰাই result;

      যদি((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        যদি(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          যদি(result)
            ঘূৰাই result;
        }
        /* otherwise fall through */
      }
      অন্যথা {
        যদি(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        অন্যথা {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        যদি(result)
          ঘূৰাই result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      যদি(complete)
        *more = DOMORE_DONE;
    }
    ঘূৰাই result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  যদি(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (পূর্ণাঙ্ক)result);
  }

  ঘূৰাই result;
}

/* call this when the DO phase has completed */
স্থির CURLcode ftp_dophase_done(সংরচনা Curl_easy *data,
                                 সংরচনা ftp_conn *ftpc,
                                 সংরচনা FTP *ftp,
                                 bool connected)
{
  যদি(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    যদি(result) {
      close_secondarysocket(data, ftpc);
      ঘূৰাই result;
    }
  }

  যদি(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  অন্যথা যদি(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode ftp_state_port_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    সংরচনা FTP *ftp,
                                    পূর্ণাঙ্ক ftpcode)
{
  সংরচনা connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  যদি(ftpcode / 100 != 2) {
    /* the command failed */

    যদি(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    যদি(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    অন্যথা
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  অন্যথা {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  ঘূৰাই result;
}

/* return TRUE on error, FALSE on success */
স্থির bool twodigit(ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক *val)
{
  যদি(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ঘূৰাই TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ঘূৰাই FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক *year, পূর্ণাঙ্ক *month, পূর্ণাঙ্ক *day,
                           পূর্ণাঙ্ক *hour, পূর্ণাঙ্ক *minute, পূর্ণাঙ্ক *second);
UNITTEST bool ftp_213_date(ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক *year, পূর্ণাঙ্ক *month, পূর্ণাঙ্ক *day,
                           পূর্ণাঙ্ক *hour, পূর্ণাঙ্ক *minute, পূর্ণাঙ্ক *second)
{
  পূর্ণাঙ্ক century;
  যদি((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    ঘূৰাই FALSE;

  *year += century * 100;
  যদি((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    ঘূৰাই FALSE;
  ঘূৰাই TRUE;
}

স্থির CURLcode client_write_header(সংরচনা Curl_easy *data,
                                    ৱর্ণ *buf, size_t blen)
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
  ঘূৰাই result;
}

স্থির CURLcode ftp_state_mdtm_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    সংরচনা FTP *ftp,
                                    পূর্ণাঙ্ক ftpcode)
{
  CURLcode result = CURLE_OK;

  ৱিকল্প(ftpcode) {
  পক্ষ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    পূর্ণাঙ্ক year, month, day, hour, minute, second;
    সংরচনা pingpong *pp = &ftpc->pp;
    ধ্রুৱ ৱর্ণ *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    যদি(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      ৱর্ণ timebuf[24];
      curl_msnprintf(timebuf, পরিমাণ(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      যদি(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#যদি defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ঠেলো
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    যদি(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#যদি defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic বেৰ_কৰো
#endif
      ৱর্ণ headerbuf[128];
      পূর্ণাঙ্ক headerbuflen;
      time_t filetime = data->info.filetime;
      সংরচনা tm buffer;
      ধ্রুৱ সংরচনা tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      যদি(result)
        ঘূৰাই result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, পরিমাণ(headerbuf),
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
      যদি(result)
        ঘূৰাই result;
    } /* end of a ridiculous amount of conditionals */
  }
    ভঙ্গ;
  স্ৱভাৱ:
    infof(data, "unsupported MDTM reply format");
    ভঙ্গ;
  পক্ষ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ভঙ্গ;
  }

  যদি(data->set.timecondition) {
    যদি((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      ৱিকল্প(data->set.timecondition) {
      পক্ষ CURL_TIMECOND_IFMODSINCE:
      স্ৱভাৱ:
        যদি(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ঘূৰাই CURLE_OK;
        }
        ভঙ্গ;
      পক্ষ CURL_TIMECOND_IFUNMODSINCE:
        যদি(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ঘূৰাই CURLE_OK;
        }
        ভঙ্গ;
      } /* switch */
    }
    অন্যথা {
      infof(data, "Skipping time comparison");
    }
  }

  যদি(!result)
    result = ftp_state_type(data, ftpc, ftp);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_type_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    সংরচনা FTP *ftp,
                                    পূর্ণাঙ্ক ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  যদি(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    ঘূৰাই CURLE_FTP_COULDNT_SET_TYPE;
  }
  যদি(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  যদি(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  অন্যথা যদি(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  অন্যথা যদি(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  অন্যথা যদি(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  অন্যথা যদি(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_size_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    সংরচনা FTP *ftp,
                                    পূর্ণাঙ্ক ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ধ্রুৱ ৱর্ণ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  যদি(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ধ্রুৱ ৱর্ণ *start = &buf[4];
    ধ্রুৱ ৱর্ণ *fdigit = memchr(start, '\r', len - 4);
    যদি(fdigit) {
      fdigit--;
      যদি(*fdigit == '\n')
        fdigit--;
      যেতিয়ালৈ(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    অন্যথা
      fdigit = start;
    যদি(curlx_str_সংখ্যা(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  অন্যথা যদি(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    ঘূৰাই CURLE_REMOTE_FILE_NOT_FOUND;
  }

  যদি(instate == FTP_SIZE) {
    যদি(filesize != -1) {
      ৱর্ণ clbuf[128];
      পূর্ণাঙ্ক clbuflen = curl_msnprintf(clbuf, পরিমাণ(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      যদি(result)
        ঘূৰাই result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  অন্যথা যদি(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  অন্যথা যদি(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_rest_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    সংরচনা FTP *ftp,
                                    পূর্ণাঙ্ক ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ৱিকল্প(instate) {
  পক্ষ FTP_REST:
  স্ৱভাৱ:
    যদি(ftpcode == 350) {
      ৱর্ণ buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      যদি(result)
        ঘূৰাই result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    ভঙ্গ;

  পক্ষ FTP_RETR_REST:
    যদি(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    অন্যথা {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      যদি(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    ভঙ্গ;
  }

  ঘূৰাই result;
}

স্থির CURLcode ftp_state_stor_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    পূর্ণাঙ্ক ftpcode)
{
  CURLcode result = CURLE_OK;

  যদি(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    ঘূৰাই CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  যদি(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    যদি(result)
      ঘূৰাই result;

    যদি(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      ঘূৰাই ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  ঘূৰাই ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
স্থির CURLcode ftp_state_get_resp(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc,
                                   সংরচনা FTP *ftp,
                                   পূর্ণাঙ্ক ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  যদি((ftpcode == 150) || (ftpcode == 125)) {

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

    যদি((instate != FTP_LIST) &&
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
      যদি(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        বাবে(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ধ্রুৱ ৱর্ণ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ধ্রুৱ ৱর্ণ *c = &buf[i];
          যদি(!curlx_str_সংখ্যা(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            ভঙ্গ;
          }
        }
      }
    }
    অন্যথা যদি(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    যদি(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    অন্যথা যদি((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    যদি(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    যদি(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      যদি(result)
        ঘূৰাই result;

      যদি(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        ঘূৰাই ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    ঘূৰাই ftp_initiate_transfer(data, ftpc);
  }
  অন্যথা {
    যদি((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    অন্যথা {
      failf(data, "RETR response: %03d", ftpcode);
      ঘূৰাই instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ঘূৰাই result;
}

/* after USER, PASS and ACCT */
স্থির CURLcode ftp_state_loggedin(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  যদি(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    যদি(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  অন্যথা {
    result = ftp_state_pwd(data, ftpc);
  }
  ঘূৰাই result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
স্থির bool ftp_has_ctrl(ধ্রুৱ ৱর্ণ *string)
{
  ধ্রুৱ unsigned ৱর্ণ *s = (ধ্রুৱ unsigned ৱর্ণ *)string;
  যেতিয়ালৈ(*s) {
    যদি(*s < 0x20)
      ঘূৰাই TRUE;
    s++;
  }
  ঘূৰাই FALSE;
}

/* for USER and PASS responses */
স্থির CURLcode ftp_state_user_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    পূর্ণাঙ্ক ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  যদি((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    যদি(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  অন্যথা যদি(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  অন্যথা যদি(ftpcode == 332) {
    ধ্রুৱ ৱর্ণ *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    যদি(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    অন্যথা যদি(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    অন্যথা {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      যদি(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  অন্যথা {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ধ্রুৱ ৱর্ণ *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    যদি(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      যদি(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      অন্যথা {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        যদি(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    অন্যথা {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  ঘূৰাই result;
}

/* for ACCT response */
স্থির CURLcode ftp_state_acct_resp(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc,
                                    পূর্ণাঙ্ক ftpcode)
{
  CURLcode result = CURLE_OK;
  যদি(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  অন্যথা
    result = ftp_state_loggedin(data, ftpc);

  ঘূৰাই result;
}

স্থির CURLcode ftp_pwd_resp(সংরচনা Curl_easy *data,
                             সংরচনা ftp_conn *ftpc,
                             পূর্ণাঙ্ক ftpcode)
{
  সংরচনা pingpong *pp = &ftpc->pp;
  CURLcode result;

  যদি(ftpcode == 257) {
    ধ্রুৱ ৱর্ণ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    সংরচনা dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    যেতিয়ালৈ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    যদি('\"' == *ptr) {
      /* it started good */
      বাবে(ptr++; *ptr; ptr++) {
        যদি('\"' == *ptr) {
          যদি('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          অন্যথা {
            /* end of path */
            যদি(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            ভঙ্গ; /* get out of this loop */
          }
        }
        অন্যথা {
          যদি(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        যদি(result) {
          curlx_dyn_free(&out);
          ঘূৰাই result;
        }
      }
    }
    যদি(entry_extracted) {
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
      ৱর্ণ *dir = curlx_dyn_ptr(&out);

      যদি(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        যদি(result) {
          curlx_dyn_free(&out);
          ঘূৰাই result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      যদি(!data->state.most_recent_ftp_entrypath)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      যদি(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        ঘূৰাই CURLE_OK;
      }
    }
    অন্যথা {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ ftpauth[] = { "SSL", "TLS" };

স্থির CURLcode ftp_wait_resp(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn,
                              সংরচনা ftp_conn *ftpc,
                              পূর্ণাঙ্ক ftpcode)
{
  CURLcode result = CURLE_OK;
  যদি(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    যদি(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ঘূৰাই ftp_state_user_resp(data, ftpc, ftpcode);
  }
  অন্যথা যদি(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  যদি(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ৱিকল্প((long)data->set.ftpsslauth) {
    পক্ষ CURLFTPAUTH_DEFAULT:
    পক্ষ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ভঙ্গ;
    পক্ষ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ভঙ্গ;
    স্ৱভাৱ:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (পূর্ণাঙ্ক)data->set.ftpsslauth);
      ঘূৰাই CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    যদি(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  অন্যথা
    result = ftp_state_user(data, ftpc, conn);
  ঘূৰাই result;
}

/* an implicit TLS scheme implies TLS on both connections */
স্থির unsigned ৱর্ণ ftp_use_ssl(সংরচনা connectdata *conn,
                                 সংরচনা ftp_conn *ftpc)
{
  যদি(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ঘূৰাই CURLUSESSL_ALL;
  ঘূৰাই ftpc->use_ssl;
}

স্থির CURLcode ftp_pp_statemachine(সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn)
{
  CURLcode result;
  পূর্ণাঙ্ক ftpcode;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  সংরচনা FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  সংরচনা pingpong *pp;
  size_t nread = 0;

  যদি(!ftpc || !ftp)
    ঘূৰাই CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  যদি(pp->sendleft)
    ঘূৰাই Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  যদি(result || !ftpcode)
    ঘূৰাই result;

  /* we have now received a full FTP server response */
  ৱিকল্প(ftpc->state) {
  পক্ষ FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    যদি(pp->overflow)
      ঘূৰাই CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    যদি((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        যদি(result) {
          /* we failed and bail out */
          ঘূৰাই CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      যদি(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    অন্যথা যদি(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    অন্যথা {
      যদি(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      অন্যথা
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    ভঙ্গ;

  পক্ষ FTP_USER:
  পক্ষ FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    যদি(!result)
      ftp_state(data, ftpc, FTP_PROT);
    ভঙ্গ;

  পক্ষ FTP_PROT:
    যদি(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    অন্যথা যদি(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ঘূৰাই CURLE_USE_SSL_FAILED;

    যদি(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      যদি(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    অন্যথা
      result = ftp_state_pwd(data, ftpc);
    ভঙ্গ;

  পক্ষ FTP_CCC:
    যদি(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned ৱর্ণ)CURLFTPSSL_CCC_ACTIVE));
      যদি(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    যদি(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    ভঙ্গ;

  পক্ষ FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_SYST:
    যদি(ftpcode == 215) {
      ধ্রুৱ ৱর্ণ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ধ্রুৱ ৱর্ণ *start;
      ৱর্ণ *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      যেতিয়ালৈ(*ptr == ' ')
        ptr++;
      বাবে(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      যদি(!os)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      যদি(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        যদি(result) {
          curlx_free(os);
          ঘূৰাই result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        ভঙ্গ;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    অন্যথা {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ভঙ্গ;

  পক্ষ FTP_NAMEFMT:
    যদি(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      ভঙ্গ;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ভঙ্গ;

  পক্ষ FTP_QUOTE:
  পক্ষ FTP_POSTQUOTE:
  পক্ষ FTP_RETR_PREQUOTE:
  পক্ষ FTP_STOR_PREQUOTE:
  পক্ষ FTP_LIST_PREQUOTE:
    যদি((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    অন্যথা
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    ভঙ্গ;

  পক্ষ FTP_CWD:
    যদি(ftpcode / 100 != 2) {
      /* failure to CWD there */
      যদি(data->set.ftp_create_missing_dirs &&
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
        যদি(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      অন্যথা {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    অন্যথা {
      /* success */
      ftpc->count2 = 0;
      যদি(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      অন্যথা {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ভঙ্গ;

  পক্ষ FTP_MKD:
    যদি((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    অন্যথা {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ভঙ্গ;

  পক্ষ FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_TYPE:
  পক্ষ FTP_LIST_TYPE:
  পক্ষ FTP_RETR_TYPE:
  পক্ষ FTP_STOR_TYPE:
  পক্ষ FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ভঙ্গ;

  পক্ষ FTP_SIZE:
  পক্ষ FTP_RETR_SIZE:
  পক্ষ FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ভঙ্গ;

  পক্ষ FTP_REST:
  পক্ষ FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ভঙ্গ;

  পক্ষ FTP_PRET:
    যদি(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      ঘূৰাই CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    ভঙ্গ;

  পক্ষ FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_LIST:
  পক্ষ FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ভঙ্গ;

  পক্ষ FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    ভঙ্গ;

  পক্ষ FTP_QUIT:
  স্ৱভাৱ:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    ভঙ্গ;
  }

  ঘূৰাই result;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode ftp_multi_statemach(সংরচনা Curl_easy *data,
                                    bool *done)
{
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ঘূৰাই ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

স্থির CURLcode ftp_block_statemach(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_conn *ftpc)
{
  সংরচনা pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(ftpc->state != FTP_STOP) {
    যদি(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    যদি(result)
      ভঙ্গ;
  }

  ঘূৰাই result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
স্থির CURLcode ftp_connect(সংরচনা Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  সংরচনা pingpong *pp;

  *done = FALSE; /* default to not done yet */
  যদি(!ftpc)
    ঘূৰাই CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    যদি(result)
      ঘূৰাই result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  ঘূৰাই result;
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
স্থির CURLcode ftp_sendquote(সংরচনা Curl_easy *data,
                              সংরচনা ftp_conn *ftpc,
                              সংরচনা curl_slist *quote)
{
  সংরচনা curl_slist *item;
  সংরচনা pingpong *pp = &ftpc->pp;

  item = quote;
  যেতিয়ালৈ(item) {
    যদি(item->data) {
      size_t nread;
      ধ্রুৱ ৱর্ণ *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      পূর্ণাঙ্ক ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      যদি(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      যদি(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      যদি(result)
        ঘূৰাই result;

      যদি(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        ঘূৰাই CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode ftp_done_status(সংরচনা Curl_easy *data,
                                সংরচনা ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  ৱিকল্প(status) {
  পক্ষ CURLE_BAD_DOWNLOAD_RESUME:
  পক্ষ CURLE_FTP_WEIRD_PASV_REPLY:
  পক্ষ CURLE_FTP_PORT_FAILED:
  পক্ষ CURLE_FTP_ACCEPT_FAILED:
  পক্ষ CURLE_FTP_ACCEPT_TIMEOUT:
  পক্ষ CURLE_FTP_COULDNT_SET_TYPE:
  পক্ষ CURLE_FTP_COULDNT_RETR_FILE:
  পক্ষ CURLE_PARTIAL_FILE:
  পক্ষ CURLE_UPLOAD_FAILED:
  পক্ষ CURLE_REMOTE_ACCESS_DENIED:
  পক্ষ CURLE_FILESIZE_EXCEEDED:
  পক্ষ CURLE_REMOTE_FILE_NOT_FOUND:
  পক্ষ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  পক্ষ CURLE_OK: /* does not affect the control connection's status */
    যদি(!premature)
      ভঙ্গ;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  স্ৱভাৱ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    ঘূৰাই status;      /* use the already set error code */
  }
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য ftp_done_wildcard(সংরচনা Curl_easy *data, সংরচনা ftp_conn *ftpc)
{
  যদি(data->state.wildcardmatch) {
    যদি(data->set.chunk_end && ftpc->file) {
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

স্থির শূন্য ftp_done_path(সংরচনা Curl_easy *data, সংরচনা ftp_conn *ftpc,
                          CURLcode result)
{
  সংরচনা connectdata *conn = data->conn;
  যদি(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  অন্যথা { /* remember working directory for connection reuse */
    ধ্রুৱ ৱর্ণ *rawPath = ftpc->rawpath;
    যদি(rawPath) {
      যদি((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      অন্যথা {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        যদি(!ftpc->cwdfail) {
          যদি(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          অন্যথা
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        অন্যথা
          ftpc->prevpath = রিক্ত; /* no path */
      }
    }
    যদি(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

স্থির CURLcode ftp_done_secondary_socket(সংরচনা Curl_easy *data,
                                          সংরচনা ftp_conn *ftpc,
                                          CURLcode result)
{
  সংরচনা connectdata *conn = data->conn;
  যদি(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    যদি(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      যদি(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  ঘূৰাই result;
}

স্থির CURLcode ftp_done_control_reply(সংরচনা Curl_easy *data,
                                       সংরচনা ftp_conn *ftpc,
                                       সংরচনা FTP *ftp, CURLcode result,
                                       bool premature)
{
  সংরচনা connectdata *conn = data->conn;
  size_t nread;
  পূর্ণাঙ্ক ftpcode;

  যদি(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    যদি(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    যদি(result)
      ঘূৰাই result;

    যদি(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      ঘূৰাই result;
    }

    যদি(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ৱিকল্প(ftpcode) {
      পক্ষ 226:
      পক্ষ 250:
        ভঙ্গ;
      পক্ষ 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        ভঙ্গ;
      স্ৱভাৱ:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        ভঙ্গ;
      }
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode ftp_done_check_partial(সংরচনা Curl_easy *data,
                                       সংরচনা ftp_conn *ftpc,
                                       সংরচনা FTP *ftp, CURLcode result,
                                       bool premature)
{
  যদি(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  অন্যথা যদি(data->state.upload) {
    যদি((ftp->transfer == PPTRANSFER_BODY) &&
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
  অন্যথা {
    যদি((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    অন্যথা যদি(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ঘূৰাই result;
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
স্থির CURLcode ftp_done(সংরচনা Curl_easy *data, CURLcode status,
                         bool premature)
{
  সংরচনা FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  যদি(!ftp || !ftpc)
    ঘূৰাই CURLE_OK;

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
  যদি(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
স্থির CURLcode ftp_nb_type(সংরচনা Curl_easy *data,
                            সংরচনা ftp_conn *ftpc,
                            সংরচনা FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  ৱর্ণ want = (ৱর্ণ)(ascii ? 'A' : 'I');

  যদি(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    ঘূৰাই ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  যদি(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ঘূৰাই result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
স্থির CURLcode ftp_perform(
  সংরচনা Curl_easy *data,
  সংরচনা ftp_conn *ftpc,
  সংরচনা FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  যদি(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  যদি(result)
    ঘূৰাই result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  যদি(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  অন্যথা
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  যদি(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ঘূৰাই result;
}

স্থির শূন্য wc_data_dtor(শূন্য *ptr)
{
  সংরচনা ftp_wc *ftpwc = ptr;
  যদি(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

স্থির CURLcode init_wc_data(সংরচনা Curl_easy *data,
                             সংরচনা ftp_conn *ftpc,
                             সংরচনা FTP *ftp)
{
  ৱর্ণ *last_slash;
  ৱর্ণ *path = ftp->path;
  সংরচনা WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  সংরচনা ftp_wc *ftpwc = রিক্ত;

  last_slash = strrchr(ftp->path, '/');
  যদি(last_slash) {
    last_slash++;
    যদি(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ঘূৰাই ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    যদি(!wildcard->pattern)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  অন্যথা { /* there is only 'wildcard pattern' or nothing */
    যদি(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      যদি(!wildcard->pattern)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    অন্যথা { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ঘূৰাই ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, পরিমাণ(সংরচনা ftp_wc));
  যদি(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  যদি(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  যদি(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  যদি(result) {
    গচ্ছ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  যদি(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  infof(data, "Wildcard - Parsing started");
  ঘূৰাই CURLE_OK;

fail:
  যদি(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = রিক্ত;
  ঘূৰাই result;
}

স্থির CURLcode wc_statemach(সংরচনা Curl_easy *data,
                             সংরচনা ftp_conn *ftpc,
                             সংরচনা FTP *ftp)
{
  সংরচনা WildcardData * ধ্রুৱ wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  বাবে(;;) {
    ৱিকল্প(wildcard->state) {
    পক্ষ CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      যদি(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ঘূৰাই result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      ঘূৰাই result;

    পক্ষ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      সংরচনা ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      যদি(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        অনুৱর্ত;
      }
      যদি(Curl_llist_গুনতি(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ঘূৰাই CURLE_REMOTE_FILE_NOT_FOUND;
      }
      অনুৱর্ত;
    }

    পক্ষ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      সংরচনা Curl_llist_গ্ৰন্থি *head = Curl_llist_head(&wildcard->filelist);
      সংরচনা curl_fileinfo *finfo = Curl_গ্ৰন্থি_elem(head);
      ৱর্ণ *tmp_path;
      ৱর্ণ *enc = curl_easy_escape(রিক্ত, finfo->filename, 0);
      যদি(!enc)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      যদি(!tmp_path)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      যদি(data->set.chunk_bgn) {
        long userresponse;
        সংরচনা Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (পূর্ণাঙ্ক)Curl_llist_গুনতি(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        ৱিকল্প(userresponse) {
        পক্ষ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          অনুৱর্ত;
        পক্ষ CURL_CHUNK_BGN_FUNC_FAIL:
          ঘূৰাই CURLE_CHUNK_FAILED;
        }
      }

      যদি(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        অনুৱর্ত;
      }

      যদি(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      যদি(result)
        ঘূৰাই result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_গ্ৰন্থি_remove(Curl_llist_head(&wildcard->filelist));

      যদি(Curl_llist_গুনতি(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ঘূৰাই CURLE_OK;
      }
      ঘূৰাই result;
    }

    পক্ষ CURLWC_SKIP: {
      যদি(data->set.chunk_end) {
        সংরচনা Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_গ্ৰন্থি_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_গুনতি(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      অনুৱর্ত;
    }

    পক্ষ CURLWC_CLEAN: {
      সংরচনা ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      যদি(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      ঘূৰাই result;
    }

    পক্ষ CURLWC_DONE:
    পক্ষ CURLWC_ERROR:
    পক্ষ CURLWC_CLEAR:
      যদি(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = রিক্ত;
      }
      ঘূৰাই result;
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
স্থির CURLcode ftp_regular_transfer(সংরচনা Curl_easy *data,
                                     সংরচনা ftp_conn *ftpc,
                                     সংরচনা FTP *ftp,
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

  যদি(!result) {

    যদি(!*dophase_done)
      /* the DO phase has not completed yet */
      ঘূৰাই CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    যদি(result)
      ঘূৰাই result;
  }
  অন্যথা
    freedirs(ftpc);

  ঘূৰাই result;
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
স্থির CURLcode ftp_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  সংরচনা FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  যদি(!ftpc || !ftp)
    ঘূৰাই CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    সংরচনা Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    যদি(result)
      ঘূৰাই result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    যদি(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      ঘূৰাই result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  যদি(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    যদি(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      ঘূৰাই CURLE_OK;
    }
    যদি(result) /* error, loop or skipping the file */
      ঘূৰাই result;
  }
  অন্যথা { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    যদি(result)
      ঘূৰাই result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  ঘূৰাই result;
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
স্থির CURLcode ftp_quit(সংরচনা Curl_easy *data,
                         সংরচনা ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  যদি(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    যদি(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      ঘূৰাই result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode ftp_disconnect(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn,
                               bool dead_connection)
{
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ঘূৰাই CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  যদি(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (শূন্য)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  ঘূৰাই CURLE_OK;
}

/* called from multi.c while DOing */
স্থির CURLcode ftp_doing(সংরচনা Curl_easy *data,
                          bool *dophase_done)
{
  সংরচনা ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  সংরচনা FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  যদি(!ftpc || !ftp)
    ঘূৰাই CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  যদি(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  অন্যথা যদি(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ঘূৰাই result;
}

স্থির শূন্য ftp_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা FTP *ftp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

স্থির শূন্য ftp_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা ftp_conn *ftpc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

স্থির শূন্য type_url_check(সংরচনা Curl_easy *data, সংরচনা FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  যদি((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    ৱর্ণ *type = &ftp->path[len - 7];
    ৱর্ণ command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ৱিকল্প(command) {
    পক্ষ 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      ভঙ্গ;

    পক্ষ 'D': /* directory mode */
      data->state.list_only = TRUE;
      ভঙ্গ;

    পক্ষ 'I': /* binary mode */
    স্ৱভাৱ:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      ভঙ্গ;
    }
  }
}

স্থির CURLcode ftp_setup_connection(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn)
{
  সংরচনা FTP *ftp;
  CURLcode result = CURLE_OK;
  সংরচনা ftp_conn *ftpc;

  ftp = curlx_calloc(1, পরিমাণ(*ftp));
  যদি(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, পরিমাণ(*ftpc));
  যদি(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  যদি(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    যদি(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }
  যদি(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    যদি(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
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
               (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

bool Curl_ftp_conns_match(সংরচনা connectdata *needle, সংরচনা connectdata *conn)
{
  সংরচনা ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  সংরচনা ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  যদি(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    ঘূৰাই FALSE;

  ৱিকল্প(nftpc->use_ssl) {
  পক্ষ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ঘূৰাই (cftpc->use_ssl != CURLUSESSL_NONE);
  স্ৱভাৱ:
    ঘূৰাই (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_ftp = {
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
