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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NI_MAXHOST
#ವ್ಯಾಖ್ಯಾನಿಸು NI_MAXHOST 1025
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ INET_ADDRSTRLEN
#ವ್ಯಾಖ್ಯಾನಿಸು INET_ADDRSTRLEN 16
#ಆದರೆ_ಅಂತ್ಯ

/* macro to check for a three-digit ftp status code at the start of the
   given string */
#ವ್ಯಾಖ್ಯಾನಿಸು STATUSCODE(ಸಾಲು) \
  (ISDIGIT((ಸಾಲು)[0]) && ISDIGIT((ಸಾಲು)[1]) && ISDIGIT((ಸಾಲು)[2]))

/* macro to check for the last line in an FTP server response */
#ವ್ಯಾಖ್ಯಾನಿಸು LASTLINE(ಸಾಲು) (STATUSCODE(ಸಾಲು) && (' ' == (ಸಾಲು)[3]))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
/* for tracing purposes */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ftp_state_ಹೆಸರುಗಳು[] = {
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
#ವ್ಯಾಖ್ಯಾನಿಸು FTP_CSTATE(ftpc)   ((ftpc) ? ftp_state_ಹೆಸರುಗಳು[(ftpc)->state] : "???")

#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

/* This is the ONLY way to change FTP state! */
ಸ್ಥಿರ ಶೂನ್ಯ ftp_state_ಕೆಳಗಿನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ ftp_conn *ftpc,
                          ftpstate newstate
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
                          , ಪೂರ್ಣಾಂಕ lineno
#ಆದರೆ_ಅಂತ್ಯ
  )
{
  ಆದರೆ(ftpc->state != newstate) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    NOVERBOSE((ಶೂನ್ಯ)lineno);
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] -> [%s] (line %d)", FTP_CSTATE(ftpc),
                 ftp_state_ಹೆಸರುಗಳು[newstate], lineno);
#ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] -> [%s]", FTP_CSTATE(ftpc),
                 ftp_state_ಹೆಸರುಗಳು[newstate]);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ftpc->state = newstate;
}

/* Local API functions */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು ftp_state(x, y, z) ftp_state_ಕೆಳಗಿನ(x, y, z)
#ಇಲ್ಲದಿದ್ದರೆ /* !DEBUGBUILD */
#ವ್ಯಾಖ್ಯಾನಿಸು ftp_state(x, y, z) ftp_state_ಕೆಳಗಿನ(x, y, z, __LINE__)
#ಆದರೆ_ಅಂತ್ಯ /* DEBUGBUILD */

ಸ್ಥಿರ CURLcode ftp_state_mdtm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp);
ಸ್ಥಿರ CURLcode ftp_state_quote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ftp_conn *ftpc,
                                ರಚನೆ FTP *ftp,
                                bool init, ftpstate instate);
ಸ್ಥಿರ CURLcode ftp_nb_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ftp_conn *ftpc,
                            ರಚನೆ FTP *ftp,
                            bool ascii, ftpstate newstate);
ಸ್ಥಿರ CURLcode getftpresponse(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t *nreadp,
                               ಪೂರ್ಣಾಂಕ *ftpcodep);

ಸ್ಥಿರ ಶೂನ್ಯ freedirs(ರಚನೆ ftp_conn *ftpc)
{
  curlx_safefree(ftpc->dirs);
  ftpc->dirdepth = 0;
  curlx_safefree(ftpc->rawpath);
  ftpc->file = ಖಾಲಿ;
}

ಸ್ಥಿರ size_t numof_slashes(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *slashPos;
  size_t ಅಂಕೆ = 0;
  ಮಾಡು {
    slashPos = strchr(str, '/');
    ಆದರೆ(slashPos) {
      ++ಅಂಕೆ;
      str = slashPos + 1;
    }
  } ಆಗ(slashPos);
  ಹಿಂತಿರುಗಿ ಅಂಕೆ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FTP_MAX_DIR_ಆಳ 1000

/***********************************************************************
 *
 * ftp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ಸ್ಥಿರ CURLcode ftp_parse_url_ಮಾರ್ಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc,
                                   ರಚನೆ FTP *ftp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *slashPos = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fileName = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *raw_ಮಾರ್ಗ = ಖಾಲಿ; /* URL-decoded "raw" path */
  size_t ಮಾರ್ಗ_Len = 0;

  ftpc->ctl_valid = FALSE;
  ftpc->cwdfail = FALSE;

  ಆದರೆ(ftpc->rawpath)
    freedirs(ftpc);
  /* URL-decode ftp path before further evaluation */
  ಫಲಿತಾಂಶ = Curl_urldecode(ftp->ಮಾರ್ಗ, 0, &ftpc->rawpath, &ಮಾರ್ಗ_Len, REJECT_CTRL);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "path contains control characters");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  raw_ಮಾರ್ಗ = ftpc->rawpath;

  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod) {
  ಸ್ಥಿತಿ FTPFILE_NOCWD: /* fastest, but less standard-compliant */

    ಆದರೆ((ಮಾರ್ಗ_Len > 0) && (raw_ಮಾರ್ಗ[ಮಾರ್ಗ_Len - 1] != '/'))
      fileName = raw_ಮಾರ್ಗ;  /* this is a full file path */
    /* else: ftpc->file is not used anywhere other than for operations on
             a file. In other words, never for directory operations,
             so we can safely leave filename as NULL here and use it as a
             argument in dir/file decisions. */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTPFILE_SINGLECWD:
    slashPos = strrchr(raw_ಮಾರ್ಗ, '/');
    ಆದರೆ(slashPos) {
      /* get path before last slash, except for / */
      size_t dirlen = slashPos - raw_ಮಾರ್ಗ;
      ಆದರೆ(dirlen == 0)
        dirlen = 1;

      ftpc->dirs = curlx_calloc(1, ಗಾತ್ರ(ftpc->dirs[0]));
      ಆದರೆ(!ftpc->dirs)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ftpc->dirs[0].ಆರಂಭ = 0;
      ftpc->dirs[0].len = (ಪೂರ್ಣಾಂಕ)dirlen;
      ftpc->dirdepth = 1; /* we consider it to be a single directory */
      fileName = slashPos + 1; /* rest is filename */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      fileName = raw_ಮಾರ್ಗ; /* filename only (or empty) */
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ: /* allow pretty much anything */
  ಸ್ಥಿತಿ FTPFILE_MULTICWD: {
    /* current position: begin of next path component */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curPos = raw_ಮಾರ್ಗ;

    /* number of entries to allocate for the 'dirs' array */
    size_t dirAlloc = numof_slashes(raw_ಮಾರ್ಗ);

    ಆದರೆ(dirAlloc >= FTP_MAX_DIR_ಆಳ)
      /* suspiciously deep directory hierarchy */
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

    ಆದರೆ(dirAlloc) {
      ftpc->dirs = curlx_calloc(dirAlloc, ಗಾತ್ರ(ftpc->dirs[0]));
      ಆದರೆ(!ftpc->dirs)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      /* parse the URL path into separate path components */
      ಆಗ(dirAlloc--) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *spos = strchr(curPos, '/');
        size_t clen = spos - curPos;

        /* path starts with a slash: add that as a directory */
        ಆದರೆ(!clen && (ftpc->dirdepth == 0))
          ++clen;

        /* we skip empty path components, like "x//y" since the FTP command
           CWD requires a parameter and a non-existent parameter a) does not
           work on many servers and b) has no effect on the others. */
        ಆದರೆ(clen) {
          ftpc->dirs[ftpc->dirdepth].ಆರಂಭ = (ಪೂರ್ಣಾಂಕ)(curPos - raw_ಮಾರ್ಗ);
          ftpc->dirs[ftpc->dirdepth].len = (ಪೂರ್ಣಾಂಕ)clen;
          ftpc->dirdepth++;
        }
        curPos = spos + 1;
      }
    }
    fileName = curPos; /* the rest is the filename (or empty) */
  }
    ನಿಲ್ಲಿಸು;
  } /* switch */

  ಆದರೆ(fileName && *fileName)
    ftpc->file = fileName;
  ಇಲ್ಲದಿದ್ದರೆ
    ftpc->file = ಖಾಲಿ; /* instead of point to a NUL byte,
                          we make it a NULL pointer */

  ಆದರೆ(ದತ್ತಾಂಶ->state.upload && !ftpc->file && (ftp->transfer == PPTRANSFER_BODY)) {
    /* We need a filename when uploading. Return error! */
    failf(ದತ್ತಾಂಶ, "Uploading to a URL without a filename");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

  ftpc->cwddone = FALSE; /* default to not done */

  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD) && (raw_ಮಾರ್ಗ[0] == '/'))
    ftpc->cwddone = TRUE; /* skip CWD for absolute paths */
  ಇಲ್ಲದಿದ್ದರೆ { /* newly created FTP connections are already in entry path */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *old_ಮಾರ್ಗ = ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse ? ftpc->prevpath : "";
    ಆದರೆ(old_ಮಾರ್ಗ) {
      size_t n = ಮಾರ್ಗ_Len;
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD)
        n = 0; /* CWD to entry for relative paths */
      ಇಲ್ಲದಿದ್ದರೆ
        n -= ftpc->file ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ftpc->file) : 0;

      ಆದರೆ((ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(old_ಮಾರ್ಗ) == n) && raw_ಮಾರ್ಗ && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(raw_ಮಾರ್ಗ, old_ಮಾರ್ಗ, n)) {
        infof(ದತ್ತಾಂಶ, "Request has same path as previous transfer");
        ftpc->cwddone = TRUE;
      }
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_need_type()
 *
 * Returns TRUE if we in the current situation should send TYPE
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ftp_need_type(ರಚನೆ ftp_conn *ftpc,
                         bool ascii_wanted)
{
  ಹಿಂತಿರುಗಿ ftpc->transfertype != (ascii_wanted ? 'A' : 'I');
}

ಸ್ಥಿರ ಶೂನ್ಯ close_secondarysocket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ftp_conn *ftpc)
{
  (ಶೂನ್ಯ)ftpc;
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] closing DATA connection", FTP_CSTATE(ftpc));
  Curl_conn_cf_discard_all(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, SECONDARYSOCKET);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_PREFER_LF_LINEENDS
/*
 * Lineend Conversions
 * On ASCII transfers, e.g. directory listings, we might get lines
 * ending in '\r\n' and we prefer '\n'.
 * We might also get a lonely '\r' which we convert into a '\n'.
 */
ರಚನೆ ftp_cw_lc_ctx {
  ರಚನೆ Curl_cwriter super;
  bool newline_pending;
};

ಸ್ಥಿರ CURLcode ftp_cw_lc_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ nl = '\n';
  ರಚನೆ ftp_cw_lc_ctx *ctx = writer->ctx;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);

  ಆದರೆ(!ftpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!(type & CLIENTWRITE_BODY) || ftpc->transfertype != 'A')
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, blen);

  /* ASCII mode BODY data, convert lineends */
  ಆಗ(blen) {
    /* do not pass EOS when writing parts */
    ಪೂರ್ಣಾಂಕ chunk_type = (type & ~CLIENTWRITE_EOS);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cp;
    size_t chunk_len;
    CURLcode ಫಲಿತಾಂಶ;

    ಆದರೆ(ctx->newline_pending) {
      ಆದರೆ(buf[0] != '\n') {
        /* previous chunk ended in '\r' and we do not see a '\n' in this one,
         * need to write a newline. */
        ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, chunk_type, &nl, 1);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      /* either we wrote the newline or it is part of the next chunk of bytes
       * we write. */
      ctx->newline_pending = FALSE;
    }

    cp = memchr(buf, '\r', blen);
    ಆದರೆ(!cp)
      ನಿಲ್ಲಿಸು;

    /* write the bytes before the '\r', excluding the '\r' */
    chunk_len = cp - buf;
    ಆದರೆ(chunk_len) {
      ಫಲಿತಾಂಶ = Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, chunk_type,
                                  buf, chunk_len);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    /* skip the '\r', we now have a newline pending */
    buf = cp + 1;
    blen = blen - chunk_len - 1;
    ctx->newline_pending = TRUE;
  }

  /* Any remaining data does not contain a '\r' */
  ಆದರೆ(blen) {
    DEBUGASSERT(!ctx->newline_pending);
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, blen);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type & CLIENTWRITE_EOS) {
    /* EndOfStream, if we have a trailing cr, now is the time to write it */
    ಆದರೆ(ctx->newline_pending) {
      ctx->newline_pending = FALSE;
      ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, &nl, 1);
    }
    /* Always pass on the EOS type indicator */
    ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, 0);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype ftp_cw_lc = {
  "ftp-lineconv",
  ಖಾಲಿ,
  0,
  Curl_cwriter_def_init,
  ftp_cw_lc_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಗಾತ್ರ(ರಚನೆ ftp_cw_lc_ctx)
};

#ಆದರೆ_ಅಂತ್ಯ /* CURL_PREFER_LF_LINEENDS */

/***********************************************************************
 *
 * ftp_check_ctrl_on_data_wait()
 *
 */
ಸ್ಥಿರ CURLcode ftp_check_ctrl_on_ದತ್ತಾಂಶ_wait(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ftp_conn *ftpc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t ctrl_sock = conn->sock[FIRSTSOCKET];
  ರಚನೆ pingpong *pp = &ftpc->pp;
  size_t nread;
  ಪೂರ್ಣಾಂಕ ftpcode;
  bool response = FALSE;

  /* First check whether there is a cached response from server */
  ಆದರೆ(curlx_dyn_len(&pp->recvbuf)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l = curlx_dyn_ptr(&pp->recvbuf);
    ಆದರೆ(!ISDIGIT(*l) || (*l > '3')) {
      /* Data connection could not be established, let's return */
      infof(ದತ್ತಾಂಶ, "There is negative response in cache while serv connect");
      (ಶೂನ್ಯ)getftpresponse(ದತ್ತಾಂಶ, &nread, &ftpcode);
      ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
    }
  }

  ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ)
    /* there is pending control data still in the buffer to read */
    response = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಪೂರ್ಣಾಂಕ socketstate = SOCKET_READABLE(ctrl_sock, 0);
    /* see if the connection request is already here */
    ಆಯ್ಕೆ(socketstate) {
    ಸ್ಥಿತಿ -1: /* error */
      /* let's die here */
      failf(ದತ್ತಾಂಶ, "Error while waiting for server connect");
      ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಆದರೆ(socketstate & CURL_CSELECT_ಒಳ)
        response = TRUE;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(response) {
    infof(ದತ್ತಾಂಶ, "Ctrl conn has data while waiting for data conn");
    ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ > 3) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *r = curlx_dyn_ptr(&pp->recvbuf);
      size_t len = curlx_dyn_len(&pp->recvbuf);

      DEBUGASSERT((pp->ಅತಿಪ್ರವಾಹ + pp->nfinal) <= curlx_dyn_len(&pp->recvbuf));
      /* move over the most recently handled response line */
      r += pp->nfinal;
      len -= pp->nfinal;

      ಆದರೆ((len > 3) && LASTLINE(r)) {
        curl_off_t ಸ್ಥಿತಿ_2;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&r, &ಸ್ಥಿತಿ_2, 999) && (ಸ್ಥಿತಿ_2 == 226)) {
          /* funny timing situation where we get the final message on the
             control connection before traffic on the data connection has been
             noticed. Leave the 226 in there and use this as a trigger to read
             the data socket. */
          infof(ದತ್ತಾಂಶ, "Got 226 before data activity");
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
      }
    }

    (ಶೂನ್ಯ)getftpresponse(ದತ್ತಾಂಶ, &nread, &ftpcode);

    infof(ದತ್ತಾಂಶ, "FTP code: %03d", ftpcode);

    ಆದರೆ(ftpcode / 100 > 3)
      ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;

    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * ftp_initiate_transfer()
 *
 * After connection from server is accepted this function is called to
 * setup transfer parameters and initiate the data transfer.
 *
 */
