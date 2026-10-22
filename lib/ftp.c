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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

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

#اگر_غیر_تعریف_شدہ NI_MAXHOST
#تعریف_کرو NI_MAXHOST 1025
#اگر_ختم
#اگر_غیر_تعریف_شدہ INET_ADDRSTRLEN
#تعریف_کرو INET_ADDRSTRLEN 16
#اگر_ختم

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#تعریف_کرو STATUSCODE(سطر) \
  (ISDIGIT((سطر)[0]) && ISDIGIT((سطر)[1]) && ISDIGIT((سطر)[2]))

/* macro to check for the last line in an FTP server response */
#تعریف_کرو LASTLINE(سطر) (STATUSCODE(سطر) && (' ' == (سطر)[3]))

#اگر_تعریف_شدہ CURLVERBOSE
/* for tracing purposes */
ساکن ثابت حرف * ثابت ftp_state_نام[] = {
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
#تعریف_کرو FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_نام[(ftpc)->state] : "???")

#اگر_ختم /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ساکن خالی ftp_state_کم(ساخت Curl_easy *ڈیٹا,
                          ساخت ftp_conn *ftpc,
                          ftpstate newstate
#اگر_تعریف_شدہ DEBUGBUILD
                          , صحیح lineno
#اگر_ختم
  )
{
  اگر(ftpc->state != newstate) {
#اگر_تعریف_شدہ DEBUGBUILD
    NOVERBOSE((خالی)lineno);
    CURL_TRC_FTP(ڈیٹا, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_نام[newstate], lineno);
#ورنہ
    CURL_TRC_FTP(ڈیٹا, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_نام[newstate]);
#اگر_ختم
  }
  ftpc->state = newstate;
}

/* Local API functions */
#اگر_غیر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو ftp_state(x, y, z) ftp_state_کم(x, y, z)
#ورنہ /* !DEBUGBUILD */
#تعریف_کرو ftp_state(x, y, z) ftp_state_کم(x, y, z, __LINE__)
#اگر_ختم /* DEBUGBUILD */

ساکن CURLcode ftp_state_mdtm(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp);
ساکن CURLcode ftp_state_quote(ساخت Curl_easy *ڈیٹا,
                                ساخت ftp_conn *ftpc,
                                ساخت FTP *ftp,
                                bool init, ftpstate instate);
ساکن CURLcode ftp_nb_type(ساخت Curl_easy *ڈیٹا,
                            ساخت ftp_conn *ftpc,
                            ساخت FTP *ftp,
                            bool ascii, ftpstate newstate);
ساکن CURLcode getftpresponse(ساخت Curl_easy *ڈیٹا, size_t *nreadp,
                               صحیح *ftpcodep);

ساکن خالی freedirs(ساخت ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = تہی;
}

ساکن size_t numof_slashes(ثابت حرف *str)
{
  ثابت حرف *slashPos;
  size_t عدد = 0;
  کرو {
    slashPos = strchr(str, '/');
    اگر(slashPos) {
      ++عدد;
      str = slashPos + 1;
    }
  } جب(slashPos);
  واپس عدد;
}

