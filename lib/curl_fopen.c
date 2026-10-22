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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "curl_fopen.h"

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
#ନଚେତଯଦି defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#ଅନ୍ୟଥା
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

ସ୍ଥିର ଅକ୍ଷର *dirslash(ଅଚଳ ଅକ୍ଷର *path)
{
  size_t n;
  ସଂରଚନା dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_ନିବେଶ_LENGTH);
  n = strlen(path);
  ଯଦି(n) {
    /* find the rightmost path separator, if any */
    ଯେତେବେଳେ(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ଯେତେବେଳେ(n && IS_SEP(path[n - 1]))
      --n;
  }
  ଯଦି(curlx_dyn_addn(&out, path, n))
    ଫେରନ୍ତୁ ଖାଲି;
  /* if there was a directory, append a single trailing slash */
  ଯଦି(n && curlx_dyn_addn(&out, PATHSEP, 1))
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *filename,
                    FILE **fh, ଅକ୍ଷର **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned ଅକ୍ଷର randbuf[41];
  ଅକ୍ଷର *tempstore = ଖାଲି;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -1;
  ଅକ୍ଷର *dir = ଖାଲି;
  *tempname = ଖାଲି;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ଯଦି(!*fh)
    ଯାଅ fail;
  ଯଦି(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    ଫେରନ୍ତୁ CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = ଖାଲି;

  result = Curl_rand_alnum(data, randbuf, ଆକାର(randbuf));
  ଯଦି(result)
    ଯାଅ fail;

  dir = dirslash(filename);
  ଯଦି(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ଯଦି(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ଅନ୍ୟଥା
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  ଯଦି(fd == -1)
    ଯାଅ fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ଯଦି((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ଯଦି(fchmod(fd, (mode_t)sb.st_mode) == -1)
        ଯାଅ fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ଯଦି(!*fh)
    ଯାଅ fail;

  *tempname = tempstore;
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  ଯଦି(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ଫେରନ୍ତୁ result;
}

#endif /* !disabled */
