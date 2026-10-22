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

#جے_بنا_تعریف CURL_DISABLE_FILE

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#جے_تعریف_شدہ HAVE_DIRENT_H
#ضمن <dirent.h>
#جے_انت

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
#تعریف_کرو DOS_FILESYSTEM 1
#وإلاإذا defined(__amigaos4__)
#تعریف_کرو AMIGA_FILESYSTEM 1
#جے_انت

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_FILE_EASY   "meta:proto:file:easy"

بنية FILEPROTO {
  حرف *رستہ; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحيح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساكن فراغ file_cleanup(بنية FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->رستہ = عدم;
  file->is_dir = FALSE;
  إذا(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساكن فراغ file_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية FILEPROTO *file = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساكن CURLcode file_تیاری_connection(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn)
{
  بنية FILEPROTO *filep;
  (فراغ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  إذا(filep)
    filep->fd = -1;
  إذا(!filep ||
     Curl_meta_سیٹ(انکڑے, CURL_META_FILE_EASY, filep, file_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

ساكن CURLcode file_پورا(بنية Curl_easy *انکڑے,
                          CURLcode حالت, bool premature)
{
  بنية FILEPROTO *file = Curl_meta_get(انکڑے, CURL_META_FILE_EASY);
  (فراغ)حالت;
  (فراغ)premature;

  إذا(file)
    file_cleanup(file);

  أرجع CURLE_OK;
}

ساكن صحيح file_stat(ثابت حرف *رستہ, curlx_struct_stat *statbuf)
{
#جے_تعریف_شدہ _WIN32
  صحيح نتیجہ = curlx_stat(رستہ, statbuf);

  إذا(نتیجہ) {
    size_t pathlen = لڑی_لمبائی(رستہ);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    إذا((pathlen > 3) &&
       ((رستہ[pathlen - 1] == '\\') || (رستہ[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(رستہ);

      إذا(trimmed) {
        افعل {
          trimmed[--pathlen] = '\0';
        } طالما((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        نتیجہ = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  أرجع نتیجہ;
#وإلا
  أرجع curlx_stat(رستہ, statbuf);
#جے_انت
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساكن CURLcode file_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  حرف *real_رستہ;
  بنية FILEPROTO *file = Curl_meta_get(انکڑے, CURL_META_FILE_EASY);
#جے_تعریف_شدہ _WIN32
  curlx_struct_stat statbuf;
#جے_انت
  صحيح fd;
#جے_تعریف_شدہ DOS_FILESYSTEM
  size_t i;
  حرف *actual_رستہ;
#جے_انت
  size_t real_رستہ_len;
  CURLcode نتیجہ;

  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(file->رستہ) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  نتیجہ = Curl_urldecode(انکڑے->state.اتے.رستہ, 0, &real_رستہ,
                          &real_رستہ_len, REJECT_ZERO);
  إذا(نتیجہ)
    أرجع نتیجہ;

#جے_تعریف_شدہ DOS_FILESYSTEM
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
  actual_رستہ = real_رستہ;
  إذا((actual_رستہ[0] == '/') &&
      actual_رستہ[1] &&
     (actual_رستہ[2] == ':' || actual_رستہ[2] == '|')) {
    actual_رستہ[2] = ':';
    actual_رستہ++;
    real_رستہ_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  لكل(i = 0; i < real_رستہ_len; ++i)
    إذا(actual_رستہ[i] == '/')
      actual_رستہ[i] = '\\';
    وإلا إذا(!actual_رستہ[i]) { /* binary zero */
      curlx_safefree(real_رستہ);
      أرجع CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_رستہ, O_RDONLY | CURL_O_ثنائی);
  file->رستہ = actual_رستہ;
#وإلا
  إذا(memchr(real_رستہ, 0, real_رستہ_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_رستہ);
    أرجع CURLE_URL_MALFORMAT;
  }

#جے_تعریف_شدہ AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->رستہ = real_رستہ;

  إذا(real_رستہ[0] == '/') {
    خارجي صحيح __unix_path_semantics;
    إذا(strchr(real_رستہ + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_رستہ + 1, O_RDONLY);
      file->رستہ++;
    }
    وإلا إذا(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_رستہ, O_RDONLY);
    }
  }
#وإلا
  fd = curlx_open(real_رستہ, O_RDONLY);
  file->رستہ = real_رستہ;
#جے_انت
#جے_انت
  curlx_free(file->freepath);
  file->freepath = real_رستہ; /* free this when done */

  file->fd = fd;
#جے_تعریف_شدہ _WIN32
  إذا(!انکڑے->state.upload && (fd == -1) &&
     !file_stat(file->رستہ, &statbuf) && S_ISDIR(statbuf.st_منوال))
    file->is_dir = TRUE;
#جے_انت

  إذا(!انکڑے->state.upload && (fd == -1) && !file->is_dir) {
    failf(انکڑے, "Could not open file %s", انکڑے->state.اتے.رستہ);
    file_پورا(انکڑے, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    أرجع CURLE_FILE_COULDNT_READ_FILE;
  }
  *پورا = TRUE;

  أرجع CURLE_OK;
}

ساكن CURLcode file_disconnect(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  (فراغ)dead_connection;
  (فراغ)conn;
  أرجع file_پورا(انکڑے, CURLE_OK, FALSE);
}

#جے_تعریف_شدہ DOS_FILESYSTEM
#تعریف_کرو DIRSEP '\\'
#وإلا
#تعریف_کرو DIRSEP '/'
#جے_انت

ساكن CURLcode file_upload(بنية Curl_easy *انکڑے,
                            بنية FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->رستہ, DIRSEP);
  صحيح fd;
  صحيح منوال;
  CURLcode نتیجہ = CURLE_OK;
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

  منوال = O_WRONLY | O_CREAT | CURL_O_ثنائی;
  إذا(انکڑے->state.resume_from)
    منوال |= O_APPEND;
  وإلا
    منوال |= O_TRUNC;

  fd = curlx_open(file->رستہ, منوال,
#جے_تعریف_شدہ _WIN32
                  انکڑے->سیٹ.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                  (mode_t)انکڑے->سیٹ.new_file_perms
#جے_انت
                  );
  إذا(fd < 0) {
    failf(انکڑے, "cannot open %s for writing", file->رستہ);
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(انکڑے->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  إذا(انکڑے->state.resume_from < 0) {
    إذا(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(انکڑے, "cannot get the size of %s", file->رستہ);
      أرجع CURLE_WRITE_ERROR;
    }
    انکڑے->state.resume_from = (curl_off_t)file_stat.st_ناپ;
  }

  نتیجہ = Curl_multi_xfer_ulbuf_borrow(انکڑے, &xfer_ulbuf, &xfer_ulblen);
  إذا(نتیجہ)
    اذهب باہرلا;

  طالما(!نتیجہ && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    نتیجہ = Curl_client_read(انکڑے, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    إذا(نتیجہ)
      اكسر;

    إذا(!readcount)
      اكسر;

    nread = readcount;

    /* skip bytes before resume point */
    إذا(انکڑے->state.resume_from) {
      إذا((curl_off_t)nread <= انکڑے->state.resume_from) {
        انکڑے->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      وإلا {
        sendbuf = xfer_ulbuf + انکڑے->state.resume_from;
        nread -= (size_t)انکڑے->state.resume_from;
        انکڑے->state.resume_from = 0;
      }
    }
    وإلا
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    إذا(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      نتیجہ = CURLE_SEND_ERROR;
      اكسر;
    }
    Curl_pgrs_upload_inc(انکڑے, nwritten);

    نتیجہ = Curl_pgrsCheck(انکڑے);
  }
  إذا(!نتیجہ)
    نتیجہ = Curl_pgrsUpdate(انکڑے);

باہرلا:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(انکڑے, xfer_ulbuf);

  أرجع نتیجہ;
}

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساكن CURLcode win32_file_list(بنية Curl_easy *انکڑے, ثابت حرف *رستہ)
{
  WIN32_لبھو_انکڑے اندراج;
  HANDLE handle;
  حرف *نمونہ;
  size_t pathlen = لڑی_لمبائی(رستہ);
  CURLcode نتیجہ = CURLE_OK;
  DWORD error;

  نمونہ = curl_maprintf("%s%s*", رستہ,
                          pathlen &&
                          (رستہ[pathlen - 1] == '\\' ||
                           رستہ[pathlen - 1] == '/') ? "" : "\\");
  إذا(!نمونہ)
    أرجع CURLE_باہرلا_دا_حافظہ;

  handle = curlx_لبھو_پہلا_File(نمونہ, &اندراج);
  curlx_free(نمونہ);
  إذا(handle == INVALID_HANDLE_قدر)
    أرجع CURLE_READ_ERROR;

  افعل {
    إذا(اندراج.cFileName[0] != متن_2('.')) {
      حرف *name = curlx_convert_tchar_منزل_UTF8(اندراج.cFileName);

      إذا(!name) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اكسر;
      }

      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                 name, لڑی_لمبائی(name));
      curlx_free(name);
      إذا(نتیجہ)
        اكسر;

      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتیجہ)
        اكسر;
    }
  } طالما(لبھو_اگلا_File(handle, &اندراج));

  error = Get_آخری_Error();
  إذا(!نتیجہ && error != ERROR_NO_ہور_FILES)
    نتیجہ = CURLE_READ_ERROR;

  لبھو_Close(handle);
  أرجع نتیجہ;
}
#جے_انت

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ساكن CURLcode file_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  بنية FILEPROTO *file = Curl_meta_get(انکڑے, CURL_META_FILE_EASY);
  CURLcode نتیجہ = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_ناپ = -1;
  bool ناپ_معلوم;
  bool fstated = FALSE;
  صحيح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *پورا = TRUE; /* unconditionally */
  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(انکڑے->state.upload)
    أرجع file_upload(انکڑے, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  إذا((file->is_dir ? file_stat(file->رستہ, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_منوال);
    إذا(!file->is_dir)
      expected_ناپ = statbuf.st_ناپ;
    /* and store the modification time */
    انکڑے->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  إذا(fstated && !انکڑے->state.range && انکڑے->سیٹ.timecondition &&
     !Curl_meets_timecondition(انکڑے, انکڑے->info.filetime))
    أرجع CURLE_OK;

  إذا(fstated) {
    time_t filetime;
    بنية tm بفر;
    ثابت بنية tm *tm = &بفر;
    حرف سرخی[80];
    صحيح headerlen;
    ساكن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    إذا(expected_ناپ >= 0) {
      headerlen =
        curl_msnprintf(سرخی, حجم(سرخی),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_ناپ);
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, سرخی, headerlen);
      إذا(نتیجہ)
        أرجع نتیجہ;

      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      إذا(نتیجہ != CURLE_OK)
        أرجع نتیجہ;
    }

    filetime = (time_t)statbuf.st_mtime;
    نتیجہ = curlx_gmtime(filetime, &بفر);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(سرخی, حجم(سرخی),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, سرخی, headerlen);
    إذا(!نتیجہ)
      /* end of headers */
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, "\r\n", 2);
    إذا(نتیجہ)
      أرجع نتیجہ;
    /* set the file size to make it available post transfer */
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, expected_ناپ);
    إذا(انکڑے->req.no_body)
      أرجع CURLE_OK;
  }

  /* Check whether file range has been specified */
  نتیجہ = Curl_range(انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  إذا(انکڑے->state.resume_from < 0) {
    إذا(!fstated) {
      failf(انکڑے, "cannot get the size of file.");
      أرجع CURLE_READ_ERROR;
    }
    انکڑے->state.resume_from += (curl_off_t)statbuf.st_ناپ;
  }

  إذا(انکڑے->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    إذا(انکڑے->state.resume_from <= expected_ناپ)
      expected_ناپ -= انکڑے->state.resume_from;
    وإلا {
      failf(انکڑے, "failed to resume file:// transfer");
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  إذا(انکڑے->req.maxdownload > 0)
    expected_ناپ = انکڑے->req.maxdownload;

  إذا(!fstated || (expected_ناپ <= 0))
    ناپ_معلوم = FALSE;
  وإلا
    ناپ_معلوم = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  إذا(ناپ_معلوم)
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, expected_ناپ);

  إذا(انکڑے->state.resume_from) {
    إذا(!file->is_dir) {
      إذا(انکڑے->state.resume_from !=
         curl_lseek(fd, انکڑے->state.resume_from, SEEK_SET))
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    وإلا {
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  نتیجہ = Curl_multi_xfer_buf_borrow(انکڑے, &xfer_buf, &xfer_blen);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(!file->is_dir) {
    طالما(!نتیجہ) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      إذا(ناپ_معلوم) {
        bytestoread = (expected_ناپ < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_ناپ) : (xfer_blen - 1);
      }
      وإلا
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      إذا(nread > 0)
        xfer_buf[nread] = 0;

      إذا(nread <= 0 || (ناپ_معلوم && (expected_ناپ == 0)))
        اكسر;

      إذا(ناپ_معلوم)
        expected_ناپ -= nread;

      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, xfer_buf, nread);
      إذا(نتیجہ)
        اذهب باہرلا;

      نتیجہ = Curl_pgrsCheck(انکڑے);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }
  وإلا {
#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    نتیجہ = win32_file_list(انکڑے, file->رستہ);
    إذا(نتیجہ)
      اذهب باہرلا;
#وإلاإذا defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->رستہ);
    بنية dirent *اندراج;

    إذا(!dir) {
      نتیجہ = CURLE_READ_ERROR;
      اذهب باہرلا;
    }
    وإلا {
      طالما((اندراج = readdir(dir))) {
        إذا(اندراج->d_name[0] != '.') {
          نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                   اندراج->d_name, لڑی_لمبائی(اندراج->d_name));
          إذا(نتیجہ)
            اكسر;
          نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, "\n", 1);
          إذا(نتیجہ)
            اكسر;
        }
      }
      closedir(dir);
    }
#وإلا
    failf(انکڑے, "Directory listing not yet implemented on this platform.");
    نتیجہ = CURLE_READ_ERROR;
#جے_انت
  }

  إذا(!نتیجہ)
    نتیجہ = Curl_pgrsUpdate(انکڑے);

باہرلا:
  Curl_multi_xfer_buf_release(انکڑے, xfer_buf);
  أرجع نتیجہ;
}

ثابت بنية Curl_protocol Curl_protocol_file = {
  file_تیاری_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_پورا,                            /* done */
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

#جے_انت
