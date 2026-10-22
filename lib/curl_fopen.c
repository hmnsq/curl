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
#ضمن "curl_setup.h"

#אם !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#ضمن "urldata.h"
#ضمن "rand.h"
#ضمن "curl_fopen.h"

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
#وإلاإذا defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#אחרת
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

ساكن حرف *dirslash(ثابت حرف *path)
{
  size_t n;
  بنية dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_INPUT_LENGTH);
  n = strlen(path);
  אם(n) {
    /* find the rightmost path separator, if any */
    כלעוד(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    כלעוד(n && IS_SEP(path[n - 1]))
      --n;
  }
  אם(curlx_dyn_addn(&out, path, n))
    החזר عدم;
  /* if there was a directory, append a single trailing slash */
  אם(n && curlx_dyn_addn(&out, PATHSEP, 1))
    החזר عدم;
  החזר curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(بنية Curl_easy *data, ثابت حرف *filename,
                    FILE **fh, حرف **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned حرف randbuf[41];
  حرف *tempstore = عدم;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  صحيح fd = -1;
  حرف *dir = عدم;
  *tempname = عدم;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  אם(!*fh)
    اذهب fail;
  אם(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    החזר CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = عدم;

  result = Curl_rand_alnum(data, randbuf, حجم(randbuf));
  אם(result)
    اذهب fail;

  dir = dirslash(filename);
  אם(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  אם(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#אחרת
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  אם(fd == -1)
    اذهب fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    אם((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      אם(fchmod(fd, (mode_t)sb.st_mode) == -1)
        اذهب fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  אם(!*fh)
    اذهب fail;

  *tempname = tempstore;
  החזר CURLE_OK;

fail:
  אם(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  החזר result;
}

#endif /* !disabled */
