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

#ಆದರೆ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ವ್ಯಾಖ್ಯಾನಿಸು PATHSEP   "\\"
#ವ್ಯಾಖ್ಯಾನಿಸು IS_SEP(x) (((x) == '/') || ((x) == '\\'))
#ಅಥವಾಆದರೆ defined(MSDOS) || defined(OS2)
#ವ್ಯಾಖ್ಯಾನಿಸು PATHSEP   "\\"
#ವ್ಯಾಖ್ಯಾನಿಸು IS_SEP(x) ((x) == '\\')
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು PATHSEP   "/"
#ವ್ಯಾಖ್ಯಾನಿಸು IS_SEP(x) ((x) == '/')
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ *dirslash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  size_t n;
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  DEBUGASSERT(ಮಾರ್ಗ);
  curlx_dyn_init(&ಹೊರಹೋಗು, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
  n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
  ಆದರೆ(n) {
    /* find the rightmost path separator, if any */
    ಆಗ(n && !IS_SEP(ಮಾರ್ಗ[n - 1]))
      --n;
    /* skip over all the path separators, if any */
    ಆಗ(n && IS_SEP(ಮಾರ್ಗ[n - 1]))
      --n;
  }
  ಆದರೆ(curlx_dyn_addn(&ಹೊರಹೋಗು, ಮಾರ್ಗ, n))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  /* if there was a directory, append a single trailing slash */
  ಆದರೆ(n && curlx_dyn_addn(&ಹೊರಹೋಗು, PATHSEP, 1))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&ಹೊರಹೋಗು);
}

/*
 * Curl_fopen() opens a file for writing with a temp name, to be renamed
 * to the final name when completed. If there is an existing file using this
 * name at the time of the open, this function will clone the mode from that
 * file. if 'tempname' is non-NULL, it needs a rename after the file is
 * written.
 */
CURLcode Curl_fopen(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                    FILE **fh, ಅಕ್ಷರ **tempname)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ randbuf[41];
  ಅಕ್ಷರ *tempstore = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
  curlx_struct_stat sb;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ fd = -1;
  ಅಕ್ಷರ *dir = ಖಾಲಿ;
  *tempname = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
  *fh = curlx_fopen(filename, FOPEN_WRITETEXT);
  ಆದರೆ(!*fh)
    ಹೋಗು fail;
  ಆದರೆ(curlx_fstat(fileno(*fh), &sb) == -1 || !S_ISREG(sb.st_ಬಹುಲಕ)) {
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  curlx_fclose(*fh);
#ಆದರೆ_ಅಂತ್ಯ /* !_WIN32 */
  *fh = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = Curl_rand_alnum(ದತ್ತಾಂಶ, randbuf, ಗಾತ್ರ(randbuf));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  dir = dirslash(filename);
  ಆದರೆ(dir) {
    /* The temp filename should not end up too long for the target file
       system */
    tempstore = curl_maprintf("%s%s.tmp", dir, randbuf);
    curlx_free(dir);
  }

  ಆದರೆ(!tempstore) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  fd = curlx_open(tempstore, _O_WRONLY | _O_CREAT | _O_EXCL,
                  _S_IREAD | _S_IWRITE);
#ಇಲ್ಲದಿದ್ದರೆ
  fd = curlx_open(tempstore, O_WRONLY | O_CREAT | O_EXCL,
                  (mode_t)(S_IRUSR | S_IWUSR | 0600));
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(fd == -1)
    ಹೋಗು fail;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHMOD
  {
    curlx_struct_stat nsb;
    ಆದರೆ((curlx_fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      ಆದರೆ(fchmod(fd, (mode_t)sb.st_ಬಹುಲಕ) == -1)
        ಹೋಗು fail;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  *fh = curlx_fdopen(fd, FOPEN_WRITETEXT);
  ಆದರೆ(!*fh)
    ಹೋಗು fail;

  *tempname = tempstore;
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  ಆದರೆ(fd != -1) {
    curlx_close(fd);
    unlink(tempstore);
  }

  curlx_free(tempstore);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !disabled */
