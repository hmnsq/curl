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
#شامل "curl_setup.h"

#شامل "curlx/win32-fopen.h"

صحیح curlx_fseek(تهی *stream, curl_off_t offset, صحیح whence)
{
#ifdef _WIN32
  بازگشت _fseeki64(stream, (__int64)offset, whence);
#وگرنهاگر defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  بازگشت fseeko(stream, (off_t)offset, whence);
#وگرنه
  اگر(offset > LONG_MAX)
    بازگشت -1;
  بازگشت fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#شامل <direct.h>  /* for _mkdir(), _wmkdir() */
#شامل <share.h>  /* for _SH_DENYNO */

#شامل "curlx/multibyte.h"
#شامل "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   آزاد(x)
#وگرنه
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
ایستا wchar_t *fn_convert_UTF8_to_wchar(ثابت نویسه *str_utf8)
{
  wchar_t *str_w = پوچ;

  اگر(str_utf8) {
    صحیح str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, پوچ, 0);
    اگر(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * اندازه(wchar_t));
      اگر(str_w) {
        اگر(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          بازگشت پوچ;
        }
      }
    }
  }
  بازگشت str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#اگر defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
ایستا bool fix_excessive_path(ثابت TCHAR *in, TCHAR **out)
{
  size_t needed, شمار;
  ثابت wchar_t *in_w;
  wchar_t *fbuf = پوچ;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = پوچ;
  نویسه *obuf = پوچ;
#endif

  *out = پوچ;

  /* skip paths already normalized */
  اگر(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    برو cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  اگر(mbstowcs_s(&needed, پوچ, 0, in, 0))
    برو cleanup;
  اگر(!needed || needed >= max_path_len)
    برو cleanup;
  ibuf = CURLX_MALLOC(needed * اندازه(wchar_t));
  اگر(!ibuf)
    برو cleanup;
  اگر(mbstowcs_s(&شمار, ibuf, needed, in, needed - 1))
    برو cleanup;
  اگر(شمار != needed)
    برو cleanup;
  in_w = ibuf;
#وگرنه
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, پوچ, پوچ);
  اگر(!needed || needed > max_path_len)
    برو cleanup;
  /* skip paths that are not excessive and do not need modification */
  اگر(needed <= MAX_PATH)
    برو cleanup;
  fbuf = CURLX_MALLOC(needed * اندازه(wchar_t));
  اگر(!fbuf)
    برو cleanup;
  شمار = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, پوچ);
  اگر(!شمار || شمار >= needed)
    برو cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  اگر(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  وگرنه اگر(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  وگرنه اگر(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    برو cleanup;
  }
  وگرنه {
    wchar_t *temp;

    اگر(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (شمار - 2) + 1;
      اگر(needed > max_path_len)
        برو cleanup;

      temp = CURLX_MALLOC(needed * اندازه(wchar_t));
      اگر(!temp)
        برو cleanup;

      اگر(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        برو cleanup;
      }
      اگر(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        برو cleanup;
      }
    }
    وگرنه {
      /* "\\?\" + full path + null */
      needed = 4 + شمار + 1;
      اگر(needed > max_path_len)
        برو cleanup;

      temp = CURLX_MALLOC(needed * اندازه(wchar_t));
      اگر(!temp)
        برو cleanup;

      اگر(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        برو cleanup;
      }
      اگر(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        برو cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  اگر(wcstombs_s(&needed, پوچ, 0, fbuf, 0))
    برو cleanup;
  اگر(!needed || needed >= max_path_len)
    برو cleanup;
  obuf = CURLX_MALLOC(needed);
  اگر(!obuf)
    برو cleanup;
  اگر(wcstombs_s(&شمار, obuf, needed, fbuf, needed - 1))
    برو cleanup;
  اگر(شمار != needed)
    برو cleanup;
  *out = obuf;
  obuf = پوچ;
#وگرنه
  *out = fbuf;
  fbuf = پوچ;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  بازگشت !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت نویسه *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_مقدار;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#وگرنه
  ثابت TCHAR *filename_t = filename;
#endif

  اگر(filename_t) {
    TCHAR *fixed = پوچ;
    ثابت TCHAR *target = پوچ;

    اگر(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    وگرنه
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

  بازگشت handle;
}

HANDLE curlx_FindFirstFile(ثابت نویسه *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_مقدار;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#وگرنه
  ثابت TCHAR *filename_t = filename;
#endif

  اگر(filename_t) {
    TCHAR *fixed = پوچ;
    ثابت TCHAR *target;

    اگر(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    وگرنه
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  بازگشت handle;
}
#endif /* !CURL_WINDOWS_UWP */

صحیح curlx_win32_open(ثابت نویسه *filename, صحیح oflag, ...)
{
  صحیح pmode = 0;
  صحیح res = -1;
  TCHAR *fixed = پوچ;
  ثابت TCHAR *target = پوچ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  اگر(oflag & O_CREAT)
    pmode = va_arg(param, صحیح);
  va_end(param);

#ifdef _UNICODE
  اگر(filename_w) {
    اگر(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وگرنه
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  وگرنه
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وگرنه
  اگر(fix_excessive_path(filename, &fixed))
    target = fixed;
  وگرنه
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  بازگشت res;
}

FILE *curlx_win32_fopen(ثابت نویسه *filename, ثابت نویسه *mode)
{
  FILE *file = پوچ;
  TCHAR *fixed = پوچ;
  ثابت TCHAR *target = پوچ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  اگر(filename_w && mode_w) {
    اگر(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وگرنه
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  وگرنه
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#وگرنه
  اگر(fix_excessive_path(filename, &fixed))
    target = fixed;
  وگرنه
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  بازگشت file;
}

#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت نویسه *filename,
                                  ثابت نویسه *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ثابت نویسه *filename, ثابت نویسه *mode, FILE *fp)
{
  FILE *file = پوچ;
  TCHAR *fixed = پوچ;
  ثابت TCHAR *target = پوچ;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  اگر(filename_w && mode_w) {
    اگر(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    وگرنه
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  وگرنه
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#وگرنه
  اگر(fix_excessive_path(filename, &fixed))
    target = fixed;
  وگرنه
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  بازگشت file;
}

صحیح curlx_win32_stat(ثابت نویسه *path, curlx_struct_stat *buffer)
{
  صحیح res = -1;
  TCHAR *fixed = پوچ;
  ثابت TCHAR *target = پوچ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  اگر(path_w) {
    اگر(fix_excessive_path(path_w, &fixed))
      target = fixed;
    وگرنه
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  وگرنه
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وگرنه
  اگر(fix_excessive_path(path, &fixed))
    target = fixed;
  وگرنه
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  بازگشت res;
}

صحیح curlx_win32_mkdir(ثابت نویسه *path)
{
  صحیح res = -1;
  TCHAR *fixed = پوچ;
  ثابت TCHAR *target = پوچ;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  اگر(path_w) {
    اگر(fix_excessive_path(path_w, &fixed))
      target = fixed;
    وگرنه
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  وگرنه
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وگرنه
  اگر(fix_excessive_path(path, &fixed))
    target = fixed;
  وگرنه
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  بازگشت res;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
صحیح curlx_win32_rename(ثابت نویسه *oldpath, ثابت نویسه *newpath)
{
  صحیح res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#وگرنه
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#endif

  اگر(tchar_oldpath && tchar_newpath) {
    ثابت صحیح max_wait_ms = 1000;
    ساختار curltime start;

    TCHAR *oldpath_fixed = پوچ;
    TCHAR *newpath_fixed = پوچ;
    ثابت TCHAR *target_oldpath;
    ثابت TCHAR *target_newpath;

    اگر(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    وگرنه
      target_oldpath = tchar_oldpath;

    اگر(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    وگرنه
      target_newpath = tchar_newpath;

    start = curlx_now();

    برای(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      اگر(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        بشکن;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      اگر(diff < 0 || diff > max_wait_ms) {
        بشکن;
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

  بازگشت res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
