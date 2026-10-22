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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#অন্তর্ভাৱ <dirent.h>
#endif

#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "parsedate.h" /* for the week day and month names */
#অন্তর্ভাৱ "curlx/win32-fopen.h"
#অন্তর্ভাৱ "curl_range.h"

#যদি defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#নোচেদ্যদি defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

সংরচনা FILEPROTO {
  ৱর্ণ *path; /* the path we operate on */
  ৱর্ণ *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  পূর্ণাঙ্ক fd;     /* open file descriptor to read from! */
  bool is_dir;
};

স্থির শূন্য file_cleanup(সংরচনা FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = রিক্ত;
  file->is_dir = FALSE;
  যদি(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

স্থির শূন্য file_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা FILEPROTO *file = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  file_cleanup(file);
  curlx_free(file);
}

স্থির CURLcode file_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা FILEPROTO *filep;
  (শূন্য)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, পরিমাণ(*filep));
  যদি(filep)
    filep->fd = -1;
  যদি(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode file_done(সংরচনা Curl_easy *data,
                          CURLcode status, bool premature)
{
  সংরচনা FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (শূন্য)status;
  (শূন্য)premature;

  যদি(file)
    file_cleanup(file);

  ঘূৰাই CURLE_OK;
}

স্থির পূর্ণাঙ্ক file_stat(ধ্রুৱ ৱর্ণ *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  পূর্ণাঙ্ক result = curlx_stat(path, statbuf);

  যদি(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    যদি((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      ৱর্ণ *trimmed = curlx_strdup(path);

      যদি(trimmed) {
        কৰা {
          trimmed[--pathlen] = '\0';
        } যেতিয়ালৈ((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ঘূৰাই result;
#অন্যথা
  ঘূৰাই curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
স্থির CURLcode file_connect(সংরচনা Curl_easy *data, bool *done)
{
  ৱর্ণ *real_path;
  সংরচনা FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  পূর্ণাঙ্ক fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  ৱর্ণ *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  যদি(!file)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  যদি(result)
    ঘূৰাই result;

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
  যদি((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  বাবে(i = 0; i < real_path_len; ++i)
    যদি(actual_path[i] == '/')
      actual_path[i] = '\\';
    অন্যথা যদি(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      ঘূৰাই CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_দ্বিমিক);
  file->path = actual_path;
#অন্যথা
  যদি(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    ঘূৰাই CURLE_URL_MALFORMAT;
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

  যদি(real_path[0] == '/') {
    বাহ্য পূর্ণাঙ্ক __unix_path_semantics;
    যদি(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    অন্যথা যদি(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#অন্যথা
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  যদি(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  যদি(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ঘূৰাই CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode file_disconnect(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn,
                                bool dead_connection)
{
  (শূন্য)dead_connection;
  (শূন্য)conn;
  ঘূৰাই file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#অন্যথা
#define DIRSEP '/'
#endif

স্থির CURLcode file_upload(সংরচনা Curl_easy *data,
                            সংরচনা FILEPROTO *file)
{
  ধ্রুৱ ৱর্ণ *dir = strchr(file->path, DIRSEP);
  পূর্ণাঙ্ক fd;
  পূর্ণাঙ্ক mode;
  CURLcode result = CURLE_OK;
  ৱর্ণ *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ধ্রুৱ ৱর্ণ *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  যদি(!dir)
    ঘূৰাই CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  যদি(!dir[1])
    ঘূৰাই CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_দ্বিমিক;
  যদি(data->state.resume_from)
    mode |= O_APPEND;
  অন্যথা
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#অন্যথা
                  (mode_t)data->set.new_file_perms
#endif
                  );
  যদি(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    ঘূৰাই CURLE_WRITE_ERROR;
  }

  যদি(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  যদি(data->state.resume_from < 0) {
    যদি(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      ঘূৰাই CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  যদি(result)
    গচ্ছ out;

  যেতিয়ালৈ(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    যদি(result)
      ভঙ্গ;

    যদি(!readcount)
      ভঙ্গ;

    nread = readcount;

    /* skip bytes before resume point */
    যদি(data->state.resume_from) {
      যদি((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      অন্যথা {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    অন্যথা
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = লিখ(fd, sendbuf, nread);
    যদি(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      ভঙ্গ;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  যদি(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  ঘূৰাই result;
}

#যদি defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
স্থির CURLcode win32_file_list(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  ৱর্ণ *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  যদি(!pattern)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  যদি(handle == INVALID_HANDLE_মান)
    ঘূৰাই CURLE_READ_ERROR;

  কৰা {
    যদি(entry.cFileName[0] != TEXT('.')) {
      ৱর্ণ *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      যদি(!name) {
        result = CURLE_OUT_OF_MEMORY;
        ভঙ্গ;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      যদি(result)
        ভঙ্গ;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      যদি(result)
        ভঙ্গ;
    }
  } যেতিয়ালৈ(FindNextFile(handle, &entry));

  error = GetLastError();
  যদি(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  ঘূৰাই result;
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
স্থির CURLcode file_do(সংরচনা Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  সংরচনা FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  পূর্ণাঙ্ক fd;
  ৱর্ণ *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  যদি(!file)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(data->state.upload)
    ঘূৰাই file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  যদি((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    যদি(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  যদি(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    ঘূৰাই CURLE_OK;

  যদি(fstated) {
    time_t filetime;
    সংরচনা tm buffer;
    ধ্রুৱ সংরচনা tm *tm = &buffer;
    ৱর্ণ header[80];
    পূর্ণাঙ্ক headerlen;
    স্থির ধ্রুৱ ৱর্ণ accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    যদি(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, পরিমাণ(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      যদি(result)
        ঘূৰাই result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      যদি(result != CURLE_OK)
        ঘূৰাই result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    যদি(result)
      ঘূৰাই result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, পরিমাণ(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    যদি(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    যদি(result)
      ঘূৰাই result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    যদি(data->req.no_body)
      ঘূৰাই CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  যদি(result)
    ঘূৰাই result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  যদি(data->state.resume_from < 0) {
    যদি(!fstated) {
      failf(data, "cannot get the size of file.");
      ঘূৰাই CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  যদি(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    যদি(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    অন্যথা {
      failf(data, "failed to resume file:// transfer");
      ঘূৰাই CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  যদি(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  যদি(!fstated || (expected_size <= 0))
    size_known = FALSE;
  অন্যথা
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  যদি(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  যদি(data->state.resume_from) {
    যদি(!file->is_dir) {
      যদি(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        ঘূৰাই CURLE_BAD_DOWNLOAD_RESUME;
    }
    অন্যথা {
      ঘূৰাই CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  যদি(result)
    গচ্ছ out;

  যদি(!file->is_dir) {
    যেতিয়ালৈ(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      যদি(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      অন্যথা
        bytestoread = xfer_blen - 1;

      nread = পঠ(fd, xfer_buf, bytestoread);

      যদি(nread > 0)
        xfer_buf[nread] = 0;

      যদি(nread <= 0 || (size_known && (expected_size == 0)))
        ভঙ্গ;

      যদি(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      যদি(result)
        গচ্ছ out;

      result = Curl_pgrsCheck(data);
      যদি(result)
        গচ্ছ out;
    }
  }
  অন্যথা {
#যদি defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    যদি(result)
      গচ্ছ out;
#নোচেদ্যদি defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    সংরচনা dirent *entry;

    যদি(!dir) {
      result = CURLE_READ_ERROR;
      গচ্ছ out;
    }
    অন্যথা {
      যেতিয়ালৈ((entry = readdir(dir))) {
        যদি(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          যদি(result)
            ভঙ্গ;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          যদি(result)
            ভঙ্গ;
        }
      }
      closedir(dir);
    }
#অন্যথা
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  যদি(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  ঘূৰাই result;
}

ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_file = {
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
