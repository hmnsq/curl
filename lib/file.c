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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "file.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_FILE

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_DIRENT_H
#అన్తర్భావ <dirent.h>
#అయితే_ముగింపు

#అన్తర్భావ "progress.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "parsedate.h" /* for the week day and month names */
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curl_range.h"

#ఒకవేళ defined(_WIN32) || defined(MSDOS)
#నిర్వచించు DOS_FILESYSTEM 1
#లేదావేళ defined(__amigaos4__)
#నిర్వచించు AMIGA_FILESYSTEM 1
#అయితే_ముగింపు

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_FILE_EASY   "meta:proto:file:easy"

నిర్మాణం FILEPROTO {
  అక్షరం *మార్గం; /* the path we operate on */
  అక్షరం *freepath; /* pointer to the allocated block we must free, this might
                     differ from the 'path' pointer */
  పూర్ణాంకం fd;     /* open file descriptor to read from! */
  bool is_dir;
};

స్థిర శూన్య file_cleanup(నిర్మాణం FILEPROTO *file)
{
  curlx_safefree(file->freepath);
  file->మార్గం = ఖాళీ;
  file->is_dir = FALSE;
  ఒకవేళ(file->fd != -1) {
    curlx_close(file->fd);
    file->fd = -1;
  }
}

స్థిర శూన్య file_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం FILEPROTO *file = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  file_cleanup(file);
  curlx_free(file);
}

స్థిర CURLcode file_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం FILEPROTO *filep;
  (శూన్య)conn;
  /* allocate the FILE specific struct */
  filep = curlx_calloc(1, పరిమాణం(*filep));
  ఒకవేళ(filep)
    filep->fd = -1;
  ఒకవేళ(!filep ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_FILE_EASY, filep, file_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode file_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                          CURLcode స్థితి_2, bool premature)
{
  నిర్మాణం FILEPROTO *file = Curl_meta_get(దత్తాంశం, CURL_META_FILE_EASY);
  (శూన్య)స్థితి_2;
  (శూన్య)premature;

  ఒకవేళ(file)
    file_cleanup(file);

  తిరిగి CURLE_OK;
}

స్థిర పూర్ణాంకం file_stat(స్థిరరాశి అక్షరం *మార్గం, curlx_struct_stat *statbuf)
{
#నిర్వచితమైతే _WIN32
  పూర్ణాంకం ఫలితం = curlx_stat(మార్గం, statbuf);

  ఒకవేళ(ఫలితం) {
    size_t pathlen = అక్షరశ్రేణి_పొడవు(మార్గం);

    /* MSVCRT's narrow stat() rejects trailing directory separators. */
    ఒకవేళ((pathlen > 3) &&
       ((మార్గం[pathlen - 1] == '\\') || (మార్గం[pathlen - 1] == '/'))) {
      అక్షరం *trimmed = curlx_strdup(మార్గం);

      ఒకవేళ(trimmed) {
        చేయి {
          trimmed[--pathlen] = '\0';
        } అయితే((pathlen > 3) &&
                ((trimmed[pathlen - 1] == '\\') ||
                 (trimmed[pathlen - 1] == '/')));

        ఫలితం = curlx_stat(trimmed, statbuf);
        curlx_free(trimmed);
      }
    }
  }

  తిరిగి ఫలితం;
#లేకపోతే
  తిరిగి curlx_stat(మార్గం, statbuf);
#అయితే_ముగింపు
}

/*
 * file_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time. We emulate a
 * connect-then-transfer protocol and "connect" to the file here
 */
స్థిర CURLcode file_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  అక్షరం *real_మార్గం;
  నిర్మాణం FILEPROTO *file = Curl_meta_get(దత్తాంశం, CURL_META_FILE_EASY);
#నిర్వచితమైతే _WIN32
  curlx_struct_stat statbuf;
#అయితే_ముగింపు
  పూర్ణాంకం fd;
#నిర్వచితమైతే DOS_FILESYSTEM
  size_t i;
  అక్షరం *actual_మార్గం;
