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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#ଅନ୍ତର୍ଭାଵ <dirent.h>
#endif

#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h" /* for the week day and month names */
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curl_range.h"

#ଯଦି defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#ନଚେତଯଦି defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

ସଂରଚନା FILEPROTO {
  ଅକ୍ଷର *path; /* the path we operate on */
  ଅକ୍ଷର *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;     /* open file descriptor to read from! */
  bool is_dir;
};

ସ୍ଥିର ଶୂନ୍ୟ file_cleanup(ସଂରଚନା FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = ଖାଲି;
  file->is_dir = FALSE;
  ଯଦି(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ file_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା FILEPROTO *file = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  file_cleanup(file);
  curlx_free(file);
}

ସ୍ଥିର CURLcode file_setup_connection(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା FILEPROTO *filep;
  (ଶୂନ୍ୟ)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, ଆକାର(*filep));
  ଯଦି(filep)
    filep->fd = -1;
  ଯଦି(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode file_done(ସଂରଚନା Curl_easy *data,
                          CURLcode status, bool premature)
{
  ସଂରଚନା FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (ଶୂନ୍ୟ)status;
  (ଶୂନ୍ୟ)premature;

  ଯଦି(file)
    file_cleanup(file);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_stat(ଅଚଳ ଅକ୍ଷର *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = curlx_stat(path, statbuf);

  ଯଦି(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    ଯଦି((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      ଅକ୍ଷର *trimmed = curlx_strdup(path);

      ଯଦି(trimmed) {
        କର {
          trimmed[--pathlen] = '\0';
        } ଯେତେବେଳେ((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
ସ୍ଥିର CURLcode file_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ଅକ୍ଷର *real_path;
  ସଂରଚନା FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  ଅକ୍ଷର *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  ଯଦି(!file)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

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
  ଯଦି((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  ପାଇଁ(i = 0; i < real_path_len; ++i)
    ଯଦି(actual_path[i] == '/')
      actual_path[i] = '\\';
    ଅନ୍ୟଥା ଯଦି(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_ଦ୍ୱିମିକ);
  file->path = actual_path;
#ଅନ୍ୟଥା
  ଯଦି(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
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

  ଯଦି(real_path[0] == '/') {
    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା __unix_path_semantics;
    ଯଦି(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    ଅନ୍ୟଥା ଯଦି(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#ଅନ୍ୟଥା
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  ଯଦି(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  ଯଦି(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ଫେରନ୍ତୁ CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode file_disconnect(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn,
                                bool dead_connection)
{
  (ଶୂନ୍ୟ)dead_connection;
  (ଶୂନ୍ୟ)conn;
  ଫେରନ୍ତୁ file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#ଅନ୍ୟଥା
#define DIRSEP '/'
#endif

ସ୍ଥିର CURLcode file_upload(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା FILEPROTO *file)
{
  ଅଚଳ ଅକ୍ଷର *dir = strchr(file->path, DIRSEP);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode;
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ଅଚଳ ଅକ୍ଷର *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  ଯଦି(!dir)
    ଫେରନ୍ତୁ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ଯଦି(!dir[1])
    ଫେରନ୍ତୁ CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_ଦ୍ୱିମିକ;
  ଯଦି(data->state.resume_from)
    mode |= O_APPEND;
  ଅନ୍ୟଥା
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#ଅନ୍ୟଥା
                  (mode_t)data->set.new_file_perms
#endif
                  );
  ଯଦି(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }

  ଯଦି(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  ଯଦି(data->state.resume_from < 0) {
    ଯଦି(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  ଯଦି(result)
    ଯାଅ out;

  ଯେତେବେଳେ(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(!readcount)
      ଭାଙ୍ଗ;

    nread = readcount;

    /* skip bytes before resume point */
    ଯଦି(data->state.resume_from) {
      ଯଦି((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      ଅନ୍ୟଥା {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    ଅନ୍ୟଥା
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = ଲିଖ(fd, sendbuf, nread);
    ଯଦି(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      ଭାଙ୍ଗ;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  ଯଦି(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  ଫେରନ୍ତୁ result;
}

#ଯଦି defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
ସ୍ଥିର CURLcode win32_file_list(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  ଅକ୍ଷର *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  ଯଦି(!pattern)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  ଯଦି(handle == INVALID_HANDLE_ମୂଲ୍ୟ)
    ଫେରନ୍ତୁ CURLE_READ_ERROR;

  କର {
    ଯଦି(entry.cFileName[0] != TEXT('.')) {
      ଅକ୍ଷର *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      ଯଦି(!name) {
        result = CURLE_OUT_OF_MEMORY;
        ଭାଙ୍ଗ;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      ଯଦି(result)
        ଭାଙ୍ଗ;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }
  } ଯେତେବେଳେ(FindNextFile(handle, &entry));

  error = GetLastError();
  ଯଦି(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode file_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  ସଂରଚନା FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ଅକ୍ଷର *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  ଯଦି(!file)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(data->state.upload)
    ଫେରନ୍ତୁ file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  ଯଦି((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    ଯଦି(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  ଯଦି(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(fstated) {
    time_t filetime;
    ସଂରଚନା tm buffer;
    ଅଚଳ ସଂରଚନା tm *tm = &buffer;
    ଅକ୍ଷର header[80];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା headerlen;
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    ଯଦି(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, ଆକାର(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      ଯଦି(result != CURLE_OK)
        ଫେରନ୍ତୁ result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, ଆକାର(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    ଯଦି(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    ଯଦି(data->req.no_body)
      ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  ଯଦି(data->state.resume_from < 0) {
    ଯଦି(!fstated) {
      failf(data, "cannot get the size of file.");
      ଫେରନ୍ତୁ CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  ଯଦି(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    ଯଦି(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    ଅନ୍ୟଥା {
      failf(data, "failed to resume file:// transfer");
      ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  ଯଦି(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  ଯଦି(!fstated || (expected_size <= 0))
    size_known = FALSE;
  ଅନ୍ୟଥା
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  ଯଦି(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  ଯଦି(data->state.resume_from) {
    ଯଦି(!file->is_dir) {
      ଯଦି(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ଅନ୍ୟଥା {
      ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(!file->is_dir) {
    ଯେତେବେଳେ(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      ଯଦି(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      ଅନ୍ୟଥା
        bytestoread = xfer_blen - 1;

      nread = ପଠ(fd, xfer_buf, bytestoread);

      ଯଦି(nread > 0)
        xfer_buf[nread] = 0;

      ଯଦି(nread <= 0 || (size_known && (expected_size == 0)))
        ଭାଙ୍ଗ;

      ଯଦି(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      ଯଦି(result)
        ଯାଅ out;

      result = Curl_pgrsCheck(data);
      ଯଦି(result)
        ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା {
#ଯଦି defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    ଯଦି(result)
      ଯାଅ out;
#ନଚେତଯଦି defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    ସଂରଚନା dirent *entry;

    ଯଦି(!dir) {
      result = CURLE_READ_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      ଯେତେବେଳେ((entry = readdir(dir))) {
        ଯଦି(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          ଯଦି(result)
            ଭାଙ୍ଗ;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          ଯଦି(result)
            ଭାଙ୍ଗ;
        }
      }
      closedir(dir);
    }
#ଅନ୍ୟଥା
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  ଯଦି(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  ଫେରନ୍ତୁ result;
}

ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_file = {
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
