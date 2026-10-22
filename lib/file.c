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
#include "curl_setup.h"
#include "urldata.h"
#include "file.h"

#ifndef CURL_DISABLE_FILE

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#include "progress.h"
#include "sendf.h"
#include "curl_trc.h"
#include "escape.h"
#include "multiif.h"
#include "transfer.h"
#include "url.h"
#include "parsedate.h" /* for the week day and month names */
#include "curlx/win32-fopen.h"
#include "curl_range.h"

#གལ་ཏེ defined(_WIN32) || defined(MSDOS)
#define DOS_FILESYSTEM 1
#elif defined(__amigaos4__)
#define AMIGA_FILESYSTEM 1
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_FILE_EASY   "meta:proto:file:easy"

sa_nracanaa FILEPROTO {
  var_na *path; /* the path we operate on */
  var_na *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  ཧྲིལ fd;     /* open file descriptor to read from! */
  bool is_dir;
};

static void file_cleanup(sa_nracanaa FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->path = NULL;
  file->is_dir = FALSE;
  གལ་ཏེ(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

static void file_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa FILEPROTO *file = entry;
  (void)key;
  (void)klen;
  file_cleanup(file);
  curlx_free(file);
}

static CURLcode file_setup_connection(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa FILEPROTO *filep;
  (void)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, parimaa_na(*filep));
  གལ་ཏེ(filep)
    filep->fd = -1;
  གལ་ཏེ(!filep ||
     Curl_meta_set(data, CURL_META_FILE_EASY, filep, file_easy_dtor))
    ཕྱིར CURLE_OUT_OF_MEMORY;

  ཕྱིར CURLE_OK;
}

static CURLcode file_done(sa_nracanaa Curl_easy *data,
                          CURLcode status, bool premature)
{
  sa_nracanaa FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  (void)status;
  (void)premature;

  གལ་ཏེ(file)
    file_cleanup(file);

  ཕྱིར CURLE_OK;
}

