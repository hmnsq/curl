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

#यदि_अपरिभाषितम् CURL_DISABLE_FILE

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_DIRENT_H
#अन्तर्भाव <dirent.h>
#यदि_अन्तः

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

#यदि defined(_WIN32) || defined(MSDOS)
#परिभाषय DOS_FILESYSTEM 1
#नोचेद्यदि defined(__amigaos4__)
#परिभाषय AMIGA_FILESYSTEM 1
#यदि_अन्तः

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_FILE_EASY   "meta:proto:file:easy"

संरचना FILEPROTO {
  वर्ण *मार्गः; /* the path we operate on */
  वर्ण *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  पूर्णाङ्क fd;     /* open file descriptor to read from! */
  bool is_dir;
};

स्थिर शून्य file_cleanup(संरचना FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->मार्गः = रिक्त;
  file->is_dir = FALSE;
  यदि(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

स्थिर शून्य file_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना FILEPROTO *file = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  file_cleanup(file);
  curlx_free(file);
}

स्थिर CURLcode file_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना FILEPROTO *filep;
  (शून्य)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, परिमाण(*filep));
  यदि(filep)
    filep->fd = -1;
  यदि(!filep ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_FILE_EASY, filep, file_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode file_समाप्तम्(संरचना Curl_easy *दत्त,
                          CURLcode स्थितिः, bool premature)
{
  संरचना FILEPROTO *file = Curl_meta_get(दत्त, CURL_META_FILE_EASY);
  (शून्य)स्थितिः;
  (शून्य)premature;

  यदि(file)
    file_cleanup(file);

  प्रत्यागम CURLE_OK;
}

स्थिर पूर्णाङ्क file_stat(ध्रुव वर्ण *मार्गः, curlx_struct_stat *statbuf)
{
#यदि_परिभाषितम् _WIN32
  पूर्णाङ्क परिणाम = curlx_stat(मार्गः, statbuf);

  यदि(परिणाम) {
    size_t pathlen = वाक्यदैर्घ्यम्(मार्गः);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    यदि((pathlen > 3) &&
       ((मार्गः[pathlen - 1] == '\\') || (मार्गः[pathlen - 1] == '/'))) {
      वर्ण *trimmed = curlx_strdup(मार्गः);

      यदि(trimmed) {
        कुरु {
          trimmed[--pathlen] = '\0';
        } यावत्((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        परिणाम = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  प्रत्यागम परिणाम;
#अन्यथा
  प्रत्यागम curlx_stat(मार्गः, statbuf);
#यदि_अन्तः
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
स्थिर CURLcode file_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  वर्ण *real_मार्गः;
  संरचना FILEPROTO *file = Curl_meta_get(दत्त, CURL_META_FILE_EASY);
#यदि_परिभाषितम् _WIN32
  curlx_struct_stat statbuf;
#यदि_अन्तः
  पूर्णाङ्क fd;
#यदि_परिभाषितम् DOS_FILESYSTEM
  size_t i;
  वर्ण *actual_मार्गः;
#यदि_अन्तः
  size_t real_मार्गः_len;
  CURLcode परिणाम;

  यदि(!file)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(file->मार्गः) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  परिणाम = Curl_urldecode(दत्त->state.ऊर्ध्वम्.मार्गः, 0, &real_मार्गः,
                          &real_मार्गः_len, REJECT_ZERO);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि_परिभाषितम् DOS_FILESYSTEM
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
  actual_मार्गः = real_मार्गः;
  यदि((actual_मार्गः[0] == '/') &&
      actual_मार्गः[1] &&
     (actual_मार्गः[2] == ':' || actual_मार्गः[2] == '|')) {
    actual_मार्गः[2] = ':';
    actual_मार्गः++;
    real_मार्गः_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  प्रति(i = 0; i < real_मार्गः_len; ++i)
    यदि(actual_मार्गः[i] == '/')
      actual_मार्गः[i] = '\\';
    अन्यथा यदि(!actual_मार्गः[i]) { /* binary zero */
      curlx_safefree(real_मार्गः);
      प्रत्यागम CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_मार्गः, O_RDONLY | CURL_O_द्विमान);
  file->मार्गः = actual_मार्गः;
#अन्यथा
  यदि(memchr(real_मार्गः, 0, real_मार्गः_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_मार्गः);
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

#यदि_परिभाषितम् AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->मार्गः = real_मार्गः;

  यदि(real_मार्गः[0] == '/') {
    बाह्य पूर्णाङ्क __unix_path_semantics;
    यदि(strchr(real_मार्गः + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_मार्गः + 1, O_RDONLY);
      file->मार्गः++;
    }
    अन्यथा यदि(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_मार्गः, O_RDONLY);
    }
  }
#अन्यथा
  fd = curlx_open(real_मार्गः, O_RDONLY);
  file->मार्गः = real_मार्गः;
#यदि_अन्तः
#यदि_अन्तः
  curlx_free(file->freepath);
  file->freepath = real_मार्गः; /* free this when done */

  file->fd = fd;
#यदि_परिभाषितम् _WIN32
  यदि(!दत्त->state.upload && (fd == -1) &&
     !file_stat(file->मार्गः, &statbuf) && S_ISDIR(statbuf.st_बहुलकम्))
    file->is_dir = TRUE;
#यदि_अन्तः

  यदि(!दत्त->state.upload && (fd == -1) && !file->is_dir) {
    failf(दत्त, "Could not open file %s", दत्त->state.ऊर्ध्वम्.मार्गः);
    file_समाप्तम्(दत्त, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    प्रत्यागम CURLE_FILE_COULDNT_READ_FILE;
  }
  *समाप्तम् = TRUE;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode file_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  (शून्य)dead_connection;
  (शून्य)conn;
  प्रत्यागम file_समाप्तम्(दत्त, CURLE_OK, FALSE);
}

#यदि_परिभाषितम् DOS_FILESYSTEM
#परिभाषय DIRSEP '\\'
#अन्यथा
#परिभाषय DIRSEP '/'
#यदि_अन्तः

स्थिर CURLcode file_upload(संरचना Curl_easy *दत्त,
                            संरचना FILEPROTO *file)
{
  ध्रुव वर्ण *dir = strchr(file->मार्गः, DIRSEP);
  पूर्णाङ्क fd;
  पूर्णाङ्क बहुलकम्;
  CURLcode परिणाम = CURLE_OK;
  वर्ण *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  ध्रुव वर्ण *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  यदि(!dir)
    प्रत्यागम CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  यदि(!dir[1])
    प्रत्यागम CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  बहुलकम् = O_WRONLY | O_CREAT | CURL_O_द्विमान;
  यदि(दत्त->state.resume_from)
    बहुलकम् |= O_APPEND;
  अन्यथा
    बहुलकम् |= O_TRUNC;

  fd = curlx_open(file->मार्गः, बहुलकम्,
#यदि_परिभाषितम् _WIN32
                  दत्त->समुच्चयः.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                  (mode_t)दत्त->समुच्चयः.new_file_perms
#यदि_अन्तः
                  );
  यदि(fd < 0) {
    failf(दत्त, "cannot open %s for writing", file->मार्गः);
    प्रत्यागम CURLE_WRITE_ERROR;
  }

  यदि(दत्त->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  यदि(दत्त->state.resume_from < 0) {
    यदि(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(दत्त, "cannot get the size of %s", file->मार्गः);
      प्रत्यागम CURLE_WRITE_ERROR;
    }
    दत्त->state.resume_from = (curl_off_t)file_stat.st_आकार;
  }

  परिणाम = Curl_multi_xfer_ulbuf_borrow(दत्त, &xfer_ulbuf, &xfer_ulblen);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यावत्(!परिणाम && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    परिणाम = Curl_client_read(दत्त, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    यदि(परिणाम)
      भङ्ग;

    यदि(!readcount)
      भङ्ग;

    nread = readcount;

    /* skip bytes before resume point */
    यदि(दत्त->state.resume_from) {
      यदि((curl_off_t)nread <= दत्त->state.resume_from) {
        दत्त->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      अन्यथा {
        sendbuf = xfer_ulbuf + दत्त->state.resume_from;
        nread -= (size_t)दत्त->state.resume_from;
        दत्त->state.resume_from = 0;
      }
    }
    अन्यथा
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = लिख(fd, sendbuf, nread);
    यदि(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      परिणाम = CURLE_SEND_ERROR;
      भङ्ग;
    }
    Curl_pgrs_upload_inc(दत्त, nwritten);

    परिणाम = Curl_pgrsCheck(दत्त);
  }
  यदि(!परिणाम)
    परिणाम = Curl_pgrsUpdate(दत्त);

बहिर्गमनम्:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(दत्त, xfer_ulbuf);

  प्रत्यागम परिणाम;
}

#यदि defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
स्थिर CURLcode win32_file_list(संरचना Curl_easy *दत्त, ध्रुव वर्ण *मार्गः)
{
  WIN32_FIND_दत्त प्रविष्टिः;
  HANDLE handle;
  वर्ण *आदर्शः;
  size_t pathlen = वाक्यदैर्घ्यम्(मार्गः);
  CURLcode परिणाम = CURLE_OK;
  DWORD error;

  आदर्शः = curl_maprintf("%s%s*", मार्गः,
                          pathlen &&
                          (मार्गः[pathlen - 1] == '\\' ||
                           मार्गः[pathlen - 1] == '/') ? "" : "\\");
  यदि(!आदर्शः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  handle = curlx_FindFirstFile(आदर्शः, &प्रविष्टिः);
  curlx_free(आदर्शः);
  यदि(handle == INVALID_HANDLE_मानम्)
    प्रत्यागम CURLE_READ_ERROR;

  कुरु {
    यदि(प्रविष्टिः.cFileName[0] != TEXT('.')) {
      वर्ण *name = curlx_convert_tchar_गन्तव्यम्_UTF8(प्रविष्टिः.cFileName);

      यदि(!name) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        भङ्ग;
      }

      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                 name, वाक्यदैर्घ्यम्(name));
      curlx_free(name);
      यदि(परिणाम)
        भङ्ग;

      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
      यदि(परिणाम)
        भङ्ग;
    }
  } यावत्(Find_अग्र_File(handle, &प्रविष्टिः));

  error = Get_चरमः_Error();
  यदि(!परिणाम && error != ERROR_NO_अधिकम्_FILES)
    परिणाम = CURLE_READ_ERROR;

  FindClose(handle);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
स्थिर CURLcode file_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  संरचना FILEPROTO *file = Curl_meta_get(दत्त, CURL_META_FILE_EASY);
  CURLcode परिणाम = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_आकार = -1;
  bool आकार_ज्ञातम्;
  bool fstated = FALSE;
  पूर्णाङ्क fd;
  वर्ण *xfer_buf;
  size_t xfer_blen;

  *समाप्तम् = TRUE; /* unconditionally */
  यदि(!file)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(दत्त->state.upload)
    प्रत्यागम file_upload(दत्त, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  यदि((file->is_dir ? file_stat(file->मार्गः, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_बहुलकम्);
    यदि(!file->is_dir)
      expected_आकार = statbuf.st_आकार;
    /* and store the modification time */
    दत्त->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  यदि(fstated && !दत्त->state.range && दत्त->समुच्चयः.timecondition &&
     !Curl_meets_timecondition(दत्त, दत्त->info.filetime))
    प्रत्यागम CURLE_OK;

  यदि(fstated) {
    time_t filetime;
    संरचना tm बफर;
    ध्रुव संरचना tm *tm = &बफर;
    वर्ण शीर्षकम्[80];
    पूर्णाङ्क headerlen;
    स्थिर ध्रुव वर्ण accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    यदि(expected_आकार >= 0) {
      headerlen =
        curl_msnprintf(शीर्षकम्, परिमाण(शीर्षकम्),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_आकार);
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, शीर्षकम्, headerlen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      यदि(परिणाम != CURLE_OK)
        प्रत्यागम परिणाम;
    }

    filetime = (time_t)statbuf.st_mtime;
    परिणाम = curlx_gmtime(filetime, &बफर);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(शीर्षकम्, परिमाण(शीर्षकम्),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, शीर्षकम्, headerlen);
    यदि(!परिणाम)
      /* end of headers */
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, "\r\n", 2);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    /* set the file size to make it available post transfer */
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, expected_आकार);
    यदि(दत्त->req.no_body)
      प्रत्यागम CURLE_OK;
  }

  /* Check whether file range has been specified */
  परिणाम = Curl_range(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  यदि(दत्त->state.resume_from < 0) {
    यदि(!fstated) {
      failf(दत्त, "cannot get the size of file.");
      प्रत्यागम CURLE_READ_ERROR;
    }
    दत्त->state.resume_from += (curl_off_t)statbuf.st_आकार;
  }

  यदि(दत्त->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    यदि(दत्त->state.resume_from <= expected_आकार)
      expected_आकार -= दत्त->state.resume_from;
    अन्यथा {
      failf(दत्त, "failed to resume file:// transfer");
      प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  यदि(दत्त->req.maxdownload > 0)
    expected_आकार = दत्त->req.maxdownload;

  यदि(!fstated || (expected_आकार <= 0))
    आकार_ज्ञातम् = FALSE;
  अन्यथा
    आकार_ज्ञातम् = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  यदि(आकार_ज्ञातम्)
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, expected_आकार);

  यदि(दत्त->state.resume_from) {
    यदि(!file->is_dir) {
      यदि(दत्त->state.resume_from !=
         curl_lseek(fd, दत्त->state.resume_from, SEEK_SET))
        प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
    }
    अन्यथा {
      प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  परिणाम = Curl_multi_xfer_buf_borrow(दत्त, &xfer_buf, &xfer_blen);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(!file->is_dir) {
    यावत्(!परिणाम) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      यदि(आकार_ज्ञातम्) {
        bytestoread = (expected_आकार < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_आकार) : (xfer_blen - 1);
      }
      अन्यथा
        bytestoread = xfer_blen - 1;

      nread = पठ(fd, xfer_buf, bytestoread);

      यदि(nread > 0)
        xfer_buf[nread] = 0;

      यदि(nread <= 0 || (आकार_ज्ञातम् && (expected_आकार == 0)))
        भङ्ग;

      यदि(आकार_ज्ञातम्)
        expected_आकार -= nread;

      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, xfer_buf, nread);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      परिणाम = Curl_pgrsCheck(दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा {
#यदि defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    परिणाम = win32_file_list(दत्त, file->मार्गः);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
#नोचेद्यदि defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->मार्गः);
    संरचना dirent *प्रविष्टिः;

    यदि(!dir) {
      परिणाम = CURLE_READ_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      यावत्((प्रविष्टिः = readdir(dir))) {
        यदि(प्रविष्टिः->d_name[0] != '.') {
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                   प्रविष्टिः->d_name, वाक्यदैर्घ्यम्(प्रविष्टिः->d_name));
          यदि(परिणाम)
            भङ्ग;
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
          यदि(परिणाम)
            भङ्ग;
        }
      }
      closedir(dir);
    }
#अन्यथा
    failf(दत्त, "Directory listing not yet implemented on this platform.");
    परिणाम = CURLE_READ_ERROR;
#यदि_अन्तः
  }

  यदि(!परिणाम)
    परिणाम = Curl_pgrsUpdate(दत्त);

बहिर्गमनम्:
  Curl_multi_xfer_buf_release(दत्त, xfer_buf);
  प्रत्यागम परिणाम;
}

ध्रुव संरचना Curl_protocol Curl_protocol_file = {
  file_सज्जता_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_समाप्तम्,                            /* done */
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

#यदि_अन्तः
