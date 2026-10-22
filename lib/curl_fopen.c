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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fopen.h"

/* The dirslash() function breaks a null-terminated pathname string into
   directory and filename components then returns the directory component up
   to, *AND INCLUDING*, a final '/'. If there is no directory in the path,
   this instead returns a "" string.

   This function returns a pointer to malloc'ed memory.

   The input path to this function is expected to have a filename part.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PATHSEP   "\\"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#ਜਾਂਜੇ defined(MSDOS) || defined(OS2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PATHSEP   "\\"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_SEP(x) ((x) == '\\')
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PATHSEP   "/"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_SEP(x) ((x) == '/')
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *dirslash(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
  size_t n;
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  DEBUGASSERT(ਰਸਤਾ);
  curlx_dyn_init(&ਬਾਹਰਲਾ, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
  n = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
  ਜੇ(n) {
    /* find the rightmost path separator, if any */
    ਜਦੋਂ(n && !IS_SEP(ਰਸਤਾ[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ਜਦੋਂ(n && IS_SEP(ਰਸਤਾ[n - 1]))
      --n;
  }
  ਜੇ(curlx_dyn_addn(&ਬਾਹਰਲਾ, ਰਸਤਾ, n))
    ਵਾਪਸ ਨਿਰਾ;
  /* if there was a directory, append a single trailing slash */
  ਜੇ(n && curlx_dyn_addn(&ਬਾਹਰਲਾ, PATHSEP, 1))
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ curlx_dyn_ptr(&ਬਾਹਰਲਾ);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *filename,
                    FILE **fh, ਅੱਖਰ **tempname)
{
  CURLcode ਨਤੀਜਾ = CURLE_WRITE_ERROR;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ randbuf[41];
  ਅੱਖਰ *tempstore = ਨਿਰਾ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
  curlx_struct_stat sb;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ fd = -1;
  ਅੱਖਰ *dir = ਨਿਰਾ;
  *tempname = ਨਿਰਾ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ਜੇ(!*fh)
    ਜਾਓ fail;
  ਜੇ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_ਬਹੁਲਕ)) {
    ਵਾਪਸ CURLE_OK;
  }
  curlx_fclose(*fh);
#ਜੇ_ਅੰਤ /* !_WIN32 */
  *fh = ਨਿਰਾ;

  ਨਤੀਜਾ = Curl_rand_alnum(ਅੰਕੜੇ, randbuf, ਆਕਾਰ(randbuf));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  dir = dirslash(filename);
  ਜੇ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ਜੇ(!tempstore) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  ਨਤੀਜਾ = CURLE_WRITE_ERROR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ਨਹੀਂਤਾਂ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#ਜੇ_ਅੰਤ
  ਜੇ(fd == -1)
    ਜਾਓ fail;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ਜੇ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ਜੇ(fchmod(fd, (mode_t)sb.st_ਬਹੁਲਕ) == -1)
        ਜਾਓ fail;
    }
  }
#ਜੇ_ਅੰਤ

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ਜੇ(!*fh)
    ਜਾਓ fail;

  *tempname = tempstore;
  ਵਾਪਸ CURLE_OK;

fail:
  ਜੇ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !disabled */
