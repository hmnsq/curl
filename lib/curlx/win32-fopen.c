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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"

ਪੂਰਨ curlx_fseek(ਖਾਲੀ *ਧਾਰਾ, curl_off_t ਵਿੱਥ, ਪੂਰਨ whence)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਵਾਪਸ _fseeki64(ਧਾਰਾ, (__int64)ਵਿੱਥ, whence);
#ਜਾਂਜੇ defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ਵਾਪਸ fseeko(ਧਾਰਾ, (off_t)ਵਿੱਥ, whence);
#ਨਹੀਂਤਾਂ
  ਜੇ(ਵਿੱਥ > LONG_MAX)
    ਵਾਪਸ -1;
  ਵਾਪਸ fseek(ਧਾਰਾ, (ਲੰਮਾ)ਵਿੱਥ, whence);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

#ਅਨ੍ਤਰ੍ਭਾਵ <direct.h>  /* for _mkdir(), _wmkdir() */
#ਅਨ੍ਤਰ੍ਭਾਵ <share.h>  /* for _SH_DENYNO */

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_MALLOC(x) ਸਮ੍ਰਿਤੀ_ਦਿਓ(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FREE(x)   ਮੁਕਤ(x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_MALLOC(x) curlx_malloc(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FREE(x)   curlx_free(x)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
ਸਥਿਰ wchar_t *fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਅਚਲ ਅੱਖਰ *str_utf8)
{
  wchar_t *str_w = ਨਿਰਾ;

  ਜੇ(str_utf8) {
    ਪੂਰਨ str_w_len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ਨਿਰਾ, 0);
    ਜੇ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * ਆਕਾਰ(wchar_t));
      ਜੇ(str_w) {
        ਜੇ(Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ਵਾਪਸ ਨਿਰਾ;
        }
      }
    }
  }
  ਵਾਪਸ str_w;
}
#ਜੇ_ਅੰਤ

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ਜੇ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_ਭਰਿਆ_ਰਸਤਾ_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ bool fix_excessive_ਰਸਤਾ(ਅਚਲ TCHAR *ਵਿੱਚ, TCHAR **ਬਾਹਰਲਾ)
{
  size_t needed, ਗਿਣਤੀ_2;
  ਅਚਲ wchar_t *ਵਿੱਚ_w;
  wchar_t *fbuf = ਨਿਰਾ;

  /* MS-documented "approximate" limit for the maximum path length */
  ਅਚਲ size_t max_ਰਸਤਾ_len = 32767;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *ibuf = ਨਿਰਾ;
  ਅੱਖਰ *obuf = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

  *ਬਾਹਰਲਾ = ਨਿਰਾ;

  /* skip paths already normalized */
  ਜੇ(!_tcsncmp(ਵਿੱਚ, ਪਾਠ("\\\\?\\"), 4))
    ਜਾਓ cleanup;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _UNICODE
  /* convert multibyte input to unicode */
  ਜੇ(mbstowcs_s(&needed, ਨਿਰਾ, 0, ਵਿੱਚ, 0))
    ਜਾਓ cleanup;
  ਜੇ(!needed || needed >= max_ਰਸਤਾ_len)
    ਜਾਓ cleanup;
  ibuf = CURLX_MALLOC(needed * ਆਕਾਰ(wchar_t));
  ਜੇ(!ibuf)
    ਜਾਓ cleanup;
  ਜੇ(mbstowcs_s(&ਗਿਣਤੀ_2, ibuf, needed, ਵਿੱਚ, needed - 1))
    ਜਾਓ cleanup;
  ਜੇ(ਗਿਣਤੀ_2 != needed)
    ਜਾਓ cleanup;
  ਵਿੱਚ_w = ibuf;
#ਨਹੀਂਤਾਂ
  ਵਿੱਚ_w = ਵਿੱਚ;
#ਜੇ_ਅੰਤ

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_ਭਰਿਆ_ਰਸਤਾ_Name_W(ਵਿੱਚ_w, 0, ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(!needed || needed > max_ਰਸਤਾ_len)
    ਜਾਓ cleanup;
  /* skip paths that are not excessive and do not need modification */
  ਜੇ(needed <= MAX_ਰਸਤਾ)
    ਜਾਓ cleanup;
  fbuf = CURLX_MALLOC(needed * ਆਕਾਰ(wchar_t));
  ਜੇ(!fbuf)
    ਜਾਓ cleanup;
  ਗਿਣਤੀ_2 = (size_t)Get_ਭਰਿਆ_ਰਸਤਾ_Name_W(ਵਿੱਚ_w, (DWORD)needed, fbuf, ਨਿਰਾ);
  ਜੇ(!ਗਿਣਤੀ_2 || ਗਿਣਤੀ_2 >= needed)
    ਜਾਓ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ਜੇ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  ਨਹੀਂਤਾਂ ਜੇ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ਨਹੀਂਤਾਂ ਜੇ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    ਜਾਓ cleanup;
  }
  ਨਹੀਂਤਾਂ {
    wchar_t *ਆਰਜ਼ੀ;

    ਜੇ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ਗਿਣਤੀ_2 - 2) + 1;
      ਜੇ(needed > max_ਰਸਤਾ_len)
        ਜਾਓ cleanup;

      ਆਰਜ਼ੀ = CURLX_MALLOC(needed * ਆਕਾਰ(wchar_t));
      ਜੇ(!ਆਰਜ਼ੀ)
        ਜਾਓ cleanup;

      ਜੇ(wcsncpy_s(ਆਰਜ਼ੀ, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(ਆਰਜ਼ੀ);
        ਜਾਓ cleanup;
      }
      ਜੇ(wcscpy_s(ਆਰਜ਼ੀ + 8, needed, fbuf + 2)) {
        CURLX_FREE(ਆਰਜ਼ੀ);
        ਜਾਓ cleanup;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* "\\?\" + full path + null */
      needed = 4 + ਗਿਣਤੀ_2 + 1;
      ਜੇ(needed > max_ਰਸਤਾ_len)
        ਜਾਓ cleanup;

      ਆਰਜ਼ੀ = CURLX_MALLOC(needed * ਆਕਾਰ(wchar_t));
      ਜੇ(!ਆਰਜ਼ੀ)
        ਜਾਓ cleanup;

      ਜੇ(wcsncpy_s(ਆਰਜ਼ੀ, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(ਆਰਜ਼ੀ);
        ਜਾਓ cleanup;
      }
      ਜੇ(wcscpy_s(ਆਰਜ਼ੀ + 4, needed, fbuf)) {
        CURLX_FREE(ਆਰਜ਼ੀ);
        ਜਾਓ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = ਆਰਜ਼ੀ;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _UNICODE
  /* convert unicode full path to multibyte output */
  ਜੇ(wcstombs_s(&needed, ਨਿਰਾ, 0, fbuf, 0))
    ਜਾਓ cleanup;
  ਜੇ(!needed || needed >= max_ਰਸਤਾ_len)
    ਜਾਓ cleanup;
  obuf = CURLX_MALLOC(needed);
  ਜੇ(!obuf)
    ਜਾਓ cleanup;
  ਜੇ(wcstombs_s(&ਗਿਣਤੀ_2, obuf, needed, fbuf, needed - 1))
    ਜਾਓ cleanup;
  ਜੇ(ਗਿਣਤੀ_2 != needed)
    ਜਾਓ cleanup;
  *ਬਾਹਰਲਾ = obuf;
  obuf = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
  *ਬਾਹਰਲਾ = fbuf;
  fbuf = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

cleanup:
  CURLX_FREE(fbuf);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ !!*ਬਾਹਰਲਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ਅਚਲ ਅੱਖਰ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ਬਹੁਲਕ,
                        LPSECURITY_ਗੁਣ lp_Security_ਗੁਣ,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ਗੁਣ,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_ਮੁੱਲ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(filename);
#ਨਹੀਂਤਾਂ
  ਅਚਲ TCHAR *filename_t = filename;
#ਜੇ_ਅੰਤ

  ਜੇ(filename_t) {
    TCHAR *fixed = ਨਿਰਾ;
    ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

    ਜੇ(fix_excessive_ਰਸਤਾ(filename_t, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(ਨਿਸ਼ਾਨਾ,
                        dwDesiredAccess,
                        dw_Share_ਬਹੁਲਕ,
                        lp_Security_ਗੁਣ,
                        dwCreationDisposition,
                        dw_Flags_And_ਗੁਣ,
                        hTemplateFile);
    CURLX_FREE(fixed);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
    curlx_free(filename_t);
#ਜੇ_ਅੰਤ
  }

  ਵਾਪਸ handle;
}

HANDLE curlx_ਲੱਭੋ_ਪਹਿਲਾ_File(ਅਚਲ ਅੱਖਰ *filename,
                           WIN32_ਲੱਭੋ_ਅੰਕੜੇ *ਲੱਭੋ_ਅੰਕੜੇ)
{
  HANDLE handle = INVALID_HANDLE_ਮੁੱਲ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(filename);
#ਨਹੀਂਤਾਂ
  ਅਚਲ TCHAR *filename_t = filename;
#ਜੇ_ਅੰਤ

  ਜੇ(filename_t) {
    TCHAR *fixed = ਨਿਰਾ;
    ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ;

    ਜੇ(fix_excessive_ਰਸਤਾ(filename_t, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = filename_t;

    handle = ਲੱਭੋ_ਪਹਿਲਾ_File(ਨਿਸ਼ਾਨਾ, ਲੱਭੋ_ਅੰਕੜੇ);
    CURLX_FREE(fixed);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
    curlx_free(filename_t);
#ਜੇ_ਅੰਤ
  }

  ਵਾਪਸ handle;
}
#ਜੇ_ਅੰਤ /* !CURL_WINDOWS_UWP */

ਪੂਰਨ curlx_win32_open(ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ oflag, ...)
{
  ਪੂਰਨ pmode = 0;
  ਪੂਰਨ res = -1;
  TCHAR *fixed = ਨਿਰਾ;
  ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(filename);
#ਜੇ_ਅੰਤ

  va_list param;
  va_start(param, oflag);
  ਜੇ(oflag & O_CREAT)
    pmode = va_arg(param, ਪੂਰਨ);
  va_end(param);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  ਜੇ(filename_w) {
    ਜੇ(fix_excessive_ਰਸਤਾ(filename_w, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = filename_w;
    errno = _wsopen_s(&res, ਨਿਸ਼ਾਨਾ, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ਨਹੀਂਤਾਂ
  ਜੇ(fix_excessive_ਰਸਤਾ(filename, &fixed))
    ਨਿਸ਼ਾਨਾ = fixed;
  ਨਹੀਂਤਾਂ
    ਨਿਸ਼ਾਨਾ = filename;
  errno = _sopen_s(&res, ਨਿਸ਼ਾਨਾ, oflag, _SH_DENYNO, pmode);
#ਜੇ_ਅੰਤ

  CURLX_FREE(fixed);
  ਵਾਪਸ res;
}

FILE *curlx_win32_fopen(ਅਚਲ ਅੱਖਰ *filename, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ)
{
  FILE *file = ਨਿਰਾ;
  TCHAR *fixed = ਨਿਰਾ;
  ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(filename);
  wchar_t *ਬਹੁਲਕ_w = fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਬਹੁਲਕ);
  ਜੇ(filename_w && ਬਹੁਲਕ_w) {
    ਜੇ(fix_excessive_ਰਸਤਾ(filename_w, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = filename_w;
    file = _wfsopen(ਨਿਸ਼ਾਨਾ, ਬਹੁਲਕ_w, _SH_DENYNO);
  }
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ਬਹੁਲਕ_w);
#ਨਹੀਂਤਾਂ
  ਜੇ(fix_excessive_ਰਸਤਾ(filename, &fixed))
    ਨਿਸ਼ਾਨਾ = fixed;
  ਨਹੀਂਤਾਂ
    ਨਿਸ਼ਾਨਾ = filename;
  file = _fsopen(ਨਿਸ਼ਾਨਾ, ਬਹੁਲਕ, _SH_DENYNO);
#ਜੇ_ਅੰਤ

  CURLX_FREE(fixed);
  ਵਾਪਸ file;
}

#ਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ਅਚਲ ਅੱਖਰ *filename,
                                  ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE *ਧਾਰਾ);
#ਜੇ_ਅੰਤ

FILE *curlx_win32_freopen(ਅਚਲ ਅੱਖਰ *filename, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE *fp)
{
  FILE *file = ਨਿਰਾ;
  TCHAR *fixed = ਨਿਰਾ;
  ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(filename);
  wchar_t *ਬਹੁਲਕ_w = fn_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਬਹੁਲਕ);
  ਜੇ(filename_w && ਬਹੁਲਕ_w) {
    ਜੇ(fix_excessive_ਰਸਤਾ(filename_w, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = filename_w;
    errno = _wfreopen_s(&file, ਨਿਸ਼ਾਨਾ, ਬਹੁਲਕ_w, fp);
  }
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ਬਹੁਲਕ_w);
#ਨਹੀਂਤਾਂ
  ਜੇ(fix_excessive_ਰਸਤਾ(filename, &fixed))
    ਨਿਸ਼ਾਨਾ = fixed;
  ਨਹੀਂਤਾਂ
    ਨਿਸ਼ਾਨਾ = filename;
  errno = freopen_s(&file, ਨਿਸ਼ਾਨਾ, ਬਹੁਲਕ, fp);
#ਜੇ_ਅੰਤ

  CURLX_FREE(fixed);
  ਵਾਪਸ file;
}

ਪੂਰਨ curlx_win32_stat(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, curlx_struct_stat *ਬਫਰ)
{
  ਪੂਰਨ res = -1;
  TCHAR *fixed = ਨਿਰਾ;
  ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *ਰਸਤਾ_w = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਰਸਤਾ);
  ਜੇ(ਰਸਤਾ_w) {
    ਜੇ(fix_excessive_ਰਸਤਾ(ਰਸਤਾ_w, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = ਰਸਤਾ_w;
    res = _wstati64(ਨਿਸ਼ਾਨਾ, ਬਫਰ);
    curlx_free(ਰਸਤਾ_w);
  }
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ਨਹੀਂਤਾਂ
  ਜੇ(fix_excessive_ਰਸਤਾ(ਰਸਤਾ, &fixed))
    ਨਿਸ਼ਾਨਾ = fixed;
  ਨਹੀਂਤਾਂ
    ਨਿਸ਼ਾਨਾ = ਰਸਤਾ;
  res = _stati64(ਨਿਸ਼ਾਨਾ, ਬਫਰ);
#ਜੇ_ਅੰਤ

  CURLX_FREE(fixed);
  ਵਾਪਸ res;
}

ਪੂਰਨ curlx_win32_mkdir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
  ਪੂਰਨ res = -1;
  TCHAR *fixed = ਨਿਰਾ;
  ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UNICODE
  wchar_t *ਰਸਤਾ_w = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(ਰਸਤਾ);
  ਜੇ(ਰਸਤਾ_w) {
    ਜੇ(fix_excessive_ਰਸਤਾ(ਰਸਤਾ_w, &fixed))
      ਨਿਸ਼ਾਨਾ = fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ = ਰਸਤਾ_w;
    res = _wmkdir(ਨਿਸ਼ਾਨਾ);
    curlx_free(ਰਸਤਾ_w);
  }
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ਨਹੀਂਤਾਂ
  ਜੇ(fix_excessive_ਰਸਤਾ(ਰਸਤਾ, &fixed))
    ਨਿਸ਼ਾਨਾ = fixed;
  ਨਹੀਂਤਾਂ
    ਨਿਸ਼ਾਨਾ = ਰਸਤਾ;
  res = _mkdir(ਨਿਸ਼ਾਨਾ);
#ਜੇ_ਅੰਤ

  CURLX_FREE(fixed);
  ਵਾਪਸ res;
}

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
ਪੂਰਨ curlx_win32_rename(ਅਚਲ ਅੱਖਰ *oldpath, ਅਚਲ ਅੱਖਰ *newpath)
{
  ਪੂਰਨ res = -1; /* fail */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_wchar(newpath);
#ਨਹੀਂਤਾਂ
  ਅਚਲ TCHAR *tchar_oldpath = oldpath;
  ਅਚਲ TCHAR *tchar_newpath = newpath;
#ਜੇ_ਅੰਤ

  ਜੇ(tchar_oldpath && tchar_newpath) {
    ਅਚਲ ਪੂਰਨ max_wait_ms = 1000;
    ਬਣਤਰ curltime ਸ਼ੁਰੂ;

    TCHAR *oldpath_fixed = ਨਿਰਾ;
    TCHAR *newpath_fixed = ਨਿਰਾ;
    ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ_oldpath;
    ਅਚਲ TCHAR *ਨਿਸ਼ਾਨਾ_newpath;

    ਜੇ(fix_excessive_ਰਸਤਾ(tchar_oldpath, &oldpath_fixed))
      ਨਿਸ਼ਾਨਾ_oldpath = oldpath_fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ_oldpath = tchar_oldpath;

    ਜੇ(fix_excessive_ਰਸਤਾ(tchar_newpath, &newpath_fixed))
      ਨਿਸ਼ਾਨਾ_newpath = newpath_fixed;
    ਨਹੀਂਤਾਂ
      ਨਿਸ਼ਾਨਾ_newpath = tchar_newpath;

    ਸ਼ੁਰੂ = curlx_now();

    ਲਈ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ਜੇ(MoveFileEx(ਨਿਸ਼ਾਨਾ_oldpath, ਨਿਸ਼ਾਨਾ_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ਰੋਕੋ;
      }
      diff = curlx_timediff_ms(curlx_now(), ਸ਼ੁਰੂ);
      ਜੇ(diff < 0 || diff > max_wait_ms) {
        ਰੋਕੋ;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CURLX_MALLOC
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CURLX_FREE

#ਜੇ_ਅੰਤ /* _WIN32 */
