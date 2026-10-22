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

#जइ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "curl_fopen.h"

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
#नोचेद्यदि defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#अण्णहा
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

स्थिर वर्ण *dirslash(ध्रुव वर्ण *path)
{
  size_t n;
  संरचना dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_निवेशः_LENGTH);
  n = strlen(path);
  जइ(n) {
    /* find the rightmost path separator, if any */
    जाव(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    जाव(n && IS_SEP(path[n - 1]))
      --n;
  }
  जइ(curlx_dyn_addn(&out, path, n))
    प्रत्यागम रिक्त;
  /* if there was a directory, append a single trailing slash */
  जइ(n && curlx_dyn_addn(&out, PATHSEP, 1))
    प्रत्यागम रिक्त;
  प्रत्यागम curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(संरचना Curl_easy *data, ध्रुव वर्ण *filename,
                    FILE **fh, वर्ण **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned वर्ण randbuf[41];
  वर्ण *tempstore = रिक्त;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  पूर्णाङ्क fd = -1;
  वर्ण *dir = रिक्त;
  *tempname = रिक्त;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  जइ(!*fh)
    गच्छ fail;
  जइ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    प्रत्यागम CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = रिक्त;

  result = Curl_rand_alnum(data, randbuf, परिमाण(randbuf));
  जइ(result)
    गच्छ fail;

  dir = dirslash(filename);
  जइ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  जइ(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#अण्णहा
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  जइ(fd == -1)
    गच्छ fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    जइ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      जइ(fchmod(fd, (mode_t)sb.st_mode) == -1)
        गच्छ fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  जइ(!*fh)
    गच्छ fail;

  *tempname = tempstore;
  प्रत्यागम CURLE_OK;

fail:
  जइ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  प्रत्यागम result;
}

#endif /* !disabled */
