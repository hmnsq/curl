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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "curlx/win32-fopen.h"

পূর্ণাঙ্ক curlx_fseek(শূন্য *stream, curl_off_t offset, পূর্ণাঙ্ক whence)
{
#ifdef _WIN32
  ঘূৰাই _fseeki64(stream, (__int64)offset, whence);
#নোচেদ্যদি defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ঘূৰাই fseeko(stream, (off_t)offset, whence);
#অন্যথা
  যদি(offset > LONG_MAX)
    ঘূৰাই -1;
  ঘূৰাই fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#অন্তর্ভাৱ <direct.h>  /* for _mkdir(), _wmkdir() */
#অন্তর্ভাৱ <share.h>  /* for _SH_DENYNO */

#অন্তর্ভাৱ "curlx/multibyte.h"
#অন্তর্ভাৱ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   মুক্ত(x)
#অন্যথা
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
স্থির wchar_t *fn_convert_UTF8_to_wchar(ধ্রুৱ ৱর্ণ *str_utf8)
{
  wchar_t *str_w = রিক্ত;

  যদি(str_utf8) {
    পূর্ণাঙ্ক str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, রিক্ত, 0);
    যদি(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * পরিমাণ(wchar_t));
      যদি(str_w) {
        যদি(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ঘূৰাই রিক্ত;
        }
      }
    }
  }
  ঘূৰাই str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#যদি defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
