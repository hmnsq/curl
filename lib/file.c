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

#जर defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#किंवाजर defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

संरचना FILEPROTO {
  अक्षर *path; /* the path we operate on */
  अक्षर *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  पूर्णांक fd;     /* open file descriptor to read from! */
  bool is_dir;
};

स्थिर शून्य file_cleanup(संरचना FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = रिक्त;
  file->is_dir = FALSE;
  जर(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

स्थिर शून्य file_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना FILEPROTO *file = entry;
  (शून्य)किल्ली;
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
  filep = curlx_calloc(1, आकार(*filep));
  जर(filep)
    filep->fd = -1;
  जर(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    परत CURLE_OUT_OF_MEMORY;

  परत CURLE_OK;
}

स्थिर CURLcode file_done(संरचना Curl_easy *data,
                          CURLcode status, bool premature)
{
  संरचना FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (शून्य)status;
  (शून्य)premature;

  जर(file)
    file_cleanup(file);

  परत CURLE_OK;
}

स्थिर पूर्णांक file_stat(स्थिरांक अक्षर *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  पूर्णांक result = curlx_stat(path, statbuf);

  जर(result) {
    माप_प्रकार pathlen = म_माप(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    जर((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      अक्षर *trimmed = curlx_strdup(path);

      जर(trimmed) {
        करा {
          trimmed[--pathlen] = '\0';
        } पर्यंत((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  परत result;
#अन्यथा
  परत curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
स्थिर CURLcode file_connect(संरचना Curl_easy *data, bool *done)
{
  अक्षर *real_path;
  संरचना FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  पूर्णांक fd;
#ifdef DOS_FILESYSTEM
  माप_प्रकार i;
  अक्षर *actual_path;
#endif
  माप_प्रकार real_path_len;
  CURLcode result;

  जर(!file)
    परत CURLE_FAILED_INIT;

  जर(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    परत CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  जर(result)
    परत result;

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
  जर((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  साठी(i = 0; i < real_path_len; ++i)
    जर(actual_path[i] == '/')
      actual_path[i] = '\\';
    अन्यथा जर(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      परत CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_द्विमान);
  file->path = actual_path;
#अन्यथा
  जर(स_प्रथम(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    परत CURLE_URL_MALFORMAT;
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

  जर(real_path[0] == '/') {
    बाह्य पूर्णांक __unix_path_semantics;
    जर(म_अक्षर(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    अन्यथा जर(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#अन्यथा
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  जर(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  जर(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    परत CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  परत CURLE_OK;
}

स्थिर CURLcode file_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  (शून्य)dead_connection;
  (शून्य)conn;
  परत file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#अन्यथा
#define DIRSEP '/'
#endif

स्थिर CURLcode file_upload(संरचना Curl_easy *data,
                            संरचना FILEPROTO *file)
{
  स्थिरांक अक्षर *dir = म_अक्षर(file->path, DIRSEP);
  पूर्णांक fd;
  पूर्णांक mode;
  CURLcode result = CURLE_OK;
  अक्षर *xfer_ulbuf;
  माप_प्रकार xfer_ulblen;
  curlx_struct_stat file_stat;
  स्थिरांक अक्षर *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  जर(!dir)
    परत CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  जर(!dir[1])
    परत CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_द्विमान;
  जर(data->state.resume_from)
    mode |= O_APPEND;
  अन्यथा
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                  (mode_t)data->set.new_file_perms
#endif
                  );
  जर(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    परत CURLE_WRITE_ERROR;
  }

  जर(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  जर(data->state.resume_from < 0) {
    जर(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      परत CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  जर(result)
    जा out;

  पर्यंत(!result && !eos) {
    माप_प्रकार nread, nwritten;
    ssize_t rv;
    माप_प्रकार readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    जर(result)
      थांबा;

    जर(!readcount)
      थांबा;

    nread = readcount;

    /* skip bytes before resume point */
    जर(data->state.resume_from) {
      जर((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      अन्यथा {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (माप_प्रकार)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    अन्यथा
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = लिख(fd, sendbuf, nread);
    जर(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      थांबा;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  जर(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  परत result;
}

#जर defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
स्थिर CURLcode win32_file_list(संरचना Curl_easy *data, स्थिरांक अक्षर *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  अक्षर *pattern;
  माप_प्रकार pathlen = म_माप(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  जर(!pattern)
    परत CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  जर(handle == INVALID_HANDLE_मूल्य)
    परत CURLE_READ_ERROR;

  करा {
    जर(entry.cFileName[0] != TEXT('.')) {
      अक्षर *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      जर(!name) {
        result = CURLE_OUT_OF_MEMORY;
        थांबा;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, म_माप(name));
      curlx_free(name);
      जर(result)
        थांबा;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      जर(result)
        थांबा;
    }
  } पर्यंत(FindNextFile(handle, &entry));

  error = GetLastError();
  जर(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  परत result;
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
  पूर्णांक fd;
  अक्षर *xfer_buf;
  माप_प्रकार xfer_blen;

  *done = TRUE; /* unconditionally */
  जर(!file)
    परत CURLE_FAILED_INIT;

  जर(data->state.upload)
    परत file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  जर((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    जर(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  जर(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    परत CURLE_OK;

  जर(fstated) {
    समय_प्रकार filetime;
    संरचना पंचांग buffer;
    स्थिरांक संरचना पंचांग *पंचांग = &buffer;
    अक्षर header[80];
    पूर्णांक headerlen;
    स्थिर स्थिरांक अक्षर accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    जर(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, आकार(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      जर(result)
        परत result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      जर(result != CURLE_OK)
        परत result;
    }

    filetime = (समय_प्रकार)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    जर(result)
      परत result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, आकार(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday - 1 : 6],
                     पंचांग->tm_mday,
                     Curl_month[पंचांग->tm_mon],
                     पंचांग->tm_year + 1900,
                     पंचांग->tm_hour,
                     पंचांग->tm_min,
                     पंचांग->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    जर(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    जर(result)
      परत result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    जर(data->req.no_body)
      परत CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  जर(result)
    परत result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  जर(data->state.resume_from < 0) {
    जर(!fstated) {
      failf(data, "cannot get the size of file.");
      परत CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  जर(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    जर(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    अन्यथा {
      failf(data, "failed to resume file:// transfer");
      परत CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  जर(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  जर(!fstated || (expected_size <= 0))
    size_known = FALSE;
  अन्यथा
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  जर(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  जर(data->state.resume_from) {
    जर(!file->is_dir) {
      जर(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, शुरू_से))
        परत CURLE_BAD_DOWNLOAD_RESUME;
    }
    अन्यथा {
      परत CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  जर(result)
    जा out;

  जर(!file->is_dir) {
    पर्यंत(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      माप_प्रकार bytestoread;

      जर(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      अन्यथा
        bytestoread = xfer_blen - 1;

      nread = पठ(fd, xfer_buf, bytestoread);

      जर(nread > 0)
        xfer_buf[nread] = 0;

      जर(nread <= 0 || (size_known && (expected_size == 0)))
        थांबा;

      जर(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      जर(result)
        जा out;

      result = Curl_pgrsCheck(data);
      जर(result)
        जा out;
    }
  }
  अन्यथा {
#जर defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    जर(result)
      जा out;
#किंवाजर defined(HAVE_OPENDIR)
    सूची *dir = सूची_खोलो(file->path);
    संरचना dirent *entry;

    जर(!dir) {
      result = CURLE_READ_ERROR;
      जा out;
    }
    अन्यथा {
      पर्यंत((entry = सूची_पढ़ो(dir))) {
        जर(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, म_माप(entry->d_name));
          जर(result)
            थांबा;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          जर(result)
            थांबा;
        }
      }
      बंद_सूची(dir);
    }
#अन्यथा
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  जर(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  परत result;
}

स्थिरांक संरचना Curl_protocol Curl_protocol_file = {
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
