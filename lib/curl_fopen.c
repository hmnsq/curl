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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"

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
#ನೋಚೇದ್ಯದಿ defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#ಅನ್ಯಥಾ
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

ಸ್ಥಿರ ವರ್ಣ *dirslash(ಧ್ರುವ ವರ್ಣ *path)
{
  size_t n;
  ಸಂರಚನಾ dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_ಒಳಹರಿವು_LENGTH);
  n = strlen(path);
  ಯದಿ(n) {
    /* find the rightmost path separator, if any */
    ಯಾವತ್(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ಯಾವತ್(n && IS_SEP(path[n - 1]))
      --n;
  }
  ಯದಿ(curlx_dyn_addn(&out, path, n))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  /* if there was a directory, append a single trailing slash */
  ಯದಿ(n && curlx_dyn_addn(&out, PATHSEP, 1))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *filename,
                    FILE **fh, ವರ್ಣ **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned ವರ್ಣ randbuf[41];
  ವರ್ಣ *tempstore = ರಿಕ್ತ;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  ಪೂರ್ಣಾಙ್ಕ fd = -1;
  ವರ್ಣ *dir = ರಿಕ್ತ;
  *tempname = ರಿಕ್ತ;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ಯದಿ(!*fh)
    ಗಚ್ಛ fail;
  ಯದಿ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = ರಿಕ್ತ;

  result = Curl_rand_alnum(data, randbuf, ಪರಿಮಾಣ(randbuf));
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  dir = dirslash(filename);
  ಯದಿ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ಯದಿ(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ಅನ್ಯಥಾ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  ಯದಿ(fd == -1)
    ಗಚ್ಛ fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ಯದಿ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ಯದಿ(fchmod(fd, (mode_t)sb.st_mode) == -1)
        ಗಚ್ಛ fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ಯದಿ(!*fh)
    ಗಚ್ಛ fail;

  *tempname = tempstore;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

fail:
  ಯದಿ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !disabled */
