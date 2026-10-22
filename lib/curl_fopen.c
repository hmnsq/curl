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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "curl_fopen.h"

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
#নোচেদ্যদি defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#অন্যথা
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

স্থির ৱর্ণ *dirslash(ধ্রুৱ ৱর্ণ *path)
{
  size_t n;
  সংরচনা dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_প্ৰবেশ_LENGTH);
  n = strlen(path);
  যদি(n) {
    /* find the rightmost path separator, if any */
    যেতিয়ালৈ(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    যেতিয়ালৈ(n && IS_SEP(path[n - 1]))
      --n;
  }
  যদি(curlx_dyn_addn(&out, path, n))
    ঘূৰাই রিক্ত;
  /* if there was a directory, append a single trailing slash */
  যদি(n && curlx_dyn_addn(&out, PATHSEP, 1))
    ঘূৰাই রিক্ত;
  ঘূৰাই curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *filename,
                    FILE **fh, ৱর্ণ **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned ৱর্ণ randbuf[41];
  ৱর্ণ *tempstore = রিক্ত;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  পূর্ণাঙ্ক fd = -1;
  ৱর্ণ *dir = রিক্ত;
  *tempname = রিক্ত;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  যদি(!*fh)
    গচ্ছ fail;
  যদি(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    ঘূৰাই CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = রিক্ত;

  result = Curl_rand_alnum(data, randbuf, পরিমাণ(randbuf));
  যদি(result)
    গচ্ছ fail;

  dir = dirslash(filename);
  যদি(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  যদি(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#অন্যথা
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  যদি(fd == -1)
    গচ্ছ fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    যদি((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      যদি(fchmod(fd, (mode_t)sb.st_mode) == -1)
        গচ্ছ fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  যদি(!*fh)
    গচ্ছ fail;

  *tempname = tempstore;
  ঘূৰাই CURLE_OK;

fail:
  যদি(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ঘূৰাই result;
}

#endif /* !disabled */
