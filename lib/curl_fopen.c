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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "curl_fopen.h"

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
#அல்லதெனில் defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#இல்லையெனில்
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

நிலையான எழுத்து *dirslash(மாறிலி எழுத்து *path)
{
  size_t n;
  கட்டமைப்பு dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_உள்ளீடு_LENGTH);
  n = strlen(path);
  எனில்(n) {
    /* find the rightmost path separator, if any */
    போது(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    போது(n && IS_SEP(path[n - 1]))
      --n;
  }
  எனில்(curlx_dyn_addn(&out, path, n))
    திரும்பு வெற்று;
  /* if there was a directory, append a single trailing slash */
  எனில்(n && curlx_dyn_addn(&out, PATHSEP, 1))
    திரும்பு வெற்று;
  திரும்பு curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *filename,
                    FILE **fh, எழுத்து **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned எழுத்து randbuf[41];
  எழுத்து *tempstore = வெற்று;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  முழு fd = -1;
  எழுத்து *dir = வெற்று;
  *tempname = வெற்று;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  எனில்(!*fh)
    செல் fail;
  எனில்(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    திரும்பு CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = வெற்று;

  result = Curl_rand_alnum(data, randbuf, அளவு(randbuf));
  எனில்(result)
    செல் fail;

  dir = dirslash(filename);
  எனில்(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  எனில்(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#இல்லையெனில்
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  எனில்(fd == -1)
    செல் fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    எனில்((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      எனில்(fchmod(fd, (mode_t)sb.st_mode) == -1)
        செல் fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  எனில்(!*fh)
    செல் fail;

  *tempname = tempstore;
  திரும்பு CURLE_OK;

fail:
  எனில்(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  திரும்பு result;
}

#endif /* !disabled */