ಸ್ಥಿರ CURLcode ftp_initiate_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ftp_conn *ftpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected;

  CURL_TRC_FTP(ದತ್ತಾಂಶ, "ftp_initiate_transfer()");
  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, SECONDARYSOCKET, TRUE, &connected);
  ಆದರೆ(ಫಲಿತಾಂಶ || !connected)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
    /* When we know we are uploading a specified file, we can get the file
       size prior to the actual upload. */
    Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);

    /* FTP upload, shutdown DATA, ignore shutdown errors, as we rely
     * on the server response on the CONTROL connection. */
    Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, SECONDARYSOCKET);
    Curl_xfer_ಗಣ_shutdown(ದತ್ತಾಂಶ, TRUE, TRUE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* FTP download, shutdown, do not ignore errors */
    Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, SECONDARYSOCKET, ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);
    Curl_xfer_ಗಣ_shutdown(ದತ್ತಾಂಶ, TRUE, FALSE);
  }

  ftpc->pp.pending_resp = TRUE; /* expect server response */
  ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool ftp_endofresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಂಕ *ಸಂಕೇತ_2)
{
  curl_off_t ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;

  ಆದರೆ((len > 3) && LASTLINE(ಸಾಲು) && !curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &ಸ್ಥಿತಿ_2, 999)) {
    *ಸಂಕೇತ_2 = (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2;
    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode ftp_readresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ftp_conn *ftpc,
                             int8_t sockindex,
                             ರಚನೆ pingpong *pp,
                             ಪೂರ್ಣಾಂಕ *ftpcodep, /* return the ftp-code if done */
                             size_t *ಗಾತ್ರ_2) /* size of the response */
{
  ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2;
  CURLcode ಫಲಿತಾಂಶ = Curl_pp_readresp(ದತ್ತಾಂಶ, sockindex, pp, &ಸಂಕೇತ_2, ಗಾತ್ರ_2);
  DEBUGASSERT(ftpcodep);

  /* store the latest code for later retrieval, except during shutdown */
  ಆದರೆ(!ftpc->shutdown)
    ದತ್ತಾಂಶ->info.httpcode = ಸಂಕೇತ_2;

  *ftpcodep = ಸಂಕೇತ_2;

  ಆದರೆ(ಸಂಕೇತ_2 == 421) {
    /* 421 means "Service not available, closing control connection." and FTP
     * servers use it to signal that idle session timeout has been exceeded.
     * If we ignored the response, it could end up hanging in some cases.
     *
     * This response code can come at any point so having it treated
     * generically is a good idea.
     */
    infof(ದತ್ತಾಂಶ, "We got a 421 - timeout");
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* --- parse FTP server responses --- */

/*
 * getftpresponse() is a BLOCKING function to read the full response from a
 * server after a command.
 *
 */
ಸ್ಥಿರ CURLcode getftpresponse(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               size_t *nreadp, /* return number of bytes
                                                  read */
                               ಪೂರ್ಣಾಂಕ *ftpcodep) /* return the ftp-code */
{
  /*
   * We cannot read one byte per read() and then go back to select() as the
   * OpenSSL read() does not grok that properly.
   *
   * Alas, read as much as possible, split up into lines, use the ending
   * line in a response or continue reading. */

  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ರಚನೆ pingpong *pp = &ftpc->pp;
  size_t nread;
  ಪೂರ್ಣಾಂಕ cache_skip = 0;
  DEBUGASSERT(ftpcodep);

  CURL_TRC_FTP(ದತ್ತಾಂಶ, "getftpresponse start");
  *nreadp = 0;
  *ftpcodep = 0; /* 0 for errors */

  ಆದರೆ(!ftpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಗ(!*ftpcodep && !ಫಲಿತಾಂಶ) {
    /* check and reset timeout value every lap */
    timediff_t timeout = Curl_pp_state_timeleft_ms(ದತ್ತಾಂಶ, pp);
    timediff_t interval_ms;

    ಆದರೆ(timeout <= 0) {
      failf(ದತ್ತಾಂಶ, "FTP response timeout");
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT; /* already too little time */
    }

    interval_ms = 1000;  /* use 1 second timeout intervals */
    ಆದರೆ(timeout < interval_ms)
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

    ಆದರೆ(curlx_dyn_len(&pp->recvbuf) && (cache_skip < 2)) {
      /*
       * There is a cache left since before. We then skipping the wait for
       * socket action, unless this is the same cache like the previous round
       * as then the cache was deemed not enough to act on and we then need to
       * wait for more data anyway.
       */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET)) {
      curl_socket_t wsock = Curl_pp_needs_flush(ದತ್ತಾಂಶ, pp) ?
        sockfd : CURL_SOCKET_BAD;
      ಪೂರ್ಣಾಂಕ ev = Curl_socket_check(sockfd, CURL_SOCKET_BAD, wsock, interval_ms);
      ಆದರೆ(ev < 0) {
        failf(ದತ್ತಾಂಶ, "FTP response aborted due to select/poll error: %d",
              SOCKERRNO);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ev == 0) {
        ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
        ಮುಂದುವರಿ; /* continue in our loop for the timeout duration */
      }
    }

    ಆದರೆ(Curl_pp_needs_flush(ದತ್ತಾಂಶ, pp)) {
      ಫಲಿತಾಂಶ = Curl_pp_flushsend(ದತ್ತಾಂಶ, pp);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = ftp_readresp(ದತ್ತಾಂಶ, ftpc, FIRSTSOCKET, pp, ftpcodep, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(!nread && curlx_dyn_len(&pp->recvbuf))
      /* bump cache skip counter as on repeated skips we must wait for more
ದತ್ತಾಂಶ*/
      cache_skip++;
    ಇಲ್ಲದಿದ್ದರೆ
      /* when we got data or there is no cache left, we reset the cache skip
         counter */
      cache_skip = 0;

    *nreadp += nread;

  } /* while there is buffer left and loop is requested */

  pp->pending_resp = FALSE;
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "getftpresponse -> result=%d, nread=%zu, ftpcode=%d",
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *nreadp, *ftpcodep);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_user(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "USER %s",
                                  Curl_creds_user(conn->creds));
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ftpc->ftp_trying_alternative = FALSE;
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_USER);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_pwd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ftp_conn *ftpc)
{
  CURLcode ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(!ದತ್ತಾಂಶ->id && getenv("CURL_FTP_PWD_STOP"))
    ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "PWD");
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PWD);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the FTP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode ftp_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ಹಿಂತಿರುಗಿ ftpc ? Curl_pp_pollset(ದತ್ತಾಂಶ, &ftpc->pp, ps) : CURLE_OK;
}

/* For the FTP "DO_MORE" phase only */
ಸ್ಥಿರ CURLcode ftp_domore_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);

  ಆದರೆ(!ftpc)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* When in DO_MORE state, we could be either waiting for us to connect to a
   * remote site, or we could wait for that site to connect to us. Or handle
   * ordinary commands.
   */
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] ftp_domore_pollset()", FTP_CSTATE(ftpc));

  ಆದರೆ(FTP_STOP == ftpc->state) {
    /* if stopped and still in this state, then we are also waiting for a
       connect on the secondary connection */
    DEBUGASSERT(ದತ್ತಾಂಶ->conn->sock[SECONDARYSOCKET] != CURL_SOCKET_BAD ||
                (ದತ್ತಾಂಶ->conn->cfilter[SECONDARYSOCKET] &&
                 !Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, SECONDARYSOCKET)));
    /* An unconnected SECONDARY will add its socket by itself
     * via its adjust_pollset() */
    ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
  }
  ಹಿಂತಿರುಗಿ Curl_pp_pollset(ದತ್ತಾಂಶ, &ftpc->pp, ps);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pathlen(ರಚನೆ ftp_conn *ftpc, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ಅಂಕೆ);
  ಹಿಂತಿರುಗಿ ftpc->dirs[ಅಂಕೆ].len;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathpiece(ರಚನೆ ftp_conn *ftpc, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
  DEBUGASSERT(ftpc->dirs);
  DEBUGASSERT(ftpc->dirdepth > ಅಂಕೆ);
  ಹಿಂತಿರುಗಿ &ftpc->rawpath[ftpc->dirs[ಅಂಕೆ].ಆರಂಭ];
}

/* This is called after the FTP_QUOTE state is passed.

   ftp_state_cwd() sends the range of CWD commands to the server to change to
   the correct directory. It may also need to send MKD commands to create
   missing ones, if that option is enabled. */
ಸ್ಥಿರ CURLcode ftp_state_cwd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ftp_conn *ftpc,
                              ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ftpc->cwddone)
    /* already done and fine */
    ಫಲಿತಾಂಶ = ftp_state_mdtm(ದತ್ತಾಂಶ, ftpc, ftp);
  ಇಲ್ಲದಿದ್ದರೆ {
    /* FTPFILE_NOCWD with full path: expect ftpc->cwddone! */
    DEBUGASSERT((ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod != FTPFILE_NOCWD) ||
                !(ftpc->dirdepth && ftpc->rawpath[0] == '/'));

    ftpc->count2 = 0; /* count2 counts failed CWDs */

    ಆದರೆ(ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse && ftpc->entrypath &&
       /* no need to go to entrypath when we have an absolute path */
       !(ftpc->dirdepth && ftpc->rawpath[0] == '/')) {
      /* This is a reused connection. Since we change directory to where the
         transfer is taking place, we must first get back to the original dir
         where we ended up after login: */
      ftpc->cwdcount = 0; /* we count this as the first path, then we add one
                             for all upcoming ones in the ftp->dirs[] array */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "CWD %s", ftpc->entrypath);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_CWD);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(ftpc->dirdepth) {
        ftpc->cwdcount = 1;
        /* issue the first CWD, the rest is sent when the CWD responses are
           received... */
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, 0), pathpiece(ftpc, 0));
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_CWD);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* No CWD necessary */
        ಫಲಿತಾಂಶ = ftp_state_mdtm(ದತ್ತಾಂಶ, ftpc, ftp);
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  EPRT,
  ಪೋರ್ಟ್_2,
  ಮುಗಿದ_2
} ftpport;

/*
 * Parse the CURLOPT_FTPPORT string
 * "(ipv4|ipv6|domain|interface)?(:port(-range)?)?"
 * and extract addr/addrlen and port_min/port_max.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_parse_string(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string_ftpport,
                                      ರಚನೆ Curl_sockaddr_storage *ss,
                                      ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *ಪೋರ್ಟ್_minp,
                                      ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *ಪೋರ್ಟ್_maxp,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **hostp,
                                      ಅಕ್ಷರ *hbuf, size_t hbuflen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ip_ಕೊನೆ = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *addr = ಖಾಲಿ;
  size_t addrlen = 0;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_min = 0;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_max = 0;
  ಅಕ್ಷರ ipstr[50];
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IPV6
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ss;
#ಆದರೆ_ಅಂತ್ಯ

  /* default to nothing */
  *hostp = ಖಾಲಿ;
  *ಪೋರ್ಟ್_minp = *ಪೋರ್ಟ್_maxp = 0;

  ಆದರೆ(!string_ftpport || (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string_ftpport) <= 1))
    ಹೋಗು ಮುಗಿದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(*string_ftpport == '[') {
    /* [ipv6]:port(-range) */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ip_ಆರಂಭ = string_ftpport + 1;
    ip_ಕೊನೆ = strchr(ip_ಆರಂಭ, ']');
    ಆದರೆ(ip_ಕೊನೆ) {
      addrlen = ip_ಕೊನೆ - ip_ಆರಂಭ;
      addr = ip_ಆರಂಭ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(*string_ftpport == ':') {
      /* :port */
      ip_ಕೊನೆ = string_ftpport;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ip_ಕೊನೆ = strchr(string_ftpport, ':');
      addr = string_ftpport;
      ಆದರೆ(ip_ಕೊನೆ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
        ರಚನೆ sockaddr_in6 * ಸ್ಥಿರಾಂಕ sa6 = (ಶೂನ್ಯ *)ss;
#ಆದರೆ_ಅಂತ್ಯ
        /* either IPv6 or (ipv4|domain|interface):port(-range) */
        addrlen = ip_ಕೊನೆ - string_ftpport;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
        ಆದರೆ(curlx_inet_pton(AF_INET6, string_ftpport, &sa6->sin6_addr) == 1) {
          /* IPv6 */
          addrlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string_ftpport);
          ip_ಕೊನೆ = ಖಾಲಿ; /* this got no port ! */
        }
#ಆದರೆ_ಅಂತ್ಯ
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* ipv4|interface */
        addrlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string_ftpport);
    }

  /* parse the port */
  ಆದರೆ(ip_ಕೊನೆ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *portp = strchr(ip_ಕೊನೆ, ':');
    ಆದರೆ(portp) {
      curl_off_t ಆರಂಭ;
      curl_off_t ಕೊನೆ;
      portp++;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&portp, &ಆರಂಭ, 0xffff)) {
        ಪೋರ್ಟ್_min = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಆರಂಭ;
        ಆದರೆ(!curlx_str_single(&portp, '-') &&
           !curlx_str_ಸಂಖ್ಯೆ(&portp, &ಕೊನೆ, 0xffff))
          ಪೋರ್ಟ್_max = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಕೊನೆ;
        ಇಲ್ಲದಿದ್ದರೆ
          ಪೋರ್ಟ್_max = ಪೋರ್ಟ್_min;
      }
    }
  }

  /* correct errors like :1234-1230 or :-4711 */
  ಆದರೆ(ಪೋರ್ಟ್_min > ಪೋರ್ಟ್_max)
    ಪೋರ್ಟ್_min = ಪೋರ್ಟ್_max = 0;

  ಆದರೆ(addrlen) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *remote_addr =
      Curl_conn_get_remote_addr(ದತ್ತಾಂಶ, FIRSTSOCKET);

    DEBUGASSERT(remote_addr);
    DEBUGASSERT(addr);
    ಆದರೆ(!remote_addr || (addrlen >= ಗಾತ್ರ(ipstr)) || (addrlen >= hbuflen))
      ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
    ಸ್ಮರಣೆ_ನಕಲು(ipstr, addr, addrlen);
    ipstr[addrlen] = 0;

    /* attempt to get the address of the given interface name */
    ಆಯ್ಕೆ(Curl_if2ip(remote_addr->family,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                      Curl_ipv6_scope(&remote_addr->curl_sa_addr),
                      conn->scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                      ipstr, hbuf, hbuflen)) {
    ಸ್ಥಿತಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು:
      /* not an interface, use the string as hostname instead */
      ಸ್ಮರಣೆ_ನಕಲು(hbuf, addr, addrlen);
      hbuf[addrlen] = 0;
      *hostp = hbuf;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IF2IP_AF_NOT_SUPPORTED:
      ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
    ಸ್ಥಿತಿ IF2_IP_ಸಿಕ್ಕಿತು:
      *hostp = hbuf; /* use the hbuf for hostname */
      ನಿಲ್ಲಿಸು;
    }
  }
  /* else: only a port(-range) given, leave host as NULL */

