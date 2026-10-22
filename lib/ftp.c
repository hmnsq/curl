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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "if2ip.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "ftp.h"
#അന്തര്ഭാവ "ftp-int.h"
#അന്തര്ഭാവ "ftplistparser.h"
#അന്തര്ഭാവ "curl_range.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "parsedate.h" /* for the week day and month names */
#അന്തര്ഭാവ "sockaddr.h" /* required for Curl_sockaddr_storage */
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(വരി) \
  (ISDIGIT((വരി)[0]) && ISDIGIT((വരി)[1]) && ISDIGIT((വരി)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(വരി) (STATUSCODE(വരി) && (' ' == (വരി)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം ftp_state_names[] = {
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
സ്ഥിതം ശൂന്യം ftp_state_താഴെയുള്ള(ഘടന Curl_easy *data,
                          ഘടന ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , പൂർണ്ണസംഖ്യ lineno
#endif
  )
{
  എങ്കിൽ(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((ശൂന്യം)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#അല്ലെങ്കിൽ
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_താഴെയുള്ള(x, y, z)
#അല്ലെങ്കിൽ /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_താഴെയുള്ള(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

സ്ഥിതം CURLcode ftp_state_mdtm(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp);
സ്ഥിതം CURLcode ftp_state_quote(ഘടന Curl_easy *data,
                                ഘടന ftp_conn *ftpc,
                                ഘടന FTP *ftp,
                                bool init, ftpstate instate);
സ്ഥിതം CURLcode ftp_nb_type(ഘടന Curl_easy *data,
                            ഘടന ftp_conn *ftpc,
                            ഘടന FTP *ftp,
                            bool ascii, ftpstate newstate);
സ്ഥിതം CURLcode getftpresponse(ഘടന Curl_easy *data, size_t *nreadp,
                               പൂർണ്ണസംഖ്യ *ftpcodep);

സ്ഥിതം ശൂന്യം freedirs(ഘടന ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ശൂന്യ;
}

സ്ഥിതം size_t numof_slashes(സ്ഥിരം അക്ഷരം *str)
{
  സ്ഥിരം അക്ഷരം *slashPos;
  size_t അക്കം = 0;
  ചെയ്യുക {
    slashPos = strchr(str, '/');
    എങ്കിൽ(slashPos) {
      ++അക്കം;
      str = slashPos + 1;
    }
  } ആകുമ്പോൾ(slashPos);
  തിരികെ അക്കം;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
സ്ഥിതം CURLcode ftp_parse_url_path(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc,
                                   ഘടന FTP *ftp)
{
  സ്ഥിരം അക്ഷരം *slashPos = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *fileName = ശൂന്യ;
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *rawPath = ശൂന്യ; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  എങ്കിൽ(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  എങ്കിൽ(result) {
    failf(data, "path contains control characters");
    തിരികെ result;
  }
  rawPath = ftpc->rawpath;

  തിരഞ്ഞെടുപ്പ്(data->set.ftp_filemethod) {
  അവസ്ഥ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    എങ്കിൽ((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    നിർത്തുക;

  അവസ്ഥ FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    എങ്കിൽ(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      എങ്കിൽ(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, വലുപ്പം(ftpc->dirs[0]));
      എങ്കിൽ(!ftpc->dirs)
        തിരികെ CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (പൂർണ്ണസംഖ്യ)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    അല്ലെങ്കിൽ
      fileName = rawPath; /* filename only (or empty) */
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി: /* allow pretty much anything */
  അവസ്ഥ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    സ്ഥിരം അക്ഷരം *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    എങ്കിൽ(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      തിരികെ CURLE_URL_MALFORMAT;

    എങ്കിൽ(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, വലുപ്പം(ftpc->dirs[0]));
      എങ്കിൽ(!ftpc->dirs)
        തിരികെ CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      ആകുമ്പോൾ(dirAlloc--) {
        സ്ഥിരം അക്ഷരം *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        എങ്കിൽ(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        എങ്കിൽ(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (പൂർണ്ണസംഖ്യ)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (പൂർണ്ണസംഖ്യ)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    നിർത്തുക;
  } /* switch */

  എങ്കിൽ(fileName && *fileName)
    ftpc->file = fileName;
  അല്ലെങ്കിൽ
    ftpc->file = ശൂന്യ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  എങ്കിൽ(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    തിരികെ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  എങ്കിൽ((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  അല്ലെങ്കിൽ { /* newly created FTP connections are already in entry path */
    സ്ഥിരം അക്ഷരം *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    എങ്കിൽ(oldPath) {
      size_t n = pathLen;
      എങ്കിൽ(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      അല്ലെങ്കിൽ
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      എങ്കിൽ((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ftp_need_type(ഘടന ftp_conn *ftpc,
                         bool ascii_wanted)
{
  തിരികെ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

സ്ഥിതം ശൂന്യം close_secondarysocket(ഘടന Curl_easy *data,
                                  ഘടന ftp_conn *ftpc)
{
  (ശൂന്യം)ftpc;
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
ഘടന ftp_cw_lc_ctx {
  ഘടന Curl_cwriter super;
  bool newline_pending;
};

സ്ഥിതം CURLcode ftp_cw_lc_write(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  സ്ഥിതം സ്ഥിരം അക്ഷരം nl = '\n';
  ഘടന ftp_cw_lc_ctx *ctx = writer->ctx;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  എങ്കിൽ(!ftpc)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ആകുമ്പോൾ(blen) {
    /* do not pass EOS when writing parts */
    പൂർണ്ണസംഖ്യ chunk_type = (type & ~CLIENTWRITE_EOS);
    സ്ഥിരം അക്ഷരം *cp;
    size_t chunk_len;
    CURLcode result;

    എങ്കിൽ(ctx->newline_pending) {
      എങ്കിൽ(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        എങ്കിൽ(result)
          തിരികെ result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    എങ്കിൽ(!cp)
      നിർത്തുക;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    എങ്കിൽ(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  എങ്കിൽ(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    എങ്കിൽ(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      തിരികെ Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    തിരികെ Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ശൂന്യ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  വലുപ്പം(ഘടന ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
സ്ഥിതം CURLcode ftp_check_ctrl_on_data_wait(ഘടന Curl_easy *data,
                                            ഘടന ftp_conn *ftpc)
{
  ഘടന connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ഘടന pingpong *pp = &ftpc->pp;
  size_t nread;
  പൂർണ്ണസംഖ്യ ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  എങ്കിൽ(curlx_dyn_len(&pp->recvbuf)) {
    സ്ഥിരം അക്ഷരം *l = curlx_dyn_ptr(&pp->recvbuf);
    എങ്കിൽ(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (ശൂന്യം)getftpresponse(data, &nread, &ftpcode);
      തിരികെ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  എങ്കിൽ(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  അല്ലെങ്കിൽ {
    പൂർണ്ണസംഖ്യ socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    തിരഞ്ഞെടുപ്പ്(socketstate) {
    അവസ്ഥ -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      തിരികെ CURLE_FTP_ACCEPT_FAILED;
    സ്ഥിരസ്ഥിതി:
      എങ്കിൽ(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      നിർത്തുക;
    }
  }

  എങ്കിൽ(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    എങ്കിൽ(pp->overflow > 3) {
      സ്ഥിരം അക്ഷരം *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      എങ്കിൽ((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        എങ്കിൽ(!curlx_str_സംഖ്യ(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          തിരികെ CURLE_OK;
        }
      }
    }

    (ശൂന്യം)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    എങ്കിൽ(ftpcode / 100 > 3)
      തിരികെ CURLE_FTP_ACCEPT_FAILED;

    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
സ്ഥിതം CURLcode ftp_initiate_transfer(ഘടന Curl_easy *data,
                                      ഘടന ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  എങ്കിൽ(result || !connected)
    തിരികെ result;

  എങ്കിൽ(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  അല്ലെങ്കിൽ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  തിരികെ CURLE_OK;
}

സ്ഥിതം bool ftp_endofresp(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                          സ്ഥിരം അക്ഷരം *വരി, size_t len, പൂർണ്ണസംഖ്യ *code)
{
  curl_off_t status;
  (ശൂന്യം)data;
  (ശൂന്യം)conn;

  എങ്കിൽ((len > 3) && LASTLINE(വരി) && !curlx_str_സംഖ്യ(&വരി, &status, 999)) {
    *code = (പൂർണ്ണസംഖ്യ)status;
    തിരികെ TRUE;
  }

  തിരികെ FALSE;
}

സ്ഥിതം CURLcode ftp_readresp(ഘടന Curl_easy *data,
                             ഘടന ftp_conn *ftpc,
                             int8_t sockindex,
                             ഘടന pingpong *pp,
                             പൂർണ്ണസംഖ്യ *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  പൂർണ്ണസംഖ്യ code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  എങ്കിൽ(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  എങ്കിൽ(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }

  തിരികെ result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
സ്ഥിതം CURLcode getftpresponse(ഘടന Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               പൂർണ്ണസംഖ്യ *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ഘടന connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ഘടന pingpong *pp = &ftpc->pp;
  size_t nread;
  പൂർണ്ണസംഖ്യ cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  എങ്കിൽ(!ftpc)
    തിരികെ CURLE_FAILED_INIT;

  ആകുമ്പോൾ(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    എങ്കിൽ(timeout <= 0) {
      failf(data, "FTP response timeout");
      തിരികെ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    എങ്കിൽ(timeout < interval_ms)
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

    എങ്കിൽ(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      പൂർണ്ണസംഖ്യ ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      എങ്കിൽ(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        തിരികെ CURLE_RECV_ERROR;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(ev == 0) {
        result = Curl_pgrsUpdate(data);
        തുടരുക; /* continue in our loop for the timeout duration */
      }
    }

    എങ്കിൽ(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      എങ്കിൽ(result)
        നിർത്തുക;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    അല്ലെങ്കിൽ
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (പൂർണ്ണസംഖ്യ)result, *nreadp, *ftpcodep);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_user(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  എങ്കിൽ(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_pwd(ഘടന Curl_easy *data,
                              ഘടന ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  എങ്കിൽ(!data->id && getenv("CURL_FTP_PWD_STOP"))
    തിരികെ CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  എങ്കിൽ(!result)
    ftp_state(data, ftpc, FTP_PWD);

  തിരികെ result;
}

/* For the FTP "protocol connect" and "doing" phases only */
സ്ഥിതം CURLcode ftp_pollset(ഘടന Curl_easy *data,
                            ഘടന easy_pollset *ps)
{
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  തിരികെ ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
സ്ഥിതം CURLcode ftp_domore_pollset(ഘടന Curl_easy *data,
                                   ഘടന easy_pollset *ps)
{
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  എങ്കിൽ(!ftpc)
    തിരികെ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  എങ്കിൽ(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    തിരികെ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  തിരികെ Curl_pp_pollset(data, &ftpc->pp, ps);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pathlen(ഘടന ftp_conn *ftpc, പൂർണ്ണസംഖ്യ അക്കം)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > അക്കം);
  തിരികെ ftpc->dirs[അക്കം].len;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *pathpiece(ഘടന ftp_conn *ftpc, പൂർണ്ണസംഖ്യ അക്കം)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > അക്കം);
  തിരികെ &ftpc->rawpath[ftpc->dirs[അക്കം].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
സ്ഥിതം CURLcode ftp_state_cwd(ഘടന Curl_easy *data,
                              ഘടന ftp_conn *ftpc,
                              ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  അല്ലെങ്കിൽ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    എങ്കിൽ(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        എങ്കിൽ(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      അല്ലെങ്കിൽ {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  തിരികെ result;
}

തരനിർവചനം എണ്ണൽ {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
സ്ഥിതം CURLcode ftp_port_parse_string(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn,
                                      സ്ഥിരം അക്ഷരം *string_ftpport,
                                      ഘടന Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      സ്ഥിരം അക്ഷരം **hostp,
                                      അക്ഷരം *hbuf, size_t hbuflen)
{
  സ്ഥിരം അക്ഷരം *ip_end = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *addr = ശൂന്യ;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  അക്ഷരം ipstr[50];
#ifndef USE_IPV6
  (ശൂന്യം)conn;
  (ശൂന്യം)ss;
#endif

  /* default to nothing */
  *hostp = ശൂന്യ;
  *port_minp = *port_maxp = 0;

  എങ്കിൽ(!string_ftpport || (strlen(string_ftpport) <= 1))
    പോകുക done;

#ifdef USE_IPV6
  എങ്കിൽ(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    സ്ഥിരം അക്ഷരം *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    എങ്കിൽ(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  അല്ലെങ്കിൽ
#endif
    എങ്കിൽ(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    അല്ലെങ്കിൽ {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      എങ്കിൽ(ip_end) {
#ifdef USE_IPV6
        ഘടന sockaddr_in6 * സ്ഥിരം sa6 = (ശൂന്യം *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        എങ്കിൽ(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = ശൂന്യ; /* this got no port ! */
        }
#endif
      }
      അല്ലെങ്കിൽ
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  എങ്കിൽ(ip_end) {
    സ്ഥിരം അക്ഷരം *portp = strchr(ip_end, ':');
    എങ്കിൽ(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        എങ്കിൽ(!curlx_str_single(&portp, '-') &&
           !curlx_str_സംഖ്യ(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        അല്ലെങ്കിൽ
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  എങ്കിൽ(port_min > port_max)
    port_min = port_max = 0;

  എങ്കിൽ(addrlen) {
    സ്ഥിരം ഘടന Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    എങ്കിൽ(!remote_addr || (addrlen >= വലുപ്പം(ipstr)) || (addrlen >= hbuflen))
      തിരികെ CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    തിരഞ്ഞെടുപ്പ്(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    അവസ്ഥ IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      നിർത്തുക;
    അവസ്ഥ IF2IP_AF_NOT_SUPPORTED:
      തിരികെ CURLE_FTP_PORT_FAILED;
    അവസ്ഥ IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      നിർത്തുക;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  തിരികെ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
സ്ഥിതം CURLcode ftp_port_default_host(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn,
                                      ഘടന Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      സ്ഥിരം അക്ഷരം **hostp,
                                      അക്ഷരം *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ഘടന sockaddr *sa = (ഘടന sockaddr *)ss;
  ഘടന sockaddr_in * സ്ഥിരം sa4 = (ശൂന്യം *)sa;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 * സ്ഥിരം sa6 = (ശൂന്യം *)sa;
#endif
  അക്ഷരം buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = വലുപ്പം(*ss);
  എങ്കിൽ(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
    തിരികെ CURLE_FTP_PORT_FAILED;
  }
  തിരഞ്ഞെടുപ്പ്(sa->sa_family) {
#ifdef USE_IPV6
  അവസ്ഥ AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    നിർത്തുക;
  }
  എങ്കിൽ(result)
    തിരികെ CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  തിരികെ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
സ്ഥിതം CURLcode ftp_port_resolve_host(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn,
                                      സ്ഥിരം അക്ഷരം *host,
                                      ഘടന Curl_dns_entry **dns_entryp,
                                      സ്ഥിരം ഘടന Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = ശൂന്യ;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  എങ്കിൽ(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  അല്ലെങ്കിൽ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  തിരികെ result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
സ്ഥിതം CURLcode ftp_port_open_socket(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn,
                                     സ്ഥിരം ഘടന Curl_addrinfo *res,
                                     സ്ഥിരം ഘടന Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  അക്ഷരം buffer[STRERROR_LEN];
  പൂർണ്ണസംഖ്യ sockerr = 0;
  സ്ഥിരം ഘടന Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  വേണ്ടി(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, ശൂന്യ,
                       Curl_conn_get_transport(data, conn), portsockp);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        തിരികെ result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      തുടരുക;
    }
    നിർത്തുക;
  }
  എങ്കിൽ(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
    തിരികെ CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  തിരികെ result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
സ്ഥിതം CURLcode ftp_port_bind_socket(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn,
                                     curl_socket_t portsock,
                                     സ്ഥിരം ഘടന Curl_addrinfo *ai,
                                     ഘടന Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  ഘടന sockaddr *sa = (ഘടന sockaddr *)ss;
  ഘടന sockaddr_in * സ്ഥിരം sa4 = (ശൂന്യം *)sa;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 * സ്ഥിരം sa6 = (ശൂന്യം *)sa;
#endif
  അക്ഷരം buffer[STRERROR_LEN];
  unsigned short port;
  പൂർണ്ണസംഖ്യ sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  വേണ്ടി(port = port_min; port <= port_max;) {
    എങ്കിൽ(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    അല്ലെങ്കിൽ
      sa6->sin6_port = htons(port);
#endif
    എങ്കിൽ(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      എങ്കിൽ(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));

        *sslen_io = വലുപ്പം(*ss);
        എങ്കിൽ(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
          തിരികെ CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        തുടരുക;
      }
      എങ്കിൽ(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
        തിരികെ CURLE_FTP_PORT_FAILED;
      }
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    എങ്കിൽ(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      തിരികെ CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = വലുപ്പം(*ss);
  എങ്കിൽ(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
    തിരികെ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  തിരികെ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
സ്ഥിതം CURLcode ftp_port_listen(ഘടന Curl_easy *data, curl_socket_t portsock)
{
  അക്ഷരം buffer[STRERROR_LEN];

  എങ്കിൽ(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
    തിരികെ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  തിരികെ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
സ്ഥിതം CURLcode ftp_port_send_command(ഘടന Curl_easy *data,
                                      ഘടന ftp_conn *ftpc,
                                      ഘടന connectdata *conn,
                                      ഘടന Curl_sockaddr_storage *ss,
                                      സ്ഥിരം ഘടന Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  സ്ഥിതം സ്ഥിരം അക്ഷരം mode[][5] = { "EPRT", "PORT" };
  ഘടന sockaddr *sa = (ഘടന sockaddr *)ss;
  ഘടന sockaddr_in * സ്ഥിരം sa4 = (ശൂന്യം *)sa;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 * സ്ഥിരം sa6 = (ശൂന്യം *)sa;
#endif
  അക്ഷരം myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, വലുപ്പം(myhost));

#ifdef USE_IPV6
  എങ്കിൽ(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  വേണ്ടി(; fcmd != DONE; fcmd++) {

    എങ്കിൽ(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      തുടരുക;

    എങ്കിൽ((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      തുടരുക;

    തിരഞ്ഞെടുപ്പ്(sa->sa_family) {
    അവസ്ഥ AF_INET:
      port = ntohs(sa4->sin_port);
      നിർത്തുക;
#ifdef USE_IPV6
    അവസ്ഥ AF_INET6:
      port = ntohs(sa6->sin6_port);
      നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
      തുടരുക; /* might as well skip this */
    }

    എങ്കിൽ(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      എങ്കിൽ(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        തിരികെ result;
      }
      നിർത്തുക;
    }
    എങ്കിൽ(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      അക്ഷരം target[വലുപ്പം(myhost) + 20];
      സ്ഥിരം അക്ഷരം *source = myhost;
      അക്ഷരം *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      ആകുമ്പോൾ(*source) {
        എങ്കിൽ(*source == '.')
          *dest = ',';
        അല്ലെങ്കിൽ
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (പൂർണ്ണസംഖ്യ)(port >> 8), (പൂർണ്ണസംഖ്യ)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      എങ്കിൽ(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        തിരികെ result;
      }
      നിർത്തുക;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  തിരികെ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
സ്ഥിതം CURLcode ftp_state_use_port(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  ഘടന connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ഘടന Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  അക്ഷരം hbuf[NI_MAXHOST];
  സ്ഥിരം അക്ഷരം *host = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  ഘടന Curl_dns_entry *dns_entry = ശൂന്യ;
  സ്ഥിരം ഘടന Curl_addrinfo *res = ശൂന്യ;
  സ്ഥിരം ഘടന Curl_addrinfo *ai = ശൂന്യ;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, വലുപ്പം(hbuf));
  എങ്കിൽ(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, വലുപ്പം(hbuf), &non_local);

  /* resolve host string to address list */
  എങ്കിൽ(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  എങ്കിൽ(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  എങ്കിൽ(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  എങ്കിൽ(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  എങ്കിൽ(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  എങ്കിൽ(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  എങ്കിൽ(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  എങ്കിൽ(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  എങ്കിൽ(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  അല്ലെങ്കിൽ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    എങ്കിൽ(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  എങ്കിൽ(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_use_pasv(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc,
                                   ഘടന connectdata *conn)
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

  സ്ഥിതം സ്ഥിരം അക്ഷരം mode[][5] = { "EPSV", "PASV" };
  പൂർണ്ണസംഖ്യ modeoff;

#ifdef PF_INET6
  എങ്കിൽ(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  എങ്കിൽ(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  തിരികെ result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
സ്ഥിതം CURLcode ftp_state_prepare_transfer(ഘടന Curl_easy *data,
                                           ഘടന ftp_conn *ftpc,
                                           ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;

  എങ്കിൽ(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  അല്ലെങ്കിൽ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    എങ്കിൽ(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      എങ്കിൽ(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      അല്ലെങ്കിൽ
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    അല്ലെങ്കിൽ
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_rest(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  അല്ലെങ്കിൽ
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_size(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  അല്ലെങ്കിൽ
    result = ftp_state_rest(data, ftpc, ftp);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_list(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp)
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
  സ്ഥിരം അക്ഷരം *lstArg = ശൂന്യ;
  പൂർണ്ണസംഖ്യ lstArglen = 0;
  അക്ഷരം *cmd;

  എങ്കിൽ((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    സ്ഥിരം അക്ഷരം *rawPath = ftpc->rawpath;
    സ്ഥിരം അക്ഷരം *slashPos = strrchr(rawPath, '/');
    എങ്കിൽ(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      എങ്കിൽ(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (പൂർണ്ണസംഖ്യ)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  എങ്കിൽ(!cmd)
    തിരികെ CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  എങ്കിൽ(!result)
    ftp_state(data, ftpc, FTP_LIST);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_list_prequote(ഘടന Curl_easy *data,
                                        ഘടന ftp_conn *ftpc,
                                        ഘടന FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  തിരികെ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

സ്ഥിതം CURLcode ftp_state_retr_prequote(ഘടന Curl_easy *data,
                                        ഘടന ftp_conn *ftpc,
                                        ഘടന FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  തിരികെ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

സ്ഥിതം CURLcode ftp_state_stor_prequote(ഘടന Curl_easy *data,
                                        ഘടന ftp_conn *ftpc,
                                        ഘടന FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  തിരികെ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

സ്ഥിതം CURLcode ftp_state_type(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  എങ്കിൽ(data->req.no_body && ftpc->file &&
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
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
    result = ftp_state_size(data, ftpc, ftp);

  തിരികെ result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
സ്ഥിതം CURLcode ftp_state_mdtm(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  എങ്കിൽ((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  അല്ലെങ്കിൽ
    result = ftp_state_type(data, ftpc, ftp);

  തിരികെ result;
}

/* This is called after the TYPE and possible quote commands have been sent */
സ്ഥിതം CURLcode ftp_state_ul_setup(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc,
                                   ഘടന FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  എങ്കിൽ((data->state.resume_from && !sizechecked) ||
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
    പൂർണ്ണസംഖ്യ seekerr = CURL_SEEKFUNC_OK;

    എങ്കിൽ(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      തിരികെ result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    എങ്കിൽ(data->set.seek_func) {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    എങ്കിൽ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      എങ്കിൽ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        തിരികെ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ചെയ്യുക {
        അക്ഷരം scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)വലുപ്പം(scratch)) ?
          വലുപ്പം(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        എങ്കിൽ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          തിരികെ CURLE_FTP_COULDNT_USE_REST;
        }
      } ആകുമ്പോൾ(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    എങ്കിൽ(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      എങ്കിൽ(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        തിരികെ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  എങ്കിൽ(!result)
    ftp_state(data, ftpc, FTP_STOR);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_retr(ഘടന Curl_easy *data,
                               ഘടന ftp_conn *ftpc,
                               ഘടന FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  എങ്കിൽ(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    തിരികെ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  എങ്കിൽ(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    എങ്കിൽ(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    അല്ലെങ്കിൽ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      എങ്കിൽ(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        എങ്കിൽ(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    എങ്കിൽ(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      തിരികെ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  അല്ലെങ്കിൽ {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_quote(ഘടന Curl_easy *data,
                                ഘടന ftp_conn *ftpc,
                                ഘടന FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  ഘടന curl_slist *item;

  തിരഞ്ഞെടുപ്പ്(instate) {
  അവസ്ഥ FTP_QUOTE:
  സ്ഥിരസ്ഥിതി:
    item = data->set.quote;
    നിർത്തുക;
  അവസ്ഥ FTP_RETR_PREQUOTE:
  അവസ്ഥ FTP_STOR_PREQUOTE:
  അവസ്ഥ FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    നിർത്തുക;
  അവസ്ഥ FTP_POSTQUOTE:
    item = data->set.postquote;
    നിർത്തുക;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  എങ്കിൽ(init)
    ftpc->count1 = 0;
  അല്ലെങ്കിൽ
    ftpc->count1++;

  എങ്കിൽ(item) {
    പൂർണ്ണസംഖ്യ i = 0;

    /* Skip count1 items in the linked list */
    ആകുമ്പോൾ((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    എങ്കിൽ(item) {
      സ്ഥിരം അക്ഷരം *cmd = item->data;
      എങ്കിൽ(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      അല്ലെങ്കിൽ
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      എങ്കിൽ(result)
        തിരികെ result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  എങ്കിൽ(!quote) {
    /* No more quote to send, continue to ... */
    തിരഞ്ഞെടുപ്പ്(instate) {
    അവസ്ഥ FTP_QUOTE:
    സ്ഥിരസ്ഥിതി:
      result = ftp_state_cwd(data, ftpc, ftp);
      നിർത്തുക;
    അവസ്ഥ FTP_RETR_PREQUOTE:
      എങ്കിൽ(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      അല്ലെങ്കിൽ {
        എങ്കിൽ(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        അല്ലെങ്കിൽ {
          എങ്കിൽ(data->set.ignorecl || data->state.prefer_ascii) {
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
            എങ്കിൽ(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          അല്ലെങ്കിൽ {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            എങ്കിൽ(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      നിർത്തുക;
    അവസ്ഥ FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      നിർത്തുക;
    അവസ്ഥ FTP_POSTQUOTE:
      നിർത്തുക;
    അവസ്ഥ FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      നിർത്തുക;
    }
  }

  തിരികെ result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
സ്ഥിതം CURLcode ftp_epsv_disable(ഘടന Curl_easy *data,
                                 ഘടന ftp_conn *ftpc,
                                 ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  എങ്കിൽ(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_control_addr_dup(ഘടന Curl_easy *data, അക്ഷരം **newhostp)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  അല്ലെങ്കിൽ {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = ശൂന്യ;
    തിരികെ CURLE_FTP_CANT_GET_HOST;
  }
  തിരികെ *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

സ്ഥിതം bool match_pasv_6nums(സ്ഥിരം അക്ഷരം *p,
                             unsigned പൂർണ്ണസംഖ്യ *array) /* 6 numbers */
{
  പൂർണ്ണസംഖ്യ i;
  വേണ്ടി(i = 0; i < 6; i++) {
    curl_off_t അക്കം;
    എങ്കിൽ(i) {
      എങ്കിൽ(*p != ',')
        തിരികെ FALSE;
      p++;
    }
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &അക്കം, 0xff))
      തിരികെ FALSE;
    array[i] = (unsigned പൂർണ്ണസംഖ്യ)അക്കം;
  }
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode ftp_state_pasv_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result;
  സ്ഥിരം ഘടന pingpong *pp = &ftpc->pp;
  അക്ഷരം *newhost = ശൂന്യ;
  unsigned short newport = 0;
  സ്ഥിരം അക്ഷരം *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  എങ്കിൽ((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    സ്ഥിരം അക്ഷരം *ptr = strchr(str, '(');
    എങ്കിൽ(ptr) {
      അക്ഷരം sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      എങ്കിൽ(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        സ്ഥിരം അക്ഷരം *p = &ptr[3];
        curl_off_t അക്കം;
        എങ്കിൽ(curlx_str_സംഖ്യ(&p, &അക്കം, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          തിരികെ CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)അക്കം;
        result = ftp_control_addr_dup(data, &newhost);
        എങ്കിൽ(result)
          തിരികെ result;
      }
      അല്ലെങ്കിൽ
        ptr = ശൂന്യ;
    }
    എങ്കിൽ(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      തിരികെ CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned പൂർണ്ണസംഖ്യ ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ആകുമ്പോൾ(*str) {
      എങ്കിൽ(match_pasv_6nums(str, ip))
        നിർത്തുക;
      str++;
    }

    എങ്കിൽ(!*str) {
      failf(data, "Could not interpret the 227-response");
      തിരികെ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    എങ്കിൽ(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    എങ്കിൽ(!newhost)
      തിരികെ CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    തിരികെ ftp_epsv_disable(data, ftpc, conn);
  }
  അല്ലെങ്കിൽ {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    തിരികെ CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  എങ്കിൽ(result)
    പോകുക error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  എങ്കിൽ(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    എങ്കിൽ(result)
      പോകുക error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  എങ്കിൽ(result) {
    എങ്കിൽ((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    പോകുക error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  എങ്കിൽ(data->set.verbose) {
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
  തിരികെ result;
}

/* called repeatedly until done from multi.c */
സ്ഥിതം CURLcode ftp_statemach(ഘടന Curl_easy *data,
                              ഘടന ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  തിരികെ result;
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
സ്ഥിതം CURLcode ftp_do_more(ഘടന Curl_easy *data, domore *more)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ഘടന FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  എങ്കിൽ(!ftpc || !ftp)
    തിരികെ CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  എങ്കിൽ(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
      തിരികെ result;
    എങ്കിൽ(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      എങ്കിൽ(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        തിരികെ ftp_epsv_disable(data, ftpc, conn);
      }
      തിരികെ result;
    }
  }

  എങ്കിൽ(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    എങ്കിൽ(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    എങ്കിൽ(result || !ftpc->wait_data_conn)
      തിരികെ result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  എങ്കിൽ(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    എങ്കിൽ(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      എങ്കിൽ(result)
        തിരികെ result; /* Failed to accept data connection */

      എങ്കിൽ(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        എങ്കിൽ(result)
          തിരികെ result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      അല്ലെങ്കിൽ {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      എങ്കിൽ(result)
        തിരികെ result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    അല്ലെങ്കിൽ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      എങ്കിൽ(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      എങ്കിൽ(result)
        തിരികെ result;

      എങ്കിൽ((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        എങ്കിൽ(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          എങ്കിൽ(result)
            തിരികെ result;
        }
        /* otherwise fall through */
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        അല്ലെങ്കിൽ {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        എങ്കിൽ(result)
          തിരികെ result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      എങ്കിൽ(complete)
        *more = DOMORE_DONE;
    }
    തിരികെ result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  എങ്കിൽ(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (പൂർണ്ണസംഖ്യ)result);
  }

  തിരികെ result;
}

/* call this when the DO phase has completed */
സ്ഥിതം CURLcode ftp_dophase_done(ഘടന Curl_easy *data,
                                 ഘടന ftp_conn *ftpc,
                                 ഘടന FTP *ftp,
                                 bool connected)
{
  എങ്കിൽ(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    എങ്കിൽ(result) {
      close_secondarysocket(data, ftpc);
      തിരികെ result;
    }
  }

  എങ്കിൽ(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  അല്ലെങ്കിൽ എങ്കിൽ(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ftp_state_port_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    ഘടന FTP *ftp,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  ഘടന connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  എങ്കിൽ(ftpcode / 100 != 2) {
    /* the command failed */

    എങ്കിൽ(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    എങ്കിൽ(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    അല്ലെങ്കിൽ
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  അല്ലെങ്കിൽ {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  തിരികെ result;
}

/* return TRUE on error, FALSE on success */
സ്ഥിതം bool twodigit(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ *val)
{
  എങ്കിൽ(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    തിരികെ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  തിരികെ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ *year, പൂർണ്ണസംഖ്യ *month, പൂർണ്ണസംഖ്യ *day,
                           പൂർണ്ണസംഖ്യ *hour, പൂർണ്ണസംഖ്യ *minute, പൂർണ്ണസംഖ്യ *second);
UNITTEST bool ftp_213_date(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ *year, പൂർണ്ണസംഖ്യ *month, പൂർണ്ണസംഖ്യ *day,
                           പൂർണ്ണസംഖ്യ *hour, പൂർണ്ണസംഖ്യ *minute, പൂർണ്ണസംഖ്യ *second)
{
  പൂർണ്ണസംഖ്യ century;
  എങ്കിൽ((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    തിരികെ FALSE;

  *year += century * 100;
  എങ്കിൽ((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    തിരികെ FALSE;
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode client_write_header(ഘടന Curl_easy *data,
                                    അക്ഷരം *buf, size_t blen)
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
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_mdtm_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    ഘടന FTP *ftp,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(ftpcode) {
  അവസ്ഥ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    പൂർണ്ണസംഖ്യ year, month, day, hour, minute, second;
    ഘടന pingpong *pp = &ftpc->pp;
    സ്ഥിരം അക്ഷരം *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    എങ്കിൽ(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      അക്ഷരം timebuf[24];
      curl_msnprintf(timebuf, വലുപ്പം(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      എങ്കിൽ(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#എങ്കിൽ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic തള്ളുക
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    എങ്കിൽ(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#എങ്കിൽ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic എടുക്കുക
#endif
      അക്ഷരം headerbuf[128];
      പൂർണ്ണസംഖ്യ headerbuflen;
      time_t filetime = data->info.filetime;
      ഘടന tm buffer;
      സ്ഥിരം ഘടന tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      എങ്കിൽ(result)
        തിരികെ result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, വലുപ്പം(headerbuf),
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
      എങ്കിൽ(result)
        തിരികെ result;
    } /* end of a ridiculous amount of conditionals */
  }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    infof(data, "unsupported MDTM reply format");
    നിർത്തുക;
  അവസ്ഥ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    നിർത്തുക;
  }

  എങ്കിൽ(data->set.timecondition) {
    എങ്കിൽ((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      തിരഞ്ഞെടുപ്പ്(data->set.timecondition) {
      അവസ്ഥ CURL_TIMECOND_IFMODSINCE:
      സ്ഥിരസ്ഥിതി:
        എങ്കിൽ(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          തിരികെ CURLE_OK;
        }
        നിർത്തുക;
      അവസ്ഥ CURL_TIMECOND_IFUNMODSINCE:
        എങ്കിൽ(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          തിരികെ CURLE_OK;
        }
        നിർത്തുക;
      } /* switch */
    }
    അല്ലെങ്കിൽ {
      infof(data, "Skipping time comparison");
    }
  }

  എങ്കിൽ(!result)
    result = ftp_state_type(data, ftpc, ftp);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_type_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    ഘടന FTP *ftp,
                                    പൂർണ്ണസംഖ്യ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    തിരികെ CURLE_FTP_COULDNT_SET_TYPE;
  }
  എങ്കിൽ(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  എങ്കിൽ(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_size_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    ഘടന FTP *ftp,
                                    പൂർണ്ണസംഖ്യ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  സ്ഥിരം അക്ഷരം *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  എങ്കിൽ(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    സ്ഥിരം അക്ഷരം *start = &buf[4];
    സ്ഥിരം അക്ഷരം *fdigit = memchr(start, '\r', len - 4);
    എങ്കിൽ(fdigit) {
      fdigit--;
      എങ്കിൽ(*fdigit == '\n')
        fdigit--;
      ആകുമ്പോൾ(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    അല്ലെങ്കിൽ
      fdigit = start;
    എങ്കിൽ(curlx_str_സംഖ്യ(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  എങ്കിൽ(instate == FTP_SIZE) {
    എങ്കിൽ(filesize != -1) {
      അക്ഷരം clbuf[128];
      പൂർണ്ണസംഖ്യ clbuflen = curl_msnprintf(clbuf, വലുപ്പം(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_rest_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    ഘടന FTP *ftp,
                                    പൂർണ്ണസംഖ്യ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(instate) {
  അവസ്ഥ FTP_REST:
  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(ftpcode == 350) {
      അക്ഷരം buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      എങ്കിൽ(result)
        തിരികെ result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    നിർത്തുക;

  അവസ്ഥ FTP_RETR_REST:
    എങ്കിൽ(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    അല്ലെങ്കിൽ {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    നിർത്തുക;
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_state_stor_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    തിരികെ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  എങ്കിൽ(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      തിരികെ ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  തിരികെ ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
സ്ഥിതം CURLcode ftp_state_get_resp(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc,
                                   ഘടന FTP *ftp,
                                   പൂർണ്ണസംഖ്യ ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ((ftpcode == 150) || (ftpcode == 125)) {

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

    എങ്കിൽ((instate != FTP_LIST) &&
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
      എങ്കിൽ(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        വേണ്ടി(i = 0; i < len - 7; i++) {
          curl_off_t what;
          സ്ഥിരം അക്ഷരം *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          സ്ഥിരം അക്ഷരം *c = &buf[i];
          എങ്കിൽ(!curlx_str_സംഖ്യ(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            നിർത്തുക;
          }
        }
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    എങ്കിൽ(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    അല്ലെങ്കിൽ എങ്കിൽ((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    എങ്കിൽ(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    എങ്കിൽ(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      എങ്കിൽ(result)
        തിരികെ result;

      എങ്കിൽ(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        തിരികെ ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    തിരികെ ftp_initiate_transfer(data, ftpc);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    അല്ലെങ്കിൽ {
      failf(data, "RETR response: %03d", ftpcode);
      തിരികെ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  തിരികെ result;
}

/* after USER, PASS and ACCT */
സ്ഥിതം CURLcode ftp_state_loggedin(ഘടന Curl_easy *data,
                                   ഘടന ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  അല്ലെങ്കിൽ {
    result = ftp_state_pwd(data, ftpc);
  }
  തിരികെ result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
സ്ഥിതം bool ftp_has_ctrl(സ്ഥിരം അക്ഷരം *string)
{
  സ്ഥിരം unsigned അക്ഷരം *s = (സ്ഥിരം unsigned അക്ഷരം *)string;
  ആകുമ്പോൾ(*s) {
    എങ്കിൽ(*s < 0x20)
      തിരികെ TRUE;
    s++;
  }
  തിരികെ FALSE;
}

/* for USER and PASS responses */
സ്ഥിതം CURLcode ftp_state_user_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  എങ്കിൽ((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ftpcode == 332) {
    സ്ഥിരം അക്ഷരം *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    എങ്കിൽ(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    അല്ലെങ്കിൽ {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  അല്ലെങ്കിൽ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    സ്ഥിരം അക്ഷരം *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    എങ്കിൽ(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      എങ്കിൽ(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      അല്ലെങ്കിൽ {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        എങ്കിൽ(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    അല്ലെങ്കിൽ {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  തിരികെ result;
}

/* for ACCT response */
സ്ഥിതം CURLcode ftp_state_acct_resp(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc,
                                    പൂർണ്ണസംഖ്യ ftpcode)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  അല്ലെങ്കിൽ
    result = ftp_state_loggedin(data, ftpc);

  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_pwd_resp(ഘടന Curl_easy *data,
                             ഘടന ftp_conn *ftpc,
                             പൂർണ്ണസംഖ്യ ftpcode)
{
  ഘടന pingpong *pp = &ftpc->pp;
  CURLcode result;

  എങ്കിൽ(ftpcode == 257) {
    സ്ഥിരം അക്ഷരം *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    ഘടന dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ആകുമ്പോൾ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    എങ്കിൽ('\"' == *ptr) {
      /* it started good */
      വേണ്ടി(ptr++; *ptr; ptr++) {
        എങ്കിൽ('\"' == *ptr) {
          എങ്കിൽ('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          അല്ലെങ്കിൽ {
            /* end of path */
            എങ്കിൽ(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            നിർത്തുക; /* get out of this loop */
          }
        }
        അല്ലെങ്കിൽ {
          എങ്കിൽ(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            തിരികെ CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        എങ്കിൽ(result) {
          curlx_dyn_free(&out);
          തിരികെ result;
        }
      }
    }
    എങ്കിൽ(entry_extracted) {
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
      അക്ഷരം *dir = curlx_dyn_ptr(&out);

      എങ്കിൽ(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        എങ്കിൽ(result) {
          curlx_dyn_free(&out);
          തിരികെ result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      എങ്കിൽ(!data->state.most_recent_ftp_entrypath)
        തിരികെ CURLE_OUT_OF_MEMORY;

      എങ്കിൽ(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        തിരികെ CURLE_OK;
      }
    }
    അല്ലെങ്കിൽ {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം ftpauth[] = { "SSL", "TLS" };

സ്ഥിതം CURLcode ftp_wait_resp(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn,
                              ഘടന ftp_conn *ftpc,
                              പൂർണ്ണസംഖ്യ ftpcode)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    എങ്കിൽ(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      തിരികെ ftp_state_user_resp(data, ftpc, ftpcode);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  എങ്കിൽ(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    തിരഞ്ഞെടുപ്പ്((long)data->set.ftpsslauth) {
    അവസ്ഥ CURLFTPAUTH_DEFAULT:
    അവസ്ഥ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      നിർത്തുക;
    അവസ്ഥ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (പൂർണ്ണസംഖ്യ)data->set.ftpsslauth);
      തിരികെ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  അല്ലെങ്കിൽ
    result = ftp_state_user(data, ftpc, conn);
  തിരികെ result;
}

/* an implicit TLS scheme implies TLS on both connections */
സ്ഥിതം unsigned അക്ഷരം ftp_use_ssl(ഘടന connectdata *conn,
                                 ഘടന ftp_conn *ftpc)
{
  എങ്കിൽ(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    തിരികെ CURLUSESSL_ALL;
  തിരികെ ftpc->use_ssl;
}

സ്ഥിതം CURLcode ftp_pp_statemachine(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ ftpcode;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ഘടന FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ഘടന pingpong *pp;
  size_t nread = 0;

  എങ്കിൽ(!ftpc || !ftp)
    തിരികെ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  എങ്കിൽ(pp->sendleft)
    തിരികെ Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  എങ്കിൽ(result || !ftpcode)
    തിരികെ result;

  /* we have now received a full FTP server response */
  തിരഞ്ഞെടുപ്പ്(ftpc->state) {
  അവസ്ഥ FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    എങ്കിൽ(pp->overflow)
      തിരികെ CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    എങ്കിൽ((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      എങ്കിൽ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        എങ്കിൽ(result) {
          /* we failed and bail out */
          തിരികെ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      എങ്കിൽ(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      അല്ലെങ്കിൽ
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    നിർത്തുക;

  അവസ്ഥ FTP_USER:
  അവസ്ഥ FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    എങ്കിൽ(!result)
      ftp_state(data, ftpc, FTP_PROT);
    നിർത്തുക;

  അവസ്ഥ FTP_PROT:
    എങ്കിൽ(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    അല്ലെങ്കിൽ എങ്കിൽ(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      തിരികെ CURLE_USE_SSL_FAILED;

    എങ്കിൽ(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      എങ്കിൽ(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    അല്ലെങ്കിൽ
      result = ftp_state_pwd(data, ftpc);
    നിർത്തുക;

  അവസ്ഥ FTP_CCC:
    എങ്കിൽ(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned അക്ഷരം)CURLFTPSSL_CCC_ACTIVE));
      എങ്കിൽ(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    എങ്കിൽ(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    നിർത്തുക;

  അവസ്ഥ FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_SYST:
    എങ്കിൽ(ftpcode == 215) {
      സ്ഥിരം അക്ഷരം *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      സ്ഥിരം അക്ഷരം *start;
      അക്ഷരം *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ആകുമ്പോൾ(*ptr == ' ')
        ptr++;
      വേണ്ടി(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      എങ്കിൽ(!os)
        തിരികെ CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      എങ്കിൽ(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        എങ്കിൽ(result) {
          curlx_free(os);
          തിരികെ result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        നിർത്തുക;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    അല്ലെങ്കിൽ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    നിർത്തുക;

  അവസ്ഥ FTP_NAMEFMT:
    എങ്കിൽ(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      നിർത്തുക;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    നിർത്തുക;

  അവസ്ഥ FTP_QUOTE:
  അവസ്ഥ FTP_POSTQUOTE:
  അവസ്ഥ FTP_RETR_PREQUOTE:
  അവസ്ഥ FTP_STOR_PREQUOTE:
  അവസ്ഥ FTP_LIST_PREQUOTE:
    എങ്കിൽ((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    അല്ലെങ്കിൽ
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    നിർത്തുക;

  അവസ്ഥ FTP_CWD:
    എങ്കിൽ(ftpcode / 100 != 2) {
      /* failure to CWD there */
      എങ്കിൽ(data->set.ftp_create_missing_dirs &&
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
        എങ്കിൽ(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      അല്ലെങ്കിൽ {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    അല്ലെങ്കിൽ {
      /* success */
      ftpc->count2 = 0;
      എങ്കിൽ(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      അല്ലെങ്കിൽ {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    നിർത്തുക;

  അവസ്ഥ FTP_MKD:
    എങ്കിൽ((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    അല്ലെങ്കിൽ {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    നിർത്തുക;

  അവസ്ഥ FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_TYPE:
  അവസ്ഥ FTP_LIST_TYPE:
  അവസ്ഥ FTP_RETR_TYPE:
  അവസ്ഥ FTP_STOR_TYPE:
  അവസ്ഥ FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    നിർത്തുക;

  അവസ്ഥ FTP_SIZE:
  അവസ്ഥ FTP_RETR_SIZE:
  അവസ്ഥ FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    നിർത്തുക;

  അവസ്ഥ FTP_REST:
  അവസ്ഥ FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    നിർത്തുക;

  അവസ്ഥ FTP_PRET:
    എങ്കിൽ(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      തിരികെ CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    നിർത്തുക;

  അവസ്ഥ FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_LIST:
  അവസ്ഥ FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    നിർത്തുക;

  അവസ്ഥ FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    നിർത്തുക;

  അവസ്ഥ FTP_QUIT:
  സ്ഥിരസ്ഥിതി:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    നിർത്തുക;
  }

  തിരികെ result;
}

/* called repeatedly until done from multi.c */
സ്ഥിതം CURLcode ftp_multi_statemach(ഘടന Curl_easy *data,
                                    bool *done)
{
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  തിരികെ ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

സ്ഥിതം CURLcode ftp_block_statemach(ഘടന Curl_easy *data,
                                    ഘടന ftp_conn *ftpc)
{
  ഘടന pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(ftpc->state != FTP_STOP) {
    എങ്കിൽ(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    എങ്കിൽ(result)
      നിർത്തുക;
  }

  തിരികെ result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
സ്ഥിതം CURLcode ftp_connect(ഘടന Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  ഘടന connectdata *conn = data->conn;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ഘടന pingpong *pp;

  *done = FALSE; /* default to not done yet */
  എങ്കിൽ(!ftpc)
    തിരികെ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  എങ്കിൽ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  തിരികെ result;
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
സ്ഥിതം CURLcode ftp_sendquote(ഘടന Curl_easy *data,
                              ഘടന ftp_conn *ftpc,
                              ഘടന curl_slist *quote)
{
  ഘടന curl_slist *item;
  ഘടന pingpong *pp = &ftpc->pp;

  item = quote;
  ആകുമ്പോൾ(item) {
    എങ്കിൽ(item->data) {
      size_t nread;
      സ്ഥിരം അക്ഷരം *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      പൂർണ്ണസംഖ്യ ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      എങ്കിൽ(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      എങ്കിൽ(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      എങ്കിൽ(result)
        തിരികെ result;

      എങ്കിൽ(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        തിരികെ CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ftp_done_status(ഘടന Curl_easy *data,
                                ഘടന ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  തിരഞ്ഞെടുപ്പ്(status) {
  അവസ്ഥ CURLE_BAD_DOWNLOAD_RESUME:
  അവസ്ഥ CURLE_FTP_WEIRD_PASV_REPLY:
  അവസ്ഥ CURLE_FTP_PORT_FAILED:
  അവസ്ഥ CURLE_FTP_ACCEPT_FAILED:
  അവസ്ഥ CURLE_FTP_ACCEPT_TIMEOUT:
  അവസ്ഥ CURLE_FTP_COULDNT_SET_TYPE:
  അവസ്ഥ CURLE_FTP_COULDNT_RETR_FILE:
  അവസ്ഥ CURLE_PARTIAL_FILE:
  അവസ്ഥ CURLE_UPLOAD_FAILED:
  അവസ്ഥ CURLE_REMOTE_ACCESS_DENIED:
  അവസ്ഥ CURLE_FILESIZE_EXCEEDED:
  അവസ്ഥ CURLE_REMOTE_FILE_NOT_FOUND:
  അവസ്ഥ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  അവസ്ഥ CURLE_OK: /* does not affect the control connection's status */
    എങ്കിൽ(!premature)
      നിർത്തുക;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  സ്ഥിരസ്ഥിതി:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    തിരികെ status;      /* use the already set error code */
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം ftp_done_wildcard(ഘടന Curl_easy *data, ഘടന ftp_conn *ftpc)
{
  എങ്കിൽ(data->state.wildcardmatch) {
    എങ്കിൽ(data->set.chunk_end && ftpc->file) {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

സ്ഥിതം ശൂന്യം ftp_done_path(ഘടന Curl_easy *data, ഘടന ftp_conn *ftpc,
                          CURLcode result)
{
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  അല്ലെങ്കിൽ { /* remember working directory for connection reuse */
    സ്ഥിരം അക്ഷരം *rawPath = ftpc->rawpath;
    എങ്കിൽ(rawPath) {
      എങ്കിൽ((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      അല്ലെങ്കിൽ {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        എങ്കിൽ(!ftpc->cwdfail) {
          എങ്കിൽ(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          അല്ലെങ്കിൽ
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        അല്ലെങ്കിൽ
          ftpc->prevpath = ശൂന്യ; /* no path */
      }
    }
    എങ്കിൽ(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

സ്ഥിതം CURLcode ftp_done_secondary_socket(ഘടന Curl_easy *data,
                                          ഘടന ftp_conn *ftpc,
                                          CURLcode result)
{
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    എങ്കിൽ(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      എങ്കിൽ(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_done_control_reply(ഘടന Curl_easy *data,
                                       ഘടന ftp_conn *ftpc,
                                       ഘടന FTP *ftp, CURLcode result,
                                       bool premature)
{
  ഘടന connectdata *conn = data->conn;
  size_t nread;
  പൂർണ്ണസംഖ്യ ftpcode;

  എങ്കിൽ(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    എങ്കിൽ(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      തിരികെ result;
    }

    എങ്കിൽ(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      തിരഞ്ഞെടുപ്പ്(ftpcode) {
      അവസ്ഥ 226:
      അവസ്ഥ 250:
        നിർത്തുക;
      അവസ്ഥ 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        നിർത്തുക;
      }
    }
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ftp_done_check_partial(ഘടന Curl_easy *data,
                                       ഘടന ftp_conn *ftpc,
                                       ഘടന FTP *ftp, CURLcode result,
                                       bool premature)
{
  എങ്കിൽ(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  അല്ലെങ്കിൽ എങ്കിൽ(data->state.upload) {
    എങ്കിൽ((ftp->transfer == PPTRANSFER_BODY) &&
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
  അല്ലെങ്കിൽ {
    എങ്കിൽ((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  തിരികെ result;
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
സ്ഥിതം CURLcode ftp_done(ഘടന Curl_easy *data, CURLcode status,
                         bool premature)
{
  ഘടന FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  എങ്കിൽ(!ftp || !ftpc)
    തിരികെ CURLE_OK;

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
  എങ്കിൽ(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
സ്ഥിതം CURLcode ftp_nb_type(ഘടന Curl_easy *data,
                            ഘടന ftp_conn *ftpc,
                            ഘടന FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  അക്ഷരം want = (അക്ഷരം)(ascii ? 'A' : 'I');

  എങ്കിൽ(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    തിരികെ ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  എങ്കിൽ(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  തിരികെ result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
സ്ഥിതം CURLcode ftp_perform(
  ഘടന Curl_easy *data,
  ഘടന ftp_conn *ftpc,
  ഘടന FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  എങ്കിൽ(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  എങ്കിൽ(result)
    തിരികെ result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  എങ്കിൽ(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  അല്ലെങ്കിൽ
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  എങ്കിൽ(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം wc_data_dtor(ശൂന്യം *ptr)
{
  ഘടന ftp_wc *ftpwc = ptr;
  എങ്കിൽ(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

സ്ഥിതം CURLcode init_wc_data(ഘടന Curl_easy *data,
                             ഘടന ftp_conn *ftpc,
                             ഘടന FTP *ftp)
{
  അക്ഷരം *last_slash;
  അക്ഷരം *path = ftp->path;
  ഘടന WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  ഘടന ftp_wc *ftpwc = ശൂന്യ;

  last_slash = strrchr(ftp->path, '/');
  എങ്കിൽ(last_slash) {
    last_slash++;
    എങ്കിൽ(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      തിരികെ ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    എങ്കിൽ(!wildcard->pattern)
      തിരികെ CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  അല്ലെങ്കിൽ { /* there is only 'wildcard pattern' or nothing */
    എങ്കിൽ(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      എങ്കിൽ(!wildcard->pattern)
        തിരികെ CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    അല്ലെങ്കിൽ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      തിരികെ ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, വലുപ്പം(ഘടന ftp_wc));
  എങ്കിൽ(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  എങ്കിൽ(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  എങ്കിൽ(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  എങ്കിൽ(result) {
    പോകുക fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  എങ്കിൽ(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  infof(data, "Wildcard - Parsing started");
  തിരികെ CURLE_OK;

fail:
  എങ്കിൽ(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ശൂന്യ;
  തിരികെ result;
}

സ്ഥിതം CURLcode wc_statemach(ഘടന Curl_easy *data,
                             ഘടന ftp_conn *ftpc,
                             ഘടന FTP *ftp)
{
  ഘടന WildcardData * സ്ഥിരം wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  വേണ്ടി(;;) {
    തിരഞ്ഞെടുപ്പ്(wildcard->state) {
    അവസ്ഥ CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      എങ്കിൽ(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        തിരികെ result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      തിരികെ result;

    അവസ്ഥ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ഘടന ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      എങ്കിൽ(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        തുടരുക;
      }
      എങ്കിൽ(Curl_llist_എണ്ണം(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;
      }
      തുടരുക;
    }

    അവസ്ഥ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ഘടന Curl_llist_കണ്ണി *head = Curl_llist_head(&wildcard->filelist);
      ഘടന curl_fileinfo *finfo = Curl_കണ്ണി_elem(head);
      അക്ഷരം *tmp_path;
      അക്ഷരം *enc = curl_easy_escape(ശൂന്യ, finfo->filename, 0);
      എങ്കിൽ(!enc)
        തിരികെ CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      എങ്കിൽ(!tmp_path)
        തിരികെ CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      എങ്കിൽ(data->set.chunk_bgn) {
        long userresponse;
        ഘടന Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (പൂർണ്ണസംഖ്യ)Curl_llist_എണ്ണം(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        തിരഞ്ഞെടുപ്പ്(userresponse) {
        അവസ്ഥ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          തുടരുക;
        അവസ്ഥ CURL_CHUNK_BGN_FUNC_FAIL:
          തിരികെ CURLE_CHUNK_FAILED;
        }
      }

      എങ്കിൽ(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        തുടരുക;
      }

      എങ്കിൽ(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      എങ്കിൽ(result)
        തിരികെ result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_കണ്ണി_remove(Curl_llist_head(&wildcard->filelist));

      എങ്കിൽ(Curl_llist_എണ്ണം(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        തിരികെ CURLE_OK;
      }
      തിരികെ result;
    }

    അവസ്ഥ CURLWC_SKIP: {
      എങ്കിൽ(data->set.chunk_end) {
        ഘടന Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_കണ്ണി_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_എണ്ണം(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      തുടരുക;
    }

    അവസ്ഥ CURLWC_CLEAN: {
      ഘടന ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      എങ്കിൽ(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      തിരികെ result;
    }

    അവസ്ഥ CURLWC_DONE:
    അവസ്ഥ CURLWC_ERROR:
    അവസ്ഥ CURLWC_CLEAR:
      എങ്കിൽ(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ശൂന്യ;
      }
      തിരികെ result;
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
സ്ഥിതം CURLcode ftp_regular_transfer(ഘടന Curl_easy *data,
                                     ഘടന ftp_conn *ftpc,
                                     ഘടന FTP *ftp,
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

  എങ്കിൽ(!result) {

    എങ്കിൽ(!*dophase_done)
      /* the DO phase has not completed yet */
      തിരികെ CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
    freedirs(ftpc);

  തിരികെ result;
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
സ്ഥിതം CURLcode ftp_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ഘടന FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  എങ്കിൽ(!ftpc || !ftp)
    തിരികെ CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ഘടന Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    എങ്കിൽ(result)
      തിരികെ result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    എങ്കിൽ(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      തിരികെ result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  എങ്കിൽ(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    എങ്കിൽ(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      തിരികെ CURLE_OK;
    }
    എങ്കിൽ(result) /* error, loop or skipping the file */
      തിരികെ result;
  }
  അല്ലെങ്കിൽ { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  തിരികെ result;
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
സ്ഥിതം CURLcode ftp_quit(ഘടന Curl_easy *data,
                         ഘടന ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    എങ്കിൽ(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      തിരികെ result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
സ്ഥിതം CURLcode ftp_disconnect(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               bool dead_connection)
{
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  എങ്കിൽ(!ftpc)
    തിരികെ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  എങ്കിൽ(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (ശൂന്യം)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  തിരികെ CURLE_OK;
}

/* called from multi.c while DOing */
സ്ഥിതം CURLcode ftp_doing(ഘടന Curl_easy *data,
                          bool *dophase_done)
{
  ഘടന ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ഘടന FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  എങ്കിൽ(!ftpc || !ftp)
    തിരികെ CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  എങ്കിൽ(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  അല്ലെങ്കിൽ എങ്കിൽ(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം ftp_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന FTP *ftp = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

സ്ഥിതം ശൂന്യം ftp_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ftp_conn *ftpc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

സ്ഥിതം ശൂന്യം type_url_check(ഘടന Curl_easy *data, ഘടന FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  എങ്കിൽ((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    അക്ഷരം *type = &ftp->path[len - 7];
    അക്ഷരം command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    തിരഞ്ഞെടുപ്പ്(command) {
    അവസ്ഥ 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      നിർത്തുക;

    അവസ്ഥ 'D': /* directory mode */
      data->state.list_only = TRUE;
      നിർത്തുക;

    അവസ്ഥ 'I': /* binary mode */
    സ്ഥിരസ്ഥിതി:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      നിർത്തുക;
    }
  }
}

സ്ഥിതം CURLcode ftp_setup_connection(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  ഘടന FTP *ftp;
  CURLcode result = CURLE_OK;
  ഘടന ftp_conn *ftpc;

  ftp = curlx_calloc(1, വലുപ്പം(*ftp));
  എങ്കിൽ(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, വലുപ്പം(*ftpc));
  എങ്കിൽ(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  എങ്കിൽ(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    എങ്കിൽ(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  എങ്കിൽ(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    എങ്കിൽ(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      തിരികെ CURLE_OUT_OF_MEMORY;
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
               (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

bool Curl_ftp_conns_match(ഘടന connectdata *needle, ഘടന connectdata *conn)
{
  ഘടന ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ഘടന ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  എങ്കിൽ(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    തിരികെ FALSE;

  തിരഞ്ഞെടുപ്പ്(nftpc->use_ssl) {
  അവസ്ഥ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    തിരികെ (cftpc->use_ssl != CURLUSESSL_NONE);
  സ്ഥിരസ്ഥിതി:
    തിരികെ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_ftp = {
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
