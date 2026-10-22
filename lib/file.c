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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#அந்தர்பாவ <dirent.h>
#endif

#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "parsedate.h" /* for the week day and month names */
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curl_range.h"

#எனில் defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#அல்லதெனில் defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

கட்டமைப்பு FILEPROTO {
  எழுத்து *path; /* the path we operate on */
  எழுத்து *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  முழு fd;     /* open file descriptor to read from! */
  bool is_dir;
};

நிலையான வெறுமை file_cleanup(கட்டமைப்பு FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = வெற்று;
  file->is_dir = FALSE;
  எனில்(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

நிலையான வெறுமை file_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு FILEPROTO *file = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  file_cleanup(file);
  curlx_free(file);
}

நிலையான CURLcode file_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு FILEPROTO *filep;
  (வெறுமை)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, அளவு(*filep));
  எனில்(filep)
    filep->fd = -1;
  எனில்(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode file_done(கட்டமைப்பு Curl_easy *data,
                          CURLcode status, bool premature)
{
  கட்டமைப்பு FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (வெறுமை)status;
  (வெறுமை)premature;

  எனில்(file)
    file_cleanup(file);

  திரும்பு CURLE_OK;
}

நிலையான முழு file_stat(மாறிலி எழுத்து *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  முழு result = curlx_stat(path, statbuf);

  எனில்(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    எனில்((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      எழுத்து *trimmed = curlx_strdup(path);

      எனில்(trimmed) {
        செய் {
          trimmed[--pathlen] = '\0';
        } போது((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  திரும்பு result;
#இல்லையெனில்
  திரும்பு curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
நிலையான CURLcode file_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  எழுத்து *real_path;
  கட்டமைப்பு FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  முழு fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  எழுத்து *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  எனில்(!file)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  எனில்(result)
    திரும்பு result;

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
  எனில்((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  க்கு(i = 0; i < real_path_len; ++i)
    எனில்(actual_path[i] == '/')
      actual_path[i] = '\\';
    இல்லையெனில் எனில்(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      திரும்பு CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_இருமம்);
  file->path = actual_path;
#இல்லையெனில்
  எனில்(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    திரும்பு CURLE_URL_MALFORMAT;
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

  எனில்(real_path[0] == '/') {
    வெளி முழு __unix_path_semantics;
    எனில்(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    இல்லையெனில் எனில்(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#இல்லையெனில்
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  எனில்(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  எனில்(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    திரும்பு CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode file_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn,
                                bool dead_connection)
{
  (வெறுமை)dead_connection;
  (வெறுமை)conn;
  திரும்பு file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#இல்லையெனில்
#define DIRSEP '/'
#endif

நிலையான CURLcode file_upload(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு FILEPROTO *file)
{
  மாறிலி எழுத்து *dir = strchr(file->path, DIRSEP);
  முழு fd;
  முழு mode;
  CURLcode result = CURLE_OK;
  எழுத்து *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  மாறிலி எழுத்து *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  எனில்(!dir)
    திரும்பு CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  எனில்(!dir[1])
    திரும்பு CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_இருமம்;
  எனில்(data->state.resume_from)
    mode |= O_APPEND;
  இல்லையெனில்
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#இல்லையெனில்
                  (mode_t)data->set.new_file_perms
#endif
                  );
  எனில்(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    திரும்பு CURLE_WRITE_ERROR;
  }

  எனில்(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  எனில்(data->state.resume_from < 0) {
    எனில்(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      திரும்பு CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  எனில்(result)
    செல் out;

  போது(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    எனில்(result)
      நிறுத்து;

    எனில்(!readcount)
      நிறுத்து;

    nread = readcount;

    /* skip bytes before resume point */
    எனில்(data->state.resume_from) {
      எனில்((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      இல்லையெனில் {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    இல்லையெனில்
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = லிக(fd, sendbuf, nread);
    எனில்(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      நிறுத்து;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  எனில்(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  திரும்பு result;
}

#எனில் defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
நிலையான CURLcode win32_file_list(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  எழுத்து *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  எனில்(!pattern)
    திரும்பு CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  எனில்(handle == INVALID_HANDLE_மதிப்பு)
    திரும்பு CURLE_READ_ERROR;

  செய் {
    எனில்(entry.cFileName[0] != TEXT('.')) {
      எழுத்து *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      எனில்(!name) {
        result = CURLE_OUT_OF_MEMORY;
        நிறுத்து;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      எனில்(result)
        நிறுத்து;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      எனில்(result)
        நிறுத்து;
    }
  } போது(FindNextFile(handle, &entry));

  error = GetLastError();
  எனில்(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  திரும்பு result;
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
நிலையான CURLcode file_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  கட்டமைப்பு FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  முழு fd;
  எழுத்து *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  எனில்(!file)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(data->state.upload)
    திரும்பு file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  எனில்((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    எனில்(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  எனில்(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    திரும்பு CURLE_OK;

  எனில்(fstated) {
    time_t filetime;
    கட்டமைப்பு tm buffer;
    மாறிலி கட்டமைப்பு tm *tm = &buffer;
    எழுத்து header[80];
    முழு headerlen;
    நிலையான மாறிலி எழுத்து accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    எனில்(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, அளவு(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      எனில்(result)
        திரும்பு result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      எனில்(result != CURLE_OK)
        திரும்பு result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    எனில்(result)
      திரும்பு result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, அளவு(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    எனில்(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    எனில்(result)
      திரும்பு result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    எனில்(data->req.no_body)
      திரும்பு CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  எனில்(result)
    திரும்பு result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  எனில்(data->state.resume_from < 0) {
    எனில்(!fstated) {
      failf(data, "cannot get the size of file.");
      திரும்பு CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  எனில்(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    எனில்(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    இல்லையெனில் {
      failf(data, "failed to resume file:// transfer");
      திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  எனில்(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  எனில்(!fstated || (expected_size <= 0))
    size_known = FALSE;
  இல்லையெனில்
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  எனில்(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  எனில்(data->state.resume_from) {
    எனில்(!file->is_dir) {
      எனில்(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
    }
    இல்லையெனில் {
      திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  எனில்(result)
    செல் out;

  எனில்(!file->is_dir) {
    போது(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      எனில்(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      இல்லையெனில்
        bytestoread = xfer_blen - 1;

      nread = பட(fd, xfer_buf, bytestoread);

      எனில்(nread > 0)
        xfer_buf[nread] = 0;

      எனில்(nread <= 0 || (size_known && (expected_size == 0)))
        நிறுத்து;

      எனில்(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      எனில்(result)
        செல் out;

      result = Curl_pgrsCheck(data);
      எனில்(result)
        செல் out;
    }
  }
  இல்லையெனில் {
#எனில் defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    எனில்(result)
      செல் out;
#அல்லதெனில் defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    கட்டமைப்பு dirent *entry;

    எனில்(!dir) {
      result = CURLE_READ_ERROR;
      செல் out;
    }
    இல்லையெனில் {
      போது((entry = readdir(dir))) {
        எனில்(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          எனில்(result)
            நிறுத்து;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          எனில்(result)
            நிறுத்து;
        }
      }
      closedir(dir);
    }
#இல்லையெனில்
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  எனில்(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  திரும்பு result;
}

மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_file = {
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