ಮುಗಿದ:
  *ಪೋರ್ಟ್_minp = ಪೋರ್ಟ್_min;
  *ಪೋರ್ಟ್_maxp = ಪೋರ್ಟ್_max;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * If no host was derived from the FTPPORT string, fall back to the IP address
 * of the control connection's local socket.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_default_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn,
                                      ರಚನೆ Curl_sockaddr_storage *ss,
                                      curl_socklen_t *sslenp,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **hostp,
                                      ಅಕ್ಷರ *hbuf, size_t hbuflen,
                                      bool *non_localp)
{
  ರಚನೆ sockaddr *sa = (ರಚನೆ sockaddr *)ss;
  ರಚನೆ sockaddr_in * ಸ್ಥಿರಾಂಕ sa4 = (ಶೂನ್ಯ *)sa;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 * ಸ್ಥಿರಾಂಕ sa6 = (ಶೂನ್ಯ *)sa;
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
  CURLcode ಫಲಿತಾಂಶ;

  *sslenp = ಗಾತ್ರ(*ss);
  ಆದರೆ(getsockname(conn->sock[FIRSTSOCKET], sa, sslenp)) {
    failf(ದತ್ತಾಂಶ, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
  }
  ಆಯ್ಕೆ(sa->sa_family) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ AF_INET6:
    ಫಲಿತಾಂಶ = curlx_inet_ntop(sa->sa_family, &sa6->sin6_addr, hbuf, hbuflen);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = curlx_inet_ntop(sa->sa_family, &sa4->sin_addr, hbuf, hbuflen);
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;

  *hostp = hbuf;
  *non_localp = FALSE; /* we know it is local now */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Resolve the host string to a list of addresses.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_resolve_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                      ರಚನೆ Curl_dns_ನಮೂದು **dns_entryp,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo **resp)
{
  CURLcode ಫಲಿತಾಂಶ;

  *resp = ಖಾಲಿ;
  ಫಲಿತಾಂಶ = Curl_resolv_blocking(
    ದತ್ತಾಂಶ, Curl_resolv_dns_queries(ದತ್ತಾಂಶ, conn->ip_ಆವೃತ್ತಿ),
    host, 0, Curl_conn_get_transport(ದತ್ತಾಂಶ, conn), dns_entryp);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "failed to resolve the address provided to PORT: %s", host);
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(*dns_entryp);
    *resp = (*dns_entryp)->addr;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Open a TCP socket for the resolved address family.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_open_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *res,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo **aip,
                                     curl_socket_t *portsockp)
{
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
  ಪೂರ್ಣಾಂಕ sockerr = 0;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai;
  CURLcode ಫಲಿತಾಂಶ = CURLE_FTP_ಪೋರ್ಟ್_FAILED;

  ಗಾಗಿ(ai = res; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಫಲಿತಾಂಶ =
      Curl_socket_open(ದತ್ತಾಂಶ, ai, ಖಾಲಿ,
                       Curl_conn_get_transport(ದತ್ತಾಂಶ, conn), portsockp);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಫಲಿತಾಂಶ = CURLE_FTP_ಪೋರ್ಟ್_FAILED;
      sockerr = SOCKERRNO;
      ಮುಂದುವರಿ;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(!ai) {
    failf(ದತ್ತಾಂಶ, "socket failure: %s",
          curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
  }
  *aip = ai;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Bind the socket to a local address and port within the requested range.
 * Falls back to the control-connection address if the user-requested address
 * is non-local.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_bind_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn,
                                     curl_socket_t portsock,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                                     ರಚನೆ Curl_sockaddr_storage *ss,
                                     curl_socklen_t *sslen_io,
                                     ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_min,
                                     ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_max,
                                     bool non_local)
{
  ರಚನೆ sockaddr *sa = (ರಚನೆ sockaddr *)ss;
  ರಚನೆ sockaddr_in * ಸ್ಥಿರಾಂಕ sa4 = (ಶೂನ್ಯ *)sa;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 * ಸ್ಥಿರಾಂಕ sa6 = (ಶೂನ್ಯ *)sa;
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್;
  ಪೂರ್ಣಾಂಕ sockerr;

  ಸ್ಮರಣೆ_ನಕಲು(sa, ai->ai_addr, ai->ai_addrlen);
  *sslen_io = ai->ai_addrlen;

  ಗಾಗಿ(ಪೋರ್ಟ್ = ಪೋರ್ಟ್_min; ಪೋರ್ಟ್ <= ಪೋರ್ಟ್_max;) {
    ಆದರೆ(sa->sa_family == AF_INET)
      sa4->sin_port = htons(ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ
      sa6->sin6_port = htons(ಪೋರ್ಟ್);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(bind(portsock, sa, *sslen_io)) {
      sockerr = SOCKERRNO;
      ಆದರೆ(non_local && (sockerr == SOCKEADDRNOTAVAIL)) {
        /* The requested bind address is not local. Use the address used for
         * the control connection instead and restart the port loop.
         */
        infof(ದತ್ತಾಂಶ, "bind(port=%hu) on non-local address failed: %s", ಪೋರ್ಟ್,
              curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));

        *sslen_io = ಗಾತ್ರ(*ss);
        ಆದರೆ(getsockname(conn->sock[FIRSTSOCKET], sa, sslen_io)) {
          failf(ದತ್ತಾಂಶ, "getsockname() failed: %s",
                curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
          ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
        }
        ಪೋರ್ಟ್ = ಪೋರ್ಟ್_min;
        non_local = FALSE; /* do not try this again */
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(sockerr != SOCKEADDRINUSE && sockerr != SOCKEACCES) {
        failf(ದತ್ತಾಂಶ, "bind(port=%hu) failed: %s", ಪೋರ್ಟ್,
              curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
        ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;

    /* check if port is the maximum value here, because it might be 0xffff
       and then the increment below will wrap the 16-bit counter */
    ಆದರೆ(ಪೋರ್ಟ್ == ಪೋರ್ಟ್_max) {
      failf(ದತ್ತಾಂಶ, "bind() failed, ran out of ports");
      ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
    }
    ಪೋರ್ಟ್++;
  }

  /* re-read the name so we can extract the actual port chosen */
  *sslen_io = ಗಾತ್ರ(*ss);
  ಆದರೆ(getsockname(portsock, sa, sslen_io)) {
    failf(ದತ್ತಾಂಶ, "getsockname() failed: %s",
          curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
  }
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "ftp_port_bind_socket(), socket bound to port %d", ಪೋರ್ಟ್);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Start listening on the data socket.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_listen(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t portsock)
{
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];

  ಆದರೆ(listen(portsock, 1)) {
    failf(ದತ್ತಾಂಶ, "socket failure: %s",
          curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED;
  }
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "ftp_port_listen(), listening on port");
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Send the EPRT or PORT command to the server.
 */
ಸ್ಥಿರ CURLcode ftp_ಪೋರ್ಟ್_send_command(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ftp_conn *ftpc,
                                      ರಚನೆ connectdata *conn,
                                      ರಚನೆ Curl_sockaddr_storage *ss,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                                      ftpport fcmd)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬಹುಲಕ[][5] = { "EPRT", "PORT" };
  ರಚನೆ sockaddr *sa = (ರಚನೆ sockaddr *)ss;
  ರಚನೆ sockaddr_in * ಸ್ಥಿರಾಂಕ sa4 = (ಶೂನ್ಯ *)sa;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 * ಸ್ಥಿರಾಂಕ sa6 = (ಶೂನ್ಯ *)sa;
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ myhost[MAX_IPADR_LEN + 1] = "";
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್;
  CURLcode ಫಲಿತಾಂಶ;

  /* Get a plain printable version of the numerical address to work with. This
     logic uses the address provided by the FTPPORT option, which at times
     might differ from the address in 'ss' used to bind to: when a user asks
     the server to connect to a specific address knowing that it works, but
     curl instead selects to listen to the local address because it cannot use
     the provided address. FTP is strange. */
  Curl_printable_ವಿಳಾಸ(ai, myhost, ಗಾತ್ರ(myhost));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.ftp_use_eprt && conn->ಬಿಟ್ಟುಗಳು.ipv6)
    /* EPRT is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPRT again! */
    conn->ಬಿಟ್ಟುಗಳು.ftp_use_eprt = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

  ಗಾಗಿ(; fcmd != ಮುಗಿದ_2; fcmd++) {

    ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.ftp_use_eprt && (EPRT == fcmd))
      /* if disabled, goto next */
      ಮುಂದುವರಿ;

    ಆದರೆ((ಪೋರ್ಟ್_2 == fcmd) && sa->sa_family != AF_INET)
      /* PORT is IPv4 only */
      ಮುಂದುವರಿ;

    ಆಯ್ಕೆ(sa->sa_family) {
    ಸ್ಥಿತಿ AF_INET:
      ಪೋರ್ಟ್ = ntohs(sa4->sin_port);
      ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಸ್ಥಿತಿ AF_INET6:
      ಪೋರ್ಟ್ = ntohs(sa6->sin6_port);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಮುಂದುವರಿ; /* might as well skip this */
    }

    ಆದರೆ(EPRT == fcmd) {
      /*
       * Two fine examples from RFC2428;
       *
       * EPRT |1|132.235.1.2|6275|
       *
       * EPRT |2|1080::8:800:200C:417A|5282|
       */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s |%d|%s|%hu|", ಬಹುಲಕ[fcmd],
                             sa->sa_family == AF_INET ? 1 : 2, myhost, ಪೋರ್ಟ್);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "Failure sending EPRT command: %s",
              curl_easy_strerror(ಫಲಿತಾಂಶ));
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(ಪೋರ್ಟ್_2 == fcmd) {
      /* large enough for [IP address],[num],[num] */
      ಅಕ್ಷರ ಗುರಿ[ಗಾತ್ರ(myhost) + 20];
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = myhost;
      ಅಕ್ಷರ *dest = ಗುರಿ;

      /* translate x.x.x.x to x,x,x,x */
      ಆಗ(*ಮೂಲ) {
        ಆದರೆ(*ಮೂಲ == '.')
          *dest = ',';
        ಇಲ್ಲದಿದ್ದರೆ
          *dest = *ಮೂಲ;
        dest++;
        ಮೂಲ++;
      }
      *dest = 0;
      curl_msnprintf(dest, 20, ",%d,%d", (ಪೂರ್ಣಾಂಕ)(ಪೋರ್ಟ್ >> 8), (ಪೂರ್ಣಾಂಕ)(ಪೋರ್ಟ್ & 0xff));

      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s %s", ಬಹುಲಕ[fcmd], ಗುರಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "Failure sending PORT command: %s",
              curl_easy_strerror(ಫಲಿತಾಂಶ));
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ನಿಲ್ಲಿಸು;
    }
  }

  /* store which command was sent */
  ftpc->count1 = fcmd;
  ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_ಪೋರ್ಟ್);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * ftp_state_use_port()
 *
 * Set up an active-mode FTP data connection (using PORT or EPRT) and start
 * listening for the server's incoming connection on SECONDARYSOCKET.
 */
ಸ್ಥಿರ CURLcode ftp_state_use_ಪೋರ್ಟ್(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc,
                                   ftpport fcmd) /* start with this */
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_FTP_ಪೋರ್ಟ್_FAILED;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t portsock = CURL_SOCKET_BAD;

  ರಚನೆ Curl_sockaddr_storage ss;
  curl_socklen_t sslen;
  ಅಕ್ಷರ hbuf[NI_MAXHOST];
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string_ftpport = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTPPORT);
  ರಚನೆ Curl_dns_ನಮೂದು *dns_ನಮೂದು = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *res = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_min = 0;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್_max = 0;
  bool non_local = TRUE;

  /* parse the FTPPORT string for address and port range */
  ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_parse_string(ದತ್ತಾಂಶ, conn, string_ftpport,
                                 &ss, &ಪೋರ್ಟ್_min, &ಪೋರ್ಟ್_max,
                                 &host, hbuf, ಗಾತ್ರ(hbuf));
  ಆದರೆ(!ಫಲಿತಾಂಶ && !host)
    /* if no host was specified, use the control connection's local IP */
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_default_host(ದತ್ತಾಂಶ, conn, &ss, &sslen, &host,
                                   hbuf, ಗಾತ್ರ(hbuf), &non_local);

  /* resolve host string to address list */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_resolve_host(ದತ್ತಾಂಶ, conn, host, &dns_ನಮೂದು, &res);

  /* Open a TCP socket for the data connection */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_open_socket(ದತ್ತಾಂಶ, conn, res, &ai, &portsock);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] ftp_state_use_port(), opened socket",
                 FTP_CSTATE(ftpc));

    /* bind to a suitable local address / port */
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_bind_socket(ದತ್ತಾಂಶ, conn, portsock, ai, &ss, &sslen,
                                  ಪೋರ್ಟ್_min, ಪೋರ್ಟ್_max, non_local);
  }

  /* listen */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_listen(ದತ್ತಾಂಶ, portsock);

  /* send the PORT / EPRT command */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ftp_ಪೋರ್ಟ್_send_command(ದತ್ತಾಂಶ, ftpc, conn, &ss, ai, fcmd);

  /* replace any filter on SECONDARY with one listening on this socket */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_tcp_listen_ಗಣ(ದತ್ತಾಂಶ, conn, SECONDARYSOCKET, &portsock);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    portsock = CURL_SOCKET_BAD; /* now held in filter */

  /* cleanup */

  ಆದರೆ(dns_ನಮೂದು)
    Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &dns_ನಮೂದು);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* successfully set up the listen socket filter. SSL needed?
     * Use the control connections origin for cert verification. */
    ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.ftp_use_ದತ್ತಾಂಶ_ssl && ದತ್ತಾಂಶ->ಗಣ.ftp_use_ಪೋರ್ಟ್ &&
       !Curl_conn_is_ssl(conn, SECONDARYSOCKET)) {
      ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
        ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET),
        conn, SECONDARYSOCKET);
    }
    conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು = FALSE;
    Curl_expire(ದತ್ತಾಂಶ, (ದತ್ತಾಂಶ->ಗಣ.accepttimeout > 0) ?
                ದತ್ತಾಂಶ->ಗಣ.accepttimeout: DEFAULT_ACCEPT_TIMEOUT,
                EXPIRE_FTP_ACCEPT);
  }
  ಆದರೆ(portsock != CURL_SOCKET_BAD)
    Curl_socket_close(ದತ್ತಾಂಶ, conn, portsock);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_use_pasv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc,
                                   ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  /* Here's the executive summary on what to do:

     PASV is RFC959, expect:
     227 Entering Passive Mode (a1,a2,a3,a4,p1,p2)

     LPSV is RFC1639, expect:
     228 Entering Long Passive Mode (4,4,a1,a2,a3,a4,2,p1,p2)

     EPSV is RFC2428, expect:
     229 Entering Extended Passive Mode (|||port|)
   */

  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬಹುಲಕ[][5] = { "EPSV", "PASV" };
  ಪೂರ್ಣಾಂಕ modeoff;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PF_INET6
  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.ftp_use_epsv && conn->ಬಿಟ್ಟುಗಳು.ipv6)
    /* EPSV is disabled but we are connected to an IPv6 host, so we ignore the
       request and enable EPSV again! */
    conn->ಬಿಟ್ಟುಗಳು.ftp_use_epsv = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

  modeoff = conn->ಬಿಟ್ಟುಗಳು.ftp_use_epsv ? 0 : 1;

  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", ಬಹುಲಕ[modeoff]);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ftpc->count1 = modeoff;
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PASV);
    infof(ದತ್ತಾಂಶ, "Connect data stream passively");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * ftp_state_prepare_transfer() starts PORT, PASV or PRET etc.
 *
 * REST is the last command in the chain of commands when a "head"-like
 * request is made. Thus, if an actual transfer is to be made this is where we
 * take off for real.
 */
ಸ್ಥಿರ CURLcode ftp_state_prepare_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ftp_conn *ftpc,
                                           ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  ಆದರೆ(ftp->transfer != PPTRANSFER_BODY) {
    /* does not transfer any data */

    /* still possibly do PRE QUOTE jobs */
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR_PREQUOTE);
    ಫಲಿತಾಂಶ = ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_use_ಪೋರ್ಟ್) {
    /* We have chosen to use the PORT (or similar) command */
    ಫಲಿತಾಂಶ = ftp_state_use_ಪೋರ್ಟ್(ದತ್ತಾಂಶ, ftpc, EPRT);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* We have chosen (this is default) to use the PASV (or similar) command */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_use_pret) {
      /* The user has requested that we send a PRET command
         to prepare the server for the upcoming PASV */
      ಆದರೆ(!ftpc->file)
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PRET %s",
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) ?
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) :
                               (ದತ್ತಾಂಶ->state.list_only ? "NLST" : "LIST"));
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PRET STOR %s", ftpc->file);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PRET RETR %s", ftpc->file);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PRET);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = ftp_state_use_pasv(ದತ್ತಾಂಶ, ftpc, conn);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_rest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((ftp->transfer != PPTRANSFER_BODY) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* Determine if server can respond to REST command and therefore
       whether it supports range */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "REST %d", 0);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_REST);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_prepare_transfer(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((ftp->transfer == PPTRANSFER_INFO) && ftpc->file) {
    /* if a "head"-like request is being made (on a file) */

    /* we know ftpc->file is a valid pointer to a filename */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "SIZE %s", ftpc->file);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_ಗಾತ್ರ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_rest(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* If this output is to be machine-parsed, the NLST command might be better
     to use, since the LIST command output is not specified or standard in any
     way. It has turned out that the NLST list output is not the same on all
     servers either... */

  /* if FTPFILE_NOCWD was specified, we should add the path
     as argument for the LIST / NLST / or custom command.
     Whether the server will support this, is uncertain.

     The other ftp_filemethods will CWD into dir/dir/ first and
     then do LIST (in that case: nothing to do here) */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lstArg = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ lstArglen = 0;
  ಅಕ್ಷರ *cmd;

  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD) && ftp->ಮಾರ್ಗ) {
    /* URL-decode before evaluation: e.g. paths starting/ending with %2f */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *raw_ಮಾರ್ಗ = ftpc->rawpath;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *slashPos = strrchr(raw_ಮಾರ್ಗ, '/');
    ಆದರೆ(slashPos) {
      /* chop off the file part if format is dir/file otherwise remove
         the trailing slash for dir/dir/ except for absolute path / */
      size_t n = slashPos - raw_ಮಾರ್ಗ;
      ಆದರೆ(n == 0)
        ++n;

      lstArg = raw_ಮಾರ್ಗ;
      lstArglen = (ಪೂರ್ಣಾಂಕ)n;
    }
  }

  cmd = curl_maprintf("%s%s%.*s",
                      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) ?
                      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) :
                      (ದತ್ತಾಂಶ->state.list_only ? "NLST" : "LIST"),
                      lstArg ? " " : "",
                      lstArglen, lstArg ? lstArg : "");

  ಆದರೆ(!cmd)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", cmd);
  curlx_free(cmd);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_LIST);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_list_prequote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ftp_conn *ftpc,
                                        ರಚನೆ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಹಿಂತಿರುಗಿ ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_LIST_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_retr_prequote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ftp_conn *ftpc,
                                        ರಚನೆ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಹಿಂತಿರುಗಿ ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_RETR_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_stor_prequote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ftp_conn *ftpc,
                                        ರಚನೆ FTP *ftp)
{
  /* We have sent the TYPE, now we must send the list of prequote strings */
  ಹಿಂತಿರುಗಿ ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_STOR_PREQUOTE);
}

