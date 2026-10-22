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

#جے_بنا_تعریف CURL_DISABLE_FTP

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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

#جے_بنا_تعریف NI_MAXHOST
#تعریف_کرو NI_MAXHOST 1025
#جے_انت
#جے_بنا_تعریف INET_ADDRSTRLEN
#تعریف_کرو INET_ADDRSTRLEN 16
#جے_انت

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#تعریف_کرو STATUSCODE(سطر) \
  (ISDIGIT((سطر)[0]) && ISDIGIT((سطر)[1]) && ISDIGIT((سطر)[2]))

/* macro to check for the last line in an FTP server response */
#تعریف_کرو LASTLINE(سطر) (STATUSCODE(سطر) && (' ' == (سطر)[3]))

#جے_تعریف_شدہ CURLVERBOSE
/* for tracing purposes */
ساكن ثابت حرف * ثابت ftp_state_ناں[] = {
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
#تعریف_کرو FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_ناں[(ftpc)->state] : "???")

#جے_انت /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ساكن فراغ ftp_state_کم(بنية Curl_easy *انکڑے,
                          بنية ftp_conn *ftpc,
                          ftpstate newstate
#جے_تعریف_شدہ DEBUGBUILD
                          , صحيح lineno
#جے_انت
  )
{
  إذا(ftpc->state != newstate) {
#جے_تعریف_شدہ DEBUGBUILD
    NOVERBOSE((فراغ)lineno);
    CURL_TRC_FTP(انکڑے, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_ناں[newstate], lineno);
#وإلا
    CURL_TRC_FTP(انکڑے, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_ناں[newstate]);
#جے_انت
  }
  ftpc->state = newstate;
}

/* Local API functions */
#جے_بنا_تعریف DEBUGBUILD
#تعریف_کرو ftp_state(x, y, z) ftp_state_کم(x, y, z)
#وإلا /* !DEBUGBUILD */
#تعریف_کرو ftp_state(x, y, z) ftp_state_کم(x, y, z, __LINE__)
#جے_انت /* DEBUGBUILD */

ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp);
ساكن CURLcode ftp_state_quote(بنية Curl_easy *انکڑے,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init, ftpstate instate);
ساكن CURLcode ftp_nb_type(بنية Curl_easy *انکڑے,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate);
ساكن CURLcode getftpresponse(بنية Curl_easy *انکڑے, size_t *nreadp,
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
  size_t عدد = 0;
  افعل {
    slashPos = strchr(str, '/');
    إذا(slashPos) {
      ++عدد;
      str = slashPos + 1;
    }
  } طالما(slashPos);
  أرجع عدد;
}

