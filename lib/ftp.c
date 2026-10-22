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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "if2ip.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "ftp-int.h"
#ಅನ್ತರ್ಭಾವ "ftplistparser.h"
#ಅನ್ತರ್ಭಾವ "curl_range.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h" /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(ಸಾಲು) \
  (ISDIGIT((ಸಾಲು)[0]) && ISDIGIT((ಸಾಲು)[1]) && ISDIGIT((ಸಾಲು)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(ಸಾಲು) (STATUSCODE(ಸಾಲು) && (' ' == (ಸಾಲು)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ftp_state_names[] = {
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
ಸ್ಥಿರ ಶೂನ್ಯ ftp_state_ಕೆಳಗಿನ(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , ಪೂರ್ಣಾಙ್ಕ lineno
#endif
  )
{
  ಯದಿ(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((ಶೂನ್ಯ)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#ಅನ್ಯಥಾ
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_ಕೆಳಗಿನ(x, y, z)
#ಅನ್ಯಥಾ /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_ಕೆಳಗಿನ(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

ಸ್ಥಿರ CURLcode ftp_state_mdtm(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp);
ಸ್ಥಿರ CURLcode ftp_state_quote(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ftp_conn *ftpc,
                                ಸಂರಚನಾ FTP *ftp,
                                bool init, ftpstate instate);
ಸ್ಥಿರ CURLcode ftp_nb_type(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ftp_conn *ftpc,
                            ಸಂರಚನಾ FTP *ftp,
                            bool ascii, ftpstate newstate);
ಸ್ಥಿರ CURLcode getftpresponse(ಸಂರಚನಾ Curl_easy *data, size_t *nreadp,
                               ಪೂರ್ಣಾಙ್ಕ *ftpcodep);

ಸ್ಥಿರ ಶೂನ್ಯ freedirs(ಸಂರಚನಾ ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ರಿಕ್ತ;
}

ಸ್ಥಿರ size_t numof_slashes(ಧ್ರುವ ವರ್ಣ *str)
{
  ಧ್ರುವ ವರ್ಣ *slashPos;
  size_t ಅಂಕೆ = 0;
  ಕುರು {
    slashPos = strchr(str, '/');
    ಯದಿ(slashPos) {
      ++ಅಂಕೆ;
      str = slashPos + 1;
    }
  } ಯಾವತ್(slashPos);
  ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ಸ್ಥಿರ CURLcode ftp_parse_url_path(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc,
                                   ಸಂರಚನಾ FTP *ftp)
{
  ಧ್ರುವ ವರ್ಣ *slashPos = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *fileName = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *rawPath = ರಿಕ್ತ; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ಯದಿ(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  ಯದಿ(result) {
    failf(data, "path contains control characters");
    ಪ್ರತ್ಯಾಗಮ result;
  }
  rawPath = ftpc->rawpath;

  ವಿಕಲ್ಪ(data->set.ftp_filemethod) {
  ಪಕ್ಷ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ಯದಿ((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ಭಙ್ಗ;

  ಪಕ್ಷ FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    ಯದಿ(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      ಯದಿ(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, ಪರಿಮಾಣ(ftpc->dirs[0]));
      ಯದಿ(!ftpc->dirs)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (ಪೂರ್ಣಾಙ್ಕ)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ಅನ್ಯಥಾ
      fileName = rawPath; /* filename only (or empty) */
    ಭಙ್ಗ;

  ಸ್ವಭಾವ: /* allow pretty much anything */
  ಪಕ್ಷ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ಧ್ರುವ ವರ್ಣ *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    ಯದಿ(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

    ಯದಿ(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, ಪರಿಮಾಣ(ftpc->dirs[0]));
      ಯದಿ(!ftpc->dirs)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      ಯಾವತ್(dirAlloc--) {
        ಧ್ರುವ ವರ್ಣ *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ಯದಿ(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ಯದಿ(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (ಪೂರ್ಣಾಙ್ಕ)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (ಪೂರ್ಣಾಙ್ಕ)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ಭಙ್ಗ;
  } /* switch */

  ಯದಿ(fileName && *fileName)
    ftpc->file = fileName;
  ಅನ್ಯಥಾ
    ftpc->file = ರಿಕ್ತ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ಯದಿ(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ಯದಿ((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ಅನ್ಯಥಾ { /* newly created FTP connections are already in entry path */
    ಧ್ರುವ ವರ್ಣ *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    ಯದಿ(oldPath) {
      size_t n = pathLen;
      ಯದಿ(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ಅನ್ಯಥಾ
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      ಯದಿ((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ftp_need_type(ಸಂರಚನಾ ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ಪ್ರತ್ಯಾಗಮ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ಸ್ಥಿರ ಶೂನ್ಯ close_secondarysocket(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ftp_conn *ftpc)
{
  (ಶೂನ್ಯ)ftpc;
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
ಸಂರಚನಾ ftp_cw_lc_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
  bool newline_pending;
};

ಸ್ಥಿರ CURLcode ftp_cw_lc_write(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ nl = '\n';
  ಸಂರಚನಾ ftp_cw_lc_ctx *ctx = writer->ctx;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ಯದಿ(!ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ಯಾವತ್(blen) {
    /* do not pass EOS when writing parts */
    ಪೂರ್ಣಾಙ್ಕ chunk_type = (type & ~CLIENTWRITE_EOS);
    ಧ್ರುವ ವರ್ಣ *cp;
    size_t chunk_len;
    CURLcode result;

    ಯದಿ(ctx->newline_pending) {
      ಯದಿ(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ಯದಿ(!cp)
      ಭಙ್ಗ;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ಯದಿ(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ಯದಿ(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  ಅನ್ಯಥಾ ಯದಿ(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ಯದಿ(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ರಿಕ್ತ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ಸ್ಥಿರ CURLcode ftp_check_ctrl_on_data_wait(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ftp_conn *ftpc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
  size_t nread;
  ಪೂರ್ಣಾಙ್ಕ ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ಯದಿ(curlx_dyn_len(&pp->recvbuf)) {
    ಧ್ರುವ ವರ್ಣ *l = curlx_dyn_ptr(&pp->recvbuf);
    ಯದಿ(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (ಶೂನ್ಯ)getftpresponse(data, &nread, &ftpcode);
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ಯದಿ(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ಅನ್ಯಥಾ {
    ಪೂರ್ಣಾಙ್ಕ socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ವಿಕಲ್ಪ(socketstate) {
    ಪಕ್ಷ -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
    ಸ್ವಭಾವ:
      ಯದಿ(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    ಯದಿ(pp->overflow > 3) {
      ಧ್ರುವ ವರ್ಣ *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ಯದಿ((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
      }
    }

    (ಶೂನ್ಯ)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    ಯದಿ(ftpcode / 100 > 3)
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;

    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ಸ್ಥಿರ CURLcode ftp_initiate_transfer(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  ಯದಿ(result || !connected)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  ಅನ್ಯಥಾ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool ftp_endofresp(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                          ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಙ್ಕ *code)
{
  curl_off_t status;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;

  ಯದಿ((len > 3) && LASTLINE(ಸಾಲು) && !curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &status, 999)) {
    *code = (ಪೂರ್ಣಾಙ್ಕ)status;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode ftp_readresp(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ftp_conn *ftpc,
                             int8_t sockindex,
                             ಸಂರಚನಾ pingpong *pp,
                             ಪೂರ್ಣಾಙ್ಕ *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  ಪೂರ್ಣಾಙ್ಕ code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ಯದಿ(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  ಯದಿ(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ಸ್ಥಿರ CURLcode getftpresponse(ಸಂರಚನಾ Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               ಪೂರ್ಣಾಙ್ಕ *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
  size_t nread;
  ಪೂರ್ಣಾಙ್ಕ cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ಯದಿ(!ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯಾವತ್(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    ಯದಿ(timeout <= 0) {
      failf(data, "FTP response timeout");
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ಯದಿ(timeout < interval_ms)
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

    ಯದಿ(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    ಅನ್ಯಥಾ ಯದಿ(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      ಪೂರ್ಣಾಙ್ಕ ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ಯದಿ(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಅನ್ಯಥಾ ಯದಿ(ev == 0) {
        result = Curl_pgrsUpdate(data);
        ಅನುವರ್ತ; /* continue in our loop for the timeout duration */
      }
    }

    ಯದಿ(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      ಯದಿ(result)
        ಭಙ್ಗ;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ಯದಿ(result)
      ಭಙ್ಗ;

    ಯದಿ(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    ಅನ್ಯಥಾ
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (ಪೂರ್ಣಾಙ್ಕ)result, *nreadp, *ftpcodep);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_user(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ಯದಿ(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_pwd(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  ಯದಿ(!data->id && getenv("CURL_FTP_PWD_STOP"))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  ಯದಿ(!result)
    ftp_state(data, ftpc, FTP_PWD);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the FTP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode ftp_pollset(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಪ್ರತ್ಯಾಗಮ ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ಸ್ಥಿರ CURLcode ftp_domore_pollset(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ಯದಿ(!ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ಯದಿ(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  ಪ್ರತ್ಯಾಗಮ Curl_pp_pollset(data, &ftpc->pp, ps);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pathlen(ಸಂರಚನಾ ftp_conn *ftpc, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ಅಂಕೆ);
  ಪ್ರತ್ಯಾಗಮ ftpc->dirs[ಅಂಕೆ].len;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *pathpiece(ಸಂರಚನಾ ftp_conn *ftpc, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ಅಂಕೆ);
  ಪ್ರತ್ಯಾಗಮ &ftpc->rawpath[ftpc->dirs[ಅಂಕೆ].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ಸ್ಥಿರ CURLcode ftp_state_cwd(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ftp_conn *ftpc,
                              ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  ಅನ್ಯಥಾ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ಯದಿ(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ಯದಿ(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      ಅನ್ಯಥಾ {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ಸ್ಥಿರ CURLcode ftp_port_parse_string(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn,
                                      ಧ್ರುವ ವರ್ಣ *string_ftpport,
                                      ಸಂರಚನಾ Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ಧ್ರುವ ವರ್ಣ **hostp,
                                      ವರ್ಣ *hbuf, size_t hbuflen)
{
  ಧ್ರುವ ವರ್ಣ *ip_end = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *addr = ರಿಕ್ತ;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  ವರ್ಣ ipstr[50];
#ifndef USE_IPV6
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ss;
#endif

  /* default to nothing */
  *hostp = ರಿಕ್ತ;
  *port_minp = *port_maxp = 0;

  ಯದಿ(!string_ftpport || (strlen(string_ftpport) <= 1))
    ಗಚ್ಛ done;

#ifdef USE_IPV6
  ಯದಿ(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ಧ್ರುವ ವರ್ಣ *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    ಯದಿ(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  ಅನ್ಯಥಾ
#endif
    ಯದಿ(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    ಅನ್ಯಥಾ {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ಯದಿ(ip_end) {
#ifdef USE_IPV6
        ಸಂರಚನಾ sockaddr_in6 * ಧ್ರುವ sa6 = (ಶೂನ್ಯ *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        ಯದಿ(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = ರಿಕ್ತ; /* this got no port ! */
        }
#endif
      }
      ಅನ್ಯಥಾ
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  ಯದಿ(ip_end) {
    ಧ್ರುವ ವರ್ಣ *portp = strchr(ip_end, ':');
    ಯದಿ(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        ಯದಿ(!curlx_str_single(&portp, '-') &&
           !curlx_str_ಸಂಖ್ಯೆ(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        ಅನ್ಯಥಾ
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ಯದಿ(port_min > port_max)
    port_min = port_max = 0;

  ಯದಿ(addrlen) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ಯದಿ(!remote_addr || (addrlen >= ಪರಿಮಾಣ(ipstr)) || (addrlen >= hbuflen))
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ವಿಕಲ್ಪ(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    ಪಕ್ಷ IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ಭಙ್ಗ;
    ಪಕ್ಷ IF2IP_AF_NOT_SUPPORTED:
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
    ಪಕ್ಷ IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      ಭಙ್ಗ;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ಸ್ಥಿರ CURLcode ftp_port_default_host(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn,
                                      ಸಂರಚನಾ Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ಧ್ರುವ ವರ್ಣ **hostp,
                                      ವರ್ಣ *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ಸಂರಚನಾ sockaddr *sa = (ಸಂರಚನಾ sockaddr *)ss;
  ಸಂರಚನಾ sockaddr_in * ಧ್ರುವ sa4 = (ಶೂನ್ಯ *)sa;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 * ಧ್ರುವ sa6 = (ಶೂನ್ಯ *)sa;
#endif
  ವರ್ಣ buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = ಪರಿಮಾಣ(*ss);
  ಯದಿ(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
  }
  ವಿಕಲ್ಪ(sa->sa_family) {
#ifdef USE_IPV6
  ಪಕ್ಷ AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ಭಙ್ಗ;
  }
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ಸ್ಥಿರ CURLcode ftp_port_resolve_host(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn,
                                      ಧ್ರುವ ವರ್ಣ *host,
                                      ಸಂರಚನಾ Curl_dns_entry **dns_entryp,
                                      ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = ರಿಕ್ತ;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  ಯದಿ(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  ಅನ್ಯಥಾ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ಸ್ಥಿರ CURLcode ftp_port_open_socket(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *res,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  ವರ್ಣ buffer[STRERROR_LEN];
  ಪೂರ್ಣಾಙ್ಕ sockerr = 0;
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  ಪ್ರತಿ(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, ರಿಕ್ತ,
                       Curl_conn_get_transport(data, conn), portsockp);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        ಪ್ರತ್ಯಾಗಮ result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      ಅನುವರ್ತ;
    }
    ಭಙ್ಗ;
  }
  ಯದಿ(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ಸ್ಥಿರ CURLcode ftp_port_bind_socket(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn,
                                     curl_socket_t portsock,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai,
                                     ಸಂರಚನಾ Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  ಸಂರಚನಾ sockaddr *sa = (ಸಂರಚನಾ sockaddr *)ss;
  ಸಂರಚನಾ sockaddr_in * ಧ್ರುವ sa4 = (ಶೂನ್ಯ *)sa;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 * ಧ್ರುವ sa6 = (ಶೂನ್ಯ *)sa;
#endif
  ವರ್ಣ buffer[STRERROR_LEN];
  unsigned short port;
  ಪೂರ್ಣಾಙ್ಕ sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  ಪ್ರತಿ(port = port_min; port <= port_max;) {
    ಯದಿ(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    ಅನ್ಯಥಾ
      sa6->sin6_port = htons(port);
#endif
    ಯದಿ(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ಯದಿ(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));

        *sslen_io = ಪರಿಮಾಣ(*ss);
        ಯದಿ(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
          ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        ಅನುವರ್ತ;
      }
      ಯದಿ(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
      }
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ಯದಿ(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = ಪರಿಮಾಣ(*ss);
  ಯದಿ(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ಸ್ಥಿರ CURLcode ftp_port_listen(ಸಂರಚನಾ Curl_easy *data, curl_socket_t portsock)
{
  ವರ್ಣ buffer[STRERROR_LEN];

  ಯದಿ(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ಸ್ಥಿರ CURLcode ftp_port_send_command(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ftp_conn *ftpc,
                                      ಸಂರಚನಾ connectdata *conn,
                                      ಸಂರಚನಾ Curl_sockaddr_storage *ss,
                                      ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ mode[][5] = { "EPRT", "PORT" };
  ಸಂರಚನಾ sockaddr *sa = (ಸಂರಚನಾ sockaddr *)ss;
  ಸಂರಚನಾ sockaddr_in * ಧ್ರುವ sa4 = (ಶೂನ್ಯ *)sa;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 * ಧ್ರುವ sa6 = (ಶೂನ್ಯ *)sa;
#endif
  ವರ್ಣ myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, ಪರಿಮಾಣ(myhost));

#ifdef USE_IPV6
  ಯದಿ(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  ಪ್ರತಿ(; fcmd != DONE; fcmd++) {

    ಯದಿ(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ಅನುವರ್ತ;

    ಯದಿ((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ಅನುವರ್ತ;

    ವಿಕಲ್ಪ(sa->sa_family) {
    ಪಕ್ಷ AF_INET:
      port = ntohs(sa4->sin_port);
      ಭಙ್ಗ;
#ifdef USE_IPV6
    ಪಕ್ಷ AF_INET6:
      port = ntohs(sa6->sin6_port);
      ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
      ಅನುವರ್ತ; /* might as well skip this */
    }

    ಯದಿ(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      ಯದಿ(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಭಙ್ಗ;
    }
    ಯದಿ(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      ವರ್ಣ target[ಪರಿಮಾಣ(myhost) + 20];
      ಧ್ರುವ ವರ್ಣ *source = myhost;
      ವರ್ಣ *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      ಯಾವತ್(*source) {
        ಯದಿ(*source == '.')
          *dest = ',';
        ಅನ್ಯಥಾ
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (ಪೂರ್ಣಾಙ್ಕ)(port >> 8), (ಪೂರ್ಣಾಙ್ಕ)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      ಯದಿ(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಭಙ್ಗ;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ಸ್ಥಿರ CURLcode ftp_state_use_port(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ಸಂರಚನಾ Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  ವರ್ಣ hbuf[NI_MAXHOST];
  ಧ್ರುವ ವರ್ಣ *host = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  ಸಂರಚನಾ Curl_dns_entry *dns_entry = ರಿಕ್ತ;
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *res = ರಿಕ್ತ;
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai = ರಿಕ್ತ;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, ಪರಿಮಾಣ(hbuf));
  ಯದಿ(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, ಪರಿಮಾಣ(hbuf), &non_local);

  /* resolve host string to address list */
  ಯದಿ(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  ಯದಿ(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  ಯದಿ(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  ಯದಿ(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  ಯದಿ(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ಯದಿ(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  ಯದಿ(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ಯದಿ(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  ಯದಿ(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  ಅನ್ಯಥಾ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ಯದಿ(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  ಯದಿ(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_use_pasv(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc,
                                   ಸಂರಚನಾ connectdata *conn)
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

  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ mode[][5] = { "EPSV", "PASV" };
  ಪೂರ್ಣಾಙ್ಕ modeoff;

#ifdef PF_INET6
  ಯದಿ(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  ಯದಿ(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ಸ್ಥಿರ CURLcode ftp_state_prepare_transfer(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ftp_conn *ftpc,
                                           ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  ಯದಿ(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ಅನ್ಯಥಾ ಯದಿ(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  ಅನ್ಯಥಾ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ಯದಿ(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ಯದಿ(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      ಅನ್ಯಥಾ ಯದಿ(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ಅನ್ಯಥಾ
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    ಅನ್ಯಥಾ
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_rest(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ಯದಿ((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  ಅನ್ಯಥಾ
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_size(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ಯದಿ((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  ಅನ್ಯಥಾ
    result = ftp_state_rest(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_list(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp)
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
  ಧ್ರುವ ವರ್ಣ *lstArg = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ lstArglen = 0;
  ವರ್ಣ *cmd;

  ಯದಿ((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ಧ್ರುವ ವರ್ಣ *rawPath = ftpc->rawpath;
    ಧ್ರುವ ವರ್ಣ *slashPos = strrchr(rawPath, '/');
    ಯದಿ(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      ಯದಿ(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (ಪೂರ್ಣಾಙ್ಕ)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ಯದಿ(!cmd)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ಯದಿ(!result)
    ftp_state(data, ftpc, FTP_LIST);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_list_prequote(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ftp_conn *ftpc,
                                        ಸಂರಚನಾ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಪ್ರತ್ಯಾಗಮ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_retr_prequote(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ftp_conn *ftpc,
                                        ಸಂರಚನಾ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಪ್ರತ್ಯಾಗಮ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_stor_prequote(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ftp_conn *ftpc,
                                        ಸಂರಚನಾ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಪ್ರತ್ಯಾಗಮ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_type(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ಯದಿ(data->req.no_body && ftpc->file &&
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
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
    result = ftp_state_size(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ಸ್ಥಿರ CURLcode ftp_state_mdtm(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ಯದಿ((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  ಅನ್ಯಥಾ
    result = ftp_state_type(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* This is called after the TYPE and possible quote commands have been sent */
ಸ್ಥಿರ CURLcode ftp_state_ul_setup(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc,
                                   ಸಂರಚನಾ FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  ಯದಿ((data->state.resume_from && !sizechecked) ||
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
    ಪೂರ್ಣಾಙ್ಕ seekerr = CURL_SEEKFUNC_OK;

    ಯದಿ(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ಯದಿ(data->set.seek_func) {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ಯದಿ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ಯದಿ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಕುರು {
        ವರ್ಣ scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)ಪರಿಮಾಣ(scratch)) ?
          ಪರಿಮಾಣ(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        ಯದಿ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ಪ್ರತ್ಯಾಗಮ CURLE_FTP_COULDNT_USE_REST;
        }
      } ಯಾವತ್(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    ಯದಿ(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      ಯದಿ(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ಯದಿ(!result)
    ftp_state(data, ftpc, FTP_STOR);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_retr(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ftp_conn *ftpc,
                               ಸಂರಚನಾ FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ಯದಿ(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    ಪ್ರತ್ಯಾಗಮ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ಯದಿ(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ಯದಿ(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ಅನ್ಯಥಾ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ಯದಿ(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ಯದಿ(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    ಯದಿ(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  ಅನ್ಯಥಾ {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_quote(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ftp_conn *ftpc,
                                ಸಂರಚನಾ FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  ಸಂರಚನಾ curl_slist *item;

  ವಿಕಲ್ಪ(instate) {
  ಪಕ್ಷ FTP_QUOTE:
  ಸ್ವಭಾವ:
    item = data->set.quote;
    ಭಙ್ಗ;
  ಪಕ್ಷ FTP_RETR_PREQUOTE:
  ಪಕ್ಷ FTP_STOR_PREQUOTE:
  ಪಕ್ಷ FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    ಭಙ್ಗ;
  ಪಕ್ಷ FTP_POSTQUOTE:
    item = data->set.postquote;
    ಭಙ್ಗ;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ಯದಿ(init)
    ftpc->count1 = 0;
  ಅನ್ಯಥಾ
    ftpc->count1++;

  ಯದಿ(item) {
    ಪೂರ್ಣಾಙ್ಕ i = 0;

    /* Skip count1 items in the linked list */
    ಯಾವತ್((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    ಯದಿ(item) {
      ಧ್ರುವ ವರ್ಣ *cmd = item->data;
      ಯದಿ(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ಅನ್ಯಥಾ
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  ಯದಿ(!quote) {
    /* No more quote to send, continue to ... */
    ವಿಕಲ್ಪ(instate) {
    ಪಕ್ಷ FTP_QUOTE:
    ಸ್ವಭಾವ:
      result = ftp_state_cwd(data, ftpc, ftp);
      ಭಙ್ಗ;
    ಪಕ್ಷ FTP_RETR_PREQUOTE:
      ಯದಿ(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      ಅನ್ಯಥಾ {
        ಯದಿ(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        ಅನ್ಯಥಾ {
          ಯದಿ(data->set.ignorecl || data->state.prefer_ascii) {
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
            ಯದಿ(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          ಅನ್ಯಥಾ {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            ಯದಿ(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      ಭಙ್ಗ;
    ಪಕ್ಷ FTP_POSTQUOTE:
      ಭಙ್ಗ;
    ಪಕ್ಷ FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      ಭಙ್ಗ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ಸ್ಥಿರ CURLcode ftp_epsv_disable(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ftp_conn *ftpc,
                                 ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  ಯದಿ(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_control_addr_dup(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ **newhostp)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  ಯದಿ(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ಅನ್ಯಥಾ
#endif
  ಯದಿ(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ಅನ್ಯಥಾ {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_CANT_GET_HOST;
  }
  ಪ್ರತ್ಯಾಗಮ *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ಸ್ಥಿರ bool match_pasv_6nums(ಧ್ರುವ ವರ್ಣ *p,
                             unsigned ಪೂರ್ಣಾಙ್ಕ *array) /* 6 numbers */
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ(i = 0; i < 6; i++) {
    curl_off_t ಅಂಕೆ;
    ಯದಿ(i) {
      ಯದಿ(*p != ',')
        ಪ್ರತ್ಯಾಗಮ FALSE;
      p++;
    }
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, 0xff))
      ಪ್ರತ್ಯಾಗಮ FALSE;
    array[i] = (unsigned ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode ftp_state_pasv_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result;
  ಧ್ರುವ ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
  ವರ್ಣ *newhost = ರಿಕ್ತ;
  unsigned short newport = 0;
  ಧ್ರುವ ವರ್ಣ *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ಯದಿ((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ಧ್ರುವ ವರ್ಣ *ptr = strchr(str, '(');
    ಯದಿ(ptr) {
      ವರ್ಣ sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ಯದಿ(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ಧ್ರುವ ವರ್ಣ *p = &ptr[3];
        curl_off_t ಅಂಕೆ;
        ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          ಪ್ರತ್ಯಾಗಮ CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)ಅಂಕೆ;
        result = ftp_control_addr_dup(data, &newhost);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ
        ptr = ರಿಕ್ತ;
    }
    ಯದಿ(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned ಪೂರ್ಣಾಙ್ಕ ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ಯಾವತ್(*str) {
      ಯದಿ(match_pasv_6nums(str, ip))
        ಭಙ್ಗ;
      str++;
    }

    ಯದಿ(!*str) {
      failf(data, "Could not interpret the 227-response");
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ಯದಿ(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ಯದಿ(!newhost)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ಅನ್ಯಥಾ ಯದಿ(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ಪ್ರತ್ಯಾಗಮ ftp_epsv_disable(data, ftpc, conn);
  }
  ಅನ್ಯಥಾ {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  ಯದಿ(result)
    ಗಚ್ಛ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ಯದಿ(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    ಯದಿ(result)
      ಗಚ್ಛ error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ಯದಿ(result) {
    ಯದಿ((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    ಗಚ್ಛ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  ಯದಿ(data->set.verbose) {
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
  ಪ್ರತ್ಯಾಗಮ result;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ftp_statemach(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_do_more(ಸಂರಚನಾ Curl_easy *data, domore *more)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ಯದಿ(!ftpc || !ftp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ಯದಿ(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ಯದಿ(result == CURLE_OUT_OF_MEMORY)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      ಯದಿ(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ಪ್ರತ್ಯಾಗಮ ftp_epsv_disable(data, ftpc, conn);
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಯದಿ(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    ಯದಿ(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ಯದಿ(result || !ftpc->wait_data_conn)
      ಪ್ರತ್ಯಾಗಮ result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  ಯದಿ(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ಯದಿ(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result; /* Failed to accept data connection */

      ಯದಿ(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      ಅನ್ಯಥಾ {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    ಅನ್ಯಥಾ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      ಯದಿ(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      ಯದಿ((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ಯದಿ(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
        }
        /* otherwise fall through */
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ಅನ್ಯಥಾ {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      ಯದಿ(complete)
        *more = DOMORE_DONE;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  ಯದಿ(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (ಪೂರ್ಣಾಙ್ಕ)result);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode ftp_dophase_done(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ftp_conn *ftpc,
                                 ಸಂರಚನಾ FTP *ftp,
                                 bool connected)
{
  ಯದಿ(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    ಯದಿ(result) {
      close_secondarysocket(data, ftpc);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಯದಿ(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  ಅನ್ಯಥಾ ಯದಿ(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ftp_state_port_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಸಂರಚನಾ FTP *ftp,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ಯದಿ(ftpcode / 100 != 2) {
    /* the command failed */

    ಯದಿ(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ಯದಿ(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    ಅನ್ಯಥಾ
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  ಅನ್ಯಥಾ {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* return TRUE on error, FALSE on success */
ಸ್ಥಿರ bool twodigit(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ *val)
{
  ಯದಿ(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ *year, ಪೂರ್ಣಾಙ್ಕ *month, ಪೂರ್ಣಾಙ್ಕ *day,
                           ಪೂರ್ಣಾಙ್ಕ *hour, ಪೂರ್ಣಾಙ್ಕ *minute, ಪೂರ್ಣಾಙ್ಕ *second);
UNITTEST bool ftp_213_date(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ *year, ಪೂರ್ಣಾಙ್ಕ *month, ಪೂರ್ಣಾಙ್ಕ *day,
                           ಪೂರ್ಣಾಙ್ಕ *hour, ಪೂರ್ಣಾಙ್ಕ *minute, ಪೂರ್ಣಾಙ್ಕ *second)
{
  ಪೂರ್ಣಾಙ್ಕ century;
  ಯದಿ((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  *year += century * 100;
  ಯದಿ((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode client_write_header(ಸಂರಚನಾ Curl_easy *data,
                                    ವರ್ಣ *buf, size_t blen)
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
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_mdtm_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಸಂರಚನಾ FTP *ftp,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(ftpcode) {
  ಪಕ್ಷ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    ಪೂರ್ಣಾಙ್ಕ year, month, day, hour, minute, second;
    ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
    ಧ್ರುವ ವರ್ಣ *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ಯದಿ(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      ವರ್ಣ timebuf[24];
      curl_msnprintf(timebuf, ಪರಿಮಾಣ(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      ಯದಿ(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ಯದಿ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ತಳ್ಳು
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    ಯದಿ(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#ಯದಿ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ತೆಗೆ
#endif
      ವರ್ಣ headerbuf[128];
      ಪೂರ್ಣಾಙ್ಕ headerbuflen;
      time_t filetime = data->info.filetime;
      ಸಂರಚನಾ tm buffer;
      ಧ್ರುವ ಸಂರಚನಾ tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, ಪರಿಮಾಣ(headerbuf),
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
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    } /* end of a ridiculous amount of conditionals */
  }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    infof(data, "unsupported MDTM reply format");
    ಭಙ್ಗ;
  ಪಕ್ಷ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ಭಙ್ಗ;
  }

  ಯದಿ(data->set.timecondition) {
    ಯದಿ((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      ವಿಕಲ್ಪ(data->set.timecondition) {
      ಪಕ್ಷ CURL_TIMECOND_IFMODSINCE:
      ಸ್ವಭಾವ:
        ಯದಿ(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_TIMECOND_IFUNMODSINCE:
        ಯದಿ(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಭಙ್ಗ;
      } /* switch */
    }
    ಅನ್ಯಥಾ {
      infof(data, "Skipping time comparison");
    }
  }

  ಯದಿ(!result)
    result = ftp_state_type(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_type_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಸಂರಚನಾ FTP *ftp,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_COULDNT_SET_TYPE;
  }
  ಯದಿ(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ಯದಿ(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_size_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಸಂರಚನಾ FTP *ftp,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ಧ್ರುವ ವರ್ಣ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ಯದಿ(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ಧ್ರುವ ವರ್ಣ *start = &buf[4];
    ಧ್ರುವ ವರ್ಣ *fdigit = memchr(start, '\r', len - 4);
    ಯದಿ(fdigit) {
      fdigit--;
      ಯದಿ(*fdigit == '\n')
        fdigit--;
      ಯಾವತ್(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    ಅನ್ಯಥಾ
      fdigit = start;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ಅನ್ಯಥಾ ಯದಿ(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  ಯದಿ(instate == FTP_SIZE) {
    ಯದಿ(filesize != -1) {
      ವರ್ಣ clbuf[128];
      ಪೂರ್ಣಾಙ್ಕ clbuflen = curl_msnprintf(clbuf, ಪರಿಮಾಣ(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  ಅನ್ಯಥಾ ಯದಿ(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_rest_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಸಂರಚನಾ FTP *ftp,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(instate) {
  ಪಕ್ಷ FTP_REST:
  ಸ್ವಭಾವ:
    ಯದಿ(ftpcode == 350) {
      ವರ್ಣ buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_RETR_REST:
    ಯದಿ(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    ಅನ್ಯಥಾ {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_state_stor_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ಯದಿ(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      ಪ್ರತ್ಯಾಗಮ ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
ಸ್ಥಿರ CURLcode ftp_state_get_resp(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc,
                                   ಸಂರಚನಾ FTP *ftp,
                                   ಪೂರ್ಣಾಙ್ಕ ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ಯದಿ((ftpcode == 150) || (ftpcode == 125)) {

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

    ಯದಿ((instate != FTP_LIST) &&
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
      ಯದಿ(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        ಪ್ರತಿ(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ಧ್ರುವ ವರ್ಣ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ಧ್ರುವ ವರ್ಣ *c = &buf[i];
          ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            ಭಙ್ಗ;
          }
        }
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    ಯದಿ(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    ಅನ್ಯಥಾ ಯದಿ((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    ಯದಿ(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    ಯದಿ(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      ಯದಿ(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        ಪ್ರತ್ಯಾಗಮ ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ ftp_initiate_transfer(data, ftpc);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    ಅನ್ಯಥಾ {
      failf(data, "RETR response: %03d", ftpcode);
      ಪ್ರತ್ಯಾಗಮ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* after USER, PASS and ACCT */
ಸ್ಥಿರ CURLcode ftp_state_loggedin(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  ಅನ್ಯಥಾ {
    result = ftp_state_pwd(data, ftpc);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ಸ್ಥಿರ bool ftp_has_ctrl(ಧ್ರುವ ವರ್ಣ *string)
{
  ಧ್ರುವ unsigned ವರ್ಣ *s = (ಧ್ರುವ unsigned ವರ್ಣ *)string;
  ಯಾವತ್(*s) {
    ಯದಿ(*s < 0x20)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    s++;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* for USER and PASS responses */
ಸ್ಥಿರ CURLcode ftp_state_user_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ಯದಿ((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  ಅನ್ಯಥಾ ಯದಿ(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  ಅನ್ಯಥಾ ಯದಿ(ftpcode == 332) {
    ಧ್ರುವ ವರ್ಣ *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    ಯದಿ(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    ಅನ್ಯಥಾ ಯದಿ(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ಅನ್ಯಥಾ {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  ಅನ್ಯಥಾ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ಧ್ರುವ ವರ್ಣ *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    ಯದಿ(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ಯದಿ(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ಅನ್ಯಥಾ {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        ಯದಿ(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    ಅನ್ಯಥಾ {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* for ACCT response */
ಸ್ಥಿರ CURLcode ftp_state_acct_resp(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  ಅನ್ಯಥಾ
    result = ftp_state_loggedin(data, ftpc);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_pwd_resp(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ftp_conn *ftpc,
                             ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
  CURLcode result;

  ಯದಿ(ftpcode == 257) {
    ಧ್ರುವ ವರ್ಣ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    ಸಂರಚನಾ dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ಯಾವತ್(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ಯದಿ('\"' == *ptr) {
      /* it started good */
      ಪ್ರತಿ(ptr++; *ptr; ptr++) {
        ಯದಿ('\"' == *ptr) {
          ಯದಿ('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          ಅನ್ಯಥಾ {
            /* end of path */
            ಯದಿ(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            ಭಙ್ಗ; /* get out of this loop */
          }
        }
        ಅನ್ಯಥಾ {
          ಯದಿ(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        ಯದಿ(result) {
          curlx_dyn_free(&out);
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }
    }
    ಯದಿ(entry_extracted) {
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
      ವರ್ಣ *dir = curlx_dyn_ptr(&out);

      ಯದಿ(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        ಯದಿ(result) {
          curlx_dyn_free(&out);
          ಪ್ರತ್ಯಾಗಮ result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ಯದಿ(!data->state.most_recent_ftp_entrypath)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      ಯದಿ(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }
    ಅನ್ಯಥಾ {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ftpauth[] = { "SSL", "TLS" };

ಸ್ಥಿರ CURLcode ftp_wait_resp(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn,
                              ಸಂರಚನಾ ftp_conn *ftpc,
                              ಪೂರ್ಣಾಙ್ಕ ftpcode)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ಯದಿ(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ಪ್ರತ್ಯಾಗಮ ftp_state_user_resp(data, ftpc, ftpcode);
  }
  ಅನ್ಯಥಾ ಯದಿ(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  ಯದಿ(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ವಿಕಲ್ಪ((long)data->set.ftpsslauth) {
    ಪಕ್ಷ CURLFTPAUTH_DEFAULT:
    ಪಕ್ಷ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (ಪೂರ್ಣಾಙ್ಕ)data->set.ftpsslauth);
      ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  ಅನ್ಯಥಾ
    result = ftp_state_user(data, ftpc, conn);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* an implicit TLS scheme implies TLS on both connections */
ಸ್ಥಿರ unsigned ವರ್ಣ ftp_use_ssl(ಸಂರಚನಾ connectdata *conn,
                                 ಸಂರಚನಾ ftp_conn *ftpc)
{
  ಯದಿ(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ಪ್ರತ್ಯಾಗಮ CURLUSESSL_ALL;
  ಪ್ರತ್ಯಾಗಮ ftpc->use_ssl;
}

ಸ್ಥಿರ CURLcode ftp_pp_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ ftpcode;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ಸಂರಚನಾ pingpong *pp;
  size_t nread = 0;

  ಯದಿ(!ftpc || !ftp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ಯದಿ(pp->sendleft)
    ಪ್ರತ್ಯಾಗಮ Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ಯದಿ(result || !ftpcode)
    ಪ್ರತ್ಯಾಗಮ result;

  /* we have now received a full FTP server response */
  ವಿಕಲ್ಪ(ftpc->state) {
  ಪಕ್ಷ FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ಯದಿ(pp->overflow)
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ಯದಿ((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      ಯದಿ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ಯದಿ(result) {
          /* we failed and bail out */
          ಪ್ರತ್ಯಾಗಮ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      ಯದಿ(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      ಅನ್ಯಥಾ
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_USER:
  ಪಕ್ಷ FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ಯದಿ(!result)
      ftp_state(data, ftpc, FTP_PROT);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PROT:
    ಯದಿ(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ಅನ್ಯಥಾ ಯದಿ(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ಪ್ರತ್ಯಾಗಮ CURLE_USE_SSL_FAILED;

    ಯದಿ(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      ಯದಿ(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    ಅನ್ಯಥಾ
      result = ftp_state_pwd(data, ftpc);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_CCC:
    ಯದಿ(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned ವರ್ಣ)CURLFTPSSL_CCC_ACTIVE));
      ಯದಿ(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    ಯದಿ(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_SYST:
    ಯದಿ(ftpcode == 215) {
      ಧ್ರುವ ವರ್ಣ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ಧ್ರುವ ವರ್ಣ *start;
      ವರ್ಣ *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ಯಾವತ್(*ptr == ' ')
        ptr++;
      ಪ್ರತಿ(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      ಯದಿ(!os)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      ಯದಿ(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ಯದಿ(result) {
          curlx_free(os);
          ಪ್ರತ್ಯಾಗಮ result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        ಭಙ್ಗ;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ಅನ್ಯಥಾ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_NAMEFMT:
    ಯದಿ(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      ಭಙ್ಗ;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_QUOTE:
  ಪಕ್ಷ FTP_POSTQUOTE:
  ಪಕ್ಷ FTP_RETR_PREQUOTE:
  ಪಕ್ಷ FTP_STOR_PREQUOTE:
  ಪಕ್ಷ FTP_LIST_PREQUOTE:
    ಯದಿ((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    ಅನ್ಯಥಾ
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_CWD:
    ಯದಿ(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ಯದಿ(data->set.ftp_create_missing_dirs &&
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
        ಯದಿ(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      ಅನ್ಯಥಾ {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ಅನ್ಯಥಾ {
      /* success */
      ftpc->count2 = 0;
      ಯದಿ(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      ಅನ್ಯಥಾ {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_MKD:
    ಯದಿ((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    ಅನ್ಯಥಾ {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_TYPE:
  ಪಕ್ಷ FTP_LIST_TYPE:
  ಪಕ್ಷ FTP_RETR_TYPE:
  ಪಕ್ಷ FTP_STOR_TYPE:
  ಪಕ್ಷ FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_SIZE:
  ಪಕ್ಷ FTP_RETR_SIZE:
  ಪಕ್ಷ FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_REST:
  ಪಕ್ಷ FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PRET:
    ಯದಿ(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_LIST:
  ಪಕ್ಷ FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    ಭಙ್ಗ;

  ಪಕ್ಷ FTP_QUIT:
  ಸ್ವಭಾವ:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ftp_multi_statemach(ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಪ್ರತ್ಯಾಗಮ ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode ftp_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_conn *ftpc)
{
  ಸಂರಚನಾ pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  ಯಾವತ್(ftpc->state != FTP_STOP) {
    ಯದಿ(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    ಯದಿ(result)
      ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ಸ್ಥಿರ CURLcode ftp_connect(ಸಂರಚನಾ Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ pingpong *pp;

  *done = FALSE; /* default to not done yet */
  ಯದಿ(!ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  ಯದಿ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_sendquote(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ftp_conn *ftpc,
                              ಸಂರಚನಾ curl_slist *quote)
{
  ಸಂರಚನಾ curl_slist *item;
  ಸಂರಚನಾ pingpong *pp = &ftpc->pp;

  item = quote;
  ಯಾವತ್(item) {
    ಯದಿ(item->data) {
      size_t nread;
      ಧ್ರುವ ವರ್ಣ *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      ಪೂರ್ಣಾಙ್ಕ ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ಯದಿ(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ಯದಿ(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      ಯದಿ(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ftp_done_status(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  ವಿಕಲ್ಪ(status) {
  ಪಕ್ಷ CURLE_BAD_DOWNLOAD_RESUME:
  ಪಕ್ಷ CURLE_FTP_WEIRD_PASV_REPLY:
  ಪಕ್ಷ CURLE_FTP_PORT_FAILED:
  ಪಕ್ಷ CURLE_FTP_ACCEPT_FAILED:
  ಪಕ್ಷ CURLE_FTP_ACCEPT_TIMEOUT:
  ಪಕ್ಷ CURLE_FTP_COULDNT_SET_TYPE:
  ಪಕ್ಷ CURLE_FTP_COULDNT_RETR_FILE:
  ಪಕ್ಷ CURLE_PARTIAL_FILE:
  ಪಕ್ಷ CURLE_UPLOAD_FAILED:
  ಪಕ್ಷ CURLE_REMOTE_ACCESS_DENIED:
  ಪಕ್ಷ CURLE_FILESIZE_EXCEEDED:
  ಪಕ್ಷ CURLE_REMOTE_FILE_NOT_FOUND:
  ಪಕ್ಷ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  ಪಕ್ಷ CURLE_OK: /* does not affect the control connection's status */
    ಯದಿ(!premature)
      ಭಙ್ಗ;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  ಸ್ವಭಾವ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    ಪ್ರತ್ಯಾಗಮ status;      /* use the already set error code */
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_done_wildcard(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ftp_conn *ftpc)
{
  ಯದಿ(data->state.wildcardmatch) {
    ಯದಿ(data->set.chunk_end && ftpc->file) {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_done_path(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ftp_conn *ftpc,
                          CURLcode result)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ಅನ್ಯಥಾ { /* remember working directory for connection reuse */
    ಧ್ರುವ ವರ್ಣ *rawPath = ftpc->rawpath;
    ಯದಿ(rawPath) {
      ಯದಿ((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ಅನ್ಯಥಾ {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ಯದಿ(!ftpc->cwdfail) {
          ಯದಿ(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          ಅನ್ಯಥಾ
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        ಅನ್ಯಥಾ
          ftpc->prevpath = ರಿಕ್ತ; /* no path */
      }
    }
    ಯದಿ(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ಸ್ಥಿರ CURLcode ftp_done_secondary_socket(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ftp_conn *ftpc,
                                          CURLcode result)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    ಯದಿ(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      ಯದಿ(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_done_control_reply(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ftp_conn *ftpc,
                                       ಸಂರಚನಾ FTP *ftp, CURLcode result,
                                       bool premature)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  size_t nread;
  ಪೂರ್ಣಾಙ್ಕ ftpcode;

  ಯದಿ(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    ಯದಿ(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ವಿಕಲ್ಪ(ftpcode) {
      ಪಕ್ಷ 226:
      ಪಕ್ಷ 250:
        ಭಙ್ಗ;
      ಪಕ್ಷ 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        ಭಙ್ಗ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ftp_done_check_partial(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ftp_conn *ftpc,
                                       ಸಂರಚನಾ FTP *ftp, CURLcode result,
                                       bool premature)
{
  ಯದಿ(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ಅನ್ಯಥಾ ಯದಿ(data->state.upload) {
    ಯದಿ((ftp->transfer == PPTRANSFER_BODY) &&
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
  ಅನ್ಯಥಾ {
    ಯದಿ((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    ಅನ್ಯಥಾ ಯದಿ(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                         bool premature)
{
  ಸಂರಚನಾ FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  ಯದಿ(!ftp || !ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

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
  ಯದಿ(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ಸ್ಥಿರ CURLcode ftp_nb_type(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ftp_conn *ftpc,
                            ಸಂರಚನಾ FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  ವರ್ಣ want = (ವರ್ಣ)(ascii ? 'A' : 'I');

  ಯದಿ(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    ಪ್ರತ್ಯಾಗಮ ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  ಯದಿ(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ಸ್ಥಿರ CURLcode ftp_perform(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ ftp_conn *ftpc,
  ಸಂರಚನಾ FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ಯದಿ(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  ಯದಿ(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ಅನ್ಯಥಾ
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ಯದಿ(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ wc_data_dtor(ಶೂನ್ಯ *ptr)
{
  ಸಂರಚನಾ ftp_wc *ftpwc = ptr;
  ಯದಿ(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ಸ್ಥಿರ CURLcode init_wc_data(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ftp_conn *ftpc,
                             ಸಂರಚನಾ FTP *ftp)
{
  ವರ್ಣ *last_slash;
  ವರ್ಣ *path = ftp->path;
  ಸಂರಚನಾ WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ftp_wc *ftpwc = ರಿಕ್ತ;

  last_slash = strrchr(ftp->path, '/');
  ಯದಿ(last_slash) {
    last_slash++;
    ಯದಿ(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ಪ್ರತ್ಯಾಗಮ ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    ಯದಿ(!wildcard->pattern)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  ಅನ್ಯಥಾ { /* there is only 'wildcard pattern' or nothing */
    ಯದಿ(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      ಯದಿ(!wildcard->pattern)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    ಅನ್ಯಥಾ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ಪ್ರತ್ಯಾಗಮ ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ ftp_wc));
  ಯದಿ(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  ಯದಿ(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ಯದಿ(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  ಯದಿ(result) {
    ಗಚ್ಛ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  ಯದಿ(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  infof(data, "Wildcard - Parsing started");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

fail:
  ಯದಿ(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode wc_statemach(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ftp_conn *ftpc,
                             ಸಂರಚನಾ FTP *ftp)
{
  ಸಂರಚನಾ WildcardData * ಧ್ರುವ wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  ಪ್ರತಿ(;;) {
    ವಿಕಲ್ಪ(wildcard->state) {
    ಪಕ್ಷ CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      ಯದಿ(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ಪ್ರತ್ಯಾಗಮ result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      ಪ್ರತ್ಯಾಗಮ result;

    ಪಕ್ಷ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ಸಂರಚನಾ ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ಯದಿ(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ಅನುವರ್ತ;
      }
      ಯದಿ(Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;
      }
      ಅನುವರ್ತ;
    }

    ಪಕ್ಷ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ಸಂರಚನಾ Curl_llist_ಗಂಟು *head = Curl_llist_head(&wildcard->filelist);
      ಸಂರಚನಾ curl_fileinfo *finfo = Curl_ಗಂಟು_elem(head);
      ವರ್ಣ *tmp_path;
      ವರ್ಣ *enc = curl_easy_escape(ರಿಕ್ತ, finfo->filename, 0);
      ಯದಿ(!enc)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      ಯದಿ(!tmp_path)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      ಯದಿ(data->set.chunk_bgn) {
        long userresponse;
        ಸಂರಚನಾ Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (ಪೂರ್ಣಾಙ್ಕ)Curl_llist_ಎಣಿಕೆ(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        ವಿಕಲ್ಪ(userresponse) {
        ಪಕ್ಷ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ಅನುವರ್ತ;
        ಪಕ್ಷ CURL_CHUNK_BGN_FUNC_FAIL:
          ಪ್ರತ್ಯಾಗಮ CURLE_CHUNK_FAILED;
        }
      }

      ಯದಿ(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ಅನುವರ್ತ;
      }

      ಯದಿ(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ಗಂಟು_remove(Curl_llist_head(&wildcard->filelist));

      ಯದಿ(Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ಪಕ್ಷ CURLWC_SKIP: {
      ಯದಿ(data->set.chunk_end) {
        ಸಂರಚನಾ Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_ಗಂಟು_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ಅನುವರ್ತ;
    }

    ಪಕ್ಷ CURLWC_CLEAN: {
      ಸಂರಚನಾ ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      ಯದಿ(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ಪಕ್ಷ CURLWC_DONE:
    ಪಕ್ಷ CURLWC_ERROR:
    ಪಕ್ಷ CURLWC_CLEAR:
      ಯದಿ(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ರಿಕ್ತ;
      }
      ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_regular_transfer(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ftp_conn *ftpc,
                                     ಸಂರಚನಾ FTP *ftp,
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

  ಯದಿ(!result) {

    ಯದಿ(!*dophase_done)
      /* the DO phase has not completed yet */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
    freedirs(ftpc);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  ಯದಿ(!ftpc || !ftp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ಸಂರಚನಾ Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    ಯದಿ(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  ಯದಿ(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    ಯದಿ(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಯದಿ(result) /* error, loop or skipping the file */
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode ftp_quit(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    ಯದಿ(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode ftp_disconnect(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               bool dead_connection)
{
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ಯದಿ(!ftpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ಯದಿ(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (ಶೂನ್ಯ)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode ftp_doing(ಸಂರಚನಾ Curl_easy *data,
                          bool *dophase_done)
{
  ಸಂರಚನಾ ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ಸಂರಚನಾ FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  ಯದಿ(!ftpc || !ftp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  ಯದಿ(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ಅನ್ಯಥಾ ಯದಿ(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ FTP *ftp = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ftp_conn *ftpc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ಸ್ಥಿರ ಶೂನ್ಯ type_url_check(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ಯದಿ((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    ವರ್ಣ *type = &ftp->path[len - 7];
    ವರ್ಣ command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ವಿಕಲ್ಪ(command) {
    ಪಕ್ಷ 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      ಭಙ್ಗ;

    ಪಕ್ಷ 'D': /* directory mode */
      data->state.list_only = TRUE;
      ಭಙ್ಗ;

    ಪಕ್ಷ 'I': /* binary mode */
    ಸ್ವಭಾವ:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      ಭಙ್ಗ;
    }
  }
}

ಸ್ಥಿರ CURLcode ftp_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ FTP *ftp;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ftp_conn *ftpc;

  ftp = curlx_calloc(1, ಪರಿಮಾಣ(*ftp));
  ಯದಿ(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, ಪರಿಮಾಣ(*ftpc));
  ಯದಿ(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  ಯದಿ(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    ಯದಿ(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  ಯದಿ(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    ಯದಿ(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
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
               (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_ftp_conns_match(ಸಂರಚನಾ connectdata *needle, ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ಸಂರಚನಾ ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ಯದಿ(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ವಿಕಲ್ಪ(nftpc->use_ssl) {
  ಪಕ್ಷ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ಪ್ರತ್ಯಾಗಮ (cftpc->use_ssl != CURLUSESSL_NONE);
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_ftp = {
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