ಸ್ಥಿರ CURLcode ftp_state_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* If we have selected NOBODY and HEADER, it means that we only want file
     information. Which in FTP cannot be much more than the file size and
     date. */
  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body && ftpc->file &&
     ftp_need_type(ftpc, (bool)ದತ್ತಾಂಶ->state.prefer_ascii)) {
    /* The SIZE command is _not_ RFC 959 specified, and therefore many servers
       may not support it! It is however the only way we have to get a file's
       size! */

    ftp->transfer = PPTRANSFER_INFO;
    /* this means no actual transfer will be made */

    /* Some servers return different sizes for different modes, and thus we
       must set the proper type before we check the size */
    ಫಲಿತಾಂಶ = ftp_nb_type(ದತ್ತಾಂಶ, ftpc, ftp, (bool)ದತ್ತಾಂಶ->state.prefer_ascii,
                         FTP_TYPE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_ಗಾತ್ರ(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* This is called after the CWD commands have been done in the beginning of
   the DO phase */
ಸ್ಥಿರ CURLcode ftp_state_mdtm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Requested time of file or time-depended transfer? */
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.get_filetime || ದತ್ತಾಂಶ->ಗಣ.timecondition) && ftpc->file) {

    /* we have requested to get the modified-time of the file, this is a white
       spot as the MDTM is not mentioned in RFC959 */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "MDTM %s", ftpc->file);

    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_MDTM);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_type(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* This is called after the TYPE and possible quote commands have been sent */
ಸ್ಥಿರ CURLcode ftp_state_ul_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc,
                                   ರಚನೆ FTP *ftp,
                                   bool sizechecked)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_ಬಿಟ್ append = ದತ್ತಾಂಶ->ಗಣ.remote_append;

  ಆದರೆ((ದತ್ತಾಂಶ->state.resume_from && !sizechecked) ||
     ((ದತ್ತಾಂಶ->state.resume_from > 0) && sizechecked)) {
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
    ಪೂರ್ಣಾಂಕ seekerr = CURL_SEEKFUNC_OK;

    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
      /* Got no given size to start from, figure it out */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "SIZE %s", ftpc->file);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOR_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* enable append */
    append = TRUE;

    /* Let's read off the proper amount of bytes from the input. */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.seek_func) {
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_seek_func);
      seekerr = ದತ್ತಾಂಶ->ಗಣ.seek_func(ದತ್ತಾಂಶ->ಗಣ.seek_client,
                                    ದತ್ತಾಂಶ->state.resume_from, SEEK_SET);
      CURL_CBAPI_ಕೊನೆ(&guard);
    }

    ಆದರೆ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;
      ಆದರೆ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ದತ್ತಾಂಶ, "Could not seek stream");
        ಹಿಂತಿರುಗಿ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಮಾಡು {
        ಅಕ್ಷರ scratch[4 * 1024];
        size_t readthisamountnow =
          (ದತ್ತಾಂಶ->state.resume_from - passed > (curl_off_t)ಗಾತ್ರ(scratch)) ?
          ಗಾತ್ರ(scratch) :
          curlx_sotouz(ದತ್ತಾಂಶ->state.resume_from - passed);

        size_t actuallyread =
          ದತ್ತಾಂಶ->state.fread_func(scratch, 1, readthisamountnow,
                                 ದತ್ತಾಂಶ->state.ಒಳ);

        passed += actuallyread;
        ಆದರೆ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ದತ್ತಾಂಶ, "Failed to read data");
          ಹಿಂತಿರುಗಿ CURLE_FTP_COULDNT_USE_REST;
        }
      } ಆಗ(passed < ದತ್ತಾಂಶ->state.resume_from);
    }
    /* now, decrease the size of the read */
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize > 0) {
      ದತ್ತಾಂಶ->state.infilesize -= ದತ್ತಾಂಶ->state.resume_from;

      ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize <= 0) {
        infof(ದತ್ತಾಂಶ, "File already completely uploaded");

        /* no data to transfer */
        Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);

        /* Set ->transfer so that we will not get any error in
         * ftp_done() because we did not transfer anything! */
        ftp->transfer = PPTRANSFER_NONE;

        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }
    /* we have passed, proceed as normal */
  } /* resume_from */

  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, append ? "APPE %s" : "STOR %s",
                         ftpc->file);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOR);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_retr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ftp_conn *ftpc,
                               ರಚನೆ FTP *ftp,
                               curl_off_t filesize)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] ftp_state_retr()", FTP_CSTATE(ftpc));
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize && (filesize > ದತ್ತಾಂಶ->ಗಣ.max_filesize)) {
    failf(ದತ್ತಾಂಶ, "Maximum file size exceeded");
    ಹಿಂತಿರುಗಿ CURLE_FILESIZE_EXCEEDED;
  }
  ftp->downloadsize = filesize;

  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    /* We always (attempt to) get the size of downloads, so it is done before
       this even when not doing resumes. */
    ಆದರೆ(filesize == -1) {
      infof(ದತ್ತಾಂಶ, "ftp server does not support SIZE");
      /* We could not get the size and therefore we cannot know if there
         really is a part of the file left to get, although the server will
         close the connection when we start the connection so it will not
         cause us any harm, not make us exit as nicely. */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* We got a file size report, so we check that there actually is a
         part of the file left to get, or else we go home. */
      ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
        /* We are supposed to download the last abs(from) bytes */
        ಆದರೆ(filesize < -ದತ್ತಾಂಶ->state.resume_from) {
          failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ದತ್ತಾಂಶ->state.resume_from, filesize);
          ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* convert to size to download */
        ftp->downloadsize = -ದತ್ತಾಂಶ->state.resume_from;
        /* download from where? */
        ದತ್ತಾಂಶ->state.resume_from = filesize - ftp->downloadsize;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(filesize < ದತ್ತಾಂಶ->state.resume_from) {
          failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T
                ") was beyond file size (%" FMT_OFF_T ")",
                ದತ್ತಾಂಶ->state.resume_from, filesize);
          ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
        }
        /* Now store the number of bytes we are expected to download */
        ftp->downloadsize = filesize - ದತ್ತಾಂಶ->state.resume_from;
      }
    }

    ಆದರೆ(ftp->downloadsize == 0) {
      /* no data to transfer */
      Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
      infof(ದತ್ತಾಂಶ, "File already completely downloaded");

      /* Set ->transfer so that we will not get any error in ftp_done()
       * because we did not transfer the any file */
      ftp->transfer = PPTRANSFER_NONE;
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    /* Set resume file transfer offset */
    infof(ದತ್ತಾಂಶ, "Instructs server to resume from offset %" FMT_OFF_T,
          ದತ್ತಾಂಶ->state.resume_from);

    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "REST %" FMT_OFF_T,
                           ದತ್ತಾಂಶ->state.resume_from);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR_REST);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* no resume */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "RETR %s", ftpc->file);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_quote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ftp_conn *ftpc,
                                ರಚನೆ FTP *ftp,
                                bool init,
                                ftpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool quote = FALSE;
  ರಚನೆ curl_slist *ಅಂಶ;

  ಆಯ್ಕೆ(instate) {
  ಸ್ಥಿತಿ FTP_QUOTE:
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.quote;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ FTP_RETR_PREQUOTE:
  ಸ್ಥಿತಿ FTP_STOR_PREQUOTE:
  ಸ್ಥಿತಿ FTP_LIST_PREQUOTE:
    ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.prequote;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ FTP_POSTQUOTE:
    ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.postquote;
    ನಿಲ್ಲಿಸು;
  }

  /*
   * This state uses:
   * 'count1' to iterate over the commands to send
   * 'count2' to store whether to allow commands to fail
   */

  ಆದರೆ(init)
    ftpc->count1 = 0;
  ಇಲ್ಲದಿದ್ದರೆ
    ftpc->count1++;

  ಆದರೆ(ಅಂಶ) {
    ಪೂರ್ಣಾಂಕ i = 0;

    /* Skip count1 items in the linked list */
    ಆಗ((i < ftpc->count1) && ಅಂಶ) {
      ಅಂಶ = ಅಂಶ->ಮುಂದಿನ;
      i++;
    }
    ಆದರೆ(ಅಂಶ) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd = ಅಂಶ->ದತ್ತಾಂಶ;
      ಆದರೆ(cmd[0] == '*') {
        cmd++;
        ftpc->count2 = 1; /* the sent command is allowed to fail */
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ftpc->count2 = 0; /* failure means cancel operation */

      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", cmd);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ftp_state(ದತ್ತಾಂಶ, ftpc, instate);
      quote = TRUE;
    }
  }

  ಆದರೆ(!quote) {
    /* No more quote to send, continue to ... */
    ಆಯ್ಕೆ(instate) {
    ಸ್ಥಿತಿ FTP_QUOTE:
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = ftp_state_cwd(ದತ್ತಾಂಶ, ftpc, ftp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ FTP_RETR_PREQUOTE:
      ಆದರೆ(ftp->transfer != PPTRANSFER_BODY)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ftpc->ತಿಳಿದ_filesize != -1) {
          Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ftpc->ತಿಳಿದ_filesize);
          ಫಲಿತಾಂಶ = ftp_state_retr(ದತ್ತಾಂಶ, ftpc, ftp, ftpc->ತಿಳಿದ_filesize);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ignorecl || ದತ್ತಾಂಶ->state.prefer_ascii) {
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
            ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "RETR %s", ftpc->file);
            ಆದರೆ(!ಫಲಿತಾಂಶ)
              ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR);
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "SIZE %s", ftpc->file);
            ಆದರೆ(!ಫಲಿತಾಂಶ)
              ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR_ಗಾತ್ರ);
          }
        }
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ FTP_STOR_PREQUOTE:
      ಫಲಿತಾಂಶ = ftp_state_ul_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, ftpc, ftp, FALSE);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ FTP_POSTQUOTE:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ FTP_LIST_PREQUOTE:
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_LIST_TYPE);
      ಫಲಿತಾಂಶ = ftp_state_list(ದತ್ತಾಂಶ, ftpc, ftp);
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called from ftp_state_pasv_resp to switch to PASV in case of EPSV
   problems */
ಸ್ಥಿರ CURLcode ftp_epsv_disable(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ftp_conn *ftpc,
                                 ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.ipv6 && !Curl_conn_is_tunneling(conn, FIRSTSOCKET)) {
    /* We cannot disable EPSV when doing IPv6, so this is instead a fail */
    failf(ದತ್ತಾಂಶ, "Failed EPSV attempt, exiting");
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  infof(ದತ್ತಾಂಶ, "Failed EPSV attempt. Disabling EPSV");
  /* disable it for next transfer */
  conn->ಬಿಟ್ಟುಗಳು.ftp_use_epsv = FALSE;
  close_secondarysocket(ದತ್ತಾಂಶ, ftpc);
  ದತ್ತಾಂಶ->state.errorbuf = FALSE; /* allow error message to get
                                         rewritten */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "PASV");
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ftpc->count1++;
    /* remain in/go to the FTP_PASV state */
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PASV);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_control_addr_dup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ **newhostp)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ip_quadruple ipquad;
  bool is_ipv6;

  /* Returns the control connection IP address.
     If a proxy tunnel is used, returns the original hostname instead, because
     the effective control connection address is the proxy address,
     not the ftp host. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(Curl_conn_is_tunneling(conn, FIRSTSOCKET))
    *newhostp = curlx_strdup(conn->origin->hostname);
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!Curl_conn_get_ip_info(ದತ್ತಾಂಶ, conn, FIRSTSOCKET, &is_ipv6, &ipquad) &&
     *ipquad.remote_ip)
    *newhostp = curlx_strdup(ipquad.remote_ip);
  ಇಲ್ಲದಿದ್ದರೆ {
    /* failed to get the remote_ip of the DATA connection */
    failf(ದತ್ತಾಂಶ, "unable to get peername of DATA connection");
    *newhostp = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_FTP_CANT_GET_HOST;
  }
  ಹಿಂತಿರುಗಿ *newhostp ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

ಸ್ಥಿರ bool ಹೊಂದಿಕೆ_pasv_6nums(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *array) /* 6 numbers */
{
  ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; i < 6; i++) {
    curl_off_t ಅಂಕೆ;
    ಆದರೆ(i) {
      ಆದರೆ(*p != ',')
        ಹಿಂತಿರುಗಿ FALSE;
      p++;
    }
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, 0xff))
      ಹಿಂತಿರುಗಿ FALSE;
    array[i] = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode ftp_state_pasv_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ pingpong *pp = &ftpc->pp;
  ಅಕ್ಷರ *newhost = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ newport = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                        letter */
  ಆದರೆ((ftpc->count1 == 0) &&
     (ftpcode == 229)) {
    /* positive EPSV response */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = strchr(str, '(');
    ಆದರೆ(ptr) {
      ಅಕ್ಷರ sep;
      ptr++;
      /* |||12345| */
      sep = ptr[0];
      ಆದರೆ(sep && (ptr[1] == sep) && (ptr[2] == sep) && ISDIGIT(ptr[3])) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &ptr[3];
        curl_off_t ಅಂಕೆ;
        ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, 0xffff) || (*p != sep)) {
          failf(ದತ್ತಾಂಶ, "Illegal port number in EPSV reply");
          ಹಿಂತಿರುಗಿ CURLE_FTP_WEIRD_PASV_ಉತ್ತರ;
        }
        newport = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಅಂಕೆ;
        ಫಲಿತಾಂಶ = ftp_control_addr_dup(ದತ್ತಾಂಶ, &newhost);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ptr = ಖಾಲಿ;
    }
    ಆದರೆ(!ptr) {
      failf(ದತ್ತಾಂಶ, "Weirdly formatted EPSV reply");
      ಹಿಂತಿರುಗಿ CURLE_FTP_WEIRD_PASV_ಉತ್ತರ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ftpc->count1 == 1) &&
          (ftpcode == 227)) {
    /* positive PASV response */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ip[6];

    /*
     * Scan for a sequence of six comma-separated numbers and use them as
     * IP+port indicators.
     *
     * Found reply-strings include:
     * "227 Entering Passive Mode (127,0,0,1,4,51)"
     * "227 Data transfer will passively listen to 127,0,0,1,4,51"
     * "227 Entering passive mode. 127,0,0,1,4,51"
     */
    ಆಗ(*str) {
      ಆದರೆ(ಹೊಂದಿಕೆ_pasv_6nums(str, ip))
        ನಿಲ್ಲಿಸು;
      str++;
    }

    ಆದರೆ(!*str) {
      failf(ದತ್ತಾಂಶ, "Could not interpret the 227-response");
      ಹಿಂತಿರುಗಿ CURLE_FTP_WEIRD_227_FORMAT;
    }

    /* we got OK from server */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_skip_ip) {
      /* told to ignore the remotely given IP but instead use the host we used
         for the control connection */
      infof(ದತ್ತಾಂಶ, "Skip %u.%u.%u.%u for data connection, reuse %s instead",
            ip[0], ip[1], ip[2], ip[3], conn->origin->hostname);
      ಫಲಿತಾಂಶ = ftp_control_addr_dup(ದತ್ತಾಂಶ, &newhost);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      newhost = curl_maprintf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    ಆದರೆ(!newhost)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    newport = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)(((ip[4] << 8) + ip[5]) & 0xffff);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpc->count1 == 0) {
    /* EPSV failed, move on to PASV */
    ಹಿಂತಿರುಗಿ ftp_epsv_disable(ದತ್ತಾಂಶ, ftpc, conn);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Bad PASV/EPSV response: %03d", ftpcode);
    ಹಿಂತಿರುಗಿ CURLE_FTP_WEIRD_PASV_ಉತ್ತರ;
  }

  DEBUGASSERT(newhost);
  Curl_peer_unlink(&conn->origin2);
  ಫಲಿತಾಂಶ = Curl_peer_create(ದತ್ತಾಂಶ, conn->scheme, newhost, newport,
                            &conn->origin2);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  /* If FIRSTSOCKET goes via another peer, SECONDARY needs as well,
   * but with its new port. */
  ಆದರೆ(conn->ಮೂಲಕ_peer) {
    Curl_peer_unlink(&conn->ಮೂಲಕ_peer2);
    ಫಲಿತಾಂಶ = Curl_peer_create(ದತ್ತಾಂಶ, conn->ಮೂಲಕ_peer->scheme,
                              conn->ಮೂಲಕ_peer->hostname, newport,
                              &conn->ಮೂಲಕ_peer2);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
  }

  ಫಲಿತಾಂಶ = Curl_conn_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, conn, SECONDARYSOCKET,
                           conn->ಬಿಟ್ಟುಗಳು.ftp_use_ದತ್ತಾಂಶ_ssl ?
                           CURL_CF_SSL_ENABLE : CURL_CF_SSL_DISABLE);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ((ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) &&
       (ftpc->count1 == 0) && (ftpcode == 229)) {
      ಫಲಿತಾಂಶ = ftp_epsv_disable(ದತ್ತಾಂಶ, ftpc, conn);
    }
    ಹೋಗು error;
  }

  /*
   * When this is used from the multi interface, this might have returned with
   * the 'connected' set to FALSE and thus we are now awaiting a non-blocking
   * connect to connect.
   */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose) {
    /* Dump information about this second connection when we have issued
     * a PASV command. */
    infof(ದತ್ತಾಂಶ, "Connecting to %s port %d",
          conn->origin2->hostname, conn->origin2->ಪೋರ್ಟ್);
  }
#ಆದರೆ_ಅಂತ್ಯ

  conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು = TRUE;
  ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* this phase is completed */

