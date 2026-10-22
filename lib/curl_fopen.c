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

#إذا !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
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

#جيڪڏهن_وصف_ٿيل _WIN32
#وصف_ڪريو PATHSEP   "\\"
#وصف_ڪريو IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#وإلاإذا defined(MSDOS) || defined(OS2)
#وصف_ڪريو PATHSEP   "\\"
#وصف_ڪريو IS_SEP(x) ((x) == '\\')
#وإلا
#وصف_ڪريو PATHSEP   "/"
#وصف_ڪريو IS_SEP(x) ((x) == '/')
#جيڪڏهن_پورو

ساكن حرف *dirslash(ثابت حرف *رستو)
{
  size_t n;
  بنية dynbuf ٻاهريون;
  DEBUGASSERT(رستو);
  curlx_dyn_init(&ٻاهريون, CURL_MAX_داخلا_ڊيگهائي);
  n = لڙي_ڊيگهه(رستو);
  إذا(n) {
    /* find the rightmost path separator, if any */
    طالما(n && !IS_SEP(رستو[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    طالما(n && IS_SEP(رستو[n - 1]))
      --n;
  }
  إذا(curlx_dyn_addn(&ٻاهريون, رستو, n))
    أرجع عدم;
  /* if there was a directory, append a single trailing slash */
  إذا(n && curlx_dyn_addn(&ٻاهريون, PATHSEP, 1))
    أرجع عدم;
  أرجع curlx_dyn_ptr(&ٻاهريون);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(بنية Curl_easy *معلومات, ثابت حرف *filename,
                    FILE **fh, حرف **tempname)
{
  CURLcode نتيجو = CURLE_WRITE_ERROR;
  بنانشان حرف randbuf[41];
  حرف *tempstore = عدم;
#جيڪڏهن_وصف_نه_ٿيل _WIN32
  curlx_struct_stat sb;
#جيڪڏهن_پورو
  صحيح fd = -1;
  حرف *dir = عدم;
  *tempname = عدم;

#جيڪڏهن_وصف_نه_ٿيل _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  إذا(!*fh)
    اذهب fail;
  إذا(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_گهڻائي)) {
    أرجع CURLE_OK;
  }
  curlx_fclose(*fh);
#جيڪڏهن_پورو /* !_WIN32 */
  *fh = عدم;

  نتيجو = Curl_rand_alnum(معلومات, randbuf, حجم(randbuf));
  إذا(نتيجو)
    اذهب fail;

  dir = dirslash(filename);
  إذا(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  إذا(!tempstore) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

  نتيجو = CURLE_WRITE_ERROR;
#جيڪڏهن_وصف_ٿيل _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#وإلا
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#جيڪڏهن_پورو
  إذا(fd == -1)
    اذهب fail;

#جيڪڏهن_وصف_ٿيل HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    إذا((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      إذا(fchmod(fd, (mode_t)sb.st_گهڻائي) == -1)
        اذهب fail;
    }
  }
#جيڪڏهن_پورو

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  إذا(!*fh)
    اذهب fail;

  *tempname = tempstore;
  أرجع CURLE_OK;

fail:
  إذا(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !disabled */
