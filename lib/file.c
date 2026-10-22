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
#ಅನ್ತರ್ಭಾವ "file.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FILE

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DIRENT_H
#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h" /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_range.h"

#ಆದರೆ defined(_WIN32) || defined(MSDOS)
#ವ್ಯಾಖ್ಯಾನಿಸು DOS_FILESYSTEM 1
#ಅಥವಾಆದರೆ defined(__amigaos4__)
#ವ್ಯಾಖ್ಯಾನಿಸು AMIGA_FILESYSTEM 1
#ಆದರೆ_ಅಂತ್ಯ

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_FILE_EASY   "meta:proto:file:easy"

ರಚನೆ FILEPROTO {
  ಅಕ್ಷರ *ಮಾರ್ಗ; /* the path we operate on */
  ಅಕ್ಷರ *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  ಪೂರ್ಣಾಂಕ fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ಸ್ಥಿರ ಶೂನ್ಯ file_cleanup(ರಚನೆ FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->ಮಾರ್ಗ = ಖಾಲಿ;
  file->is_dir = FALSE;
  ಆದರೆ(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ file_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ FILEPROTO *file = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ಸ್ಥಿರ CURLcode file_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ FILEPROTO *filep;
  (ಶೂನ್ಯ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, ಗಾತ್ರ(*filep));
  ಆದರೆ(filep)
    filep->fd = -1;
  ಆದರೆ(!filep ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode file_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          CURLcode ಸ್ಥಿತಿ_2, bool premature)
{
  ರಚನೆ FILEPROTO *file = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FILE_EASY);
  (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)premature;

  ಆದರೆ(file)
    file_cleanup(file);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ file_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, curlx_struct_stat *statbuf)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = curlx_stat(ಮಾರ್ಗ, statbuf);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    size_t pathlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    ಆದರೆ((pathlen > 3) &&
       ((ಮಾರ್ಗ[pathlen - 1] == '\\') || (ಮಾರ್ಗ[pathlen - 1] == '/'))) {
      ಅಕ್ಷರ *trimmed = curlx_strdup(ಮಾರ್ಗ);

      ಆದರೆ(trimmed) {
        ಮಾಡು {
          trimmed[--pathlen] = '\0';
        } ಆಗ((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        ಫಲಿತಾಂಶ = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ curlx_stat(ಮಾರ್ಗ, statbuf);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ಸ್ಥಿರ CURLcode file_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಅಕ್ಷರ *real_ಮಾರ್ಗ;
  ರಚನೆ FILEPROTO *file = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FILE_EASY);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  curlx_struct_stat statbuf;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ fd;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DOS_FILESYSTEM
  size_t i;
  ಅಕ್ಷರ *actual_ಮಾರ್ಗ;
#ಆದರೆ_ಅಂತ್ಯ
  size_t real_ಮಾರ್ಗ_len;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!file)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(file->ಮಾರ್ಗ) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = Curl_urldecode(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ, 0, &real_ಮಾರ್ಗ,
                          &real_ಮಾರ್ಗ_len, REJECT_ZERO);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DOS_FILESYSTEM
  /* If the first character is a slash, and there is
     something that looks like a drive at the beginning of
     the path, skip the slash. If we remove the initial
     slash in all cases, paths without drive letters end up
     relative to the current directory which is not how
     browsers work.

     Some browsers accept | instead of : as the drive letter
     separator, so we do too.

     On other platforms, we need the slash to indicate an
     absolute pathname. On Windows, absolute paths start
     with a drive letter. */
  actual_ಮಾರ್ಗ = real_ಮಾರ್ಗ;
  ಆದರೆ((actual_ಮಾರ್ಗ[0] == '/') &&
      actual_ಮಾರ್ಗ[1] &&
     (actual_ಮಾರ್ಗ[2] == ':' || actual_ಮಾರ್ಗ[2] == '|')) {
    actual_ಮಾರ್ಗ[2] = ':';
    actual_ಮಾರ್ಗ++;
    real_ಮಾರ್ಗ_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  ಗಾಗಿ(i = 0; i < real_ಮಾರ್ಗ_len; ++i)
    ಆದರೆ(actual_ಮಾರ್ಗ[i] == '/')
      actual_ಮಾರ್ಗ[i] = '\\';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!actual_ಮಾರ್ಗ[i]) { /* binary zero */
      curlx_safefree(real_ಮಾರ್ಗ);
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_ಮಾರ್ಗ, O_RDONLY | CURL_O_ದ್ವಿಮಾನ);
  file->ಮಾರ್ಗ = actual_ಮಾರ್ಗ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(memchr(real_ಮಾರ್ಗ, 0, real_ಮಾರ್ಗ_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_ಮಾರ್ಗ);
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->ಮಾರ್ಗ = real_ಮಾರ್ಗ;

  ಆದರೆ(real_ಮಾರ್ಗ[0] == '/') {
    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ __unix_path_semantics;
    ಆದರೆ(strchr(real_ಮಾರ್ಗ + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_ಮಾರ್ಗ + 1, O_RDONLY);
      file->ಮಾರ್ಗ++;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_ಮಾರ್ಗ, O_RDONLY);
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  fd = curlx_open(real_ಮಾರ್ಗ, O_RDONLY);
  file->ಮಾರ್ಗ = real_ಮಾರ್ಗ;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
  curlx_free(file->freepath);
  file->freepath = real_ಮಾರ್ಗ; /* free this when done */

  file->fd = fd;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ(!ದತ್ತಾಂಶ->state.upload && (fd == -1) &&
     !file_stat(file->ಮಾರ್ಗ, &statbuf) && S_ISDIR(statbuf.st_ಬಹುಲಕ))
    file->is_dir = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!ದತ್ತಾಂಶ->state.upload && (fd == -1) && !file->is_dir) {
    failf(ದತ್ತಾಂಶ, "Could not open file %s", ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ);
    file_ಮುಗಿದ(ದತ್ತಾಂಶ, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ಹಿಂತಿರುಗಿ CURLE_FILE_COULDNT_READ_FILE;
  }
  *ಮುಗಿದ = TRUE;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode file_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn,
                                bool dead_connection)
{
  (ಶೂನ್ಯ)dead_connection;
  (ಶೂನ್ಯ)conn;
  ಹಿಂತಿರುಗಿ file_ಮುಗಿದ(ದತ್ತಾಂಶ, CURLE_OK, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DOS_FILESYSTEM
#ವ್ಯಾಖ್ಯಾನಿಸು DIRSEP '\\'
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DIRSEP '/'
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode file_upload(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ FILEPROTO *file)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir = strchr(file->ಮಾರ್ಗ, DIRSEP);
  ಪೂರ್ಣಾಂಕ fd;
  ಪೂರ್ಣಾಂಕ ಬಹುಲಕ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  ಆದರೆ(!dir)
    ಹಿಂತಿರುಗಿ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ಆದರೆ(!dir[1])
    ಹಿಂತಿರುಗಿ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ಬಹುಲಕ = O_WRONLY | O_CREAT | CURL_O_ದ್ವಿಮಾನ;
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from)
    ಬಹುಲಕ |= O_APPEND;
  ಇಲ್ಲದಿದ್ದರೆ
    ಬಹುಲಕ |= O_TRUNC;

  fd = curlx_open(file->ಮಾರ್ಗ, ಬಹುಲಕ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
                  ದತ್ತಾಂಶ->ಗಣ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ಇಲ್ಲದಿದ್ದರೆ
                  (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_file_perms
#ಆದರೆ_ಅಂತ್ಯ
                  );
  ಆದರೆ(fd < 0) {
    failf(ದತ್ತಾಂಶ, "cannot open %s for writing", file->ಮಾರ್ಗ);
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
    ಆದರೆ(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(ದತ್ತಾಂಶ, "cannot get the size of %s", file->ಮಾರ್ಗ);
      ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
    }
    ದತ್ತಾಂಶ->state.resume_from = (curl_off_t)file_stat.st_ಗಾತ್ರ;
  }

  ಫಲಿತಾಂಶ = Curl_multi_xfer_ulbuf_borrow(ದತ್ತಾಂಶ, &xfer_ulbuf, &xfer_ulblen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆಗ(!ಫಲಿತಾಂಶ && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    ಫಲಿತಾಂಶ = Curl_client_read(ದತ್ತಾಂಶ, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(!readcount)
      ನಿಲ್ಲಿಸು;

    nread = readcount;

    /* skip bytes before resume point */
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
      ಆದರೆ((curl_off_t)nread <= ದತ್ತಾಂಶ->state.resume_from) {
        ದತ್ತಾಂಶ->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        sendbuf = xfer_ulbuf + ದತ್ತಾಂಶ->state.resume_from;
        nread -= (size_t)ದತ್ತಾಂಶ->state.resume_from;
        ದತ್ತಾಂಶ->state.resume_from = 0;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = ಲಿಖ(fd, sendbuf, nread);
    ಆದರೆ(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    Curl_pgrs_upload_inc(ದತ್ತಾಂಶ, nwritten);

    ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(ದತ್ತಾಂಶ, xfer_ulbuf);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ಸ್ಥಿರ CURLcode win32_file_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  WIN32_ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ ನಮೂದು;
  HANDLE handle;
  ಅಕ್ಷರ *ನಮೂನೆ;
  size_t pathlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  DWORD error;

  ನಮೂನೆ = curl_maprintf("%s%s*", ಮಾರ್ಗ,
                          pathlen &&
                          (ಮಾರ್ಗ[pathlen - 1] == '\\' ||
                           ಮಾರ್ಗ[pathlen - 1] == '/') ? "" : "\\");
  ಆದರೆ(!ನಮೂನೆ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  handle = curlx_ಕಂಡುಹಿಡಿ_ಮೊದಲ_File(ನಮೂನೆ, &ನಮೂದು);
  curlx_free(ನಮೂನೆ);
  ಆದರೆ(handle == INVALID_HANDLE_ಮೌಲ್ಯ)
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;

  ಮಾಡು {
    ಆದರೆ(ನಮೂದು.cFileName[0] != ಪಠ್ಯ_2('.')) {
      ಅಕ್ಷರ *name = curlx_convert_tchar_ಗಮ್ಯ_UTF8(ನಮೂದು.cFileName);

      ಆದರೆ(!name) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ನಿಲ್ಲಿಸು;
      }

      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                 name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
      curlx_free(name);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;

      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_File(handle, &ನಮೂದು));

  error = Get_ಕೊನೆಯ_Error();
  ಆದರೆ(!ಫಲಿತಾಂಶ && error != ERROR_NO_ಇನ್ನಷ್ಟು_FILES)
    ಫಲಿತಾಂಶ = CURLE_READ_ERROR;

  ಕಂಡುಹಿಡಿ_Close(handle);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ಸ್ಥಿರ CURLcode file_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ರಚನೆ FILEPROTO *file = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_FILE_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_ಗಾತ್ರ = -1;
  bool ಗಾತ್ರ_ತಿಳಿದ;
  bool fstated = FALSE;
  ಪೂರ್ಣಾಂಕ fd;
  ಅಕ್ಷರ *xfer_buf;
  size_t xfer_blen;

  *ಮುಗಿದ = TRUE; /* unconditionally */
  ಆದರೆ(!file)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
    ಹಿಂತಿರುಗಿ file_upload(ದತ್ತಾಂಶ, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  ಆದರೆ((file->is_dir ? file_stat(file->ಮಾರ್ಗ, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_ಬಹುಲಕ);
    ಆದರೆ(!file->is_dir)
      expected_ಗಾತ್ರ = statbuf.st_ಗಾತ್ರ;
    /* and store the modification time */
    ದತ್ತಾಂಶ->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  ಆದರೆ(fstated && !ದತ್ತಾಂಶ->state.range && ದತ್ತಾಂಶ->ಗಣ.timecondition &&
     !Curl_meets_timecondition(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->info.filetime))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(fstated) {
    time_t filetime;
    ರಚನೆ tm ಬಫರ್;
    ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm = &ಬಫರ್;
    ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ[80];
    ಪೂರ್ಣಾಂಕ headerlen;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    ಆದರೆ(expected_ಗಾತ್ರ >= 0) {
      headerlen =
        curl_msnprintf(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_ಗಾತ್ರ);
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, ಶೀರ್ಷಿಕೆ, headerlen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    filetime = (time_t)statbuf.st_mtime;
    ಫಲಿತಾಂಶ = curlx_gmtime(filetime, &ಬಫರ್);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, ಶೀರ್ಷಿಕೆ, headerlen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      /* end of headers */
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, "\r\n", 2);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    /* set the file size to make it available post transfer */
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, expected_ಗಾತ್ರ);
    ಆದರೆ(ದತ್ತಾಂಶ->req.no_body)
      ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Check whether file range has been specified */
  ಫಲಿತಾಂಶ = Curl_range(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
    ಆದರೆ(!fstated) {
      failf(ದತ್ತಾಂಶ, "cannot get the size of file.");
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
    }
    ದತ್ತಾಂಶ->state.resume_from += (curl_off_t)statbuf.st_ಗಾತ್ರ;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from <= expected_ಗಾತ್ರ)
      expected_ಗಾತ್ರ -= ದತ್ತಾಂಶ->state.resume_from;
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "failed to resume file:// transfer");
      ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  ಆದರೆ(ದತ್ತಾಂಶ->req.maxdownload > 0)
    expected_ಗಾತ್ರ = ದತ್ತಾಂಶ->req.maxdownload;

  ಆದರೆ(!fstated || (expected_ಗಾತ್ರ <= 0))
    ಗಾತ್ರ_ತಿಳಿದ = FALSE;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗಾತ್ರ_ತಿಳಿದ = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  ಆದರೆ(ಗಾತ್ರ_ತಿಳಿದ)
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, expected_ಗಾತ್ರ);

  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    ಆದರೆ(!file->is_dir) {
      ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from !=
         curl_lseek(fd, ದತ್ತಾಂಶ->state.resume_from, SEEK_SET))
        ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  ಫಲಿತಾಂಶ = Curl_multi_xfer_buf_borrow(ದತ್ತಾಂಶ, &xfer_buf, &xfer_blen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!file->is_dir) {
    ಆಗ(!ಫಲಿತಾಂಶ) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      ಆದರೆ(ಗಾತ್ರ_ತಿಳಿದ) {
        bytestoread = (expected_ಗಾತ್ರ < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_ಗಾತ್ರ) : (xfer_blen - 1);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        bytestoread = xfer_blen - 1;

      nread = ಪಠ(fd, xfer_buf, bytestoread);

      ಆದರೆ(nread > 0)
        xfer_buf[nread] = 0;

      ಆದರೆ(nread <= 0 || (ಗಾತ್ರ_ತಿಳಿದ && (expected_ಗಾತ್ರ == 0)))
        ನಿಲ್ಲಿಸು;

      ಆದರೆ(ಗಾತ್ರ_ತಿಳಿದ)
        expected_ಗಾತ್ರ -= nread;

      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, xfer_buf, nread);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    ಫಲಿತಾಂಶ = win32_file_list(ದತ್ತಾಂಶ, file->ಮಾರ್ಗ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
#ಅಥವಾಆದರೆ defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->ಮಾರ್ಗ);
    ರಚನೆ dirent *ನಮೂದು;

    ಆದರೆ(!dir) {
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆಗ((ನಮೂದು = readdir(dir))) {
        ಆದರೆ(ನಮೂದು->d_name[0] != '.') {
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                   ನಮೂದು->d_name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ನಮೂದು->d_name));
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ನಿಲ್ಲಿಸು;
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ನಿಲ್ಲಿಸು;
        }
      }
      closedir(dir);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "Directory listing not yet implemented on this platform.");
    ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  Curl_multi_xfer_buf_release(ದತ್ತಾಂಶ, xfer_buf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_file = {
  file_ಸಿದ್ಧತೆ_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_ಮುಗಿದ,                            /* done */
  ZERO_NULL,                            /* do_more */
  file_connect,                         /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  file_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#ಆದರೆ_ಅಂತ್ಯ
