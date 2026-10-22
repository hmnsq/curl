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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "file.h"

#अगर_अघोषित CURL_DISABLE_FILE

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_DIRENT_H
#समावेश <dirent.h>
#पूर्ण_अगर

#समावेश "progress.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "escape.h"
#समावेश "multiif.h"
#समावेश "transfer.h"
#समावेश "url.h"
#समावेश "parsedate.h" /* for the week day and month names */
#समावेश "curlx/win32-fopen.h"
#समावेश "curl_range.h"

#अगर defined(_WIN32) || defined(MSDOS)
#घोषणा DOS_FILESYSTEM 1
#या_अगर defined(__amigaos4__)
#घोषणा AMIGA_FILESYSTEM 1
#पूर्ण_अगर

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_FILE_EASY   "meta:proto:file:easy"

काष्ठा FILEPROTO {
  अक्षर *path; /* the path we operate on */
  अक्षर *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  पूर्णांक fd;     /* open file descriptor to read from! */
  bool is_dir;
};

अटल व्योम file_cleanup(काष्ठा FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = शून्य;
  file->is_dir = FALSE;
  अगर(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

अटल व्योम file_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा FILEPROTO *file = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  file_cleanup(file);
  curlx_free(file);
}

अटल CURLcode file_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा FILEPROTO *filep;
  (व्योम)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, माप(*filep));
  अगर(filep)
    filep->fd = -1;
  अगर(!filep ||
     Curl_meta_set(तथ्य, CURL_META_FILE_EASY, filep, file_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

अटल CURLcode file_done(काष्ठा Curl_easy *तथ्य,
                          CURLcode status, bool premature)
{
  काष्ठा FILEPROTO *file = Curl_meta_get(तथ्य, CURL_META_FILE_EASY);
  (व्योम)status;
  (व्योम)premature;

  अगर(file)
    file_cleanup(file);

  वापस CURLE_OK;
}

अटल पूर्णांक file_stat(स्थिर अक्षर *path, curlx_struct_stat *statbuf)
{
#अगर_घोषित _WIN32
  पूर्णांक परिणाम = curlx_stat(path, statbuf);

  अगर(परिणाम) {
    माप_प्रकार pathlen = म_माप(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    अगर((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      अक्षर *trimmed = curlx_strdup(path);

      अगर(trimmed) {
        करो {
          trimmed[--pathlen] = '\0';
        } जबतक((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        परिणाम = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  वापस परिणाम;
#अन्यथा
  वापस curlx_stat(path, statbuf);
#पूर्ण_अगर
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
अटल CURLcode file_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  अक्षर *real_path;
  काष्ठा FILEPROTO *file = Curl_meta_get(तथ्य, CURL_META_FILE_EASY);
#अगर_घोषित _WIN32
  curlx_struct_stat statbuf;
#पूर्ण_अगर
  पूर्णांक fd;
#अगर_घोषित DOS_FILESYSTEM
  माप_प्रकार i;
  अक्षर *actual_path;
#पूर्ण_अगर
  माप_प्रकार real_path_len;
  CURLcode परिणाम;

  अगर(!file)
    वापस CURLE_FAILED_INIT;

  अगर(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    वापस CURLE_OK;
  }

  परिणाम = Curl_urldecode(तथ्य->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  अगर(परिणाम)
    वापस परिणाम;

#अगर_घोषित DOS_FILESYSTEM
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
  अगर((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  क्रम(i = 0; i < real_path_len; ++i)
    अगर(actual_path[i] == '/')
      actual_path[i] = '\\';
    अन्यथा अगर(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      वापस CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_द्विआधारी);
  file->path = actual_path;
#अन्यथा
  अगर(स_प्रथम(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    वापस CURLE_URL_MALFORMAT;
  }

#अगर_घोषित AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->path = real_path;

  अगर(real_path[0] == '/') {
    बाह्य पूर्णांक __unix_path_semantics;
    अगर(म_अक्षर(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    अन्यथा अगर(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#अन्यथा
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#पूर्ण_अगर
#पूर्ण_अगर
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#अगर_घोषित _WIN32
  अगर(!तथ्य->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#पूर्ण_अगर

  अगर(!तथ्य->state.upload && (fd == -1) && !file->is_dir) {
    failf(तथ्य, "Could not open file %s", तथ्य->state.up.path);
    file_done(तथ्य, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    वापस CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  वापस CURLE_OK;
}

अटल CURLcode file_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn,
                                bool dead_connection)
{
  (व्योम)dead_connection;
  (व्योम)conn;
  वापस file_done(तथ्य, CURLE_OK, FALSE);
}

#अगर_घोषित DOS_FILESYSTEM
#घोषणा DIRSEP '\\'
#अन्यथा
#घोषणा DIRSEP '/'
#पूर्ण_अगर

अटल CURLcode file_upload(काष्ठा Curl_easy *तथ्य,
                            काष्ठा FILEPROTO *file)
{
  स्थिर अक्षर *dir = म_अक्षर(file->path, DIRSEP);
  पूर्णांक fd;
  पूर्णांक mode;
  CURLcode परिणाम = CURLE_OK;
  अक्षर *xfer_ulbuf;
  माप_प्रकार xfer_ulblen;
  curlx_struct_stat file_stat;
  स्थिर अक्षर *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  अगर(!dir)
    वापस CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  अगर(!dir[1])
    वापस CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_द्विआधारी;
  अगर(तथ्य->state.resume_from)
    mode |= O_APPEND;
  अन्यथा
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#अगर_घोषित _WIN32
                  तथ्य->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                  (mode_t)तथ्य->set.new_file_perms
#पूर्ण_अगर
                  );
  अगर(fd < 0) {
    failf(तथ्य, "cannot open %s for writing", file->path);
    वापस CURLE_WRITE_ERROR;
  }

  अगर(तथ्य->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  अगर(तथ्य->state.resume_from < 0) {
    अगर(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(तथ्य, "cannot get the size of %s", file->path);
      वापस CURLE_WRITE_ERROR;
    }
    तथ्य->state.resume_from = (curl_off_t)file_stat.st_आकार;
  }

  परिणाम = Curl_multi_xfer_ulbuf_borrow(तथ्य, &xfer_ulbuf, &xfer_ulblen);
  अगर(परिणाम)
    जाओ out;

  जबतक(!परिणाम && !eos) {
    माप_प्रकार nread, nwritten;
    ssize_t rv;
    माप_प्रकार readcount;

    परिणाम = Curl_client_read(तथ्य, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    अगर(परिणाम)
      अवरोध;

    अगर(!readcount)
      अवरोध;

    nread = readcount;

    /* skip bytes before resume point */
    अगर(तथ्य->state.resume_from) {
      अगर((curl_off_t)nread <= तथ्य->state.resume_from) {
        तथ्य->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      अन्यथा {
        sendbuf = xfer_ulbuf + तथ्य->state.resume_from;
        nread -= (माप_प्रकार)तथ्य->state.resume_from;
        तथ्य->state.resume_from = 0;
      }
    }
    अन्यथा
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = ग_लिखो(fd, sendbuf, nread);
    अगर(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      परिणाम = CURLE_SEND_ERROR;
      अवरोध;
    }
    Curl_pgrs_upload_inc(तथ्य, nwritten);

    परिणाम = Curl_pgrsCheck(तथ्य);
  }
  अगर(!परिणाम)
    परिणाम = Curl_pgrsUpdate(तथ्य);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(तथ्य, xfer_ulbuf);

  वापस परिणाम;
}

#अगर defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
अटल CURLcode win32_file_list(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *path)
{
  WIN32_FIND_तथ्य entry;
  HANDLE handle;
  अक्षर *pattern;
  माप_प्रकार pathlen = म_माप(path);
  CURLcode परिणाम = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  अगर(!pattern)
    वापस CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  अगर(handle == INVALID_HANDLE_मूल्य)
    वापस CURLE_READ_ERROR;

  करो {
    अगर(entry.cFileName[0] != TEXT('.')) {
      अक्षर *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      अगर(!name) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        अवरोध;
      }

      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                 name, म_माप(name));
      curlx_free(name);
      अगर(परिणाम)
        अवरोध;

      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
      अगर(परिणाम)
        अवरोध;
    }
  } जबतक(Find_अग्र_File(handle, &entry));

  error = GetLastError();
  अगर(!परिणाम && error != ERROR_NO_MORE_FILES)
    परिणाम = CURLE_READ_ERROR;

  FindClose(handle);
  वापस परिणाम;
}
#पूर्ण_अगर

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
अटल CURLcode file_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  काष्ठा FILEPROTO *file = Curl_meta_get(तथ्य, CURL_META_FILE_EASY);
  CURLcode परिणाम = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_आकार = -1;
  bool आकार_known;
  bool fstated = FALSE;
  पूर्णांक fd;
  अक्षर *xfer_buf;
  माप_प्रकार xfer_blen;

  *done = TRUE; /* unconditionally */
  अगर(!file)
    वापस CURLE_FAILED_INIT;

  अगर(तथ्य->state.upload)
    वापस file_upload(तथ्य, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  अगर((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    अगर(!file->is_dir)
      expected_आकार = statbuf.st_आकार;
    /* and store the modification time */
    तथ्य->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  अगर(fstated && !तथ्य->state.range && तथ्य->set.timecondition &&
     !Curl_meets_timecondition(तथ्य, तथ्य->info.filetime))
    वापस CURLE_OK;

  अगर(fstated) {
    समय_प्रकार filetime;
    काष्ठा पंचांग बफर;
    स्थिर काष्ठा पंचांग *पंचांग = &बफर;
    अक्षर header[80];
    पूर्णांक headerlen;
    अटल स्थिर अक्षर accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    अगर(expected_आकार >= 0) {
      headerlen =
        curl_msnprintf(header, माप(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_आकार);
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, header, headerlen);
      अगर(परिणाम)
        वापस परिणाम;

      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      अगर(परिणाम != CURLE_OK)
        वापस परिणाम;
    }

    filetime = (समय_प्रकार)statbuf.st_mtime;
    परिणाम = curlx_gmtime(filetime, &बफर);
    अगर(परिणाम)
      वापस परिणाम;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, माप(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday - 1 : 6],
                     पंचांग->tm_mday,
                     Curl_month[पंचांग->tm_mon],
                     पंचांग->tm_year + 1900,
                     पंचांग->tm_hour,
                     पंचांग->tm_min,
                     पंचांग->tm_sec);
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, header, headerlen);
    अगर(!परिणाम)
      /* end of headers */
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, "\r\n", 2);
    अगर(परिणाम)
      वापस परिणाम;
    /* set the file size to make it available post transfer */
    Curl_pgrs_Set_Download_आकार(तथ्य, expected_आकार);
    अगर(तथ्य->req.no_body)
      वापस CURLE_OK;
  }

  /* Check whether file range has been specified */
  परिणाम = Curl_range(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  अगर(तथ्य->state.resume_from < 0) {
    अगर(!fstated) {
      failf(तथ्य, "cannot get the size of file.");
      वापस CURLE_READ_ERROR;
    }
    तथ्य->state.resume_from += (curl_off_t)statbuf.st_आकार;
  }

  अगर(तथ्य->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    अगर(तथ्य->state.resume_from <= expected_आकार)
      expected_आकार -= तथ्य->state.resume_from;
    अन्यथा {
      failf(तथ्य, "failed to resume file:// transfer");
      वापस CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  अगर(तथ्य->req.maxdownload > 0)
    expected_आकार = तथ्य->req.maxdownload;

  अगर(!fstated || (expected_आकार <= 0))
    आकार_known = FALSE;
  अन्यथा
    आकार_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  अगर(आकार_known)
    Curl_pgrs_Set_Download_आकार(तथ्य, expected_आकार);

  अगर(तथ्य->state.resume_from) {
    अगर(!file->is_dir) {
      अगर(तथ्य->state.resume_from !=
         curl_lseek(fd, तथ्य->state.resume_from, शुरू_से))
        वापस CURLE_BAD_DOWNLOAD_RESUME;
    }
    अन्यथा {
      वापस CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  परिणाम = Curl_multi_xfer_buf_borrow(तथ्य, &xfer_buf, &xfer_blen);
  अगर(परिणाम)
    जाओ out;

  अगर(!file->is_dir) {
    जबतक(!परिणाम) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      माप_प्रकार bytestoread;

      अगर(आकार_known) {
        bytestoread = (expected_आकार < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_आकार) : (xfer_blen - 1);
      }
      अन्यथा
        bytestoread = xfer_blen - 1;

      nread = पढ़ो(fd, xfer_buf, bytestoread);

      अगर(nread > 0)
        xfer_buf[nread] = 0;

      अगर(nread <= 0 || (आकार_known && (expected_आकार == 0)))
        अवरोध;

      अगर(आकार_known)
        expected_आकार -= nread;

      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, xfer_buf, nread);
      अगर(परिणाम)
        जाओ out;

      परिणाम = Curl_pgrsCheck(तथ्य);
      अगर(परिणाम)
        जाओ out;
    }
  }
  अन्यथा {
#अगर defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    परिणाम = win32_file_list(तथ्य, file->path);
    अगर(परिणाम)
      जाओ out;
#या_अगर defined(HAVE_OPENDIR)
    सूची *dir = सूची_खोलो(file->path);
    काष्ठा dirent *entry;

    अगर(!dir) {
      परिणाम = CURLE_READ_ERROR;
      जाओ out;
    }
    अन्यथा {
      जबतक((entry = सूची_पढ़ो(dir))) {
        अगर(entry->d_name[0] != '.') {
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                   entry->d_name, म_माप(entry->d_name));
          अगर(परिणाम)
            अवरोध;
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
          अगर(परिणाम)
            अवरोध;
        }
      }
      बंद_सूची(dir);
    }
#अन्यथा
    failf(तथ्य, "Directory listing not yet implemented on this platform.");
    परिणाम = CURLE_READ_ERROR;
#पूर्ण_अगर
  }

  अगर(!परिणाम)
    परिणाम = Curl_pgrsUpdate(तथ्य);

out:
  Curl_multi_xfer_buf_release(तथ्य, xfer_buf);
  वापस परिणाम;
}

स्थिर काष्ठा Curl_protocol Curl_protocol_file = {
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

#पूर्ण_अगर