static ཧྲིལ file_stat(const var_na *path, curlx_struct_stat *statbuf)
{
#ifdef _WIN32
  ཧྲིལ result = curlx_stat(path, statbuf);

  གལ་ཏེ(result) {
    size_t pathlen = strlen(path);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    གལ་ཏེ((pathlen > 3) &&
       ((path[pathlen - 1] == '\\') || (path[pathlen - 1] == '/'))) {
      var_na *trimmed = curlx_strdup(path);

      གལ་ཏེ(trimmed) {
        do {
          trimmed[--pathlen] = '\0';
        } སྐབས((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        result = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  ཕྱིར result;
#དེ་མིན
  ཕྱིར curlx_stat(path, statbuf);
#endif
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
static CURLcode file_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  var_na *real_path;
  sa_nracanaa FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
#ifdef _WIN32
  curlx_struct_stat statbuf;
#endif
  ཧྲིལ fd;
#ifdef DOS_FILESYSTEM
  size_t i;
  var_na *actual_path;
#endif
  size_t real_path_len;
  CURLcode result;

  གལ་ཏེ(!file)
    ཕྱིར CURLE_FAILED_INIT;

  གལ་ཏེ(file->path) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *done = TRUE;
    ཕྱིར CURLE_OK;
  }

  result = Curl_urldecode(data->state.up.path, 0, &real_path,
                          &real_path_len, REJECT_ZERO);
  གལ་ཏེ(result)
    ཕྱིར result;

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
  གལ་ཏེ((actual_path[0] == '/') &&
      actual_path[1] &&
     (actual_path[2] == ':' || actual_path[2] == '|')) {
    actual_path[2] = ':';
    actual_path++;
    real_path_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  ཆེད་དུ(i = 0; i < real_path_len; ++i)
    གལ་ཏེ(actual_path[i] == '/')
      actual_path[i] = '\\';
    དེ་མིན གལ་ཏེ(!actual_path[i]) { /* binary zero */
      curlx_safefree(real_path);
      ཕྱིར CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_path, O_RDONLY | CURL_O_BINARY);
  file->path = actual_path;
#དེ་མིན
  གལ་ཏེ(memchr(real_path, 0, real_path_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_path);
    ཕྱིར CURLE_URL_MALFORMAT;
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

  གལ་ཏེ(real_path[0] == '/') {
    extern ཧྲིལ __unix_path_semantics;
    གལ་ཏེ(strchr(real_path + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_path + 1, O_RDONLY);
      file->path++;
    }
    དེ་མིན གལ་ཏེ(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_path, O_RDONLY);
    }
  }
#དེ་མིན
  fd = curlx_open(real_path, O_RDONLY);
  file->path = real_path;
#endif
#endif
  curlx_free(file->freepath);
  file->freepath = real_path; /* free this when done */

  file->fd = fd;
#ifdef _WIN32
  གལ་ཏེ(!data->state.upload && (fd == -1) &&
     !file_stat(file->path, &statbuf) && S_ISDIR(statbuf.st_mode))
    file->is_dir = TRUE;
#endif

  གལ་ཏེ(!data->state.upload && (fd == -1) && !file->is_dir) {
    failf(data, "Could not open file %s", data->state.up.path);
    file_done(data, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    ཕྱིར CURLE_FILE_COULDNT_READ_FILE;
  }
  *done = TRUE;

  ཕྱིར CURLE_OK;
}

static CURLcode file_disconnect(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn,
                                bool dead_connection)
{
  (void)dead_connection;
  (void)conn;
  ཕྱིར file_done(data, CURLE_OK, FALSE);
}

#ifdef DOS_FILESYSTEM
#define DIRSEP '\\'
#དེ་མིན
#define DIRSEP '/'
#endif

static CURLcode file_upload(sa_nracanaa Curl_easy *data,
                            sa_nracanaa FILEPROTO *file)
{
  const var_na *dir = strchr(file->path, DIRSEP);
  ཧྲིལ fd;
  ཧྲིལ mode;
  CURLcode result = CURLE_OK;
  var_na *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  const var_na *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  གལ་ཏེ(!dir)
    ཕྱིར CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  གལ་ཏེ(!dir[1])
    ཕྱིར CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  mode = O_WRONLY | O_CREAT | CURL_O_BINARY;
  གལ་ཏེ(data->state.resume_from)
    mode |= O_APPEND;
  དེ་མིན
    mode |= O_TRUNC;

  fd = curlx_open(file->path, mode,
#ifdef _WIN32
                  data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#དེ་མིན
                  (mode_t)data->set.new_file_perms
#endif
                  );
  གལ་ཏེ(fd < 0) {
    failf(data, "cannot open %s for writing", file->path);
    ཕྱིར CURLE_WRITE_ERROR;
  }

  གལ་ཏེ(data->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  གལ་ཏེ(data->state.resume_from < 0) {
    གལ་ཏེ(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(data, "cannot get the size of %s", file->path);
      ཕྱིར CURLE_WRITE_ERROR;
    }
    data->state.resume_from = (curl_off_t)file_stat.st_size;
  }

  result = Curl_multi_xfer_ulbuf_borrow(data, &xfer_ulbuf, &xfer_ulblen);
  གལ་ཏེ(result)
    goto out;

  སྐབས(!result && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    result = Curl_client_read(data, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    གལ་ཏེ(result)
      bha_nga;

    གལ་ཏེ(!readcount)
      bha_nga;

    nread = readcount;

    /* skip bytes before resume point */
    གལ་ཏེ(data->state.resume_from) {
      གལ་ཏེ((curl_off_t)nread <= data->state.resume_from) {
        data->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      དེ་མིན {
        sendbuf = xfer_ulbuf + data->state.resume_from;
        nread -= (size_t)data->state.resume_from;
        data->state.resume_from = 0;
      }
    }
    དེ་མིན
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = write(fd, sendbuf, nread);
    གལ་ཏེ(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      result = CURLE_SEND_ERROR;
      bha_nga;
    }
    Curl_pgrs_upload_inc(data, nwritten);

    result = Curl_pgrsCheck(data);
  }
  གལ་ཏེ(!result)
    result = Curl_pgrsUpdate(data);

out:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(data, xfer_ulbuf);

  ཕྱིར result;
}

#གལ་ཏེ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
static CURLcode win32_file_list(sa_nracanaa Curl_easy *data, const var_na *path)
{
  WIN32_FIND_DATA entry;
  HANDLE handle;
  var_na *pattern;
  size_t pathlen = strlen(path);
  CURLcode result = CURLE_OK;
  DWORD error;

  pattern = curl_maprintf("%s%s*", path,
                          pathlen &&
                          (path[pathlen - 1] == '\\' ||
                           path[pathlen - 1] == '/') ? "" : "\\");
  གལ་ཏེ(!pattern)
    ཕྱིར CURLE_OUT_OF_MEMORY;

  handle = curlx_FindFirstFile(pattern, &entry);
  curlx_free(pattern);
  གལ་ཏེ(handle == INVALID_HANDLE_VALUE)
    ཕྱིར CURLE_READ_ERROR;

  do {
    གལ་ཏེ(entry.cFileName[0] != TEXT('.')) {
      var_na *name = curlx_convert_tchar_to_UTF8(entry.cFileName);

      གལ་ཏེ(!name) {
        result = CURLE_OUT_OF_MEMORY;
        bha_nga;
      }

      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 name, strlen(name));
      curlx_free(name);
      གལ་ཏེ(result)
        bha_nga;

      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      གལ་ཏེ(result)
        bha_nga;
    }
  } སྐབས(FindNextFile(handle, &entry));

  error = GetLastError();
  གལ་ཏེ(!result && error != ERROR_NO_MORE_FILES)
    result = CURLE_READ_ERROR;

  FindClose(handle);
  ཕྱིར result;
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
static CURLcode file_do(sa_nracanaa Curl_easy *data, bool *done)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  sa_nracanaa FILEPROTO *file = Curl_meta_get(data, CURL_META_FILE_EASY);
  CURLcode result = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_size = -1;
  bool size_known;
  bool fstated = FALSE;
  ཧྲིལ fd;
  var_na *xfer_buf;
  size_t xfer_blen;

  *done = TRUE; /* unconditionally */
  གལ་ཏེ(!file)
    ཕྱིར CURLE_FAILED_INIT;

  གལ་ཏེ(data->state.upload)
    ཕྱིར file_upload(data, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  གལ་ཏེ((file->is_dir ? file_stat(file->path, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_mode);
    གལ་ཏེ(!file->is_dir)
      expected_size = statbuf.st_size;
    /* and store the modification time */
    data->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  གལ་ཏེ(fstated && !data->state.range && data->set.timecondition &&
     !Curl_meets_timecondition(data, data->info.filetime))
    ཕྱིར CURLE_OK;

  གལ་ཏེ(fstated) {
    time_t filetime;
    sa_nracanaa tm buffer;
    const sa_nracanaa tm *tm = &buffer;
    var_na header[80];
    ཧྲིལ headerlen;
    static const var_na accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    གལ་ཏེ(expected_size >= 0) {
      headerlen =
        curl_msnprintf(header, parimaa_na(header),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_size);
      result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
      གལ་ཏེ(result)
        ཕྱིར result;

      result = Curl_client_write(data, CLIENTWRITE_HEADER,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      གལ་ཏེ(result != CURLE_OK)
        ཕྱིར result;
    }

    filetime = (time_t)statbuf.st_mtime;
    result = curlx_gmtime(filetime, &buffer);
    གལ་ཏེ(result)
      ཕྱིར result;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(header, parimaa_na(header),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    result = Curl_client_write(data, CLIENTWRITE_HEADER, header, headerlen);
    གལ་ཏེ(!result)
      /* end of headers */
      result = Curl_client_write(data, CLIENTWRITE_HEADER, "\r\n", 2);
    གལ་ཏེ(result)
      ཕྱིར result;
    /* set the file size to make it available post transfer */
    Curl_pgrsSetDownloadSize(data, expected_size);
    གལ་ཏེ(data->req.no_body)
      ཕྱིར CURLE_OK;
  }

  /* Check whether file range has been specified */
  result = Curl_range(data);
  གལ་ཏེ(result)
    ཕྱིར result;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  གལ་ཏེ(data->state.resume_from < 0) {
    གལ་ཏེ(!fstated) {
      failf(data, "cannot get the size of file.");
      ཕྱིར CURLE_READ_ERROR;
    }
    data->state.resume_from += (curl_off_t)statbuf.st_size;
  }

  གལ་ཏེ(data->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    གལ་ཏེ(data->state.resume_from <= expected_size)
      expected_size -= data->state.resume_from;
    དེ་མིན {
      failf(data, "failed to resume file:// transfer");
      ཕྱིར CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  གལ་ཏེ(data->req.maxdownload > 0)
    expected_size = data->req.maxdownload;

  གལ་ཏེ(!fstated || (expected_size <= 0))
    size_known = FALSE;
  དེ་མིན
    size_known = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  གལ་ཏེ(size_known)
    Curl_pgrsSetDownloadSize(data, expected_size);

  གལ་ཏེ(data->state.resume_from) {
    གལ་ཏེ(!file->is_dir) {
      གལ་ཏེ(data->state.resume_from !=
         curl_lseek(fd, data->state.resume_from, SEEK_SET))
        ཕྱིར CURLE_BAD_DOWNLOAD_RESUME;
    }
    དེ་མིན {
      ཕྱིར CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  result = Curl_multi_xfer_buf_borrow(data, &xfer_buf, &xfer_blen);
  གལ་ཏེ(result)
    goto out;

  གལ་ཏེ(!file->is_dir) {
    སྐབས(!result) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      གལ་ཏེ(size_known) {
        bytestoread = (expected_size < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_size) : (xfer_blen - 1);
      }
      དེ་མིན
        bytestoread = xfer_blen - 1;

      nread = read(fd, xfer_buf, bytestoread);

      གལ་ཏེ(nread > 0)
        xfer_buf[nread] = 0;

      གལ་ཏེ(nread <= 0 || (size_known && (expected_size == 0)))
        bha_nga;

      གལ་ཏེ(size_known)
        expected_size -= nread;

      result = Curl_client_write(data, CLIENTWRITE_BODY, xfer_buf, nread);
      གལ་ཏེ(result)
        goto out;

      result = Curl_pgrsCheck(data);
      གལ་ཏེ(result)
        goto out;
    }
  }
  དེ་མིན {
#གལ་ཏེ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    result = win32_file_list(data, file->path);
    གལ་ཏེ(result)
      goto out;
#elif defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->path);
    sa_nracanaa dirent *entry;

    གལ་ཏེ(!dir) {
      result = CURLE_READ_ERROR;
      goto out;
    }
    དེ་མིན {
      སྐབས((entry = readdir(dir))) {
        གལ་ཏེ(entry->d_name[0] != '.') {
          result = Curl_client_write(data, CLIENTWRITE_BODY,
                   entry->d_name, strlen(entry->d_name));
          གལ་ཏེ(result)
            bha_nga;
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
          གལ་ཏེ(result)
            bha_nga;
        }
      }
      closedir(dir);
    }
#དེ་མིན
    failf(data, "Directory listing not yet implemented on this platform.");
    result = CURLE_READ_ERROR;
#endif
  }

  གལ་ཏེ(!result)
    result = Curl_pgrsUpdate(data);

out:
  Curl_multi_xfer_buf_release(data, xfer_buf);
  ཕྱིར result;
}

const sa_nracanaa Curl_protocol Curl_protocol_file = {
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
