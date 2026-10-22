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

#اگر_تعریف_نشده CURL_DISABLE_FTP

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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

#اگر_تعریف_نشده NI_MAXHOST
#تعریف_کن NI_MAXHOST 1025
#پایان_اگر
#اگر_تعریف_نشده INET_ADDRSTRLEN
#تعریف_کن INET_ADDRSTRLEN 16
#پایان_اگر

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#تعریف_کن STATUSCODE(سطر) \
  (ISDIGIT((سطر)[0]) && ISDIGIT((سطر)[1]) && ISDIGIT((سطر)[2]))

/* macro to check for the last line in an FTP server response */
#تعریف_کن LASTLINE(سطر) (STATUSCODE(سطر) && (' ' == (سطر)[3]))

#اگر_تعریف_شده CURLVERBOSE
/* for tracing purposes */
ساكن ثابت حرف * ثابت ftp_state_نامها[] = {
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
#تعریف_کن FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_نامها[(ftpc)->state] : "???")

#پایان_اگر /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ساكن فراغ ftp_state_کم(بنية Curl_easy *داده,
                          بنية ftp_conn *ftpc,
                          ftpstate newstate
#اگر_تعریف_شده DEBUGBUILD
                          , صحيح lineno
#پایان_اگر
  )
{
  إذا(ftpc->state != newstate) {
#اگر_تعریف_شده DEBUGBUILD
    NOVERBOSE((فراغ)lineno);
    CURL_TRC_FTP(داده, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_نامها[newstate], lineno);
#وإلا
    CURL_TRC_FTP(داده, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_نامها[newstate]);
#پایان_اگر
  }
  ftpc->state = newstate;
}

