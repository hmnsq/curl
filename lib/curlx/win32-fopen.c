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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/win32-fopen.h"

முழு curlx_fseek(வெறுமை *stream, curl_off_t offset, முழு whence)
{
#ifdef _WIN32
  திரும்பு _fseeki64(stream, (__int64)offset, whence);
#அல்லதெனில் defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  திரும்பு fseeko(stream, (off_t)offset, whence);
#இல்லையெனில்
  எனில்(offset > LONG_MAX)
    திரும்பு -1;
  திரும்பு fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#அந்தர்பாவ <direct.h>  /* for _mkdir(), _wmkdir() */
#அந்தர்பாவ <share.h>  /* for _SH_DENYNO */

#அந்தர்பாவ "curlx/multibyte.h"
#அந்தர்பாவ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   விடுவி(x)
#இல்லையெனில்
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
நிலையான wchar_t *fn_convert_UTF8_to_wchar(மாறிலி எழுத்து *str_utf8)
{
  wchar_t *str_w = வெற்று;

  எனில்(str_utf8) {
    முழு str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, வெற்று, 0);
    எனில்(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * அளவு(wchar_t));
      எனில்(str_w) {
        எனில்(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          திரும்பு வெற்று;
        }
      }
    }
  }
  திரும்பு str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#எனில் defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