#تعریف_کرو FTP_MAX_DIR_گہرائی 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساکن CURLcode ftp_parse_url_راستہ(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc,
                                   ساخت FTP *ftp)
{
  ثابت حرف *slashPos = تہی;
  ثابت حرف *fileName = تہی;
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *raw_راستہ = تہی; /* URL-decoded "raw" path */
  size_t راستہ_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  اگر(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  نتیجہ = Curl_urldecode(ftp->راستہ, 0, &ftpc->rawpath, &راستہ_Len, REJECT_CTRL);
  اگر(نتیجہ) {
    failf(ڈیٹا, "path contains control characters");
    واپس نتیجہ;
  }
  raw_راستہ = ftpc->rawpath;

  انتخاب(ڈیٹا->سیٹ.ftp_filemethod) {
  حالت FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    اگر((راستہ_Len > 0) && (raw_راستہ[راستہ_Len - 1] != '/'))
      fileName = raw_راستہ;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    توڑو;

  حالت FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_راستہ, '/');
    اگر(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_راستہ;
      اگر(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, حجم(ftpc->dirs[0]));
      اگر(!ftpc->dirs)
        واپس CURLE_برآمد_کا_حافظہ;

      ftpc->dirs[0].آغاز = 0;
      ftpc->dirs[0].len = (صحیح)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ورنہ
      fileName = raw_راستہ; /* filename only (or empty) */
    توڑو;

  طےشدہ: /* allow pretty much anything */
  حالت FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ثابت حرف *curPos = raw_راستہ;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_راستہ);

    اگر(dirAlloc >= FTP_MAX_DIR_گہرائی)
      /* suspiciously deep directory hierarchy */
      واپس CURLE_URL_MALFORMAT;

    اگر(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, حجم(ftpc->dirs[0]));
      اگر(!ftpc->dirs)
        واپس CURLE_برآمد_کا_حافظہ;

      /* parse the URL path into separate path components */
      جب(dirAlloc--) {
        ثابت حرف *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        اگر(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        اگر(clen) {
          ftpc->dirs[ftpc->dirdepth].آغاز = (صحیح)(curPos - raw_راستہ);
          ftpc->dirs[ftpc->dirdepth].len = (صحیح)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    توڑو;
  } /* switch */

  اگر(fileName && *fileName)
    ftpc->file = fileName;
  ورنہ
    ftpc->file = تہی; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  اگر(ڈیٹا->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(ڈیٹا, "Uploading to a URL without a filename");
    واپس CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  اگر((ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && (raw_راستہ[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ورنہ { /* newly created FTP connections are already in entry path */
    ثابت حرف *old_راستہ = ڈیٹا->conn->بٹ.reuse ? ftpc->prevpath : "";
    اگر(old_راستہ) {
      size_t n = راستہ_Len;
      اگر(ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ورنہ
        n -= ftpc->file ? لڑی_لمبائی(ftpc->file) : 0;

      اگر((لڑی_لمبائی(old_راستہ) == n) && raw_راستہ && !لڑی_موازنہ(raw_راستہ, old_راستہ, n)) {
        infof(ڈیٹا, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ساکن صحیح ftp_need_type(ساخت ftp_conn *ftpc,
                         bool ascii_wanted)
{
  واپس ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ساکن خالی close_secondarysocket(ساخت Curl_easy *ڈیٹا,
                                  ساخت ftp_conn *ftpc)
{
  (خالی)ftpc;
  CURL_TRC_FTP(ڈیٹا, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(ڈیٹا, ڈیٹا->conn, SECONDARYSOCKET);
}

#اگر_تعریف_شدہ CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
ساخت ftp_cw_lc_ctx {
  ساخت Curl_cwriter super;
  bool newline_pending;
};

ساکن CURLcode ftp_cw_lc_write(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_cwriter *writer, صحیح type,
                                ثابت حرف *buf, size_t blen)
{
  ساکن ثابت حرف nl = '\n';
  ساخت ftp_cw_lc_ctx *ctx = writer->ctx;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    واپس CURLE_FAILED_INIT;

  اگر(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  جب(blen) {
    /* do not pass EOS when writing parts */
    صحیح chunk_type = (type & ~CLIENTWRITE_EOS);
    ثابت حرف *cp;
    size_t chunk_len;
    CURLcode نتیجہ;

    اگر(ctx->newline_pending) {
      اگر(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, chunk_type, &nl, 1);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    اگر(!cp)
      توڑو;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    اگر(chunk_len) {
      نتیجہ = Curl_cwriter_write(ڈیٹا, writer->اگلا, chunk_type,
                                  buf, chunk_len);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  اگر(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, blen);
  }
  ورنہ اگر(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    اگر(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, 0);
  }
  واپس CURLE_OK;
}

ساکن ثابت ساخت Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  تہی,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(ساخت ftp_cw_lc_ctx)
};

#اگر_ختم /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ساکن CURLcode ftp_check_ctrl_on_ڈیٹا_wait(ساخت Curl_easy *ڈیٹا,
                                            ساخت ftp_conn *ftpc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ساخت pingpong *pp = &ftpc->pp;
  size_t nread;
  صحیح ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  اگر(curlx_dyn_len(&pp->recvbuf)) {
    ثابت حرف *l = curlx_dyn_ptr(&pp->recvbuf);
    اگر(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(ڈیٹا, "There is negative response in cache while serv connect");
      (خالی)getftpresponse(ڈیٹا, &nread, &ftpcode);
      واپس CURLE_FTP_ACCEPT_FAILED;
    }
  }

  اگر(pp->حد_سے_تجاوز)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ورنہ {
    صحیح socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    انتخاب(socketstate) {
    حالت -1: /* error */
      /* let's die here */
      failf(ڈیٹا, "Error while waiting for server connect");
      واپس CURLE_FTP_ACCEPT_FAILED;
    طےشدہ:
      اگر(socketstate & CURL_CSELECT_اندر)
        response = TRUE;
      توڑو;
    }
  }

  اگر(response) {
    infof(ڈیٹا, "Ctrl conn has data while waiting for data conn");
    اگر(pp->حد_سے_تجاوز > 3) {
      ثابت حرف *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->حد_سے_تجاوز + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      اگر((len > 3) && LASTLINE(r)) {
        curl_off_t حالت_2;
        اگر(!curlx_str_نمبر(&r, &حالت_2, 999) && (حالت_2 == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(ڈیٹا, "Got 226 before data activity");
          واپس CURLE_OK;
        }
      }
    }

    (خالی)getftpresponse(ڈیٹا, &nread, &ftpcode);

    infof(ڈیٹا, "FTP code: %03d", ftpcode);

    اگر(ftpcode / 100 > 3)
      واپس CURLE_FTP_ACCEPT_FAILED;

    واپس CURLE_WEIRD_SERVER_جواب;
  }

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ساکن CURLcode ftp_initiate_transfer(ساخت Curl_easy *ڈیٹا,
                                      ساخت ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(ڈیٹا, "ftp_initiate_transfer()");
  نتیجہ = Curl_conn_connect(ڈیٹا, SECONDARYSOCKET, TRUE, &connected);
  اگر(نتیجہ || !connected)
    واپس نتیجہ;

  اگر(ڈیٹا->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_تیاری_send(ڈیٹا, SECONDARYSOCKET);
    Curl_xfer_سیٹ_shutdown(ڈیٹا, TRUE, TRUE);
  }
  ورنہ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_تیاری_recv(ڈیٹا, SECONDARYSOCKET, ڈیٹا->req.سائز);
    Curl_xfer_سیٹ_shutdown(ڈیٹا, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(ڈیٹا, ftpc, FTP_STOP);

  واپس CURLE_OK;
}

ساکن bool ftp_endofresp(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                          ثابت حرف *سطر, size_t len, صحیح *رمز)
{
  curl_off_t حالت_2;
  (خالی)ڈیٹا;
  (خالی)conn;

  اگر((len > 3) && LASTLINE(سطر) && !curlx_str_نمبر(&سطر, &حالت_2, 999)) {
    *رمز = (صحیح)حالت_2;
    واپس TRUE;
  }

  واپس FALSE;
}

ساکن CURLcode ftp_readresp(ساخت Curl_easy *ڈیٹا,
                             ساخت ftp_conn *ftpc,
                             int8_t sockindex,
                             ساخت pingpong *pp,
                             صحیح *ftpcodep, /* return the ftp-code if done */
                             size_t *سائز) /* size of the response */
{
  صحیح رمز;
  CURLcode نتیجہ = Curl_pp_readresp(ڈیٹا, sockindex, pp, &رمز, سائز);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  اگر(!ftpc->shutdown)
    ڈیٹا->info.httpcode = رمز;

  *ftpcodep = رمز;

  اگر(رمز == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(ڈیٹا, "We got a 421 - timeout");
    ftp_state(ڈیٹا, ftpc, FTP_STOP);
    واپس CURLE_OPERATION_TIMEDOUT;
  }

  واپس نتیجہ;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ساکن CURLcode getftpresponse(ساخت Curl_easy *ڈیٹا,
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

  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode نتیجہ = CURLE_OK;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  ساخت pingpong *pp = &ftpc->pp;
  size_t nread;
  صحیح cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(ڈیٹا, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  اگر(!ftpc)
    واپس CURLE_FAILED_INIT;

  جب(!*ftpcodep && !نتیجہ) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(ڈیٹا, pp);
    timediff_t interval_ms;

    اگر(timeout <= 0) {
      failf(ڈیٹا, "FTP response timeout");
      واپس CURLE_OPERATION_TIMEDOUT; /* already too little time */
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
    ورنہ اگر(!Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(ڈیٹا, pp) ?
        sockfd : CURL_SOCKET_BAD;
      صحیح ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      اگر(ev < 0) {
        failf(ڈیٹا, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        واپس CURLE_RECV_ERROR;
      }
      ورنہ اگر(ev == 0) {
        نتیجہ = Curl_pgrsUpdate(ڈیٹا);
        جاری; /* continue in our loop for the timeout duration */
      }
    }

    اگر(Curl_pp_needs_flush(ڈیٹا, pp)) {
      نتیجہ = Curl_pp_flushsend(ڈیٹا, pp);
      اگر(نتیجہ)
        توڑو;
    }

    نتیجہ = ftp_readresp(ڈیٹا, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    اگر(نتیجہ)
      توڑو;

    اگر(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
ڈیٹا*/
      cache_skip++;
    ورنہ
      /* when we got data or there is no cache left, we reset the cache skip
شمار*/
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(ڈیٹا, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (صحیح)نتیجہ, *nreadp, *ftpcodep);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_user(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت connectdata *conn)
{
  CURLcode نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  اگر(!نتیجہ) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(ڈیٹا, ftpc, FTP_USER);
  }
  واپس نتیجہ;
}

ساکن CURLcode ftp_state_pwd(ساخت Curl_easy *ڈیٹا,
                              ساخت ftp_conn *ftpc)
{
  CURLcode نتیجہ;
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(!ڈیٹا->id && getenv("CURL_FTP_PWD_STOP"))
    واپس CURLE_OK;
#اگر_ختم
  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "PWD");
  اگر(!نتیجہ)
    ftp_state(ڈیٹا, ftpc, FTP_PWD);

  واپس نتیجہ;
}

/* For the FTP "protocol connect" and "doing" phases only */
ساکن CURLcode ftp_pollset(ساخت Curl_easy *ڈیٹا,
                            ساخت easy_pollset *ps)
{
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  واپس ftpc ? Curl_pp_pollset(ڈیٹا, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ساکن CURLcode ftp_domore_pollset(ساخت Curl_easy *ڈیٹا,
                                   ساخت easy_pollset *ps)
{
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    واپس CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(ڈیٹا, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  اگر(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(ڈیٹا->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (ڈیٹا->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(ڈیٹا->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    واپس Curl_pollset_جوڑو_اندر(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
  }
  واپس Curl_pp_pollset(ڈیٹا, &ftpc->pp, ps);
}

ساکن صحیح pathlen(ساخت ftp_conn *ftpc, صحیح عدد)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > عدد);
  واپس ftpc->dirs[عدد].len;
}

ساکن ثابت حرف *pathpiece(ساخت ftp_conn *ftpc, صحیح عدد)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > عدد);
  واپس &ftpc->rawpath[ftpc->dirs[عدد].آغاز];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ساکن CURLcode ftp_state_cwd(ساخت Curl_easy *ڈیٹا,
                              ساخت ftp_conn *ftpc,
                              ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ftpc->cwddone)
    /* already done and fine */
    نتیجہ = ftp_state_mdtm(ڈیٹا, ftpc, ftp);
  ورنہ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((ڈیٹا->سیٹ.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    اگر(ڈیٹا->conn->بٹ.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "CWD %s", ftpc->entrypath);
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_CWD);
    }
    ورنہ {
      اگر(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        اگر(!نتیجہ)
          ftp_state(ڈیٹا, ftpc, FTP_CWD);
      }
      ورنہ {
        /* No CWD necessary */
        نتیجہ = ftp_state_mdtm(ڈیٹا, ftpc, ftp);
      }
    }
  }
  واپس نتیجہ;
}

قسمتعریف شمار {
  EPRT,
  پورٹ_2,
  مکمل_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ساکن CURLcode ftp_پورٹ_parse_string(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn,
                                      ثابت حرف *string_ftpport,
                                      ساخت Curl_sockaddr_storage *ss,
                                      غیرعلامتی مختصر *پورٹ_minp,
                                      غیرعلامتی مختصر *پورٹ_maxp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen)
{
  ثابت حرف *ip_انتہا = تہی;
  ثابت حرف *addr = تہی;
  size_t addrlen = 0;
  غیرعلامتی مختصر پورٹ_min = 0;
  غیرعلامتی مختصر پورٹ_max = 0;
  حرف ipstr[50];
#اگر_غیر_تعریف_شدہ USE_IPV6
  (خالی)conn;
  (خالی)ss;
#اگر_ختم

  /* default to nothing */
  *hostp = تہی;
  *پورٹ_minp = *پورٹ_maxp = 0;

  اگر(!string_ftpport || (لڑی_لمبائی(string_ftpport) <= 1))
    جاؤ مکمل;

#اگر_تعریف_شدہ USE_IPV6
  اگر(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ثابت حرف *ip_آغاز = string_ftpport + 1;
    ip_انتہا = strchr(ip_آغاز, ']');
    اگر(ip_انتہا) {
      addrlen = ip_انتہا - ip_آغاز;
      addr = ip_آغاز;
    }
  }
  ورنہ
#اگر_ختم
    اگر(*string_ftpport == ':') {
      /* :port */
      ip_انتہا = string_ftpport;
    }
    ورنہ {
      ip_انتہا = strchr(string_ftpport, ':');
      addr = string_ftpport;
      اگر(ip_انتہا) {
#اگر_تعریف_شدہ USE_IPV6
        ساخت sockaddr_in6 * ثابت sa6 = (خالی *)ss;
#اگر_ختم
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_انتہا - string_ftpport;
#اگر_تعریف_شدہ USE_IPV6
        اگر(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = لڑی_لمبائی(string_ftpport);
          ip_انتہا = تہی; /* this got no port ! */
        }
#اگر_ختم
      }
      ورنہ
        /* ipv4|interface */
        addrlen = لڑی_لمبائی(string_ftpport);
    }

  /* parse the port */
  اگر(ip_انتہا) {
    ثابت حرف *portp = strchr(ip_انتہا, ':');
    اگر(portp) {
      curl_off_t آغاز;
      curl_off_t انتہا;
      portp++;
      اگر(!curlx_str_نمبر(&portp, &آغاز, 0xffff)) {
        پورٹ_min = (غیرعلامتی مختصر)آغاز;
        اگر(!curlx_str_single(&portp, '-') &&
           !curlx_str_نمبر(&portp, &انتہا, 0xffff))
          پورٹ_max = (غیرعلامتی مختصر)انتہا;
        ورنہ
          پورٹ_max = پورٹ_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  اگر(پورٹ_min > پورٹ_max)
    پورٹ_min = پورٹ_max = 0;

  اگر(addrlen) {
    ثابت ساخت Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(ڈیٹا, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    اگر(!remote_addr || (addrlen >= حجم(ipstr)) || (addrlen >= hbuflen))
      واپس CURLE_FTP_پورٹ_FAILED;
    حافظہ_نقل(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    انتخاب(Curl_if2ip(remote_addr->family,
#اگر_تعریف_شدہ USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#اگر_ختم
                      ipstr, hbuf, hbuflen)) {
    حالت IF2_IP_NOT_ملا:
      /* not an interface, use the string as hostname instead */
      حافظہ_نقل(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      توڑو;
    حالت IF2IP_AF_NOT_SUPPORTED:
      واپس CURLE_FTP_پورٹ_FAILED;
    حالت IF2_IP_ملا:
      *hostp = hbuf; /* use the hbuf for hostname */
      توڑو;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

مکمل:
  *پورٹ_minp = پورٹ_min;
  *پورٹ_maxp = پورٹ_max;
  واپس CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ساکن CURLcode ftp_پورٹ_default_host(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn,
                                      ساخت Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ثابت حرف **hostp,
                                      حرف *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ساخت sockaddr *sa = (ساخت sockaddr *)ss;
  ساخت sockaddr_in * ثابت sa4 = (خالی *)sa;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 * ثابت sa6 = (خالی *)sa;
#اگر_ختم
  حرف بفر[STRERROR_LEN];
  CURLcode نتیجہ;

  *sslenp = حجم(*ss);
  اگر(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(ڈیٹا, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    واپس CURLE_FTP_پورٹ_FAILED;
  }
  انتخاب(sa->sa_family) {
#اگر_تعریف_شدہ USE_IPV6
  حالت AF_INET6:
    نتیجہ = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    توڑو;
#اگر_ختم
  طےشدہ:
    نتیجہ = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    توڑو;
  }
  اگر(نتیجہ)
    واپس CURLE_FTP_پورٹ_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  واپس CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ساکن CURLcode ftp_پورٹ_resolve_host(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn,
                                      ثابت حرف *host,
                                      ساخت Curl_dns_اندراج **dns_entryp,
                                      ثابت ساخت Curl_addrinfo **resp)
{
  CURLcode نتیجہ;

  *resp = تہی;
  نتیجہ = Curl_resolv_blocking(
    ڈیٹا, Curl_resolv_dns_queries(ڈیٹا, conn->ip_نسخہ),
    host, 0, Curl_conn_get_transport(ڈیٹا, conn), dns_entryp);
  اگر(نتیجہ)
    failf(ڈیٹا, "failed to resolve the address provided to PORT: %s", host);
  ورنہ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  واپس نتیجہ;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ساکن CURLcode ftp_پورٹ_open_socket(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn,
                                     ثابت ساخت Curl_addrinfo *res,
                                     ثابت ساخت Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  حرف بفر[STRERROR_LEN];
  صحیح sockerr = 0;
  ثابت ساخت Curl_addrinfo *ai;
  CURLcode نتیجہ = CURLE_FTP_پورٹ_FAILED;

  برائے(ai = res; ai; ai = ai->ai_اگلا) {
    نتیجہ =
      Curl_socket_open(ڈیٹا, ai, تہی,
                       Curl_conn_get_transport(ڈیٹا, conn), portsockp);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        واپس نتیجہ;
      نتیجہ = CURLE_FTP_پورٹ_FAILED;
      sockerr = SOCKERRNO;
      جاری;
    }
    توڑو;
  }
  اگر(!ai) {
    failf(ڈیٹا, "socket failure: %s",
          curlx_strerror(sockerr, بفر, حجم(بفر)));
    واپس CURLE_FTP_پورٹ_FAILED;
  }
  *aip = ai;
  واپس نتیجہ;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ساکن CURLcode ftp_پورٹ_bind_socket(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn,
                                     curl_socket_t portsock,
                                     ثابت ساخت Curl_addrinfo *ai,
                                     ساخت Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     غیرعلامتی مختصر پورٹ_min,
                                     غیرعلامتی مختصر پورٹ_max,
                                     bool non_local)
{
  ساخت sockaddr *sa = (ساخت sockaddr *)ss;
  ساخت sockaddr_in * ثابت sa4 = (خالی *)sa;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 * ثابت sa6 = (خالی *)sa;
#اگر_ختم
  حرف بفر[STRERROR_LEN];
  غیرعلامتی مختصر پورٹ;
  صحیح sockerr;

  حافظہ_نقل(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  برائے(پورٹ = پورٹ_min; پورٹ <= پورٹ_max;) {
    اگر(sa->sa_family == AF_INET)
      sa4->sin_port = htons(پورٹ);
#اگر_تعریف_شدہ USE_IPV6
    ورنہ
      sa6->sin6_port = htons(پورٹ);
#اگر_ختم
    اگر(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      اگر(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(ڈیٹا, "bind(port=%hu) on non-local address failed: %s", پورٹ,
              curlx_strerror(sockerr, بفر, حجم(بفر)));

        *sslen_io = حجم(*ss);
        اگر(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(ڈیٹا, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
          واپس CURLE_FTP_پورٹ_FAILED;
        }
        پورٹ = پورٹ_min;
        non_local = FALSE; /* do not try this again */
        جاری;
      }
      اگر(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(ڈیٹا, "bind(port=%hu) failed: %s", پورٹ,
              curlx_strerror(sockerr, بفر, حجم(بفر)));
        واپس CURLE_FTP_پورٹ_FAILED;
      }
    }
    ورنہ
      توڑو;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    اگر(پورٹ == پورٹ_max) {
      failf(ڈیٹا, "bind() failed, ran out of ports");
      واپس CURLE_FTP_پورٹ_FAILED;
    }
    پورٹ++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = حجم(*ss);
  اگر(getsockname(portsock, sa, sslen_io)) {
    failf(ڈیٹا, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    واپس CURLE_FTP_پورٹ_FAILED;
  }
  CURL_TRC_FTP(ڈیٹا, "ftp_port_bind_socket(), socket bound to port %d", پورٹ);
  واپس CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ساکن CURLcode ftp_پورٹ_listen(ساخت Curl_easy *ڈیٹا, curl_socket_t portsock)
{
  حرف بفر[STRERROR_LEN];

  اگر(listen(portsock, 1)) {
    failf(ڈیٹا, "socket failure: %s",
          curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
    واپس CURLE_FTP_پورٹ_FAILED;
  }
  CURL_TRC_FTP(ڈیٹا, "ftp_port_listen(), listening on port");
  واپس CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ساکن CURLcode ftp_پورٹ_send_command(ساخت Curl_easy *ڈیٹا,
                                      ساخت ftp_conn *ftpc,
                                      ساخت connectdata *conn,
                                      ساخت Curl_sockaddr_storage *ss,
                                      ثابت ساخت Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ساکن ثابت حرف منوال[][5] = { "EPRT", "PORT" };
  ساخت sockaddr *sa = (ساخت sockaddr *)ss;
  ساخت sockaddr_in * ثابت sa4 = (خالی *)sa;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 * ثابت sa6 = (خالی *)sa;
#اگر_ختم
  حرف myhost[MAX_IPADR_LEN + 1] = "";
  غیرعلامتی مختصر پورٹ;
  CURLcode نتیجہ;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_پتہ(ai, myhost, حجم(myhost));

#اگر_تعریف_شدہ USE_IPV6
  اگر(!conn->بٹ.ftp_use_eprt && conn->بٹ.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->بٹ.ftp_use_eprt = TRUE;
#اگر_ختم

  برائے(; fcmd != مکمل_2; fcmd++) {

    اگر(!conn->بٹ.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      جاری;

    اگر((پورٹ_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      جاری;

    انتخاب(sa->sa_family) {
    حالت AF_INET:
      پورٹ = ntohs(sa4->sin_port);
      توڑو;
#اگر_تعریف_شدہ USE_IPV6
    حالت AF_INET6:
      پورٹ = ntohs(sa6->sin6_port);
      توڑو;
#اگر_ختم
    طےشدہ:
      جاری; /* might as well skip this */
    }

    اگر(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s |%d|%s|%hu|", منوال[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, پورٹ);
      اگر(نتیجہ) {
        failf(ڈیٹا, "Failure sending EPRT command: %s",
              curl_easy_strerror(نتیجہ));
        واپس نتیجہ;
      }
      توڑو;
    }
    اگر(پورٹ_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      حرف ہدف[حجم(myhost) + 20];
      ثابت حرف *ماخذ = myhost;
      حرف *dest = ہدف;

      /* translate x.x.x.x to x,x,x,x */
      جب(*ماخذ) {
        اگر(*ماخذ == '.')
          *dest = ',';
        ورنہ
          *dest = *ماخذ;
        dest++;
        ماخذ++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (صحیح)(پورٹ >> 8), (صحیح)(پورٹ & 0xff));

      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s %s", منوال[fcmd], ہدف);
      اگر(نتیجہ) {
        failf(ڈیٹا, "Failure sending PORT command: %s",
              curl_easy_strerror(نتیجہ));
        واپس نتیجہ;
      }
      توڑو;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(ڈیٹا, ftpc, FTP_پورٹ);
  واپس CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ساکن CURLcode ftp_state_use_پورٹ(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode نتیجہ = CURLE_FTP_پورٹ_FAILED;
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ساخت Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  حرف hbuf[NI_MAXHOST];
  ثابت حرف *host = تہی;
  ثابت حرف *string_ftpport = CURL_EASY_STR(ڈیٹا, STRING_FTPPORT);
  ساخت Curl_dns_اندراج *dns_اندراج = تہی;
  ثابت ساخت Curl_addrinfo *res = تہی;
  ثابت ساخت Curl_addrinfo *ai = تہی;
  غیرعلامتی مختصر پورٹ_min = 0;
  غیرعلامتی مختصر پورٹ_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  نتیجہ = ftp_پورٹ_parse_string(ڈیٹا, conn, string_ftpport,
                                 &ss, &پورٹ_min, &پورٹ_max,
                                 &host, hbuf, حجم(hbuf));
  اگر(!نتیجہ && !host)
    /* if no host was specified, use the control connection's local IP */
    نتیجہ = ftp_پورٹ_default_host(ڈیٹا, conn, &ss, &sslen, &host,
                                   hbuf, حجم(hbuf), &non_local);

  /* resolve host string to address list */
  اگر(!نتیجہ)
    نتیجہ = ftp_پورٹ_resolve_host(ڈیٹا, conn, host, &dns_اندراج, &res);

  /* Open a TCP socket for the data connection */
  اگر(!نتیجہ)
    نتیجہ = ftp_پورٹ_open_socket(ڈیٹا, conn, res, &ai, &portsock);
  اگر(!نتیجہ) {
    CURL_TRC_FTP(ڈیٹا, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    نتیجہ = ftp_پورٹ_bind_socket(ڈیٹا, conn, portsock, ai, &ss, &sslen,
                                  پورٹ_min, پورٹ_max, non_local);
  }

  /* listen */
  اگر(!نتیجہ)
    نتیجہ = ftp_پورٹ_listen(ڈیٹا, portsock);

  /* send the PORT / EPRT command */
  اگر(!نتیجہ)
    نتیجہ = ftp_پورٹ_send_command(ڈیٹا, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_tcp_listen_سیٹ(ڈیٹا, conn, SECONDARYSOCKET, &portsock);

  اگر(!نتیجہ)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  اگر(dns_اندراج)
    Curl_dns_اندراج_unlink(ڈیٹا, &dns_اندراج);
  اگر(نتیجہ) {
    ftp_state(ڈیٹا, ftpc, FTP_STOP);
  }
  ورنہ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    اگر(conn->بٹ.ftp_use_ڈیٹا_ssl && ڈیٹا->سیٹ.ftp_use_پورٹ &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      نتیجہ = Curl_ssl_cfilter_جوڑو(
        ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->بٹ.do_مزید = FALSE;
    Curl_expire(ڈیٹا, (ڈیٹا->سیٹ.accepttimeout > 0) ?
                ڈیٹا->سیٹ.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  اگر(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(ڈیٹا, conn, portsock);
  واپس نتیجہ;
}

ساکن CURLcode ftp_state_use_pasv(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc,
                                   ساخت connectdata *conn)
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

  ساکن ثابت حرف منوال[][5] = { "EPSV", "PASV" };
  صحیح modeoff;

#اگر_تعریف_شدہ PF_INET6
  اگر(!conn->بٹ.ftp_use_epsv && conn->بٹ.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->بٹ.ftp_use_epsv = TRUE;
#اگر_ختم

  modeoff = conn->بٹ.ftp_use_epsv ? 0 : 1;

  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", منوال[modeoff]);
  اگر(!نتیجہ) {
    ftpc->count1 = modeoff;
    ftp_state(ڈیٹا, ftpc, FTP_PASV);
    infof(ڈیٹا, "Connect data stream passively");
  }
  واپس نتیجہ;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ساکن CURLcode ftp_state_prepare_transfer(ساخت Curl_easy *ڈیٹا,
                                           ساخت ftp_conn *ftpc,
                                           ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  اگر(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(ڈیٹا, ftpc, FTP_RETR_PREQUOTE);
    نتیجہ = ftp_state_quote(ڈیٹا, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ورنہ اگر(ڈیٹا->سیٹ.ftp_use_پورٹ) {
    /* We have chosen to use the PORT (or similar) command */
    نتیجہ = ftp_state_use_پورٹ(ڈیٹا, ftpc, EPRT);
  }
  ورنہ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    اگر(ڈیٹا->سیٹ.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      اگر(!ftpc->file)
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) :
                               (ڈیٹا->state.list_only ? "NLST" : "LIST"));
      ورنہ اگر(ڈیٹا->state.upload)
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ورنہ
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PRET RETR %s", ftpc->file);
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_PRET);
    }
    ورنہ
      نتیجہ = ftp_state_use_pasv(ڈیٹا, ftpc, conn);
  }
  واپس نتیجہ;
}

ساکن CURLcode ftp_state_rest(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "REST %d", 0);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_REST);
  }
  ورنہ
    نتیجہ = ftp_state_prepare_transfer(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_سائز(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "SIZE %s", ftpc->file);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_سائز);
  }
  ورنہ
    نتیجہ = ftp_state_rest(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_list(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp)
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
  ثابت حرف *lstArg = تہی;
  صحیح lstArglen = 0;
  حرف *cmd;

  اگر((ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && ftp->راستہ) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ثابت حرف *raw_راستہ = ftpc->rawpath;
    ثابت حرف *slashPos = strrchr(raw_راستہ, '/');
    اگر(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_راستہ;
      اگر(n == 0)
        ++n;

      lstArg = raw_راستہ;
      lstArglen = (صحیح)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) :
                      (ڈیٹا->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  اگر(!cmd)
    واپس CURLE_برآمد_کا_حافظہ;

  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  اگر(!نتیجہ)
    ftp_state(ڈیٹا, ftpc, FTP_LIST);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_list_prequote(ساخت Curl_easy *ڈیٹا,
                                        ساخت ftp_conn *ftpc,
                                        ساخت FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  واپس ftp_state_quote(ڈیٹا, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ساکن CURLcode ftp_state_retr_prequote(ساخت Curl_easy *ڈیٹا,
                                        ساخت ftp_conn *ftpc,
                                        ساخت FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  واپس ftp_state_quote(ڈیٹا, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ساکن CURLcode ftp_state_stor_prequote(ساخت Curl_easy *ڈیٹا,
                                        ساخت ftp_conn *ftpc,
                                        ساخت FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  واپس ftp_state_quote(ڈیٹا, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ساکن CURLcode ftp_state_type(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  اگر(ڈیٹا->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)ڈیٹا->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    نتیجہ = ftp_nb_type(ڈیٹا, ftpc, ftp, (bool)ڈیٹا->state.prefer_ascii,
                         FTP_TYPE);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
    نتیجہ = ftp_state_سائز(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ساکن CURLcode ftp_state_mdtm(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  اگر((ڈیٹا->سیٹ.get_filetime || ڈیٹا->سیٹ.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "MDTM %s", ftpc->file);

    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_MDTM);
  }
  ورنہ
    نتیجہ = ftp_state_type(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

/* This is called after the TYPE and possible quote commands have been sent */
ساکن CURLcode ftp_state_ul_تیاری(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc,
                                   ساخت FTP *ftp,
                                   bool sizechecked)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_ثنائی_ہندسہ append = ڈیٹا->سیٹ.remote_append;

  اگر((ڈیٹا->state.resume_from && !sizechecked) ||
     ((ڈیٹا->state.resume_from > 0) && sizechecked)) {
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

    اگر(ڈیٹا->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "SIZE %s", ftpc->file);
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_STOR_سائز);
      واپس نتیجہ;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    اگر(ڈیٹا->سیٹ.seek_func) {
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_seek_func);
      seekerr = ڈیٹا->سیٹ.seek_func(ڈیٹا->سیٹ.seek_client,
                                    ڈیٹا->state.resume_from, SEEK_SET);
      CURL_CBAPI_انتہا(&guard);
    }

    اگر(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ڈیٹا, "Could not seek stream");
        واپس CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      کرو {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (ڈیٹا->state.resume_from - passed > (curl_off_t)حجم(scratch)) ?
          حجم(scratch) :
          curlx_sotouz(ڈیٹا->state.resume_from - passed);

        size_t actuallyread =
          ڈیٹا->state.fread_func(scratch, 1, readthisamountnow,
                                 ڈیٹا->state.اندر);

        passed += actuallyread;
        اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ڈیٹا, "Failed to read data");
          واپس CURLE_FTP_COULDNT_USE_REST;
        }
      } جب(passed < ڈیٹا->state.resume_from);
    }
    /* now, decrease the size of the read */
    اگر(ڈیٹا->state.infilesize > 0) {
      ڈیٹا->state.infilesize -= ڈیٹا->state.resume_from;

      اگر(ڈیٹا->state.infilesize <= 0) {
        infof(ڈیٹا, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_تیاری_nop(ڈیٹا);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(ڈیٹا, ftpc, FTP_STOP);
        واپس CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  اگر(!نتیجہ)
    ftp_state(ڈیٹا, ftpc, FTP_STOR);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_retr(ساخت Curl_easy *ڈیٹا,
                               ساخت ftp_conn *ftpc,
                               ساخت FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_FTP(ڈیٹا, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  اگر(ڈیٹا->سیٹ.max_filesize && (filesize > ڈیٹا->سیٹ.max_filesize)) {
    failf(ڈیٹا, "Maximum file size exceeded");
    واپس CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  اگر(ڈیٹا->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    اگر(filesize == -1) {
      infof(ڈیٹا, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ورنہ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      اگر(ڈیٹا->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        اگر(filesize < -ڈیٹا->state.resume_from) {
          failf(ڈیٹا, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ڈیٹا->state.resume_from, filesize);
          واپس CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -ڈیٹا->state.resume_from;
        /* download from where? */
        ڈیٹا->state.resume_from = filesize - ftp->downloadsize;
      }
      ورنہ {
        اگر(filesize < ڈیٹا->state.resume_from) {
          failf(ڈیٹا, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ڈیٹا->state.resume_from, filesize);
          واپس CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - ڈیٹا->state.resume_from;
      }
    }

    اگر(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_تیاری_nop(ڈیٹا);
      infof(ڈیٹا, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(ڈیٹا, ftpc, FTP_STOP);
      واپس CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(ڈیٹا, "Instructs server to resume from offset %" FMT_OFF_T,
          ڈیٹا->state.resume_from);

    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "REST %" FMT_OFF_T,
                           ڈیٹا->state.resume_from);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_RETR_REST);
  }
  ورنہ {
    /* no resume */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "RETR %s", ftpc->file);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_RETR);
  }

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_quote(ساخت Curl_easy *ڈیٹا,
                                ساخت ftp_conn *ftpc,
                                ساخت FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  bool quote = FALSE;
  ساخت curl_slist *شے;

  انتخاب(instate) {
  حالت FTP_QUOTE:
  طےشدہ:
    شے = ڈیٹا->سیٹ.quote;
    توڑو;
  حالت FTP_RETR_PREQUOTE:
  حالت FTP_STOR_PREQUOTE:
  حالت FTP_LIST_PREQUOTE:
    شے = ڈیٹا->سیٹ.prequote;
    توڑو;
  حالت FTP_POSTQUOTE:
    شے = ڈیٹا->سیٹ.postquote;
    توڑو;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  اگر(init)
    ftpc->count1 = 0;
  ورنہ
    ftpc->count1++;

  اگر(شے) {
    صحیح i = 0;

    /* Skip count1 items in the linked list */
    جب((i < ftpc->count1) && شے) {
      شے = شے->اگلا;
      i++;
    }
    اگر(شے) {
      ثابت حرف *cmd = شے->ڈیٹا;
      اگر(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ورنہ
        ftpc->count2 = 0; /* failure means cancel operation */

      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", cmd);
      اگر(نتیجہ)
        واپس نتیجہ;
      ftp_state(ڈیٹا, ftpc, instate);
      quote = TRUE;
    }
  }

  اگر(!quote) {
    /* No more quote to send, continue to ... */
    انتخاب(instate) {
    حالت FTP_QUOTE:
    طےشدہ:
      نتیجہ = ftp_state_cwd(ڈیٹا, ftpc, ftp);
      توڑو;
    حالت FTP_RETR_PREQUOTE:
      اگر(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(ڈیٹا, ftpc, FTP_STOP);
      ورنہ {
        اگر(ftpc->معلوم_filesize != -1) {
          Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, ftpc->معلوم_filesize);
          نتیجہ = ftp_state_retr(ڈیٹا, ftpc, ftp, ftpc->معلوم_filesize);
        }
        ورنہ {
          اگر(ڈیٹا->سیٹ.ignorecl || ڈیٹا->state.prefer_ascii) {
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
            نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "RETR %s", ftpc->file);
            اگر(!نتیجہ)
              ftp_state(ڈیٹا, ftpc, FTP_RETR);
          }
          ورنہ {
            نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "SIZE %s", ftpc->file);
            اگر(!نتیجہ)
              ftp_state(ڈیٹا, ftpc, FTP_RETR_سائز);
          }
        }
      }
      توڑو;
    حالت FTP_STOR_PREQUOTE:
      نتیجہ = ftp_state_ul_تیاری(ڈیٹا, ftpc, ftp, FALSE);
      توڑو;
    حالت FTP_POSTQUOTE:
      توڑو;
    حالت FTP_LIST_PREQUOTE:
      ftp_state(ڈیٹا, ftpc, FTP_LIST_TYPE);
      نتیجہ = ftp_state_list(ڈیٹا, ftpc, ftp);
      توڑو;
    }
  }

  واپس نتیجہ;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ساکن CURLcode ftp_epsv_disable(ساخت Curl_easy *ڈیٹا,
                                 ساخت ftp_conn *ftpc,
                                 ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(conn->بٹ.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(ڈیٹا, "Failed EPSV attempt, exiting");
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  infof(ڈیٹا, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->بٹ.ftp_use_epsv = FALSE;
  close_secondarysocket(ڈیٹا, ftpc);
  ڈیٹا->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "PASV");
  اگر(!نتیجہ) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(ڈیٹا, ftpc, FTP_PASV);
  }
  واپس نتیجہ;
}

ساکن CURLcode ftp_control_addr_dup(ساخت Curl_easy *ڈیٹا, حرف **newhostp)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ورنہ
#اگر_ختم
  اگر(!Curl_conn_get_ip_info(ڈیٹا, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ورنہ {
    /* failed to get the remote_ip of the DATA connection */
    failf(ڈیٹا, "unable to get peername of DATA connection");
    *newhostp = تہی;
    واپس CURLE_FTP_CANT_GET_HOST;
  }
  واپس *newhostp ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

ساکن bool مطابقت_pasv_6nums(ثابت حرف *p,
                             غیرعلامتی صحیح *array) /* 6 numbers */
{
  صحیح i;
  برائے(i = 0; i < 6; i++) {
    curl_off_t عدد;
    اگر(i) {
      اگر(*p != ',')
        واپس FALSE;
      p++;
    }
    اگر(curlx_str_نمبر(&p, &عدد, 0xff))
      واپس FALSE;
    array[i] = (غیرعلامتی صحیح)عدد;
  }
  واپس TRUE;
}

ساکن CURLcode ftp_state_pasv_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ;
  ثابت ساخت pingpong *pp = &ftpc->pp;
  حرف *newhost = تہی;
  غیرعلامتی مختصر newport = 0;
  ثابت حرف *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  اگر((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ثابت حرف *ptr = strchr(str, '(');
    اگر(ptr) {
      حرف sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      اگر(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ثابت حرف *p = &ptr[3];
        curl_off_t عدد;
        اگر(curlx_str_نمبر(&p, &عدد, 0xffff) || (*p != sep)) {
          failf(ڈیٹا, "Illegal port number in EPSV reply");
          واپس CURLE_FTP_WEIRD_PASV_جواب;
        }
        newport = (غیرعلامتی مختصر)عدد;
        نتیجہ = ftp_control_addr_dup(ڈیٹا, &newhost);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
      ورنہ
        ptr = تہی;
    }
    اگر(!ptr) {
      failf(ڈیٹا, "Weirdly formatted EPSV reply");
      واپس CURLE_FTP_WEIRD_PASV_جواب;
    }
  }
  ورنہ اگر((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    غیرعلامتی صحیح ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    جب(*str) {
      اگر(مطابقت_pasv_6nums(str, ip))
        توڑو;
      str++;
    }

    اگر(!*str) {
      failf(ڈیٹا, "Could not interpret the 227-response");
      واپس CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    اگر(ڈیٹا->سیٹ.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(ڈیٹا, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      نتیجہ = ftp_control_addr_dup(ڈیٹا, &newhost);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    اگر(!newhost)
      واپس CURLE_برآمد_کا_حافظہ;

    newport = (غیرعلامتی مختصر)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ورنہ اگر(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    واپس ftp_epsv_disable(ڈیٹا, ftpc, conn);
  }
  ورنہ {
    failf(ڈیٹا, "Bad PASV/EPSV response: %03d", ftpcode);
    واپس CURLE_FTP_WEIRD_PASV_جواب;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  نتیجہ = Curl_peer_create(ڈیٹا, conn->scheme, newhost, newport,
                            &conn->origin2);
  اگر(نتیجہ)
    جاؤ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  اگر(conn->بذریعہ_peer) {
    Curl_peer_unlink(&conn->بذریعہ_peer2);
    نتیجہ = Curl_peer_create(ڈیٹا, conn->بذریعہ_peer->scheme,
                              conn->بذریعہ_peer->hostname, newport,
                              &conn->بذریعہ_peer2);
    اگر(نتیجہ)
      جاؤ error;
  }

  نتیجہ = Curl_conn_تیاری(ڈیٹا, conn, SECONDARYSOCKET,
                           conn->بٹ.ftp_use_ڈیٹا_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  اگر(نتیجہ) {
    اگر((نتیجہ != CURLE_برآمد_کا_حافظہ) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      نتیجہ = ftp_epsv_disable(ڈیٹا, ftpc, conn);
    }
    جاؤ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(ڈیٹا->سیٹ.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(ڈیٹا, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->پورٹ);
  }
#اگر_ختم

  conn->بٹ.do_مزید = TRUE;
  ftp_state(ڈیٹا, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  واپس نتیجہ;
}

/* called repeatedly until done from multi.c */
ساکن CURLcode ftp_statemach(ساخت Curl_easy *ڈیٹا,
                              ساخت ftp_conn *ftpc,
                              bool *مکمل)
{
  CURLcode نتیجہ = Curl_pp_statemach(ڈیٹا, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *مکمل = (ftpc->state == FTP_STOP);

  واپس نتیجہ;
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
ساکن CURLcode ftp_do_مزید(ساخت Curl_easy *ڈیٹا, domore *مزید)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  ساخت FTP *ftp = Curl_meta_get(ڈیٹا, CURL_META_FTP_EASY);
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  اگر(!ftpc || !ftp)
    واپس CURLE_FAILED_INIT;

  *مزید = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  اگر(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(ڈیٹا, SECONDARYSOCKET);
    نتیجہ = Curl_conn_connect(ڈیٹا, SECONDARYSOCKET, FALSE, &connected);
    اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
      واپس نتیجہ;
    اگر(نتیجہ || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(ڈیٹا, SECONDARYSOCKET))) {
      اگر(نتیجہ && !is_eptr && (ftpc->count1 == 0)) {
        *مزید = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        واپس ftp_epsv_disable(ڈیٹا, ftpc, conn);
      }
      واپس نتیجہ;
    }
  }

  اگر(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    نتیجہ = ftp_statemach(ڈیٹا, ftpc, &complete);

    اگر(complete)
      *مزید = DOMORE_مکمل;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    اگر(نتیجہ || !ftpc->wait_ڈیٹا_conn)
      واپس نتیجہ;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *مزید = DOMORE_INCOMPLETE;
  }

  اگر(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    اگر(ftpc->wait_ڈیٹا_conn) {
      bool serv_conned;

      نتیجہ = Curl_conn_connect(ڈیٹا, SECONDARYSOCKET, FALSE, &serv_conned);
      اگر(نتیجہ)
        واپس نتیجہ; /* Failed to accept data connection */

      اگر(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_ڈیٹا_conn = FALSE;
        نتیجہ = ftp_initiate_transfer(ڈیٹا, ftpc);

        اگر(نتیجہ)
          واپس نتیجہ;

        *مزید = DOMORE_مکمل; /* this state is now complete when the server has
                                connected back to us */
      }
      ورنہ {
        نتیجہ = ftp_check_ctrl_on_ڈیٹا_wait(ڈیٹا, ftpc);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
    ورنہ اگر(ڈیٹا->state.upload) {
      نتیجہ = ftp_nb_type(ڈیٹا, ftpc, ftp, (bool)ڈیٹا->state.prefer_ascii,
                           FTP_STOR_TYPE);
      اگر(نتیجہ)
        واپس نتیجہ;

      نتیجہ = ftp_statemach(ڈیٹا, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *مزید = (!ftpc->wait_ڈیٹا_conn && complete) ?
        DOMORE_مکمل : DOMORE_INCOMPLETE;
    }
    ورنہ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      نتیجہ = Curl_range(ڈیٹا);

      اگر(نتیجہ == CURLE_OK && ڈیٹا->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      اگر(نتیجہ)
        واپس نتیجہ;

      اگر((ڈیٹا->state.list_only || !ftpc->file) &&
         !ڈیٹا->سیٹ.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        اگر(ftp->transfer == PPTRANSFER_BODY) {
          نتیجہ = ftp_nb_type(ڈیٹا, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          اگر(نتیجہ)
            واپس نتیجہ;
        }
        /* otherwise fall through */
      }
      ورنہ {
        اگر(ڈیٹا->سیٹ.prequote && !ftpc->file) {
          نتیجہ = ftp_nb_type(ڈیٹا, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ورنہ {
          نتیجہ = ftp_nb_type(ڈیٹا, ftpc, ftp, (bool)ڈیٹا->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        اگر(نتیجہ)
          واپس نتیجہ;
      }

      نتیجہ = ftp_statemach(ڈیٹا, ftpc, &complete);
      اگر(complete)
        *مزید = DOMORE_مکمل;
    }
    واپس نتیجہ;
  }

  /* no data to transfer */
  Curl_xfer_تیاری_nop(ڈیٹا);

  اگر(!ftpc->wait_ڈیٹا_conn) {
    /* no waiting for the data connection so this is now complete */
    *مزید = DOMORE_مکمل;
    CURL_TRC_FTP(ڈیٹا, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (صحیح)نتیجہ);
  }

  واپس نتیجہ;
}

/* call this when the DO phase has completed */
ساکن CURLcode ftp_dophase_مکمل(ساخت Curl_easy *ڈیٹا,
                                 ساخت ftp_conn *ftpc,
                                 ساخت FTP *ftp,
                                 bool connected)
{
  اگر(connected) {
    domore completed;
    CURLcode نتیجہ = ftp_do_مزید(ڈیٹا, &completed);

    اگر(نتیجہ) {
      close_secondarysocket(ڈیٹا, ftpc);
      واپس نتیجہ;
    }
  }

  اگر(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(ڈیٹا);
  ورنہ اگر(!connected)
    /* since we did not connect now, we want do_more to get called */
    ڈیٹا->conn->بٹ.do_مزید = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  واپس CURLE_OK;
}

ساکن CURLcode ftp_state_پورٹ_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    ساخت FTP *ftp,
                                    صحیح ftpcode)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode نتیجہ = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  اگر(ftpcode / 100 != 2) {
    /* the command failed */

    اگر(EPRT == fcmd) {
      infof(ڈیٹا, "disabling EPRT usage");
      conn->بٹ.ftp_use_eprt = FALSE;
    }
    fcmd++;

    اگر(fcmd == مکمل_2) {
      failf(ڈیٹا, "Failed to do PORT");
      نتیجہ = CURLE_FTP_پورٹ_FAILED;
    }
    ورنہ
      /* try next */
      نتیجہ = ftp_state_use_پورٹ(ڈیٹا, ftpc, fcmd);
  }
  ورنہ {
    infof(ڈیٹا, "Connect data stream actively");
    ftp_state(ڈیٹا, ftpc, FTP_STOP); /* end of DO phase */
    نتیجہ = ftp_dophase_مکمل(ڈیٹا, ftpc, ftp, FALSE);
  }

  واپس نتیجہ;
}

/* return TRUE on error, FALSE on success */
ساکن bool twodigit(ثابت حرف *p, صحیح *val)
{
  اگر(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    واپس TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  واپس FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ثابت حرف *p, صحیح *year, صحیح *month, صحیح *day,
                           صحیح *hour, صحیح *minute, صحیح *دوسرا);
UNITTEST bool ftp_213_date(ثابت حرف *p, صحیح *year, صحیح *month, صحیح *day,
                           صحیح *hour, صحیح *minute, صحیح *دوسرا)
{
  صحیح century;
  اگر((لڑی_لمبائی(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], دوسرا))
    واپس FALSE;

  *year += century * 100;
  اگر((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*دوسرا > 60))
    واپس FALSE;
  واپس TRUE;
}

ساکن CURLcode client_write_سرخی(ساخت Curl_easy *ڈیٹا,
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
  bool save = (bool)ڈیٹا->سیٹ.include_سرخی;
  ڈیٹا->سیٹ.include_سرخی = TRUE;
  نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, buf, blen);
  ڈیٹا->سیٹ.include_سرخی = save;
  واپس نتیجہ;
}

ساکن CURLcode ftp_state_mdtm_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    ساخت FTP *ftp,
                                    صحیح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(ftpcode) {
  حالت 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    صحیح year, month, day, hour, minute, دوسرا;
    ساخت pingpong *pp = &ftpc->pp;
    ثابت حرف *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    اگر(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &دوسرا)) {
      /* we have a time, reformat it */
      حرف timebuf[24];
      curl_msnprintf(timebuf, حجم(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, دوسرا);
      /* now, convert this into a time() value: */
      اگر(!Curl_getdate_capped(timebuf, &ڈیٹا->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#اگر defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ہدایت GCC diagnostic ڈالو
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#ہدایت GCC diagnostic ignored "-Wtype-limits"
#اگر_ختم
    اگر(ڈیٹا->req.no_body && ftpc->file &&
       ڈیٹا->سیٹ.get_filetime && showtime) {
#اگر defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ہدایت GCC diagnostic نکالو
#اگر_ختم
      حرف headerbuf[128];
      صحیح headerbuflen;
      time_t filetime = ڈیٹا->info.filetime;
      ساخت tm بفر;
      ثابت ساخت tm *tm = &بفر;

      نتیجہ = curlx_gmtime(filetime, &بفر);
      اگر(نتیجہ)
        واپس نتیجہ;

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
      نتیجہ = client_write_سرخی(ڈیٹا, headerbuf, headerbuflen);
      اگر(نتیجہ)
        واپس نتیجہ;
    } /* end of a ridiculous amount of conditionals */
  }
    توڑو;
  طےشدہ:
    infof(ڈیٹا, "unsupported MDTM reply format");
    توڑو;
  حالت 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(ڈیٹا, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    توڑو;
  }

  اگر(ڈیٹا->سیٹ.timecondition) {
    اگر((ڈیٹا->info.filetime > 0) && (ڈیٹا->سیٹ.timevalue > 0)) {
      انتخاب(ڈیٹا->سیٹ.timecondition) {
      حالت CURL_TIMECOND_IFMODSINCE:
      طےشدہ:
        اگر(ڈیٹا->info.filetime <= ڈیٹا->سیٹ.timevalue) {
          infof(ڈیٹا, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ڈیٹا->info.timecond = TRUE;
          ftp_state(ڈیٹا, ftpc, FTP_STOP);
          واپس CURLE_OK;
        }
        توڑو;
      حالت CURL_TIMECOND_IFUNMODSINCE:
        اگر(ڈیٹا->info.filetime > ڈیٹا->سیٹ.timevalue) {
          infof(ڈیٹا, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ڈیٹا->info.timecond = TRUE;
          ftp_state(ڈیٹا, ftpc, FTP_STOP);
          واپس CURLE_OK;
        }
        توڑو;
      } /* switch */
    }
    ورنہ {
      infof(ڈیٹا, "Skipping time comparison");
    }
  }

  اگر(!نتیجہ)
    نتیجہ = ftp_state_type(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_type_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    ساخت FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(ڈیٹا, "Could not set desired mode");
    واپس CURLE_FTP_COULDNT_سیٹ_TYPE;
  }
  اگر(ftpcode != 200)
    infof(ڈیٹا, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  اگر(instate == FTP_TYPE)
    نتیجہ = ftp_state_سائز(ڈیٹا, ftpc, ftp);
  ورنہ اگر(instate == FTP_LIST_TYPE)
    نتیجہ = ftp_state_list(ڈیٹا, ftpc, ftp);
  ورنہ اگر(instate == FTP_RETR_TYPE)
    نتیجہ = ftp_state_retr_prequote(ڈیٹا, ftpc, ftp);
  ورنہ اگر(instate == FTP_STOR_TYPE)
    نتیجہ = ftp_state_stor_prequote(ڈیٹا, ftpc, ftp);
  ورنہ اگر(instate == FTP_RETR_LIST_TYPE)
    نتیجہ = ftp_state_list_prequote(ڈیٹا, ftpc, ftp);

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_سائز_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    ساخت FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t filesize = -1;
  ثابت حرف *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  اگر(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ثابت حرف *آغاز = &buf[4];
    ثابت حرف *fdigit = memchr(آغاز, '\r', len - 4);
    اگر(fdigit) {
      fdigit--;
      اگر(*fdigit == '\n')
        fdigit--;
      جب(ISDIGIT(fdigit[-1]) && (fdigit > آغاز))
        fdigit--;
    }
    ورنہ
      fdigit = آغاز;
    اگر(curlx_str_نمبر(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ورنہ اگر(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_سائز) {
    failf(ڈیٹا, "The file does not exist");
    واپس CURLE_REMOTE_FILE_NOT_ملا;
  }

  اگر(instate == FTP_سائز) {
    اگر(filesize != -1) {
      حرف clbuf[128];
      صحیح clbuflen = curl_msnprintf(clbuf, حجم(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      نتیجہ = client_write_سرخی(ڈیٹا, clbuf, clbuflen);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, filesize);
    نتیجہ = ftp_state_rest(ڈیٹا, ftpc, ftp);
  }
  ورنہ اگر(instate == FTP_RETR_سائز) {
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, filesize);
    نتیجہ = ftp_state_retr(ڈیٹا, ftpc, ftp, filesize);
  }
  ورنہ اگر(instate == FTP_STOR_سائز) {
    ڈیٹا->state.resume_from = filesize;
    نتیجہ = ftp_state_ul_تیاری(ڈیٹا, ftpc, ftp, TRUE);
  }

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_rest_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    ساخت FTP *ftp,
                                    صحیح ftpcode,
                                    ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(instate) {
  حالت FTP_REST:
  طےشدہ:
    اگر(ftpcode == 350) {
      حرف بفر[24] = { "Accept-ranges: bytes\r\n" };
      نتیجہ = client_write_سرخی(ڈیٹا, بفر, لڑی_لمبائی(بفر));
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    نتیجہ = ftp_state_prepare_transfer(ڈیٹا, ftpc, ftp);
    توڑو;

  حالت FTP_RETR_REST:
    اگر(ftpcode != 350) {
      failf(ڈیٹا, "Could not use REST");
      نتیجہ = CURLE_FTP_COULDNT_USE_REST;
    }
    ورنہ {
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "RETR %s", ftpc->file);
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_RETR);
    }
    توڑو;
  }

  واپس نتیجہ;
}

ساکن CURLcode ftp_state_stor_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ftpcode >= 400) {
    failf(ڈیٹا, "Failed FTP upload: %0d", ftpcode);
    ftp_state(ڈیٹا, ftpc, FTP_STOP);
    واپس CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  اگر(ڈیٹا->سیٹ.ftp_use_پورٹ) {
    bool connected;

    ftp_state(ڈیٹا, ftpc, FTP_STOP); /* no longer in STOR state */

    نتیجہ = Curl_conn_connect(ڈیٹا, SECONDARYSOCKET, FALSE, &connected);
    اگر(نتیجہ)
      واپس نتیجہ;

    اگر(!connected) {
      infof(ڈیٹا, "Data conn was not available immediately");
      ftpc->wait_ڈیٹا_conn = TRUE;
      واپس ftp_check_ctrl_on_ڈیٹا_wait(ڈیٹا, ftpc);
    }
    ftpc->wait_ڈیٹا_conn = FALSE;
  }
  واپس ftp_initiate_transfer(ڈیٹا, ftpc);
}

/* for LIST and RETR responses */
ساکن CURLcode ftp_state_get_resp(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc,
                                   ساخت FTP *ftp,
                                   صحیح ftpcode,
                                   ftpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

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

    ڈیٹا->req.سائز = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    اگر((instate != FTP_LIST) &&
       !ڈیٹا->state.prefer_ascii &&
       !ڈیٹا->سیٹ.ignorecl &&
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
        برائے(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ثابت حرف *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ثابت حرف *c = &buf[i];
          اگر(!curlx_str_نمبر(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !لڑی_موازنہ(c, "bytes", 5)) {
            ڈیٹا->req.سائز = what;
            توڑو;
          }
        }
      }
    }
    ورنہ اگر(ftp->downloadsize > -1)
      ڈیٹا->req.سائز = ftp->downloadsize;

    اگر(ڈیٹا->req.سائز > ڈیٹا->req.maxdownload && ڈیٹا->req.maxdownload > 0)
      ڈیٹا->req.سائز = ڈیٹا->req.maxdownload;
    ورنہ اگر((instate != FTP_LIST) && ڈیٹا->state.prefer_ascii)
      ڈیٹا->req.سائز = -1; /* for servers that understate ASCII mode file
سائز*/

    infof(ڈیٹا, "Maxdownload = %" FMT_OFF_T, ڈیٹا->req.maxdownload);

    اگر(instate != FTP_LIST)
      infof(ڈیٹا, "Getting file with size: %" FMT_OFF_T, ڈیٹا->req.سائز);

    اگر(ڈیٹا->سیٹ.ftp_use_پورٹ) {
      bool connected;

      نتیجہ = Curl_conn_connect(ڈیٹا, SECONDARYSOCKET, FALSE, &connected);
      اگر(نتیجہ)
        واپس نتیجہ;

      اگر(!connected) {
        infof(ڈیٹا, "Data conn was not available immediately");
        ftp_state(ڈیٹا, ftpc, FTP_STOP);
        ftpc->wait_ڈیٹا_conn = TRUE;
        واپس ftp_check_ctrl_on_ڈیٹا_wait(ڈیٹا, ftpc);
      }
      ftpc->wait_ڈیٹا_conn = FALSE;
    }
    واپس ftp_initiate_transfer(ڈیٹا, ftpc);
  }
  ورنہ {
    اگر((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(ڈیٹا, ftpc, FTP_STOP); /* this phase is over */
    }
    ورنہ {
      failf(ڈیٹا, "RETR response: %03d", ftpcode);
      واپس instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_ملا :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  واپس نتیجہ;
}

/* after USER, PASS and ACCT */
ساکن CURLcode ftp_state_loggedin(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET)) {
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
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PBSZ %d", 0);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_PBSZ);
  }
  ورنہ {
    نتیجہ = ftp_state_pwd(ڈیٹا, ftpc);
  }
  واپس نتیجہ;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ساکن bool ftp_has_ctrl(ثابت حرف *string)
{
  ثابت غیرعلامتی حرف *s = (ثابت غیرعلامتی حرف *)string;
  جب(*s) {
    اگر(*s < 0x20)
      واپس TRUE;
    s++;
  }
  واپس FALSE;
}

/* for USER and PASS responses */
ساکن CURLcode ftp_state_user_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  اگر((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(ڈیٹا->conn->creds));
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_PASS);
  }
  ورنہ اگر(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    نتیجہ = ftp_state_loggedin(ڈیٹا, ftpc);
  }
  ورنہ اگر(ftpcode == 332) {
    ثابت حرف *account = CURL_EASY_STR(ڈیٹا, STRING_FTP_ACCOUNT);
    اگر(!account) {
      failf(ڈیٹا, "ACCT requested but none available");
      نتیجہ = CURLE_LOGIN_DENIED;
    }
    ورنہ اگر(ftp_has_ctrl(account)) {
      failf(ڈیٹا, "Control byte in FTP account");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    }
    ورنہ {
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "ACCT %s", account);
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_ACCT);
    }
  }
  ورنہ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ثابت حرف *alt = CURL_EASY_STR(ڈیٹا, STRING_FTP_ALTERNATIVE_مقصد_USER);
    اگر(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      اگر(ftp_has_ctrl(alt)) {
        failf(ڈیٹا, "Control byte in FTP alternative-to-user command");
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      }
      ورنہ {
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", alt);
        اگر(!نتیجہ) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(ڈیٹا, ftpc, FTP_USER);
        }
      }
    }
    ورنہ {
      failf(ڈیٹا, "Access denied: %03d", ftpcode);
      نتیجہ = CURLE_LOGIN_DENIED;
    }
  }
  واپس نتیجہ;
}

/* for ACCT response */
ساکن CURLcode ftp_state_acct_resp(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc,
                                    صحیح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(ftpcode != 230) {
    failf(ڈیٹا, "ACCT rejected by server: %03d", ftpcode);
    نتیجہ = CURLE_FTP_WEIRD_PASS_جواب; /* FIX */
  }
  ورنہ
    نتیجہ = ftp_state_loggedin(ڈیٹا, ftpc);

  واپس نتیجہ;
}

ساکن CURLcode ftp_pwd_resp(ساخت Curl_easy *ڈیٹا,
                             ساخت ftp_conn *ftpc,
                             صحیح ftpcode)
{
  ساخت pingpong *pp = &ftpc->pp;
  CURLcode نتیجہ;

  اگر(ftpcode == 257) {
    ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool اندراج_extracted = FALSE;
    ساخت dynbuf برآمد;
    curlx_dyn_init(&برآمد, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    جب(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    اگر('\"' == *ptr) {
      /* it started good */
      برائے(ptr++; *ptr; ptr++) {
        اگر('\"' == *ptr) {
          اگر('\"' == ptr[1]) {
            /* "quote-doubling" */
            نتیجہ = curlx_dyn_addn(&برآمد, &ptr[1], 1);
            ptr++;
          }
          ورنہ {
            /* end of path */
            اگر(curlx_dyn_len(&برآمد))
              اندراج_extracted = TRUE;
            توڑو; /* get out of this loop */
          }
        }
        ورنہ {
          اگر(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&برآمد);
            واپس CURLE_WEIRD_SERVER_جواب;
          }
          نتیجہ = curlx_dyn_addn(&برآمد, ptr, 1);
        }
        اگر(نتیجہ) {
          curlx_dyn_free(&برآمد);
          واپس نتیجہ;
        }
      }
    }
    اگر(اندراج_extracted) {
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
      حرف *dir = curlx_dyn_ptr(&برآمد);

      اگر(!ftpc->server_os && dir[0] != '/') {
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "SYST");
        اگر(نتیجہ) {
          curlx_dyn_free(&برآمد);
          واپس نتیجہ;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(ڈیٹا, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(ڈیٹا->state.most_recent_ftp_entrypath);
      ڈیٹا->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      اگر(!ڈیٹا->state.most_recent_ftp_entrypath)
        واپس CURLE_برآمد_کا_حافظہ;

      اگر(!ftpc->server_os && dir[0] != '/') {
        ftp_state(ڈیٹا, ftpc, FTP_SYST);
        واپس CURLE_OK;
      }
    }
    ورنہ {
      /* could not get the path */
      curlx_dyn_free(&برآمد);
      infof(ڈیٹا, "Failed to figure out path");
    }
  }
  ftp_state(ڈیٹا, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(ڈیٹا, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  واپس CURLE_OK;
}

ساکن ثابت حرف * ثابت ftpauth[] = { "SSL", "TLS" };

ساکن CURLcode ftp_wait_resp(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn,
                              ساخت ftp_conn *ftpc,
                              صحیح ftpcode)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    اگر(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      واپس ftp_state_user_resp(ڈیٹا, ftpc, ftpcode);
  }
  ورنہ اگر(ftpcode != 220) {
    failf(ڈیٹا, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  اگر(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    انتخاب((طویل)ڈیٹا->سیٹ.ftpsslauth) {
    حالت CURLFTPAUTH_DEFAULT:
    حالت CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      توڑو;
    حالت CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      توڑو;
    طےشدہ:
      failf(ڈیٹا, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (صحیح)ڈیٹا->سیٹ.ftpsslauth);
      واپس CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_AUTH);
  }
  ورنہ
    نتیجہ = ftp_state_user(ڈیٹا, ftpc, conn);
  واپس نتیجہ;
}

/* an implicit TLS scheme implies TLS on both connections */
ساکن غیرعلامتی حرف ftp_use_ssl(ساخت connectdata *conn,
                                 ساخت ftp_conn *ftpc)
{
  اگر(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    واپس CURLUSESSL_ALL;
  واپس ftpc->use_ssl;
}

ساکن CURLcode ftp_pp_statemachine(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn)
{
  CURLcode نتیجہ;
  صحیح ftpcode;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ساخت FTP *ftp = Curl_meta_get(ڈیٹا, CURL_META_FTP_EASY);
  ساخت pingpong *pp;
  size_t nread = 0;

  اگر(!ftpc || !ftp)
    واپس CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  اگر(pp->sendleft)
    واپس Curl_pp_flushsend(ڈیٹا, pp);

  نتیجہ = ftp_readresp(ڈیٹا, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  اگر(نتیجہ || !ftpcode)
    واپس نتیجہ;

  /* we have now received a full FTP server response */
  انتخاب(ftpc->state) {
  حالت FTP_WAIT220:
    نتیجہ = ftp_wait_resp(ڈیٹا, conn, ftpc, ftpcode);
    توڑو;

  حالت FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    اگر(pp->حد_سے_تجاوز)
      واپس CURLE_WEIRD_SERVER_جواب; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    اگر((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool مکمل;
      اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        نتیجہ = Curl_ssl_cfilter_جوڑو(
          ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        اگر(نتیجہ) {
          /* we failed and bail out */
          واپس CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, TRUE, &مکمل);
      اگر(!نتیجہ) {
        conn->بٹ.ftp_use_ڈیٹا_ssl = FALSE; /* clear-text data */
        نتیجہ = ftp_state_user(ڈیٹا, ftpc, conn);
      }
    }
    ورنہ اگر(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ورنہ {
      اگر(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        نتیجہ = CURLE_USE_SSL_FAILED;
      ورنہ
        /* ignore the failure and continue */
        نتیجہ = ftp_state_user(ڈیٹا, ftpc, conn);
    }
    توڑو;

  حالت FTP_USER:
  حالت FTP_PASS:
    نتیجہ = ftp_state_user_resp(ڈیٹا, ftpc, ftpcode);
    توڑو;

  حالت FTP_ACCT:
    نتیجہ = ftp_state_acct_resp(ڈیٹا, ftpc, ftpcode);
    توڑو;

  حالت FTP_PBSZ:
    نتیجہ =
      Curl_pp_sendf(ڈیٹا, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    اگر(!نتیجہ)
      ftp_state(ڈیٹا, ftpc, FTP_PROT);
    توڑو;

  حالت FTP_PROT:
    اگر(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->بٹ.ftp_use_ڈیٹا_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ورنہ اگر(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      واپس CURLE_USE_SSL_FAILED;

    اگر(ڈیٹا->سیٹ.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "CCC");
      اگر(!نتیجہ)
        ftp_state(ڈیٹا, ftpc, FTP_CCC);
    }
    ورنہ
      نتیجہ = ftp_state_pwd(ڈیٹا, ftpc);
    توڑو;

  حالت FTP_CCC:
    اگر(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      نتیجہ = Curl_ssl_cfilter_remove(ڈیٹا, FIRSTSOCKET,
                                       (ڈیٹا->سیٹ.ftp_ccc ==
                                        (غیرعلامتی حرف)CURLFTPSSL_CCC_فعال));
      اگر(نتیجہ)
        failf(ڈیٹا, "Failed to clear the command channel (CCC)");
    }
    اگر(!نتیجہ)
      /* Then continue as normal */
      نتیجہ = ftp_state_pwd(ڈیٹا, ftpc);
    توڑو;

  حالت FTP_PWD:
    نتیجہ = ftp_pwd_resp(ڈیٹا, ftpc, ftpcode);
    توڑو;

  حالت FTP_SYST:
    اگر(ftpcode == 215) {
      ثابت حرف *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ثابت حرف *آغاز;
      حرف *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      جب(*ptr == ' ')
        ptr++;
      برائے(آغاز = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(آغاز, ptr - آغاز);
      اگر(!os)
        واپس CURLE_برآمد_کا_حافظہ;

      /* Check for special servers here. */
      اگر(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        اگر(نتیجہ) {
          curlx_free(os);
          واپس نتیجہ;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(ڈیٹا, ftpc, FTP_NAMEFMT);
        توڑو;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ورنہ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(ڈیٹا, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ڈیٹا, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    توڑو;

  حالت FTP_NAMEFMT:
    اگر(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(ڈیٹا, ftpc);
      توڑو;
    }

    ftp_state(ڈیٹا, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ڈیٹا, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    توڑو;

  حالت FTP_QUOTE:
  حالت FTP_POSTQUOTE:
  حالت FTP_RETR_PREQUOTE:
  حالت FTP_STOR_PREQUOTE:
  حالت FTP_LIST_PREQUOTE:
    اگر((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(ڈیٹا, "QUOT command failed with %03d", ftpcode);
      نتیجہ = CURLE_QUOTE_ERROR;
    }
    ورنہ
      نتیجہ = ftp_state_quote(ڈیٹا, ftpc, ftp, FALSE, ftpc->state);
    توڑو;

  حالت FTP_CWD:
    اگر(ftpcode / 100 != 2) {
      /* failure to CWD there */
      اگر(ڈیٹا->سیٹ.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (ڈیٹا->سیٹ.ftp_create_missing_dirs == 2) ? 1 : 0;

        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        اگر(!نتیجہ)
          ftp_state(ڈیٹا, ftpc, FTP_MKD);
      }
      ورنہ {
        /* return failure */
        failf(ڈیٹا, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ورنہ {
      /* success */
      ftpc->count2 = 0;
      اگر(ftpc->cwdcount >= ftpc->dirdepth)
        نتیجہ = ftp_state_mdtm(ڈیٹا, ftpc, ftp);
      ورنہ {
        ftpc->cwdcount++;
        /* send next CWD */
        نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    توڑو;

  حالت FTP_MKD:
    اگر((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(ڈیٹا, "Failed to MKD dir: %03d", ftpcode);
      نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    }
    ورنہ {
      ftp_state(ڈیٹا, ftpc, FTP_CWD);
      /* send CWD */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    توڑو;

  حالت FTP_MDTM:
    نتیجہ = ftp_state_mdtm_resp(ڈیٹا, ftpc, ftp, ftpcode);
    توڑو;

  حالت FTP_TYPE:
  حالت FTP_LIST_TYPE:
  حالت FTP_RETR_TYPE:
  حالت FTP_STOR_TYPE:
  حالت FTP_RETR_LIST_TYPE:
    نتیجہ = ftp_state_type_resp(ڈیٹا, ftpc, ftp, ftpcode, ftpc->state);
    توڑو;

  حالت FTP_سائز:
  حالت FTP_RETR_سائز:
  حالت FTP_STOR_سائز:
    نتیجہ = ftp_state_سائز_resp(ڈیٹا, ftpc, ftp, ftpcode, ftpc->state);
    توڑو;

  حالت FTP_REST:
  حالت FTP_RETR_REST:
    نتیجہ = ftp_state_rest_resp(ڈیٹا, ftpc, ftp, ftpcode, ftpc->state);
    توڑو;

  حالت FTP_PRET:
    اگر(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(ڈیٹا, "PRET command not accepted: %03d", ftpcode);
      واپس CURLE_FTP_PRET_FAILED;
    }
    نتیجہ = ftp_state_use_pasv(ڈیٹا, ftpc, conn);
    توڑو;

  حالت FTP_PASV:
    نتیجہ = ftp_state_pasv_resp(ڈیٹا, ftpc, ftpcode);
    توڑو;

  حالت FTP_پورٹ:
    نتیجہ = ftp_state_پورٹ_resp(ڈیٹا, ftpc, ftp, ftpcode);
    توڑو;

  حالت FTP_LIST:
  حالت FTP_RETR:
    نتیجہ = ftp_state_get_resp(ڈیٹا, ftpc, ftp, ftpcode, ftpc->state);
    توڑو;

  حالت FTP_STOR:
    نتیجہ = ftp_state_stor_resp(ڈیٹا, ftpc, ftpcode);
    توڑو;

  حالت FTP_QUIT:
  طےشدہ:
    /* internal error */
    ftp_state(ڈیٹا, ftpc, FTP_STOP);
    توڑو;
  }

  واپس نتیجہ;
}

/* called repeatedly until done from multi.c */
ساکن CURLcode ftp_multi_statemach(ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  واپس ftpc ? ftp_statemach(ڈیٹا, ftpc, مکمل) : CURLE_FAILED_INIT;
}

ساکن CURLcode ftp_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_conn *ftpc)
{
  ساخت pingpong *pp = &ftpc->pp;
  CURLcode نتیجہ = CURLE_OK;

  جب(ftpc->state != FTP_STOP) {
    اگر(ftpc->shutdown)
      CURL_TRC_FTP(ڈیٹا, "in shutdown, waiting for server response");
    نتیجہ = Curl_pp_statemach(ڈیٹا, pp, TRUE, TRUE /* disconnecting */);
    اگر(نتیجہ)
      توڑو;
  }

  واپس نتیجہ;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ساکن CURLcode ftp_connect(ساخت Curl_easy *ڈیٹا,
                            bool *مکمل) /* see description above */
{
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  ساخت pingpong *pp;

  *مکمل = FALSE; /* default to not done yet */
  اگر(!ftpc)
    واپس CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_تیاری(pp, ftp_pp_statemachine, ftp_endofresp);

  اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, TRUE, مکمل);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  Curl_pp_init(pp, Curl_pgrs_now(ڈیٹا)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(ڈیٹا, ftpc, FTP_WAIT220);

  نتیجہ = ftp_statemach(ڈیٹا, ftpc, مکمل);

  واپس نتیجہ;
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
ساکن CURLcode ftp_sendquote(ساخت Curl_easy *ڈیٹا,
                              ساخت ftp_conn *ftpc,
                              ساخت curl_slist *quote)
{
  ساخت curl_slist *شے;
  ساخت pingpong *pp = &ftpc->pp;

  شے = quote;
  جب(شے) {
    اگر(شے->ڈیٹا) {
      size_t nread;
      ثابت حرف *cmd = شے->ڈیٹا;
      bool acceptfail = FALSE;
      CURLcode نتیجہ;
      صحیح ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      اگر(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", cmd);
      اگر(!نتیجہ) {
        pp->response = *Curl_pgrs_now(ڈیٹا); /* timeout relative now */
        نتیجہ = getftpresponse(ڈیٹا, &nread, &ftpcode);
      }
      اگر(نتیجہ)
        واپس نتیجہ;

      اگر(!acceptfail && (ftpcode >= 400)) {
        failf(ڈیٹا, "QUOT string not accepted: %s", cmd);
        واپس CURLE_QUOTE_ERROR;
      }
    }

    شے = شے->اگلا;
  }

  واپس CURLE_OK;
}

ساکن CURLcode ftp_مکمل_حالت(ساخت Curl_easy *ڈیٹا,
                                ساخت ftp_conn *ftpc, CURLcode حالت_2,
                                bool premature)
{
  انتخاب(حالت_2) {
  حالت CURLE_BAD_DOWNLOAD_RESUME:
  حالت CURLE_FTP_WEIRD_PASV_جواب:
  حالت CURLE_FTP_پورٹ_FAILED:
  حالت CURLE_FTP_ACCEPT_FAILED:
  حالت CURLE_FTP_ACCEPT_TIMEOUT:
  حالت CURLE_FTP_COULDNT_سیٹ_TYPE:
  حالت CURLE_FTP_COULDNT_RETR_FILE:
  حالت CURLE_PARTIAL_FILE:
  حالت CURLE_UPLOAD_FAILED:
  حالت CURLE_REMOTE_ACCESS_DENIED:
  حالت CURLE_FILESIZE_EXCEEDED:
  حالت CURLE_REMOTE_FILE_NOT_ملا:
  حالت CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  حالت CURLE_OK: /* does not affect the control connection's status */
    اگر(!premature)
      توڑو;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  طےشدہ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(ڈیٹا, "FTP ended with bad error code");
    connclose(ڈیٹا->conn);
    واپس حالت_2;      /* use the already set error code */
  }
  واپس CURLE_OK;
}

ساکن خالی ftp_مکمل_wildcard(ساخت Curl_easy *ڈیٹا, ساخت ftp_conn *ftpc)
{
  اگر(ڈیٹا->state.wildcardmatch) {
    اگر(ڈیٹا->سیٹ.chunk_انتہا && ftpc->file) {
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_chunk_انتہا);
      ڈیٹا->سیٹ.chunk_انتہا(ڈیٹا->سیٹ.wildcardptr);
      CURL_CBAPI_انتہا(&guard);
      freedirs(ftpc);
    }
    ftpc->معلوم_filesize = -1;
  }
}

ساکن خالی ftp_مکمل_راستہ(ساخت Curl_easy *ڈیٹا, ساخت ftp_conn *ftpc,
                          CURLcode نتیجہ)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(نتیجہ) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ورنہ { /* remember working directory for connection reuse */
    ثابت حرف *raw_راستہ = ftpc->rawpath;
    اگر(raw_راستہ) {
      اگر((ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD) && (raw_راستہ[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ورنہ {
        size_t راستہ_Len = لڑی_لمبائی(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        اگر(!ftpc->cwdfail) {
          اگر(ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
            راستہ_Len = 0; /* relative path => working directory is FTP home */
          ورنہ
            /* file is URL-decoded */
            راستہ_Len -= ftpc->file ? لڑی_لمبائی(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_راستہ, راستہ_Len);
        }
        ورنہ
          ftpc->prevpath = تہی; /* no path */
      }
    }
    اگر(ftpc->prevpath)
      infof(ڈیٹا, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ساکن CURLcode ftp_مکمل_secondary_socket(ساخت Curl_easy *ڈیٹا,
                                          ساخت ftp_conn *ftpc,
                                          CURLcode نتیجہ)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(Curl_conn_is_تیاری(conn, SECONDARYSOCKET)) {
    اگر(!نتیجہ && ftpc->dont_check && ڈیٹا->req.maxdownload > 0) {
      /* partial download completed */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "ABOR");
      اگر(نتیجہ) {
        failf(ڈیٹا, "Failure sending ABOR command: %s",
              curl_easy_strerror(نتیجہ));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(ڈیٹا, ftpc);
  }
  واپس نتیجہ;
}

ساکن CURLcode ftp_مکمل_control_جواب(ساخت Curl_easy *ڈیٹا,
                                       ساخت ftp_conn *ftpc,
                                       ساخت FTP *ftp, CURLcode نتیجہ,
                                       bool premature)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  size_t nread;
  صحیح ftpcode;

  اگر(!نتیجہ && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(ڈیٹا); /* timeout relative now */
    نتیجہ = getftpresponse(ڈیٹا, &nread, &ftpcode);

    اگر(!nread && (نتیجہ == CURLE_OPERATION_TIMEDOUT)) {
      failf(ڈیٹا, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    اگر(نتیجہ)
      واپس نتیجہ;

    اگر(ftpc->dont_check && ڈیٹا->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(ڈیٹا, "partial download completed, closing connection");
      connclose(conn);
      واپس نتیجہ;
    }

    اگر(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      انتخاب(ftpcode) {
      حالت 226:
      حالت 250:
        توڑو;
      حالت 552:
        failf(ڈیٹا, "Exceeded storage allocation");
        نتیجہ = CURLE_REMOTE_DISK_بھرا;
        توڑو;
      طےشدہ:
        failf(ڈیٹا, "server did not report OK, got %d", ftpcode);
        نتیجہ = CURLE_PARTIAL_FILE;
        توڑو;
      }
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode ftp_مکمل_check_partial(ساخت Curl_easy *ڈیٹا,
                                       ساخت ftp_conn *ftpc,
                                       ساخت FTP *ftp, CURLcode نتیجہ,
                                       bool premature)
{
  اگر(نتیجہ || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ورنہ اگر(ڈیٹا->state.upload) {
    اگر((ftp->transfer == PPTRANSFER_BODY) &&
       (ڈیٹا->state.infilesize != -1) && /* upload with known size */
       ((!ڈیٹا->سیٹ.crlf && !ڈیٹا->state.prefer_ascii && /* no conversion */
         (ڈیٹا->state.infilesize != ڈیٹا->req.writebytecount)) ||
        ((ڈیٹا->سیٹ.crlf || ڈیٹا->state.prefer_ascii) && /* maybe CRLF conv */
         (ڈیٹا->state.infilesize > ڈیٹا->req.writebytecount))
       )) {
      failf(ڈیٹا, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            ڈیٹا->req.writebytecount, ڈیٹا->state.infilesize);
      نتیجہ = CURLE_PARTIAL_FILE;
    }
  }
  ورنہ {
    اگر((ڈیٹا->req.سائز != -1) &&
       (ڈیٹا->req.سائز != ڈیٹا->req.bytecount) &&
       (ڈیٹا->req.maxdownload != ڈیٹا->req.bytecount)) {
      failf(ڈیٹا, "Received only partial file: %" FMT_OFF_T " bytes",
            ڈیٹا->req.bytecount);
      نتیجہ = CURLE_PARTIAL_FILE;
    }
    ورنہ اگر(!ftpc->dont_check &&
            !ڈیٹا->req.bytecount &&
            (ڈیٹا->req.سائز > 0)) {
      failf(ڈیٹا, "No data was received");
      نتیجہ = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  واپس نتیجہ;
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
ساکن CURLcode ftp_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                         bool premature)
{
  ساخت FTP *ftp = Curl_meta_get(ڈیٹا, CURL_META_FTP_EASY);
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  CURLcode نتیجہ;

  اگر(!ftp || !ftpc)
    واپس CURLE_OK;

  نتیجہ = ftp_مکمل_حالت(ڈیٹا, ftpc, حالت_2, premature);

  ftp_مکمل_wildcard(ڈیٹا, ftpc);
  ftp_مکمل_راستہ(ڈیٹا, ftpc, نتیجہ);
  نتیجہ = ftp_مکمل_secondary_socket(ڈیٹا, ftpc, نتیجہ);
  نتیجہ = ftp_مکمل_control_جواب(ڈیٹا, ftpc, ftp, نتیجہ, premature);
  نتیجہ = ftp_مکمل_check_partial(ڈیٹا, ftpc, ftp, نتیجہ, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  اگر(!حالت_2 && !نتیجہ && !premature && ڈیٹا->سیٹ.postquote)
    نتیجہ = ftp_sendquote(ڈیٹا, ftpc, ڈیٹا->سیٹ.postquote);
  CURL_TRC_FTP(ڈیٹا, "[%s] done, result=%d", FTP_CSTATE(ftpc), (صحیح)نتیجہ);
  واپس نتیجہ;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ساکن CURLcode ftp_nb_type(ساخت Curl_easy *ڈیٹا,
                            ساخت ftp_conn *ftpc,
                            ساخت FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode نتیجہ;
  حرف want = (حرف)(ascii ? 'A' : 'I');

  اگر(ftpc->transfertype == want) {
    ftp_state(ڈیٹا, ftpc, newstate);
    واپس ftp_state_type_resp(ڈیٹا, ftpc, ftp, 200, newstate);
  }

  نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "TYPE %c", want);
  اگر(!نتیجہ) {
    ftp_state(ڈیٹا, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  واپس نتیجہ;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ساکن CURLcode ftp_perform(
  ساخت Curl_easy *ڈیٹا,
  ساخت ftp_conn *ftpc,
  ساخت FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_مکمل)
{
  /* this is FTP and no proxy */
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_FTP(ڈیٹا, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  اگر(ڈیٹا->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_مکمل = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  نتیجہ = ftp_state_quote(ڈیٹا, ftpc, ftp, TRUE, FTP_QUOTE);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* run the state-machine */
  نتیجہ = ftp_statemach(ڈیٹا, ftpc, dophase_مکمل);

  *connected = Curl_conn_is_connected(ڈیٹا->conn, SECONDARYSOCKET);

  اگر(*connected)
    infof(ڈیٹا, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ورنہ
    CURL_TRC_FTP(ڈیٹا, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  اگر(*dophase_مکمل)
    CURL_TRC_FTP(ڈیٹا, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  واپس نتیجہ;
}

ساکن خالی wc_ڈیٹا_dtor(خالی *ptr)
{
  ساخت ftp_wc *ftpwc = ptr;
  اگر(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_ڈیٹا_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ساکن CURLcode init_wc_ڈیٹا(ساخت Curl_easy *ڈیٹا,
                             ساخت ftp_conn *ftpc,
                             ساخت FTP *ftp)
{
  حرف *آخری_slash;
  حرف *راستہ = ftp->راستہ;
  ساخت Wildcard_ڈیٹا *wildcard = ڈیٹا->wildcard;
  CURLcode نتیجہ = CURLE_OK;
  ساخت ftp_wc *ftpwc = تہی;

  آخری_slash = strrchr(ftp->راستہ, '/');
  اگر(آخری_slash) {
    آخری_slash++;
    اگر(آخری_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      واپس ftp_parse_url_راستہ(ڈیٹا, ftpc, ftp);
    }
    wildcard->نمونہ = curlx_strdup(آخری_slash);
    اگر(!wildcard->نمونہ)
      واپس CURLE_برآمد_کا_حافظہ;
    آخری_slash[0] = '\0'; /* cut file from path */
  }
  ورنہ { /* there is only 'wildcard pattern' or nothing */
    اگر(راستہ[0]) {
      wildcard->نمونہ = curlx_strdup(راستہ);
      اگر(!wildcard->نمونہ)
        واپس CURLE_برآمد_کا_حافظہ;
      راستہ[0] = '\0';
    }
    ورنہ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      واپس ftp_parse_url_راستہ(ڈیٹا, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, حجم(ساخت ftp_wc));
  اگر(!ftpwc) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_ڈیٹا_alloc();
  اگر(!ftpwc->parser) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_ڈیٹا_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  اگر(ڈیٹا->سیٹ.ftp_filemethod == FTPFILE_NOCWD)
    ڈیٹا->سیٹ.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  نتیجہ = ftp_parse_url_راستہ(ڈیٹا, ftpc, ftp);
  اگر(نتیجہ) {
    جاؤ fail;
  }

  wildcard->راستہ = curlx_strdup(ftp->راستہ);
  اگر(!wildcard->راستہ) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  infof(ڈیٹا, "Wildcard - Parsing started");
  واپس CURLE_OK;

fail:
  اگر(ftpwc) {
    Curl_ftp_parselist_ڈیٹا_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->نمونہ);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = تہی;
  واپس نتیجہ;
}

ساکن CURLcode wc_statemach(ساخت Curl_easy *ڈیٹا,
                             ساخت ftp_conn *ftpc,
                             ساخت FTP *ftp)
{
  ساخت Wildcard_ڈیٹا * ثابت wildcard = ڈیٹا->wildcard;
  CURLcode نتیجہ = CURLE_OK;

  برائے(;;) {
    انتخاب(wildcard->state) {
    حالت CURLWC_INIT:
      نتیجہ = init_wc_ڈیٹا(ڈیٹا, ftpc, ftp);
      اگر(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        واپس نتیجہ;
      wildcard->state = نتیجہ ? CURLWC_ERROR : CURLWC_MATCHING;
      واپس نتیجہ;

    حالت CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ساخت ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      اگر(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        جاری;
      }
      اگر(Curl_llist_گنتی(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        واپس CURLE_REMOTE_FILE_NOT_ملا;
      }
      جاری;
    }

    حالت CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ساخت Curl_llist_گرہ *سر = Curl_llist_سر(&wildcard->filelist);
      ساخت curl_fileinfo *finfo = Curl_گرہ_elem(سر);
      حرف *tmp_راستہ;
      حرف *enc = curl_easy_escape(تہی, finfo->filename, 0);
      اگر(!enc)
        واپس CURLE_برآمد_کا_حافظہ;

      tmp_راستہ = curl_maprintf("%s%s", wildcard->راستہ, enc);
      curl_free(enc);
      اگر(!tmp_راستہ)
        واپس CURLE_برآمد_کا_حافظہ;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->راستہ = tmp_راستہ;

      infof(ڈیٹا, "Wildcard - START of \"%s\"", finfo->filename);
      اگر(ڈیٹا->سیٹ.chunk_bgn) {
        طویل userresponse;
        ساخت Curl_mapi_guard guard;
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_chunk_bgn);
        userresponse = ڈیٹا->سیٹ.chunk_bgn(
          finfo, ڈیٹا->سیٹ.wildcardptr,
          (صحیح)Curl_llist_گنتی(&wildcard->filelist));
        CURL_CBAPI_انتہا(&guard);
        انتخاب(userresponse) {
        حالت CURL_CHUNK_BGN_FUNC_SKIP:
          infof(ڈیٹا, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          جاری;
        حالت CURL_CHUNK_BGN_FUNC_FAIL:
          واپس CURLE_CHUNK_FAILED;
        }
      }

      اگر(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        جاری;
      }

      اگر(finfo->flags & CURLFINFOFLAG_معلوم_سائز)
        ftpc->معلوم_filesize = finfo->سائز;

      نتیجہ = ftp_parse_url_راستہ(ڈیٹا, ftpc, ftp);
      اگر(نتیجہ)
        واپس نتیجہ;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_گرہ_remove(Curl_llist_سر(&wildcard->filelist));

      اگر(Curl_llist_گنتی(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        واپس CURLE_OK;
      }
      واپس نتیجہ;
    }

    حالت CURLWC_SKIP: {
      اگر(ڈیٹا->سیٹ.chunk_انتہا) {
        ساخت Curl_mapi_guard guard;
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_chunk_انتہا);
        ڈیٹا->سیٹ.chunk_انتہا(ڈیٹا->سیٹ.wildcardptr);
        CURL_CBAPI_انتہا(&guard);
      }
      Curl_گرہ_remove(Curl_llist_سر(&wildcard->filelist));
      wildcard->state = (Curl_llist_گنتی(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      جاری;
    }

    حالت CURLWC_CLEAN: {
      ساخت ftp_wc *ftpwc = wildcard->ftpwc;
      نتیجہ = CURLE_OK;
      اگر(ftpwc)
        نتیجہ = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = نتیجہ ? CURLWC_ERROR : CURLWC_مکمل;
      واپس نتیجہ;
    }

    حالت CURLWC_مکمل:
    حالت CURLWC_ERROR:
    حالت CURLWC_CLEAR:
      اگر(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = تہی;
      }
      واپس نتیجہ;
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
ساکن CURLcode ftp_regular_transfer(ساخت Curl_easy *ڈیٹا,
                                     ساخت ftp_conn *ftpc,
                                     ساخت FTP *ftp,
                                     bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;
  ڈیٹا->req.سائز = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(ڈیٹا);

  ftpc->ctl_valid = TRUE; /* starts good */

  نتیجہ = ftp_perform(ڈیٹا, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_مکمل); /* all commands in the DO-phase done? */

  اگر(!نتیجہ) {

    اگر(!*dophase_مکمل)
      /* the DO phase has not completed yet */
      واپس CURLE_OK;

    نتیجہ = ftp_dophase_مکمل(ڈیٹا, ftpc, ftp, connected);

    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
    freedirs(ftpc);

  واپس نتیجہ;
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
ساکن CURLcode ftp_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  ساخت FTP *ftp = Curl_meta_get(ڈیٹا, CURL_META_FTP_EASY);

  *مکمل = FALSE; /* default to false */
  اگر(!ftpc || !ftp)
    واپس CURLE_FAILED_INIT;
  ftpc->wait_ڈیٹا_conn = FALSE; /* default to no such wait */

#اگر_تعریف_شدہ CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ساخت Curl_cwriter *ftp_lc_writer;

    نتیجہ = Curl_cwriter_create(&ftp_lc_writer, ڈیٹا, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    اگر(نتیجہ)
      واپس نتیجہ;

    نتیجہ = Curl_cwriter_جوڑو(ڈیٹا, ftp_lc_writer);
    اگر(نتیجہ) {
      Curl_cwriter_free(ڈیٹا, ftp_lc_writer);
      واپس نتیجہ;
    }
  }
#اگر_ختم /* CURL_PREFER_LF_LINEENDS */

  اگر(ڈیٹا->state.wildcardmatch) {
    نتیجہ = wc_statemach(ڈیٹا, ftpc, ftp);
    اگر(ڈیٹا->wildcard->state == CURLWC_SKIP ||
       ڈیٹا->wildcard->state == CURLWC_مکمل) {
      /* do not call ftp_regular_transfer */
      واپس CURLE_OK;
    }
    اگر(نتیجہ) /* error, loop or skipping the file */
      واپس نتیجہ;
  }
  ورنہ { /* no wildcard FSM needed */
    نتیجہ = ftp_parse_url_راستہ(ڈیٹا, ftpc, ftp);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  نتیجہ = ftp_regular_transfer(ڈیٹا, ftpc, ftp, مکمل);

  واپس نتیجہ;
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
ساکن CURLcode ftp_quit(ساخت Curl_easy *ڈیٹا,
                         ساخت ftp_conn *ftpc)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ftpc->ctl_valid) {
    CURL_TRC_FTP(ڈیٹا, "sending QUIT to close session");
    نتیجہ = Curl_pp_sendf(ڈیٹا, &ftpc->pp, "%s", "QUIT");
    اگر(نتیجہ) {
      failf(ڈیٹا, "Failure sending QUIT command: %s",
            curl_easy_strerror(نتیجہ));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(ڈیٹا->conn); /* mark for closure */
      ftp_state(ڈیٹا, ftpc, FTP_STOP);
      واپس نتیجہ;
    }

    ftp_state(ڈیٹا, ftpc, FTP_QUIT);

    نتیجہ = ftp_بلاک_statemach(ڈیٹا, ftpc);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساکن CURLcode ftp_disconnect(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               bool dead_connection)
{
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  اگر(!ftpc)
    واپس CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  اگر(dead_connection || Curl_pp_needs_flush(ڈیٹا, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (خالی)ftp_quit(ڈیٹا, ftpc); /* ignore errors on the QUIT */
  واپس CURLE_OK;
}

/* called from multi.c while DOing */
ساکن CURLcode ftp_doing(ساخت Curl_easy *ڈیٹا,
                          bool *dophase_مکمل)
{
  ساخت ftp_conn *ftpc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_FTP_CONN);
  ساخت FTP *ftp = Curl_meta_get(ڈیٹا, CURL_META_FTP_EASY);
  CURLcode نتیجہ;

  اگر(!ftpc || !ftp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = ftp_statemach(ڈیٹا, ftpc, dophase_مکمل);

  اگر(نتیجہ)
    CURL_TRC_FTP(ڈیٹا, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ورنہ اگر(*dophase_مکمل) {
    نتیجہ = ftp_dophase_مکمل(ڈیٹا, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(ڈیٹا, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  واپس نتیجہ;
}

ساکن خالی ftp_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت FTP *ftp = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ساکن خالی ftp_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ftp_conn *ftpc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_مقصد_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ساکن خالی type_url_check(ساخت Curl_easy *ڈیٹا, ساخت FTP *ftp)
{
  size_t len = لڑی_لمبائی(ftp->راستہ);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  اگر((len >= 7) && !memcmp(&ftp->راستہ[len - 7], ";type=", 6)) {
    حرف *type = &ftp->راستہ[len - 7];
    حرف command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    انتخاب(command) {
    حالت 'A': /* ASCII mode */
      ڈیٹا->state.prefer_ascii = TRUE;
      توڑو;

    حالت 'D': /* directory mode */
      ڈیٹا->state.list_only = TRUE;
      توڑو;

    حالت 'I': /* binary mode */
    طےشدہ:
      /* switch off ASCII */
      ڈیٹا->state.prefer_ascii = FALSE;
      توڑو;
    }
  }
}

ساکن CURLcode ftp_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  ساخت FTP *ftp;
  CURLcode نتیجہ = CURLE_OK;
  ساخت ftp_conn *ftpc;

  ftp = curlx_calloc(1, حجم(*ftp));
  اگر(!ftp ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  ftpc = curlx_calloc(1, حجم(*ftpc));
  اگر(!ftpc ||
     Curl_conn_meta_سیٹ(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  /* clone connection related data that is FTP specific */
  اگر(CURL_EASY_STR(ڈیٹا, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(ڈیٹا, STRING_FTP_ACCOUNT));
    اگر(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  اگر(CURL_EASY_STR(ڈیٹا, STRING_FTP_ALTERNATIVE_مقصد_USER)) {
    ftpc->alternative_مقصد_user =
      curlx_strdup(CURL_EASY_STR(ڈیٹا, STRING_FTP_ALTERNATIVE_مقصد_USER));
    اگر(!ftpc->alternative_مقصد_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  ftp->راستہ = &ڈیٹا->state.اوپر.راستہ[1]; /* do not include the initial slash */

  type_url_check(ڈیٹا, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->معلوم_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = ڈیٹا->سیٹ.use_ssl;
  ftpc->ccc = ڈیٹا->سیٹ.ftp_ccc;

  CURL_TRC_FTP(ڈیٹا, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (صحیح)نتیجہ);
  واپس نتیجہ;
}

bool Curl_ftp_conns_مطابقت(ساخت connectdata *needle, ساخت connectdata *conn)
{
  ساخت ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ساخت ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  اگر(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_مقصد_user,
                     cftpc->alternative_مقصد_user) ||
     (nftpc->ccc != cftpc->ccc))
    واپس FALSE;

  انتخاب(nftpc->use_ssl) {
  حالت CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    واپس (cftpc->use_ssl != CURLUSESSL_NONE);
  طےشدہ:
    واپس (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ثابت ساخت Curl_protocol Curl_protocol_ftp = {
  ftp_تیاری_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_مکمل,                        /* done */
  ftp_do_مزید,                     /* do_more */
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

#اگر_ختم /* CURL_DISABLE_FTP */
