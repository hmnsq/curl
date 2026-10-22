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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/win32-fopen.h"

पूर्णाङ्क curlx_fseek(शून्य *stream, curl_off_t offset, पूर्णाङ्क whence)
{
#ifdef _WIN32
  पटिनिवत्त _fseeki64(stream, (__int64)offset, whence);
#नोचेद्यदि defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  पटिनिवत्त fseeko(stream, (off_t)offset, whence);
#अञ्ञथा
  सचे(offset > LONG_MAX)
    पटिनिवत्त -1;
  पटिनिवत्त fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#अन्तर्भाव <direct.h>  /* for _mkdir(), _wmkdir() */
#अन्तर्भाव <share.h>  /* for _SH_DENYNO */

#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   मुक्त(x)
#अञ्ञथा
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
स्थिर wchar_t *fn_convert_UTF8_to_wchar(ध्रुव वर्ण *str_utf8)
{
  wchar_t *str_w = रिक्त;

  सचे(str_utf8) {
    पूर्णाङ्क str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, रिक्त, 0);
    सचे(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * परिमाण(wchar_t));
      सचे(str_w) {
        सचे(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          पटिनिवत्त रिक्त;
        }
      }
    }
  }
  पटिनिवत्त str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#सचे defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
स्थिर bool fix_excessive_path(ध्रुव TCHAR *in, TCHAR **out)
{
  size_t needed, गणनम्;
  ध्रुव wchar_t *in_w;
  wchar_t *fbuf = रिक्त;

  /* MS-documented "approximate" limit for the maximum path length */
  ध्रुव size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = रिक्त;
  वर्ण *obuf = रिक्त;
#endif

  *out = रिक्त;

  /* skip paths already normalized */
  सचे(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    गच्छ cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  सचे(mbstowcs_s(&needed, रिक्त, 0, in, 0))
    गच्छ cleanup;
  सचे(!needed || needed >= max_path_len)
    गच्छ cleanup;
  ibuf = CURLX_MALLOC(needed * परिमाण(wchar_t));
  सचे(!ibuf)
    गच्छ cleanup;
  सचे(mbstowcs_s(&गणनम्, ibuf, needed, in, needed - 1))
    गच्छ cleanup;
  सचे(गणनम् != needed)
    गच्छ cleanup;
  in_w = ibuf;
#अञ्ञथा
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, रिक्त, रिक्त);
  सचे(!needed || needed > max_path_len)
    गच्छ cleanup;
  /* skip paths that are not excessive and do not need modification */
  सचे(needed <= MAX_PATH)
    गच्छ cleanup;
  fbuf = CURLX_MALLOC(needed * परिमाण(wchar_t));
  सचे(!fbuf)
    गच्छ cleanup;
  गणनम् = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, रिक्त);
  सचे(!गणनम् || गणनम् >= needed)
    गच्छ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  सचे(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  अञ्ञथा सचे(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  अञ्ञथा सचे(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    गच्छ cleanup;
  }
  अञ्ञथा {
    wchar_t *temp;

    सचे(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (गणनम् - 2) + 1;
      सचे(needed > max_path_len)
        गच्छ cleanup;

      temp = CURLX_MALLOC(needed * परिमाण(wchar_t));
      सचे(!temp)
        गच्छ cleanup;

      सचे(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        गच्छ cleanup;
      }
      सचे(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        गच्छ cleanup;
      }
    }
    अञ्ञथा {
      /* "\\?\" + full path + null */
      needed = 4 + गणनम् + 1;
      सचे(needed > max_path_len)
        गच्छ cleanup;

      temp = CURLX_MALLOC(needed * परिमाण(wchar_t));
      सचे(!temp)
        गच्छ cleanup;

      सचे(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        गच्छ cleanup;
      }
      सचे(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        गच्छ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  सचे(wcstombs_s(&needed, रिक्त, 0, fbuf, 0))
    गच्छ cleanup;
  सचे(!needed || needed >= max_path_len)
    गच्छ cleanup;
  obuf = CURLX_MALLOC(needed);
  सचे(!obuf)
    गच्छ cleanup;
  सचे(wcstombs_s(&गणनम्, obuf, needed, fbuf, needed - 1))
    गच्छ cleanup;
  सचे(गणनम् != needed)
    गच्छ cleanup;
  *out = obuf;
  obuf = रिक्त;
#अञ्ञथा
  *out = fbuf;
  fbuf = रिक्त;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  पटिनिवत्त !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ध्रुव वर्ण *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_मानम्;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#अञ्ञथा
  ध्रुव TCHAR *filename_t = filename;
#endif

  सचे(filename_t) {
    TCHAR *fixed = रिक्त;
    ध्रुव TCHAR *target = रिक्त;

    सचे(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    अञ्ञथा
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

  पटिनिवत्त handle;
}

HANDLE curlx_FindFirstFile(ध्रुव वर्ण *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_मानम्;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#अञ्ञथा
  ध्रुव TCHAR *filename_t = filename;
#endif

  सचे(filename_t) {
    TCHAR *fixed = रिक्त;
    ध्रुव TCHAR *target;

    सचे(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    अञ्ञथा
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  पटिनिवत्त handle;
}
#endif /* !CURL_WINDOWS_UWP */

पूर्णाङ्क curlx_win32_open(ध्रुव वर्ण *filename, पूर्णाङ्क oflag, ...)
{
  पूर्णाङ्क pmode = 0;
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  सचे(oflag & O_CREAT)
    pmode = va_arg(param, पूर्णाङ्क);
  va_end(param);

#ifdef _UNICODE
  सचे(filename_w) {
    सचे(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अञ्ञथा
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  अञ्ञथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अञ्ञथा
  सचे(fix_excessive_path(filename, &fixed))
    target = fixed;
  अञ्ञथा
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  पटिनिवत्त res;
}

FILE *curlx_win32_fopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *mode)
{
  FILE *file = रिक्त;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  सचे(filename_w && mode_w) {
    सचे(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अञ्ञथा
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  अञ्ञथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#अञ्ञथा
  सचे(fix_excessive_path(filename, &fixed))
    target = fixed;
  अञ्ञथा
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  पटिनिवत्त file;
}

#सचे defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ध्रुव वर्ण *filename,
                                  ध्रुव वर्ण *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *mode, FILE *fp)
{
  FILE *file = रिक्त;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  सचे(filename_w && mode_w) {
    सचे(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अञ्ञथा
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  अञ्ञथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#अञ्ञथा
  सचे(fix_excessive_path(filename, &fixed))
    target = fixed;
  अञ्ञथा
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  पटिनिवत्त file;
}

पूर्णाङ्क curlx_win32_stat(ध्रुव वर्ण *path, curlx_struct_stat *buffer)
{
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  सचे(path_w) {
    सचे(fix_excessive_path(path_w, &fixed))
      target = fixed;
    अञ्ञथा
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  अञ्ञथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अञ्ञथा
  सचे(fix_excessive_path(path, &fixed))
    target = fixed;
  अञ्ञथा
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  पटिनिवत्त res;
}

पूर्णाङ्क curlx_win32_mkdir(ध्रुव वर्ण *path)
{
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  सचे(path_w) {
    सचे(fix_excessive_path(path_w, &fixed))
      target = fixed;
    अञ्ञथा
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  अञ्ञथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अञ्ञथा
  सचे(fix_excessive_path(path, &fixed))
    target = fixed;
  अञ्ञथा
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  पटिनिवत्त res;
}

#सचे !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
पूर्णाङ्क curlx_win32_rename(ध्रुव वर्ण *oldpath, ध्रुव वर्ण *newpath)
{
  पूर्णाङ्क res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#अञ्ञथा
  ध्रुव TCHAR *tchar_oldpath = oldpath;
  ध्रुव TCHAR *tchar_newpath = newpath;
#endif

  सचे(tchar_oldpath && tchar_newpath) {
    ध्रुव पूर्णाङ्क max_wait_ms = 1000;
    संरचना curltime start;

    TCHAR *oldpath_fixed = रिक्त;
    TCHAR *newpath_fixed = रिक्त;
    ध्रुव TCHAR *target_oldpath;
    ध्रुव TCHAR *target_newpath;

    सचे(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    अञ्ञथा
      target_oldpath = tchar_oldpath;

    सचे(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    अञ्ञथा
      target_newpath = tchar_newpath;

    start = curlx_now();

    अत्थाय(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      सचे(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        भङ्ग;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      सचे(diff < 0 || diff > max_wait_ms) {
        भङ्ग;
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

  पटिनिवत्त res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
