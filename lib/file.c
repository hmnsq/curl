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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#അന്തര്ഭാവ <dirent.h>
#endif

#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "parsedate.h" /* for the week day and month names */
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curl_range.h"

#എങ്കിൽ defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#അല്ലെങ്കിലെങ്കിൽ defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

ഘടന FILEPROTO {
  അക്ഷരം *path; /* the path we operate on */
  അക്ഷരം *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  പൂർണ്ണസംഖ്യ fd;     /* open file descriptor to read from! */
  bool is_dir;
};

സ്ഥിതം ശൂന്യം file_cleanup(ഘടന FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = ശൂന്യ;
  file->is_dir = FALSE;
  എങ്കിൽ(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

സ്ഥിതം ശൂന്യം file_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന FILEPROTO *file = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  file_cleanup(file);
  curlx_free(file);
}

സ്ഥിതം CURLcode file_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന FILEPROTO *filep;
  (ശൂന്യം)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, വലുപ്പം(*filep));
  എങ്കിൽ(filep)
    filep->fd = -1;
  എങ്കിൽ(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode file_done(ഘടന Curl_easy *data,
                          CURLcode status, bool premature)
{
  ഘടന FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (ശൂന്യം)status;
  (ശൂന്യം)premature;

  എങ്കിൽ(file)
    file_cleanup(file);

  തിരികെ CURLE_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ file_stat(സ്ഥിരം അക്ഷരം *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  പൂർണ്ണസംഖ്യ result = curlx_stat(path, statbuf);

  എങ്കിൽ(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    എങ്കിൽ((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      അക്ഷരം *trimmed = curlx_strdup(path);

      എങ്കിൽ(trimmed) {
        ചെയ്യുക {
          trimmed[--pathlen] = '\0';
        } ആകുമ്പോൾ((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  തിരികെ result;
#അല്ലെങ്കിൽ
  തിരികെ curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
സ്ഥിതം CURLcode file_connect(ഘടന Curl_easy *data, bool *done)
{
  അക്ഷരം *real_path;
  ഘടന FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  പൂർണ്ണസംഖ്യ fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  അക്ഷരം *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  എങ്കിൽ(!file)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  എങ്കിൽ(result)
    തിരികെ result;

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
  എങ്കിൽ((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  വേണ്ടി(i = 0; i < real_path_len; ++i)
    എങ്കിൽ(actual_path[i] == '/')
      actual_path[i] = '\\';
    അല്ലെങ്കിൽ എങ്കിൽ(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      തിരികെ CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_ദ്വിമാന);
  file->path = actual_path;
#അല്ലെങ്കിൽ
  എങ്കിൽ(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    തിരികെ CURLE_URL_MALFORMAT;
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

  എങ്കിൽ(real_path[0] == '/') {
    ബാഹ്യ പൂർണ്ണസംഖ്യ __unix_path_semantics;
    എങ്കിൽ(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#അല്ലെങ്കിൽ
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  എങ്കിൽ(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  എങ്കിൽ(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    തിരികെ CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode file_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn,
                                bool dead_connection)
{
  (ശൂന്യം)dead_connection;
  (ശൂന്യം)conn;
  തിരികെ file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#അല്ലെങ്കിൽ
#define DIRSEP '/'
#endif

സ്ഥിതം CURLcode file_upload(ഘടന Curl_easy *data,
                            ഘടന FILEPROTO *file)
{
  സ്ഥിരം അക്ഷരം *dir = strchr(file->path, DIRSEP);
  പൂർണ്ണസംഖ്യ fd;
  പൂർണ്ണസംഖ്യ mode;
  CURLcode result = CURLE_OK;
  അക്ഷരം *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  സ്ഥിരം അക്ഷരം *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  എങ്കിൽ(!dir)
    തിരികെ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  എങ്കിൽ(!dir[1])
    തിരികെ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_ദ്വിമാന;
  എങ്കിൽ(data->state.resume_from)
    mode |= O_APPEND;
  അല്ലെങ്കിൽ
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#അല്ലെങ്കിൽ
                  (mode_t)data->set.new_file_perms
#endif
                  );
  എങ്കിൽ(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    തിരികെ CURLE_WRITE_ERROR;
  }

  എങ്കിൽ(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  എങ്കിൽ(data->state.resume_from < 0) {
    എങ്കിൽ(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      തിരികെ CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  എങ്കിൽ(result)
    പോകുക out;

  ആകുമ്പോൾ(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(!readcount)
      നിർത്തുക;

    nread = readcount;

    /* skip bytes before resume point */
    എങ്കിൽ(data->state.resume_from) {
      എങ്കിൽ((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      അല്ലെങ്കിൽ {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    അല്ലെങ്കിൽ
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = ലിഖ(fd, sendbuf, nread);
    എങ്കിൽ(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      നിർത്തുക;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  എങ്കിൽ(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  തിരികെ result;
}

#എങ്കിൽ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
സ്ഥിതം CURLcode win32_file_list(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  അക്ഷരം *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  എങ്കിൽ(!pattern)
    തിരികെ CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  എങ്കിൽ(handle == INVALID_HANDLE_മൂല്യം)
    തിരികെ CURLE_READ_ERROR;

  ചെയ്യുക {
    എങ്കിൽ(entry.cFileName[0] != TEXT('.')) {
      അക്ഷരം *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      എങ്കിൽ(!name) {
        result = CURLE_OUT_OF_MEMORY;
        നിർത്തുക;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      എങ്കിൽ(result)
        നിർത്തുക;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      എങ്കിൽ(result)
        നിർത്തുക;
    }
  } ആകുമ്പോൾ(FindNextFile(handle, &entry));

  error = GetLastError();
  എങ്കിൽ(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  തിരികെ result;
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
സ്ഥിതം CURLcode file_do(ഘടന Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ഘടന FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  പൂർണ്ണസംഖ്യ fd;
  അക്ഷരം *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  എങ്കിൽ(!file)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(data->state.upload)
    തിരികെ file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  എങ്കിൽ((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    എങ്കിൽ(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  എങ്കിൽ(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    തിരികെ CURLE_OK;

  എങ്കിൽ(fstated) {
    time_t filetime;
    ഘടന tm buffer;
    സ്ഥിരം ഘടന tm *tm = &buffer;
    അക്ഷരം header[80];
    പൂർണ്ണസംഖ്യ headerlen;
    സ്ഥിതം സ്ഥിരം അക്ഷരം accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    എങ്കിൽ(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, വലുപ്പം(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      എങ്കിൽ(result)
        തിരികെ result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      എങ്കിൽ(result != CURLE_OK)
        തിരികെ result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    എങ്കിൽ(result)
      തിരികെ result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, വലുപ്പം(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    എങ്കിൽ(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    എങ്കിൽ(result)
      തിരികെ result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    എങ്കിൽ(data->req.no_body)
      തിരികെ CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  എങ്കിൽ(data->state.resume_from < 0) {
    എങ്കിൽ(!fstated) {
      failf(data, "cannot get the size of file.");
      തിരികെ CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  എങ്കിൽ(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    എങ്കിൽ(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    അല്ലെങ്കിൽ {
      failf(data, "failed to resume file:// transfer");
      തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  എങ്കിൽ(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  എങ്കിൽ(!fstated || (expected_size <= 0))
    size_known = FALSE;
  അല്ലെങ്കിൽ
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  എങ്കിൽ(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  എങ്കിൽ(data->state.resume_from) {
    എങ്കിൽ(!file->is_dir) {
      എങ്കിൽ(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
    }
    അല്ലെങ്കിൽ {
      തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(!file->is_dir) {
    ആകുമ്പോൾ(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      എങ്കിൽ(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      അല്ലെങ്കിൽ
        bytestoread = xfer_blen - 1;

      nread = പഠ(fd, xfer_buf, bytestoread);

      എങ്കിൽ(nread > 0)
        xfer_buf[nread] = 0;

      എങ്കിൽ(nread <= 0 || (size_known && (expected_size == 0)))
        നിർത്തുക;

      എങ്കിൽ(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      എങ്കിൽ(result)
        പോകുക out;

      result = Curl_pgrsCheck(data);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }
  അല്ലെങ്കിൽ {
#എങ്കിൽ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    എങ്കിൽ(result)
      പോകുക out;
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    ഘടന dirent *entry;

    എങ്കിൽ(!dir) {
      result = CURLE_READ_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      ആകുമ്പോൾ((entry = readdir(dir))) {
        എങ്കിൽ(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          എങ്കിൽ(result)
            നിർത്തുക;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          എങ്കിൽ(result)
            നിർത്തുക;
        }
      }
      closedir(dir);
    }
#അല്ലെങ്കിൽ
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  എങ്കിൽ(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  തിരികെ result;
}

സ്ഥിരം ഘടന Curl_protocol Curl_protocol_file = {
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
