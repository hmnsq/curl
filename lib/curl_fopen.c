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
#include "curl_setup.h"

#ຖ້າ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#include "urldata.h"
#include "rand.h"
#include "curl_fopen.h"

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
#elif defined(MSDOS) || defined(OS2)
#define PATHSEP   "\\"
#define IS_SEP(x) ((x) == '\\')
#ບໍ່ດັ່ງນັ້ນ
#define PATHSEP   "/"
#define IS_SEP(x) ((x) == '/')
#endif

static var_na *dirslash(const var_na *path)
{
  size_t n;
  sa_nracanaa dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_INPUT_LENGTH);
  n = strlen(path);
  ຖ້າ(n) {
    /* find the rightmost path separator, if any */
    ໃນຂະນະທີ່(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ໃນຂະນະທີ່(n && IS_SEP(path[n - 1]))
      --n;
  }
  ຖ້າ(curlx_dyn_addn(&out, path, n))
    ສົ່ງຄືນ NULL;
  /* if there was a directory, append a single trailing slash */
  ຖ້າ(n && curlx_dyn_addn(&out, PATHSEP, 1))
    ສົ່ງຄືນ NULL;
  ສົ່ງຄືນ curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(sa_nracanaa Curl_easy *data, const var_na *filename,
                    FILE **fh, var_na **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned var_na randbuf[41];
  var_na *tempstore = NULL;
#ifndef _WIN32
  curlx_struct_stat sb;
#endif
  ຈຳນວນເຕັມ fd = -1;
  var_na *dir = NULL;
  *tempname = NULL;

#ifndef _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ຖ້າ(!*fh)
    goto fail;
  ຖ້າ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    ສົ່ງຄືນ CURLE_OK;
  }
  curlx_fclose(*fh);
#endif /* !_WIN32 */
  *fh = NULL;

  result = Curl_rand_alnum(data, randbuf, parimaa_na(randbuf));
  ຖ້າ(result)
    goto fail;

  dir = dirslash(filename);
  ຖ້າ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ຖ້າ(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  result = CURLE_WRITE_ERROR;
#ifdef _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ບໍ່ດັ່ງນັ້ນ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#endif
  ຖ້າ(fd == -1)
    goto fail;

#ifdef HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ຖ້າ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ຖ້າ(fchmod(fd, (mode_t)sb.st_mode) == -1)
        goto fail;
    }
  }
#endif

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ຖ້າ(!*fh)
    goto fail;

  *tempname = tempstore;
  ສົ່ງຄືນ CURLE_OK;

fail:
  ຖ້າ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ສົ່ງຄືນ result;
}

#endif /* !disabled */