#అయితే_ముగింపు
  size_t real_మార్గం_len;
  CURLcode ఫలితం;

  ఒకవేళ(!file)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(file->మార్గం) {
    /* already connected.
     * the handler->connect_it() is normally only called once, but
     * FILE does a special check on setting up the connection which
     * calls this explicitly. */
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఫలితం = Curl_urldecode(దత్తాంశం->state.పైకి.మార్గం, 0, &real_మార్గం,
                          &real_మార్గం_len, REJECT_ZERO);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#నిర్వచితమైతే DOS_FILESYSTEM
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
  actual_మార్గం = real_మార్గం;
  ఒకవేళ((actual_మార్గం[0] == '/') &&
      actual_మార్గం[1] &&
     (actual_మార్గం[2] == ':' || actual_మార్గం[2] == '|')) {
    actual_మార్గం[2] = ':';
    actual_మార్గం++;
    real_మార్గం_len--;
  }

  /* change path separators from '/' to '\\' for DOS, Windows and OS/2 */
  కొరకు(i = 0; i < real_మార్గం_len; ++i)
    ఒకవేళ(actual_మార్గం[i] == '/')
      actual_మార్గం[i] = '\\';
    లేకపోతే ఒకవేళ(!actual_మార్గం[i]) { /* binary zero */
      curlx_safefree(real_మార్గం);
      తిరిగి CURLE_URL_MALFORMAT;
    }

  fd = curlx_open(actual_మార్గం, O_RDONLY | CURL_O_ద్విమాన);
  file->మార్గం = actual_మార్గం;
#లేకపోతే
  ఒకవేళ(memchr(real_మార్గం, 0, real_మార్గం_len)) {
    /* binary zeroes indicate foul play */
    curlx_safefree(real_మార్గం);
    తిరిగి CURLE_URL_MALFORMAT;
  }

#నిర్వచితమైతే AMIGA_FILESYSTEM
  /*
   * A leading slash in an AmigaDOS path denotes the parent
   * directory, and hence we block this as it is relative.
   * Absolute paths start with 'volumename:', so we check for
   * this first. Failing that, we treat the path as a real Unix
   * path, but only if the application was compiled with -lunix.
   */
  fd = -1;
  file->మార్గం = real_మార్గం;

  ఒకవేళ(real_మార్గం[0] == '/') {
    బాహ్య పూర్ణాంకం __unix_path_semantics;
    ఒకవేళ(strchr(real_మార్గం + 1, ':')) {
      /* Amiga absolute path */
      fd = curlx_open(real_మార్గం + 1, O_RDONLY);
      file->మార్గం++;
    }
    లేకపోతే ఒకవేళ(__unix_path_semantics) {
      /* -lunix fallback */
      fd = curlx_open(real_మార్గం, O_RDONLY);
    }
  }
#లేకపోతే
  fd = curlx_open(real_మార్గం, O_RDONLY);
  file->మార్గం = real_మార్గం;
#అయితే_ముగింపు
#అయితే_ముగింపు
  curlx_free(file->freepath);
  file->freepath = real_మార్గం; /* free this when done */

  file->fd = fd;
#నిర్వచితమైతే _WIN32
  ఒకవేళ(!దత్తాంశం->state.upload && (fd == -1) &&
     !file_stat(file->మార్గం, &statbuf) && S_ISDIR(statbuf.st_బహుళకం))
    file->is_dir = TRUE;
#అయితే_ముగింపు

  ఒకవేళ(!దత్తాంశం->state.upload && (fd == -1) && !file->is_dir) {
    failf(దత్తాంశం, "Could not open file %s", దత్తాంశం->state.పైకి.మార్గం);
    file_పూర్తి(దత్తాంశం, CURLE_FILE_COULDNT_READ_FILE, FALSE);
    తిరిగి CURLE_FILE_COULDNT_READ_FILE;
  }
  *పూర్తి = TRUE;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode file_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn,
                                bool dead_connection)
{
  (శూన్య)dead_connection;
  (శూన్య)conn;
  తిరిగి file_పూర్తి(దత్తాంశం, CURLE_OK, FALSE);
}

#నిర్వచితమైతే DOS_FILESYSTEM
#నిర్వచించు DIRSEP '\\'
#లేకపోతే
#నిర్వచించు DIRSEP '/'
#అయితే_ముగింపు

