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

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#شامل <dirent.h>
#endif

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
#define DOS_FILESYSTEM 1
#وگرنهاگر defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

ساختار FILEPROTO {
  نویسه *path; /* the path we operate on */
  نویسه *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحیح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ایستا تهی file_cleanup(ساختار FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = پوچ;
  file->is_dir = FALSE;
  اگر(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ایستا تهی file_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار FILEPROTO *file = entry;
  (تهی)کلید;
  (تهی)klen;
  file_cleanup(file);
  curlx_free(file);
}

ایستا CURLcode file_setup_connection(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار FILEPROTO *filep;
  (تهی)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, اندازه(*filep));
  اگر(filep)
    filep->fd = -1;
  اگر(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

ایستا CURLcode file_done(ساختار Curl_easy *data,
                          CURLcode status, bool premature)
{
  ساختار FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (تهی)status;
  (تهی)premature;

  اگر(file)
    file_cleanup(file);

  بازگشت CURLE_OK;
}

ایستا صحیح file_stat(ثابت نویسه *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  صحیح result = curlx_stat(path, statbuf);

  اگر(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    اگر((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      نویسه *trimmed = curlx_strdup(path);

      اگر(trimmed) {
        انجامبده {
          trimmed[--pathlen] = '\0';
        } وقتی((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  بازگشت result;
#وگرنه
  بازگشت curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ایستا CURLcode file_connect(ساختار Curl_easy *data, bool *done)
{
  نویسه *real_path;
  ساختار FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  صحیح fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  نویسه *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  اگر(!file)
    بازگشت CURLE_FAILED_INIT;

  اگر(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  اگر(result)
    بازگشت result;

#ifdef DOS_FILESYSTEM
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
  actual_path = real_path;
  اگر((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  برای(i = 0; i < real_path_len; ++i)
    اگر(actual_path[i] == '/')
      actual_path[i] = '\\';
    وگرنه اگر(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      بازگشت CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_دودویی);
  file->path = actual_path;
#وگرنه
  اگر(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    بازگشت CURLE_URL_MALFORMAT;
  }

#ifdef AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->path = real_path;

  اگر(real_path[0] == '/') {
    بیرونی صحیح __unix_path_semantics;
    اگر(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    وگرنه اگر(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#وگرنه
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  اگر(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  اگر(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    بازگشت CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  بازگشت CURLE_OK;
}

ایستا CURLcode file_disconnect(ساختار Curl_easy *data,
                                ساختار connectdata *conn,
                                bool dead_connection)
{
  (تهی)dead_connection;
  (تهی)conn;
  بازگشت file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#وگرنه
#define DIRSEP '/'
#endif

ایستا CURLcode file_upload(ساختار Curl_easy *data,
                            ساختار FILEPROTO *file)
{
  ثابت نویسه *dir = strchr(file->path, DIRSEP);
  صحیح fd;
  صحیح mode;
  CURLcode result = CURLE_OK;
  نویسه *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ثابت نویسه *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  اگر(!dir)
    بازگشت CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  اگر(!dir[1])
    بازگشت CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_دودویی;
  اگر(data->state.resume_from)
    mode |= O_APPEND;
  وگرنه
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#وگرنه
                  (mode_t)data->set.new_file_perms
#endif
                  );
  اگر(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    بازگشت CURLE_WRITE_ERROR;
  }

  اگر(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  اگر(data->state.resume_from < 0) {
    اگر(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      بازگشت CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  اگر(result)
    برو out;

  وقتی(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    اگر(result)
      بشکن;

    اگر(!readcount)
      بشکن;

    nread = readcount;

    /* skip bytes before resume point */
    اگر(data->state.resume_from) {
      اگر((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      وگرنه {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    وگرنه
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    اگر(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      بشکن;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  اگر(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  بازگشت result;
}

#اگر defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ایستا CURLcode win32_file_list(ساختار Curl_easy *data, ثابت نویسه *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  نویسه *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  اگر(!pattern)
    بازگشت CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  اگر(handle == INVALID_HANDLE_مقدار)
    بازگشت CURLE_READ_ERROR;

  انجامبده {
    اگر(entry.cFileName[0] != TEXT('.')) {
      نویسه *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      اگر(!name) {
        result = CURLE_OUT_OF_MEMORY;
        بشکن;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      اگر(result)
        بشکن;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      اگر(result)
        بشکن;
    }
  } وقتی(FindNextFile(handle, &entry));

  error = GetLastError();
  اگر(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  بازگشت result;
}
#endif

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
ایستا CURLcode file_do(ساختار Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ساختار FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  صحیح fd;
  نویسه *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  اگر(!file)
    بازگشت CURLE_FAILED_INIT;

  اگر(data->state.upload)
    بازگشت file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  اگر((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    اگر(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  اگر(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    بازگشت CURLE_OK;

  اگر(fstated) {
    time_t filetime;
    ساختار tm buffer;
    ثابت ساختار tm *tm = &buffer;
    نویسه header[80];
    صحیح headerlen;
    ایستا ثابت نویسه accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    اگر(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, اندازه(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      اگر(result)
        بازگشت result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      اگر(result != CURLE_OK)
        بازگشت result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    اگر(result)
      بازگشت result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, اندازه(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    اگر(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    اگر(result)
      بازگشت result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    اگر(data->req.no_body)
      بازگشت CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  اگر(result)
    بازگشت result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  اگر(data->state.resume_from < 0) {
    اگر(!fstated) {
      failf(data, "cannot get the size of file.");
      بازگشت CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  اگر(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    اگر(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    وگرنه {
      failf(data, "failed to resume file:// transfer");
      بازگشت CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  اگر(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  اگر(!fstated || (expected_size <= 0))
    size_known = FALSE;
  وگرنه
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  اگر(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  اگر(data->state.resume_from) {
    اگر(!file->is_dir) {
      اگر(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        بازگشت CURLE_BAD_DOWNLOAD_RESUME;
    }
    وگرنه {
      بازگشت CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  اگر(result)
    برو out;

  اگر(!file->is_dir) {
    وقتی(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      اگر(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      وگرنه
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      اگر(nread > 0)
        xfer_buf[nread] = 0;

      اگر(nread <= 0 || (size_known && (expected_size == 0)))
        بشکن;

      اگر(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      اگر(result)
        برو out;

      result = Curl_pgrsCheck(data);
      اگر(result)
        برو out;
    }
  }
  وگرنه {
#اگر defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    اگر(result)
      برو out;
#وگرنهاگر defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    ساختار dirent *entry;

    اگر(!dir) {
      result = CURLE_READ_ERROR;
      برو out;
    }
    وگرنه {
      وقتی((entry = readdir(dir))) {
        اگر(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          اگر(result)
            بشکن;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          اگر(result)
            بشکن;
        }
      }
      closedir(dir);
    }
#وگرنه
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  اگر(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  بازگشت result;
}

ثابت ساختار Curl_protocol Curl_protocol_file = {
  file_setup_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_done,                            /* done */
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

#endif
