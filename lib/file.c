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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "file.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FILE

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_DIRENT_H
#شامل <dirent.h>
#اگر_ختم

#شامل "progress.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "escape.h"
#شامل "multiif.h"
#شامل "transfer.h"
#شامل "url.h"
#شامل "parsedate.h" /* for the week day and month names */
#شامل "curlx/win32-fopen.h"
#شامل "curl_range.h"

#اگر defined(_WIN32) || defined(MSDOS)
#تعریف_کرو DOS_FILESYSTEM 1
#وگرنہ_اگر defined(__amigaos4__)
#تعریف_کرو AMIGA_FILESYSTEM 1
#اگر_ختم

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_FILE_EASY   "meta:proto:file:easy"

ساخت FILEPROTO {
  حرف *راستہ; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحیح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساکن خالی file_cleanup(ساخت FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->راستہ = تہی;
  file->is_dir = FALSE;
  اگر(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساکن خالی file_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت FILEPROTO *file = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساکن CURLcode file_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت FILEPROTO *filep;
  (خالی)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  اگر(filep)
    filep->fd = -1;
  اگر(!filep ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_FILE_EASY, filep, file_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

ساکن CURLcode file_مکمل(ساخت Curl_easy *ڈیٹا,
                          CURLcode حالت_2, bool premature)
{
  ساخت FILEPROTO *file = Curl_meta_get(ڈیٹا, CURL_META_FILE_EASY);
  (خالی)حالت_2;
  (خالی)premature;

  اگر(file)
    file_cleanup(file);

  واپس CURLE_OK;
}

ساکن صحیح file_stat(ثابت حرف *راستہ, curlx_struct_stat *statbuf)
{
#اگر_تعریف_شدہ _WIN32
  صحیح نتیجہ = curlx_stat(راستہ, statbuf);

  اگر(نتیجہ) {
    size_t pathlen = لڑی_لمبائی(راستہ);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    اگر((pathlen > 3) &&
       ((راستہ[pathlen - 1] == '\\') || (راستہ[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(راستہ);

      اگر(trimmed) {
        کرو {
          trimmed[--pathlen] = '\0';
        } جب((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        نتیجہ = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  واپس نتیجہ;
#ورنہ
  واپس curlx_stat(راستہ, statbuf);
#اگر_ختم
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساکن CURLcode file_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  حرف *real_راستہ;
  ساخت FILEPROTO *file = Curl_meta_get(ڈیٹا, CURL_META_FILE_EASY);
#اگر_تعریف_شدہ _WIN32
  curlx_struct_stat statbuf;
#اگر_ختم
  صحیح fd;
#اگر_تعریف_شدہ DOS_FILESYSTEM
  size_t i;
  حرف *actual_راستہ;
#اگر_ختم
  size_t real_راستہ_len;
  CURLcode نتیجہ;

  اگر(!file)
    واپس CURLE_FAILED_INIT;

  اگر(file->راستہ) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  نتیجہ = Curl_urldecode(ڈیٹا->state.اوپر.راستہ, 0, &real_راستہ,
                          &real_راستہ_len, REJECT_ZERO);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر_تعریف_شدہ DOS_FILESYSTEM
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
  actual_راستہ = real_راستہ;
  اگر((actual_راستہ[0] == '/') &&
      actual_راستہ[1] &&
     (actual_راستہ[2] == ':' || actual_راستہ[2] == '|')) {
    actual_راستہ[2] = ':';
    actual_راستہ++;
    real_راستہ_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  برائے(i = 0; i < real_راستہ_len; ++i)
    اگر(actual_راستہ[i] == '/')
      actual_راستہ[i] = '\\';
    ورنہ اگر(!actual_راستہ[i]) { /* binary zero */
      curlx_safefree(real_راستہ);
      واپس CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_راستہ, O_RDONLY | CURL_O_ثنائی);
  file->راستہ = actual_راستہ;
#ورنہ
  اگر(memchr(real_راستہ, 0, real_راستہ_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_راستہ);
    واپس CURLE_URL_MALFORMAT;
  }

#اگر_تعریف_شدہ AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->راستہ = real_راستہ;

  اگر(real_راستہ[0] == '/') {
    خارجی صحیح __unix_path_semantics;
    اگر(strchr(real_راستہ + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_راستہ + 1, O_RDONLY);
      file->راستہ++;
    }
    ورنہ اگر(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_راستہ, O_RDONLY);
    }
  }
#ورنہ
  fd = curlx_open(real_راستہ, O_RDONLY);
  file->راستہ = real_راستہ;
#اگر_ختم
#اگر_ختم
  curlx_free(file->freepath);
  file->freepath = real_راستہ; /* free this when done */

  file->fd = fd;
#اگر_تعریف_شدہ _WIN32
  اگر(!ڈیٹا->state.upload && (fd == -1) &&
     !file_stat(file->راستہ, &statbuf) && S_ISDIR(statbuf.st_منوال))
    file->is_dir = TRUE;
#اگر_ختم

  اگر(!ڈیٹا->state.upload && (fd == -1) && !file->is_dir) {
    failf(ڈیٹا, "Could not open file %s", ڈیٹا->state.اوپر.راستہ);
    file_مکمل(ڈیٹا, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    واپس CURLE_FILE_COULDNT_READ_FILE;
  }
  *مکمل = TRUE;

  واپس CURLE_OK;
}

ساکن CURLcode file_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn,
                                bool dead_connection)
{
  (خالی)dead_connection;
  (خالی)conn;
  واپس file_مکمل(ڈیٹا, CURLE_OK, FALSE);
}

#اگر_تعریف_شدہ DOS_FILESYSTEM
#تعریف_کرو DIRSEP '\\'
#ورنہ
#تعریف_کرو DIRSEP '/'
#اگر_ختم

ساکن CURLcode file_upload(ساخت Curl_easy *ڈیٹا,
                            ساخت FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->راستہ, DIRSEP);
  صحیح fd;
  صحیح منوال;
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

  اگر(!dir)
    واپس CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  اگر(!dir[1])
    واپس CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  منوال = O_WRONLY | O_CREAT | CURL_O_ثنائی;
  اگر(ڈیٹا->state.resume_from)
    منوال |= O_APPEND;
  ورنہ
    منوال |= O_TRUNC;

  fd = curlx_open(file->راستہ, منوال,
#اگر_تعریف_شدہ _WIN32
                  ڈیٹا->سیٹ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ورنہ
                  (mode_t)ڈیٹا->سیٹ.new_file_perms
#اگر_ختم
                  );
  اگر(fd < 0) {
    failf(ڈیٹا, "cannot open %s for writing", file->راستہ);
    واپس CURLE_WRITE_ERROR;
  }

  اگر(ڈیٹا->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  اگر(ڈیٹا->state.resume_from < 0) {
    اگر(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(ڈیٹا, "cannot get the size of %s", file->راستہ);
      واپس CURLE_WRITE_ERROR;
    }
    ڈیٹا->state.resume_from = (curl_off_t)file_stat.st_سائز;
  }

  نتیجہ = Curl_multi_xfer_ulbuf_borrow(ڈیٹا, &xfer_ulbuf, &xfer_ulblen);
  اگر(نتیجہ)
    جاؤ برآمد;

  جب(!نتیجہ && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    نتیجہ = Curl_client_read(ڈیٹا, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    اگر(نتیجہ)
      توڑو;

    اگر(!readcount)
      توڑو;

    nread = readcount;

    /* skip bytes before resume point */
    اگر(ڈیٹا->state.resume_from) {
      اگر((curl_off_t)nread <= ڈیٹا->state.resume_from) {
        ڈیٹا->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      ورنہ {
        sendbuf = xfer_ulbuf + ڈیٹا->state.resume_from;
        nread -= (size_t)ڈیٹا->state.resume_from;
        ڈیٹا->state.resume_from = 0;
      }
    }
    ورنہ
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    اگر(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      نتیجہ = CURLE_SEND_ERROR;
      توڑو;
    }
    Curl_pgrs_upload_inc(ڈیٹا, nwritten);

    نتیجہ = Curl_pgrsCheck(ڈیٹا);
  }
  اگر(!نتیجہ)
    نتیجہ = Curl_pgrsUpdate(ڈیٹا);

برآمد:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(ڈیٹا, xfer_ulbuf);

  واپس نتیجہ;
}

#اگر defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساکن CURLcode win32_file_list(ساخت Curl_easy *ڈیٹا, ثابت حرف *راستہ)
{
  WIN32_ڈھونڈو_ڈیٹا اندراج_2;
  HANDLE handle;
  حرف *نمونہ;
  size_t pathlen = لڑی_لمبائی(راستہ);
  CURLcode نتیجہ = CURLE_OK;
  DWORD error;

  نمونہ = curl_maprintf("%s%s*", راستہ,
                          pathlen &&
                          (راستہ[pathlen - 1] == '\\' ||
                           راستہ[pathlen - 1] == '/') ? "" : "\\");
  اگر(!نمونہ)
    واپس CURLE_برآمد_کا_حافظہ;

  handle = curlx_ڈھونڈو_پہلا_File(نمونہ, &اندراج_2);
  curlx_free(نمونہ);
  اگر(handle == INVALID_HANDLE_قدر)
    واپس CURLE_READ_ERROR;

  کرو {
    اگر(اندراج_2.cFileName[0] != متن_2('.')) {
      حرف *name = curlx_convert_tchar_مقصد_UTF8(اندراج_2.cFileName);

      اگر(!name) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        توڑو;
      }

      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                 name, لڑی_لمبائی(name));
      curlx_free(name);
      اگر(نتیجہ)
        توڑو;

      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
      اگر(نتیجہ)
        توڑو;
    }
  } جب(ڈھونڈو_اگلا_File(handle, &اندراج_2));

  error = Get_آخری_Error();
  اگر(!نتیجہ && error != ERROR_NO_مزید_FILES)
    نتیجہ = CURLE_READ_ERROR;

  ڈھونڈو_Close(handle);
  واپس نتیجہ;
}
#اگر_ختم

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ساکن CURLcode file_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ساخت FILEPROTO *file = Curl_meta_get(ڈیٹا, CURL_META_FILE_EASY);
  CURLcode نتیجہ = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_سائز = -1;
  bool سائز_معلوم;
  bool fstated = FALSE;
  صحیح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *مکمل = TRUE; /* unconditionally */
  اگر(!file)
    واپس CURLE_FAILED_INIT;

  اگر(ڈیٹا->state.upload)
    واپس file_upload(ڈیٹا, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  اگر((file->is_dir ? file_stat(file->راستہ, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_منوال);
    اگر(!file->is_dir)
      expected_سائز = statbuf.st_سائز;
    /* and store the modification time */
    ڈیٹا->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  اگر(fstated && !ڈیٹا->state.range && ڈیٹا->سیٹ.timecondition &&
     !Curl_meets_timecondition(ڈیٹا, ڈیٹا->info.filetime))
    واپس CURLE_OK;

  اگر(fstated) {
    time_t filetime;
    ساخت tm بفر;
    ثابت ساخت tm *tm = &بفر;
    حرف سرخی[80];
    صحیح headerlen;
    ساکن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    اگر(expected_سائز >= 0) {
      headerlen =
        curl_msnprintf(سرخی, حجم(سرخی),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_سائز);
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, سرخی, headerlen);
      اگر(نتیجہ)
        واپس نتیجہ;

      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      اگر(نتیجہ != CURLE_OK)
        واپس نتیجہ;
    }

    filetime = (time_t)statbuf.st_mtime;
    نتیجہ = curlx_gmtime(filetime, &بفر);
    اگر(نتیجہ)
      واپس نتیجہ;

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
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, سرخی, headerlen);
    اگر(!نتیجہ)
      /* end of headers */
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, "\r\n", 2);
    اگر(نتیجہ)
      واپس نتیجہ;
    /* set the file size to make it available post transfer */
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, expected_سائز);
    اگر(ڈیٹا->req.no_body)
      واپس CURLE_OK;
  }

  /* Check whether file range has been specified */
  نتیجہ = Curl_range(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  اگر(ڈیٹا->state.resume_from < 0) {
    اگر(!fstated) {
      failf(ڈیٹا, "cannot get the size of file.");
      واپس CURLE_READ_ERROR;
    }
    ڈیٹا->state.resume_from += (curl_off_t)statbuf.st_سائز;
  }

  اگر(ڈیٹا->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    اگر(ڈیٹا->state.resume_from <= expected_سائز)
      expected_سائز -= ڈیٹا->state.resume_from;
    ورنہ {
      failf(ڈیٹا, "failed to resume file:// transfer");
      واپس CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  اگر(ڈیٹا->req.maxdownload > 0)
    expected_سائز = ڈیٹا->req.maxdownload;

  اگر(!fstated || (expected_سائز <= 0))
    سائز_معلوم = FALSE;
  ورنہ
    سائز_معلوم = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  اگر(سائز_معلوم)
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, expected_سائز);

  اگر(ڈیٹا->state.resume_from) {
    اگر(!file->is_dir) {
      اگر(ڈیٹا->state.resume_from !=
         curl_lseek(fd, ڈیٹا->state.resume_from, SEEK_SET))
        واپس CURLE_BAD_DOWNLOAD_RESUME;
    }
    ورنہ {
      واپس CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  نتیجہ = Curl_multi_xfer_buf_borrow(ڈیٹا, &xfer_buf, &xfer_blen);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(!file->is_dir) {
    جب(!نتیجہ) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      اگر(سائز_معلوم) {
        bytestoread = (expected_سائز < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_سائز) : (xfer_blen - 1);
      }
      ورنہ
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      اگر(nread > 0)
        xfer_buf[nread] = 0;

      اگر(nread <= 0 || (سائز_معلوم && (expected_سائز == 0)))
        توڑو;

      اگر(سائز_معلوم)
        expected_سائز -= nread;

      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, xfer_buf, nread);
      اگر(نتیجہ)
        جاؤ برآمد;

      نتیجہ = Curl_pgrsCheck(ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }
  ورنہ {
#اگر defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    نتیجہ = win32_file_list(ڈیٹا, file->راستہ);
    اگر(نتیجہ)
      جاؤ برآمد;
#وگرنہ_اگر defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->راستہ);
    ساخت dirent *اندراج_2;

    اگر(!dir) {
      نتیجہ = CURLE_READ_ERROR;
      جاؤ برآمد;
    }
    ورنہ {
      جب((اندراج_2 = readdir(dir))) {
        اگر(اندراج_2->d_name[0] != '.') {
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                   اندراج_2->d_name, لڑی_لمبائی(اندراج_2->d_name));
          اگر(نتیجہ)
            توڑو;
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
          اگر(نتیجہ)
            توڑو;
        }
      }
      closedir(dir);
    }
#ورنہ
    failf(ڈیٹا, "Directory listing not yet implemented on this platform.");
    نتیجہ = CURLE_READ_ERROR;
#اگر_ختم
  }

  اگر(!نتیجہ)
    نتیجہ = Curl_pgrsUpdate(ڈیٹا);

برآمد:
  Curl_multi_xfer_buf_release(ڈیٹا, xfer_buf);
  واپس نتیجہ;
}

ثابت ساخت Curl_protocol Curl_protocol_file = {
  file_تیاری_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_مکمل,                            /* done */
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

#اگر_ختم
