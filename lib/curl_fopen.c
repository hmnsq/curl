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

#ఒకవేళ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "curl_fopen.h"

/* The dirslash() function breaks a null-terminated pathname string into
   directory and filename components then returns the directory component up
   to, *AND INCLUDING*, a final '/'. If there is no directory in the path,
   this instead returns a "" string.

   This function returns a pointer to malloc'ed memory.

   The input path to this function is expected to have a filename part.
 */

#నిర్వచితమైతే _WIN32
#నిర్వచించు PATHSEP   "\\"
#నిర్వచించు IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#లేదావేళ defined(MSDOS) || defined(OS2)
#నిర్వచించు PATHSEP   "\\"
#నిర్వచించు IS_SEP(x) ((x) == '\\')
#లేకపోతే
#నిర్వచించు PATHSEP   "/"
#నిర్వచించు IS_SEP(x) ((x) == '/')
#అయితే_ముగింపు

స్థిర అక్షరం *dirslash(స్థిరరాశి అక్షరం *మార్గం)
{
  size_t n;
  నిర్మాణం dynbuf బహిర్గతం;
  DEBUGASSERT(మార్గం);
  curlx_dyn_init(&బహిర్గతం, CURL_MAX_ఆదానం_నిడివి);
  n = అక్షరశ్రేణి_పొడవు(మార్గం);
  ఒకవేళ(n) {
    /* find the rightmost path separator, if any */
    అయితే(n && !IS_SEP(మార్గం[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    అయితే(n && IS_SEP(మార్గం[n - 1]))
      --n;
  }
  ఒకవేళ(curlx_dyn_addn(&బహిర్గతం, మార్గం, n))
    తిరిగి ఖాళీ;
  /* if there was a directory, append a single trailing slash */
  ఒకవేళ(n && curlx_dyn_addn(&బహిర్గతం, PATHSEP, 1))
    తిరిగి ఖాళీ;
  తిరిగి curlx_dyn_ptr(&బహిర్గతం);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *filename,
                    FILE **fh, అక్షరం **tempname)
{
  CURLcode ఫలితం = CURLE_WRITE_ERROR;
  గుర్తులేని అక్షరం randbuf[41];
  అక్షరం *tempstore = ఖాళీ;
#నిర్వచితం_కాకపోతే _WIN32
  curlx_struct_stat sb;
#అయితే_ముగింపు
  పూర్ణాంకం fd = -1;
  అక్షరం *dir = ఖాళీ;
  *tempname = ఖాళీ;

#నిర్వచితం_కాకపోతే _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ఒకవేళ(!*fh)
    వెళ్ళు fail;
  ఒకవేళ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_బహుళకం)) {
    తిరిగి CURLE_OK;
  }
  curlx_fclose(*fh);
#అయితే_ముగింపు /* !_WIN32 */
  *fh = ఖాళీ;

  ఫలితం = Curl_rand_alnum(దత్తాంశం, randbuf, పరిమాణం(randbuf));
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  dir = dirslash(filename);
  ఒకవేళ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ఒకవేళ(!tempstore) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  ఫలితం = CURLE_WRITE_ERROR;
#నిర్వచితమైతే _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#లేకపోతే
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#అయితే_ముగింపు
  ఒకవేళ(fd == -1)
    వెళ్ళు fail;

#నిర్వచితమైతే HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ఒకవేళ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ఒకవేళ(fchmod(fd, (mode_t)sb.st_బహుళకం) == -1)
        వెళ్ళు fail;
    }
  }
#అయితే_ముగింపు

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ఒకవేళ(!*fh)
    వెళ్ళు fail;

  *tempname = tempstore;
  తిరిగి CURLE_OK;

fail:
  ఒకవేళ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !disabled */