error:
  curlx_free(newhost);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ftp_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ftp_conn *ftpc,
                              bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &ftpc->pp, FALSE, FALSE);

  /* Check for the state outside of the Curl_socket_check() return code checks
     since at times we are in fact already in this state when this function
     gets called. */
  *ಮುಗಿದ = (ftpc->state == FTP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_do_ಇನ್ನಷ್ಟು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, domore *ಇನ್ನಷ್ಟು)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ರಚನೆ FTP *ftp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FTP_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected = FALSE;
  bool complete = FALSE;
  /* the ftp struct is inited in ftp_connect(). If we are connecting to an HTTP
   * proxy then the state will not be valid until after that connection is
   * complete */

  ಆದರೆ(!ftpc || !ftp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  *ಇನ್ನಷ್ಟು = DOMORE_INCOMPLETE; /* default to stay in the state */

  /* if the second connection has been set up, try to connect it fully
   * to the remote host. This may not complete at this time, for several
   * reasons:
   * - we do EPTR and the server will not connect to our listen socket
   *   until we send more FTP commands
   * - an SSL filter is in place and the server will not start the TLS
   *   handshake until we send more FTP commands
   */
  ಆದರೆ(conn->cfilter[SECONDARYSOCKET]) {
    bool is_eptr = Curl_conn_is_tcp_listen(ದತ್ತಾಂಶ, SECONDARYSOCKET);
    ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, SECONDARYSOCKET, FALSE, &connected);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(ಫಲಿತಾಂಶ || (!connected && !is_eptr &&
                  !Curl_conn_is_ip_connected(ದತ್ತಾಂಶ, SECONDARYSOCKET))) {
      ಆದರೆ(ಫಲಿತಾಂಶ && !is_eptr && (ftpc->count1 == 0)) {
        *ಇನ್ನಷ್ಟು = DOMORE_GOBACK; /* go back to DOING please */
        /* this is a EPSV connect failing, try PASV instead */
        ಹಿಂತಿರುಗಿ ftp_epsv_disable(ದತ್ತಾಂಶ, ftpc, conn);
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆದರೆ(ftpc->state) {
    /* already in a state so skip the initial commands.
       They are only done to kickstart the do_more state */
    ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, &complete);

    ಆದರೆ(complete)
      *ಇನ್ನಷ್ಟು = DOMORE_ಮುಗಿದ;

    /* if we got an error or if we do not wait for a data connection return
       immediately */
    ಆದರೆ(ಫಲಿತಾಂಶ || !ftpc->wait_ದತ್ತಾಂಶ_conn)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* if we reach the end of the FTP state machine here, *complete will be
       TRUE but so is ftpc->wait_data_conn, which says we need to wait for the
       data connection and therefore we are not actually complete */
    *ಇನ್ನಷ್ಟು = DOMORE_INCOMPLETE;
  }

  ಆದರೆ(ftp->transfer <= PPTRANSFER_INFO) {
    /* a transfer is about to take place, or if not a filename was given so we
       will do a SIZE on it later and then we need the right TYPE first */

    ಆದರೆ(ftpc->wait_ದತ್ತಾಂಶ_conn) {
      bool serv_conned;

      ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, SECONDARYSOCKET, FALSE, &serv_conned);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; /* Failed to accept data connection */

      ಆದರೆ(serv_conned) {
        /* It looks data connection is established */
        ftpc->wait_ದತ್ತಾಂಶ_conn = FALSE;
        ಫಲಿತಾಂಶ = ftp_initiate_transfer(ದತ್ತಾಂಶ, ftpc);

        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        *ಇನ್ನಷ್ಟು = DOMORE_ಮುಗಿದ; /* this state is now complete when the server has
                                connected back to us */
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = ftp_check_ctrl_on_ದತ್ತಾಂಶ_wait(ದತ್ತಾಂಶ, ftpc);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
      ಫಲಿತಾಂಶ = ftp_nb_type(ದತ್ತಾಂಶ, ftpc, ftp, (bool)ದತ್ತಾಂಶ->state.prefer_ascii,
                           FTP_STOR_TYPE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, &complete);
      /* ftp_nb_type() might have skipped sending `TYPE A|I` when not
       * deemed necessary and directly sent `STORE name`. If this was
       * then complete, but we are still waiting on the data connection,
       * the transfer has not been initiated yet. */
      *ಇನ್ನಷ್ಟು = (!ftpc->wait_ದತ್ತಾಂಶ_conn && complete) ?
        DOMORE_ಮುಗಿದ : DOMORE_INCOMPLETE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* download */
      ftp->downloadsize = -1; /* unknown as of yet */

      ಫಲಿತಾಂಶ = Curl_range(ದತ್ತಾಂಶ);

      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK && ದತ್ತಾಂಶ->req.maxdownload >= 0) {
        /* Do not check for successful transfer */
        ftpc->dont_check = TRUE;
      }

      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಆದರೆ((ದತ್ತಾಂಶ->state.list_only || !ftpc->file) &&
         !ದತ್ತಾಂಶ->ಗಣ.prequote) {
        /* The specified path ends with a slash, and therefore we think this
           is a directory that is requested, use LIST. Before that, we also
           need to set ASCII transfer mode. */

        /* Only if a body transfer was requested. */
        ಆದರೆ(ftp->transfer == PPTRANSFER_BODY) {
          ಫಲಿತಾಂಶ = ftp_nb_type(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_LIST_TYPE);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        /* otherwise fall through */
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.prequote && !ftpc->file) {
          ಫಲಿತಾಂಶ = ftp_nb_type(ದತ್ತಾಂಶ, ftpc, ftp, TRUE,
                               FTP_RETR_LIST_TYPE);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಫಲಿತಾಂಶ = ftp_nb_type(ದತ್ತಾಂಶ, ftpc, ftp, (bool)ದತ್ತಾಂಶ->state.prefer_ascii,
                               FTP_RETR_TYPE);
        }
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }

      ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, &complete);
      ಆದರೆ(complete)
        *ಇನ್ನಷ್ಟು = DOMORE_ಮುಗಿದ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* no data to transfer */
  Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);

  ಆದರೆ(!ftpc->wait_ದತ್ತಾಂಶ_conn) {
    /* no waiting for the data connection so this is now complete */
    *ಇನ್ನಷ್ಟು = DOMORE_ಮುಗಿದ;
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] DO-MORE phase ends with %d", FTP_CSTATE(ftpc),
                 (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode ftp_dophase_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ftp_conn *ftpc,
                                 ರಚನೆ FTP *ftp,
                                 bool connected)
{
  ಆದರೆ(connected) {
    domore completed;
    CURLcode ಫಲಿತಾಂಶ = ftp_do_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ, &completed);

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      close_secondarysocket(ದತ್ತಾಂಶ, ftpc);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆದರೆ(ftp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!connected)
    /* since we did not connect now, we want do_more to get called */
    ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು = TRUE;

  ftpc->ctl_valid = TRUE; /* seems good */

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ftp_state_ಪೋರ್ಟ್_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ರಚನೆ FTP *ftp,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ftpport fcmd = (ftpport)ftpc->count1;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* The FTP spec tells a positive response should have code 200.
     Be more permissive here to tolerate deviant servers. */
  ಆದರೆ(ftpcode / 100 != 2) {
    /* the command failed */

    ಆದರೆ(EPRT == fcmd) {
      infof(ದತ್ತಾಂಶ, "disabling EPRT usage");
      conn->ಬಿಟ್ಟುಗಳು.ftp_use_eprt = FALSE;
    }
    fcmd++;

    ಆದರೆ(fcmd == ಮುಗಿದ_2) {
      failf(ದತ್ತಾಂಶ, "Failed to do PORT");
      ಫಲಿತಾಂಶ = CURLE_FTP_ಪೋರ್ಟ್_FAILED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* try next */
      ಫಲಿತಾಂಶ = ftp_state_use_ಪೋರ್ಟ್(ದತ್ತಾಂಶ, ftpc, fcmd);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    infof(ದತ್ತಾಂಶ, "Connect data stream actively");
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* end of DO phase */
    ಫಲಿತಾಂಶ = ftp_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, ftpc, ftp, FALSE);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* return TRUE on error, FALSE on success */
ಸ್ಥಿರ bool twodigit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ *val)
{
  ಆದರೆ(!ISDIGIT(p[0]) || !ISDIGIT(p[1]))
    ಹಿಂತಿರುಗಿ TRUE;
  /* curlx_hexval() works fine here since we make sure it is decimal above */
  *val = (curlx_hexval(p[0]) * 10) + curlx_hexval(p[1]);
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * @unittest 1668
 */
UNITTEST bool ftp_213_date(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ *year, ಪೂರ್ಣಾಂಕ *month, ಪೂರ್ಣಾಂಕ *day,
                           ಪೂರ್ಣಾಂಕ *hour, ಪೂರ್ಣಾಂಕ *minute, ಪೂರ್ಣಾಂಕ *ಎರಡನೇ);
UNITTEST bool ftp_213_date(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ *year, ಪೂರ್ಣಾಂಕ *month, ಪೂರ್ಣಾಂಕ *day,
                           ಪೂರ್ಣಾಂಕ *hour, ಪೂರ್ಣಾಂಕ *minute, ಪೂರ್ಣಾಂಕ *ಎರಡನೇ)
{
  ಪೂರ್ಣಾಂಕ century;
  ಆದರೆ((ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) < 14) || twodigit(&p[0], &century) || twodigit(&p[2], year) ||
     twodigit(&p[4], month) || twodigit(&p[6], day) ||
     twodigit(&p[8], hour) || twodigit(&p[10], minute) ||
     twodigit(&p[12], ಎರಡನೇ))
    ಹಿಂತಿರುಗಿ FALSE;

  *year += century * 100;
  ಆದರೆ((*month > 12) || (*day > 31) || (*hour > 23) || (*minute > 59) ||
     (*ಎರಡನೇ > 60))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode client_write_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಅಕ್ಷರ *buf, size_t blen)
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
  CURLcode ಫಲಿತಾಂಶ;
  bool save = (bool)ದತ್ತಾಂಶ->ಗಣ.include_ಶೀರ್ಷಿಕೆ;
  ದತ್ತಾಂಶ->ಗಣ.include_ಶೀರ್ಷಿಕೆ = TRUE;
  ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, buf, blen);
  ದತ್ತಾಂಶ->ಗಣ.include_ಶೀರ್ಷಿಕೆ = save;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_mdtm_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ರಚನೆ FTP *ftp,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(ftpcode) {
  ಸ್ಥಿತಿ 213: {
    /* we got a time. Format should be: "YYYYMMDDHHMMSS[.sss]" where the
       last .sss part is optional and means fractions of a second */
    ಪೂರ್ಣಾಂಕ year, month, day, hour, minute, ಎರಡನೇ;
    ರಚನೆ pingpong *pp = &ftpc->pp;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *resp = curlx_dyn_ptr(&pp->recvbuf) + 4;
    bool showtime = FALSE;
    ಆದರೆ(ftp_213_date(resp, &year, &month, &day, &hour, &minute, &ಎರಡನೇ)) {
      /* we have a time, reformat it */
      ಅಕ್ಷರ timebuf[24];
      curl_msnprintf(timebuf, ಗಾತ್ರ(timebuf),
                     "%04d%02d%02d %02d:%02d:%02d GMT",
                     year, month, day, hour, minute, ಎರಡನೇ);
      /* now, convert this into a time() value: */
      ಆದರೆ(!Curl_getdate_capped(timebuf, &ದತ್ತಾಂಶ->info.filetime))
        showtime = TRUE;
    }

    /* If we asked for a time of the file and we actually got one as well,
       we "emulate" an HTTP-style header in our output. */

#ಆದರೆ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
/* 'time_t' is unsigned in MSDOS and AmigaOS. Silence:
   warning: comparison of unsigned expression in '>= 0' is always true */
#ಸೂಚನೆ GCC diagnostic ignored "-Wtype-limits"
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ದತ್ತಾಂಶ->req.no_body && ftpc->file &&
       ದತ್ತಾಂಶ->ಗಣ.get_filetime && showtime) {
#ಆದರೆ defined(CURL_HAVE_DIAG) && (defined(__DJGPP__) || defined(__AMIGA__))
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಅಕ್ಷರ headerbuf[128];
      ಪೂರ್ಣಾಂಕ headerbuflen;
      time_t filetime = ದತ್ತಾಂಶ->info.filetime;
      ರಚನೆ tm ಬಫರ್;
      ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm = &ಬಫರ್;

      ಫಲಿತಾಂಶ = curlx_gmtime(filetime, &ಬಫರ್);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      /* format: "Tue, 15 Nov 1994 12:45:26" */
      headerbuflen =
        curl_msnprintf(headerbuf, ಗಾತ್ರ(headerbuf),
                       "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d "
                       "GMT\r\n",
                       Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                       tm->tm_mday,
                       Curl_month[tm->tm_mon],
                       tm->tm_year + 1900,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec);
      ಫಲಿತಾಂಶ = client_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, headerbuf, headerbuflen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    } /* end of a ridiculous amount of conditionals */
  }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    infof(ದತ್ತಾಂಶ, "unsupported MDTM reply format");
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 550: /* 550 is used for several different problems, e.g.
               "No such file or directory" or "Permission denied".
               It does not mean that the file does not exist at all. */
    infof(ದತ್ತಾಂಶ, "MDTM failed: file does not exist or permission problem,"
          " continuing");
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition) {
    ಆದರೆ((ದತ್ತಾಂಶ->info.filetime > 0) && (ದತ್ತಾಂಶ->ಗಣ.timevalue > 0)) {
      ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition) {
      ಸ್ಥಿತಿ CURL_TIMECOND_IFMODSINCE:
      ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ(ದತ್ತಾಂಶ->info.filetime <= ದತ್ತಾಂಶ->ಗಣ.timevalue) {
          infof(ದತ್ತಾಂಶ, "The requested document is not new enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ದತ್ತಾಂಶ->info.timecond = TRUE;
          ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_TIMECOND_IFUNMODSINCE:
        ಆದರೆ(ದತ್ತಾಂಶ->info.filetime > ದತ್ತಾಂಶ->ಗಣ.timevalue) {
          infof(ದತ್ತಾಂಶ, "The requested document is not old enough");
          ftp->transfer = PPTRANSFER_NONE; /* mark to not transfer data */
          ದತ್ತಾಂಶ->info.timecond = TRUE;
          ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ನಿಲ್ಲಿಸು;
      } /* switch */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "Skipping time comparison");
    }
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ftp_state_type(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_type_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ರಚನೆ FTP *ftp,
                                    ಪೂರ್ಣಾಂಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ftpcode / 100 != 2) {
    /* "sasserftpd" and "(u)r(x)bot ftpd" both responds with 226 after a
       successful 'TYPE I'. While that is not as RFC959 says, it is still a
       positive response code and we allow that. */
    failf(ದತ್ತಾಂಶ, "Could not set desired mode");
    ಹಿಂತಿರುಗಿ CURLE_FTP_COULDNT_ಗಣ_TYPE;
  }
  ಆದರೆ(ftpcode != 200)
    infof(ದತ್ತಾಂಶ, "Got a %03d response code instead of the assumed 200",
          ftpcode);

  ಆದರೆ(instate == FTP_TYPE)
    ಫಲಿತಾಂಶ = ftp_state_ಗಾತ್ರ(ದತ್ತಾಂಶ, ftpc, ftp);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_LIST_TYPE)
    ಫಲಿತಾಂಶ = ftp_state_list(ದತ್ತಾಂಶ, ftpc, ftp);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_RETR_TYPE)
    ಫಲಿತಾಂಶ = ftp_state_retr_prequote(ದತ್ತಾಂಶ, ftpc, ftp);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_STOR_TYPE)
    ಫಲಿತಾಂಶ = ftp_state_stor_prequote(ದತ್ತಾಂಶ, ftpc, ftp);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_RETR_LIST_TYPE)
    ಫಲಿತಾಂಶ = ftp_state_list_prequote(ದತ್ತಾಂಶ, ftpc, ftp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_ಗಾತ್ರ_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ರಚನೆ FTP *ftp,
                                    ಪೂರ್ಣಾಂಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_off_t filesize = -1;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
  size_t len = ftpc->pp.nfinal;

  /* get the size from the ascii string: */
  ಆದರೆ(ftpcode == 213) {
    /* To allow servers to prepend "rubbish" in the response string, we scan
       for all the digits at the end of the response and parse only those as a
       number. */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ = &buf[4];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdigit = memchr(ಆರಂಭ, '\r', len - 4);
    ಆದರೆ(fdigit) {
      fdigit--;
      ಆದರೆ(*fdigit == '\n')
        fdigit--;
      ಆಗ(ISDIGIT(fdigit[-1]) && (fdigit > ಆರಂಭ))
        fdigit--;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      fdigit = ಆರಂಭ;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&fdigit, &filesize, CURL_OFF_T_MAX))
      filesize = -1; /* size remain unknown */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpcode == 550 && /* "No such file or directory" */
          /* allow a SIZE failure for (resumed) uploads, when probing what
             command to use */
          instate != FTP_STOR_ಗಾತ್ರ) {
    failf(ದತ್ತಾಂಶ, "The file does not exist");
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;
  }

  ಆದರೆ(instate == FTP_ಗಾತ್ರ) {
    ಆದರೆ(filesize != -1) {
      ಅಕ್ಷರ clbuf[128];
      ಪೂರ್ಣಾಂಕ clbuflen = curl_msnprintf(clbuf, ಗಾತ್ರ(clbuf),
                                    "Content-Length: %" FMT_OFF_T "\r\n",
                                    filesize);
      ಫಲಿತಾಂಶ = client_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, clbuf, clbuflen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, filesize);
    ಫಲಿತಾಂಶ = ftp_state_rest(ದತ್ತಾಂಶ, ftpc, ftp);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_RETR_ಗಾತ್ರ) {
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, filesize);
    ಫಲಿತಾಂಶ = ftp_state_retr(ದತ್ತಾಂಶ, ftpc, ftp, filesize);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(instate == FTP_STOR_ಗಾತ್ರ) {
    ದತ್ತಾಂಶ->state.resume_from = filesize;
    ಫಲಿತಾಂಶ = ftp_state_ul_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, ftpc, ftp, TRUE);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_rest_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ರಚನೆ FTP *ftp,
                                    ಪೂರ್ಣಾಂಕ ftpcode,
                                    ftpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(instate) {
  ಸ್ಥಿತಿ FTP_REST:
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(ftpcode == 350) {
      ಅಕ್ಷರ ಬಫರ್[24] = { "Accept-ranges: bytes\r\n" };
      ಫಲಿತಾಂಶ = client_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, ಬಫರ್, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಫರ್));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಫಲಿತಾಂಶ = ftp_state_prepare_transfer(ದತ್ತಾಂಶ, ftpc, ftp);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_RETR_REST:
    ಆದರೆ(ftpcode != 350) {
      failf(ದತ್ತಾಂಶ, "Could not use REST");
      ಫಲಿತಾಂಶ = CURLE_FTP_COULDNT_USE_REST;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "RETR %s", ftpc->file);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_RETR);
    }
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_state_stor_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ftpcode >= 400) {
    failf(ದತ್ತಾಂಶ, "Failed FTP upload: %0d", ftpcode);
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
    ಹಿಂತಿರುಗಿ CURLE_UPLOAD_FAILED;
  }

  /* PORT means we are now awaiting the server to connect to us. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_use_ಪೋರ್ಟ್) {
    bool connected;

    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* no longer in STOR state */

    ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, SECONDARYSOCKET, FALSE, &connected);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ(!connected) {
      infof(ದತ್ತಾಂಶ, "Data conn was not available immediately");
      ftpc->wait_ದತ್ತಾಂಶ_conn = TRUE;
      ಹಿಂತಿರುಗಿ ftp_check_ctrl_on_ದತ್ತಾಂಶ_wait(ದತ್ತಾಂಶ, ftpc);
    }
    ftpc->wait_ದತ್ತಾಂಶ_conn = FALSE;
  }
  ಹಿಂತಿರುಗಿ ftp_initiate_transfer(ದತ್ತಾಂಶ, ftpc);
}

/* for LIST and RETR responses */
ಸ್ಥಿರ CURLcode ftp_state_get_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc,
                                   ರಚನೆ FTP *ftp,
                                   ಪೂರ್ಣಾಂಕ ftpcode,
                                   ftpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((ftpcode == 150) || (ftpcode == 125)) {

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

    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1; /* default unknown size */

    /*
     * It appears that there are FTP-servers that return size 0 for files when
     * SIZE is used on the file while being in BINARY mode. To work around
     * that (stupid) behavior, we attempt to parse the RETR response even if
     * the SIZE returned size zero.
     *
     * Debugging help from Salvatore Sorrentino on February 26, 2003.
     */

    ಆದರೆ((instate != FTP_LIST) &&
       !ದತ್ತಾಂಶ->state.prefer_ascii &&
       !ದತ್ತಾಂಶ->ಗಣ.ignorecl &&
       (ftp->downloadsize < 1)) {
      /*
       * It seems directory listings either do not show the size or often uses
       * size 0 anyway. ASCII transfers may cause that the transferred amount
       * of data is not the same as this line tells, why using this number in
       * those cases only confuses us.
       *
       * Example D above makes this parsing a little tricky */
      size_t len = curlx_dyn_len(&ftpc->pp.recvbuf);
      ಆದರೆ(len >= 7) { /* "1 bytes" is 7 characters */
        size_t i;
        ಗಾಗಿ(i = 0; i < len - 7; i++) {
          curl_off_t what;
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf = curlx_dyn_ptr(&ftpc->pp.recvbuf);
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c = &buf[i];
          ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&c, &what, CURL_OFF_T_MAX) &&
             !curlx_str_single(&c, ' ') &&
             !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(c, "bytes", 5)) {
            ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = what;
            ನಿಲ್ಲಿಸು;
          }
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftp->downloadsize > -1)
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ftp->downloadsize;

    ಆದರೆ(ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 > ದತ್ತಾಂಶ->req.maxdownload && ದತ್ತಾಂಶ->req.maxdownload > 0)
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->req.maxdownload;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((instate != FTP_LIST) && ದತ್ತಾಂಶ->state.prefer_ascii)
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1; /* for servers that understate ASCII mode file
ಗಾತ್ರ*/

    infof(ದತ್ತಾಂಶ, "Maxdownload = %" FMT_OFF_T, ದತ್ತಾಂಶ->req.maxdownload);

    ಆದರೆ(instate != FTP_LIST)
      infof(ದತ್ತಾಂಶ, "Getting file with size: %" FMT_OFF_T, ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_use_ಪೋರ್ಟ್) {
      bool connected;

      ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, SECONDARYSOCKET, FALSE, &connected);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಆದರೆ(!connected) {
        infof(ದತ್ತಾಂಶ, "Data conn was not available immediately");
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
        ftpc->wait_ದತ್ತಾಂಶ_conn = TRUE;
        ಹಿಂತಿರುಗಿ ftp_check_ctrl_on_ದತ್ತಾಂಶ_wait(ದತ್ತಾಂಶ, ftpc);
      }
      ftpc->wait_ದತ್ತಾಂಶ_conn = FALSE;
    }
    ಹಿಂತಿರುಗಿ ftp_initiate_transfer(ದತ್ತಾಂಶ, ftpc);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ((instate == FTP_LIST) && (ftpcode == 450)) {
      /* no matching files in the directory listing */
      ftp->transfer = PPTRANSFER_NONE; /* do not download anything */
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* this phase is over */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "RETR response: %03d", ftpcode);
      ಹಿಂತಿರುಗಿ instate == FTP_RETR && ftpcode == 550 ?
        CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು :
        CURLE_FTP_COULDNT_RETR_FILE;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* after USER, PASS and ACCT */
