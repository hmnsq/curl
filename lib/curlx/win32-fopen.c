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
#ضمن "curl_setup.h"

#ضمن "curlx/win32-fopen.h"

صحيح curlx_fseek(فراغ *stream, curl_off_t offset, صحيح whence)
{
#ifdef _WIN32
  أرجع _fseeki64(stream, (__int64)offset, whence);
#وإلاإذا defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  أرجع fseeko(stream, (off_t)offset, whence);
#وإلا
  إذا(offset > LONG_MAX)
    أرجع -1;
  أرجع fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#ضمن <direct.h>  /* for _mkdir(), _wmkdir() */
#ضمن <share.h>  /* for _SH_DENYNO */

#ضمن "curlx/multibyte.h"
#ضمن "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   حرر(x)
#وإلا
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
ساكن wchar_t *fn_convert_UTF8_to_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = عدم;

  إذا(str_utf8) {
    صحيح str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, عدم, 0);
    إذا(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * حجم(wchar_t));
      إذا(str_w) {
        إذا(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          أرجع عدم;
        }
      }
    }
  }
  أرجع str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#إذا defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
ساكن bool fix_excessive_path(ثابت TCHAR *in, TCHAR **out)
{
  size_t needed, ژماردن;
  ثابت wchar_t *in_w;
  wchar_t *fbuf = عدم;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = عدم;
  حرف *obuf = عدم;
#endif

  *out = عدم;

  /* skip paths already normalized */
  إذا(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    اذهب cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  إذا(mbstowcs_s(&needed, عدم, 0, in, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_path_len)
    اذهب cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!ibuf)
    اذهب cleanup;
  إذا(mbstowcs_s(&ژماردن, ibuf, needed, in, needed - 1))
    اذهب cleanup;
  إذا(ژماردن != needed)
    اذهب cleanup;
  in_w = ibuf;
#وإلا
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, عدم, عدم);
  إذا(!needed || needed > max_path_len)
    اذهب cleanup;
  /* skip paths that are not excessive and do not need modification */
  إذا(needed <= MAX_PATH)
    اذهب cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!fbuf)
    اذهب cleanup;
  ژماردن = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, عدم);
  إذا(!ژماردن || ژماردن >= needed)
    اذهب cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  إذا(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  وإلا إذا(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  وإلا إذا(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    اذهب cleanup;
  }
  وإلا {
    wchar_t *temp;

    إذا(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ژماردن - 2) + 1;
      إذا(needed > max_path_len)
        اذهب cleanup;

      temp = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!temp)
        اذهب cleanup;

      إذا(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        اذهب cleanup;
      }
      إذا(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        اذهب cleanup;
      }
    }
    وإلا {
      /* "\\?\" + full path + null */
      needed = 4 + ژماردن + 1;
      إذا(needed > max_path_len)
        اذهب cleanup;

      temp = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!temp)
        اذهب cleanup;

      إذا(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        اذهب cleanup;
      }
      إذا(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        اذهب cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  إذا(wcstombs_s(&needed, عدم, 0, fbuf, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_path_len)
    اذهب cleanup;
  obuf = CURLX_MALLOC(needed);
  إذا(!obuf)
    اذهب cleanup;
  إذا(wcstombs_s(&ژماردن, obuf, needed, fbuf, needed - 1))
    اذهب cleanup;
  إذا(ژماردن != needed)
    اذهب cleanup;
  *out = obuf;
  obuf = عدم;
#وإلا
  *out = fbuf;
  fbuf = عدم;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  أرجع !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_بەها;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#endif

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *target = عدم;

    إذا(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    وإلا
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

  أرجع handle;
}

HANDLE curlx_FindFirstFile(ثابت حرف *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_بەها;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#endif

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *target;

    إذا(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    وإلا
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  أرجع handle;
}
#endif /* !CURL_WINDOWS_UWP */

صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...)
{
  صحيح pmode = 0;
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *target = عدم;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  إذا(oflag & O_CREAT)
    pmode = va_arg(param, صحيح);
  va_end(param);

#ifdef _UNICODE
  إذا(filename_w) {
    إذا(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وإلا
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_path(filename, &fixed))
    target = fixed;
  وإلا
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  أرجع res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *mode)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *target = عدم;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  إذا(filename_w && mode_w) {
    إذا(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وإلا
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#وإلا
  إذا(fix_excessive_path(filename, &fixed))
    target = fixed;
  وإلا
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  أرجع file;
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *mode, FILE *fp)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *target = عدم;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  إذا(filename_w && mode_w) {
    إذا(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وإلا
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#وإلا
  إذا(fix_excessive_path(filename, &fixed))
    target = fixed;
  وإلا
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  أرجع file;
}

صحيح curlx_win32_stat(ثابت حرف *path, curlx_struct_stat *buffer)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *target = عدم;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  إذا(path_w) {
    إذا(fix_excessive_path(path_w, &fixed))
      target = fixed;
    وإلا
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_path(path, &fixed))
    target = fixed;
  وإلا
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  أرجع res;
}

صحيح curlx_win32_mkdir(ثابت حرف *path)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *target = عدم;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  إذا(path_w) {
    إذا(fix_excessive_path(path_w, &fixed))
      target = fixed;
    وإلا
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_path(path, &fixed))
    target = fixed;
  وإلا
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  أرجع res;
}

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath)
{
  صحيح res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#وإلا
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#endif

  إذا(tchar_oldpath && tchar_newpath) {
    ثابت صحيح max_wait_ms = 1000;
    بنية curltime start;

    TCHAR *oldpath_fixed = عدم;
    TCHAR *newpath_fixed = عدم;
    ثابت TCHAR *target_oldpath;
    ثابت TCHAR *target_newpath;

    إذا(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    وإلا
      target_oldpath = tchar_oldpath;

    إذا(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    وإلا
      target_newpath = tchar_newpath;

    start = curlx_now();

    لكل(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      إذا(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        اكسر;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      إذا(diff < 0 || diff > max_wait_ms) {
        اكسر;
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

  أرجع res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