நிலையான bool fix_excessive_path(மாறிலி TCHAR *in, TCHAR **out)
{
  size_t needed, மொத்தம்;
  மாறிலி wchar_t *in_w;
  wchar_t *fbuf = வெற்று;

  /* MS-documented "approximate" limit for the maximum path length */
  மாறிலி size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = வெற்று;
  எழுத்து *obuf = வெற்று;
#endif

  *out = வெற்று;

  /* skip paths already normalized */
  எனில்(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    செல் cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  எனில்(mbstowcs_s(&needed, வெற்று, 0, in, 0))
    செல் cleanup;
  எனில்(!needed || needed >= max_path_len)
    செல் cleanup;
  ibuf = CURLX_MALLOC(needed * அளவு(wchar_t));
  எனில்(!ibuf)
    செல் cleanup;
  எனில்(mbstowcs_s(&மொத்தம், ibuf, needed, in, needed - 1))
    செல் cleanup;
  எனில்(மொத்தம் != needed)
    செல் cleanup;
  in_w = ibuf;
#இல்லையெனில்
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, வெற்று, வெற்று);
  எனில்(!needed || needed > max_path_len)
    செல் cleanup;
  /* skip paths that are not excessive and do not need modification */
  எனில்(needed <= MAX_PATH)
    செல் cleanup;
  fbuf = CURLX_MALLOC(needed * அளவு(wchar_t));
  எனில்(!fbuf)
    செல் cleanup;
  மொத்தம் = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, வெற்று);
  எனில்(!மொத்தம் || மொத்தம் >= needed)
    செல் cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  எனில்(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  இல்லையெனில் எனில்(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  இல்லையெனில் எனில்(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    செல் cleanup;
  }
  இல்லையெனில் {
    wchar_t *temp;

    எனில்(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (மொத்தம் - 2) + 1;
      எனில்(needed > max_path_len)
        செல் cleanup;

      temp = CURLX_MALLOC(needed * அளவு(wchar_t));
      எனில்(!temp)
        செல் cleanup;

      எனில்(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        செல் cleanup;
      }
      எனில்(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        செல் cleanup;
      }
    }
    இல்லையெனில் {
      /* "\\?\" + full path + null */
      needed = 4 + மொத்தம் + 1;
      எனில்(needed > max_path_len)
        செல் cleanup;

      temp = CURLX_MALLOC(needed * அளவு(wchar_t));
      எனில்(!temp)
        செல் cleanup;

      எனில்(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        செல் cleanup;
      }
      எனில்(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        செல் cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  எனில்(wcstombs_s(&needed, வெற்று, 0, fbuf, 0))
    செல் cleanup;
  எனில்(!needed || needed >= max_path_len)
    செல் cleanup;
  obuf = CURLX_MALLOC(needed);
  எனில்(!obuf)
    செல் cleanup;
  எனில்(wcstombs_s(&மொத்தம், obuf, needed, fbuf, needed - 1))
    செல் cleanup;
  எனில்(மொத்தம் != needed)
    செல் cleanup;
  *out = obuf;
  obuf = வெற்று;
#இல்லையெனில்
  *out = fbuf;
  fbuf = வெற்று;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  திரும்பு !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(மாறிலி எழுத்து *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_மதிப்பு;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#இல்லையெனில்
  மாறிலி TCHAR *filename_t = filename;
#endif

  எனில்(filename_t) {
    TCHAR *fixed = வெற்று;
    மாறிலி TCHAR *target = வெற்று;

    எனில்(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    இல்லையெனில்
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

  திரும்பு handle;
}

HANDLE curlx_FindFirstFile(மாறிலி எழுத்து *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_மதிப்பு;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#இல்லையெனில்
  மாறிலி TCHAR *filename_t = filename;
#endif

  எனில்(filename_t) {
    TCHAR *fixed = வெற்று;
    மாறிலி TCHAR *target;

    எனில்(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    இல்லையெனில்
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  திரும்பு handle;
}
#endif /* !CURL_WINDOWS_UWP */

முழு curlx_win32_open(மாறிலி எழுத்து *filename, முழு oflag, ...)
{
  முழு pmode = 0;
  முழு res = -1;
  TCHAR *fixed = வெற்று;
  மாறிலி TCHAR *target = வெற்று;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  எனில்(oflag & O_CREAT)
    pmode = va_arg(param, முழு);
  va_end(param);

#ifdef _UNICODE
  எனில்(filename_w) {
    எனில்(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    இல்லையெனில்
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  இல்லையெனில்
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#இல்லையெனில்
  எனில்(fix_excessive_path(filename, &fixed))
    target = fixed;
  இல்லையெனில்
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  திரும்பு res;
}

FILE *curlx_win32_fopen(மாறிலி எழுத்து *filename, மாறிலி எழுத்து *mode)
{
  FILE *file = வெற்று;
  TCHAR *fixed = வெற்று;
  மாறிலி TCHAR *target = வெற்று;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  எனில்(filename_w && mode_w) {
    எனில்(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    இல்லையெனில்
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  இல்லையெனில்
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#இல்லையெனில்
  எனில்(fix_excessive_path(filename, &fixed))
    target = fixed;
  இல்லையெனில்
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  திரும்பு file;
}

#எனில் defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, மாறிலி எழுத்து *filename,
                                  மாறிலி எழுத்து *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(மாறிலி எழுத்து *filename, மாறிலி எழுத்து *mode, FILE *fp)
{
  FILE *file = வெற்று;
  TCHAR *fixed = வெற்று;
  மாறிலி TCHAR *target = வெற்று;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  எனில்(filename_w && mode_w) {
    எனில்(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    இல்லையெனில்
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  இல்லையெனில்
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#இல்லையெனில்
  எனில்(fix_excessive_path(filename, &fixed))
    target = fixed;
  இல்லையெனில்
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  திரும்பு file;
}

முழு curlx_win32_stat(மாறிலி எழுத்து *path, curlx_struct_stat *buffer)
{
  முழு res = -1;
  TCHAR *fixed = வெற்று;
  மாறிலி TCHAR *target = வெற்று;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  எனில்(path_w) {
    எனில்(fix_excessive_path(path_w, &fixed))
      target = fixed;
    இல்லையெனில்
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  இல்லையெனில்
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#இல்லையெனில்
  எனில்(fix_excessive_path(path, &fixed))
    target = fixed;
  இல்லையெனில்
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  திரும்பு res;
}

முழு curlx_win32_mkdir(மாறிலி எழுத்து *path)
{
  முழு res = -1;
  TCHAR *fixed = வெற்று;
  மாறிலி TCHAR *target = வெற்று;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  எனில்(path_w) {
    எனில்(fix_excessive_path(path_w, &fixed))
      target = fixed;
    இல்லையெனில்
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  இல்லையெனில்
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#இல்லையெனில்
  எனில்(fix_excessive_path(path, &fixed))
    target = fixed;
  இல்லையெனில்
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  திரும்பு res;
}

#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
முழு curlx_win32_rename(மாறிலி எழுத்து *oldpath, மாறிலி எழுத்து *newpath)
{
  முழு res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#இல்லையெனில்
  மாறிலி TCHAR *tchar_oldpath = oldpath;
  மாறிலி TCHAR *tchar_newpath = newpath;
#endif

  எனில்(tchar_oldpath && tchar_newpath) {
    மாறிலி முழு max_wait_ms = 1000;
    கட்டமைப்பு curltime start;

    TCHAR *oldpath_fixed = வெற்று;
    TCHAR *newpath_fixed = வெற்று;
    மாறிலி TCHAR *target_oldpath;
    மாறிலி TCHAR *target_newpath;

    எனில்(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    இல்லையெனில்
      target_oldpath = tchar_oldpath;

    எனில்(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    இல்லையெனில்
      target_newpath = tchar_newpath;

    start = curlx_now();

    க்கு(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      எனில்(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        நிறுத்து;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      எனில்(diff < 0 || diff > max_wait_ms) {
        நிறுத்து;
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

  திரும்பு res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
