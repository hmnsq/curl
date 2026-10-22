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
#অন্তর্ভাব "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "if2ip.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "ftp.h"
#অন্তর্ভাব "ftp-int.h"
#অন্তর্ভাব "ftplistparser.h"
#অন্তর্ভাব "curl_range.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "parsedate.h" /* for the week day and month names */
#অন্তর্ভাব "sockaddr.h" /* required for Curl_sockaddr_storage */
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(সারি) \
  (ISDIGIT((সারি)[0]) && ISDIGIT((সারি)[1]) && ISDIGIT((সারি)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(সারি) (STATUSCODE(সারি) && (' ' == (সারি)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
স্থির ধ্রুবক অক্ষর * ধ্রুবক ftp_state_names[] = {
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
স্থির শূন্য ftp_state_নিচের(কাঠামো Curl_easy *তথ্য,
                          কাঠামো ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , পূর্ণসংখ্যা lineno
#endif
  )
{
  যদি(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((শূন্য)lineno);
    CURL_TRC_FTP(তথ্য, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#অন্যথা
    CURL_TRC_FTP(তথ্য, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_নিচের(x, y, z)
#অন্যথা /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_নিচের(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

স্থির CURLcode ftp_state_mdtm(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp);
স্থির CURLcode ftp_state_quote(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ftp_conn *ftpc,
                                কাঠামো FTP *ftp,
                                bool init, ftpstate instate);
স্থির CURLcode ftp_nb_type(কাঠামো Curl_easy *তথ্য,
                            কাঠামো ftp_conn *ftpc,
                            কাঠামো FTP *ftp,
                            bool ascii, ftpstate newstate);
স্থির CURLcode getftpresponse(কাঠামো Curl_easy *তথ্য, size_t *nreadp,
                               পূর্ণসংখ্যা *ftpcodep);

স্থির শূন্য freedirs(কাঠামো ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ফাঁকা;
}

স্থির size_t numof_slashes(ধ্রুবক অক্ষর *str)
{
  ধ্রুবক অক্ষর *slashPos;
  size_t অঙ্ক = 0;
  করো {
    slashPos = strchr(str, '/');
    যদি(slashPos) {
      ++অঙ্ক;
      str = slashPos + 1;
    }
  } যখন(slashPos);
  ফেরত অঙ্ক;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
স্থির CURLcode ftp_parse_url_path(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc,
                                   কাঠামো FTP *ftp)
{
  ধ্রুবক অক্ষর *slashPos = ফাঁকা;
  ধ্রুবক অক্ষর *fileName = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *rawPath = ফাঁকা; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  যদি(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  ফলাফল = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  যদি(ফলাফল) {
    failf(তথ্য, "path contains control characters");
    ফেরত ফলাফল;
  }
  rawPath = ftpc->rawpath;

  নির্বাচন(তথ্য->set.ftp_filemethod) {
  ক্ষেত্র FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    যদি((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    বিরতি;

  ক্ষেত্র FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    যদি(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      যদি(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, আকার(ftpc->dirs[0]));
      যদি(!ftpc->dirs)
        ফেরত CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (পূর্ণসংখ্যা)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    অন্যথা
      fileName = rawPath; /* filename only (or empty) */
    বিরতি;

  পূর্বনির্ধারিত: /* allow pretty much anything */
  ক্ষেত্র FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ধ্রুবক অক্ষর *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    যদি(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      ফেরত CURLE_URL_MALFORMAT;

    যদি(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, আকার(ftpc->dirs[0]));
      যদি(!ftpc->dirs)
        ফেরত CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      যখন(dirAlloc--) {
        ধ্রুবক অক্ষর *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        যদি(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        যদি(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (পূর্ণসংখ্যা)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (পূর্ণসংখ্যা)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    বিরতি;
  } /* switch */

  যদি(fileName && *fileName)
    ftpc->file = fileName;
  অন্যথা
    ftpc->file = ফাঁকা; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  যদি(তথ্য->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(তথ্য, "Uploading to a URL without a filename");
    ফেরত CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  যদি((তথ্য->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  অন্যথা { /* newly created FTP connections are already in entry path */
    ধ্রুবক অক্ষর *oldPath = তথ্য->conn->bits.reuse ? ftpc->prevpath : "";
    যদি(oldPath) {
      size_t n = pathLen;
      যদি(তথ্য->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      অন্যথা
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      যদি((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(তথ্য, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ফেরত CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
স্থির পূর্ণসংখ্যা ftp_need_type(কাঠামো ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ফেরত ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

স্থির শূন্য close_secondarysocket(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ftp_conn *ftpc)
{
  (শূন্য)ftpc;
  CURL_TRC_FTP(তথ্য, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(তথ্য, তথ্য->conn, SECONDARYSOCKET);
}

#ifdef CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
কাঠামো ftp_cw_lc_ctx {
  কাঠামো Curl_cwriter super;
  bool newline_pending;
};

স্থির CURLcode ftp_cw_lc_write(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                ধ্রুবক অক্ষর *buf, size_t blen)
{
  স্থির ধ্রুবক অক্ষর nl = '\n';
  কাঠামো ftp_cw_lc_ctx *ctx = writer->ctx;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ফেরত CURLE_FAILED_INIT;

  যদি(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  যখন(blen) {
    /* do not pass EOS when writing parts */
    পূর্ণসংখ্যা chunk_type = (type & ~CLIENTWRITE_EOS);
    ধ্রুবক অক্ষর *cp;
    size_t chunk_len;
    CURLcode ফলাফল;

    যদি(ctx->newline_pending) {
      যদি(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, chunk_type, &nl, 1);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    যদি(!cp)
      বিরতি;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    যদি(chunk_len) {
      ফলাফল = Curl_cwriter_write(তথ্য, writer->পরবর্তী, chunk_type,
                                  buf, chunk_len);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  যদি(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, blen);
  }
  অন্যথা যদি(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    যদি(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, 0);
  }
  ফেরত CURLE_OK;
}

স্থির ধ্রুবক কাঠামো Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ফাঁকা,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  আকার(কাঠামো ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
স্থির CURLcode ftp_check_ctrl_on_তথ্য_wait(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ftp_conn *ftpc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  কাঠামো pingpong *pp = &ftpc->pp;
  size_t nread;
  পূর্ণসংখ্যা ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  যদি(curlx_dyn_len(&pp->recvbuf)) {
    ধ্রুবক অক্ষর *l = curlx_dyn_ptr(&pp->recvbuf);
    যদি(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(তথ্য, "There is negative response in cache while serv connect");
      (শূন্য)getftpresponse(তথ্য, &nread, &ftpcode);
      ফেরত CURLE_FTP_ACCEPT_FAILED;
    }
  }

  যদি(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  অন্যথা {
    পূর্ণসংখ্যা socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    নির্বাচন(socketstate) {
    ক্ষেত্র -1: /* error */
      /* let's die here */
      failf(তথ্য, "Error while waiting for server connect");
      ফেরত CURLE_FTP_ACCEPT_FAILED;
    পূর্বনির্ধারিত:
      যদি(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      বিরতি;
    }
  }

  যদি(response) {
    infof(তথ্য, "Ctrl conn has data while waiting for data conn");
    যদি(pp->overflow > 3) {
      ধ্রুবক অক্ষর *r = curlx_dyn_ptr(&pp->recvbuf);
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
          infof(তথ্য, "Got 226 before data activity");
          ফেরত CURLE_OK;
        }
      }
    }

    (শূন্য)getftpresponse(তথ্য, &nread, &ftpcode);

    infof(তথ্য, "FTP code: %03d", ftpcode);

    যদি(ftpcode / 100 > 3)
      ফেরত CURLE_FTP_ACCEPT_FAILED;

    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  ফেরত CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
স্থির CURLcode ftp_initiate_transfer(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ftp_conn *ftpc)
{
  CURLcode ফলাফল = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(তথ্য, "ftp_initiate_transfer()");
  ফলাফল = Curl_conn_connect(তথ্য, SECONDARYSOCKET, TRUE, &connected);
  যদি(ফলাফল || !connected)
    ফেরত ফলাফল;

  যদি(তথ্য->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(তথ্য, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(তথ্য, TRUE, TRUE);
  }
  অন্যথা {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(তথ্য, SECONDARYSOCKET, তথ্য->req.আকার_2);
    Curl_xfer_set_shutdown(তথ্য, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(তথ্য, ftpc, FTP_STOP);

  ফেরত CURLE_OK;
}

স্থির bool ftp_endofresp(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                          ধ্রুবক অক্ষর *সারি, size_t len, পূর্ণসংখ্যা *code)
{
  curl_off_t status;
  (শূন্য)তথ্য;
  (শূন্য)conn;

  যদি((len > 3) && LASTLINE(সারি) && !curlx_str_সংখ্যা(&সারি, &status, 999)) {
    *code = (পূর্ণসংখ্যা)status;
    ফেরত TRUE;
  }

  ফেরত FALSE;
}

স্থির CURLcode ftp_readresp(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ftp_conn *ftpc,
                             int8_t sockindex,
                             কাঠামো pingpong *pp,
                             পূর্ণসংখ্যা *ftpcodep, /* return the ftp-code if done */
                             size_t *আকার_2) /* size of the response */
{
  পূর্ণসংখ্যা code;
  CURLcode ফলাফল = Curl_pp_readresp(তথ্য, sockindex, pp, &code, আকার_2);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  যদি(!ftpc->shutdown)
    তথ্য->info.httpcode = code;

  *ftpcodep = code;

  যদি(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(তথ্য, "We got a 421 - timeout");
    ftp_state(তথ্য, ftpc, FTP_STOP);
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }

  ফেরত ফলাফল;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
স্থির CURLcode getftpresponse(কাঠামো Curl_easy *তথ্য,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               পূর্ণসংখ্যা *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  কাঠামো pingpong *pp = &ftpc->pp;
  size_t nread;
  পূর্ণসংখ্যা cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(তথ্য, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  যদি(!ftpc)
    ফেরত CURLE_FAILED_INIT;

  যখন(!*ftpcodep && !ফলাফল) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(তথ্য, pp);
    timediff_t interval_ms;

    যদি(timeout <= 0) {
      failf(তথ্য, "FTP response timeout");
      ফেরত CURLE_OPERATION_TIMEDOUT; /* already too little time */
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
    অন্যথা যদি(!Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(তথ্য, pp) ?
        sockfd : CURL_SOCKET_BAD;
      পূর্ণসংখ্যা ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      যদি(ev < 0) {
        failf(তথ্য, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ফেরত CURLE_RECV_ERROR;
      }
      অন্যথা যদি(ev == 0) {
        ফলাফল = Curl_pgrsUpdate(তথ্য);
        চালু; /* continue in our loop for the timeout duration */
      }
    }

    যদি(Curl_pp_needs_flush(তথ্য, pp)) {
      ফলাফল = Curl_pp_flushsend(তথ্য, pp);
      যদি(ফলাফল)
        বিরতি;
    }

    ফলাফল = ftp_readresp(তথ্য, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    যদি(ফলাফল)
      বিরতি;

    যদি(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
তথ্য*/
      cache_skip++;
    অন্যথা
      /* when we got data or there is no cache left, we reset the cache skip
গণক*/
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(তথ্য, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (পূর্ণসংখ্যা)ফলাফল, *nreadp, *ftpcodep);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_user(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  যদি(!ফলাফল) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(তথ্য, ftpc, FTP_USER);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_pwd(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ftp_conn *ftpc)
{
  CURLcode ফলাফল;
#ifdef DEBUGBUILD
  যদি(!তথ্য->id && getenv("CURL_FTP_PWD_STOP"))
    ফেরত CURLE_OK;
#endif
  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "PWD");
  যদি(!ফলাফল)
    ftp_state(তথ্য, ftpc, FTP_PWD);

  ফেরত ফলাফল;
}

/* For the FTP "protocol connect" and "doing" phases only */
স্থির CURLcode ftp_pollset(কাঠামো Curl_easy *তথ্য,
                            কাঠামো easy_pollset *ps)
{
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  ফেরত ftpc ? Curl_pp_pollset(তথ্য, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
স্থির CURLcode ftp_domore_pollset(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো easy_pollset *ps)
{
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ফেরত CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(তথ্য, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  যদি(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(তথ্য->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (তথ্য->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(তথ্য->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ফেরত Curl_pollset_add_in(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
  }
  ফেরত Curl_pp_pollset(তথ্য, &ftpc->pp, ps);
}

স্থির পূর্ণসংখ্যা pathlen(কাঠামো ftp_conn *ftpc, পূর্ণসংখ্যা অঙ্ক)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > অঙ্ক);
  ফেরত ftpc->dirs[অঙ্ক].len;
}

স্থির ধ্রুবক অক্ষর *pathpiece(কাঠামো ftp_conn *ftpc, পূর্ণসংখ্যা অঙ্ক)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > অঙ্ক);
  ফেরত &ftpc->rawpath[ftpc->dirs[অঙ্ক].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
স্থির CURLcode ftp_state_cwd(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ftp_conn *ftpc,
                              কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(ftpc->cwddone)
    /* already done and fine */
    ফলাফল = ftp_state_mdtm(তথ্য, ftpc, ftp);
  অন্যথা {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((তথ্য->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    যদি(তথ্য->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "CWD %s", ftpc->entrypath);
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_CWD);
    }
    অন্যথা {
      যদি(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        যদি(!ফলাফল)
          ftp_state(তথ্য, ftpc, FTP_CWD);
      }
      অন্যথা {
        /* No CWD necessary */
        ফলাফল = ftp_state_mdtm(তথ্য, ftpc, ftp);
      }
    }
  }
  ফেরত ফলাফল;
}

প্রকারলেখ গণনা {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
স্থির CURLcode ftp_port_parse_string(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn,
                                      ধ্রুবক অক্ষর *string_ftpport,
                                      কাঠামো Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ধ্রুবক অক্ষর **hostp,
                                      অক্ষর *hbuf, size_t hbuflen)
{
  ধ্রুবক অক্ষর *ip_end = ফাঁকা;
  ধ্রুবক অক্ষর *addr = ফাঁকা;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  অক্ষর ipstr[50];
#ifndef USE_IPV6
  (শূন্য)conn;
  (শূন্য)ss;
#endif

  /* default to nothing */
  *hostp = ফাঁকা;
  *port_minp = *port_maxp = 0;

  যদি(!string_ftpport || (strlen(string_ftpport) <= 1))
    যাও done;

#ifdef USE_IPV6
  যদি(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ধ্রুবক অক্ষর *ip_start = string_ftpport + 1;
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
        কাঠামো sockaddr_in6 * ধ্রুবক sa6 = (শূন্য *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        যদি(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = ফাঁকা; /* this got no port ! */
        }
#endif
      }
      অন্যথা
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  যদি(ip_end) {
    ধ্রুবক অক্ষর *portp = strchr(ip_end, ':');
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
    ধ্রুবক কাঠামো Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(তথ্য, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    যদি(!remote_addr || (addrlen >= আকার(ipstr)) || (addrlen >= hbuflen))
      ফেরত CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    নির্বাচন(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    ক্ষেত্র IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      বিরতি;
    ক্ষেত্র IF2IP_AF_NOT_SUPPORTED:
      ফেরত CURLE_FTP_PORT_FAILED;
    ক্ষেত্র IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      বিরতি;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  ফেরত CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
স্থির CURLcode ftp_port_default_host(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn,
                                      কাঠামো Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ধ্রুবক অক্ষর **hostp,
                                      অক্ষর *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  কাঠামো sockaddr *sa = (কাঠামো sockaddr *)ss;
  কাঠামো sockaddr_in * ধ্রুবক sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 * ধ্রুবক sa6 = (শূন্য *)sa;
#endif
  অক্ষর বাফার[STRERROR_LEN];
  CURLcode ফলাফল;

  *sslenp = আকার(*ss);
  যদি(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(তথ্য, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
    ফেরত CURLE_FTP_PORT_FAILED;
  }
  নির্বাচন(sa->sa_family) {
#ifdef USE_IPV6
  ক্ষেত্র AF_INET6:
    ফলাফল = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    ফলাফল = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    বিরতি;
  }
  যদি(ফলাফল)
    ফেরত CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ফেরত CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
স্থির CURLcode ftp_port_resolve_host(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn,
                                      ধ্রুবক অক্ষর *host,
                                      কাঠামো Curl_dns_entry **dns_entryp,
                                      ধ্রুবক কাঠামো Curl_addrinfo **resp)
{
  CURLcode ফলাফল;

  *resp = ফাঁকা;
  ফলাফল = Curl_resolv_blocking(
    তথ্য, Curl_resolv_dns_queries(তথ্য, conn->ip_version),
    host, 0, Curl_conn_get_transport(তথ্য, conn), dns_entryp);
  যদি(ফলাফল)
    failf(তথ্য, "failed to resolve the address provided to PORT: %s", host);
  অন্যথা {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ফেরত ফলাফল;
}

/*
 * Open a TCP socket for the resolved address family.
 */
স্থির CURLcode ftp_port_open_socket(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn,
                                     ধ্রুবক কাঠামো Curl_addrinfo *res,
                                     ধ্রুবক কাঠামো Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  অক্ষর বাফার[STRERROR_LEN];
  পূর্ণসংখ্যা sockerr = 0;
  ধ্রুবক কাঠামো Curl_addrinfo *ai;
  CURLcode ফলাফল = CURLE_FTP_PORT_FAILED;

  জন্য(ai = res; ai; ai = ai->ai_পরবর্তী) {
    ফলাফল =
      Curl_socket_open(তথ্য, ai, ফাঁকা,
                       Curl_conn_get_transport(তথ্য, conn), portsockp);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        ফেরত ফলাফল;
      ফলাফল = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      চালু;
    }
    বিরতি;
  }
  যদি(!ai) {
    failf(তথ্য, "socket failure: %s",
          curlx_strerror(sockerr, বাফার, আকার(বাফার)));
    ফেরত CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  ফেরত ফলাফল;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
স্থির CURLcode ftp_port_bind_socket(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn,
                                     curl_socket_t portsock,
                                     ধ্রুবক কাঠামো Curl_addrinfo *ai,
                                     কাঠামো Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  কাঠামো sockaddr *sa = (কাঠামো sockaddr *)ss;
  কাঠামো sockaddr_in * ধ্রুবক sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 * ধ্রুবক sa6 = (শূন্য *)sa;
#endif
  অক্ষর বাফার[STRERROR_LEN];
  unsigned short port;
  পূর্ণসংখ্যা sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  জন্য(port = port_min; port <= port_max;) {
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
        infof(তথ্য, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, বাফার, আকার(বাফার)));

        *sslen_io = আকার(*ss);
        যদি(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(তথ্য, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
          ফেরত CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        চালু;
      }
      যদি(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(তথ্য, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, বাফার, আকার(বাফার)));
        ফেরত CURLE_FTP_PORT_FAILED;
      }
    }
    অন্যথা
      বিরতি;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    যদি(port == port_max) {
      failf(তথ্য, "bind() failed, ran out of ports");
      ফেরত CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = আকার(*ss);
  যদি(getsockname(portsock, sa, sslen_io)) {
    failf(তথ্য, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
    ফেরত CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(তথ্য, "ftp_port_bind_socket(), socket bound to port %d", port);
  ফেরত CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
স্থির CURLcode ftp_port_listen(কাঠামো Curl_easy *তথ্য, curl_socket_t portsock)
{
  অক্ষর বাফার[STRERROR_LEN];

  যদি(listen(portsock, 1)) {
    failf(তথ্য, "socket failure: %s",
          curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
    ফেরত CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(তথ্য, "ftp_port_listen(), listening on port");
  ফেরত CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
স্থির CURLcode ftp_port_send_command(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ftp_conn *ftpc,
                                      কাঠামো connectdata *conn,
                                      কাঠামো Curl_sockaddr_storage *ss,
                                      ধ্রুবক কাঠামো Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  স্থির ধ্রুবক অক্ষর mode[][5] = { "EPRT", "PORT" };
  কাঠামো sockaddr *sa = (কাঠামো sockaddr *)ss;
  কাঠামো sockaddr_in * ধ্রুবক sa4 = (শূন্য *)sa;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 * ধ্রুবক sa6 = (শূন্য *)sa;
#endif
  অক্ষর myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode ফলাফল;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, আকার(myhost));

#ifdef USE_IPV6
  যদি(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  জন্য(; fcmd != DONE; fcmd++) {

    যদি(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      চালু;

    যদি((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      চালু;

    নির্বাচন(sa->sa_family) {
    ক্ষেত্র AF_INET:
      port = ntohs(sa4->sin_port);
      বিরতি;
#ifdef USE_IPV6
    ক্ষেত্র AF_INET6:
      port = ntohs(sa6->sin6_port);
      বিরতি;
#endif
    পূর্বনির্ধারিত:
      চালু; /* might as well skip this */
    }

    যদি(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      যদি(ফলাফল) {
        failf(তথ্য, "Failure sending EPRT command: %s",
              curl_easy_strerror(ফলাফল));
        ফেরত ফলাফল;
      }
      বিরতি;
    }
    যদি(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      অক্ষর target[আকার(myhost) + 20];
      ধ্রুবক অক্ষর *source = myhost;
      অক্ষর *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      যখন(*source) {
        যদি(*source == '.')
          *dest = ',';
        অন্যথা
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (পূর্ণসংখ্যা)(port >> 8), (পূর্ণসংখ্যা)(port & 0xff));

      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s %s", mode[fcmd], target);
      যদি(ফলাফল) {
        failf(তথ্য, "Failure sending PORT command: %s",
              curl_easy_strerror(ফলাফল));
        ফেরত ফলাফল;
      }
      বিরতি;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(তথ্য, ftpc, FTP_PORT);
  ফেরত CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
স্থির CURLcode ftp_state_use_port(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode ফলাফল = CURLE_FTP_PORT_FAILED;
  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  কাঠামো Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  অক্ষর hbuf[NI_MAXHOST];
  ধ্রুবক অক্ষর *host = ফাঁকা;
  ধ্রুবক অক্ষর *string_ftpport = CURL_EASY_STR(তথ্য, STRING_FTPPORT);
  কাঠামো Curl_dns_entry *dns_entry = ফাঁকা;
  ধ্রুবক কাঠামো Curl_addrinfo *res = ফাঁকা;
  ধ্রুবক কাঠামো Curl_addrinfo *ai = ফাঁকা;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  ফলাফল = ftp_port_parse_string(তথ্য, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, আকার(hbuf));
  যদি(!ফলাফল && !host)
    /* if no host was specified, use the control connection's local IP */
    ফলাফল = ftp_port_default_host(তথ্য, conn, &ss, &sslen, &host,
                                   hbuf, আকার(hbuf), &non_local);

  /* resolve host string to address list */
  যদি(!ফলাফল)
    ফলাফল = ftp_port_resolve_host(তথ্য, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  যদি(!ফলাফল)
    ফলাফল = ftp_port_open_socket(তথ্য, conn, res, &ai, &portsock);
  যদি(!ফলাফল) {
    CURL_TRC_FTP(তথ্য, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    ফলাফল = ftp_port_bind_socket(তথ্য, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  যদি(!ফলাফল)
    ফলাফল = ftp_port_listen(তথ্য, portsock);

  /* send the PORT / EPRT command */
  যদি(!ফলাফল)
    ফলাফল = ftp_port_send_command(তথ্য, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_tcp_listen_set(তথ্য, conn, SECONDARYSOCKET, &portsock);

  যদি(!ফলাফল)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  যদি(dns_entry)
    Curl_dns_entry_unlink(তথ্য, &dns_entry);
  যদি(ফলাফল) {
    ftp_state(তথ্য, ftpc, FTP_STOP);
  }
  অন্যথা {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    যদি(conn->bits.ftp_use_তথ্য_ssl && তথ্য->set.ftp_use_port &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      ফলাফল = Curl_ssl_cfilter_add(
        তথ্য, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->bits.do_more = FALSE;
    Curl_expire(তথ্য, (তথ্য->set.accepttimeout > 0) ?
                তথ্য->set.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  যদি(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(তথ্য, conn, portsock);
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_use_pasv(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc,
                                   কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  স্থির ধ্রুবক অক্ষর mode[][5] = { "EPSV", "PASV" };
  পূর্ণসংখ্যা modeoff;

#ifdef PF_INET6
  যদি(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", mode[modeoff]);
  যদি(!ফলাফল) {
    ftpc->count1 = modeoff;
    ftp_state(তথ্য, ftpc, FTP_PASV);
    infof(তথ্য, "Connect data stream passively");
  }
  ফেরত ফলাফল;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
স্থির CURLcode ftp_state_prepare_transfer(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ftp_conn *ftpc,
                                           কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  যদি(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(তথ্য, ftpc, FTP_RETR_PREQUOTE);
    ফলাফল = ftp_state_quote(তথ্য, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  অন্যথা যদি(তথ্য->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    ফলাফল = ftp_state_use_port(তথ্য, ftpc, EPRT);
  }
  অন্যথা {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    যদি(তথ্য->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      যদি(!ftpc->file)
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) :
                               (তথ্য->state.list_only ? "NLST" : "LIST"));
      অন্যথা যদি(তথ্য->state.upload)
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "PRET STOR %s", ftpc->file);
      অন্যথা
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "PRET RETR %s", ftpc->file);
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_PRET);
    }
    অন্যথা
      ফলাফল = ftp_state_use_pasv(তথ্য, ftpc, conn);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_rest(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "REST %d", 0);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_REST);
  }
  অন্যথা
    ফলাফল = ftp_state_prepare_transfer(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_আকার(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "SIZE %s", ftpc->file);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_আকার);
  }
  অন্যথা
    ফলাফল = ftp_state_rest(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_list(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  ধ্রুবক অক্ষর *lstArg = ফাঁকা;
  পূর্ণসংখ্যা lstArglen = 0;
  অক্ষর *cmd;

  যদি((তথ্য->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ধ্রুবক অক্ষর *rawPath = ftpc->rawpath;
    ধ্রুবক অক্ষর *slashPos = strrchr(rawPath, '/');
    যদি(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      যদি(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (পূর্ণসংখ্যা)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) :
                      (তথ্য->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  যদি(!cmd)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  যদি(!ফলাফল)
    ftp_state(তথ্য, ftpc, FTP_LIST);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_list_prequote(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ftp_conn *ftpc,
                                        কাঠামো FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ফেরত ftp_state_quote(তথ্য, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

স্থির CURLcode ftp_state_retr_prequote(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ftp_conn *ftpc,
                                        কাঠামো FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ফেরত ftp_state_quote(তথ্য, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

স্থির CURLcode ftp_state_stor_prequote(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ftp_conn *ftpc,
                                        কাঠামো FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ফেরত ftp_state_quote(তথ্য, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

স্থির CURLcode ftp_state_type(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  যদি(তথ্য->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)তথ্য->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    ফলাফল = ftp_nb_type(তথ্য, ftpc, ftp, (bool)তথ্য->state.prefer_ascii,
                         FTP_TYPE);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
    ফলাফল = ftp_state_আকার(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
স্থির CURLcode ftp_state_mdtm(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp)
{
  CURLcode ফলাফল = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  যদি((তথ্য->set.get_filetime || তথ্য->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "MDTM %s", ftpc->file);

    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_MDTM);
  }
  অন্যথা
    ফলাফল = ftp_state_type(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

/* This is called after the TYPE and possible quote commands have been sent */
স্থির CURLcode ftp_state_ul_setup(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc,
                                   কাঠামো FTP *ftp,
                                   bool sizechecked)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_bit append = তথ্য->set.remote_append;

  যদি((তথ্য->state.resume_from && !sizechecked) ||
     ((তথ্য->state.resume_from > 0) && sizechecked)) {
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
    পূর্ণসংখ্যা seekerr = CURL_SEEKFUNC_OK;

    যদি(তথ্য->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "SIZE %s", ftpc->file);
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_STOR_আকার);
      ফেরত ফলাফল;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    যদি(তথ্য->set.seek_func) {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_seek_func);
      seekerr = তথ্য->set.seek_func(তথ্য->set.seek_client,
                                    তথ্য->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    যদি(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(তথ্য, "Could not seek stream");
        ফেরত CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      করো {
        অক্ষর scratch[4 * 1024];
        size_t readthisamountnow =
          (তথ্য->state.resume_from - passed > (curl_off_t)আকার(scratch)) ?
          আকার(scratch) :
          curlx_sotouz(তথ্য->state.resume_from - passed);

        size_t actuallyread =
          তথ্য->state.fread_func(scratch, 1, readthisamountnow,
                                 তথ্য->state.in);

        passed += actuallyread;
        যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(তথ্য, "Failed to read data");
          ফেরত CURLE_FTP_COULDNT_USE_REST;
        }
      } যখন(passed < তথ্য->state.resume_from);
    }
    /* now, decrease the size of the read */
    যদি(তথ্য->state.infilesize > 0) {
      তথ্য->state.infilesize -= তথ্য->state.resume_from;

      যদি(তথ্য->state.infilesize <= 0) {
        infof(তথ্য, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(তথ্য);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(তথ্য, ftpc, FTP_STOP);
        ফেরত CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  যদি(!ফলাফল)
    ftp_state(তথ্য, ftpc, FTP_STOR);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_retr(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ftp_conn *ftpc,
                               কাঠামো FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_FTP(তথ্য, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  যদি(তথ্য->set.max_filesize && (filesize > তথ্য->set.max_filesize)) {
    failf(তথ্য, "Maximum file size exceeded");
    ফেরত CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  যদি(তথ্য->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    যদি(filesize == -1) {
      infof(তথ্য, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    অন্যথা {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      যদি(তথ্য->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        যদি(filesize < -তথ্য->state.resume_from) {
          failf(তথ্য, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                তথ্য->state.resume_from, filesize);
          ফেরত CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -তথ্য->state.resume_from;
        /* download from where? */
        তথ্য->state.resume_from = filesize - ftp->downloadsize;
      }
      অন্যথা {
        যদি(filesize < তথ্য->state.resume_from) {
          failf(তথ্য, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                তথ্য->state.resume_from, filesize);
          ফেরত CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - তথ্য->state.resume_from;
      }
    }

    যদি(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(তথ্য);
      infof(তথ্য, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(তথ্য, ftpc, FTP_STOP);
      ফেরত CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(তথ্য, "Instructs server to resume from offset %" FMT_OFF_T,
          তথ্য->state.resume_from);

    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "REST %" FMT_OFF_T,
                           তথ্য->state.resume_from);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_RETR_REST);
  }
  অন্যথা {
    /* no resume */
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "RETR %s", ftpc->file);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_RETR);
  }

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_quote(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ftp_conn *ftpc,
                                কাঠামো FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  bool quote = FALSE;
  কাঠামো curl_slist *item;

  নির্বাচন(instate) {
  ক্ষেত্র FTP_QUOTE:
  পূর্বনির্ধারিত:
    item = তথ্য->set.quote;
    বিরতি;
  ক্ষেত্র FTP_RETR_PREQUOTE:
  ক্ষেত্র FTP_STOR_PREQUOTE:
  ক্ষেত্র FTP_LIST_PREQUOTE:
    item = তথ্য->set.prequote;
    বিরতি;
  ক্ষেত্র FTP_POSTQUOTE:
    item = তথ্য->set.postquote;
    বিরতি;
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
    পূর্ণসংখ্যা i = 0;

    /* Skip count1 items in the linked list */
    যখন((i < ftpc->count1) && item) {
      item = item->পরবর্তী;
      i++;
    }
    যদি(item) {
      ধ্রুবক অক্ষর *cmd = item->তথ্য;
      যদি(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      অন্যথা
        ftpc->count2 = 0; /* failure means cancel operation */

      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", cmd);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      ftp_state(তথ্য, ftpc, instate);
      quote = TRUE;
    }
  }

  যদি(!quote) {
    /* No more quote to send, continue to ... */
    নির্বাচন(instate) {
    ক্ষেত্র FTP_QUOTE:
    পূর্বনির্ধারিত:
      ফলাফল = ftp_state_cwd(তথ্য, ftpc, ftp);
      বিরতি;
    ক্ষেত্র FTP_RETR_PREQUOTE:
      যদি(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(তথ্য, ftpc, FTP_STOP);
      অন্যথা {
        যদি(ftpc->known_filesize != -1) {
          Curl_pgrs_Set_Download_আকার(তথ্য, ftpc->known_filesize);
          ফলাফল = ftp_state_retr(তথ্য, ftpc, ftp, ftpc->known_filesize);
        }
        অন্যথা {
          যদি(তথ্য->set.ignorecl || তথ্য->state.prefer_ascii) {
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
            ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "RETR %s", ftpc->file);
            যদি(!ফলাফল)
              ftp_state(তথ্য, ftpc, FTP_RETR);
          }
          অন্যথা {
            ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "SIZE %s", ftpc->file);
            যদি(!ফলাফল)
              ftp_state(তথ্য, ftpc, FTP_RETR_আকার);
          }
        }
      }
      বিরতি;
    ক্ষেত্র FTP_STOR_PREQUOTE:
      ফলাফল = ftp_state_ul_setup(তথ্য, ftpc, ftp, FALSE);
      বিরতি;
    ক্ষেত্র FTP_POSTQUOTE:
      বিরতি;
    ক্ষেত্র FTP_LIST_PREQUOTE:
      ftp_state(তথ্য, ftpc, FTP_LIST_TYPE);
      ফলাফল = ftp_state_list(তথ্য, ftpc, ftp);
      বিরতি;
    }
  }

  ফেরত ফলাফল;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
স্থির CURLcode ftp_epsv_disable(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ftp_conn *ftpc,
                                 কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(তথ্য, "Failed EPSV attempt, exiting");
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  infof(তথ্য, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(তথ্য, ftpc);
  তথ্য->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "PASV");
  যদি(!ফলাফল) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(তথ্য, ftpc, FTP_PASV);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_control_addr_dup(কাঠামো Curl_easy *তথ্য, অক্ষর **newhostp)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ip_quadruple ipquad;
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
  যদি(!Curl_conn_get_ip_info(তথ্য, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  অন্যথা {
    /* failed to get the remote_ip of the DATA connection */
    failf(তথ্য, "unable to get peername of DATA connection");
    *newhostp = ফাঁকা;
    ফেরত CURLE_FTP_CANT_GET_HOST;
  }
  ফেরত *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির bool match_pasv_6nums(ধ্রুবক অক্ষর *p,
                             unsigned পূর্ণসংখ্যা *array) /* 6 numbers */
{
  পূর্ণসংখ্যা i;
  জন্য(i = 0; i < 6; i++) {
    curl_off_t অঙ্ক;
    যদি(i) {
      যদি(*p != ',')
        ফেরত FALSE;
      p++;
    }
    যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, 0xff))
      ফেরত FALSE;
    array[i] = (unsigned পূর্ণসংখ্যা)অঙ্ক;
  }
  ফেরত TRUE;
}

স্থির CURLcode ftp_state_pasv_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    পূর্ণসংখ্যা ftpcode)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল;
  ধ্রুবক কাঠামো pingpong *pp = &ftpc->pp;
  অক্ষর *newhost = ফাঁকা;
  unsigned short newport = 0;
  ধ্রুবক অক্ষর *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  যদি((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ধ্রুবক অক্ষর *ptr = strchr(str, '(');
    যদি(ptr) {
      অক্ষর sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      যদি(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ধ্রুবক অক্ষর *p = &ptr[3];
        curl_off_t অঙ্ক;
        যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, 0xffff) || (*p != sep)) {
          failf(তথ্য, "Illegal port number in EPSV reply");
          ফেরত CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)অঙ্ক;
        ফলাফল = ftp_control_addr_dup(তথ্য, &newhost);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
      অন্যথা
        ptr = ফাঁকা;
    }
    যদি(!ptr) {
      failf(তথ্য, "Weirdly formatted EPSV reply");
      ফেরত CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  অন্যথা যদি((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned পূর্ণসংখ্যা ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    যখন(*str) {
      যদি(match_pasv_6nums(str, ip))
        বিরতি;
      str++;
    }

    যদি(!*str) {
      failf(তথ্য, "Could not interpret the 227-response");
      ফেরত CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    যদি(তথ্য->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(তথ্য, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      ফলাফল = ftp_control_addr_dup(তথ্য, &newhost);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    যদি(!newhost)
      ফেরত CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  অন্যথা যদি(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ফেরত ftp_epsv_disable(তথ্য, ftpc, conn);
  }
  অন্যথা {
    failf(তথ্য, "Bad PASV/EPSV response: %03d", ftpcode);
    ফেরত CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  ফলাফল = Curl_peer_create(তথ্য, conn->scheme, newhost, newport,
                            &conn->origin2);
  যদি(ফলাফল)
    যাও error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  যদি(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    ফলাফল = Curl_peer_create(তথ্য, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    যদি(ফলাফল)
      যাও error;
  }

  ফলাফল = Curl_conn_setup(তথ্য, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_তথ্য_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  যদি(ফলাফল) {
    যদি((ফলাফল != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      ফলাফল = ftp_epsv_disable(তথ্য, ftpc, conn);
    }
    যাও error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  যদি(তথ্য->set.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(তথ্য, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->port);
  }
#endif

  conn->bits.do_more = TRUE;
  ftp_state(তথ্য, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  ফেরত ফলাফল;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode ftp_statemach(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ftp_conn *ftpc,
                              bool *done)
{
  CURLcode ফলাফল = Curl_pp_statemach(তথ্য, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  ফেরত ফলাফল;
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
স্থির CURLcode ftp_do_more(কাঠামো Curl_easy *তথ্য, domore *more)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  কাঠামো FTP *ftp = Curl_meta_get(তথ্য, CURL_META_FTP_EASY);
  CURLcode ফলাফল = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  যদি(!ftpc || !ftp)
    ফেরত CURLE_FAILED_INIT;

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
    bool is_eptr = Curl_conn_is_tcp_listen(তথ্য, SECONDARYSOCKET);
    ফলাফল = Curl_conn_connect(তথ্য, SECONDARYSOCKET, FALSE, &connected);
    যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
      ফেরত ফলাফল;
    যদি(ফলাফল || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(তথ্য, SECONDARYSOCKET))) {
      যদি(ফলাফল && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ফেরত ftp_epsv_disable(তথ্য, ftpc, conn);
      }
      ফেরত ফলাফল;
    }
  }

  যদি(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    ফলাফল = ftp_statemach(তথ্য, ftpc, &complete);

    যদি(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    যদি(ফলাফল || !ftpc->wait_তথ্য_conn)
      ফেরত ফলাফল;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  যদি(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    যদি(ftpc->wait_তথ্য_conn) {
      bool serv_conned;

      ফলাফল = Curl_conn_connect(তথ্য, SECONDARYSOCKET, FALSE, &serv_conned);
      যদি(ফলাফল)
        ফেরত ফলাফল; /* Failed to accept data connection */

      যদি(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_তথ্য_conn = FALSE;
        ফলাফল = ftp_initiate_transfer(তথ্য, ftpc);

        যদি(ফলাফল)
          ফেরত ফলাফল;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      অন্যথা {
        ফলাফল = ftp_check_ctrl_on_তথ্য_wait(তথ্য, ftpc);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
    অন্যথা যদি(তথ্য->state.upload) {
      ফলাফল = ftp_nb_type(তথ্য, ftpc, ftp, (bool)তথ্য->state.prefer_ascii,
                           FTP_STOR_TYPE);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      ফলাফল = ftp_statemach(তথ্য, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_তথ্য_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    অন্যথা {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      ফলাফল = Curl_range(তথ্য);

      যদি(ফলাফল == CURLE_OK && তথ্য->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      যদি(ফলাফল)
        ফেরত ফলাফল;

      যদি((তথ্য->state.list_only || !ftpc->file) &&
         !তথ্য->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        যদি(ftp->transfer == PPTRANSFER_BODY) {
          ফলাফল = ftp_nb_type(তথ্য, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          যদি(ফলাফল)
            ফেরত ফলাফল;
        }
        /* otherwise fall through */
      }
      অন্যথা {
        যদি(তথ্য->set.prequote && !ftpc->file) {
          ফলাফল = ftp_nb_type(তথ্য, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        অন্যথা {
          ফলাফল = ftp_nb_type(তথ্য, ftpc, ftp, (bool)তথ্য->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }

      ফলাফল = ftp_statemach(তথ্য, ftpc, &complete);
      যদি(complete)
        *more = DOMORE_DONE;
    }
    ফেরত ফলাফল;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(তথ্য);

  যদি(!ftpc->wait_তথ্য_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(তথ্য, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (পূর্ণসংখ্যা)ফলাফল);
  }

  ফেরত ফলাফল;
}

/* call this when the DO phase has completed */
স্থির CURLcode ftp_dophase_done(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ftp_conn *ftpc,
                                 কাঠামো FTP *ftp,
                                 bool connected)
{
  যদি(connected) {
    domore completed;
    CURLcode ফলাফল = ftp_do_more(তথ্য, &completed);

    যদি(ফলাফল) {
      close_secondarysocket(তথ্য, ftpc);
      ফেরত ফলাফল;
    }
  }

  যদি(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(তথ্য);
  অন্যথা যদি(!connected)
    /* since we did not connect now, we want do_more to get called */
    তথ্য->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ফেরত CURLE_OK;
}

স্থির CURLcode ftp_state_port_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    কাঠামো FTP *ftp,
                                    পূর্ণসংখ্যা ftpcode)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode ফলাফল = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  যদি(ftpcode / 100 != 2) {
    /* the command failed */

    যদি(EPRT == fcmd) {
      infof(তথ্য, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    যদি(fcmd == DONE) {
      failf(তথ্য, "Failed to do PORT");
      ফলাফল = CURLE_FTP_PORT_FAILED;
    }
    অন্যথা
      /* try next */
      ফলাফল = ftp_state_use_port(তথ্য, ftpc, fcmd);
  }
  অন্যথা {
    infof(তথ্য, "Connect data stream actively");
    ftp_state(তথ্য, ftpc, FTP_STOP); /* end of DO phase */
    ফলাফল = ftp_dophase_done(তথ্য, ftpc, ftp, FALSE);
  }

  ফেরত ফলাফল;
}

/* return TRUE on error, FALSE on success */
স্থির bool twodigit(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা *val)
{
  যদি(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ফেরত TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ফেরত FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা *year, পূর্ণসংখ্যা *month, পূর্ণসংখ্যা *day,
                           পূর্ণসংখ্যা *hour, পূর্ণসংখ্যা *minute, পূর্ণসংখ্যা *second);
UNITTEST bool ftp_213_date(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা *year, পূর্ণসংখ্যা *month, পূর্ণসংখ্যা *day,
                           পূর্ণসংখ্যা *hour, পূর্ণসংখ্যা *minute, পূর্ণসংখ্যা *second)
{
  পূর্ণসংখ্যা century;
  যদি((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    ফেরত FALSE;

  *year += century * 100;
  যদি((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    ফেরত FALSE;
  ফেরত TRUE;
}

স্থির CURLcode client_write_header(কাঠামো Curl_easy *তথ্য,
                                    অক্ষর *buf, size_t blen)
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
  CURLcode ফলাফল;
  bool save = (bool)তথ্য->set.include_header;
  তথ্য->set.include_header = TRUE;
  ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, buf, blen);
  তথ্য->set.include_header = save;
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_mdtm_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    কাঠামো FTP *ftp,
                                    পূর্ণসংখ্যা ftpcode)
{
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(ftpcode) {
  ক্ষেত্র 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    পূর্ণসংখ্যা year, month, day, hour, minute, second;
    কাঠামো pingpong *pp = &ftpc->pp;
    ধ্রুবক অক্ষর *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    যদি(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      অক্ষর timebuf[24];
      curl_msnprintf(timebuf, আকার(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      যদি(!Curl_getdate_capped(timebuf, &তথ্য->info.filetime))
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
    যদি(তথ্য->req.no_body && ftpc->file &&
       তথ্য->set.get_filetime && showtime) {
#যদি defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic বের_করো
#endif
      অক্ষর headerbuf[128];
      পূর্ণসংখ্যা headerbuflen;
      time_t filetime = তথ্য->info.filetime;
      কাঠামো tm বাফার;
      ধ্রুবক কাঠামো tm *tm = &বাফার;

      ফলাফল = curlx_gmtime(filetime, &বাফার);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, আকার(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                       tm->tm_mday,
                       Curl_month[tm->tm_mon],
                       tm->tm_year + 1900,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec);
      ফলাফল = client_write_header(তথ্য, headerbuf, headerbuflen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    } /* end of a ridiculous amount of conditionals */
  }
    বিরতি;
  পূর্বনির্ধারিত:
    infof(তথ্য, "unsupported MDTM reply format");
    বিরতি;
  ক্ষেত্র 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(তথ্য, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    বিরতি;
  }

  যদি(তথ্য->set.timecondition) {
    যদি((তথ্য->info.filetime > 0) && (তথ্য->set.timevalue > 0)) {
      নির্বাচন(তথ্য->set.timecondition) {
      ক্ষেত্র CURL_TIMECOND_IFMODSINCE:
      পূর্বনির্ধারিত:
        যদি(তথ্য->info.filetime <= তথ্য->set.timevalue) {
          infof(তথ্য, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          তথ্য->info.timecond = TRUE;
          ftp_state(তথ্য, ftpc, FTP_STOP);
          ফেরত CURLE_OK;
        }
        বিরতি;
      ক্ষেত্র CURL_TIMECOND_IFUNMODSINCE:
        যদি(তথ্য->info.filetime > তথ্য->set.timevalue) {
          infof(তথ্য, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          তথ্য->info.timecond = TRUE;
          ftp_state(তথ্য, ftpc, FTP_STOP);
          ফেরত CURLE_OK;
        }
        বিরতি;
      } /* switch */
    }
    অন্যথা {
      infof(তথ্য, "Skipping time comparison");
    }
  }

  যদি(!ফলাফল)
    ফলাফল = ftp_state_type(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_type_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    কাঠামো FTP *ftp,
                                    পূর্ণসংখ্যা ftpcode,
                                    ftpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(তথ্য, "Could not set desired mode");
    ফেরত CURLE_FTP_COULDNT_SET_TYPE;
  }
  যদি(ftpcode != 200)
    infof(তথ্য, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  যদি(instate == FTP_TYPE)
    ফলাফল = ftp_state_আকার(তথ্য, ftpc, ftp);
  অন্যথা যদি(instate == FTP_LIST_TYPE)
    ফলাফল = ftp_state_list(তথ্য, ftpc, ftp);
  অন্যথা যদি(instate == FTP_RETR_TYPE)
    ফলাফল = ftp_state_retr_prequote(তথ্য, ftpc, ftp);
  অন্যথা যদি(instate == FTP_STOR_TYPE)
    ফলাফল = ftp_state_stor_prequote(তথ্য, ftpc, ftp);
  অন্যথা যদি(instate == FTP_RETR_LIST_TYPE)
    ফলাফল = ftp_state_list_prequote(তথ্য, ftpc, ftp);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_আকার_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    কাঠামো FTP *ftp,
                                    পূর্ণসংখ্যা ftpcode,
                                    ftpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_off_t filesize = -1;
  ধ্রুবক অক্ষর *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  যদি(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ধ্রুবক অক্ষর *start = &buf[4];
    ধ্রুবক অক্ষর *fdigit = memchr(start, '\r', len - 4);
    যদি(fdigit) {
      fdigit--;
      যদি(*fdigit == '\n')
        fdigit--;
      যখন(ISDIGIT(fdigit[-1]) && (fdigit > start))
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
          instate != FTP_STOR_আকার) {
    failf(তথ্য, "The file does not exist");
    ফেরত CURLE_REMOTE_FILE_NOT_FOUND;
  }

  যদি(instate == FTP_আকার) {
    যদি(filesize != -1) {
      অক্ষর clbuf[128];
      পূর্ণসংখ্যা clbuflen = curl_msnprintf(clbuf, আকার(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      ফলাফল = client_write_header(তথ্য, clbuf, clbuflen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    Curl_pgrs_Set_Download_আকার(তথ্য, filesize);
    ফলাফল = ftp_state_rest(তথ্য, ftpc, ftp);
  }
  অন্যথা যদি(instate == FTP_RETR_আকার) {
    Curl_pgrs_Set_Download_আকার(তথ্য, filesize);
    ফলাফল = ftp_state_retr(তথ্য, ftpc, ftp, filesize);
  }
  অন্যথা যদি(instate == FTP_STOR_আকার) {
    তথ্য->state.resume_from = filesize;
    ফলাফল = ftp_state_ul_setup(তথ্য, ftpc, ftp, TRUE);
  }

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_rest_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    কাঠামো FTP *ftp,
                                    পূর্ণসংখ্যা ftpcode,
                                    ftpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(instate) {
  ক্ষেত্র FTP_REST:
  পূর্বনির্ধারিত:
    যদি(ftpcode == 350) {
      অক্ষর বাফার[24] = { "Accept-ranges: bytes\r\n" };
      ফলাফল = client_write_header(তথ্য, বাফার, strlen(বাফার));
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    ফলাফল = ftp_state_prepare_transfer(তথ্য, ftpc, ftp);
    বিরতি;

  ক্ষেত্র FTP_RETR_REST:
    যদি(ftpcode != 350) {
      failf(তথ্য, "Could not use REST");
      ফলাফল = CURLE_FTP_COULDNT_USE_REST;
    }
    অন্যথা {
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "RETR %s", ftpc->file);
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_RETR);
    }
    বিরতি;
  }

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_state_stor_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    পূর্ণসংখ্যা ftpcode)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(ftpcode >= 400) {
    failf(তথ্য, "Failed FTP upload: %0d", ftpcode);
    ftp_state(তথ্য, ftpc, FTP_STOP);
    ফেরত CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  যদি(তথ্য->set.ftp_use_port) {
    bool connected;

    ftp_state(তথ্য, ftpc, FTP_STOP); /* no longer in STOR state */

    ফলাফল = Curl_conn_connect(তথ্য, SECONDARYSOCKET, FALSE, &connected);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    যদি(!connected) {
      infof(তথ্য, "Data conn was not available immediately");
      ftpc->wait_তথ্য_conn = TRUE;
      ফেরত ftp_check_ctrl_on_তথ্য_wait(তথ্য, ftpc);
    }
    ftpc->wait_তথ্য_conn = FALSE;
  }
  ফেরত ftp_initiate_transfer(তথ্য, ftpc);
}

/* for LIST and RETR responses */
স্থির CURLcode ftp_state_get_resp(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc,
                                   কাঠামো FTP *ftp,
                                   পূর্ণসংখ্যা ftpcode,
                                   ftpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

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

    তথ্য->req.আকার_2 = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    যদি((instate != FTP_LIST) &&
       !তথ্য->state.prefer_ascii &&
       !তথ্য->set.ignorecl &&
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
        জন্য(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ধ্রুবক অক্ষর *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ধ্রুবক অক্ষর *c = &buf[i];
          যদি(!curlx_str_সংখ্যা(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            তথ্য->req.আকার_2 = what;
            বিরতি;
          }
        }
      }
    }
    অন্যথা যদি(ftp->downloadsize > -1)
      তথ্য->req.আকার_2 = ftp->downloadsize;

    যদি(তথ্য->req.আকার_2 > তথ্য->req.maxdownload && তথ্য->req.maxdownload > 0)
      তথ্য->req.আকার_2 = তথ্য->req.maxdownload;
    অন্যথা যদি((instate != FTP_LIST) && তথ্য->state.prefer_ascii)
      তথ্য->req.আকার_2 = -1; /* for servers that understate ASCII mode file
আকার*/

    infof(তথ্য, "Maxdownload = %" FMT_OFF_T, তথ্য->req.maxdownload);

    যদি(instate != FTP_LIST)
      infof(তথ্য, "Getting file with size: %" FMT_OFF_T, তথ্য->req.আকার_2);

    যদি(তথ্য->set.ftp_use_port) {
      bool connected;

      ফলাফল = Curl_conn_connect(তথ্য, SECONDARYSOCKET, FALSE, &connected);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      যদি(!connected) {
        infof(তথ্য, "Data conn was not available immediately");
        ftp_state(তথ্য, ftpc, FTP_STOP);
        ftpc->wait_তথ্য_conn = TRUE;
        ফেরত ftp_check_ctrl_on_তথ্য_wait(তথ্য, ftpc);
      }
      ftpc->wait_তথ্য_conn = FALSE;
    }
    ফেরত ftp_initiate_transfer(তথ্য, ftpc);
  }
  অন্যথা {
    যদি((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(তথ্য, ftpc, FTP_STOP); /* this phase is over */
    }
    অন্যথা {
      failf(তথ্য, "RETR response: %03d", ftpcode);
      ফেরত instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ফেরত ফলাফল;
}

/* after USER, PASS and ACCT */
স্থির CURLcode ftp_state_loggedin(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_conn *ftpc)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET)) {
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
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "PBSZ %d", 0);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_PBSZ);
  }
  অন্যথা {
    ফলাফল = ftp_state_pwd(তথ্য, ftpc);
  }
  ফেরত ফলাফল;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
স্থির bool ftp_has_ctrl(ধ্রুবক অক্ষর *string)
{
  ধ্রুবক unsigned অক্ষর *s = (ধ্রুবক unsigned অক্ষর *)string;
  যখন(*s) {
    যদি(*s < 0x20)
      ফেরত TRUE;
    s++;
  }
  ফেরত FALSE;
}

/* for USER and PASS responses */
স্থির CURLcode ftp_state_user_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    পূর্ণসংখ্যা ftpcode)
{
  CURLcode ফলাফল = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  যদি((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(তথ্য->conn->creds));
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_PASS);
  }
  অন্যথা যদি(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    ফলাফল = ftp_state_loggedin(তথ্য, ftpc);
  }
  অন্যথা যদি(ftpcode == 332) {
    ধ্রুবক অক্ষর *account = CURL_EASY_STR(তথ্য, STRING_FTP_ACCOUNT);
    যদি(!account) {
      failf(তথ্য, "ACCT requested but none available");
      ফলাফল = CURLE_LOGIN_DENIED;
    }
    অন্যথা যদি(ftp_has_ctrl(account)) {
      failf(তথ্য, "Control byte in FTP account");
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    অন্যথা {
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "ACCT %s", account);
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_ACCT);
    }
  }
  অন্যথা {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ধ্রুবক অক্ষর *alt = CURL_EASY_STR(তথ্য, STRING_FTP_ALTERNATIVE_TO_USER);
    যদি(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      যদি(ftp_has_ctrl(alt)) {
        failf(তথ্য, "Control byte in FTP alternative-to-user command");
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      অন্যথা {
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", alt);
        যদি(!ফলাফল) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(তথ্য, ftpc, FTP_USER);
        }
      }
    }
    অন্যথা {
      failf(তথ্য, "Access denied: %03d", ftpcode);
      ফলাফল = CURLE_LOGIN_DENIED;
    }
  }
  ফেরত ফলাফল;
}

/* for ACCT response */
স্থির CURLcode ftp_state_acct_resp(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc,
                                    পূর্ণসংখ্যা ftpcode)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(ftpcode != 230) {
    failf(তথ্য, "ACCT rejected by server: %03d", ftpcode);
    ফলাফল = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  অন্যথা
    ফলাফল = ftp_state_loggedin(তথ্য, ftpc);

  ফেরত ফলাফল;
}

স্থির CURLcode ftp_pwd_resp(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ftp_conn *ftpc,
                             পূর্ণসংখ্যা ftpcode)
{
  কাঠামো pingpong *pp = &ftpc->pp;
  CURLcode ফলাফল;

  যদি(ftpcode == 257) {
    ধ্রুবক অক্ষর *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    কাঠামো dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    যখন(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    যদি('\"' == *ptr) {
      /* it started good */
      জন্য(ptr++; *ptr; ptr++) {
        যদি('\"' == *ptr) {
          যদি('\"' == ptr[1]) {
            /* "quote-doubling" */
            ফলাফল = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          অন্যথা {
            /* end of path */
            যদি(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            বিরতি; /* get out of this loop */
          }
        }
        অন্যথা {
          যদি(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            ফেরত CURLE_WEIRD_SERVER_REPLY;
          }
          ফলাফল = curlx_dyn_addn(&out, ptr, 1);
        }
        যদি(ফলাফল) {
          curlx_dyn_free(&out);
          ফেরত ফলাফল;
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
      অক্ষর *dir = curlx_dyn_ptr(&out);

      যদি(!ftpc->server_os && dir[0] != '/') {
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "SYST");
        যদি(ফলাফল) {
          curlx_dyn_free(&out);
          ফেরত ফলাফল;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(তথ্য, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(তথ্য->state.most_recent_ftp_entrypath);
      তথ্য->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      যদি(!তথ্য->state.most_recent_ftp_entrypath)
        ফেরত CURLE_OUT_OF_MEMORY;

      যদি(!ftpc->server_os && dir[0] != '/') {
        ftp_state(তথ্য, ftpc, FTP_SYST);
        ফেরত CURLE_OK;
      }
    }
    অন্যথা {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(তথ্য, "Failed to figure out path");
    }
  }
  ftp_state(তথ্য, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(তথ্য, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ফেরত CURLE_OK;
}

স্থির ধ্রুবক অক্ষর * ধ্রুবক ftpauth[] = { "SSL", "TLS" };

স্থির CURLcode ftp_wait_resp(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn,
                              কাঠামো ftp_conn *ftpc,
                              পূর্ণসংখ্যা ftpcode)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    যদি(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ফেরত ftp_state_user_resp(তথ্য, ftpc, ftpcode);
  }
  অন্যথা যদি(ftpcode != 220) {
    failf(তথ্য, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  যদি(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    নির্বাচন((long)তথ্য->set.ftpsslauth) {
    ক্ষেত্র CURLFTPAUTH_DEFAULT:
    ক্ষেত্র CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      বিরতি;
    ক্ষেত্র CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      বিরতি;
    পূর্বনির্ধারিত:
      failf(তথ্য, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (পূর্ণসংখ্যা)তথ্য->set.ftpsslauth);
      ফেরত CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_AUTH);
  }
  অন্যথা
    ফলাফল = ftp_state_user(তথ্য, ftpc, conn);
  ফেরত ফলাফল;
}

/* an implicit TLS scheme implies TLS on both connections */
স্থির unsigned অক্ষর ftp_use_ssl(কাঠামো connectdata *conn,
                                 কাঠামো ftp_conn *ftpc)
{
  যদি(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ফেরত CURLUSESSL_ALL;
  ফেরত ftpc->use_ssl;
}

স্থির CURLcode ftp_pp_statemachine(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা ftpcode;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  কাঠামো FTP *ftp = Curl_meta_get(তথ্য, CURL_META_FTP_EASY);
  কাঠামো pingpong *pp;
  size_t nread = 0;

  যদি(!ftpc || !ftp)
    ফেরত CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  যদি(pp->sendleft)
    ফেরত Curl_pp_flushsend(তথ্য, pp);

  ফলাফল = ftp_readresp(তথ্য, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  যদি(ফলাফল || !ftpcode)
    ফেরত ফলাফল;

  /* we have now received a full FTP server response */
  নির্বাচন(ftpc->state) {
  ক্ষেত্র FTP_WAIT220:
    ফলাফল = ftp_wait_resp(তথ্য, conn, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    যদি(pp->overflow)
      ফেরত CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

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
        ফলাফল = Curl_ssl_cfilter_add(
          তথ্য, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        যদি(ফলাফল) {
          /* we failed and bail out */
          ফেরত CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, TRUE, &done);
      যদি(!ফলাফল) {
        conn->bits.ftp_use_তথ্য_ssl = FALSE; /* clear-text data */
        ফলাফল = ftp_state_user(তথ্য, ftpc, conn);
      }
    }
    অন্যথা যদি(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    অন্যথা {
      যদি(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        ফলাফল = CURLE_USE_SSL_FAILED;
      অন্যথা
        /* ignore the failure and continue */
        ফলাফল = ftp_state_user(তথ্য, ftpc, conn);
    }
    বিরতি;

  ক্ষেত্র FTP_USER:
  ক্ষেত্র FTP_PASS:
    ফলাফল = ftp_state_user_resp(তথ্য, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_ACCT:
    ফলাফল = ftp_state_acct_resp(তথ্য, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_PBSZ:
    ফলাফল =
      Curl_pp_sendf(তথ্য, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    যদি(!ফলাফল)
      ftp_state(তথ্য, ftpc, FTP_PROT);
    বিরতি;

  ক্ষেত্র FTP_PROT:
    যদি(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_তথ্য_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    অন্যথা যদি(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ফেরত CURLE_USE_SSL_FAILED;

    যদি(তথ্য->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "CCC");
      যদি(!ফলাফল)
        ftp_state(তথ্য, ftpc, FTP_CCC);
    }
    অন্যথা
      ফলাফল = ftp_state_pwd(তথ্য, ftpc);
    বিরতি;

  ক্ষেত্র FTP_CCC:
    যদি(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      ফলাফল = Curl_ssl_cfilter_remove(তথ্য, FIRSTSOCKET,
                                       (তথ্য->set.ftp_ccc ==
                                        (unsigned অক্ষর)CURLFTPSSL_CCC_ACTIVE));
      যদি(ফলাফল)
        failf(তথ্য, "Failed to clear the command channel (CCC)");
    }
    যদি(!ফলাফল)
      /* Then continue as normal */
      ফলাফল = ftp_state_pwd(তথ্য, ftpc);
    বিরতি;

  ক্ষেত্র FTP_PWD:
    ফলাফল = ftp_pwd_resp(তথ্য, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_SYST:
    যদি(ftpcode == 215) {
      ধ্রুবক অক্ষর *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ধ্রুবক অক্ষর *start;
      অক্ষর *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      যখন(*ptr == ' ')
        ptr++;
      জন্য(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      যদি(!os)
        ফেরত CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      যদি(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        যদি(ফলাফল) {
          curlx_free(os);
          ফেরত ফলাফল;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(তথ্য, ftpc, FTP_NAMEFMT);
        বিরতি;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    অন্যথা {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(তথ্য, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(তথ্য, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    বিরতি;

  ক্ষেত্র FTP_NAMEFMT:
    যদি(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(তথ্য, ftpc);
      বিরতি;
    }

    ftp_state(তথ্য, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(তথ্য, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    বিরতি;

  ক্ষেত্র FTP_QUOTE:
  ক্ষেত্র FTP_POSTQUOTE:
  ক্ষেত্র FTP_RETR_PREQUOTE:
  ক্ষেত্র FTP_STOR_PREQUOTE:
  ক্ষেত্র FTP_LIST_PREQUOTE:
    যদি((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(তথ্য, "QUOT command failed with %03d", ftpcode);
      ফলাফল = CURLE_QUOTE_ERROR;
    }
    অন্যথা
      ফলাফল = ftp_state_quote(তথ্য, ftpc, ftp, FALSE, ftpc->state);
    বিরতি;

  ক্ষেত্র FTP_CWD:
    যদি(ftpcode / 100 != 2) {
      /* failure to CWD there */
      যদি(তথ্য->set.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (তথ্য->set.ftp_create_missing_dirs == 2) ? 1 : 0;

        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        যদি(!ফলাফল)
          ftp_state(তথ্য, ftpc, FTP_MKD);
      }
      অন্যথা {
        /* return failure */
        failf(তথ্য, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    অন্যথা {
      /* success */
      ftpc->count2 = 0;
      যদি(ftpc->cwdcount >= ftpc->dirdepth)
        ফলাফল = ftp_state_mdtm(তথ্য, ftpc, ftp);
      অন্যথা {
        ftpc->cwdcount++;
        /* send next CWD */
        ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    বিরতি;

  ক্ষেত্র FTP_MKD:
    যদি((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(তথ্য, "Failed to MKD dir: %03d", ftpcode);
      ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
    }
    অন্যথা {
      ftp_state(তথ্য, ftpc, FTP_CWD);
      /* send CWD */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    বিরতি;

  ক্ষেত্র FTP_MDTM:
    ফলাফল = ftp_state_mdtm_resp(তথ্য, ftpc, ftp, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_TYPE:
  ক্ষেত্র FTP_LIST_TYPE:
  ক্ষেত্র FTP_RETR_TYPE:
  ক্ষেত্র FTP_STOR_TYPE:
  ক্ষেত্র FTP_RETR_LIST_TYPE:
    ফলাফল = ftp_state_type_resp(তথ্য, ftpc, ftp, ftpcode, ftpc->state);
    বিরতি;

  ক্ষেত্র FTP_আকার:
  ক্ষেত্র FTP_RETR_আকার:
  ক্ষেত্র FTP_STOR_আকার:
    ফলাফল = ftp_state_আকার_resp(তথ্য, ftpc, ftp, ftpcode, ftpc->state);
    বিরতি;

  ক্ষেত্র FTP_REST:
  ক্ষেত্র FTP_RETR_REST:
    ফলাফল = ftp_state_rest_resp(তথ্য, ftpc, ftp, ftpcode, ftpc->state);
    বিরতি;

  ক্ষেত্র FTP_PRET:
    যদি(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(তথ্য, "PRET command not accepted: %03d", ftpcode);
      ফেরত CURLE_FTP_PRET_FAILED;
    }
    ফলাফল = ftp_state_use_pasv(তথ্য, ftpc, conn);
    বিরতি;

  ক্ষেত্র FTP_PASV:
    ফলাফল = ftp_state_pasv_resp(তথ্য, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_PORT:
    ফলাফল = ftp_state_port_resp(তথ্য, ftpc, ftp, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_LIST:
  ক্ষেত্র FTP_RETR:
    ফলাফল = ftp_state_get_resp(তথ্য, ftpc, ftp, ftpcode, ftpc->state);
    বিরতি;

  ক্ষেত্র FTP_STOR:
    ফলাফল = ftp_state_stor_resp(তথ্য, ftpc, ftpcode);
    বিরতি;

  ক্ষেত্র FTP_QUIT:
  পূর্বনির্ধারিত:
    /* internal error */
    ftp_state(তথ্য, ftpc, FTP_STOP);
    বিরতি;
  }

  ফেরত ফলাফল;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode ftp_multi_statemach(কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  ফেরত ftpc ? ftp_statemach(তথ্য, ftpc, done) : CURLE_FAILED_INIT;
}

স্থির CURLcode ftp_block_statemach(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_conn *ftpc)
{
  কাঠামো pingpong *pp = &ftpc->pp;
  CURLcode ফলাফল = CURLE_OK;

  যখন(ftpc->state != FTP_STOP) {
    যদি(ftpc->shutdown)
      CURL_TRC_FTP(তথ্য, "in shutdown, waiting for server response");
    ফলাফল = Curl_pp_statemach(তথ্য, pp, TRUE, TRUE /* disconnecting */);
    যদি(ফলাফল)
      বিরতি;
  }

  ফেরত ফলাফল;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
স্থির CURLcode ftp_connect(কাঠামো Curl_easy *তথ্য,
                            bool *done) /* see description above */
{
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  কাঠামো pingpong *pp;

  *done = FALSE; /* default to not done yet */
  যদি(!ftpc)
    ফেরত CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, TRUE, done);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  Curl_pp_init(pp, Curl_pgrs_now(তথ্য)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(তথ্য, ftpc, FTP_WAIT220);

  ফলাফল = ftp_statemach(তথ্য, ftpc, done);

  ফেরত ফলাফল;
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
স্থির CURLcode ftp_sendquote(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ftp_conn *ftpc,
                              কাঠামো curl_slist *quote)
{
  কাঠামো curl_slist *item;
  কাঠামো pingpong *pp = &ftpc->pp;

  item = quote;
  যখন(item) {
    যদি(item->তথ্য) {
      size_t nread;
      ধ্রুবক অক্ষর *cmd = item->তথ্য;
      bool acceptfail = FALSE;
      CURLcode ফলাফল;
      পূর্ণসংখ্যা ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      যদি(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", cmd);
      যদি(!ফলাফল) {
        pp->response = *Curl_pgrs_now(তথ্য); /* timeout relative now */
        ফলাফল = getftpresponse(তথ্য, &nread, &ftpcode);
      }
      যদি(ফলাফল)
        ফেরত ফলাফল;

      যদি(!acceptfail && (ftpcode >= 400)) {
        failf(তথ্য, "QUOT string not accepted: %s", cmd);
        ফেরত CURLE_QUOTE_ERROR;
      }
    }

    item = item->পরবর্তী;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode ftp_done_status(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  নির্বাচন(status) {
  ক্ষেত্র CURLE_BAD_DOWNLOAD_RESUME:
  ক্ষেত্র CURLE_FTP_WEIRD_PASV_REPLY:
  ক্ষেত্র CURLE_FTP_PORT_FAILED:
  ক্ষেত্র CURLE_FTP_ACCEPT_FAILED:
  ক্ষেত্র CURLE_FTP_ACCEPT_TIMEOUT:
  ক্ষেত্র CURLE_FTP_COULDNT_SET_TYPE:
  ক্ষেত্র CURLE_FTP_COULDNT_RETR_FILE:
  ক্ষেত্র CURLE_PARTIAL_FILE:
  ক্ষেত্র CURLE_UPLOAD_FAILED:
  ক্ষেত্র CURLE_REMOTE_ACCESS_DENIED:
  ক্ষেত্র CURLE_FILESIZE_EXCEEDED:
  ক্ষেত্র CURLE_REMOTE_FILE_NOT_FOUND:
  ক্ষেত্র CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  ক্ষেত্র CURLE_OK: /* does not affect the control connection's status */
    যদি(!premature)
      বিরতি;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  পূর্বনির্ধারিত:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(তথ্য, "FTP ended with bad error code");
    connclose(তথ্য->conn);
    ফেরত status;      /* use the already set error code */
  }
  ফেরত CURLE_OK;
}

স্থির শূন্য ftp_done_wildcard(কাঠামো Curl_easy *তথ্য, কাঠামো ftp_conn *ftpc)
{
  যদি(তথ্য->state.wildcardmatch) {
    যদি(তথ্য->set.chunk_end && ftpc->file) {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_chunk_end);
      তথ্য->set.chunk_end(তথ্য->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

স্থির শূন্য ftp_done_path(কাঠামো Curl_easy *তথ্য, কাঠামো ftp_conn *ftpc,
                          CURLcode ফলাফল)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(ফলাফল) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  অন্যথা { /* remember working directory for connection reuse */
    ধ্রুবক অক্ষর *rawPath = ftpc->rawpath;
    যদি(rawPath) {
      যদি((তথ্য->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      অন্যথা {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        যদি(!ftpc->cwdfail) {
          যদি(তথ্য->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          অন্যথা
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        অন্যথা
          ftpc->prevpath = ফাঁকা; /* no path */
      }
    }
    যদি(ftpc->prevpath)
      infof(তথ্য, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

স্থির CURLcode ftp_done_secondary_socket(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ftp_conn *ftpc,
                                          CURLcode ফলাফল)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    যদি(!ফলাফল && ftpc->dont_check && তথ্য->req.maxdownload > 0) {
      /* partial download completed */
      ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "ABOR");
      যদি(ফলাফল) {
        failf(তথ্য, "Failure sending ABOR command: %s",
              curl_easy_strerror(ফলাফল));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(তথ্য, ftpc);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_done_control_reply(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ftp_conn *ftpc,
                                       কাঠামো FTP *ftp, CURLcode ফলাফল,
                                       bool premature)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  size_t nread;
  পূর্ণসংখ্যা ftpcode;

  যদি(!ফলাফল && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(তথ্য); /* timeout relative now */
    ফলাফল = getftpresponse(তথ্য, &nread, &ftpcode);

    যদি(!nread && (ফলাফল == CURLE_OPERATION_TIMEDOUT)) {
      failf(তথ্য, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    যদি(ফলাফল)
      ফেরত ফলাফল;

    যদি(ftpc->dont_check && তথ্য->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(তথ্য, "partial download completed, closing connection");
      connclose(conn);
      ফেরত ফলাফল;
    }

    যদি(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      নির্বাচন(ftpcode) {
      ক্ষেত্র 226:
      ক্ষেত্র 250:
        বিরতি;
      ক্ষেত্র 552:
        failf(তথ্য, "Exceeded storage allocation");
        ফলাফল = CURLE_REMOTE_DISK_FULL;
        বিরতি;
      পূর্বনির্ধারিত:
        failf(তথ্য, "server did not report OK, got %d", ftpcode);
        ফলাফল = CURLE_PARTIAL_FILE;
        বিরতি;
      }
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ftp_done_check_partial(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ftp_conn *ftpc,
                                       কাঠামো FTP *ftp, CURLcode ফলাফল,
                                       bool premature)
{
  যদি(ফলাফল || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  অন্যথা যদি(তথ্য->state.upload) {
    যদি((ftp->transfer == PPTRANSFER_BODY) &&
       (তথ্য->state.infilesize != -1) && /* upload with known size */
       ((!তথ্য->set.crlf && !তথ্য->state.prefer_ascii && /* no conversion */
         (তথ্য->state.infilesize != তথ্য->req.writebytecount)) ||
        ((তথ্য->set.crlf || তথ্য->state.prefer_ascii) && /* maybe CRLF conv */
         (তথ্য->state.infilesize > তথ্য->req.writebytecount))
       )) {
      failf(তথ্য, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            তথ্য->req.writebytecount, তথ্য->state.infilesize);
      ফলাফল = CURLE_PARTIAL_FILE;
    }
  }
  অন্যথা {
    যদি((তথ্য->req.আকার_2 != -1) &&
       (তথ্য->req.আকার_2 != তথ্য->req.bytecount) &&
       (তথ্য->req.maxdownload != তথ্য->req.bytecount)) {
      failf(তথ্য, "Received only partial file: %" FMT_OFF_T " bytes",
            তথ্য->req.bytecount);
      ফলাফল = CURLE_PARTIAL_FILE;
    }
    অন্যথা যদি(!ftpc->dont_check &&
            !তথ্য->req.bytecount &&
            (তথ্য->req.আকার_2 > 0)) {
      failf(তথ্য, "No data was received");
      ফলাফল = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ফেরত ফলাফল;
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
স্থির CURLcode ftp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                         bool premature)
{
  কাঠামো FTP *ftp = Curl_meta_get(তথ্য, CURL_META_FTP_EASY);
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  CURLcode ফলাফল;

  যদি(!ftp || !ftpc)
    ফেরত CURLE_OK;

  ফলাফল = ftp_done_status(তথ্য, ftpc, status, premature);

  ftp_done_wildcard(তথ্য, ftpc);
  ftp_done_path(তথ্য, ftpc, ফলাফল);
  ফলাফল = ftp_done_secondary_socket(তথ্য, ftpc, ফলাফল);
  ফলাফল = ftp_done_control_reply(তথ্য, ftpc, ftp, ফলাফল, premature);
  ফলাফল = ftp_done_check_partial(তথ্য, ftpc, ftp, ফলাফল, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  যদি(!status && !ফলাফল && !premature && তথ্য->set.postquote)
    ফলাফল = ftp_sendquote(তথ্য, ftpc, তথ্য->set.postquote);
  CURL_TRC_FTP(তথ্য, "[%s] done, result=%d", FTP_CSTATE(ftpc), (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
স্থির CURLcode ftp_nb_type(কাঠামো Curl_easy *তথ্য,
                            কাঠামো ftp_conn *ftpc,
                            কাঠামো FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode ফলাফল;
  অক্ষর want = (অক্ষর)(ascii ? 'A' : 'I');

  যদি(ftpc->transfertype == want) {
    ftp_state(তথ্য, ftpc, newstate);
    ফেরত ftp_state_type_resp(তথ্য, ftpc, ftp, 200, newstate);
  }

  ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "TYPE %c", want);
  যদি(!ফলাফল) {
    ftp_state(তথ্য, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
স্থির CURLcode ftp_perform(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো ftp_conn *ftpc,
  কাঠামো FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_FTP(তথ্য, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  যদি(তথ্য->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  ফলাফল = ftp_state_quote(তথ্য, ftpc, ftp, TRUE, FTP_QUOTE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* run the state-machine */
  ফলাফল = ftp_statemach(তথ্য, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, SECONDARYSOCKET);

  যদি(*connected)
    infof(তথ্য, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  অন্যথা
    CURL_TRC_FTP(তথ্য, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  যদি(*dophase_done)
    CURL_TRC_FTP(তথ্য, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ফেরত ফলাফল;
}

স্থির শূন্য wc_তথ্য_dtor(শূন্য *ptr)
{
  কাঠামো ftp_wc *ftpwc = ptr;
  যদি(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_তথ্য_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

স্থির CURLcode init_wc_তথ্য(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ftp_conn *ftpc,
                             কাঠামো FTP *ftp)
{
  অক্ষর *last_slash;
  অক্ষর *path = ftp->path;
  কাঠামো Wildcard_তথ্য *wildcard = তথ্য->wildcard;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ftp_wc *ftpwc = ফাঁকা;

  last_slash = strrchr(ftp->path, '/');
  যদি(last_slash) {
    last_slash++;
    যদি(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ফেরত ftp_parse_url_path(তথ্য, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    যদি(!wildcard->pattern)
      ফেরত CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  অন্যথা { /* there is only 'wildcard pattern' or nothing */
    যদি(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      যদি(!wildcard->pattern)
        ফেরত CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    অন্যথা { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ফেরত ftp_parse_url_path(তথ্য, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, আকার(কাঠামো ftp_wc));
  যদি(!ftpwc) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_তথ্য_alloc();
  যদি(!ftpwc->parser) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_তথ্য_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  যদি(তথ্য->set.ftp_filemethod == FTPFILE_NOCWD)
    তথ্য->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  ফলাফল = ftp_parse_url_path(তথ্য, ftpc, ftp);
  যদি(ফলাফল) {
    যাও fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  যদি(!wildcard->path) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  infof(তথ্য, "Wildcard - Parsing started");
  ফেরত CURLE_OK;

fail:
  যদি(ftpwc) {
    Curl_ftp_parselist_তথ্য_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ফাঁকা;
  ফেরত ফলাফল;
}

স্থির CURLcode wc_statemach(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ftp_conn *ftpc,
                             কাঠামো FTP *ftp)
{
  কাঠামো Wildcard_তথ্য * ধ্রুবক wildcard = তথ্য->wildcard;
  CURLcode ফলাফল = CURLE_OK;

  জন্য(;;) {
    নির্বাচন(wildcard->state) {
    ক্ষেত্র CURLWC_INIT:
      ফলাফল = init_wc_তথ্য(তথ্য, ftpc, ftp);
      যদি(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ফেরত ফলাফল;
      wildcard->state = ফলাফল ? CURLWC_ERROR : CURLWC_MATCHING;
      ফেরত ফলাফল;

    ক্ষেত্র CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      কাঠামো ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      যদি(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        চালু;
      }
      যদি(Curl_llist_গুনতি(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ফেরত CURLE_REMOTE_FILE_NOT_FOUND;
      }
      চালু;
    }

    ক্ষেত্র CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      কাঠামো Curl_llist_গ্রন্থি *শীর্ষ = Curl_llist_শীর্ষ(&wildcard->filelist);
      কাঠামো curl_fileinfo *finfo = Curl_গ্রন্থি_elem(শীর্ষ);
      অক্ষর *tmp_path;
      অক্ষর *enc = curl_easy_escape(ফাঁকা, finfo->filename, 0);
      যদি(!enc)
        ফেরত CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      যদি(!tmp_path)
        ফেরত CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(তথ্য, "Wildcard - START of \"%s\"", finfo->filename);
      যদি(তথ্য->set.chunk_bgn) {
        long userresponse;
        কাঠামো Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, তথ্য, easy_chunk_bgn);
        userresponse = তথ্য->set.chunk_bgn(
          finfo, তথ্য->set.wildcardptr,
          (পূর্ণসংখ্যা)Curl_llist_গুনতি(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        নির্বাচন(userresponse) {
        ক্ষেত্র CURL_CHUNK_BGN_FUNC_SKIP:
          infof(তথ্য, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          চালু;
        ক্ষেত্র CURL_CHUNK_BGN_FUNC_FAIL:
          ফেরত CURLE_CHUNK_FAILED;
        }
      }

      যদি(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        চালু;
      }

      যদি(finfo->flags & CURLFINFOFLAG_KNOWN_আকার)
        ftpc->known_filesize = finfo->আকার_2;

      ফলাফল = ftp_parse_url_path(তথ্য, ftpc, ftp);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_গ্রন্থি_remove(Curl_llist_শীর্ষ(&wildcard->filelist));

      যদি(Curl_llist_গুনতি(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ফেরত CURLE_OK;
      }
      ফেরত ফলাফল;
    }

    ক্ষেত্র CURLWC_SKIP: {
      যদি(তথ্য->set.chunk_end) {
        কাঠামো Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, তথ্য, easy_chunk_end);
        তথ্য->set.chunk_end(তথ্য->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_গ্রন্থি_remove(Curl_llist_শীর্ষ(&wildcard->filelist));
      wildcard->state = (Curl_llist_গুনতি(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      চালু;
    }

    ক্ষেত্র CURLWC_CLEAN: {
      কাঠামো ftp_wc *ftpwc = wildcard->ftpwc;
      ফলাফল = CURLE_OK;
      যদি(ftpwc)
        ফলাফল = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = ফলাফল ? CURLWC_ERROR : CURLWC_DONE;
      ফেরত ফলাফল;
    }

    ক্ষেত্র CURLWC_DONE:
    ক্ষেত্র CURLWC_ERROR:
    ক্ষেত্র CURLWC_CLEAR:
      যদি(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ফাঁকা;
      }
      ফেরত ফলাফল;
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
স্থির CURLcode ftp_regular_transfer(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ftp_conn *ftpc,
                                     কাঠামো FTP *ftp,
                                     bool *dophase_done)
{
  CURLcode ফলাফল = CURLE_OK;
  bool connected = FALSE;
  তথ্য->req.আকার_2 = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(তথ্য);

  ftpc->ctl_valid = TRUE; /* starts good */

  ফলাফল = ftp_perform(তথ্য, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_done); /* all commands in the DO-phase done? */

  যদি(!ফলাফল) {

    যদি(!*dophase_done)
      /* the DO phase has not completed yet */
      ফেরত CURLE_OK;

    ফলাফল = ftp_dophase_done(তথ্য, ftpc, ftp, connected);

    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
    freedirs(ftpc);

  ফেরত ফলাফল;
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
স্থির CURLcode ftp_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  কাঠামো FTP *ftp = Curl_meta_get(তথ্য, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  যদি(!ftpc || !ftp)
    ফেরত CURLE_FAILED_INIT;
  ftpc->wait_তথ্য_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    কাঠামো Curl_cwriter *ftp_lc_writer;

    ফলাফল = Curl_cwriter_create(&ftp_lc_writer, তথ্য, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    ফলাফল = Curl_cwriter_add(তথ্য, ftp_lc_writer);
    যদি(ফলাফল) {
      Curl_cwriter_free(তথ্য, ftp_lc_writer);
      ফেরত ফলাফল;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  যদি(তথ্য->state.wildcardmatch) {
    ফলাফল = wc_statemach(তথ্য, ftpc, ftp);
    যদি(তথ্য->wildcard->state == CURLWC_SKIP ||
       তথ্য->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      ফেরত CURLE_OK;
    }
    যদি(ফলাফল) /* error, loop or skipping the file */
      ফেরত ফলাফল;
  }
  অন্যথা { /* no wildcard FSM needed */
    ফলাফল = ftp_parse_url_path(তথ্য, ftpc, ftp);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফলাফল = ftp_regular_transfer(তথ্য, ftpc, ftp, done);

  ফেরত ফলাফল;
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
স্থির CURLcode ftp_quit(কাঠামো Curl_easy *তথ্য,
                         কাঠামো ftp_conn *ftpc)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(ftpc->ctl_valid) {
    CURL_TRC_FTP(তথ্য, "sending QUIT to close session");
    ফলাফল = Curl_pp_sendf(তথ্য, &ftpc->pp, "%s", "QUIT");
    যদি(ফলাফল) {
      failf(তথ্য, "Failure sending QUIT command: %s",
            curl_easy_strerror(ফলাফল));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(তথ্য->conn); /* mark for closure */
      ftp_state(তথ্য, ftpc, FTP_STOP);
      ফেরত ফলাফল;
    }

    ftp_state(তথ্য, ftpc, FTP_QUIT);

    ফলাফল = ftp_block_statemach(তথ্য, ftpc);
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode ftp_disconnect(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               bool dead_connection)
{
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  যদি(!ftpc)
    ফেরত CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  যদি(dead_connection || Curl_pp_needs_flush(তথ্য, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (শূন্য)ftp_quit(তথ্য, ftpc); /* ignore errors on the QUIT */
  ফেরত CURLE_OK;
}

/* called from multi.c while DOing */
স্থির CURLcode ftp_doing(কাঠামো Curl_easy *তথ্য,
                          bool *dophase_done)
{
  কাঠামো ftp_conn *ftpc = Curl_conn_meta_get(তথ্য->conn, CURL_META_FTP_CONN);
  কাঠামো FTP *ftp = Curl_meta_get(তথ্য, CURL_META_FTP_EASY);
  CURLcode ফলাফল;

  যদি(!ftpc || !ftp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = ftp_statemach(তথ্য, ftpc, dophase_done);

  যদি(ফলাফল)
    CURL_TRC_FTP(তথ্য, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  অন্যথা যদি(*dophase_done) {
    ফলাফল = ftp_dophase_done(তথ্য, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(তথ্য, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ফেরত ফলাফল;
}

স্থির শূন্য ftp_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো FTP *ftp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

স্থির শূন্য ftp_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো ftp_conn *ftpc = entry;
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

স্থির শূন্য type_url_check(কাঠামো Curl_easy *তথ্য, কাঠামো FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  যদি((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    অক্ষর *type = &ftp->path[len - 7];
    অক্ষর command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    নির্বাচন(command) {
    ক্ষেত্র 'A': /* ASCII mode */
      তথ্য->state.prefer_ascii = TRUE;
      বিরতি;

    ক্ষেত্র 'D': /* directory mode */
      তথ্য->state.list_only = TRUE;
      বিরতি;

    ক্ষেত্র 'I': /* binary mode */
    পূর্বনির্ধারিত:
      /* switch off ASCII */
      তথ্য->state.prefer_ascii = FALSE;
      বিরতি;
    }
  }
}

স্থির CURLcode ftp_setup_connection(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  কাঠামো FTP *ftp;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ftp_conn *ftpc;

  ftp = curlx_calloc(1, আকার(*ftp));
  যদি(!ftp ||
     Curl_meta_set(তথ্য, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, আকার(*ftpc));
  যদি(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  যদি(CURL_EASY_STR(তথ্য, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(তথ্য, STRING_FTP_ACCOUNT));
    যদি(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }
  যদি(CURL_EASY_STR(তথ্য, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(তথ্য, STRING_FTP_ALTERNATIVE_TO_USER));
    যদি(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  ftp->path = &তথ্য->state.up.path[1]; /* do not include the initial slash */

  type_url_check(তথ্য, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->known_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = তথ্য->set.use_ssl;
  ftpc->ccc = তথ্য->set.ftp_ccc;

  CURL_TRC_FTP(তথ্য, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

bool Curl_ftp_conns_match(কাঠামো connectdata *needle, কাঠামো connectdata *conn)
{
  কাঠামো ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  কাঠামো ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  যদি(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    ফেরত FALSE;

  নির্বাচন(nftpc->use_ssl) {
  ক্ষেত্র CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ফেরত (cftpc->use_ssl != CURLUSESSL_NONE);
  পূর্বনির্ধারিত:
    ফেরত (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_ftp = {
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
