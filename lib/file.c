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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#অন্তর্ভাব <dirent.h>
#endif

#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "parsedate.h" /* for the week day and month names */
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curl_range.h"

#যদি defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#নতুবা_যদি defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

কাঠামো FILEPROTO {
  অক্ষর *path; /* the path we operate on */
  অক্ষর *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  পূর্ণসংখ্যা fd;     /* open file descriptor to read from! */
  bool is_dir;
};

স্থির শূন্য file_cleanup(কাঠামো FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = ফাঁকা;
  file->is_dir = FALSE;
  যদি(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

স্থির শূন্য file_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো FILEPROTO *file = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  file_cleanup(file);
  curlx_free(file);
}

স্থির CURLcode file_setup_connection(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  কাঠামো FILEPROTO *filep;
  (শূন্য)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, আকার(*filep));
  যদি(filep)
    filep->fd = -1;
  যদি(!filep ||
     Curl_meta_set(তথ্য, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

স্থির CURLcode file_done(কাঠামো Curl_easy *তথ্য,
                          CURLcode status, bool premature)
{
  কাঠামো FILEPROTO *file = Curl_meta_get(তথ্য, CURL_META_FILE_EASY);
  (শূন্য)status;
  (শূন্য)premature;

  যদি(file)
    file_cleanup(file);

  ফেরত CURLE_OK;
}

স্থির পূর্ণসংখ্যা file_stat(ধ্রুবক অক্ষর *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  পূর্ণসংখ্যা ফলাফল = curlx_stat(path, statbuf);

  যদি(ফলাফল) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    যদি((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      অক্ষর *trimmed = curlx_strdup(path);

      যদি(trimmed) {
        করো {
          trimmed[--pathlen] = '\0';
        } যখন((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        ফলাফল = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ফেরত ফলাফল;
#অন্যথা
  ফেরত curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
স্থির CURLcode file_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  অক্ষর *real_path;
  কাঠামো FILEPROTO *file = Curl_meta_get(তথ্য, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  পূর্ণসংখ্যা fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  অক্ষর *actual_path;
#endif
  size_t real_path_len;
  CURLcode ফলাফল;

  যদি(!file)
    ফেরত CURLE_FAILED_INIT;

  যদি(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  ফলাফল = Curl_urldecode(তথ্য->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  যদি(ফলাফল)
    ফেরত ফলাফল;

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
  জন্য(i = 0; i < real_path_len; ++i)
    যদি(actual_path[i] == '/')
      actual_path[i] = '\\';
    অন্যথা যদি(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      ফেরত CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_দ্বিমিক);
  file->path = actual_path;
#অন্যথা
  যদি(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    ফেরত CURLE_URL_MALFORMAT;
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
    বাহ্যিক পূর্ণসংখ্যা __unix_path_semantics;
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
  যদি(!তথ্য->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  যদি(!তথ্য->state.upload && (fd == -1) && !file->is_dir) {
    failf(তথ্য, "Could not open file %s", তথ্য->state.up.path);
    file_done(তথ্য, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ফেরত CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  ফেরত CURLE_OK;
}

স্থির CURLcode file_disconnect(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn,
                                bool dead_connection)
{
  (শূন্য)dead_connection;
  (শূন্য)conn;
  ফেরত file_done(তথ্য, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#অন্যথা
#define DIRSEP '/'
#endif

স্থির CURLcode file_upload(কাঠামো Curl_easy *তথ্য,
                            কাঠামো FILEPROTO *file)
{
  ধ্রুবক অক্ষর *dir = strchr(file->path, DIRSEP);
  পূর্ণসংখ্যা fd;
  পূর্ণসংখ্যা mode;
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ধ্রুবক অক্ষর *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  যদি(!dir)
    ফেরত CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  যদি(!dir[1])
    ফেরত CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_দ্বিমিক;
  যদি(তথ্য->state.resume_from)
    mode |= O_APPEND;
  অন্যথা
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  তথ্য->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#অন্যথা
                  (mode_t)তথ্য->set.new_file_perms
#endif
                  );
  যদি(fd < 0) {
    failf(তথ্য, "cannot open %s for writing", file->path);
    ফেরত CURLE_WRITE_ERROR;
  }

  যদি(তথ্য->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  যদি(তথ্য->state.resume_from < 0) {
    যদি(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(তথ্য, "cannot get the size of %s", file->path);
      ফেরত CURLE_WRITE_ERROR;
    }
    তথ্য->state.resume_from = (curl_off_t)file_stat.st_আকার;
  }

  ফলাফল = Curl_multi_xfer_ulbuf_borrow(তথ্য, &xfer_ulbuf, &xfer_ulblen);
  যদি(ফলাফল)
    যাও out;

  যখন(!ফলাফল && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    ফলাফল = Curl_client_read(তথ্য, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    যদি(ফলাফল)
      বিরতি;

    যদি(!readcount)
      বিরতি;

    nread = readcount;

    /* skip bytes before resume point */
    যদি(তথ্য->state.resume_from) {
      যদি((curl_off_t)nread <= তথ্য->state.resume_from) {
        তথ্য->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      অন্যথা {
        sendbuf = xfer_ulbuf + তথ্য->state.resume_from;
        nread -= (size_t)তথ্য->state.resume_from;
        তথ্য->state.resume_from = 0;
      }
    }
    অন্যথা
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = লিখ(fd, sendbuf, nread);
    যদি(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      ফলাফল = CURLE_SEND_ERROR;
      বিরতি;
    }
    Curl_pgrs_upload_inc(তথ্য, nwritten);

    ফলাফল = Curl_pgrsCheck(তথ্য);
  }
  যদি(!ফলাফল)
    ফলাফল = Curl_pgrsUpdate(তথ্য);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(তথ্য, xfer_ulbuf);

  ফেরত ফলাফল;
}

#যদি defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
স্থির CURLcode win32_file_list(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *path)
{
  WIN32_FIND_তথ্য entry;
  HANDLE handle;
  অক্ষর *pattern;
  size_t pathlen = strlen(path);
  CURLcode ফলাফল = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  যদি(!pattern)
    ফেরত CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  যদি(handle == INVALID_HANDLE_মান)
    ফেরত CURLE_READ_ERROR;

  করো {
    যদি(entry.cFileName[0] != TEXT('.')) {
      অক্ষর *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      যদি(!name) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        বিরতি;
      }

      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      যদি(ফলাফল)
        বিরতি;

      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
      যদি(ফলাফল)
        বিরতি;
    }
  } যখন(Find_পরবর্তী_File(handle, &entry));

  error = GetLastError();
  যদি(!ফলাফল && error != ERROR_NO_MORE_FILES)
    ফলাফল = CURLE_READ_ERROR;

  FindClose(handle);
  ফেরত ফলাফল;
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
স্থির CURLcode file_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  কাঠামো FILEPROTO *file = Curl_meta_get(তথ্য, CURL_META_FILE_EASY);
  CURLcode ফলাফল = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_আকার = -1;
  bool আকার_known;
  bool fstated = FALSE;
  পূর্ণসংখ্যা fd;
  অক্ষর *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  যদি(!file)
    ফেরত CURLE_FAILED_INIT;

  যদি(তথ্য->state.upload)
    ফেরত file_upload(তথ্য, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  যদি((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    যদি(!file->is_dir)
      expected_আকার = statbuf.st_আকার;
    /* and store the modification time */
    তথ্য->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  যদি(fstated && !তথ্য->state.range && তথ্য->set.timecondition &&
     !Curl_meets_timecondition(তথ্য, তথ্য->info.filetime))
    ফেরত CURLE_OK;

  যদি(fstated) {
    time_t filetime;
    কাঠামো tm বাফার;
    ধ্রুবক কাঠামো tm *tm = &বাফার;
    অক্ষর header[80];
    পূর্ণসংখ্যা headerlen;
    স্থির ধ্রুবক অক্ষর accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    যদি(expected_আকার >= 0) {
      headerlen =
        curl_msnprintf(header, আকার(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_আকার);
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, header, headerlen);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      যদি(ফলাফল != CURLE_OK)
        ফেরত ফলাফল;
    }

    filetime = (time_t)statbuf.st_mtime;
    ফলাফল = curlx_gmtime(filetime, &বাফার);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, আকার(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, header, headerlen);
    যদি(!ফলাফল)
      /* end of headers */
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, "\r\n", 2);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    /* set the file size to make it available post transfer */
    Curl_pgrs_Set_Download_আকার(তথ্য, expected_আকার);
    যদি(তথ্য->req.no_body)
      ফেরত CURLE_OK;
  }

  /* Check whether file range has been specified */
  ফলাফল = Curl_range(তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  যদি(তথ্য->state.resume_from < 0) {
    যদি(!fstated) {
      failf(তথ্য, "cannot get the size of file.");
      ফেরত CURLE_READ_ERROR;
    }
    তথ্য->state.resume_from += (curl_off_t)statbuf.st_আকার;
  }

  যদি(তথ্য->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    যদি(তথ্য->state.resume_from <= expected_আকার)
      expected_আকার -= তথ্য->state.resume_from;
    অন্যথা {
      failf(তথ্য, "failed to resume file:// transfer");
      ফেরত CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  যদি(তথ্য->req.maxdownload > 0)
    expected_আকার = তথ্য->req.maxdownload;

  যদি(!fstated || (expected_আকার <= 0))
    আকার_known = FALSE;
  অন্যথা
    আকার_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  যদি(আকার_known)
    Curl_pgrs_Set_Download_আকার(তথ্য, expected_আকার);

  যদি(তথ্য->state.resume_from) {
    যদি(!file->is_dir) {
      যদি(তথ্য->state.resume_from !=
         curl_lseek(fd, তথ্য->state.resume_from, SEEK_SET))
        ফেরত CURLE_BAD_DOWNLOAD_RESUME;
    }
    অন্যথা {
      ফেরত CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  ফলাফল = Curl_multi_xfer_buf_borrow(তথ্য, &xfer_buf, &xfer_blen);
  যদি(ফলাফল)
    যাও out;

  যদি(!file->is_dir) {
    যখন(!ফলাফল) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      যদি(আকার_known) {
        bytestoread = (expected_আকার < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_আকার) : (xfer_blen - 1);
      }
      অন্যথা
        bytestoread = xfer_blen - 1;

      nread = পঠ(fd, xfer_buf, bytestoread);

      যদি(nread > 0)
        xfer_buf[nread] = 0;

      যদি(nread <= 0 || (আকার_known && (expected_আকার == 0)))
        বিরতি;

      যদি(আকার_known)
        expected_আকার -= nread;

      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, xfer_buf, nread);
      যদি(ফলাফল)
        যাও out;

      ফলাফল = Curl_pgrsCheck(তথ্য);
      যদি(ফলাফল)
        যাও out;
    }
  }
  অন্যথা {
#যদি defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    ফলাফল = win32_file_list(তথ্য, file->path);
    যদি(ফলাফল)
      যাও out;
#নতুবা_যদি defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    কাঠামো dirent *entry;

    যদি(!dir) {
      ফলাফল = CURLE_READ_ERROR;
      যাও out;
    }
    অন্যথা {
      যখন((entry = readdir(dir))) {
        যদি(entry->d_name[0] != '.') {
          ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          যদি(ফলাফল)
            বিরতি;
          ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
          যদি(ফলাফল)
            বিরতি;
        }
      }
      closedir(dir);
    }
#অন্যথা
    failf(তথ্য, "Directory listing not yet implemented on this platform.");
    ফলাফল = CURLE_READ_ERROR;
#endif
  }

  যদি(!ফলাফল)
    ফলাফল = Curl_pgrsUpdate(তথ্য);

out:
  Curl_multi_xfer_buf_release(তথ্য, xfer_buf);
  ফেরত ফলাফল;
}

ধ্রুবক কাঠামো Curl_protocol Curl_protocol_file = {
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
