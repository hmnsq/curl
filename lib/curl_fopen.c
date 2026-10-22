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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#समावेश "urldata.h"
#समावेश "rand.h"
#समावेश "curl_fopen.h"

/* The dirslash() function breaks a null-terminated pathname string into
   directory and filename components then returns the directory component up
   to, *AND INCLUDING*, a final '/'. If there is no directory in the path,
   this instead returns a "" string.

   This function returns a pointer to malloc'ed memory.

   The input path to this function is expected to have a filename part.
 */

#अगर_घोषित _WIN32
#घोषणा PATHSEP   "\\"
#घोषणा IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#या_अगर defined(MSDOS) || defined(OS2)
#घोषणा PATHSEP   "\\"
#घोषणा IS_SEP(x) ((x) == '\\')
#अन्यथा
#घोषणा PATHSEP   "/"
#घोषणा IS_SEP(x) ((x) == '/')
#पूर्ण_अगर

अटल अक्षर *dirslash(स्थिर अक्षर *path)
{
  माप_प्रकार n;
  काष्ठा dynbuf out;
  DEBUGASSERT(path);
  curlx_dyn_init(&out, CURL_MAX_निवेश_लंबाई);
  n = म_माप(path);
  अगर(n) {
    /* find the rightmost path separator, if any */
    जबतक(n && !IS_SEP(path[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    जबतक(n && IS_SEP(path[n - 1]))
      --n;
  }
  अगर(curlx_dyn_addn(&out, path, n))
    वापस शून्य;
  /* if there was a directory, append a single trailing slash */
  अगर(n && curlx_dyn_addn(&out, PATHSEP, 1))
    वापस शून्य;
  वापस curlx_dyn_ptr(&out);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *filename,
                    खाता **fh, अक्षर **tempname)
{
  CURLcode परिणाम = CURLE_WRITE_ERROR;
  अचिन्हित अक्षर randbuf[41];
  अक्षर *tempstore = शून्य;
#अगर_अघोषित _WIN32
  curlx_struct_stat sb;
#पूर्ण_अगर
  पूर्णांक fd = -1;
  अक्षर *dir = शून्य;
  *tempname = शून्य;

#अगर_अघोषित _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  अगर(!*fh)
    जाओ fail;
  अगर(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_mode)) {
    वापस CURLE_OK;
  }
  curlx_fclose(*fh);
#पूर्ण_अगर /* !_WIN32 */
  *fh = शून्य;

  परिणाम = Curl_rand_alnum(तथ्य, randbuf, माप(randbuf));
  अगर(परिणाम)
    जाओ fail;

  dir = dirslash(filename);
  अगर(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  अगर(!tempstore) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  परिणाम = CURLE_WRITE_ERROR;
#अगर_घोषित _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#अन्यथा
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#पूर्ण_अगर
  अगर(fd == -1)
    जाओ fail;

#अगर_घोषित HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    अगर((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      अगर(fchmod(fd, (mode_t)sb.st_mode) == -1)
        जाओ fail;
    }
  }
#पूर्ण_अगर

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  अगर(!*fh)
    जाओ fail;

  *tempname = tempstore;
  वापस CURLE_OK;

fail:
  अगर(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  वापस परिणाम;
}

#पूर्ण_अगर /* !disabled */
