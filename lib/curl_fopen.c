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

#جے_تعریف_شدہ _WIN32
#تعریف_کرو PATHSEP   "\\"
#تعریف_کرو IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#وإلاإذا defined(MSDOS) || defined(OS2)
#تعریف_کرو PATHSEP   "\\"
#تعریف_کرو IS_SEP(x) ((x) == '\\')
#وإلا
#تعریف_کرو PATHSEP   "/"
#تعریف_کرو IS_SEP(x) ((x) == '/')
#جے_انت

ساكن حرف *dirslash(ثابت حرف *رستہ)
{
  size_t n;
  بنية dynbuf باہرلا;
  DEBUGASSERT(رستہ);
  curlx_dyn_init(&باہرلا, CURL_MAX_داخلہ_لمائی);
  n = لڑی_لمبائی(رستہ);
  إذا(n) {
    /* find the rightmost path separator, if any */
    طالما(n && !IS_SEP(رستہ[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    طالما(n && IS_SEP(رستہ[n - 1]))
      --n;
  }
  إذا(curlx_dyn_addn(&باہرلا, رستہ, n))
    أرجع عدم;
  /* if there was a directory, append a single trailing slash */
  إذا(n && curlx_dyn_addn(&باہرلا, PATHSEP, 1))
    أرجع عدم;
  أرجع curlx_dyn_ptr(&باہرلا);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(بنية Curl_easy *انکڑے, ثابت حرف *filename,
                    FILE **fh, حرف **tempname)
{
  CURLcode نتیجہ = CURLE_WRITE_ERROR;
  بنانشانی حرف randbuf[41];
  حرف *tempstore = عدم;
#جے_بنا_تعریف _WIN32
  curlx_struct_stat sb;
#جے_انت
  صحيح fd = -1;
  حرف *dir = عدم;
  *tempname = عدم;

#جے_بنا_تعریف _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  إذا(!*fh)
    اذهب fail;
  إذا(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_منوال)) {
    أرجع CURLE_OK;
  }
  curlx_fclose(*fh);
#جے_انت /* !_WIN32 */
  *fh = عدم;

  نتیجہ = Curl_rand_alnum(انکڑے, randbuf, حجم(randbuf));
  إذا(نتیجہ)
    اذهب fail;

  dir = dirslash(filename);
  إذا(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  إذا(!tempstore) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  نتیجہ = CURLE_WRITE_ERROR;
#جے_تعریف_شدہ _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#وإلا
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#جے_انت
  إذا(fd == -1)
    اذهب fail;

#جے_تعریف_شدہ HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    إذا((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      إذا(fchmod(fd, (mode_t)sb.st_منوال) == -1)
        اذهب fail;
    }
  }
#جے_انت

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
  أرجع نتیجہ;
}

#جے_انت /* !disabled */
