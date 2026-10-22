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

#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

ಪೂರ್ಣಾಙ್ಕ curlx_fseek(ಶೂನ್ಯ *stream, curl_off_t offset, ಪೂರ್ಣಾಙ್ಕ whence)
{
#ifdef _WIN32
  ಪ್ರತ್ಯಾಗಮ _fseeki64(stream, (__int64)offset, whence);
#ನೋಚೇದ್ಯದಿ defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ಪ್ರತ್ಯಾಗಮ fseeko(stream, (off_t)offset, whence);
#ಅನ್ಯಥಾ
  ಯದಿ(offset > LONG_MAX)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#ಅನ್ತರ್ಭಾವ <direct.h>  /* for _mkdir(), _wmkdir() */
#ಅನ್ತರ್ಭಾವ <share.h>  /* for _SH_DENYNO */

#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   ಮುಕ್ತ(x)
#ಅನ್ಯಥಾ
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
ಸ್ಥಿರ wchar_t *fn_convert_UTF8_to_wchar(ಧ್ರುವ ವರ್ಣ *str_utf8)
{
  wchar_t *str_w = ರಿಕ್ತ;

  ಯದಿ(str_utf8) {
    ಪೂರ್ಣಾಙ್ಕ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ರಿಕ್ತ, 0);
    ಯದಿ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * ಪರಿಮಾಣ(wchar_t));
      ಯದಿ(str_w) {
        ಯದಿ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ಯದಿ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
ಸ್ಥಿರ bool fix_excessive_path(ಧ್ರುವ TCHAR *in, TCHAR **out)
{
  size_t needed, ಎಣಿಕೆ;
  ಧ್ರುವ wchar_t *in_w;
  wchar_t *fbuf = ರಿಕ್ತ;

  /* MS-documented "approximate" limit for the maximum path length */
  ಧ್ರುವ size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = ರಿಕ್ತ;
  ವರ್ಣ *obuf = ರಿಕ್ತ;
#endif

  *out = ರಿಕ್ತ;

  /* skip paths already normalized */
  ಯದಿ(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    ಗಚ್ಛ cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  ಯದಿ(mbstowcs_s(&needed, ರಿಕ್ತ, 0, in, 0))
    ಗಚ್ಛ cleanup;
  ಯದಿ(!needed || needed >= max_path_len)
    ಗಚ್ಛ cleanup;
  ibuf = CURLX_MALLOC(needed * ಪರಿಮಾಣ(wchar_t));
  ಯದಿ(!ibuf)
    ಗಚ್ಛ cleanup;
  ಯದಿ(mbstowcs_s(&ಎಣಿಕೆ, ibuf, needed, in, needed - 1))
    ಗಚ್ಛ cleanup;
  ಯದಿ(ಎಣಿಕೆ != needed)
    ಗಚ್ಛ cleanup;
  in_w = ibuf;
#ಅನ್ಯಥಾ
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, ರಿಕ್ತ, ರಿಕ್ತ);
  ಯದಿ(!needed || needed > max_path_len)
    ಗಚ್ಛ cleanup;
  /* skip paths that are not excessive and do not need modification */
  ಯದಿ(needed <= MAX_PATH)
    ಗಚ್ಛ cleanup;
  fbuf = CURLX_MALLOC(needed * ಪರಿಮಾಣ(wchar_t));
  ಯದಿ(!fbuf)
    ಗಚ್ಛ cleanup;
  ಎಣಿಕೆ = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, ರಿಕ್ತ);
  ಯದಿ(!ಎಣಿಕೆ || ಎಣಿಕೆ >= needed)
    ಗಚ್ಛ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ಯದಿ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  ಅನ್ಯಥಾ ಯದಿ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ಅನ್ಯಥಾ ಯದಿ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    ಗಚ್ಛ cleanup;
  }
  ಅನ್ಯಥಾ {
    wchar_t *temp;

    ಯದಿ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ಎಣಿಕೆ - 2) + 1;
      ಯದಿ(needed > max_path_len)
        ಗಚ್ಛ cleanup;

      temp = CURLX_MALLOC(needed * ಪರಿಮಾಣ(wchar_t));
      ಯದಿ(!temp)
        ಗಚ್ಛ cleanup;

      ಯದಿ(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        ಗಚ್ಛ cleanup;
      }
      ಯದಿ(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        ಗಚ್ಛ cleanup;
      }
    }
    ಅನ್ಯಥಾ {
      /* "\\?\" + full path + null */
      needed = 4 + ಎಣಿಕೆ + 1;
      ಯದಿ(needed > max_path_len)
        ಗಚ್ಛ cleanup;

      temp = CURLX_MALLOC(needed * ಪರಿಮಾಣ(wchar_t));
      ಯದಿ(!temp)
        ಗಚ್ಛ cleanup;

      ಯದಿ(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        ಗಚ್ಛ cleanup;
      }
      ಯದಿ(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        ಗಚ್ಛ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  ಯದಿ(wcstombs_s(&needed, ರಿಕ್ತ, 0, fbuf, 0))
    ಗಚ್ಛ cleanup;
  ಯದಿ(!needed || needed >= max_path_len)
    ಗಚ್ಛ cleanup;
  obuf = CURLX_MALLOC(needed);
  ಯದಿ(!obuf)
    ಗಚ್ಛ cleanup;
  ಯದಿ(wcstombs_s(&ಎಣಿಕೆ, obuf, needed, fbuf, needed - 1))
    ಗಚ್ಛ cleanup;
  ಯದಿ(ಎಣಿಕೆ != needed)
    ಗಚ್ಛ cleanup;
  *out = obuf;
  obuf = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
  *out = fbuf;
  fbuf = ರಿಕ್ತ;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  ಪ್ರತ್ಯಾಗಮ !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ಧ್ರುವ ವರ್ಣ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_ಮೌಲ್ಯ;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ಅನ್ಯಥಾ
  ಧ್ರುವ TCHAR *filename_t = filename;
#endif

  ಯದಿ(filename_t) {
    TCHAR *fixed = ರಿಕ್ತ;
    ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

    ಯದಿ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
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

  ಪ್ರತ್ಯಾಗಮ handle;
}

HANDLE curlx_FindFirstFile(ಧ್ರುವ ವರ್ಣ *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_ಮೌಲ್ಯ;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#ಅನ್ಯಥಾ
  ಧ್ರುವ TCHAR *filename_t = filename;
#endif

  ಯದಿ(filename_t) {
    TCHAR *fixed = ರಿಕ್ತ;
    ಧ್ರುವ TCHAR *target;

    ಯದಿ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  ಪ್ರತ್ಯಾಗಮ handle;
}
#endif /* !CURL_WINDOWS_UWP */

ಪೂರ್ಣಾಙ್ಕ curlx_win32_open(ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ oflag, ...)
{
  ಪೂರ್ಣಾಙ್ಕ pmode = 0;
  ಪೂರ್ಣಾಙ್ಕ res = -1;
  TCHAR *fixed = ರಿಕ್ತ;
  ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  ಯದಿ(oflag & O_CREAT)
    pmode = va_arg(param, ಪೂರ್ಣಾಙ್ಕ);
  va_end(param);

#ifdef _UNICODE
  ಯದಿ(filename_w) {
    ಯದಿ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ಅನ್ಯಥಾ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಅನ್ಯಥಾ
  ಯದಿ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ಅನ್ಯಥಾ
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  ಪ್ರತ್ಯಾಗಮ res;
}

FILE *curlx_win32_fopen(ಧ್ರುವ ವರ್ಣ *filename, ಧ್ರುವ ವರ್ಣ *mode)
{
  FILE *file = ರಿಕ್ತ;
  TCHAR *fixed = ರಿಕ್ತ;
  ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ಯದಿ(filename_w && mode_w) {
    ಯದಿ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  ಅನ್ಯಥಾ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ಅನ್ಯಥಾ
  ಯದಿ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ಅನ್ಯಥಾ
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  ಪ್ರತ್ಯಾಗಮ file;
}

#ಯದಿ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ಧ್ರುವ ವರ್ಣ *filename,
                                  ಧ್ರುವ ವರ್ಣ *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ಧ್ರುವ ವರ್ಣ *filename, ಧ್ರುವ ವರ್ಣ *mode, FILE *fp)
{
  FILE *file = ರಿಕ್ತ;
  TCHAR *fixed = ರಿಕ್ತ;
  ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  ಯದಿ(filename_w && mode_w) {
    ಯದಿ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  ಅನ್ಯಥಾ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#ಅನ್ಯಥಾ
  ಯದಿ(fix_excessive_path(filename, &fixed))
    target = fixed;
  ಅನ್ಯಥಾ
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  ಪ್ರತ್ಯಾಗಮ file;
}

ಪೂರ್ಣಾಙ್ಕ curlx_win32_stat(ಧ್ರುವ ವರ್ಣ *path, curlx_struct_stat *buffer)
{
  ಪೂರ್ಣಾಙ್ಕ res = -1;
  TCHAR *fixed = ರಿಕ್ತ;
  ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ಯದಿ(path_w) {
    ಯದಿ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  ಅನ್ಯಥಾ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಅನ್ಯಥಾ
  ಯದಿ(fix_excessive_path(path, &fixed))
    target = fixed;
  ಅನ್ಯಥಾ
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  ಪ್ರತ್ಯಾಗಮ res;
}

ಪೂರ್ಣಾಙ್ಕ curlx_win32_mkdir(ಧ್ರುವ ವರ್ಣ *path)
{
  ಪೂರ್ಣಾಙ್ಕ res = -1;
  TCHAR *fixed = ರಿಕ್ತ;
  ಧ್ರುವ TCHAR *target = ರಿಕ್ತ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  ಯದಿ(path_w) {
    ಯದಿ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    ಅನ್ಯಥಾ
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  ಅನ್ಯಥಾ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಅನ್ಯಥಾ
  ಯದಿ(fix_excessive_path(path, &fixed))
    target = fixed;
  ಅನ್ಯಥಾ
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  ಪ್ರತ್ಯಾಗಮ res;
}

#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
ಪೂರ್ಣಾಙ್ಕ curlx_win32_rename(ಧ್ರುವ ವರ್ಣ *oldpath, ಧ್ರುವ ವರ್ಣ *newpath)
{
  ಪೂರ್ಣಾಙ್ಕ res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#ಅನ್ಯಥಾ
  ಧ್ರುವ TCHAR *tchar_oldpath = oldpath;
  ಧ್ರುವ TCHAR *tchar_newpath = newpath;
#endif

  ಯದಿ(tchar_oldpath && tchar_newpath) {
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ max_wait_ms = 1000;
    ಸಂರಚನಾ curltime start;

    TCHAR *oldpath_fixed = ರಿಕ್ತ;
    TCHAR *newpath_fixed = ರಿಕ್ತ;
    ಧ್ರುವ TCHAR *target_oldpath;
    ಧ್ರುವ TCHAR *target_newpath;

    ಯದಿ(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    ಅನ್ಯಥಾ
      target_oldpath = tchar_oldpath;

    ಯದಿ(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    ಅನ್ಯಥಾ
      target_newpath = tchar_newpath;

    start = curlx_now();

    ಪ್ರತಿ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ಯದಿ(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ಭಙ್ಗ;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      ಯದಿ(diff < 0 || diff > max_wait_ms) {
        ಭಙ್ಗ;
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

  ಪ್ರತ್ಯಾಗಮ res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
