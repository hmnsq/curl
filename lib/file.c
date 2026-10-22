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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#अन्तर्भाव <dirent.h>
#endif

#अन्तर्भाव "progress.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "parsedate.h" /* for the week day and month names */
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curl_range.h"

#सचे defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#नोचेद्यदि defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

संरचना FILEPROTO {
  वर्ण *path; /* the path we operate on */
  वर्ण *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  पूर्णाङ्क fd;     /* open file descriptor to read from! */
  bool is_dir;
};

स्थिर शून्य file_cleanup(संरचना FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = रिक्त;
  file->is_dir = FALSE;
  सचे(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

स्थिर शून्य file_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना FILEPROTO *file = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  file_cleanup(file);
  curlx_free(file);
}

स्थिर CURLcode file_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना FILEPROTO *filep;
  (शून्य)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, परिमाण(*filep));
  सचे(filep)
    filep->fd = -1;
  सचे(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode file_done(संरचना Curl_easy *data,
                          CURLcode status, bool premature)
{
  संरचना FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (शून्य)status;
  (शून्य)premature;

  सचे(file)
    file_cleanup(file);

  पटिनिवत्त CURLE_OK;
}

स्थिर पूर्णाङ्क file_stat(ध्रुव वर्ण *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  पूर्णाङ्क result = curlx_stat(path, statbuf);

  सचे(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    सचे((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      वर्ण *trimmed = curlx_strdup(path);

      सचे(trimmed) {
        करोहि {
          trimmed[--pathlen] = '\0';
        } याव((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  पटिनिवत्त result;
#अञ्ञथा
  पटिनिवत्त curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
स्थिर CURLcode file_connect(संरचना Curl_easy *data, bool *done)
{
  वर्ण *real_path;
  संरचना FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  पूर्णाङ्क fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  वर्ण *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  सचे(!file)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  सचे(result)
    पटिनिवत्त result;

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
  सचे((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  अत्थाय(i = 0; i < real_path_len; ++i)
    सचे(actual_path[i] == '/')
      actual_path[i] = '\\';
    अञ्ञथा सचे(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      पटिनिवत्त CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_द्विमान);
  file->path = actual_path;
#अञ्ञथा
  सचे(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    पटिनिवत्त CURLE_URL_MALFORMAT;
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

  सचे(real_path[0] == '/') {
    बाह्य पूर्णाङ्क __unix_path_semantics;
    सचे(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    अञ्ञथा सचे(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#अञ्ञथा
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  सचे(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  सचे(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    पटिनिवत्त CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode file_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  (शून्य)dead_connection;
  (शून्य)conn;
  पटिनिवत्त file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#अञ्ञथा
#define DIRSEP '/'
#endif

स्थिर CURLcode file_upload(संरचना Curl_easy *data,
                            संरचना FILEPROTO *file)
{
  ध्रुव वर्ण *dir = strchr(file->path, DIRSEP);
  पूर्णाङ्क fd;
  पूर्णाङ्क mode;
  CURLcode result = CURLE_OK;
  वर्ण *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ध्रुव वर्ण *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  सचे(!dir)
    पटिनिवत्त CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  सचे(!dir[1])
    पटिनिवत्त CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_द्विमान;
  सचे(data->state.resume_from)
    mode |= O_APPEND;
  अञ्ञथा
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अञ्ञथा
                  (mode_t)data->set.new_file_perms
#endif
                  );
  सचे(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    पटिनिवत्त CURLE_WRITE_ERROR;
  }

  सचे(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  सचे(data->state.resume_from < 0) {
    सचे(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      पटिनिवत्त CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  सचे(result)
    गच्छ out;

  याव(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    सचे(result)
      भङ्ग;

    सचे(!readcount)
      भङ्ग;

    nread = readcount;

    /* skip bytes before resume point */
    सचे(data->state.resume_from) {
      सचे((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      अञ्ञथा {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    अञ्ञथा
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = लिख(fd, sendbuf, nread);
    सचे(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      भङ्ग;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  सचे(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  पटिनिवत्त result;
}

#सचे defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
स्थिर CURLcode win32_file_list(संरचना Curl_easy *data, ध्रुव वर्ण *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  वर्ण *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  सचे(!pattern)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  सचे(handle == INVALID_HANDLE_मानम्)
    पटिनिवत्त CURLE_READ_ERROR;

  करोहि {
    सचे(entry.cFileName[0] != TEXT('.')) {
      वर्ण *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      सचे(!name) {
        result = CURLE_OUT_OF_MEMORY;
        भङ्ग;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      सचे(result)
        भङ्ग;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      सचे(result)
        भङ्ग;
    }
  } याव(FindNextFile(handle, &entry));

  error = GetLastError();
  सचे(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  पटिनिवत्त result;
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
स्थिर CURLcode file_do(संरचना Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  संरचना FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  पूर्णाङ्क fd;
  वर्ण *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  सचे(!file)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(data->state.upload)
    पटिनिवत्त file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  सचे((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    सचे(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  सचे(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    पटिनिवत्त CURLE_OK;

  सचे(fstated) {
    time_t filetime;
    संरचना tm buffer;
    ध्रुव संरचना tm *tm = &buffer;
    वर्ण header[80];
    पूर्णाङ्क headerlen;
    स्थिर ध्रुव वर्ण accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    सचे(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, परिमाण(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      सचे(result)
        पटिनिवत्त result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      सचे(result != CURLE_OK)
        पटिनिवत्त result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    सचे(result)
      पटिनिवत्त result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, परिमाण(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    सचे(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    सचे(result)
      पटिनिवत्त result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    सचे(data->req.no_body)
      पटिनिवत्त CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  सचे(result)
    पटिनिवत्त result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  सचे(data->state.resume_from < 0) {
    सचे(!fstated) {
      failf(data, "cannot get the size of file.");
      पटिनिवत्त CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  सचे(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    सचे(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    अञ्ञथा {
      failf(data, "failed to resume file:// transfer");
      पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  सचे(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  सचे(!fstated || (expected_size <= 0))
    size_known = FALSE;
  अञ्ञथा
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  सचे(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  सचे(data->state.resume_from) {
    सचे(!file->is_dir) {
      सचे(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
    }
    अञ्ञथा {
      पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  सचे(result)
    गच्छ out;

  सचे(!file->is_dir) {
    याव(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      सचे(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      अञ्ञथा
        bytestoread = xfer_blen - 1;

      nread = पठ(fd, xfer_buf, bytestoread);

      सचे(nread > 0)
        xfer_buf[nread] = 0;

      सचे(nread <= 0 || (size_known && (expected_size == 0)))
        भङ्ग;

      सचे(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      सचे(result)
        गच्छ out;

      result = Curl_pgrsCheck(data);
      सचे(result)
        गच्छ out;
    }
  }
  अञ्ञथा {
#सचे defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    सचे(result)
      गच्छ out;
#नोचेद्यदि defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    संरचना dirent *entry;

    सचे(!dir) {
      result = CURLE_READ_ERROR;
      गच्छ out;
    }
    अञ्ञथा {
      याव((entry = readdir(dir))) {
        सचे(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          सचे(result)
            भङ्ग;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          सचे(result)
            भङ्ग;
        }
      }
      closedir(dir);
    }
#अञ्ञथा
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  सचे(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  पटिनिवत्त result;
}

ध्रुव संरचना Curl_protocol Curl_protocol_file = {
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
