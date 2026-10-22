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

#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_fseek(ଶୂନ୍ୟ *stream, curl_off_t offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା whence)
{
#ifdef _WIN32
  ଫେରନ୍ତୁ _fseeki64(stream, (__int64)offset, whence);
#ନଚେତଯଦି defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ଫେରନ୍ତୁ fseeko(stream, (off_t)offset, whence);
#ଅନ୍ୟଥା
  ଯଦି(offset > LONG_MAX)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#ଅନ୍ତର୍ଭାଵ <direct.h>  /* for _mkdir(), _wmkdir() */
#ଅନ୍ତର୍ଭାଵ <share.h>  /* for _SH_DENYNO */

#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   ମୁକ୍ତ(x)
#ଅନ୍ୟଥା
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
ସ୍ଥିର wchar_t *fn_convert_UTF8_to_wchar(ଅଚଳ ଅକ୍ଷର *str_utf8)
{
  wchar_t *str_w = ଖାଲି;

  ଯଦି(str_utf8) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ଖାଲି, 0);
    ଯଦି(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * ଆକାର(wchar_t));
      ଯଦି(str_w) {
        ଯଦି(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ଫେରନ୍ତୁ ଖାଲି;
        }
      }
    }
  }
  ଫେରନ୍ତୁ str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ଯଦି defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI GetFullPathNameW(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#endif

/* Fix excessive paths (paths that exceed MAX_PATH length of 260).
 *
 * This is a helper function to fix paths that would exceed the MAX_PATH
 * limitation check done by Windows APIs. It does so by normalizing the passed
 * in filename or path 'in' to its full canonical path, and if that path is
 * longer than MAX_PATH then setting 'out' to "\\?\" prefix + that full path.
 *
 * For example 'in' filename255chars in current directory C:\foo\bar is
 * fixed as \\?\C:\foo\bar\filename255chars for 'out' which tells Windows
 * it is ok to access that filename even though the actual full path is longer
 * than 260 chars.
 *
 * For non-Unicode builds this function may fail sometimes because only the
 * Unicode versions of some Windows API functions can access paths longer than
 * MAX_PATH, for example GetFullPathNameW which is used in this function. When
 * the full path is then converted from Unicode to multibyte that fails if any
 * directories in the path contain characters not in the current codepage.
 */
ସ୍ଥିର bool fix_excessive_path(ଅଚଳ TCHAR *in, TCHAR **out)
{
  size_t needed, ଗଣନା_2;
  ଅଚଳ wchar_t *in_w;
  wchar_t *fbuf = ଖାଲି;

  /* MS-documented "approximate" limit for the maximum path length */
  ଅଚଳ size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = ଖାଲି;
  ଅକ୍ଷର *obuf = ଖାଲି;
#endif

  *out = ଖାଲି;

  /* skip paths already normalized */
  ଯଦି(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    ଯାଅ cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  ଯଦି(mbstowcs_s(&needed, ଖାଲି, 0, in, 0))
    ଯାଅ cleanup;
  ଯଦି(!needed || needed >= max_path_len)
    ଯାଅ cleanup;
  ibuf = CURLX_MALLOC(needed * ଆକାର(wchar_t));
  ଯଦି(!ibuf)
    ଯାଅ cleanup;
  ଯଦି(mbstowcs_s(&ଗଣନା_2, ibuf, needed, in, needed - 1))
    ଯାଅ cleanup;
  ଯଦି(ଗଣନା_2 != needed)
    ଯାଅ cleanup;
  in_w = ibuf;
#ଅନ୍ୟଥା
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, ଖାଲି, ଖାଲି);
  ଯଦି(!needed || needed > max_path_len)
    ଯାଅ cleanup;
  /* skip paths that are not excessive and do not need modification */
  ଯଦି(needed <= MAX_PATH)
    ଯାଅ cleanup;
  fbuf = CURLX_MALLOC(needed * ଆକାର(wchar_t));
  ଯଦି(!fbuf)
    ଯାଅ cleanup;
  ଗଣନା_2 = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, ଖାଲି);
  ଯଦି(!ଗଣନା_2 || ଗଣନା_2 >= needed)
    ଯାଅ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ଯଦି(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  ଅନ୍ୟଥା ଯଦି(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ଅନ୍ୟଥା ଯଦି(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    ଯାଅ cleanup;
  }
  ଅନ୍ୟଥା {
    wchar_t *temp;

    ଯଦି(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ଗଣନା_2 - 2) + 1;
      ଯଦି(needed > max_path_len)
        ଯାଅ cleanup;

      temp = CURLX_MALLOC(needed * ଆକାର(wchar_t));
      ଯଦି(!temp)
        ଯାଅ cleanup;

      ଯଦି(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        ଯାଅ cleanup;
      }
      ଯଦି(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        ଯାଅ cleanup;
      }
    }
    ଅନ୍ୟଥା {
      /* "\\?\" + full path + null */
      needed = 4 + ଗଣନା_2 + 1;
      ଯଦି(needed > max_path_len)
        ଯାଅ cleanup;

      temp = CURLX_MALLOC(needed * ଆକାର(wchar_t));
      ଯଦି(!temp)
        ଯାଅ cleanup;

      ଯଦି(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        ଯାଅ cleanup;
      }
      ଯଦି(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        ଯାଅ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  ଯଦି(wcstombs_s(&needed, ଖାଲି, 0, fbuf, 0))
    ଯାଅ cleanup;
  ଯଦି(!needed || needed >= max_path_len)
    ଯାଅ cleanup;
  obuf = CURLX_MALLOC(needed);
  ଯଦି(!obuf)
    ଯାଅ cleanup;
  ଯଦି(wcstombs_s(&ଗଣନା_2, obuf, needed, fbuf, needed - 1))
    ଯାଅ cleanup;
  ଯଦି(ଗଣନା_2 != needed)
    ଯାଅ cleanup;
  *out = obuf;
  obuf = ଖାଲି;
#ଅନ୍ୟଥା
  *out = fbuf;
  fbuf = ଖାଲି;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  ଫେରନ୍ତୁ !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ଅଚଳ ଅକ୍ଷର *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_ମୂଲ୍ୟ;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ଅନ୍ୟଥା
  ଅଚଳ TCHAR *filename_t = filename;
#endif

  ଯଦି(filename_t) {
    TCHAR *fixed = ଖାଲି;
    ଅଚଳ TCHAR *target = ଖାଲି;

    ଯଦି(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(target,
                        dwDesiredAccess,
                        dwShareMode,
                        lpSecurityAttributes,
                        dwCreationDisposition,
                        dwFlagsAndAttributes,
                        hTemplateFile);
    CURLX_FREE(fixed);
#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  ଫେରନ୍ତୁ handle;
}

HANDLE curlx_FindFirstFile(ଅଚଳ ଅକ୍ଷର *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_ମୂଲ୍ୟ;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ଅନ୍ୟଥା
  ଅଚଳ TCHAR *filename_t = filename;
#endif

  ଯଦି(filename_t) {
    TCHAR *fixed = ଖାଲି;
    ଅଚଳ TCHAR *target;

    ଯଦି(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  ଫେରନ୍ତୁ handle;
}
#endif /* !CURL_WINDOWS_UWP */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_open(ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା oflag, ...)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmode = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = -1;
  TCHAR *fixed = ଖାଲି;
  ଅଚଳ TCHAR *target = ଖାଲି;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  ଯଦି(oflag & O_CREAT)
    pmode = va_arg(param, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
  va_end(param);

#ifdef _UNICODE
  ଯଦି(filename_w) {
    ଯଦି(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ଅନ୍ୟଥା
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ଅନ୍ୟଥା
  ଯଦି(fix_excessive_path(filename, &fixed))
    target = fixed;
  ଅନ୍ୟଥା
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  ଫେରନ୍ତୁ res;
}

FILE *curlx_win32_fopen(ଅଚଳ ଅକ୍ଷର *filename, ଅଚଳ ଅକ୍ଷର *mode)
{
  FILE *file = ଖାଲି;
  TCHAR *fixed = ଖାଲି;
  ଅଚଳ TCHAR *target = ଖାଲି;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ଯଦି(filename_w && mode_w) {
    ଯଦି(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  ଅନ୍ୟଥା
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ଅନ୍ୟଥା
  ଯଦି(fix_excessive_path(filename, &fixed))
    target = fixed;
  ଅନ୍ୟଥା
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  ଫେରନ୍ତୁ file;
}

#ଯଦି defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ଅଚଳ ଅକ୍ଷର *filename,
                                  ଅଚଳ ଅକ୍ଷର *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ଅଚଳ ଅକ୍ଷର *filename, ଅଚଳ ଅକ୍ଷର *mode, FILE *fp)
{
  FILE *file = ଖାଲି;
  TCHAR *fixed = ଖାଲି;
  ଅଚଳ TCHAR *target = ଖାଲି;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ଯଦି(filename_w && mode_w) {
    ଯଦି(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  ଅନ୍ୟଥା
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ଅନ୍ୟଥା
  ଯଦି(fix_excessive_path(filename, &fixed))
    target = fixed;
  ଅନ୍ୟଥା
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  ଫେରନ୍ତୁ file;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_stat(ଅଚଳ ଅକ୍ଷର *path, curlx_struct_stat *buffer)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = -1;
  TCHAR *fixed = ଖାଲି;
  ଅଚଳ TCHAR *target = ଖାଲି;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ଯଦି(path_w) {
    ଯଦି(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  ଅନ୍ୟଥା
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ଅନ୍ୟଥା
  ଯଦି(fix_excessive_path(path, &fixed))
    target = fixed;
  ଅନ୍ୟଥା
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  ଫେରନ୍ତୁ res;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_mkdir(ଅଚଳ ଅକ୍ଷର *path)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = -1;
  TCHAR *fixed = ଖାଲି;
  ଅଚଳ TCHAR *target = ଖାଲି;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ଯଦି(path_w) {
    ଯଦି(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ଅନ୍ୟଥା
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  ଅନ୍ୟଥା
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ଅନ୍ୟଥା
  ଯଦି(fix_excessive_path(path, &fixed))
    target = fixed;
  ଅନ୍ୟଥା
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  ଫେରନ୍ତୁ res;
}

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_rename(ଅଚଳ ଅକ୍ଷର *oldpath, ଅଚଳ ଅକ୍ଷର *newpath)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#ଅନ୍ୟଥା
  ଅଚଳ TCHAR *tchar_oldpath = oldpath;
  ଅଚଳ TCHAR *tchar_newpath = newpath;
#endif

  ଯଦି(tchar_oldpath && tchar_newpath) {
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_wait_ms = 1000;
    ସଂରଚନା curltime start;

    TCHAR *oldpath_fixed = ଖାଲି;
    TCHAR *newpath_fixed = ଖାଲି;
    ଅଚଳ TCHAR *target_oldpath;
    ଅଚଳ TCHAR *target_newpath;

    ଯଦି(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    ଅନ୍ୟଥା
      target_oldpath = tchar_oldpath;

    ଯଦି(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    ଅନ୍ୟଥା
      target_newpath = tchar_newpath;

    start = curlx_now();

    ପାଇଁ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ଯଦି(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ଭାଙ୍ଗ;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      ଯଦି(diff < 0 || diff > max_wait_ms) {
        ଭାଙ୍ଗ;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#ifdef UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#endif

  ଫେରନ୍ତୁ res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
