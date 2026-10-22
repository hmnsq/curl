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

#که_نه_تعریف_شوی CURL_DISABLE_FILE

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#د_که_پای

#که_تعریف_شوی HAVE_DIRENT_H
#ضمن <dirent.h>
#د_که_پای

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
#تعریف_کړه DOS_FILESYSTEM 1
#وإلاإذا defined(__amigaos4__)
#تعریف_کړه AMIGA_FILESYSTEM 1
#د_که_پای

/* meta key for storing protocol meta at easy handle */
#تعریف_کړه CURL_META_FILE_EASY   "meta:proto:file:easy"

بنية FILEPROTO {
  حرف *لار; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحيح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساكن فراغ file_cleanup(بنية FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->لار = عدم;
  file->is_dir = FALSE;
  إذا(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساكن فراغ file_easy_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية FILEPROTO *file = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساكن CURLcode file_تیاری_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية FILEPROTO *filep;
  (فراغ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  إذا(filep)
    filep->fd = -1;
  إذا(!filep ||
     Curl_meta_سېټ(معلومات, CURL_META_FILE_EASY, filep, file_easy_dtor))
    أرجع CURLE_بهر_د_حافظه;

  أرجع CURLE_OK;
}

ساكن CURLcode file_بشپړ(بنية Curl_easy *معلومات,
                          CURLcode حالت, bool premature)
{
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
  (فراغ)حالت;
  (فراغ)premature;

  إذا(file)
    file_cleanup(file);

  أرجع CURLE_OK;
}

ساكن صحيح file_stat(ثابت حرف *لار, curlx_struct_stat *statbuf)
{
#که_تعریف_شوی _WIN32
  صحيح پایله = curlx_stat(لار, statbuf);

  إذا(پایله) {
    size_t pathlen = مزي_اوږدوالی(لار);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    إذا((pathlen > 3) &&
       ((لار[pathlen - 1] == '\\') || (لار[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(لار);

      إذا(trimmed) {
        افعل {
          trimmed[--pathlen] = '\0';
        } طالما((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        پایله = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  أرجع پایله;
#وإلا
  أرجع curlx_stat(لار, statbuf);
#د_که_پای
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساكن CURLcode file_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
  حرف *real_لار;
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
#که_تعریف_شوی _WIN32
  curlx_struct_stat statbuf;
#د_که_پای
  صحيح fd;
#که_تعریف_شوی DOS_FILESYSTEM
  size_t i;
  حرف *actual_لار;
#د_که_پای
  size_t real_لار_len;
  CURLcode پایله;

  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(file->لار) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  پایله = Curl_urldecode(معلومات->state.پورته.لار, 0, &real_لار,
                          &real_لار_len, REJECT_ZERO);
  إذا(پایله)
    أرجع پایله;

#که_تعریف_شوی DOS_FILESYSTEM
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
  actual_لار = real_لار;
  إذا((actual_لار[0] == '/') &&
      actual_لار[1] &&
     (actual_لار[2] == ':' || actual_لار[2] == '|')) {
    actual_لار[2] = ':';
    actual_لار++;
    real_لار_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  لكل(i = 0; i < real_لار_len; ++i)
    إذا(actual_لار[i] == '/')
      actual_لار[i] = '\\';
    وإلا إذا(!actual_لار[i]) { /* binary zero */
      curlx_safefree(real_لار);
      أرجع CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_لار, O_RDONLY | CURL_O_دوګونی);
  file->لار = actual_لار;
#وإلا
  إذا(memchr(real_لار, 0, real_لار_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_لار);
    أرجع CURLE_URL_MALFORMAT;
  }

#که_تعریف_شوی AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->لار = real_لار;

  إذا(real_لار[0] == '/') {
    خارجي صحيح __unix_path_semantics;
    إذا(strchr(real_لار + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_لار + 1, O_RDONLY);
      file->لار++;
    }
    وإلا إذا(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_لار, O_RDONLY);
    }
  }
#وإلا
  fd = curlx_open(real_لار, O_RDONLY);
  file->لار = real_لار;
#د_که_پای
#د_که_پای
  curlx_free(file->freepath);
  file->freepath = real_لار; /* free this when done */

  file->fd = fd;
#که_تعریف_شوی _WIN32
  إذا(!معلومات->state.upload && (fd == -1) &&
     !file_stat(file->لار, &statbuf) && S_ISDIR(statbuf.st_ډېرپېښ))
    file->is_dir = TRUE;
#د_که_پای

  إذا(!معلومات->state.upload && (fd == -1) && !file->is_dir) {
    failf(معلومات, "Could not open file %s", معلومات->state.پورته.لار);
    file_بشپړ(معلومات, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    أرجع CURLE_FILE_COULDNT_READ_FILE;
  }
  *بشپړ = TRUE;

  أرجع CURLE_OK;
}

ساكن CURLcode file_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  (فراغ)dead_connection;
  (فراغ)conn;
  أرجع file_بشپړ(معلومات, CURLE_OK, FALSE);
}

#که_تعریف_شوی DOS_FILESYSTEM
#تعریف_کړه DIRSEP '\\'
#وإلا
#تعریف_کړه DIRSEP '/'
#د_که_پای

ساكن CURLcode file_upload(بنية Curl_easy *معلومات,
                            بنية FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->لار, DIRSEP);
  صحيح fd;
  صحيح ډېرپېښ;
  CURLcode پایله = CURLE_OK;
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

  ډېرپېښ = O_WRONLY | O_CREAT | CURL_O_دوګونی;
  إذا(معلومات->state.resume_from)
    ډېرپېښ |= O_APPEND;
  وإلا
    ډېرپېښ |= O_TRUNC;

  fd = curlx_open(file->لار, ډېرپېښ,
#که_تعریف_شوی _WIN32
                  معلومات->سېټ.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                  (mode_t)معلومات->سېټ.new_file_perms
#د_که_پای
                  );
  إذا(fd < 0) {
    failf(معلومات, "cannot open %s for writing", file->لار);
    أرجع CURLE_WRITE_ERROR;
  }

  إذا(معلومات->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  إذا(معلومات->state.resume_from < 0) {
    إذا(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(معلومات, "cannot get the size of %s", file->لار);
      أرجع CURLE_WRITE_ERROR;
    }
    معلومات->state.resume_from = (curl_off_t)file_stat.st_اندازه;
  }

  پایله = Curl_multi_xfer_ulbuf_borrow(معلومات, &xfer_ulbuf, &xfer_ulblen);
  إذا(پایله)
    اذهب بهر;

  طالما(!پایله && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    پایله = Curl_client_read(معلومات, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    إذا(پایله)
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
      پایله = CURLE_SEND_ERROR;
      اكسر;
    }
    Curl_pgrs_upload_inc(معلومات, nwritten);

    پایله = Curl_pgrsCheck(معلومات);
  }
  إذا(!پایله)
    پایله = Curl_pgrsUpdate(معلومات);

بهر:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(معلومات, xfer_ulbuf);

  أرجع پایله;
}

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساكن CURLcode win32_file_list(بنية Curl_easy *معلومات, ثابت حرف *لار)
{
  WIN32_پیدا_کړه_معلومات ثبت;
  HANDLE handle;
  حرف *نمونه;
  size_t pathlen = مزي_اوږدوالی(لار);
  CURLcode پایله = CURLE_OK;
  DWORD error;

  نمونه = curl_maprintf("%s%s*", لار,
                          pathlen &&
                          (لار[pathlen - 1] == '\\' ||
                           لار[pathlen - 1] == '/') ? "" : "\\");
  إذا(!نمونه)
    أرجع CURLE_بهر_د_حافظه;

  handle = curlx_پیدا_کړه_لومړی_File(نمونه, &ثبت);
  curlx_free(نمونه);
  إذا(handle == INVALID_HANDLE_ارزښت)
    أرجع CURLE_READ_ERROR;

  افعل {
    إذا(ثبت.cFileName[0] != متن_2('.')) {
      حرف *name = curlx_convert_tchar_منزل_UTF8(ثبت.cFileName);

      إذا(!name) {
        پایله = CURLE_بهر_د_حافظه;
        اكسر;
      }

      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                 name, مزي_اوږدوالی(name));
      curlx_free(name);
      إذا(پایله)
        اكسر;

      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
      إذا(پایله)
        اكسر;
    }
  } طالما(پیدا_کړه_بل_File(handle, &ثبت));

  error = Get_وروستی_Error();
  إذا(!پایله && error != ERROR_NO_نور_FILES)
    پایله = CURLE_READ_ERROR;

  پیدا_کړه_Close(handle);
  أرجع پایله;
}
#د_که_پای

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ساكن CURLcode file_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  بنية FILEPROTO *file = Curl_meta_get(معلومات, CURL_META_FILE_EASY);
  CURLcode پایله = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_اندازه = -1;
  bool اندازه_معلوم;
  bool fstated = FALSE;
  صحيح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *بشپړ = TRUE; /* unconditionally */
  إذا(!file)
    أرجع CURLE_FAILED_INIT;

  إذا(معلومات->state.upload)
    أرجع file_upload(معلومات, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  إذا((file->is_dir ? file_stat(file->لار, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_ډېرپېښ);
    إذا(!file->is_dir)
      expected_اندازه = statbuf.st_اندازه;
    /* and store the modification time */
    معلومات->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  إذا(fstated && !معلومات->state.range && معلومات->سېټ.timecondition &&
     !Curl_meets_timecondition(معلومات, معلومات->info.filetime))
    أرجع CURLE_OK;

  إذا(fstated) {
    time_t filetime;
    بنية tm بفر;
    ثابت بنية tm *tm = &بفر;
    حرف سرلیک[80];
    صحيح headerlen;
    ساكن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    إذا(expected_اندازه >= 0) {
      headerlen =
        curl_msnprintf(سرلیک, حجم(سرلیک),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_اندازه);
      پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, سرلیک, headerlen);
      إذا(پایله)
        أرجع پایله;

      پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      إذا(پایله != CURLE_OK)
        أرجع پایله;
    }

    filetime = (time_t)statbuf.st_mtime;
    پایله = curlx_gmtime(filetime, &بفر);
    إذا(پایله)
      أرجع پایله;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(سرلیک, حجم(سرلیک),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, سرلیک, headerlen);
    إذا(!پایله)
      /* end of headers */
      پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, "\r\n", 2);
    إذا(پایله)
      أرجع پایله;
    /* set the file size to make it available post transfer */
    Curl_pgrs_سېټ_Download_اندازه(معلومات, expected_اندازه);
    إذا(معلومات->req.no_body)
      أرجع CURLE_OK;
  }

  /* Check whether file range has been specified */
  پایله = Curl_range(معلومات);
  إذا(پایله)
    أرجع پایله;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  إذا(معلومات->state.resume_from < 0) {
    إذا(!fstated) {
      failf(معلومات, "cannot get the size of file.");
      أرجع CURLE_READ_ERROR;
    }
    معلومات->state.resume_from += (curl_off_t)statbuf.st_اندازه;
  }

  إذا(معلومات->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    إذا(معلومات->state.resume_from <= expected_اندازه)
      expected_اندازه -= معلومات->state.resume_from;
    وإلا {
      failf(معلومات, "failed to resume file:// transfer");
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  إذا(معلومات->req.maxdownload > 0)
    expected_اندازه = معلومات->req.maxdownload;

  إذا(!fstated || (expected_اندازه <= 0))
    اندازه_معلوم = FALSE;
  وإلا
    اندازه_معلوم = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  إذا(اندازه_معلوم)
    Curl_pgrs_سېټ_Download_اندازه(معلومات, expected_اندازه);

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

  پایله = Curl_multi_xfer_buf_borrow(معلومات, &xfer_buf, &xfer_blen);
  إذا(پایله)
    اذهب بهر;

  إذا(!file->is_dir) {
    طالما(!پایله) {
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

      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, xfer_buf, nread);
      إذا(پایله)
        اذهب بهر;

      پایله = Curl_pgrsCheck(معلومات);
      إذا(پایله)
        اذهب بهر;
    }
  }
  وإلا {
#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    پایله = win32_file_list(معلومات, file->لار);
    إذا(پایله)
      اذهب بهر;
#وإلاإذا defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->لار);
    بنية dirent *ثبت;

    إذا(!dir) {
      پایله = CURLE_READ_ERROR;
      اذهب بهر;
    }
    وإلا {
      طالما((ثبت = readdir(dir))) {
        إذا(ثبت->d_name[0] != '.') {
          پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                   ثبت->d_name, مزي_اوږدوالی(ثبت->d_name));
          إذا(پایله)
            اكسر;
          پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
          إذا(پایله)
            اكسر;
        }
      }
      closedir(dir);
    }
#وإلا
    failf(معلومات, "Directory listing not yet implemented on this platform.");
    پایله = CURLE_READ_ERROR;
#د_که_پای
  }

  إذا(!پایله)
    پایله = Curl_pgrsUpdate(معلومات);

بهر:
  Curl_multi_xfer_buf_release(معلومات, xfer_buf);
  أرجع پایله;
}

ثابت بنية Curl_protocol Curl_protocol_file = {
  file_تیاری_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_بشپړ,                            /* done */
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

#د_که_پای
