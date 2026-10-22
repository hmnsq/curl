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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "file.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FILE

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_DIRENT_H
#ضمن <dirent.h>
#جيڪڏهن_پورو

#ضمن "progress.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "multiif.h"
#ضمن "transfer.h"
#ضمن "url.h"
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "curlx/win32-fopen.h"
#ضمن "curl_range.h"

#إذا defined(_WIN32) || defined(MSDOS)
#وصف_ڪريو DOS_FILESYSTEM 1
#وإلاإذا defined(__amigaos4__)
#وصف_ڪريو AMIGA_FILESYSTEM 1
#جيڪڏهن_پورو

/* meta key for storing protocol meta at easy handle */
#وصف_ڪريو CURL_META_FILE_EASY   "meta:proto:file:easy"

بنية FILEPROTO {
  حرف *رستو; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحيح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساكن فراغ file_cleanup(بنية FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->رستو = عدم;
  file->is_dir = FALSE;
  إذا(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساكن فراغ file_easy_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية FILEPROTO *file = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساكن CURLcode file_تياري_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية FILEPROTO *filep;
  (فراغ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  إذا(filep)
    filep->fd = -1;
  إذا(!filep ||
     Curl_meta_سيٽ(معلومات, CURL_META_FILE_EASY, filep, file_easy_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  أرجع CURLE_OK;
}

ساكن CURLcode file_پورو(بنية Curl_easy *معلومات,
                          CURLcode حالت, bool premature)
{
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
  (فراغ)حالت;
  (فراغ)premature;

  إذا(file)
    file_cleanup(file);

  أرجع CURLE_OK;
}

ساكن صحيح file_stat(ثابت حرف *رستو, curlx_struct_stat *statbuf)
{
#جيڪڏهن_وصف_ٿيل _WIN32
  صحيح نتيجو = curlx_stat(رستو, statbuf);

  إذا(نتيجو) {
    size_t pathlen = لڙي_ڊيگهه(رستو);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    إذا((pathlen > 3) &&
       ((رستو[pathlen - 1] == '\\') || (رستو[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(رستو);

      إذا(trimmed) {
        افعل {
          trimmed[--pathlen] = '\0';
        } طالما((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        نتيجو = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  أرجع نتيجو;
#وإلا
  أرجع curlx_stat(رستو, statbuf);
#جيڪڏهن_پورو
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساكن CURLcode file_connect(بنية Curl_easy *معلومات, bool *پورو)
{
  حرف *real_رستو;
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
#جيڪڏهن_وصف_ٿيل _WIN32
  curlx_struct_stat statbuf;
#جيڪڏهن_پورو
  صحيح fd;
#جيڪڏهن_وصف_ٿيل DOS_FILESYSTEM
  size_t i;
  حرف *actual_رستو;
#جيڪڏهن_پورو
  size_t real_رستو_len;
  CURLcode نتيجو;

  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(file->رستو) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  نتيجو = Curl_urldecode(معلومات->state.مٿي.رستو, 0, &real_رستو,
                          &real_رستو_len, REJECT_ZERO);
  إذا(نتيجو)
    أرجع نتيجو;

#جيڪڏهن_وصف_ٿيل DOS_FILESYSTEM
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
  actual_رستو = real_رستو;
  إذا((actual_رستو[0] == '/') &&
      actual_رستو[1] &&
     (actual_رستو[2] == ':' || actual_رستو[2] == '|')) {
    actual_رستو[2] = ':';
    actual_رستو++;
    real_رستو_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  لكل(i = 0; i < real_رستو_len; ++i)
    إذا(actual_رستو[i] == '/')
      actual_رستو[i] = '\\';
    وإلا إذا(!actual_رستو[i]) { /* binary zero */
      curlx_safefree(real_رستو);
      أرجع CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_رستو, O_RDONLY | CURL_O_ٻٽو);
  file->رستو = actual_رستو;
#وإلا
  إذا(memchr(real_رستو, 0, real_رستو_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_رستو);
    أرجع CURLE_URL_MALFORMAT;
  }

#جيڪڏهن_وصف_ٿيل AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->رستو = real_رستو;

  إذا(real_رستو[0] == '/') {
    خارجي صحيح __unix_path_semantics;
    إذا(strchr(real_رستو + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_رستو + 1, O_RDONLY);
      file->رستو++;
    }
    وإلا إذا(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_رستو, O_RDONLY);
    }
  }
#وإلا
  fd = curlx_open(real_رستو, O_RDONLY);
  file->رستو = real_رستو;
#جيڪڏهن_پورو
#جيڪڏهن_پورو
  curlx_free(file->freepath);
  file->freepath = real_رستو; /* free this when done */

  file->fd = fd;
#جيڪڏهن_وصف_ٿيل _WIN32
  إذا(!معلومات->state.upload && (fd == -1) &&
     !file_stat(file->رستو, &statbuf) && S_ISDIR(statbuf.st_گهڻائي))
    file->is_dir = TRUE;
#جيڪڏهن_پورو

  إذا(!معلومات->state.upload && (fd == -1) && !file->is_dir) {
    failf(معلومات, "Could not open file %s", معلومات->state.مٿي.رستو);
    file_پورو(معلومات, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    أرجع CURLE_FILE_COULDNT_READ_FILE;
  }
  *پورو = TRUE;

  أرجع CURLE_OK;
}

ساكن CURLcode file_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  (فراغ)dead_connection;
  (فراغ)conn;
  أرجع file_پورو(معلومات, CURLE_OK, FALSE);
}

#جيڪڏهن_وصف_ٿيل DOS_FILESYSTEM
#وصف_ڪريو DIRSEP '\\'
#وإلا
#وصف_ڪريو DIRSEP '/'
#جيڪڏهن_پورو

ساكن CURLcode file_upload(بنية Curl_easy *معلومات,
                            بنية FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->رستو, DIRSEP);
  صحيح fd;
  صحيح گهڻائي;
  CURLcode نتيجو = CURLE_OK;
  حرف *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ثابت حرف *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  إذا(!dir)
    أرجع CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  إذا(!dir[1])
    أرجع CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  گهڻائي = O_WRONLY | O_CREAT | CURL_O_ٻٽو;
  إذا(معلومات->state.resume_from)
    گهڻائي |= O_APPEND;
  وإلا
    گهڻائي |= O_TRUNC;

  fd = curlx_open(file->رستو, گهڻائي,
#جيڪڏهن_وصف_ٿيل _WIN32
                  معلومات->سيٽ.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                  (mode_t)معلومات->سيٽ.new_file_perms
#جيڪڏهن_پورو
                  );
  إذا(fd < 0) {
    failf(معلومات, "cannot open %s for writing", file->رستو);
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(معلومات->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_سيٽ_Upload_سائيز(معلومات, معلومات->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  إذا(معلومات->state.resume_from < 0) {
    إذا(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(معلومات, "cannot get the size of %s", file->رستو);
      أرجع CURLE_WRITE_ERROR;
    }
    معلومات->state.resume_from = (curl_off_t)file_stat.st_سائيز;
  }

  نتيجو = Curl_multi_xfer_ulbuf_borrow(معلومات, &xfer_ulbuf, &xfer_ulblen);
  إذا(نتيجو)
    اذهب ٻاهريون;

  طالما(!نتيجو && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    نتيجو = Curl_client_read(معلومات, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    إذا(نتيجو)
      اكسر;

    إذا(!readcount)
      اكسر;

    nread = readcount;

    /* skip bytes before resume point */
    إذا(معلومات->state.resume_from) {
      إذا((curl_off_t)nread <= معلومات->state.resume_from) {
        معلومات->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      وإلا {
        sendbuf = xfer_ulbuf + معلومات->state.resume_from;
        nread -= (size_t)معلومات->state.resume_from;
        معلومات->state.resume_from = 0;
      }
    }
    وإلا
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    إذا(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      نتيجو = CURLE_SEND_ERROR;
      اكسر;
    }
    Curl_pgrs_upload_inc(معلومات, nwritten);

    نتيجو = Curl_pgrsCheck(معلومات);
  }
  إذا(!نتيجو)
    نتيجو = Curl_pgrsUpdate(معلومات);

ٻاهريون:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(معلومات, xfer_ulbuf);

  أرجع نتيجو;
}

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساكن CURLcode win32_file_list(بنية Curl_easy *معلومات, ثابت حرف *رستو)
{
  WIN32_ڳوليو_معلومات داخلا_2;
  HANDLE handle;
  حرف *نمونو;
  size_t pathlen = لڙي_ڊيگهه(رستو);
  CURLcode نتيجو = CURLE_OK;
  DWORD error;

  نمونو = curl_maprintf("%s%s*", رستو,
                          pathlen &&
                          (رستو[pathlen - 1] == '\\' ||
                           رستو[pathlen - 1] == '/') ? "" : "\\");
  إذا(!نمونو)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  handle = curlx_ڳوليو_پهريون_File(نمونو, &داخلا_2);
  curlx_free(نمونو);
  إذا(handle == INVALID_HANDLE_قدر)
    أرجع CURLE_READ_ERROR;

  افعل {
    إذا(داخلا_2.cFileName[0] != متن_2('.')) {
      حرف *name = curlx_convert_tchar_منزل_UTF8(داخلا_2.cFileName);

      إذا(!name) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اكسر;
      }

      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                 name, لڙي_ڊيگهه(name));
      curlx_free(name);
      إذا(نتيجو)
        اكسر;

      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتيجو)
        اكسر;
    }
  } طالما(ڳوليو_ايندڙ_File(handle, &داخلا_2));

  error = Get_آخري_Error();
  إذا(!نتيجو && error != ERROR_NO_وڌيڪ_FILES)
    نتيجو = CURLE_READ_ERROR;

  ڳوليو_Close(handle);
  أرجع نتيجو;
}
#جيڪڏهن_پورو

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ساكن CURLcode file_do(بنية Curl_easy *معلومات, bool *پورو)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
  CURLcode نتيجو = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_سائيز = -1;
  bool سائيز_معلوم;
  bool fstated = FALSE;
  صحيح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *پورو = TRUE; /* unconditionally */
  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(معلومات->state.upload)
    أرجع file_upload(معلومات, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  إذا((file->is_dir ? file_stat(file->رستو, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_گهڻائي);
    إذا(!file->is_dir)
      expected_سائيز = statbuf.st_سائيز;
    /* and store the modification time */
    معلومات->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  إذا(fstated && !معلومات->state.range && معلومات->سيٽ.timecondition &&
     !Curl_meets_timecondition(معلومات, معلومات->info.filetime))
    أرجع CURLE_OK;

  إذا(fstated) {
    time_t filetime;
    بنية tm بفر;
    ثابت بنية tm *tm = &بفر;
    حرف سرو[80];
    صحيح headerlen;
    ساكن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    إذا(expected_سائيز >= 0) {
      headerlen =
        curl_msnprintf(سرو, حجم(سرو),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_سائيز);
      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_سرو, سرو, headerlen);
      إذا(نتيجو)
        أرجع نتيجو;

      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_سرو,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      إذا(نتيجو != CURLE_OK)
        أرجع نتيجو;
    }

    filetime = (time_t)statbuf.st_mtime;
    نتيجو = curlx_gmtime(filetime, &بفر);
    إذا(نتيجو)
      أرجع نتيجو;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(سرو, حجم(سرو),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    نتيجو = Curl_client_write(معلومات, CLIENTWRITE_سرو, سرو, headerlen);
    إذا(!نتيجو)
      /* end of headers */
      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_سرو, "\r\n", 2);
    إذا(نتيجو)
      أرجع نتيجو;
    /* set the file size to make it available post transfer */
    Curl_pgrs_سيٽ_Download_سائيز(معلومات, expected_سائيز);
    إذا(معلومات->req.no_body)
      أرجع CURLE_OK;
  }

  /* Check whether file range has been specified */
  نتيجو = Curl_range(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  إذا(معلومات->state.resume_from < 0) {
    إذا(!fstated) {
      failf(معلومات, "cannot get the size of file.");
      أرجع CURLE_READ_ERROR;
    }
    معلومات->state.resume_from += (curl_off_t)statbuf.st_سائيز;
  }

  إذا(معلومات->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    إذا(معلومات->state.resume_from <= expected_سائيز)
      expected_سائيز -= معلومات->state.resume_from;
    وإلا {
      failf(معلومات, "failed to resume file:// transfer");
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  إذا(معلومات->req.maxdownload > 0)
    expected_سائيز = معلومات->req.maxdownload;

  إذا(!fstated || (expected_سائيز <= 0))
    سائيز_معلوم = FALSE;
  وإلا
    سائيز_معلوم = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  إذا(سائيز_معلوم)
    Curl_pgrs_سيٽ_Download_سائيز(معلومات, expected_سائيز);

  إذا(معلومات->state.resume_from) {
    إذا(!file->is_dir) {
      إذا(معلومات->state.resume_from !=
         curl_lseek(fd, معلومات->state.resume_from, SEEK_SET))
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    وإلا {
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  نتيجو = Curl_multi_xfer_buf_borrow(معلومات, &xfer_buf, &xfer_blen);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(!file->is_dir) {
    طالما(!نتيجو) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      إذا(سائيز_معلوم) {
        bytestoread = (expected_سائيز < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_سائيز) : (xfer_blen - 1);
      }
      وإلا
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      إذا(nread > 0)
        xfer_buf[nread] = 0;

      إذا(nread <= 0 || (سائيز_معلوم && (expected_سائيز == 0)))
        اكسر;

      إذا(سائيز_معلوم)
        expected_سائيز -= nread;

      نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, xfer_buf, nread);
      إذا(نتيجو)
        اذهب ٻاهريون;

      نتيجو = Curl_pgrsCheck(معلومات);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }
  وإلا {
#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    نتيجو = win32_file_list(معلومات, file->رستو);
    إذا(نتيجو)
      اذهب ٻاهريون;
#وإلاإذا defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->رستو);
    بنية dirent *داخلا_2;

    إذا(!dir) {
      نتيجو = CURLE_READ_ERROR;
      اذهب ٻاهريون;
    }
    وإلا {
      طالما((داخلا_2 = readdir(dir))) {
        إذا(داخلا_2->d_name[0] != '.') {
          نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                   داخلا_2->d_name, لڙي_ڊيگهه(داخلا_2->d_name));
          إذا(نتيجو)
            اكسر;
          نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
          إذا(نتيجو)
            اكسر;
        }
      }
      closedir(dir);
    }
#وإلا
    failf(معلومات, "Directory listing not yet implemented on this platform.");
    نتيجو = CURLE_READ_ERROR;
#جيڪڏهن_پورو
  }

  إذا(!نتيجو)
    نتيجو = Curl_pgrsUpdate(معلومات);

ٻاهريون:
  Curl_multi_xfer_buf_release(معلومات, xfer_buf);
  أرجع نتيجو;
}

ثابت بنية Curl_protocol Curl_protocol_file = {
  file_تياري_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_پورو,                            /* done */
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

#جيڪڏهن_پورو
