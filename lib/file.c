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

#اگر_تعریف_نشده CURL_DISABLE_FILE

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_DIRENT_H
#ضمن <dirent.h>
#پایان_اگر

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
#تعریف_کن DOS_FILESYSTEM 1
#وإلاإذا defined(__amigaos4__)
#تعریف_کن AMIGA_FILESYSTEM 1
#پایان_اگر

/* meta key for storing protocol meta at easy handle */
#تعریف_کن CURL_META_FILE_EASY   "meta:proto:file:easy"

بنية FILEPROTO {
  حرف *مسیر; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحيح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساكن فراغ file_cleanup(بنية FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->مسیر = عدم;
  file->is_dir = FALSE;
  إذا(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساكن فراغ file_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية FILEPROTO *file = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساكن CURLcode file_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية FILEPROTO *filep;
  (فراغ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  إذا(filep)
    filep->fd = -1;
  إذا(!filep ||
     Curl_meta_مجموعه(داده, CURL_META_FILE_EASY, filep, file_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

ساكن CURLcode file_انجامشده(بنية Curl_easy *داده,
                          CURLcode وضعیت, bool premature)
{
  بنية FILEPROTO *file = Curl_meta_get(داده, CURL_META_FILE_EASY);
  (فراغ)وضعیت;
  (فراغ)premature;

  إذا(file)
    file_cleanup(file);

  أرجع CURLE_OK;
}

ساكن صحيح file_stat(ثابت حرف *مسیر, curlx_struct_stat *statbuf)
{
#اگر_تعریف_شده _WIN32
  صحيح نتیجه = curlx_stat(مسیر, statbuf);

  إذا(نتیجه) {
    size_t pathlen = طول_رشته(مسیر);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    إذا((pathlen > 3) &&
       ((مسیر[pathlen - 1] == '\\') || (مسیر[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(مسیر);

      إذا(trimmed) {
        افعل {
          trimmed[--pathlen] = '\0';
        } طالما((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        نتیجه = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  أرجع نتیجه;
#وإلا
  أرجع curlx_stat(مسیر, statbuf);
#پایان_اگر
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساكن CURLcode file_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  حرف *real_مسیر;
  بنية FILEPROTO *file = Curl_meta_get(داده, CURL_META_FILE_EASY);
#اگر_تعریف_شده _WIN32
  curlx_struct_stat statbuf;
#پایان_اگر
  صحيح fd;
#اگر_تعریف_شده DOS_FILESYSTEM
  size_t i;
  حرف *actual_مسیر;
#پایان_اگر
  size_t real_مسیر_len;
  CURLcode نتیجه;

  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(file->مسیر) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  نتیجه = Curl_urldecode(داده->state.به_بالا.مسیر, 0, &real_مسیر,
                          &real_مسیر_len, REJECT_ZERO);
  إذا(نتیجه)
    أرجع نتیجه;

#اگر_تعریف_شده DOS_FILESYSTEM
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
  actual_مسیر = real_مسیر;
  إذا((actual_مسیر[0] == '/') &&
      actual_مسیر[1] &&
     (actual_مسیر[2] == ':' || actual_مسیر[2] == '|')) {
    actual_مسیر[2] = ':';
    actual_مسیر++;
    real_مسیر_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  لكل(i = 0; i < real_مسیر_len; ++i)
    إذا(actual_مسیر[i] == '/')
      actual_مسیر[i] = '\\';
    وإلا إذا(!actual_مسیر[i]) { /* binary zero */
      curlx_safefree(real_مسیر);
      أرجع CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_مسیر, O_RDONLY | CURL_O_دودویی);
  file->مسیر = actual_مسیر;
#وإلا
  إذا(memchr(real_مسیر, 0, real_مسیر_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_مسیر);
    أرجع CURLE_URL_MALFORMAT;
  }

#اگر_تعریف_شده AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->مسیر = real_مسیر;

  إذا(real_مسیر[0] == '/') {
    خارجي صحيح __unix_path_semantics;
    إذا(strchr(real_مسیر + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_مسیر + 1, O_RDONLY);
      file->مسیر++;
    }
    وإلا إذا(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_مسیر, O_RDONLY);
    }
  }
#وإلا
  fd = curlx_open(real_مسیر, O_RDONLY);
  file->مسیر = real_مسیر;
#پایان_اگر
#پایان_اگر
  curlx_free(file->freepath);
  file->freepath = real_مسیر; /* free this when done */

  file->fd = fd;
#اگر_تعریف_شده _WIN32
  إذا(!داده->state.upload && (fd == -1) &&
     !file_stat(file->مسیر, &statbuf) && S_ISDIR(statbuf.st_نما))
    file->is_dir = TRUE;
#پایان_اگر

  إذا(!داده->state.upload && (fd == -1) && !file->is_dir) {
    failf(داده, "Could not open file %s", داده->state.به_بالا.مسیر);
    file_انجامشده(داده, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    أرجع CURLE_FILE_COULDNT_READ_FILE;
  }
  *انجامشده = TRUE;

  أرجع CURLE_OK;
}

ساكن CURLcode file_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  (فراغ)dead_connection;
  (فراغ)conn;
  أرجع file_انجامشده(داده, CURLE_OK, FALSE);
}

#اگر_تعریف_شده DOS_FILESYSTEM
#تعریف_کن DIRSEP '\\'
#وإلا
#تعریف_کن DIRSEP '/'
#پایان_اگر

ساكن CURLcode file_upload(بنية Curl_easy *داده,
                            بنية FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->مسیر, DIRSEP);
  صحيح fd;
  صحيح نما;
  CURLcode نتیجه = CURLE_OK;
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

  نما = O_WRONLY | O_CREAT | CURL_O_دودویی;
  إذا(داده->state.resume_from)
    نما |= O_APPEND;
  وإلا
    نما |= O_TRUNC;

  fd = curlx_open(file->مسیر, نما,
#اگر_تعریف_شده _WIN32
                  داده->مجموعه.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                  (mode_t)داده->مجموعه.new_file_perms
#پایان_اگر
                  );
  إذا(fd < 0) {
    failf(داده, "cannot open %s for writing", file->مسیر);
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(داده->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  إذا(داده->state.resume_from < 0) {
    إذا(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(داده, "cannot get the size of %s", file->مسیر);
      أرجع CURLE_WRITE_ERROR;
    }
    داده->state.resume_from = (curl_off_t)file_stat.st_اندازه;
  }

  نتیجه = Curl_multi_xfer_ulbuf_borrow(داده, &xfer_ulbuf, &xfer_ulblen);
  إذا(نتیجه)
    اذهب بیرون;

  طالما(!نتیجه && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    نتیجه = Curl_client_read(داده, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    إذا(نتیجه)
      اكسر;

    إذا(!readcount)
      اكسر;

    nread = readcount;

    /* skip bytes before resume point */
    إذا(داده->state.resume_from) {
      إذا((curl_off_t)nread <= داده->state.resume_from) {
        داده->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      وإلا {
        sendbuf = xfer_ulbuf + داده->state.resume_from;
        nread -= (size_t)داده->state.resume_from;
        داده->state.resume_from = 0;
      }
    }
    وإلا
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    إذا(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      نتیجه = CURLE_SEND_ERROR;
      اكسر;
    }
    Curl_pgrs_upload_inc(داده, nwritten);

    نتیجه = Curl_pgrsCheck(داده);
  }
  إذا(!نتیجه)
    نتیجه = Curl_pgrsUpdate(داده);

بیرون:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(داده, xfer_ulbuf);

  أرجع نتیجه;
}

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساكن CURLcode win32_file_list(بنية Curl_easy *داده, ثابت حرف *مسیر)
{
  WIN32_پیدا_کن_داده مدخل;
  HANDLE handle;
  حرف *الگو;
  size_t pathlen = طول_رشته(مسیر);
  CURLcode نتیجه = CURLE_OK;
  DWORD error;

  الگو = curl_maprintf("%s%s*", مسیر,
                          pathlen &&
                          (مسیر[pathlen - 1] == '\\' ||
                           مسیر[pathlen - 1] == '/') ? "" : "\\");
  إذا(!الگو)
    أرجع CURLE_بیرون_از_حافظه;

  handle = curlx_پیدا_کن_اولی_File(الگو, &مدخل);
  curlx_free(الگو);
  إذا(handle == INVALID_HANDLE_مقدار)
    أرجع CURLE_READ_ERROR;

  افعل {
    إذا(مدخل.cFileName[0] != متن_2('.')) {
      حرف *name = curlx_convert_tchar_مقصد_UTF8(مدخل.cFileName);

      إذا(!name) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اكسر;
      }

      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                 name, طول_رشته(name));
      curlx_free(name);
      إذا(نتیجه)
        اكسر;

      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتیجه)
        اكسر;
    }
  } طالما(پیدا_کن_بعدی_File(handle, &مدخل));

  error = Get_آخرین_Error();
  إذا(!نتیجه && error != ERROR_NO_بیشتر_FILES)
    نتیجه = CURLE_READ_ERROR;

  پیدا_کن_Close(handle);
  أرجع نتیجه;
}
#پایان_اگر

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ساكن CURLcode file_do(بنية Curl_easy *داده, bool *انجامشده)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  بنية FILEPROTO *file = Curl_meta_get(داده, CURL_META_FILE_EASY);
  CURLcode نتیجه = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_اندازه = -1;
  bool اندازه_معلوم;
  bool fstated = FALSE;
  صحيح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *انجامشده = TRUE; /* unconditionally */
  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(داده->state.upload)
    أرجع file_upload(داده, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  إذا((file->is_dir ? file_stat(file->مسیر, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_نما);
    إذا(!file->is_dir)
      expected_اندازه = statbuf.st_اندازه;
    /* and store the modification time */
    داده->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  إذا(fstated && !داده->state.range && داده->مجموعه.timecondition &&
     !Curl_meets_timecondition(داده, داده->info.filetime))
    أرجع CURLE_OK;

  إذا(fstated) {
    time_t filetime;
    بنية tm میانگیر;
    ثابت بنية tm *tm = &میانگیر;
    حرف سرآیند[80];
    صحيح headerlen;
    ساكن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    إذا(expected_اندازه >= 0) {
      headerlen =
        curl_msnprintf(سرآیند, حجم(سرآیند),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_اندازه);
      نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, سرآیند, headerlen);
      إذا(نتیجه)
        أرجع نتیجه;

      نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      إذا(نتیجه != CURLE_OK)
        أرجع نتیجه;
    }

    filetime = (time_t)statbuf.st_mtime;
    نتیجه = curlx_gmtime(filetime, &میانگیر);
    إذا(نتیجه)
      أرجع نتیجه;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(سرآیند, حجم(سرآیند),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, سرآیند, headerlen);
    إذا(!نتیجه)
      /* end of headers */
      نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, "\r\n", 2);
    إذا(نتیجه)
      أرجع نتیجه;
    /* set the file size to make it available post transfer */
    Curl_pgrs_مجموعه_Download_اندازه(داده, expected_اندازه);
    إذا(داده->req.no_body)
      أرجع CURLE_OK;
  }

  /* Check whether file range has been specified */
  نتیجه = Curl_range(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  إذا(داده->state.resume_from < 0) {
    إذا(!fstated) {
      failf(داده, "cannot get the size of file.");
      أرجع CURLE_READ_ERROR;
    }
    داده->state.resume_from += (curl_off_t)statbuf.st_اندازه;
  }

  إذا(داده->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    إذا(داده->state.resume_from <= expected_اندازه)
      expected_اندازه -= داده->state.resume_from;
    وإلا {
      failf(داده, "failed to resume file:// transfer");
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  إذا(داده->req.maxdownload > 0)
    expected_اندازه = داده->req.maxdownload;

  إذا(!fstated || (expected_اندازه <= 0))
    اندازه_معلوم = FALSE;
  وإلا
    اندازه_معلوم = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  إذا(اندازه_معلوم)
    Curl_pgrs_مجموعه_Download_اندازه(داده, expected_اندازه);

  إذا(داده->state.resume_from) {
    إذا(!file->is_dir) {
      إذا(داده->state.resume_from !=
         curl_lseek(fd, داده->state.resume_from, SEEK_SET))
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    وإلا {
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  نتیجه = Curl_multi_xfer_buf_borrow(داده, &xfer_buf, &xfer_blen);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(!file->is_dir) {
    طالما(!نتیجه) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      إذا(اندازه_معلوم) {
        bytestoread = (expected_اندازه < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_اندازه) : (xfer_blen - 1);
      }
      وإلا
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      إذا(nread > 0)
        xfer_buf[nread] = 0;

      إذا(nread <= 0 || (اندازه_معلوم && (expected_اندازه == 0)))
        اكسر;

      إذا(اندازه_معلوم)
        expected_اندازه -= nread;

      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, xfer_buf, nread);
      إذا(نتیجه)
        اذهب بیرون;

      نتیجه = Curl_pgrsCheck(داده);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }
  وإلا {
#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    نتیجه = win32_file_list(داده, file->مسیر);
    إذا(نتیجه)
      اذهب بیرون;
#وإلاإذا defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->مسیر);
    بنية dirent *مدخل;

    إذا(!dir) {
      نتیجه = CURLE_READ_ERROR;
      اذهب بیرون;
    }
    وإلا {
      طالما((مدخل = readdir(dir))) {
        إذا(مدخل->d_name[0] != '.') {
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                   مدخل->d_name, طول_رشته(مدخل->d_name));
          إذا(نتیجه)
            اكسر;
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
          إذا(نتیجه)
            اكسر;
        }
      }
      closedir(dir);
    }
#وإلا
    failf(داده, "Directory listing not yet implemented on this platform.");
    نتیجه = CURLE_READ_ERROR;
#پایان_اگر
  }

  إذا(!نتیجه)
    نتیجه = Curl_pgrsUpdate(داده);

بیرون:
  Curl_multi_xfer_buf_release(داده, xfer_buf);
  أرجع نتیجه;
}

ثابت بنية Curl_protocol Curl_protocol_file = {
  file_راهاندازی_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_انجامشده,                            /* done */
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

#پایان_اگر
