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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "if2ip.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "ftp.h"
#అన్తర్భావ "ftp-int.h"
#అన్తర్భావ "ftplistparser.h"
#అన్తర్భావ "curl_range.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "parsedate.h" /* for the week day and month names */
#అన్తర్భావ "sockaddr.h" /* required for Curl_sockaddr_storage */
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curl_ctype.h"

#నిర్వచితం_కాకపోతే NI_MAXHOST
#నిర్వచించు NI_MAXHOST 1025
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే INET_ADDRSTRLEN
#నిర్వచించు INET_ADDRSTRLEN 16
#అయితే_ముగింపు

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#నిర్వచించు STATUSCODE(పంక్తి) \
  (ISDIGIT((పంక్తి)[0]) && ISDIGIT((పంక్తి)[1]) && ISDIGIT((పంక్తి)[2]))

/* macro to check for the last line in an FTP server response */
#నిర్వచించు LASTLINE(పంక్తి) (STATUSCODE(పంక్తి) && (' ' == (పంక్తి)[3]))

#నిర్వచితమైతే CURLVERBOSE
/* for tracing purposes */
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి ftp_state_పేర్లు[] = {
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
#నిర్వచించు FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_పేర్లు[(ftpc)->state] : "???")

#అయితే_ముగింపు /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
స్థిర శూన్య ftp_state_దిగువ(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం ftp_conn *ftpc,
                          ftpstate newstate
#నిర్వచితమైతే DEBUGBUILD
                          , పూర్ణాంకం lineno
#అయితే_ముగింపు
  )
{
  ఒకవేళ(ftpc->state != newstate) {
#నిర్వచితమైతే DEBUGBUILD
    NOVERBOSE((శూన్య)lineno);
    CURL_TRC_FTP(దత్తాంశం, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_పేర్లు[newstate], lineno);
#లేకపోతే
    CURL_TRC_FTP(దత్తాంశం, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_పేర్లు[newstate]);
#అయితే_ముగింపు
  }
  ftpc->state = newstate;
}

/* Local API functions */
#నిర్వచితం_కాకపోతే DEBUGBUILD
#నిర్వచించు ftp_state(x, y, z) ftp_state_దిగువ(x, y, z)
#లేకపోతే /* !DEBUGBUILD */
#నిర్వచించు ftp_state(x, y, z) ftp_state_దిగువ(x, y, z, __LINE__)
#అయితే_ముగింపు /* DEBUGBUILD */

స్థిర CURLcode ftp_state_mdtm(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp);
స్థిర CURLcode ftp_state_quote(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ftp_conn *ftpc,
                                నిర్మాణం FTP *ftp,
                                bool init, ftpstate instate);
స్థిర CURLcode ftp_nb_type(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ftp_conn *ftpc,
                            నిర్మాణం FTP *ftp,
                            bool ascii, ftpstate newstate);
స్థిర CURLcode getftpresponse(నిర్మాణం Curl_easy *దత్తాంశం, size_t *nreadp,
                               పూర్ణాంకం *ftpcodep);

స్థిర శూన్య freedirs(నిర్మాణం ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ఖాళీ;
}

స్థిర size_t numof_slashes(స్థిరరాశి అక్షరం *str)
{
  స్థిరరాశి అక్షరం *slashPos;
  size_t అంకె = 0;
  చేయి {
    slashPos = strchr(str, '/');
    ఒకవేళ(slashPos) {
      ++అంకె;
      str = slashPos + 1;
    }
  } అయితే(slashPos);
  తిరిగి అంకె;
}

#నిర్వచించు FTP_MAX_DIR_లోతు 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
స్థిర CURLcode ftp_parse_url_మార్గం(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc,
                                   నిర్మాణం FTP *ftp)
{
  స్థిరరాశి అక్షరం *slashPos = ఖాళీ;
  స్థిరరాశి అక్షరం *fileName = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *raw_మార్గం = ఖాళీ; /* URL-decoded "raw" path */
  size_t మార్గం_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ఒకవేళ(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  ఫలితం = Curl_urldecode(ftp->మార్గం, 0, &ftpc->rawpath, &మార్గం_Len, REJECT_CTRL);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "path contains control characters");
    తిరిగి ఫలితం;
  }
  raw_మార్గం = ftpc->rawpath;

  ఎంపిక(దత్తాంశం->సమితి.ftp_filemethod) {
  స్థితి FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ఒకవేళ((మార్గం_Len > 0) && (raw_మార్గం[మార్గం_Len - 1] != '/'))
      fileName = raw_మార్గం;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ఆపు;

  స్థితి FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_మార్గం, '/');
    ఒకవేళ(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_మార్గం;
      ఒకవేళ(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, పరిమాణం(ftpc->dirs[0]));
      ఒకవేళ(!ftpc->dirs)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      ftpc->dirs[0].ప్రారంభం = 0;
      ftpc->dirs[0].len = (పూర్ణాంకం)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    లేకపోతే
      fileName = raw_మార్గం; /* filename only (or empty) */
    ఆపు;

  అప్రమేయ: /* allow pretty much anything */
  స్థితి FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    స్థిరరాశి అక్షరం *curPos = raw_మార్గం;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_మార్గం);

    ఒకవేళ(dirAlloc >= FTP_MAX_DIR_లోతు)
      /* suspiciously deep directory hierarchy */
      తిరిగి CURLE_URL_MALFORMAT;

    ఒకవేళ(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, పరిమాణం(ftpc->dirs[0]));
      ఒకవేళ(!ftpc->dirs)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      /* parse the URL path into separate path components */
      అయితే(dirAlloc--) {
        స్థిరరాశి అక్షరం *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ఒకవేళ(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ఒకవేళ(clen) {
          ftpc->dirs[ftpc->dirdepth].ప్రారంభం = (పూర్ణాంకం)(curPos - raw_మార్గం);
          ftpc->dirs[ftpc->dirdepth].len = (పూర్ణాంకం)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ఆపు;
  } /* switch */

  ఒకవేళ(fileName && *fileName)
    ftpc->file = fileName;
  లేకపోతే
    ftpc->file = ఖాళీ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ఒకవేళ(దత్తాంశం->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(దత్తాంశం, "Uploading to a URL without a filename");
    తిరిగి CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ఒకవేళ((దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD) && (raw_మార్గం[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  లేకపోతే { /* newly created FTP connections are already in entry path */
    స్థిరరాశి అక్షరం *old_మార్గం = దత్తాంశం->conn->బిట్లు.reuse ? ftpc->prevpath : "";
    ఒకవేళ(old_మార్గం) {
      size_t n = మార్గం_Len;
      ఒకవేళ(దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      లేకపోతే
        n -= ftpc->file ? అక్షరశ్రేణి_పొడవు(ftpc->file) : 0;

      ఒకవేళ((అక్షరశ్రేణి_పొడవు(old_మార్గం) == n) && raw_మార్గం && !అక్షరశ్రేణి_పోలిక(raw_మార్గం, old_మార్గం, n)) {
        infof(దత్తాంశం, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
స్థిర పూర్ణాంకం ftp_need_type(నిర్మాణం ftp_conn *ftpc,
                         bool ascii_wanted)
{
  తిరిగి ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

స్థిర శూన్య close_secondarysocket(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ftp_conn *ftpc)
{
  (శూన్య)ftpc;
  CURL_TRC_FTP(దత్తాంశం, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(దత్తాంశం, దత్తాంశం->conn, SECONDARYSOCKET);
}

#నిర్వచితమైతే CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
నిర్మాణం ftp_cw_lc_ctx {
  నిర్మాణం Curl_cwriter super;
  bool newline_pending;
};

స్థిర CURLcode ftp_cw_lc_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                స్థిరరాశి అక్షరం *buf, size_t blen)
{
  స్థిర స్థిరరాశి అక్షరం nl = '\n';
  నిర్మాణం ftp_cw_lc_ctx *ctx = writer->ctx;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);

  ఒకవేళ(!ftpc)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  అయితే(blen) {
    /* do not pass EOS when writing parts */
    పూర్ణాంకం chunk_type = (type & ~CLIENTWRITE_EOS);
    స్థిరరాశి అక్షరం *cp;
    size_t chunk_len;
    CURLcode ఫలితం;

    ఒకవేళ(ctx->newline_pending) {
      ఒకవేళ(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, chunk_type, &nl, 1);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ఒకవేళ(!cp)
      ఆపు;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ఒకవేళ(chunk_len) {
      ఫలితం = Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, chunk_type,
                                  buf, chunk_len);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ఒకవేళ(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, blen);
  }
  లేకపోతే ఒకవేళ(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ఒకవేళ(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, 0);
  }
  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ఖాళీ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  పరిమాణం(నిర్మాణం ftp_cw_lc_ctx)
};

#అయితే_ముగింపు /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
స్థిర CURLcode ftp_check_ctrl_on_దత్తాంశం_wait(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ftp_conn *ftpc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  నిర్మాణం pingpong *pp = &ftpc->pp;
  size_t nread;
  పూర్ణాంకం ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ఒకవేళ(curlx_dyn_len(&pp->recvbuf)) {
    స్థిరరాశి అక్షరం *l = curlx_dyn_ptr(&pp->recvbuf);
    ఒకవేళ(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(దత్తాంశం, "There is negative response in cache while serv connect");
      (శూన్య)getftpresponse(దత్తాంశం, &nread, &ftpcode);
      తిరిగి CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ఒకవేళ(pp->అధికప్రవాహం)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  లేకపోతే {
    పూర్ణాంకం socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ఎంపిక(socketstate) {
    స్థితి -1: /* error */
      /* let's die here */
      failf(దత్తాంశం, "Error while waiting for server connect");
      తిరిగి CURLE_FTP_ACCEPT_FAILED;
    అప్రమేయ:
      ఒకవేళ(socketstate & CURL_CSELECT_లోపల)
        response = TRUE;
      ఆపు;
    }
  }

  ఒకవేళ(response) {
    infof(దత్తాంశం, "Ctrl conn has data while waiting for data conn");
    ఒకవేళ(pp->అధికప్రవాహం > 3) {
      స్థిరరాశి అక్షరం *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->అధికప్రవాహం + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ఒకవేళ((len > 3) && LASTLINE(r)) {
        curl_off_t స్థితి_2;
        ఒకవేళ(!curlx_str_సంఖ్య(&r, &స్థితి_2, 999) && (స్థితి_2 == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(దత్తాంశం, "Got 226 before data activity");
          తిరిగి CURLE_OK;
        }
      }
    }

    (శూన్య)getftpresponse(దత్తాంశం, &nread, &ftpcode);

    infof(దత్తాంశం, "FTP code: %03d", ftpcode);

    ఒకవేళ(ftpcode / 100 > 3)
      తిరిగి CURLE_FTP_ACCEPT_FAILED;

    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
స్థిర CURLcode ftp_initiate_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ftp_conn *ftpc)
{
  CURLcode ఫలితం = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(దత్తాంశం, "ftp_initiate_transfer()");
  ఫలితం = Curl_conn_connect(దత్తాంశం, SECONDARYSOCKET, TRUE, &connected);
  ఒకవేళ(ఫలితం || !connected)
    తిరిగి ఫలితం;

  ఒకవేళ(దత్తాంశం->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_అమరిక_send(దత్తాంశం, SECONDARYSOCKET);
    Curl_xfer_సమితి_shutdown(దత్తాంశం, TRUE, TRUE);
  }
  లేకపోతే {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_అమరిక_recv(దత్తాంశం, SECONDARYSOCKET, దత్తాంశం->req.కొలత);
    Curl_xfer_సమితి_shutdown(దత్తాంశం, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(దత్తాంశం, ftpc, FTP_STOP);

  తిరిగి CURLE_OK;
}

స్థిర bool ftp_endofresp(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                          స్థిరరాశి అక్షరం *పంక్తి, size_t len, పూర్ణాంకం *సంకేతం_2)
{
  curl_off_t స్థితి_2;
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;

  ఒకవేళ((len > 3) && LASTLINE(పంక్తి) && !curlx_str_సంఖ్య(&పంక్తి, &స్థితి_2, 999)) {
    *సంకేతం_2 = (పూర్ణాంకం)స్థితి_2;
    తిరిగి TRUE;
  }

  తిరిగి FALSE;
}

స్థిర CURLcode ftp_readresp(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ftp_conn *ftpc,
                             int8_t sockindex,
                             నిర్మాణం pingpong *pp,
                             పూర్ణాంకం *ftpcodep, /* return the ftp-code if done */
                             size_t *కొలత) /* size of the response */
{
  పూర్ణాంకం సంకేతం_2;
  CURLcode ఫలితం = Curl_pp_readresp(దత్తాంశం, sockindex, pp, &సంకేతం_2, కొలత);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ఒకవేళ(!ftpc->shutdown)
    దత్తాంశం->info.httpcode = సంకేతం_2;

  *ftpcodep = సంకేతం_2;

  ఒకవేళ(సంకేతం_2 == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(దత్తాంశం, "We got a 421 - timeout");
    ftp_state(దత్తాంశం, ftpc, FTP_STOP);
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }

  తిరిగి ఫలితం;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
స్థిర CURLcode getftpresponse(నిర్మాణం Curl_easy *దత్తాంశం,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               పూర్ణాంకం *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  నిర్మాణం pingpong *pp = &ftpc->pp;
  size_t nread;
  పూర్ణాంకం cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(దత్తాంశం, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ఒకవేళ(!ftpc)
    తిరిగి CURLE_FAILED_INIT;

  అయితే(!*ftpcodep && !ఫలితం) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(దత్తాంశం, pp);
    timediff_t interval_ms;

    ఒకవేళ(timeout <= 0) {
      failf(దత్తాంశం, "FTP response timeout");
      తిరిగి CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ఒకవేళ(timeout < interval_ms)
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

    ఒకవేళ(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    లేకపోతే ఒకవేళ(!Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(దత్తాంశం, pp) ?
        sockfd : CURL_SOCKET_BAD;
      పూర్ణాంకం ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ఒకవేళ(ev < 0) {
        failf(దత్తాంశం, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        తిరిగి CURLE_RECV_ERROR;
      }
      లేకపోతే ఒకవేళ(ev == 0) {
        ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
        కొనసాగు; /* continue in our loop for the timeout duration */
      }
    }

    ఒకవేళ(Curl_pp_needs_flush(దత్తాంశం, pp)) {
      ఫలితం = Curl_pp_flushsend(దత్తాంశం, pp);
      ఒకవేళ(ఫలితం)
        ఆపు;
    }

    ఫలితం = ftp_readresp(దత్తాంశం, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
దత్తాంశం*/
      cache_skip++;
    లేకపోతే
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(దత్తాంశం, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (పూర్ణాంకం)ఫలితం, *nreadp, *ftpcodep);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_user(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ఒకవేళ(!ఫలితం) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(దత్తాంశం, ftpc, FTP_USER);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_pwd(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ftp_conn *ftpc)
{
  CURLcode ఫలితం;
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(!దత్తాంశం->id && getenv("CURL_FTP_PWD_STOP"))
    తిరిగి CURLE_OK;
#అయితే_ముగింపు
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "PWD");
  ఒకవేళ(!ఫలితం)
    ftp_state(దత్తాంశం, ftpc, FTP_PWD);

  తిరిగి ఫలితం;
}

/* For the FTP "protocol connect" and "doing" phases only */
స్థిర CURLcode ftp_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  తిరిగి ftpc ? Curl_pp_pollset(దత్తాంశం, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
స్థిర CURLcode ftp_domore_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);

  ఒకవేళ(!ftpc)
    తిరిగి CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(దత్తాంశం, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ఒకవేళ(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(దత్తాంశం->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (దత్తాంశం->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(దత్తాంశం->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    తిరిగి Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
  }
  తిరిగి Curl_pp_pollset(దత్తాంశం, &ftpc->pp, ps);
}

స్థిర పూర్ణాంకం pathlen(నిర్మాణం ftp_conn *ftpc, పూర్ణాంకం అంకె)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > అంకె);
  తిరిగి ftpc->dirs[అంకె].len;
}

స్థిర స్థిరరాశి అక్షరం *pathpiece(నిర్మాణం ftp_conn *ftpc, పూర్ణాంకం అంకె)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > అంకె);
  తిరిగి &ftpc->rawpath[ftpc->dirs[అంకె].ప్రారంభం];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
స్థిర CURLcode ftp_state_cwd(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ftp_conn *ftpc,
                              నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ftpc->cwddone)
    /* already done and fine */
    ఫలితం = ftp_state_mdtm(దత్తాంశం, ftpc, ftp);
  లేకపోతే {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((దత్తాంశం->సమితి.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ఒకవేళ(దత్తాంశం->conn->బిట్లు.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_CWD);
    }
    లేకపోతే {
      ఒకవేళ(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ఒకవేళ(!ఫలితం)
          ftp_state(దత్తాంశం, ftpc, FTP_CWD);
      }
      లేకపోతే {
        /* No CWD necessary */
        ఫలితం = ftp_state_mdtm(దత్తాంశం, ftpc, ftp);
      }
    }
  }
  తిరిగి ఫలితం;
}

రకనిర్వచనం గణన {
  EPRT,
  పోర్టు_2,
  పూర్తి_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
స్థిర CURLcode ftp_పోర్టు_parse_string(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn,
                                      స్థిరరాశి అక్షరం *string_ftpport,
                                      నిర్మాణం Curl_sockaddr_storage *ss,
                                      గుర్తులేని పొట్టి *పోర్టు_minp,
                                      గుర్తులేని పొట్టి *పోర్టు_maxp,
                                      స్థిరరాశి అక్షరం **hostp,
                                      అక్షరం *hbuf, size_t hbuflen)
{
  స్థిరరాశి అక్షరం *ip_చివర = ఖాళీ;
  స్థిరరాశి అక్షరం *addr = ఖాళీ;
  size_t addrlen = 0;
  గుర్తులేని పొట్టి పోర్టు_min = 0;
  గుర్తులేని పొట్టి పోర్టు_max = 0;
  అక్షరం ipstr[50];
#నిర్వచితం_కాకపోతే USE_IPV6
  (శూన్య)conn;
  (శూన్య)ss;
#అయితే_ముగింపు

  /* default to nothing */
  *hostp = ఖాళీ;
  *పోర్టు_minp = *పోర్టు_maxp = 0;

  ఒకవేళ(!string_ftpport || (అక్షరశ్రేణి_పొడవు(string_ftpport) <= 1))
    వెళ్ళు పూర్తి;

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    స్థిరరాశి అక్షరం *ip_ప్రారంభం = string_ftpport + 1;
    ip_చివర = strchr(ip_ప్రారంభం, ']');
    ఒకవేళ(ip_చివర) {
      addrlen = ip_చివర - ip_ప్రారంభం;
      addr = ip_ప్రారంభం;
    }
  }
  లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ(*string_ftpport == ':') {
      /* :port */
      ip_చివర = string_ftpport;
    }
    లేకపోతే {
      ip_చివర = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ఒకవేళ(ip_చివర) {
#నిర్వచితమైతే USE_IPV6
        నిర్మాణం sockaddr_in6 * స్థిరరాశి sa6 = (శూన్య *)ss;
#అయితే_ముగింపు
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_చివర - string_ftpport;
#నిర్వచితమైతే USE_IPV6
        ఒకవేళ(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = అక్షరశ్రేణి_పొడవు(string_ftpport);
          ip_చివర = ఖాళీ; /* this got no port ! */
        }
#అయితే_ముగింపు
      }
      లేకపోతే
        /* ipv4|interface */
        addrlen = అక్షరశ్రేణి_పొడవు(string_ftpport);
    }

  /* parse the port */
  ఒకవేళ(ip_చివర) {
    స్థిరరాశి అక్షరం *portp = strchr(ip_చివర, ':');
    ఒకవేళ(portp) {
      curl_off_t ప్రారంభం;
      curl_off_t చివర;
      portp++;
      ఒకవేళ(!curlx_str_సంఖ్య(&portp, &ప్రారంభం, 0xffff)) {
        పోర్టు_min = (గుర్తులేని పొట్టి)ప్రారంభం;
        ఒకవేళ(!curlx_str_single(&portp, '-') &&
           !curlx_str_సంఖ్య(&portp, &చివర, 0xffff))
          పోర్టు_max = (గుర్తులేని పొట్టి)చివర;
        లేకపోతే
          పోర్టు_max = పోర్టు_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ఒకవేళ(పోర్టు_min > పోర్టు_max)
    పోర్టు_min = పోర్టు_max = 0;

  ఒకవేళ(addrlen) {
    స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(దత్తాంశం, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ఒకవేళ(!remote_addr || (addrlen >= పరిమాణం(ipstr)) || (addrlen >= hbuflen))
      తిరిగి CURLE_FTP_పోర్టు_FAILED;
    జ్ఞాపకం_నకలు(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ఎంపిక(Curl_if2ip(remote_addr->family,
#నిర్వచితమైతే USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#అయితే_ముగింపు
                      ipstr, hbuf, hbuflen)) {
    స్థితి IF2_IP_NOT_దొరికింది:
      /* not an interface, use the string as hostname instead */
      జ్ఞాపకం_నకలు(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ఆపు;
    స్థితి IF2IP_AF_NOT_SUPPORTED:
      తిరిగి CURLE_FTP_పోర్టు_FAILED;
    స్థితి IF2_IP_దొరికింది:
      *hostp = hbuf; /* use the hbuf for hostname */
      ఆపు;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

పూర్తి:
  *పోర్టు_minp = పోర్టు_min;
  *పోర్టు_maxp = పోర్టు_max;
  తిరిగి CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
స్థిర CURLcode ftp_పోర్టు_default_host(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn,
                                      నిర్మాణం Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      స్థిరరాశి అక్షరం **hostp,
                                      అక్షరం *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  నిర్మాణం sockaddr *sa = (నిర్మాణం sockaddr *)ss;
  నిర్మాణం sockaddr_in * స్థిరరాశి sa4 = (శూన్య *)sa;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 * స్థిరరాశి sa6 = (శూన్య *)sa;
#అయితే_ముగింపు
  అక్షరం బఫరు[STRERROR_LEN];
  CURLcode ఫలితం;

  *sslenp = పరిమాణం(*ss);
  ఒకవేళ(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(దత్తాంశం, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_FTP_పోర్టు_FAILED;
  }
  ఎంపిక(sa->sa_family) {
#నిర్వచితమైతే USE_IPV6
  స్థితి AF_INET6:
    ఫలితం = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    ఫలితం = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ఆపు;
  }
  ఒకవేళ(ఫలితం)
    తిరిగి CURLE_FTP_పోర్టు_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  తిరిగి CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
స్థిర CURLcode ftp_పోర్టు_resolve_host(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn,
                                      స్థిరరాశి అక్షరం *host,
                                      నిర్మాణం Curl_dns_నమోదు **dns_entryp,
                                      స్థిరరాశి నిర్మాణం Curl_addrinfo **resp)
{
  CURLcode ఫలితం;

  *resp = ఖాళీ;
  ఫలితం = Curl_resolv_blocking(
    దత్తాంశం, Curl_resolv_dns_queries(దత్తాంశం, conn->ip_సంస్కరణ),
    host, 0, Curl_conn_get_transport(దత్తాంశం, conn), dns_entryp);
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "failed to resolve the address provided to PORT: %s", host);
  లేకపోతే {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  తిరిగి ఫలితం;
}

/*
 * Open a TCP socket for the resolved address family.
 */
స్థిర CURLcode ftp_పోర్టు_open_socket(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn,
                                     స్థిరరాశి నిర్మాణం Curl_addrinfo *res,
                                     స్థిరరాశి నిర్మాణం Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  అక్షరం బఫరు[STRERROR_LEN];
  పూర్ణాంకం sockerr = 0;
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai;
  CURLcode ఫలితం = CURLE_FTP_పోర్టు_FAILED;

  కొరకు(ai = res; ai; ai = ai->ai_తదుపరి) {
    ఫలితం =
      Curl_socket_open(దత్తాంశం, ai, ఖాళీ,
                       Curl_conn_get_transport(దత్తాంశం, conn), portsockp);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        తిరిగి ఫలితం;
      ఫలితం = CURLE_FTP_పోర్టు_FAILED;
      sockerr = SOCKERRNO;
      కొనసాగు;
    }
    ఆపు;
  }
  ఒకవేళ(!ai) {
    failf(దత్తాంశం, "socket failure: %s",
          curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_FTP_పోర్టు_FAILED;
  }
  *aip = ai;
  తిరిగి ఫలితం;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
స్థిర CURLcode ftp_పోర్టు_bind_socket(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn,
                                     curl_socket_t portsock,
                                     స్థిరరాశి నిర్మాణం Curl_addrinfo *ai,
                                     నిర్మాణం Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     గుర్తులేని పొట్టి పోర్టు_min,
                                     గుర్తులేని పొట్టి పోర్టు_max,
                                     bool non_local)
{
  నిర్మాణం sockaddr *sa = (నిర్మాణం sockaddr *)ss;
  నిర్మాణం sockaddr_in * స్థిరరాశి sa4 = (శూన్య *)sa;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 * స్థిరరాశి sa6 = (శూన్య *)sa;
#అయితే_ముగింపు
  అక్షరం బఫరు[STRERROR_LEN];
  గుర్తులేని పొట్టి పోర్టు;
  పూర్ణాంకం sockerr;

  జ్ఞాపకం_నకలు(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  కొరకు(పోర్టు = పోర్టు_min; పోర్టు <= పోర్టు_max;) {
    ఒకవేళ(sa->sa_family == AF_INET)
      sa4->sin_port = htons(పోర్టు);
#నిర్వచితమైతే USE_IPV6
    లేకపోతే
      sa6->sin6_port = htons(పోర్టు);
#అయితే_ముగింపు
    ఒకవేళ(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ఒకవేళ(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(దత్తాంశం, "bind(port=%hu) on non-local address failed: %s", పోర్టు,
              curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));

        *sslen_io = పరిమాణం(*ss);
        ఒకవేళ(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(దత్తాంశం, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
          తిరిగి CURLE_FTP_పోర్టు_FAILED;
        }
        పోర్టు = పోర్టు_min;
        non_local = FALSE; /* do not try this again */
        కొనసాగు;
      }
      ఒకవేళ(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(దత్తాంశం, "bind(port=%hu) failed: %s", పోర్టు,
              curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
        తిరిగి CURLE_FTP_పోర్టు_FAILED;
      }
    }
    లేకపోతే
      ఆపు;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ఒకవేళ(పోర్టు == పోర్టు_max) {
      failf(దత్తాంశం, "bind() failed, ran out of ports");
      తిరిగి CURLE_FTP_పోర్టు_FAILED;
    }
    పోర్టు++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = పరిమాణం(*ss);
  ఒకవేళ(getsockname(portsock, sa, sslen_io)) {
    failf(దత్తాంశం, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_FTP_పోర్టు_FAILED;
  }
  CURL_TRC_FTP(దత్తాంశం, "ftp_port_bind_socket(), socket bound to port %d", పోర్టు);
  తిరిగి CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
స్థిర CURLcode ftp_పోర్టు_listen(నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t portsock)
{
  అక్షరం బఫరు[STRERROR_LEN];

  ఒకవేళ(listen(portsock, 1)) {
    failf(దత్తాంశం, "socket failure: %s",
          curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_FTP_పోర్టు_FAILED;
  }
  CURL_TRC_FTP(దత్తాంశం, "ftp_port_listen(), listening on port");
  తిరిగి CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
స్థిర CURLcode ftp_పోర్టు_send_command(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ftp_conn *ftpc,
                                      నిర్మాణం connectdata *conn,
                                      నిర్మాణం Curl_sockaddr_storage *ss,
                                      స్థిరరాశి నిర్మాణం Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  స్థిర స్థిరరాశి అక్షరం బహుళకం[][5] = { "EPRT", "PORT" };
  నిర్మాణం sockaddr *sa = (నిర్మాణం sockaddr *)ss;
  నిర్మాణం sockaddr_in * స్థిరరాశి sa4 = (శూన్య *)sa;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 * స్థిరరాశి sa6 = (శూన్య *)sa;
#అయితే_ముగింపు
  అక్షరం myhost[MAX_IPADR_LEN + 1] = "";
  గుర్తులేని పొట్టి పోర్టు;
  CURLcode ఫలితం;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_చిరునామా(ai, myhost, పరిమాణం(myhost));

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(!conn->బిట్లు.ftp_use_eprt && conn->బిట్లు.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->బిట్లు.ftp_use_eprt = TRUE;
#అయితే_ముగింపు

  కొరకు(; fcmd != పూర్తి_2; fcmd++) {

    ఒకవేళ(!conn->బిట్లు.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      కొనసాగు;

    ఒకవేళ((పోర్టు_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      కొనసాగు;

    ఎంపిక(sa->sa_family) {
    స్థితి AF_INET:
      పోర్టు = ntohs(sa4->sin_port);
      ఆపు;
#నిర్వచితమైతే USE_IPV6
    స్థితి AF_INET6:
      పోర్టు = ntohs(sa6->sin6_port);
      ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
      కొనసాగు; /* might as well skip this */
    }

    ఒకవేళ(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s |%d|%s|%hu|", బహుళకం[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, పోర్టు);
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "Failure sending EPRT command: %s",
              curl_easy_strerror(ఫలితం));
        తిరిగి ఫలితం;
      }
      ఆపు;
    }
    ఒకవేళ(పోర్టు_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      అక్షరం లక్ష్యం[పరిమాణం(myhost) + 20];
      స్థిరరాశి అక్షరం *ఆధారం = myhost;
      అక్షరం *dest = లక్ష్యం;

      /* translate x.x.x.x to x,x,x,x */
      అయితే(*ఆధారం) {
        ఒకవేళ(*ఆధారం == '.')
          *dest = ',';
        లేకపోతే
          *dest = *ఆధారం;
        dest++;
        ఆధారం++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (పూర్ణాంకం)(పోర్టు >> 8), (పూర్ణాంకం)(పోర్టు & 0xff));

      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s %s", బహుళకం[fcmd], లక్ష్యం);
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "Failure sending PORT command: %s",
              curl_easy_strerror(ఫలితం));
        తిరిగి ఫలితం;
      }
      ఆపు;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(దత్తాంశం, ftpc, FTP_పోర్టు);
  తిరిగి CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
స్థిర CURLcode ftp_state_use_పోర్టు(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode ఫలితం = CURLE_FTP_పోర్టు_FAILED;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  నిర్మాణం Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  అక్షరం hbuf[NI_MAXHOST];
  స్థిరరాశి అక్షరం *host = ఖాళీ;
  స్థిరరాశి అక్షరం *string_ftpport = CURL_EASY_STR(దత్తాంశం, STRING_FTPPORT);
  నిర్మాణం Curl_dns_నమోదు *dns_నమోదు = ఖాళీ;
  స్థిరరాశి నిర్మాణం Curl_addrinfo *res = ఖాళీ;
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai = ఖాళీ;
  గుర్తులేని పొట్టి పోర్టు_min = 0;
  గుర్తులేని పొట్టి పోర్టు_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  ఫలితం = ftp_పోర్టు_parse_string(దత్తాంశం, conn, string_ftpport,
                                 &ss, &పోర్టు_min, &పోర్టు_max,
                                 &host, hbuf, పరిమాణం(hbuf));
  ఒకవేళ(!ఫలితం && !host)
    /* if no host was specified, use the control connection's local IP */
    ఫలితం = ftp_పోర్టు_default_host(దత్తాంశం, conn, &ss, &sslen, &host,
                                   hbuf, పరిమాణం(hbuf), &non_local);

  /* resolve host string to address list */
  ఒకవేళ(!ఫలితం)
    ఫలితం = ftp_పోర్టు_resolve_host(దత్తాంశం, conn, host, &dns_నమోదు, &res);

  /* Open a TCP socket for the data connection */
  ఒకవేళ(!ఫలితం)
    ఫలితం = ftp_పోర్టు_open_socket(దత్తాంశం, conn, res, &ai, &portsock);
  ఒకవేళ(!ఫలితం) {
    CURL_TRC_FTP(దత్తాంశం, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    ఫలితం = ftp_పోర్టు_bind_socket(దత్తాంశం, conn, portsock, ai, &ss, &sslen,
                                  పోర్టు_min, పోర్టు_max, non_local);
  }

  /* listen */
  ఒకవేళ(!ఫలితం)
    ఫలితం = ftp_పోర్టు_listen(దత్తాంశం, portsock);

  /* send the PORT / EPRT command */
  ఒకవేళ(!ఫలితం)
    ఫలితం = ftp_పోర్టు_send_command(దత్తాంశం, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_tcp_listen_సమితి(దత్తాంశం, conn, SECONDARYSOCKET, &portsock);

  ఒకవేళ(!ఫలితం)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ఒకవేళ(dns_నమోదు)
    Curl_dns_నమోదు_unlink(దత్తాంశం, &dns_నమోదు);
  ఒకవేళ(ఫలితం) {
    ftp_state(దత్తాంశం, ftpc, FTP_STOP);
  }
  లేకపోతే {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ఒకవేళ(conn->బిట్లు.ftp_use_దత్తాంశం_ssl && దత్తాంశం->సమితి.ftp_use_పోర్టు &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      ఫలితం = Curl_ssl_cfilter_కూడు(
        దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->బిట్లు.do_మరిన్ని = FALSE;
    Curl_expire(దత్తాంశం, (దత్తాంశం->సమితి.accepttimeout > 0) ?
                దత్తాంశం->సమితి.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  ఒకవేళ(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(దత్తాంశం, conn, portsock);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_use_pasv(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc,
                                   నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  స్థిర స్థిరరాశి అక్షరం బహుళకం[][5] = { "EPSV", "PASV" };
  పూర్ణాంకం modeoff;

#నిర్వచితమైతే PF_INET6
  ఒకవేళ(!conn->బిట్లు.ftp_use_epsv && conn->బిట్లు.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->బిట్లు.ftp_use_epsv = TRUE;
#అయితే_ముగింపు

  modeoff = conn->బిట్లు.ftp_use_epsv ? 0 : 1;

  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", బహుళకం[modeoff]);
  ఒకవేళ(!ఫలితం) {
    ftpc->count1 = modeoff;
    ftp_state(దత్తాంశం, ftpc, FTP_PASV);
    infof(దత్తాంశం, "Connect data stream passively");
  }
  తిరిగి ఫలితం;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
స్థిర CURLcode ftp_state_prepare_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ftp_conn *ftpc,
                                           నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  ఒకవేళ(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(దత్తాంశం, ftpc, FTP_RETR_PREQUOTE);
    ఫలితం = ftp_state_quote(దత్తాంశం, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.ftp_use_పోర్టు) {
    /* We have chosen to use the PORT (or similar) command */
    ఫలితం = ftp_state_use_పోర్టు(దత్తాంశం, ftpc, EPRT);
  }
  లేకపోతే {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ఒకవేళ(దత్తాంశం->సమితి.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ఒకవేళ(!ftpc->file)
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) :
                               (దత్తాంశం->state.list_only ? "NLST" : "LIST"));
      లేకపోతే ఒకవేళ(దత్తాంశం->state.upload)
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PRET STOR %s", ftpc->file);
      లేకపోతే
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_PRET);
    }
    లేకపోతే
      ఫలితం = ftp_state_use_pasv(దత్తాంశం, ftpc, conn);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_rest(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "REST %d", 0);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_REST);
  }
  లేకపోతే
    ఫలితం = ftp_state_prepare_transfer(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_కొలత(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "SIZE %s", ftpc->file);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_కొలత);
  }
  లేకపోతే
    ఫలితం = ftp_state_rest(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_list(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  స్థిరరాశి అక్షరం *lstArg = ఖాళీ;
  పూర్ణాంకం lstArglen = 0;
  అక్షరం *cmd;

  ఒకవేళ((దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD) && ftp->మార్గం) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    స్థిరరాశి అక్షరం *raw_మార్గం = ftpc->rawpath;
    స్థిరరాశి అక్షరం *slashPos = strrchr(raw_మార్గం, '/');
    ఒకవేళ(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_మార్గం;
      ఒకవేళ(n == 0)
        ++n;

      lstArg = raw_మార్గం;
      lstArglen = (పూర్ణాంకం)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) :
                      (దత్తాంశం->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ఒకవేళ(!cmd)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ఒకవేళ(!ఫలితం)
    ftp_state(దత్తాంశం, ftpc, FTP_LIST);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_list_prequote(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ftp_conn *ftpc,
                                        నిర్మాణం FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  తిరిగి ftp_state_quote(దత్తాంశం, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

స్థిర CURLcode ftp_state_retr_prequote(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ftp_conn *ftpc,
                                        నిర్మాణం FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  తిరిగి ftp_state_quote(దత్తాంశం, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

స్థిర CURLcode ftp_state_stor_prequote(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ftp_conn *ftpc,
                                        నిర్మాణం FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  తిరిగి ftp_state_quote(దత్తాంశం, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

స్థిర CURLcode ftp_state_type(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ఒకవేళ(దత్తాంశం->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)దత్తాంశం->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    ఫలితం = ftp_nb_type(దత్తాంశం, ftpc, ftp, (bool)దత్తాంశం->state.prefer_ascii,
                         FTP_TYPE);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
    ఫలితం = ftp_state_కొలత(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
స్థిర CURLcode ftp_state_mdtm(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp)
{
  CURLcode ఫలితం = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ఒకవేళ((దత్తాంశం->సమితి.get_filetime || దత్తాంశం->సమితి.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "MDTM %s", ftpc->file);

    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_MDTM);
  }
  లేకపోతే
    ఫలితం = ftp_state_type(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

/* This is called after the TYPE and possible quote commands have been sent */
స్థిర CURLcode ftp_state_ul_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc,
                                   నిర్మాణం FTP *ftp,
                                   bool sizechecked)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_బిట్టు append = దత్తాంశం->సమితి.remote_append;

  ఒకవేళ((దత్తాంశం->state.resume_from && !sizechecked) ||
     ((దత్తాంశం->state.resume_from > 0) && sizechecked)) {
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
    పూర్ణాంకం seekerr = CURL_SEEKFUNC_OK;

    ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "SIZE %s", ftpc->file);
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_STOR_కొలత);
      తిరిగి ఫలితం;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ఒకవేళ(దత్తాంశం->సమితి.seek_func) {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_seek_func);
      seekerr = దత్తాంశం->సమితి.seek_func(దత్తాంశం->సమితి.seek_client,
                                    దత్తాంశం->state.resume_from, SEEK_SET);
      CURL_CBAPI_చివర(&guard);
    }

    ఒకవేళ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ఒకవేళ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(దత్తాంశం, "Could not seek stream");
        తిరిగి CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      చేయి {
        అక్షరం scratch[4 * 1024];
        size_t readthisamountnow =
          (దత్తాంశం->state.resume_from - passed > (curl_off_t)పరిమాణం(scratch)) ?
          పరిమాణం(scratch) :
          curlx_sotouz(దత్తాంశం->state.resume_from - passed);

        size_t actuallyread =
          దత్తాంశం->state.fread_func(scratch, 1, readthisamountnow,
                                 దత్తాంశం->state.లోపల);

        passed += actuallyread;
        ఒకవేళ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(దత్తాంశం, "Failed to read data");
          తిరిగి CURLE_FTP_COULDNT_USE_REST;
        }
      } అయితే(passed < దత్తాంశం->state.resume_from);
    }
    /* now, decrease the size of the read */
    ఒకవేళ(దత్తాంశం->state.infilesize > 0) {
      దత్తాంశం->state.infilesize -= దత్తాంశం->state.resume_from;

      ఒకవేళ(దత్తాంశం->state.infilesize <= 0) {
        infof(దత్తాంశం, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_అమరిక_nop(దత్తాంశం);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(దత్తాంశం, ftpc, FTP_STOP);
        తిరిగి CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ఒకవేళ(!ఫలితం)
    ftp_state(దత్తాంశం, ftpc, FTP_STOR);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_retr(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ftp_conn *ftpc,
                               నిర్మాణం FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_FTP(దత్తాంశం, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ఒకవేళ(దత్తాంశం->సమితి.max_filesize && (filesize > దత్తాంశం->సమితి.max_filesize)) {
    failf(దత్తాంశం, "Maximum file size exceeded");
    తిరిగి CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ఒకవేళ(దత్తాంశం->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ఒకవేళ(filesize == -1) {
      infof(దత్తాంశం, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    లేకపోతే {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ఒకవేళ(filesize < -దత్తాంశం->state.resume_from) {
          failf(దత్తాంశం, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                దత్తాంశం->state.resume_from, filesize);
          తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -దత్తాంశం->state.resume_from;
        /* download from where? */
        దత్తాంశం->state.resume_from = filesize - ftp->downloadsize;
      }
      లేకపోతే {
        ఒకవేళ(filesize < దత్తాంశం->state.resume_from) {
          failf(దత్తాంశం, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                దత్తాంశం->state.resume_from, filesize);
          తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - దత్తాంశం->state.resume_from;
      }
    }

    ఒకవేళ(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_అమరిక_nop(దత్తాంశం);
      infof(దత్తాంశం, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(దత్తాంశం, ftpc, FTP_STOP);
      తిరిగి CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(దత్తాంశం, "Instructs server to resume from offset %" FMT_OFF_T,
          దత్తాంశం->state.resume_from);

    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "REST %" FMT_OFF_T,
                           దత్తాంశం->state.resume_from);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_RETR_REST);
  }
  లేకపోతే {
    /* no resume */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "RETR %s", ftpc->file);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_RETR);
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_quote(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ftp_conn *ftpc,
                                నిర్మాణం FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  bool quote = FALSE;
  నిర్మాణం curl_slist *అంశం;

  ఎంపిక(instate) {
  స్థితి FTP_QUOTE:
  అప్రమేయ:
    అంశం = దత్తాంశం->సమితి.quote;
    ఆపు;
  స్థితి FTP_RETR_PREQUOTE:
  స్థితి FTP_STOR_PREQUOTE:
  స్థితి FTP_LIST_PREQUOTE:
    అంశం = దత్తాంశం->సమితి.prequote;
    ఆపు;
  స్థితి FTP_POSTQUOTE:
    అంశం = దత్తాంశం->సమితి.postquote;
    ఆపు;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ఒకవేళ(init)
    ftpc->count1 = 0;
  లేకపోతే
    ftpc->count1++;

  ఒకవేళ(అంశం) {
    పూర్ణాంకం i = 0;

    /* Skip count1 items in the linked list */
    అయితే((i < ftpc->count1) && అంశం) {
      అంశం = అంశం->తదుపరి;
      i++;
    }
    ఒకవేళ(అంశం) {
      స్థిరరాశి అక్షరం *cmd = అంశం->దత్తాంశం;
      ఒకవేళ(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      లేకపోతే
        ftpc->count2 = 0; /* failure means cancel operation */

      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", cmd);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ftp_state(దత్తాంశం, ftpc, instate);
      quote = TRUE;
    }
  }

  ఒకవేళ(!quote) {
    /* No more quote to send, continue to ... */
    ఎంపిక(instate) {
    స్థితి FTP_QUOTE:
    అప్రమేయ:
      ఫలితం = ftp_state_cwd(దత్తాంశం, ftpc, ftp);
      ఆపు;
    స్థితి FTP_RETR_PREQUOTE:
      ఒకవేళ(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(దత్తాంశం, ftpc, FTP_STOP);
      లేకపోతే {
        ఒకవేళ(ftpc->తెలిసిన_filesize != -1) {
          Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, ftpc->తెలిసిన_filesize);
          ఫలితం = ftp_state_retr(దత్తాంశం, ftpc, ftp, ftpc->తెలిసిన_filesize);
        }
        లేకపోతే {
          ఒకవేళ(దత్తాంశం->సమితి.ignorecl || దత్తాంశం->state.prefer_ascii) {
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
            ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "RETR %s", ftpc->file);
            ఒకవేళ(!ఫలితం)
              ftp_state(దత్తాంశం, ftpc, FTP_RETR);
          }
          లేకపోతే {
            ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "SIZE %s", ftpc->file);
            ఒకవేళ(!ఫలితం)
              ftp_state(దత్తాంశం, ftpc, FTP_RETR_కొలత);
          }
        }
      }
      ఆపు;
    స్థితి FTP_STOR_PREQUOTE:
      ఫలితం = ftp_state_ul_అమరిక(దత్తాంశం, ftpc, ftp, FALSE);
      ఆపు;
    స్థితి FTP_POSTQUOTE:
      ఆపు;
    స్థితి FTP_LIST_PREQUOTE:
      ftp_state(దత్తాంశం, ftpc, FTP_LIST_TYPE);
      ఫలితం = ftp_state_list(దత్తాంశం, ftpc, ftp);
      ఆపు;
    }
  }

  తిరిగి ఫలితం;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
స్థిర CURLcode ftp_epsv_disable(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ftp_conn *ftpc,
                                 నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(conn->బిట్లు.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(దత్తాంశం, "Failed EPSV attempt, exiting");
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  infof(దత్తాంశం, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->బిట్లు.ftp_use_epsv = FALSE;
  close_secondarysocket(దత్తాంశం, ftpc);
  దత్తాంశం->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "PASV");
  ఒకవేళ(!ఫలితం) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(దత్తాంశం, ftpc, FTP_PASV);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_control_addr_dup(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం **newhostp)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ(!Curl_conn_get_ip_info(దత్తాంశం, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  లేకపోతే {
    /* failed to get the remote_ip of the DATA connection */
    failf(దత్తాంశం, "unable to get peername of DATA connection");
    *newhostp = ఖాళీ;
    తిరిగి CURLE_FTP_CANT_GET_HOST;
  }
  తిరిగి *newhostp ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

స్థిర bool సరిపోలిక_pasv_6nums(స్థిరరాశి అక్షరం *p,
                             గుర్తులేని పూర్ణాంకం *array) /* 6 numbers */
{
  పూర్ణాంకం i;
  కొరకు(i = 0; i < 6; i++) {
    curl_off_t అంకె;
    ఒకవేళ(i) {
      ఒకవేళ(*p != ',')
        తిరిగి FALSE;
      p++;
    }
    ఒకవేళ(curlx_str_సంఖ్య(&p, &అంకె, 0xff))
      తిరిగి FALSE;
    array[i] = (గుర్తులేని పూర్ణాంకం)అంకె;
  }
  తిరిగి TRUE;
}

స్థిర CURLcode ftp_state_pasv_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    పూర్ణాంకం ftpcode)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం;
  స్థిరరాశి నిర్మాణం pingpong *pp = &ftpc->pp;
  అక్షరం *newhost = ఖాళీ;
  గుర్తులేని పొట్టి newport = 0;
  స్థిరరాశి అక్షరం *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ఒకవేళ((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    స్థిరరాశి అక్షరం *ptr = strchr(str, '(');
    ఒకవేళ(ptr) {
      అక్షరం sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ఒకవేళ(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        స్థిరరాశి అక్షరం *p = &ptr[3];
        curl_off_t అంకె;
        ఒకవేళ(curlx_str_సంఖ్య(&p, &అంకె, 0xffff) || (*p != sep)) {
          failf(దత్తాంశం, "Illegal port number in EPSV reply");
          తిరిగి CURLE_FTP_WEIRD_PASV_జవాబు;
        }
        newport = (గుర్తులేని పొట్టి)అంకె;
        ఫలితం = ftp_control_addr_dup(దత్తాంశం, &newhost);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
      లేకపోతే
        ptr = ఖాళీ;
    }
    ఒకవేళ(!ptr) {
      failf(దత్తాంశం, "Weirdly formatted EPSV reply");
      తిరిగి CURLE_FTP_WEIRD_PASV_జవాబు;
    }
  }
  లేకపోతే ఒకవేళ((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    గుర్తులేని పూర్ణాంకం ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    అయితే(*str) {
      ఒకవేళ(సరిపోలిక_pasv_6nums(str, ip))
        ఆపు;
      str++;
    }

    ఒకవేళ(!*str) {
      failf(దత్తాంశం, "Could not interpret the 227-response");
      తిరిగి CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ఒకవేళ(దత్తాంశం->సమితి.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(దత్తాంశం, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      ఫలితం = ftp_control_addr_dup(దత్తాంశం, &newhost);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ఒకవేళ(!newhost)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    newport = (గుర్తులేని పొట్టి)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  లేకపోతే ఒకవేళ(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    తిరిగి ftp_epsv_disable(దత్తాంశం, ftpc, conn);
  }
  లేకపోతే {
    failf(దత్తాంశం, "Bad PASV/EPSV response: %03d", ftpcode);
    తిరిగి CURLE_FTP_WEIRD_PASV_జవాబు;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  ఫలితం = Curl_peer_create(దత్తాంశం, conn->scheme, newhost, newport,
                            &conn->origin2);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ఒకవేళ(conn->ద్వారా_peer) {
    Curl_peer_unlink(&conn->ద్వారా_peer2);
    ఫలితం = Curl_peer_create(దత్తాంశం, conn->ద్వారా_peer->scheme,
                              conn->ద్వారా_peer->hostname, newport,
                              &conn->ద్వారా_peer2);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
  }

  ఫలితం = Curl_conn_అమరిక(దత్తాంశం, conn, SECONDARYSOCKET,
                           conn->బిట్లు.ftp_use_దత్తాంశం_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ఒకవేళ(ఫలితం) {
    ఒకవేళ((ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      ఫలితం = ftp_epsv_disable(దత్తాంశం, ftpc, conn);
    }
    వెళ్ళు error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(దత్తాంశం->సమితి.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(దత్తాంశం, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->పోర్టు);
  }
#అయితే_ముగింపు

  conn->బిట్లు.do_మరిన్ని = TRUE;
  ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  తిరిగి ఫలితం;
}

/* called repeatedly until done from multi.c */
స్థిర CURLcode ftp_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ftp_conn *ftpc,
                              bool *పూర్తి)
{
  CURLcode ఫలితం = Curl_pp_statemach(దత్తాంశం, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *పూర్తి = (ftpc->state == FTP_STOP);

  తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_do_మరిన్ని(నిర్మాణం Curl_easy *దత్తాంశం, domore *మరిన్ని)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  నిర్మాణం FTP *ftp = Curl_meta_get(దత్తాంశం, CURL_META_FTP_EASY);
  CURLcode ఫలితం = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ఒకవేళ(!ftpc || !ftp)
    తిరిగి CURLE_FAILED_INIT;

  *మరిన్ని = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ఒకవేళ(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(దత్తాంశం, SECONDARYSOCKET);
    ఫలితం = Curl_conn_connect(దత్తాంశం, SECONDARYSOCKET, FALSE, &connected);
    ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      తిరిగి ఫలితం;
    ఒకవేళ(ఫలితం || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(దత్తాంశం, SECONDARYSOCKET))) {
      ఒకవేళ(ఫలితం && !is_eptr && (ftpc->count1 == 0)) {
        *మరిన్ని = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        తిరిగి ftp_epsv_disable(దత్తాంశం, ftpc, conn);
      }
      తిరిగి ఫలితం;
    }
  }

  ఒకవేళ(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    ఫలితం = ftp_statemach(దత్తాంశం, ftpc, &complete);

    ఒకవేళ(complete)
      *మరిన్ని = DOMORE_పూర్తి;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ఒకవేళ(ఫలితం || !ftpc->wait_దత్తాంశం_conn)
      తిరిగి ఫలితం;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *మరిన్ని = DOMORE_INCOMPLETE;
  }

  ఒకవేళ(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ఒకవేళ(ftpc->wait_దత్తాంశం_conn) {
      bool serv_conned;

      ఫలితం = Curl_conn_connect(దత్తాంశం, SECONDARYSOCKET, FALSE, &serv_conned);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం; /* Failed to accept data connection */

      ఒకవేళ(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_దత్తాంశం_conn = FALSE;
        ఫలితం = ftp_initiate_transfer(దత్తాంశం, ftpc);

        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        *మరిన్ని = DOMORE_పూర్తి; /* this state is now complete when the server has
                                connected back to us */
      }
      లేకపోతే {
        ఫలితం = ftp_check_ctrl_on_దత్తాంశం_wait(దత్తాంశం, ftpc);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->state.upload) {
      ఫలితం = ftp_nb_type(దత్తాంశం, ftpc, ftp, (bool)దత్తాంశం->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఫలితం = ftp_statemach(దత్తాంశం, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *మరిన్ని = (!ftpc->wait_దత్తాంశం_conn && complete) ?
        DOMORE_పూర్తి : DOMORE_INCOMPLETE;
    }
    లేకపోతే {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      ఫలితం = Curl_range(దత్తాంశం);

      ఒకవేళ(ఫలితం == CURLE_OK && దత్తాంశం->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఒకవేళ((దత్తాంశం->state.list_only || !ftpc->file) &&
         !దత్తాంశం->సమితి.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ఒకవేళ(ftp->transfer == PPTRANSFER_BODY) {
          ఫలితం = ftp_nb_type(దత్తాంశం, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
        }
        /* otherwise fall through */
      }
      లేకపోతే {
        ఒకవేళ(దత్తాంశం->సమితి.prequote && !ftpc->file) {
          ఫలితం = ftp_nb_type(దత్తాంశం, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        లేకపోతే {
          ఫలితం = ftp_nb_type(దత్తాంశం, ftpc, ftp, (bool)దత్తాంశం->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }

      ఫలితం = ftp_statemach(దత్తాంశం, ftpc, &complete);
      ఒకవేళ(complete)
        *మరిన్ని = DOMORE_పూర్తి;
    }
    తిరిగి ఫలితం;
  }

  /* no data to transfer */
  Curl_xfer_అమరిక_nop(దత్తాంశం);

  ఒకవేళ(!ftpc->wait_దత్తాంశం_conn) {
    /* no waiting for the data connection so this is now complete */
    *మరిన్ని = DOMORE_పూర్తి;
    CURL_TRC_FTP(దత్తాంశం, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (పూర్ణాంకం)ఫలితం);
  }

  తిరిగి ఫలితం;
}

/* call this when the DO phase has completed */
స్థిర CURLcode ftp_dophase_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ftp_conn *ftpc,
                                 నిర్మాణం FTP *ftp,
                                 bool connected)
{
  ఒకవేళ(connected) {
    domore completed;
    CURLcode ఫలితం = ftp_do_మరిన్ని(దత్తాంశం, &completed);

    ఒకవేళ(ఫలితం) {
      close_secondarysocket(దత్తాంశం, ftpc);
      తిరిగి ఫలితం;
    }
  }

  ఒకవేళ(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_అమరిక_nop(దత్తాంశం);
  లేకపోతే ఒకవేళ(!connected)
    /* since we did not connect now, we want do_more to get called */
    దత్తాంశం->conn->బిట్లు.do_మరిన్ని = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  తిరిగి CURLE_OK;
}

స్థిర CURLcode ftp_state_పోర్టు_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    నిర్మాణం FTP *ftp,
                                    పూర్ణాంకం ftpcode)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode ఫలితం = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ఒకవేళ(ftpcode / 100 != 2) {
    /* the command failed */

    ఒకవేళ(EPRT == fcmd) {
      infof(దత్తాంశం, "disabling EPRT usage");
      conn->బిట్లు.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ఒకవేళ(fcmd == పూర్తి_2) {
      failf(దత్తాంశం, "Failed to do PORT");
      ఫలితం = CURLE_FTP_పోర్టు_FAILED;
    }
    లేకపోతే
      /* try next */
      ఫలితం = ftp_state_use_పోర్టు(దత్తాంశం, ftpc, fcmd);
  }
  లేకపోతే {
    infof(దత్తాంశం, "Connect data stream actively");
    ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* end of DO phase */
    ఫలితం = ftp_dophase_పూర్తి(దత్తాంశం, ftpc, ftp, FALSE);
  }

  తిరిగి ఫలితం;
}

/* return TRUE on error, FALSE on success */
స్థిర bool twodigit(స్థిరరాశి అక్షరం *p, పూర్ణాంకం *val)
{
  ఒకవేళ(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    తిరిగి TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  తిరిగి FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(స్థిరరాశి అక్షరం *p, పూర్ణాంకం *year, పూర్ణాంకం *month, పూర్ణాంకం *day,
                           పూర్ణాంకం *hour, పూర్ణాంకం *minute, పూర్ణాంకం *రెండవ);
UNITTEST bool ftp_213_date(స్థిరరాశి అక్షరం *p, పూర్ణాంకం *year, పూర్ణాంకం *month, పూర్ణాంకం *day,
                           పూర్ణాంకం *hour, పూర్ణాంకం *minute, పూర్ణాంకం *రెండవ)
{
  పూర్ణాంకం century;
  ఒకవేళ((అక్షరశ్రేణి_పొడవు(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], రెండవ))
    తిరిగి FALSE;

  *year += century * 100;
  ఒకవేళ((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*రెండవ > 60))
    తిరిగి FALSE;
  తిరిగి TRUE;
}

స్థిర CURLcode client_write_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                                    అక్షరం *buf, size_t blen)
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
  CURLcode ఫలితం;
  bool save = (bool)దత్తాంశం->సమితి.include_శీర్షిక;
  దత్తాంశం->సమితి.include_శీర్షిక = TRUE;
  ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, buf, blen);
  దత్తాంశం->సమితి.include_శీర్షిక = save;
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_mdtm_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    నిర్మాణం FTP *ftp,
                                    పూర్ణాంకం ftpcode)
{
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(ftpcode) {
  స్థితి 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    పూర్ణాంకం year, month, day, hour, minute, రెండవ;
    నిర్మాణం pingpong *pp = &ftpc->pp;
    స్థిరరాశి అక్షరం *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ఒకవేళ(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &రెండవ)) {
      /* we have a time, reformat it */
      అక్షరం timebuf[24];
      curl_msnprintf(timebuf, పరిమాణం(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, రెండవ);
      /* now, convert this into a time() value: */
      ఒకవేళ(!Curl_getdate_capped(timebuf, &దత్తాంశం->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ఒకవేళ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#సూచన GCC diagnostic నెట్టు
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#సూచన GCC diagnostic ignored "-Wtype-limits"
#అయితే_ముగింపు
    ఒకవేళ(దత్తాంశం->req.no_body && ftpc->file &&
       దత్తాంశం->సమితి.get_filetime && showtime) {
#ఒకవేళ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
      అక్షరం headerbuf[128];
      పూర్ణాంకం headerbuflen;
      time_t filetime = దత్తాంశం->info.filetime;
      నిర్మాణం tm బఫరు;
      స్థిరరాశి నిర్మాణం tm *tm = &బఫరు;

      ఫలితం = curlx_gmtime(filetime, &బఫరు);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, పరిమాణం(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                       tm->tm_mday,
                       Curl_month[tm->tm_mon],
                       tm->tm_year + 1900,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec);
      ఫలితం = client_write_శీర్షిక(దత్తాంశం, headerbuf, headerbuflen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    } /* end of a ridiculous amount of conditionals */
  }
    ఆపు;
  అప్రమేయ:
    infof(దత్తాంశం, "unsupported MDTM reply format");
    ఆపు;
  స్థితి 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(దత్తాంశం, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ఆపు;
  }

  ఒకవేళ(దత్తాంశం->సమితి.timecondition) {
    ఒకవేళ((దత్తాంశం->info.filetime > 0) && (దత్తాంశం->సమితి.timevalue > 0)) {
      ఎంపిక(దత్తాంశం->సమితి.timecondition) {
      స్థితి CURL_TIMECOND_IFMODSINCE:
      అప్రమేయ:
        ఒకవేళ(దత్తాంశం->info.filetime <= దత్తాంశం->సమితి.timevalue) {
          infof(దత్తాంశం, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          దత్తాంశం->info.timecond = TRUE;
          ftp_state(దత్తాంశం, ftpc, FTP_STOP);
          తిరిగి CURLE_OK;
        }
        ఆపు;
      స్థితి CURL_TIMECOND_IFUNMODSINCE:
        ఒకవేళ(దత్తాంశం->info.filetime > దత్తాంశం->సమితి.timevalue) {
          infof(దత్తాంశం, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          దత్తాంశం->info.timecond = TRUE;
          ftp_state(దత్తాంశం, ftpc, FTP_STOP);
          తిరిగి CURLE_OK;
        }
        ఆపు;
      } /* switch */
    }
    లేకపోతే {
      infof(దత్తాంశం, "Skipping time comparison");
    }
  }

  ఒకవేళ(!ఫలితం)
    ఫలితం = ftp_state_type(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_type_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    నిర్మాణం FTP *ftp,
                                    పూర్ణాంకం ftpcode,
                                    ftpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(దత్తాంశం, "Could not set desired mode");
    తిరిగి CURLE_FTP_COULDNT_సమితి_TYPE;
  }
  ఒకవేళ(ftpcode != 200)
    infof(దత్తాంశం, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ఒకవేళ(instate == FTP_TYPE)
    ఫలితం = ftp_state_కొలత(దత్తాంశం, ftpc, ftp);
  లేకపోతే ఒకవేళ(instate == FTP_LIST_TYPE)
    ఫలితం = ftp_state_list(దత్తాంశం, ftpc, ftp);
  లేకపోతే ఒకవేళ(instate == FTP_RETR_TYPE)
    ఫలితం = ftp_state_retr_prequote(దత్తాంశం, ftpc, ftp);
  లేకపోతే ఒకవేళ(instate == FTP_STOR_TYPE)
    ఫలితం = ftp_state_stor_prequote(దత్తాంశం, ftpc, ftp);
  లేకపోతే ఒకవేళ(instate == FTP_RETR_LIST_TYPE)
    ఫలితం = ftp_state_list_prequote(దత్తాంశం, ftpc, ftp);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_కొలత_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    నిర్మాణం FTP *ftp,
                                    పూర్ణాంకం ftpcode,
                                    ftpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_off_t filesize = -1;
  స్థిరరాశి అక్షరం *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ఒకవేళ(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    స్థిరరాశి అక్షరం *ప్రారంభం = &buf[4];
    స్థిరరాశి అక్షరం *fdigit = memchr(ప్రారంభం, '\r', len - 4);
    ఒకవేళ(fdigit) {
      fdigit--;
      ఒకవేళ(*fdigit == '\n')
        fdigit--;
      అయితే(ISDIGIT(fdigit[-1]) && (fdigit > ప్రారంభం))
        fdigit--;
    }
    లేకపోతే
      fdigit = ప్రారంభం;
    ఒకవేళ(curlx_str_సంఖ్య(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  లేకపోతే ఒకవేళ(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_కొలత) {
    failf(దత్తాంశం, "The file does not exist");
    తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;
  }

  ఒకవేళ(instate == FTP_కొలత) {
    ఒకవేళ(filesize != -1) {
      అక్షరం clbuf[128];
      పూర్ణాంకం clbuflen = curl_msnprintf(clbuf, పరిమాణం(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      ఫలితం = client_write_శీర్షిక(దత్తాంశం, clbuf, clbuflen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, filesize);
    ఫలితం = ftp_state_rest(దత్తాంశం, ftpc, ftp);
  }
  లేకపోతే ఒకవేళ(instate == FTP_RETR_కొలత) {
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, filesize);
    ఫలితం = ftp_state_retr(దత్తాంశం, ftpc, ftp, filesize);
  }
  లేకపోతే ఒకవేళ(instate == FTP_STOR_కొలత) {
    దత్తాంశం->state.resume_from = filesize;
    ఫలితం = ftp_state_ul_అమరిక(దత్తాంశం, ftpc, ftp, TRUE);
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_rest_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    నిర్మాణం FTP *ftp,
                                    పూర్ణాంకం ftpcode,
                                    ftpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(instate) {
  స్థితి FTP_REST:
  అప్రమేయ:
    ఒకవేళ(ftpcode == 350) {
      అక్షరం బఫరు[24] = { "Accept-ranges: bytes\r\n" };
      ఫలితం = client_write_శీర్షిక(దత్తాంశం, బఫరు, అక్షరశ్రేణి_పొడవు(బఫరు));
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    ఫలితం = ftp_state_prepare_transfer(దత్తాంశం, ftpc, ftp);
    ఆపు;

  స్థితి FTP_RETR_REST:
    ఒకవేళ(ftpcode != 350) {
      failf(దత్తాంశం, "Could not use REST");
      ఫలితం = CURLE_FTP_COULDNT_USE_REST;
    }
    లేకపోతే {
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "RETR %s", ftpc->file);
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_RETR);
    }
    ఆపు;
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_state_stor_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    పూర్ణాంకం ftpcode)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ftpcode >= 400) {
    failf(దత్తాంశం, "Failed FTP upload: %0d", ftpcode);
    ftp_state(దత్తాంశం, ftpc, FTP_STOP);
    తిరిగి CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ఒకవేళ(దత్తాంశం->సమితి.ftp_use_పోర్టు) {
    bool connected;

    ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* no longer in STOR state */

    ఫలితం = Curl_conn_connect(దత్తాంశం, SECONDARYSOCKET, FALSE, &connected);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ(!connected) {
      infof(దత్తాంశం, "Data conn was not available immediately");
      ftpc->wait_దత్తాంశం_conn = TRUE;
      తిరిగి ftp_check_ctrl_on_దత్తాంశం_wait(దత్తాంశం, ftpc);
    }
    ftpc->wait_దత్తాంశం_conn = FALSE;
  }
  తిరిగి ftp_initiate_transfer(దత్తాంశం, ftpc);
}

/* for LIST and RETR responses */
స్థిర CURLcode ftp_state_get_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc,
                                   నిర్మాణం FTP *ftp,
                                   పూర్ణాంకం ftpcode,
                                   ftpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((ftpcode == 150) || (ftpcode == 125)) {

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

    దత్తాంశం->req.కొలత = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    ఒకవేళ((instate != FTP_LIST) &&
       !దత్తాంశం->state.prefer_ascii &&
       !దత్తాంశం->సమితి.ignorecl &&
       (ftp->downloadsize < 1)) {
      /*
       * It seems directory listings either do not show the size or often uses
       * size 0 anyway. ASCII transfers may cause that the transferred amount
       * of data is not the same as this line tells, why using this number in
       * those cases only confuses us.
       *
       * Example D above makes this parsing a little tricky */
      size_t len = curlx_dyn_len(&ftpc->pp.recvbuf);
      ఒకవేళ(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        కొరకు(i = 0; i < len - 7; i++) {
          curl_off_t what;
          స్థిరరాశి అక్షరం *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          స్థిరరాశి అక్షరం *c = &buf[i];
          ఒకవేళ(!curlx_str_సంఖ్య(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !అక్షరశ్రేణి_పోలిక(c, "bytes", 5)) {
            దత్తాంశం->req.కొలత = what;
            ఆపు;
          }
        }
      }
    }
    లేకపోతే ఒకవేళ(ftp->downloadsize > -1)
      దత్తాంశం->req.కొలత = ftp->downloadsize;

    ఒకవేళ(దత్తాంశం->req.కొలత > దత్తాంశం->req.maxdownload && దత్తాంశం->req.maxdownload > 0)
      దత్తాంశం->req.కొలత = దత్తాంశం->req.maxdownload;
    లేకపోతే ఒకవేళ((instate != FTP_LIST) && దత్తాంశం->state.prefer_ascii)
      దత్తాంశం->req.కొలత = -1; /* for servers that understate ASCII mode file
కొలత*/

    infof(దత్తాంశం, "Maxdownload = %" FMT_OFF_T, దత్తాంశం->req.maxdownload);

    ఒకవేళ(instate != FTP_LIST)
      infof(దత్తాంశం, "Getting file with size: %" FMT_OFF_T, దత్తాంశం->req.కొలత);

    ఒకవేళ(దత్తాంశం->సమితి.ftp_use_పోర్టు) {
      bool connected;

      ఫలితం = Curl_conn_connect(దత్తాంశం, SECONDARYSOCKET, FALSE, &connected);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఒకవేళ(!connected) {
        infof(దత్తాంశం, "Data conn was not available immediately");
        ftp_state(దత్తాంశం, ftpc, FTP_STOP);
        ftpc->wait_దత్తాంశం_conn = TRUE;
        తిరిగి ftp_check_ctrl_on_దత్తాంశం_wait(దత్తాంశం, ftpc);
      }
      ftpc->wait_దత్తాంశం_conn = FALSE;
    }
    తిరిగి ftp_initiate_transfer(దత్తాంశం, ftpc);
  }
  లేకపోతే {
    ఒకవేళ((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* this phase is over */
    }
    లేకపోతే {
      failf(దత్తాంశం, "RETR response: %03d", ftpcode);
      తిరిగి instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_దొరికింది :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  తిరిగి ఫలితం;
}

/* after USER, PASS and ACCT */
స్థిర CURLcode ftp_state_loggedin(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_conn *ftpc)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET)) {
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
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PBSZ %d", 0);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_PBSZ);
  }
  లేకపోతే {
    ఫలితం = ftp_state_pwd(దత్తాంశం, ftpc);
  }
  తిరిగి ఫలితం;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
స్థిర bool ftp_has_ctrl(స్థిరరాశి అక్షరం *string)
{
  స్థిరరాశి గుర్తులేని అక్షరం *s = (స్థిరరాశి గుర్తులేని అక్షరం *)string;
  అయితే(*s) {
    ఒకవేళ(*s < 0x20)
      తిరిగి TRUE;
    s++;
  }
  తిరిగి FALSE;
}

/* for USER and PASS responses */
స్థిర CURLcode ftp_state_user_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    పూర్ణాంకం ftpcode)
{
  CURLcode ఫలితం = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ఒకవేళ((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(దత్తాంశం->conn->creds));
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_PASS);
  }
  లేకపోతే ఒకవేళ(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    ఫలితం = ftp_state_loggedin(దత్తాంశం, ftpc);
  }
  లేకపోతే ఒకవేళ(ftpcode == 332) {
    స్థిరరాశి అక్షరం *account = CURL_EASY_STR(దత్తాంశం, STRING_FTP_ACCOUNT);
    ఒకవేళ(!account) {
      failf(దత్తాంశం, "ACCT requested but none available");
      ఫలితం = CURLE_LOGIN_DENIED;
    }
    లేకపోతే ఒకవేళ(ftp_has_ctrl(account)) {
      failf(దత్తాంశం, "Control byte in FTP account");
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    }
    లేకపోతే {
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "ACCT %s", account);
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_ACCT);
    }
  }
  లేకపోతే {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    స్థిరరాశి అక్షరం *alt = CURL_EASY_STR(దత్తాంశం, STRING_FTP_ALTERNATIVE_గమ్యం_USER);
    ఒకవేళ(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ఒకవేళ(ftp_has_ctrl(alt)) {
        failf(దత్తాంశం, "Control byte in FTP alternative-to-user command");
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      }
      లేకపోతే {
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", alt);
        ఒకవేళ(!ఫలితం) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(దత్తాంశం, ftpc, FTP_USER);
        }
      }
    }
    లేకపోతే {
      failf(దత్తాంశం, "Access denied: %03d", ftpcode);
      ఫలితం = CURLE_LOGIN_DENIED;
    }
  }
  తిరిగి ఫలితం;
}

/* for ACCT response */
స్థిర CURLcode ftp_state_acct_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc,
                                    పూర్ణాంకం ftpcode)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(ftpcode != 230) {
    failf(దత్తాంశం, "ACCT rejected by server: %03d", ftpcode);
    ఫలితం = CURLE_FTP_WEIRD_PASS_జవాబు; /* FIX */
  }
  లేకపోతే
    ఫలితం = ftp_state_loggedin(దత్తాంశం, ftpc);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_pwd_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ftp_conn *ftpc,
                             పూర్ణాంకం ftpcode)
{
  నిర్మాణం pingpong *pp = &ftpc->pp;
  CURLcode ఫలితం;

  ఒకవేళ(ftpcode == 257) {
    స్థిరరాశి అక్షరం *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool నమోదు_extracted = FALSE;
    నిర్మాణం dynbuf బహిర్గతం;
    curlx_dyn_init(&బహిర్గతం, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    అయితే(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ఒకవేళ('\"' == *ptr) {
      /* it started good */
      కొరకు(ptr++; *ptr; ptr++) {
        ఒకవేళ('\"' == *ptr) {
          ఒకవేళ('\"' == ptr[1]) {
            /* "quote-doubling" */
            ఫలితం = curlx_dyn_addn(&బహిర్గతం, &ptr[1], 1);
            ptr++;
          }
          లేకపోతే {
            /* end of path */
            ఒకవేళ(curlx_dyn_len(&బహిర్గతం))
              నమోదు_extracted = TRUE;
            ఆపు; /* get out of this loop */
          }
        }
        లేకపోతే {
          ఒకవేళ(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&బహిర్గతం);
            తిరిగి CURLE_WEIRD_SERVER_జవాబు;
          }
          ఫలితం = curlx_dyn_addn(&బహిర్గతం, ptr, 1);
        }
        ఒకవేళ(ఫలితం) {
          curlx_dyn_free(&బహిర్గతం);
          తిరిగి ఫలితం;
        }
      }
    }
    ఒకవేళ(నమోదు_extracted) {
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
      అక్షరం *dir = curlx_dyn_ptr(&బహిర్గతం);

      ఒకవేళ(!ftpc->server_os && dir[0] != '/') {
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "SYST");
        ఒకవేళ(ఫలితం) {
          curlx_dyn_free(&బహిర్గతం);
          తిరిగి ఫలితం;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(దత్తాంశం, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(దత్తాంశం->state.most_recent_ftp_entrypath);
      దత్తాంశం->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ఒకవేళ(!దత్తాంశం->state.most_recent_ftp_entrypath)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      ఒకవేళ(!ftpc->server_os && dir[0] != '/') {
        ftp_state(దత్తాంశం, ftpc, FTP_SYST);
        తిరిగి CURLE_OK;
      }
    }
    లేకపోతే {
      /* could not get the path */
      curlx_dyn_free(&బహిర్గతం);
      infof(దత్తాంశం, "Failed to figure out path");
    }
  }
  ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(దత్తాంశం, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి ftpauth[] = { "SSL", "TLS" };

స్థిర CURLcode ftp_wait_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn,
                              నిర్మాణం ftp_conn *ftpc,
                              పూర్ణాంకం ftpcode)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ఒకవేళ(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      తిరిగి ftp_state_user_resp(దత్తాంశం, ftpc, ftpcode);
  }
  లేకపోతే ఒకవేళ(ftpcode != 220) {
    failf(దత్తాంశం, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  ఒకవేళ(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ఎంపిక((దీర్ఘ)దత్తాంశం->సమితి.ftpsslauth) {
    స్థితి CURLFTPAUTH_DEFAULT:
    స్థితి CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ఆపు;
    స్థితి CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ఆపు;
    అప్రమేయ:
      failf(దత్తాంశం, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (పూర్ణాంకం)దత్తాంశం->సమితి.ftpsslauth);
      తిరిగి CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_AUTH);
  }
  లేకపోతే
    ఫలితం = ftp_state_user(దత్తాంశం, ftpc, conn);
  తిరిగి ఫలితం;
}

/* an implicit TLS scheme implies TLS on both connections */
స్థిర గుర్తులేని అక్షరం ftp_use_ssl(నిర్మాణం connectdata *conn,
                                 నిర్మాణం ftp_conn *ftpc)
{
  ఒకవేళ(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    తిరిగి CURLUSESSL_ALL;
  తిరిగి ftpc->use_ssl;
}

స్థిర CURLcode ftp_pp_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం;
  పూర్ణాంకం ftpcode;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  నిర్మాణం FTP *ftp = Curl_meta_get(దత్తాంశం, CURL_META_FTP_EASY);
  నిర్మాణం pingpong *pp;
  size_t nread = 0;

  ఒకవేళ(!ftpc || !ftp)
    తిరిగి CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ఒకవేళ(pp->sendleft)
    తిరిగి Curl_pp_flushsend(దత్తాంశం, pp);

  ఫలితం = ftp_readresp(దత్తాంశం, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ఒకవేళ(ఫలితం || !ftpcode)
    తిరిగి ఫలితం;

  /* we have now received a full FTP server response */
  ఎంపిక(ftpc->state) {
  స్థితి FTP_WAIT220:
    ఫలితం = ftp_wait_resp(దత్తాంశం, conn, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ఒకవేళ(pp->అధికప్రవాహం)
      తిరిగి CURLE_WEIRD_SERVER_జవాబు; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ఒకవేళ((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool పూర్తి;
      ఒకవేళ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        ఫలితం = Curl_ssl_cfilter_కూడు(
          దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ఒకవేళ(ఫలితం) {
          /* we failed and bail out */
          తిరిగి CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, TRUE, &పూర్తి);
      ఒకవేళ(!ఫలితం) {
        conn->బిట్లు.ftp_use_దత్తాంశం_ssl = FALSE; /* clear-text data */
        ఫలితం = ftp_state_user(దత్తాంశం, ftpc, conn);
      }
    }
    లేకపోతే ఒకవేళ(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    లేకపోతే {
      ఒకవేళ(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        ఫలితం = CURLE_USE_SSL_FAILED;
      లేకపోతే
        /* ignore the failure and continue */
        ఫలితం = ftp_state_user(దత్తాంశం, ftpc, conn);
    }
    ఆపు;

  స్థితి FTP_USER:
  స్థితి FTP_PASS:
    ఫలితం = ftp_state_user_resp(దత్తాంశం, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_ACCT:
    ఫలితం = ftp_state_acct_resp(దత్తాంశం, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_PBSZ:
    ఫలితం =
      Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ఒకవేళ(!ఫలితం)
      ftp_state(దత్తాంశం, ftpc, FTP_PROT);
    ఆపు;

  స్థితి FTP_PROT:
    ఒకవేళ(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->బిట్లు.ftp_use_దత్తాంశం_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    లేకపోతే ఒకవేళ(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      తిరిగి CURLE_USE_SSL_FAILED;

    ఒకవేళ(దత్తాంశం->సమితి.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "CCC");
      ఒకవేళ(!ఫలితం)
        ftp_state(దత్తాంశం, ftpc, FTP_CCC);
    }
    లేకపోతే
      ఫలితం = ftp_state_pwd(దత్తాంశం, ftpc);
    ఆపు;

  స్థితి FTP_CCC:
    ఒకవేళ(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      ఫలితం = Curl_ssl_cfilter_remove(దత్తాంశం, FIRSTSOCKET,
                                       (దత్తాంశం->సమితి.ftp_ccc ==
                                        (గుర్తులేని అక్షరం)CURLFTPSSL_CCC_చురుకైన));
      ఒకవేళ(ఫలితం)
        failf(దత్తాంశం, "Failed to clear the command channel (CCC)");
    }
    ఒకవేళ(!ఫలితం)
      /* Then continue as normal */
      ఫలితం = ftp_state_pwd(దత్తాంశం, ftpc);
    ఆపు;

  స్థితి FTP_PWD:
    ఫలితం = ftp_pwd_resp(దత్తాంశం, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_SYST:
    ఒకవేళ(ftpcode == 215) {
      స్థిరరాశి అక్షరం *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      స్థిరరాశి అక్షరం *ప్రారంభం;
      అక్షరం *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      అయితే(*ptr == ' ')
        ptr++;
      కొరకు(ప్రారంభం = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(ప్రారంభం, ptr - ప్రారంభం);
      ఒకవేళ(!os)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      /* Check for special servers here. */
      ఒకవేళ(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ఒకవేళ(ఫలితం) {
          curlx_free(os);
          తిరిగి ఫలితం;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(దత్తాంశం, ftpc, FTP_NAMEFMT);
        ఆపు;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    లేకపోతే {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(దత్తాంశం, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ఆపు;

  స్థితి FTP_NAMEFMT:
    ఒకవేళ(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(దత్తాంశం, ftpc);
      ఆపు;
    }

    ftp_state(దత్తాంశం, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(దత్తాంశం, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ఆపు;

  స్థితి FTP_QUOTE:
  స్థితి FTP_POSTQUOTE:
  స్థితి FTP_RETR_PREQUOTE:
  స్థితి FTP_STOR_PREQUOTE:
  స్థితి FTP_LIST_PREQUOTE:
    ఒకవేళ((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(దత్తాంశం, "QUOT command failed with %03d", ftpcode);
      ఫలితం = CURLE_QUOTE_ERROR;
    }
    లేకపోతే
      ఫలితం = ftp_state_quote(దత్తాంశం, ftpc, ftp, FALSE, ftpc->state);
    ఆపు;

  స్థితి FTP_CWD:
    ఒకవేళ(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ఒకవేళ(దత్తాంశం->సమితి.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (దత్తాంశం->సమితి.ftp_create_missing_dirs == 2) ? 1 : 0;

        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        ఒకవేళ(!ఫలితం)
          ftp_state(దత్తాంశం, ftpc, FTP_MKD);
      }
      లేకపోతే {
        /* return failure */
        failf(దత్తాంశం, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    లేకపోతే {
      /* success */
      ftpc->count2 = 0;
      ఒకవేళ(ftpc->cwdcount >= ftpc->dirdepth)
        ఫలితం = ftp_state_mdtm(దత్తాంశం, ftpc, ftp);
      లేకపోతే {
        ftpc->cwdcount++;
        /* send next CWD */
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ఆపు;

  స్థితి FTP_MKD:
    ఒకవేళ((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(దత్తాంశం, "Failed to MKD dir: %03d", ftpcode);
      ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
    }
    లేకపోతే {
      ftp_state(దత్తాంశం, ftpc, FTP_CWD);
      /* send CWD */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ఆపు;

  స్థితి FTP_MDTM:
    ఫలితం = ftp_state_mdtm_resp(దత్తాంశం, ftpc, ftp, ftpcode);
    ఆపు;

  స్థితి FTP_TYPE:
  స్థితి FTP_LIST_TYPE:
  స్థితి FTP_RETR_TYPE:
  స్థితి FTP_STOR_TYPE:
  స్థితి FTP_RETR_LIST_TYPE:
    ఫలితం = ftp_state_type_resp(దత్తాంశం, ftpc, ftp, ftpcode, ftpc->state);
    ఆపు;

  స్థితి FTP_కొలత:
  స్థితి FTP_RETR_కొలత:
  స్థితి FTP_STOR_కొలత:
    ఫలితం = ftp_state_కొలత_resp(దత్తాంశం, ftpc, ftp, ftpcode, ftpc->state);
    ఆపు;

  స్థితి FTP_REST:
  స్థితి FTP_RETR_REST:
    ఫలితం = ftp_state_rest_resp(దత్తాంశం, ftpc, ftp, ftpcode, ftpc->state);
    ఆపు;

  స్థితి FTP_PRET:
    ఒకవేళ(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(దత్తాంశం, "PRET command not accepted: %03d", ftpcode);
      తిరిగి CURLE_FTP_PRET_FAILED;
    }
    ఫలితం = ftp_state_use_pasv(దత్తాంశం, ftpc, conn);
    ఆపు;

  స్థితి FTP_PASV:
    ఫలితం = ftp_state_pasv_resp(దత్తాంశం, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_పోర్టు:
    ఫలితం = ftp_state_పోర్టు_resp(దత్తాంశం, ftpc, ftp, ftpcode);
    ఆపు;

  స్థితి FTP_LIST:
  స్థితి FTP_RETR:
    ఫలితం = ftp_state_get_resp(దత్తాంశం, ftpc, ftp, ftpcode, ftpc->state);
    ఆపు;

  స్థితి FTP_STOR:
    ఫలితం = ftp_state_stor_resp(దత్తాంశం, ftpc, ftpcode);
    ఆపు;

  స్థితి FTP_QUIT:
  అప్రమేయ:
    /* internal error */
    ftp_state(దత్తాంశం, ftpc, FTP_STOP);
    ఆపు;
  }

  తిరిగి ఫలితం;
}

/* called repeatedly until done from multi.c */
స్థిర CURLcode ftp_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  తిరిగి ftpc ? ftp_statemach(దత్తాంశం, ftpc, పూర్తి) : CURLE_FAILED_INIT;
}

స్థిర CURLcode ftp_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_conn *ftpc)
{
  నిర్మాణం pingpong *pp = &ftpc->pp;
  CURLcode ఫలితం = CURLE_OK;

  అయితే(ftpc->state != FTP_STOP) {
    ఒకవేళ(ftpc->shutdown)
      CURL_TRC_FTP(దత్తాంశం, "in shutdown, waiting for server response");
    ఫలితం = Curl_pp_statemach(దత్తాంశం, pp, TRUE, TRUE /* disconnecting */);
    ఒకవేళ(ఫలితం)
      ఆపు;
  }

  తిరిగి ఫలితం;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
స్థిర CURLcode ftp_connect(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool *పూర్తి) /* see description above */
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  నిర్మాణం pingpong *pp;

  *పూర్తి = FALSE; /* default to not done yet */
  ఒకవేళ(!ftpc)
    తిరిగి CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_అమరిక(pp, ftp_pp_statemachine, ftp_endofresp);

  ఒకవేళ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, TRUE, పూర్తి);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  Curl_pp_init(pp, Curl_pgrs_now(దత్తాంశం)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(దత్తాంశం, ftpc, FTP_WAIT220);

  ఫలితం = ftp_statemach(దత్తాంశం, ftpc, పూర్తి);

  తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_sendquote(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ftp_conn *ftpc,
                              నిర్మాణం curl_slist *quote)
{
  నిర్మాణం curl_slist *అంశం;
  నిర్మాణం pingpong *pp = &ftpc->pp;

  అంశం = quote;
  అయితే(అంశం) {
    ఒకవేళ(అంశం->దత్తాంశం) {
      size_t nread;
      స్థిరరాశి అక్షరం *cmd = అంశం->దత్తాంశం;
      bool acceptfail = FALSE;
      CURLcode ఫలితం;
      పూర్ణాంకం ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ఒకవేళ(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", cmd);
      ఒకవేళ(!ఫలితం) {
        pp->response = *Curl_pgrs_now(దత్తాంశం); /* timeout relative now */
        ఫలితం = getftpresponse(దత్తాంశం, &nread, &ftpcode);
      }
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఒకవేళ(!acceptfail && (ftpcode >= 400)) {
        failf(దత్తాంశం, "QUOT string not accepted: %s", cmd);
        తిరిగి CURLE_QUOTE_ERROR;
      }
    }

    అంశం = అంశం->తదుపరి;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode ftp_పూర్తి_స్థితి(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ftp_conn *ftpc, CURLcode స్థితి_2,
                                bool premature)
{
  ఎంపిక(స్థితి_2) {
  స్థితి CURLE_BAD_DOWNLOAD_RESUME:
  స్థితి CURLE_FTP_WEIRD_PASV_జవాబు:
  స్థితి CURLE_FTP_పోర్టు_FAILED:
  స్థితి CURLE_FTP_ACCEPT_FAILED:
  స్థితి CURLE_FTP_ACCEPT_TIMEOUT:
  స్థితి CURLE_FTP_COULDNT_సమితి_TYPE:
  స్థితి CURLE_FTP_COULDNT_RETR_FILE:
  స్థితి CURLE_PARTIAL_FILE:
  స్థితి CURLE_UPLOAD_FAILED:
  స్థితి CURLE_REMOTE_ACCESS_DENIED:
  స్థితి CURLE_FILESIZE_EXCEEDED:
  స్థితి CURLE_REMOTE_FILE_NOT_దొరికింది:
  స్థితి CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  స్థితి CURLE_OK: /* does not affect the control connection's status */
    ఒకవేళ(!premature)
      ఆపు;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  అప్రమేయ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(దత్తాంశం, "FTP ended with bad error code");
    connclose(దత్తాంశం->conn);
    తిరిగి స్థితి_2;      /* use the already set error code */
  }
  తిరిగి CURLE_OK;
}

స్థిర శూన్య ftp_పూర్తి_wildcard(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ftp_conn *ftpc)
{
  ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
    ఒకవేళ(దత్తాంశం->సమితి.chunk_చివర && ftpc->file) {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_chunk_చివర);
      దత్తాంశం->సమితి.chunk_చివర(దత్తాంశం->సమితి.wildcardptr);
      CURL_CBAPI_చివర(&guard);
      freedirs(ftpc);
    }
    ftpc->తెలిసిన_filesize = -1;
  }
}

స్థిర శూన్య ftp_పూర్తి_మార్గం(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ftp_conn *ftpc,
                          CURLcode ఫలితం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(ఫలితం) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  లేకపోతే { /* remember working directory for connection reuse */
    స్థిరరాశి అక్షరం *raw_మార్గం = ftpc->rawpath;
    ఒకవేళ(raw_మార్గం) {
      ఒకవేళ((దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD) && (raw_మార్గం[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      లేకపోతే {
        size_t మార్గం_Len = అక్షరశ్రేణి_పొడవు(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ఒకవేళ(!ftpc->cwdfail) {
          ఒకవేళ(దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD)
            మార్గం_Len = 0; /* relative path => working directory is FTP home */
          లేకపోతే
            /* file is URL-decoded */
            మార్గం_Len -= ftpc->file ? అక్షరశ్రేణి_పొడవు(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_మార్గం, మార్గం_Len);
        }
        లేకపోతే
          ftpc->prevpath = ఖాళీ; /* no path */
      }
    }
    ఒకవేళ(ftpc->prevpath)
      infof(దత్తాంశం, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

స్థిర CURLcode ftp_పూర్తి_secondary_socket(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ftp_conn *ftpc,
                                          CURLcode ఫలితం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(Curl_conn_is_అమరిక(conn, SECONDARYSOCKET)) {
    ఒకవేళ(!ఫలితం && ftpc->dont_check && దత్తాంశం->req.maxdownload > 0) {
      /* partial download completed */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "ABOR");
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "Failure sending ABOR command: %s",
              curl_easy_strerror(ఫలితం));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(దత్తాంశం, ftpc);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_పూర్తి_control_జవాబు(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ftp_conn *ftpc,
                                       నిర్మాణం FTP *ftp, CURLcode ఫలితం,
                                       bool premature)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  size_t nread;
  పూర్ణాంకం ftpcode;

  ఒకవేళ(!ఫలితం && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(దత్తాంశం); /* timeout relative now */
    ఫలితం = getftpresponse(దత్తాంశం, &nread, &ftpcode);

    ఒకవేళ(!nread && (ఫలితం == CURLE_OPERATION_TIMEDOUT)) {
      failf(దత్తాంశం, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ(ftpc->dont_check && దత్తాంశం->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(దత్తాంశం, "partial download completed, closing connection");
      connclose(conn);
      తిరిగి ఫలితం;
    }

    ఒకవేళ(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ఎంపిక(ftpcode) {
      స్థితి 226:
      స్థితి 250:
        ఆపు;
      స్థితి 552:
        failf(దత్తాంశం, "Exceeded storage allocation");
        ఫలితం = CURLE_REMOTE_DISK_నిండిన;
        ఆపు;
      అప్రమేయ:
        failf(దత్తాంశం, "server did not report OK, got %d", ftpcode);
        ఫలితం = CURLE_PARTIAL_FILE;
        ఆపు;
      }
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ftp_పూర్తి_check_partial(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ftp_conn *ftpc,
                                       నిర్మాణం FTP *ftp, CURLcode ఫలితం,
                                       bool premature)
{
  ఒకవేళ(ఫలితం || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  లేకపోతే ఒకవేళ(దత్తాంశం->state.upload) {
    ఒకవేళ((ftp->transfer == PPTRANSFER_BODY) &&
       (దత్తాంశం->state.infilesize != -1) && /* upload with known size */
       ((!దత్తాంశం->సమితి.crlf && !దత్తాంశం->state.prefer_ascii && /* no conversion */
         (దత్తాంశం->state.infilesize != దత్తాంశం->req.writebytecount)) ||
        ((దత్తాంశం->సమితి.crlf || దత్తాంశం->state.prefer_ascii) && /* maybe CRLF conv */
         (దత్తాంశం->state.infilesize > దత్తాంశం->req.writebytecount))
       )) {
      failf(దత్తాంశం, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            దత్తాంశం->req.writebytecount, దత్తాంశం->state.infilesize);
      ఫలితం = CURLE_PARTIAL_FILE;
    }
  }
  లేకపోతే {
    ఒకవేళ((దత్తాంశం->req.కొలత != -1) &&
       (దత్తాంశం->req.కొలత != దత్తాంశం->req.bytecount) &&
       (దత్తాంశం->req.maxdownload != దత్తాంశం->req.bytecount)) {
      failf(దత్తాంశం, "Received only partial file: %" FMT_OFF_T " bytes",
            దత్తాంశం->req.bytecount);
      ఫలితం = CURLE_PARTIAL_FILE;
    }
    లేకపోతే ఒకవేళ(!ftpc->dont_check &&
            !దత్తాంశం->req.bytecount &&
            (దత్తాంశం->req.కొలత > 0)) {
      failf(దత్తాంశం, "No data was received");
      ఫలితం = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                         bool premature)
{
  నిర్మాణం FTP *ftp = Curl_meta_get(దత్తాంశం, CURL_META_FTP_EASY);
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  CURLcode ఫలితం;

  ఒకవేళ(!ftp || !ftpc)
    తిరిగి CURLE_OK;

  ఫలితం = ftp_పూర్తి_స్థితి(దత్తాంశం, ftpc, స్థితి_2, premature);

  ftp_పూర్తి_wildcard(దత్తాంశం, ftpc);
  ftp_పూర్తి_మార్గం(దత్తాంశం, ftpc, ఫలితం);
  ఫలితం = ftp_పూర్తి_secondary_socket(దత్తాంశం, ftpc, ఫలితం);
  ఫలితం = ftp_పూర్తి_control_జవాబు(దత్తాంశం, ftpc, ftp, ఫలితం, premature);
  ఫలితం = ftp_పూర్తి_check_partial(దత్తాంశం, ftpc, ftp, ఫలితం, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  ఒకవేళ(!స్థితి_2 && !ఫలితం && !premature && దత్తాంశం->సమితి.postquote)
    ఫలితం = ftp_sendquote(దత్తాంశం, ftpc, దత్తాంశం->సమితి.postquote);
  CURL_TRC_FTP(దత్తాంశం, "[%s] done, result=%d", FTP_CSTATE(ftpc), (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
స్థిర CURLcode ftp_nb_type(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ftp_conn *ftpc,
                            నిర్మాణం FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode ఫలితం;
  అక్షరం want = (అక్షరం)(ascii ? 'A' : 'I');

  ఒకవేళ(ftpc->transfertype == want) {
    ftp_state(దత్తాంశం, ftpc, newstate);
    తిరిగి ftp_state_type_resp(దత్తాంశం, ftpc, ftp, 200, newstate);
  }

  ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "TYPE %c", want);
  ఒకవేళ(!ఫలితం) {
    ftp_state(దత్తాంశం, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
స్థిర CURLcode ftp_perform(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం ftp_conn *ftpc,
  నిర్మాణం FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_పూర్తి)
{
  /* this is FTP and no proxy */
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_FTP(దత్తాంశం, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ఒకవేళ(దత్తాంశం->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_పూర్తి = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  ఫలితం = ftp_state_quote(దత్తాంశం, ftpc, ftp, TRUE, FTP_QUOTE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* run the state-machine */
  ఫలితం = ftp_statemach(దత్తాంశం, ftpc, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, SECONDARYSOCKET);

  ఒకవేళ(*connected)
    infof(దత్తాంశం, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  లేకపోతే
    CURL_TRC_FTP(దత్తాంశం, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ఒకవేళ(*dophase_పూర్తి)
    CURL_TRC_FTP(దత్తాంశం, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  తిరిగి ఫలితం;
}

స్థిర శూన్య wc_దత్తాంశం_dtor(శూన్య *ptr)
{
  నిర్మాణం ftp_wc *ftpwc = ptr;
  ఒకవేళ(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_దత్తాంశం_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

స్థిర CURLcode init_wc_దత్తాంశం(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ftp_conn *ftpc,
                             నిర్మాణం FTP *ftp)
{
  అక్షరం *చివరి_slash;
  అక్షరం *మార్గం = ftp->మార్గం;
  నిర్మాణం Wildcard_దత్తాంశం *wildcard = దత్తాంశం->wildcard;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ftp_wc *ftpwc = ఖాళీ;

  చివరి_slash = strrchr(ftp->మార్గం, '/');
  ఒకవేళ(చివరి_slash) {
    చివరి_slash++;
    ఒకవేళ(చివరి_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      తిరిగి ftp_parse_url_మార్గం(దత్తాంశం, ftpc, ftp);
    }
    wildcard->నమూనా = curlx_strdup(చివరి_slash);
    ఒకవేళ(!wildcard->నమూనా)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    చివరి_slash[0] = '\0'; /* cut file from path */
  }
  లేకపోతే { /* there is only 'wildcard pattern' or nothing */
    ఒకవేళ(మార్గం[0]) {
      wildcard->నమూనా = curlx_strdup(మార్గం);
      ఒకవేళ(!wildcard->నమూనా)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      మార్గం[0] = '\0';
    }
    లేకపోతే { /* only list */
      wildcard->state = CURLWC_CLEAN;
      తిరిగి ftp_parse_url_మార్గం(దత్తాంశం, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, పరిమాణం(నిర్మాణం ftp_wc));
  ఒకవేళ(!ftpwc) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_దత్తాంశం_alloc();
  ఒకవేళ(!ftpwc->parser) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_దత్తాంశం_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ఒకవేళ(దత్తాంశం->సమితి.ftp_filemethod == FTPFILE_NOCWD)
    దత్తాంశం->సమితి.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  ఫలితం = ftp_parse_url_మార్గం(దత్తాంశం, ftpc, ftp);
  ఒకవేళ(ఫలితం) {
    వెళ్ళు fail;
  }

  wildcard->మార్గం = curlx_strdup(ftp->మార్గం);
  ఒకవేళ(!wildcard->మార్గం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  infof(దత్తాంశం, "Wildcard - Parsing started");
  తిరిగి CURLE_OK;

fail:
  ఒకవేళ(ftpwc) {
    Curl_ftp_parselist_దత్తాంశం_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->నమూనా);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ఖాళీ;
  తిరిగి ఫలితం;
}

స్థిర CURLcode wc_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ftp_conn *ftpc,
                             నిర్మాణం FTP *ftp)
{
  నిర్మాణం Wildcard_దత్తాంశం * స్థిరరాశి wildcard = దత్తాంశం->wildcard;
  CURLcode ఫలితం = CURLE_OK;

  కొరకు(;;) {
    ఎంపిక(wildcard->state) {
    స్థితి CURLWC_INIT:
      ఫలితం = init_wc_దత్తాంశం(దత్తాంశం, ftpc, ftp);
      ఒకవేళ(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        తిరిగి ఫలితం;
      wildcard->state = ఫలితం ? CURLWC_ERROR : CURLWC_MATCHING;
      తిరిగి ఫలితం;

    స్థితి CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      నిర్మాణం ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ఒకవేళ(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        కొనసాగు;
      }
      ఒకవేళ(Curl_llist_లెక్క(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;
      }
      కొనసాగు;
    }

    స్థితి CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      నిర్మాణం Curl_llist_కణుపు *తల = Curl_llist_తల(&wildcard->filelist);
      నిర్మాణం curl_fileinfo *finfo = Curl_కణుపు_elem(తల);
      అక్షరం *tmp_మార్గం;
      అక్షరం *enc = curl_easy_escape(ఖాళీ, finfo->filename, 0);
      ఒకవేళ(!enc)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      tmp_మార్గం = curl_maprintf("%s%s", wildcard->మార్గం, enc);
      curl_free(enc);
      ఒకవేళ(!tmp_మార్గం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->మార్గం = tmp_మార్గం;

      infof(దత్తాంశం, "Wildcard - START of \"%s\"", finfo->filename);
      ఒకవేళ(దత్తాంశం->సమితి.chunk_bgn) {
        దీర్ఘ userresponse;
        నిర్మాణం Curl_mapi_guard guard;
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_chunk_bgn);
        userresponse = దత్తాంశం->సమితి.chunk_bgn(
          finfo, దత్తాంశం->సమితి.wildcardptr,
          (పూర్ణాంకం)Curl_llist_లెక్క(&wildcard->filelist));
        CURL_CBAPI_చివర(&guard);
        ఎంపిక(userresponse) {
        స్థితి CURL_CHUNK_BGN_FUNC_SKIP:
          infof(దత్తాంశం, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          కొనసాగు;
        స్థితి CURL_CHUNK_BGN_FUNC_FAIL:
          తిరిగి CURLE_CHUNK_FAILED;
        }
      }

      ఒకవేళ(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        కొనసాగు;
      }

      ఒకవేళ(finfo->flags & CURLFINFOFLAG_తెలిసిన_కొలత)
        ftpc->తెలిసిన_filesize = finfo->కొలత;

      ఫలితం = ftp_parse_url_మార్గం(దత్తాంశం, ftpc, ftp);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_కణుపు_remove(Curl_llist_తల(&wildcard->filelist));

      ఒకవేళ(Curl_llist_లెక్క(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        తిరిగి CURLE_OK;
      }
      తిరిగి ఫలితం;
    }

    స్థితి CURLWC_SKIP: {
      ఒకవేళ(దత్తాంశం->సమితి.chunk_చివర) {
        నిర్మాణం Curl_mapi_guard guard;
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_chunk_చివర);
        దత్తాంశం->సమితి.chunk_చివర(దత్తాంశం->సమితి.wildcardptr);
        CURL_CBAPI_చివర(&guard);
      }
      Curl_కణుపు_remove(Curl_llist_తల(&wildcard->filelist));
      wildcard->state = (Curl_llist_లెక్క(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      కొనసాగు;
    }

    స్థితి CURLWC_CLEAN: {
      నిర్మాణం ftp_wc *ftpwc = wildcard->ftpwc;
      ఫలితం = CURLE_OK;
      ఒకవేళ(ftpwc)
        ఫలితం = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = ఫలితం ? CURLWC_ERROR : CURLWC_పూర్తి;
      తిరిగి ఫలితం;
    }

    స్థితి CURLWC_పూర్తి:
    స్థితి CURLWC_ERROR:
    స్థితి CURLWC_CLEAR:
      ఒకవేళ(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ఖాళీ;
      }
      తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_regular_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ftp_conn *ftpc,
                                     నిర్మాణం FTP *ftp,
                                     bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  bool connected = FALSE;
  దత్తాంశం->req.కొలత = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(దత్తాంశం);

  ftpc->ctl_valid = TRUE; /* starts good */

  ఫలితం = ftp_perform(దత్తాంశం, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_పూర్తి); /* all commands in the DO-phase done? */

  ఒకవేళ(!ఫలితం) {

    ఒకవేళ(!*dophase_పూర్తి)
      /* the DO phase has not completed yet */
      తిరిగి CURLE_OK;

    ఫలితం = ftp_dophase_పూర్తి(దత్తాంశం, ftpc, ftp, connected);

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
    freedirs(ftpc);

  తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  నిర్మాణం FTP *ftp = Curl_meta_get(దత్తాంశం, CURL_META_FTP_EASY);

  *పూర్తి = FALSE; /* default to false */
  ఒకవేళ(!ftpc || !ftp)
    తిరిగి CURLE_FAILED_INIT;
  ftpc->wait_దత్తాంశం_conn = FALSE; /* default to no such wait */

#నిర్వచితమైతే CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    నిర్మాణం Curl_cwriter *ftp_lc_writer;

    ఫలితం = Curl_cwriter_create(&ftp_lc_writer, దత్తాంశం, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఫలితం = Curl_cwriter_కూడు(దత్తాంశం, ftp_lc_writer);
    ఒకవేళ(ఫలితం) {
      Curl_cwriter_free(దత్తాంశం, ftp_lc_writer);
      తిరిగి ఫలితం;
    }
  }
#అయితే_ముగింపు /* CURL_PREFER_LF_LINEENDS */

  ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
    ఫలితం = wc_statemach(దత్తాంశం, ftpc, ftp);
    ఒకవేళ(దత్తాంశం->wildcard->state == CURLWC_SKIP ||
       దత్తాంశం->wildcard->state == CURLWC_పూర్తి) {
      /* do not call ftp_regular_transfer */
      తిరిగి CURLE_OK;
    }
    ఒకవేళ(ఫలితం) /* error, loop or skipping the file */
      తిరిగి ఫలితం;
  }
  లేకపోతే { /* no wildcard FSM needed */
    ఫలితం = ftp_parse_url_మార్గం(దత్తాంశం, ftpc, ftp);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఫలితం = ftp_regular_transfer(దత్తాంశం, ftpc, ftp, పూర్తి);

  తిరిగి ఫలితం;
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
స్థిర CURLcode ftp_quit(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం ftp_conn *ftpc)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ftpc->ctl_valid) {
    CURL_TRC_FTP(దత్తాంశం, "sending QUIT to close session");
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &ftpc->pp, "%s", "QUIT");
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Failure sending QUIT command: %s",
            curl_easy_strerror(ఫలితం));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(దత్తాంశం->conn); /* mark for closure */
      ftp_state(దత్తాంశం, ftpc, FTP_STOP);
      తిరిగి ఫలితం;
    }

    ftp_state(దత్తాంశం, ftpc, FTP_QUIT);

    ఫలితం = ftp_ఖండం_statemach(దత్తాంశం, ftpc);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
స్థిర CURLcode ftp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               bool dead_connection)
{
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ఒకవేళ(!ftpc)
    తిరిగి CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ఒకవేళ(dead_connection || Curl_pp_needs_flush(దత్తాంశం, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (శూన్య)ftp_quit(దత్తాంశం, ftpc); /* ignore errors on the QUIT */
  తిరిగి CURLE_OK;
}

/* called from multi.c while DOing */
స్థిర CURLcode ftp_doing(నిర్మాణం Curl_easy *దత్తాంశం,
                          bool *dophase_పూర్తి)
{
  నిర్మాణం ftp_conn *ftpc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_FTP_CONN);
  నిర్మాణం FTP *ftp = Curl_meta_get(దత్తాంశం, CURL_META_FTP_EASY);
  CURLcode ఫలితం;

  ఒకవేళ(!ftpc || !ftp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = ftp_statemach(దత్తాంశం, ftpc, dophase_పూర్తి);

  ఒకవేళ(ఫలితం)
    CURL_TRC_FTP(దత్తాంశం, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  లేకపోతే ఒకవేళ(*dophase_పూర్తి) {
    ఫలితం = ftp_dophase_పూర్తి(దత్తాంశం, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(దత్తాంశం, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య ftp_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం FTP *ftp = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

స్థిర శూన్య ftp_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ftp_conn *ftpc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_గమ్యం_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

స్థిర శూన్య type_url_check(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం FTP *ftp)
{
  size_t len = అక్షరశ్రేణి_పొడవు(ftp->మార్గం);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ఒకవేళ((len >= 7) && !memcmp(&ftp->మార్గం[len - 7], ";type=", 6)) {
    అక్షరం *type = &ftp->మార్గం[len - 7];
    అక్షరం command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ఎంపిక(command) {
    స్థితి 'A': /* ASCII mode */
      దత్తాంశం->state.prefer_ascii = TRUE;
      ఆపు;

    స్థితి 'D': /* directory mode */
      దత్తాంశం->state.list_only = TRUE;
      ఆపు;

    స్థితి 'I': /* binary mode */
    అప్రమేయ:
      /* switch off ASCII */
      దత్తాంశం->state.prefer_ascii = FALSE;
      ఆపు;
    }
  }
}

స్థిర CURLcode ftp_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  నిర్మాణం FTP *ftp;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ftp_conn *ftpc;

  ftp = curlx_calloc(1, పరిమాణం(*ftp));
  ఒకవేళ(!ftp ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ftpc = curlx_calloc(1, పరిమాణం(*ftpc));
  ఒకవేళ(!ftpc ||
     Curl_conn_meta_సమితి(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* clone connection related data that is FTP specific */
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(దత్తాంశం, STRING_FTP_ACCOUNT));
    ఒకవేళ(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_FTP_ALTERNATIVE_గమ్యం_USER)) {
    ftpc->alternative_గమ్యం_user =
      curlx_strdup(CURL_EASY_STR(దత్తాంశం, STRING_FTP_ALTERNATIVE_గమ్యం_USER));
    ఒకవేళ(!ftpc->alternative_గమ్యం_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  ftp->మార్గం = &దత్తాంశం->state.పైకి.మార్గం[1]; /* do not include the initial slash */

  type_url_check(దత్తాంశం, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->తెలిసిన_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = దత్తాంశం->సమితి.use_ssl;
  ftpc->ccc = దత్తాంశం->సమితి.ftp_ccc;

  CURL_TRC_FTP(దత్తాంశం, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

bool Curl_ftp_conns_సరిపోలిక(నిర్మాణం connectdata *needle, నిర్మాణం connectdata *conn)
{
  నిర్మాణం ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  నిర్మాణం ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ఒకవేళ(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_గమ్యం_user,
                     cftpc->alternative_గమ్యం_user) ||
     (nftpc->ccc != cftpc->ccc))
    తిరిగి FALSE;

  ఎంపిక(nftpc->use_ssl) {
  స్థితి CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    తిరిగి (cftpc->use_ssl != CURLUSESSL_NONE);
  అప్రమేయ:
    తిరిగి (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ftp = {
  ftp_అమరిక_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_పూర్తి,                        /* done */
  ftp_do_మరిన్ని,                     /* do_more */
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

#అయితే_ముగింపు /* CURL_DISABLE_FTP */
