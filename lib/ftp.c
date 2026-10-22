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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"

#ifndef CURL_DISABLE_FTP

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "if2ip.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "ftp.h"
#ଅନ୍ତର୍ଭାଵ "ftp-int.h"
#ଅନ୍ତର୍ଭାଵ "ftplistparser.h"
#ଅନ୍ତର୍ଭାଵ "curl_range.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h" /* for the week day and month names */
#ଅନ୍ତର୍ଭାଵ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curl_ctype.h"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#define STATUSCODE(ଧାଡ଼ି) \
  (ISDIGIT((ଧାଡ଼ି)[0]) && ISDIGIT((ଧାଡ଼ି)[1]) && ISDIGIT((ଧାଡ଼ି)[2]))

/* macro to check for the last line in an FTP server response */
#define LASTLINE(ଧାଡ଼ି) (STATUSCODE(ଧାଡ଼ି) && (' ' == (ଧାଡ଼ି)[3]))

#ifdef CURLVERBOSE
/* for tracing purposes */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ ftp_state_names[] = {
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
ସ୍ଥିର ଶୂନ୍ୟ ftp_state_ତଳ(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା ftp_conn *ftpc,
                          ftpstate newstate
#ifdef DEBUGBUILD
                          , ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineno
#endif
  )
{
  ଯଦି(ftpc->state != newstate) {
#ifdef DEBUGBUILD
    NOVERBOSE((ଶୂନ୍ୟ)lineno);
    CURL_TRC_FTP(data, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate], lineno);
#ଅନ୍ୟଥା
    CURL_TRC_FTP(data, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_names[newstate]);
#endif
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ifndef DEBUGBUILD
#define ftp_state(x, y, z) ftp_state_ତଳ(x, y, z)
#ଅନ୍ୟଥା /* !DEBUGBUILD */
#define ftp_state(x, y, z) ftp_state_ତଳ(x, y, z, __LINE__)
#endif /* DEBUGBUILD */

ସ୍ଥିର CURLcode ftp_state_mdtm(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp);
ସ୍ଥିର CURLcode ftp_state_quote(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ftp_conn *ftpc,
                                ସଂରଚନା FTP *ftp,
                                bool init, ftpstate instate);
ସ୍ଥିର CURLcode ftp_nb_type(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ftp_conn *ftpc,
                            ସଂରଚନା FTP *ftp,
                            bool ascii, ftpstate newstate);
ସ୍ଥିର CURLcode getftpresponse(ସଂରଚନା Curl_easy *data, size_t *nreadp,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ftpcodep);

ସ୍ଥିର ଶୂନ୍ୟ freedirs(ସଂରଚନା ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ଖାଲି;
}

ସ୍ଥିର size_t numof_slashes(ଅଚଳ ଅକ୍ଷର *str)
{
  ଅଚଳ ଅକ୍ଷର *slashPos;
  size_t ଅଙ୍କ = 0;
  କର {
    slashPos = strchr(str, '/');
    ଯଦି(slashPos) {
      ++ଅଙ୍କ;
      str = slashPos + 1;
    }
  } ଯେତେବେଳେ(slashPos);
  ଫେରନ୍ତୁ ଅଙ୍କ;
}

#define FTP_MAX_DIR_DEPTH 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ସ୍ଥିର CURLcode ftp_parse_url_path(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc,
                                   ସଂରଚନା FTP *ftp)
{
  ଅଚଳ ଅକ୍ଷର *slashPos = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *fileName = ଖାଲି;
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *rawPath = ଖାଲି; /* URL-decoded "raw" path */
  size_t pathLen = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ଯଦି(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  result = Curl_urldecode(ftp->path, 0, &ftpc->rawpath, &pathLen, REJECT_CTRL);
  ଯଦି(result) {
    failf(data, "path contains control characters");
    ଫେରନ୍ତୁ result;
  }
  rawPath = ftpc->rawpath;

  ବାଛ(data->set.ftp_filemethod) {
  ସ୍ଥିତି FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ଯଦି((pathLen > 0) && (rawPath[pathLen - 1] != '/'))
      fileName = rawPath;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTPFILE_SINGLECWD:
    slashPos = strrchr(rawPath, '/');
    ଯଦି(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - rawPath;
      ଯଦି(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, ଆକାର(ftpc->dirs[0]));
      ଯଦି(!ftpc->dirs)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      ftpc->dirs[0].start = 0;
      ftpc->dirs[0].len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ଅନ୍ୟଥା
      fileName = rawPath; /* filename only (or empty) */
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* allow pretty much anything */
  ସ୍ଥିତି FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ଅଚଳ ଅକ୍ଷର *curPos = rawPath;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(rawPath);

    ଯଦି(dirAlloc >= FTP_MAX_DIR_DEPTH)
      /* suspiciously deep directory hierarchy */
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

    ଯଦି(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, ଆକାର(ftpc->dirs[0]));
      ଯଦି(!ftpc->dirs)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      /* parse the URL path into separate path components */
      ଯେତେବେଳେ(dirAlloc--) {
        ଅଚଳ ଅକ୍ଷର *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ଯଦି(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ଯଦି(clen) {
          ftpc->dirs[ftpc->dirdepth].start = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(curPos - rawPath);
          ftpc->dirs[ftpc->dirdepth].len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ଭାଙ୍ଗ;
  } /* switch */

  ଯଦି(fileName && *fileName)
    ftpc->file = fileName;
  ଅନ୍ୟଥା
    ftpc->file = ଖାଲି; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ଯଦି(data->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(data, "Uploading to a URL without a filename");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ଯଦି((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ଅନ୍ୟଥା { /* newly created FTP connections are already in entry path */
    ଅଚଳ ଅକ୍ଷର *oldPath = data->conn->bits.reuse ? ftpc->prevpath : "";
    ଯଦି(oldPath) {
      size_t n = pathLen;
      ଯଦି(data->set.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ଅନ୍ୟଥା
        n -= ftpc->file ? strlen(ftpc->file) : 0;

      ଯଦି((strlen(oldPath) == n) && rawPath && !strncmp(rawPath, oldPath, n)) {
        infof(data, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftp_need_type(ସଂରଚନା ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ଫେରନ୍ତୁ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ସ୍ଥିର ଶୂନ୍ୟ close_secondarysocket(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ftp_conn *ftpc)
{
  (ଶୂନ୍ୟ)ftpc;
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
ସଂରଚନା ftp_cw_lc_ctx {
  ସଂରଚନା Curl_cwriter super;
  bool newline_pending;
};

ସ୍ଥିର CURLcode ftp_cw_lc_write(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର nl = '\n';
  ସଂରଚନା ftp_cw_lc_ctx *ctx = writer->ctx;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ଯଦି(!ftpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ଯେତେବେଳେ(blen) {
    /* do not pass EOS when writing parts */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା chunk_type = (type & ~CLIENTWRITE_EOS);
    ଅଚଳ ଅକ୍ଷର *cp;
    size_t chunk_len;
    CURLcode result;

    ଯଦି(ctx->newline_pending) {
      ଯଦି(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        result = Curl_cwriter_write(data, writer->next, chunk_type, &nl, 1);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ଯଦି(!cp)
      ଭାଙ୍ଗ;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ଯଦି(chunk_len) {
      result = Curl_cwriter_write(data, writer->next, chunk_type,
                                  buf, chunk_len);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ଯଦି(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, blen);
  }
  ଅନ୍ୟଥା ଯଦି(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ଯଦି(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, 0);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ଖାଲି,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ଆକାର(ସଂରଚନା ftp_cw_lc_ctx)
};

#endif /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ସ୍ଥିର CURLcode ftp_check_ctrl_on_data_wait(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ftp_conn *ftpc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ସଂରଚନା pingpong *pp = &ftpc->pp;
  size_t nread;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ଯଦି(curlx_dyn_len(&pp->recvbuf)) {
    ଅଚଳ ଅକ୍ଷର *l = curlx_dyn_ptr(&pp->recvbuf);
    ଯଦି(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(data, "There is negative response in cache while serv connect");
      (ଶୂନ୍ୟ)getftpresponse(data, &nread, &ftpcode);
      ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ଯଦି(pp->overflow)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ଅନ୍ୟଥା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ବାଛ(socketstate) {
    ସ୍ଥିତି -1: /* error */
      /* let's die here */
      failf(data, "Error while waiting for server connect");
      ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଯଦି(socketstate & CURL_CSELECT_IN)
        response = TRUE;
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(response) {
    infof(data, "Ctrl conn has data while waiting for data conn");
    ଯଦି(pp->overflow > 3) {
      ଅଚଳ ଅକ୍ଷର *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->overflow + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ଯଦି((len > 3) && LASTLINE(r)) {
        curl_off_t status;
        ଯଦି(!curlx_str_ସଂଖ୍ୟା(&r, &status, 999) && (status == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(data, "Got 226 before data activity");
          ଫେରନ୍ତୁ CURLE_OK;
        }
      }
    }

    (ଶୂନ୍ୟ)getftpresponse(data, &nread, &ftpcode);

    infof(data, "FTP code: %03d", ftpcode);

    ଯଦି(ftpcode / 100 > 3)
      ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;

    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ସ୍ଥିର CURLcode ftp_initiate_transfer(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(data, "ftp_initiate_transfer()");
  result = Curl_conn_connect(data, SECONDARYSOCKET, TRUE, &connected);
  ଯଦି(result || !connected)
    ଫେରନ୍ତୁ result;

  ଯଦି(data->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_setup_send(data, SECONDARYSOCKET);
    Curl_xfer_set_shutdown(data, TRUE, TRUE);
  }
  ଅନ୍ୟଥା {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_setup_recv(data, SECONDARYSOCKET, data->req.size);
    Curl_xfer_set_shutdown(data, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(data, ftpc, FTP_STOP);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool ftp_endofresp(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                          ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *code)
{
  curl_off_t status;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;

  ଯଦି((len > 3) && LASTLINE(ଧାଡ଼ି) && !curlx_str_ସଂଖ୍ୟା(&ଧାଡ଼ି, &status, 999)) {
    *code = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status;
    ଫେରନ୍ତୁ TRUE;
  }

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode ftp_readresp(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ftp_conn *ftpc,
                             int8_t sockindex,
                             ସଂରଚନା pingpong *pp,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ftpcodep, /* return the ftp-code if done */
                             size_t *size) /* size of the response */
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା code;
  CURLcode result = Curl_pp_readresp(data, sockindex, pp, &code, size);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ଯଦି(!ftpc->shutdown)
    data->info.httpcode = code;

  *ftpcodep = code;

  ଯଦି(code == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(data, "We got a 421 - timeout");
    ftp_state(data, ftpc, FTP_STOP);
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }

  ଫେରନ୍ତୁ result;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ସ୍ଥିର CURLcode getftpresponse(ସଂରଚନା Curl_easy *data,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode result = CURLE_OK;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ସଂରଚନା pingpong *pp = &ftpc->pp;
  size_t nread;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(data, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ଯଦି(!ftpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯେତେବେଳେ(!*ftpcodep && !result) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(data, pp);
    timediff_t interval_ms;

    ଯଦି(timeout <= 0) {
      failf(data, "FTP response timeout");
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ଯଦି(timeout < interval_ms)
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

    ଯଦି(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    ଅନ୍ୟଥା ଯଦି(!Curl_conn_data_pending(data, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(data, pp) ?
        sockfd : CURL_SOCKET_BAD;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ଯଦି(ev < 0) {
        failf(data, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଅନ୍ୟଥା ଯଦି(ev == 0) {
        result = Curl_pgrsUpdate(data);
        ଜାରି; /* continue in our loop for the timeout duration */
      }
    }

    ଯଦି(Curl_pp_needs_flush(data, pp)) {
      result = Curl_pp_flushsend(data, pp);
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }

    result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
         data */
      cache_skip++;
    ଅନ୍ୟଥା
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(data, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *nreadp, *ftpcodep);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_user(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା connectdata *conn)
{
  CURLcode result = Curl_pp_sendf(data, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ଯଦି(!result) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(data, ftpc, FTP_USER);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_pwd(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ftp_conn *ftpc)
{
  CURLcode result;
#ifdef DEBUGBUILD
  ଯଦି(!data->id && getenv("CURL_FTP_PWD_STOP"))
    ଫେରନ୍ତୁ CURLE_OK;
#endif
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PWD");
  ଯଦି(!result)
    ftp_state(data, ftpc, FTP_PWD);

  ଫେରନ୍ତୁ result;
}

/* For the FTP "protocol connect" and "doing" phases only */
ସ୍ଥିର CURLcode ftp_pollset(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ଫେରନ୍ତୁ ftpc ? Curl_pp_pollset(data, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ସ୍ଥିର CURLcode ftp_domore_pollset(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);

  ଯଦି(!ftpc)
    ଫେରନ୍ତୁ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(data, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ଯଦି(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(data->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (data->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(data->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ଫେରନ୍ତୁ Curl_pollset_add_in(data, ps, data->conn->sock[FIRSTSOCKET]);
  }
  ଫେରନ୍ତୁ Curl_pp_pollset(data, &ftpc->pp, ps);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pathlen(ସଂରଚନା ftp_conn *ftpc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ଅଙ୍କ);
  ଫେରନ୍ତୁ ftpc->dirs[ଅଙ୍କ].len;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *pathpiece(ସଂରଚନା ftp_conn *ftpc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ଅଙ୍କ);
  ଫେରନ୍ତୁ &ftpc->rawpath[ftpc->dirs[ଅଙ୍କ].start];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ସ୍ଥିର CURLcode ftp_state_cwd(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ftp_conn *ftpc,
                              ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ଯଦି(ftpc->cwddone)
    /* already done and fine */
    result = ftp_state_mdtm(data, ftpc, ftp);
  ଅନ୍ୟଥା {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((data->set.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ଯଦି(data->conn->bits.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_CWD);
    }
    ଅନ୍ୟଥା {
      ଯଦି(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ଯଦି(!result)
          ftp_state(data, ftpc, FTP_CWD);
      }
      ଅନ୍ୟଥା {
        /* No CWD necessary */
        result = ftp_state_mdtm(data, ftpc, ftp);
      }
    }
  }
  ଫେରନ୍ତୁ result;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  EPRT,
  PORT,
  DONE
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ସ୍ଥିର CURLcode ftp_port_parse_string(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn,
                                      ଅଚଳ ଅକ୍ଷର *string_ftpport,
                                      ସଂରଚନା Curl_sockaddr_storage *ss,
                                      unsigned short *port_minp,
                                      unsigned short *port_maxp,
                                      ଅଚଳ ଅକ୍ଷର **hostp,
                                      ଅକ୍ଷର *hbuf, size_t hbuflen)
{
  ଅଚଳ ଅକ୍ଷର *ip_end = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *addr = ଖାଲି;
  size_t addrlen = 0;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  ଅକ୍ଷର ipstr[50];
#ifndef USE_IPV6
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)ss;
#endif

  /* default to nothing */
  *hostp = ଖାଲି;
  *port_minp = *port_maxp = 0;

  ଯଦି(!string_ftpport || (strlen(string_ftpport) <= 1))
    ଯାଅ done;

#ifdef USE_IPV6
  ଯଦି(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ଅଚଳ ଅକ୍ଷର *ip_start = string_ftpport + 1;
    ip_end = strchr(ip_start, ']');
    ଯଦି(ip_end) {
      addrlen = ip_end - ip_start;
      addr = ip_start;
    }
  }
  ଅନ୍ୟଥା
#endif
    ଯଦି(*string_ftpport == ':') {
      /* :port */
      ip_end = string_ftpport;
    }
    ଅନ୍ୟଥା {
      ip_end = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ଯଦି(ip_end) {
#ifdef USE_IPV6
        ସଂରଚନା sockaddr_in6 * ଅଚଳ sa6 = (ଶୂନ୍ୟ *)ss;
#endif
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_end - string_ftpport;
#ifdef USE_IPV6
        ଯଦି(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = strlen(string_ftpport);
          ip_end = ଖାଲି; /* this got no port ! */
        }
#endif
      }
      ଅନ୍ୟଥା
        /* ipv4|interface */
        addrlen = strlen(string_ftpport);
    }

  /* parse the port */
  ଯଦି(ip_end) {
    ଅଚଳ ଅକ୍ଷର *portp = strchr(ip_end, ':');
    ଯଦି(portp) {
      curl_off_t start;
      curl_off_t end;
      portp++;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&portp, &start, 0xffff)) {
        port_min = (unsigned short)start;
        ଯଦି(!curlx_str_single(&portp, '-') &&
           !curlx_str_ସଂଖ୍ୟା(&portp, &end, 0xffff))
          port_max = (unsigned short)end;
        ଅନ୍ୟଥା
          port_max = port_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ଯଦି(port_min > port_max)
    port_min = port_max = 0;

  ଯଦି(addrlen) {
    ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(data, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ଯଦି(!remote_addr || (addrlen >= ଆକାର(ipstr)) || (addrlen >= hbuflen))
      ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
    memcpy(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ବାଛ(Curl_if2ip(remote_addr->family,
#ifdef USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#endif
                      ipstr, hbuf, hbuflen)) {
    ସ୍ଥିତି IF2IP_NOT_FOUND:
      /* not an interface, use the string as hostname instead */
      memcpy(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି IF2IP_AF_NOT_SUPPORTED:
      ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
    ସ୍ଥିତି IF2IP_FOUND:
      *hostp = hbuf; /* use the hbuf for hostname */
      ଭାଙ୍ଗ;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

done:
  *port_minp = port_min;
  *port_maxp = port_max;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ସ୍ଥିର CURLcode ftp_port_default_host(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn,
                                      ସଂରଚନା Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ଅଚଳ ଅକ୍ଷର **hostp,
                                      ଅକ୍ଷର *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ସଂରଚନା sockaddr *sa = (ସଂରଚନା sockaddr *)ss;
  ସଂରଚନା sockaddr_in * ଅଚଳ sa4 = (ଶୂନ୍ୟ *)sa;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 * ଅଚଳ sa6 = (ଶୂନ୍ୟ *)sa;
#endif
  ଅକ୍ଷର buffer[STRERROR_LEN];
  CURLcode result;

  *sslenp = ଆକାର(*ss);
  ଯଦି(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
  }
  ବାଛ(sa->sa_family) {
#ifdef USE_IPV6
  ସ୍ଥିତି AF_INET6:
    result = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    result = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ଭାଙ୍ଗ;
  }
  ଯଦି(result)
    ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ସ୍ଥିର CURLcode ftp_port_resolve_host(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn,
                                      ଅଚଳ ଅକ୍ଷର *host,
                                      ସଂରଚନା Curl_dns_entry **dns_entryp,
                                      ଅଚଳ ସଂରଚନା Curl_addrinfo **resp)
{
  CURLcode result;

  *resp = ଖାଲି;
  result = Curl_resolv_blocking(
    data, Curl_resolv_dns_queries(data, conn->ip_version),
    host, 0, Curl_conn_get_transport(data, conn), dns_entryp);
  ଯଦି(result)
    failf(data, "failed to resolve the address provided to PORT: %s", host);
  ଅନ୍ୟଥା {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ସ୍ଥିର CURLcode ftp_port_open_socket(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn,
                                     ଅଚଳ ସଂରଚନା Curl_addrinfo *res,
                                     ଅଚଳ ସଂରଚନା Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  ଅକ୍ଷର buffer[STRERROR_LEN];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = 0;
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai;
  CURLcode result = CURLE_FTP_PORT_FAILED;

  ପାଇଁ(ai = res; ai; ai = ai->ai_next) {
    result =
      Curl_socket_open(data, ai, ଖାଲି,
                       Curl_conn_get_transport(data, conn), portsockp);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        ଫେରନ୍ତୁ result;
      result = CURLE_FTP_PORT_FAILED;
      sockerr = SOCKERRNO;
      ଜାରି;
    }
    ଭାଙ୍ଗ;
  }
  ଯଦି(!ai) {
    failf(data, "socket failure: %s",
          curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
  }
  *aip = ai;
  ଫେରନ୍ତୁ result;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ସ୍ଥିର CURLcode ftp_port_bind_socket(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn,
                                     curl_socket_t portsock,
                                     ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
                                     ସଂରଚନା Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     unsigned short port_min,
                                     unsigned short port_max,
                                     bool non_local)
{
  ସଂରଚନା sockaddr *sa = (ସଂରଚନା sockaddr *)ss;
  ସଂରଚନା sockaddr_in * ଅଚଳ sa4 = (ଶୂନ୍ୟ *)sa;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 * ଅଚଳ sa6 = (ଶୂନ୍ୟ *)sa;
#endif
  ଅକ୍ଷର buffer[STRERROR_LEN];
  unsigned short port;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr;

  memcpy(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  ପାଇଁ(port = port_min; port <= port_max;) {
    ଯଦି(sa->sa_family == AF_INET)
      sa4->sin_port = htons(port);
#ifdef USE_IPV6
    ଅନ୍ୟଥା
      sa6->sin6_port = htons(port);
#endif
    ଯଦି(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ଯଦି(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(data, "bind(port=%hu) on non-local address failed: %s", port,
              curlx_strerror(sockerr, buffer, ଆକାର(buffer)));

        *sslen_io = ଆକାର(*ss);
        ଯଦି(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(data, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
          ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
        }
        port = port_min;
        non_local = FALSE; /* do not try this again */
        ଜାରି;
      }
      ଯଦି(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(data, "bind(port=%hu) failed: %s", port,
              curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
        ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
      }
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ଯଦି(port == port_max) {
      failf(data, "bind() failed, ran out of ports");
      ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
    }
    port++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = ଆକାର(*ss);
  ଯଦି(getsockname(portsock, sa, sslen_io)) {
    failf(data, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_bind_socket(), socket bound to port %d", port);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ସ୍ଥିର CURLcode ftp_port_listen(ସଂରଚନା Curl_easy *data, curl_socket_t portsock)
{
  ଅକ୍ଷର buffer[STRERROR_LEN];

  ଯଦି(listen(portsock, 1)) {
    failf(data, "socket failure: %s",
          curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ CURLE_FTP_PORT_FAILED;
  }
  CURL_TRC_FTP(data, "ftp_port_listen(), listening on port");
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ସ୍ଥିର CURLcode ftp_port_send_command(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ftp_conn *ftpc,
                                      ସଂରଚନା connectdata *conn,
                                      ସଂରଚନା Curl_sockaddr_storage *ss,
                                      ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର mode[][5] = { "EPRT", "PORT" };
  ସଂରଚନା sockaddr *sa = (ସଂରଚନା sockaddr *)ss;
  ସଂରଚନା sockaddr_in * ଅଚଳ sa4 = (ଶୂନ୍ୟ *)sa;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 * ଅଚଳ sa6 = (ଶୂନ୍ୟ *)sa;
#endif
  ଅକ୍ଷର myhost[MAX_IPADR_LEN + 1] = "";
  unsigned short port;
  CURLcode result;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_address(ai, myhost, ଆକାର(myhost));

#ifdef USE_IPV6
  ଯଦି(!conn->bits.ftp_use_eprt && conn->bits.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->bits.ftp_use_eprt = TRUE;
#endif

  ପାଇଁ(; fcmd != DONE; fcmd++) {

    ଯଦି(!conn->bits.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ଜାରି;

    ଯଦି((PORT == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ଜାରି;

    ବାଛ(sa->sa_family) {
    ସ୍ଥିତି AF_INET:
      port = ntohs(sa4->sin_port);
      ଭାଙ୍ଗ;
#ifdef USE_IPV6
    ସ୍ଥିତି AF_INET6:
      port = ntohs(sa6->sin6_port);
      ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଜାରି; /* might as well skip this */
    }

    ଯଦି(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s |%d|%s|%hu|", mode[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, port);
      ଯଦି(result) {
        failf(data, "Failure sending EPRT command: %s",
              curl_easy_strerror(result));
        ଫେରନ୍ତୁ result;
      }
      ଭାଙ୍ଗ;
    }
    ଯଦି(PORT == fcmd) {
      /* large enough for [IP address],[num],[num] */
      ଅକ୍ଷର target[ଆକାର(myhost) + 20];
      ଅଚଳ ଅକ୍ଷର *source = myhost;
      ଅକ୍ଷର *dest = target;

      /* translate x.x.x.x to x,x,x,x */
      ଯେତେବେଳେ(*source) {
        ଯଦି(*source == '.')
          *dest = ',';
        ଅନ୍ୟଥା
          *dest = *source;
        dest++;
        source++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(port >> 8), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(port & 0xff));

      result = Curl_pp_sendf(data, &ftpc->pp, "%s %s", mode[fcmd], target);
      ଯଦି(result) {
        failf(data, "Failure sending PORT command: %s",
              curl_easy_strerror(result));
        ଫେରନ୍ତୁ result;
      }
      ଭାଙ୍ଗ;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(data, ftpc, FTP_PORT);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ସ୍ଥିର CURLcode ftp_state_use_port(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode result = CURLE_FTP_PORT_FAILED;
  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ସଂରଚନା Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  ଅକ୍ଷର hbuf[NI_MAXHOST];
  ଅଚଳ ଅକ୍ଷର *host = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *string_ftpport = CURL_EASY_STR(data, STRING_FTPPORT);
  ସଂରଚନା Curl_dns_entry *dns_entry = ଖାଲି;
  ଅଚଳ ସଂରଚନା Curl_addrinfo *res = ଖାଲି;
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai = ଖାଲି;
  unsigned short port_min = 0;
  unsigned short port_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  result = ftp_port_parse_string(data, conn, string_ftpport,
                                 &ss, &port_min, &port_max,
                                 &host, hbuf, ଆକାର(hbuf));
  ଯଦି(!result && !host)
    /* if no host was specified, use the control connection's local IP */
    result = ftp_port_default_host(data, conn, &ss, &sslen, &host,
                                   hbuf, ଆକାର(hbuf), &non_local);

  /* resolve host string to address list */
  ଯଦି(!result)
    result = ftp_port_resolve_host(data, conn, host, &dns_entry, &res);

  /* Open a TCP socket for the data connection */
  ଯଦି(!result)
    result = ftp_port_open_socket(data, conn, res, &ai, &portsock);
  ଯଦି(!result) {
    CURL_TRC_FTP(data, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    result = ftp_port_bind_socket(data, conn, portsock, ai, &ss, &sslen,
                                  port_min, port_max, non_local);
  }

  /* listen */
  ଯଦି(!result)
    result = ftp_port_listen(data, portsock);

  /* send the PORT / EPRT command */
  ଯଦି(!result)
    result = ftp_port_send_command(data, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ଯଦି(!result)
    result = Curl_conn_tcp_listen_set(data, conn, SECONDARYSOCKET, &portsock);

  ଯଦି(!result)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ଯଦି(dns_entry)
    Curl_dns_entry_unlink(data, &dns_entry);
  ଯଦି(result) {
    ftp_state(data, ftpc, FTP_STOP);
  }
  ଅନ୍ୟଥା {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ଯଦି(conn->bits.ftp_use_data_ssl && data->set.ftp_use_port &&
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
  ଯଦି(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(data, conn, portsock);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_use_pasv(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc,
                                   ସଂରଚନା connectdata *conn)
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

  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର mode[][5] = { "EPSV", "PASV" };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା modeoff;

#ifdef PF_INET6
  ଯଦି(!conn->bits.ftp_use_epsv && conn->bits.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->bits.ftp_use_epsv = TRUE;
#endif

  modeoff = conn->bits.ftp_use_epsv ? 0 : 1;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", mode[modeoff]);
  ଯଦି(!result) {
    ftpc->count1 = modeoff;
    ftp_state(data, ftpc, FTP_PASV);
    infof(data, "Connect data stream passively");
  }
  ଫେରନ୍ତୁ result;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ସ୍ଥିର CURLcode ftp_state_prepare_transfer(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ftp_conn *ftpc,
                                           ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;

  ଯଦି(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(data, ftpc, FTP_RETR_PREQUOTE);
    result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ଅନ୍ୟଥା ଯଦି(data->set.ftp_use_port) {
    /* We have chosen to use the PORT (or similar) command */
    result = ftp_state_use_port(data, ftpc, EPRT);
  }
  ଅନ୍ୟଥା {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ଯଦି(data->set.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ଯଦି(!ftpc->file)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                               (data->state.list_only ? "NLST" : "LIST"));
      ଅନ୍ୟଥା ଯଦି(data->state.upload)
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ଅନ୍ୟଥା
        result = Curl_pp_sendf(data, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_PRET);
    }
    ଅନ୍ୟଥା
      result = ftp_state_use_pasv(data, ftpc, conn);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_rest(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ଯଦି((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    result = Curl_pp_sendf(data, &ftpc->pp, "REST %d", 0);
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_REST);
  }
  ଅନ୍ୟଥା
    result = ftp_state_prepare_transfer(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_size(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;

  ଯଦି((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_SIZE);
  }
  ଅନ୍ୟଥା
    result = ftp_state_rest(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_list(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp)
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
  ଅଚଳ ଅକ୍ଷର *lstArg = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା lstArglen = 0;
  ଅକ୍ଷର *cmd;

  ଯଦି((data->set.ftp_filemethod == FTPFILE_NOCWD) && ftp->path) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ଅଚଳ ଅକ୍ଷର *rawPath = ftpc->rawpath;
    ଅଚଳ ଅକ୍ଷର *slashPos = strrchr(rawPath, '/');
    ଯଦି(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - rawPath;
      ଯଦି(n == 0)
        ++n;

      lstArg = rawPath;
      lstArglen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(data, STRING_CUSTOMREQUEST) :
                      (data->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ଯଦି(!cmd)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ଯଦି(!result)
    ftp_state(data, ftpc, FTP_LIST);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_list_prequote(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ftp_conn *ftpc,
                                        ସଂରଚନା FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ଫେରନ୍ତୁ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ସ୍ଥିର CURLcode ftp_state_retr_prequote(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ftp_conn *ftpc,
                                        ସଂରଚନା FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ଫେରନ୍ତୁ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ସ୍ଥିର CURLcode ftp_state_stor_prequote(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ftp_conn *ftpc,
                                        ସଂରଚନା FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ଫେରନ୍ତୁ ftp_state_quote(data, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ସ୍ଥିର CURLcode ftp_state_type(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ଯଦି(data->req.no_body && ftpc->file &&
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
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
    result = ftp_state_size(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ସ୍ଥିର CURLcode ftp_state_mdtm(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp)
{
  CURLcode result = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ଯଦି((data->set.get_filetime || data->set.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    result = Curl_pp_sendf(data, &ftpc->pp, "MDTM %s", ftpc->file);

    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_MDTM);
  }
  ଅନ୍ୟଥା
    result = ftp_state_type(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

/* This is called after the TYPE and possible quote commands have been sent */
ସ୍ଥିର CURLcode ftp_state_ul_setup(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc,
                                   ସଂରଚନା FTP *ftp,
                                   bool sizechecked)
{
  CURLcode result = CURLE_OK;
  curl_bit append = data->set.remote_append;

  ଯଦି((data->state.resume_from && !sizechecked) ||
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା seekerr = CURL_SEEKFUNC_OK;

    ଯଦି(data->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_STOR_SIZE);
      ଫେରନ୍ତୁ result;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ଯଦି(data->set.seek_func) {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ଯଦି(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ଯଦି(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ଫେରନ୍ତୁ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      କର {
        ଅକ୍ଷର scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed > (curl_off_t)ଆକାର(scratch)) ?
          ଆକାର(scratch) :
          curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1, readthisamountnow,
                                 data->state.in);

        passed += actuallyread;
        ଯଦି((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ଫେରନ୍ତୁ CURLE_FTP_COULDNT_USE_REST;
        }
      } ଯେତେବେଳେ(passed < data->state.resume_from);
    }
    /* now, decrease the size of the read */
    ଯଦି(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;

      ଯଦି(data->state.infilesize <= 0) {
        infof(data, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_setup_nop(data);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(data, ftpc, FTP_STOP);
        ଫେରନ୍ତୁ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  result = Curl_pp_sendf(data, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ଯଦି(!result)
    ftp_state(data, ftpc, FTP_STOR);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_retr(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ftp_conn *ftpc,
                               ସଂରଚନା FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ଯଦି(data->set.max_filesize && (filesize > data->set.max_filesize)) {
    failf(data, "Maximum file size exceeded");
    ଫେରନ୍ତୁ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ଯଦି(data->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ଯଦି(filesize == -1) {
      infof(data, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ଅନ୍ୟଥା {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ଯଦି(data->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ଯଦି(filesize < -data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -data->state.resume_from;
        /* download from where? */
        data->state.resume_from = filesize - ftp->downloadsize;
      }
      ଅନ୍ୟଥା {
        ଯଦି(filesize < data->state.resume_from) {
          failf(data, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                data->state.resume_from, filesize);
          ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - data->state.resume_from;
      }
    }

    ଯଦି(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_setup_nop(data);
      infof(data, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(data, ftpc, FTP_STOP);
      ଫେରନ୍ତୁ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(data, "Instructs server to resume from offset %" FMT_OFF_T,
          data->state.resume_from);

    result = Curl_pp_sendf(data, &ftpc->pp, "REST %" FMT_OFF_T,
                           data->state.resume_from);
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_RETR_REST);
  }
  ଅନ୍ୟଥା {
    /* no resume */
    result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_RETR);
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_quote(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ftp_conn *ftpc,
                                ସଂରଚନା FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode result = CURLE_OK;
  bool quote = FALSE;
  ସଂରଚନା curl_slist *item;

  ବାଛ(instate) {
  ସ୍ଥିତି FTP_QUOTE:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    item = data->set.quote;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି FTP_RETR_PREQUOTE:
  ସ୍ଥିତି FTP_STOR_PREQUOTE:
  ସ୍ଥିତି FTP_LIST_PREQUOTE:
    item = data->set.prequote;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି FTP_POSTQUOTE:
    item = data->set.postquote;
    ଭାଙ୍ଗ;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ଯଦି(init)
    ftpc->count1 = 0;
  ଅନ୍ୟଥା
    ftpc->count1++;

  ଯଦି(item) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

    /* Skip count1 items in the linked list */
    ଯେତେବେଳେ((i < ftpc->count1) && item) {
      item = item->next;
      i++;
    }
    ଯଦି(item) {
      ଅଚଳ ଅକ୍ଷର *cmd = item->data;
      ଯଦି(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ଅନ୍ୟଥା
        ftpc->count2 = 0; /* failure means cancel operation */

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      ftp_state(data, ftpc, instate);
      quote = TRUE;
    }
  }

  ଯଦି(!quote) {
    /* No more quote to send, continue to ... */
    ବାଛ(instate) {
    ସ୍ଥିତି FTP_QUOTE:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = ftp_state_cwd(data, ftpc, ftp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି FTP_RETR_PREQUOTE:
      ଯଦି(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(data, ftpc, FTP_STOP);
      ଅନ୍ୟଥା {
        ଯଦି(ftpc->known_filesize != -1) {
          Curl_pgrsSetDownloadSize(data, ftpc->known_filesize);
          result = ftp_state_retr(data, ftpc, ftp, ftpc->known_filesize);
        }
        ଅନ୍ୟଥା {
          ଯଦି(data->set.ignorecl || data->state.prefer_ascii) {
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
            ଯଦି(!result)
              ftp_state(data, ftpc, FTP_RETR);
          }
          ଅନ୍ୟଥା {
            result = Curl_pp_sendf(data, &ftpc->pp, "SIZE %s", ftpc->file);
            ଯଦି(!result)
              ftp_state(data, ftpc, FTP_RETR_SIZE);
          }
        }
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି FTP_STOR_PREQUOTE:
      result = ftp_state_ul_setup(data, ftpc, ftp, FALSE);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି FTP_POSTQUOTE:
      ଭାଙ୍ଗ;
    ସ୍ଥିତି FTP_LIST_PREQUOTE:
      ftp_state(data, ftpc, FTP_LIST_TYPE);
      result = ftp_state_list(data, ftpc, ftp);
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ result;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ସ୍ଥିର CURLcode ftp_epsv_disable(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ftp_conn *ftpc,
                                 ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;

  ଯଦି(conn->bits.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(data, "Failed EPSV attempt, exiting");
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  infof(data, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->bits.ftp_use_epsv = FALSE;
  close_secondarysocket(data, ftpc);
  data->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  result = Curl_pp_sendf(data, &ftpc->pp, "%s", "PASV");
  ଯଦି(!result) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(data, ftpc, FTP_PASV);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_control_addr_dup(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର **newhostp)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ifndef CURL_DISABLE_PROXY
  ଯଦି(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ଅନ୍ୟଥା
#endif
  ଯଦି(!Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ଅନ୍ୟଥା {
    /* failed to get the remote_ip of the DATA connection */
    failf(data, "unable to get peername of DATA connection");
    *newhostp = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_FTP_CANT_GET_HOST;
  }
  ଫେରନ୍ତୁ *newhostp ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ସ୍ଥିର bool match_pasv_6nums(ଅଚଳ ଅକ୍ଷର *p,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *array) /* 6 numbers */
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; i < 6; i++) {
    curl_off_t ଅଙ୍କ;
    ଯଦି(i) {
      ଯଦି(*p != ',')
        ଫେରନ୍ତୁ FALSE;
      p++;
    }
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, 0xff))
      ଫେରନ୍ତୁ FALSE;
    array[i] = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode ftp_state_pasv_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result;
  ଅଚଳ ସଂରଚନା pingpong *pp = &ftpc->pp;
  ଅକ୍ଷର *newhost = ଖାଲି;
  unsigned short newport = 0;
  ଅଚଳ ଅକ୍ଷର *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ଯଦି((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ଅଚଳ ଅକ୍ଷର *ptr = strchr(str, '(');
    ଯଦି(ptr) {
      ଅକ୍ଷର sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ଯଦି(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ଅଚଳ ଅକ୍ଷର *p = &ptr[3];
        curl_off_t ଅଙ୍କ;
        ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, 0xffff) || (*p != sep)) {
          failf(data, "Illegal port number in EPSV reply");
          ଫେରନ୍ତୁ CURLE_FTP_WEIRD_PASV_REPLY;
        }
        newport = (unsigned short)ଅଙ୍କ;
        result = ftp_control_addr_dup(data, &newhost);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା
        ptr = ଖାଲି;
    }
    ଯଦି(!ptr) {
      failf(data, "Weirdly formatted EPSV reply");
      ଫେରନ୍ତୁ CURLE_FTP_WEIRD_PASV_REPLY;
    }
  }
  ଅନ୍ୟଥା ଯଦି((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ଯେତେବେଳେ(*str) {
      ଯଦି(match_pasv_6nums(str, ip))
        ଭାଙ୍ଗ;
      str++;
    }

    ଯଦି(!*str) {
      failf(data, "Could not interpret the 227-response");
      ଫେରନ୍ତୁ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ଯଦି(data->set.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(data, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      result = ftp_control_addr_dup(data, &newhost);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ଯଦି(!newhost)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    newport = (unsigned short)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ଅନ୍ୟଥା ଯଦି(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ଫେରନ୍ତୁ ftp_epsv_disable(data, ftpc, conn);
  }
  ଅନ୍ୟଥା {
    failf(data, "Bad PASV/EPSV response: %03d", ftpcode);
    ଫେରନ୍ତୁ CURLE_FTP_WEIRD_PASV_REPLY;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  result = Curl_peer_create(data, conn->scheme, newhost, newport,
                            &conn->origin2);
  ଯଦି(result)
    ଯାଅ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ଯଦି(conn->via_peer) {
    Curl_peer_unlink(&conn->via_peer2);
    result = Curl_peer_create(data, conn->via_peer->scheme,
                              conn->via_peer->hostname, newport,
                              &conn->via_peer2);
    ଯଦି(result)
      ଯାଅ error;
  }

  result = Curl_conn_setup(data, conn, SECONDARYSOCKET,
                           conn->bits.ftp_use_data_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ଯଦି(result) {
    ଯଦି((result != CURLE_OUT_OF_MEMORY) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      result = ftp_epsv_disable(data, ftpc, conn);
    }
    ଯାଅ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ifdef CURLVERBOSE
  ଯଦି(data->set.verbose) {
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
  ଫେରନ୍ତୁ result;
}

/* called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode ftp_statemach(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ftp_conn *ftpc,
                              bool *done)
{
  CURLcode result = Curl_pp_statemach(data, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *done = (ftpc->state == FTP_STOP);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_do_more(ସଂରଚନା Curl_easy *data, domore *more)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ସଂରଚନା FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ଯଦି(!ftpc || !ftp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  *more = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ଯଦି(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(data, SECONDARYSOCKET);
    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ଯଦି(result == CURLE_OUT_OF_MEMORY)
      ଫେରନ୍ତୁ result;
    ଯଦି(result || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(data, SECONDARYSOCKET))) {
      ଯଦି(result && !is_eptr && (ftpc->count1 == 0)) {
        *more = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ଫେରନ୍ତୁ ftp_epsv_disable(data, ftpc, conn);
      }
      ଫେରନ୍ତୁ result;
    }
  }

  ଯଦି(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    result = ftp_statemach(data, ftpc, &complete);

    ଯଦି(complete)
      *more = DOMORE_DONE;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ଯଦି(result || !ftpc->wait_data_conn)
      ଫେରନ୍ତୁ result;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *more = DOMORE_INCOMPLETE;
  }

  ଯଦି(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ଯଦି(ftpc->wait_data_conn) {
      bool serv_conned;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &serv_conned);
      ଯଦି(result)
        ଫେରନ୍ତୁ result; /* Failed to accept data connection */

      ଯଦି(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_data_conn = FALSE;
        result = ftp_initiate_transfer(data, ftpc);

        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        *more = DOMORE_DONE; /* this state is now complete when the server has
                                connected back to us */
      }
      ଅନ୍ୟଥା {
        result = ftp_check_ctrl_on_data_wait(data, ftpc);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
    ଅନ୍ୟଥା ଯଦି(data->state.upload) {
      result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      result = ftp_statemach(data, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *more = (!ftpc->wait_data_conn && complete) ?
        DOMORE_DONE : DOMORE_INCOMPLETE;
    }
    ଅନ୍ୟଥା {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      result = Curl_range(data);

      ଯଦି(result == CURLE_OK && data->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      ଯଦି((data->state.list_only || !ftpc->file) &&
         !data->set.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ଯଦି(ftp->transfer == PPTRANSFER_BODY) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
        }
        /* otherwise fall through */
      }
      ଅନ୍ୟଥା {
        ଯଦି(data->set.prequote && !ftpc->file) {
          result = ftp_nb_type(data, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ଅନ୍ୟଥା {
          result = ftp_nb_type(data, ftpc, ftp, (bool)data->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }

      result = ftp_statemach(data, ftpc, &complete);
      ଯଦି(complete)
        *more = DOMORE_DONE;
    }
    ଫେରନ୍ତୁ result;
  }

  /* no data to transfer */
  Curl_xfer_setup_nop(data);

  ଯଦି(!ftpc->wait_data_conn) {
    /* no waiting for the data connection so this is now complete */
    *more = DOMORE_DONE;
    CURL_TRC_FTP(data, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }

  ଫେରନ୍ତୁ result;
}

/* call this when the DO phase has completed */
ସ୍ଥିର CURLcode ftp_dophase_done(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ftp_conn *ftpc,
                                 ସଂରଚନା FTP *ftp,
                                 bool connected)
{
  ଯଦି(connected) {
    domore completed;
    CURLcode result = ftp_do_more(data, &completed);

    ଯଦି(result) {
      close_secondarysocket(data, ftpc);
      ଫେରନ୍ତୁ result;
    }
  }

  ଯଦି(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
  ଅନ୍ୟଥା ଯଦି(!connected)
    /* since we did not connect now, we want do_more to get called */
    data->conn->bits.do_more = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ftp_state_port_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ସଂରଚନା FTP *ftp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode result = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ଯଦି(ftpcode / 100 != 2) {
    /* the command failed */

    ଯଦି(EPRT == fcmd) {
      infof(data, "disabling EPRT usage");
      conn->bits.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ଯଦି(fcmd == DONE) {
      failf(data, "Failed to do PORT");
      result = CURLE_FTP_PORT_FAILED;
    }
    ଅନ୍ୟଥା
      /* try next */
      result = ftp_state_use_port(data, ftpc, fcmd);
  }
  ଅନ୍ୟଥା {
    infof(data, "Connect data stream actively");
    ftp_state(data, ftpc, FTP_STOP); /* end of DO phase */
    result = ftp_dophase_done(data, ftpc, ftp, FALSE);
  }

  ଫେରନ୍ତୁ result;
}

/* return TRUE on error, FALSE on success */
ସ୍ଥିର bool twodigit(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
  ଯଦି(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ଫେରନ୍ତୁ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ଫେରନ୍ତୁ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *year, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *month, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *day,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା *hour, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minute, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *second);
UNITTEST bool ftp_213_date(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *year, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *month, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *day,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା *hour, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minute, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *second)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା century;
  ଯଦି((strlen(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], second))
    ଫେରନ୍ତୁ FALSE;

  *year += century * 100;
  ଯଦି((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*second > 60))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode client_write_header(ସଂରଚନା Curl_easy *data,
                                    ଅକ୍ଷର *buf, size_t blen)
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
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_mdtm_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ସଂରଚନା FTP *ftp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  CURLcode result = CURLE_OK;

  ବାଛ(ftpcode) {
  ସ୍ଥିତି 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା year, month, day, hour, minute, second;
    ସଂରଚନା pingpong *pp = &ftpc->pp;
    ଅଚଳ ଅକ୍ଷର *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ଯଦି(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &second)) {
      /* we have a time, reformat it */
      ଅକ୍ଷର timebuf[24];
      curl_msnprintf(timebuf, ଆକାର(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, second);
      /* now, convert this into a time() value: */
      ଯଦି(!Curl_getdate_capped(timebuf, &data->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ଯଦି defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic ଠେଲ
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    ଯଦି(data->req.no_body && ftpc->file &&
       data->set.get_filetime && showtime) {
#ଯଦି defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#pragma GCC diagnostic କାଢ଼
#endif
      ଅକ୍ଷର headerbuf[128];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା headerbuflen;
      time_t filetime = data->info.filetime;
      ସଂରଚନା tm buffer;
      ଅଚଳ ସଂରଚନା tm *tm = &buffer;

      result = curlx_gmtime(filetime, &buffer);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, ଆକାର(headerbuf),
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
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    } /* end of a ridiculous amount of conditionals */
  }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    infof(data, "unsupported MDTM reply format");
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(data, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ଭାଙ୍ଗ;
  }

  ଯଦି(data->set.timecondition) {
    ଯଦି((data->info.filetime > 0) && (data->set.timevalue > 0)) {
      ବାଛ(data->set.timecondition) {
      ସ୍ଥିତି CURL_TIMECOND_IFMODSINCE:
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଯଦି(data->info.filetime <= data->set.timevalue) {
          infof(data, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_TIMECOND_IFUNMODSINCE:
        ଯଦି(data->info.filetime > data->set.timevalue) {
          infof(data, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          data->info.timecond = TRUE;
          ftp_state(data, ftpc, FTP_STOP);
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଭାଙ୍ଗ;
      } /* switch */
    }
    ଅନ୍ୟଥା {
      infof(data, "Skipping time comparison");
    }
  }

  ଯଦି(!result)
    result = ftp_state_type(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_type_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ସଂରଚନା FTP *ftp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ଯଦି(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(data, "Could not set desired mode");
    ଫେରନ୍ତୁ CURLE_FTP_COULDNT_SET_TYPE;
  }
  ଯଦି(ftpcode != 200)
    infof(data, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ଯଦି(instate == FTP_TYPE)
    result = ftp_state_size(data, ftpc, ftp);
  ଅନ୍ୟଥା ଯଦି(instate == FTP_LIST_TYPE)
    result = ftp_state_list(data, ftpc, ftp);
  ଅନ୍ୟଥା ଯଦି(instate == FTP_RETR_TYPE)
    result = ftp_state_retr_prequote(data, ftpc, ftp);
  ଅନ୍ୟଥା ଯଦି(instate == FTP_STOR_TYPE)
    result = ftp_state_stor_prequote(data, ftpc, ftp);
  ଅନ୍ୟଥା ଯଦି(instate == FTP_RETR_LIST_TYPE)
    result = ftp_state_list_prequote(data, ftpc, ftp);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_size_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ସଂରଚନା FTP *ftp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;
  curl_off_t filesize = -1;
  ଅଚଳ ଅକ୍ଷର *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ଯଦି(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ଅଚଳ ଅକ୍ଷର *start = &buf[4];
    ଅଚଳ ଅକ୍ଷର *fdigit = memchr(start, '\r', len - 4);
    ଯଦି(fdigit) {
      fdigit--;
      ଯଦି(*fdigit == '\n')
        fdigit--;
      ଯେତେବେଳେ(ISDIGIT(fdigit[-1]) && (fdigit > start))
        fdigit--;
    }
    ଅନ୍ୟଥା
      fdigit = start;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ଅନ୍ୟଥା ଯଦି(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_SIZE) {
    failf(data, "The file does not exist");
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  ଯଦି(instate == FTP_SIZE) {
    ଯଦି(filesize != -1) {
      ଅକ୍ଷର clbuf[128];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା clbuflen = curl_msnprintf(clbuf, ଆକାର(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      result = client_write_header(data, clbuf, clbuflen);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_rest(data, ftpc, ftp);
  }
  ଅନ୍ୟଥା ଯଦି(instate == FTP_RETR_SIZE) {
    Curl_pgrsSetDownloadSize(data, filesize);
    result = ftp_state_retr(data, ftpc, ftp, filesize);
  }
  ଅନ୍ୟଥା ଯଦି(instate == FTP_STOR_SIZE) {
    data->state.resume_from = filesize;
    result = ftp_state_ul_setup(data, ftpc, ftp, TRUE);
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_rest_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ସଂରଚନା FTP *ftp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode,
                                    ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ବାଛ(instate) {
  ସ୍ଥିତି FTP_REST:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(ftpcode == 350) {
      ଅକ୍ଷର buffer[24] = { "Accept-ranges: bytes\r\n" };
      result = client_write_header(data, buffer, strlen(buffer));
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    result = ftp_state_prepare_transfer(data, ftpc, ftp);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_RETR_REST:
    ଯଦି(ftpcode != 350) {
      failf(data, "Could not use REST");
      result = CURLE_FTP_COULDNT_USE_REST;
    }
    ଅନ୍ୟଥା {
      result = Curl_pp_sendf(data, &ftpc->pp, "RETR %s", ftpc->file);
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_RETR);
    }
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_state_stor_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  CURLcode result = CURLE_OK;

  ଯଦି(ftpcode >= 400) {
    failf(data, "Failed FTP upload: %0d", ftpcode);
    ftp_state(data, ftpc, FTP_STOP);
    ଫେରନ୍ତୁ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ଯଦି(data->set.ftp_use_port) {
    bool connected;

    ftp_state(data, ftpc, FTP_STOP); /* no longer in STOR state */

    result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯଦି(!connected) {
      infof(data, "Data conn was not available immediately");
      ftpc->wait_data_conn = TRUE;
      ଫେରନ୍ତୁ ftp_check_ctrl_on_data_wait(data, ftpc);
    }
    ftpc->wait_data_conn = FALSE;
  }
  ଫେରନ୍ତୁ ftp_initiate_transfer(data, ftpc);
}

/* for LIST and RETR responses */
ସ୍ଥିର CURLcode ftp_state_get_resp(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc,
                                   ସଂରଚନା FTP *ftp,
                                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode,
                                   ftpstate instate)
{
  CURLcode result = CURLE_OK;

  ଯଦି((ftpcode == 150) || (ftpcode == 125)) {

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

    ଯଦି((instate != FTP_LIST) &&
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
      ଯଦି(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        ପାଇଁ(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ଅଚଳ ଅକ୍ଷର *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ଅଚଳ ଅକ୍ଷର *c = &buf[i];
          ଯଦି(!curlx_str_ସଂଖ୍ୟା(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !strncmp(c, "bytes", 5)) {
            data->req.size = what;
            ଭାଙ୍ଗ;
          }
        }
      }
    }
    ଅନ୍ୟଥା ଯଦି(ftp->downloadsize > -1)
      data->req.size = ftp->downloadsize;

    ଯଦି(data->req.size > data->req.maxdownload && data->req.maxdownload > 0)
      data->req.size = data->req.maxdownload;
    ଅନ୍ୟଥା ଯଦି((instate != FTP_LIST) && data->state.prefer_ascii)
      data->req.size = -1; /* for servers that understate ASCII mode file
                              size */

    infof(data, "Maxdownload = %" FMT_OFF_T, data->req.maxdownload);

    ଯଦି(instate != FTP_LIST)
      infof(data, "Getting file with size: %" FMT_OFF_T, data->req.size);

    ଯଦି(data->set.ftp_use_port) {
      bool connected;

      result = Curl_conn_connect(data, SECONDARYSOCKET, FALSE, &connected);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      ଯଦି(!connected) {
        infof(data, "Data conn was not available immediately");
        ftp_state(data, ftpc, FTP_STOP);
        ftpc->wait_data_conn = TRUE;
        ଫେରନ୍ତୁ ftp_check_ctrl_on_data_wait(data, ftpc);
      }
      ftpc->wait_data_conn = FALSE;
    }
    ଫେରନ୍ତୁ ftp_initiate_transfer(data, ftpc);
  }
  ଅନ୍ୟଥା {
    ଯଦି((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(data, ftpc, FTP_STOP); /* this phase is over */
    }
    ଅନ୍ୟଥା {
      failf(data, "RETR response: %03d", ftpcode);
      ଫେରନ୍ତୁ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_FOUND :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ଫେରନ୍ତୁ result;
}

/* after USER, PASS and ACCT */
ସ୍ଥିର CURLcode ftp_state_loggedin(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ଯଦି(Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
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
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_PBSZ);
  }
  ଅନ୍ୟଥା {
    result = ftp_state_pwd(data, ftpc);
  }
  ଫେରନ୍ତୁ result;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ସ୍ଥିର bool ftp_has_ctrl(ଅଚଳ ଅକ୍ଷର *string)
{
  ଅଚଳ unsigned ଅକ୍ଷର *s = (ଅଚଳ unsigned ଅକ୍ଷର *)string;
  ଯେତେବେଳେ(*s) {
    ଯଦି(*s < 0x20)
      ଫେରନ୍ତୁ TRUE;
    s++;
  }
  ଫେରନ୍ତୁ FALSE;
}

/* for USER and PASS responses */
ସ୍ଥିର CURLcode ftp_state_user_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  CURLcode result = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ଯଦି((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    result = Curl_pp_sendf(data, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(data->conn->creds));
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_PASS);
  }
  ଅନ୍ୟଥା ଯଦି(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    result = ftp_state_loggedin(data, ftpc);
  }
  ଅନ୍ୟଥା ଯଦି(ftpcode == 332) {
    ଅଚଳ ଅକ୍ଷର *account = CURL_EASY_STR(data, STRING_FTP_ACCOUNT);
    ଯଦି(!account) {
      failf(data, "ACCT requested but none available");
      result = CURLE_LOGIN_DENIED;
    }
    ଅନ୍ୟଥା ଯଦି(ftp_has_ctrl(account)) {
      failf(data, "Control byte in FTP account");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ଅନ୍ୟଥା {
      result = Curl_pp_sendf(data, &ftpc->pp, "ACCT %s", account);
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_ACCT);
    }
  }
  ଅନ୍ୟଥା {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ଅଚଳ ଅକ୍ଷର *alt = CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER);
    ଯଦି(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ଯଦି(ftp_has_ctrl(alt)) {
        failf(data, "Control byte in FTP alternative-to-user command");
        result = CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ଅନ୍ୟଥା {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", alt);
        ଯଦି(!result) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(data, ftpc, FTP_USER);
        }
      }
    }
    ଅନ୍ୟଥା {
      failf(data, "Access denied: %03d", ftpcode);
      result = CURLE_LOGIN_DENIED;
    }
  }
  ଫେରନ୍ତୁ result;
}

/* for ACCT response */
ସ୍ଥିର CURLcode ftp_state_acct_resp(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  CURLcode result = CURLE_OK;
  ଯଦି(ftpcode != 230) {
    failf(data, "ACCT rejected by server: %03d", ftpcode);
    result = CURLE_FTP_WEIRD_PASS_REPLY; /* FIX */
  }
  ଅନ୍ୟଥା
    result = ftp_state_loggedin(data, ftpc);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_pwd_resp(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ftp_conn *ftpc,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  ସଂରଚନା pingpong *pp = &ftpc->pp;
  CURLcode result;

  ଯଦି(ftpcode == 257) {
    ଅଚଳ ଅକ୍ଷର *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool entry_extracted = FALSE;
    ସଂରଚନା dynbuf out;
    curlx_dyn_init(&out, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ଯେତେବେଳେ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ଯଦି('\"' == *ptr) {
      /* it started good */
      ପାଇଁ(ptr++; *ptr; ptr++) {
        ଯଦି('\"' == *ptr) {
          ଯଦି('\"' == ptr[1]) {
            /* "quote-doubling" */
            result = curlx_dyn_addn(&out, &ptr[1], 1);
            ptr++;
          }
          ଅନ୍ୟଥା {
            /* end of path */
            ଯଦି(curlx_dyn_len(&out))
              entry_extracted = TRUE;
            ଭାଙ୍ଗ; /* get out of this loop */
          }
        }
        ଅନ୍ୟଥା {
          ଯଦି(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&out);
            ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
          }
          result = curlx_dyn_addn(&out, ptr, 1);
        }
        ଯଦି(result) {
          curlx_dyn_free(&out);
          ଫେରନ୍ତୁ result;
        }
      }
    }
    ଯଦି(entry_extracted) {
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
      ଅକ୍ଷର *dir = curlx_dyn_ptr(&out);

      ଯଦି(!ftpc->server_os && dir[0] != '/') {
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SYST");
        ଯଦି(result) {
          curlx_dyn_free(&out);
          ଫେରନ୍ତୁ result;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(data, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(data->state.most_recent_ftp_entrypath);
      data->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ଯଦି(!data->state.most_recent_ftp_entrypath)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      ଯଦି(!ftpc->server_os && dir[0] != '/') {
        ftp_state(data, ftpc, FTP_SYST);
        ଫେରନ୍ତୁ CURLE_OK;
      }
    }
    ଅନ୍ୟଥା {
      /* could not get the path */
      curlx_dyn_free(&out);
      infof(data, "Failed to figure out path");
    }
  }
  ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ ftpauth[] = { "SSL", "TLS" };

ସ୍ଥିର CURLcode ftp_wait_resp(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn,
                              ସଂରଚନା ftp_conn *ftpc,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode)
{
  CURLcode result = CURLE_OK;
  ଯଦି(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ଯଦି(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ଫେରନ୍ତୁ ftp_state_user_resp(data, ftpc, ftpcode);
  }
  ଅନ୍ୟଥା ଯଦି(ftpcode != 220) {
    failf(data, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  ଯଦି(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ବାଛ((long)data->set.ftpsslauth) {
    ସ୍ଥିତି CURLFTPAUTH_DEFAULT:
    ସ୍ଥିତି CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->set.ftpsslauth);
      ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_AUTH);
  }
  ଅନ୍ୟଥା
    result = ftp_state_user(data, ftpc, conn);
  ଫେରନ୍ତୁ result;
}

/* an implicit TLS scheme implies TLS on both connections */
ସ୍ଥିର unsigned ଅକ୍ଷର ftp_use_ssl(ସଂରଚନା connectdata *conn,
                                 ସଂରଚନା ftp_conn *ftpc)
{
  ଯଦି(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ଫେରନ୍ତୁ CURLUSESSL_ALL;
  ଫେରନ୍ତୁ ftpc->use_ssl;
}

ସ୍ଥିର CURLcode ftp_pp_statemachine(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ସଂରଚନା FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ସଂରଚନା pingpong *pp;
  size_t nread = 0;

  ଯଦି(!ftpc || !ftp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ଯଦି(pp->sendleft)
    ଫେରନ୍ତୁ Curl_pp_flushsend(data, pp);

  result = ftp_readresp(data, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ଯଦି(result || !ftpcode)
    ଫେରନ୍ତୁ result;

  /* we have now received a full FTP server response */
  ବାଛ(ftpc->state) {
  ସ୍ଥିତି FTP_WAIT220:
    result = ftp_wait_resp(data, conn, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ଯଦି(pp->overflow)
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ଯଦି((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool done;
      ଯଦି(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        result = Curl_ssl_cfilter_add(
          data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ଯଦି(result) {
          /* we failed and bail out */
          ଫେରନ୍ତୁ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, &done);
      ଯଦି(!result) {
        conn->bits.ftp_use_data_ssl = FALSE; /* clear-text data */
        result = ftp_state_user(data, ftpc, conn);
      }
    }
    ଅନ୍ୟଥା ଯଦି(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      result = Curl_pp_sendf(data, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ଅନ୍ୟଥା {
      ଯଦି(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        result = CURLE_USE_SSL_FAILED;
      ଅନ୍ୟଥା
        /* ignore the failure and continue */
        result = ftp_state_user(data, ftpc, conn);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_USER:
  ସ୍ଥିତି FTP_PASS:
    result = ftp_state_user_resp(data, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_ACCT:
    result = ftp_state_acct_resp(data, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PBSZ:
    result =
      Curl_pp_sendf(data, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ଯଦି(!result)
      ftp_state(data, ftpc, FTP_PROT);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PROT:
    ଯଦି(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->bits.ftp_use_data_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ଅନ୍ୟଥା ଯଦି(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ଫେରନ୍ତୁ CURLE_USE_SSL_FAILED;

    ଯଦି(data->set.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "CCC");
      ଯଦି(!result)
        ftp_state(data, ftpc, FTP_CCC);
    }
    ଅନ୍ୟଥା
      result = ftp_state_pwd(data, ftpc);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_CCC:
    ଯଦି(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      result = Curl_ssl_cfilter_remove(data, FIRSTSOCKET,
                                       (data->set.ftp_ccc ==
                                        (unsigned ଅକ୍ଷର)CURLFTPSSL_CCC_ACTIVE));
      ଯଦି(result)
        failf(data, "Failed to clear the command channel (CCC)");
    }
    ଯଦି(!result)
      /* Then continue as normal */
      result = ftp_state_pwd(data, ftpc);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PWD:
    result = ftp_pwd_resp(data, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_SYST:
    ଯଦି(ftpcode == 215) {
      ଅଚଳ ଅକ୍ଷର *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ଅଚଳ ଅକ୍ଷର *start;
      ଅକ୍ଷର *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ଯେତେବେଳେ(*ptr == ' ')
        ptr++;
      ପାଇଁ(start = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(start, ptr - start);
      ଯଦି(!os)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      /* Check for special servers here. */
      ଯଦି(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        result = Curl_pp_sendf(data, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ଯଦି(result) {
          curlx_free(os);
          ଫେରନ୍ତୁ result;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(data, ftpc, FTP_NAMEFMT);
        ଭାଙ୍ଗ;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ଅନ୍ୟଥା {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_NAMEFMT:
    ଯଦି(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(data, ftpc);
      ଭାଙ୍ଗ;
    }

    ftp_state(data, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(data, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_QUOTE:
  ସ୍ଥିତି FTP_POSTQUOTE:
  ସ୍ଥିତି FTP_RETR_PREQUOTE:
  ସ୍ଥିତି FTP_STOR_PREQUOTE:
  ସ୍ଥିତି FTP_LIST_PREQUOTE:
    ଯଦି((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(data, "QUOT command failed with %03d", ftpcode);
      result = CURLE_QUOTE_ERROR;
    }
    ଅନ୍ୟଥା
      result = ftp_state_quote(data, ftpc, ftp, FALSE, ftpc->state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_CWD:
    ଯଦି(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ଯଦି(data->set.ftp_create_missing_dirs &&
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
        ଯଦି(!result)
          ftp_state(data, ftpc, FTP_MKD);
      }
      ଅନ୍ୟଥା {
        /* return failure */
        failf(data, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        result = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ଅନ୍ୟଥା {
      /* success */
      ftpc->count2 = 0;
      ଯଦି(ftpc->cwdcount >= ftpc->dirdepth)
        result = ftp_state_mdtm(data, ftpc, ftp);
      ଅନ୍ୟଥା {
        ftpc->cwdcount++;
        /* send next CWD */
        result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_MKD:
    ଯଦି((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(data, "Failed to MKD dir: %03d", ftpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
    ଅନ୍ୟଥା {
      ftp_state(data, ftpc, FTP_CWD);
      /* send CWD */
      result = Curl_pp_sendf(data, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_MDTM:
    result = ftp_state_mdtm_resp(data, ftpc, ftp, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_TYPE:
  ସ୍ଥିତି FTP_LIST_TYPE:
  ସ୍ଥିତି FTP_RETR_TYPE:
  ସ୍ଥିତି FTP_STOR_TYPE:
  ସ୍ଥିତି FTP_RETR_LIST_TYPE:
    result = ftp_state_type_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_SIZE:
  ସ୍ଥିତି FTP_RETR_SIZE:
  ସ୍ଥିତି FTP_STOR_SIZE:
    result = ftp_state_size_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_REST:
  ସ୍ଥିତି FTP_RETR_REST:
    result = ftp_state_rest_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PRET:
    ଯଦି(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(data, "PRET command not accepted: %03d", ftpcode);
      ଫେରନ୍ତୁ CURLE_FTP_PRET_FAILED;
    }
    result = ftp_state_use_pasv(data, ftpc, conn);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PASV:
    result = ftp_state_pasv_resp(data, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_PORT:
    result = ftp_state_port_resp(data, ftpc, ftp, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_LIST:
  ସ୍ଥିତି FTP_RETR:
    result = ftp_state_get_resp(data, ftpc, ftp, ftpcode, ftpc->state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_STOR:
    result = ftp_state_stor_resp(data, ftpc, ftpcode);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି FTP_QUIT:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* internal error */
    ftp_state(data, ftpc, FTP_STOP);
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

/* called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode ftp_multi_statemach(ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ଫେରନ୍ତୁ ftpc ? ftp_statemach(data, ftpc, done) : CURLE_FAILED_INIT;
}

ସ୍ଥିର CURLcode ftp_block_statemach(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_conn *ftpc)
{
  ସଂରଚନା pingpong *pp = &ftpc->pp;
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(ftpc->state != FTP_STOP) {
    ଯଦି(ftpc->shutdown)
      CURL_TRC_FTP(data, "in shutdown, waiting for server response");
    result = Curl_pp_statemach(data, pp, TRUE, TRUE /* disconnecting */);
    ଯଦି(result)
      ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ସ୍ଥିର CURLcode ftp_connect(ସଂରଚନା Curl_easy *data,
                            bool *done) /* see description above */
{
  CURLcode result;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ସଂରଚନା pingpong *pp;

  *done = FALSE; /* default to not done yet */
  ଯଦି(!ftpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_SETUP(pp, ftp_pp_statemachine, ftp_endofresp);

  ଯଦି(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    result = Curl_conn_connect(data, FIRSTSOCKET, TRUE, done);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  Curl_pp_init(pp, Curl_pgrs_now(data)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(data, ftpc, FTP_WAIT220);

  result = ftp_statemach(data, ftpc, done);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_sendquote(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ftp_conn *ftpc,
                              ସଂରଚନା curl_slist *quote)
{
  ସଂରଚନା curl_slist *item;
  ସଂରଚନା pingpong *pp = &ftpc->pp;

  item = quote;
  ଯେତେବେଳେ(item) {
    ଯଦି(item->data) {
      size_t nread;
      ଅଚଳ ଅକ୍ଷର *cmd = item->data;
      bool acceptfail = FALSE;
      CURLcode result;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ଯଦି(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      result = Curl_pp_sendf(data, &ftpc->pp, "%s", cmd);
      ଯଦି(!result) {
        pp->response = *Curl_pgrs_now(data); /* timeout relative now */
        result = getftpresponse(data, &nread, &ftpcode);
      }
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      ଯଦି(!acceptfail && (ftpcode >= 400)) {
        failf(data, "QUOT string not accepted: %s", cmd);
        ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
      }
    }

    item = item->next;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ftp_done_status(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ftp_conn *ftpc, CURLcode status,
                                bool premature)
{
  ବାଛ(status) {
  ସ୍ଥିତି CURLE_BAD_DOWNLOAD_RESUME:
  ସ୍ଥିତି CURLE_FTP_WEIRD_PASV_REPLY:
  ସ୍ଥିତି CURLE_FTP_PORT_FAILED:
  ସ୍ଥିତି CURLE_FTP_ACCEPT_FAILED:
  ସ୍ଥିତି CURLE_FTP_ACCEPT_TIMEOUT:
  ସ୍ଥିତି CURLE_FTP_COULDNT_SET_TYPE:
  ସ୍ଥିତି CURLE_FTP_COULDNT_RETR_FILE:
  ସ୍ଥିତି CURLE_PARTIAL_FILE:
  ସ୍ଥିତି CURLE_UPLOAD_FAILED:
  ସ୍ଥିତି CURLE_REMOTE_ACCESS_DENIED:
  ସ୍ଥିତି CURLE_FILESIZE_EXCEEDED:
  ସ୍ଥିତି CURLE_REMOTE_FILE_NOT_FOUND:
  ସ୍ଥିତି CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  ସ୍ଥିତି CURLE_OK: /* does not affect the control connection's status */
    ଯଦି(!premature)
      ଭାଙ୍ଗ;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(data, "FTP ended with bad error code");
    connclose(data->conn);
    ଫେରନ୍ତୁ status;      /* use the already set error code */
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ ftp_done_wildcard(ସଂରଚନା Curl_easy *data, ସଂରଚନା ftp_conn *ftpc)
{
  ଯଦି(data->state.wildcardmatch) {
    ଯଦି(data->set.chunk_end && ftpc->file) {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_chunk_end);
      data->set.chunk_end(data->set.wildcardptr);
      CURL_CBAPI_END(&guard);
      freedirs(ftpc);
    }
    ftpc->known_filesize = -1;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ ftp_done_path(ସଂରଚନା Curl_easy *data, ସଂରଚନା ftp_conn *ftpc,
                          CURLcode result)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(result) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ଅନ୍ୟଥା { /* remember working directory for connection reuse */
    ଅଚଳ ଅକ୍ଷର *rawPath = ftpc->rawpath;
    ଯଦି(rawPath) {
      ଯଦି((data->set.ftp_filemethod == FTPFILE_NOCWD) && (rawPath[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ଅନ୍ୟଥା {
        size_t pathLen = strlen(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ଯଦି(!ftpc->cwdfail) {
          ଯଦି(data->set.ftp_filemethod == FTPFILE_NOCWD)
            pathLen = 0; /* relative path => working directory is FTP home */
          ଅନ୍ୟଥା
            /* file is URL-decoded */
            pathLen -= ftpc->file ? strlen(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(rawPath, pathLen);
        }
        ଅନ୍ୟଥା
          ftpc->prevpath = ଖାଲି; /* no path */
      }
    }
    ଯଦି(ftpc->prevpath)
      infof(data, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ସ୍ଥିର CURLcode ftp_done_secondary_socket(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ftp_conn *ftpc,
                                          CURLcode result)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(Curl_conn_is_setup(conn, SECONDARYSOCKET)) {
    ଯଦି(!result && ftpc->dont_check && data->req.maxdownload > 0) {
      /* partial download completed */
      result = Curl_pp_sendf(data, &ftpc->pp, "%s", "ABOR");
      ଯଦି(result) {
        failf(data, "Failure sending ABOR command: %s",
              curl_easy_strerror(result));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(data, ftpc);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_done_control_reply(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ftp_conn *ftpc,
                                       ସଂରଚନା FTP *ftp, CURLcode result,
                                       bool premature)
{
  ସଂରଚନା connectdata *conn = data->conn;
  size_t nread;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftpcode;

  ଯଦି(!result && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(data); /* timeout relative now */
    result = getftpresponse(data, &nread, &ftpcode);

    ଯଦି(!nread && (result == CURLE_OPERATION_TIMEDOUT)) {
      failf(data, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯଦି(ftpc->dont_check && data->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(data, "partial download completed, closing connection");
      connclose(conn);
      ଫେରନ୍ତୁ result;
    }

    ଯଦି(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ବାଛ(ftpcode) {
      ସ୍ଥିତି 226:
      ସ୍ଥିତି 250:
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 552:
        failf(data, "Exceeded storage allocation");
        result = CURLE_REMOTE_DISK_FULL;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        failf(data, "server did not report OK, got %d", ftpcode);
        result = CURLE_PARTIAL_FILE;
        ଭାଙ୍ଗ;
      }
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ftp_done_check_partial(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ftp_conn *ftpc,
                                       ସଂରଚନା FTP *ftp, CURLcode result,
                                       bool premature)
{
  ଯଦି(result || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ଅନ୍ୟଥା ଯଦି(data->state.upload) {
    ଯଦି((ftp->transfer == PPTRANSFER_BODY) &&
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
  ଅନ୍ୟଥା {
    ଯଦି((data->req.size != -1) &&
       (data->req.size != data->req.bytecount) &&
       (data->req.maxdownload != data->req.bytecount)) {
      failf(data, "Received only partial file: %" FMT_OFF_T " bytes",
            data->req.bytecount);
      result = CURLE_PARTIAL_FILE;
    }
    ଅନ୍ୟଥା ଯଦି(!ftpc->dont_check &&
            !data->req.bytecount &&
            (data->req.size > 0)) {
      failf(data, "No data was received");
      result = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                         bool premature)
{
  ସଂରଚନା FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  CURLcode result;

  ଯଦି(!ftp || !ftpc)
    ଫେରନ୍ତୁ CURLE_OK;

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
  ଯଦି(!status && !result && !premature && data->set.postquote)
    result = ftp_sendquote(data, ftpc, data->set.postquote);
  CURL_TRC_FTP(data, "[%s] done, result=%d", FTP_CSTATE(ftpc), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ସ୍ଥିର CURLcode ftp_nb_type(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ftp_conn *ftpc,
                            ସଂରଚନା FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode result;
  ଅକ୍ଷର want = (ଅକ୍ଷର)(ascii ? 'A' : 'I');

  ଯଦି(ftpc->transfertype == want) {
    ftp_state(data, ftpc, newstate);
    ଫେରନ୍ତୁ ftp_state_type_resp(data, ftpc, ftp, 200, newstate);
  }

  result = Curl_pp_sendf(data, &ftpc->pp, "TYPE %c", want);
  ଯଦି(!result) {
    ftp_state(data, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ସ୍ଥିର CURLcode ftp_perform(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା ftp_conn *ftpc,
  ସଂରଚନା FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_done)
{
  /* this is FTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_FTP(data, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ଯଦି(data->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  result = ftp_state_quote(data, ftpc, ftp, TRUE, FTP_QUOTE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* run the state-machine */
  result = ftp_statemach(data, ftpc, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, SECONDARYSOCKET);

  ଯଦି(*connected)
    infof(data, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ଅନ୍ୟଥା
    CURL_TRC_FTP(data, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ଯଦି(*dophase_done)
    CURL_TRC_FTP(data, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ wc_data_dtor(ଶୂନ୍ୟ *ptr)
{
  ସଂରଚନା ftp_wc *ftpwc = ptr;
  ଯଦି(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_data_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ସ୍ଥିର CURLcode init_wc_data(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ftp_conn *ftpc,
                             ସଂରଚନା FTP *ftp)
{
  ଅକ୍ଷର *last_slash;
  ଅକ୍ଷର *path = ftp->path;
  ସଂରଚନା WildcardData *wildcard = data->wildcard;
  CURLcode result = CURLE_OK;
  ସଂରଚନା ftp_wc *ftpwc = ଖାଲି;

  last_slash = strrchr(ftp->path, '/');
  ଯଦି(last_slash) {
    last_slash++;
    ଯଦି(last_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ଫେରନ୍ତୁ ftp_parse_url_path(data, ftpc, ftp);
    }
    wildcard->pattern = curlx_strdup(last_slash);
    ଯଦି(!wildcard->pattern)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    last_slash[0] = '\0'; /* cut file from path */
  }
  ଅନ୍ୟଥା { /* there is only 'wildcard pattern' or nothing */
    ଯଦି(path[0]) {
      wildcard->pattern = curlx_strdup(path);
      ଯଦି(!wildcard->pattern)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      path[0] = '\0';
    }
    ଅନ୍ୟଥା { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ଫେରନ୍ତୁ ftp_parse_url_path(data, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, ଆକାର(ସଂରଚନା ftp_wc));
  ଯଦି(!ftpwc) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_data_alloc();
  ଯଦି(!ftpwc->parser) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_data_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ଯଦି(data->set.ftp_filemethod == FTPFILE_NOCWD)
    data->set.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  result = ftp_parse_url_path(data, ftpc, ftp);
  ଯଦି(result) {
    ଯାଅ fail;
  }

  wildcard->path = curlx_strdup(ftp->path);
  ଯଦି(!wildcard->path) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  infof(data, "Wildcard - Parsing started");
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  ଯଦି(ftpwc) {
    Curl_ftp_parselist_data_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->pattern);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ଖାଲି;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode wc_statemach(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ftp_conn *ftpc,
                             ସଂରଚନା FTP *ftp)
{
  ସଂରଚନା WildcardData * ଅଚଳ wildcard = data->wildcard;
  CURLcode result = CURLE_OK;

  ପାଇଁ(;;) {
    ବାଛ(wildcard->state) {
    ସ୍ଥିତି CURLWC_INIT:
      result = init_wc_data(data, ftpc, ftp);
      ଯଦି(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ଫେରନ୍ତୁ result;
      wildcard->state = result ? CURLWC_ERROR : CURLWC_MATCHING;
      ଫେରନ୍ତୁ result;

    ସ୍ଥିତି CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ସଂରଚନା ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ଯଦି(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ଜାରି;
      }
      ଯଦି(Curl_llist_ଗଣନା(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;
      }
      ଜାରି;
    }

    ସ୍ଥିତି CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ସଂରଚନା Curl_llist_ଗଣ୍ଠି *head = Curl_llist_head(&wildcard->filelist);
      ସଂରଚନା curl_fileinfo *finfo = Curl_ଗଣ୍ଠି_elem(head);
      ଅକ୍ଷର *tmp_path;
      ଅକ୍ଷର *enc = curl_easy_escape(ଖାଲି, finfo->filename, 0);
      ଯଦି(!enc)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      tmp_path = curl_maprintf("%s%s", wildcard->path, enc);
      curl_free(enc);
      ଯଦି(!tmp_path)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->path = tmp_path;

      infof(data, "Wildcard - START of \"%s\"", finfo->filename);
      ଯଦି(data->set.chunk_bgn) {
        long userresponse;
        ସଂରଚନା Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_bgn);
        userresponse = data->set.chunk_bgn(
          finfo, data->set.wildcardptr,
          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Curl_llist_ଗଣନା(&wildcard->filelist));
        CURL_CBAPI_END(&guard);
        ବାଛ(userresponse) {
        ସ୍ଥିତି CURL_CHUNK_BGN_FUNC_SKIP:
          infof(data, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ଜାରି;
        ସ୍ଥିତି CURL_CHUNK_BGN_FUNC_FAIL:
          ଫେରନ୍ତୁ CURLE_CHUNK_FAILED;
        }
      }

      ଯଦି(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ଜାରି;
      }

      ଯଦି(finfo->flags & CURLFINFOFLAG_KNOWN_SIZE)
        ftpc->known_filesize = finfo->size;

      result = ftp_parse_url_path(data, ftpc, ftp);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ଗଣ୍ଠି_remove(Curl_llist_head(&wildcard->filelist));

      ଯଦି(Curl_llist_ଗଣନା(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଫେରନ୍ତୁ result;
    }

    ସ୍ଥିତି CURLWC_SKIP: {
      ଯଦି(data->set.chunk_end) {
        ସଂରଚନା Curl_mapi_guard guard;
        CURL_CBAPI_START(&guard, data, easy_chunk_end);
        data->set.chunk_end(data->set.wildcardptr);
        CURL_CBAPI_END(&guard);
      }
      Curl_ଗଣ୍ଠି_remove(Curl_llist_head(&wildcard->filelist));
      wildcard->state = (Curl_llist_ଗଣନା(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ଜାରି;
    }

    ସ୍ଥିତି CURLWC_CLEAN: {
      ସଂରଚନା ftp_wc *ftpwc = wildcard->ftpwc;
      result = CURLE_OK;
      ଯଦି(ftpwc)
        result = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = result ? CURLWC_ERROR : CURLWC_DONE;
      ଫେରନ୍ତୁ result;
    }

    ସ୍ଥିତି CURLWC_DONE:
    ସ୍ଥିତି CURLWC_ERROR:
    ସ୍ଥିତି CURLWC_CLEAR:
      ଯଦି(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ଖାଲି;
      }
      ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_regular_transfer(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ftp_conn *ftpc,
                                     ସଂରଚନା FTP *ftp,
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

  ଯଦି(!result) {

    ଯଦି(!*dophase_done)
      /* the DO phase has not completed yet */
      ଫେରନ୍ତୁ CURLE_OK;

    result = ftp_dophase_done(data, ftpc, ftp, connected);

    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
    freedirs(ftpc);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ସଂରଚନା FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);

  *done = FALSE; /* default to false */
  ଯଦି(!ftpc || !ftp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ftpc->wait_data_conn = FALSE; /* default to no such wait */

#ifdef CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ସଂରଚନା Curl_cwriter *ftp_lc_writer;

    result = Curl_cwriter_create(&ftp_lc_writer, data, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    result = Curl_cwriter_add(data, ftp_lc_writer);
    ଯଦି(result) {
      Curl_cwriter_free(data, ftp_lc_writer);
      ଫେରନ୍ତୁ result;
    }
  }
#endif /* CURL_PREFER_LF_LINEENDS */

  ଯଦି(data->state.wildcardmatch) {
    result = wc_statemach(data, ftpc, ftp);
    ଯଦି(data->wildcard->state == CURLWC_SKIP ||
       data->wildcard->state == CURLWC_DONE) {
      /* do not call ftp_regular_transfer */
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଯଦି(result) /* error, loop or skipping the file */
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା { /* no wildcard FSM needed */
    result = ftp_parse_url_path(data, ftpc, ftp);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  result = ftp_regular_transfer(data, ftpc, ftp, done);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode ftp_quit(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା ftp_conn *ftpc)
{
  CURLcode result = CURLE_OK;

  ଯଦି(ftpc->ctl_valid) {
    CURL_TRC_FTP(data, "sending QUIT to close session");
    result = Curl_pp_sendf(data, &ftpc->pp, "%s", "QUIT");
    ଯଦି(result) {
      failf(data, "Failure sending QUIT command: %s",
            curl_easy_strerror(result));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(data->conn); /* mark for closure */
      ftp_state(data, ftpc, FTP_STOP);
      ଫେରନ୍ତୁ result;
    }

    ftp_state(data, ftpc, FTP_QUIT);

    result = ftp_block_statemach(data, ftpc);
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ସ୍ଥିର CURLcode ftp_disconnect(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               bool dead_connection)
{
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ଯଦି(!ftpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ଯଦି(dead_connection || Curl_pp_needs_flush(data, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (ଶୂନ୍ୟ)ftp_quit(data, ftpc); /* ignore errors on the QUIT */
  ଫେରନ୍ତୁ CURLE_OK;
}

/* called from multi.c while DOing */
ସ୍ଥିର CURLcode ftp_doing(ସଂରଚନା Curl_easy *data,
                          bool *dophase_done)
{
  ସଂରଚନା ftp_conn *ftpc = Curl_conn_meta_get(data->conn, CURL_META_FTP_CONN);
  ସଂରଚନା FTP *ftp = Curl_meta_get(data, CURL_META_FTP_EASY);
  CURLcode result;

  ଯଦି(!ftpc || !ftp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = ftp_statemach(data, ftpc, dophase_done);

  ଯଦି(result)
    CURL_TRC_FTP(data, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ଅନ୍ୟଥା ଯଦି(*dophase_done) {
    result = ftp_dophase_done(data, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(data, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ ftp_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା FTP *ftp = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ସ୍ଥିର ଶୂନ୍ୟ ftp_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ftp_conn *ftpc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_to_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ସ୍ଥିର ଶୂନ୍ୟ type_url_check(ସଂରଚନା Curl_easy *data, ସଂରଚନା FTP *ftp)
{
  size_t len = strlen(ftp->path);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ଯଦି((len >= 7) && !memcmp(&ftp->path[len - 7], ";type=", 6)) {
    ଅକ୍ଷର *type = &ftp->path[len - 7];
    ଅକ୍ଷର command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ବାଛ(command) {
    ସ୍ଥିତି 'A': /* ASCII mode */
      data->state.prefer_ascii = TRUE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 'D': /* directory mode */
      data->state.list_only = TRUE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 'I': /* binary mode */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* switch off ASCII */
      data->state.prefer_ascii = FALSE;
      ଭାଙ୍ଗ;
    }
  }
}

ସ୍ଥିର CURLcode ftp_setup_connection(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା FTP *ftp;
  CURLcode result = CURLE_OK;
  ସଂରଚନା ftp_conn *ftpc;

  ftp = curlx_calloc(1, ଆକାର(*ftp));
  ଯଦି(!ftp ||
     Curl_meta_set(data, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ftpc = curlx_calloc(1, ଆକାର(*ftpc));
  ଯଦି(!ftpc ||
     Curl_conn_meta_set(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* clone connection related data that is FTP specific */
  ଯଦି(CURL_EASY_STR(data, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ACCOUNT));
    ଯଦି(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  ଯଦି(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER)) {
    ftpc->alternative_to_user =
      curlx_strdup(CURL_EASY_STR(data, STRING_FTP_ALTERNATIVE_TO_USER));
    ଯଦି(!ftpc->alternative_to_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
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
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

bool Curl_ftp_conns_match(ସଂରଚନା connectdata *needle, ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ସଂରଚନା ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ଯଦି(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_to_user,
                     cftpc->alternative_to_user) ||
     (nftpc->ccc != cftpc->ccc))
    ଫେରନ୍ତୁ FALSE;

  ବାଛ(nftpc->use_ssl) {
  ସ୍ଥିତି CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ଫେରନ୍ତୁ (cftpc->use_ssl != CURLUSESSL_NONE);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_ftp = {
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
