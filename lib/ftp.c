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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "if2ip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftplistparser.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_range.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h" /* for the week day and month names */
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h" /* required for Curl_sockaddr_storage */
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ctype.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NI_MAXHOST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NI_MAXHOST 1025
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ INET_ADDRSTRLEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INET_ADDRSTRLEN 16
#ਜੇ_ਅੰਤ

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STATUSCODE(ਸਤਰ) \
  (ISDIGIT((ਸਤਰ)[0]) && ISDIGIT((ਸਤਰ)[1]) && ISDIGIT((ਸਤਰ)[2]))

/* macro to check for the last line in an FTP server response */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LASTLINE(ਸਤਰ) (STATUSCODE(ਸਤਰ) && (' ' == (ਸਤਰ)[3]))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
/* for tracing purposes */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ftp_state_ਨਾਂ[] = {
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_ਨਾਂ[(ftpc)->state] : "???")

#ਜੇ_ਅੰਤ /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ਸਥਿਰ ਖਾਲੀ ftp_state_ਨੀਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ ftp_conn *ftpc,
                          ftpstate newstate
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                          , ਪੂਰਨ lineno
#ਜੇ_ਅੰਤ
  )
{
  ਜੇ(ftpc->state != newstate) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    NOVERBOSE((ਖਾਲੀ)lineno);
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_ਨਾਂ[newstate], lineno);
#ਨਹੀਂਤਾਂ
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_ਨਾਂ[newstate]);
#ਜੇ_ਅੰਤ
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ftp_state(x, y, z) ftp_state_ਨੀਵਾਂ(x, y, z)
#ਨਹੀਂਤਾਂ /* !DEBUGBUILD */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ftp_state(x, y, z) ftp_state_ਨੀਵਾਂ(x, y, z, __LINE__)
#ਜੇ_ਅੰਤ /* DEBUGBUILD */

ਸਥਿਰ CURLcode ftp_state_mdtm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp);
ਸਥਿਰ CURLcode ftp_state_quote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ftp_conn *ftpc,
                                ਬਣਤਰ FTP *ftp,
                                bool init, ftpstate instate);
ਸਥਿਰ CURLcode ftp_nb_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ftp_conn *ftpc,
                            ਬਣਤਰ FTP *ftp,
                            bool ascii, ftpstate newstate);
ਸਥਿਰ CURLcode getftpresponse(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t *nreadp,
                               ਪੂਰਨ *ftpcodep);

ਸਥਿਰ ਖਾਲੀ freedirs(ਬਣਤਰ ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ਨਿਰਾ;
}

