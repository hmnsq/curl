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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS)

#شامل "urldata.h"
#شامل "rand.h"
#شامل "curl_fopen.h"

/* The dirslash() function breaks a null-terminated pathname string into
   directory and filename components then returns the directory component up
   to, *AND INCLUDING*, a final '/'. If there is no directory in the path,
   this instead returns a "" string.

   This function returns a pointer to malloc'ed memory.

   The input path to this function is expected to have a filename part.
 */

#اگر_تعریف_شدہ _WIN32
#تعریف_کرو PATHSEP   "\\"
#تعریف_کرو IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#وگرنہ_اگر defined(MSDOS) || defined(OS2)
#تعریف_کرو PATHSEP   "\\"
#تعریف_کرو IS_SEP(x) ((x) == '\\')
#ورنہ
#تعریف_کرو PATHSEP   "/"
#تعریف_کرو IS_SEP(x) ((x) == '/')
#اگر_ختم

ساکن حرف *dirslash(ثابت حرف *راستہ)
{
  size_t n;
  ساخت dynbuf برآمد;
  DEBUGASSERT(راستہ);
  curlx_dyn_init(&برآمد, CURL_MAX_داخلہ_لمبائی);
  n = لڑی_لمبائی(راستہ);
  اگر(n) {
    /* find the rightmost path separator, if any */
    جب(n && !IS_SEP(راستہ[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    جب(n && IS_SEP(راستہ[n - 1]))
      --n;
  }
  اگر(curlx_dyn_addn(&برآمد, راستہ, n))
    واپس تہی;
  /* if there was a directory, append a single trailing slash */
  اگر(n && curlx_dyn_addn(&برآمد, PATHSEP, 1))
    واپس تہی;
  واپس curlx_dyn_ptr(&برآمد);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ساخت Curl_easy *ڈیٹا, ثابت حرف *filename,
                    FILE **fh, حرف **tempname)
{
  CURLcode نتیجہ = CURLE_WRITE_ERROR;
  غیرعلامتی حرف randbuf[41];
  حرف *tempstore = تہی;
#اگر_غیر_تعریف_شدہ _WIN32
  curlx_struct_stat sb;
#اگر_ختم
  صحیح fd = -1;
  حرف *dir = تہی;
  *tempname = تہی;

#اگر_غیر_تعریف_شدہ _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  اگر(!*fh)
    جاؤ fail;
  اگر(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_منوال)) {
    واپس CURLE_OK;
  }
  curlx_fclose(*fh);
#اگر_ختم /* !_WIN32 */
  *fh = تہی;

  نتیجہ = Curl_rand_alnum(ڈیٹا, randbuf, حجم(randbuf));
  اگر(نتیجہ)
    جاؤ fail;

  dir = dirslash(filename);
  اگر(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  اگر(!tempstore) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  نتیجہ = CURLE_WRITE_ERROR;
#اگر_تعریف_شدہ _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ورنہ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#اگر_ختم
  اگر(fd == -1)
    جاؤ fail;

#اگر_تعریف_شدہ HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    اگر((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      اگر(fchmod(fd, (mode_t)sb.st_منوال) == -1)
        جاؤ fail;
    }
  }
#اگر_ختم

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  اگر(!*fh)
    جاؤ fail;

  *tempname = tempstore;
  واپس CURLE_OK;

fail:
  اگر(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  واپس نتیجہ;
}

#اگر_ختم /* !disabled */
