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

#include "curlx/win32-fopen.h"

ຈຳນວນເຕັມ curlx_fseek(void *stream, curl_off_t offset, ຈຳນວນເຕັມ whence)
{
#ifdef _WIN32
  ສົ່ງຄືນ _fseeki64(stream, (__int64)offset, whence);
#elif defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ສົ່ງຄືນ fseeko(stream, (off_t)offset, whence);
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(offset > LONG_MAX)
    ສົ່ງຄືນ -1;
  ສົ່ງຄືນ fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#include <direct.h>  /* for _mkdir(), _wmkdir() */
#include <share.h>  /* for _SH_DENYNO */

#include "curlx/multibyte.h"
#include "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   free(x)
#ບໍ່ດັ່ງນັ້ນ
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
static wchar_t *fn_convert_UTF8_to_wchar(const var_na *str_utf8)
{
  wchar_t *str_w = NULL;

  ຖ້າ(str_utf8) {
    ຈຳນວນເຕັມ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, NULL, 0);
    ຖ້າ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * parimaa_na(wchar_t));
      ຖ້າ(str_w) {
        ຖ້າ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ສົ່ງຄືນ NULL;
        }
      }
    }
  }
  ສົ່ງຄືນ str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ຖ້າ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
static bool fix_excessive_path(const TCHAR *in, TCHAR **out)
{
  size_t needed, count;
  const wchar_t *in_w;
  wchar_t *fbuf = NULL;

  /* MS-documented "approximate" limit for the maximum path length */
  const size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = NULL;
  var_na *obuf = NULL;
#endif

  *out = NULL;

  /* skip paths already normalized */
  ຖ້າ(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    goto cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  ຖ້າ(mbstowcs_s(&needed, NULL, 0, in, 0))
    goto cleanup;
  ຖ້າ(!needed || needed >= max_path_len)
    goto cleanup;
  ibuf = CURLX_MALLOC(needed * parimaa_na(wchar_t));
  ຖ້າ(!ibuf)
    goto cleanup;
  ຖ້າ(mbstowcs_s(&count, ibuf, needed, in, needed - 1))
    goto cleanup;
  ຖ້າ(count != needed)
    goto cleanup;
  in_w = ibuf;
#ບໍ່ດັ່ງນັ້ນ
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, NULL, NULL);
  ຖ້າ(!needed || needed > max_path_len)
    goto cleanup;
  /* skip paths that are not excessive and do not need modification */
  ຖ້າ(needed <= MAX_PATH)
    goto cleanup;
  fbuf = CURLX_MALLOC(needed * parimaa_na(wchar_t));
  ຖ້າ(!fbuf)
    goto cleanup;
  count = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, NULL);
  ຖ້າ(!count || count >= needed)
    goto cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ຖ້າ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    goto cleanup;
  }
  ບໍ່ດັ່ງນັ້ນ {
    wchar_t *temp;

    ຖ້າ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (count - 2) + 1;
      ຖ້າ(needed > max_path_len)
        goto cleanup;

      temp = CURLX_MALLOC(needed * parimaa_na(wchar_t));
      ຖ້າ(!temp)
        goto cleanup;

      ຖ້າ(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
      ຖ້າ(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
    }
    ບໍ່ດັ່ງນັ້ນ {
      /* "\\?\" + full path + null */
      needed = 4 + count + 1;
      ຖ້າ(needed > max_path_len)
        goto cleanup;

      temp = CURLX_MALLOC(needed * parimaa_na(wchar_t));
      ຖ້າ(!temp)
        goto cleanup;

      ຖ້າ(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
      ຖ້າ(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  ຖ້າ(wcstombs_s(&needed, NULL, 0, fbuf, 0))
    goto cleanup;
  ຖ້າ(!needed || needed >= max_path_len)
    goto cleanup;
  obuf = CURLX_MALLOC(needed);
  ຖ້າ(!obuf)
    goto cleanup;
  ຖ້າ(wcstombs_s(&count, obuf, needed, fbuf, needed - 1))
    goto cleanup;
  ຖ້າ(count != needed)
    goto cleanup;
  *out = obuf;
  obuf = NULL;
#ບໍ່ດັ່ງນັ້ນ
  *out = fbuf;
  fbuf = NULL;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  ສົ່ງຄືນ !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(const var_na *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_VALUE;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ບໍ່ດັ່ງນັ້ນ
  const TCHAR *filename_t = filename;
#endif

  ຖ້າ(filename_t) {
    TCHAR *fixed = NULL;
    const TCHAR *target = NULL;

    ຖ້າ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
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

  ສົ່ງຄືນ handle;
}

HANDLE curlx_FindFirstFile(const var_na *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_VALUE;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ບໍ່ດັ່ງນັ້ນ
  const TCHAR *filename_t = filename;
#endif

  ຖ້າ(filename_t) {
    TCHAR *fixed = NULL;
    const TCHAR *target;

    ຖ້າ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  ສົ່ງຄືນ handle;
}
#endif /* !CURL_WINDOWS_UWP */

ຈຳນວນເຕັມ curlx_win32_open(const var_na *filename, ຈຳນວນເຕັມ oflag, ...)
{
  ຈຳນວນເຕັມ pmode = 0;
  ຈຳນວນເຕັມ res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  ຖ້າ(oflag & O_CREAT)
    pmode = va_arg(param, ຈຳນວນເຕັມ);
  va_end(param);

#ifdef _UNICODE
  ຖ້າ(filename_w) {
    ຖ້າ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ບໍ່ດັ່ງນັ້ນ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ບໍ່ດັ່ງນັ້ນ
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  ສົ່ງຄືນ res;
}

FILE *curlx_win32_fopen(const var_na *filename, const var_na *mode)
{
  FILE *file = NULL;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ຖ້າ(filename_w && mode_w) {
    ຖ້າ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  ບໍ່ດັ່ງນັ້ນ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ບໍ່ດັ່ງນັ້ນ
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  ສົ່ງຄືນ file;
}

#ຖ້າ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, const var_na *filename,
                                  const var_na *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(const var_na *filename, const var_na *mode, FILE *fp)
{
  FILE *file = NULL;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ຖ້າ(filename_w && mode_w) {
    ຖ້າ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  ບໍ່ດັ່ງນັ້ນ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ບໍ່ດັ່ງນັ້ນ
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  ສົ່ງຄືນ file;
}

ຈຳນວນເຕັມ curlx_win32_stat(const var_na *path, curlx_struct_stat *buffer)
{
  ຈຳນວນເຕັມ res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ຖ້າ(path_w) {
    ຖ້າ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  ບໍ່ດັ່ງນັ້ນ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(fix_excessive_path(path, &fixed))
    target = fixed;
  ບໍ່ດັ່ງນັ້ນ
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  ສົ່ງຄືນ res;
}

ຈຳນວນເຕັມ curlx_win32_mkdir(const var_na *path)
{
  ຈຳນວນເຕັມ res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ຖ້າ(path_w) {
    ຖ້າ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ບໍ່ດັ່ງນັ້ນ
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  ບໍ່ດັ່ງນັ້ນ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ບໍ່ດັ່ງນັ້ນ
  ຖ້າ(fix_excessive_path(path, &fixed))
    target = fixed;
  ບໍ່ດັ່ງນັ້ນ
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  ສົ່ງຄືນ res;
}

#ຖ້າ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
ຈຳນວນເຕັມ curlx_win32_rename(const var_na *oldpath, const var_na *newpath)
{
  ຈຳນວນເຕັມ res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#ບໍ່ດັ່ງນັ້ນ
  const TCHAR *tchar_oldpath = oldpath;
  const TCHAR *tchar_newpath = newpath;
#endif

  ຖ້າ(tchar_oldpath && tchar_newpath) {
    const ຈຳນວນເຕັມ max_wait_ms = 1000;
    sa_nracanaa curltime start;

    TCHAR *oldpath_fixed = NULL;
    TCHAR *newpath_fixed = NULL;
    const TCHAR *target_oldpath;
    const TCHAR *target_newpath;

    ຖ້າ(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    ບໍ່ດັ່ງນັ້ນ
      target_oldpath = tchar_oldpath;

    ຖ້າ(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    ບໍ່ດັ່ງນັ້ນ
      target_newpath = tchar_newpath;

    start = curlx_now();

    ສຳລັບ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ຖ້າ(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        bha_nga;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      ຖ້າ(diff < 0 || diff > max_wait_ms) {
        bha_nga;
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

  ສົ່ງຄືນ res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