ಸ್ಥಿರ CURLcode ftp_state_loggedin(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_conn *ftpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
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
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PBSZ %d", 0);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PBSZ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = ftp_state_pwd(ದತ್ತಾಂಶ, ftpc);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* A value that becomes part of an FTP control command must not carry a
   control byte: a CR or LF would end the command line and let a second
   command be smuggled onto the control connection. */
ಸ್ಥಿರ bool ftp_has_ctrl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *s = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)string;
  ಆಗ(*s) {
    ಆದರೆ(*s < 0x20)
      ಹಿಂತಿರುಗಿ TRUE;
    s++;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* for USER and PASS responses */
ಸ್ಥಿರ CURLcode ftp_state_user_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* some need password anyway, and others return 2xx ignored */
  ಆದರೆ((ftpcode == 331) && (ftpc->state == FTP_USER)) {
    /* 331 Password required for ...
       (the server requires to send the user's password too) */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PASS %s",
                           Curl_creds_passwd(ದತ್ತಾಂಶ->conn->creds));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PASS);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpcode / 100 == 2) {
    /* 230 User ... logged in.
       (the user logged in with or without password) */
    ಫಲಿತಾಂಶ = ftp_state_loggedin(ದತ್ತಾಂಶ, ftpc);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpcode == 332) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *account = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ACCOUNT);
    ಆದರೆ(!account) {
      failf(ದತ್ತಾಂಶ, "ACCT requested but none available");
      ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftp_has_ctrl(account)) {
      failf(ದತ್ತಾಂಶ, "Control byte in FTP account");
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "ACCT %s", account);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_ACCT);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* All other response codes, like:

    530 User ... access denied
    (the server denies to log the specified user) */

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alt = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ALTERNATIVE_ಗಮ್ಯ_USER);
    ಆದರೆ(alt && !ftpc->ftp_trying_alternative) {
      /* Ok, USER failed. Let's try the supplied command. */
      ಆದರೆ(ftp_has_ctrl(alt)) {
        failf(ದತ್ತಾಂಶ, "Control byte in FTP alternative-to-user command");
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", alt);
        ಆದರೆ(!ಫಲಿತಾಂಶ) {
          ftpc->ftp_trying_alternative = TRUE;
          ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_USER);
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "Access denied: %03d", ftpcode);
      ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* for ACCT response */
ಸ್ಥಿರ CURLcode ftp_state_acct_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc,
                                    ಪೂರ್ಣಾಂಕ ftpcode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(ftpcode != 230) {
    failf(ದತ್ತಾಂಶ, "ACCT rejected by server: %03d", ftpcode);
    ಫಲಿತಾಂಶ = CURLE_FTP_WEIRD_PASS_ಉತ್ತರ; /* FIX */
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_loggedin(ದತ್ತಾಂಶ, ftpc);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_pwd_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ftp_conn *ftpc,
                             ಪೂರ್ಣಾಂಕ ftpcode)
{
  ರಚನೆ pingpong *pp = &ftpc->pp;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(ftpcode == 257) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                          letter */
    bool ನಮೂದು_extracted = FALSE;
    ರಚನೆ dynbuf ಹೊರಹೋಗು;
    curlx_dyn_init(&ಹೊರಹೋಗು, 1000);

    /* Reply format is like
       257<space>[rubbish]"<directory-name>"<space><commentary> and the
       RFC959 says

       The directory name can contain any character; embedded
       double-quotes should be escaped by double-quotes (the
       "quote-doubling" convention).
     */

    /* scan for the first double-quote for non-standard responses */
    ಆಗ(*ptr != '\n' && *ptr != '\0' && *ptr != '"')
      ptr++;

    ಆದರೆ('\"' == *ptr) {
      /* it started good */
      ಗಾಗಿ(ptr++; *ptr; ptr++) {
        ಆದರೆ('\"' == *ptr) {
          ಆದರೆ('\"' == ptr[1]) {
            /* "quote-doubling" */
            ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, &ptr[1], 1);
            ptr++;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            /* end of path */
            ಆದರೆ(curlx_dyn_len(&ಹೊರಹೋಗು))
              ನಮೂದು_extracted = TRUE;
            ನಿಲ್ಲಿಸು; /* get out of this loop */
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಆದರೆ(ISCNTRL(*ptr)) {
            /* control characters have no business in a path */
            curlx_dyn_free(&ಹೊರಹೋಗು);
            ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
          }
          ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, ptr, 1);
        }
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          curlx_dyn_free(&ಹೊರಹೋಗು);
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
    }
    ಆದರೆ(ನಮೂದು_extracted) {
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
      ಅಕ್ಷರ *dir = curlx_dyn_ptr(&ಹೊರಹೋಗು);

      ಆದರೆ(!ftpc->server_os && dir[0] != '/') {
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "SYST");
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          curlx_dyn_free(&ಹೊರಹೋಗು);
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }

      curlx_free(ftpc->entrypath);
      ftpc->entrypath = dir; /* remember this */
      infof(ದತ್ತಾಂಶ, "Entry path is '%s'", ftpc->entrypath);
      /* also save it where getinfo can access it: */
      curlx_free(ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath);
      ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath = curlx_strdup(ftpc->entrypath);
      ಆದರೆ(!ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ಆದರೆ(!ftpc->server_os && dir[0] != '/') {
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_SYST);
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* could not get the path */
      curlx_dyn_free(&ಹೊರಹೋಗು);
      infof(ದತ್ತಾಂಶ, "Failed to figure out path");
    }
  }
  ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ftpauth[] = { "SSL", "TLS" };

ಸ್ಥಿರ CURLcode ftp_wait_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn,
                              ರಚನೆ ftp_conn *ftpc,
                              ಪೂರ್ಣಾಂಕ ftpcode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(ftpcode == 230) {
    /* 230 User logged in - already! Take as 220 if TLS required. */
    ಆದರೆ(ftpc->use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ಹಿಂತಿರುಗಿ ftp_state_user_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpcode != 220) {
    failf(ದತ್ತಾಂಶ, "Got a %03d ftp-server response when 220 was expected",
          ftpcode);
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಆದರೆ(ftpc->use_ssl && !Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* We do not have an SSL/TLS control connection yet, but FTPS is
       requested. Try an FTPS connection now */

    ftpc->count3 = 0;
    ಆಯ್ಕೆ((ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.ftpsslauth) {
    ಸ್ಥಿತಿ CURLFTPAUTH_DEFAULT:
    ಸ್ಥಿತಿ CURLFTPAUTH_SSL:
      ftpc->count2 = 1; /* add one to get next */
      ftpc->count1 = 0;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFTPAUTH_TLS:
      ftpc->count2 = -1; /* subtract one to get next */
      ftpc->count1 = 1;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "unsupported parameter to CURLOPT_FTPSSLAUTH: %d",
            (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->ಗಣ.ftpsslauth);
      ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION; /* we do not know what to do */
    }
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "AUTH %s", ftpauth[ftpc->count1]);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_AUTH);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ftp_state_user(ದತ್ತಾಂಶ, ftpc, conn);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* an implicit TLS scheme implies TLS on both connections */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ftp_use_ssl(ರಚನೆ connectdata *conn,
                                 ರಚನೆ ftp_conn *ftpc)
{
  ಆದರೆ(!ftpc->use_ssl && (conn->scheme->flags & PROTOPT_SSL))
    ಹಿಂತಿರುಗಿ CURLUSESSL_ALL;
  ಹಿಂತಿರುಗಿ ftpc->use_ssl;
}

ಸ್ಥಿರ CURLcode ftp_pp_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ftpcode;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  ರಚನೆ FTP *ftp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FTP_EASY);
  ರಚನೆ pingpong *pp;
  size_t nread = 0;

  ಆದರೆ(!ftpc || !ftp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  ಆದರೆ(pp->sendleft)
    ಹಿಂತಿರುಗಿ Curl_pp_flushsend(ದತ್ತಾಂಶ, pp);

  ಫಲಿತಾಂಶ = ftp_readresp(ದತ್ತಾಂಶ, ftpc, FIRSTSOCKET, pp, &ftpcode, &nread);
  ಆದರೆ(ಫಲಿತಾಂಶ || !ftpcode)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* we have now received a full FTP server response */
  ಆಯ್ಕೆ(ftpc->state) {
  ಸ್ಥಿತಿ FTP_WAIT220:
    ಫಲಿತಾಂಶ = ftp_wait_resp(ದತ್ತಾಂಶ, conn, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_AUTH:
    /* we have gotten the response to a previous AUTH command */

    ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ)
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ; /* Forbid pipelining in response. */

    /* RFC2228 (page 5) says:
     *
     * If the server is willing to accept the named security mechanism,
     * and does not require any security data, it must respond with
     * reply code 234/334.
     */

    ಆದರೆ((ftpcode == 234) || (ftpcode == 334)) {
      /* this was BLOCKING, keep it so for now */
      bool ಮುಗಿದ;
      ಆದರೆ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
        ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
          ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          /* we failed and bail out */
          ಹಿಂತಿರುಗಿ CURLE_USE_SSL_FAILED;
        }
      }
      /* BLOCKING */
      ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, TRUE, &ಮುಗಿದ);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        conn->ಬಿಟ್ಟುಗಳು.ftp_use_ದತ್ತಾಂಶ_ssl = FALSE; /* clear-text data */
        ಫಲಿತಾಂಶ = ftp_state_user(ದತ್ತಾಂಶ, ftpc, conn);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftpc->count3 < 1) {
      ftpc->count3++;
      ftpc->count1 += ftpc->count2; /* get next attempt */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "AUTH %s",
                             ftpauth[ftpc->count1]);
      /* remain in this same state */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(ftpc->use_ssl > CURLUSESSL_TRY)
        /* we failed and CURLUSESSL_CONTROL or CURLUSESSL_ALL is set */
        ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
      ಇಲ್ಲದಿದ್ದರೆ
        /* ignore the failure and continue */
        ಫಲಿತಾಂಶ = ftp_state_user(ದತ್ತಾಂಶ, ftpc, conn);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_USER:
  ಸ್ಥಿತಿ FTP_PASS:
    ಫಲಿತಾಂಶ = ftp_state_user_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_ACCT:
    ಫಲಿತಾಂಶ = ftp_state_acct_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_PBSZ:
    ಫಲಿತಾಂಶ =
      Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "PROT %c",
                    ftp_use_ssl(conn, ftpc) == CURLUSESSL_CONTROL ? 'C' : 'P');
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_PROT);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_PROT:
    ಆದರೆ(ftpcode / 100 == 2)
      /* We have enabled SSL for the data connection! */
      conn->ಬಿಟ್ಟುಗಳು.ftp_use_ದತ್ತಾಂಶ_ssl =
        (ftp_use_ssl(conn, ftpc) != CURLUSESSL_CONTROL);
    /* FTP servers typically responds with 500 if they decide to reject
       our 'P' request */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ftp_use_ssl(conn, ftpc) > CURLUSESSL_CONTROL)
      /* we failed and bails out */
      ಹಿಂತಿರುಗಿ CURLE_USE_SSL_FAILED;

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_ccc) {
      /* CCC - Clear Command Channel
       */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "CCC");
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_CCC);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = ftp_state_pwd(ದತ್ತಾಂಶ, ftpc);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_CCC:
    ಆದರೆ(ftpcode < 500) {
      /* First shut down the SSL layer (note: this call will block) */
      /* This has only been tested on the proftpd server, and the mod_tls
       * code sends a close notify alert without waiting for a close notify
       * alert in response. Thus we wait for a close notify alert from the
       * server, but we do not send one. Let's hope other servers do
       * the same... */
      ಫಲಿತಾಂಶ = Curl_ssl_cfilter_remove(ದತ್ತಾಂಶ, FIRSTSOCKET,
                                       (ದತ್ತಾಂಶ->ಗಣ.ftp_ccc ==
                                        (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)CURLFTPSSL_CCC_ಸಕ್ರಿಯ));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        failf(ದತ್ತಾಂಶ, "Failed to clear the command channel (CCC)");
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      /* Then continue as normal */
      ಫಲಿತಾಂಶ = ftp_state_pwd(ದತ್ತಾಂಶ, ftpc);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_PWD:
    ಫಲಿತಾಂಶ = ftp_pwd_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_SYST:
    ಆದರೆ(ftpcode == 215) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = curlx_dyn_ptr(&pp->recvbuf) + 4; /* start on the first
                                                            letter */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ;
      ಅಕ್ಷರ *os;

      /* Reply format is like
         215<space><OS-name><space><commentary>
       */
      ಆಗ(*ptr == ' ')
        ptr++;
      ಗಾಗಿ(ಆರಂಭ = ptr; *ptr && *ptr != ' '; ptr++)
        ;
      os = curlx_memdup0(ಆರಂಭ, ptr - ಆರಂಭ);
      ಆದರೆ(!os)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      /* Check for special servers here. */
      ಆದರೆ(curl_strequal(os, "OS/400")) {
        /* Force OS400 name format 1. */
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "SITE NAMEFMT 1");
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          curlx_free(os);
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        /* remember target server OS */
        curlx_free(ftpc->server_os);
        ftpc->server_os = os;
        ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_NAMEFMT);
        ನಿಲ್ಲಿಸು;
      }
      /* Nothing special for the target server. */
      /* remember target server OS */
      curlx_free(ftpc->server_os);
      ftpc->server_os = os;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Cannot identify server OS. Continue anyway and cross fingers. */
    }

    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_NAMEFMT:
    ಆದರೆ(ftpcode == 250) {
      /* Name format change successful: reload initial path. */
      ftp_state_pwd(ದತ್ತಾಂಶ, ftpc);
      ನಿಲ್ಲಿಸು;
    }

    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP); /* we are done with CONNECT phase! */
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] protocol connect phase DONE", FTP_CSTATE(ftpc));
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_QUOTE:
  ಸ್ಥಿತಿ FTP_POSTQUOTE:
  ಸ್ಥಿತಿ FTP_RETR_PREQUOTE:
  ಸ್ಥಿತಿ FTP_STOR_PREQUOTE:
  ಸ್ಥಿತಿ FTP_LIST_PREQUOTE:
    ಆದರೆ((ftpcode >= 400) && !ftpc->count2) {
      /* failure response code, and not allowed to fail */
      failf(ದತ್ತಾಂಶ, "QUOT command failed with %03d", ftpcode);
      ಫಲಿತಾಂಶ = CURLE_QUOTE_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, FALSE, ftpc->state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_CWD:
    ಆದರೆ(ftpcode / 100 != 2) {
      /* failure to CWD there */
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_create_missing_dirs &&
         ftpc->cwdcount && !ftpc->count2) {
        /* try making it */
        ftpc->count2++; /* counter to prevent CWD-MKD loops */

        /* count3 is set to allow MKD to fail once per dir. In the case when
           CWD fails and then MKD fails (due to another session raced it to
           create the dir) this then allows for a second try to CWD to it. */
        ftpc->count3 = (ದತ್ತಾಂಶ->ಗಣ.ftp_create_missing_dirs == 2) ? 1 : 0;

        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "MKD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_MKD);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* return failure */
        failf(ದತ್ತಾಂಶ, "Server denied you to change to the given directory");
        ftpc->cwdfail = TRUE; /* do not remember this path as we failed
                                 to enter it */
        ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* success */
      ftpc->count2 = 0;
      ಆದರೆ(ftpc->cwdcount >= ftpc->dirdepth)
        ಫಲಿತಾಂಶ = ftp_state_mdtm(ದತ್ತಾಂಶ, ftpc, ftp);
      ಇಲ್ಲದಿದ್ದರೆ {
        ftpc->cwdcount++;
        /* send next CWD */
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "CWD %.*s",
                               pathlen(ftpc, ftpc->cwdcount - 1),
                               pathpiece(ftpc, ftpc->cwdcount - 1));
      }
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_MKD:
    ಆದರೆ((ftpcode / 100 != 2) && !ftpc->count3--) {
      /* failure to MKD the directory */
      failf(ದತ್ತಾಂಶ, "Failed to MKD dir: %03d", ftpcode);
      ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_CWD);
      /* send CWD */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "CWD %.*s",
                             pathlen(ftpc, ftpc->cwdcount - 1),
                             pathpiece(ftpc, ftpc->cwdcount - 1));
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_MDTM:
    ಫಲಿತಾಂಶ = ftp_state_mdtm_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_TYPE:
  ಸ್ಥಿತಿ FTP_LIST_TYPE:
  ಸ್ಥಿತಿ FTP_RETR_TYPE:
  ಸ್ಥಿತಿ FTP_STOR_TYPE:
  ಸ್ಥಿತಿ FTP_RETR_LIST_TYPE:
    ಫಲಿತಾಂಶ = ftp_state_type_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode, ftpc->state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_ಗಾತ್ರ:
  ಸ್ಥಿತಿ FTP_RETR_ಗಾತ್ರ:
  ಸ್ಥಿತಿ FTP_STOR_ಗಾತ್ರ:
    ಫಲಿತಾಂಶ = ftp_state_ಗಾತ್ರ_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode, ftpc->state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_REST:
  ಸ್ಥಿತಿ FTP_RETR_REST:
    ಫಲಿತಾಂಶ = ftp_state_rest_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode, ftpc->state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_PRET:
    ಆದರೆ(ftpcode != 200) {
      /* there only is this one standard OK return code. */
      failf(ದತ್ತಾಂಶ, "PRET command not accepted: %03d", ftpcode);
      ಹಿಂತಿರುಗಿ CURLE_FTP_PRET_FAILED;
    }
    ಫಲಿತಾಂಶ = ftp_state_use_pasv(ದತ್ತಾಂಶ, ftpc, conn);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_PASV:
    ಫಲಿತಾಂಶ = ftp_state_pasv_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_ಪೋರ್ಟ್:
    ಫಲಿತಾಂಶ = ftp_state_ಪೋರ್ಟ್_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_LIST:
  ಸ್ಥಿತಿ FTP_RETR:
    ಫಲಿತಾಂಶ = ftp_state_get_resp(ದತ್ತಾಂಶ, ftpc, ftp, ftpcode, ftpc->state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_STOR:
    ಫಲಿತಾಂಶ = ftp_state_stor_resp(ದತ್ತಾಂಶ, ftpc, ftpcode);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ FTP_QUIT:
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* internal error */
    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ftp_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ಹಿಂತಿರುಗಿ ftpc ? ftp_statemach(ದತ್ತಾಂಶ, ftpc, ಮುಗಿದ) : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode ftp_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_conn *ftpc)
{
  ರಚನೆ pingpong *pp = &ftpc->pp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(ftpc->state != FTP_STOP) {
    ಆದರೆ(ftpc->shutdown)
      CURL_TRC_FTP(ದತ್ತಾಂಶ, "in shutdown, waiting for server response");
    ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, pp, TRUE, TRUE /* disconnecting */);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * ftp_connect() should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 *
 */
ಸ್ಥಿರ CURLcode ftp_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool *ಮುಗಿದ) /* see description above */
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ರಚನೆ pingpong *pp;

  *ಮುಗಿದ = FALSE; /* default to not done yet */
  ಆದರೆ(!ftpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  pp = &ftpc->pp;
  PINGPONG_ಸಿದ್ಧತೆ(pp, ftp_pp_statemachine, ftp_endofresp);

  ಆದರೆ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* BLOCKING */
    ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, TRUE, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  Curl_pp_init(pp, Curl_pgrs_now(ದತ್ತಾಂಶ)); /* once per transfer */

  /* When we connect, we start in the state where we await the 220
     response */
  ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_WAIT220);

  ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_sendquote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ftp_conn *ftpc,
                              ರಚನೆ curl_slist *quote)
{
  ರಚನೆ curl_slist *ಅಂಶ;
  ರಚನೆ pingpong *pp = &ftpc->pp;

  ಅಂಶ = quote;
  ಆಗ(ಅಂಶ) {
    ಆದರೆ(ಅಂಶ->ದತ್ತಾಂಶ) {
      size_t nread;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd = ಅಂಶ->ದತ್ತಾಂಶ;
      bool acceptfail = FALSE;
      CURLcode ಫಲಿತಾಂಶ;
      ಪೂರ್ಣಾಂಕ ftpcode = 0;

      /* if a command starts with an asterisk, which a legal FTP command never
         can, the command will be allowed to fail without it causing any
         aborts or cancels etc. It will cause libcurl to act as if the command
         is successful, whatever the server responds. */

      ಆದರೆ(cmd[0] == '*') {
        cmd++;
        acceptfail = TRUE;
      }

      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", cmd);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        pp->response = *Curl_pgrs_now(ದತ್ತಾಂಶ); /* timeout relative now */
        ಫಲಿತಾಂಶ = getftpresponse(ದತ್ತಾಂಶ, &nread, &ftpcode);
      }
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಆದರೆ(!acceptfail && (ftpcode >= 400)) {
        failf(ದತ್ತಾಂಶ, "QUOT string not accepted: %s", cmd);
        ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
      }
    }

    ಅಂಶ = ಅಂಶ->ಮುಂದಿನ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ftp_ಮುಗಿದ_ಸ್ಥಿತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ftp_conn *ftpc, CURLcode ಸ್ಥಿತಿ_2,
                                bool premature)
{
  ಆಯ್ಕೆ(ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿತಿ CURLE_BAD_DOWNLOAD_RESUME:
  ಸ್ಥಿತಿ CURLE_FTP_WEIRD_PASV_ಉತ್ತರ:
  ಸ್ಥಿತಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED:
  ಸ್ಥಿತಿ CURLE_FTP_ACCEPT_FAILED:
  ಸ್ಥಿತಿ CURLE_FTP_ACCEPT_TIMEOUT:
  ಸ್ಥಿತಿ CURLE_FTP_COULDNT_ಗಣ_TYPE:
  ಸ್ಥಿತಿ CURLE_FTP_COULDNT_RETR_FILE:
  ಸ್ಥಿತಿ CURLE_PARTIAL_FILE:
  ಸ್ಥಿತಿ CURLE_UPLOAD_FAILED:
  ಸ್ಥಿತಿ CURLE_REMOTE_ACCESS_DENIED:
  ಸ್ಥಿತಿ CURLE_FILESIZE_EXCEEDED:
  ಸ್ಥಿತಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು:
  ಸ್ಥಿತಿ CURLE_WRITE_ERROR:
    /* the connection stays alive fine even though this happened */
  ಸ್ಥಿತಿ CURLE_OK: /* does not affect the control connection's status */
    ಆದರೆ(!premature)
      ನಿಲ್ಲಿಸು;

    /* until we cope better with prematurely ended requests, let them
     * fallback as if in complete failure */
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:       /* by default, an error means the control connection is
                    wedged and should not be used anymore */
    ftpc->ctl_valid = FALSE;
    ftpc->cwdfail = TRUE; /* set this TRUE to prevent us to remember the
                             current path, as this connection is going */
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "FTP ended with bad error code");
    connclose(ದತ್ತಾಂಶ->conn);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;      /* use the already set error code */
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_ಮುಗಿದ_wildcard(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ftp_conn *ftpc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.chunk_ಕೊನೆ && ftpc->file) {
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_chunk_ಕೊನೆ);
      ದತ್ತಾಂಶ->ಗಣ.chunk_ಕೊನೆ(ದತ್ತಾಂಶ->ಗಣ.wildcardptr);
      CURL_CBAPI_ಕೊನೆ(&guard);
      freedirs(ftpc);
    }
    ftpc->ತಿಳಿದ_filesize = -1;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_ಮುಗಿದ_ಮಾರ್ಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ftp_conn *ftpc,
                          CURLcode ಫಲಿತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* We can limp along anyway (and should try to since we may already be in
     * the error path) */
    ftpc->ctl_valid = FALSE; /* mark control connection as bad */
    connclose(conn); /* mark for connection closure */
    curlx_safefree(ftpc->prevpath); /* no path remembering */
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* remember working directory for connection reuse */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *raw_ಮಾರ್ಗ = ftpc->rawpath;
    ಆದರೆ(raw_ಮಾರ್ಗ) {
      ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD) && (raw_ಮಾರ್ಗ[0] == '/'))
        ; /* full path => no CWDs happened => keep ftpc->prevpath */
      ಇಲ್ಲದಿದ್ದರೆ {
        size_t ಮಾರ್ಗ_Len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ftpc->rawpath);

        curlx_free(ftpc->prevpath);

        ಆದರೆ(!ftpc->cwdfail) {
          ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD)
            ಮಾರ್ಗ_Len = 0; /* relative path => working directory is FTP home */
          ಇಲ್ಲದಿದ್ದರೆ
            /* file is URL-decoded */
            ಮಾರ್ಗ_Len -= ftpc->file ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ftpc->file) : 0;
          ftpc->prevpath = curlx_memdup0(raw_ಮಾರ್ಗ, ಮಾರ್ಗ_Len);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ftpc->prevpath = ಖಾಲಿ; /* no path */
      }
    }
    ಆದರೆ(ftpc->prevpath)
      infof(ದತ್ತಾಂಶ, "Remembering we are in directory \"%s\"", ftpc->prevpath);
  }
}