స్థిర CURLcode file_upload(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం FILEPROTO *file)
{
  స్థిరరాశి అక్షరం *dir = strchr(file->మార్గం, DIRSEP);
  పూర్ణాంకం fd;
  పూర్ణాంకం బహుళకం;
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *xfer_ulbuf;
  size_t xfer_ulblen;
  curlx_struct_stat file_stat;
  స్థిరరాశి అక్షరం *sendbuf;
  bool eos = FALSE;

  /*
   * Since FILE: does not do the full init, we need to provide some extra
   * assignments here.
   */

  ఒకవేళ(!dir)
    తిరిగి CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  ఒకవేళ(!dir[1])
    తిరిగి CURLE_FILE_COULDNT_READ_FILE; /* fix: better error code */

  బహుళకం = O_WRONLY | O_CREAT | CURL_O_ద్విమాన;
  ఒకవేళ(దత్తాంశం->state.resume_from)
    బహుళకం |= O_APPEND;
  లేకపోతే
    బహుళకం |= O_TRUNC;

  fd = curlx_open(file->మార్గం, బహుళకం,
#నిర్వచితమైతే _WIN32
                  దత్తాంశం->సమితి.new_file_perms & (_S_IREAD | _S_IWRITE)
#లేకపోతే
                  (mode_t)దత్తాంశం->సమితి.new_file_perms
#అయితే_ముగింపు
                  );
  ఒకవేళ(fd < 0) {
    failf(దత్తాంశం, "cannot open %s for writing", file->మార్గం);
    తిరిగి CURLE_WRITE_ERROR;
  }

  ఒకవేళ(దత్తాంశం->state.infilesize != -1)
    /* known size of data to "upload" */
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);

  /* treat the negative resume offset value as the case of "-" */
  ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
    ఒకవేళ(curlx_fstat(fd, &file_stat)) {
      curlx_close(fd);
      failf(దత్తాంశం, "cannot get the size of %s", file->మార్గం);
      తిరిగి CURLE_WRITE_ERROR;
    }
    దత్తాంశం->state.resume_from = (curl_off_t)file_stat.st_కొలత;
  }

  ఫలితం = Curl_multi_xfer_ulbuf_borrow(దత్తాంశం, &xfer_ulbuf, &xfer_ulblen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  అయితే(!ఫలితం && !eos) {
    size_t nread, nwritten;
    ssize_t rv;
    size_t readcount;

    ఫలితం = Curl_client_read(దత్తాంశం, xfer_ulbuf, xfer_ulblen, &readcount, &eos);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(!readcount)
      ఆపు;

    nread = readcount;

    /* skip bytes before resume point */
    ఒకవేళ(దత్తాంశం->state.resume_from) {
      ఒకవేళ((curl_off_t)nread <= దత్తాంశం->state.resume_from) {
        దత్తాంశం->state.resume_from -= nread;
        nread = 0;
        sendbuf = xfer_ulbuf;
      }
      లేకపోతే {
        sendbuf = xfer_ulbuf + దత్తాంశం->state.resume_from;
        nread -= (size_t)దత్తాంశం->state.resume_from;
        దత్తాంశం->state.resume_from = 0;
      }
    }
    లేకపోతే
      sendbuf = xfer_ulbuf;

    /* write the data to the target */
    rv = లిఖ(fd, sendbuf, nread);
    ఒకవేళ(!curlx_sztouz(rv, &nwritten) || (nwritten != nread)) {
      ఫలితం = CURLE_SEND_ERROR;
      ఆపు;
    }
    Curl_pgrs_upload_inc(దత్తాంశం, nwritten);

    ఫలితం = Curl_pgrsCheck(దత్తాంశం);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_pgrsUpdate(దత్తాంశం);

బహిర్గతం:
  curlx_close(fd);
  Curl_multi_xfer_ulbuf_release(దత్తాంశం, xfer_ulbuf);

  తిరిగి ఫలితం;
}

#ఒకవేళ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
స్థిర CURLcode win32_file_list(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *మార్గం)
{
  WIN32_కనుగొను_దత్తాంశం నమోదు_2;
  HANDLE handle;
  అక్షరం *నమూనా;
  size_t pathlen = అక్షరశ్రేణి_పొడవు(మార్గం);
  CURLcode ఫలితం = CURLE_OK;
  DWORD error;

  నమూనా = curl_maprintf("%s%s*", మార్గం,
                          pathlen &&
                          (మార్గం[pathlen - 1] == '\\' ||
                           మార్గం[pathlen - 1] == '/') ? "" : "\\");
  ఒకవేళ(!నమూనా)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  handle = curlx_కనుగొను_మొదటి_File(నమూనా, &నమోదు_2);
  curlx_free(నమూనా);
  ఒకవేళ(handle == INVALID_HANDLE_విలువ)
    తిరిగి CURLE_READ_ERROR;

  చేయి {
    ఒకవేళ(నమోదు_2.cFileName[0] != పాఠ్యం_2('.')) {
      అక్షరం *name = curlx_convert_tchar_గమ్యం_UTF8(నమోదు_2.cFileName);

      ఒకవేళ(!name) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        ఆపు;
      }

      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                 name, అక్షరశ్రేణి_పొడవు(name));
      curlx_free(name);
      ఒకవేళ(ఫలితం)
        ఆపు;

      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
      ఒకవేళ(ఫలితం)
        ఆపు;
    }
  } అయితే(కనుగొను_తదుపరి_File(handle, &నమోదు_2));

  error = Get_చివరి_Error();
  ఒకవేళ(!ఫలితం && error != ERROR_NO_మరిన్ని_FILES)
    ఫలితం = CURLE_READ_ERROR;

  కనుగొను_Close(handle);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * file_do() is the protocol-specific function for the do-phase, separated
 * from the connect-phase above. Other protocols merely setup the transfer in
 * the do-phase, to have it done in the main transfer loop but since some
 * platforms we support do not allow select()ing etc on file handles (as
 * opposed to sockets) we instead perform the whole do-operation in this
 * function.
 */
