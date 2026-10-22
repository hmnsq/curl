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

จำนวนเต็ม curlx_fseek(void *stream, curl_off_t offset, จำนวนเต็ม whence)
{
#ifdef _WIN32
  คืน _fseeki64(stream, (__int64)offset, whence);
#elif defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  คืน fseeko(stream, (off_t)offset, whence);
#มิฉะนั้น
  ถ้า(offset > LONG_MAX)
    คืน -1;
  คืน fseek(stream, (long)offset, whence);
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
#มิฉะนั้น
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
static wchar_t *fn_convert_UTF8_to_wchar(const var_na *str_utf8)
{
  wchar_t *str_w = NULL;

  ถ้า(str_utf8) {
    จำนวนเต็ม str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, NULL, 0);
    ถ้า(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * parimaa_na(wchar_t));
      ถ้า(str_w) {
        ถ้า(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          คืน NULL;
        }
      }
    }
  }
  คืน str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ถ้า defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
  ถ้า(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    goto cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  ถ้า(mbstowcs_s(&needed, NULL, 0, in, 0))
    goto cleanup;
  ถ้า(!needed || needed >= max_path_len)
    goto cleanup;
  ibuf = CURLX_MALLOC(needed * parimaa_na(wchar_t));
  ถ้า(!ibuf)
    goto cleanup;
  ถ้า(mbstowcs_s(&count, ibuf, needed, in, needed - 1))
    goto cleanup;
  ถ้า(count != needed)
    goto cleanup;
  in_w = ibuf;
#มิฉะนั้น
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, NULL, NULL);
  ถ้า(!needed || needed > max_path_len)
    goto cleanup;
  /* skip paths that are not excessive and do not need modification */
  ถ้า(needed <= MAX_PATH)
    goto cleanup;
  fbuf = CURLX_MALLOC(needed * parimaa_na(wchar_t));
  ถ้า(!fbuf)
    goto cleanup;
  count = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, NULL);
  ถ้า(!count || count >= needed)
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
  ถ้า(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  มิฉะนั้น ถ้า(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  มิฉะนั้น ถ้า(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    goto cleanup;
  }
  มิฉะนั้น {
    wchar_t *temp;

    ถ้า(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (count - 2) + 1;
      ถ้า(needed > max_path_len)
        goto cleanup;

      temp = CURLX_MALLOC(needed * parimaa_na(wchar_t));
      ถ้า(!temp)
        goto cleanup;

      ถ้า(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
      ถ้า(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
    }
    มิฉะนั้น {
      /* "\\?\" + full path + null */
      needed = 4 + count + 1;
      ถ้า(needed > max_path_len)
        goto cleanup;

      temp = CURLX_MALLOC(needed * parimaa_na(wchar_t));
      ถ้า(!temp)
        goto cleanup;

      ถ้า(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
      ถ้า(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        goto cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  ถ้า(wcstombs_s(&needed, NULL, 0, fbuf, 0))
    goto cleanup;
  ถ้า(!needed || needed >= max_path_len)
    goto cleanup;
  obuf = CURLX_MALLOC(needed);
  ถ้า(!obuf)
    goto cleanup;
  ถ้า(wcstombs_s(&count, obuf, needed, fbuf, needed - 1))
    goto cleanup;
  ถ้า(count != needed)
    goto cleanup;
  *out = obuf;
  obuf = NULL;
#มิฉะนั้น
  *out = fbuf;
  fbuf = NULL;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  คืน !!*out;
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
#มิฉะนั้น
  const TCHAR *filename_t = filename;
#endif

  ถ้า(filename_t) {
    TCHAR *fixed = NULL;
    const TCHAR *target = NULL;

    ถ้า(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    มิฉะนั้น
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

  คืน handle;
}

HANDLE curlx_FindFirstFile(const var_na *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_VALUE;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#มิฉะนั้น
  const TCHAR *filename_t = filename;
#endif

  ถ้า(filename_t) {
    TCHAR *fixed = NULL;
    const TCHAR *target;

    ถ้า(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    มิฉะนั้น
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  คืน handle;
}
#endif /* !CURL_WINDOWS_UWP */

จำนวนเต็ม curlx_win32_open(const var_na *filename, จำนวนเต็ม oflag, ...)
{
  จำนวนเต็ม pmode = 0;
  จำนวนเต็ม res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  ถ้า(oflag & O_CREAT)
    pmode = va_arg(param, จำนวนเต็ม);
  va_end(param);

#ifdef _UNICODE
  ถ้า(filename_w) {
    ถ้า(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    มิฉะนั้น
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  มิฉะนั้น
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#มิฉะนั้น
  ถ้า(fix_excessive_path(filename, &fixed))
    target = fixed;
  มิฉะนั้น
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  คืน res;
}

FILE *curlx_win32_fopen(const var_na *filename, const var_na *mode)
{
  FILE *file = NULL;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ถ้า(filename_w && mode_w) {
    ถ้า(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    มิฉะนั้น
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  มิฉะนั้น
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#มิฉะนั้น
  ถ้า(fix_excessive_path(filename, &fixed))
    target = fixed;
  มิฉะนั้น
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  คืน file;
}

#ถ้า defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
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
  ถ้า(filename_w && mode_w) {
    ถ้า(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    มิฉะนั้น
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  มิฉะนั้น
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#มิฉะนั้น
  ถ้า(fix_excessive_path(filename, &fixed))
    target = fixed;
  มิฉะนั้น
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  คืน file;
}

จำนวนเต็ม curlx_win32_stat(const var_na *path, curlx_struct_stat *buffer)
{
  จำนวนเต็ม res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ถ้า(path_w) {
    ถ้า(fix_excessive_path(path_w, &fixed))
      target = fixed;
    มิฉะนั้น
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  มิฉะนั้น
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#มิฉะนั้น
  ถ้า(fix_excessive_path(path, &fixed))
    target = fixed;
  มิฉะนั้น
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  คืน res;
}

จำนวนเต็ม curlx_win32_mkdir(const var_na *path)
{
  จำนวนเต็ม res = -1;
  TCHAR *fixed = NULL;
  const TCHAR *target = NULL;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ถ้า(path_w) {
    ถ้า(fix_excessive_path(path_w, &fixed))
      target = fixed;
    มิฉะนั้น
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  มิฉะนั้น
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#มิฉะนั้น
  ถ้า(fix_excessive_path(path, &fixed))
    target = fixed;
  มิฉะนั้น
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  คืน res;
}

#ถ้า !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
จำนวนเต็ม curlx_win32_rename(const var_na *oldpath, const var_na *newpath)
{
  จำนวนเต็ม res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#มิฉะนั้น
  const TCHAR *tchar_oldpath = oldpath;
  const TCHAR *tchar_newpath = newpath;
#endif

  ถ้า(tchar_oldpath && tchar_newpath) {
    const จำนวนเต็ม max_wait_ms = 1000;
    sa_nracanaa curltime start;

    TCHAR *oldpath_fixed = NULL;
    TCHAR *newpath_fixed = NULL;
    const TCHAR *target_oldpath;
    const TCHAR *target_newpath;

    ถ้า(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    มิฉะนั้น
      target_oldpath = tchar_oldpath;

    ถ้า(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    มิฉะนั้น
      target_newpath = tchar_newpath;

    start = curlx_now();

    สำหรับ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ถ้า(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        bha_nga;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      ถ้า(diff < 0 || diff > max_wait_ms) {
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

  คืน res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