ಸ್ಥಿರ CURLcode ftp_ಮುಗಿದ_secondary_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ftp_conn *ftpc,
                                          CURLcode ಫಲಿತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(Curl_conn_is_ಸಿದ್ಧತೆ(conn, SECONDARYSOCKET)) {
    ಆದರೆ(!ಫಲಿತಾಂಶ && ftpc->dont_check && ದತ್ತಾಂಶ->req.maxdownload > 0) {
      /* partial download completed */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "ABOR");
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "Failure sending ABOR command: %s",
              curl_easy_strerror(ಫಲಿತಾಂಶ));
        ftpc->ctl_valid = FALSE; /* mark control connection as bad */
        connclose(conn); /* connection closure */
      }
    }

    close_secondarysocket(ದತ್ತಾಂಶ, ftpc);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_ಮುಗಿದ_control_ಉತ್ತರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ftp_conn *ftpc,
                                       ರಚನೆ FTP *ftp, CURLcode ಫಲಿತಾಂಶ,
                                       bool premature)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  size_t nread;
  ಪೂರ್ಣಾಂಕ ftpcode;

  ಆದರೆ(!ಫಲಿತಾಂಶ && (ftp->transfer == PPTRANSFER_BODY) && ftpc->ctl_valid &&
     ftpc->pp.pending_resp && !premature) {
    /*
     * Let's see what the server says about the transfer we performed, but
     * lower the timeout as sometimes this connection has died while the data
     * has been transferred. This happens when doing through NATs etc that
     * abandon old silent connections.
     */
    ftpc->pp.response = *Curl_pgrs_now(ದತ್ತಾಂಶ); /* timeout relative now */
    ಫಲಿತಾಂಶ = getftpresponse(ದತ್ತಾಂಶ, &nread, &ftpcode);

    ಆದರೆ(!nread && (ಫಲಿತಾಂಶ == CURLE_OPERATION_TIMEDOUT)) {
      failf(ದತ್ತಾಂಶ, "control connection looks dead");
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(conn); /* close */
    }

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ(ftpc->dont_check && ದತ್ತಾಂಶ->req.maxdownload > 0) {
      /* we have sent ABOR and there is no reliable way to check if it was
       * successful or not; we have to close the connection now */
      infof(ದತ್ತಾಂಶ, "partial download completed, closing connection");
      connclose(conn);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ(!ftpc->dont_check) {
      /* 226 Transfer complete, 250 Requested file action okay, completed. */
      ಆಯ್ಕೆ(ftpcode) {
      ಸ್ಥಿತಿ 226:
      ಸ್ಥಿತಿ 250:
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 552:
        failf(ದತ್ತಾಂಶ, "Exceeded storage allocation");
        ಫಲಿತಾಂಶ = CURLE_REMOTE_DISK_ತುಂಬಿದ;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        failf(ದತ್ತಾಂಶ, "server did not report OK, got %d", ftpcode);
        ಫಲಿತಾಂಶ = CURLE_PARTIAL_FILE;
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ftp_ಮುಗಿದ_check_partial(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ftp_conn *ftpc,
                                       ರಚನೆ FTP *ftp, CURLcode ಫಲಿತಾಂಶ,
                                       bool premature)
{
  ಆದರೆ(ಫಲಿತಾಂಶ || premature)
    /* the response code from the transfer showed an error already so no
       use checking further */
    ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
    ಆದರೆ((ftp->transfer == PPTRANSFER_BODY) &&
       (ದತ್ತಾಂಶ->state.infilesize != -1) && /* upload with known size */
       ((!ದತ್ತಾಂಶ->ಗಣ.crlf && !ದತ್ತಾಂಶ->state.prefer_ascii && /* no conversion */
         (ದತ್ತಾಂಶ->state.infilesize != ದತ್ತಾಂಶ->req.writebytecount)) ||
        ((ದತ್ತಾಂಶ->ಗಣ.crlf || ದತ್ತಾಂಶ->state.prefer_ascii) && /* maybe CRLF conv */
         (ದತ್ತಾಂಶ->state.infilesize > ದತ್ತಾಂಶ->req.writebytecount))
       )) {
      failf(ದತ್ತಾಂಶ, "Uploaded unaligned file size (%" FMT_OFF_T
            " out of %" FMT_OFF_T " bytes)",
            ದತ್ತಾಂಶ->req.writebytecount, ದತ್ತಾಂಶ->state.infilesize);
      ಫಲಿತಾಂಶ = CURLE_PARTIAL_FILE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ((ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 != -1) &&
       (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 != ದತ್ತಾಂಶ->req.bytecount) &&
       (ದತ್ತಾಂಶ->req.maxdownload != ದತ್ತಾಂಶ->req.bytecount)) {
      failf(ದತ್ತಾಂಶ, "Received only partial file: %" FMT_OFF_T " bytes",
            ದತ್ತಾಂಶ->req.bytecount);
      ಫಲಿತಾಂಶ = CURLE_PARTIAL_FILE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ftpc->dont_check &&
            !ದತ್ತಾಂಶ->req.bytecount &&
            (ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 > 0)) {
      failf(ದತ್ತಾಂಶ, "No data was received");
      ಫಲಿತಾಂಶ = CURLE_FTP_COULDNT_RETR_FILE;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                         bool premature)
{
  ರಚನೆ FTP *ftp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FTP_EASY);
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!ftp || !ftpc)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = ftp_ಮುಗಿದ_ಸ್ಥಿತಿ(ದತ್ತಾಂಶ, ftpc, ಸ್ಥಿತಿ_2, premature);

  ftp_ಮುಗಿದ_wildcard(ದತ್ತಾಂಶ, ftpc);
  ftp_ಮುಗಿದ_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ಫಲಿತಾಂಶ);
  ಫಲಿತಾಂಶ = ftp_ಮುಗಿದ_secondary_socket(ದತ್ತಾಂಶ, ftpc, ಫಲಿತಾಂಶ);
  ಫಲಿತಾಂಶ = ftp_ಮುಗಿದ_control_ಉತ್ತರ(ದತ್ತಾಂಶ, ftpc, ftp, ಫಲಿತಾಂಶ, premature);
  ಫಲಿತಾಂಶ = ftp_ಮುಗಿದ_check_partial(ದತ್ತಾಂಶ, ftpc, ftp, ಫಲಿತಾಂಶ, premature);

  /* clear these for next connection */
  ftp->transfer = PPTRANSFER_BODY;
  ftpc->dont_check = FALSE;

  /* Send any post-transfer QUOTE strings? */
  ಆದರೆ(!ಸ್ಥಿತಿ_2 && !ಫಲಿತಾಂಶ && !premature && ದತ್ತಾಂಶ->ಗಣ.postquote)
    ಫಲಿತಾಂಶ = ftp_sendquote(ದತ್ತಾಂಶ, ftpc, ದತ್ತಾಂಶ->ಗಣ.postquote);
  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] done, result=%d", FTP_CSTATE(ftpc), (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * ftp_nb_type()
 *
 * Set TYPE. We only deal with ASCII or BINARY so this function
 * sets one of them.
 * If the transfer type is not sent, simulate on OK response in newstate
 */
ಸ್ಥಿರ CURLcode ftp_nb_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ftp_conn *ftpc,
                            ರಚನೆ FTP *ftp,
                            bool ascii, ftpstate newstate)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ want = (ಅಕ್ಷರ)(ascii ? 'A' : 'I');

  ಆದರೆ(ftpc->transfertype == want) {
    ftp_state(ದತ್ತಾಂಶ, ftpc, newstate);
    ಹಿಂತಿರುಗಿ ftp_state_type_resp(ದತ್ತಾಂಶ, ftpc, ftp, 200, newstate);
  }

  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "TYPE %c", want);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ftp_state(ದತ್ತಾಂಶ, ftpc, newstate);

    /* keep track of our current transfer type */
    ftpc->transfertype = want;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * ftp_perform()
 *
 * This is the actual DO function for FTP. Get a file/directory according to
 * the options previously setup.
 */
ಸ್ಥಿರ CURLcode ftp_perform(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ ftp_conn *ftpc,
  ರಚನೆ FTP *ftp,
  bool *connected,  /* connect status after PASV / PORT */
  bool *dophase_ಮುಗಿದ)
{
  /* this is FTP and no proxy */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] DO phase starts", FTP_CSTATE(ftpc));

  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body) {
    /* requested no body means no transfer... */
    ftp->transfer = PPTRANSFER_INFO;
  }

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */

  /* start the first command in the DO phase */
  ಫಲಿತಾಂಶ = ftp_state_quote(ದತ್ತಾಂಶ, ftpc, ftp, TRUE, FTP_QUOTE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* run the state-machine */
  ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, SECONDARYSOCKET);

  ಆದರೆ(*connected)
    infof(ದತ್ತಾಂಶ, "[FTP] [%s] perform, DATA connection established",
          FTP_CSTATE(ftpc));
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] perform, awaiting DATA connect",
                 FTP_CSTATE(ftpc));

  ಆದರೆ(*dophase_ಮುಗಿದ)
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] DO phase is complete1", FTP_CSTATE(ftpc));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ wc_ದತ್ತಾಂಶ_dtor(ಶೂನ್ಯ *ptr)
{
  ರಚನೆ ftp_wc *ftpwc = ptr;
  ಆದರೆ(ftpwc && ftpwc->parser)
    Curl_ftp_parselist_ದತ್ತಾಂಶ_free(&ftpwc->parser);
  curlx_free(ftpwc);
}

