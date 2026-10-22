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
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FILE

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DIRENT_H
#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h" /* for the week day and month names */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_range.h"

#ਜੇ defined(_WIN32) || defined(MSDOS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOS_FILESYSTEM 1
#ਜਾਂਜੇ defined(__amigaos4__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMIGA_FILESYSTEM 1
#ਜੇ_ਅੰਤ

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_FILE_EASY   "meta:proto:file:easy"

ਬਣਤਰ FILEPROTO {
  ਅੱਖਰ *ਰਸਤਾ; /* the path we operate on */
  ਅੱਖਰ *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  ਪੂਰਨ fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ਸਥਿਰ ਖਾਲੀ file_cleanup(ਬਣਤਰ FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->ਰਸਤਾ = ਨਿਰਾ;
  file->is_dir = FALSE;
  ਜੇ(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ਸਥਿਰ ਖਾਲੀ file_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ FILEPROTO *file = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ਸਥਿਰ CURLcode file_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ FILEPROTO *filep;
  (ਖਾਲੀ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, ਆਕਾਰ(*filep));
  ਜੇ(filep)
    filep->fd = -1;
  ਜੇ(!filep ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode file_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          CURLcode ਸਥਿਤੀ, bool premature)
{
  ਬਣਤਰ FILEPROTO *file = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FILE_EASY);
  (ਖਾਲੀ)ਸਥਿਤੀ;
  (ਖਾਲੀ)premature;

  ਜੇ(file)
    file_cleanup(file);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਪੂਰਨ file_stat(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, curlx_struct_stat *statbuf)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਪੂਰਨ ਨਤੀਜਾ = curlx_stat(ਰਸਤਾ, statbuf);

  ਜੇ(ਨਤੀਜਾ) {
    size_t pathlen = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    ਜੇ((pathlen > 3) &&
       ((ਰਸਤਾ[pathlen - 1] == '\\') || (ਰਸਤਾ[pathlen - 1] == '/'))) {
      ਅੱਖਰ *trimmed = curlx_strdup(ਰਸਤਾ);

      ਜੇ(trimmed) {
        ਕਰੋ {
          trimmed[--pathlen] = '\0';
        } ਜਦੋਂ((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        ਨਤੀਜਾ = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ curlx_stat(ਰਸਤਾ, statbuf);
#ਜੇ_ਅੰਤ
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ਸਥਿਰ CURLcode file_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਅੱਖਰ *real_ਰਸਤਾ;
  ਬਣਤਰ FILEPROTO *file = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FILE_EASY);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  curlx_struct_stat statbuf;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ fd;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DOS_FILESYSTEM
  size_t i;
  ਅੱਖਰ *actual_ਰਸਤਾ;
#ਜੇ_ਅੰਤ
  size_t real_ਰਸਤਾ_len;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!file)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(file->ਰਸਤਾ) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = Curl_urldecode(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ, 0, &real_ਰਸਤਾ,
                          &real_ਰਸਤਾ_len, REJECT_ZERO);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DOS_FILESYSTEM
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
  actual_ਰਸਤਾ = real_ਰਸਤਾ;
  ਜੇ((actual_ਰਸਤਾ[0] == '/') &&
      actual_ਰਸਤਾ[1] &&
     (actual_ਰਸਤਾ[2] == ':' || actual_ਰਸਤਾ[2] == '|')) {
    actual_ਰਸਤਾ[2] = ':';
    actual_ਰਸਤਾ++;
    real_ਰਸਤਾ_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  ਲਈ(i = 0; i < real_ਰਸਤਾ_len; ++i)
    ਜੇ(actual_ਰਸਤਾ[i] == '/')
      actual_ਰਸਤਾ[i] = '\\';
    ਨਹੀਂਤਾਂ ਜੇ(!actual_ਰਸਤਾ[i]) { /* binary zero */
      curlx_safefree(real_ਰਸਤਾ);
      ਵਾਪਸ CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_ਰਸਤਾ, O_RDONLY | CURL_O_ਦੁਆਧਾਰੀ);
  file->ਰਸਤਾ = actual_ਰਸਤਾ;
#ਨਹੀਂਤਾਂ
  ਜੇ(memchr(real_ਰਸਤਾ, 0, real_ਰਸਤਾ_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_ਰਸਤਾ);
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->ਰਸਤਾ = real_ਰਸਤਾ;

  ਜੇ(real_ਰਸਤਾ[0] == '/') {
    ਬਾਹਰੀ ਪੂਰਨ __unix_path_semantics;
    ਜੇ(strchr(real_ਰਸਤਾ + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_ਰਸਤਾ + 1, O_RDONLY);
      file->ਰਸਤਾ++;
    }
    ਨਹੀਂਤਾਂ ਜੇ(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_ਰਸਤਾ, O_RDONLY);
    }
  }
#ਨਹੀਂਤਾਂ
  fd = curlx_open(real_ਰਸਤਾ, O_RDONLY);
  file->ਰਸਤਾ = real_ਰਸਤਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
  curlx_free(file->freepath);
  file->freepath = real_ਰਸਤਾ; /* free this when done */

  file->fd = fd;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ(!ਅੰਕੜੇ->state.upload && (fd == -1) &&
     !file_stat(file->ਰਸਤਾ, &statbuf) && S_ISDIR(statbuf.st_ਬਹੁਲਕ))
    file->is_dir = TRUE;
#ਜੇ_ਅੰਤ

  ਜੇ(!ਅੰਕੜੇ->state.upload && (fd == -1) && !file->is_dir) {
    failf(ਅੰਕੜੇ, "Could not open file %s", ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ);
    file_ਪੂਰਾ(ਅੰਕੜੇ, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ਵਾਪਸ CURLE_FILE_COULDNT_READ_FILE;
  }
  *ਪੂਰਾ = TRUE;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode file_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn,
                                bool dead_connection)
{
  (ਖਾਲੀ)dead_connection;
  (ਖਾਲੀ)conn;
  ਵਾਪਸ file_ਪੂਰਾ(ਅੰਕੜੇ, CURLE_OK, FALSE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DOS_FILESYSTEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRSEP '\\'
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRSEP '/'
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode file_upload(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ FILEPROTO *file)
{
  ਅਚਲ ਅੱਖਰ *dir = strchr(file->ਰਸਤਾ, DIRSEP);
  ਪੂਰਨ fd;
  ਪੂਰਨ ਬਹੁਲਕ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ਅਚਲ ਅੱਖਰ *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  ਜੇ(!dir)
    ਵਾਪਸ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ਜੇ(!dir[1])
    ਵਾਪਸ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ਬਹੁਲਕ = O_WRONLY | O_CREAT | CURL_O_ਦੁਆਧਾਰੀ;
  ਜੇ(ਅੰਕੜੇ->state.resume_from)
    ਬਹੁਲਕ |= O_APPEND;
  ਨਹੀਂਤਾਂ
    ਬਹੁਲਕ |= O_TRUNC;

  fd = curlx_open(file->ਰਸਤਾ, ਬਹੁਲਕ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
                  ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ਨਹੀਂਤਾਂ
                  (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms
#ਜੇ_ਅੰਤ
                  );
  ਜੇ(fd < 0) {
    failf(ਅੰਕੜੇ, "cannot open %s for writing", file->ਰਸਤਾ);
    ਵਾਪਸ CURLE_WRITE_ERROR;
  }

  ਜੇ(ਅੰਕੜੇ->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
    ਜੇ(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(ਅੰਕੜੇ, "cannot get the size of %s", file->ਰਸਤਾ);
      ਵਾਪਸ CURLE_WRITE_ERROR;
    }
    ਅੰਕੜੇ->state.resume_from = (curl_off_t)file_stat.st_ਮਾਪ;
  }

  ਨਤੀਜਾ = Curl_multi_xfer_ulbuf_borrow(ਅੰਕੜੇ, &xfer_ulbuf, &xfer_ulblen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜਦੋਂ(!ਨਤੀਜਾ && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    ਨਤੀਜਾ = Curl_client_read(ਅੰਕੜੇ, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਜੇ(!readcount)
      ਰੋਕੋ;

    nread = readcount;

    /* skip bytes before resume point */
    ਜੇ(ਅੰਕੜੇ->state.resume_from) {
      ਜੇ((curl_off_t)nread <= ਅੰਕੜੇ->state.resume_from) {
        ਅੰਕੜੇ->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      ਨਹੀਂਤਾਂ {
        sendbuf = xfer_ulbuf + ਅੰਕੜੇ->state.resume_from;
        nread -= (size_t)ਅੰਕੜੇ->state.resume_from;
        ਅੰਕੜੇ->state.resume_from = 0;
      }
    }
    ਨਹੀਂਤਾਂ
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = ਲਿਖ(fd, sendbuf, nread);
    ਜੇ(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਰੋਕੋ;
    }
    Curl_pgrs_upload_inc(ਅੰਕੜੇ, nwritten);

    ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(ਅੰਕੜੇ, xfer_ulbuf);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ਸਥਿਰ CURLcode win32_file_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
  WIN32_ਲੱਭੋ_ਅੰਕੜੇ ਇੰਦਰਾਜ;
  HANDLE handle;
  ਅੱਖਰ *ਨਮੂਨਾ;
  size_t pathlen = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  DWORD error;

  ਨਮੂਨਾ = curl_maprintf("%s%s*", ਰਸਤਾ,
                          pathlen &&
                          (ਰਸਤਾ[pathlen - 1] == '\\' ||
                           ਰਸਤਾ[pathlen - 1] == '/') ? "" : "\\");
  ਜੇ(!ਨਮੂਨਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  handle = curlx_ਲੱਭੋ_ਪਹਿਲਾ_File(ਨਮੂਨਾ, &ਇੰਦਰਾਜ);
  curlx_free(ਨਮੂਨਾ);
  ਜੇ(handle == INVALID_HANDLE_ਮੁੱਲ)
    ਵਾਪਸ CURLE_READ_ERROR;

  ਕਰੋ {
    ਜੇ(ਇੰਦਰਾਜ.cFileName[0] != ਪਾਠ_2('.')) {
      ਅੱਖਰ *name = curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(ਇੰਦਰਾਜ.cFileName);

      ਜੇ(!name) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਰੋਕੋ;
      }

      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                 name, ਲੜੀ_ਲੰਬਾਈ(name));
      curlx_free(name);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;

      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }
  } ਜਦੋਂ(ਲੱਭੋ_ਅਗਲਾ_File(handle, &ਇੰਦਰਾਜ));

  error = Get_ਆਖਰੀ_Error();
  ਜੇ(!ਨਤੀਜਾ && error != ERROR_NO_ਹੋਰ_FILES)
    ਨਤੀਜਾ = CURLE_READ_ERROR;

  ਲੱਭੋ_Close(handle);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ਸਥਿਰ CURLcode file_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ਬਣਤਰ FILEPROTO *file = Curl_meta_get(ਅੰਕੜੇ, CURL_META_FILE_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_ਮਾਪ = -1;
  bool ਮਾਪ_ਗਿਆਤ;
  bool fstated = FALSE;
  ਪੂਰਨ fd;
  ਅੱਖਰ *xfer_buf;
  size_t xfer_blen;

  *ਪੂਰਾ = TRUE; /* unconditionally */
  ਜੇ(!file)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ਅੰਕੜੇ->state.upload)
    ਵਾਪਸ file_upload(ਅੰਕੜੇ, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  ਜੇ((file->is_dir ? file_stat(file->ਰਸਤਾ, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_ਬਹੁਲਕ);
    ਜੇ(!file->is_dir)
      expected_ਮਾਪ = statbuf.st_ਮਾਪ;
    /* and store the modification time */
    ਅੰਕੜੇ->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  ਜੇ(fstated && !ਅੰਕੜੇ->state.range && ਅੰਕੜੇ->ਸੈੱਟ.timecondition &&
     !Curl_meets_timecondition(ਅੰਕੜੇ, ਅੰਕੜੇ->info.filetime))
    ਵਾਪਸ CURLE_OK;

  ਜੇ(fstated) {
    time_t filetime;
    ਬਣਤਰ tm ਬਫਰ;
    ਅਚਲ ਬਣਤਰ tm *tm = &ਬਫਰ;
    ਅੱਖਰ ਸਿਰਲੇਖ[80];
    ਪੂਰਨ headerlen;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    ਜੇ(expected_ਮਾਪ >= 0) {
      headerlen =
        curl_msnprintf(ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_ਮਾਪ);
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, ਸਿਰਲੇਖ, headerlen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      ਜੇ(ਨਤੀਜਾ != CURLE_OK)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    filetime = (time_t)statbuf.st_mtime;
    ਨਤੀਜਾ = curlx_gmtime(filetime, &ਬਫਰ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, ਸਿਰਲੇਖ, headerlen);
    ਜੇ(!ਨਤੀਜਾ)
      /* end of headers */
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, "\r\n", 2);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    /* set the file size to make it available post transfer */
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, expected_ਮਾਪ);
    ਜੇ(ਅੰਕੜੇ->req.no_body)
      ਵਾਪਸ CURLE_OK;
  }

  /* Check whether file range has been specified */
  ਨਤੀਜਾ = Curl_range(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
    ਜੇ(!fstated) {
      failf(ਅੰਕੜੇ, "cannot get the size of file.");
      ਵਾਪਸ CURLE_READ_ERROR;
    }
    ਅੰਕੜੇ->state.resume_from += (curl_off_t)statbuf.st_ਮਾਪ;
  }

  ਜੇ(ਅੰਕੜੇ->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    ਜੇ(ਅੰਕੜੇ->state.resume_from <= expected_ਮਾਪ)
      expected_ਮਾਪ -= ਅੰਕੜੇ->state.resume_from;
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "failed to resume file:// transfer");
      ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  ਜੇ(ਅੰਕੜੇ->req.maxdownload > 0)
    expected_ਮਾਪ = ਅੰਕੜੇ->req.maxdownload;

  ਜੇ(!fstated || (expected_ਮਾਪ <= 0))
    ਮਾਪ_ਗਿਆਤ = FALSE;
  ਨਹੀਂਤਾਂ
    ਮਾਪ_ਗਿਆਤ = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  ਜੇ(ਮਾਪ_ਗਿਆਤ)
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, expected_ਮਾਪ);

  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    ਜੇ(!file->is_dir) {
      ਜੇ(ਅੰਕੜੇ->state.resume_from !=
         curl_lseek(fd, ਅੰਕੜੇ->state.resume_from, SEEK_SET))
        ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ਨਹੀਂਤਾਂ {
      ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  ਨਤੀਜਾ = Curl_multi_xfer_buf_borrow(ਅੰਕੜੇ, &xfer_buf, &xfer_blen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!file->is_dir) {
    ਜਦੋਂ(!ਨਤੀਜਾ) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      ਜੇ(ਮਾਪ_ਗਿਆਤ) {
        bytestoread = (expected_ਮਾਪ < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_ਮਾਪ) : (xfer_blen - 1);
      }
      ਨਹੀਂਤਾਂ
        bytestoread = xfer_blen - 1;

      nread = ਪਠ(fd, xfer_buf, bytestoread);

      ਜੇ(nread > 0)
        xfer_buf[nread] = 0;

      ਜੇ(nread <= 0 || (ਮਾਪ_ਗਿਆਤ && (expected_ਮਾਪ == 0)))
        ਰੋਕੋ;

      ਜੇ(ਮਾਪ_ਗਿਆਤ)
        expected_ਮਾਪ -= nread;

      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, xfer_buf, nread);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    ਨਤੀਜਾ = win32_file_list(ਅੰਕੜੇ, file->ਰਸਤਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
#ਜਾਂਜੇ defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->ਰਸਤਾ);
    ਬਣਤਰ dirent *ਇੰਦਰਾਜ;

    ਜੇ(!dir) {
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਜਦੋਂ((ਇੰਦਰਾਜ = readdir(dir))) {
        ਜੇ(ਇੰਦਰਾਜ->d_name[0] != '.') {
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                   ਇੰਦਰਾਜ->d_name, ਲੜੀ_ਲੰਬਾਈ(ਇੰਦਰਾਜ->d_name));
          ਜੇ(ਨਤੀਜਾ)
            ਰੋਕੋ;
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
          ਜੇ(ਨਤੀਜਾ)
            ਰੋਕੋ;
        }
      }
      closedir(dir);
    }
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "Directory listing not yet implemented on this platform.");
    ਨਤੀਜਾ = CURLE_READ_ERROR;
#ਜੇ_ਅੰਤ
  }

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  Curl_multi_xfer_buf_release(ਅੰਕੜੇ, xfer_buf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_file = {
  file_ਤਿਆਰੀ_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_ਪੂਰਾ,                            /* done */
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

#ਜੇ_ਅੰਤ
