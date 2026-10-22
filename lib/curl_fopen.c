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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "curl_fopen.h"

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
#അല്ലെങ്കിലെങ്കിൽ defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#അല്ലെങ്കിൽ
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

സ്ഥിതം അക്ഷരം *dirslash(സ്ഥിരം അക്ഷരം *path)
{
  size_t n;
  ഘടന dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_നിവേശം_LENGTH);
  n = strlen(path);
  എങ്കിൽ(n) {
    /* find the rightmost path separator, if any */
    ആകുമ്പോൾ(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ആകുമ്പോൾ(n && IS_SEP(path[n - 1]))
      --n;
  }
  എങ്കിൽ(curlx_dyn_addn(&out, path, n))
    തിരികെ ശൂന്യ;
  /* if there was a directory, append a single trailing slash */
  എങ്കിൽ(n && curlx_dyn_addn(&out, PATHSEP, 1))
    തിരികെ ശൂന്യ;
  തിരികെ curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *filename,
                    FILE **fh, അക്ഷരം **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned അക്ഷരം randbuf[41];
  അക്ഷരം *tempstore = ശൂന്യ;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  പൂർണ്ണസംഖ്യ fd = -1;
  അക്ഷരം *dir = ശൂന്യ;
  *tempname = ശൂന്യ;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  എങ്കിൽ(!*fh)
    പോകുക fail;
  എങ്കിൽ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    തിരികെ CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = ശൂന്യ;

  result = Curl_rand_alnum(data, randbuf, വലുപ്പം(randbuf));
  എങ്കിൽ(result)
    പോകുക fail;

  dir = dirslash(filename);
  എങ്കിൽ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  എങ്കിൽ(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#അല്ലെങ്കിൽ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  എങ്കിൽ(fd == -1)
    പോകുക fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    എങ്കിൽ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      എങ്കിൽ(fchmod(fd, (mode_t)sb.st_mode) == -1)
        പോകുക fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  എങ്കിൽ(!*fh)
    പോകുക fail;

  *tempname = tempstore;
  തിരികെ CURLE_OK;

fail:
  എങ്കിൽ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  തിരികെ result;
}

#endif /* !disabled */