স্থির bool fix_excessive_path(ধ্রুৱ TCHAR *in, TCHAR **out)
{
  size_t needed, গুনতি;
  ধ্রুৱ wchar_t *in_w;
  wchar_t *fbuf = রিক্ত;

  /* MS-documented "approximate" limit for the maximum path length */
  ধ্রুৱ size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = রিক্ত;
  ৱর্ণ *obuf = রিক্ত;
#endif

  *out = রিক্ত;

  /* skip paths already normalized */
  যদি(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    গচ্ছ cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  যদি(mbstowcs_s(&needed, রিক্ত, 0, in, 0))
    গচ্ছ cleanup;
  যদি(!needed || needed >= max_path_len)
    গচ্ছ cleanup;
  ibuf = CURLX_MALLOC(needed * পরিমাণ(wchar_t));
  যদি(!ibuf)
    গচ্ছ cleanup;
  যদি(mbstowcs_s(&গুনতি, ibuf, needed, in, needed - 1))
    গচ্ছ cleanup;
  যদি(গুনতি != needed)
    গচ্ছ cleanup;
  in_w = ibuf;
#অন্যথা
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, রিক্ত, রিক্ত);
  যদি(!needed || needed > max_path_len)
    গচ্ছ cleanup;
  /* skip paths that are not excessive and do not need modification */
  যদি(needed <= MAX_PATH)
    গচ্ছ cleanup;
  fbuf = CURLX_MALLOC(needed * পরিমাণ(wchar_t));
  যদি(!fbuf)
    গচ্ছ cleanup;
  গুনতি = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, রিক্ত);
  যদি(!গুনতি || গুনতি >= needed)
    গচ্ছ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  যদি(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  অন্যথা যদি(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  অন্যথা যদি(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    গচ্ছ cleanup;
  }
  অন্যথা {
    wchar_t *temp;

    যদি(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (গুনতি - 2) + 1;
      যদি(needed > max_path_len)
        গচ্ছ cleanup;

      temp = CURLX_MALLOC(needed * পরিমাণ(wchar_t));
      যদি(!temp)
        গচ্ছ cleanup;

      যদি(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        গচ্ছ cleanup;
      }
      যদি(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        গচ্ছ cleanup;
      }
    }
    অন্যথা {
      /* "\\?\" + full path + null */
      needed = 4 + গুনতি + 1;
      যদি(needed > max_path_len)
        গচ্ছ cleanup;

      temp = CURLX_MALLOC(needed * পরিমাণ(wchar_t));
      যদি(!temp)
        গচ্ছ cleanup;

      যদি(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        গচ্ছ cleanup;
      }
      যদি(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        গচ্ছ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  যদি(wcstombs_s(&needed, রিক্ত, 0, fbuf, 0))
    গচ্ছ cleanup;
  যদি(!needed || needed >= max_path_len)
    গচ্ছ cleanup;
  obuf = CURLX_MALLOC(needed);
  যদি(!obuf)
    গচ্ছ cleanup;
  যদি(wcstombs_s(&গুনতি, obuf, needed, fbuf, needed - 1))
    গচ্ছ cleanup;
  যদি(গুনতি != needed)
    গচ্ছ cleanup;
  *out = obuf;
  obuf = রিক্ত;
#অন্যথা
  *out = fbuf;
  fbuf = রিক্ত;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  ঘূৰাই !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ধ্রুৱ ৱর্ণ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_মান;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#অন্যথা
  ধ্রুৱ TCHAR *filename_t = filename;
#endif

  যদি(filename_t) {
    TCHAR *fixed = রিক্ত;
    ধ্রুৱ TCHAR *target = রিক্ত;

    যদি(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    অন্যথা
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

  ঘূৰাই handle;
}

HANDLE curlx_FindFirstFile(ধ্রুৱ ৱর্ণ *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_মান;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#অন্যথা
  ধ্রুৱ TCHAR *filename_t = filename;
#endif

  যদি(filename_t) {
    TCHAR *fixed = রিক্ত;
    ধ্রুৱ TCHAR *target;

    যদি(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    অন্যথা
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  ঘূৰাই handle;
}
#endif /* !CURL_WINDOWS_UWP */

পূর্ণাঙ্ক curlx_win32_open(ধ্রুৱ ৱর্ণ *filename, পূর্ণাঙ্ক oflag, ...)
{
  পূর্ণাঙ্ক pmode = 0;
  পূর্ণাঙ্ক res = -1;
  TCHAR *fixed = রিক্ত;
  ধ্রুৱ TCHAR *target = রিক্ত;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  যদি(oflag & O_CREAT)
    pmode = va_arg(param, পূর্ণাঙ্ক);
  va_end(param);

#ifdef _UNICODE
  যদি(filename_w) {
    যদি(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    অন্যথা
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  অন্যথা
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#অন্যথা
  যদি(fix_excessive_path(filename, &fixed))
    target = fixed;
  অন্যথা
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  ঘূৰাই res;
}

FILE *curlx_win32_fopen(ধ্রুৱ ৱর্ণ *filename, ধ্রুৱ ৱর্ণ *mode)
{
  FILE *file = রিক্ত;
  TCHAR *fixed = রিক্ত;
  ধ্রুৱ TCHAR *target = রিক্ত;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  যদি(filename_w && mode_w) {
    যদি(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    অন্যথা
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  অন্যথা
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#অন্যথা
  যদি(fix_excessive_path(filename, &fixed))
    target = fixed;
  অন্যথা
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  ঘূৰাই file;
}

#যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ধ্রুৱ ৱর্ণ *filename,
                                  ধ্রুৱ ৱর্ণ *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ধ্রুৱ ৱর্ণ *filename, ধ্রুৱ ৱর্ণ *mode, FILE *fp)
{
  FILE *file = রিক্ত;
  TCHAR *fixed = রিক্ত;
  ধ্রুৱ TCHAR *target = রিক্ত;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  যদি(filename_w && mode_w) {
    যদি(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    অন্যথা
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  অন্যথা
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#অন্যথা
  যদি(fix_excessive_path(filename, &fixed))
    target = fixed;
  অন্যথা
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  ঘূৰাই file;
}

পূর্ণাঙ্ক curlx_win32_stat(ধ্রুৱ ৱর্ণ *path, curlx_struct_stat *buffer)
{
  পূর্ণাঙ্ক res = -1;
  TCHAR *fixed = রিক্ত;
  ধ্রুৱ TCHAR *target = রিক্ত;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  যদি(path_w) {
    যদি(fix_excessive_path(path_w, &fixed))
      target = fixed;
    অন্যথা
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  অন্যথা
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#অন্যথা
  যদি(fix_excessive_path(path, &fixed))
    target = fixed;
  অন্যথা
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  ঘূৰাই res;
}

পূর্ণাঙ্ক curlx_win32_mkdir(ধ্রুৱ ৱর্ণ *path)
{
  পূর্ণাঙ্ক res = -1;
  TCHAR *fixed = রিক্ত;
  ধ্রুৱ TCHAR *target = রিক্ত;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  যদি(path_w) {
    যদি(fix_excessive_path(path_w, &fixed))
      target = fixed;
    অন্যথা
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  অন্যথা
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#অন্যথা
  যদি(fix_excessive_path(path, &fixed))
    target = fixed;
  অন্যথা
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  ঘূৰাই res;
}

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
পূর্ণাঙ্ক curlx_win32_rename(ধ্রুৱ ৱর্ণ *oldpath, ধ্রুৱ ৱর্ণ *newpath)
{
  পূর্ণাঙ্ক res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#অন্যথা
  ধ্রুৱ TCHAR *tchar_oldpath = oldpath;
  ধ্রুৱ TCHAR *tchar_newpath = newpath;
#endif

  যদি(tchar_oldpath && tchar_newpath) {
    ধ্রুৱ পূর্ণাঙ্ক max_wait_ms = 1000;
    সংরচনা curltime start;

    TCHAR *oldpath_fixed = রিক্ত;
    TCHAR *newpath_fixed = রিক্ত;
    ধ্রুৱ TCHAR *target_oldpath;
    ধ্রুৱ TCHAR *target_newpath;

    যদি(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    অন্যথা
      target_oldpath = tchar_oldpath;

    যদি(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    অন্যথা
      target_newpath = tchar_newpath;

    start = curlx_now();

    বাবে(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      যদি(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ভঙ্গ;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      যদি(diff < 0 || diff > max_wait_ms) {
        ভঙ্গ;
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

  ঘূৰাই res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