/* Local API functions */
#اگر_تعریف_نشده DEBUGBUILD
#تعریف_کن ftp_state(x, y, z) ftp_state_کم(x, y, z)
#وإلا /* !DEBUGBUILD */
#تعریف_کن ftp_state(x, y, z) ftp_state_کم(x, y, z, __LINE__)
#پایان_اگر /* DEBUGBUILD */

ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp);
ساكن CURLcode ftp_state_quote(بنية Curl_easy *داده,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init, ftpstate instate);
ساكن CURLcode ftp_nb_type(بنية Curl_easy *داده,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate);
ساكن CURLcode getftpresponse(بنية Curl_easy *داده, size_t *nreadp,
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

#تعریف_کن FTP_MAX_DIR_عمق 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode ftp_parse_url_مسیر(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp)
{
  ثابت حرف *slashPos = عدم;
  ثابت حرف *fileName = عدم;
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *raw_مسیر = عدم; /* URL-decoded "raw" path */
  size_t مسیر_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  إذا(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  نتیجه = Curl_urldecode(ftp->مسیر, 0, &ftpc->rawpath, &مسیر_Len, REJECT_CTRL);
  إذا(نتیجه) {
    failf(داده, "path contains control characters");
    أرجع نتیجه;
  }
  raw_مسیر = ftpc->rawpath;

  بدل(داده->مجموعه.ftp_filemethod) {
  حالة FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    إذا((مسیر_Len > 0) && (raw_مسیر[مسیر_Len - 1] != '/'))
      fileName = raw_مسیر;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    اكسر;

  حالة FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_مسیر, '/');
    إذا(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_مسیر;
      إذا(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_بیرون_از_حافظه;

      ftpc->dirs[0].شروع = 0;
      ftpc->dirs[0].len = (صحيح)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    وإلا
      fileName = raw_مسیر; /* filename only (or empty) */
    اكسر;

  افتراضي: /* allow pretty much anything */
  حالة FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ثابت حرف *curPos = raw_مسیر;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_مسیر);

    إذا(dirAlloc >= FTP_MAX_DIR_عمق)
      /* suspiciously deep directory hierarchy */
      أرجع CURLE_URL_MALFORMAT;

    إذا(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, حجم(ftpc->dirs[0]));
      إذا(!ftpc->dirs)
        أرجع CURLE_بیرون_از_حافظه;

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
          ftpc->dirs[ftpc->dirdepth].شروع = (صحيح)(curPos - raw_مسیر);
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

  إذا(داده->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(داده, "Uploading to a URL without a filename");
    أرجع CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  إذا((داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD) && (raw_مسیر[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  وإلا { /* newly created FTP connections are already in entry path */
    ثابت حرف *old_مسیر = داده->conn->بیتها.reuse ? ftpc->prevpath : "";
    إذا(old_مسیر) {
      size_t n = مسیر_Len;
      إذا(داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      وإلا
        n -= ftpc->file ? طول_رشته(ftpc->file) : 0;

      إذا((طول_رشته(old_مسیر) == n) && raw_مسیر && !مقایسه_رشته(raw_مسیر, old_مسیر, n)) {
        infof(داده, "Request has same path as previous transfer");
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

ساكن فراغ close_secondarysocket(بنية Curl_easy *داده,
                                  بنية ftp_conn *ftpc)
{
  (فراغ)ftpc;
  CURL_TRC_FTP(داده, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(داده, داده->conn, SECONDARYSOCKET);
}

#اگر_تعریف_شده CURL_PREFER_LF_LINEENDS
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

ساكن CURLcode ftp_cw_lc_write(بنية Curl_easy *داده,
                                بنية Curl_cwriter *writer, صحيح type,
                                ثابت حرف *buf, size_t blen)
{
  ساكن ثابت حرف nl = '\n';
  بنية ftp_cw_lc_ctx *ctx = writer->ctx;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  إذا(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  طالما(blen) {
    /* do not pass EOS when writing parts */
    صحيح chunk_type = (type & ~CLIENTWRITE_EOS);
    ثابت حرف *cp;
    size_t chunk_len;
    CURLcode نتیجه;

    إذا(ctx->newline_pending) {
      إذا(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        نتیجه = Curl_cwriter_write(داده, writer->بعدی, chunk_type, &nl, 1);
        إذا(نتیجه)
          أرجع نتیجه;
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
      نتیجه = Curl_cwriter_write(داده, writer->بعدی, chunk_type,
                                  buf, chunk_len);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  إذا(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, blen);
  }
  وإلا إذا(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    إذا(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      أرجع Curl_cwriter_write(داده, writer->بعدی, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, 0);
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

#پایان_اگر /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ساكن CURLcode ftp_check_ctrl_on_داده_wait(بنية Curl_easy *داده,
                                            بنية ftp_conn *ftpc)
{
  بنية connectdata *conn = داده->conn;
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
      infof(داده, "There is negative response in cache while serv connect");
      (فراغ)getftpresponse(داده, &nread, &ftpcode);
      أرجع CURLE_FTP_ACCEPT_FAILED;
    }
  }

  إذا(pp->سرریز)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  وإلا {
    صحيح socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    بدل(socketstate) {
    حالة -1: /* error */
      /* let's die here */
      failf(داده, "Error while waiting for server connect");
      أرجع CURLE_FTP_ACCEPT_FAILED;
    افتراضي:
      إذا(socketstate & CURL_CSELECT_درون)
        response = TRUE;
      اكسر;
    }
  }

  إذا(response) {
    infof(داده, "Ctrl conn has data while waiting for data conn");
    إذا(pp->سرریز > 3) {
      ثابت حرف *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->سرریز + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      إذا((len > 3) && LASTLINE(r)) {
        curl_off_t وضعیت;
        إذا(!curlx_str_عدد(&r, &وضعیت, 999) && (وضعیت == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(داده, "Got 226 before data activity");
          أرجع CURLE_OK;
        }
      }
    }

    (فراغ)getftpresponse(داده, &nread, &ftpcode);

    infof(داده, "FTP code: %03d", ftpcode);

    إذا(ftpcode / 100 > 3)
      أرجع CURLE_FTP_ACCEPT_FAILED;

    أرجع CURLE_WEIRD_SERVER_پاسخ;
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
ساكن CURLcode ftp_initiate_transfer(بنية Curl_easy *داده,
                                      بنية ftp_conn *ftpc)
{
  CURLcode نتیجه = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(داده, "ftp_initiate_transfer()");
  نتیجه = Curl_conn_connect(داده, SECONDARYSOCKET, TRUE, &connected);
  إذا(نتیجه || !connected)
    أرجع نتیجه;

  إذا(داده->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_راهاندازی_send(داده, SECONDARYSOCKET);
    Curl_xfer_مجموعه_shutdown(داده, TRUE, TRUE);
  }
  وإلا {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_راهاندازی_recv(داده, SECONDARYSOCKET, داده->req.اندازه);
    Curl_xfer_مجموعه_shutdown(داده, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(داده, ftpc, FTP_STOP);

  أرجع CURLE_OK;
}

ساكن bool ftp_endofresp(بنية Curl_easy *داده, بنية connectdata *conn,
                          ثابت حرف *سطر, size_t len, صحيح *کد)
{
  curl_off_t وضعیت;
  (فراغ)داده;
  (فراغ)conn;

  إذا((len > 3) && LASTLINE(سطر) && !curlx_str_عدد(&سطر, &وضعیت, 999)) {
    *کد = (صحيح)وضعیت;
    أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن CURLcode ftp_readresp(بنية Curl_easy *داده,
                             بنية ftp_conn *ftpc,
                             int8_t sockindex,
                             بنية pingpong *pp,
                             صحيح *ftpcodep, /* return the ftp-code if done */
                             size_t *اندازه) /* size of the response */
{
  صحيح کد;
  CURLcode نتیجه = Curl_pp_readresp(داده, sockindex, pp, &کد, اندازه);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  إذا(!ftpc->shutdown)
    داده->info.httpcode = کد;

  *ftpcodep = کد;

  إذا(کد == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(داده, "We got a 421 - timeout");
    ftp_state(داده, ftpc, FTP_STOP);
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  أرجع نتیجه;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ساكن CURLcode getftpresponse(بنية Curl_easy *داده,
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

  بنية connectdata *conn = داده->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode نتیجه = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp = &ftpc->pp;
  size_t nread;
  صحيح cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(داده, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;

  طالما(!*ftpcodep && !نتیجه) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(داده, pp);
    timediff_t interval_ms;

    إذا(timeout <= 0) {
      failf(داده, "FTP response timeout");
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
    وإلا إذا(!Curl_conn_داده_pending(داده, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(داده, pp) ?
        sockfd : CURL_SOCKET_BAD;
      صحيح ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      إذا(ev < 0) {
        failf(داده, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        أرجع CURLE_RECV_ERROR;
      }
      وإلا إذا(ev == 0) {
        نتیجه = Curl_pgrsUpdate(داده);
        استمر; /* continue in our loop for the timeout duration */
      }
    }

    إذا(Curl_pp_needs_flush(داده, pp)) {
      نتیجه = Curl_pp_flushsend(داده, pp);
      إذا(نتیجه)
        اكسر;
    }

    نتیجه = ftp_readresp(داده, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    إذا(نتیجه)
      اكسر;

    إذا(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
داده*/
      cache_skip++;
    وإلا
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(داده, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (صحيح)نتیجه, *nreadp, *ftpcodep);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_user(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية connectdata *conn)
{
  CURLcode نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  إذا(!نتیجه) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(داده, ftpc, FTP_USER);
  }
  أرجع نتیجه;
}

ساكن CURLcode ftp_state_pwd(بنية Curl_easy *داده,
                              بنية ftp_conn *ftpc)
{
  CURLcode نتیجه;
#اگر_تعریف_شده DEBUGBUILD
  إذا(!داده->id && getenv("CURL_FTP_PWD_STOP"))
    أرجع CURLE_OK;
#پایان_اگر
  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "PWD");
  إذا(!نتیجه)
    ftp_state(داده, ftpc, FTP_PWD);

  أرجع نتیجه;
}

/* For the FTP "protocol connect" and "doing" phases only */
ساكن CURLcode ftp_pollset(بنية Curl_easy *داده,
                            بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? Curl_pp_pollset(داده, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ساكن CURLcode ftp_domore_pollset(بنية Curl_easy *داده,
                                   بنية easy_pollset *ps)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);

  إذا(!ftpc)
    أرجع CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(داده, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  إذا(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(داده->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (داده->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(داده->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    أرجع Curl_pollset_جمع_کن_درون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
  }
  أرجع Curl_pp_pollset(داده, &ftpc->pp, ps);
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
  أرجع &ftpc->rawpath[ftpc->dirs[رقم].شروع];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ساكن CURLcode ftp_state_cwd(بنية Curl_easy *داده,
                              بنية ftp_conn *ftpc,
                              بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(ftpc->cwddone)
    /* already done and fine */
    نتیجه = ftp_state_mdtm(داده, ftpc, ftp);
  وإلا {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((داده->مجموعه.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    إذا(داده->conn->بیتها.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "CWD %s", ftpc->entrypath);
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_CWD);
    }
    وإلا {
      إذا(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        إذا(!نتیجه)
          ftp_state(داده, ftpc, FTP_CWD);
      }
      وإلا {
        /* No CWD necessary */
        نتیجه = ftp_state_mdtm(داده, ftpc, ftp);
      }
    }
  }
  أرجع نتیجه;
}

تعريفنوع تعداد {
  EPRT,
  پورت_2,
  انجامشده_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ساكن CURLcode ftp_پورت_parse_string(بنية Curl_easy *داده,
                                      بنية connectdata *conn,
                                      ثابت حرف *string_ftpport,
                                      بنية Curl_sockaddr_storage *ss,
                                      بیعلامت کوتاه *پورت_minp,
                                      بیعلامت کوتاه *پورت_maxp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen)
{
  ثابت حرف *ip_انتها = عدم;
  ثابت حرف *addr = عدم;
  size_t addrlen = 0;
  بیعلامت کوتاه پورت_min = 0;
  بیعلامت کوتاه پورت_max = 0;
  حرف ipstr[50];
#اگر_تعریف_نشده USE_IPV6
  (فراغ)conn;
  (فراغ)ss;
#پایان_اگر

  /* default to nothing */
  *hostp = عدم;
  *پورت_minp = *پورت_maxp = 0;

  إذا(!string_ftpport || (طول_رشته(string_ftpport) <= 1))
    اذهب انجامشده;

#اگر_تعریف_شده USE_IPV6
  إذا(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ثابت حرف *ip_شروع = string_ftpport + 1;
    ip_انتها = strchr(ip_شروع, ']');
    إذا(ip_انتها) {
      addrlen = ip_انتها - ip_شروع;
      addr = ip_شروع;
    }
  }
  وإلا
#پایان_اگر
    إذا(*string_ftpport == ':') {
      /* :port */
      ip_انتها = string_ftpport;
    }
    وإلا {
      ip_انتها = strchr(string_ftpport, ':');
      addr = string_ftpport;
      إذا(ip_انتها) {
#اگر_تعریف_شده USE_IPV6
        بنية sockaddr_in6 * ثابت sa6 = (فراغ *)ss;
#پایان_اگر
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_انتها - string_ftpport;
#اگر_تعریف_شده USE_IPV6
        إذا(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = طول_رشته(string_ftpport);
          ip_انتها = عدم; /* this got no port ! */
        }
#پایان_اگر
      }
      وإلا
        /* ipv4|interface */
        addrlen = طول_رشته(string_ftpport);
    }

  /* parse the port */
  إذا(ip_انتها) {
    ثابت حرف *portp = strchr(ip_انتها, ':');
    إذا(portp) {
      curl_off_t شروع;
      curl_off_t انتها;
      portp++;
      إذا(!curlx_str_عدد(&portp, &شروع, 0xffff)) {
        پورت_min = (بیعلامت کوتاه)شروع;
        إذا(!curlx_str_single(&portp, '-') &&
           !curlx_str_عدد(&portp, &انتها, 0xffff))
          پورت_max = (بیعلامت کوتاه)انتها;
        وإلا
          پورت_max = پورت_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  إذا(پورت_min > پورت_max)
    پورت_min = پورت_max = 0;

  إذا(addrlen) {
    ثابت بنية Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(داده, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    إذا(!remote_addr || (addrlen >= حجم(ipstr)) || (addrlen >= hbuflen))
      أرجع CURLE_FTP_پورت_FAILED;
    کپی_حافظه(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    بدل(Curl_if2ip(remote_addr->family,
#اگر_تعریف_شده USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#پایان_اگر
                      ipstr, hbuf, hbuflen)) {
    حالة IF2_IP_NOT_یافت:
      /* not an interface, use the string as hostname instead */
      کپی_حافظه(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      اكسر;
    حالة IF2IP_AF_NOT_SUPPORTED:
      أرجع CURLE_FTP_پورت_FAILED;
    حالة IF2_IP_یافت:
      *hostp = hbuf; /* use the hbuf for hostname */
      اكسر;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

انجامشده:
  *پورت_minp = پورت_min;
  *پورت_maxp = پورت_max;
  أرجع CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ساكن CURLcode ftp_پورت_default_host(بنية Curl_easy *داده,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#پایان_اگر
  حرف میانگیر[STRERROR_LEN];
  CURLcode نتیجه;

  *sslenp = حجم(*ss);
  إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(داده, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
    أرجع CURLE_FTP_پورت_FAILED;
  }
  بدل(sa->sa_family) {
#اگر_تعریف_شده USE_IPV6
  حالة AF_INET6:
    نتیجه = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    اكسر;
#پایان_اگر
  افتراضي:
    نتیجه = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    اكسر;
  }
  إذا(نتیجه)
    أرجع CURLE_FTP_پورت_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  أرجع CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ساكن CURLcode ftp_پورت_resolve_host(بنية Curl_easy *داده,
                                      بنية connectdata *conn,
                                      ثابت حرف *host,
                                      بنية Curl_dns_مدخل **dns_entryp,
                                      ثابت بنية Curl_addrinfo **resp)
{
  CURLcode نتیجه;

  *resp = عدم;
  نتیجه = Curl_resolv_blocking(
    داده, Curl_resolv_dns_queries(داده, conn->ip_نسخه),
    host, 0, Curl_conn_get_transport(داده, conn), dns_entryp);
  إذا(نتیجه)
    failf(داده, "failed to resolve the address provided to PORT: %s", host);
  وإلا {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  أرجع نتیجه;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ساكن CURLcode ftp_پورت_open_socket(بنية Curl_easy *داده,
                                     بنية connectdata *conn,
                                     ثابت بنية Curl_addrinfo *res,
                                     ثابت بنية Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  حرف میانگیر[STRERROR_LEN];
  صحيح sockerr = 0;
  ثابت بنية Curl_addrinfo *ai;
  CURLcode نتیجه = CURLE_FTP_پورت_FAILED;

  لكل(ai = res; ai; ai = ai->ai_بعدی) {
    نتیجه =
      Curl_socket_open(داده, ai, عدم,
                       Curl_conn_get_transport(داده, conn), portsockp);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        أرجع نتیجه;
      نتیجه = CURLE_FTP_پورت_FAILED;
      sockerr = SOCKERRNO;
      استمر;
    }
    اكسر;
  }
  إذا(!ai) {
    failf(داده, "socket failure: %s",
          curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
    أرجع CURLE_FTP_پورت_FAILED;
  }
  *aip = ai;
  أرجع نتیجه;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ساكن CURLcode ftp_پورت_bind_socket(بنية Curl_easy *داده,
                                     بنية connectdata *conn,
                                     curl_socket_t portsock,
                                     ثابت بنية Curl_addrinfo *ai,
                                     بنية Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     بیعلامت کوتاه پورت_min,
                                     بیعلامت کوتاه پورت_max,
                                     bool non_local)
{
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#پایان_اگر
  حرف میانگیر[STRERROR_LEN];
  بیعلامت کوتاه پورت;
  صحيح sockerr;

  کپی_حافظه(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  لكل(پورت = پورت_min; پورت <= پورت_max;) {
    إذا(sa->sa_family == AF_INET)
      sa4->sin_port = htons(پورت);
#اگر_تعریف_شده USE_IPV6
    وإلا
      sa6->sin6_port = htons(پورت);
#پایان_اگر
    إذا(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      إذا(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(داده, "bind(port=%hu) on non-local address failed: %s", پورت,
              curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));

        *sslen_io = حجم(*ss);
        إذا(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(داده, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
          أرجع CURLE_FTP_پورت_FAILED;
        }
        پورت = پورت_min;
        non_local = FALSE; /* do not try this again */
        استمر;
      }
      إذا(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(داده, "bind(port=%hu) failed: %s", پورت,
              curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
        أرجع CURLE_FTP_پورت_FAILED;
      }
    }
    وإلا
      اكسر;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    إذا(پورت == پورت_max) {
      failf(داده, "bind() failed, ran out of ports");
      أرجع CURLE_FTP_پورت_FAILED;
    }
    پورت++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = حجم(*ss);
  إذا(getsockname(portsock, sa, sslen_io)) {
    failf(داده, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
    أرجع CURLE_FTP_پورت_FAILED;
  }
  CURL_TRC_FTP(داده, "ftp_port_bind_socket(), socket bound to port %d", پورت);
  أرجع CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ساكن CURLcode ftp_پورت_listen(بنية Curl_easy *داده, curl_socket_t portsock)
{
  حرف میانگیر[STRERROR_LEN];

  إذا(listen(portsock, 1)) {
    failf(داده, "socket failure: %s",
          curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
    أرجع CURLE_FTP_پورت_FAILED;
  }
  CURL_TRC_FTP(داده, "ftp_port_listen(), listening on port");
  أرجع CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ساكن CURLcode ftp_پورت_send_command(بنية Curl_easy *داده,
                                      بنية ftp_conn *ftpc,
                                      بنية connectdata *conn,
                                      بنية Curl_sockaddr_storage *ss,
                                      ثابت بنية Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ساكن ثابت حرف نما[][5] = { "EPRT", "PORT" };
  بنية sockaddr *sa = (بنية sockaddr *)ss;
  بنية sockaddr_in * ثابت sa4 = (فراغ *)sa;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 * ثابت sa6 = (فراغ *)sa;
#پایان_اگر
  حرف myhost[MAX_IPADR_LEN + 1] = "";
  بیعلامت کوتاه پورت;
  CURLcode نتیجه;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_آدرس(ai, myhost, حجم(myhost));

#اگر_تعریف_شده USE_IPV6
  إذا(!conn->بیتها.ftp_use_eprt && conn->بیتها.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->بیتها.ftp_use_eprt = TRUE;
#پایان_اگر

  لكل(; fcmd != انجامشده_2; fcmd++) {

    إذا(!conn->بیتها.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      استمر;

    إذا((پورت_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      استمر;

    بدل(sa->sa_family) {
    حالة AF_INET:
      پورت = ntohs(sa4->sin_port);
      اكسر;
#اگر_تعریف_شده USE_IPV6
    حالة AF_INET6:
      پورت = ntohs(sa6->sin6_port);
      اكسر;
#پایان_اگر
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
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s |%d|%s|%hu|", نما[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, پورت);
      إذا(نتیجه) {
        failf(داده, "Failure sending EPRT command: %s",
              curl_easy_strerror(نتیجه));
        أرجع نتیجه;
      }
      اكسر;
    }
    إذا(پورت_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      حرف هدف[حجم(myhost) + 20];
      ثابت حرف *منبع = myhost;
      حرف *dest = هدف;

      /* translate x.x.x.x to x,x,x,x */
      طالما(*منبع) {
        إذا(*منبع == '.')
          *dest = ',';
        وإلا
          *dest = *منبع;
        dest++;
        منبع++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (صحيح)(پورت >> 8), (صحيح)(پورت & 0xff));

      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s %s", نما[fcmd], هدف);
      إذا(نتیجه) {
        failf(داده, "Failure sending PORT command: %s",
              curl_easy_strerror(نتیجه));
        أرجع نتیجه;
      }
      اكسر;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(داده, ftpc, FTP_پورت);
  أرجع CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ساكن CURLcode ftp_state_use_پورت(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode نتیجه = CURLE_FTP_پورت_FAILED;
  بنية connectdata *conn = داده->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  بنية Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  حرف hbuf[NI_MAXHOST];
  ثابت حرف *host = عدم;
  ثابت حرف *string_ftpport = CURL_EASY_STR(داده, STRING_FTPPORT);
  بنية Curl_dns_مدخل *dns_مدخل = عدم;
  ثابت بنية Curl_addrinfo *res = عدم;
  ثابت بنية Curl_addrinfo *ai = عدم;
  بیعلامت کوتاه پورت_min = 0;
  بیعلامت کوتاه پورت_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  نتیجه = ftp_پورت_parse_string(داده, conn, string_ftpport,
                                 &ss, &پورت_min, &پورت_max,
                                 &host, hbuf, حجم(hbuf));
  إذا(!نتیجه && !host)
    /* if no host was specified, use the control connection's local IP */
    نتیجه = ftp_پورت_default_host(داده, conn, &ss, &sslen, &host,
                                   hbuf, حجم(hbuf), &non_local);

  /* resolve host string to address list */
  إذا(!نتیجه)
    نتیجه = ftp_پورت_resolve_host(داده, conn, host, &dns_مدخل, &res);

  /* Open a TCP socket for the data connection */
  إذا(!نتیجه)
    نتیجه = ftp_پورت_open_socket(داده, conn, res, &ai, &portsock);
  إذا(!نتیجه) {
    CURL_TRC_FTP(داده, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    نتیجه = ftp_پورت_bind_socket(داده, conn, portsock, ai, &ss, &sslen,
                                  پورت_min, پورت_max, non_local);
  }

  /* listen */
  إذا(!نتیجه)
    نتیجه = ftp_پورت_listen(داده, portsock);

  /* send the PORT / EPRT command */
  إذا(!نتیجه)
    نتیجه = ftp_پورت_send_command(داده, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  إذا(!نتیجه)
    نتیجه = Curl_conn_tcp_listen_مجموعه(داده, conn, SECONDARYSOCKET, &portsock);

  إذا(!نتیجه)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  إذا(dns_مدخل)
    Curl_dns_مدخل_unlink(داده, &dns_مدخل);
  إذا(نتیجه) {
    ftp_state(داده, ftpc, FTP_STOP);
  }
  وإلا {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    إذا(conn->بیتها.ftp_use_داده_ssl && داده->مجموعه.ftp_use_پورت &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      نتیجه = Curl_ssl_cfilter_جمع_کن(
        داده, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->بیتها.do_بیشتر = FALSE;
    Curl_expire(داده, (داده->مجموعه.accepttimeout > 0) ?
                داده->مجموعه.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  إذا(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(داده, conn, portsock);
  أرجع نتیجه;
}

ساكن CURLcode ftp_state_use_pasv(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc,
                                   بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  ساكن ثابت حرف نما[][5] = { "EPSV", "PASV" };
  صحيح modeoff;

#اگر_تعریف_شده PF_INET6
  إذا(!conn->بیتها.ftp_use_epsv && conn->بیتها.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->بیتها.ftp_use_epsv = TRUE;
#پایان_اگر

  modeoff = conn->بیتها.ftp_use_epsv ? 0 : 1;

  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", نما[modeoff]);
  إذا(!نتیجه) {
    ftpc->count1 = modeoff;
    ftp_state(داده, ftpc, FTP_PASV);
    infof(داده, "Connect data stream passively");
  }
  أرجع نتیجه;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ساكن CURLcode ftp_state_prepare_transfer(بنية Curl_easy *داده,
                                           بنية ftp_conn *ftpc,
                                           بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  إذا(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(داده, ftpc, FTP_RETR_PREQUOTE);
    نتیجه = ftp_state_quote(داده, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  وإلا إذا(داده->مجموعه.ftp_use_پورت) {
    /* We have chosen to use the PORT (or similar) command */
    نتیجه = ftp_state_use_پورت(داده, ftpc, EPRT);
  }
  وإلا {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    إذا(داده->مجموعه.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      إذا(!ftpc->file)
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) :
                               (داده->state.list_only ? "NLST" : "LIST"));
      وإلا إذا(داده->state.upload)
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "PRET STOR %s", ftpc->file);
      وإلا
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "PRET RETR %s", ftpc->file);
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_PRET);
    }
    وإلا
      نتیجه = ftp_state_use_pasv(داده, ftpc, conn);
  }
  أرجع نتیجه;
}

ساكن CURLcode ftp_state_rest(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

  إذا((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "REST %d", 0);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_REST);
  }
  وإلا
    نتیجه = ftp_state_prepare_transfer(داده, ftpc, ftp);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_اندازه(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

  إذا((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "SIZE %s", ftpc->file);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_اندازه);
  }
  وإلا
    نتیجه = ftp_state_rest(داده, ftpc, ftp);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_list(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

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

  إذا((داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD) && ftp->مسیر) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ثابت حرف *raw_مسیر = ftpc->rawpath;
    ثابت حرف *slashPos = strrchr(raw_مسیر, '/');
    إذا(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_مسیر;
      إذا(n == 0)
        ++n;

      lstArg = raw_مسیر;
      lstArglen = (صحيح)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) :
                      (داده->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  إذا(!cmd)
    أرجع CURLE_بیرون_از_حافظه;

  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  إذا(!نتیجه)
    ftp_state(داده, ftpc, FTP_LIST);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_list_prequote(بنية Curl_easy *داده,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(داده, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ساكن CURLcode ftp_state_retr_prequote(بنية Curl_easy *داده,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(داده, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ساكن CURLcode ftp_state_stor_prequote(بنية Curl_easy *داده,
                                        بنية ftp_conn *ftpc,
                                        بنية FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  أرجع ftp_state_quote(داده, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ساكن CURLcode ftp_state_type(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  إذا(داده->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)داده->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    نتیجه = ftp_nb_type(داده, ftpc, ftp, (bool)داده->state.prefer_ascii,
                         FTP_TYPE);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
    نتیجه = ftp_state_اندازه(داده, ftpc, ftp);

  أرجع نتیجه;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ساكن CURLcode ftp_state_mdtm(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp)
{
  CURLcode نتیجه = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  إذا((داده->مجموعه.get_filetime || داده->مجموعه.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "MDTM %s", ftpc->file);

    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_MDTM);
  }
  وإلا
    نتیجه = ftp_state_type(داده, ftpc, ftp);

  أرجع نتیجه;
}

/* This is called after the TYPE and possible quote commands have been sent */
ساكن CURLcode ftp_state_ul_راهاندازی(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   bool sizechecked)
{
  CURLcode نتیجه = CURLE_OK;
  curl_بیت append = داده->مجموعه.remote_append;

  إذا((داده->state.resume_from && !sizechecked) ||
     ((داده->state.resume_from > 0) && sizechecked)) {
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

    إذا(داده->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "SIZE %s", ftpc->file);
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_STOR_اندازه);
      أرجع نتیجه;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    إذا(داده->مجموعه.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, easy_seek_func);
      seekerr = داده->مجموعه.seek_func(داده->مجموعه.seek_client,
                                    داده->state.resume_from, SEEK_SET);
      CURL_CBAPI_انتها(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(داده, "Could not seek stream");
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (داده->state.resume_from - passed > (curl_off_t)حجم(scratch)) ?
          حجم(scratch) :
          curlx_sotouz(داده->state.resume_from - passed);

        size_t actuallyread =
          داده->state.fread_func(scratch, 1, readthisamountnow,
                                 داده->state.درون);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(داده, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < داده->state.resume_from);
    }
    /* now, decrease the size of the read */
    إذا(داده->state.infilesize > 0) {
      داده->state.infilesize -= داده->state.resume_from;

      إذا(داده->state.infilesize <= 0) {
        infof(داده, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_راهاندازی_nop(داده);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(داده, ftpc, FTP_STOP);
        أرجع CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  إذا(!نتیجه)
    ftp_state(داده, ftpc, FTP_STOR);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_retr(بنية Curl_easy *داده,
                               بنية ftp_conn *ftpc,
                               بنية FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_FTP(داده, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  إذا(داده->مجموعه.max_filesize && (filesize > داده->مجموعه.max_filesize)) {
    failf(داده, "Maximum file size exceeded");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  إذا(داده->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    إذا(filesize == -1) {
      infof(داده, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    وإلا {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      إذا(داده->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        إذا(filesize < -داده->state.resume_from) {
          failf(داده, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                داده->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -داده->state.resume_from;
        /* download from where? */
        داده->state.resume_from = filesize - ftp->downloadsize;
      }
      وإلا {
        إذا(filesize < داده->state.resume_from) {
          failf(داده, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                داده->state.resume_from, filesize);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - داده->state.resume_from;
      }
    }

    إذا(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_راهاندازی_nop(داده);
      infof(داده, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(داده, ftpc, FTP_STOP);
      أرجع CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(داده, "Instructs server to resume from offset %" FMT_OFF_T,
          داده->state.resume_from);

    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "REST %" FMT_OFF_T,
                           داده->state.resume_from);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_RETR_REST);
  }
  وإلا {
    /* no resume */
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "RETR %s", ftpc->file);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_RETR);
  }

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_quote(بنية Curl_easy *داده,
                                بنية ftp_conn *ftpc,
                                بنية FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  bool quote = FALSE;
  بنية curl_slist *قلم;

  بدل(instate) {
  حالة FTP_QUOTE:
  افتراضي:
    قلم = داده->مجموعه.quote;
    اكسر;
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    قلم = داده->مجموعه.prequote;
    اكسر;
  حالة FTP_POSTQUOTE:
    قلم = داده->مجموعه.postquote;
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

  إذا(قلم) {
    صحيح i = 0;

    /* Skip count1 items in the linked list */
    طالما((i < ftpc->count1) && قلم) {
      قلم = قلم->بعدی;
      i++;
    }
    إذا(قلم) {
      ثابت حرف *cmd = قلم->داده;
      إذا(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      وإلا
        ftpc->count2 = 0; /* failure means cancel operation */

      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", cmd);
      إذا(نتیجه)
        أرجع نتیجه;
      ftp_state(داده, ftpc, instate);
      quote = TRUE;
    }
  }

  إذا(!quote) {
    /* No more quote to send, continue to ... */
    بدل(instate) {
    حالة FTP_QUOTE:
    افتراضي:
      نتیجه = ftp_state_cwd(داده, ftpc, ftp);
      اكسر;
    حالة FTP_RETR_PREQUOTE:
      إذا(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(داده, ftpc, FTP_STOP);
      وإلا {
        إذا(ftpc->معلوم_filesize != -1) {
          Curl_pgrs_مجموعه_Download_اندازه(داده, ftpc->معلوم_filesize);
          نتیجه = ftp_state_retr(داده, ftpc, ftp, ftpc->معلوم_filesize);
        }
        وإلا {
          إذا(داده->مجموعه.ignorecl || داده->state.prefer_ascii) {
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
            نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "RETR %s", ftpc->file);
            إذا(!نتیجه)
              ftp_state(داده, ftpc, FTP_RETR);
          }
          وإلا {
            نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "SIZE %s", ftpc->file);
            إذا(!نتیجه)
              ftp_state(داده, ftpc, FTP_RETR_اندازه);
          }
        }
      }
      اكسر;
    حالة FTP_STOR_PREQUOTE:
      نتیجه = ftp_state_ul_راهاندازی(داده, ftpc, ftp, FALSE);
      اكسر;
    حالة FTP_POSTQUOTE:
      اكسر;
    حالة FTP_LIST_PREQUOTE:
      ftp_state(داده, ftpc, FTP_LIST_TYPE);
      نتیجه = ftp_state_list(داده, ftpc, ftp);
      اكسر;
    }
  }

  أرجع نتیجه;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ساكن CURLcode ftp_epsv_disable(بنية Curl_easy *داده,
                                 بنية ftp_conn *ftpc,
                                 بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(conn->بیتها.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(داده, "Failed EPSV attempt, exiting");
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  infof(داده, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->بیتها.ftp_use_epsv = FALSE;
  close_secondarysocket(داده, ftpc);
  داده->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "PASV");
  إذا(!نتیجه) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(داده, ftpc, FTP_PASV);
  }
  أرجع نتیجه;
}

ساكن CURLcode ftp_control_addr_dup(بنية Curl_easy *داده, حرف **newhostp)
{
  بنية connectdata *conn = داده->conn;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  وإلا
#پایان_اگر
  إذا(!Curl_conn_get_ip_info(داده, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  وإلا {
    /* failed to get the remote_ip of the DATA connection */
    failf(داده, "unable to get peername of DATA connection");
    *newhostp = عدم;
    أرجع CURLE_FTP_CANT_GET_HOST;
  }
  أرجع *newhostp ? CURLE_OK : CURLE_بیرون_از_حافظه;
}

ساكن bool تطابق_pasv_6nums(ثابت حرف *p,
                             بیعلامت صحيح *array) /* 6 numbers */
{
  صحيح i;
  لكل(i = 0; i < 6; i++) {
    curl_off_t رقم;
    إذا(i) {
      إذا(*p != ',')
        أرجع FALSE;
      p++;
    }
    إذا(curlx_str_عدد(&p, &رقم, 0xff))
      أرجع FALSE;
    array[i] = (بیعلامت صحيح)رقم;
  }
  أرجع TRUE;
}

ساكن CURLcode ftp_state_pasv_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه;
  ثابت بنية pingpong *pp = &ftpc->pp;
  حرف *newhost = عدم;
  بیعلامت کوتاه newport = 0;
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
        إذا(curlx_str_عدد(&p, &رقم, 0xffff) || (*p != sep)) {
          failf(داده, "Illegal port number in EPSV reply");
          أرجع CURLE_FTP_WEIRD_PASV_پاسخ;
        }
        newport = (بیعلامت کوتاه)رقم;
        نتیجه = ftp_control_addr_dup(داده, &newhost);
        إذا(نتیجه)
          أرجع نتیجه;
      }
      وإلا
        ptr = عدم;
    }
    إذا(!ptr) {
      failf(داده, "Weirdly formatted EPSV reply");
      أرجع CURLE_FTP_WEIRD_PASV_پاسخ;
    }
  }
  وإلا إذا((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    بیعلامت صحيح ip[6];

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
      إذا(تطابق_pasv_6nums(str, ip))
        اكسر;
      str++;
    }

    إذا(!*str) {
      failf(داده, "Could not interpret the 227-response");
      أرجع CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    إذا(داده->مجموعه.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(داده, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      نتیجه = ftp_control_addr_dup(داده, &newhost);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    إذا(!newhost)
      أرجع CURLE_بیرون_از_حافظه;

    newport = (بیعلامت کوتاه)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  وإلا إذا(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    أرجع ftp_epsv_disable(داده, ftpc, conn);
  }
  وإلا {
    failf(داده, "Bad PASV/EPSV response: %03d", ftpcode);
    أرجع CURLE_FTP_WEIRD_PASV_پاسخ;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  نتیجه = Curl_peer_create(داده, conn->scheme, newhost, newport,
                            &conn->origin2);
  إذا(نتیجه)
    اذهب error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  إذا(conn->از_طریق_peer) {
    Curl_peer_unlink(&conn->از_طریق_peer2);
    نتیجه = Curl_peer_create(داده, conn->از_طریق_peer->scheme,
                              conn->از_طریق_peer->hostname, newport,
                              &conn->از_طریق_peer2);
    إذا(نتیجه)
      اذهب error;
  }

  نتیجه = Curl_conn_راهاندازی(داده, conn, SECONDARYSOCKET,
                           conn->بیتها.ftp_use_داده_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  إذا(نتیجه) {
    إذا((نتیجه != CURLE_بیرون_از_حافظه) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      نتیجه = ftp_epsv_disable(داده, ftpc, conn);
    }
    اذهب error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#اگر_تعریف_شده CURLVERBOSE
  إذا(داده->مجموعه.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(داده, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->پورت);
  }
#پایان_اگر

  conn->بیتها.do_بیشتر = TRUE;
  ftp_state(داده, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  أرجع نتیجه;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_statemach(بنية Curl_easy *داده,
                              بنية ftp_conn *ftpc,
                              bool *انجامشده)
{
  CURLcode نتیجه = Curl_pp_statemach(داده, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *انجامشده = (ftpc->state == FTP_STOP);

  أرجع نتیجه;
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
ساكن CURLcode ftp_do_بیشتر(بنية Curl_easy *داده, domore *بیشتر)
{
  بنية connectdata *conn = داده->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(داده, CURL_META_FTP_EASY);
  CURLcode نتیجه = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;

  *بیشتر = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  إذا(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(داده, SECONDARYSOCKET);
    نتیجه = Curl_conn_connect(داده, SECONDARYSOCKET, FALSE, &connected);
    إذا(نتیجه == CURLE_بیرون_از_حافظه)
      أرجع نتیجه;
    إذا(نتیجه || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(داده, SECONDARYSOCKET))) {
      إذا(نتیجه && !is_eptr && (ftpc->count1 == 0)) {
        *بیشتر = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        أرجع ftp_epsv_disable(داده, ftpc, conn);
      }
      أرجع نتیجه;
    }
  }

  إذا(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    نتیجه = ftp_statemach(داده, ftpc, &complete);

    إذا(complete)
      *بیشتر = DOMORE_انجامشده;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    إذا(نتیجه || !ftpc->wait_داده_conn)
      أرجع نتیجه;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *بیشتر = DOMORE_INCOMPLETE;
  }

  إذا(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    إذا(ftpc->wait_داده_conn) {
      bool serv_conned;

      نتیجه = Curl_conn_connect(داده, SECONDARYSOCKET, FALSE, &serv_conned);
      إذا(نتیجه)
        أرجع نتیجه; /* Failed to accept data connection */

      إذا(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_داده_conn = FALSE;
        نتیجه = ftp_initiate_transfer(داده, ftpc);

        إذا(نتیجه)
          أرجع نتیجه;

        *بیشتر = DOMORE_انجامشده; /* this state is now complete when the server has
                                connected back to us */
      }
      وإلا {
        نتیجه = ftp_check_ctrl_on_داده_wait(داده, ftpc);
        إذا(نتیجه)
          أرجع نتیجه;
      }
    }
    وإلا إذا(داده->state.upload) {
      نتیجه = ftp_nb_type(داده, ftpc, ftp, (bool)داده->state.prefer_ascii,
                           FTP_STOR_TYPE);
      إذا(نتیجه)
        أرجع نتیجه;

      نتیجه = ftp_statemach(داده, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *بیشتر = (!ftpc->wait_داده_conn && complete) ?
        DOMORE_انجامشده : DOMORE_INCOMPLETE;
    }
    وإلا {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      نتیجه = Curl_range(داده);

      إذا(نتیجه == CURLE_OK && داده->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      إذا(نتیجه)
        أرجع نتیجه;

      إذا((داده->state.list_only || !ftpc->file) &&
         !داده->مجموعه.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        إذا(ftp->transfer == PPTRANSFER_BODY) {
          نتیجه = ftp_nb_type(داده, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          إذا(نتیجه)
            أرجع نتیجه;
        }
        /* otherwise fall through */
      }
      وإلا {
        إذا(داده->مجموعه.prequote && !ftpc->file) {
          نتیجه = ftp_nb_type(داده, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        وإلا {
          نتیجه = ftp_nb_type(داده, ftpc, ftp, (bool)داده->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        إذا(نتیجه)
          أرجع نتیجه;
      }

      نتیجه = ftp_statemach(داده, ftpc, &complete);
      إذا(complete)
        *بیشتر = DOMORE_انجامشده;
    }
    أرجع نتیجه;
  }

  /* no data to transfer */
  Curl_xfer_راهاندازی_nop(داده);

  إذا(!ftpc->wait_داده_conn) {
    /* no waiting for the data connection so this is now complete */
    *بیشتر = DOMORE_انجامشده;
    CURL_TRC_FTP(داده, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (صحيح)نتیجه);
  }

  أرجع نتیجه;
}

/* call this when the DO phase has completed */
ساكن CURLcode ftp_dophase_انجامشده(بنية Curl_easy *داده,
                                 بنية ftp_conn *ftpc,
                                 بنية FTP *ftp,
                                 bool connected)
{
  إذا(connected) {
    domore completed;
    CURLcode نتیجه = ftp_do_بیشتر(داده, &completed);

    إذا(نتیجه) {
      close_secondarysocket(داده, ftpc);
      أرجع نتیجه;
    }
  }

  إذا(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_راهاندازی_nop(داده);
  وإلا إذا(!connected)
    /* since we did not connect now, we want do_more to get called */
    داده->conn->بیتها.do_بیشتر = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_state_پورت_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  بنية connectdata *conn = داده->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode نتیجه = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  إذا(ftpcode / 100 != 2) {
    /* the command failed */

    إذا(EPRT == fcmd) {
      infof(داده, "disabling EPRT usage");
      conn->بیتها.ftp_use_eprt = FALSE;
    }
    fcmd++;

    إذا(fcmd == انجامشده_2) {
      failf(داده, "Failed to do PORT");
      نتیجه = CURLE_FTP_پورت_FAILED;
    }
    وإلا
      /* try next */
      نتیجه = ftp_state_use_پورت(داده, ftpc, fcmd);
  }
  وإلا {
    infof(داده, "Connect data stream actively");
    ftp_state(داده, ftpc, FTP_STOP); /* end of DO phase */
    نتیجه = ftp_dophase_انجامشده(داده, ftpc, ftp, FALSE);
  }

  أرجع نتیجه;
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
                           صحيح *hour, صحيح *minute, صحيح *دومی);
UNITTEST bool ftp_213_date(ثابت حرف *p, صحيح *year, صحيح *month, صحيح *day,
                           صحيح *hour, صحيح *minute, صحيح *دومی)
{
  صحيح century;
  إذا((طول_رشته(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], دومی))
    أرجع FALSE;

  *year += century * 100;
  إذا((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*دومی > 60))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode client_write_سرآیند(بنية Curl_easy *داده,
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
  CURLcode نتیجه;
  bool save = (bool)داده->مجموعه.include_سرآیند;
  داده->مجموعه.include_سرآیند = TRUE;
  نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, buf, blen);
  داده->مجموعه.include_سرآیند = save;
  أرجع نتیجه;
}

ساكن CURLcode ftp_state_mdtm_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode)
{
  CURLcode نتیجه = CURLE_OK;

  بدل(ftpcode) {
  حالة 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    صحيح year, month, day, hour, minute, دومی;
    بنية pingpong *pp = &ftpc->pp;
    ثابت حرف *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    إذا(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &دومی)) {
      /* we have a time, reformat it */
      حرف timebuf[24];
      curl_msnprintf(timebuf, حجم(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, دومی);
      /* now, convert this into a time() value: */
      إذا(!Curl_getdate_capped(timebuf, &داده->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#رهنمود GCC diagnostic افزودن
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#رهنمود GCC diagnostic ignored "-Wtype-limits"
#پایان_اگر
    إذا(داده->req.no_body && ftpc->file &&
       داده->مجموعه.get_filetime && showtime) {
#إذا defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#رهنمود GCC diagnostic برداشت
#پایان_اگر
      حرف headerbuf[128];
      صحيح headerbuflen;
      time_t filetime = داده->info.filetime;
      بنية tm میانگیر;
      ثابت بنية tm *tm = &میانگیر;

      نتیجه = curlx_gmtime(filetime, &میانگیر);
      إذا(نتیجه)
        أرجع نتیجه;

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
      نتیجه = client_write_سرآیند(داده, headerbuf, headerbuflen);
      إذا(نتیجه)
        أرجع نتیجه;
    } /* end of a ridiculous amount of conditionals */
  }
    اكسر;
  افتراضي:
    infof(داده, "unsupported MDTM reply format");
    اكسر;
  حالة 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(داده, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    اكسر;
  }

  إذا(داده->مجموعه.timecondition) {
    إذا((داده->info.filetime > 0) && (داده->مجموعه.timevalue > 0)) {
      بدل(داده->مجموعه.timecondition) {
      حالة CURL_TIMECOND_IFMODSINCE:
      افتراضي:
        إذا(داده->info.filetime <= داده->مجموعه.timevalue) {
          infof(داده, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          داده->info.timecond = TRUE;
          ftp_state(داده, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      حالة CURL_TIMECOND_IFUNMODSINCE:
        إذا(داده->info.filetime > داده->مجموعه.timevalue) {
          infof(داده, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          داده->info.timecond = TRUE;
          ftp_state(داده, ftpc, FTP_STOP);
          أرجع CURLE_OK;
        }
        اكسر;
      } /* switch */
    }
    وإلا {
      infof(داده, "Skipping time comparison");
    }
  }

  إذا(!نتیجه)
    نتیجه = ftp_state_type(داده, ftpc, ftp);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_type_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(داده, "Could not set desired mode");
    أرجع CURLE_FTP_COULDNT_مجموعه_TYPE;
  }
  إذا(ftpcode != 200)
    infof(داده, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  إذا(instate == FTP_TYPE)
    نتیجه = ftp_state_اندازه(داده, ftpc, ftp);
  وإلا إذا(instate == FTP_LIST_TYPE)
    نتیجه = ftp_state_list(داده, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_TYPE)
    نتیجه = ftp_state_retr_prequote(داده, ftpc, ftp);
  وإلا إذا(instate == FTP_STOR_TYPE)
    نتیجه = ftp_state_stor_prequote(داده, ftpc, ftp);
  وإلا إذا(instate == FTP_RETR_LIST_TYPE)
    نتیجه = ftp_state_list_prequote(داده, ftpc, ftp);

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_اندازه_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجه = CURLE_OK;
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
    إذا(curlx_str_عدد(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  وإلا إذا(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_اندازه) {
    failf(داده, "The file does not exist");
    أرجع CURLE_REMOTE_FILE_NOT_یافت;
  }

  إذا(instate == FTP_اندازه) {
    إذا(filesize != -1) {
      حرف clbuf[128];
      صحيح clbuflen = curl_msnprintf(clbuf, حجم(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      نتیجه = client_write_سرآیند(داده, clbuf, clbuflen);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    Curl_pgrs_مجموعه_Download_اندازه(داده, filesize);
    نتیجه = ftp_state_rest(داده, ftpc, ftp);
  }
  وإلا إذا(instate == FTP_RETR_اندازه) {
    Curl_pgrs_مجموعه_Download_اندازه(داده, filesize);
    نتیجه = ftp_state_retr(داده, ftpc, ftp, filesize);
  }
  وإلا إذا(instate == FTP_STOR_اندازه) {
    داده->state.resume_from = filesize;
    نتیجه = ftp_state_ul_راهاندازی(داده, ftpc, ftp, TRUE);
  }

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_rest_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    بنية FTP *ftp,
                                    صحيح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  بدل(instate) {
  حالة FTP_REST:
  افتراضي:
    إذا(ftpcode == 350) {
      حرف میانگیر[24] = { "Accept-ranges: bytes\r\n" };
      نتیجه = client_write_سرآیند(داده, میانگیر, طول_رشته(میانگیر));
      إذا(نتیجه)
        أرجع نتیجه;
    }
    نتیجه = ftp_state_prepare_transfer(داده, ftpc, ftp);
    اكسر;

  حالة FTP_RETR_REST:
    إذا(ftpcode != 350) {
      failf(داده, "Could not use REST");
      نتیجه = CURLE_FTP_COULDNT_USE_REST;
    }
    وإلا {
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "RETR %s", ftpc->file);
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_RETR);
    }
    اكسر;
  }

  أرجع نتیجه;
}

ساكن CURLcode ftp_state_stor_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(ftpcode >= 400) {
    failf(داده, "Failed FTP upload: %0d", ftpcode);
    ftp_state(داده, ftpc, FTP_STOP);
    أرجع CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  إذا(داده->مجموعه.ftp_use_پورت) {
    bool connected;

    ftp_state(داده, ftpc, FTP_STOP); /* no longer in STOR state */

    نتیجه = Curl_conn_connect(داده, SECONDARYSOCKET, FALSE, &connected);
    إذا(نتیجه)
      أرجع نتیجه;

    إذا(!connected) {
      infof(داده, "Data conn was not available immediately");
      ftpc->wait_داده_conn = TRUE;
      أرجع ftp_check_ctrl_on_داده_wait(داده, ftpc);
    }
    ftpc->wait_داده_conn = FALSE;
  }
  أرجع ftp_initiate_transfer(داده, ftpc);
}

/* for LIST and RETR responses */
ساكن CURLcode ftp_state_get_resp(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc,
                                   بنية FTP *ftp,
                                   صحيح ftpcode,
                                   ftpstate instate)
{
  CURLcode نتیجه = CURLE_OK;

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

    داده->req.اندازه = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    إذا((instate != FTP_LIST) &&
       !داده->state.prefer_ascii &&
       !داده->مجموعه.ignorecl &&
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
          إذا(!curlx_str_عدد(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !مقایسه_رشته(c, "bytes", 5)) {
            داده->req.اندازه = what;
            اكسر;
          }
        }
      }
    }
    وإلا إذا(ftp->downloadsize > -1)
      داده->req.اندازه = ftp->downloadsize;

    إذا(داده->req.اندازه > داده->req.maxdownload && داده->req.maxdownload > 0)
      داده->req.اندازه = داده->req.maxdownload;
    وإلا إذا((instate != FTP_LIST) && داده->state.prefer_ascii)
      داده->req.اندازه = -1; /* for servers that understate ASCII mode file
اندازه*/

    infof(داده, "Maxdownload = %" FMT_OFF_T, داده->req.maxdownload);

    إذا(instate != FTP_LIST)
      infof(داده, "Getting file with size: %" FMT_OFF_T, داده->req.اندازه);

    إذا(داده->مجموعه.ftp_use_پورت) {
      bool connected;

      نتیجه = Curl_conn_connect(داده, SECONDARYSOCKET, FALSE, &connected);
      إذا(نتیجه)
        أرجع نتیجه;

      إذا(!connected) {
        infof(داده, "Data conn was not available immediately");
        ftp_state(داده, ftpc, FTP_STOP);
        ftpc->wait_داده_conn = TRUE;
        أرجع ftp_check_ctrl_on_داده_wait(داده, ftpc);
      }
      ftpc->wait_داده_conn = FALSE;
    }
    أرجع ftp_initiate_transfer(داده, ftpc);
  }
  وإلا {
    إذا((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(داده, ftpc, FTP_STOP); /* this phase is over */
    }
    وإلا {
      failf(داده, "RETR response: %03d", ftpcode);
      أرجع instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_یافت :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  أرجع نتیجه;
}

/* after USER, PASS and ACCT */
ساكن CURLcode ftp_state_loggedin(بنية Curl_easy *داده,
                                   بنية ftp_conn *ftpc)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(Curl_conn_is_ssl(داده->conn, FIRSTSOCKET)) {
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
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "PBSZ %d", 0);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_PBSZ);
  }
  وإلا {
    نتیجه = ftp_state_pwd(داده, ftpc);
  }
  أرجع نتیجه;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ساكن bool ftp_has_ctrl(ثابت حرف *string)
{
  ثابت بیعلامت حرف *s = (ثابت بیعلامت حرف *)string;
  طالما(*s) {
    إذا(*s < 0x20)
      أرجع TRUE;
    s++;
  }
  أرجع FALSE;
}

/* for USER and PASS responses */
ساكن CURLcode ftp_state_user_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجه = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  إذا((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(داده->conn->creds));
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_PASS);
  }
  وإلا إذا(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    نتیجه = ftp_state_loggedin(داده, ftpc);
  }
  وإلا إذا(ftpcode == 332) {
    ثابت حرف *account = CURL_EASY_STR(داده, STRING_FTP_ACCOUNT);
    إذا(!account) {
      failf(داده, "ACCT requested but none available");
      نتیجه = CURLE_LOGIN_DENIED;
    }
    وإلا إذا(ftp_has_ctrl(account)) {
      failf(داده, "Control byte in FTP account");
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    }
    وإلا {
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "ACCT %s", account);
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_ACCT);
    }
  }
  وإلا {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ثابت حرف *alt = CURL_EASY_STR(داده, STRING_FTP_ALTERNATIVE_مقصد_USER);
    إذا(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      إذا(ftp_has_ctrl(alt)) {
        failf(داده, "Control byte in FTP alternative-to-user command");
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      }
      وإلا {
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", alt);
        إذا(!نتیجه) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(داده, ftpc, FTP_USER);
        }
      }
    }
    وإلا {
      failf(داده, "Access denied: %03d", ftpcode);
      نتیجه = CURLE_LOGIN_DENIED;
    }
  }
  أرجع نتیجه;
}

/* for ACCT response */
ساكن CURLcode ftp_state_acct_resp(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc,
                                    صحيح ftpcode)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(ftpcode != 230) {
    failf(داده, "ACCT rejected by server: %03d", ftpcode);
    نتیجه = CURLE_FTP_WEIRD_PASS_پاسخ; /* FIX */
  }
  وإلا
    نتیجه = ftp_state_loggedin(داده, ftpc);

  أرجع نتیجه;
}

ساكن CURLcode ftp_pwd_resp(بنية Curl_easy *داده,
                             بنية ftp_conn *ftpc,
                             صحيح ftpcode)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode نتیجه;

  إذا(ftpcode == 257) {
    ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool مدخل_extracted = FALSE;
    بنية dynbuf بیرون;
    curlx_dyn_init(&بیرون, 1000);

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
            نتیجه = curlx_dyn_addn(&بیرون, &ptr[1], 1);
            ptr++;
          }
          وإلا {
            /* end of path */
            إذا(curlx_dyn_len(&بیرون))
              مدخل_extracted = TRUE;
            اكسر; /* get out of this loop */
          }
        }
        وإلا {
          إذا(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&بیرون);
            أرجع CURLE_WEIRD_SERVER_پاسخ;
          }
          نتیجه = curlx_dyn_addn(&بیرون, ptr, 1);
        }
        إذا(نتیجه) {
          curlx_dyn_free(&بیرون);
          أرجع نتیجه;
        }
      }
    }
    إذا(مدخل_extracted) {
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
      حرف *dir = curlx_dyn_ptr(&بیرون);

      إذا(!ftpc->server_os && dir[0] != '/') {
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "SYST");
        إذا(نتیجه) {
          curlx_dyn_free(&بیرون);
          أرجع نتیجه;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(داده, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(داده->state.most_recent_ftp_entrypath);
      داده->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      إذا(!داده->state.most_recent_ftp_entrypath)
        أرجع CURLE_بیرون_از_حافظه;

      إذا(!ftpc->server_os && dir[0] != '/') {
        ftp_state(داده, ftpc, FTP_SYST);
        أرجع CURLE_OK;
      }
    }
    وإلا {
      /* could not get the path */
      curlx_dyn_free(&بیرون);
      infof(داده, "Failed to figure out path");
    }
  }
  ftp_state(داده, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(داده, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  أرجع CURLE_OK;
}

ساكن ثابت حرف * ثابت ftpauth[] = { "SSL", "TLS" };

ساكن CURLcode ftp_wait_resp(بنية Curl_easy *داده,
                              بنية connectdata *conn,
                              بنية ftp_conn *ftpc,
                              صحيح ftpcode)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    إذا(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      أرجع ftp_state_user_resp(داده, ftpc, ftpcode);
  }
  وإلا إذا(ftpcode != 220) {
    failf(داده, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  إذا(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    بدل((دراز)داده->مجموعه.ftpsslauth) {
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
      failf(داده, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (صحيح)داده->مجموعه.ftpsslauth);
      أرجع CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_AUTH);
  }
  وإلا
    نتیجه = ftp_state_user(داده, ftpc, conn);
  أرجع نتیجه;
}

/* an implicit TLS scheme implies TLS on both connections */
ساكن بیعلامت حرف ftp_use_ssl(بنية connectdata *conn,
                                 بنية ftp_conn *ftpc)
{
  إذا(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    أرجع CURLUSESSL_ALL;
  أرجع ftpc->use_ssl;
}

ساكن CURLcode ftp_pp_statemachine(بنية Curl_easy *داده,
                                    بنية connectdata *conn)
{
  CURLcode نتیجه;
  صحيح ftpcode;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(داده, CURL_META_FTP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(داده, pp);

  نتیجه = ftp_readresp(داده, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  إذا(نتیجه || !ftpcode)
    أرجع نتیجه;

  /* we have now received a full FTP server response */
  بدل(ftpc->state) {
  حالة FTP_WAIT220:
    نتیجه = ftp_wait_resp(داده, conn, ftpc, ftpcode);
    اكسر;

  حالة FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    إذا(pp->سرریز)
      أرجع CURLE_WEIRD_SERVER_پاسخ; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    إذا((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool انجامشده;
      إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        نتیجه = Curl_ssl_cfilter_جمع_کن(
          داده, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        إذا(نتیجه) {
          /* we failed and bail out */
          أرجع CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, TRUE, &انجامشده);
      إذا(!نتیجه) {
        conn->بیتها.ftp_use_داده_ssl = FALSE; /* clear-text data */
        نتیجه = ftp_state_user(داده, ftpc, conn);
      }
    }
    وإلا إذا(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    وإلا {
      إذا(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        نتیجه = CURLE_USE_SSL_FAILED;
      وإلا
        /* ignore the failure and continue */
        نتیجه = ftp_state_user(داده, ftpc, conn);
    }
    اكسر;

  حالة FTP_USER:
  حالة FTP_PASS:
    نتیجه = ftp_state_user_resp(داده, ftpc, ftpcode);
    اكسر;

  حالة FTP_ACCT:
    نتیجه = ftp_state_acct_resp(داده, ftpc, ftpcode);
    اكسر;

  حالة FTP_PBSZ:
    نتیجه =
      Curl_pp_sendf(داده, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    إذا(!نتیجه)
      ftp_state(داده, ftpc, FTP_PROT);
    اكسر;

  حالة FTP_PROT:
    إذا(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->بیتها.ftp_use_داده_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    وإلا إذا(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      أرجع CURLE_USE_SSL_FAILED;

    إذا(داده->مجموعه.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "CCC");
      إذا(!نتیجه)
        ftp_state(داده, ftpc, FTP_CCC);
    }
    وإلا
      نتیجه = ftp_state_pwd(داده, ftpc);
    اكسر;

  حالة FTP_CCC:
    إذا(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      نتیجه = Curl_ssl_cfilter_remove(داده, FIRSTSOCKET,
                                       (داده->مجموعه.ftp_ccc ==
                                        (بیعلامت حرف)CURLFTPSSL_CCC_فعال));
      إذا(نتیجه)
        failf(داده, "Failed to clear the command channel (CCC)");
    }
    إذا(!نتیجه)
      /* Then continue as normal */
      نتیجه = ftp_state_pwd(داده, ftpc);
    اكسر;

  حالة FTP_PWD:
    نتیجه = ftp_pwd_resp(داده, ftpc, ftpcode);
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
        أرجع CURLE_بیرون_از_حافظه;

      /* Check for special servers here. */
      إذا(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        إذا(نتیجه) {
          curlx_free(os);
          أرجع نتیجه;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(داده, ftpc, FTP_NAMEFMT);
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

    ftp_state(داده, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(داده, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_NAMEFMT:
    إذا(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(داده, ftpc);
      اكسر;
    }

    ftp_state(داده, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(داده, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    اكسر;

  حالة FTP_QUOTE:
  حالة FTP_POSTQUOTE:
  حالة FTP_RETR_PREQUOTE:
  حالة FTP_STOR_PREQUOTE:
  حالة FTP_LIST_PREQUOTE:
    إذا((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(داده, "QUOT command failed with %03d", ftpcode);
      نتیجه = CURLE_QUOTE_ERROR;
    }
    وإلا
      نتیجه = ftp_state_quote(داده, ftpc, ftp, FALSE, ftpc->state);
    اكسر;

  حالة FTP_CWD:
    إذا(ftpcode / 100 != 2) {
      /* failure to CWD there */
      إذا(داده->مجموعه.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (داده->مجموعه.ftp_create_missing_dirs == 2) ? 1 : 0;

        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        إذا(!نتیجه)
          ftp_state(داده, ftpc, FTP_MKD);
      }
      وإلا {
        /* return failure */
        failf(داده, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        نتیجه = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    وإلا {
      /* success */
      ftpc->count2 = 0;
      إذا(ftpc->cwdcount >= ftpc->dirdepth)
        نتیجه = ftp_state_mdtm(داده, ftpc, ftp);
      وإلا {
        ftpc->cwdcount++;
        /* send next CWD */
        نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    اكسر;

  حالة FTP_MKD:
    إذا((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(داده, "Failed to MKD dir: %03d", ftpcode);
      نتیجه = CURLE_REMOTE_ACCESS_DENIED;
    }
    وإلا {
      ftp_state(داده, ftpc, FTP_CWD);
      /* send CWD */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    اكسر;

  حالة FTP_MDTM:
    نتیجه = ftp_state_mdtm_resp(داده, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_TYPE:
  حالة FTP_LIST_TYPE:
  حالة FTP_RETR_TYPE:
  حالة FTP_STOR_TYPE:
  حالة FTP_RETR_LIST_TYPE:
    نتیجه = ftp_state_type_resp(داده, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_اندازه:
  حالة FTP_RETR_اندازه:
  حالة FTP_STOR_اندازه:
    نتیجه = ftp_state_اندازه_resp(داده, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_REST:
  حالة FTP_RETR_REST:
    نتیجه = ftp_state_rest_resp(داده, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_PRET:
    إذا(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(داده, "PRET command not accepted: %03d", ftpcode);
      أرجع CURLE_FTP_PRET_FAILED;
    }
    نتیجه = ftp_state_use_pasv(داده, ftpc, conn);
    اكسر;

  حالة FTP_PASV:
    نتیجه = ftp_state_pasv_resp(داده, ftpc, ftpcode);
    اكسر;

  حالة FTP_پورت:
    نتیجه = ftp_state_پورت_resp(داده, ftpc, ftp, ftpcode);
    اكسر;

  حالة FTP_LIST:
  حالة FTP_RETR:
    نتیجه = ftp_state_get_resp(داده, ftpc, ftp, ftpcode, ftpc->state);
    اكسر;

  حالة FTP_STOR:
    نتیجه = ftp_state_stor_resp(داده, ftpc, ftpcode);
    اكسر;

  حالة FTP_QUIT:
  افتراضي:
    /* internal error */
    ftp_state(داده, ftpc, FTP_STOP);
    اكسر;
  }

  أرجع نتیجه;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ftp_multi_statemach(بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  أرجع ftpc ? ftp_statemach(داده, ftpc, انجامشده) : CURLE_FAILED_INIT;
}

ساكن CURLcode ftp_بلوک_statemach(بنية Curl_easy *داده,
                                    بنية ftp_conn *ftpc)
{
  بنية pingpong *pp = &ftpc->pp;
  CURLcode نتیجه = CURLE_OK;

  طالما(ftpc->state != FTP_STOP) {
    إذا(ftpc->shutdown)
      CURL_TRC_FTP(داده, "in shutdown, waiting for server response");
    نتیجه = Curl_pp_statemach(داده, pp, TRUE, TRUE /* disconnecting */);
    إذا(نتیجه)
      اكسر;
  }

  أرجع نتیجه;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ساكن CURLcode ftp_connect(بنية Curl_easy *داده,
                            bool *انجامشده) /* see description above */
{
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  بنية pingpong *pp;

  *انجامشده = FALSE; /* default to not done yet */
  إذا(!ftpc)
    أرجع CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_راهاندازی(pp, ftp_pp_statemachine, ftp_endofresp);

  إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, TRUE, انجامشده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  Curl_pp_init(pp, Curl_pgrs_now(داده)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(داده, ftpc, FTP_WAIT220);

  نتیجه = ftp_statemach(داده, ftpc, انجامشده);

  أرجع نتیجه;
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
ساكن CURLcode ftp_sendquote(بنية Curl_easy *داده,
                              بنية ftp_conn *ftpc,
                              بنية curl_slist *quote)
{
  بنية curl_slist *قلم;
  بنية pingpong *pp = &ftpc->pp;

  قلم = quote;
  طالما(قلم) {
    إذا(قلم->داده) {
      size_t nread;
      ثابت حرف *cmd = قلم->داده;
      bool acceptfail = FALSE;
      CURLcode نتیجه;
      صحيح ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      إذا(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", cmd);
      إذا(!نتیجه) {
        pp->response = *Curl_pgrs_now(داده); /* timeout relative now */
        نتیجه = getftpresponse(داده, &nread, &ftpcode);
      }
      إذا(نتیجه)
        أرجع نتیجه;

      إذا(!acceptfail && (ftpcode >= 400)) {
        failf(داده, "QUOT string not accepted: %s", cmd);
        أرجع CURLE_QUOTE_ERROR;
      }
    }

    قلم = قلم->بعدی;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode ftp_انجامشده_وضعیت(بنية Curl_easy *داده,
                                بنية ftp_conn *ftpc, CURLcode وضعیت,
                                bool premature)
{
  بدل(وضعیت) {
  حالة CURLE_BAD_DOWNLOAD_RESUME:
  حالة CURLE_FTP_WEIRD_PASV_پاسخ:
  حالة CURLE_FTP_پورت_FAILED:
  حالة CURLE_FTP_ACCEPT_FAILED:
  حالة CURLE_FTP_ACCEPT_TIMEOUT:
  حالة CURLE_FTP_COULDNT_مجموعه_TYPE:
  حالة CURLE_FTP_COULDNT_RETR_FILE:
  حالة CURLE_PARTIAL_FILE:
  حالة CURLE_UPLOAD_FAILED:
  حالة CURLE_REMOTE_ACCESS_DENIED:
  حالة CURLE_FILESIZE_EXCEEDED:
  حالة CURLE_REMOTE_FILE_NOT_یافت:
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
    CURL_TRC_FTP(داده, "FTP ended with bad error code");
    connclose(داده->conn);
    أرجع وضعیت;      /* use the already set error code */
  }
  أرجع CURLE_OK;
}

ساكن فراغ ftp_انجامشده_wildcard(بنية Curl_easy *داده, بنية ftp_conn *ftpc)
{
  إذا(داده->state.wildcardmatch) {
    إذا(داده->مجموعه.chunk_انتها && ftpc->file) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, easy_chunk_انتها);
      داده->مجموعه.chunk_انتها(داده->مجموعه.wildcardptr);
      CURL_CBAPI_انتها(&guard);
      freedirs(ftpc);
    }
    ftpc->معلوم_filesize = -1;
  }
}

ساكن فراغ ftp_انجامشده_مسیر(بنية Curl_easy *داده, بنية ftp_conn *ftpc,
                          CURLcode نتیجه)
{
  بنية connectdata *conn = داده->conn;
  إذا(نتیجه) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  وإلا { /* remember working directory for connection reuse */
    ثابت حرف *raw_مسیر = ftpc->rawpath;
    إذا(raw_مسیر) {
      إذا((داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD) && (raw_مسیر[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      وإلا {
        size_t مسیر_Len = طول_رشته(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        إذا(!ftpc->cwdfail) {
          إذا(داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD)
            مسیر_Len = 0; /* relative path => working directory is FTP home */
          وإلا
            /* file is URL-decoded */
            مسیر_Len -= ftpc->file ? طول_رشته(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_مسیر, مسیر_Len);
        }
        وإلا
          ftpc->prevpath = عدم; /* no path */
      }
    }
    إذا(ftpc->prevpath)
      infof(داده, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ساكن CURLcode ftp_انجامشده_secondary_socket(بنية Curl_easy *داده,
                                          بنية ftp_conn *ftpc,
                                          CURLcode نتیجه)
{
  بنية connectdata *conn = داده->conn;
  إذا(Curl_conn_is_راهاندازی(conn, SECONDARYSOCKET)) {
    إذا(!نتیجه && ftpc->dont_check && داده->req.maxdownload > 0) {
      /* partial download completed */
      نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "ABOR");
      إذا(نتیجه) {
        failf(داده, "Failure sending ABOR command: %s",
              curl_easy_strerror(نتیجه));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(داده, ftpc);
  }
  أرجع نتیجه;
}

ساكن CURLcode ftp_انجامشده_control_پاسخ(بنية Curl_easy *داده,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode نتیجه,
                                       bool premature)
{
  بنية connectdata *conn = داده->conn;
  size_t nread;
  صحيح ftpcode;

  إذا(!نتیجه && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(داده); /* timeout relative now */
    نتیجه = getftpresponse(داده, &nread, &ftpcode);

    إذا(!nread && (نتیجه == CURLE_OPERATION_TIMEDOUT)) {
      failf(داده, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    إذا(نتیجه)
      أرجع نتیجه;

    إذا(ftpc->dont_check && داده->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(داده, "partial download completed, closing connection");
      connclose(conn);
      أرجع نتیجه;
    }

    إذا(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      بدل(ftpcode) {
      حالة 226:
      حالة 250:
        اكسر;
      حالة 552:
        failf(داده, "Exceeded storage allocation");
        نتیجه = CURLE_REMOTE_DISK_پر;
        اكسر;
      افتراضي:
        failf(داده, "server did not report OK, got %d", ftpcode);
        نتیجه = CURLE_PARTIAL_FILE;
        اكسر;
      }
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode ftp_انجامشده_check_partial(بنية Curl_easy *داده,
                                       بنية ftp_conn *ftpc,
                                       بنية FTP *ftp, CURLcode نتیجه,
                                       bool premature)
{
  إذا(نتیجه || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  وإلا إذا(داده->state.upload) {
    إذا((ftp->transfer == PPTRANSFER_BODY) &&
       (داده->state.infilesize != -1) && /* upload with known size */
       ((!داده->مجموعه.crlf && !داده->state.prefer_ascii && /* no conversion */
         (داده->state.infilesize != داده->req.writebytecount)) ||
        ((داده->مجموعه.crlf || داده->state.prefer_ascii) && /* maybe CRLF conv */
         (داده->state.infilesize > داده->req.writebytecount))
       )) {
      failf(داده, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            داده->req.writebytecount, داده->state.infilesize);
      نتیجه = CURLE_PARTIAL_FILE;
    }
  }
  وإلا {
    إذا((داده->req.اندازه != -1) &&
       (داده->req.اندازه != داده->req.bytecount) &&
       (داده->req.maxdownload != داده->req.bytecount)) {
      failf(داده, "Received only partial file: %" FMT_OFF_T " bytes",
            داده->req.bytecount);
      نتیجه = CURLE_PARTIAL_FILE;
    }
    وإلا إذا(!ftpc->dont_check &&
            !داده->req.bytecount &&
            (داده->req.اندازه > 0)) {
      failf(داده, "No data was received");
      نتیجه = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  أرجع نتیجه;
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
ساكن CURLcode ftp_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                         bool premature)
{
  بنية FTP *ftp = Curl_meta_get(داده, CURL_META_FTP_EASY);
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  CURLcode نتیجه;

  إذا(!ftp || !ftpc)
    أرجع CURLE_OK;

  نتیجه = ftp_انجامشده_وضعیت(داده, ftpc, وضعیت, premature);

  ftp_انجامشده_wildcard(داده, ftpc);
  ftp_انجامشده_مسیر(داده, ftpc, نتیجه);
  نتیجه = ftp_انجامشده_secondary_socket(داده, ftpc, نتیجه);
  نتیجه = ftp_انجامشده_control_پاسخ(داده, ftpc, ftp, نتیجه, premature);
  نتیجه = ftp_انجامشده_check_partial(داده, ftpc, ftp, نتیجه, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  إذا(!وضعیت && !نتیجه && !premature && داده->مجموعه.postquote)
    نتیجه = ftp_sendquote(داده, ftpc, داده->مجموعه.postquote);
  CURL_TRC_FTP(داده, "[%s] done, result=%d", FTP_CSTATE(ftpc), (صحيح)نتیجه);
  أرجع نتیجه;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ساكن CURLcode ftp_nb_type(بنية Curl_easy *داده,
                            بنية ftp_conn *ftpc,
                            بنية FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode نتیجه;
  حرف want = (حرف)(ascii ? 'A' : 'I');

  إذا(ftpc->transfertype == want) {
    ftp_state(داده, ftpc, newstate);
    أرجع ftp_state_type_resp(داده, ftpc, ftp, 200, newstate);
  }

  نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "TYPE %c", want);
  إذا(!نتیجه) {
    ftp_state(داده, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  أرجع نتیجه;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ساكن CURLcode ftp_perform(
  بنية Curl_easy *داده,
  بنية ftp_conn *ftpc,
  بنية FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_انجامشده)
{
  /* this is FTP and no proxy */
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_FTP(داده, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  إذا(داده->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_انجامشده = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  نتیجه = ftp_state_quote(داده, ftpc, ftp, TRUE, FTP_QUOTE);
  إذا(نتیجه)
    أرجع نتیجه;

  /* run the state-machine */
  نتیجه = ftp_statemach(داده, ftpc, dophase_انجامشده);

  *connected = Curl_conn_is_connected(داده->conn, SECONDARYSOCKET);

  إذا(*connected)
    infof(داده, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  وإلا
    CURL_TRC_FTP(داده, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  إذا(*dophase_انجامشده)
    CURL_TRC_FTP(داده, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  أرجع نتیجه;
}

ساكن فراغ wc_داده_dtor(فراغ *ptr)
{
  بنية ftp_wc *ftpwc = ptr;
  إذا(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_داده_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ساكن CURLcode init_wc_داده(بنية Curl_easy *داده,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  حرف *آخرین_slash;
  حرف *مسیر = ftp->مسیر;
  بنية Wildcard_داده *wildcard = داده->wildcard;
  CURLcode نتیجه = CURLE_OK;
  بنية ftp_wc *ftpwc = عدم;

  آخرین_slash = strrchr(ftp->مسیر, '/');
  إذا(آخرین_slash) {
    آخرین_slash++;
    إذا(آخرین_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_مسیر(داده, ftpc, ftp);
    }
    wildcard->الگو = curlx_strdup(آخرین_slash);
    إذا(!wildcard->الگو)
      أرجع CURLE_بیرون_از_حافظه;
    آخرین_slash[0] = '\0'; /* cut file from path */
  }
  وإلا { /* there is only 'wildcard pattern' or nothing */
    إذا(مسیر[0]) {
      wildcard->الگو = curlx_strdup(مسیر);
      إذا(!wildcard->الگو)
        أرجع CURLE_بیرون_از_حافظه;
      مسیر[0] = '\0';
    }
    وإلا { /* only list */
      wildcard->state = CURLWC_CLEAN;
      أرجع ftp_parse_url_مسیر(داده, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, حجم(بنية ftp_wc));
  إذا(!ftpwc) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_داده_alloc();
  إذا(!ftpwc->parser) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_داده_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  إذا(داده->مجموعه.ftp_filemethod == FTPFILE_NOCWD)
    داده->مجموعه.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  نتیجه = ftp_parse_url_مسیر(داده, ftpc, ftp);
  إذا(نتیجه) {
    اذهب fail;
  }

  wildcard->مسیر = curlx_strdup(ftp->مسیر);
  إذا(!wildcard->مسیر) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  infof(داده, "Wildcard - Parsing started");
  أرجع CURLE_OK;

fail:
  إذا(ftpwc) {
    Curl_ftp_parselist_داده_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->الگو);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = عدم;
  أرجع نتیجه;
}

ساكن CURLcode wc_statemach(بنية Curl_easy *داده,
                             بنية ftp_conn *ftpc,
                             بنية FTP *ftp)
{
  بنية Wildcard_داده * ثابت wildcard = داده->wildcard;
  CURLcode نتیجه = CURLE_OK;

  لكل(;;) {
    بدل(wildcard->state) {
    حالة CURLWC_INIT:
      نتیجه = init_wc_داده(داده, ftpc, ftp);
      إذا(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        أرجع نتیجه;
      wildcard->state = نتیجه ? CURLWC_ERROR : CURLWC_MATCHING;
      أرجع نتیجه;

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
      إذا(Curl_llist_شمار(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        أرجع CURLE_REMOTE_FILE_NOT_یافت;
      }
      استمر;
    }

    حالة CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      بنية Curl_llist_گره *سر = Curl_llist_سر(&wildcard->filelist);
      بنية curl_fileinfo *finfo = Curl_گره_elem(سر);
      حرف *tmp_مسیر;
      حرف *enc = curl_easy_escape(عدم, finfo->filename, 0);
      إذا(!enc)
        أرجع CURLE_بیرون_از_حافظه;

      tmp_مسیر = curl_maprintf("%s%s", wildcard->مسیر, enc);
      curl_free(enc);
      إذا(!tmp_مسیر)
        أرجع CURLE_بیرون_از_حافظه;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->مسیر = tmp_مسیر;

      infof(داده, "Wildcard - START of \"%s\"", finfo->filename);
      إذا(داده->مجموعه.chunk_bgn) {
        دراز userresponse;
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_شروع(&guard, داده, easy_chunk_bgn);
        userresponse = داده->مجموعه.chunk_bgn(
          finfo, داده->مجموعه.wildcardptr,
          (صحيح)Curl_llist_شمار(&wildcard->filelist));
        CURL_CBAPI_انتها(&guard);
        بدل(userresponse) {
        حالة CURL_CHUNK_BGN_FUNC_SKIP:
          infof(داده, "Wildcard - \"%s\" skipped by user", finfo->filename);
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

      إذا(finfo->flags & CURLFINFOFLAG_معلوم_اندازه)
        ftpc->معلوم_filesize = finfo->اندازه;

      نتیجه = ftp_parse_url_مسیر(داده, ftpc, ftp);
      إذا(نتیجه)
        أرجع نتیجه;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_گره_remove(Curl_llist_سر(&wildcard->filelist));

      إذا(Curl_llist_شمار(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        أرجع CURLE_OK;
      }
      أرجع نتیجه;
    }

    حالة CURLWC_SKIP: {
      إذا(داده->مجموعه.chunk_انتها) {
        بنية Curl_mapi_guard guard;
        CURL_CBAPI_شروع(&guard, داده, easy_chunk_انتها);
        داده->مجموعه.chunk_انتها(داده->مجموعه.wildcardptr);
        CURL_CBAPI_انتها(&guard);
      }
      Curl_گره_remove(Curl_llist_سر(&wildcard->filelist));
      wildcard->state = (Curl_llist_شمار(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      استمر;
    }

    حالة CURLWC_CLEAN: {
      بنية ftp_wc *ftpwc = wildcard->ftpwc;
      نتیجه = CURLE_OK;
      إذا(ftpwc)
        نتیجه = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = نتیجه ? CURLWC_ERROR : CURLWC_انجامشده;
      أرجع نتیجه;
    }

    حالة CURLWC_انجامشده:
    حالة CURLWC_ERROR:
    حالة CURLWC_CLEAR:
      إذا(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = عدم;
      }
      أرجع نتیجه;
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
ساكن CURLcode ftp_regular_transfer(بنية Curl_easy *داده,
                                     بنية ftp_conn *ftpc,
                                     بنية FTP *ftp,
                                     bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  bool connected = FALSE;
  داده->req.اندازه = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(داده);

  ftpc->ctl_valid = TRUE; /* starts good */

  نتیجه = ftp_perform(داده, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_انجامشده); /* all commands in the DO-phase done? */

  إذا(!نتیجه) {

    إذا(!*dophase_انجامشده)
      /* the DO phase has not completed yet */
      أرجع CURLE_OK;

    نتیجه = ftp_dophase_انجامشده(داده, ftpc, ftp, connected);

    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
    freedirs(ftpc);

  أرجع نتیجه;
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
ساكن CURLcode ftp_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(داده, CURL_META_FTP_EASY);

  *انجامشده = FALSE; /* default to false */
  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  ftpc->wait_داده_conn = FALSE; /* default to no such wait */

#اگر_تعریف_شده CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    بنية Curl_cwriter *ftp_lc_writer;

    نتیجه = Curl_cwriter_create(&ftp_lc_writer, داده, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    إذا(نتیجه)
      أرجع نتیجه;

    نتیجه = Curl_cwriter_جمع_کن(داده, ftp_lc_writer);
    إذا(نتیجه) {
      Curl_cwriter_free(داده, ftp_lc_writer);
      أرجع نتیجه;
    }
  }
#پایان_اگر /* CURL_PREFER_LF_LINEENDS */

  إذا(داده->state.wildcardmatch) {
    نتیجه = wc_statemach(داده, ftpc, ftp);
    إذا(داده->wildcard->state == CURLWC_SKIP ||
       داده->wildcard->state == CURLWC_انجامشده) {
      /* do not call ftp_regular_transfer */
      أرجع CURLE_OK;
    }
    إذا(نتیجه) /* error, loop or skipping the file */
      أرجع نتیجه;
  }
  وإلا { /* no wildcard FSM needed */
    نتیجه = ftp_parse_url_مسیر(داده, ftpc, ftp);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  نتیجه = ftp_regular_transfer(داده, ftpc, ftp, انجامشده);

  أرجع نتیجه;
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
ساكن CURLcode ftp_quit(بنية Curl_easy *داده,
                         بنية ftp_conn *ftpc)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(ftpc->ctl_valid) {
    CURL_TRC_FTP(داده, "sending QUIT to close session");
    نتیجه = Curl_pp_sendf(داده, &ftpc->pp, "%s", "QUIT");
    إذا(نتیجه) {
      failf(داده, "Failure sending QUIT command: %s",
            curl_easy_strerror(نتیجه));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(داده->conn); /* mark for closure */
      ftp_state(داده, ftpc, FTP_STOP);
      أرجع نتیجه;
    }

    ftp_state(داده, ftpc, FTP_QUIT);

    نتیجه = ftp_بلوک_statemach(داده, ftpc);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode ftp_disconnect(بنية Curl_easy *داده,
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
  إذا(dead_connection || Curl_pp_needs_flush(داده, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (فراغ)ftp_quit(داده, ftpc); /* ignore errors on the QUIT */
  أرجع CURLE_OK;
}

/* called from multi.c while DOing */
ساكن CURLcode ftp_doing(بنية Curl_easy *داده,
                          bool *dophase_انجامشده)
{
  بنية ftp_conn *ftpc = Curl_conn_meta_get(داده->conn, CURL_META_FTP_CONN);
  بنية FTP *ftp = Curl_meta_get(داده, CURL_META_FTP_EASY);
  CURLcode نتیجه;

  إذا(!ftpc || !ftp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = ftp_statemach(داده, ftpc, dophase_انجامشده);

  إذا(نتیجه)
    CURL_TRC_FTP(داده, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  وإلا إذا(*dophase_انجامشده) {
    نتیجه = ftp_dophase_انجامشده(داده, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(داده, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  أرجع نتیجه;
}

ساكن فراغ ftp_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية FTP *ftp = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ساكن فراغ ftp_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية ftp_conn *ftpc = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_مقصد_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ساكن فراغ type_url_check(بنية Curl_easy *داده, بنية FTP *ftp)
{
  size_t len = طول_رشته(ftp->مسیر);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  إذا((len >= 7) && !memcmp(&ftp->مسیر[len - 7], ";type=", 6)) {
    حرف *type = &ftp->مسیر[len - 7];
    حرف command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    بدل(command) {
    حالة 'A': /* ASCII mode */
      داده->state.prefer_ascii = TRUE;
      اكسر;

    حالة 'D': /* directory mode */
      داده->state.list_only = TRUE;
      اكسر;

    حالة 'I': /* binary mode */
    افتراضي:
      /* switch off ASCII */
      داده->state.prefer_ascii = FALSE;
      اكسر;
    }
  }
}

ساكن CURLcode ftp_راهاندازی_connection(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  بنية FTP *ftp;
  CURLcode نتیجه = CURLE_OK;
  بنية ftp_conn *ftpc;

  ftp = curlx_calloc(1, حجم(*ftp));
  إذا(!ftp ||
     Curl_meta_مجموعه(داده, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  ftpc = curlx_calloc(1, حجم(*ftpc));
  إذا(!ftpc ||
     Curl_conn_meta_مجموعه(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  /* clone connection related data that is FTP specific */
  إذا(CURL_EASY_STR(داده, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(داده, STRING_FTP_ACCOUNT));
    إذا(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }
  إذا(CURL_EASY_STR(داده, STRING_FTP_ALTERNATIVE_مقصد_USER)) {
    ftpc->alternative_مقصد_user =
      curlx_strdup(CURL_EASY_STR(داده, STRING_FTP_ALTERNATIVE_مقصد_USER));
    إذا(!ftpc->alternative_مقصد_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  ftp->مسیر = &داده->state.به_بالا.مسیر[1]; /* do not include the initial slash */

  type_url_check(داده, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->معلوم_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = داده->مجموعه.use_ssl;
  ftpc->ccc = داده->مجموعه.ftp_ccc;

  CURL_TRC_FTP(داده, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (صحيح)نتیجه);
  أرجع نتیجه;
}

bool Curl_ftp_conns_تطابق(بنية connectdata *needle, بنية connectdata *conn)
{
  بنية ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  بنية ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  إذا(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_مقصد_user,
                     cftpc->alternative_مقصد_user) ||
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
  ftp_راهاندازی_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_انجامشده,                        /* done */
  ftp_do_بیشتر,                     /* do_more */
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

#پایان_اگر /* CURL_DISABLE_FTP */