స్థిర CURLcode file_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  /* This implementation ignores the hostname in conformance with
     RFC 1738. Only local files (reachable via the standard file system)
     are supported. This means that files on remotely mounted directories
     (via NFS, Samba, NT sharing) can be accessed through a file:// URL */
  నిర్మాణం FILEPROTO *file = Curl_meta_get(దత్తాంశం, CURL_META_FILE_EASY);
  CURLcode ఫలితం = CURLE_OK;
  curlx_struct_stat statbuf;
  curl_off_t expected_కొలత = -1;
  bool కొలత_తెలిసిన;
  bool fstated = FALSE;
  పూర్ణాంకం fd;
  అక్షరం *xfer_buf;
  size_t xfer_blen;

  *పూర్తి = TRUE; /* unconditionally */
  ఒకవేళ(!file)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(దత్తాంశం->state.upload)
    తిరిగి file_upload(దత్తాంశం, file);

  /* get the fd from the connection phase */
  fd = file->fd;

  /* VMS: This only works reliable for STREAMLF files */
  ఒకవేళ((file->is_dir ? file_stat(file->మార్గం, &statbuf) :
                     curlx_fstat(fd, &statbuf)) != -1) {
    file->is_dir = (fd == -1) || S_ISDIR(statbuf.st_బహుళకం);
    ఒకవేళ(!file->is_dir)
      expected_కొలత = statbuf.st_కొలత;
    /* and store the modification time */
    దత్తాంశం->info.filetime = statbuf.st_mtime;
    fstated = TRUE;
  }

  ఒకవేళ(fstated && !దత్తాంశం->state.range && దత్తాంశం->సమితి.timecondition &&
     !Curl_meets_timecondition(దత్తాంశం, దత్తాంశం->info.filetime))
    తిరిగి CURLE_OK;

  ఒకవేళ(fstated) {
    time_t filetime;
    నిర్మాణం tm బఫరు;
    స్థిరరాశి నిర్మాణం tm *tm = &బఫరు;
    అక్షరం శీర్షిక[80];
    పూర్ణాంకం headerlen;
    స్థిర స్థిరరాశి అక్షరం accept_ranges[] = { "Accept-ranges: bytes\r\n" };
    ఒకవేళ(expected_కొలత >= 0) {
      headerlen =
        curl_msnprintf(శీర్షిక, పరిమాణం(శీర్షిక),
                       "Content-Length: %" FMT_OFF_T "\r\n", expected_కొలత);
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, శీర్షిక, headerlen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక,
                                 accept_ranges, CURL_CSTRLEN(accept_ranges));
      ఒకవేళ(ఫలితం != CURLE_OK)
        తిరిగి ఫలితం;
    }

    filetime = (time_t)statbuf.st_mtime;
    ఫలితం = curlx_gmtime(filetime, &బఫరు);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    headerlen =
      curl_msnprintf(శీర్షిక, పరిమాణం(శీర్షిక),
                     "Last-Modified: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                     Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                     tm->tm_mday,
                     Curl_month[tm->tm_mon],
                     tm->tm_year + 1900,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, శీర్షిక, headerlen);
    ఒకవేళ(!ఫలితం)
      /* end of headers */
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, "\r\n", 2);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    /* set the file size to make it available post transfer */
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, expected_కొలత);
    ఒకవేళ(దత్తాంశం->req.no_body)
      తిరిగి CURLE_OK;
  }

  /* Check whether file range has been specified */
  ఫలితం = Curl_range(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Adjust the start offset in case we want to get the N last bytes
   * of the stream if the filesize could be determined */
  ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
    ఒకవేళ(!fstated) {
      failf(దత్తాంశం, "cannot get the size of file.");
      తిరిగి CURLE_READ_ERROR;
    }
    దత్తాంశం->state.resume_from += (curl_off_t)statbuf.st_కొలత;
  }

  ఒకవేళ(దత్తాంశం->state.resume_from > 0) {
    /* We check explicitly if we have a start offset, because
     * expected_size may be -1 if we do not know how large the file is,
     * in which case we should not adjust it. */
    ఒకవేళ(దత్తాంశం->state.resume_from <= expected_కొలత)
      expected_కొలత -= దత్తాంశం->state.resume_from;
    లేకపోతే {
      failf(దత్తాంశం, "failed to resume file:// transfer");
      తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  /* A high water mark has been specified so we obey... */
  ఒకవేళ(దత్తాంశం->req.maxdownload > 0)
    expected_కొలత = దత్తాంశం->req.maxdownload;

  ఒకవేళ(!fstated || (expected_కొలత <= 0))
    కొలత_తెలిసిన = FALSE;
  లేకపోతే
    కొలత_తెలిసిన = TRUE;

  /* The following is a shortcut implementation of file reading
     this is both more efficient than the former call to download() and
     it avoids problems with select() and recv() on file descriptors
     in Winsock */
  ఒకవేళ(కొలత_తెలిసిన)
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, expected_కొలత);

  ఒకవేళ(దత్తాంశం->state.resume_from) {
    ఒకవేళ(!file->is_dir) {
      ఒకవేళ(దత్తాంశం->state.resume_from !=
         curl_lseek(fd, దత్తాంశం->state.resume_from, SEEK_SET))
        తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
    }
    లేకపోతే {
      తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
    }
  }

  ఫలితం = Curl_multi_xfer_buf_borrow(దత్తాంశం, &xfer_buf, &xfer_blen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!file->is_dir) {
    అయితే(!ఫలితం) {
      ssize_t nread;
      /* Do not fill a whole buffer if we want less than all data */
      size_t bytestoread;

      ఒకవేళ(కొలత_తెలిసిన) {
        bytestoread = (expected_కొలత < (curl_off_t)(xfer_blen - 1)) ?
          curlx_sotouz(expected_కొలత) : (xfer_blen - 1);
      }
      లేకపోతే
        bytestoread = xfer_blen - 1;

      nread = పఠ(fd, xfer_buf, bytestoread);

      ఒకవేళ(nread > 0)
        xfer_buf[nread] = 0;

      ఒకవేళ(nread <= 0 || (కొలత_తెలిసిన && (expected_కొలత == 0)))
        ఆపు;

      ఒకవేళ(కొలత_తెలిసిన)
        expected_కొలత -= nread;

      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, xfer_buf, nread);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      ఫలితం = Curl_pgrsCheck(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే {
#ఒకవేళ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
    ఫలితం = win32_file_list(దత్తాంశం, file->మార్గం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
#లేదావేళ defined(HAVE_OPENDIR)
    DIR *dir = opendir(file->మార్గం);
    నిర్మాణం dirent *నమోదు_2;

    ఒకవేళ(!dir) {
      ఫలితం = CURLE_READ_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      అయితే((నమోదు_2 = readdir(dir))) {
        ఒకవేళ(నమోదు_2->d_name[0] != '.') {
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                   నమోదు_2->d_name, అక్షరశ్రేణి_పొడవు(నమోదు_2->d_name));
          ఒకవేళ(ఫలితం)
            ఆపు;
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
          ఒకవేళ(ఫలితం)
            ఆపు;
        }
      }
      closedir(dir);
    }
#లేకపోతే
    failf(దత్తాంశం, "Directory listing not yet implemented on this platform.");
    ఫలితం = CURLE_READ_ERROR;
#అయితే_ముగింపు
  }

  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_pgrsUpdate(దత్తాంశం);

బహిర్గతం:
  Curl_multi_xfer_buf_release(దత్తాంశం, xfer_buf);
  తిరిగి ఫలితం;
}

స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_file = {
  file_అమరిక_connection,                /* setup_connection */
  file_do,                              /* do_it */
  file_పూర్తి,                            /* done */
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

#అయితే_ముగింపు
