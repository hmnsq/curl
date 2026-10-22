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

#যদি !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curl_fopen.h"

/* The dirslash() function breaks a null-terminated pathname string into
   directory and filename components then returns the directory component up
   to, *AND INCLUDING*, a final '/'. If there is no directory in the path,
   this instead returns a "" string.

   This function returns a pointer to malloc'ed memory.

   The input path to this function is expected to have a filename part.
 */

#ifdef _WIN32
#define PATHSEP   "\\"
#define IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#নতুবা_যদি defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#অন্যথা
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

স্থির অক্ষর *dirslash(ধ্রুবক অক্ষর *path)
{
  size_t n;
  কাঠামো dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_প্রবেশ_দৈর্ঘ্য);
  n = strlen(path);
  যদি(n) {
    /* find the rightmost path separator, if any */
    যখন(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    যখন(n && IS_SEP(path[n - 1]))
      --n;
  }
  যদি(curlx_dyn_addn(&out, path, n))
    ফেরত ফাঁকা;
  /* if there was a directory, append a single trailing slash */
  যদি(n && curlx_dyn_addn(&out, PATHSEP, 1))
    ফেরত ফাঁকা;
  ফেরত curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *filename,
                    FILE **fh, অক্ষর **tempname)
{
  CURLcode ফলাফল = CURLE_WRITE_ERROR;
  unsigned অক্ষর randbuf[41];
  অক্ষর *tempstore = ফাঁকা;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  পূর্ণসংখ্যা fd = -1;
  অক্ষর *dir = ফাঁকা;
  *tempname = ফাঁকা;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  যদি(!*fh)
    যাও fail;
  যদি(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    ফেরত CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = ফাঁকা;

  ফলাফল = Curl_rand_alnum(তথ্য, randbuf, আকার(randbuf));
  যদি(ফলাফল)
    যাও fail;

  dir = dirslash(filename);
  যদি(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  যদি(!tempstore) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  ফলাফল = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#অন্যথা
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  যদি(fd == -1)
    যাও fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    যদি((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      যদি(fchmod(fd, (mode_t)sb.st_mode) == -1)
        যাও fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  যদি(!*fh)
    যাও fail;

  *tempname = tempstore;
  ফেরত CURLE_OK;

fail:
  যদি(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ফেরত ফলাফল;
}

#endif /* !disabled */