ಸ್ಥಿರ CURLcode init_wc_ದತ್ತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ftp_conn *ftpc,
                             ರಚನೆ FTP *ftp)
{
  ಅಕ್ಷರ *ಕೊನೆಯ_slash;
  ಅಕ್ಷರ *ಮಾರ್ಗ = ftp->ಮಾರ್ಗ;
  ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wildcard = ದತ್ತಾಂಶ->wildcard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ftp_wc *ftpwc = ಖಾಲಿ;

  ಕೊನೆಯ_slash = strrchr(ftp->ಮಾರ್ಗ, '/');
  ಆದರೆ(ಕೊನೆಯ_slash) {
    ಕೊನೆಯ_slash++;
    ಆದರೆ(ಕೊನೆಯ_slash[0] == '\0') {
      wildcard->state = CURLWC_CLEAN;
      ಹಿಂತಿರುಗಿ ftp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ftp);
    }
    wildcard->ನಮೂನೆ = curlx_strdup(ಕೊನೆಯ_slash);
    ಆದರೆ(!wildcard->ನಮೂನೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಕೊನೆಯ_slash[0] = '\0'; /* cut file from path */
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* there is only 'wildcard pattern' or nothing */
    ಆದರೆ(ಮಾರ್ಗ[0]) {
      wildcard->ನಮೂನೆ = curlx_strdup(ಮಾರ್ಗ);
      ಆದರೆ(!wildcard->ನಮೂನೆ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಮಾರ್ಗ[0] = '\0';
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* only list */
      wildcard->state = CURLWC_CLEAN;
      ಹಿಂತಿರುಗಿ ftp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ftp);
    }
  }

  /* program continues only if URL is not ending with slash, allocate needed
     resources for wildcard transfer */

  /* allocate ftp protocol specific wildcard data */
  ftpwc = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ ftp_wc));
  ಆದರೆ(!ftpwc) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  /* INITIALIZE parselist structure */
  ftpwc->parser = Curl_ftp_parselist_ದತ್ತಾಂಶ_alloc();
  ಆದರೆ(!ftpwc->parser) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  wildcard->ftpwc = ftpwc; /* put it to the WildcardData tmp pointer */
  wildcard->dtor = wc_ದತ್ತಾಂಶ_dtor;

  /* wildcard does not support NOCWD option (assert it?) */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod == FTPFILE_NOCWD)
    ದತ್ತಾಂಶ->ಗಣ.ftp_filemethod = FTPFILE_MULTICWD;

  /* try to parse ftp URL */
  ಫಲಿತಾಂಶ = ftp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ftp);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಹೋಗು fail;
  }

  wildcard->ಮಾರ್ಗ = curlx_strdup(ftp->ಮಾರ್ಗ);
  ಆದರೆ(!wildcard->ಮಾರ್ಗ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  infof(ದತ್ತಾಂಶ, "Wildcard - Parsing started");
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  ಆದರೆ(ftpwc) {
    Curl_ftp_parselist_ದತ್ತಾಂಶ_free(&ftpwc->parser);
    curlx_free(ftpwc);
  }
  curlx_safefree(wildcard->ನಮೂನೆ);
  wildcard->dtor = ZERO_NULL;
  wildcard->ftpwc = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode wc_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ftp_conn *ftpc,
                             ರಚನೆ FTP *ftp)
{
  ರಚನೆ Wildcard_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ wildcard = ದತ್ತಾಂಶ->wildcard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಗಾಗಿ(;;) {
    ಆಯ್ಕೆ(wildcard->state) {
    ಸ್ಥಿತಿ CURLWC_INIT:
      ಫಲಿತಾಂಶ = init_wc_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, ftpc, ftp);
      ಆದರೆ(wildcard->state == CURLWC_CLEAN)
        /* only listing! */
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      wildcard->state = ಫಲಿತಾಂಶ ? CURLWC_ERROR : CURLWC_MATCHING;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಸ್ಥಿತಿ CURLWC_MATCHING: {
      /* In this state is LIST response successfully parsed, so lets restore
         previous WRITEFUNCTION callback and WRITEDATA pointer */
      ರಚನೆ ftp_wc *ftpwc = wildcard->ftpwc;
      wildcard->state = CURLWC_DOWNLOADING;

      ಆದರೆ(Curl_ftp_parselist_geterror(ftpwc->parser)) {
        /* error found in LIST parsing */
        wildcard->state = CURLWC_CLEAN;
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) {
        /* no corresponding file */
        wildcard->state = CURLWC_CLEAN;
        ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;
      }
      ಮುಂದುವರಿ;
    }

    ಸ್ಥಿತಿ CURLWC_DOWNLOADING: {
      /* filelist has at least one file, lets get first one */
      ರಚನೆ Curl_llist_ಗಂಟು *ತಲೆ = Curl_llist_ತಲೆ(&wildcard->filelist);
      ರಚನೆ curl_fileinfo *finfo = Curl_ಗಂಟು_elem(ತಲೆ);
      ಅಕ್ಷರ *tmp_ಮಾರ್ಗ;
      ಅಕ್ಷರ *enc = curl_easy_escape(ಖಾಲಿ, finfo->filename, 0);
      ಆದರೆ(!enc)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      tmp_ಮಾರ್ಗ = curl_maprintf("%s%s", wildcard->ಮಾರ್ಗ, enc);
      curl_free(enc);
      ಆದರೆ(!tmp_ಮಾರ್ಗ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      /* switch default ftp->path and tmp_path */
      curlx_free(ftp->pathalloc);
      ftp->pathalloc = ftp->ಮಾರ್ಗ = tmp_ಮಾರ್ಗ;

      infof(ದತ್ತಾಂಶ, "Wildcard - START of \"%s\"", finfo->filename);
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.chunk_bgn) {
        ದೀರ್ಘ userresponse;
        ರಚನೆ Curl_mapi_guard guard;
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_chunk_bgn);
        userresponse = ದತ್ತಾಂಶ->ಗಣ.chunk_bgn(
          finfo, ದತ್ತಾಂಶ->ಗಣ.wildcardptr,
          (ಪೂರ್ಣಾಂಕ)Curl_llist_ಎಣಿಕೆ(&wildcard->filelist));
        CURL_CBAPI_ಕೊನೆ(&guard);
        ಆಯ್ಕೆ(userresponse) {
        ಸ್ಥಿತಿ CURL_CHUNK_BGN_FUNC_SKIP:
          infof(ದತ್ತಾಂಶ, "Wildcard - \"%s\" skipped by user", finfo->filename);
          wildcard->state = CURLWC_SKIP;
          ಮುಂದುವರಿ;
        ಸ್ಥಿತಿ CURL_CHUNK_BGN_FUNC_FAIL:
          ಹಿಂತಿರುಗಿ CURLE_CHUNK_FAILED;
        }
      }

      ಆದರೆ(finfo->filetype != CURLFILETYPE_FILE) {
        wildcard->state = CURLWC_SKIP;
        ಮುಂದುವರಿ;
      }

      ಆದರೆ(finfo->flags & CURLFINFOFLAG_ತಿಳಿದ_ಗಾತ್ರ)
        ftpc->ತಿಳಿದ_filesize = finfo->ಗಾತ್ರ_2;

      ಫಲಿತಾಂಶ = ftp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ftp);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      /* we do not need the Curl_fileinfo of first file anymore */
      Curl_ಗಂಟು_remove(Curl_llist_ತಲೆ(&wildcard->filelist));

      ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) {
        /* remains only one file to down. */
        wildcard->state = CURLWC_CLEAN;
        /* after that will be ftp_do called once again and no transfer
           will be done because of CURLWC_CLEAN state */
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಸ್ಥಿತಿ CURLWC_SKIP: {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.chunk_ಕೊನೆ) {
        ರಚನೆ Curl_mapi_guard guard;
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_chunk_ಕೊನೆ);
        ದತ್ತಾಂಶ->ಗಣ.chunk_ಕೊನೆ(ದತ್ತಾಂಶ->ಗಣ.wildcardptr);
        CURL_CBAPI_ಕೊನೆ(&guard);
      }
      Curl_ಗಂಟು_remove(Curl_llist_ತಲೆ(&wildcard->filelist));
      wildcard->state = (Curl_llist_ಎಣಿಕೆ(&wildcard->filelist) == 0) ?
        CURLWC_CLEAN : CURLWC_DOWNLOADING;
      ಮುಂದುವರಿ;
    }

    ಸ್ಥಿತಿ CURLWC_CLEAN: {
      ರಚನೆ ftp_wc *ftpwc = wildcard->ftpwc;
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಆದರೆ(ftpwc)
        ಫಲಿತಾಂಶ = Curl_ftp_parselist_geterror(ftpwc->parser);

      wildcard->state = ಫಲಿತಾಂಶ ? CURLWC_ERROR : CURLWC_ಮುಗಿದ;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಸ್ಥಿತಿ CURLWC_ಮುಗಿದ:
    ಸ್ಥಿತಿ CURLWC_ERROR:
    ಸ್ಥಿತಿ CURLWC_CLEAR:
      ಆದರೆ(wildcard->dtor) {
        wildcard->dtor(wildcard->ftpwc);
        wildcard->ftpwc = ಖಾಲಿ;
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_regular_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ftp_conn *ftpc,
                                     ರಚನೆ FTP *ftp,
                                     bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected = FALSE;
  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1; /* make sure this is unknown at this point */

  Curl_pgrsReset(ದತ್ತಾಂಶ);

  ftpc->ctl_valid = TRUE; /* starts good */

  ಫಲಿತಾಂಶ = ftp_perform(ದತ್ತಾಂಶ, ftpc, ftp,
                       &connected, /* have we connected after PASV/PORT */
                       dophase_ಮುಗಿದ); /* all commands in the DO-phase done? */

  ಆದರೆ(!ಫಲಿತಾಂಶ) {

    ಆದರೆ(!*dophase_ಮುಗಿದ)
      /* the DO phase has not completed yet */
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಫಲಿತಾಂಶ = ftp_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, ftpc, ftp, connected);

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    freedirs(ftpc);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ರಚನೆ FTP *ftp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FTP_EASY);

  *ಮುಗಿದ = FALSE; /* default to false */
  ಆದರೆ(!ftpc || !ftp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ftpc->wait_ದತ್ತಾಂಶ_conn = FALSE; /* default to no such wait */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_PREFER_LF_LINEENDS
  {
    /* FTP data may need conversion. */
    ರಚನೆ Curl_cwriter *ftp_lc_writer;

    ಫಲಿತಾಂಶ = Curl_cwriter_create(&ftp_lc_writer, ದತ್ತಾಂಶ, &ftp_cw_lc,
                                 CURL_CW_CONTENT_DECODE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = Curl_cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ftp_lc_writer);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      Curl_cwriter_free(ದತ್ತಾಂಶ, ftp_lc_writer);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURL_PREFER_LF_LINEENDS */

  ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
    ಫಲಿತಾಂಶ = wc_statemach(ದತ್ತಾಂಶ, ftpc, ftp);
    ಆದರೆ(ದತ್ತಾಂಶ->wildcard->state == CURLWC_SKIP ||
       ದತ್ತಾಂಶ->wildcard->state == CURLWC_ಮುಗಿದ) {
      /* do not call ftp_regular_transfer */
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಆದರೆ(ಫಲಿತಾಂಶ) /* error, loop or skipping the file */
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* no wildcard FSM needed */
    ಫಲಿತಾಂಶ = ftp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, ftpc, ftp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = ftp_regular_transfer(ದತ್ತಾಂಶ, ftpc, ftp, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ftp_quit(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ ftp_conn *ftpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ftpc->ctl_valid) {
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "sending QUIT to close session");
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &ftpc->pp, "%s", "QUIT");
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Failure sending QUIT command: %s",
            curl_easy_strerror(ಫಲಿತಾಂಶ));
      ftpc->ctl_valid = FALSE; /* mark control connection as bad */
      connclose(ದತ್ತಾಂಶ->conn); /* mark for closure */
      ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_STOP);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ftp_state(ದತ್ತಾಂಶ, ftpc, FTP_QUIT);

    ಫಲಿತಾಂಶ = ftp_ಖಂಡ_statemach(ದತ್ತಾಂಶ, ftpc);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * ftp_disconnect()
 *
 * Disconnect from an FTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode ftp_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               bool dead_connection)
{
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);

  ಆದರೆ(!ftpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to.

     ftp_quit() will check the state of ftp->ctl_valid. If it is ok it
     will try to send the QUIT command, otherwise it will return. */
  ftpc->shutdown = TRUE;
  ಆದರೆ(dead_connection || Curl_pp_needs_flush(ದತ್ತಾಂಶ, &ftpc->pp))
    ftpc->ctl_valid = FALSE;

  /* The FTP session may or may not have been allocated/setup at this point! */
  (ಶೂನ್ಯ)ftp_quit(ದತ್ತಾಂಶ, ftpc); /* ignore errors on the QUIT */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode ftp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          bool *dophase_ಮುಗಿದ)
{
  ರಚನೆ ftp_conn *ftpc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_FTP_CONN);
  ರಚನೆ FTP *ftp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FTP_EASY);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!ftpc || !ftp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = ftp_statemach(ದತ್ತಾಂಶ, ftpc, dophase_ಮುಗಿದ);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] DO phase failed", FTP_CSTATE(ftpc));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*dophase_ಮುಗಿದ) {
    ಫಲಿತಾಂಶ = ftp_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, ftpc, ftp, FALSE /* not connected */);

    CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] DO phase is complete2", FTP_CSTATE(ftpc));
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ FTP *ftp = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_safefree(ftp->pathalloc);
  curlx_free(ftp);
}

ಸ್ಥಿರ ಶೂನ್ಯ ftp_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ftp_conn *ftpc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  freedirs(ftpc);
  curlx_safefree(ftpc->account);
  curlx_safefree(ftpc->alternative_ಗಮ್ಯ_user);
  curlx_safefree(ftpc->entrypath);
  curlx_safefree(ftpc->prevpath);
  curlx_safefree(ftpc->server_os);
  Curl_pp_disconnect(&ftpc->pp);
  curlx_free(ftpc);
}

ಸ್ಥಿರ ಶೂನ್ಯ type_url_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ FTP *ftp)
{
  size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ftp->ಮಾರ್ಗ);
  /* FTP URLs support an extension like ";type=<typecode>" that
   * we will try to get now! */
  ಆದರೆ((len >= 7) && !memcmp(&ftp->ಮಾರ್ಗ[len - 7], ";type=", 6)) {
    ಅಕ್ಷರ *type = &ftp->ಮಾರ್ಗ[len - 7];
    ಅಕ್ಷರ command = Curl_raw_toupper(type[6]);

    *type = 0; /* cut it off */

    ಆಯ್ಕೆ(command) {
    ಸ್ಥಿತಿ 'A': /* ASCII mode */
      ದತ್ತಾಂಶ->state.prefer_ascii = TRUE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 'D': /* directory mode */
      ದತ್ತಾಂಶ->state.list_only = TRUE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 'I': /* binary mode */
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* switch off ASCII */
      ದತ್ತಾಂಶ->state.prefer_ascii = FALSE;
      ನಿಲ್ಲಿಸು;
    }
  }
}

ಸ್ಥಿರ CURLcode ftp_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  ರಚನೆ FTP *ftp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ftp_conn *ftpc;

  ftp = curlx_calloc(1, ಗಾತ್ರ(*ftp));
  ಆದರೆ(!ftp ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_FTP_EASY, ftp, ftp_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ftpc = curlx_calloc(1, ಗಾತ್ರ(*ftpc));
  ಆದರೆ(!ftpc ||
     Curl_conn_meta_ಗಣ(conn, CURL_META_FTP_CONN, ftpc, ftp_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* clone connection related data that is FTP specific */
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ACCOUNT)) {
    ftpc->account = curlx_strdup(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ACCOUNT));
    ಆದರೆ(!ftpc->account) {
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ALTERNATIVE_ಗಮ್ಯ_USER)) {
    ftpc->alternative_ಗಮ್ಯ_user =
      curlx_strdup(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTP_ALTERNATIVE_ಗಮ್ಯ_USER));
    ಆದರೆ(!ftpc->alternative_ಗಮ್ಯ_user) {
      curlx_safefree(ftpc->account);
      Curl_conn_meta_remove(conn, CURL_META_FTP_CONN);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ftp->ಮಾರ್ಗ = &ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1]; /* do not include the initial slash */

  type_url_check(ದತ್ತಾಂಶ, ftp);

  /* get some initial data into the ftp struct */
  ftp->transfer = PPTRANSFER_BODY;
  ftp->downloadsize = 0;
  ftpc->ತಿಳಿದ_filesize = -1; /* unknown size for now */
  ftpc->use_ssl = ದತ್ತಾಂಶ->ಗಣ.use_ssl;
  ftpc->ccc = ದತ್ತಾಂಶ->ಗಣ.ftp_ccc;

  CURL_TRC_FTP(ದತ್ತಾಂಶ, "[%s] setup connection -> %d", FTP_CSTATE(ftpc),
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_ftp_conns_ಹೊಂದಿಕೆ(ರಚನೆ connectdata *needle, ರಚನೆ connectdata *conn)
{
  ರಚನೆ ftp_conn *nftpc = Curl_conn_meta_get(needle, CURL_META_FTP_CONN);
  ರಚನೆ ftp_conn *cftpc = Curl_conn_meta_get(conn, CURL_META_FTP_CONN);
  /* Also match ACCOUNT, ALTERNATIVE-TO-USER and CCC options */
  ಆದರೆ(!nftpc || !cftpc ||
     Curl_timestrcmp(nftpc->account, cftpc->account) ||
     Curl_timestrcmp(nftpc->alternative_ಗಮ್ಯ_user,
                     cftpc->alternative_ಗಮ್ಯ_user) ||
     (nftpc->ccc != cftpc->ccc))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆಯ್ಕೆ(nftpc->use_ssl) {
  ಸ್ಥಿತಿ CURLUSESSL_TRY:
    /* A transfer that only "tries" SSL, is compatible with a connection
     * of similar or stricter SSL use setting. */
    ಹಿಂತಿರುಗಿ (cftpc->use_ssl != CURLUSESSL_NONE);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ (nftpc->use_ssl == cftpc->use_ssl);
  }
}

/*
 * FTP protocol.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_ftp = {
  ftp_ಸಿದ್ಧತೆ_connection,            /* setup_connection */
  ftp_do,                          /* do_it */
  ftp_ಮುಗಿದ,                        /* done */
  ftp_do_ಇನ್ನಷ್ಟು,                     /* do_more */
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_FTP */
