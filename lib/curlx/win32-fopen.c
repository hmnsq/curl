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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curlx/win32-fopen.h"

પૂર્ણાંક curlx_fseek(શૂન્ય *stream, curl_off_t offset, પૂર્ણાંક whence)
{
#ifdef _WIN32
  પાછા _fseeki64(stream, (__int64)offset, whence);
#અથવાજો defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  પાછા fseeko(stream, (off_t)offset, whence);
#નહીંતર
  જો(offset > LONG_MAX)
    પાછા -1;
  પાછા fseek(stream, (long)offset, whence);
#endif
}

#ifdef _WIN32

#અન્તર્ભાવ <direct.h>  /* for _mkdir(), _wmkdir() */
#અન્તર્ભાવ <share.h>  /* for _SH_DENYNO */

#અન્તર્ભાવ "curlx/multibyte.h"
#અન્તર્ભાવ "curlx/timeval.h"

#ifdef CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#define CURLX_MALLOC(x) malloc(x)
#define CURLX_FREE(x)   મુક્ત(x)
#નહીંતર
#define CURLX_MALLOC(x) curlx_malloc(x)
#define CURLX_FREE(x)   curlx_free(x)
#endif

#ifdef _UNICODE
સ્થિર wchar_t *fn_convert_UTF8_to_wchar(અચલ અક્ષર *str_utf8)
{
  wchar_t *str_w = ખાલી;

  જો(str_utf8) {
    પૂર્ણાંક str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ખાલી, 0);
    જો(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * કદ(wchar_t));
      જો(str_w) {
        જો(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          પાછા ખાલી;
        }
      }
    }
  }
  પાછા str_w;
}
#endif

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#જો defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
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
સ્થિર bool fix_excessive_path(અચલ TCHAR *in, TCHAR **out)
{
  size_t needed, ગણતરી;
  અચલ wchar_t *in_w;
  wchar_t *fbuf = ખાલી;

  /* MS-documented "approximate" limit for the maximum path length */
  અચલ size_t max_path_len = 32767;

#ifndef _UNICODE
  wchar_t *ibuf = ખાલી;
  અક્ષર *obuf = ખાલી;
#endif

  *out = ખાલી;

  /* skip paths already normalized */
  જો(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    જાઓ cleanup;

#ifndef _UNICODE
  /* convert multibyte input to unicode */
  જો(mbstowcs_s(&needed, ખાલી, 0, in, 0))
    જાઓ cleanup;
  જો(!needed || needed >= max_path_len)
    જાઓ cleanup;
  ibuf = CURLX_MALLOC(needed * કદ(wchar_t));
  જો(!ibuf)
    જાઓ cleanup;
  જો(mbstowcs_s(&ગણતરી, ibuf, needed, in, needed - 1))
    જાઓ cleanup;
  જો(ગણતરી != needed)
    જાઓ cleanup;
  in_w = ibuf;
#નહીંતર
  in_w = in;
#endif

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)GetFullPathNameW(in_w, 0, ખાલી, ખાલી);
  જો(!needed || needed > max_path_len)
    જાઓ cleanup;
  /* skip paths that are not excessive and do not need modification */
  જો(needed <= MAX_PATH)
    જાઓ cleanup;
  fbuf = CURLX_MALLOC(needed * કદ(wchar_t));
  જો(!fbuf)
    જાઓ cleanup;
  ગણતરી = (size_t)GetFullPathNameW(in_w, (DWORD)needed, fbuf, ખાલી);
  જો(!ગણતરી || ગણતરી >= needed)
    જાઓ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  જો(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  નહીંતર જો(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  નહીંતર જો(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    જાઓ cleanup;
  }
  નહીંતર {
    wchar_t *temp;

    જો(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ગણતરી - 2) + 1;
      જો(needed > max_path_len)
        જાઓ cleanup;

      temp = CURLX_MALLOC(needed * કદ(wchar_t));
      જો(!temp)
        જાઓ cleanup;

      જો(wcsncpy_s(temp, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(temp);
        જાઓ cleanup;
      }
      જો(wcscpy_s(temp + 8, needed, fbuf + 2)) {
        CURLX_FREE(temp);
        જાઓ cleanup;
      }
    }
    નહીંતર {
      /* "\\?\" + full path + null */
      needed = 4 + ગણતરી + 1;
      જો(needed > max_path_len)
        જાઓ cleanup;

      temp = CURLX_MALLOC(needed * કદ(wchar_t));
      જો(!temp)
        જાઓ cleanup;

      જો(wcsncpy_s(temp, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(temp);
        જાઓ cleanup;
      }
      જો(wcscpy_s(temp + 4, needed, fbuf)) {
        CURLX_FREE(temp);
        જાઓ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = temp;
  }

#ifndef _UNICODE
  /* convert unicode full path to multibyte output */
  જો(wcstombs_s(&needed, ખાલી, 0, fbuf, 0))
    જાઓ cleanup;
  જો(!needed || needed >= max_path_len)
    જાઓ cleanup;
  obuf = CURLX_MALLOC(needed);
  જો(!obuf)
    જાઓ cleanup;
  જો(wcstombs_s(&ગણતરી, obuf, needed, fbuf, needed - 1))
    જાઓ cleanup;
  જો(ગણતરી != needed)
    જાઓ cleanup;
  *out = obuf;
  obuf = ખાલી;
#નહીંતર
  *out = fbuf;
  fbuf = ખાલી;
#endif

cleanup:
  CURLX_FREE(fbuf);
#ifndef _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#endif
  પાછા !!*out;
}

#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(અચલ અક્ષર *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_કિંમત;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#નહીંતર
  અચલ TCHAR *filename_t = filename;
#endif

  જો(filename_t) {
    TCHAR *fixed = ખાલી;
    અચલ TCHAR *target = ખાલી;

    જો(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    નહીંતર
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

  પાછા handle;
}

HANDLE curlx_FindFirstFile(અચલ અક્ષર *filename,
                           WIN32_FIND_DATA *find_data)
{
  HANDLE handle = INVALID_HANDLE_કિંમત;

#ifdef UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#નહીંતર
  અચલ TCHAR *filename_t = filename;
#endif

  જો(filename_t) {
    TCHAR *fixed = ખાલી;
    અચલ TCHAR *target;

    જો(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    નહીંતર
      target = filename_t;

    handle = FindFirstFile(target, find_data);
    CURLX_FREE(fixed);

#ifdef UNICODE
    curlx_free(filename_t);
#endif
  }

  પાછા handle;
}
#endif /* !CURL_WINDOWS_UWP */

પૂર્ણાંક curlx_win32_open(અચલ અક્ષર *filename, પૂર્ણાંક oflag, ...)
{
  પૂર્ણાંક pmode = 0;
  પૂર્ણાંક res = -1;
  TCHAR *fixed = ખાલી;
  અચલ TCHAR *target = ખાલી;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
#endif

  va_list param;
  va_start(param, oflag);
  જો(oflag & O_CREAT)
    pmode = va_arg(param, પૂર્ણાંક);
  va_end(param);

#ifdef _UNICODE
  જો(filename_w) {
    જો(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    નહીંતર
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  નહીંતર
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#નહીંતર
  જો(fix_excessive_path(filename, &fixed))
    target = fixed;
  નહીંતર
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#endif

  CURLX_FREE(fixed);
  પાછા res;
}

FILE *curlx_win32_fopen(અચલ અક્ષર *filename, અચલ અક્ષર *mode)
{
  FILE *file = ખાલી;
  TCHAR *fixed = ખાલી;
  અચલ TCHAR *target = ખાલી;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  જો(filename_w && mode_w) {
    જો(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    નહીંતર
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  નહીંતર
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#નહીંતર
  જો(fix_excessive_path(filename, &fixed))
    target = fixed;
  નહીંતર
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#endif

  CURLX_FREE(fixed);
  પાછા file;
}

#જો defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, અચલ અક્ષર *filename,
                                  અચલ અક્ષર *mode, FILE *stream);
#endif

FILE *curlx_win32_freopen(અચલ અક્ષર *filename, અચલ અક્ષર *mode, FILE *fp)
{
  FILE *file = ખાલી;
  TCHAR *fixed = ખાલી;
  અચલ TCHAR *target = ખાલી;

#ifdef _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_to_wchar(filename);
  wchar_t *mode_w = fn_convert_UTF8_to_wchar(mode);
  જો(filename_w && mode_w) {
    જો(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    નહીંતર
      target = filename_w;
    errno = _wfreopen_s(&file, target, mode_w, fp);
  }
  નહીંતર
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#નહીંતર
  જો(fix_excessive_path(filename, &fixed))
    target = fixed;
  નહીંતર
    target = filename;
  errno = freopen_s(&file, target, mode, fp);
#endif

  CURLX_FREE(fixed);
  પાછા file;
}

પૂર્ણાંક curlx_win32_stat(અચલ અક્ષર *path, curlx_struct_stat *buffer)
{
  પૂર્ણાંક res = -1;
  TCHAR *fixed = ખાલી;
  અચલ TCHAR *target = ખાલી;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  જો(path_w) {
    જો(fix_excessive_path(path_w, &fixed))
      target = fixed;
    નહીંતર
      target = path_w;
    res = _wstati64(target, buffer);
    curlx_free(path_w);
  }
  નહીંતર
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#નહીંતર
  જો(fix_excessive_path(path, &fixed))
    target = fixed;
  નહીંતર
    target = path;
  res = _stati64(target, buffer);
#endif

  CURLX_FREE(fixed);
  પાછા res;
}

પૂર્ણાંક curlx_win32_mkdir(અચલ અક્ષર *path)
{
  પૂર્ણાંક res = -1;
  TCHAR *fixed = ખાલી;
  અચલ TCHAR *target = ખાલી;

#ifdef _UNICODE
  wchar_t *path_w = curlx_convert_UTF8_to_wchar(path);
  જો(path_w) {
    જો(fix_excessive_path(path_w, &fixed))
      target = fixed;
    નહીંતર
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  નહીંતર
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#નહીંતર
  જો(fix_excessive_path(path, &fixed))
    target = fixed;
  નહીંતર
    target = path;
  res = _mkdir(target);
#endif

  CURLX_FREE(fixed);
  પાછા res;
}

#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
પૂર્ણાંક curlx_win32_rename(અચલ અક્ષર *oldpath, અચલ અક્ષર *newpath)
{
  પૂર્ણાંક res = -1; /* fail */

#ifdef UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#નહીંતર
  અચલ TCHAR *tchar_oldpath = oldpath;
  અચલ TCHAR *tchar_newpath = newpath;
#endif

  જો(tchar_oldpath && tchar_newpath) {
    અચલ પૂર્ણાંક max_wait_ms = 1000;
    સંરચના curltime start;

    TCHAR *oldpath_fixed = ખાલી;
    TCHAR *newpath_fixed = ખાલી;
    અચલ TCHAR *target_oldpath;
    અચલ TCHAR *target_newpath;

    જો(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    નહીંતર
      target_oldpath = tchar_oldpath;

    જો(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    નહીંતર
      target_newpath = tchar_newpath;

    start = curlx_now();

    માટે(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      જો(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        તોડો;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      જો(diff < 0 || diff > max_wait_ms) {
        તોડો;
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

  પાછા res;
}
#endif

#undef CURLX_MALLOC
#undef CURLX_FREE

#endif /* _WIN32 */
