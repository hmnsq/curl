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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/win32-fopen.h"

പൂർണ്ണസംഖ്യ curlx_fseek(ശൂന്യം *stream, curl_off_t offset, പൂർണ്ണസംഖ്യ whence)
{
#ifdef _WIN32
  തിരികെ _fseeki64(stream, (__int64)offset, whence);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  തിരികെ fseeko(stream, (off_t)offset, whence);
#അല്ലെങ്കിൽ
  എങ്കിൽ(offset > LONG_MAX)
    തിരികെ -1;
  തിരികെ fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#അന്തര്ഭാവ <direct.h>  /* for _mkdir(), _wmkdir() */
#അന്തര്ഭാവ <share.h>  /* for _SH_DENYNO */

#അന്തര്ഭാവ "curlx/multibyte.h"
#അന്തര്ഭാവ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   സ്വതന്ത്രം(x)
#അല്ലെങ്കിൽ
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
സ്ഥിതം wchar_t *fn_convert_UTF8_to_wchar(സ്ഥിരം അക്ഷരം *str_utf8)
{
  wchar_t *str_w = ശൂന്യ;

  എങ്കിൽ(str_utf8) {
    പൂർണ്ണസംഖ്യ str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ശൂന്യ, 0);
    എങ്കിൽ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * വലുപ്പം(wchar_t));
      എങ്കിൽ(str_w) {
        എങ്കിൽ(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          തിരികെ ശൂന്യ;
        }
      }
    }
  }
  തിരികെ str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#എങ്കിൽ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
