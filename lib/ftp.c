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
#include "curl_setup.h"
#include "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "sendf.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "if2ip.h"
#include "progress.h"
#include "transfer.h"
#include "escape.h"
#include "ftp.h"
#include "ftp-int.h"
#include "ftplistparser.h"
#include "curl_range.h"
#include "strcase.h"
#include "vtls/vtls.h"
#include "cfilters.h"
#include "cf-socket.h"
#include "connect.h"
#include "curlx/inet_ntop.h"
#include "curlx/inet_pton.h"
#include "select.h"
#include "parsedate.h" /* for the week day and month names */
#include "sockaddr.h" /* required for Curl_sockaddr_storage */
#include "multiif.h"
#include "url.h"
#include "http_proxy.h"
#include "vdns/hostip.h"
#include "curlx/strdup.h"
#include "curlx/strerr.h"
#include "curlx/strparse.h"
#include "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(line) \
  (ISDIGIT((line)[0]) && ISDIGIT((line)[1]) && ISDIGIT((line)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(line) (STATUSCODE(line) && (' ' == (line)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
static const var_na * const ftp_state_names[] = {
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
static void ftp_state_low(sa_nracanaa Curl_easy *data,
                          sa_nracanaa ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , puur_naa_nka lineno
#endif
  )
{
  ᬬᬾᬦ᭄(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((void)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#ᬮᬶᬬ
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_low(x, y, z)
#ᬮᬶᬬ /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_low(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

static CURLcode ftp_state_mdtm(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp);
static CURLcode ftp_state_quote(sa_nracanaa Curl_easy *data,
                                sa_nracanaa ftp_conn *ftpc,
                                sa_nracanaa FTP *ftp,
                                bool init, ftpstate instate);
static CURLcode ftp_nb_type(sa_nracanaa Curl_easy *data,
                            sa_nracanaa ftp_conn *ftpc,
                            sa_nracanaa FTP *ftp,
                            bool ascii, ftpstate newstate);
static CURLcode getftpresponse(sa_nracanaa Curl_easy *data, size_t *nreadp,
                               puur_naa_nka *ftpcodep);

static void freedirs(sa_nracanaa ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = NULL;
}

static size_t numof_slashes(const var_na *str)
{
  const var_na *slashPos;
  size_t num = 0;
  do {
    slashPos = strchr(str, '/');
    ᬬᬾᬦ᭄(slashPos) {
      ++num;
      str = slashPos + 1;
    }
  } ᬲᭂᬤᭂᬮ(slashPos);
  ᬯᬮᬶ num;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
static CURLcode ftp_parse_url_path(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc,
                                   sa_nracanaa FTP *ftp)
{
  const var_na *slashPos = NULL;
  const var_na *fileName = NULL;
  CURLcode result = CURLE_OK;
  const var_na *rawPath = NULL; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ᬬᬾᬦ᭄(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  ᬬᬾᬦ᭄(result) {
    failf(data, "path contains control characters");
    ᬯᬮᬶ result;
  }
  rawPath = ftpc->rawpath;

  switch(data->set.ftp_filemethod) {
  pak_sa FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ᬬᬾᬦ᭄((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    bha_nga;

  pak_sa FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    ᬬᬾᬦ᭄(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      ᬬᬾᬦ᭄(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, parimaa_na(ftpc->dirs[0]));
      ᬬᬾᬦ᭄(!ftpc->dirs)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (puur_naa_nka)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ᬮᬶᬬ
      fileName = rawPath; /* filename only (or empty) */
    bha_nga;

  default: /* allow pretty much anything */
  pak_sa FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    const var_na *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    ᬬᬾᬦ᭄(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      ᬯᬮᬶ CURLE_URL_MALFORMAT;

    ᬬᬾᬦ᭄(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, parimaa_na(ftpc->dirs[0]));
      ᬬᬾᬦ᭄(!ftpc->dirs)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      ᬲᭂᬤᭂᬮ(dirAlloc--) {
        const var_na *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ᬬᬾᬦ᭄(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ᬬᬾᬦ᭄(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (puur_naa_nka)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (puur_naa_nka)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    bha_nga;
  } /* switch */

  ᬬᬾᬦ᭄(fileName && *fileName)
    ftpc->file = fileName;
  ᬮᬶᬬ
    ftpc->file = NULL; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ᬬᬾᬦ᭄(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    ᬯᬮᬶ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ᬬᬾᬦ᭄((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ᬮᬶᬬ { /* newly created FTP connections are already in entry path */
    const var_na *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    ᬬᬾᬦ᭄(oldPath) {
      size_t n = pathLen;
      ᬬᬾᬦ᭄(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ᬮᬶᬬ
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      ᬬᬾᬦ᭄((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ᬯᬮᬶ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
static puur_naa_nka ftp_need_type(sa_nracanaa ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ᬯᬮᬶ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

static void close_secondarysocket(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ftp_conn *ftpc)
{
  (void)ftpc;
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
sa_nracanaa ftp_cw_lc_ctx {
  sa_nracanaa Curl_cwriter super;
  bool newline_pending;
};

static CURLcode ftp_cw_lc_write(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_cwriter *writer, puur_naa_nka type,
                                const var_na *buf, size_t blen)
{
  static const var_na nl = '\n';
  sa_nracanaa ftp_cw_lc_ctx *ctx = writer->ctx;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ᬬᬾᬦ᭄(!ftpc)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬬᬾᬦ᭄(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ᬯᬮᬶ Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ᬲᭂᬤᭂᬮ(blen) {
    /* do not pass EOS when writing parts */
    puur_naa_nka chunk_type = (type & ~CLIENTWRITE_EOS);
    const var_na *cp;
    size_t chunk_len;
    CURLcode result;

    ᬬᬾᬦ᭄(ctx->newline_pending) {
      ᬬᬾᬦ᭄(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        ᬬᬾᬦ᭄(result)
          ᬯᬮᬶ result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ᬬᬾᬦ᭄(!cp)
      bha_nga;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ᬬᬾᬦ᭄(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ᬬᬾᬦ᭄(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ᬯᬮᬶ Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ᬬᬾᬦ᭄(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ᬯᬮᬶ Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ᬯᬮᬶ Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  ᬯᬮᬶ CURLE_OK;
}

static const sa_nracanaa Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  NULL,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  parimaa_na(sa_nracanaa ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
static CURLcode ftp_check_ctrl_on_data_wait(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ftp_conn *ftpc)
{
  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  sa_nracanaa pingpong *pp = &ftpc->pp;
  size_t nread;
  puur_naa_nka ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ᬬᬾᬦ᭄(curlx_dyn_len(&pp->recvbuf)) {
    const var_na *l = curlx_dyn_ptr(&pp->recvbuf);
    ᬬᬾᬦ᭄(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (void)getftpresponse(data, &nread, &ftpcode);
      ᬯᬮᬶ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ᬬᬾᬦ᭄(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ᬮᬶᬬ {
    puur_naa_nka socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    switch(socketstate) {
    pak_sa -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      ᬯᬮᬶ CURLE_FTP_ACCEPT_FAILED;
    default:
      ᬬᬾᬦ᭄(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      bha_nga;
    }
  }

  ᬬᬾᬦ᭄(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    ᬬᬾᬦ᭄(pp->overflow > 3) {
      const var_na *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ᬬᬾᬦ᭄((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        ᬬᬾᬦ᭄(!curlx_str_number(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          ᬯᬮᬶ CURLE_OK;
        }
      }
    }

    (void)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    ᬬᬾᬦ᭄(ftpcode / 100 > 3)
      ᬯᬮᬶ CURLE_FTP_ACCEPT_FAILED;

    ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY;
  }

  ᬯᬮᬶ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
static CURLcode ftp_initiate_transfer(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  ᬬᬾᬦ᭄(result || !connected)
    ᬯᬮᬶ result;

  ᬬᬾᬦ᭄(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  ᬮᬶᬬ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  ᬯᬮᬶ CURLE_OK;
}

static bool ftp_endofresp(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                          const var_na *line, size_t len, puur_naa_nka *code)
{
  curl_off_t status;
  (void)data;
  (void)conn;

  ᬬᬾᬦ᭄((len > 3) && LASTLINE(line) && !curlx_str_number(&line, &status, 999)) {
    *code = (puur_naa_nka)status;
    ᬯᬮᬶ TRUE;
  }

  ᬯᬮᬶ FALSE;
}

static CURLcode ftp_readresp(sa_nracanaa Curl_easy *data,
                             sa_nracanaa ftp_conn *ftpc,
                             int8_t sockindex,
                             sa_nracanaa pingpong *pp,
                             puur_naa_nka *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  puur_naa_nka code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ᬬᬾᬦ᭄(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  ᬬᬾᬦ᭄(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    ᬯᬮᬶ CURLE_OPERATION_TIMEDOUT;
  }

  ᬯᬮᬶ result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
static CURLcode getftpresponse(sa_nracanaa Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               puur_naa_nka *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  sa_nracanaa pingpong *pp = &ftpc->pp;
  size_t nread;
  puur_naa_nka cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ᬬᬾᬦ᭄(!ftpc)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬲᭂᬤᭂᬮ(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    ᬬᬾᬦ᭄(timeout <= 0) {
      failf(data, "FTP response timeout");
      ᬯᬮᬶ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ᬬᬾᬦ᭄(timeout < interval_ms)
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

    ᬬᬾᬦ᭄(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      puur_naa_nka ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ᬬᬾᬦ᭄(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ᬯᬮᬶ CURLE_RECV_ERROR;
      }
      ᬮᬶᬬ ᬬᬾᬦ᭄(ev == 0) {
        result = Curl_pgrsUpdate(data);
        continue; /* continue in our loop for the timeout duration */
      }
    }

    ᬬᬾᬦ᭄(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      ᬬᬾᬦ᭄(result)
        bha_nga;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ᬬᬾᬦ᭄(result)
      bha_nga;

    ᬬᬾᬦ᭄(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    ᬮᬶᬬ
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (puur_naa_nka)result, *nreadp, *ftpcodep);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_user(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ᬬᬾᬦ᭄(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  ᬯᬮᬶ result;
}

static CURLcode ftp_state_pwd(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  ᬬᬾᬦ᭄(!data->id && getenv("CURL_FTP_PWD_STOP"))
    ᬯᬮᬶ CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  ᬬᬾᬦ᭄(!result)
    ftp_state(data, ftpc, FTP_PWD);

  ᬯᬮᬶ result;
}

/* For the FTP "protocol connect" and "doing" phases only */
static CURLcode ftp_pollset(sa_nracanaa Curl_easy *data,
                            sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ᬯᬮᬶ ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
static CURLcode ftp_domore_pollset(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ᬬᬾᬦ᭄(!ftpc)
    ᬯᬮᬶ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ᬬᬾᬦ᭄(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ᬯᬮᬶ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  ᬯᬮᬶ Curl_pp_pollset(data, &ftpc->pp, ps);
}

static puur_naa_nka pathlen(sa_nracanaa ftp_conn *ftpc, puur_naa_nka num)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > num);
  ᬯᬮᬶ ftpc->dirs[num].len;
}

static const var_na *pathpiece(sa_nracanaa ftp_conn *ftpc, puur_naa_nka num)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > num);
  ᬯᬮᬶ &ftpc->rawpath[ftpc->dirs[num].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
static CURLcode ftp_state_cwd(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ftp_conn *ftpc,
                              sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  ᬮᬶᬬ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ᬬᬾᬦ᭄(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    ᬮᬶᬬ {
      ᬬᬾᬦ᭄(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ᬬᬾᬦ᭄(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      ᬮᬶᬬ {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  ᬯᬮᬶ result;
}

typedef ga_nanaa {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
static CURLcode ftp_port_parse_string(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn,
                                      const var_na *string_ftpport,
                                      sa_nracanaa Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      const var_na **hostp,
                                      var_na *hbuf, size_t hbuflen)
{
  const var_na *ip_end = NULL;
  const var_na *addr = NULL;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  var_na ipstr[50];
#ifndef USE_IPV6
  (void)conn;
  (void)ss;
#endif

  /* default to nothing */
  *hostp = NULL;
  *port_minp = *port_maxp = 0;

  ᬬᬾᬦ᭄(!string_ftpport || (strlen(string_ftpport) <= 1))
    goto done;

#ifdef USE_IPV6
  ᬬᬾᬦ᭄(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    const var_na *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    ᬬᬾᬦ᭄(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    ᬮᬶᬬ {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ᬬᬾᬦ᭄(ip_end) {
#ifdef USE_IPV6
        sa_nracanaa sockaddr_in6 * const sa6 = (void *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = NULL; /* this got no port ! */
        }
#endif
      }
      ᬮᬶᬬ
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  ᬬᬾᬦ᭄(ip_end) {
    const var_na *portp = strchr(ip_end, ':');
    ᬬᬾᬦ᭄(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      ᬬᬾᬦ᭄(!curlx_str_number(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        ᬬᬾᬦ᭄(!curlx_str_single(&portp, '-') &&
           !curlx_str_number(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        ᬮᬶᬬ
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ᬬᬾᬦ᭄(port_min > port_max)
    port_min = port_max = 0;

  ᬬᬾᬦ᭄(addrlen) {
    const sa_nracanaa Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ᬬᬾᬦ᭄(!remote_addr || (addrlen >= parimaa_na(ipstr)) || (addrlen >= hbuflen))
      ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    switch(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    pak_sa IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      bha_nga;
    pak_sa IF2IP_AF_NOT_SUPPORTED:
      ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
    pak_sa IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      bha_nga;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  ᬯᬮᬶ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
static CURLcode ftp_port_default_host(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn,
                                      sa_nracanaa Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      const var_na **hostp,
                                      var_na *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  sa_nracanaa sockaddr *sa = (sa_nracanaa sockaddr *)ss;
  sa_nracanaa sockaddr_in * const sa4 = (void *)sa;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 * const sa6 = (void *)sa;
#endif
  var_na buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = parimaa_na(*ss);
  ᬬᬾᬦ᭄(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
    ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
  }
  switch(sa->sa_family) {
#ifdef USE_IPV6
  pak_sa AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    bha_nga;
#endif
  default:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    bha_nga;
  }
  ᬬᬾᬦ᭄(result)
    ᬯᬮᬶ CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ᬯᬮᬶ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
static CURLcode ftp_port_resolve_host(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn,
                                      const var_na *host,
                                      sa_nracanaa Curl_dns_entry **dns_entryp,
                                      const sa_nracanaa Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = NULL;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  ᬬᬾᬦ᭄(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  ᬮᬶᬬ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ᬯᬮᬶ result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
static CURLcode ftp_port_open_socket(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn,
                                     const sa_nracanaa Curl_addrinfo *res,
                                     const sa_nracanaa Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  var_na buffer[STRERROR_LEN];
  puur_naa_nka sockerr = 0;
  const sa_nracanaa Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  for(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, NULL,
                       Curl_conn_get_transport(data, conn), portsockp);
    ᬬᬾᬦ᭄(result) {
      ᬬᬾᬦ᭄(result == CURLE_OUT_OF_MEMORY)
        ᬯᬮᬶ result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      continue;
    }
    bha_nga;
  }
  ᬬᬾᬦ᭄(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
    ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  ᬯᬮᬶ result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
static CURLcode ftp_port_bind_socket(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn,
                                     curl_socket_t portsock,
                                     const sa_nracanaa Curl_addrinfo *ai,
                                     sa_nracanaa Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  sa_nracanaa sockaddr *sa = (sa_nracanaa sockaddr *)ss;
  sa_nracanaa sockaddr_in * const sa4 = (void *)sa;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 * const sa6 = (void *)sa;
#endif
  var_na buffer[STRERROR_LEN];
  unsigned short port;
  puur_naa_nka sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  for(port = port_min; port <= port_max;) {
    ᬬᬾᬦ᭄(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    ᬮᬶᬬ
      sa6->sin6_port = htons(port);
#endif
    ᬬᬾᬦ᭄(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ᬬᬾᬦ᭄(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, parimaa_na(buffer)));

        *sslen_io = parimaa_na(*ss);
        ᬬᬾᬦ᭄(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
          ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        continue;
      }
      ᬬᬾᬦ᭄(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
        ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
      }
    }
    ᬮᬶᬬ
      bha_nga;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ᬬᬾᬦ᭄(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = parimaa_na(*ss);
  ᬬᬾᬦ᭄(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
    ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  ᬯᬮᬶ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
static CURLcode ftp_port_listen(sa_nracanaa Curl_easy *data, curl_socket_t portsock)
{
  var_na buffer[STRERROR_LEN];

  ᬬᬾᬦ᭄(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
    ᬯᬮᬶ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  ᬯᬮᬶ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
static CURLcode ftp_port_send_command(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa ftp_conn *ftpc,
                                      sa_nracanaa connectdata *conn,
                                      sa_nracanaa Curl_sockaddr_storage *ss,
                                      const sa_nracanaa Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  static const var_na mode[][5] = { "EPRT", "PORT" };
  sa_nracanaa sockaddr *sa = (sa_nracanaa sockaddr *)ss;
  sa_nracanaa sockaddr_in * const sa4 = (void *)sa;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 * const sa6 = (void *)sa;
#endif
  var_na myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, parimaa_na(myhost));

#ifdef USE_IPV6
  ᬬᬾᬦ᭄(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  for(; fcmd != DONE; fcmd++) {

    ᬬᬾᬦ᭄(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      continue;

    ᬬᬾᬦ᭄((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      continue;

    switch(sa->sa_family) {
    pak_sa AF_INET:
      port = ntohs(sa4->sin_port);
      bha_nga;
#ifdef USE_IPV6
    pak_sa AF_INET6:
      port = ntohs(sa6->sin6_port);
      bha_nga;
#endif
    default:
      continue; /* might as well skip this */
    }

    ᬬᬾᬦ᭄(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      ᬬᬾᬦ᭄(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        ᬯᬮᬶ result;
      }
      bha_nga;
    }
    ᬬᬾᬦ᭄(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      var_na target[parimaa_na(myhost) + 20];
      const var_na *source = myhost;
      var_na *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      ᬲᭂᬤᭂᬮ(*source) {
        ᬬᬾᬦ᭄(*source == '.')
          *dest = ',';
        ᬮᬶᬬ
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (puur_naa_nka)(port >> 8), (puur_naa_nka)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      ᬬᬾᬦ᭄(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        ᬯᬮᬶ result;
      }
      bha_nga;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  ᬯᬮᬶ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
static CURLcode ftp_state_use_port(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  sa_nracanaa connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  sa_nracanaa Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  var_na hbuf[NI_MAXHOST];
  const var_na *host = NULL;
  const var_na *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  sa_nracanaa Curl_dns_entry *dns_entry = NULL;
  const sa_nracanaa Curl_addrinfo *res = NULL;
  const sa_nracanaa Curl_addrinfo *ai = NULL;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, parimaa_na(hbuf));
  ᬬᬾᬦ᭄(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, parimaa_na(hbuf), &non_local);

  /* resolve host string to address list */
  ᬬᬾᬦ᭄(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  ᬬᬾᬦ᭄(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  ᬬᬾᬦ᭄(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  ᬬᬾᬦ᭄(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  ᬬᬾᬦ᭄(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ᬬᬾᬦ᭄(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  ᬬᬾᬦ᭄(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ᬬᬾᬦ᭄(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  ᬬᬾᬦ᭄(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  ᬮᬶᬬ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ᬬᬾᬦ᭄(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  ᬬᬾᬦ᭄(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  ᬯᬮᬶ result;
}

static CURLcode ftp_state_use_pasv(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc,
                                   sa_nracanaa connectdata *conn)
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

  static const var_na mode[][5] = { "EPSV", "PASV" };
  puur_naa_nka modeoff;

#ifdef PF_INET6
  ᬬᬾᬦ᭄(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  ᬬᬾᬦ᭄(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  ᬯᬮᬶ result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
static CURLcode ftp_state_prepare_transfer(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa ftp_conn *ftpc,
                                           sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;

  ᬬᬾᬦ᭄(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  ᬮᬶᬬ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ᬬᬾᬦ᭄(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ᬬᬾᬦ᭄(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      ᬮᬶᬬ ᬬᬾᬦ᭄(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ᬮᬶᬬ
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    ᬮᬶᬬ
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  ᬯᬮᬶ result;
}

static CURLcode ftp_state_rest(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  ᬮᬶᬬ
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_size(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  ᬮᬶᬬ
    result = ftp_state_rest(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_list(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp)
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
  const var_na *lstArg = NULL;
  puur_naa_nka lstArglen = 0;
  var_na *cmd;

  ᬬᬾᬦ᭄((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    const var_na *rawPath = ftpc->rawpath;
    const var_na *slashPos = strrchr(rawPath, '/');
    ᬬᬾᬦ᭄(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      ᬬᬾᬦ᭄(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (puur_naa_nka)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ᬬᬾᬦ᭄(!cmd)
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ᬬᬾᬦ᭄(!result)
    ftp_state(data, ftpc, FTP_LIST);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_list_prequote(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ftp_conn *ftpc,
                                        sa_nracanaa FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ᬯᬮᬶ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

static CURLcode ftp_state_retr_prequote(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ftp_conn *ftpc,
                                        sa_nracanaa FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ᬯᬮᬶ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

static CURLcode ftp_state_stor_prequote(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ftp_conn *ftpc,
                                        sa_nracanaa FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ᬯᬮᬶ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

static CURLcode ftp_state_type(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ᬬᬾᬦ᭄(data->req.no_body && ftpc->file &&
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
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;
  }
  ᬮᬶᬬ
    result = ftp_state_size(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
static CURLcode ftp_state_mdtm(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ᬬᬾᬦ᭄((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  ᬮᬶᬬ
    result = ftp_state_type(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

/* This is called after the TYPE and possible quote commands have been sent */
static CURLcode ftp_state_ul_setup(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc,
                                   sa_nracanaa FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  ᬬᬾᬦ᭄((data->state.resume_from && !sizechecked) ||
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
    puur_naa_nka seekerr = CURL_SEEKFUNC_OK;

    ᬬᬾᬦ᭄(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      ᬯᬮᬶ result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ᬬᬾᬦ᭄(data->set.seek_func) {
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ᬬᬾᬦ᭄(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ᬬᬾᬦ᭄(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ᬯᬮᬶ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      do {
        var_na scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)parimaa_na(scratch)) ?
          parimaa_na(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        ᬬᬾᬦ᭄((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ᬯᬮᬶ CURLE_FTP_COULDNT_USE_REST;
        }
      } ᬲᭂᬤᭂᬮ(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    ᬬᬾᬦ᭄(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      ᬬᬾᬦ᭄(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        ᬯᬮᬶ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ᬬᬾᬦ᭄(!result)
    ftp_state(data, ftpc, FTP_STOR);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_retr(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ftp_conn *ftpc,
                               sa_nracanaa FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ᬬᬾᬦ᭄(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    ᬯᬮᬶ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ᬬᬾᬦ᭄(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ᬬᬾᬦ᭄(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ᬮᬶᬬ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ᬬᬾᬦ᭄(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ᬬᬾᬦ᭄(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ᬯᬮᬶ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      ᬮᬶᬬ {
        ᬬᬾᬦ᭄(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ᬯᬮᬶ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    ᬬᬾᬦ᭄(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      ᬯᬮᬶ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  ᬮᬶᬬ {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_quote(sa_nracanaa Curl_easy *data,
                                sa_nracanaa ftp_conn *ftpc,
                                sa_nracanaa FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  sa_nracanaa curl_slist *item;

  switch(instate) {
  pak_sa FTP_QUOTE:
  default:
    item = data->set.quote;
    bha_nga;
  pak_sa FTP_RETR_PREQUOTE:
  pak_sa FTP_STOR_PREQUOTE:
  pak_sa FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    bha_nga;
  pak_sa FTP_POSTQUOTE:
    item = data->set.postquote;
    bha_nga;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ᬬᬾᬦ᭄(init)
    ftpc->count1 = 0;
  ᬮᬶᬬ
    ftpc->count1++;

  ᬬᬾᬦ᭄(item) {
    puur_naa_nka i = 0;

    /* Skip count1 items in the linked list */
    ᬲᭂᬤᭂᬮ((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    ᬬᬾᬦ᭄(item) {
      const var_na *cmd = item->data;
      ᬬᬾᬦ᭄(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ᬮᬶᬬ
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  ᬬᬾᬦ᭄(!quote) {
    /* No more quote to send, continue to ... */
    switch(instate) {
    pak_sa FTP_QUOTE:
    default:
      result = ftp_state_cwd(data, ftpc, ftp);
      bha_nga;
    pak_sa FTP_RETR_PREQUOTE:
      ᬬᬾᬦ᭄(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      ᬮᬶᬬ {
        ᬬᬾᬦ᭄(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        ᬮᬶᬬ {
          ᬬᬾᬦ᭄(data->set.ignorecl || data->state.prefer_ascii) {
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
            ᬬᬾᬦ᭄(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          ᬮᬶᬬ {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            ᬬᬾᬦ᭄(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      bha_nga;
    pak_sa FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      bha_nga;
    pak_sa FTP_POSTQUOTE:
      bha_nga;
    pak_sa FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      bha_nga;
    }
  }

  ᬯᬮᬶ result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
static CURLcode ftp_epsv_disable(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ftp_conn *ftpc,
                                 sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  ᬬᬾᬦ᭄(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  ᬯᬮᬶ result;
}

static CURLcode ftp_control_addr_dup(sa_nracanaa Curl_easy *data, var_na **newhostp)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  ᬬᬾᬦ᭄(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ᬮᬶᬬ
#endif
  ᬬᬾᬦ᭄(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ᬮᬶᬬ {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = NULL;
    ᬯᬮᬶ CURLE_FTP_CANT_GET_HOST;
  }
  ᬯᬮᬶ *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

static bool match_pasv_6nums(const var_na *p,
                             unsigned puur_naa_nka *array) /* 6 numbers */
{
  puur_naa_nka i;
  for(i = 0; i < 6; i++) {
    curl_off_t num;
    ᬬᬾᬦ᭄(i) {
      ᬬᬾᬦ᭄(*p != ',')
        ᬯᬮᬶ FALSE;
      p++;
    }
    ᬬᬾᬦ᭄(curlx_str_number(&p, &num, 0xff))
      ᬯᬮᬶ FALSE;
    array[i] = (unsigned puur_naa_nka)num;
  }
  ᬯᬮᬶ TRUE;
}

static CURLcode ftp_state_pasv_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    puur_naa_nka ftpcode)
{
  sa_nracanaa connectdata *conn = data->conn;
  CURLcode result;
  const sa_nracanaa pingpong *pp = &ftpc->pp;
  var_na *newhost = NULL;
  unsigned short newport = 0;
  const var_na *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ᬬᬾᬦ᭄((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    const var_na *ptr = strchr(str, '(');
    ᬬᬾᬦ᭄(ptr) {
      var_na sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ᬬᬾᬦ᭄(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        const var_na *p = &ptr[3];
        curl_off_t num;
        ᬬᬾᬦ᭄(curlx_str_number(&p, &num, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          ᬯᬮᬶ CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)num;
        result = ftp_control_addr_dup(data, &newhost);
        ᬬᬾᬦ᭄(result)
          ᬯᬮᬶ result;
      }
      ᬮᬶᬬ
        ptr = NULL;
    }
    ᬬᬾᬦ᭄(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      ᬯᬮᬶ CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned puur_naa_nka ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ᬲᭂᬤᭂᬮ(*str) {
      ᬬᬾᬦ᭄(match_pasv_6nums(str, ip))
        bha_nga;
      str++;
    }

    ᬬᬾᬦ᭄(!*str) {
      failf(data, "Could not interpret the 227-response");
      ᬯᬮᬶ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ᬬᬾᬦ᭄(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
    }
    ᬮᬶᬬ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ᬬᬾᬦ᭄(!newhost)
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ᬯᬮᬶ ftp_epsv_disable(data, ftpc, conn);
  }
  ᬮᬶᬬ {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    ᬯᬮᬶ CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  ᬬᬾᬦ᭄(result)
    goto error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ᬬᬾᬦ᭄(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    ᬬᬾᬦ᭄(result)
      goto error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ᬬᬾᬦ᭄(result) {
    ᬬᬾᬦ᭄((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    goto error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  ᬬᬾᬦ᭄(data->set.verbose) {
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
  ᬯᬮᬶ result;
}

/* called repeatedly until done from multi.c */
static CURLcode ftp_statemach(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  ᬯᬮᬶ result;
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
static CURLcode ftp_do_more(sa_nracanaa Curl_easy *data, domore *more)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  sa_nracanaa FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ᬬᬾᬦ᭄(!ftpc || !ftp)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ᬬᬾᬦ᭄(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ᬬᬾᬦ᭄(result == CURLE_OUT_OF_MEMORY)
      ᬯᬮᬶ result;
    ᬬᬾᬦ᭄(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      ᬬᬾᬦ᭄(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ᬯᬮᬶ ftp_epsv_disable(data, ftpc, conn);
      }
      ᬯᬮᬶ result;
    }
  }

  ᬬᬾᬦ᭄(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    ᬬᬾᬦ᭄(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ᬬᬾᬦ᭄(result || !ftpc->wait_data_conn)
      ᬯᬮᬶ result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  ᬬᬾᬦ᭄(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ᬬᬾᬦ᭄(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result; /* Failed to accept data connection */

      ᬬᬾᬦ᭄(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        ᬬᬾᬦ᭄(result)
          ᬯᬮᬶ result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      ᬮᬶᬬ {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        ᬬᬾᬦ᭄(result)
          ᬯᬮᬶ result;
      }
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    ᬮᬶᬬ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      ᬬᬾᬦ᭄(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      ᬬᬾᬦ᭄((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ᬬᬾᬦ᭄(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ᬬᬾᬦ᭄(result)
            ᬯᬮᬶ result;
        }
        /* otherwise fall through */
      }
      ᬮᬶᬬ {
        ᬬᬾᬦ᭄(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ᬮᬶᬬ {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ᬬᬾᬦ᭄(result)
          ᬯᬮᬶ result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      ᬬᬾᬦ᭄(complete)
        *more = DOMORE_DONE;
    }
    ᬯᬮᬶ result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  ᬬᬾᬦ᭄(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (puur_naa_nka)result);
  }

  ᬯᬮᬶ result;
}

/* call this when the DO phase has completed */
static CURLcode ftp_dophase_done(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ftp_conn *ftpc,
                                 sa_nracanaa FTP *ftp,
                                 bool connected)
{
  ᬬᬾᬦ᭄(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    ᬬᬾᬦ᭄(result) {
      close_secondarysocket(data, ftpc);
      ᬯᬮᬶ result;
    }
  }

  ᬬᬾᬦ᭄(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  ᬮᬶᬬ ᬬᬾᬦ᭄(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ᬯᬮᬶ CURLE_OK;
}

static CURLcode ftp_state_port_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    sa_nracanaa FTP *ftp,
                                    puur_naa_nka ftpcode)
{
  sa_nracanaa connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ᬬᬾᬦ᭄(ftpcode / 100 != 2) {
    /* the command failed */

    ᬬᬾᬦ᭄(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ᬬᬾᬦ᭄(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    ᬮᬶᬬ
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  ᬮᬶᬬ {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  ᬯᬮᬶ result;
}

/* return TRUE on error, FALSE on success */
static bool twodigit(const var_na *p, puur_naa_nka *val)
{
  ᬬᬾᬦ᭄(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ᬯᬮᬶ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ᬯᬮᬶ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(const var_na *p, puur_naa_nka *year, puur_naa_nka *month, puur_naa_nka *day,
                           puur_naa_nka *hour, puur_naa_nka *minute, puur_naa_nka *second);
UNITTEST bool ftp_213_date(const var_na *p, puur_naa_nka *year, puur_naa_nka *month, puur_naa_nka *day,
                           puur_naa_nka *hour, puur_naa_nka *minute, puur_naa_nka *second)
{
  puur_naa_nka century;
  ᬬᬾᬦ᭄((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    ᬯᬮᬶ FALSE;

  *year += century * 100;
  ᬬᬾᬦ᭄((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    ᬯᬮᬶ FALSE;
  ᬯᬮᬶ TRUE;
}

static CURLcode client_write_header(sa_nracanaa Curl_easy *data,
                                    var_na *buf, size_t blen)
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
  ᬯᬮᬶ result;
}

static CURLcode ftp_state_mdtm_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    sa_nracanaa FTP *ftp,
                                    puur_naa_nka ftpcode)
{
  CURLcode result = CURLE_OK;

  switch(ftpcode) {
  pak_sa 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    puur_naa_nka year, month, day, hour, minute, second;
    sa_nracanaa pingpong *pp = &ftpc->pp;
    const var_na *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ᬬᬾᬦ᭄(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      var_na timebuf[24];
      curl_msnprintf(timebuf, parimaa_na(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      ᬬᬾᬦ᭄(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ᬬᬾᬦ᭄ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic push
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    ᬬᬾᬦ᭄(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#ᬬᬾᬦ᭄ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic pop
#endif
      var_na headerbuf[128];
      puur_naa_nka headerbuflen;
      time_t filetime = data->info.filetime;
      sa_nracanaa tm buffer;
      const sa_nracanaa tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, parimaa_na(headerbuf),
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
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
    } /* end of a ridiculous amount of conditionals */
  }
    bha_nga;
  default:
    infof(data, "unsupported MDTM reply format");
    bha_nga;
  pak_sa 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    bha_nga;
  }

  ᬬᬾᬦ᭄(data->set.timecondition) {
    ᬬᬾᬦ᭄((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      switch(data->set.timecondition) {
      pak_sa CURL_TIMECOND_IFMODSINCE:
      default:
        ᬬᬾᬦ᭄(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ᬯᬮᬶ CURLE_OK;
        }
        bha_nga;
      pak_sa CURL_TIMECOND_IFUNMODSINCE:
        ᬬᬾᬦ᭄(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ᬯᬮᬶ CURLE_OK;
        }
        bha_nga;
      } /* switch */
    }
    ᬮᬶᬬ {
      infof(data, "Skipping time comparison");
    }
  }

  ᬬᬾᬦ᭄(!result)
    result = ftp_state_type(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_type_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    sa_nracanaa FTP *ftp,
                                    puur_naa_nka ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    ᬯᬮᬶ CURLE_FTP_COULDNT_SET_TYPE;
  }
  ᬬᬾᬦ᭄(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ᬬᬾᬦ᭄(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_size_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    sa_nracanaa FTP *ftp,
                                    puur_naa_nka ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  const var_na *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ᬬᬾᬦ᭄(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    const var_na *start = &buf[4];
    const var_na *fdigit = memchr(start, '\r', len - 4);
    ᬬᬾᬦ᭄(fdigit) {
      fdigit--;
      ᬬᬾᬦ᭄(*fdigit == '\n')
        fdigit--;
      ᬲᭂᬤᭂᬮ(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    ᬮᬶᬬ
      fdigit = start;
    ᬬᬾᬦ᭄(curlx_str_number(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    ᬯᬮᬶ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  ᬬᬾᬦ᭄(instate == FTP_SIZE) {
    ᬬᬾᬦ᭄(filesize != -1) {
      var_na clbuf[128];
      puur_naa_nka clbuflen = curl_msnprintf(clbuf, parimaa_na(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_rest_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    sa_nracanaa FTP *ftp,
                                    puur_naa_nka ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  switch(instate) {
  pak_sa FTP_REST:
  default:
    ᬬᬾᬦ᭄(ftpcode == 350) {
      var_na buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    bha_nga;

  pak_sa FTP_RETR_REST:
    ᬬᬾᬦ᭄(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    ᬮᬶᬬ {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    bha_nga;
  }

  ᬯᬮᬶ result;
}

static CURLcode ftp_state_stor_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    puur_naa_nka ftpcode)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    ᬯᬮᬶ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ᬬᬾᬦ᭄(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;

    ᬬᬾᬦ᭄(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      ᬯᬮᬶ ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  ᬯᬮᬶ ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
static CURLcode ftp_state_get_resp(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc,
                                   sa_nracanaa FTP *ftp,
                                   puur_naa_nka ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄((ftpcode == 150) || (ftpcode == 125)) {

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

    ᬬᬾᬦ᭄((instate != FTP_LIST) &&
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
      ᬬᬾᬦ᭄(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        for(i = 0; i < len - 7; i++) {
          curl_off_t what;
          const var_na *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          const var_na *c = &buf[i];
          ᬬᬾᬦ᭄(!curlx_str_number(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            bha_nga;
          }
        }
      }
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    ᬬᬾᬦ᭄(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    ᬮᬶᬬ ᬬᬾᬦ᭄((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    ᬬᬾᬦ᭄(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    ᬬᬾᬦ᭄(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      ᬬᬾᬦ᭄(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        ᬯᬮᬶ ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    ᬯᬮᬶ ftp_initiate_transfer(data, ftpc);
  }
  ᬮᬶᬬ {
    ᬬᬾᬦ᭄((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    ᬮᬶᬬ {
      failf(data, "RETR response: %03d", ftpcode);
      ᬯᬮᬶ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ᬯᬮᬶ result;
}

/* after USER, PASS and ACCT */
static CURLcode ftp_state_loggedin(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  ᬮᬶᬬ {
    result = ftp_state_pwd(data, ftpc);
  }
  ᬯᬮᬶ result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
static bool ftp_has_ctrl(const var_na *string)
{
  const unsigned var_na *s = (const unsigned var_na *)string;
  ᬲᭂᬤᭂᬮ(*s) {
    ᬬᬾᬦ᭄(*s < 0x20)
      ᬯᬮᬶ TRUE;
    s++;
  }
  ᬯᬮᬶ FALSE;
}

/* for USER and PASS responses */
static CURLcode ftp_state_user_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    puur_naa_nka ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ᬬᬾᬦ᭄((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ftpcode == 332) {
    const var_na *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    ᬬᬾᬦ᭄(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ᬮᬶᬬ {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  ᬮᬶᬬ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    const var_na *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    ᬬᬾᬦ᭄(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ᬬᬾᬦ᭄(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ᬮᬶᬬ {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        ᬬᬾᬦ᭄(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    ᬮᬶᬬ {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  ᬯᬮᬶ result;
}

/* for ACCT response */
static CURLcode ftp_state_acct_resp(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc,
                                    puur_naa_nka ftpcode)
{
  CURLcode result = CURLE_OK;
  ᬬᬾᬦ᭄(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  ᬮᬶᬬ
    result = ftp_state_loggedin(data, ftpc);

  ᬯᬮᬶ result;
}

static CURLcode ftp_pwd_resp(sa_nracanaa Curl_easy *data,
                             sa_nracanaa ftp_conn *ftpc,
                             puur_naa_nka ftpcode)
{
  sa_nracanaa pingpong *pp = &ftpc->pp;
  CURLcode result;

  ᬬᬾᬦ᭄(ftpcode == 257) {
    const var_na *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    sa_nracanaa dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ᬲᭂᬤᭂᬮ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ᬬᬾᬦ᭄('\"' == *ptr) {
      /* it started good */
      for(ptr++; *ptr; ptr++) {
        ᬬᬾᬦ᭄('\"' == *ptr) {
          ᬬᬾᬦ᭄('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          ᬮᬶᬬ {
            /* end of path */
            ᬬᬾᬦ᭄(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            bha_nga; /* get out of this loop */
          }
        }
        ᬮᬶᬬ {
          ᬬᬾᬦ᭄(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        ᬬᬾᬦ᭄(result) {
          curlx_dyn_free(&out);
          ᬯᬮᬶ result;
        }
      }
    }
    ᬬᬾᬦ᭄(entry_extracted) {
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
      var_na *dir = curlx_dyn_ptr(&out);

      ᬬᬾᬦ᭄(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        ᬬᬾᬦ᭄(result) {
          curlx_dyn_free(&out);
          ᬯᬮᬶ result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ᬬᬾᬦ᭄(!data->state.most_recent_ftp_entrypath)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      ᬬᬾᬦ᭄(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        ᬯᬮᬶ CURLE_OK;
      }
    }
    ᬮᬶᬬ {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ᬯᬮᬶ CURLE_OK;
}

static const var_na * const ftpauth[] = { "SSL", "TLS" };

static CURLcode ftp_wait_resp(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn,
                              sa_nracanaa ftp_conn *ftpc,
                              puur_naa_nka ftpcode)
{
  CURLcode result = CURLE_OK;
  ᬬᬾᬦ᭄(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ᬬᬾᬦ᭄(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ᬯᬮᬶ ftp_state_user_resp(data, ftpc, ftpcode);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY;
  }

  ᬬᬾᬦ᭄(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    switch((long)data->set.ftpsslauth) {
    pak_sa CURLFTPAUTH_DEFAULT:
    pak_sa CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      bha_nga;
    pak_sa CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      bha_nga;
    default:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (puur_naa_nka)data->set.ftpsslauth);
      ᬯᬮᬶ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  ᬮᬶᬬ
    result = ftp_state_user(data, ftpc, conn);
  ᬯᬮᬶ result;
}

/* an implicit TLS scheme implies TLS on both connections */
static unsigned var_na ftp_use_ssl(sa_nracanaa connectdata *conn,
                                 sa_nracanaa ftp_conn *ftpc)
{
  ᬬᬾᬦ᭄(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ᬯᬮᬶ CURLUSESSL_ALL;
  ᬯᬮᬶ ftpc->use_ssl;
}

static CURLcode ftp_pp_statemachine(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn)
{
  CURLcode result;
  puur_naa_nka ftpcode;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  sa_nracanaa FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  sa_nracanaa pingpong *pp;
  size_t nread = 0;

  ᬬᬾᬦ᭄(!ftpc || !ftp)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ᬬᬾᬦ᭄(pp->sendleft)
    ᬯᬮᬶ Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ᬬᬾᬦ᭄(result || !ftpcode)
    ᬯᬮᬶ result;

  /* we have now received a full FTP server response */
  switch(ftpc->state) {
  pak_sa FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ᬬᬾᬦ᭄(pp->overflow)
      ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ᬬᬾᬦ᭄((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      ᬬᬾᬦ᭄(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ᬬᬾᬦ᭄(result) {
          /* we failed and bail out */
          ᬯᬮᬶ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      ᬬᬾᬦ᭄(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ᬮᬶᬬ {
      ᬬᬾᬦ᭄(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      ᬮᬶᬬ
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    bha_nga;

  pak_sa FTP_USER:
  pak_sa FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ᬬᬾᬦ᭄(!result)
      ftp_state(data, ftpc, FTP_PROT);
    bha_nga;

  pak_sa FTP_PROT:
    ᬬᬾᬦ᭄(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ᬮᬶᬬ ᬬᬾᬦ᭄(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ᬯᬮᬶ CURLE_USE_SSL_FAILED;

    ᬬᬾᬦ᭄(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      ᬬᬾᬦ᭄(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    ᬮᬶᬬ
      result = ftp_state_pwd(data, ftpc);
    bha_nga;

  pak_sa FTP_CCC:
    ᬬᬾᬦ᭄(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned var_na)CURLFTPSSL_CCC_ACTIVE));
      ᬬᬾᬦ᭄(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    ᬬᬾᬦ᭄(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    bha_nga;

  pak_sa FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_SYST:
    ᬬᬾᬦ᭄(ftpcode == 215) {
      const var_na *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      const var_na *start;
      var_na *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ᬲᭂᬤᭂᬮ(*ptr == ' ')
        ptr++;
      for(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      ᬬᬾᬦ᭄(!os)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      ᬬᬾᬦ᭄(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ᬬᬾᬦ᭄(result) {
          curlx_free(os);
          ᬯᬮᬶ result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        bha_nga;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ᬮᬶᬬ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    bha_nga;

  pak_sa FTP_NAMEFMT:
    ᬬᬾᬦ᭄(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      bha_nga;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    bha_nga;

  pak_sa FTP_QUOTE:
  pak_sa FTP_POSTQUOTE:
  pak_sa FTP_RETR_PREQUOTE:
  pak_sa FTP_STOR_PREQUOTE:
  pak_sa FTP_LIST_PREQUOTE:
    ᬬᬾᬦ᭄((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    ᬮᬶᬬ
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    bha_nga;

  pak_sa FTP_CWD:
    ᬬᬾᬦ᭄(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ᬬᬾᬦ᭄(data->set.ftp_create_missing_dirs &&
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
        ᬬᬾᬦ᭄(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      ᬮᬶᬬ {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ᬮᬶᬬ {
      /* success */
      ftpc->count2 = 0;
      ᬬᬾᬦ᭄(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      ᬮᬶᬬ {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    bha_nga;

  pak_sa FTP_MKD:
    ᬬᬾᬦ᭄((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    ᬮᬶᬬ {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    bha_nga;

  pak_sa FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    bha_nga;

  pak_sa FTP_TYPE:
  pak_sa FTP_LIST_TYPE:
  pak_sa FTP_RETR_TYPE:
  pak_sa FTP_STOR_TYPE:
  pak_sa FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    bha_nga;

  pak_sa FTP_SIZE:
  pak_sa FTP_RETR_SIZE:
  pak_sa FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    bha_nga;

  pak_sa FTP_REST:
  pak_sa FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    bha_nga;

  pak_sa FTP_PRET:
    ᬬᬾᬦ᭄(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      ᬯᬮᬶ CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    bha_nga;

  pak_sa FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    bha_nga;

  pak_sa FTP_LIST:
  pak_sa FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    bha_nga;

  pak_sa FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    bha_nga;

  pak_sa FTP_QUIT:
  default:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    bha_nga;
  }

  ᬯᬮᬶ result;
}

/* called repeatedly until done from multi.c */
static CURLcode ftp_multi_statemach(sa_nracanaa Curl_easy *data,
                                    bool *done)
{
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ᬯᬮᬶ ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

static CURLcode ftp_block_statemach(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_conn *ftpc)
{
  sa_nracanaa pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  ᬲᭂᬤᭂᬮ(ftpc->state != FTP_STOP) {
    ᬬᬾᬦ᭄(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    ᬬᬾᬦ᭄(result)
      bha_nga;
  }

  ᬯᬮᬶ result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
static CURLcode ftp_connect(sa_nracanaa Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  sa_nracanaa pingpong *pp;

  *done = FALSE; /* default to not done yet */
  ᬬᬾᬦ᭄(!ftpc)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  ᬬᬾᬦ᭄(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  ᬯᬮᬶ result;
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
static CURLcode ftp_sendquote(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ftp_conn *ftpc,
                              sa_nracanaa curl_slist *quote)
{
  sa_nracanaa curl_slist *item;
  sa_nracanaa pingpong *pp = &ftpc->pp;

  item = quote;
  ᬲᭂᬤᭂᬮ(item) {
    ᬬᬾᬦ᭄(item->data) {
      size_t nread;
      const var_na *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      puur_naa_nka ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ᬬᬾᬦ᭄(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ᬬᬾᬦ᭄(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      ᬬᬾᬦ᭄(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        ᬯᬮᬶ CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  ᬯᬮᬶ CURLE_OK;
}

static CURLcode ftp_done_status(sa_nracanaa Curl_easy *data,
                                sa_nracanaa ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  switch(status) {
  pak_sa CURLE_BAD_DOWNLOAD_RESUME:
  pak_sa CURLE_FTP_WEIRD_PASV_REPLY:
  pak_sa CURLE_FTP_PORT_FAILED:
  pak_sa CURLE_FTP_ACCEPT_FAILED:
  pak_sa CURLE_FTP_ACCEPT_TIMEOUT:
  pak_sa CURLE_FTP_COULDNT_SET_TYPE:
  pak_sa CURLE_FTP_COULDNT_RETR_FILE:
  pak_sa CURLE_PARTIAL_FILE:
  pak_sa CURLE_UPLOAD_FAILED:
  pak_sa CURLE_REMOTE_ACCESS_DENIED:
  pak_sa CURLE_FILESIZE_EXCEEDED:
  pak_sa CURLE_REMOTE_FILE_NOT_FOUND:
  pak_sa CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  pak_sa CURLE_OK: /* does not affect the control connection's status */
    ᬬᬾᬦ᭄(!premature)
      bha_nga;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  default:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    ᬯᬮᬶ status;      /* use the already set error code */
  }
  ᬯᬮᬶ CURLE_OK;
}

static void ftp_done_wildcard(sa_nracanaa Curl_easy *data, sa_nracanaa ftp_conn *ftpc)
{
  ᬬᬾᬦ᭄(data->state.wildcardmatch) {
    ᬬᬾᬦ᭄(data->set.chunk_end && ftpc->file) {
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

static void ftp_done_path(sa_nracanaa Curl_easy *data, sa_nracanaa ftp_conn *ftpc,
                          CURLcode result)
{
  sa_nracanaa connectdata *conn = data->conn;
  ᬬᬾᬦ᭄(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ᬮᬶᬬ { /* remember working directory for connection reuse */
    const var_na *rawPath = ftpc->rawpath;
    ᬬᬾᬦ᭄(rawPath) {
      ᬬᬾᬦ᭄((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ᬮᬶᬬ {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ᬬᬾᬦ᭄(!ftpc->cwdfail) {
          ᬬᬾᬦ᭄(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          ᬮᬶᬬ
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        ᬮᬶᬬ
          ftpc->prevpath = NULL; /* no path */
      }
    }
    ᬬᬾᬦ᭄(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

static CURLcode ftp_done_secondary_socket(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ftp_conn *ftpc,
                                          CURLcode result)
{
  sa_nracanaa connectdata *conn = data->conn;
  ᬬᬾᬦ᭄(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    ᬬᬾᬦ᭄(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      ᬬᬾᬦ᭄(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  ᬯᬮᬶ result;
}

static CURLcode ftp_done_control_reply(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ftp_conn *ftpc,
                                       sa_nracanaa FTP *ftp, CURLcode result,
                                       bool premature)
{
  sa_nracanaa connectdata *conn = data->conn;
  size_t nread;
  puur_naa_nka ftpcode;

  ᬬᬾᬦ᭄(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    ᬬᬾᬦ᭄(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;

    ᬬᬾᬦ᭄(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      ᬯᬮᬶ result;
    }

    ᬬᬾᬦ᭄(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      switch(ftpcode) {
      pak_sa 226:
      pak_sa 250:
        bha_nga;
      pak_sa 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        bha_nga;
      default:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        bha_nga;
      }
    }
  }
  ᬯᬮᬶ result;
}

static CURLcode ftp_done_check_partial(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ftp_conn *ftpc,
                                       sa_nracanaa FTP *ftp, CURLcode result,
                                       bool premature)
{
  ᬬᬾᬦ᭄(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ᬮᬶᬬ ᬬᬾᬦ᭄(data->state.upload) {
    ᬬᬾᬦ᭄((ftp->transfer == PPTRANSFER_BODY) &&
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
  ᬮᬶᬬ {
    ᬬᬾᬦ᭄((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ᬯᬮᬶ result;
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
static CURLcode ftp_done(sa_nracanaa Curl_easy *data, CURLcode status,
                         bool premature)
{
  sa_nracanaa FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  ᬬᬾᬦ᭄(!ftp || !ftpc)
    ᬯᬮᬶ CURLE_OK;

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
  ᬬᬾᬦ᭄(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (puur_naa_nka)result);
  ᬯᬮᬶ result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
static CURLcode ftp_nb_type(sa_nracanaa Curl_easy *data,
                            sa_nracanaa ftp_conn *ftpc,
                            sa_nracanaa FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  var_na want = (var_na)(ascii ? 'A' : 'I');

  ᬬᬾᬦ᭄(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    ᬯᬮᬶ ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  ᬬᬾᬦ᭄(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ᬯᬮᬶ result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
static CURLcode ftp_perform(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa ftp_conn *ftpc,
  sa_nracanaa FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ᬬᬾᬦ᭄(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  ᬬᬾᬦ᭄(result)
    ᬯᬮᬶ result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  ᬬᬾᬦ᭄(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ᬮᬶᬬ
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ᬬᬾᬦ᭄(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ᬯᬮᬶ result;
}

static void wc_data_dtor(void *ptr)
{
  sa_nracanaa ftp_wc *ftpwc = ptr;
  ᬬᬾᬦ᭄(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

static CURLcode init_wc_data(sa_nracanaa Curl_easy *data,
                             sa_nracanaa ftp_conn *ftpc,
                             sa_nracanaa FTP *ftp)
{
  var_na *last_slash;
  var_na *path = ftp->path;
  sa_nracanaa WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  sa_nracanaa ftp_wc *ftpwc = NULL;

  last_slash = strrchr(ftp->path, '/');
  ᬬᬾᬦ᭄(last_slash) {
    last_slash++;
    ᬬᬾᬦ᭄(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ᬯᬮᬶ ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    ᬬᬾᬦ᭄(!wildcard->pattern)
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  ᬮᬶᬬ { /* there is only 'wildcard pattern' or nothing */
    ᬬᬾᬦ᭄(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      ᬬᬾᬦ᭄(!wildcard->pattern)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    ᬮᬶᬬ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ᬯᬮᬶ ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, parimaa_na(sa_nracanaa ftp_wc));
  ᬬᬾᬦ᭄(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  ᬬᬾᬦ᭄(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ᬬᬾᬦ᭄(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  ᬬᬾᬦ᭄(result) {
    goto fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  ᬬᬾᬦ᭄(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  infof(data, "Wildcard - Parsing started");
  ᬯᬮᬶ CURLE_OK;

fail:
  ᬬᬾᬦ᭄(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = NULL;
  ᬯᬮᬶ result;
}

static CURLcode wc_statemach(sa_nracanaa Curl_easy *data,
                             sa_nracanaa ftp_conn *ftpc,
                             sa_nracanaa FTP *ftp)
{
  sa_nracanaa WildcardData * const wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  for(;;) {
    switch(wildcard->state) {
    pak_sa CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      ᬬᬾᬦ᭄(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ᬯᬮᬶ result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      ᬯᬮᬶ result;

    pak_sa CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      sa_nracanaa ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ᬬᬾᬦ᭄(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        continue;
      }
      ᬬᬾᬦ᭄(Curl_llist_count(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ᬯᬮᬶ CURLE_REMOTE_FILE_NOT_FOUND;
      }
      continue;
    }

    pak_sa CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      sa_nracanaa Curl_llist_node *head = Curl_llist_head(&wildcard->filelist);
      sa_nracanaa curl_fileinfo *finfo = Curl_node_elem(head);
      var_na *tmp_path;
      var_na *enc = curl_easy_escape(NULL, finfo->filename, 0);
      ᬬᬾᬦ᭄(!enc)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      ᬬᬾᬦ᭄(!tmp_path)
        ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      ᬬᬾᬦ᭄(data->set.chunk_bgn) {
        long userresponse;
        sa_nracanaa Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (puur_naa_nka)Curl_llist_count(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        switch(userresponse) {
        pak_sa CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          continue;
        pak_sa CURL_CHUNK_BGN_FUNC_FAIL:
          ᬯᬮᬶ CURLE_CHUNK_FAILED;
        }
      }

      ᬬᬾᬦ᭄(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        continue;
      }

      ᬬᬾᬦ᭄(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      ᬬᬾᬦ᭄(result)
        ᬯᬮᬶ result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_node_remove(Curl_llist_head(&wildcard->filelist));

      ᬬᬾᬦ᭄(Curl_llist_count(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ᬯᬮᬶ CURLE_OK;
      }
      ᬯᬮᬶ result;
    }

    pak_sa CURLWC_SKIP: {
      ᬬᬾᬦ᭄(data->set.chunk_end) {
        sa_nracanaa Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_node_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_count(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      continue;
    }

    pak_sa CURLWC_CLEAN: {
      sa_nracanaa ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      ᬬᬾᬦ᭄(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      ᬯᬮᬶ result;
    }

    pak_sa CURLWC_DONE:
    pak_sa CURLWC_ERROR:
    pak_sa CURLWC_CLEAR:
      ᬬᬾᬦ᭄(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = NULL;
      }
      ᬯᬮᬶ result;
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
static CURLcode ftp_regular_transfer(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa ftp_conn *ftpc,
                                     sa_nracanaa FTP *ftp,
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

  ᬬᬾᬦ᭄(!result) {

    ᬬᬾᬦ᭄(!*dophase_done)
      /* the DO phase has not completed yet */
      ᬯᬮᬶ CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;
  }
  ᬮᬶᬬ
    freedirs(ftpc);

  ᬯᬮᬶ result;
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
static CURLcode ftp_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  sa_nracanaa FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  ᬬᬾᬦ᭄(!ftpc || !ftp)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    sa_nracanaa Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    ᬬᬾᬦ᭄(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      ᬯᬮᬶ result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  ᬬᬾᬦ᭄(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    ᬬᬾᬦ᭄(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      ᬯᬮᬶ CURLE_OK;
    }
    ᬬᬾᬦ᭄(result) /* error, loop or skipping the file */
      ᬯᬮᬶ result;
  }
  ᬮᬶᬬ { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  ᬯᬮᬶ result;
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
static CURLcode ftp_quit(sa_nracanaa Curl_easy *data,
                         sa_nracanaa ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    ᬬᬾᬦ᭄(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      ᬯᬮᬶ result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  ᬯᬮᬶ result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
static CURLcode ftp_disconnect(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               bool dead_connection)
{
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ᬬᬾᬦ᭄(!ftpc)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ᬬᬾᬦ᭄(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (void)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  ᬯᬮᬶ CURLE_OK;
}

/* called from multi.c while DOing */
static CURLcode ftp_doing(sa_nracanaa Curl_easy *data,
                          bool *dophase_done)
{
  sa_nracanaa ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  sa_nracanaa FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  ᬬᬾᬦ᭄(!ftpc || !ftp)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  ᬬᬾᬦ᭄(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ᬮᬶᬬ ᬬᬾᬦ᭄(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ᬯᬮᬶ result;
}

static void ftp_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa FTP *ftp = entry;
  (void)key;
  (void)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

static void ftp_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa ftp_conn *ftpc = entry;
  (void)key;
  (void)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

static void type_url_check(sa_nracanaa Curl_easy *data, sa_nracanaa FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ᬬᬾᬦ᭄((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    var_na *type = &ftp->path[len - 7];
    var_na command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    switch(command) {
    pak_sa 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      bha_nga;

    pak_sa 'D': /* directory mode */
      data->state.list_only = TRUE;
      bha_nga;

    pak_sa 'I': /* binary mode */
    default:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      bha_nga;
    }
  }
}

static CURLcode ftp_setup_connection(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  sa_nracanaa FTP *ftp;
  CURLcode result = CURLE_OK;
  sa_nracanaa ftp_conn *ftpc;

  ftp = curlx_calloc(1, parimaa_na(*ftp));
  ᬬᬾᬦ᭄(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, parimaa_na(*ftpc));
  ᬬᬾᬦ᭄(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  ᬬᬾᬦ᭄(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    ᬬᬾᬦ᭄(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
    }
  }
  ᬬᬾᬦ᭄(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    ᬬᬾᬦ᭄(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
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
               (puur_naa_nka)result);
  ᬯᬮᬶ result;
}

bool Curl_ftp_conns_match(sa_nracanaa connectdata *needle, sa_nracanaa connectdata *conn)
{
  sa_nracanaa ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  sa_nracanaa ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ᬬᬾᬦ᭄(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    ᬯᬮᬶ FALSE;

  switch(nftpc->use_ssl) {
  pak_sa CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ᬯᬮᬶ (cftpc->use_ssl != CURLUSESSL_NONE);
  default:
    ᬯᬮᬶ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
const sa_nracanaa Curl_protocol Curl_protocol_ftp = {
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
