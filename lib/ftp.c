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

#यदि_अपरिभाषितम् CURL_DISABLE_FTP

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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

#यदि_अपरिभाषितम् NI_MAXHOST
#परिभाषय NI_MAXHOST 1025
#यदि_अन्तः
#यदि_अपरिभाषितम् INET_ADDRSTRLEN
#परिभाषय INET_ADDRSTRLEN 16
#यदि_अन्तः

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#परिभाषय STATUSCODE(पङ्क्तिः) \
  (ISDIGIT((पङ्क्तिः)[0]) && ISDIGIT((पङ्क्तिः)[1]) && ISDIGIT((पङ्क्तिः)[2]))

/* macro to check for the last line in an FTP server response */
#परिभाषय LASTLINE(पङ्क्तिः) (STATUSCODE(पङ्क्तिः) && (' ' == (पङ्क्तिः)[3]))

#यदि_परिभाषितम् CURLVERBOSE
/* for tracing purposes */
स्थिर ध्रुव वर्ण * ध्रुव ftp_state_नामानि[] = {
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
#परिभाषय FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_नामानि[(ftpc)->state] : "???")

#यदि_अन्तः /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
स्थिर शून्य ftp_state_नीचः(संरचना Curl_easy *दत्त,
                          संरचना ftp_conn *ftpc,
                          ftpstate newstate
#यदि_परिभाषितम् DEBUGBUILD
                          , पूर्णाङ्क lineno
#यदि_अन्तः
  )
{
  यदि(ftpc->state != newstate) {
#यदि_परिभाषितम् DEBUGBUILD
    NOVERBOSE((शून्य)lineno);
    CURL_TRC_FTP(दत्त, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_नामानि[newstate], lineno);
#अन्यथा
    CURL_TRC_FTP(दत्त, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_नामानि[newstate]);
#यदि_अन्तः
  }
  ftpc->state = newstate;
}

/* Local API functions */
#यदि_अपरिभाषितम् DEBUGBUILD
#परिभाषय ftp_state(x, y, z) ftp_state_नीचः(x, y, z)
#अन्यथा /* !DEBUGBUILD */
#परिभाषय ftp_state(x, y, z) ftp_state_नीचः(x, y, z, __LINE__)
#यदि_अन्तः /* DEBUGBUILD */

स्थिर CURLcode ftp_state_mdtm(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp);
स्थिर CURLcode ftp_state_quote(संरचना Curl_easy *दत्त,
                                संरचना ftp_conn *ftpc,
                                संरचना FTP *ftp,
                                bool init, ftpstate instate);
स्थिर CURLcode ftp_nb_type(संरचना Curl_easy *दत्त,
                            संरचना ftp_conn *ftpc,
                            संरचना FTP *ftp,
                            bool ascii, ftpstate newstate);
स्थिर CURLcode getftpresponse(संरचना Curl_easy *दत्त, size_t *nreadp,
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
  size_t अङ्कः = 0;
  कुरु {
    slashPos = strchr(str, '/');
    यदि(slashPos) {
      ++अङ्कः;
      str = slashPos + 1;
    }
  } यावत्(slashPos);
  प्रत्यागम अङ्कः;
}

#परिभाषय FTP_MAX_DIR_गभीरता 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
स्थिर CURLcode ftp_parse_url_मार्गः(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp)
{
  ध्रुव वर्ण *slashPos = रिक्त;
  ध्रुव वर्ण *fileName = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *raw_मार्गः = रिक्त; /* URL-decoded "raw" path */
  size_t मार्गः_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  यदि(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  परिणाम = Curl_urldecode(ftp->मार्गः, 0, &ftpc->rawpath, &मार्गः_Len, REJECT_CTRL);
  यदि(परिणाम) {
    failf(दत्त, "path contains control characters");
    प्रत्यागम परिणाम;
  }
  raw_मार्गः = ftpc->rawpath;

  विकल्प(दत्त->समुच्चयः.ftp_filemethod) {
  पक्ष FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    यदि((मार्गः_Len > 0) && (raw_मार्गः[मार्गः_Len - 1] != '/'))
      fileName = raw_मार्गः;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    भङ्ग;

  पक्ष FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_मार्गः, '/');
    यदि(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_मार्गः;
      यदि(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, परिमाण(ftpc->dirs[0]));
      यदि(!ftpc->dirs)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (पूर्णाङ्क)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    अन्यथा
      fileName = raw_मार्गः; /* filename only (or empty) */
    भङ्ग;

  स्वभाव: /* allow pretty much anything */
  पक्ष FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ध्रुव वर्ण *curPos = raw_मार्गः;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_मार्गः);

    यदि(dirAlloc >= FTP_MAX_DIR_गभीरता)
      /* suspiciously deep directory hierarchy */
      प्रत्यागम CURLE_URL_MALFORMAT;

    यदि(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, परिमाण(ftpc->dirs[0]));
      यदि(!ftpc->dirs)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

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
          ftpc->dirs[ftpc->dirdepth].start = (पूर्णाङ्क)(curPos - raw_मार्गः);
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

  यदि(दत्त->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(दत्त, "Uploading to a URL without a filename");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  यदि((दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD) && (raw_मार्गः[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  अन्यथा { /* newly created FTP connections are already in entry path */
    ध्रुव वर्ण *old_मार्गः = दत्त->conn->द्वयङ्काः.reuse ? ftpc->prevpath : "";
    यदि(old_मार्गः) {
      size_t n = मार्गः_Len;
      यदि(दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      अन्यथा
        n -= ftpc->file ? वाक्यदैर्घ्यम्(ftpc->file) : 0;

      यदि((वाक्यदैर्घ्यम्(old_मार्गः) == n) && raw_मार्गः && !वाक्ये_तुलय(raw_मार्गः, old_मार्गः, n)) {
        infof(दत्त, "Request has same path as previous transfer");
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

स्थिर शून्य close_secondarysocket(संरचना Curl_easy *दत्त,
                                  संरचना ftp_conn *ftpc)
{
  (शून्य)ftpc;
  CURL_TRC_FTP(दत्त, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(दत्त, दत्त->conn, SECONDARYSOCKET);
}

#यदि_परिभाषितम् CURL_PREFER_LF_LINEENDS
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

स्थिर CURLcode ftp_cw_lc_write(संरचना Curl_easy *दत्त,
                                संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                ध्रुव वर्ण *buf, size_t blen)
{
  स्थिर ध्रुव वर्ण nl = '\n';
  संरचना ftp_cw_lc_ctx *ctx = writer->ctx;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);

  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  यावत्(blen) {
    /* do not pass EOS when writing parts */
    पूर्णाङ्क chunk_type = (type & ~CLIENTWRITE_EOS);
    ध्रुव वर्ण *cp;
    size_t chunk_len;
    CURLcode परिणाम;

    यदि(ctx->newline_pending) {
      यदि(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        परिणाम = Curl_cwriter_write(दत्त, writer->अग्र, chunk_type, &nl, 1);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
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
      परिणाम = Curl_cwriter_write(दत्त, writer->अग्र, chunk_type,
                                  buf, chunk_len);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  यदि(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, blen);
  }
  अन्यथा यदि(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    यदि(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, 0);
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

#यदि_अन्तः /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
स्थिर CURLcode ftp_check_ctrl_on_दत्त_wait(संरचना Curl_easy *दत्त,
                                            संरचना ftp_conn *ftpc)
{
  संरचना connectdata *conn = दत्त->conn;
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
      infof(दत्त, "There is negative response in cache while serv connect");
      (शून्य)getftpresponse(दत्त, &nread, &ftpcode);
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
    }
  }

  यदि(pp->अतिप्रवाहः)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  अन्यथा {
    पूर्णाङ्क socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    विकल्प(socketstate) {
    पक्ष -1: /* error */
      /* let's die here */
      failf(दत्त, "Error while waiting for server connect");
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
    स्वभाव:
      यदि(socketstate & CURL_CSELECT_अन्तर्)
        response = TRUE;
      भङ्ग;
    }
  }

  यदि(response) {
    infof(दत्त, "Ctrl conn has data while waiting for data conn");
    यदि(pp->अतिप्रवाहः > 3) {
      ध्रुव वर्ण *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->अतिप्रवाहः + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      यदि((len > 3) && LASTLINE(r)) {
        curl_off_t स्थितिः;
        यदि(!curlx_str_सङ्ख्या(&r, &स्थितिः, 999) && (स्थितिः == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(दत्त, "Got 226 before data activity");
          प्रत्यागम CURLE_OK;
        }
      }
    }

    (शून्य)getftpresponse(दत्त, &nread, &ftpcode);

    infof(दत्त, "FTP code: %03d", ftpcode);

    यदि(ftpcode / 100 > 3)
      प्रत्यागम CURLE_FTP_ACCEPT_FAILED;

    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
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
स्थिर CURLcode ftp_initiate_transfer(संरचना Curl_easy *दत्त,
                                      संरचना ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(दत्त, "ftp_initiate_transfer()");
  परिणाम = Curl_conn_connect(दत्त, SECONDARYSOCKET, TRUE, &connected);
  यदि(परिणाम || !connected)
    प्रत्यागम परिणाम;

  यदि(दत्त->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_सज्जता_send(दत्त, SECONDARYSOCKET);
    Curl_xfer_समुच्चयः_shutdown(दत्त, TRUE, TRUE);
  }
  अन्यथा {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_सज्जता_recv(दत्त, SECONDARYSOCKET, दत्त->req.आकार);
    Curl_xfer_समुच्चयः_shutdown(दत्त, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(दत्त, ftpc, FTP_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर bool ftp_endofresp(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                          ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *संहिता)
{
  curl_off_t स्थितिः;
  (शून्य)दत्त;
  (शून्य)conn;

  यदि((len > 3) && LASTLINE(पङ्क्तिः) && !curlx_str_सङ्ख्या(&पङ्क्तिः, &स्थितिः, 999)) {
    *संहिता = (पूर्णाङ्क)स्थितिः;
    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

स्थिर CURLcode ftp_readresp(संरचना Curl_easy *दत्त,
                             संरचना ftp_conn *ftpc,
                             int8_t sockindex,
                             संरचना pingpong *pp,
                             पूर्णाङ्क *ftpcodep, /* return the ftp-code if done */
                             size_t *आकार) /* size of the response */
{
  पूर्णाङ्क संहिता;
  CURLcode परिणाम = Curl_pp_readresp(दत्त, sockindex, pp, &संहिता, आकार);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  यदि(!ftpc->shutdown)
    दत्त->info.httpcode = संहिता;

  *ftpcodep = संहिता;

  यदि(संहिता == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(दत्त, "We got a 421 - timeout");
    ftp_state(दत्त, ftpc, FTP_STOP);
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }

  प्रत्यागम परिणाम;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
स्थिर CURLcode getftpresponse(संरचना Curl_easy *दत्त,
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

  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode परिणाम = CURLE_OK;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  संरचना pingpong *pp = &ftpc->pp;
  size_t nread;
  पूर्णाङ्क cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(दत्त, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(!*ftpcodep && !परिणाम) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(दत्त, pp);
    timediff_t interval_ms;

    यदि(timeout <= 0) {
      failf(दत्त, "FTP response timeout");
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
    अन्यथा यदि(!Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(दत्त, pp) ?
        sockfd : CURL_SOCKET_BAD;
      पूर्णाङ्क ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      यदि(ev < 0) {
        failf(दत्त, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        प्रत्यागम CURLE_RECV_ERROR;
      }
      अन्यथा यदि(ev == 0) {
        परिणाम = Curl_pgrsUpdate(दत्त);
        अनुवर्त; /* continue in our loop for the timeout duration */
      }
    }

    यदि(Curl_pp_needs_flush(दत्त, pp)) {
      परिणाम = Curl_pp_flushsend(दत्त, pp);
      यदि(परिणाम)
        भङ्ग;
    }

    परिणाम = ftp_readresp(दत्त, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    यदि(परिणाम)
      भङ्ग;

    यदि(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
दत्त*/
      cache_skip++;
    अन्यथा
      /* when we got data or there is no cache left, we reset the cache skip
गणक*/
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(दत्त, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (पूर्णाङ्क)परिणाम, *nreadp, *ftpcodep);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_user(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना connectdata *conn)
{
  CURLcode परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  यदि(!परिणाम) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(दत्त, ftpc, FTP_USER);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_pwd(संरचना Curl_easy *दत्त,
                              संरचना ftp_conn *ftpc)
{
  CURLcode परिणाम;
#यदि_परिभाषितम् DEBUGBUILD
  यदि(!दत्त->id && getenv("CURL_FTP_PWD_STOP"))
    प्रत्यागम CURLE_OK;
#यदि_अन्तः
  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "PWD");
  यदि(!परिणाम)
    ftp_state(दत्त, ftpc, FTP_PWD);

  प्रत्यागम परिणाम;
}

/* For the FTP "protocol connect" and "doing" phases only */
स्थिर CURLcode ftp_pollset(संरचना Curl_easy *दत्त,
                            संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  प्रत्यागम ftpc ? Curl_pp_pollset(दत्त, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
स्थिर CURLcode ftp_domore_pollset(संरचना Curl_easy *दत्त,
                                   संरचना easy_pollset *ps)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);

  यदि(!ftpc)
    प्रत्यागम CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(दत्त, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  यदि(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(दत्त->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (दत्त->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(दत्त->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    प्रत्यागम Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
  }
  प्रत्यागम Curl_pp_pollset(दत्त, &ftpc->pp, ps);
}

स्थिर पूर्णाङ्क pathlen(संरचना ftp_conn *ftpc, पूर्णाङ्क अङ्कः)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अङ्कः);
  प्रत्यागम ftpc->dirs[अङ्कः].len;
}

स्थिर ध्रुव वर्ण *pathpiece(संरचना ftp_conn *ftpc, पूर्णाङ्क अङ्कः)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > अङ्कः);
  प्रत्यागम &ftpc->rawpath[ftpc->dirs[अङ्कः].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
स्थिर CURLcode ftp_state_cwd(संरचना Curl_easy *दत्त,
                              संरचना ftp_conn *ftpc,
                              संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(ftpc->cwddone)
    /* already done and fine */
    परिणाम = ftp_state_mdtm(दत्त, ftpc, ftp);
  अन्यथा {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((दत्त->समुच्चयः.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    यदि(दत्त->conn->द्वयङ्काः.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "CWD %s", ftpc->entrypath);
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_CWD);
    }
    अन्यथा {
      यदि(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        यदि(!परिणाम)
          ftp_state(दत्त, ftpc, FTP_CWD);
      }
      अन्यथा {
        /* No CWD necessary */
        परिणाम = ftp_state_mdtm(दत्त, ftpc, ftp);
      }
    }
  }
  प्रत्यागम परिणाम;
}

प्रकारनिर्देश गणना {
  EPRT,
  पत्तनम्_2,
  समाप्तम्_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
स्थिर CURLcode ftp_पत्तनम्_parse_string(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn,
                                      ध्रुव वर्ण *string_ftpport,
                                      संरचना Curl_sockaddr_storage *ss,
                                      अचिह्नित ह्रस्व *पत्तनम्_minp,
                                      अचिह्नित ह्रस्व *पत्तनम्_maxp,
                                      ध्रुव वर्ण **hostp,
                                      वर्ण *hbuf, size_t hbuflen)
{
  ध्रुव वर्ण *ip_अन्तः = रिक्त;
  ध्रुव वर्ण *addr = रिक्त;
  size_t addrlen = 0;
  अचिह्नित ह्रस्व पत्तनम्_min = 0;
  अचिह्नित ह्रस्व पत्तनम्_max = 0;
  वर्ण ipstr[50];
#यदि_अपरिभाषितम् USE_IPV6
  (शून्य)conn;
  (शून्य)ss;
#यदि_अन्तः

  /* default to nothing */
  *hostp = रिक्त;
  *पत्तनम्_minp = *पत्तनम्_maxp = 0;

  यदि(!string_ftpport || (वाक्यदैर्घ्यम्(string_ftpport) <= 1))
    गच्छ समाप्तम्;

#यदि_परिभाषितम् USE_IPV6
  यदि(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ध्रुव वर्ण *ip_start = string_ftpport + 1;
    ip_अन्तः = strchr(ip_start, ']');
    यदि(ip_अन्तः) {
      addrlen = ip_अन्तः - ip_start;
      addr = ip_start;
    }
  }
  अन्यथा
#यदि_अन्तः
    यदि(*string_ftpport == ':') {
      /* :port */
      ip_अन्तः = string_ftpport;
    }
    अन्यथा {
      ip_अन्तः = strchr(string_ftpport, ':');
      addr = string_ftpport;
      यदि(ip_अन्तः) {
#यदि_परिभाषितम् USE_IPV6
        संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)ss;
#यदि_अन्तः
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_अन्तः - string_ftpport;
#यदि_परिभाषितम् USE_IPV6
        यदि(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = वाक्यदैर्घ्यम्(string_ftpport);
          ip_अन्तः = रिक्त; /* this got no port ! */
        }
#यदि_अन्तः
      }
      अन्यथा
        /* ipv4|interface */
        addrlen = वाक्यदैर्घ्यम्(string_ftpport);
    }

  /* parse the port */
  यदि(ip_अन्तः) {
    ध्रुव वर्ण *portp = strchr(ip_अन्तः, ':');
    यदि(portp) {
      curl_off_t start;
      curl_off_t अन्तः;
      portp++;
      यदि(!curlx_str_सङ्ख्या(&portp, &start, 0xffff)) {
        पत्तनम्_min = (अचिह्नित ह्रस्व)start;
        यदि(!curlx_str_single(&portp, '-') &&
           !curlx_str_सङ्ख्या(&portp, &अन्तः, 0xffff))
          पत्तनम्_max = (अचिह्नित ह्रस्व)अन्तः;
        अन्यथा
          पत्तनम्_max = पत्तनम्_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  यदि(पत्तनम्_min > पत्तनम्_max)
    पत्तनम्_min = पत्तनम्_max = 0;

  यदि(addrlen) {
    ध्रुव संरचना Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(दत्त, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    यदि(!remote_addr || (addrlen >= परिमाण(ipstr)) || (addrlen >= hbuflen))
      प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
    स्मृतिं_प्रतिलिख(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    विकल्प(Curl_if2ip(remote_addr->family,
#यदि_परिभाषितम् USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#यदि_अन्तः
                      ipstr, hbuf, hbuflen)) {
    पक्ष IF2_IP_NOT_प्राप्तम्:
      /* not an interface, use the string as hostname instead */
      स्मृतिं_प्रतिलिख(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      भङ्ग;
    पक्ष IF2IP_AF_NOT_SUPPORTED:
      प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
    पक्ष IF2_IP_प्राप्तम्:
      *hostp = hbuf; /* use the hbuf for hostname */
      भङ्ग;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

समाप्तम्:
  *पत्तनम्_minp = पत्तनम्_min;
  *पत्तनम्_maxp = पत्तनम्_max;
  प्रत्यागम CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
स्थिर CURLcode ftp_पत्तनम्_default_host(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ध्रुव वर्ण **hostp,
                                      वर्ण *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#यदि_अन्तः
  वर्ण बफर[STRERROR_LEN];
  CURLcode परिणाम;

  *sslenp = परिमाण(*ss);
  यदि(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(दत्त, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
  }
  विकल्प(sa->sa_family) {
#यदि_परिभाषितम् USE_IPV6
  पक्ष AF_INET6:
    परिणाम = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    भङ्ग;
#यदि_अन्तः
  स्वभाव:
    परिणाम = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    भङ्ग;
  }
  यदि(परिणाम)
    प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  प्रत्यागम CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
स्थिर CURLcode ftp_पत्तनम्_resolve_host(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn,
                                      ध्रुव वर्ण *host,
                                      संरचना Curl_dns_प्रविष्टिः **dns_entryp,
                                      ध्रुव संरचना Curl_addrinfo **resp)
{
  CURLcode परिणाम;

  *resp = रिक्त;
  परिणाम = Curl_resolv_blocking(
    दत्त, Curl_resolv_dns_queries(दत्त, conn->ip_संस्करणम्),
    host, 0, Curl_conn_get_transport(दत्त, conn), dns_entryp);
  यदि(परिणाम)
    failf(दत्त, "failed to resolve the address provided to PORT: %s", host);
  अन्यथा {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  प्रत्यागम परिणाम;
}

/*
 * Open a TCP socket for the resolved address family.
 */
स्थिर CURLcode ftp_पत्तनम्_open_socket(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn,
                                     ध्रुव संरचना Curl_addrinfo *res,
                                     ध्रुव संरचना Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  वर्ण बफर[STRERROR_LEN];
  पूर्णाङ्क sockerr = 0;
  ध्रुव संरचना Curl_addrinfo *ai;
  CURLcode परिणाम = CURLE_FTP_पत्तनम्_FAILED;

  प्रति(ai = res; ai; ai = ai->ai_अग्र) {
    परिणाम =
      Curl_socket_open(दत्त, ai, रिक्त,
                       Curl_conn_get_transport(दत्त, conn), portsockp);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        प्रत्यागम परिणाम;
      परिणाम = CURLE_FTP_पत्तनम्_FAILED;
      sockerr = SOCKERRNO;
      अनुवर्त;
    }
    भङ्ग;
  }
  यदि(!ai) {
    failf(दत्त, "socket failure: %s",
          curlx_strerror(sockerr, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
  }
  *aip = ai;
  प्रत्यागम परिणाम;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
स्थिर CURLcode ftp_पत्तनम्_bind_socket(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn,
                                     curl_socket_t portsock,
                                     ध्रुव संरचना Curl_addrinfo *ai,
                                     संरचना Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     अचिह्नित ह्रस्व पत्तनम्_min,
                                     अचिह्नित ह्रस्व पत्तनम्_max,
                                     bool non_local)
{
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#यदि_अन्तः
  वर्ण बफर[STRERROR_LEN];
  अचिह्नित ह्रस्व पत्तनम्;
  पूर्णाङ्क sockerr;

  स्मृतिं_प्रतिलिख(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  प्रति(पत्तनम् = पत्तनम्_min; पत्तनम् <= पत्तनम्_max;) {
    यदि(sa->sa_family == AF_INET)
      sa4->sin_port = htons(पत्तनम्);
#यदि_परिभाषितम् USE_IPV6
    अन्यथा
      sa6->sin6_port = htons(पत्तनम्);
#यदि_अन्तः
    यदि(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      यदि(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(दत्त, "bind(port=%hu) on non-local address failed: %s", पत्तनम्,
              curlx_strerror(sockerr, बफर, परिमाण(बफर)));

        *sslen_io = परिमाण(*ss);
        यदि(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(दत्त, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
          प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
        }
        पत्तनम् = पत्तनम्_min;
        non_local = FALSE; /* do not try this again */
        अनुवर्त;
      }
      यदि(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(दत्त, "bind(port=%hu) failed: %s", पत्तनम्,
              curlx_strerror(sockerr, बफर, परिमाण(बफर)));
        प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
      }
    }
    अन्यथा
      भङ्ग;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    यदि(पत्तनम् == पत्तनम्_max) {
      failf(दत्त, "bind() failed, ran out of ports");
      प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
    }
    पत्तनम्++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = परिमाण(*ss);
  यदि(getsockname(portsock, sa, sslen_io)) {
    failf(दत्त, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
  }
  CURL_TRC_FTP(दत्त, "ftp_port_bind_socket(), socket bound to port %d", पत्तनम्);
  प्रत्यागम CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
स्थिर CURLcode ftp_पत्तनम्_listen(संरचना Curl_easy *दत्त, curl_socket_t portsock)
{
  वर्ण बफर[STRERROR_LEN];

  यदि(listen(portsock, 1)) {
    failf(दत्त, "socket failure: %s",
          curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_FTP_पत्तनम्_FAILED;
  }
  CURL_TRC_FTP(दत्त, "ftp_port_listen(), listening on port");
  प्रत्यागम CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
स्थिर CURLcode ftp_पत्तनम्_send_command(संरचना Curl_easy *दत्त,
                                      संरचना ftp_conn *ftpc,
                                      संरचना connectdata *conn,
                                      संरचना Curl_sockaddr_storage *ss,
                                      ध्रुव संरचना Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  स्थिर ध्रुव वर्ण बहुलकम्[][5] = { "EPRT", "PORT" };
  संरचना sockaddr *sa = (संरचना sockaddr *)ss;
  संरचना sockaddr_in * ध्रुव sa4 = (शून्य *)sa;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)sa;
#यदि_अन्तः
  वर्ण myhost[MAX_IPADR_LEN + 1] = "";
  अचिह्नित ह्रस्व पत्तनम्;
  CURLcode परिणाम;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_स्थानाङ्कः(ai, myhost, परिमाण(myhost));

#यदि_परिभाषितम् USE_IPV6
  यदि(!conn->द्वयङ्काः.ftp_use_eprt && conn->द्वयङ्काः.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->द्वयङ्काः.ftp_use_eprt = TRUE;
#यदि_अन्तः

  प्रति(; fcmd != समाप्तम्_2; fcmd++) {

    यदि(!conn->द्वयङ्काः.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      अनुवर्त;

    यदि((पत्तनम्_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      अनुवर्त;

    विकल्प(sa->sa_family) {
    पक्ष AF_INET:
      पत्तनम् = ntohs(sa4->sin_port);
      भङ्ग;
#यदि_परिभाषितम् USE_IPV6
    पक्ष AF_INET6:
      पत्तनम् = ntohs(sa6->sin6_port);
      भङ्ग;
#यदि_अन्तः
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
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s |%d|%s|%hu|", बहुलकम्[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, पत्तनम्);
      यदि(परिणाम) {
        failf(दत्त, "Failure sending EPRT command: %s",
              curl_easy_strerror(परिणाम));
        प्रत्यागम परिणाम;
      }
      भङ्ग;
    }
    यदि(पत्तनम्_2 == fcmd) {
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
      curl_msnprintf(dest, 20, ",%d,%d", (पूर्णाङ्क)(पत्तनम् >> 8), (पूर्णाङ्क)(पत्तनम् & 0xff));

      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s %s", बहुलकम्[fcmd], target);
      यदि(परिणाम) {
        failf(दत्त, "Failure sending PORT command: %s",
              curl_easy_strerror(परिणाम));
        प्रत्यागम परिणाम;
      }
      भङ्ग;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(दत्त, ftpc, FTP_पत्तनम्);
  प्रत्यागम CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
स्थिर CURLcode ftp_state_use_पत्तनम्(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode परिणाम = CURLE_FTP_पत्तनम्_FAILED;
  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  संरचना Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  वर्ण hbuf[NI_MAXHOST];
  ध्रुव वर्ण *host = रिक्त;
  ध्रुव वर्ण *string_ftpport = CURL_EASY_STR(दत्त, STRING_FTPPORT);
  संरचना Curl_dns_प्रविष्टिः *dns_प्रविष्टिः = रिक्त;
  ध्रुव संरचना Curl_addrinfo *res = रिक्त;
  ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
  अचिह्नित ह्रस्व पत्तनम्_min = 0;
  अचिह्नित ह्रस्व पत्तनम्_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  परिणाम = ftp_पत्तनम्_parse_string(दत्त, conn, string_ftpport,
                                 &ss, &पत्तनम्_min, &पत्तनम्_max,
                                 &host, hbuf, परिमाण(hbuf));
  यदि(!परिणाम && !host)
    /* if no host was specified, use the control connection's local IP */
    परिणाम = ftp_पत्तनम्_default_host(दत्त, conn, &ss, &sslen, &host,
                                   hbuf, परिमाण(hbuf), &non_local);

  /* resolve host string to address list */
  यदि(!परिणाम)
    परिणाम = ftp_पत्तनम्_resolve_host(दत्त, conn, host, &dns_प्रविष्टिः, &res);

  /* Open a TCP socket for the data connection */
  यदि(!परिणाम)
    परिणाम = ftp_पत्तनम्_open_socket(दत्त, conn, res, &ai, &portsock);
  यदि(!परिणाम) {
    CURL_TRC_FTP(दत्त, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    परिणाम = ftp_पत्तनम्_bind_socket(दत्त, conn, portsock, ai, &ss, &sslen,
                                  पत्तनम्_min, पत्तनम्_max, non_local);
  }

  /* listen */
  यदि(!परिणाम)
    परिणाम = ftp_पत्तनम्_listen(दत्त, portsock);

  /* send the PORT / EPRT command */
  यदि(!परिणाम)
    परिणाम = ftp_पत्तनम्_send_command(दत्त, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  यदि(!परिणाम)
    परिणाम = Curl_conn_tcp_listen_समुच्चयः(दत्त, conn, SECONDARYSOCKET, &portsock);

  यदि(!परिणाम)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  यदि(dns_प्रविष्टिः)
    Curl_dns_प्रविष्टिः_unlink(दत्त, &dns_प्रविष्टिः);
  यदि(परिणाम) {
    ftp_state(दत्त, ftpc, FTP_STOP);
  }
  अन्यथा {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    यदि(conn->द्वयङ्काः.ftp_use_दत्त_ssl && दत्त->समुच्चयः.ftp_use_पत्तनम् &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      परिणाम = Curl_ssl_cfilter_सङ्कलय(
        दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->द्वयङ्काः.do_अधिकम् = FALSE;
    Curl_expire(दत्त, (दत्त->समुच्चयः.accepttimeout > 0) ?
                दत्त->समुच्चयः.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  यदि(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(दत्त, conn, portsock);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_use_pasv(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc,
                                   संरचना connectdata *conn)
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

  स्थिर ध्रुव वर्ण बहुलकम्[][5] = { "EPSV", "PASV" };
  पूर्णाङ्क modeoff;

#यदि_परिभाषितम् PF_INET6
  यदि(!conn->द्वयङ्काः.ftp_use_epsv && conn->द्वयङ्काः.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->द्वयङ्काः.ftp_use_epsv = TRUE;
#यदि_अन्तः

  modeoff = conn->द्वयङ्काः.ftp_use_epsv ? 0 : 1;

  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", बहुलकम्[modeoff]);
  यदि(!परिणाम) {
    ftpc->count1 = modeoff;
    ftp_state(दत्त, ftpc, FTP_PASV);
    infof(दत्त, "Connect data stream passively");
  }
  प्रत्यागम परिणाम;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
स्थिर CURLcode ftp_state_prepare_transfer(संरचना Curl_easy *दत्त,
                                           संरचना ftp_conn *ftpc,
                                           संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  यदि(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(दत्त, ftpc, FTP_RETR_PREQUOTE);
    परिणाम = ftp_state_quote(दत्त, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  अन्यथा यदि(दत्त->समुच्चयः.ftp_use_पत्तनम्) {
    /* We have chosen to use the PORT (or similar) command */
    परिणाम = ftp_state_use_पत्तनम्(दत्त, ftpc, EPRT);
  }
  अन्यथा {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    यदि(दत्त->समुच्चयः.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      यदि(!ftpc->file)
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) :
                               (दत्त->state.list_only ? "NLST" : "LIST"));
      अन्यथा यदि(दत्त->state.upload)
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "PRET STOR %s", ftpc->file);
      अन्यथा
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "PRET RETR %s", ftpc->file);
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_PRET);
    }
    अन्यथा
      परिणाम = ftp_state_use_pasv(दत्त, ftpc, conn);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_rest(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  यदि((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "REST %d", 0);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_REST);
  }
  अन्यथा
    परिणाम = ftp_state_prepare_transfer(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_आकार(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  यदि((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "SIZE %s", ftpc->file);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_आकार);
  }
  अन्यथा
    परिणाम = ftp_state_rest(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_list(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
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
  ध्रुव वर्ण *lstArg = रिक्त;
  पूर्णाङ्क lstArglen = 0;
  वर्ण *cmd;

  यदि((दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD) && ftp->मार्गः) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ध्रुव वर्ण *raw_मार्गः = ftpc->rawpath;
    ध्रुव वर्ण *slashPos = strrchr(raw_मार्गः, '/');
    यदि(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_मार्गः;
      यदि(n == 0)
        ++n;

      lstArg = raw_मार्गः;
      lstArglen = (पूर्णाङ्क)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) :
                      (दत्त->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  यदि(!cmd)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  यदि(!परिणाम)
    ftp_state(दत्त, ftpc, FTP_LIST);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_list_prequote(संरचना Curl_easy *दत्त,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(दत्त, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

स्थिर CURLcode ftp_state_retr_prequote(संरचना Curl_easy *दत्त,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(दत्त, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

स्थिर CURLcode ftp_state_stor_prequote(संरचना Curl_easy *दत्त,
                                        संरचना ftp_conn *ftpc,
                                        संरचना FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  प्रत्यागम ftp_state_quote(दत्त, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

स्थिर CURLcode ftp_state_type(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  यदि(दत्त->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)दत्त->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    परिणाम = ftp_nb_type(दत्त, ftpc, ftp, (bool)दत्त->state.prefer_ascii,
                         FTP_TYPE);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
    परिणाम = ftp_state_आकार(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
स्थिर CURLcode ftp_state_mdtm(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp)
{
  CURLcode परिणाम = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  यदि((दत्त->समुच्चयः.get_filetime || दत्त->समुच्चयः.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "MDTM %s", ftpc->file);

    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_MDTM);
  }
  अन्यथा
    परिणाम = ftp_state_type(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

/* This is called after the TYPE and possible quote commands have been sent */
स्थिर CURLcode ftp_state_ul_सज्जता(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   bool sizechecked)
{
  CURLcode परिणाम = CURLE_OK;
  curl_bit append = दत्त->समुच्चयः.remote_append;

  यदि((दत्त->state.resume_from && !sizechecked) ||
     ((दत्त->state.resume_from > 0) && sizechecked)) {
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

    यदि(दत्त->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "SIZE %s", ftpc->file);
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_STOR_आकार);
      प्रत्यागम परिणाम;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    यदि(दत्त->समुच्चयः.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_seek_func);
      seekerr = दत्त->समुच्चयः.seek_func(दत्त->समुच्चयः.seek_client,
                                    दत्त->state.resume_from, SEEK_SET);
      CURL_CBAPI_अन्तः(&guard);
    }

    यदि(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(दत्त, "Could not seek stream");
        प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      कुरु {
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (दत्त->state.resume_from - passed > (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) :
          curlx_sotouz(दत्त->state.resume_from - passed);

        size_t actuallyread =
          दत्त->state.fread_func(scratch, 1, readthisamountnow,
                                 दत्त->state.अन्तर्);

        passed += actuallyread;
        यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(दत्त, "Failed to read data");
          प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
        }
      } यावत्(passed < दत्त->state.resume_from);
    }
    /* now, decrease the size of the read */
    यदि(दत्त->state.infilesize > 0) {
      दत्त->state.infilesize -= दत्त->state.resume_from;

      यदि(दत्त->state.infilesize <= 0) {
        infof(दत्त, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_सज्जता_nop(दत्त);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(दत्त, ftpc, FTP_STOP);
        प्रत्यागम CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  यदि(!परिणाम)
    ftp_state(दत्त, ftpc, FTP_STOR);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_retr(संरचना Curl_easy *दत्त,
                               संरचना ftp_conn *ftpc,
                               संरचना FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_FTP(दत्त, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  यदि(दत्त->समुच्चयः.max_filesize && (filesize > दत्त->समुच्चयः.max_filesize)) {
    failf(दत्त, "Maximum file size exceeded");
    प्रत्यागम CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  यदि(दत्त->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    यदि(filesize == -1) {
      infof(दत्त, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    अन्यथा {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      यदि(दत्त->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        यदि(filesize < -दत्त->state.resume_from) {
          failf(दत्त, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                दत्त->state.resume_from, filesize);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -दत्त->state.resume_from;
        /* download from where? */
        दत्त->state.resume_from = filesize - ftp->downloadsize;
      }
      अन्यथा {
        यदि(filesize < दत्त->state.resume_from) {
          failf(दत्त, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                दत्त->state.resume_from, filesize);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - दत्त->state.resume_from;
      }
    }

    यदि(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_सज्जता_nop(दत्त);
      infof(दत्त, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(दत्त, ftpc, FTP_STOP);
      प्रत्यागम CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(दत्त, "Instructs server to resume from offset %" FMT_OFF_T,
          दत्त->state.resume_from);

    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "REST %" FMT_OFF_T,
                           दत्त->state.resume_from);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_RETR_REST);
  }
  अन्यथा {
    /* no resume */
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "RETR %s", ftpc->file);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_RETR);
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_quote(संरचना Curl_easy *दत्त,
                                संरचना ftp_conn *ftpc,
                                संरचना FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  bool quote = FALSE;
  संरचना curl_slist *item;

  विकल्प(instate) {
  पक्ष FTP_QUOTE:
  स्वभाव:
    item = दत्त->समुच्चयः.quote;
    भङ्ग;
  पक्ष FTP_RETR_PREQUOTE:
  पक्ष FTP_STOR_PREQUOTE:
  पक्ष FTP_LIST_PREQUOTE:
    item = दत्त->समुच्चयः.prequote;
    भङ्ग;
  पक्ष FTP_POSTQUOTE:
    item = दत्त->समुच्चयः.postquote;
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
      item = item->अग्र;
      i++;
    }
    यदि(item) {
      ध्रुव वर्ण *cmd = item->दत्त;
      यदि(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      अन्यथा
        ftpc->count2 = 0; /* failure means cancel operation */

      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", cmd);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      ftp_state(दत्त, ftpc, instate);
      quote = TRUE;
    }
  }

  यदि(!quote) {
    /* No more quote to send, continue to ... */
    विकल्प(instate) {
    पक्ष FTP_QUOTE:
    स्वभाव:
      परिणाम = ftp_state_cwd(दत्त, ftpc, ftp);
      भङ्ग;
    पक्ष FTP_RETR_PREQUOTE:
      यदि(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(दत्त, ftpc, FTP_STOP);
      अन्यथा {
        यदि(ftpc->ज्ञातम्_filesize != -1) {
          Curl_pgrs_समुच्चयः_Download_आकार(दत्त, ftpc->ज्ञातम्_filesize);
          परिणाम = ftp_state_retr(दत्त, ftpc, ftp, ftpc->ज्ञातम्_filesize);
        }
        अन्यथा {
          यदि(दत्त->समुच्चयः.ignorecl || दत्त->state.prefer_ascii) {
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
            परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "RETR %s", ftpc->file);
            यदि(!परिणाम)
              ftp_state(दत्त, ftpc, FTP_RETR);
          }
          अन्यथा {
            परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "SIZE %s", ftpc->file);
            यदि(!परिणाम)
              ftp_state(दत्त, ftpc, FTP_RETR_आकार);
          }
        }
      }
      भङ्ग;
    पक्ष FTP_STOR_PREQUOTE:
      परिणाम = ftp_state_ul_सज्जता(दत्त, ftpc, ftp, FALSE);
      भङ्ग;
    पक्ष FTP_POSTQUOTE:
      भङ्ग;
    पक्ष FTP_LIST_PREQUOTE:
      ftp_state(दत्त, ftpc, FTP_LIST_TYPE);
      परिणाम = ftp_state_list(दत्त, ftpc, ftp);
      भङ्ग;
    }
  }

  प्रत्यागम परिणाम;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
स्थिर CURLcode ftp_epsv_disable(संरचना Curl_easy *दत्त,
                                 संरचना ftp_conn *ftpc,
                                 संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(conn->द्वयङ्काः.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(दत्त, "Failed EPSV attempt, exiting");
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  infof(दत्त, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->द्वयङ्काः.ftp_use_epsv = FALSE;
  close_secondarysocket(दत्त, ftpc);
  दत्त->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "PASV");
  यदि(!परिणाम) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(दत्त, ftpc, FTP_PASV);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_control_addr_dup(संरचना Curl_easy *दत्त, वर्ण **newhostp)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  अन्यथा
#यदि_अन्तः
  यदि(!Curl_conn_get_ip_info(दत्त, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  अन्यथा {
    /* failed to get the remote_ip of the DATA connection */
    failf(दत्त, "unable to get peername of DATA connection");
    *newhostp = रिक्त;
    प्रत्यागम CURLE_FTP_CANT_GET_HOST;
  }
  प्रत्यागम *newhostp ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

स्थिर bool मेलनम्_pasv_6nums(ध्रुव वर्ण *p,
                             अचिह्नित पूर्णाङ्क *array) /* 6 numbers */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 6; i++) {
    curl_off_t अङ्कः;
    यदि(i) {
      यदि(*p != ',')
        प्रत्यागम FALSE;
      p++;
    }
    यदि(curlx_str_सङ्ख्या(&p, &अङ्कः, 0xff))
      प्रत्यागम FALSE;
    array[i] = (अचिह्नित पूर्णाङ्क)अङ्कः;
  }
  प्रत्यागम TRUE;
}

स्थिर CURLcode ftp_state_pasv_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम;
  ध्रुव संरचना pingpong *pp = &ftpc->pp;
  वर्ण *newhost = रिक्त;
  अचिह्नित ह्रस्व newport = 0;
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
        curl_off_t अङ्कः;
        यदि(curlx_str_सङ्ख्या(&p, &अङ्कः, 0xffff) || (*p != sep)) {
          failf(दत्त, "Illegal port number in EPSV reply");
          प्रत्यागम CURLE_FTP_WEIRD_PASV_प्रत्युत्तरम्;
        }
        newport = (अचिह्नित ह्रस्व)अङ्कः;
        परिणाम = ftp_control_addr_dup(दत्त, &newhost);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
      अन्यथा
        ptr = रिक्त;
    }
    यदि(!ptr) {
      failf(दत्त, "Weirdly formatted EPSV reply");
      प्रत्यागम CURLE_FTP_WEIRD_PASV_प्रत्युत्तरम्;
    }
  }
  अन्यथा यदि((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    अचिह्नित पूर्णाङ्क ip[6];

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
      यदि(मेलनम्_pasv_6nums(str, ip))
        भङ्ग;
      str++;
    }

    यदि(!*str) {
      failf(दत्त, "Could not interpret the 227-response");
      प्रत्यागम CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    यदि(दत्त->समुच्चयः.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(दत्त, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      परिणाम = ftp_control_addr_dup(दत्त, &newhost);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    यदि(!newhost)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    newport = (अचिह्नित ह्रस्व)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  अन्यथा यदि(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    प्रत्यागम ftp_epsv_disable(दत्त, ftpc, conn);
  }
  अन्यथा {
    failf(दत्त, "Bad PASV/EPSV response: %03d", ftpcode);
    प्रत्यागम CURLE_FTP_WEIRD_PASV_प्रत्युत्तरम्;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  परिणाम = Curl_peer_create(दत्त, conn->scheme, newhost, newport,
                            &conn->origin2);
  यदि(परिणाम)
    गच्छ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  यदि(conn->द्वारा_peer) {
    Curl_peer_unlink(&conn->द्वारा_peer2);
    परिणाम = Curl_peer_create(दत्त, conn->द्वारा_peer->scheme,
                              conn->द्वारा_peer->hostname, newport,
                              &conn->द्वारा_peer2);
    यदि(परिणाम)
      गच्छ error;
  }

  परिणाम = Curl_conn_सज्जता(दत्त, conn, SECONDARYSOCKET,
                           conn->द्वयङ्काः.ftp_use_दत्त_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  यदि(परिणाम) {
    यदि((परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      परिणाम = ftp_epsv_disable(दत्त, ftpc, conn);
    }
    गच्छ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#यदि_परिभाषितम् CURLVERBOSE
  यदि(दत्त->समुच्चयः.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(दत्त, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->पत्तनम्);
  }
#यदि_अन्तः

  conn->द्वयङ्काः.do_अधिकम् = TRUE;
  ftp_state(दत्त, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  प्रत्यागम परिणाम;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ftp_statemach(संरचना Curl_easy *दत्त,
                              संरचना ftp_conn *ftpc,
                              bool *समाप्तम्)
{
  CURLcode परिणाम = Curl_pp_statemach(दत्त, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *समाप्तम् = (ftpc->state == FTP_STOP);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_do_अधिकम्(संरचना Curl_easy *दत्त, domore *अधिकम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(दत्त, CURL_META_FTP_EASY);
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;

  *अधिकम् = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  यदि(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(दत्त, SECONDARYSOCKET);
    परिणाम = Curl_conn_connect(दत्त, SECONDARYSOCKET, FALSE, &connected);
    यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      प्रत्यागम परिणाम;
    यदि(परिणाम || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(दत्त, SECONDARYSOCKET))) {
      यदि(परिणाम && !is_eptr && (ftpc->count1 == 0)) {
        *अधिकम् = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        प्रत्यागम ftp_epsv_disable(दत्त, ftpc, conn);
      }
      प्रत्यागम परिणाम;
    }
  }

  यदि(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    परिणाम = ftp_statemach(दत्त, ftpc, &complete);

    यदि(complete)
      *अधिकम् = DOMORE_समाप्तम्;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    यदि(परिणाम || !ftpc->wait_दत्त_conn)
      प्रत्यागम परिणाम;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *अधिकम् = DOMORE_INCOMPLETE;
  }

  यदि(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    यदि(ftpc->wait_दत्त_conn) {
      bool serv_conned;

      परिणाम = Curl_conn_connect(दत्त, SECONDARYSOCKET, FALSE, &serv_conned);
      यदि(परिणाम)
        प्रत्यागम परिणाम; /* Failed to accept data connection */

      यदि(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_दत्त_conn = FALSE;
        परिणाम = ftp_initiate_transfer(दत्त, ftpc);

        यदि(परिणाम)
          प्रत्यागम परिणाम;

        *अधिकम् = DOMORE_समाप्तम्; /* this state is now complete when the server has
                                connected back to us */
      }
      अन्यथा {
        परिणाम = ftp_check_ctrl_on_दत्त_wait(दत्त, ftpc);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
    अन्यथा यदि(दत्त->state.upload) {
      परिणाम = ftp_nb_type(दत्त, ftpc, ftp, (bool)दत्त->state.prefer_ascii,
                           FTP_STOR_TYPE);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      परिणाम = ftp_statemach(दत्त, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *अधिकम् = (!ftpc->wait_दत्त_conn && complete) ?
        DOMORE_समाप्तम् : DOMORE_INCOMPLETE;
    }
    अन्यथा {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      परिणाम = Curl_range(दत्त);

      यदि(परिणाम == CURLE_OK && दत्त->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      यदि(परिणाम)
        प्रत्यागम परिणाम;

      यदि((दत्त->state.list_only || !ftpc->file) &&
         !दत्त->समुच्चयः.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        यदि(ftp->transfer == PPTRANSFER_BODY) {
          परिणाम = ftp_nb_type(दत्त, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          यदि(परिणाम)
            प्रत्यागम परिणाम;
        }
        /* otherwise fall through */
      }
      अन्यथा {
        यदि(दत्त->समुच्चयः.prequote && !ftpc->file) {
          परिणाम = ftp_nb_type(दत्त, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        अन्यथा {
          परिणाम = ftp_nb_type(दत्त, ftpc, ftp, (bool)दत्त->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }

      परिणाम = ftp_statemach(दत्त, ftpc, &complete);
      यदि(complete)
        *अधिकम् = DOMORE_समाप्तम्;
    }
    प्रत्यागम परिणाम;
  }

  /* no data to transfer */
  Curl_xfer_सज्जता_nop(दत्त);

  यदि(!ftpc->wait_दत्त_conn) {
    /* no waiting for the data connection so this is now complete */
    *अधिकम् = DOMORE_समाप्तम्;
    CURL_TRC_FTP(दत्त, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (पूर्णाङ्क)परिणाम);
  }

  प्रत्यागम परिणाम;
}

/* call this when the DO phase has completed */
स्थिर CURLcode ftp_dophase_समाप्तम्(संरचना Curl_easy *दत्त,
                                 संरचना ftp_conn *ftpc,
                                 संरचना FTP *ftp,
                                 bool connected)
{
  यदि(connected) {
    domore completed;
    CURLcode परिणाम = ftp_do_अधिकम्(दत्त, &completed);

    यदि(परिणाम) {
      close_secondarysocket(दत्त, ftpc);
      प्रत्यागम परिणाम;
    }
  }

  यदि(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_सज्जता_nop(दत्त);
  अन्यथा यदि(!connected)
    /* since we did not connect now, we want do_more to get called */
    दत्त->conn->द्वयङ्काः.do_अधिकम् = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ftp_state_पत्तनम्_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode)
{
  संरचना connectdata *conn = दत्त->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode परिणाम = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  यदि(ftpcode / 100 != 2) {
    /* the command failed */

    यदि(EPRT == fcmd) {
      infof(दत्त, "disabling EPRT usage");
      conn->द्वयङ्काः.ftp_use_eprt = FALSE;
    }
    fcmd++;

    यदि(fcmd == समाप्तम्_2) {
      failf(दत्त, "Failed to do PORT");
      परिणाम = CURLE_FTP_पत्तनम्_FAILED;
    }
    अन्यथा
      /* try next */
      परिणाम = ftp_state_use_पत्तनम्(दत्त, ftpc, fcmd);
  }
  अन्यथा {
    infof(दत्त, "Connect data stream actively");
    ftp_state(दत्त, ftpc, FTP_STOP); /* end of DO phase */
    परिणाम = ftp_dophase_समाप्तम्(दत्त, ftpc, ftp, FALSE);
  }

  प्रत्यागम परिणाम;
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
  यदि((वाक्यदैर्घ्यम्(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
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

स्थिर CURLcode client_write_शीर्षकम्(संरचना Curl_easy *दत्त,
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
  CURLcode परिणाम;
  bool save = (bool)दत्त->समुच्चयः.include_शीर्षकम्;
  दत्त->समुच्चयः.include_शीर्षकम् = TRUE;
  परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, buf, blen);
  दत्त->समुच्चयः.include_शीर्षकम् = save;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_mdtm_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

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
      यदि(!Curl_getdate_capped(timebuf, &दत्त->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#यदि defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#निर्देशः GCC diagnostic प्रक्षिप
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#निर्देशः GCC diagnostic ignored "-Wtype-limits"
#यदि_अन्तः
    यदि(दत्त->req.no_body && ftpc->file &&
       दत्त->समुच्चयः.get_filetime && showtime) {
#यदि defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
      वर्ण headerbuf[128];
      पूर्णाङ्क headerbuflen;
      time_t filetime = दत्त->info.filetime;
      संरचना tm बफर;
      ध्रुव संरचना tm *tm = &बफर;

      परिणाम = curlx_gmtime(filetime, &बफर);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

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
      परिणाम = client_write_शीर्षकम्(दत्त, headerbuf, headerbuflen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    } /* end of a ridiculous amount of conditionals */
  }
    भङ्ग;
  स्वभाव:
    infof(दत्त, "unsupported MDTM reply format");
    भङ्ग;
  पक्ष 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(दत्त, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    भङ्ग;
  }

  यदि(दत्त->समुच्चयः.timecondition) {
    यदि((दत्त->info.filetime > 0) && (दत्त->समुच्चयः.timevalue > 0)) {
      विकल्प(दत्त->समुच्चयः.timecondition) {
      पक्ष CURL_TIMECOND_IFMODSINCE:
      स्वभाव:
        यदि(दत्त->info.filetime <= दत्त->समुच्चयः.timevalue) {
          infof(दत्त, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          दत्त->info.timecond = TRUE;
          ftp_state(दत्त, ftpc, FTP_STOP);
          प्रत्यागम CURLE_OK;
        }
        भङ्ग;
      पक्ष CURL_TIMECOND_IFUNMODSINCE:
        यदि(दत्त->info.filetime > दत्त->समुच्चयः.timevalue) {
          infof(दत्त, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          दत्त->info.timecond = TRUE;
          ftp_state(दत्त, ftpc, FTP_STOP);
          प्रत्यागम CURLE_OK;
        }
        भङ्ग;
      } /* switch */
    }
    अन्यथा {
      infof(दत्त, "Skipping time comparison");
    }
  }

  यदि(!परिणाम)
    परिणाम = ftp_state_type(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_type_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(दत्त, "Could not set desired mode");
    प्रत्यागम CURLE_FTP_COULDNT_समुच्चयः_TYPE;
  }
  यदि(ftpcode != 200)
    infof(दत्त, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  यदि(instate == FTP_TYPE)
    परिणाम = ftp_state_आकार(दत्त, ftpc, ftp);
  अन्यथा यदि(instate == FTP_LIST_TYPE)
    परिणाम = ftp_state_list(दत्त, ftpc, ftp);
  अन्यथा यदि(instate == FTP_RETR_TYPE)
    परिणाम = ftp_state_retr_prequote(दत्त, ftpc, ftp);
  अन्यथा यदि(instate == FTP_STOR_TYPE)
    परिणाम = ftp_state_stor_prequote(दत्त, ftpc, ftp);
  अन्यथा यदि(instate == FTP_RETR_LIST_TYPE)
    परिणाम = ftp_state_list_prequote(दत्त, ftpc, ftp);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_आकार_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
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
    यदि(curlx_str_सङ्ख्या(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  अन्यथा यदि(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_आकार) {
    failf(दत्त, "The file does not exist");
    प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;
  }

  यदि(instate == FTP_आकार) {
    यदि(filesize != -1) {
      वर्ण clbuf[128];
      पूर्णाङ्क clbuflen = curl_msnprintf(clbuf, परिमाण(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      परिणाम = client_write_शीर्षकम्(दत्त, clbuf, clbuflen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, filesize);
    परिणाम = ftp_state_rest(दत्त, ftpc, ftp);
  }
  अन्यथा यदि(instate == FTP_RETR_आकार) {
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, filesize);
    परिणाम = ftp_state_retr(दत्त, ftpc, ftp, filesize);
  }
  अन्यथा यदि(instate == FTP_STOR_आकार) {
    दत्त->state.resume_from = filesize;
    परिणाम = ftp_state_ul_सज्जता(दत्त, ftpc, ftp, TRUE);
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_rest_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    संरचना FTP *ftp,
                                    पूर्णाङ्क ftpcode,
                                    ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  विकल्प(instate) {
  पक्ष FTP_REST:
  स्वभाव:
    यदि(ftpcode == 350) {
      वर्ण बफर[24] = { "Accept-ranges: bytes\r\n" };
      परिणाम = client_write_शीर्षकम्(दत्त, बफर, वाक्यदैर्घ्यम्(बफर));
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    परिणाम = ftp_state_prepare_transfer(दत्त, ftpc, ftp);
    भङ्ग;

  पक्ष FTP_RETR_REST:
    यदि(ftpcode != 350) {
      failf(दत्त, "Could not use REST");
      परिणाम = CURLE_FTP_COULDNT_USE_REST;
    }
    अन्यथा {
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "RETR %s", ftpc->file);
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_RETR);
    }
    भङ्ग;
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_state_stor_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(ftpcode >= 400) {
    failf(दत्त, "Failed FTP upload: %0d", ftpcode);
    ftp_state(दत्त, ftpc, FTP_STOP);
    प्रत्यागम CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  यदि(दत्त->समुच्चयः.ftp_use_पत्तनम्) {
    bool connected;

    ftp_state(दत्त, ftpc, FTP_STOP); /* no longer in STOR state */

    परिणाम = Curl_conn_connect(दत्त, SECONDARYSOCKET, FALSE, &connected);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि(!connected) {
      infof(दत्त, "Data conn was not available immediately");
      ftpc->wait_दत्त_conn = TRUE;
      प्रत्यागम ftp_check_ctrl_on_दत्त_wait(दत्त, ftpc);
    }
    ftpc->wait_दत्त_conn = FALSE;
  }
  प्रत्यागम ftp_initiate_transfer(दत्त, ftpc);
}

/* for LIST and RETR responses */
स्थिर CURLcode ftp_state_get_resp(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc,
                                   संरचना FTP *ftp,
                                   पूर्णाङ्क ftpcode,
                                   ftpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

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

    दत्त->req.आकार = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    यदि((instate != FTP_LIST) &&
       !दत्त->state.prefer_ascii &&
       !दत्त->समुच्चयः.ignorecl &&
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
          यदि(!curlx_str_सङ्ख्या(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !वाक्ये_तुलय(c, "bytes", 5)) {
            दत्त->req.आकार = what;
            भङ्ग;
          }
        }
      }
    }
    अन्यथा यदि(ftp->downloadsize > -1)
      दत्त->req.आकार = ftp->downloadsize;

    यदि(दत्त->req.आकार > दत्त->req.maxdownload && दत्त->req.maxdownload > 0)
      दत्त->req.आकार = दत्त->req.maxdownload;
    अन्यथा यदि((instate != FTP_LIST) && दत्त->state.prefer_ascii)
      दत्त->req.आकार = -1; /* for servers that understate ASCII mode file
आकार*/

    infof(दत्त, "Maxdownload = %" FMT_OFF_T, दत्त->req.maxdownload);

    यदि(instate != FTP_LIST)
      infof(दत्त, "Getting file with size: %" FMT_OFF_T, दत्त->req.आकार);

    यदि(दत्त->समुच्चयः.ftp_use_पत्तनम्) {
      bool connected;

      परिणाम = Curl_conn_connect(दत्त, SECONDARYSOCKET, FALSE, &connected);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      यदि(!connected) {
        infof(दत्त, "Data conn was not available immediately");
        ftp_state(दत्त, ftpc, FTP_STOP);
        ftpc->wait_दत्त_conn = TRUE;
        प्रत्यागम ftp_check_ctrl_on_दत्त_wait(दत्त, ftpc);
      }
      ftpc->wait_दत्त_conn = FALSE;
    }
    प्रत्यागम ftp_initiate_transfer(दत्त, ftpc);
  }
  अन्यथा {
    यदि((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(दत्त, ftpc, FTP_STOP); /* this phase is over */
    }
    अन्यथा {
      failf(दत्त, "RETR response: %03d", ftpcode);
      प्रत्यागम instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_प्राप्तम् :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  प्रत्यागम परिणाम;
}

/* after USER, PASS and ACCT */
स्थिर CURLcode ftp_state_loggedin(संरचना Curl_easy *दत्त,
                                   संरचना ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET)) {
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
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "PBSZ %d", 0);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_PBSZ);
  }
  अन्यथा {
    परिणाम = ftp_state_pwd(दत्त, ftpc);
  }
  प्रत्यागम परिणाम;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
स्थिर bool ftp_has_ctrl(ध्रुव वर्ण *string)
{
  ध्रुव अचिह्नित वर्ण *s = (ध्रुव अचिह्नित वर्ण *)string;
  यावत्(*s) {
    यदि(*s < 0x20)
      प्रत्यागम TRUE;
    s++;
  }
  प्रत्यागम FALSE;
}

/* for USER and PASS responses */
स्थिर CURLcode ftp_state_user_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode परिणाम = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  यदि((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(दत्त->conn->creds));
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_PASS);
  }
  अन्यथा यदि(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    परिणाम = ftp_state_loggedin(दत्त, ftpc);
  }
  अन्यथा यदि(ftpcode == 332) {
    ध्रुव वर्ण *account = CURL_EASY_STR(दत्त, STRING_FTP_ACCOUNT);
    यदि(!account) {
      failf(दत्त, "ACCT requested but none available");
      परिणाम = CURLE_LOGIN_DENIED;
    }
    अन्यथा यदि(ftp_has_ctrl(account)) {
      failf(दत्त, "Control byte in FTP account");
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    }
    अन्यथा {
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "ACCT %s", account);
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_ACCT);
    }
  }
  अन्यथा {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ध्रुव वर्ण *alt = CURL_EASY_STR(दत्त, STRING_FTP_ALTERNATIVE_गन्तव्यम्_USER);
    यदि(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      यदि(ftp_has_ctrl(alt)) {
        failf(दत्त, "Control byte in FTP alternative-to-user command");
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      }
      अन्यथा {
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", alt);
        यदि(!परिणाम) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(दत्त, ftpc, FTP_USER);
        }
      }
    }
    अन्यथा {
      failf(दत्त, "Access denied: %03d", ftpcode);
      परिणाम = CURLE_LOGIN_DENIED;
    }
  }
  प्रत्यागम परिणाम;
}

/* for ACCT response */
स्थिर CURLcode ftp_state_acct_resp(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc,
                                    पूर्णाङ्क ftpcode)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(ftpcode != 230) {
    failf(दत्त, "ACCT rejected by server: %03d", ftpcode);
    परिणाम = CURLE_FTP_WEIRD_PASS_प्रत्युत्तरम्; /* FIX */
  }
  अन्यथा
    परिणाम = ftp_state_loggedin(दत्त, ftpc);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_pwd_resp(संरचना Curl_easy *दत्त,
                             संरचना ftp_conn *ftpc,
                             पूर्णाङ्क ftpcode)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode परिणाम;

  यदि(ftpcode == 257) {
    ध्रुव वर्ण *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool प्रविष्टिः_extracted = FALSE;
    संरचना dynbuf बहिर्गमनम्;
    curlx_dyn_init(&बहिर्गमनम्, 1000);

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
            परिणाम = curlx_dyn_addn(&बहिर्गमनम्, &ptr[1], 1);
            ptr++;
          }
          अन्यथा {
            /* end of path */
            यदि(curlx_dyn_len(&बहिर्गमनम्))
              प्रविष्टिः_extracted = TRUE;
            भङ्ग; /* get out of this loop */
          }
        }
        अन्यथा {
          यदि(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&बहिर्गमनम्);
            प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
          }
          परिणाम = curlx_dyn_addn(&बहिर्गमनम्, ptr, 1);
        }
        यदि(परिणाम) {
          curlx_dyn_free(&बहिर्गमनम्);
          प्रत्यागम परिणाम;
        }
      }
    }
    यदि(प्रविष्टिः_extracted) {
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
      वर्ण *dir = curlx_dyn_ptr(&बहिर्गमनम्);

      यदि(!ftpc->server_os && dir[0] != '/') {
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "SYST");
        यदि(परिणाम) {
          curlx_dyn_free(&बहिर्गमनम्);
          प्रत्यागम परिणाम;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(दत्त, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(दत्त->state.most_recent_ftp_entrypath);
      दत्त->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      यदि(!दत्त->state.most_recent_ftp_entrypath)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      यदि(!ftpc->server_os && dir[0] != '/') {
        ftp_state(दत्त, ftpc, FTP_SYST);
        प्रत्यागम CURLE_OK;
      }
    }
    अन्यथा {
      /* could not get the path */
      curlx_dyn_free(&बहिर्गमनम्);
      infof(दत्त, "Failed to figure out path");
    }
  }
  ftp_state(दत्त, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(दत्त, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव वर्ण * ध्रुव ftpauth[] = { "SSL", "TLS" };

स्थिर CURLcode ftp_wait_resp(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn,
                              संरचना ftp_conn *ftpc,
                              पूर्णाङ्क ftpcode)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    यदि(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      प्रत्यागम ftp_state_user_resp(दत्त, ftpc, ftpcode);
  }
  अन्यथा यदि(ftpcode != 220) {
    failf(दत्त, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  यदि(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    विकल्प((दीर्घ)दत्त->समुच्चयः.ftpsslauth) {
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
      failf(दत्त, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (पूर्णाङ्क)दत्त->समुच्चयः.ftpsslauth);
      प्रत्यागम CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_AUTH);
  }
  अन्यथा
    परिणाम = ftp_state_user(दत्त, ftpc, conn);
  प्रत्यागम परिणाम;
}

/* an implicit TLS scheme implies TLS on both connections */
स्थिर अचिह्नित वर्ण ftp_use_ssl(संरचना connectdata *conn,
                                 संरचना ftp_conn *ftpc)
{
  यदि(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    प्रत्यागम CURLUSESSL_ALL;
  प्रत्यागम ftpc->use_ssl;
}

स्थिर CURLcode ftp_pp_statemachine(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn)
{
  CURLcode परिणाम;
  पूर्णाङ्क ftpcode;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(दत्त, CURL_META_FTP_EASY);
  संरचना pingpong *pp;
  size_t nread = 0;

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  यदि(pp->sendleft)
    प्रत्यागम Curl_pp_flushsend(दत्त, pp);

  परिणाम = ftp_readresp(दत्त, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  यदि(परिणाम || !ftpcode)
    प्रत्यागम परिणाम;

  /* we have now received a full FTP server response */
  विकल्प(ftpc->state) {
  पक्ष FTP_WAIT220:
    परिणाम = ftp_wait_resp(दत्त, conn, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    यदि(pp->अतिप्रवाहः)
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    यदि((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool समाप्तम्;
      यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        परिणाम = Curl_ssl_cfilter_सङ्कलय(
          दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        यदि(परिणाम) {
          /* we failed and bail out */
          प्रत्यागम CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, TRUE, &समाप्तम्);
      यदि(!परिणाम) {
        conn->द्वयङ्काः.ftp_use_दत्त_ssl = FALSE; /* clear-text data */
        परिणाम = ftp_state_user(दत्त, ftpc, conn);
      }
    }
    अन्यथा यदि(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    अन्यथा {
      यदि(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        परिणाम = CURLE_USE_SSL_FAILED;
      अन्यथा
        /* ignore the failure and continue */
        परिणाम = ftp_state_user(दत्त, ftpc, conn);
    }
    भङ्ग;

  पक्ष FTP_USER:
  पक्ष FTP_PASS:
    परिणाम = ftp_state_user_resp(दत्त, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_ACCT:
    परिणाम = ftp_state_acct_resp(दत्त, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_PBSZ:
    परिणाम =
      Curl_pp_sendf(दत्त, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    यदि(!परिणाम)
      ftp_state(दत्त, ftpc, FTP_PROT);
    भङ्ग;

  पक्ष FTP_PROT:
    यदि(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->द्वयङ्काः.ftp_use_दत्त_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    अन्यथा यदि(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      प्रत्यागम CURLE_USE_SSL_FAILED;

    यदि(दत्त->समुच्चयः.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "CCC");
      यदि(!परिणाम)
        ftp_state(दत्त, ftpc, FTP_CCC);
    }
    अन्यथा
      परिणाम = ftp_state_pwd(दत्त, ftpc);
    भङ्ग;

  पक्ष FTP_CCC:
    यदि(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      परिणाम = Curl_ssl_cfilter_remove(दत्त, FIRSTSOCKET,
                                       (दत्त->समुच्चयः.ftp_ccc ==
                                        (अचिह्नित वर्ण)CURLFTPSSL_CCC_सक्रिय));
      यदि(परिणाम)
        failf(दत्त, "Failed to clear the command channel (CCC)");
    }
    यदि(!परिणाम)
      /* Then continue as normal */
      परिणाम = ftp_state_pwd(दत्त, ftpc);
    भङ्ग;

  पक्ष FTP_PWD:
    परिणाम = ftp_pwd_resp(दत्त, ftpc, ftpcode);
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
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      /* Check for special servers here. */
      यदि(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        यदि(परिणाम) {
          curlx_free(os);
          प्रत्यागम परिणाम;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(दत्त, ftpc, FTP_NAMEFMT);
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

    ftp_state(दत्त, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(दत्त, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    भङ्ग;

  पक्ष FTP_NAMEFMT:
    यदि(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(दत्त, ftpc);
      भङ्ग;
    }

    ftp_state(दत्त, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(दत्त, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    भङ्ग;

  पक्ष FTP_QUOTE:
  पक्ष FTP_POSTQUOTE:
  पक्ष FTP_RETR_PREQUOTE:
  पक्ष FTP_STOR_PREQUOTE:
  पक्ष FTP_LIST_PREQUOTE:
    यदि((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(दत्त, "QUOT command failed with %03d", ftpcode);
      परिणाम = CURLE_QUOTE_ERROR;
    }
    अन्यथा
      परिणाम = ftp_state_quote(दत्त, ftpc, ftp, FALSE, ftpc->state);
    भङ्ग;

  पक्ष FTP_CWD:
    यदि(ftpcode / 100 != 2) {
      /* failure to CWD there */
      यदि(दत्त->समुच्चयः.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (दत्त->समुच्चयः.ftp_create_missing_dirs == 2) ? 1 : 0;

        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        यदि(!परिणाम)
          ftp_state(दत्त, ftpc, FTP_MKD);
      }
      अन्यथा {
        /* return failure */
        failf(दत्त, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        परिणाम = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    अन्यथा {
      /* success */
      ftpc->count2 = 0;
      यदि(ftpc->cwdcount >= ftpc->dirdepth)
        परिणाम = ftp_state_mdtm(दत्त, ftpc, ftp);
      अन्यथा {
        ftpc->cwdcount++;
        /* send next CWD */
        परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    भङ्ग;

  पक्ष FTP_MKD:
    यदि((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(दत्त, "Failed to MKD dir: %03d", ftpcode);
      परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    }
    अन्यथा {
      ftp_state(दत्त, ftpc, FTP_CWD);
      /* send CWD */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    भङ्ग;

  पक्ष FTP_MDTM:
    परिणाम = ftp_state_mdtm_resp(दत्त, ftpc, ftp, ftpcode);
    भङ्ग;

  पक्ष FTP_TYPE:
  पक्ष FTP_LIST_TYPE:
  पक्ष FTP_RETR_TYPE:
  पक्ष FTP_STOR_TYPE:
  पक्ष FTP_RETR_LIST_TYPE:
    परिणाम = ftp_state_type_resp(दत्त, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_आकार:
  पक्ष FTP_RETR_आकार:
  पक्ष FTP_STOR_आकार:
    परिणाम = ftp_state_आकार_resp(दत्त, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_REST:
  पक्ष FTP_RETR_REST:
    परिणाम = ftp_state_rest_resp(दत्त, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_PRET:
    यदि(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(दत्त, "PRET command not accepted: %03d", ftpcode);
      प्रत्यागम CURLE_FTP_PRET_FAILED;
    }
    परिणाम = ftp_state_use_pasv(दत्त, ftpc, conn);
    भङ्ग;

  पक्ष FTP_PASV:
    परिणाम = ftp_state_pasv_resp(दत्त, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_पत्तनम्:
    परिणाम = ftp_state_पत्तनम्_resp(दत्त, ftpc, ftp, ftpcode);
    भङ्ग;

  पक्ष FTP_LIST:
  पक्ष FTP_RETR:
    परिणाम = ftp_state_get_resp(दत्त, ftpc, ftp, ftpcode, ftpc->state);
    भङ्ग;

  पक्ष FTP_STOR:
    परिणाम = ftp_state_stor_resp(दत्त, ftpc, ftpcode);
    भङ्ग;

  पक्ष FTP_QUIT:
  स्वभाव:
    /* internal error */
    ftp_state(दत्त, ftpc, FTP_STOP);
    भङ्ग;
  }

  प्रत्यागम परिणाम;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ftp_multi_statemach(संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  प्रत्यागम ftpc ? ftp_statemach(दत्त, ftpc, समाप्तम्) : CURLE_FAILED_INIT;
}

स्थिर CURLcode ftp_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                    संरचना ftp_conn *ftpc)
{
  संरचना pingpong *pp = &ftpc->pp;
  CURLcode परिणाम = CURLE_OK;

  यावत्(ftpc->state != FTP_STOP) {
    यदि(ftpc->shutdown)
      CURL_TRC_FTP(दत्त, "in shutdown, waiting for server response");
    परिणाम = Curl_pp_statemach(दत्त, pp, TRUE, TRUE /* disconnecting */);
    यदि(परिणाम)
      भङ्ग;
  }

  प्रत्यागम परिणाम;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
स्थिर CURLcode ftp_connect(संरचना Curl_easy *दत्त,
                            bool *समाप्तम्) /* see description above */
{
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  संरचना pingpong *pp;

  *समाप्तम् = FALSE; /* default to not done yet */
  यदि(!ftpc)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_सज्जता(pp, ftp_pp_statemachine, ftp_endofresp);

  यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, TRUE, समाप्तम्);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  Curl_pp_init(pp, Curl_pgrs_now(दत्त)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(दत्त, ftpc, FTP_WAIT220);

  परिणाम = ftp_statemach(दत्त, ftpc, समाप्तम्);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_sendquote(संरचना Curl_easy *दत्त,
                              संरचना ftp_conn *ftpc,
                              संरचना curl_slist *quote)
{
  संरचना curl_slist *item;
  संरचना pingpong *pp = &ftpc->pp;

  item = quote;
  यावत्(item) {
    यदि(item->दत्त) {
      size_t nread;
      ध्रुव वर्ण *cmd = item->दत्त;
      bool acceptfail = FALSE;
      CURLcode परिणाम;
      पूर्णाङ्क ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      यदि(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", cmd);
      यदि(!परिणाम) {
        pp->response = *Curl_pgrs_now(दत्त); /* timeout relative now */
        परिणाम = getftpresponse(दत्त, &nread, &ftpcode);
      }
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      यदि(!acceptfail && (ftpcode >= 400)) {
        failf(दत्त, "QUOT string not accepted: %s", cmd);
        प्रत्यागम CURLE_QUOTE_ERROR;
      }
    }

    item = item->अग्र;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ftp_समाप्तम्_स्थितिः(संरचना Curl_easy *दत्त,
                                संरचना ftp_conn *ftpc, CURLcode स्थितिः,
                                bool premature)
{
  विकल्प(स्थितिः) {
  पक्ष CURLE_BAD_DOWNLOAD_RESUME:
  पक्ष CURLE_FTP_WEIRD_PASV_प्रत्युत्तरम्:
  पक्ष CURLE_FTP_पत्तनम्_FAILED:
  पक्ष CURLE_FTP_ACCEPT_FAILED:
  पक्ष CURLE_FTP_ACCEPT_TIMEOUT:
  पक्ष CURLE_FTP_COULDNT_समुच्चयः_TYPE:
  पक्ष CURLE_FTP_COULDNT_RETR_FILE:
  पक्ष CURLE_PARTIAL_FILE:
  पक्ष CURLE_UPLOAD_FAILED:
  पक्ष CURLE_REMOTE_ACCESS_DENIED:
  पक्ष CURLE_FILESIZE_EXCEEDED:
  पक्ष CURLE_REMOTE_FILE_NOT_प्राप्तम्:
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
    CURL_TRC_FTP(दत्त, "FTP ended with bad error code");
    connclose(दत्त->conn);
    प्रत्यागम स्थितिः;      /* use the already set error code */
  }
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ftp_समाप्तम्_wildcard(संरचना Curl_easy *दत्त, संरचना ftp_conn *ftpc)
{
  यदि(दत्त->state.wildcardmatch) {
    यदि(दत्त->समुच्चयः.chunk_अन्तः && ftpc->file) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_chunk_अन्तः);
      दत्त->समुच्चयः.chunk_अन्तः(दत्त->समुच्चयः.wildcardptr);
      CURL_CBAPI_अन्तः(&guard);
      freedirs(ftpc);
    }
    ftpc->ज्ञातम्_filesize = -1;
  }
}

स्थिर शून्य ftp_समाप्तम्_मार्गः(संरचना Curl_easy *दत्त, संरचना ftp_conn *ftpc,
                          CURLcode परिणाम)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(परिणाम) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  अन्यथा { /* remember working directory for connection reuse */
    ध्रुव वर्ण *raw_मार्गः = ftpc->rawpath;
    यदि(raw_मार्गः) {
      यदि((दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD) && (raw_मार्गः[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      अन्यथा {
        size_t मार्गः_Len = वाक्यदैर्घ्यम्(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        यदि(!ftpc->cwdfail) {
          यदि(दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD)
            मार्गः_Len = 0; /* relative path => working directory is FTP home */
          अन्यथा
            /* file is URL-decoded */
            मार्गः_Len -= ftpc->file ? वाक्यदैर्घ्यम्(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_मार्गः, मार्गः_Len);
        }
        अन्यथा
          ftpc->prevpath = रिक्त; /* no path */
      }
    }
    यदि(ftpc->prevpath)
      infof(दत्त, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

स्थिर CURLcode ftp_समाप्तम्_secondary_socket(संरचना Curl_easy *दत्त,
                                          संरचना ftp_conn *ftpc,
                                          CURLcode परिणाम)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(Curl_conn_is_सज्जता(conn, SECONDARYSOCKET)) {
    यदि(!परिणाम && ftpc->dont_check && दत्त->req.maxdownload > 0) {
      /* partial download completed */
      परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "ABOR");
      यदि(परिणाम) {
        failf(दत्त, "Failure sending ABOR command: %s",
              curl_easy_strerror(परिणाम));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(दत्त, ftpc);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_समाप्तम्_control_प्रत्युत्तरम्(संरचना Curl_easy *दत्त,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode परिणाम,
                                       bool premature)
{
  संरचना connectdata *conn = दत्त->conn;
  size_t nread;
  पूर्णाङ्क ftpcode;

  यदि(!परिणाम && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(दत्त); /* timeout relative now */
    परिणाम = getftpresponse(दत्त, &nread, &ftpcode);

    यदि(!nread && (परिणाम == CURLE_OPERATION_TIMEDOUT)) {
      failf(दत्त, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि(ftpc->dont_check && दत्त->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(दत्त, "partial download completed, closing connection");
      connclose(conn);
      प्रत्यागम परिणाम;
    }

    यदि(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      विकल्प(ftpcode) {
      पक्ष 226:
      पक्ष 250:
        भङ्ग;
      पक्ष 552:
        failf(दत्त, "Exceeded storage allocation");
        परिणाम = CURLE_REMOTE_DISK_पूर्णम्;
        भङ्ग;
      स्वभाव:
        failf(दत्त, "server did not report OK, got %d", ftpcode);
        परिणाम = CURLE_PARTIAL_FILE;
        भङ्ग;
      }
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ftp_समाप्तम्_check_partial(संरचना Curl_easy *दत्त,
                                       संरचना ftp_conn *ftpc,
                                       संरचना FTP *ftp, CURLcode परिणाम,
                                       bool premature)
{
  यदि(परिणाम || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  अन्यथा यदि(दत्त->state.upload) {
    यदि((ftp->transfer == PPTRANSFER_BODY) &&
       (दत्त->state.infilesize != -1) && /* upload with known size */
       ((!दत्त->समुच्चयः.crlf && !दत्त->state.prefer_ascii && /* no conversion */
         (दत्त->state.infilesize != दत्त->req.writebytecount)) ||
        ((दत्त->समुच्चयः.crlf || दत्त->state.prefer_ascii) && /* maybe CRLF conv */
         (दत्त->state.infilesize > दत्त->req.writebytecount))
       )) {
      failf(दत्त, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            दत्त->req.writebytecount, दत्त->state.infilesize);
      परिणाम = CURLE_PARTIAL_FILE;
    }
  }
  अन्यथा {
    यदि((दत्त->req.आकार != -1) &&
       (दत्त->req.आकार != दत्त->req.bytecount) &&
       (दत्त->req.maxdownload != दत्त->req.bytecount)) {
      failf(दत्त, "Received only partial file: %" FMT_OFF_T " bytes",
            दत्त->req.bytecount);
      परिणाम = CURLE_PARTIAL_FILE;
    }
    अन्यथा यदि(!ftpc->dont_check &&
            !दत्त->req.bytecount &&
            (दत्त->req.आकार > 0)) {
      failf(दत्त, "No data was received");
      परिणाम = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                         bool premature)
{
  संरचना FTP *ftp = Curl_meta_get(दत्त, CURL_META_FTP_EASY);
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  CURLcode परिणाम;

  यदि(!ftp || !ftpc)
    प्रत्यागम CURLE_OK;

  परिणाम = ftp_समाप्तम्_स्थितिः(दत्त, ftpc, स्थितिः, premature);

  ftp_समाप्तम्_wildcard(दत्त, ftpc);
  ftp_समाप्तम्_मार्गः(दत्त, ftpc, परिणाम);
  परिणाम = ftp_समाप्तम्_secondary_socket(दत्त, ftpc, परिणाम);
  परिणाम = ftp_समाप्तम्_control_प्रत्युत्तरम्(दत्त, ftpc, ftp, परिणाम, premature);
  परिणाम = ftp_समाप्तम्_check_partial(दत्त, ftpc, ftp, परिणाम, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  यदि(!स्थितिः && !परिणाम && !premature && दत्त->समुच्चयः.postquote)
    परिणाम = ftp_sendquote(दत्त, ftpc, दत्त->समुच्चयः.postquote);
  CURL_TRC_FTP(दत्त, "[%s] done, result=%d", FTP_CSTATE(ftpc), (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
स्थिर CURLcode ftp_nb_type(संरचना Curl_easy *दत्त,
                            संरचना ftp_conn *ftpc,
                            संरचना FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode परिणाम;
  वर्ण want = (वर्ण)(ascii ? 'A' : 'I');

  यदि(ftpc->transfertype == want) {
    ftp_state(दत्त, ftpc, newstate);
    प्रत्यागम ftp_state_type_resp(दत्त, ftpc, ftp, 200, newstate);
  }

  परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "TYPE %c", want);
  यदि(!परिणाम) {
    ftp_state(दत्त, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
स्थिर CURLcode ftp_perform(
  संरचना Curl_easy *दत्त,
  संरचना ftp_conn *ftpc,
  संरचना FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_समाप्तम्)
{
  /* this is FTP and no proxy */
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_FTP(दत्त, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  यदि(दत्त->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_समाप्तम् = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  परिणाम = ftp_state_quote(दत्त, ftpc, ftp, TRUE, FTP_QUOTE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* run the state-machine */
  परिणाम = ftp_statemach(दत्त, ftpc, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, SECONDARYSOCKET);

  यदि(*connected)
    infof(दत्त, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  अन्यथा
    CURL_TRC_FTP(दत्त, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  यदि(*dophase_समाप्तम्)
    CURL_TRC_FTP(दत्त, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  प्रत्यागम परिणाम;
}

स्थिर शून्य wc_दत्त_dtor(शून्य *ptr)
{
  संरचना ftp_wc *ftpwc = ptr;
  यदि(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_दत्त_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

स्थिर CURLcode init_wc_दत्त(संरचना Curl_easy *दत्त,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  वर्ण *चरमः_slash;
  वर्ण *मार्गः = ftp->मार्गः;
  संरचना Wildcard_दत्त *wildcard = दत्त->wildcard;
  CURLcode परिणाम = CURLE_OK;
  संरचना ftp_wc *ftpwc = रिक्त;

  चरमः_slash = strrchr(ftp->मार्गः, '/');
  यदि(चरमः_slash) {
    चरमः_slash++;
    यदि(चरमः_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      प्रत्यागम ftp_parse_url_मार्गः(दत्त, ftpc, ftp);
    }
    wildcard->आदर्शः = curlx_strdup(चरमः_slash);
    यदि(!wildcard->आदर्शः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    चरमः_slash[0] = '\0'; /* cut file from path */
  }
  अन्यथा { /* there is only 'wildcard pattern' or nothing */
    यदि(मार्गः[0]) {
      wildcard->आदर्शः = curlx_strdup(मार्गः);
      यदि(!wildcard->आदर्शः)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      मार्गः[0] = '\0';
    }
    अन्यथा { /* only list */
      wildcard->state = CURLWC_CLEAN;
      प्रत्यागम ftp_parse_url_मार्गः(दत्त, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, परिमाण(संरचना ftp_wc));
  यदि(!ftpwc) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_दत्त_alloc();
  यदि(!ftpwc->parser) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_दत्त_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  यदि(दत्त->समुच्चयः.ftp_filemethod == FTPFILE_NOCWD)
    दत्त->समुच्चयः.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  परिणाम = ftp_parse_url_मार्गः(दत्त, ftpc, ftp);
  यदि(परिणाम) {
    गच्छ fail;
  }

  wildcard->मार्गः = curlx_strdup(ftp->मार्गः);
  यदि(!wildcard->मार्गः) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  infof(दत्त, "Wildcard - Parsing started");
  प्रत्यागम CURLE_OK;

fail:
  यदि(ftpwc) {
    Curl_ftp_parselist_दत्त_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->आदर्शः);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = रिक्त;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode wc_statemach(संरचना Curl_easy *दत्त,
                             संरचना ftp_conn *ftpc,
                             संरचना FTP *ftp)
{
  संरचना Wildcard_दत्त * ध्रुव wildcard = दत्त->wildcard;
  CURLcode परिणाम = CURLE_OK;

  प्रति(;;) {
    विकल्प(wildcard->state) {
    पक्ष CURLWC_INIT:
      परिणाम = init_wc_दत्त(दत्त, ftpc, ftp);
      यदि(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        प्रत्यागम परिणाम;
      wildcard->state = परिणाम ? CURLWC_ERROR : CURLWC_MATCHING;
      प्रत्यागम परिणाम;

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
      यदि(Curl_llist_गणनम्(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;
      }
      अनुवर्त;
    }

    पक्ष CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      संरचना Curl_llist_ग्रन्थिः *शीर्ष = Curl_llist_शीर्ष(&wildcard->filelist);
      संरचना curl_fileinfo *finfo = Curl_ग्रन्थिः_elem(शीर्ष);
      वर्ण *tmp_मार्गः;
      वर्ण *enc = curl_easy_escape(रिक्त, finfo->filename, 0);
      यदि(!enc)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      tmp_मार्गः = curl_maprintf("%s%s", wildcard->मार्गः, enc);
      curl_free(enc);
      यदि(!tmp_मार्गः)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->मार्गः = tmp_मार्गः;

      infof(दत्त, "Wildcard - START of \"%s\"", finfo->filename);
      यदि(दत्त->समुच्चयः.chunk_bgn) {
        दीर्घ userresponse;
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, दत्त, easy_chunk_bgn);
        userresponse = दत्त->समुच्चयः.chunk_bgn(
          finfo, दत्त->समुच्चयः.wildcardptr,
          (पूर्णाङ्क)Curl_llist_गणनम्(&wildcard->filelist));
        CURL_CBAPI_अन्तः(&guard);
        विकल्प(userresponse) {
        पक्ष CURL_CHUNK_BGN_FUNC_SKIP:
          infof(दत्त, "Wildcard - \"%s\" skipped by user", finfo->filename);
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

      यदि(finfo->flags & CURLFINFOFLAG_ज्ञातम्_आकार)
        ftpc->ज्ञातम्_filesize = finfo->आकार;

      परिणाम = ftp_parse_url_मार्गः(दत्त, ftpc, ftp);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ग्रन्थिः_remove(Curl_llist_शीर्ष(&wildcard->filelist));

      यदि(Curl_llist_गणनम्(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        प्रत्यागम CURLE_OK;
      }
      प्रत्यागम परिणाम;
    }

    पक्ष CURLWC_SKIP: {
      यदि(दत्त->समुच्चयः.chunk_अन्तः) {
        संरचना Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, दत्त, easy_chunk_अन्तः);
        दत्त->समुच्चयः.chunk_अन्तः(दत्त->समुच्चयः.wildcardptr);
        CURL_CBAPI_अन्तः(&guard);
      }
      Curl_ग्रन्थिः_remove(Curl_llist_शीर्ष(&wildcard->filelist));
      wildcard->state = (Curl_llist_गणनम्(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      अनुवर्त;
    }

    पक्ष CURLWC_CLEAN: {
      संरचना ftp_wc *ftpwc = wildcard->ftpwc;
      परिणाम = CURLE_OK;
      यदि(ftpwc)
        परिणाम = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = परिणाम ? CURLWC_ERROR : CURLWC_समाप्तम्;
      प्रत्यागम परिणाम;
    }

    पक्ष CURLWC_समाप्तम्:
    पक्ष CURLWC_ERROR:
    पक्ष CURLWC_CLEAR:
      यदि(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = रिक्त;
      }
      प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_regular_transfer(संरचना Curl_easy *दत्त,
                                     संरचना ftp_conn *ftpc,
                                     संरचना FTP *ftp,
                                     bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;
  दत्त->req.आकार = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(दत्त);

  ftpc->ctl_valid = TRUE; /* starts good */

  परिणाम = ftp_perform(दत्त, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_समाप्तम्); /* all commands in the DO-phase done? */

  यदि(!परिणाम) {

    यदि(!*dophase_समाप्तम्)
      /* the DO phase has not completed yet */
      प्रत्यागम CURLE_OK;

    परिणाम = ftp_dophase_समाप्तम्(दत्त, ftpc, ftp, connected);

    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
    freedirs(ftpc);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(दत्त, CURL_META_FTP_EASY);

  *समाप्तम् = FALSE; /* default to false */
  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  ftpc->wait_दत्त_conn = FALSE; /* default to no such wait */

#यदि_परिभाषितम् CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    संरचना Curl_cwriter *ftp_lc_writer;

    परिणाम = Curl_cwriter_create(&ftp_lc_writer, दत्त, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    परिणाम = Curl_cwriter_सङ्कलय(दत्त, ftp_lc_writer);
    यदि(परिणाम) {
      Curl_cwriter_free(दत्त, ftp_lc_writer);
      प्रत्यागम परिणाम;
    }
  }
#यदि_अन्तः /* CURL_PREFER_LF_LINEENDS */

  यदि(दत्त->state.wildcardmatch) {
    परिणाम = wc_statemach(दत्त, ftpc, ftp);
    यदि(दत्त->wildcard->state == CURLWC_SKIP ||
       दत्त->wildcard->state == CURLWC_समाप्तम्) {
      /* do not call ftp_regular_transfer */
      प्रत्यागम CURLE_OK;
    }
    यदि(परिणाम) /* error, loop or skipping the file */
      प्रत्यागम परिणाम;
  }
  अन्यथा { /* no wildcard FSM needed */
    परिणाम = ftp_parse_url_मार्गः(दत्त, ftpc, ftp);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  परिणाम = ftp_regular_transfer(दत्त, ftpc, ftp, समाप्तम्);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode ftp_quit(संरचना Curl_easy *दत्त,
                         संरचना ftp_conn *ftpc)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(ftpc->ctl_valid) {
    CURL_TRC_FTP(दत्त, "sending QUIT to close session");
    परिणाम = Curl_pp_sendf(दत्त, &ftpc->pp, "%s", "QUIT");
    यदि(परिणाम) {
      failf(दत्त, "Failure sending QUIT command: %s",
            curl_easy_strerror(परिणाम));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(दत्त->conn); /* mark for closure */
      ftp_state(दत्त, ftpc, FTP_STOP);
      प्रत्यागम परिणाम;
    }

    ftp_state(दत्त, ftpc, FTP_QUIT);

    परिणाम = ftp_खण्डम्_statemach(दत्त, ftpc);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode ftp_disconnect(संरचना Curl_easy *दत्त,
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
  यदि(dead_connection || Curl_pp_needs_flush(दत्त, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (शून्य)ftp_quit(दत्त, ftpc); /* ignore errors on the QUIT */
  प्रत्यागम CURLE_OK;
}

/* called from multi.c while DOing */
स्थिर CURLcode ftp_doing(संरचना Curl_easy *दत्त,
                          bool *dophase_समाप्तम्)
{
  संरचना ftp_conn *ftpc = Curl_conn_meta_get(दत्त->conn, CURL_META_FTP_CONN);
  संरचना FTP *ftp = Curl_meta_get(दत्त, CURL_META_FTP_EASY);
  CURLcode परिणाम;

  यदि(!ftpc || !ftp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = ftp_statemach(दत्त, ftpc, dophase_समाप्तम्);

  यदि(परिणाम)
    CURL_TRC_FTP(दत्त, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  अन्यथा यदि(*dophase_समाप्तम्) {
    परिणाम = ftp_dophase_समाप्तम्(दत्त, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(दत्त, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य ftp_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना FTP *ftp = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

स्थिर शून्य ftp_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना ftp_conn *ftpc = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_गन्तव्यम्_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

स्थिर शून्य type_url_check(संरचना Curl_easy *दत्त, संरचना FTP *ftp)
{
  size_t len = वाक्यदैर्घ्यम्(ftp->मार्गः);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  यदि((len >= 7) && !memcmp(&ftp->मार्गः[len - 7], ";type=", 6)) {
    वर्ण *type = &ftp->मार्गः[len - 7];
    वर्ण command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    विकल्प(command) {
    पक्ष 'A': /* ASCII mode */
      दत्त->state.prefer_ascii = TRUE;
      भङ्ग;

    पक्ष 'D': /* directory mode */
      दत्त->state.list_only = TRUE;
      भङ्ग;

    पक्ष 'I': /* binary mode */
    स्वभाव:
      /* switch off ASCII */
      दत्त->state.prefer_ascii = FALSE;
      भङ्ग;
    }
  }
}

स्थिर CURLcode ftp_सज्जता_connection(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  संरचना FTP *ftp;
  CURLcode परिणाम = CURLE_OK;
  संरचना ftp_conn *ftpc;

  ftp = curlx_calloc(1, परिमाण(*ftp));
  यदि(!ftp ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ftpc = curlx_calloc(1, परिमाण(*ftpc));
  यदि(!ftpc ||
     Curl_conn_meta_समुच्चयः(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* clone connection related data that is FTP specific */
  यदि(CURL_EASY_STR(दत्त, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(दत्त, STRING_FTP_ACCOUNT));
    यदि(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  यदि(CURL_EASY_STR(दत्त, STRING_FTP_ALTERNATIVE_गन्तव्यम्_USER)) {
    ftpc->alternative_गन्तव्यम्_user =
      curlx_strdup(CURL_EASY_STR(दत्त, STRING_FTP_ALTERNATIVE_गन्तव्यम्_USER));
    यदि(!ftpc->alternative_गन्तव्यम्_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  ftp->मार्गः = &दत्त->state.ऊर्ध्वम्.मार्गः[1]; /* do not include the initial slash */

  type_url_check(दत्त, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->ज्ञातम्_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = दत्त->समुच्चयः.use_ssl;
  ftpc->ccc = दत्त->समुच्चयः.ftp_ccc;

  CURL_TRC_FTP(दत्त, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

bool Curl_ftp_conns_मेलनम्(संरचना connectdata *needle, संरचना connectdata *conn)
{
  संरचना ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  संरचना ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  यदि(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_गन्तव्यम्_user,
                     cftpc->alternative_गन्तव्यम्_user) ||
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
  ftp_सज्जता_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_समाप्तम्,                        /* done */
  ftp_do_अधिकम्,                     /* do_more */
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

#यदि_अन्तः /* CURL_DISABLE_FTP */