സ്ഥിതം bool fix_excessive_path(സ്ഥിരം TCHAR *in, TCHAR **out)
{
  size_t needed, എണ്ണം;
  സ്ഥിരം wchar_t *in_w;
  wchar_t *fbuf = ശൂന്യ;

  /* MS-documented "approximate" limit for the maximum path length */
  സ്ഥിരം size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = ശൂന്യ;
  അക്ഷരം *obuf = ശൂന്യ;
#endif

  *out = ശൂന്യ;

  /* skip paths already normalized */
  എങ്കിൽ(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    പോകുക cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  എങ്കിൽ(mbstowcs_s(&needed, ശൂന്യ, 0, in, 0))
    പോകുക cleanup;
  എങ്കിൽ(!needed || needed >= max_path_len)
    പോകുക cleanup;
  ibuf = CURLX_MALLOC(needed * വലുപ്പം(wchar_t));
  എങ്കിൽ(!ibuf)
    പോകുക cleanup;
  എങ്കിൽ(mbstowcs_s(&എണ്ണം, ibuf, needed, in, needed - 1))
    പോകുക cleanup;
  എങ്കിൽ(എണ്ണം != needed)
    പോകുക cleanup;
  in_w = ibuf;
#അല്ലെങ്കിൽ
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, ശൂന്യ, ശൂന്യ);
  എങ്കിൽ(!needed || needed > max_path_len)
    പോകുക cleanup;
  /* skip paths that are not excessive and do not need modification */
  എങ്കിൽ(needed <= MAX_PATH)
    പോകുക cleanup;
  fbuf = CURLX_MALLOC(needed * വലുപ്പം(wchar_t));
  എങ്കിൽ(!fbuf)
    പോകുക cleanup;
  എണ്ണം = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, ശൂന്യ);
  എങ്കിൽ(!എണ്ണം || എണ്ണം >= needed)
    പോകുക cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  എങ്കിൽ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  അല്ലെങ്കിൽ എങ്കിൽ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  അല്ലെങ്കിൽ എങ്കിൽ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    പോകുക cleanup;
  }
  അല്ലെങ്കിൽ {
    wchar_t *temp;

    എങ്കിൽ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (എണ്ണം - 2) + 1;
      എങ്കിൽ(needed > max_path_len)
        പോകുക cleanup;

      temp = CURLX_MALLOC(needed * വലുപ്പം(wchar_t));
      എങ്കിൽ(!temp)
        പോകുക cleanup;

      എങ്കിൽ(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        പോകുക cleanup;
      }
      എങ്കിൽ(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        പോകുക cleanup;
      }
    }
    അല്ലെങ്കിൽ {
      /* "\\?\" + full path + null */
      needed = 4 + എണ്ണം + 1;
      എങ്കിൽ(needed > max_path_len)
        പോകുക cleanup;

      temp = CURLX_MALLOC(needed * വലുപ്പം(wchar_t));
      എങ്കിൽ(!temp)
        പോകുക cleanup;

      എങ്കിൽ(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        പോകുക cleanup;
      }
      എങ്കിൽ(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        പോകുക cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  എങ്കിൽ(wcstombs_s(&needed, ശൂന്യ, 0, fbuf, 0))
    പോകുക cleanup;
  എങ്കിൽ(!needed || needed >= max_path_len)
    പോകുക cleanup;
  obuf = CURLX_MALLOC(needed);
  എങ്കിൽ(!obuf)
    പോകുക cleanup;
  എങ്കിൽ(wcstombs_s(&എണ്ണം, obuf, needed, fbuf, needed - 1))
    പോകുക cleanup;
  എങ്കിൽ(എണ്ണം != needed)
    പോകുക cleanup;
  *out = obuf;
  obuf = ശൂന്യ;
#അല്ലെങ്കിൽ
  *out = fbuf;
  fbuf = ശൂന്യ;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  തിരികെ !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(സ്ഥിരം അക്ഷരം *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_മൂല്യം;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#അല്ലെങ്കിൽ
  സ്ഥിരം TCHAR *filename_t = filename;
#endif

  എങ്കിൽ(filename_t) {
    TCHAR *fixed = ശൂന്യ;
    സ്ഥിരം TCHAR *target = ശൂന്യ;

    എങ്കിൽ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
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

  തിരികെ handle;
}

HANDLE curlx_FindFirstFile(സ്ഥിരം അക്ഷരം *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_മൂല്യം;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#അല്ലെങ്കിൽ
  സ്ഥിരം TCHAR *filename_t = filename;
#endif

  എങ്കിൽ(filename_t) {
    TCHAR *fixed = ശൂന്യ;
    സ്ഥിരം TCHAR *target;

    എങ്കിൽ(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  തിരികെ handle;
}
#endif /* !CURL_WINDOWS_UWP */

പൂർണ്ണസംഖ്യ curlx_win32_open(സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ oflag, ...)
{
  പൂർണ്ണസംഖ്യ pmode = 0;
  പൂർണ്ണസംഖ്യ res = -1;
  TCHAR *fixed = ശൂന്യ;
  സ്ഥിരം TCHAR *target = ശൂന്യ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  എങ്കിൽ(oflag & O_CREAT)
    pmode = va_arg(param, പൂർണ്ണസംഖ്യ);
  va_end(param);

#ifdef _UNICODE
  എങ്കിൽ(filename_w) {
    എങ്കിൽ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  അല്ലെങ്കിൽ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#അല്ലെങ്കിൽ
  എങ്കിൽ(fix_excessive_path(filename, &fixed))
    target = fixed;
  അല്ലെങ്കിൽ
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  തിരികെ res;
}

FILE *curlx_win32_fopen(സ്ഥിരം അക്ഷരം *filename, സ്ഥിരം അക്ഷരം *mode)
{
  FILE *file = ശൂന്യ;
  TCHAR *fixed = ശൂന്യ;
  സ്ഥിരം TCHAR *target = ശൂന്യ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  എങ്കിൽ(filename_w && mode_w) {
    എങ്കിൽ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  അല്ലെങ്കിൽ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#അല്ലെങ്കിൽ
  എങ്കിൽ(fix_excessive_path(filename, &fixed))
    target = fixed;
  അല്ലെങ്കിൽ
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  തിരികെ file;
}

#എങ്കിൽ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, സ്ഥിരം അക്ഷരം *filename,
                                  സ്ഥിരം അക്ഷരം *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(സ്ഥിരം അക്ഷരം *filename, സ്ഥിരം അക്ഷരം *mode, FILE *fp)
{
  FILE *file = ശൂന്യ;
  TCHAR *fixed = ശൂന്യ;
  സ്ഥിരം TCHAR *target = ശൂന്യ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  എങ്കിൽ(filename_w && mode_w) {
    എങ്കിൽ(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  അല്ലെങ്കിൽ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#അല്ലെങ്കിൽ
  എങ്കിൽ(fix_excessive_path(filename, &fixed))
    target = fixed;
  അല്ലെങ്കിൽ
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  തിരികെ file;
}

പൂർണ്ണസംഖ്യ curlx_win32_stat(സ്ഥിരം അക്ഷരം *path, curlx_struct_stat *buffer)
{
  പൂർണ്ണസംഖ്യ res = -1;
  TCHAR *fixed = ശൂന്യ;
  സ്ഥിരം TCHAR *target = ശൂന്യ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  എങ്കിൽ(path_w) {
    എങ്കിൽ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  അല്ലെങ്കിൽ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#അല്ലെങ്കിൽ
  എങ്കിൽ(fix_excessive_path(path, &fixed))
    target = fixed;
  അല്ലെങ്കിൽ
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  തിരികെ res;
}

പൂർണ്ണസംഖ്യ curlx_win32_mkdir(സ്ഥിരം അക്ഷരം *path)
{
  പൂർണ്ണസംഖ്യ res = -1;
  TCHAR *fixed = ശൂന്യ;
  സ്ഥിരം TCHAR *target = ശൂന്യ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  എങ്കിൽ(path_w) {
    എങ്കിൽ(fix_excessive_path(path_w, &fixed))
      target = fixed;
    അല്ലെങ്കിൽ
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  അല്ലെങ്കിൽ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#അല്ലെങ്കിൽ
  എങ്കിൽ(fix_excessive_path(path, &fixed))
    target = fixed;
  അല്ലെങ്കിൽ
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  തിരികെ res;
}

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
പൂർണ്ണസംഖ്യ curlx_win32_rename(സ്ഥിരം അക്ഷരം *oldpath, സ്ഥിരം അക്ഷരം *newpath)
{
  പൂർണ്ണസംഖ്യ res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#അല്ലെങ്കിൽ
  സ്ഥിരം TCHAR *tchar_oldpath = oldpath;
  സ്ഥിരം TCHAR *tchar_newpath = newpath;
#endif

  എങ്കിൽ(tchar_oldpath && tchar_newpath) {
    സ്ഥിരം പൂർണ്ണസംഖ്യ max_wait_ms = 1000;
    ഘടന curltime start;

    TCHAR *oldpath_fixed = ശൂന്യ;
    TCHAR *newpath_fixed = ശൂന്യ;
    സ്ഥിരം TCHAR *target_oldpath;
    സ്ഥിരം TCHAR *target_newpath;

    എങ്കിൽ(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    അല്ലെങ്കിൽ
      target_oldpath = tchar_oldpath;

    എങ്കിൽ(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    അല്ലെങ്കിൽ
      target_newpath = tchar_newpath;

    start = curlx_now();

    വേണ്ടി(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      എങ്കിൽ(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        നിർത്തുക;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      എങ്കിൽ(diff < 0 || diff > max_wait_ms) {
        നിർത്തുക;
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

  തിരികെ res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