ਸਥਿਰ size_t numof_slashes(ਅਚਲ ਅੱਖਰ *str)
{
  ਅਚਲ ਅੱਖਰ *slashPos;
  size_t ਅੰਕ = 0;
  ਕਰੋ {
    slashPos = strchr(str, '/');
    ਜੇ(slashPos) {
      ++ਅੰਕ;
      str = slashPos + 1;
    }
  } ਜਦੋਂ(slashPos);
  ਵਾਪਸ ਅੰਕ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTP_MAX_DIR_ਡੂੰਘਾਈ 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ਸਥਿਰ CURLcode ftp_parse_url_ਰਸਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc,
                                   ਬਣਤਰ FTP *ftp)
{
  ਅਚਲ ਅੱਖਰ *slashPos = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *fileName = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *raw_ਰਸਤਾ = ਨਿਰਾ; /* URL-decoded "raw" path */
  size_t ਰਸਤਾ_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ਜੇ(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  ਨਤੀਜਾ = Curl_urldecode(ftp->ਰਸਤਾ, 0, &ftpc->rawpath, &ਰਸਤਾ_Len, REJECT_CTRL);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "path contains control characters");
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  raw_ਰਸਤਾ = ftpc->rawpath;

  ਚੋਣ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod) {
  ਹਾਲਤ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ਜੇ((ਰਸਤਾ_Len > 0) && (raw_ਰਸਤਾ[ਰਸਤਾ_Len - 1] != '/'))
      fileName = raw_ਰਸਤਾ;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ਰੋਕੋ;

  ਹਾਲਤ FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_ਰਸਤਾ, '/');
    ਜੇ(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_ਰਸਤਾ;
      ਜੇ(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, ਆਕਾਰ(ftpc->dirs[0]));
      ਜੇ(!ftpc->dirs)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ftpc->dirs[0].ਸ਼ੁਰੂ = 0;
      ftpc->dirs[0].len = (ਪੂਰਨ)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ਨਹੀਂਤਾਂ
      fileName = raw_ਰਸਤਾ; /* filename only (or empty) */
    ਰੋਕੋ;

  ਮੂਲ: /* allow pretty much anything */
  ਹਾਲਤ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ਅਚਲ ਅੱਖਰ *curPos = raw_ਰਸਤਾ;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_ਰਸਤਾ);

    ਜੇ(dirAlloc >= FTP_MAX_DIR_ਡੂੰਘਾਈ)
      /* suspiciously deep directory hierarchy */
      ਵਾਪਸ CURLE_URL_MALFORMAT;

    ਜੇ(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, ਆਕਾਰ(ftpc->dirs[0]));
      ਜੇ(!ftpc->dirs)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      /* parse the URL path into separate path components */
      ਜਦੋਂ(dirAlloc--) {
        ਅਚਲ ਅੱਖਰ *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ਜੇ(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ਜੇ(clen) {
          ftpc->dirs[ftpc->dirdepth].ਸ਼ੁਰੂ = (ਪੂਰਨ)(curPos - raw_ਰਸਤਾ);
          ftpc->dirs[ftpc->dirdepth].len = (ਪੂਰਨ)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ਰੋਕੋ;
  } /* switch */

  ਜੇ(fileName && *fileName)
    ftpc->file = fileName;
  ਨਹੀਂਤਾਂ
    ftpc->file = ਨਿਰਾ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ਜੇ(ਅੰਕੜੇ->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(ਅੰਕੜੇ, "Uploading to a URL without a filename");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD) && (raw_ਰਸਤਾ[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ਨਹੀਂਤਾਂ { /* newly created FTP connections are already in entry path */
    ਅਚਲ ਅੱਖਰ *old_ਰਸਤਾ = ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse ? ftpc->prevpath : "";
    ਜੇ(old_ਰਸਤਾ) {
      size_t n = ਰਸਤਾ_Len;
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ਨਹੀਂਤਾਂ
        n -= ftpc->file ? ਲੜੀ_ਲੰਬਾਈ(ftpc->file) : 0;

      ਜੇ((ਲੜੀ_ਲੰਬਾਈ(old_ਰਸਤਾ) == n) && raw_ਰਸਤਾ && !ਲੜੀ_ਤੁਲਨਾ(raw_ਰਸਤਾ, old_ਰਸਤਾ, n)) {
        infof(ਅੰਕੜੇ, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ਸਥਿਰ ਪੂਰਨ ftp_need_type(ਬਣਤਰ ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ਵਾਪਸ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ਸਥਿਰ ਖਾਲੀ close_secondarysocket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ftp_conn *ftpc)
{
  (ਖਾਲੀ)ftpc;
  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, SECONDARYSOCKET);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
ਬਣਤਰ ftp_cw_lc_ctx {
  ਬਣਤਰ Curl_cwriter super;
  bool newline_pending;
};

ਸਥਿਰ CURLcode ftp_cw_lc_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ nl = '\n';
  ਬਣਤਰ ftp_cw_lc_ctx *ctx = writer->ctx;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);

  ਜੇ(!ftpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ਜਦੋਂ(blen) {
    /* do not pass EOS when writing parts */
    ਪੂਰਨ chunk_type = (type & ~CLIENTWRITE_EOS);
    ਅਚਲ ਅੱਖਰ *cp;
    size_t chunk_len;
    CURLcode ਨਤੀਜਾ;

    ਜੇ(ctx->newline_pending) {
      ਜੇ(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, chunk_type, &nl, 1);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ਜੇ(!cp)
      ਰੋਕੋ;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ਜੇ(chunk_len) {
      ਨਤੀਜਾ = Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, chunk_type,
                                  buf, chunk_len);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ਜੇ(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, blen);
  }
  ਨਹੀਂਤਾਂ ਜੇ(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ਜੇ(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, 0);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ਨਿਰਾ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ਆਕਾਰ(ਬਣਤਰ ftp_cw_lc_ctx)
};

#ਜੇ_ਅੰਤ /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ਸਥਿਰ CURLcode ftp_check_ctrl_on_ਅੰਕੜੇ_wait(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ftp_conn *ftpc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ਬਣਤਰ pingpong *pp = &ftpc->pp;
  size_t nread;
  ਪੂਰਨ ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ਜੇ(curlx_dyn_len(&pp->recvbuf)) {
    ਅਚਲ ਅੱਖਰ *l = curlx_dyn_ptr(&pp->recvbuf);
    ਜੇ(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(ਅੰਕੜੇ, "There is negative response in cache while serv connect");
      (ਖਾਲੀ)getftpresponse(ਅੰਕੜੇ, &nread, &ftpcode);
      ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ਜੇ(pp->ਉਛਾਲ)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ਨਹੀਂਤਾਂ {
    ਪੂਰਨ socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ਚੋਣ(socketstate) {
    ਹਾਲਤ -1: /* error */
      /* let's die here */
      failf(ਅੰਕੜੇ, "Error while waiting for server connect");
      ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
    ਮੂਲ:
      ਜੇ(socketstate & CURL_CSELECT_ਵਿੱਚ)
        response = TRUE;
      ਰੋਕੋ;
    }
  }

  ਜੇ(response) {
    infof(ਅੰਕੜੇ, "Ctrl conn has data while waiting for data conn");
    ਜੇ(pp->ਉਛਾਲ > 3) {
      ਅਚਲ ਅੱਖਰ *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->ਉਛਾਲ + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ਜੇ((len > 3) && LASTLINE(r)) {
        curl_off_t ਸਥਿਤੀ;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&r, &ਸਥਿਤੀ, 999) && (ਸਥਿਤੀ == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(ਅੰਕੜੇ, "Got 226 before data activity");
          ਵਾਪਸ CURLE_OK;
        }
      }
    }

    (ਖਾਲੀ)getftpresponse(ਅੰਕੜੇ, &nread, &ftpcode);

    infof(ਅੰਕੜੇ, "FTP code: %03d", ftpcode);

    ਜੇ(ftpcode / 100 > 3)
      ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;

    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ਸਥਿਰ CURLcode ftp_initiate_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ftp_conn *ftpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(ਅੰਕੜੇ, "ftp_initiate_transfer()");
  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, SECONDARYSOCKET, TRUE, &connected);
  ਜੇ(ਨਤੀਜਾ || !connected)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(ਅੰਕੜੇ->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, SECONDARYSOCKET);
    Curl_xfer_ਸੈੱਟ_shutdown(ਅੰਕੜੇ, TRUE, TRUE);
  }
  ਨਹੀਂਤਾਂ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, SECONDARYSOCKET, ਅੰਕੜੇ->req.ਮਾਪ);
    Curl_xfer_ਸੈੱਟ_shutdown(ਅੰਕੜੇ, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool ftp_endofresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                          ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len, ਪੂਰਨ *ਕੋਡ)
{
  curl_off_t ਸਥਿਤੀ;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;

  ਜੇ((len > 3) && LASTLINE(ਸਤਰ) && !curlx_str_ਸੰਖਿਆ(&ਸਤਰ, &ਸਥਿਤੀ, 999)) {
    *ਕੋਡ = (ਪੂਰਨ)ਸਥਿਤੀ;
    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode ftp_readresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ftp_conn *ftpc,
                             int8_t sockindex,
                             ਬਣਤਰ pingpong *pp,
                             ਪੂਰਨ *ftpcodep, /* return the ftp-code if done */
                             size_t *ਮਾਪ) /* size of the response */
{
  ਪੂਰਨ ਕੋਡ;
  CURLcode ਨਤੀਜਾ = Curl_pp_readresp(ਅੰਕੜੇ, sockindex, pp, &ਕੋਡ, ਮਾਪ);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ਜੇ(!ftpc->shutdown)
    ਅੰਕੜੇ->info.httpcode = ਕੋਡ;

  *ftpcodep = ਕੋਡ;

  ਜੇ(ਕੋਡ == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(ਅੰਕੜੇ, "We got a 421 - timeout");
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ਸਥਿਰ CURLcode getftpresponse(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               ਪੂਰਨ *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਬਣਤਰ pingpong *pp = &ftpc->pp;
  size_t nread;
  ਪੂਰਨ cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(ਅੰਕੜੇ, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ਜੇ(!ftpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜਦੋਂ(!*ftpcodep && !ਨਤੀਜਾ) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(ਅੰਕੜੇ, pp);
    timediff_t interval_ms;

    ਜੇ(timeout <= 0) {
      failf(ਅੰਕੜੇ, "FTP response timeout");
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ਜੇ(timeout < interval_ms)
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

    ਜੇ(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    ਨਹੀਂਤਾਂ ਜੇ(!Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(ਅੰਕੜੇ, pp) ?
        sockfd : CURL_SOCKET_BAD;
      ਪੂਰਨ ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ਜੇ(ev < 0) {
        failf(ਅੰਕੜੇ, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ev == 0) {
        ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
        ਜਾਰੀ; /* continue in our loop for the timeout duration */
      }
    }

    ਜੇ(Curl_pp_needs_flush(ਅੰਕੜੇ, pp)) {
      ਨਤੀਜਾ = Curl_pp_flushsend(ਅੰਕੜੇ, pp);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }

    ਨਤੀਜਾ = ftp_readresp(ਅੰਕੜੇ, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਜੇ(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
ਅੰਕੜੇ*/
      cache_skip++;
    ਨਹੀਂਤਾਂ
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(ਅੰਕੜੇ, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (ਪੂਰਨ)ਨਤੀਜਾ, *nreadp, *ftpcodep);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_user(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ਜੇ(!ਨਤੀਜਾ) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_USER);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_pwd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ftp_conn *ftpc)
{
  CURLcode ਨਤੀਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(!ਅੰਕੜੇ->id && getenv("CURL_FTP_PWD_STOP"))
    ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "PWD");
  ਜੇ(!ਨਤੀਜਾ)
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_PWD);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the FTP "protocol connect" and "doing" phases only */
ਸਥਿਰ CURLcode ftp_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਵਾਪਸ ftpc ? Curl_pp_pollset(ਅੰਕੜੇ, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ਸਥਿਰ CURLcode ftp_domore_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);

  ਜੇ(!ftpc)
    ਵਾਪਸ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ਜੇ(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(ਅੰਕੜੇ->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (ਅੰਕੜੇ->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(ਅੰਕੜੇ->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
  }
  ਵਾਪਸ Curl_pp_pollset(ਅੰਕੜੇ, &ftpc->pp, ps);
}

ਸਥਿਰ ਪੂਰਨ pathlen(ਬਣਤਰ ftp_conn *ftpc, ਪੂਰਨ ਅੰਕ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ਅੰਕ);
  ਵਾਪਸ ftpc->dirs[ਅੰਕ].len;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *pathpiece(ਬਣਤਰ ftp_conn *ftpc, ਪੂਰਨ ਅੰਕ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ਅੰਕ);
  ਵਾਪਸ &ftpc->rawpath[ftpc->dirs[ਅੰਕ].ਸ਼ੁਰੂ];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ਸਥਿਰ CURLcode ftp_state_cwd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ftp_conn *ftpc,
                              ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ftpc->cwddone)
    /* already done and fine */
    ਨਤੀਜਾ = ftp_state_mdtm(ਅੰਕੜੇ, ftpc, ftp);
  ਨਹੀਂਤਾਂ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ਜੇ(ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_CWD);
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ਜੇ(!ਨਤੀਜਾ)
          ftp_state(ਅੰਕੜੇ, ftpc, FTP_CWD);
      }
      ਨਹੀਂਤਾਂ {
        /* No CWD necessary */
        ਨਤੀਜਾ = ftp_state_mdtm(ਅੰਕੜੇ, ftpc, ftp);
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  EPRT,
  ਪੋਰਟ_2,
  ਪੂਰਾ_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_parse_string(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn,
                                      ਅਚਲ ਅੱਖਰ *string_ftpport,
                                      ਬਣਤਰ Curl_sockaddr_storage *ss,
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *ਪੋਰਟ_minp,
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *ਪੋਰਟ_maxp,
                                      ਅਚਲ ਅੱਖਰ **hostp,
                                      ਅੱਖਰ *hbuf, size_t hbuflen)
{
  ਅਚਲ ਅੱਖਰ *ip_ਸਿਰਾ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *addr = ਨਿਰਾ;
  size_t addrlen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_min = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_max = 0;
  ਅੱਖਰ ipstr[50];
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ss;
#ਜੇ_ਅੰਤ

  /* default to nothing */
  *hostp = ਨਿਰਾ;
  *ਪੋਰਟ_minp = *ਪੋਰਟ_maxp = 0;

  ਜੇ(!string_ftpport || (ਲੜੀ_ਲੰਬਾਈ(string_ftpport) <= 1))
    ਜਾਓ ਪੂਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ਅਚਲ ਅੱਖਰ *ip_ਸ਼ੁਰੂ = string_ftpport + 1;
    ip_ਸਿਰਾ = strchr(ip_ਸ਼ੁਰੂ, ']');
    ਜੇ(ip_ਸਿਰਾ) {
      addrlen = ip_ਸਿਰਾ - ip_ਸ਼ੁਰੂ;
      addr = ip_ਸ਼ੁਰੂ;
    }
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ(*string_ftpport == ':') {
      /* :port */
      ip_ਸਿਰਾ = string_ftpport;
    }
    ਨਹੀਂਤਾਂ {
      ip_ਸਿਰਾ = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ਜੇ(ip_ਸਿਰਾ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
        ਬਣਤਰ sockaddr_in6 * ਅਚਲ sa6 = (ਖਾਲੀ *)ss;
#ਜੇ_ਅੰਤ
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_ਸਿਰਾ - string_ftpport;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
        ਜੇ(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = ਲੜੀ_ਲੰਬਾਈ(string_ftpport);
          ip_ਸਿਰਾ = ਨਿਰਾ; /* this got no port ! */
        }
#ਜੇ_ਅੰਤ
      }
      ਨਹੀਂਤਾਂ
        /* ipv4|interface */
        addrlen = ਲੜੀ_ਲੰਬਾਈ(string_ftpport);
    }

  /* parse the port */
  ਜੇ(ip_ਸਿਰਾ) {
    ਅਚਲ ਅੱਖਰ *portp = strchr(ip_ਸਿਰਾ, ':');
    ਜੇ(portp) {
      curl_off_t ਸ਼ੁਰੂ;
      curl_off_t ਸਿਰਾ;
      portp++;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&portp, &ਸ਼ੁਰੂ, 0xffff)) {
        ਪੋਰਟ_min = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਸ਼ੁਰੂ;
        ਜੇ(!curlx_str_single(&portp, '-') &&
           !curlx_str_ਸੰਖਿਆ(&portp, &ਸਿਰਾ, 0xffff))
          ਪੋਰਟ_max = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਸਿਰਾ;
        ਨਹੀਂਤਾਂ
          ਪੋਰਟ_max = ਪੋਰਟ_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ਜੇ(ਪੋਰਟ_min > ਪੋਰਟ_max)
    ਪੋਰਟ_min = ਪੋਰਟ_max = 0;

  ਜੇ(addrlen) {
    ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(ਅੰਕੜੇ, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ਜੇ(!remote_addr || (addrlen >= ਆਕਾਰ(ipstr)) || (addrlen >= hbuflen))
      ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ਚੋਣ(Curl_if2ip(remote_addr->family,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#ਜੇ_ਅੰਤ
                      ipstr, hbuf, hbuflen)) {
    ਹਾਲਤ IF2_IP_NOT_ਮਿਲਿਆ:
      /* not an interface, use the string as hostname instead */
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ਰੋਕੋ;
    ਹਾਲਤ IF2IP_AF_NOT_SUPPORTED:
      ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
    ਹਾਲਤ IF2_IP_ਮਿਲਿਆ:
      *hostp = hbuf; /* use the hbuf for hostname */
      ਰੋਕੋ;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

ਪੂਰਾ:
  *ਪੋਰਟ_minp = ਪੋਰਟ_min;
  *ਪੋਰਟ_maxp = ਪੋਰਟ_max;
  ਵਾਪਸ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_default_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn,
                                      ਬਣਤਰ Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ਅਚਲ ਅੱਖਰ **hostp,
                                      ਅੱਖਰ *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ਬਣਤਰ sockaddr *sa = (ਬਣਤਰ sockaddr *)ss;
  ਬਣਤਰ sockaddr_in * ਅਚਲ sa4 = (ਖਾਲੀ *)sa;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 * ਅਚਲ sa6 = (ਖਾਲੀ *)sa;
#ਜੇ_ਅੰਤ
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
  CURLcode ਨਤੀਜਾ;

  *sslenp = ਆਕਾਰ(*ss);
  ਜੇ(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(ਅੰਕੜੇ, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
  }
  ਚੋਣ(sa->sa_family) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ AF_INET6:
    ਨਤੀਜਾ = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਨਤੀਜਾ = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ਰੋਕੋ;
  }
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ਵਾਪਸ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_resolve_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn,
                                      ਅਚਲ ਅੱਖਰ *host,
                                      ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **dns_entryp,
                                      ਅਚਲ ਬਣਤਰ Curl_addrinfo **resp)
{
  CURLcode ਨਤੀਜਾ;

  *resp = ਨਿਰਾ;
  ਨਤੀਜਾ = Curl_resolv_blocking(
    ਅੰਕੜੇ, Curl_resolv_dns_queries(ਅੰਕੜੇ, conn->ip_ਸੰਸਕਰਣ),
    host, 0, Curl_conn_get_transport(ਅੰਕੜੇ, conn), dns_entryp);
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "failed to resolve the address provided to PORT: %s", host);
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_open_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn,
                                     ਅਚਲ ਬਣਤਰ Curl_addrinfo *res,
                                     ਅਚਲ ਬਣਤਰ Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
  ਪੂਰਨ sockerr = 0;
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai;
  CURLcode ਨਤੀਜਾ = CURLE_FTP_ਪੋਰਟ_FAILED;

  ਲਈ(ai = res; ai; ai = ai->ai_ਅਗਲਾ) {
    ਨਤੀਜਾ =
      Curl_socket_open(ਅੰਕੜੇ, ai, ਨਿਰਾ,
                       Curl_conn_get_transport(ਅੰਕੜੇ, conn), portsockp);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਨਤੀਜਾ = CURLE_FTP_ਪੋਰਟ_FAILED;
      sockerr = SOCKERRNO;
      ਜਾਰੀ;
    }
    ਰੋਕੋ;
  }
  ਜੇ(!ai) {
    failf(ਅੰਕੜੇ, "socket failure: %s",
          curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
  }
  *aip = ai;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_bind_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn,
                                     curl_socket_t portsock,
                                     ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                                     ਬਣਤਰ Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_min,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_max,
                                     bool non_local)
{
  ਬਣਤਰ sockaddr *sa = (ਬਣਤਰ sockaddr *)ss;
  ਬਣਤਰ sockaddr_in * ਅਚਲ sa4 = (ਖਾਲੀ *)sa;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 * ਅਚਲ sa6 = (ਖਾਲੀ *)sa;
#ਜੇ_ਅੰਤ
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ;
  ਪੂਰਨ sockerr;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  ਲਈ(ਪੋਰਟ = ਪੋਰਟ_min; ਪੋਰਟ <= ਪੋਰਟ_max;) {
    ਜੇ(sa->sa_family == AF_INET)
      sa4->sin_port = htons(ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ
      sa6->sin6_port = htons(ਪੋਰਟ);
#ਜੇ_ਅੰਤ
    ਜੇ(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ਜੇ(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(ਅੰਕੜੇ, "bind(port=%hu) on non-local address failed: %s", ਪੋਰਟ,
              curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));

        *sslen_io = ਆਕਾਰ(*ss);
        ਜੇ(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(ਅੰਕੜੇ, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
          ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
        }
        ਪੋਰਟ = ਪੋਰਟ_min;
        non_local = FALSE; /* do not try this again */
        ਜਾਰੀ;
      }
      ਜੇ(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(ਅੰਕੜੇ, "bind(port=%hu) failed: %s", ਪੋਰਟ,
              curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
        ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
      }
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ਜੇ(ਪੋਰਟ == ਪੋਰਟ_max) {
      failf(ਅੰਕੜੇ, "bind() failed, ran out of ports");
      ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
    }
    ਪੋਰਟ++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = ਆਕਾਰ(*ss);
  ਜੇ(getsockname(portsock, sa, sslen_io)) {
    failf(ਅੰਕੜੇ, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
  }
  CURL_TRC_FTP(ਅੰਕੜੇ, "ftp_port_bind_socket(), socket bound to port %d", ਪੋਰਟ);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_listen(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_socket_t portsock)
{
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];

  ਜੇ(listen(portsock, 1)) {
    failf(ਅੰਕੜੇ, "socket failure: %s",
          curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_FTP_ਪੋਰਟ_FAILED;
  }
  CURL_TRC_FTP(ਅੰਕੜੇ, "ftp_port_listen(), listening on port");
  ਵਾਪਸ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ਸਥਿਰ CURLcode ftp_ਪੋਰਟ_send_command(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ftp_conn *ftpc,
                                      ਬਣਤਰ connectdata *conn,
                                      ਬਣਤਰ Curl_sockaddr_storage *ss,
                                      ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਬਹੁਲਕ[][5] = { "EPRT", "PORT" };
  ਬਣਤਰ sockaddr *sa = (ਬਣਤਰ sockaddr *)ss;
  ਬਣਤਰ sockaddr_in * ਅਚਲ sa4 = (ਖਾਲੀ *)sa;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 * ਅਚਲ sa6 = (ਖਾਲੀ *)sa;
#ਜੇ_ਅੰਤ
  ਅੱਖਰ myhost[MAX_IPADR_LEN + 1] = "";
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ;
  CURLcode ਨਤੀਜਾ;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_ਪਤਾ(ai, myhost, ਆਕਾਰ(myhost));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(!conn->ਬਿੱਟ.ftp_use_eprt && conn->ਬਿੱਟ.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->ਬਿੱਟ.ftp_use_eprt = TRUE;
#ਜੇ_ਅੰਤ

  ਲਈ(; fcmd != ਪੂਰਾ_2; fcmd++) {

    ਜੇ(!conn->ਬਿੱਟ.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ਜਾਰੀ;

    ਜੇ((ਪੋਰਟ_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ਜਾਰੀ;

    ਚੋਣ(sa->sa_family) {
    ਹਾਲਤ AF_INET:
      ਪੋਰਟ = ntohs(sa4->sin_port);
      ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਹਾਲਤ AF_INET6:
      ਪੋਰਟ = ntohs(sa6->sin6_port);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
      ਜਾਰੀ; /* might as well skip this */
    }

    ਜੇ(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s |%d|%s|%hu|", ਬਹੁਲਕ[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, ਪੋਰਟ);
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "Failure sending EPRT command: %s",
              curl_easy_strerror(ਨਤੀਜਾ));
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਰੋਕੋ;
    }
    ਜੇ(ਪੋਰਟ_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      ਅੱਖਰ ਨਿਸ਼ਾਨਾ[ਆਕਾਰ(myhost) + 20];
      ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = myhost;
      ਅੱਖਰ *dest = ਨਿਸ਼ਾਨਾ;

      /* translate x.x.x.x to x,x,x,x */
      ਜਦੋਂ(*ਸਰੋਤ) {
        ਜੇ(*ਸਰੋਤ == '.')
          *dest = ',';
        ਨਹੀਂਤਾਂ
          *dest = *ਸਰੋਤ;
        dest++;
        ਸਰੋਤ++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (ਪੂਰਨ)(ਪੋਰਟ >> 8), (ਪੂਰਨ)(ਪੋਰਟ & 0xff));

      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s %s", ਬਹੁਲਕ[fcmd], ਨਿਸ਼ਾਨਾ);
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "Failure sending PORT command: %s",
              curl_easy_strerror(ਨਤੀਜਾ));
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਰੋਕੋ;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(ਅੰਕੜੇ, ftpc, FTP_ਪੋਰਟ);
  ਵਾਪਸ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ਸਥਿਰ CURLcode ftp_state_use_ਪੋਰਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode ਨਤੀਜਾ = CURLE_FTP_ਪੋਰਟ_FAILED;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ਬਣਤਰ Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  ਅੱਖਰ hbuf[NI_MAXHOST];
  ਅਚਲ ਅੱਖਰ *host = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *string_ftpport = CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTPPORT);
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns_ਇੰਦਰਾਜ = ਨਿਰਾ;
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *res = ਨਿਰਾ;
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_min = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  ਨਤੀਜਾ = ftp_ਪੋਰਟ_parse_string(ਅੰਕੜੇ, conn, string_ftpport,
                                 &ss, &ਪੋਰਟ_min, &ਪੋਰਟ_max,
                                 &host, hbuf, ਆਕਾਰ(hbuf));
  ਜੇ(!ਨਤੀਜਾ && !host)
    /* if no host was specified, use the control connection's local IP */
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_default_host(ਅੰਕੜੇ, conn, &ss, &sslen, &host,
                                   hbuf, ਆਕਾਰ(hbuf), &non_local);

  /* resolve host string to address list */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_resolve_host(ਅੰਕੜੇ, conn, host, &dns_ਇੰਦਰਾਜ, &res);

  /* Open a TCP socket for the data connection */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_open_socket(ਅੰਕੜੇ, conn, res, &ai, &portsock);
  ਜੇ(!ਨਤੀਜਾ) {
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_bind_socket(ਅੰਕੜੇ, conn, portsock, ai, &ss, &sslen,
                                  ਪੋਰਟ_min, ਪੋਰਟ_max, non_local);
  }

  /* listen */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_listen(ਅੰਕੜੇ, portsock);

  /* send the PORT / EPRT command */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ftp_ਪੋਰਟ_send_command(ਅੰਕੜੇ, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_tcp_listen_ਸੈੱਟ(ਅੰਕੜੇ, conn, SECONDARYSOCKET, &portsock);

  ਜੇ(!ਨਤੀਜਾ)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ਜੇ(dns_ਇੰਦਰਾਜ)
    Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &dns_ਇੰਦਰਾਜ);
  ਜੇ(ਨਤੀਜਾ) {
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
  }
  ਨਹੀਂਤਾਂ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ਜੇ(conn->ਬਿੱਟ.ftp_use_ਅੰਕੜੇ_ssl && ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_ਪੋਰਟ &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
        ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->ਬਿੱਟ.do_ਹੋਰ = FALSE;
    Curl_expire(ਅੰਕੜੇ, (ਅੰਕੜੇ->ਸੈੱਟ.accepttimeout > 0) ?
                ਅੰਕੜੇ->ਸੈੱਟ.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  ਜੇ(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(ਅੰਕੜੇ, conn, portsock);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_use_pasv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc,
                                   ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਬਹੁਲਕ[][5] = { "EPSV", "PASV" };
  ਪੂਰਨ modeoff;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PF_INET6
  ਜੇ(!conn->ਬਿੱਟ.ftp_use_epsv && conn->ਬਿੱਟ.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->ਬਿੱਟ.ftp_use_epsv = TRUE;
#ਜੇ_ਅੰਤ

  modeoff = conn->ਬਿੱਟ.ftp_use_epsv ? 0 : 1;

  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", ਬਹੁਲਕ[modeoff]);
  ਜੇ(!ਨਤੀਜਾ) {
    ftpc->count1 = modeoff;
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_PASV);
    infof(ਅੰਕੜੇ, "Connect data stream passively");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ਸਥਿਰ CURLcode ftp_state_prepare_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ftp_conn *ftpc,
                                           ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  ਜੇ(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR_PREQUOTE);
    ਨਤੀਜਾ = ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_ਪੋਰਟ) {
    /* We have chosen to use the PORT (or similar) command */
    ਨਤੀਜਾ = ftp_state_use_ਪੋਰਟ(ਅੰਕੜੇ, ftpc, EPRT);
  }
  ਨਹੀਂਤਾਂ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ਜੇ(!ftpc->file)
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) :
                               (ਅੰਕੜੇ->state.list_only ? "NLST" : "LIST"));
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.upload)
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_PRET);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = ftp_state_use_pasv(ਅੰਕੜੇ, ftpc, conn);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_rest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "REST %d", 0);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_REST);
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_prepare_transfer(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "SIZE %s", ftpc->file);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_ਮਾਪ);
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_rest(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  ਅਚਲ ਅੱਖਰ *lstArg = ਨਿਰਾ;
  ਪੂਰਨ lstArglen = 0;
  ਅੱਖਰ *cmd;

  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD) && ftp->ਰਸਤਾ) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ਅਚਲ ਅੱਖਰ *raw_ਰਸਤਾ = ftpc->rawpath;
    ਅਚਲ ਅੱਖਰ *slashPos = strrchr(raw_ਰਸਤਾ, '/');
    ਜੇ(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_ਰਸਤਾ;
      ਜੇ(n == 0)
        ++n;

      lstArg = raw_ਰਸਤਾ;
      lstArglen = (ਪੂਰਨ)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) :
                      (ਅੰਕੜੇ->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ਜੇ(!cmd)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ਜੇ(!ਨਤੀਜਾ)
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_LIST);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_list_prequote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ftp_conn *ftpc,
                                        ਬਣਤਰ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ਵਾਪਸ ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ਸਥਿਰ CURLcode ftp_state_retr_prequote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ftp_conn *ftpc,
                                        ਬਣਤਰ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ਵਾਪਸ ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ਸਥਿਰ CURLcode ftp_state_stor_prequote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ftp_conn *ftpc,
                                        ਬਣਤਰ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ਵਾਪਸ ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ਸਥਿਰ CURLcode ftp_state_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ਜੇ(ਅੰਕੜੇ->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)ਅੰਕੜੇ->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    ਨਤੀਜਾ = ftp_nb_type(ਅੰਕੜੇ, ftpc, ftp, (bool)ਅੰਕੜੇ->state.prefer_ascii,
                         FTP_TYPE);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_ਮਾਪ(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ਸਥਿਰ CURLcode ftp_state_mdtm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.get_filetime || ਅੰਕੜੇ->ਸੈੱਟ.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "MDTM %s", ftpc->file);

    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_MDTM);
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_type(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* This is called after the TYPE and possible quote commands have been sent */
ਸਥਿਰ CURLcode ftp_state_ul_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc,
                                   ਬਣਤਰ FTP *ftp,
                                   bool sizechecked)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_ਦੋਆਧਾਰੀ_ਅੰਕ append = ਅੰਕੜੇ->ਸੈੱਟ.remote_append;

  ਜੇ((ਅੰਕੜੇ->state.resume_from && !sizechecked) ||
     ((ਅੰਕੜੇ->state.resume_from > 0) && sizechecked)) {
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
    ਪੂਰਨ seekerr = CURL_SEEKFUNC_OK;

    ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "SIZE %s", ftpc->file);
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOR_ਮਾਪ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.seek_func) {
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_seek_func);
      seekerr = ਅੰਕੜੇ->ਸੈੱਟ.seek_func(ਅੰਕੜੇ->ਸੈੱਟ.seek_client,
                                    ਅੰਕੜੇ->state.resume_from, SEEK_SET);
      CURL_CBAPI_ਸਿਰਾ(&guard);
    }

    ਜੇ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ਜੇ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ਅੰਕੜੇ, "Could not seek stream");
        ਵਾਪਸ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ਕਰੋ {
        ਅੱਖਰ scratch[4 * 1024];
        size_t readthisamountnow =
          (ਅੰਕੜੇ->state.resume_from - passed > (curl_off_t)ਆਕਾਰ(scratch)) ?
          ਆਕਾਰ(scratch) :
          curlx_sotouz(ਅੰਕੜੇ->state.resume_from - passed);

        size_t actuallyread =
          ਅੰਕੜੇ->state.fread_func(scratch, 1, readthisamountnow,
                                 ਅੰਕੜੇ->state.ਵਿੱਚ);

        passed += actuallyread;
        ਜੇ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ਅੰਕੜੇ, "Failed to read data");
          ਵਾਪਸ CURLE_FTP_COULDNT_USE_REST;
        }
      } ਜਦੋਂ(passed < ਅੰਕੜੇ->state.resume_from);
    }
    /* now, decrease the size of the read */
    ਜੇ(ਅੰਕੜੇ->state.infilesize > 0) {
      ਅੰਕੜੇ->state.infilesize -= ਅੰਕੜੇ->state.resume_from;

      ਜੇ(ਅੰਕੜੇ->state.infilesize <= 0) {
        infof(ਅੰਕੜੇ, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
        ਵਾਪਸ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ਜੇ(!ਨਤੀਜਾ)
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOR);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_retr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ftp_conn *ftpc,
                               ਬਣਤਰ FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize && (filesize > ਅੰਕੜੇ->ਸੈੱਟ.max_filesize)) {
    failf(ਅੰਕੜੇ, "Maximum file size exceeded");
    ਵਾਪਸ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ਜੇ(filesize == -1) {
      infof(ਅੰਕੜੇ, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ਨਹੀਂਤਾਂ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ਜੇ(filesize < -ਅੰਕੜੇ->state.resume_from) {
          failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ਅੰਕੜੇ->state.resume_from, filesize);
          ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -ਅੰਕੜੇ->state.resume_from;
        /* download from where? */
        ਅੰਕੜੇ->state.resume_from = filesize - ftp->downloadsize;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(filesize < ਅੰਕੜੇ->state.resume_from) {
          failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ਅੰਕੜੇ->state.resume_from, filesize);
          ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - ਅੰਕੜੇ->state.resume_from;
      }
    }

    ਜੇ(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
      infof(ਅੰਕੜੇ, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
      ਵਾਪਸ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(ਅੰਕੜੇ, "Instructs server to resume from offset %" FMT_OFF_T,
          ਅੰਕੜੇ->state.resume_from);

    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "REST %" FMT_OFF_T,
                           ਅੰਕੜੇ->state.resume_from);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR_REST);
  }
  ਨਹੀਂਤਾਂ {
    /* no resume */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "RETR %s", ftpc->file);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_quote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ftp_conn *ftpc,
                                ਬਣਤਰ FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool quote = FALSE;
  ਬਣਤਰ curl_slist *ਵਸਤੂ;

  ਚੋਣ(instate) {
  ਹਾਲਤ FTP_QUOTE:
  ਮੂਲ:
    ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.quote;
    ਰੋਕੋ;
  ਹਾਲਤ FTP_RETR_PREQUOTE:
  ਹਾਲਤ FTP_STOR_PREQUOTE:
  ਹਾਲਤ FTP_LIST_PREQUOTE:
    ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.prequote;
    ਰੋਕੋ;
  ਹਾਲਤ FTP_POSTQUOTE:
    ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.postquote;
    ਰੋਕੋ;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ਜੇ(init)
    ftpc->count1 = 0;
  ਨਹੀਂਤਾਂ
    ftpc->count1++;

  ਜੇ(ਵਸਤੂ) {
    ਪੂਰਨ i = 0;

    /* Skip count1 items in the linked list */
    ਜਦੋਂ((i < ftpc->count1) && ਵਸਤੂ) {
      ਵਸਤੂ = ਵਸਤੂ->ਅਗਲਾ;
      i++;
    }
    ਜੇ(ਵਸਤੂ) {
      ਅਚਲ ਅੱਖਰ *cmd = ਵਸਤੂ->ਅੰਕੜੇ;
      ਜੇ(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ਨਹੀਂਤਾਂ
        ftpc->count2 = 0; /* failure means cancel operation */

      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", cmd);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ftp_state(ਅੰਕੜੇ, ftpc, instate);
      quote = TRUE;
    }
  }

  ਜੇ(!quote) {
    /* No more quote to send, continue to ... */
    ਚੋਣ(instate) {
    ਹਾਲਤ FTP_QUOTE:
    ਮੂਲ:
      ਨਤੀਜਾ = ftp_state_cwd(ਅੰਕੜੇ, ftpc, ftp);
      ਰੋਕੋ;
    ਹਾਲਤ FTP_RETR_PREQUOTE:
      ਜੇ(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
      ਨਹੀਂਤਾਂ {
        ਜੇ(ftpc->ਗਿਆਤ_filesize != -1) {
          Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ftpc->ਗਿਆਤ_filesize);
          ਨਤੀਜਾ = ftp_state_retr(ਅੰਕੜੇ, ftpc, ftp, ftpc->ਗਿਆਤ_filesize);
        }
        ਨਹੀਂਤਾਂ {
          ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ignorecl || ਅੰਕੜੇ->state.prefer_ascii) {
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
            ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "RETR %s", ftpc->file);
            ਜੇ(!ਨਤੀਜਾ)
              ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR);
          }
          ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "SIZE %s", ftpc->file);
            ਜੇ(!ਨਤੀਜਾ)
              ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR_ਮਾਪ);
          }
        }
      }
      ਰੋਕੋ;
    ਹਾਲਤ FTP_STOR_PREQUOTE:
      ਨਤੀਜਾ = ftp_state_ul_ਤਿਆਰੀ(ਅੰਕੜੇ, ftpc, ftp, FALSE);
      ਰੋਕੋ;
    ਹਾਲਤ FTP_POSTQUOTE:
      ਰੋਕੋ;
    ਹਾਲਤ FTP_LIST_PREQUOTE:
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_LIST_TYPE);
      ਨਤੀਜਾ = ftp_state_list(ਅੰਕੜੇ, ftpc, ftp);
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ਸਥਿਰ CURLcode ftp_epsv_disable(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ftp_conn *ftpc,
                                 ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(conn->ਬਿੱਟ.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(ਅੰਕੜੇ, "Failed EPSV attempt, exiting");
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  infof(ਅੰਕੜੇ, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->ਬਿੱਟ.ftp_use_epsv = FALSE;
  close_secondarysocket(ਅੰਕੜੇ, ftpc);
  ਅੰਕੜੇ->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "PASV");
  ਜੇ(!ਨਤੀਜਾ) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_PASV);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_control_addr_dup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ **newhostp)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ(!Curl_conn_get_ip_info(ਅੰਕੜੇ, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ਨਹੀਂਤਾਂ {
    /* failed to get the remote_ip of the DATA connection */
    failf(ਅੰਕੜੇ, "unable to get peername of DATA connection");
    *newhostp = ਨਿਰਾ;
    ਵਾਪਸ CURLE_FTP_CANT_GET_HOST;
  }
  ਵਾਪਸ *newhostp ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

ਸਥਿਰ bool ਮੇਲ_pasv_6nums(ਅਚਲ ਅੱਖਰ *p,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *array) /* 6 numbers */
{
  ਪੂਰਨ i;
  ਲਈ(i = 0; i < 6; i++) {
    curl_off_t ਅੰਕ;
    ਜੇ(i) {
      ਜੇ(*p != ',')
        ਵਾਪਸ FALSE;
      p++;
    }
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, 0xff))
      ਵਾਪਸ FALSE;
    array[i] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਅੰਕ;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode ftp_state_pasv_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਪੂਰਨ ftpcode)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਬਣਤਰ pingpong *pp = &ftpc->pp;
  ਅੱਖਰ *newhost = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ newport = 0;
  ਅਚਲ ਅੱਖਰ *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ਜੇ((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ਅਚਲ ਅੱਖਰ *ptr = strchr(str, '(');
    ਜੇ(ptr) {
      ਅੱਖਰ sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ਜੇ(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ਅਚਲ ਅੱਖਰ *p = &ptr[3];
        curl_off_t ਅੰਕ;
        ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, 0xffff) || (*p != sep)) {
          failf(ਅੰਕੜੇ, "Illegal port number in EPSV reply");
          ਵਾਪਸ CURLE_FTP_WEIRD_PASV_ਜਵਾਬ;
        }
        newport = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਅੰਕ;
        ਨਤੀਜਾ = ftp_control_addr_dup(ਅੰਕੜੇ, &newhost);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ
        ptr = ਨਿਰਾ;
    }
    ਜੇ(!ptr) {
      failf(ਅੰਕੜੇ, "Weirdly formatted EPSV reply");
      ਵਾਪਸ CURLE_FTP_WEIRD_PASV_ਜਵਾਬ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ਜਦੋਂ(*str) {
      ਜੇ(ਮੇਲ_pasv_6nums(str, ip))
        ਰੋਕੋ;
      str++;
    }

    ਜੇ(!*str) {
      failf(ਅੰਕੜੇ, "Could not interpret the 227-response");
      ਵਾਪਸ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(ਅੰਕੜੇ, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      ਨਤੀਜਾ = ftp_control_addr_dup(ਅੰਕੜੇ, &newhost);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ਜੇ(!newhost)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    newport = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ਵਾਪਸ ftp_epsv_disable(ਅੰਕੜੇ, ftpc, conn);
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Bad PASV/EPSV response: %03d", ftpcode);
    ਵਾਪਸ CURLE_FTP_WEIRD_PASV_ਜਵਾਬ;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  ਨਤੀਜਾ = Curl_peer_create(ਅੰਕੜੇ, conn->scheme, newhost, newport,
                            &conn->origin2);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ਜੇ(conn->ਰਾਹੀਂ_peer) {
    Curl_peer_unlink(&conn->ਰਾਹੀਂ_peer2);
    ਨਤੀਜਾ = Curl_peer_create(ਅੰਕੜੇ, conn->ਰਾਹੀਂ_peer->scheme,
                              conn->ਰਾਹੀਂ_peer->hostname, newport,
                              &conn->ਰਾਹੀਂ_peer2);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
  }

  ਨਤੀਜਾ = Curl_conn_ਤਿਆਰੀ(ਅੰਕੜੇ, conn, SECONDARYSOCKET,
                           conn->ਬਿੱਟ.ftp_use_ਅੰਕੜੇ_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ((ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      ਨਤੀਜਾ = ftp_epsv_disable(ਅੰਕੜੇ, ftpc, conn);
    }
    ਜਾਓ error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(ਅੰਕੜੇ, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->ਪੋਰਟ);
  }
#ਜੇ_ਅੰਤ

  conn->ਬਿੱਟ.do_ਹੋਰ = TRUE;
  ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode ftp_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ftp_conn *ftpc,
                              bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *ਪੂਰਾ = (ftpc->state == FTP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_do_ਹੋਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, domore *ਹੋਰ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਬਣਤਰ FTP *ftp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FTP_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ਜੇ(!ftpc || !ftp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  *ਹੋਰ = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ਜੇ(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(ਅੰਕੜੇ, SECONDARYSOCKET);
    ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, SECONDARYSOCKET, FALSE, &connected);
    ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(ਨਤੀਜਾ || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(ਅੰਕੜੇ, SECONDARYSOCKET))) {
      ਜੇ(ਨਤੀਜਾ && !is_eptr && (ftpc->count1 == 0)) {
        *ਹੋਰ = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ਵਾਪਸ ftp_epsv_disable(ਅੰਕੜੇ, ftpc, conn);
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜੇ(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, &complete);

    ਜੇ(complete)
      *ਹੋਰ = DOMORE_ਪੂਰਾ;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ਜੇ(ਨਤੀਜਾ || !ftpc->wait_ਅੰਕੜੇ_conn)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *ਹੋਰ = DOMORE_INCOMPLETE;
  }

  ਜੇ(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ਜੇ(ftpc->wait_ਅੰਕੜੇ_conn) {
      bool serv_conned;

      ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, SECONDARYSOCKET, FALSE, &serv_conned);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ; /* Failed to accept data connection */

      ਜੇ(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_ਅੰਕੜੇ_conn = FALSE;
        ਨਤੀਜਾ = ftp_initiate_transfer(ਅੰਕੜੇ, ftpc);

        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        *ਹੋਰ = DOMORE_ਪੂਰਾ; /* this state is now complete when the server has
                                connected back to us */
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = ftp_check_ctrl_on_ਅੰਕੜੇ_wait(ਅੰਕੜੇ, ftpc);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.upload) {
      ਨਤੀਜਾ = ftp_nb_type(ਅੰਕੜੇ, ftpc, ftp, (bool)ਅੰਕੜੇ->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *ਹੋਰ = (!ftpc->wait_ਅੰਕੜੇ_conn && complete) ?
        DOMORE_ਪੂਰਾ : DOMORE_INCOMPLETE;
    }
    ਨਹੀਂਤਾਂ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      ਨਤੀਜਾ = Curl_range(ਅੰਕੜੇ);

      ਜੇ(ਨਤੀਜਾ == CURLE_OK && ਅੰਕੜੇ->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਜੇ((ਅੰਕੜੇ->state.list_only || !ftpc->file) &&
         !ਅੰਕੜੇ->ਸੈੱਟ.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ਜੇ(ftp->transfer == PPTRANSFER_BODY) {
          ਨਤੀਜਾ = ftp_nb_type(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        /* otherwise fall through */
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.prequote && !ftpc->file) {
          ਨਤੀਜਾ = ftp_nb_type(ਅੰਕੜੇ, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ਨਹੀਂਤਾਂ {
          ਨਤੀਜਾ = ftp_nb_type(ਅੰਕੜੇ, ftpc, ftp, (bool)ਅੰਕੜੇ->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }

      ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, &complete);
      ਜੇ(complete)
        *ਹੋਰ = DOMORE_ਪੂਰਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* no data to transfer */
  Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);

  ਜੇ(!ftpc->wait_ਅੰਕੜੇ_conn) {
    /* no waiting for the data connection so this is now complete */
    *ਹੋਰ = DOMORE_ਪੂਰਾ;
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (ਪੂਰਨ)ਨਤੀਜਾ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* call this when the DO phase has completed */
ਸਥਿਰ CURLcode ftp_dophase_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ftp_conn *ftpc,
                                 ਬਣਤਰ FTP *ftp,
                                 bool connected)
{
  ਜੇ(connected) {
    domore completed;
    CURLcode ਨਤੀਜਾ = ftp_do_ਹੋਰ(ਅੰਕੜੇ, &completed);

    ਜੇ(ਨਤੀਜਾ) {
      close_secondarysocket(ਅੰਕੜੇ, ftpc);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜੇ(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
  ਨਹੀਂਤਾਂ ਜੇ(!connected)
    /* since we did not connect now, we want do_more to get called */
    ਅੰਕੜੇ->conn->ਬਿੱਟ.do_ਹੋਰ = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ftp_state_ਪੋਰਟ_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਬਣਤਰ FTP *ftp,
                                    ਪੂਰਨ ftpcode)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ਜੇ(ftpcode / 100 != 2) {
    /* the command failed */

    ਜੇ(EPRT == fcmd) {
      infof(ਅੰਕੜੇ, "disabling EPRT usage");
      conn->ਬਿੱਟ.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ਜੇ(fcmd == ਪੂਰਾ_2) {
      failf(ਅੰਕੜੇ, "Failed to do PORT");
      ਨਤੀਜਾ = CURLE_FTP_ਪੋਰਟ_FAILED;
    }
    ਨਹੀਂਤਾਂ
      /* try next */
      ਨਤੀਜਾ = ftp_state_use_ਪੋਰਟ(ਅੰਕੜੇ, ftpc, fcmd);
  }
  ਨਹੀਂਤਾਂ {
    infof(ਅੰਕੜੇ, "Connect data stream actively");
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* end of DO phase */
    ਨਤੀਜਾ = ftp_dophase_ਪੂਰਾ(ਅੰਕੜੇ, ftpc, ftp, FALSE);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* return TRUE on error, FALSE on success */
ਸਥਿਰ bool twodigit(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ *val)
{
  ਜੇ(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ਵਾਪਸ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ਵਾਪਸ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ *year, ਪੂਰਨ *month, ਪੂਰਨ *day,
                           ਪੂਰਨ *hour, ਪੂਰਨ *minute, ਪੂਰਨ *ਦੂਜਾ);
UNITTEST bool ftp_213_date(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ *year, ਪੂਰਨ *month, ਪੂਰਨ *day,
                           ਪੂਰਨ *hour, ਪੂਰਨ *minute, ਪੂਰਨ *ਦੂਜਾ)
{
  ਪੂਰਨ century;
  ਜੇ((ਲੜੀ_ਲੰਬਾਈ(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], ਦੂਜਾ))
    ਵਾਪਸ FALSE;

  *year += century * 100;
  ਜੇ((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*ਦੂਜਾ > 60))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode client_write_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅੱਖਰ *buf, size_t blen)
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
  CURLcode ਨਤੀਜਾ;
  bool save = (bool)ਅੰਕੜੇ->ਸੈੱਟ.include_ਸਿਰਲੇਖ;
  ਅੰਕੜੇ->ਸੈੱਟ.include_ਸਿਰਲੇਖ = TRUE;
  ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, buf, blen);
  ਅੰਕੜੇ->ਸੈੱਟ.include_ਸਿਰਲੇਖ = save;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_mdtm_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਬਣਤਰ FTP *ftp,
                                    ਪੂਰਨ ftpcode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(ftpcode) {
  ਹਾਲਤ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    ਪੂਰਨ year, month, day, hour, minute, ਦੂਜਾ;
    ਬਣਤਰ pingpong *pp = &ftpc->pp;
    ਅਚਲ ਅੱਖਰ *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ਜੇ(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &ਦੂਜਾ)) {
      /* we have a time, reformat it */
      ਅੱਖਰ timebuf[24];
      curl_msnprintf(timebuf, ਆਕਾਰ(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, ਦੂਜਾ);
      /* now, convert this into a time() value: */
      ਜੇ(!Curl_getdate_capped(timebuf, &ਅੰਕੜੇ->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ਜੇ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#ਹਦਾਇਤ GCC diagnostic ignored "-Wtype-limits"
#ਜੇ_ਅੰਤ
    ਜੇ(ਅੰਕੜੇ->req.no_body && ftpc->file &&
       ਅੰਕੜੇ->ਸੈੱਟ.get_filetime && showtime) {
#ਜੇ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
      ਅੱਖਰ headerbuf[128];
      ਪੂਰਨ headerbuflen;
      time_t filetime = ਅੰਕੜੇ->info.filetime;
      ਬਣਤਰ tm ਬਫਰ;
      ਅਚਲ ਬਣਤਰ tm *tm = &ਬਫਰ;

      ਨਤੀਜਾ = curlx_gmtime(filetime, &ਬਫਰ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, ਆਕਾਰ(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                       tm->tm_mday,
                       Curl_month[tm->tm_mon],
                       tm->tm_year + 1900,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec);
      ਨਤੀਜਾ = client_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, headerbuf, headerbuflen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    } /* end of a ridiculous amount of conditionals */
  }
    ਰੋਕੋ;
  ਮੂਲ:
    infof(ਅੰਕੜੇ, "unsupported MDTM reply format");
    ਰੋਕੋ;
  ਹਾਲਤ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(ਅੰਕੜੇ, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ਰੋਕੋ;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition) {
    ਜੇ((ਅੰਕੜੇ->info.filetime > 0) && (ਅੰਕੜੇ->ਸੈੱਟ.timevalue > 0)) {
      ਚੋਣ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition) {
      ਹਾਲਤ CURL_TIMECOND_IFMODSINCE:
      ਮੂਲ:
        ਜੇ(ਅੰਕੜੇ->info.filetime <= ਅੰਕੜੇ->ਸੈੱਟ.timevalue) {
          infof(ਅੰਕੜੇ, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ਅੰਕੜੇ->info.timecond = TRUE;
          ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
          ਵਾਪਸ CURLE_OK;
        }
        ਰੋਕੋ;
      ਹਾਲਤ CURL_TIMECOND_IFUNMODSINCE:
        ਜੇ(ਅੰਕੜੇ->info.filetime > ਅੰਕੜੇ->ਸੈੱਟ.timevalue) {
          infof(ਅੰਕੜੇ, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ਅੰਕੜੇ->info.timecond = TRUE;
          ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
          ਵਾਪਸ CURLE_OK;
        }
        ਰੋਕੋ;
      } /* switch */
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "Skipping time comparison");
    }
  }

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ftp_state_type(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_type_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਬਣਤਰ FTP *ftp,
                                    ਪੂਰਨ ftpcode,
                                    ftpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(ਅੰਕੜੇ, "Could not set desired mode");
    ਵਾਪਸ CURLE_FTP_COULDNT_ਸੈੱਟ_TYPE;
  }
  ਜੇ(ftpcode != 200)
    infof(ਅੰਕੜੇ, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ਜੇ(instate == FTP_TYPE)
    ਨਤੀਜਾ = ftp_state_ਮਾਪ(ਅੰਕੜੇ, ftpc, ftp);
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_LIST_TYPE)
    ਨਤੀਜਾ = ftp_state_list(ਅੰਕੜੇ, ftpc, ftp);
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_RETR_TYPE)
    ਨਤੀਜਾ = ftp_state_retr_prequote(ਅੰਕੜੇ, ftpc, ftp);
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_STOR_TYPE)
    ਨਤੀਜਾ = ftp_state_stor_prequote(ਅੰਕੜੇ, ftpc, ftp);
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_RETR_LIST_TYPE)
    ਨਤੀਜਾ = ftp_state_list_prequote(ਅੰਕੜੇ, ftpc, ftp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_ਮਾਪ_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਬਣਤਰ FTP *ftp,
                                    ਪੂਰਨ ftpcode,
                                    ftpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_off_t filesize = -1;
  ਅਚਲ ਅੱਖਰ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ਜੇ(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ = &buf[4];
    ਅਚਲ ਅੱਖਰ *fdigit = memchr(ਸ਼ੁਰੂ, '\r', len - 4);
    ਜੇ(fdigit) {
      fdigit--;
      ਜੇ(*fdigit == '\n')
        fdigit--;
      ਜਦੋਂ(ISDIGIT(fdigit[-1]) && (fdigit > ਸ਼ੁਰੂ))
        fdigit--;
    }
    ਨਹੀਂਤਾਂ
      fdigit = ਸ਼ੁਰੂ;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ਨਹੀਂਤਾਂ ਜੇ(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_ਮਾਪ) {
    failf(ਅੰਕੜੇ, "The file does not exist");
    ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;
  }

  ਜੇ(instate == FTP_ਮਾਪ) {
    ਜੇ(filesize != -1) {
      ਅੱਖਰ clbuf[128];
      ਪੂਰਨ clbuflen = curl_msnprintf(clbuf, ਆਕਾਰ(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      ਨਤੀਜਾ = client_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, clbuf, clbuflen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, filesize);
    ਨਤੀਜਾ = ftp_state_rest(ਅੰਕੜੇ, ftpc, ftp);
  }
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_RETR_ਮਾਪ) {
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, filesize);
    ਨਤੀਜਾ = ftp_state_retr(ਅੰਕੜੇ, ftpc, ftp, filesize);
  }
  ਨਹੀਂਤਾਂ ਜੇ(instate == FTP_STOR_ਮਾਪ) {
    ਅੰਕੜੇ->state.resume_from = filesize;
    ਨਤੀਜਾ = ftp_state_ul_ਤਿਆਰੀ(ਅੰਕੜੇ, ftpc, ftp, TRUE);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_rest_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਬਣਤਰ FTP *ftp,
                                    ਪੂਰਨ ftpcode,
                                    ftpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(instate) {
  ਹਾਲਤ FTP_REST:
  ਮੂਲ:
    ਜੇ(ftpcode == 350) {
      ਅੱਖਰ ਬਫਰ[24] = { "Accept-ranges: bytes\r\n" };
      ਨਤੀਜਾ = client_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, ਬਫਰ, ਲੜੀ_ਲੰਬਾਈ(ਬਫਰ));
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਤੀਜਾ = ftp_state_prepare_transfer(ਅੰਕੜੇ, ftpc, ftp);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_RETR_REST:
    ਜੇ(ftpcode != 350) {
      failf(ਅੰਕੜੇ, "Could not use REST");
      ਨਤੀਜਾ = CURLE_FTP_COULDNT_USE_REST;
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "RETR %s", ftpc->file);
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_RETR);
    }
    ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_state_stor_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਪੂਰਨ ftpcode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ftpcode >= 400) {
    failf(ਅੰਕੜੇ, "Failed FTP upload: %0d", ftpcode);
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
    ਵਾਪਸ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_ਪੋਰਟ) {
    bool connected;

    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* no longer in STOR state */

    ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, SECONDARYSOCKET, FALSE, &connected);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ(!connected) {
      infof(ਅੰਕੜੇ, "Data conn was not available immediately");
      ftpc->wait_ਅੰਕੜੇ_conn = TRUE;
      ਵਾਪਸ ftp_check_ctrl_on_ਅੰਕੜੇ_wait(ਅੰਕੜੇ, ftpc);
    }
    ftpc->wait_ਅੰਕੜੇ_conn = FALSE;
  }
  ਵਾਪਸ ftp_initiate_transfer(ਅੰਕੜੇ, ftpc);
}

/* for LIST and RETR responses */
ਸਥਿਰ CURLcode ftp_state_get_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc,
                                   ਬਣਤਰ FTP *ftp,
                                   ਪੂਰਨ ftpcode,
                                   ftpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((ftpcode == 150) || (ftpcode == 125)) {

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

    ਅੰਕੜੇ->req.ਮਾਪ = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    ਜੇ((instate != FTP_LIST) &&
       !ਅੰਕੜੇ->state.prefer_ascii &&
       !ਅੰਕੜੇ->ਸੈੱਟ.ignorecl &&
       (ftp->downloadsize < 1)) {
      /*
       * It seems directory listings either do not show the size or often uses
       * size 0 anyway. ASCII transfers may cause that the transferred amount
       * of data is not the same as this line tells, why using this number in
       * those cases only confuses us.
       *
       * Example D above makes this parsing a little tricky */
      size_t len = curlx_dyn_len(&ftpc->pp.recvbuf);
      ਜੇ(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        ਲਈ(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ਅਚਲ ਅੱਖਰ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ਅਚਲ ਅੱਖਰ *c = &buf[i];
          ਜੇ(!curlx_str_ਸੰਖਿਆ(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !ਲੜੀ_ਤੁਲਨਾ(c, "bytes", 5)) {
            ਅੰਕੜੇ->req.ਮਾਪ = what;
            ਰੋਕੋ;
          }
        }
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ftp->downloadsize > -1)
      ਅੰਕੜੇ->req.ਮਾਪ = ftp->downloadsize;

    ਜੇ(ਅੰਕੜੇ->req.ਮਾਪ > ਅੰਕੜੇ->req.maxdownload && ਅੰਕੜੇ->req.maxdownload > 0)
      ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->req.maxdownload;
    ਨਹੀਂਤਾਂ ਜੇ((instate != FTP_LIST) && ਅੰਕੜੇ->state.prefer_ascii)
      ਅੰਕੜੇ->req.ਮਾਪ = -1; /* for servers that understate ASCII mode file
ਮਾਪ*/

    infof(ਅੰਕੜੇ, "Maxdownload = %" FMT_OFF_T, ਅੰਕੜੇ->req.maxdownload);

    ਜੇ(instate != FTP_LIST)
      infof(ਅੰਕੜੇ, "Getting file with size: %" FMT_OFF_T, ਅੰਕੜੇ->req.ਮਾਪ);

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_ਪੋਰਟ) {
      bool connected;

      ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, SECONDARYSOCKET, FALSE, &connected);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਜੇ(!connected) {
        infof(ਅੰਕੜੇ, "Data conn was not available immediately");
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
        ftpc->wait_ਅੰਕੜੇ_conn = TRUE;
        ਵਾਪਸ ftp_check_ctrl_on_ਅੰਕੜੇ_wait(ਅੰਕੜੇ, ftpc);
      }
      ftpc->wait_ਅੰਕੜੇ_conn = FALSE;
    }
    ਵਾਪਸ ftp_initiate_transfer(ਅੰਕੜੇ, ftpc);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* this phase is over */
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "RETR response: %03d", ftpcode);
      ਵਾਪਸ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* after USER, PASS and ACCT */
ਸਥਿਰ CURLcode ftp_state_loggedin(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_conn *ftpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
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
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PBSZ %d", 0);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_PBSZ);
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = ftp_state_pwd(ਅੰਕੜੇ, ftpc);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ਸਥਿਰ bool ftp_has_ctrl(ਅਚਲ ਅੱਖਰ *string)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *s = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)string;
  ਜਦੋਂ(*s) {
    ਜੇ(*s < 0x20)
      ਵਾਪਸ TRUE;
    s++;
  }
  ਵਾਪਸ FALSE;
}

/* for USER and PASS responses */
ਸਥਿਰ CURLcode ftp_state_user_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਪੂਰਨ ftpcode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ਜੇ((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(ਅੰਕੜੇ->conn->creds));
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_PASS);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    ਨਤੀਜਾ = ftp_state_loggedin(ਅੰਕੜੇ, ftpc);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ftpcode == 332) {
    ਅਚਲ ਅੱਖਰ *account = CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ACCOUNT);
    ਜੇ(!account) {
      failf(ਅੰਕੜੇ, "ACCT requested but none available");
      ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ftp_has_ctrl(account)) {
      failf(ਅੰਕੜੇ, "Control byte in FTP account");
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "ACCT %s", account);
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_ACCT);
    }
  }
  ਨਹੀਂਤਾਂ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ਅਚਲ ਅੱਖਰ *alt = CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER);
    ਜੇ(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ਜੇ(ftp_has_ctrl(alt)) {
        failf(ਅੰਕੜੇ, "Control byte in FTP alternative-to-user command");
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", alt);
        ਜੇ(!ਨਤੀਜਾ) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(ਅੰਕੜੇ, ftpc, FTP_USER);
        }
      }
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "Access denied: %03d", ftpcode);
      ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* for ACCT response */
ਸਥਿਰ CURLcode ftp_state_acct_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc,
                                    ਪੂਰਨ ftpcode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(ftpcode != 230) {
    failf(ਅੰਕੜੇ, "ACCT rejected by server: %03d", ftpcode);
    ਨਤੀਜਾ = CURLE_FTP_WEIRD_PASS_ਜਵਾਬ; /* FIX */
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_loggedin(ਅੰਕੜੇ, ftpc);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_pwd_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ftp_conn *ftpc,
                             ਪੂਰਨ ftpcode)
{
  ਬਣਤਰ pingpong *pp = &ftpc->pp;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(ftpcode == 257) {
    ਅਚਲ ਅੱਖਰ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool ਇੰਦਰਾਜ_extracted = FALSE;
    ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
    curlx_dyn_init(&ਬਾਹਰਲਾ, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ਜਦੋਂ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ਜੇ('\"' == *ptr) {
      /* it started good */
      ਲਈ(ptr++; *ptr; ptr++) {
        ਜੇ('\"' == *ptr) {
          ਜੇ('\"' == ptr[1]) {
            /* "quote-doubling" */
            ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, &ptr[1], 1);
            ptr++;
          }
          ਨਹੀਂਤਾਂ {
            /* end of path */
            ਜੇ(curlx_dyn_len(&ਬਾਹਰਲਾ))
              ਇੰਦਰਾਜ_extracted = TRUE;
            ਰੋਕੋ; /* get out of this loop */
          }
        }
        ਨਹੀਂਤਾਂ {
          ਜੇ(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&ਬਾਹਰਲਾ);
            ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
          }
          ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, ptr, 1);
        }
        ਜੇ(ਨਤੀਜਾ) {
          curlx_dyn_free(&ਬਾਹਰਲਾ);
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
    }
    ਜੇ(ਇੰਦਰਾਜ_extracted) {
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
      ਅੱਖਰ *dir = curlx_dyn_ptr(&ਬਾਹਰਲਾ);

      ਜੇ(!ftpc->server_os && dir[0] != '/') {
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "SYST");
        ਜੇ(ਨਤੀਜਾ) {
          curlx_dyn_free(&ਬਾਹਰਲਾ);
          ਵਾਪਸ ਨਤੀਜਾ;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(ਅੰਕੜੇ, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(ਅੰਕੜੇ->state.most_recent_ftp_entrypath);
      ਅੰਕੜੇ->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ਜੇ(!ਅੰਕੜੇ->state.most_recent_ftp_entrypath)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ਜੇ(!ftpc->server_os && dir[0] != '/') {
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_SYST);
        ਵਾਪਸ CURLE_OK;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* could not get the path */
      curlx_dyn_free(&ਬਾਹਰਲਾ);
      infof(ਅੰਕੜੇ, "Failed to figure out path");
    }
  }
  ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ftpauth[] = { "SSL", "TLS" };

ਸਥਿਰ CURLcode ftp_wait_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn,
                              ਬਣਤਰ ftp_conn *ftpc,
                              ਪੂਰਨ ftpcode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ਜੇ(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ਵਾਪਸ ftp_state_user_resp(ਅੰਕੜੇ, ftpc, ftpcode);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ftpcode != 220) {
    failf(ਅੰਕੜੇ, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਜੇ(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ਚੋਣ((ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.ftpsslauth) {
    ਹਾਲਤ CURLFTPAUTH_DEFAULT:
    ਹਾਲਤ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ਰੋਕੋ;
    ਹਾਲਤ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ਰੋਕੋ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (ਪੂਰਨ)ਅੰਕੜੇ->ਸੈੱਟ.ftpsslauth);
      ਵਾਪਸ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_AUTH);
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ftp_state_user(ਅੰਕੜੇ, ftpc, conn);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* an implicit TLS scheme implies TLS on both connections */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ftp_use_ssl(ਬਣਤਰ connectdata *conn,
                                 ਬਣਤਰ ftp_conn *ftpc)
{
  ਜੇ(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ਵਾਪਸ CURLUSESSL_ALL;
  ਵਾਪਸ ftpc->use_ssl;
}

ਸਥਿਰ CURLcode ftp_pp_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ftpcode;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ਬਣਤਰ FTP *ftp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FTP_EASY);
  ਬਣਤਰ pingpong *pp;
  size_t nread = 0;

  ਜੇ(!ftpc || !ftp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ਜੇ(pp->sendleft)
    ਵਾਪਸ Curl_pp_flushsend(ਅੰਕੜੇ, pp);

  ਨਤੀਜਾ = ftp_readresp(ਅੰਕੜੇ, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ਜੇ(ਨਤੀਜਾ || !ftpcode)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* we have now received a full FTP server response */
  ਚੋਣ(ftpc->state) {
  ਹਾਲਤ FTP_WAIT220:
    ਨਤੀਜਾ = ftp_wait_resp(ਅੰਕੜੇ, conn, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ਜੇ(pp->ਉਛਾਲ)
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ਜੇ((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool ਪੂਰਾ;
      ਜੇ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
          ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ਜੇ(ਨਤੀਜਾ) {
          /* we failed and bail out */
          ਵਾਪਸ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, TRUE, &ਪੂਰਾ);
      ਜੇ(!ਨਤੀਜਾ) {
        conn->ਬਿੱਟ.ftp_use_ਅੰਕੜੇ_ssl = FALSE; /* clear-text data */
        ਨਤੀਜਾ = ftp_state_user(ਅੰਕੜੇ, ftpc, conn);
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
      ਨਹੀਂਤਾਂ
        /* ignore the failure and continue */
        ਨਤੀਜਾ = ftp_state_user(ਅੰਕੜੇ, ftpc, conn);
    }
    ਰੋਕੋ;

  ਹਾਲਤ FTP_USER:
  ਹਾਲਤ FTP_PASS:
    ਨਤੀਜਾ = ftp_state_user_resp(ਅੰਕੜੇ, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_ACCT:
    ਨਤੀਜਾ = ftp_state_acct_resp(ਅੰਕੜੇ, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_PBSZ:
    ਨਤੀਜਾ =
      Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ਜੇ(!ਨਤੀਜਾ)
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_PROT);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_PROT:
    ਜੇ(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->ਬਿੱਟ.ftp_use_ਅੰਕੜੇ_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ਨਹੀਂਤਾਂ ਜੇ(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ਵਾਪਸ CURLE_USE_SSL_FAILED;

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "CCC");
      ਜੇ(!ਨਤੀਜਾ)
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_CCC);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = ftp_state_pwd(ਅੰਕੜੇ, ftpc);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_CCC:
    ਜੇ(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      ਨਤੀਜਾ = Curl_ssl_cfilter_remove(ਅੰਕੜੇ, FIRSTSOCKET,
                                       (ਅੰਕੜੇ->ਸੈੱਟ.ftp_ccc ==
                                        (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)CURLFTPSSL_CCC_ਸਰਗਰਮ));
      ਜੇ(ਨਤੀਜਾ)
        failf(ਅੰਕੜੇ, "Failed to clear the command channel (CCC)");
    }
    ਜੇ(!ਨਤੀਜਾ)
      /* Then continue as normal */
      ਨਤੀਜਾ = ftp_state_pwd(ਅੰਕੜੇ, ftpc);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_PWD:
    ਨਤੀਜਾ = ftp_pwd_resp(ਅੰਕੜੇ, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_SYST:
    ਜੇ(ftpcode == 215) {
      ਅਚਲ ਅੱਖਰ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ;
      ਅੱਖਰ *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ਜਦੋਂ(*ptr == ' ')
        ptr++;
      ਲਈ(ਸ਼ੁਰੂ = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(ਸ਼ੁਰੂ, ptr - ਸ਼ੁਰੂ);
      ਜੇ(!os)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      /* Check for special servers here. */
      ਜੇ(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ਜੇ(ਨਤੀਜਾ) {
          curlx_free(os);
          ਵਾਪਸ ਨਤੀਜਾ;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(ਅੰਕੜੇ, ftpc, FTP_NAMEFMT);
        ਰੋਕੋ;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ਨਹੀਂਤਾਂ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ਰੋਕੋ;

  ਹਾਲਤ FTP_NAMEFMT:
    ਜੇ(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(ਅੰਕੜੇ, ftpc);
      ਰੋਕੋ;
    }

    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ਰੋਕੋ;

  ਹਾਲਤ FTP_QUOTE:
  ਹਾਲਤ FTP_POSTQUOTE:
  ਹਾਲਤ FTP_RETR_PREQUOTE:
  ਹਾਲਤ FTP_STOR_PREQUOTE:
  ਹਾਲਤ FTP_LIST_PREQUOTE:
    ਜੇ((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(ਅੰਕੜੇ, "QUOT command failed with %03d", ftpcode);
      ਨਤੀਜਾ = CURLE_QUOTE_ERROR;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, FALSE, ftpc->state);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_CWD:
    ਜੇ(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (ਅੰਕੜੇ->ਸੈੱਟ.ftp_create_missing_dirs == 2) ? 1 : 0;

        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        ਜੇ(!ਨਤੀਜਾ)
          ftp_state(ਅੰਕੜੇ, ftpc, FTP_MKD);
      }
      ਨਹੀਂਤਾਂ {
        /* return failure */
        failf(ਅੰਕੜੇ, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* success */
      ftpc->count2 = 0;
      ਜੇ(ftpc->cwdcount >= ftpc->dirdepth)
        ਨਤੀਜਾ = ftp_state_mdtm(ਅੰਕੜੇ, ftpc, ftp);
      ਨਹੀਂਤਾਂ {
        ftpc->cwdcount++;
        /* send next CWD */
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ਰੋਕੋ;

  ਹਾਲਤ FTP_MKD:
    ਜੇ((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(ਅੰਕੜੇ, "Failed to MKD dir: %03d", ftpcode);
      ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
    }
    ਨਹੀਂਤਾਂ {
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_CWD);
      /* send CWD */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ਰੋਕੋ;

  ਹਾਲਤ FTP_MDTM:
    ਨਤੀਜਾ = ftp_state_mdtm_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_TYPE:
  ਹਾਲਤ FTP_LIST_TYPE:
  ਹਾਲਤ FTP_RETR_TYPE:
  ਹਾਲਤ FTP_STOR_TYPE:
  ਹਾਲਤ FTP_RETR_LIST_TYPE:
    ਨਤੀਜਾ = ftp_state_type_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode, ftpc->state);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_ਮਾਪ:
  ਹਾਲਤ FTP_RETR_ਮਾਪ:
  ਹਾਲਤ FTP_STOR_ਮਾਪ:
    ਨਤੀਜਾ = ftp_state_ਮਾਪ_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode, ftpc->state);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_REST:
  ਹਾਲਤ FTP_RETR_REST:
    ਨਤੀਜਾ = ftp_state_rest_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode, ftpc->state);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_PRET:
    ਜੇ(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(ਅੰਕੜੇ, "PRET command not accepted: %03d", ftpcode);
      ਵਾਪਸ CURLE_FTP_PRET_FAILED;
    }
    ਨਤੀਜਾ = ftp_state_use_pasv(ਅੰਕੜੇ, ftpc, conn);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_PASV:
    ਨਤੀਜਾ = ftp_state_pasv_resp(ਅੰਕੜੇ, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_ਪੋਰਟ:
    ਨਤੀਜਾ = ftp_state_ਪੋਰਟ_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_LIST:
  ਹਾਲਤ FTP_RETR:
    ਨਤੀਜਾ = ftp_state_get_resp(ਅੰਕੜੇ, ftpc, ftp, ftpcode, ftpc->state);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_STOR:
    ਨਤੀਜਾ = ftp_state_stor_resp(ਅੰਕੜੇ, ftpc, ftpcode);
    ਰੋਕੋ;

  ਹਾਲਤ FTP_QUIT:
  ਮੂਲ:
    /* internal error */
    ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
    ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode ftp_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਵਾਪਸ ftpc ? ftp_statemach(ਅੰਕੜੇ, ftpc, ਪੂਰਾ) : CURLE_FAILED_INIT;
}

ਸਥਿਰ CURLcode ftp_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_conn *ftpc)
{
  ਬਣਤਰ pingpong *pp = &ftpc->pp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(ftpc->state != FTP_STOP) {
    ਜੇ(ftpc->shutdown)
      CURL_TRC_FTP(ਅੰਕੜੇ, "in shutdown, waiting for server response");
    ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, pp, TRUE, TRUE /* disconnecting */);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ਸਥਿਰ CURLcode ftp_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool *ਪੂਰਾ) /* see description above */
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਬਣਤਰ pingpong *pp;

  *ਪੂਰਾ = FALSE; /* default to not done yet */
  ਜੇ(!ftpc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_ਤਿਆਰੀ(pp, ftp_pp_statemachine, ftp_endofresp);

  ਜੇ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, TRUE, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  Curl_pp_init(pp, Curl_pgrs_now(ਅੰਕੜੇ)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(ਅੰਕੜੇ, ftpc, FTP_WAIT220);

  ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_sendquote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ftp_conn *ftpc,
                              ਬਣਤਰ curl_slist *quote)
{
  ਬਣਤਰ curl_slist *ਵਸਤੂ;
  ਬਣਤਰ pingpong *pp = &ftpc->pp;

  ਵਸਤੂ = quote;
  ਜਦੋਂ(ਵਸਤੂ) {
    ਜੇ(ਵਸਤੂ->ਅੰਕੜੇ) {
      size_t nread;
      ਅਚਲ ਅੱਖਰ *cmd = ਵਸਤੂ->ਅੰਕੜੇ;
      bool acceptfail = FALSE;
      CURLcode ਨਤੀਜਾ;
      ਪੂਰਨ ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ਜੇ(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", cmd);
      ਜੇ(!ਨਤੀਜਾ) {
        pp->response = *Curl_pgrs_now(ਅੰਕੜੇ); /* timeout relative now */
        ਨਤੀਜਾ = getftpresponse(ਅੰਕੜੇ, &nread, &ftpcode);
      }
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਜੇ(!acceptfail && (ftpcode >= 400)) {
        failf(ਅੰਕੜੇ, "QUOT string not accepted: %s", cmd);
        ਵਾਪਸ CURLE_QUOTE_ERROR;
      }
    }

    ਵਸਤੂ = ਵਸਤੂ->ਅਗਲਾ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ftp_ਪੂਰਾ_ਸਥਿਤੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ftp_conn *ftpc, CURLcode ਸਥਿਤੀ,
                                bool premature)
{
  ਚੋਣ(ਸਥਿਤੀ) {
  ਹਾਲਤ CURLE_BAD_DOWNLOAD_RESUME:
  ਹਾਲਤ CURLE_FTP_WEIRD_PASV_ਜਵਾਬ:
  ਹਾਲਤ CURLE_FTP_ਪੋਰਟ_FAILED:
  ਹਾਲਤ CURLE_FTP_ACCEPT_FAILED:
  ਹਾਲਤ CURLE_FTP_ACCEPT_TIMEOUT:
  ਹਾਲਤ CURLE_FTP_COULDNT_ਸੈੱਟ_TYPE:
  ਹਾਲਤ CURLE_FTP_COULDNT_RETR_FILE:
  ਹਾਲਤ CURLE_PARTIAL_FILE:
  ਹਾਲਤ CURLE_UPLOAD_FAILED:
  ਹਾਲਤ CURLE_REMOTE_ACCESS_DENIED:
  ਹਾਲਤ CURLE_FILESIZE_EXCEEDED:
  ਹਾਲਤ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ:
  ਹਾਲਤ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  ਹਾਲਤ CURLE_OK: /* does not affect the control connection's status */
    ਜੇ(!premature)
      ਰੋਕੋ;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  ਮੂਲ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(ਅੰਕੜੇ, "FTP ended with bad error code");
    connclose(ਅੰਕੜੇ->conn);
    ਵਾਪਸ ਸਥਿਤੀ;      /* use the already set error code */
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ ftp_ਪੂਰਾ_wildcard(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ftp_conn *ftpc)
{
  ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.chunk_ਸਿਰਾ && ftpc->file) {
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_chunk_ਸਿਰਾ);
      ਅੰਕੜੇ->ਸੈੱਟ.chunk_ਸਿਰਾ(ਅੰਕੜੇ->ਸੈੱਟ.wildcardptr);
      CURL_CBAPI_ਸਿਰਾ(&guard);
      freedirs(ftpc);
    }
    ftpc->ਗਿਆਤ_filesize = -1;
  }
}

ਸਥਿਰ ਖਾਲੀ ftp_ਪੂਰਾ_ਰਸਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ftp_conn *ftpc,
                          CURLcode ਨਤੀਜਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(ਨਤੀਜਾ) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ਨਹੀਂਤਾਂ { /* remember working directory for connection reuse */
    ਅਚਲ ਅੱਖਰ *raw_ਰਸਤਾ = ftpc->rawpath;
    ਜੇ(raw_ਰਸਤਾ) {
      ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD) && (raw_ਰਸਤਾ[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ਨਹੀਂਤਾਂ {
        size_t ਰਸਤਾ_Len = ਲੜੀ_ਲੰਬਾਈ(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ਜੇ(!ftpc->cwdfail) {
          ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD)
            ਰਸਤਾ_Len = 0; /* relative path => working directory is FTP home */
          ਨਹੀਂਤਾਂ
            /* file is URL-decoded */
            ਰਸਤਾ_Len -= ftpc->file ? ਲੜੀ_ਲੰਬਾਈ(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_ਰਸਤਾ, ਰਸਤਾ_Len);
        }
        ਨਹੀਂਤਾਂ
          ftpc->prevpath = ਨਿਰਾ; /* no path */
      }
    }
    ਜੇ(ftpc->prevpath)
      infof(ਅੰਕੜੇ, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ਸਥਿਰ CURLcode ftp_ਪੂਰਾ_secondary_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ftp_conn *ftpc,
                                          CURLcode ਨਤੀਜਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(Curl_conn_is_ਤਿਆਰੀ(conn, SECONDARYSOCKET)) {
    ਜੇ(!ਨਤੀਜਾ && ftpc->dont_check && ਅੰਕੜੇ->req.maxdownload > 0) {
      /* partial download completed */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "ABOR");
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "Failure sending ABOR command: %s",
              curl_easy_strerror(ਨਤੀਜਾ));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(ਅੰਕੜੇ, ftpc);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_ਪੂਰਾ_control_ਜਵਾਬ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ftp_conn *ftpc,
                                       ਬਣਤਰ FTP *ftp, CURLcode ਨਤੀਜਾ,
                                       bool premature)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  size_t nread;
  ਪੂਰਨ ftpcode;

  ਜੇ(!ਨਤੀਜਾ && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(ਅੰਕੜੇ); /* timeout relative now */
    ਨਤੀਜਾ = getftpresponse(ਅੰਕੜੇ, &nread, &ftpcode);

    ਜੇ(!nread && (ਨਤੀਜਾ == CURLE_OPERATION_TIMEDOUT)) {
      failf(ਅੰਕੜੇ, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ(ftpc->dont_check && ਅੰਕੜੇ->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(ਅੰਕੜੇ, "partial download completed, closing connection");
      connclose(conn);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ਚੋਣ(ftpcode) {
      ਹਾਲਤ 226:
      ਹਾਲਤ 250:
        ਰੋਕੋ;
      ਹਾਲਤ 552:
        failf(ਅੰਕੜੇ, "Exceeded storage allocation");
        ਨਤੀਜਾ = CURLE_REMOTE_DISK_ਭਰਿਆ;
        ਰੋਕੋ;
      ਮੂਲ:
        failf(ਅੰਕੜੇ, "server did not report OK, got %d", ftpcode);
        ਨਤੀਜਾ = CURLE_PARTIAL_FILE;
        ਰੋਕੋ;
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ftp_ਪੂਰਾ_check_partial(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ftp_conn *ftpc,
                                       ਬਣਤਰ FTP *ftp, CURLcode ਨਤੀਜਾ,
                                       bool premature)
{
  ਜੇ(ਨਤੀਜਾ || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.upload) {
    ਜੇ((ftp->transfer == PPTRANSFER_BODY) &&
       (ਅੰਕੜੇ->state.infilesize != -1) && /* upload with known size */
       ((!ਅੰਕੜੇ->ਸੈੱਟ.crlf && !ਅੰਕੜੇ->state.prefer_ascii && /* no conversion */
         (ਅੰਕੜੇ->state.infilesize != ਅੰਕੜੇ->req.writebytecount)) ||
        ((ਅੰਕੜੇ->ਸੈੱਟ.crlf || ਅੰਕੜੇ->state.prefer_ascii) && /* maybe CRLF conv */
         (ਅੰਕੜੇ->state.infilesize > ਅੰਕੜੇ->req.writebytecount))
       )) {
      failf(ਅੰਕੜੇ, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            ਅੰਕੜੇ->req.writebytecount, ਅੰਕੜੇ->state.infilesize);
      ਨਤੀਜਾ = CURLE_PARTIAL_FILE;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ((ਅੰਕੜੇ->req.ਮਾਪ != -1) &&
       (ਅੰਕੜੇ->req.ਮਾਪ != ਅੰਕੜੇ->req.bytecount) &&
       (ਅੰਕੜੇ->req.maxdownload != ਅੰਕੜੇ->req.bytecount)) {
      failf(ਅੰਕੜੇ, "Received only partial file: %" FMT_OFF_T " bytes",
            ਅੰਕੜੇ->req.bytecount);
      ਨਤੀਜਾ = CURLE_PARTIAL_FILE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ftpc->dont_check &&
            !ਅੰਕੜੇ->req.bytecount &&
            (ਅੰਕੜੇ->req.ਮਾਪ > 0)) {
      failf(ਅੰਕੜੇ, "No data was received");
      ਨਤੀਜਾ = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                         bool premature)
{
  ਬਣਤਰ FTP *ftp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FTP_EASY);
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!ftp || !ftpc)
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = ftp_ਪੂਰਾ_ਸਥਿਤੀ(ਅੰਕੜੇ, ftpc, ਸਥਿਤੀ, premature);

  ftp_ਪੂਰਾ_wildcard(ਅੰਕੜੇ, ftpc);
  ftp_ਪੂਰਾ_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ਨਤੀਜਾ);
  ਨਤੀਜਾ = ftp_ਪੂਰਾ_secondary_socket(ਅੰਕੜੇ, ftpc, ਨਤੀਜਾ);
  ਨਤੀਜਾ = ftp_ਪੂਰਾ_control_ਜਵਾਬ(ਅੰਕੜੇ, ftpc, ftp, ਨਤੀਜਾ, premature);
  ਨਤੀਜਾ = ftp_ਪੂਰਾ_check_partial(ਅੰਕੜੇ, ftpc, ftp, ਨਤੀਜਾ, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  ਜੇ(!ਸਥਿਤੀ && !ਨਤੀਜਾ && !premature && ਅੰਕੜੇ->ਸੈੱਟ.postquote)
    ਨਤੀਜਾ = ftp_sendquote(ਅੰਕੜੇ, ftpc, ਅੰਕੜੇ->ਸੈੱਟ.postquote);
  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] done, result=%d", FTP_CSTATE(ftpc), (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ਸਥਿਰ CURLcode ftp_nb_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ftp_conn *ftpc,
                            ਬਣਤਰ FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ want = (ਅੱਖਰ)(ascii ? 'A' : 'I');

  ਜੇ(ftpc->transfertype == want) {
    ftp_state(ਅੰਕੜੇ, ftpc, newstate);
    ਵਾਪਸ ftp_state_type_resp(ਅੰਕੜੇ, ftpc, ftp, 200, newstate);
  }

  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "TYPE %c", want);
  ਜੇ(!ਨਤੀਜਾ) {
    ftp_state(ਅੰਕੜੇ, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ਸਥਿਰ CURLcode ftp_perform(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ ftp_conn *ftpc,
  ਬਣਤਰ FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_ਪੂਰਾ)
{
  /* this is FTP and no proxy */
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ਜੇ(ਅੰਕੜੇ->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  ਨਤੀਜਾ = ftp_state_quote(ਅੰਕੜੇ, ftpc, ftp, TRUE, FTP_QUOTE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* run the state-machine */
  ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, SECONDARYSOCKET);

  ਜੇ(*connected)
    infof(ਅੰਕੜੇ, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ਨਹੀਂਤਾਂ
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ਜੇ(*dophase_ਪੂਰਾ)
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ wc_ਅੰਕੜੇ_dtor(ਖਾਲੀ *ptr)
{
  ਬਣਤਰ ftp_wc *ftpwc = ptr;
  ਜੇ(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_ਅੰਕੜੇ_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ਸਥਿਰ CURLcode init_wc_ਅੰਕੜੇ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ftp_conn *ftpc,
                             ਬਣਤਰ FTP *ftp)
{
  ਅੱਖਰ *ਆਖਰੀ_slash;
  ਅੱਖਰ *ਰਸਤਾ = ftp->ਰਸਤਾ;
  ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wildcard = ਅੰਕੜੇ->wildcard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ftp_wc *ftpwc = ਨਿਰਾ;

  ਆਖਰੀ_slash = strrchr(ftp->ਰਸਤਾ, '/');
  ਜੇ(ਆਖਰੀ_slash) {
    ਆਖਰੀ_slash++;
    ਜੇ(ਆਖਰੀ_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ਵਾਪਸ ftp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ftp);
    }
    wildcard->ਨਮੂਨਾ = curlx_strdup(ਆਖਰੀ_slash);
    ਜੇ(!wildcard->ਨਮੂਨਾ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਆਖਰੀ_slash[0] = '\0'; /* cut file from path */
  }
  ਨਹੀਂਤਾਂ { /* there is only 'wildcard pattern' or nothing */
    ਜੇ(ਰਸਤਾ[0]) {
      wildcard->ਨਮੂਨਾ = curlx_strdup(ਰਸਤਾ);
      ਜੇ(!wildcard->ਨਮੂਨਾ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰਸਤਾ[0] = '\0';
    }
    ਨਹੀਂਤਾਂ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ਵਾਪਸ ftp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ ftp_wc));
  ਜੇ(!ftpwc) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_ਅੰਕੜੇ_alloc();
  ਜੇ(!ftpwc->parser) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_ਅੰਕੜੇ_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod == FTPFILE_NOCWD)
    ਅੰਕੜੇ->ਸੈੱਟ.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  ਨਤੀਜਾ = ftp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ftp);
  ਜੇ(ਨਤੀਜਾ) {
    ਜਾਓ fail;
  }

  wildcard->ਰਸਤਾ = curlx_strdup(ftp->ਰਸਤਾ);
  ਜੇ(!wildcard->ਰਸਤਾ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  infof(ਅੰਕੜੇ, "Wildcard - Parsing started");
  ਵਾਪਸ CURLE_OK;

fail:
  ਜੇ(ftpwc) {
    Curl_ftp_parselist_ਅੰਕੜੇ_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->ਨਮੂਨਾ);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ਨਿਰਾ;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode wc_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ftp_conn *ftpc,
                             ਬਣਤਰ FTP *ftp)
{
  ਬਣਤਰ Wildcard_ਅੰਕੜੇ * ਅਚਲ wildcard = ਅੰਕੜੇ->wildcard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਲਈ(;;) {
    ਚੋਣ(wildcard->state) {
    ਹਾਲਤ CURLWC_INIT:
      ਨਤੀਜਾ = init_wc_ਅੰਕੜੇ(ਅੰਕੜੇ, ftpc, ftp);
      ਜੇ(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ਵਾਪਸ ਨਤੀਜਾ;
      wildcard->state = ਨਤੀਜਾ ? CURLWC_ERROR : CURLWC_MATCHING;
      ਵਾਪਸ ਨਤੀਜਾ;

    ਹਾਲਤ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ਬਣਤਰ ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ਜੇ(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ਜਾਰੀ;
      }
      ਜੇ(Curl_llist_ਗਿਣਤੀ(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;
      }
      ਜਾਰੀ;
    }

    ਹਾਲਤ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ਬਣਤਰ Curl_llist_ਗੰਢ *ਸਿਰ = Curl_llist_ਸਿਰ(&wildcard->filelist);
      ਬਣਤਰ curl_fileinfo *finfo = Curl_ਗੰਢ_elem(ਸਿਰ);
      ਅੱਖਰ *tmp_ਰਸਤਾ;
      ਅੱਖਰ *enc = curl_easy_escape(ਨਿਰਾ, finfo->filename, 0);
      ਜੇ(!enc)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      tmp_ਰਸਤਾ = curl_maprintf("%s%s", wildcard->ਰਸਤਾ, enc);
      curl_free(enc);
      ਜੇ(!tmp_ਰਸਤਾ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->ਰਸਤਾ = tmp_ਰਸਤਾ;

      infof(ਅੰਕੜੇ, "Wildcard - START of \"%s\"", finfo->filename);
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.chunk_bgn) {
        ਲੰਮਾ userresponse;
        ਬਣਤਰ Curl_mapi_guard guard;
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_chunk_bgn);
        userresponse = ਅੰਕੜੇ->ਸੈੱਟ.chunk_bgn(
          finfo, ਅੰਕੜੇ->ਸੈੱਟ.wildcardptr,
          (ਪੂਰਨ)Curl_llist_ਗਿਣਤੀ(&wildcard->filelist));
        CURL_CBAPI_ਸਿਰਾ(&guard);
        ਚੋਣ(userresponse) {
        ਹਾਲਤ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(ਅੰਕੜੇ, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ਜਾਰੀ;
        ਹਾਲਤ CURL_CHUNK_BGN_FUNC_FAIL:
          ਵਾਪਸ CURLE_CHUNK_FAILED;
        }
      }

      ਜੇ(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ਜਾਰੀ;
      }

      ਜੇ(finfo->flags & CURLFINFOFLAG_ਗਿਆਤ_ਮਾਪ)
        ftpc->ਗਿਆਤ_filesize = finfo->ਮਾਪ;

      ਨਤੀਜਾ = ftp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ftp);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ਗੰਢ_remove(Curl_llist_ਸਿਰ(&wildcard->filelist));

      ਜੇ(Curl_llist_ਗਿਣਤੀ(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ਵਾਪਸ CURLE_OK;
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਹਾਲਤ CURLWC_SKIP: {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.chunk_ਸਿਰਾ) {
        ਬਣਤਰ Curl_mapi_guard guard;
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_chunk_ਸਿਰਾ);
        ਅੰਕੜੇ->ਸੈੱਟ.chunk_ਸਿਰਾ(ਅੰਕੜੇ->ਸੈੱਟ.wildcardptr);
        CURL_CBAPI_ਸਿਰਾ(&guard);
      }
      Curl_ਗੰਢ_remove(Curl_llist_ਸਿਰ(&wildcard->filelist));
      wildcard->state = (Curl_llist_ਗਿਣਤੀ(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ਜਾਰੀ;
    }

    ਹਾਲਤ CURLWC_CLEAN: {
      ਬਣਤਰ ftp_wc *ftpwc = wildcard->ftpwc;
      ਨਤੀਜਾ = CURLE_OK;
      ਜੇ(ftpwc)
        ਨਤੀਜਾ = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = ਨਤੀਜਾ ? CURLWC_ERROR : CURLWC_ਪੂਰਾ;
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਹਾਲਤ CURLWC_ਪੂਰਾ:
    ਹਾਲਤ CURLWC_ERROR:
    ਹਾਲਤ CURLWC_CLEAR:
      ਜੇ(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ਨਿਰਾ;
      }
      ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_regular_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ftp_conn *ftpc,
                                     ਬਣਤਰ FTP *ftp,
                                     bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected = FALSE;
  ਅੰਕੜੇ->req.ਮਾਪ = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(ਅੰਕੜੇ);

  ftpc->ctl_valid = TRUE; /* starts good */

  ਨਤੀਜਾ = ftp_perform(ਅੰਕੜੇ, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_ਪੂਰਾ); /* all commands in the DO-phase done? */

  ਜੇ(!ਨਤੀਜਾ) {

    ਜੇ(!*dophase_ਪੂਰਾ)
      /* the DO phase has not completed yet */
      ਵਾਪਸ CURLE_OK;

    ਨਤੀਜਾ = ftp_dophase_ਪੂਰਾ(ਅੰਕੜੇ, ftpc, ftp, connected);

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
    freedirs(ftpc);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਬਣਤਰ FTP *ftp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FTP_EASY);

  *ਪੂਰਾ = FALSE; /* default to false */
  ਜੇ(!ftpc || !ftp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ftpc->wait_ਅੰਕੜੇ_conn = FALSE; /* default to no such wait */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ਬਣਤਰ Curl_cwriter *ftp_lc_writer;

    ਨਤੀਜਾ = Curl_cwriter_create(&ftp_lc_writer, ਅੰਕੜੇ, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਨਤੀਜਾ = Curl_cwriter_ਜੋੜੋ(ਅੰਕੜੇ, ftp_lc_writer);
    ਜੇ(ਨਤੀਜਾ) {
      Curl_cwriter_free(ਅੰਕੜੇ, ftp_lc_writer);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
#ਜੇ_ਅੰਤ /* CURL_PREFER_LF_LINEENDS */

  ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
    ਨਤੀਜਾ = wc_statemach(ਅੰਕੜੇ, ftpc, ftp);
    ਜੇ(ਅੰਕੜੇ->wildcard->state == CURLWC_SKIP ||
       ਅੰਕੜੇ->wildcard->state == CURLWC_ਪੂਰਾ) {
      /* do not call ftp_regular_transfer */
      ਵਾਪਸ CURLE_OK;
    }
    ਜੇ(ਨਤੀਜਾ) /* error, loop or skipping the file */
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ { /* no wildcard FSM needed */
    ਨਤੀਜਾ = ftp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, ftpc, ftp);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = ftp_regular_transfer(ਅੰਕੜੇ, ftpc, ftp, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode ftp_quit(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ ftp_conn *ftpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ftpc->ctl_valid) {
    CURL_TRC_FTP(ਅੰਕੜੇ, "sending QUIT to close session");
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &ftpc->pp, "%s", "QUIT");
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Failure sending QUIT command: %s",
            curl_easy_strerror(ਨਤੀਜਾ));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(ਅੰਕੜੇ->conn); /* mark for closure */
      ftp_state(ਅੰਕੜੇ, ftpc, FTP_STOP);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ftp_state(ਅੰਕੜੇ, ftpc, FTP_QUIT);

    ਨਤੀਜਾ = ftp_ਖੰਡ_statemach(ਅੰਕੜੇ, ftpc);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ਸਥਿਰ CURLcode ftp_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               bool dead_connection)
{
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ਜੇ(!ftpc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ਜੇ(dead_connection || Curl_pp_needs_flush(ਅੰਕੜੇ, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (ਖਾਲੀ)ftp_quit(ਅੰਕੜੇ, ftpc); /* ignore errors on the QUIT */
  ਵਾਪਸ CURLE_OK;
}

/* called from multi.c while DOing */
ਸਥਿਰ CURLcode ftp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          bool *dophase_ਪੂਰਾ)
{
  ਬਣਤਰ ftp_conn *ftpc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_FTP_CONN);
  ਬਣਤਰ FTP *ftp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FTP_EASY);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!ftpc || !ftp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = ftp_statemach(ਅੰਕੜੇ, ftpc, dophase_ਪੂਰਾ);

  ਜੇ(ਨਤੀਜਾ)
    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ਨਹੀਂਤਾਂ ਜੇ(*dophase_ਪੂਰਾ) {
    ਨਤੀਜਾ = ftp_dophase_ਪੂਰਾ(ਅੰਕੜੇ, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ ftp_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ FTP *ftp = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ਸਥਿਰ ਖਾਲੀ ftp_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ftp_conn *ftpc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_ਮੰਜ਼ਿਲ_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ਸਥਿਰ ਖਾਲੀ type_url_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ FTP *ftp)
{
  size_t len = ਲੜੀ_ਲੰਬਾਈ(ftp->ਰਸਤਾ);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ਜੇ((len >= 7) && !memcmp(&ftp->ਰਸਤਾ[len - 7], ";type=", 6)) {
    ਅੱਖਰ *type = &ftp->ਰਸਤਾ[len - 7];
    ਅੱਖਰ command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ਚੋਣ(command) {
    ਹਾਲਤ 'A': /* ASCII mode */
      ਅੰਕੜੇ->state.prefer_ascii = TRUE;
      ਰੋਕੋ;

    ਹਾਲਤ 'D': /* directory mode */
      ਅੰਕੜੇ->state.list_only = TRUE;
      ਰੋਕੋ;

    ਹਾਲਤ 'I': /* binary mode */
    ਮੂਲ:
      /* switch off ASCII */
      ਅੰਕੜੇ->state.prefer_ascii = FALSE;
      ਰੋਕੋ;
    }
  }
}

ਸਥਿਰ CURLcode ftp_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ FTP *ftp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ftp_conn *ftpc;

  ftp = curlx_calloc(1, ਆਕਾਰ(*ftp));
  ਜੇ(!ftp ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ftpc = curlx_calloc(1, ਆਕਾਰ(*ftpc));
  ਜੇ(!ftpc ||
     Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* clone connection related data that is FTP specific */
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ACCOUNT));
    ਜੇ(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER)) {
    ftpc->alternative_ਮੰਜ਼ਿਲ_user =
      curlx_strdup(CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER));
    ਜੇ(!ftpc->alternative_ਮੰਜ਼ਿਲ_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ftp->ਰਸਤਾ = &ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1]; /* do not include the initial slash */

  type_url_check(ਅੰਕੜੇ, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->ਗਿਆਤ_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = ਅੰਕੜੇ->ਸੈੱਟ.use_ssl;
  ftpc->ccc = ਅੰਕੜੇ->ਸੈੱਟ.ftp_ccc;

  CURL_TRC_FTP(ਅੰਕੜੇ, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_ftp_conns_ਮੇਲ(ਬਣਤਰ connectdata *needle, ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ਬਣਤਰ ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ਜੇ(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_ਮੰਜ਼ਿਲ_user,
                     cftpc->alternative_ਮੰਜ਼ਿਲ_user) ||
     (nftpc->ccc != cftpc->ccc))
    ਵਾਪਸ FALSE;

  ਚੋਣ(nftpc->use_ssl) {
  ਹਾਲਤ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ਵਾਪਸ (cftpc->use_ssl != CURLUSESSL_NONE);
  ਮੂਲ:
    ਵਾਪਸ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_ftp = {
  ftp_ਤਿਆਰੀ_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_ਪੂਰਾ,                        /* done */
  ftp_do_ਹੋਰ,                     /* do_more */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_FTP */
