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

#यदि !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
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

#यदि_परिभाषितम् _WIN32
#परिभाषय PATHSEP   "\\"
#परिभाषय IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#नोचेद्यदि defined(MSDOS) || defined(OS2)
#परिभाषय PATHSEP   "\\"
#परिभाषय IS_SEP(x) ((x) == '\\')
#अन्यथा
#परिभाषय PATHSEP   "/"
#परिभाषय IS_SEP(x) ((x) == '/')
#यदि_अन्तः

स्थिर वर्ण *dirslash(ध्रुव वर्ण *मार्गः)
{
  size_t n;
  संरचना dynbuf बहिर्गमनम्;
  DEBUGASSERT(मार्गः);
  curlx_dyn_init(&बहिर्गमनम्, CURL_MAX_निवेशः_दीर्घता);
  n = वाक्यदैर्घ्यम्(मार्गः);
  यदि(n) {
    /* find the rightmost path separator, if any */
    यावत्(n && !IS_SEP(मार्गः[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    यावत्(n && IS_SEP(मार्गः[n - 1]))
      --n;
  }
  यदि(curlx_dyn_addn(&बहिर्गमनम्, मार्गः, n))
    प्रत्यागम रिक्त;
  /* if there was a directory, append a single trailing slash */
  यदि(n && curlx_dyn_addn(&बहिर्गमनम्, PATHSEP, 1))
    प्रत्यागम रिक्त;
  प्रत्यागम curlx_dyn_ptr(&बहिर्गमनम्);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(संरचना Curl_easy *दत्त, ध्रुव वर्ण *filename,
                    FILE **fh, वर्ण **tempname)
{
  CURLcode परिणाम = CURLE_WRITE_ERROR;
  अचिह्नित वर्ण randbuf[41];
  वर्ण *tempstore = रिक्त;
#यदि_अपरिभाषितम् _WIN32
  curlx_struct_stat sb;
#यदि_अन्तः
  पूर्णाङ्क fd = -1;
  वर्ण *dir = रिक्त;
  *tempname = रिक्त;

#यदि_अपरिभाषितम् _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  यदि(!*fh)
    गच्छ fail;
  यदि(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_बहुलकम्)) {
    प्रत्यागम CURLE_OK;
  }
  curlx_fclose(*fh);
#यदि_अन्तः /* !_WIN32 */
  *fh = रिक्त;

  परिणाम = Curl_rand_alnum(दत्त, randbuf, परिमाण(randbuf));
  यदि(परिणाम)
    गच्छ fail;

  dir = dirslash(filename);
  यदि(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  यदि(!tempstore) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  परिणाम = CURLE_WRITE_ERROR;
#यदि_परिभाषितम् _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#अन्यथा
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#यदि_अन्तः
  यदि(fd == -1)
    गच्छ fail;

#यदि_परिभाषितम् HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    यदि((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      यदि(fchmod(fd, (mode_t)sb.st_बहुलकम्) == -1)
        गच्छ fail;
    }
  }
#यदि_अन्तः

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  यदि(!*fh)
    गच्छ fail;

  *tempname = tempstore;
  प्रत्यागम CURLE_OK;

fail:
  यदि(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !disabled */