#تعریف_کرو FTP_MAX_DIR_ڈونگھائی 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode ftp_parse_url_رستہ(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp)
{
  ثابت حرف *slashPos = عدم;
  ثابت حرف *fileName = عدم;
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *raw_رستہ = عدم; /* URL-decoded "raw" path */
  size_t رستہ_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  إذا(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  نتیجہ = Curl_urldecode(ftp->رستہ, 0, &ftpc->rawpath, &رستہ_Len, REJECT_CTRL);
  إذا(نتیجہ) {
    failf(انکڑے, "path contains control characters");
    أرجع نتیجہ;
  }
  raw_رستہ = ftpc->rawpath;

  بدل(انکڑے->سیٹ.ftp_filemethod) {
  حالة FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    إذا((رستہ_Len > 0) && (raw_رستہ[رستہ_Len - 1] != '/'))
      fileName = raw_رستہ;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    اكسر;

  حالة FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_رستہ, '/');
    إذا(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_رستہ;
      إذا(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_باہرلا_دا_حافظہ;

      ftpc->dirs[0].شروع = 0;
      ftpc->dirs[0].len = (صحيح)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    وإلا
      fileName = raw_رستہ; /* filename only (or empty) */
    اكسر;

  افتراضي: /* allow pretty much anything */
  حالة FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ثابت حرف *curPos = raw_رستہ;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_رستہ);

    إذا(dirAlloc >= FTP_MAX_DIR_ڈونگھائی)
      /* suspiciously deep directory hierarchy */
      أرجع CURLE_URL_MALFORMAT;

    إذا(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_باہرلا_دا_حافظہ;

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
          ftpc->dirs[ftpc->dirdepth].شروع = (صحيح)(curPos - raw_رستہ);
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

  إذا(انکڑے->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(انکڑے, "Uploading to a URL without a filename");
    أرجع CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  إذا((انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && (raw_رستہ[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  وإلا { /* newly created FTP connections are already in entry path */
    ثابت حرف *old_رستہ = انکڑے->conn->بٹ.reuse ? ftpc->prevpath : "";
    إذا(old_رستہ) {
      size_t n = رستہ_Len;
      إذا(انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      وإلا
        n -= ftpc->file ? لڑی_لمبائی(ftpc->file) : 0;

      إذا((لڑی_لمبائی(old_رستہ) == n) && raw_رستہ && !لڑی_مقابلہ(raw_رستہ, old_رستہ, n)) {
        infof(انکڑے, "Request has same path as previous transfer");
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

ساكن فراغ close_secondarysocket(بنية Curl_easy *انکڑے,
                                  بنية ftp_conn *ftpc)
{
  (فراغ)ftpc;
  CURL_TRC_FTP(انکڑے, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(انکڑے, انکڑے->conn, SECONDARYSOCKET);
}

#جے_تعریف_شدہ CURL_PREFER_LF_LINEENDS
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

ساكن CURLcode ftp_cw_lc_write(بنية Curl_easy *انکڑے,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t blen)
{
  ساكن ثابت حرف nl = '\n';
  بنية ftp_cw_lc_ctx *ctx = writer->ctx;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  إذا(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  طالما(blen) {
    /* do not pass EOS when writing parts */
    صحيح chunk_type = (type & ~CLIENTWRITE_EOS);
    ثابت حرف *cp;
    size_t chunk_len;
    CURLcode نتیجہ;

    إذا(ctx->newline_pending) {
      إذا(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        نتیجہ = Curl_cwriter_write(انکڑے, writer->اگلا, chunk_type, &nl, 1);
        إذا(نتیجہ)
          أرجع نتیجہ;
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
      نتیجہ = Curl_cwriter_write(انکڑے, writer->اگلا, chunk_type,
                                  buf, chunk_len);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  إذا(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, blen);
  }
  وإلا إذا(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    إذا(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, 0);
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

#جے_انت /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ساكن CURLcode ftp_check_ctrl_on_انکڑے_wait(بنية Curl_easy *انکڑے,
                                            بنية ftp_conn *ftpc)
{
  بنية connectdata *conn = انکڑے->conn;
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
      infof(انکڑے, "There is negative response in cache while serv connect");
      (فراغ)getftpresponse(انکڑے, &nread, &ftpcode);
      أرجع CURLE_FTP_ACCEPT_FAILED;
    }
  }

  إذا(pp->حد_توں_ودھ)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  وإلا {
    صحيح socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    بدل(socketstate) {
    حالة -1: /* error */
      /* let's die here */
      failf(انکڑے, "Error while waiting for server connect");
      أرجع CURLE_FTP_ACCEPT_FAILED;
    افتراضي:
      إذا(socketstate & CURL_CSELECT_وچ)
        response = TRUE;
      اكسر;
    }
  }

  إذا(response) {
    infof(انکڑے, "Ctrl conn has data while waiting for data conn");
    إذا(pp->حد_توں_ودھ > 3) {
      ثابت حرف *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->حد_توں_ودھ + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      إذا((len > 3) && LASTLINE(r)) {
        curl_off_t حالت;
        إذا(!curlx_str_نمبر(&r, &حالت, 999) && (حالت == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(انکڑے, "Got 226 before data activity");
          أرجع CURLE_OK;
        }
      }
    }

    (فراغ)getftpresponse(انکڑے, &nread, &ftpcode);

    infof(انکڑے, "FTP code: %03d", ftpcode);

    إذا(ftpcode / 100 > 3)
      أرجع CURLE_FTP_ACCEPT_FAILED;

    أرجع CURLE_WEIRD_SERVER_جواب;
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
ساكن CURLcode ftp_initiate_transfer(بنية Curl_easy *انکڑے,
                                      بنية ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(انکڑے, "ftp_initiate_transfer()");
  نتیجہ = Curl_conn_connect(انکڑے, SECONDARYSOCKET, TRUE, &connected);
  إذا(نتیجہ || !connected)
    أرجع نتیجہ;

  إذا(انکڑے->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_تیاری_send(انکڑے, SECONDARYSOCKET);
    Curl_xfer_سیٹ_shutdown(انکڑے, TRUE, TRUE);
  }
  وإلا {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_تیاری_recv(انکڑے, SECONDARYSOCKET, انکڑے->req.ناپ);
    Curl_xfer_سیٹ_shutdown(انکڑے, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(انکڑے, ftpc, FTP_STOP);

  أرجع CURLE_OK;
}

ساكن bool ftp_endofresp(بنية Curl_easy *انکڑے, بنية connectdata *conn,
                          ثابت حرف *سطر, size_t len, صحيح *کوڈ)
{
  curl_off_t حالت;
  (فراغ)انکڑے;
  (فراغ)conn;

  إذا((len > 3) && LASTLINE(سطر) && !curlx_str_نمبر(&سطر, &حالت, 999)) {
    *کوڈ = (صحيح)حالت;
    أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن CURLcode ftp_readresp(بنية Curl_easy *انکڑے,
                             بنية ftp_conn *ftpc,
                             int8_t sockindex,
                             بنية pingpong *pp,
                             صحيح *ftpcodep, /* return the ftp-code if done */
                             size_t *ناپ) /* size of the response */
{
  صحيح کوڈ;
  CURLcode نتیجہ = Curl_pp_readresp(انکڑے, sockindex, pp, &کوڈ, ناپ);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  إذا(!ftpc->shutdown)
    انکڑے->info.httpcode = کوڈ;

  *ftpcodep = کوڈ;

  إذا(کوڈ == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(انکڑے, "We got a 421 - timeout");
    ftp_state(انکڑے, ftpc, FTP_STOP);
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  أرجع نتیجہ;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ساكن CURLcode getftpresponse(بنية Curl_easy *انکڑے,
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

  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode نتیجہ = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp = &ftpc->pp;
  size_t nread;
  صحيح cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(انکڑے, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  طالما(!*ftpcodep && !نتیجہ) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(انکڑے, pp);
    timediff_t interval_ms;

    إذا(timeout <= 0) {
      failf(انکڑے, "FTP response timeout");
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
    وإلا إذا(!Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(انکڑے, pp) ?
        sockfd : CURL_SOCKET_BAD;
      صحيح ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      إذا(ev < 0) {
        failf(انکڑے, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }
      وإلا إذا(ev == 0) {
        نتیجہ = Curl_pgrsUpdate(انکڑے);
        استمر; /* continue in our loop for the timeout duration */
      }
    }

    إذا(Curl_pp_needs_flush(انکڑے, pp)) {
      نتیجہ = Curl_pp_flushsend(انکڑے, pp);
      إذا(نتیجہ)
        اكسر;
    }

    نتیجہ = ftp_readresp(انکڑے, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    إذا(نتیجہ)
      اكسر;

    إذا(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
انکڑے*/
      cache_skip++;
    وإلا
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(انکڑے, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (صحيح)نتیجہ, *nreadp, *ftpcodep);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_user(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية connectdata *conn)
{
  CURLcode نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  إذا(!نتیجہ) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(انکڑے, ftpc, FTP_USER);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_pwd(بنية Curl_easy *انکڑے,
                              بنية ftp_conn *ftpc)
{
  CURLcode نتیجہ;
#جے_تعریف_شدہ DEBUGBUILD
  إذا(!انکڑے->id && getenv("CURL_FTP_PWD_STOP"))
    أرجع CURLE_OK;
#جے_انت
  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "PWD");
  إذا(!نتیجہ)
    ftp_state(انکڑے, ftpc, FTP_PWD);

  أرجع نتیجہ;
}

/* For the FTP "protocol connect" and "doing" phases only */
ساكن CURLcode ftp_pollset(بنية Curl_easy *انکڑے,
                            بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? Curl_pp_pollset(انکڑے, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ساكن CURLcode ftp_domore_pollset(بنية Curl_easy *انکڑے,
                                   بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(انکڑے, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  إذا(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(انکڑے->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (انکڑے->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(انکڑے->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    أرجع Curl_pollset_جوڑو_وچ(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
  }
  أرجع Curl_pp_pollset(انکڑے, &ftpc->pp, ps);
}

ساكن صحيح pathlen(بنية ftp_conn *ftpc, صحيح عدد)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > عدد);
  أرجع ftpc->dirs[عدد].len;
}

ساكن ثابت حرف *pathpiece(بنية ftp_conn *ftpc, صحيح عدد)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > عدد);
  أرجع &ftpc->rawpath[ftpc->dirs[عدد].شروع];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ساكن CURLcode ftp_state_cwd(بنية Curl_easy *انکڑے,
                              بنية ftp_conn *ftpc,
                              بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(ftpc->cwddone)
    /* already done and fine */
    نتیجہ = ftp_state_mdtm(انکڑے, ftpc, ftp);
  وإلا {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((انکڑے->سیٹ.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    إذا(انکڑے->conn->بٹ.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "CWD %s", ftpc->entrypath);
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_CWD);
    }
    وإلا {
      إذا(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        إذا(!نتیجہ)
          ftp_state(انکڑے, ftpc, FTP_CWD);
      }
      وإلا {
        /* No CWD necessary */
        نتیجہ = ftp_state_mdtm(انکڑے, ftpc, ftp);
      }
    }
  }
  أرجع نتیجہ;
}

تعريفنوع تعداد {
  EPRT,
  پورٹ_2,
  پورا_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ساكن CURLcode ftp_پورٹ_parse_string(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn,
                                      ثابت حرف *string_ftpport,
                                      بنية Curl_sockaddr_storage *ss,
                                      بنانشانی چھوٹا *پورٹ_minp,
                                      بنانشانی چھوٹا *پورٹ_maxp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen)
{
  ثابت حرف *ip_سرا = عدم;
  ثابت حرف *addr = عدم;
  size_t addrlen = 0;
  بنانشانی چھوٹا پورٹ_min = 0;
  بنانشانی چھوٹا پورٹ_max = 0;
  حرف ipstr[50];
#جے_بنا_تعریف USE_IPV6
  (فراغ)conn;
  (فراغ)ss;
#جے_انت

  /* default to nothing */
  *hostp = عدم;
  *پورٹ_minp = *پورٹ_maxp = 0;

  إذا(!string_ftpport || (لڑی_لمبائی(string_ftpport) <= 1))
    اذهب پورا;

#جے_تعریف_شدہ USE_IPV6
  إذا(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ثابت حرف *ip_شروع = string_ftpport + 1;
    ip_سرا = strchr(ip_شروع, ']');
    إذا(ip_سرا) {
      addrlen = ip_سرا - ip_شروع;
      addr = ip_شروع;
    }
  }
  وإلا
#جے_انت
    إذا(*string_ftpport == ':') {
      /* :port */
      ip_سرا = string_ftpport;
    }
    وإلا {
      ip_سرا = strchr(string_ftpport, ':');
      addr = string_ftpport;
      إذا(ip_سرا) {
#جے_تعریف_شدہ USE_IPV6
        بنية sockaddr_in6 * ثابت sa6 = (فراغ *)ss;
#جے_انت
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_سرا - string_ftpport;
#جے_تعریف_شدہ USE_IPV6
        إذا(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = لڑی_لمبائی(string_ftpport);
          ip_سرا = عدم; /* this got no port ! */
        }
#جے_انت
      }
      وإلا
        /* ipv4|interface */
        addrlen = لڑی_لمبائی(string_ftpport);
    }

  /* parse the port */
  إذا(ip_سرا) {
    ثابت حرف *portp = strchr(ip_سرا, ':');
    إذا(portp) {
      curl_off_t شروع;
      curl_off_t سرا;
      portp++;
      إذا(!curlx_str_نمبر(&portp, &شروع, 0xffff)) {
        پورٹ_min = (بنانشانی چھوٹا)شروع;
        إذا(!curlx_str_single(&portp, '-') &&
           !curlx_str_نمبر(&portp, &سرا, 0xffff))
          پورٹ_max = (بنانشانی چھوٹا)سرا;
        وإلا
          پورٹ_max = پورٹ_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  إذا(پورٹ_min > پورٹ_max)
    پورٹ_min = پورٹ_max = 0;

  إذا(addrlen) {
    ثابت بنية Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(انکڑے, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    إذا(!remote_addr || (addrlen >= حجم(ipstr)) || (addrlen >= hbuflen))
      أرجع CURLE_FTP_پورٹ_FAILED;
    حافظہ_نقل(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    بدل(Curl_if2ip(remote_addr->family,
#جے_تعریف_شدہ USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#جے_انت
                      ipstr, hbuf, hbuflen)) {
    حالة IF2_IP_NOT_لبھیا:
      /* not an interface, use the string as hostname instead */
      حافظہ_نقل(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      اكسر;
    حالة IF2IP_AF_NOT_SUPPORTED:
      أرجع CURLE_FTP_پورٹ_FAILED;
    حالة IF2_IP_لبھیا:
      *hostp = hbuf; /* use the hbuf for hostname */
      اكسر;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

پورا:
  *پورٹ_minp = پورٹ_min;
  *پورٹ_maxp = پورٹ_max;
  أرجع CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ساكن CURLcode ftp_پورٹ_default_host(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#جے_تعریف_شدہ USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#جے_انت
  حرف بفر[STRERROR_LEN];
  CURLcode نتیجہ;

  *sslenp = حجم(*ss);
  إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(انکڑے, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    أرجع CURLE_FTP_پورٹ_FAILED;
  }
  بدل(sa->sa_family) {
#جے_تعریف_شدہ USE_IPV6
  حالة AF_INET6:
    نتیجہ = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    اكسر;
#جے_انت
  افتراضي:
    نتیجہ = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    اكسر;
  }
  إذا(نتیجہ)
    أرجع CURLE_FTP_پورٹ_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  أرجع CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ساكن CURLcode ftp_پورٹ_resolve_host(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn,
                                      ثابت حرف *host,
                                      بنية Curl_dns_اندراج **dns_entryp,
                                      ثابت بنية Curl_addrinfo **resp)
{
  CURLcode نتیجہ;

  *resp = عدم;
  نتیجہ = Curl_resolv_blocking(
    انکڑے, Curl_resolv_dns_queries(انکڑے, conn->ip_نسخہ),
    host, 0, Curl_conn_get_transport(انکڑے, conn), dns_entryp);
  إذا(نتیجہ)
    failf(انکڑے, "failed to resolve the address provided to PORT: %s", host);
  وإلا {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  أرجع نتیجہ;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ساكن CURLcode ftp_پورٹ_open_socket(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn,
                                     ثابت بنية Curl_addrinfo *res,
                                     ثابت بنية Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  حرف بفر[STRERROR_LEN];
  صحيح sockerr = 0;
  ثابت بنية Curl_addrinfo *ai;
  CURLcode نتیجہ = CURLE_FTP_پورٹ_FAILED;

  لكل(ai = res; ai; ai = ai->ai_اگلا) {
    نتیجہ =
      Curl_socket_open(انکڑے, ai, عدم,
                       Curl_conn_get_transport(انکڑے, conn), portsockp);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
        أرجع نتیجہ;
      نتیجہ = CURLE_FTP_پورٹ_FAILED;
      sockerr = SOCKERRNO;
      استمر;
    }
    اكسر;
  }
  إذا(!ai) {
    failf(انکڑے, "socket failure: %s",
          curlx_strerror(sockerr, بفر, حجم(بفر)));
    أرجع CURLE_FTP_پورٹ_FAILED;
  }
  *aip = ai;
  أرجع نتیجہ;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ساكن CURLcode ftp_پورٹ_bind_socket(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn,
                                     curl_socket_t portsock,
                                     ثابت بنية Curl_addrinfo *ai,
                                     بنية Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     بنانشانی چھوٹا پورٹ_min,
                                     بنانشانی چھوٹا پورٹ_max,
                                     bool non_local)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#جے_تعریف_شدہ USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#جے_انت
  حرف بفر[STRERROR_LEN];
  بنانشانی چھوٹا پورٹ;
  صحيح sockerr;

  حافظہ_نقل(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  لكل(پورٹ = پورٹ_min; پورٹ <= پورٹ_max;) {
    إذا(sa->sa_family == AF_INET)
      sa4->sin_port = htons(پورٹ);
#جے_تعریف_شدہ USE_IPV6
    وإلا
      sa6->sin6_port = htons(پورٹ);
#جے_انت
    إذا(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      إذا(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(انکڑے, "bind(port=%hu) on non-local address failed: %s", پورٹ,
              curlx_strerror(sockerr, بفر, حجم(بفر)));

        *sslen_io = حجم(*ss);
        إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(انکڑے, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
          أرجع CURLE_FTP_پورٹ_FAILED;
        }
        پورٹ = پورٹ_min;
        non_local = FALSE; /* do not try this again */
        استمر;
      }
      إذا(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(انکڑے, "bind(port=%hu) failed: %s", پورٹ,
              curlx_strerror(sockerr, بفر, حجم(بفر)));
        أرجع CURLE_FTP_پورٹ_FAILED;
      }
    }
    وإلا
      اكسر;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    إذا(پورٹ == پورٹ_max) {
      failf(انکڑے, "bind() failed, ran out of ports");
      أرجع CURLE_FTP_پورٹ_FAILED;
    }
    پورٹ++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = حجم(*ss);
  إذا(getsockname(portsock, sa, sslen_io)) {
    failf(انکڑے, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    أرجع CURLE_FTP_پورٹ_FAILED;
  }
  CURL_TRC_FTP(انکڑے, "ftp_port_bind_socket(), socket bound to port %d", پورٹ);
  أرجع CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ساكن CURLcode ftp_پورٹ_listen(بنية Curl_easy *انکڑے, curl_socket_t portsock)
{
  حرف بفر[STRERROR_LEN];

  إذا(listen(portsock, 1)) {
    failf(انکڑے, "socket failure: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    أرجع CURLE_FTP_پورٹ_FAILED;
  }
  CURL_TRC_FTP(انکڑے, "ftp_port_listen(), listening on port");
  أرجع CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ساكن CURLcode ftp_پورٹ_send_command(بنية Curl_easy *انکڑے,
                                      بنية ftp_conn *ftpc,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      ثابت بنية Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ساكن ثابت حرف منوال[][5] = { "EPRT", "PORT" };
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#جے_تعریف_شدہ USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#جے_انت
  حرف myhost[MAX_IPADR_LEN + 1] = "";
  بنانشانی چھوٹا پورٹ;
  CURLcode نتیجہ;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_پتہ(ai, myhost, حجم(myhost));

#جے_تعریف_شدہ USE_IPV6
  إذا(!conn->بٹ.ftp_use_eprt && conn->بٹ.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->بٹ.ftp_use_eprt = TRUE;
#جے_انت

  لكل(; fcmd != پورا_2; fcmd++) {

    إذا(!conn->بٹ.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      استمر;

    إذا((پورٹ_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      استمر;

    بدل(sa->sa_family) {
    حالة AF_INET:
      پورٹ = ntohs(sa4->sin_port);
      اكسر;
#جے_تعریف_شدہ USE_IPV6
    حالة AF_INET6:
      پورٹ = ntohs(sa6->sin6_port);
      اكسر;
#جے_انت
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
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s |%d|%s|%hu|", منوال[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, پورٹ);
      إذا(نتیجہ) {
        failf(انکڑے, "Failure sending EPRT command: %s",
              curl_easy_strerror(نتیجہ));
        أرجع نتیجہ;
      }
      اكسر;
    }
    إذا(پورٹ_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      حرف نشانہ[حجم(myhost) + 20];
      ثابت حرف *سروت = myhost;
      حرف *dest = نشانہ;

      /* translate x.x.x.x to x,x,x,x */
      طالما(*سروت) {
        إذا(*سروت == '.')
          *dest = ',';
        وإلا
          *dest = *سروت;
        dest++;
        سروت++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (صحيح)(پورٹ >> 8), (صحيح)(پورٹ & 0xff));

      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s %s", منوال[fcmd], نشانہ);
      إذا(نتیجہ) {
        failf(انکڑے, "Failure sending PORT command: %s",
              curl_easy_strerror(نتیجہ));
        أرجع نتیجہ;
      }
      اكسر;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(انکڑے, ftpc, FTP_پورٹ);
  أرجع CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ساكن CURLcode ftp_state_use_پورٹ(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode نتیجہ = CURLE_FTP_پورٹ_FAILED;
  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  بنية Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  حرف hbuf[NI_MAXHOST];
  ثابت حرف *host = عدم;
  ثابت حرف *string_ftpport = CURL_EASY_STR(انکڑے, STRING_FTPPORT);
  بنية Curl_dns_اندراج *dns_اندراج = عدم;
  ثابت بنية Curl_addrinfo *res = عدم;
  ثابت بنية Curl_addrinfo *ai = عدم;
  بنانشانی چھوٹا پورٹ_min = 0;
  بنانشانی چھوٹا پورٹ_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  نتیجہ = ftp_پورٹ_parse_string(انکڑے, conn, string_ftpport,
                                 &ss, &پورٹ_min, &پورٹ_max,
                                 &host, hbuf, حجم(hbuf));
  إذا(!نتیجہ && !host)
    /* if no host was specified, use the control connection's local IP */
    نتیجہ = ftp_پورٹ_default_host(انکڑے, conn, &ss, &sslen, &host,
                                   hbuf, حجم(hbuf), &non_local);

  /* resolve host string to address list */
  إذا(!نتیجہ)
    نتیجہ = ftp_پورٹ_resolve_host(انکڑے, conn, host, &dns_اندراج, &res);

  /* Open a TCP socket for the data connection */
  إذا(!نتیجہ)
    نتیجہ = ftp_پورٹ_open_socket(انکڑے, conn, res, &ai, &portsock);
  إذا(!نتیجہ) {
    CURL_TRC_FTP(انکڑے, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    نتیجہ = ftp_پورٹ_bind_socket(انکڑے, conn, portsock, ai, &ss, &sslen,
                                  پورٹ_min, پورٹ_max, non_local);
  }

  /* listen */
  إذا(!نتیجہ)
    نتیجہ = ftp_پورٹ_listen(انکڑے, portsock);

  /* send the PORT / EPRT command */
  إذا(!نتیجہ)
    نتیجہ = ftp_پورٹ_send_command(انکڑے, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_tcp_listen_سیٹ(انکڑے, conn, SECONDARYSOCKET, &portsock);

  إذا(!نتیجہ)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  إذا(dns_اندراج)
    Curl_dns_اندراج_unlink(انکڑے, &dns_اندراج);
  إذا(نتیجہ) {
    ftp_state(انکڑے, ftpc, FTP_STOP);
  }
  وإلا {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    إذا(conn->بٹ.ftp_use_انکڑے_ssl && انکڑے->سیٹ.ftp_use_پورٹ &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      نتیجہ = Curl_ssl_cfilter_جوڑو(
        انکڑے, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->بٹ.do_ہور = FALSE;
    Curl_expire(انکڑے, (انکڑے->سیٹ.accepttimeout > 0) ?
                انکڑے->سیٹ.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  إذا(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(انکڑے, conn, portsock);
  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_use_pasv(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc,
                                   بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  ساكن ثابت حرف منوال[][5] = { "EPSV", "PASV" };
  صحيح modeoff;

#جے_تعریف_شدہ PF_INET6
  إذا(!conn->بٹ.ftp_use_epsv && conn->بٹ.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->بٹ.ftp_use_epsv = TRUE;
#جے_انت

  modeoff = conn->بٹ.ftp_use_epsv ? 0 : 1;

  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", منوال[modeoff]);
  إذا(!نتیجہ) {
    ftpc->count1 = modeoff;
    ftp_state(انکڑے, ftpc, FTP_PASV);
    infof(انکڑے, "Connect data stream passively");
  }
  أرجع نتیجہ;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ساكن CURLcode ftp_state_prepare_transfer(بنية Curl_easy *انکڑے,
                                           بنية ftp_conn *ftpc,
                                           بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  إذا(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(انکڑے, ftpc, FTP_RETR_PREQUOTE);
    نتیجہ = ftp_state_quote(انکڑے, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  وإلا إذا(انکڑے->سیٹ.ftp_use_پورٹ) {
    /* We have chosen to use the PORT (or similar) command */
    نتیجہ = ftp_state_use_پورٹ(انکڑے, ftpc, EPRT);
  }
  وإلا {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    إذا(انکڑے->سیٹ.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      إذا(!ftpc->file)
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) :
                               (انکڑے->state.list_only ? "NLST" : "LIST"));
      وإلا إذا(انکڑے->state.upload)
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "PRET STOR %s", ftpc->file);
      وإلا
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "PRET RETR %s", ftpc->file);
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_PRET);
    }
    وإلا
      نتیجہ = ftp_state_use_pasv(انکڑے, ftpc, conn);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_rest(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "REST %d", 0);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_REST);
  }
  وإلا
    نتیجہ = ftp_state_prepare_transfer(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_ناپ(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "SIZE %s", ftpc->file);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_ناپ);
  }
  وإلا
    نتیجہ = ftp_state_rest(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_list(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

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

  إذا((انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && ftp->رستہ) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ثابت حرف *raw_رستہ = ftpc->rawpath;
    ثابت حرف *slashPos = strrchr(raw_رستہ, '/');
    إذا(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_رستہ;
      إذا(n == 0)
        ++n;

      lstArg = raw_رستہ;
      lstArglen = (صحيح)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) :
                      (انکڑے->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  إذا(!cmd)
    أرجع CURLE_باہرلا_دا_حافظہ;

  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  إذا(!نتیجہ)
    ftp_state(انکڑے, ftpc, FTP_LIST);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_list_prequote(بنية Curl_easy *انکڑے,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(انکڑے, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ساكن CURLcode ftp_state_retr_prequote(بنية Curl_easy *انکڑے,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(انکڑے, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ساكن CURLcode ftp_state_stor_prequote(بنية Curl_easy *انکڑے,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(انکڑے, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ساكن CURLcode ftp_state_type(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  إذا(انکڑے->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)انکڑے->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    نتیجہ = ftp_nb_type(انکڑے, ftpc, ftp, (bool)انکڑے->state.prefer_ascii,
                         FTP_TYPE);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
    نتیجہ = ftp_state_ناپ(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  إذا((انکڑے->سیٹ.get_filetime || انکڑے->سیٹ.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "MDTM %s", ftpc->file);

    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_MDTM);
  }
  وإلا
    نتیجہ = ftp_state_type(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

/* This is called after the TYPE and possible quote commands have been sent */
ساكن CURLcode ftp_state_ul_تیاری(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   bool sizechecked)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_ثنائی_ہندسہ append = انکڑے->سیٹ.remote_append;

  إذا((انکڑے->state.resume_from && !sizechecked) ||
     ((انکڑے->state.resume_from > 0) && sizechecked)) {
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

    إذا(انکڑے->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "SIZE %s", ftpc->file);
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_STOR_ناپ);
      أرجع نتیجہ;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    إذا(انکڑے->سیٹ.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, easy_seek_func);
      seekerr = انکڑے->سیٹ.seek_func(انکڑے->سیٹ.seek_client,
                                    انکڑے->state.resume_from, SEEK_SET);
      CURL_CBAPI_سرا(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(انکڑے, "Could not seek stream");
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (انکڑے->state.resume_from - passed > (curl_off_t)حجم(scratch)) ?
          حجم(scratch) :
          curlx_sotouz(انکڑے->state.resume_from - passed);

        size_t actuallyread =
          انکڑے->state.fread_func(scratch, 1, readthisamountnow,
                                 انکڑے->state.وچ);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(انکڑے, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < انکڑے->state.resume_from);
    }
    /* now, decrease the size of the read */
    إذا(انکڑے->state.infilesize > 0) {
      انکڑے->state.infilesize -= انکڑے->state.resume_from;

      إذا(انکڑے->state.infilesize <= 0) {
        infof(انکڑے, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_تیاری_nop(انکڑے);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(انکڑے, ftpc, FTP_STOP);
        أرجع CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  إذا(!نتیجہ)
    ftp_state(انکڑے, ftpc, FTP_STOR);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_retr(بنية Curl_easy *انکڑے,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_FTP(انکڑے, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  إذا(انکڑے->سیٹ.max_filesize && (filesize > انکڑے->سیٹ.max_filesize)) {
    failf(انکڑے, "Maximum file size exceeded");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  إذا(انکڑے->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    إذا(filesize == -1) {
      infof(انکڑے, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    وإلا {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      إذا(انکڑے->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        إذا(filesize < -انکڑے->state.resume_from) {
          failf(انکڑے, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                انکڑے->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -انکڑے->state.resume_from;
        /* download from where? */
        انکڑے->state.resume_from = filesize - ftp->downloadsize;
      }
      وإلا {
        إذا(filesize < انکڑے->state.resume_from) {
          failf(انکڑے, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                انکڑے->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - انکڑے->state.resume_from;
      }
    }

    إذا(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_تیاری_nop(انکڑے);
      infof(انکڑے, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(انکڑے, ftpc, FTP_STOP);
      أرجع CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(انکڑے, "Instructs server to resume from offset %" FMT_OFF_T,
          انکڑے->state.resume_from);

    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "REST %" FMT_OFF_T,
                           انکڑے->state.resume_from);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_RETR_REST);
  }
  وإلا {
    /* no resume */
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "RETR %s", ftpc->file);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_RETR);
  }

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_quote(بنية Curl_easy *انکڑے,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  bool quote = FALSE;
  بنية curl_slist *شے;

  بدل(instate) {
  حالة FTP_QUOTE:
  افتراضي:
    شے = انکڑے->سیٹ.quote;
    اكسر;
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    شے = انکڑے->سیٹ.prequote;
    اكسر;
  حالة FTP_POSTQUOTE:
    شے = انکڑے->سیٹ.postquote;
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

  إذا(شے) {
    صحيح i = 0;

    /* Skip count1 items in the linked list */
    طالما((i < ftpc->count1) && شے) {
      شے = شے->اگلا;
      i++;
    }
    إذا(شے) {
      ثابت حرف *cmd = شے->انکڑے;
      إذا(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      وإلا
        ftpc->count2 = 0; /* failure means cancel operation */

      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", cmd);
      إذا(نتیجہ)
        أرجع نتیجہ;
      ftp_state(انکڑے, ftpc, instate);
      quote = TRUE;
    }
  }

  إذا(!quote) {
    /* No more quote to send, continue to ... */
    بدل(instate) {
    حالة FTP_QUOTE:
    افتراضي:
      نتیجہ = ftp_state_cwd(انکڑے, ftpc, ftp);
      اكسر;
    حالة FTP_RETR_PREQUOTE:
      إذا(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(انکڑے, ftpc, FTP_STOP);
      وإلا {
        إذا(ftpc->معلوم_filesize != -1) {
          Curl_pgrs_سیٹ_Download_ناپ(انکڑے, ftpc->معلوم_filesize);
          نتیجہ = ftp_state_retr(انکڑے, ftpc, ftp, ftpc->معلوم_filesize);
        }
        وإلا {
          إذا(انکڑے->سیٹ.ignorecl || انکڑے->state.prefer_ascii) {
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
            نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "RETR %s", ftpc->file);
            إذا(!نتیجہ)
              ftp_state(انکڑے, ftpc, FTP_RETR);
          }
          وإلا {
            نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "SIZE %s", ftpc->file);
            إذا(!نتیجہ)
              ftp_state(انکڑے, ftpc, FTP_RETR_ناپ);
          }
        }
      }
      اكسر;
    حالة FTP_STOR_PREQUOTE:
      نتیجہ = ftp_state_ul_تیاری(انکڑے, ftpc, ftp, FALSE);
      اكسر;
    حالة FTP_POSTQUOTE:
      اكسر;
    حالة FTP_LIST_PREQUOTE:
      ftp_state(انکڑے, ftpc, FTP_LIST_TYPE);
      نتیجہ = ftp_state_list(انکڑے, ftpc, ftp);
      اكسر;
    }
  }

  أرجع نتیجہ;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ساكن CURLcode ftp_epsv_disable(بنية Curl_easy *انکڑے,
                                 بنية ftp_conn *ftpc,
                                 بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(conn->بٹ.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(انکڑے, "Failed EPSV attempt, exiting");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  infof(انکڑے, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->بٹ.ftp_use_epsv = FALSE;
  close_secondarysocket(انکڑے, ftpc);
  انکڑے->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "PASV");
  إذا(!نتیجہ) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(انکڑے, ftpc, FTP_PASV);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ftp_control_addr_dup(بنية Curl_easy *انکڑے, حرف **newhostp)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  وإلا
#جے_انت
  إذا(!Curl_conn_get_ip_info(انکڑے, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  وإلا {
    /* failed to get the remote_ip of the DATA connection */
    failf(انکڑے, "unable to get peername of DATA connection");
    *newhostp = عدم;
    أرجع CURLE_FTP_CANT_GET_HOST;
  }
  أرجع *newhostp ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
}

ساكن bool میل_pasv_6nums(ثابت حرف *p,
                             بنانشانی صحيح *array) /* 6 numbers */
{
  صحيح i;
  لكل(i = 0; i < 6; i++) {
    curl_off_t عدد;
    إذا(i) {
      إذا(*p != ',')
        أرجع FALSE;
      p++;
    }
    إذا(curlx_str_نمبر(&p, &عدد, 0xff))
      أرجع FALSE;
    array[i] = (بنانشانی صحيح)عدد;
  }
  أرجع TRUE;
}

ساكن CURLcode ftp_state_pasv_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ;
  ثابت بنية pingpong *pp = &ftpc->pp;
  حرف *newhost = عدم;
  بنانشانی چھوٹا newport = 0;
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
        curl_off_t عدد;
        إذا(curlx_str_نمبر(&p, &عدد, 0xffff) || (*p != sep)) {
          failf(انکڑے, "Illegal port number in EPSV reply");
          أرجع CURLE_FTP_WEIRD_PASV_جواب;
        }
        newport = (بنانشانی چھوٹا)عدد;
        نتیجہ = ftp_control_addr_dup(انکڑے, &newhost);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
      وإلا
        ptr = عدم;
    }
    إذا(!ptr) {
      failf(انکڑے, "Weirdly formatted EPSV reply");
      أرجع CURLE_FTP_WEIRD_PASV_جواب;
    }
  }
  وإلا إذا((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    بنانشانی صحيح ip[6];

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
      إذا(میل_pasv_6nums(str, ip))
        اكسر;
      str++;
    }

    إذا(!*str) {
      failf(انکڑے, "Could not interpret the 227-response");
      أرجع CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    إذا(انکڑے->سیٹ.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(انکڑے, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      نتیجہ = ftp_control_addr_dup(انکڑے, &newhost);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    إذا(!newhost)
      أرجع CURLE_باہرلا_دا_حافظہ;

    newport = (بنانشانی چھوٹا)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  وإلا إذا(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    أرجع ftp_epsv_disable(انکڑے, ftpc, conn);
  }
  وإلا {
    failf(انکڑے, "Bad PASV/EPSV response: %03d", ftpcode);
    أرجع CURLE_FTP_WEIRD_PASV_جواب;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  نتیجہ = Curl_peer_create(انکڑے, conn->scheme, newhost, newport,
                            &conn->origin2);
  إذا(نتیجہ)
    اذهب error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  إذا(conn->راہیں_peer) {
    Curl_peer_unlink(&conn->راہیں_peer2);
    نتیجہ = Curl_peer_create(انکڑے, conn->راہیں_peer->scheme,
                              conn->راہیں_peer->hostname, newport,
                              &conn->راہیں_peer2);
    إذا(نتیجہ)
      اذهب error;
  }

  نتیجہ = Curl_conn_تیاری(انکڑے, conn, SECONDARYSOCKET,
                           conn->بٹ.ftp_use_انکڑے_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  إذا(نتیجہ) {
    إذا((نتیجہ != CURLE_باہرلا_دا_حافظہ) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      نتیجہ = ftp_epsv_disable(انکڑے, ftpc, conn);
    }
    اذهب error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#جے_تعریف_شدہ CURLVERBOSE
  إذا(انکڑے->سیٹ.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(انکڑے, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->پورٹ);
  }
#جے_انت

  conn->بٹ.do_ہور = TRUE;
  ftp_state(انکڑے, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  أرجع نتیجہ;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_statemach(بنية Curl_easy *انکڑے,
                              بنية ftp_conn *ftpc,
                              bool *پورا)
{
  CURLcode نتیجہ = Curl_pp_statemach(انکڑے, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *پورا = (ftpc->state == FTP_STOP);

  أرجع نتیجہ;
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
ساكن CURLcode ftp_do_ہور(بنية Curl_easy *انکڑے, domore *ہور)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(انکڑے, CURL_META_FTP_EASY);
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;

  *ہور = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  إذا(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(انکڑے, SECONDARYSOCKET);
    نتیجہ = Curl_conn_connect(انکڑے, SECONDARYSOCKET, FALSE, &connected);
    إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
      أرجع نتیجہ;
    إذا(نتیجہ || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(انکڑے, SECONDARYSOCKET))) {
      إذا(نتیجہ && !is_eptr && (ftpc->count1 == 0)) {
        *ہور = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        أرجع ftp_epsv_disable(انکڑے, ftpc, conn);
      }
      أرجع نتیجہ;
    }
  }

  إذا(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    نتیجہ = ftp_statemach(انکڑے, ftpc, &complete);

    إذا(complete)
      *ہور = DOMORE_پورا;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    إذا(نتیجہ || !ftpc->wait_انکڑے_conn)
      أرجع نتیجہ;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *ہور = DOMORE_INCOMPLETE;
  }

  إذا(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    إذا(ftpc->wait_انکڑے_conn) {
      bool serv_conned;

      نتیجہ = Curl_conn_connect(انکڑے, SECONDARYSOCKET, FALSE, &serv_conned);
      إذا(نتیجہ)
        أرجع نتیجہ; /* Failed to accept data connection */

      إذا(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_انکڑے_conn = FALSE;
        نتیجہ = ftp_initiate_transfer(انکڑے, ftpc);

        إذا(نتیجہ)
          أرجع نتیجہ;

        *ہور = DOMORE_پورا; /* this state is now complete when the server has
                                connected back to us */
      }
      وإلا {
        نتیجہ = ftp_check_ctrl_on_انکڑے_wait(انکڑے, ftpc);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
    }
    وإلا إذا(انکڑے->state.upload) {
      نتیجہ = ftp_nb_type(انکڑے, ftpc, ftp, (bool)انکڑے->state.prefer_ascii,
                           FTP_STOR_TYPE);
      إذا(نتیجہ)
        أرجع نتیجہ;

      نتیجہ = ftp_statemach(انکڑے, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *ہور = (!ftpc->wait_انکڑے_conn && complete) ?
        DOMORE_پورا : DOMORE_INCOMPLETE;
    }
    وإلا {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      نتیجہ = Curl_range(انکڑے);

      إذا(نتیجہ == CURLE_OK && انکڑے->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      إذا(نتیجہ)
        أرجع نتیجہ;

      إذا((انکڑے->state.list_only || !ftpc->file) &&
         !انکڑے->سیٹ.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        إذا(ftp->transfer == PPTRANSFER_BODY) {
          نتیجہ = ftp_nb_type(انکڑے, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          إذا(نتیجہ)
            أرجع نتیجہ;
        }
        /* otherwise fall through */
      }
      وإلا {
        إذا(انکڑے->سیٹ.prequote && !ftpc->file) {
          نتیجہ = ftp_nb_type(انکڑے, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        وإلا {
          نتیجہ = ftp_nb_type(انکڑے, ftpc, ftp, (bool)انکڑے->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        إذا(نتیجہ)
          أرجع نتیجہ;
      }

      نتیجہ = ftp_statemach(انکڑے, ftpc, &complete);
      إذا(complete)
        *ہور = DOMORE_پورا;
    }
    أرجع نتیجہ;
  }

  /* no data to transfer */
  Curl_xfer_تیاری_nop(انکڑے);

  إذا(!ftpc->wait_انکڑے_conn) {
    /* no waiting for the data connection so this is now complete */
    *ہور = DOMORE_پورا;
    CURL_TRC_FTP(انکڑے, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (صحيح)نتیجہ);
  }

  أرجع نتیجہ;
}

/* call this when the DO phase has completed */
ساكن CURLcode ftp_dophase_پورا(بنية Curl_easy *انکڑے,
                                 بنية ftp_conn *ftpc,
                                 بنية FTP *ftp,
                                 bool connected)
{
  إذا(connected) {
    domore completed;
    CURLcode نتیجہ = ftp_do_ہور(انکڑے, &completed);

    إذا(نتیجہ) {
      close_secondarysocket(انکڑے, ftpc);
      أرجع نتیجہ;
    }
  }

  إذا(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(انکڑے);
  وإلا إذا(!connected)
    /* since we did not connect now, we want do_more to get called */
    انکڑے->conn->بٹ.do_ہور = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_state_پورٹ_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = انکڑے->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode نتیجہ = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  إذا(ftpcode / 100 != 2) {
    /* the command failed */

    إذا(EPRT == fcmd) {
      infof(انکڑے, "disabling EPRT usage");
      conn->بٹ.ftp_use_eprt = FALSE;
    }
    fcmd++;

    إذا(fcmd == پورا_2) {
      failf(انکڑے, "Failed to do PORT");
      نتیجہ = CURLE_FTP_پورٹ_FAILED;
    }
    وإلا
      /* try next */
      نتیجہ = ftp_state_use_پورٹ(انکڑے, ftpc, fcmd);
  }
  وإلا {
    infof(انکڑے, "Connect data stream actively");
    ftp_state(انکڑے, ftpc, FTP_STOP); /* end of DO phase */
    نتیجہ = ftp_dophase_پورا(انکڑے, ftpc, ftp, FALSE);
  }

  أرجع نتیجہ;
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
                           صحيح *hour, صحيح *minute, صحيح *دوجا);
UNITTEST bool ftp_213_date(ثابت حرف *p, صحيح *year, صحيح *month, صحيح *day,
                           صحيح *hour, صحيح *minute, صحيح *دوجا)
{
  صحيح century;
  إذا((لڑی_لمبائی(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], دوجا))
    أرجع FALSE;

  *year += century * 100;
  إذا((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*دوجا > 60))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode client_write_سرخی(بنية Curl_easy *انکڑے,
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
  CURLcode نتیجہ;
  bool save = (bool)انکڑے->سیٹ.include_سرخی;
  انکڑے->سیٹ.include_سرخی = TRUE;
  نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, buf, blen);
  انکڑے->سیٹ.include_سرخی = save;
  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_mdtm_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  بدل(ftpcode) {
  حالة 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    صحيح year, month, day, hour, minute, دوجا;
    بنية pingpong *pp = &ftpc->pp;
    ثابت حرف *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    إذا(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &دوجا)) {
      /* we have a time, reformat it */
      حرف timebuf[24];
      curl_msnprintf(timebuf, حجم(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, دوجا);
      /* now, convert this into a time() value: */
      إذا(!Curl_getdate_capped(timebuf, &انکڑے->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ہدایت GCC diagnostic ڈالو
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#ہدایت GCC diagnostic ignored "-Wtype-limits"
#جے_انت
    إذا(انکڑے->req.no_body && ftpc->file &&
       انکڑے->سیٹ.get_filetime && showtime) {
#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ہدایت GCC diagnostic نکالو
#جے_انت
      حرف headerbuf[128];
      صحيح headerbuflen;
      time_t filetime = انکڑے->info.filetime;
      بنية tm بفر;
      ثابت بنية tm *tm = &بفر;

      نتیجہ = curlx_gmtime(filetime, &بفر);
      إذا(نتیجہ)
        أرجع نتیجہ;

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
      نتیجہ = client_write_سرخی(انکڑے, headerbuf, headerbuflen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    } /* end of a ridiculous amount of conditionals */
  }
    اكسر;
  افتراضي:
    infof(انکڑے, "unsupported MDTM reply format");
    اكسر;
  حالة 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(انکڑے, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    اكسر;
  }

  إذا(انکڑے->سیٹ.timecondition) {
    إذا((انکڑے->info.filetime > 0) && (انکڑے->سیٹ.timevalue > 0)) {
      بدل(انکڑے->سیٹ.timecondition) {
      حالة CURL_TIMECOND_IFMODSINCE:
      افتراضي:
        إذا(انکڑے->info.filetime <= انکڑے->سیٹ.timevalue) {
          infof(انکڑے, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          انکڑے->info.timecond = TRUE;
          ftp_state(انکڑے, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      حالة CURL_TIMECOND_IFUNMODSINCE:
        إذا(انکڑے->info.filetime > انکڑے->سیٹ.timevalue) {
          infof(انکڑے, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          انکڑے->info.timecond = TRUE;
          ftp_state(انکڑے, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      } /* switch */
    }
    وإلا {
      infof(انکڑے, "Skipping time comparison");
    }
  }

  إذا(!نتیجہ)
    نتیجہ = ftp_state_type(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_type_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(انکڑے, "Could not set desired mode");
    أرجع CURLE_FTP_COULDNT_سیٹ_TYPE;
  }
  إذا(ftpcode != 200)
    infof(انکڑے, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  إذا(instate == FTP_TYPE)
    نتیجہ = ftp_state_ناپ(انکڑے, ftpc, ftp);
  وإلا إذا(instate == FTP_LIST_TYPE)
    نتیجہ = ftp_state_list(انکڑے, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_TYPE)
    نتیجہ = ftp_state_retr_prequote(انکڑے, ftpc, ftp);
  وإلا إذا(instate == FTP_STOR_TYPE)
    نتیجہ = ftp_state_stor_prequote(انکڑے, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_LIST_TYPE)
    نتیجہ = ftp_state_list_prequote(انکڑے, ftpc, ftp);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_ناپ_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t filesize = -1;
  ثابت حرف *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  إذا(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ثابت حرف *شروع = &buf[4];
    ثابت حرف *fdigit = memchr(شروع, '\r', len - 4);
    إذا(fdigit) {
      fdigit--;
      إذا(*fdigit == '\n')
        fdigit--;
      طالما(ISDIGIT(fdigit[-1]) && (fdigit > شروع))
        fdigit--;
    }
    وإلا
      fdigit = شروع;
    إذا(curlx_str_نمبر(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  وإلا إذا(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_ناپ) {
    failf(انکڑے, "The file does not exist");
    أرجع CURLE_REMOTE_FILE_NOT_لبھیا;
  }

  إذا(instate == FTP_ناپ) {
    إذا(filesize != -1) {
      حرف clbuf[128];
      صحيح clbuflen = curl_msnprintf(clbuf, حجم(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      نتیجہ = client_write_سرخی(انکڑے, clbuf, clbuflen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, filesize);
    نتیجہ = ftp_state_rest(انکڑے, ftpc, ftp);
  }
  وإلا إذا(instate == FTP_RETR_ناپ) {
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, filesize);
    نتیجہ = ftp_state_retr(انکڑے, ftpc, ftp, filesize);
  }
  وإلا إذا(instate == FTP_STOR_ناپ) {
    انکڑے->state.resume_from = filesize;
    نتیجہ = ftp_state_ul_تیاری(انکڑے, ftpc, ftp, TRUE);
  }

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_rest_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  بدل(instate) {
  حالة FTP_REST:
  افتراضي:
    إذا(ftpcode == 350) {
      حرف بفر[24] = { "Accept-ranges: bytes\r\n" };
      نتیجہ = client_write_سرخی(انکڑے, بفر, لڑی_لمبائی(بفر));
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    نتیجہ = ftp_state_prepare_transfer(انکڑے, ftpc, ftp);
    اكسر;

  حالة FTP_RETR_REST:
    إذا(ftpcode != 350) {
      failf(انکڑے, "Could not use REST");
      نتیجہ = CURLE_FTP_COULDNT_USE_REST;
    }
    وإلا {
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "RETR %s", ftpc->file);
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_RETR);
    }
    اكسر;
  }

  أرجع نتیجہ;
}

ساكن CURLcode ftp_state_stor_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(ftpcode >= 400) {
    failf(انکڑے, "Failed FTP upload: %0d", ftpcode);
    ftp_state(انکڑے, ftpc, FTP_STOP);
    أرجع CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  إذا(انکڑے->سیٹ.ftp_use_پورٹ) {
    bool connected;

    ftp_state(انکڑے, ftpc, FTP_STOP); /* no longer in STOR state */

    نتیجہ = Curl_conn_connect(انکڑے, SECONDARYSOCKET, FALSE, &connected);
    إذا(نتیجہ)
      أرجع نتیجہ;

    إذا(!connected) {
      infof(انکڑے, "Data conn was not available immediately");
      ftpc->wait_انکڑے_conn = TRUE;
      أرجع ftp_check_ctrl_on_انکڑے_wait(انکڑے, ftpc);
    }
    ftpc->wait_انکڑے_conn = FALSE;
  }
  أرجع ftp_initiate_transfer(انکڑے, ftpc);
}

/* for LIST and RETR responses */
ساكن CURLcode ftp_state_get_resp(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   صحيح ftpcode,
                                   ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

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

    انکڑے->req.ناپ = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    إذا((instate != FTP_LIST) &&
       !انکڑے->state.prefer_ascii &&
       !انکڑے->سیٹ.ignorecl &&
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
          إذا(!curlx_str_نمبر(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !لڑی_مقابلہ(c, "bytes", 5)) {
            انکڑے->req.ناپ = what;
            اكسر;
          }
        }
      }
    }
    وإلا إذا(ftp->downloadsize > -1)
      انکڑے->req.ناپ = ftp->downloadsize;

    إذا(انکڑے->req.ناپ > انکڑے->req.maxdownload && انکڑے->req.maxdownload > 0)
      انکڑے->req.ناپ = انکڑے->req.maxdownload;
    وإلا إذا((instate != FTP_LIST) && انکڑے->state.prefer_ascii)
      انکڑے->req.ناپ = -1; /* for servers that understate ASCII mode file
ناپ*/

    infof(انکڑے, "Maxdownload = %" FMT_OFF_T, انکڑے->req.maxdownload);

    إذا(instate != FTP_LIST)
      infof(انکڑے, "Getting file with size: %" FMT_OFF_T, انکڑے->req.ناپ);

    إذا(انکڑے->سیٹ.ftp_use_پورٹ) {
      bool connected;

      نتیجہ = Curl_conn_connect(انکڑے, SECONDARYSOCKET, FALSE, &connected);
      إذا(نتیجہ)
        أرجع نتیجہ;

      إذا(!connected) {
        infof(انکڑے, "Data conn was not available immediately");
        ftp_state(انکڑے, ftpc, FTP_STOP);
        ftpc->wait_انکڑے_conn = TRUE;
        أرجع ftp_check_ctrl_on_انکڑے_wait(انکڑے, ftpc);
      }
      ftpc->wait_انکڑے_conn = FALSE;
    }
    أرجع ftp_initiate_transfer(انکڑے, ftpc);
  }
  وإلا {
    إذا((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(انکڑے, ftpc, FTP_STOP); /* this phase is over */
    }
    وإلا {
      failf(انکڑے, "RETR response: %03d", ftpcode);
      أرجع instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_لبھیا :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  أرجع نتیجہ;
}

/* after USER, PASS and ACCT */
ساكن CURLcode ftp_state_loggedin(بنية Curl_easy *انکڑے,
                                   بنية ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET)) {
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
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "PBSZ %d", 0);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_PBSZ);
  }
  وإلا {
    نتیجہ = ftp_state_pwd(انکڑے, ftpc);
  }
  أرجع نتیجہ;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ساكن bool ftp_has_ctrl(ثابت حرف *string)
{
  ثابت بنانشانی حرف *s = (ثابت بنانشانی حرف *)string;
  طالما(*s) {
    إذا(*s < 0x20)
      أرجع TRUE;
    s++;
  }
  أرجع FALSE;
}

/* for USER and PASS responses */
ساكن CURLcode ftp_state_user_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  إذا((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(انکڑے->conn->creds));
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_PASS);
  }
  وإلا إذا(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    نتیجہ = ftp_state_loggedin(انکڑے, ftpc);
  }
  وإلا إذا(ftpcode == 332) {
    ثابت حرف *account = CURL_EASY_STR(انکڑے, STRING_FTP_ACCOUNT);
    إذا(!account) {
      failf(انکڑے, "ACCT requested but none available");
      نتیجہ = CURLE_LOGIN_DENIED;
    }
    وإلا إذا(ftp_has_ctrl(account)) {
      failf(انکڑے, "Control byte in FTP account");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    }
    وإلا {
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "ACCT %s", account);
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_ACCT);
    }
  }
  وإلا {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ثابت حرف *alt = CURL_EASY_STR(انکڑے, STRING_FTP_ALTERNATIVE_منزل_USER);
    إذا(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      إذا(ftp_has_ctrl(alt)) {
        failf(انکڑے, "Control byte in FTP alternative-to-user command");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      }
      وإلا {
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", alt);
        إذا(!نتیجہ) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(انکڑے, ftpc, FTP_USER);
        }
      }
    }
    وإلا {
      failf(انکڑے, "Access denied: %03d", ftpcode);
      نتیجہ = CURLE_LOGIN_DENIED;
    }
  }
  أرجع نتیجہ;
}

/* for ACCT response */
ساكن CURLcode ftp_state_acct_resp(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(ftpcode != 230) {
    failf(انکڑے, "ACCT rejected by server: %03d", ftpcode);
    نتیجہ = CURLE_FTP_WEIRD_PASS_جواب; /* FIX */
  }
  وإلا
    نتیجہ = ftp_state_loggedin(انکڑے, ftpc);

  أرجع نتیجہ;
}

ساكن CURLcode ftp_pwd_resp(بنية Curl_easy *انکڑے,
                             بنية ftp_conn *ftpc,
                             صحيح ftpcode)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode نتیجہ;

  إذا(ftpcode == 257) {
    ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool اندراج_extracted = FALSE;
    بنية dynbuf باہرلا;
    curlx_dyn_init(&باہرلا, 1000);

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
            نتیجہ = curlx_dyn_addn(&باہرلا, &ptr[1], 1);
            ptr++;
          }
          وإلا {
            /* end of path */
            إذا(curlx_dyn_len(&باہرلا))
              اندراج_extracted = TRUE;
            اكسر; /* get out of this loop */
          }
        }
        وإلا {
          إذا(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&باہرلا);
            أرجع CURLE_WEIRD_SERVER_جواب;
          }
          نتیجہ = curlx_dyn_addn(&باہرلا, ptr, 1);
        }
        إذا(نتیجہ) {
          curlx_dyn_free(&باہرلا);
          أرجع نتیجہ;
        }
      }
    }
    إذا(اندراج_extracted) {
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
      حرف *dir = curlx_dyn_ptr(&باہرلا);

      إذا(!ftpc->server_os && dir[0] != '/') {
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "SYST");
        إذا(نتیجہ) {
          curlx_dyn_free(&باہرلا);
          أرجع نتیجہ;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(انکڑے, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(انکڑے->state.most_recent_ftp_entrypath);
      انکڑے->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      إذا(!انکڑے->state.most_recent_ftp_entrypath)
        أرجع CURLE_باہرلا_دا_حافظہ;

      إذا(!ftpc->server_os && dir[0] != '/') {
        ftp_state(انکڑے, ftpc, FTP_SYST);
        أرجع CURLE_OK;
      }
    }
    وإلا {
      /* could not get the path */
      curlx_dyn_free(&باہرلا);
      infof(انکڑے, "Failed to figure out path");
    }
  }
  ftp_state(انکڑے, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(انکڑے, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  أرجع CURLE_OK;
}

ساكن ثابت حرف * ثابت ftpauth[] = { "SSL", "TLS" };

ساكن CURLcode ftp_wait_resp(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn,
                              بنية ftp_conn *ftpc,
                              صحيح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    إذا(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      أرجع ftp_state_user_resp(انکڑے, ftpc, ftpcode);
  }
  وإلا إذا(ftpcode != 220) {
    failf(انکڑے, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  إذا(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    بدل((لما)انکڑے->سیٹ.ftpsslauth) {
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
      failf(انکڑے, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (صحيح)انکڑے->سیٹ.ftpsslauth);
      أرجع CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_AUTH);
  }
  وإلا
    نتیجہ = ftp_state_user(انکڑے, ftpc, conn);
  أرجع نتیجہ;
}

/* an implicit TLS scheme implies TLS on both connections */
ساكن بنانشانی حرف ftp_use_ssl(بنية connectdata *conn,
                                 بنية ftp_conn *ftpc)
{
  إذا(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    أرجع CURLUSESSL_ALL;
  أرجع ftpc->use_ssl;
}

ساكن CURLcode ftp_pp_statemachine(بنية Curl_easy *انکڑے,
                                    بنية connectdata *conn)
{
  CURLcode نتیجہ;
  صحيح ftpcode;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(انکڑے, CURL_META_FTP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(انکڑے, pp);

  نتیجہ = ftp_readresp(انکڑے, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  إذا(نتیجہ || !ftpcode)
    أرجع نتیجہ;

  /* we have now received a full FTP server response */
  بدل(ftpc->state) {
  حالة FTP_WAIT220:
    نتیجہ = ftp_wait_resp(انکڑے, conn, ftpc, ftpcode);
    اكسر;

  حالة FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    إذا(pp->حد_توں_ودھ)
      أرجع CURLE_WEIRD_SERVER_جواب; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    إذا((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool پورا;
      إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        نتیجہ = Curl_ssl_cfilter_جوڑو(
          انکڑے, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        إذا(نتیجہ) {
          /* we failed and bail out */
          أرجع CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, TRUE, &پورا);
      إذا(!نتیجہ) {
        conn->بٹ.ftp_use_انکڑے_ssl = FALSE; /* clear-text data */
        نتیجہ = ftp_state_user(انکڑے, ftpc, conn);
      }
    }
    وإلا إذا(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    وإلا {
      إذا(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        نتیجہ = CURLE_USE_SSL_FAILED;
      وإلا
        /* ignore the failure and continue */
        نتیجہ = ftp_state_user(انکڑے, ftpc, conn);
    }
    اكسر;

  حالة FTP_USER:
  حالة FTP_PASS:
    نتیجہ = ftp_state_user_resp(انکڑے, ftpc, ftpcode);
    اكسر;

  حالة FTP_ACCT:
    نتیجہ = ftp_state_acct_resp(انکڑے, ftpc, ftpcode);
    اكسر;

  حالة FTP_PBSZ:
    نتیجہ =
      Curl_pp_sendf(انکڑے, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    إذا(!نتیجہ)
      ftp_state(انکڑے, ftpc, FTP_PROT);
    اكسر;

  حالة FTP_PROT:
    إذا(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->بٹ.ftp_use_انکڑے_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    وإلا إذا(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      أرجع CURLE_USE_SSL_FAILED;

    إذا(انکڑے->سیٹ.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "CCC");
      إذا(!نتیجہ)
        ftp_state(انکڑے, ftpc, FTP_CCC);
    }
    وإلا
      نتیجہ = ftp_state_pwd(انکڑے, ftpc);
    اكسر;

  حالة FTP_CCC:
    إذا(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      نتیجہ = Curl_ssl_cfilter_remove(انکڑے, FIRSTSOCKET,
                                       (انکڑے->سیٹ.ftp_ccc ==
                                        (بنانشانی حرف)CURLFTPSSL_CCC_سرگرم));
      إذا(نتیجہ)
        failf(انکڑے, "Failed to clear the command channel (CCC)");
    }
    إذا(!نتیجہ)
      /* Then continue as normal */
      نتیجہ = ftp_state_pwd(انکڑے, ftpc);
    اكسر;

  حالة FTP_PWD:
    نتیجہ = ftp_pwd_resp(انکڑے, ftpc, ftpcode);
    اكسر;

  حالة FTP_SYST:
    إذا(ftpcode == 215) {
      ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ثابت حرف *شروع;
      حرف *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      طالما(*ptr == ' ')
        ptr++;
      لكل(شروع = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(شروع, ptr - شروع);
      إذا(!os)
        أرجع CURLE_باہرلا_دا_حافظہ;

      /* Check for special servers here. */
      إذا(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        إذا(نتیجہ) {
          curlx_free(os);
          أرجع نتیجہ;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(انکڑے, ftpc, FTP_NAMEFMT);
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

    ftp_state(انکڑے, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(انکڑے, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_NAMEFMT:
    إذا(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(انکڑے, ftpc);
      اكسر;
    }

    ftp_state(انکڑے, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(انکڑے, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_QUOTE:
  حالة FTP_POSTQUOTE:
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    إذا((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(انکڑے, "QUOT command failed with %03d", ftpcode);
      نتیجہ = CURLE_QUOTE_ERROR;
    }
    وإلا
      نتیجہ = ftp_state_quote(انکڑے, ftpc, ftp, FALSE, ftpc->state);
    اكسر;

  حالة FTP_CWD:
    إذا(ftpcode / 100 != 2) {
      /* failure to CWD there */
      إذا(انکڑے->سیٹ.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (انکڑے->سیٹ.ftp_create_missing_dirs == 2) ? 1 : 0;

        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        إذا(!نتیجہ)
          ftp_state(انکڑے, ftpc, FTP_MKD);
      }
      وإلا {
        /* return failure */
        failf(انکڑے, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    وإلا {
      /* success */
      ftpc->count2 = 0;
      إذا(ftpc->cwdcount >= ftpc->dirdepth)
        نتیجہ = ftp_state_mdtm(انکڑے, ftpc, ftp);
      وإلا {
        ftpc->cwdcount++;
        /* send next CWD */
        نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    اكسر;

  حالة FTP_MKD:
    إذا((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(انکڑے, "Failed to MKD dir: %03d", ftpcode);
      نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    }
    وإلا {
      ftp_state(انکڑے, ftpc, FTP_CWD);
      /* send CWD */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    اكسر;

  حالة FTP_MDTM:
    نتیجہ = ftp_state_mdtm_resp(انکڑے, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_TYPE:
  حالة FTP_LIST_TYPE:
  حالة FTP_RETR_TYPE:
  حالة FTP_STOR_TYPE:
  حالة FTP_RETR_LIST_TYPE:
    نتیجہ = ftp_state_type_resp(انکڑے, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_ناپ:
  حالة FTP_RETR_ناپ:
  حالة FTP_STOR_ناپ:
    نتیجہ = ftp_state_ناپ_resp(انکڑے, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_REST:
  حالة FTP_RETR_REST:
    نتیجہ = ftp_state_rest_resp(انکڑے, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_PRET:
    إذا(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(انکڑے, "PRET command not accepted: %03d", ftpcode);
      أرجع CURLE_FTP_PRET_FAILED;
    }
    نتیجہ = ftp_state_use_pasv(انکڑے, ftpc, conn);
    اكسر;

  حالة FTP_PASV:
    نتیجہ = ftp_state_pasv_resp(انکڑے, ftpc, ftpcode);
    اكسر;

  حالة FTP_پورٹ:
    نتیجہ = ftp_state_پورٹ_resp(انکڑے, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_LIST:
  حالة FTP_RETR:
    نتیجہ = ftp_state_get_resp(انکڑے, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_STOR:
    نتیجہ = ftp_state_stor_resp(انکڑے, ftpc, ftpcode);
    اكسر;

  حالة FTP_QUIT:
  افتراضي:
    /* internal error */
    ftp_state(انکڑے, ftpc, FTP_STOP);
    اكسر;
  }

  أرجع نتیجہ;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_multi_statemach(بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? ftp_statemach(انکڑے, ftpc, پورا) : CURLE_FAILED_INIT;
}

ساكن CURLcode ftp_بلاک_statemach(بنية Curl_easy *انکڑے,
                                    بنية ftp_conn *ftpc)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode نتیجہ = CURLE_OK;

  طالما(ftpc->state != FTP_STOP) {
    إذا(ftpc->shutdown)
      CURL_TRC_FTP(انکڑے, "in shutdown, waiting for server response");
    نتیجہ = Curl_pp_statemach(انکڑے, pp, TRUE, TRUE /* disconnecting */);
    إذا(نتیجہ)
      اكسر;
  }

  أرجع نتیجہ;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ساكن CURLcode ftp_connect(بنية Curl_easy *انکڑے,
                            bool *پورا) /* see description above */
{
  CURLcode نتیجہ;
  بنية connectdata *conn = انکڑے->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp;

  *پورا = FALSE; /* default to not done yet */
  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_تیاری(pp, ftp_pp_statemachine, ftp_endofresp);

  إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, TRUE, پورا);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  Curl_pp_init(pp, Curl_pgrs_now(انکڑے)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(انکڑے, ftpc, FTP_WAIT220);

  نتیجہ = ftp_statemach(انکڑے, ftpc, پورا);

  أرجع نتیجہ;
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
ساكن CURLcode ftp_sendquote(بنية Curl_easy *انکڑے,
                              بنية ftp_conn *ftpc,
                              بنية curl_slist *quote)
{
  بنية curl_slist *شے;
  بنية pingpong *pp = &ftpc->pp;

  شے = quote;
  طالما(شے) {
    إذا(شے->انکڑے) {
      size_t nread;
      ثابت حرف *cmd = شے->انکڑے;
      bool acceptfail = FALSE;
      CURLcode نتیجہ;
      صحيح ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      إذا(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", cmd);
      إذا(!نتیجہ) {
        pp->response = *Curl_pgrs_now(انکڑے); /* timeout relative now */
        نتیجہ = getftpresponse(انکڑے, &nread, &ftpcode);
      }
      إذا(نتیجہ)
        أرجع نتیجہ;

      إذا(!acceptfail && (ftpcode >= 400)) {
        failf(انکڑے, "QUOT string not accepted: %s", cmd);
        أرجع CURLE_QUOTE_ERROR;
      }
    }

    شے = شے->اگلا;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_پورا_حالت(بنية Curl_easy *انکڑے,
                                بنية ftp_conn *ftpc, CURLcode حالت,
                                bool premature)
{
  بدل(حالت) {
  حالة CURLE_BAD_DOWNLOAD_RESUME:
  حالة CURLE_FTP_WEIRD_PASV_جواب:
  حالة CURLE_FTP_پورٹ_FAILED:
  حالة CURLE_FTP_ACCEPT_FAILED:
  حالة CURLE_FTP_ACCEPT_TIMEOUT:
  حالة CURLE_FTP_COULDNT_سیٹ_TYPE:
  حالة CURLE_FTP_COULDNT_RETR_FILE:
  حالة CURLE_PARTIAL_FILE:
  حالة CURLE_UPLOAD_FAILED:
  حالة CURLE_REMOTE_ACCESS_DENIED:
  حالة CURLE_FILESIZE_EXCEEDED:
  حالة CURLE_REMOTE_FILE_NOT_لبھیا:
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
    CURL_TRC_FTP(انکڑے, "FTP ended with bad error code");
    connclose(انکڑے->conn);
    أرجع حالت;      /* use the already set error code */
  }
  أرجع CURLE_OK;
}

ساكن فراغ ftp_پورا_wildcard(بنية Curl_easy *انکڑے, بنية ftp_conn *ftpc)
{
  إذا(انکڑے->state.wildcardmatch) {
    إذا(انکڑے->سیٹ.chunk_سرا && ftpc->file) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, easy_chunk_سرا);
      انکڑے->سیٹ.chunk_سرا(انکڑے->سیٹ.wildcardptr);
      CURL_CBAPI_سرا(&guard);
      freedirs(ftpc);
    }
    ftpc->معلوم_filesize = -1;
  }
}

ساكن فراغ ftp_پورا_رستہ(بنية Curl_easy *انکڑے, بنية ftp_conn *ftpc,
                          CURLcode نتیجہ)
{
  بنية connectdata *conn = انکڑے->conn;
  إذا(نتیجہ) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  وإلا { /* remember working directory for connection reuse */
    ثابت حرف *raw_رستہ = ftpc->rawpath;
    إذا(raw_رستہ) {
      إذا((انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && (raw_رستہ[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      وإلا {
        size_t رستہ_Len = لڑی_لمبائی(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        إذا(!ftpc->cwdfail) {
          إذا(انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
            رستہ_Len = 0; /* relative path => working directory is FTP home */
          وإلا
            /* file is URL-decoded */
            رستہ_Len -= ftpc->file ? لڑی_لمبائی(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_رستہ, رستہ_Len);
        }
        وإلا
          ftpc->prevpath = عدم; /* no path */
      }
    }
    إذا(ftpc->prevpath)
      infof(انکڑے, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ساكن CURLcode ftp_پورا_secondary_socket(بنية Curl_easy *انکڑے,
                                          بنية ftp_conn *ftpc,
                                          CURLcode نتیجہ)
{
  بنية connectdata *conn = انکڑے->conn;
  إذا(Curl_conn_is_تیاری(conn, SECONDARYSOCKET)) {
    إذا(!نتیجہ && ftpc->dont_check && انکڑے->req.maxdownload > 0) {
      /* partial download completed */
      نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "ABOR");
      إذا(نتیجہ) {
        failf(انکڑے, "Failure sending ABOR command: %s",
              curl_easy_strerror(نتیجہ));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(انکڑے, ftpc);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ftp_پورا_control_جواب(بنية Curl_easy *انکڑے,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode نتیجہ,
                                       bool premature)
{
  بنية connectdata *conn = انکڑے->conn;
  size_t nread;
  صحيح ftpcode;

  إذا(!نتیجہ && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(انکڑے); /* timeout relative now */
    نتیجہ = getftpresponse(انکڑے, &nread, &ftpcode);

    إذا(!nread && (نتیجہ == CURLE_OPERATION_TIMEDOUT)) {
      failf(انکڑے, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    إذا(نتیجہ)
      أرجع نتیجہ;

    إذا(ftpc->dont_check && انکڑے->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(انکڑے, "partial download completed, closing connection");
      connclose(conn);
      أرجع نتیجہ;
    }

    إذا(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      بدل(ftpcode) {
      حالة 226:
      حالة 250:
        اكسر;
      حالة 552:
        failf(انکڑے, "Exceeded storage allocation");
        نتیجہ = CURLE_REMOTE_DISK_بھریا;
        اكسر;
      افتراضي:
        failf(انکڑے, "server did not report OK, got %d", ftpcode);
        نتیجہ = CURLE_PARTIAL_FILE;
        اكسر;
      }
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode ftp_پورا_check_partial(بنية Curl_easy *انکڑے,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode نتیجہ,
                                       bool premature)
{
  إذا(نتیجہ || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  وإلا إذا(انکڑے->state.upload) {
    إذا((ftp->transfer == PPTRANSFER_BODY) &&
       (انکڑے->state.infilesize != -1) && /* upload with known size */
       ((!انکڑے->سیٹ.crlf && !انکڑے->state.prefer_ascii && /* no conversion */
         (انکڑے->state.infilesize != انکڑے->req.writebytecount)) ||
        ((انکڑے->سیٹ.crlf || انکڑے->state.prefer_ascii) && /* maybe CRLF conv */
         (انکڑے->state.infilesize > انکڑے->req.writebytecount))
       )) {
      failf(انکڑے, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            انکڑے->req.writebytecount, انکڑے->state.infilesize);
      نتیجہ = CURLE_PARTIAL_FILE;
    }
  }
  وإلا {
    إذا((انکڑے->req.ناپ != -1) &&
       (انکڑے->req.ناپ != انکڑے->req.bytecount) &&
       (انکڑے->req.maxdownload != انکڑے->req.bytecount)) {
      failf(انکڑے, "Received only partial file: %" FMT_OFF_T " bytes",
            انکڑے->req.bytecount);
      نتیجہ = CURLE_PARTIAL_FILE;
    }
    وإلا إذا(!ftpc->dont_check &&
            !انکڑے->req.bytecount &&
            (انکڑے->req.ناپ > 0)) {
      failf(انکڑے, "No data was received");
      نتیجہ = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  أرجع نتیجہ;
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
ساكن CURLcode ftp_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                         bool premature)
{
  بنية FTP *ftp = Curl_meta_get(انکڑے, CURL_META_FTP_EASY);
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  CURLcode نتیجہ;

  إذا(!ftp || !ftpc)
    أرجع CURLE_OK;

  نتیجہ = ftp_پورا_حالت(انکڑے, ftpc, حالت, premature);

  ftp_پورا_wildcard(انکڑے, ftpc);
  ftp_پورا_رستہ(انکڑے, ftpc, نتیجہ);
  نتیجہ = ftp_پورا_secondary_socket(انکڑے, ftpc, نتیجہ);
  نتیجہ = ftp_پورا_control_جواب(انکڑے, ftpc, ftp, نتیجہ, premature);
  نتیجہ = ftp_پورا_check_partial(انکڑے, ftpc, ftp, نتیجہ, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  إذا(!حالت && !نتیجہ && !premature && انکڑے->سیٹ.postquote)
    نتیجہ = ftp_sendquote(انکڑے, ftpc, انکڑے->سیٹ.postquote);
  CURL_TRC_FTP(انکڑے, "[%s] done, result=%d", FTP_CSTATE(ftpc), (صحيح)نتیجہ);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ساكن CURLcode ftp_nb_type(بنية Curl_easy *انکڑے,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode نتیجہ;
  حرف want = (حرف)(ascii ? 'A' : 'I');

  إذا(ftpc->transfertype == want) {
    ftp_state(انکڑے, ftpc, newstate);
    أرجع ftp_state_type_resp(انکڑے, ftpc, ftp, 200, newstate);
  }

  نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "TYPE %c", want);
  إذا(!نتیجہ) {
    ftp_state(انکڑے, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ساكن CURLcode ftp_perform(
  بنية Curl_easy *انکڑے,
  بنية ftp_conn *ftpc,
  بنية FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_پورا)
{
  /* this is FTP and no proxy */
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_FTP(انکڑے, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  إذا(انکڑے->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_پورا = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  نتیجہ = ftp_state_quote(انکڑے, ftpc, ftp, TRUE, FTP_QUOTE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* run the state-machine */
  نتیجہ = ftp_statemach(انکڑے, ftpc, dophase_پورا);

  *connected = Curl_conn_is_connected(انکڑے->conn, SECONDARYSOCKET);

  إذا(*connected)
    infof(انکڑے, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  وإلا
    CURL_TRC_FTP(انکڑے, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  إذا(*dophase_پورا)
    CURL_TRC_FTP(انکڑے, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  أرجع نتیجہ;
}

ساكن فراغ wc_انکڑے_dtor(فراغ *ptr)
{
  بنية ftp_wc *ftpwc = ptr;
  إذا(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_انکڑے_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ساكن CURLcode init_wc_انکڑے(بنية Curl_easy *انکڑے,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  حرف *آخری_slash;
  حرف *رستہ = ftp->رستہ;
  بنية Wildcard_انکڑے *wildcard = انکڑے->wildcard;
  CURLcode نتیجہ = CURLE_OK;
  بنية ftp_wc *ftpwc = عدم;

  آخری_slash = strrchr(ftp->رستہ, '/');
  إذا(آخری_slash) {
    آخری_slash++;
    إذا(آخری_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_رستہ(انکڑے, ftpc, ftp);
    }
    wildcard->نمونہ = curlx_strdup(آخری_slash);
    إذا(!wildcard->نمونہ)
      أرجع CURLE_باہرلا_دا_حافظہ;
    آخری_slash[0] = '\0'; /* cut file from path */
  }
  وإلا { /* there is only 'wildcard pattern' or nothing */
    إذا(رستہ[0]) {
      wildcard->نمونہ = curlx_strdup(رستہ);
      إذا(!wildcard->نمونہ)
        أرجع CURLE_باہرلا_دا_حافظہ;
      رستہ[0] = '\0';
    }
    وإلا { /* only list */
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_رستہ(انکڑے, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, حجم(بنية ftp_wc));
  إذا(!ftpwc) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_انکڑے_alloc();
  إذا(!ftpwc->parser) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_انکڑے_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  إذا(انکڑے->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
    انکڑے->سیٹ.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  نتیجہ = ftp_parse_url_رستہ(انکڑے, ftpc, ftp);
  إذا(نتیجہ) {
    اذهب fail;
  }

  wildcard->رستہ = curlx_strdup(ftp->رستہ);
  إذا(!wildcard->رستہ) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  infof(انکڑے, "Wildcard - Parsing started");
  أرجع CURLE_OK;

fail:
  إذا(ftpwc) {
    Curl_ftp_parselist_انکڑے_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->نمونہ);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = عدم;
  أرجع نتیجہ;
}

ساكن CURLcode wc_statemach(بنية Curl_easy *انکڑے,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  بنية Wildcard_انکڑے * ثابت wildcard = انکڑے->wildcard;
  CURLcode نتیجہ = CURLE_OK;

  لكل(;;) {
    بدل(wildcard->state) {
    حالة CURLWC_INIT:
      نتیجہ = init_wc_انکڑے(انکڑے, ftpc, ftp);
      إذا(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        أرجع نتیجہ;
      wildcard->state = نتیجہ ? CURLWC_ERROR : CURLWC_MATCHING;
      أرجع نتیجہ;

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
      إذا(Curl_llist_گنتی(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        أرجع CURLE_REMOTE_FILE_NOT_لبھیا;
      }
      استمر;
    }

    حالة CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      بنية Curl_llist_گرہ *سر = Curl_llist_سر(&wildcard->filelist);
      بنية curl_fileinfo *finfo = Curl_گرہ_elem(سر);
      حرف *tmp_رستہ;
      حرف *enc = curl_easy_escape(عدم, finfo->filename, 0);
      إذا(!enc)
        أرجع CURLE_باہرلا_دا_حافظہ;

      tmp_رستہ = curl_maprintf("%s%s", wildcard->رستہ, enc);
      curl_free(enc);
      إذا(!tmp_رستہ)
        أرجع CURLE_باہرلا_دا_حافظہ;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->رستہ = tmp_رستہ;

      infof(انکڑے, "Wildcard - START of \"%s\"", finfo->filename);
      إذا(انکڑے->سیٹ.chunk_bgn) {
        لما userresponse;
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_شروع(&guard, انکڑے, easy_chunk_bgn);
        userresponse = انکڑے->سیٹ.chunk_bgn(
          finfo, انکڑے->سیٹ.wildcardptr,
          (صحيح)Curl_llist_گنتی(&wildcard->filelist));
        CURL_CBAPI_سرا(&guard);
        بدل(userresponse) {
        حالة CURL_CHUNK_BGN_FUNC_SKIP:
          infof(انکڑے, "Wildcard - \"%s\" skipped by user", finfo->filename);
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

      إذا(finfo->flags & CURLFINFOFLAG_معلوم_ناپ)
        ftpc->معلوم_filesize = finfo->ناپ;

      نتیجہ = ftp_parse_url_رستہ(انکڑے, ftpc, ftp);
      إذا(نتیجہ)
        أرجع نتیجہ;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_گرہ_remove(Curl_llist_سر(&wildcard->filelist));

      إذا(Curl_llist_گنتی(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        أرجع CURLE_OK;
      }
      أرجع نتیجہ;
    }

    حالة CURLWC_SKIP: {
      إذا(انکڑے->سیٹ.chunk_سرا) {
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_شروع(&guard, انکڑے, easy_chunk_سرا);
        انکڑے->سیٹ.chunk_سرا(انکڑے->سیٹ.wildcardptr);
        CURL_CBAPI_سرا(&guard);
      }
      Curl_گرہ_remove(Curl_llist_سر(&wildcard->filelist));
      wildcard->state = (Curl_llist_گنتی(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      استمر;
    }

    حالة CURLWC_CLEAN: {
      بنية ftp_wc *ftpwc = wildcard->ftpwc;
      نتیجہ = CURLE_OK;
      إذا(ftpwc)
        نتیجہ = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = نتیجہ ? CURLWC_ERROR : CURLWC_پورا;
      أرجع نتیجہ;
    }

    حالة CURLWC_پورا:
    حالة CURLWC_ERROR:
    حالة CURLWC_CLEAR:
      إذا(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = عدم;
      }
      أرجع نتیجہ;
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
ساكن CURLcode ftp_regular_transfer(بنية Curl_easy *انکڑے,
                                     بنية ftp_conn *ftpc,
                                     بنية FTP *ftp,
                                     bool *dophase_پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;
  انکڑے->req.ناپ = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(انکڑے);

  ftpc->ctl_valid = TRUE; /* starts good */

  نتیجہ = ftp_perform(انکڑے, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_پورا); /* all commands in the DO-phase done? */

  إذا(!نتیجہ) {

    إذا(!*dophase_پورا)
      /* the DO phase has not completed yet */
      أرجع CURLE_OK;

    نتیجہ = ftp_dophase_پورا(انکڑے, ftpc, ftp, connected);

    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
    freedirs(ftpc);

  أرجع نتیجہ;
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
ساكن CURLcode ftp_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(انکڑے, CURL_META_FTP_EASY);

  *پورا = FALSE; /* default to false */
  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  ftpc->wait_انکڑے_conn = FALSE; /* default to no such wait */

#جے_تعریف_شدہ CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    بنية Curl_cwriter *ftp_lc_writer;

    نتیجہ = Curl_cwriter_create(&ftp_lc_writer, انکڑے, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    إذا(نتیجہ)
      أرجع نتیجہ;

    نتیجہ = Curl_cwriter_جوڑو(انکڑے, ftp_lc_writer);
    إذا(نتیجہ) {
      Curl_cwriter_free(انکڑے, ftp_lc_writer);
      أرجع نتیجہ;
    }
  }
#جے_انت /* CURL_PREFER_LF_LINEENDS */

  إذا(انکڑے->state.wildcardmatch) {
    نتیجہ = wc_statemach(انکڑے, ftpc, ftp);
    إذا(انکڑے->wildcard->state == CURLWC_SKIP ||
       انکڑے->wildcard->state == CURLWC_پورا) {
      /* do not call ftp_regular_transfer */
      أرجع CURLE_OK;
    }
    إذا(نتیجہ) /* error, loop or skipping the file */
      أرجع نتیجہ;
  }
  وإلا { /* no wildcard FSM needed */
    نتیجہ = ftp_parse_url_رستہ(انکڑے, ftpc, ftp);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  نتیجہ = ftp_regular_transfer(انکڑے, ftpc, ftp, پورا);

  أرجع نتیجہ;
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
ساكن CURLcode ftp_quit(بنية Curl_easy *انکڑے,
                         بنية ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(ftpc->ctl_valid) {
    CURL_TRC_FTP(انکڑے, "sending QUIT to close session");
    نتیجہ = Curl_pp_sendf(انکڑے, &ftpc->pp, "%s", "QUIT");
    إذا(نتیجہ) {
      failf(انکڑے, "Failure sending QUIT command: %s",
            curl_easy_strerror(نتیجہ));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(انکڑے->conn); /* mark for closure */
      ftp_state(انکڑے, ftpc, FTP_STOP);
      أرجع نتیجہ;
    }

    ftp_state(انکڑے, ftpc, FTP_QUIT);

    نتیجہ = ftp_بلاک_statemach(انکڑے, ftpc);
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode ftp_disconnect(بنية Curl_easy *انکڑے,
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
  إذا(dead_connection || Curl_pp_needs_flush(انکڑے, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (فراغ)ftp_quit(انکڑے, ftpc); /* ignore errors on the QUIT */
  أرجع CURLE_OK;
}

/* called from multi.c while DOing */
ساكن CURLcode ftp_doing(بنية Curl_easy *انکڑے,
                          bool *dophase_پورا)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(انکڑے->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(انکڑے, CURL_META_FTP_EASY);
  CURLcode نتیجہ;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = ftp_statemach(انکڑے, ftpc, dophase_پورا);

  إذا(نتیجہ)
    CURL_TRC_FTP(انکڑے, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  وإلا إذا(*dophase_پورا) {
    نتیجہ = ftp_dophase_پورا(انکڑے, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(انکڑے, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  أرجع نتیجہ;
}

ساكن فراغ ftp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية FTP *ftp = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ساكن فراغ ftp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية ftp_conn *ftpc = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_منزل_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ساكن فراغ type_url_check(بنية Curl_easy *انکڑے, بنية FTP *ftp)
{
  size_t len = لڑی_لمبائی(ftp->رستہ);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  إذا((len >= 7) && !memcmp(&ftp->رستہ[len - 7], ";type=", 6)) {
    حرف *type = &ftp->رستہ[len - 7];
    حرف command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    بدل(command) {
    حالة 'A': /* ASCII mode */
      انکڑے->state.prefer_ascii = TRUE;
      اكسر;

    حالة 'D': /* directory mode */
      انکڑے->state.list_only = TRUE;
      اكسر;

    حالة 'I': /* binary mode */
    افتراضي:
      /* switch off ASCII */
      انکڑے->state.prefer_ascii = FALSE;
      اكسر;
    }
  }
}

ساكن CURLcode ftp_تیاری_connection(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn)
{
  بنية FTP *ftp;
  CURLcode نتیجہ = CURLE_OK;
  بنية ftp_conn *ftpc;

  ftp = curlx_calloc(1, حجم(*ftp));
  إذا(!ftp ||
     Curl_meta_سیٹ(انکڑے, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  ftpc = curlx_calloc(1, حجم(*ftpc));
  إذا(!ftpc ||
     Curl_conn_meta_سیٹ(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* clone connection related data that is FTP specific */
  إذا(CURL_EASY_STR(انکڑے, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(انکڑے, STRING_FTP_ACCOUNT));
    إذا(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }
  إذا(CURL_EASY_STR(انکڑے, STRING_FTP_ALTERNATIVE_منزل_USER)) {
    ftpc->alternative_منزل_user =
      curlx_strdup(CURL_EASY_STR(انکڑے, STRING_FTP_ALTERNATIVE_منزل_USER));
    إذا(!ftpc->alternative_منزل_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }

  ftp->رستہ = &انکڑے->state.اتے.رستہ[1]; /* do not include the initial slash */

  type_url_check(انکڑے, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->معلوم_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = انکڑے->سیٹ.use_ssl;
  ftpc->ccc = انکڑے->سیٹ.ftp_ccc;

  CURL_TRC_FTP(انکڑے, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (صحيح)نتیجہ);
  أرجع نتیجہ;
}

bool Curl_ftp_conns_میل(بنية connectdata *needle, بنية connectdata *conn)
{
  بنية ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  بنية ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  إذا(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_منزل_user,
                     cftpc->alternative_منزل_user) ||
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
  ftp_تیاری_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_پورا,                        /* done */
  ftp_do_ہور,                     /* do_more */
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

#جے_انت /* CURL_DISABLE_FTP */
