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

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#ضمن <dirent.h>
#endif

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

#𐡄𐡍 defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#وإلاإذا defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

بنية FILEPROTO {
  حرف *path; /* the path we operate on */
  حرف *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  صحيح fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ساكن فراغ file_cleanup(بنية FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = عدم;
  file->is_dir = FALSE;
  𐡄𐡍(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ساكن فراغ file_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية FILEPROTO *file = entry;
  (فراغ)key;
  (فراغ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ساكن CURLcode file_setup_connection(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  بنية FILEPROTO *filep;
  (فراغ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, حجم(*filep));
  𐡄𐡍(filep)
    filep->fd = -1;
  𐡄𐡍(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  أرجع CURLE_OK;
}

ساكن CURLcode file_done(بنية Curl_easy *data,
                          CURLcode status, bool premature)
{
  بنية FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (فراغ)status;
  (فراغ)premature;

  𐡄𐡍(file)
    file_cleanup(file);

  أرجع CURLE_OK;
}

ساكن صحيح file_stat(ثابت حرف *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  صحيح result = curlx_stat(path, statbuf);

  𐡄𐡍(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    𐡄𐡍((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      حرف *trimmed = curlx_strdup(path);

      𐡄𐡍(trimmed) {
        𐡏𐡁𐡃 {
          trimmed[--pathlen] = '\0';
        } طالما((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  أرجع result;
#وإلا
  أرجع curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ساكن CURLcode file_connect(بنية Curl_easy *data, bool *done)
{
  حرف *real_path;
  بنية FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  صحيح fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  حرف *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  𐡄𐡍(!file)
    أرجع CURLE_FAILED_INIT;

  𐡄𐡍(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    أرجع CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  𐡄𐡍(result)
    أرجع result;

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
  𐡄𐡍((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  لكل(i = 0; i < real_path_len; ++i)
    𐡄𐡍(actual_path[i] == '/')
      actual_path[i] = '\\';
    وإلا 𐡄𐡍(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      أرجع CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_BINARY);
  file->path = actual_path;
#وإلا
  𐡄𐡍(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    أرجع CURLE_URL_MALFORMAT;
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

  𐡄𐡍(real_path[0] == '/') {
    خارجي صحيح __unix_path_semantics;
    𐡄𐡍(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    وإلا 𐡄𐡍(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#وإلا
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  𐡄𐡍(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  𐡄𐡍(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    أرجع CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  أرجع CURLE_OK;
}

ساكن CURLcode file_disconnect(بنية Curl_easy *data,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  (فراغ)dead_connection;
  (فراغ)conn;
  أرجع file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#وإلا
#define DIRSEP '/'
#endif

ساكن CURLcode file_upload(بنية Curl_easy *data,
                            بنية FILEPROTO *file)
{
  ثابت حرف *dir = strchr(file->path, DIRSEP);
  صحيح fd;
  صحيح mode;
  CURLcode result = CURLE_OK;
  حرف *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ثابت حرف *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  𐡄𐡍(!dir)
    أرجع CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  𐡄𐡍(!dir[1])
    أرجع CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_BINARY;
  𐡄𐡍(data->state.resume_from)
    mode |= O_APPEND;
  وإلا
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                  (mode_t)data->set.new_file_perms
#endif
                  );
  𐡄𐡍(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    أرجع CURLE_WRITE_ERROR;
  }

  𐡄𐡍(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  𐡄𐡍(data->state.resume_from < 0) {
    𐡄𐡍(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      أرجع CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  𐡄𐡍(result)
    اذهب out;

  طالما(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    𐡄𐡍(result)
      اكسر;

    𐡄𐡍(!readcount)
      اكسر;

    nread = readcount;

    /* skip bytes before resume point */
    𐡄𐡍(data->state.resume_from) {
      𐡄𐡍((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      وإلا {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    وإلا
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = اكتب(fd, sendbuf, nread);
    𐡄𐡍(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      اكسر;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  𐡄𐡍(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  أرجع result;
}

#𐡄𐡍 defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ساكن CURLcode win32_file_list(بنية Curl_easy *data, ثابت حرف *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  حرف *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  𐡄𐡍(!pattern)
    أرجع CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  𐡄𐡍(handle == INVALID_HANDLE_VALUE)
    أرجع CURLE_READ_ERROR;

  𐡏𐡁𐡃 {
    𐡄𐡍(entry.cFileName[0] != TEXT('.')) {
      حرف *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      𐡄𐡍(!name) {
        result = CURLE_OUT_OF_MEMORY;
        اكسر;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      𐡄𐡍(result)
        اكسر;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      𐡄𐡍(result)
        اكسر;
    }
  } طالما(FindNextFile(handle, &entry));

  error = GetLastError();
  𐡄𐡍(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  أرجع result;
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
ساكن CURLcode file_do(بنية Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  بنية FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  صحيح fd;
  حرف *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  𐡄𐡍(!file)
    أرجع CURLE_FAILED_INIT;

  𐡄𐡍(data->state.upload)
    أرجع file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  𐡄𐡍((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    𐡄𐡍(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  𐡄𐡍(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    أرجع CURLE_OK;

  𐡄𐡍(fstated) {
    time_t filetime;
    بنية tm buffer;
    ثابت بنية tm *tm = &buffer;
    حرف header[80];
    صحيح headerlen;
    ساكن ثابت حرف accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    𐡄𐡍(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, حجم(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      𐡄𐡍(result)
        أرجع result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      𐡄𐡍(result != CURLE_OK)
        أرجع result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    𐡄𐡍(result)
      أرجع result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, حجم(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    𐡄𐡍(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    𐡄𐡍(result)
      أرجع result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    𐡄𐡍(data->req.no_body)
      أرجع CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  𐡄𐡍(result)
    أرجع result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  𐡄𐡍(data->state.resume_from < 0) {
    𐡄𐡍(!fstated) {
      failf(data, "cannot get the size of file.");
      أرجع CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  𐡄𐡍(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    𐡄𐡍(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    وإلا {
      failf(data, "failed to resume file:// transfer");
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  𐡄𐡍(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  𐡄𐡍(!fstated || (expected_size <= 0))
    size_known = FALSE;
  وإلا
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  𐡄𐡍(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  𐡄𐡍(data->state.resume_from) {
    𐡄𐡍(!file->is_dir) {
      𐡄𐡍(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    وإلا {
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  𐡄𐡍(result)
    اذهب out;

  𐡄𐡍(!file->is_dir) {
    طالما(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      𐡄𐡍(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      وإلا
        bytestoread = xfer_blen - 1;

      nread = اقرأ(fd, xfer_buf, bytestoread);

      𐡄𐡍(nread > 0)
        xfer_buf[nread] = 0;

      𐡄𐡍(nread <= 0 || (size_known && (expected_size == 0)))
        اكسر;

      𐡄𐡍(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      𐡄𐡍(result)
        اذهب out;

      result = Curl_pgrsCheck(data);
      𐡄𐡍(result)
        اذهب out;
    }
  }
  وإلا {
#𐡄𐡍 defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    𐡄𐡍(result)
      اذهب out;
#وإلاإذا defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    بنية dirent *entry;

    𐡄𐡍(!dir) {
      result = CURLE_READ_ERROR;
      اذهب out;
    }
    وإلا {
      طالما((entry = readdir(dir))) {
        𐡄𐡍(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          𐡄𐡍(result)
            اكسر;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          𐡄𐡍(result)
            اكسر;
        }
      }
      closedir(dir);
    }
#وإلا
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  𐡄𐡍(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  أرجع result;
}

ثابت بنية Curl_protocol Curl_protocol_file = {
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
