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

صحيح curlx_fseek(فراغ *بهیر, curl_off_t دننه_واټن, صحيح whence)
{
#که_تعریف_شوی _WIN32
  أرجع _fseeki64(بهیر, (__int64)دننه_واټن, whence);
#وإلاإذا defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  أرجع fseeko(بهیر, (off_t)دننه_واټن, whence);
#وإلا
  إذا(دننه_واټن > LONG_MAX)
    أرجع -1;
  أرجع fseek(بهیر, (اوږد)دننه_واټن, whence);
#د_که_پای
}

#که_تعریف_شوی _WIN32

#ضمن <direct.h>  /* for _mkdir(), _wmkdir() */
#ضمن <share.h>  /* for _SH_DENYNO */

#ضمن "curlx/multibyte.h"
#ضمن "curlx/timeval.h"

#که_تعریف_شوی CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#تعریف_کړه CURLX_MALLOC(x) حافظه_ورکړه(x)
#تعریف_کړه CURLX_FREE(x)   حرر(x)
#وإلا
#تعریف_کړه CURLX_MALLOC(x) curlx_malloc(x)
#تعریف_کړه CURLX_FREE(x)   curlx_free(x)
#د_که_پای

#که_تعریف_شوی _UNICODE
ساكن wchar_t *fn_convert_UTF8_منزل_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = عدم;

  إذا(str_utf8) {
    صحيح str_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, عدم, 0);
    إذا(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * حجم(wchar_t));
      إذا(str_w) {
        إذا(Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          أرجع عدم;
        }
      }
    }
  }
  أرجع str_w;
}
#د_که_پای

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#إذا defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_ډک_لار_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#د_که_پای

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
ساكن bool fix_excessive_لار(ثابت TCHAR *دننه, TCHAR **بهر)
{
  size_t needed, شمېر;
  ثابت wchar_t *دننه_w;
  wchar_t *fbuf = عدم;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_لار_len = 32767;

#که_نه_تعریف_شوی _UNICODE
  wchar_t *ibuf = عدم;
  حرف *obuf = عدم;
#د_که_پای

  *بهر = عدم;

  /* skip paths already normalized */
  إذا(!_tcsncmp(دننه, متن("\\\\?\\"), 4))
    اذهب cleanup;

#که_نه_تعریف_شوی _UNICODE
  /* convert multibyte input to unicode */
  إذا(mbstowcs_s(&needed, عدم, 0, دننه, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_لار_len)
    اذهب cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!ibuf)
    اذهب cleanup;
  إذا(mbstowcs_s(&شمېر, ibuf, needed, دننه, needed - 1))
    اذهب cleanup;
  إذا(شمېر != needed)
    اذهب cleanup;
  دننه_w = ibuf;
#وإلا
  دننه_w = دننه;
#د_که_پای

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_ډک_لار_Name_W(دننه_w, 0, عدم, عدم);
  إذا(!needed || needed > max_لار_len)
    اذهب cleanup;
  /* skip paths that are not excessive and do not need modification */
  إذا(needed <= MAX_لار)
    اذهب cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!fbuf)
    اذهب cleanup;
  شمېر = (size_t)Get_ډک_لار_Name_W(دننه_w, (DWORD)needed, fbuf, عدم);
  إذا(!شمېر || شمېر >= needed)
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
    wchar_t *لنډمهاله;

    إذا(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (شمېر - 2) + 1;
      إذا(needed > max_لار_len)
        اذهب cleanup;

      لنډمهاله = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!لنډمهاله)
        اذهب cleanup;

      إذا(wcsncpy_s(لنډمهاله, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(لنډمهاله);
        اذهب cleanup;
      }
      إذا(wcscpy_s(لنډمهاله + 8, needed, fbuf + 2)) {
        CURLX_FREE(لنډمهاله);
        اذهب cleanup;
      }
    }
    وإلا {
      /* "\\?\" + full path + null */
      needed = 4 + شمېر + 1;
      إذا(needed > max_لار_len)
        اذهب cleanup;

      لنډمهاله = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!لنډمهاله)
        اذهب cleanup;

      إذا(wcsncpy_s(لنډمهاله, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(لنډمهاله);
        اذهب cleanup;
      }
      إذا(wcscpy_s(لنډمهاله + 4, needed, fbuf)) {
        CURLX_FREE(لنډمهاله);
        اذهب cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = لنډمهاله;
  }

#که_نه_تعریف_شوی _UNICODE
  /* convert unicode full path to multibyte output */
  إذا(wcstombs_s(&needed, عدم, 0, fbuf, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_لار_len)
    اذهب cleanup;
  obuf = CURLX_MALLOC(needed);
  إذا(!obuf)
    اذهب cleanup;
  إذا(wcstombs_s(&شمېر, obuf, needed, fbuf, needed - 1))
    اذهب cleanup;
  إذا(شمېر != needed)
    اذهب cleanup;
  *بهر = obuf;
  obuf = عدم;
#وإلا
  *بهر = fbuf;
  fbuf = عدم;
#د_که_پای

cleanup:
  CURLX_FREE(fbuf);
#که_نه_تعریف_شوی _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#د_که_پای
  أرجع !!*بهر;
}

#که_نه_تعریف_شوی CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ډېرپېښ,
                        LPSECURITY_ځانګړتیاوې lp_Security_ځانګړتیاوې,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ځانګړتیاوې,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_ارزښت;

#که_تعریف_شوی UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#د_که_پای

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *موخه = عدم;

    إذا(fix_excessive_لار(filename_t, &fixed))
      موخه = fixed;
    وإلا
      موخه = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(موخه,
                        dwDesiredAccess,
                        dw_Share_ډېرپېښ,
                        lp_Security_ځانګړتیاوې,
                        dwCreationDisposition,
                        dw_Flags_And_ځانګړتیاوې,
                        hTemplateFile);
    CURLX_FREE(fixed);
#که_تعریف_شوی UNICODE
    curlx_free(filename_t);
#د_که_پای
  }

  أرجع handle;
}

HANDLE curlx_پیدا_کړه_لومړی_File(ثابت حرف *filename,
                           WIN32_پیدا_کړه_معلومات *پیدا_کړه_معلومات)
{
  HANDLE handle = INVALID_HANDLE_ارزښت;

#که_تعریف_شوی UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#د_که_پای

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *موخه;

    إذا(fix_excessive_لار(filename_t, &fixed))
      موخه = fixed;
    وإلا
      موخه = filename_t;

    handle = پیدا_کړه_لومړی_File(موخه, پیدا_کړه_معلومات);
    CURLX_FREE(fixed);

#که_تعریف_شوی UNICODE
    curlx_free(filename_t);
#د_که_پای
  }

  أرجع handle;
}
#د_که_پای /* !CURL_WINDOWS_UWP */

صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...)
{
  صحيح pmode = 0;
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *موخه = عدم;

#که_تعریف_شوی _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
#د_که_پای

  va_list param;
  va_start(param, oflag);
  إذا(oflag & O_CREAT)
    pmode = va_arg(param, صحيح);
  va_end(param);

#که_تعریف_شوی _UNICODE
  إذا(filename_w) {
    إذا(fix_excessive_لار(filename_w, &fixed))
      موخه = fixed;
    وإلا
      موخه = filename_w;
    errno = _wsopen_s(&res, موخه, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_لار(filename, &fixed))
    موخه = fixed;
  وإلا
    موخه = filename;
  errno = _sopen_s(&res, موخه, oflag, _SH_DENYNO, pmode);
#د_که_پای

  CURLX_FREE(fixed);
  أرجع res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *ډېرپېښ)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *موخه = عدم;

#که_تعریف_شوی _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *ډېرپېښ_w = fn_convert_UTF8_منزل_wchar(ډېرپېښ);
  إذا(filename_w && ډېرپېښ_w) {
    إذا(fix_excessive_لار(filename_w, &fixed))
      موخه = fixed;
    وإلا
      موخه = filename_w;
    file = _wfsopen(موخه, ډېرپېښ_w, _SH_DENYNO);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ډېرپېښ_w);
#وإلا
  إذا(fix_excessive_لار(filename, &fixed))
    موخه = fixed;
  وإلا
    موخه = filename;
  file = _fsopen(موخه, ډېرپېښ, _SH_DENYNO);
#د_که_پای

  CURLX_FREE(fixed);
  أرجع file;
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *ډېرپېښ, FILE *بهیر);
#د_که_پای

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *ډېرپېښ, FILE *fp)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *موخه = عدم;

#که_تعریف_شوی _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *ډېرپېښ_w = fn_convert_UTF8_منزل_wchar(ډېرپېښ);
  إذا(filename_w && ډېرپېښ_w) {
    إذا(fix_excessive_لار(filename_w, &fixed))
      موخه = fixed;
    وإلا
      موخه = filename_w;
    errno = _wfreopen_s(&file, موخه, ډېرپېښ_w, fp);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ډېرپېښ_w);
#وإلا
  إذا(fix_excessive_لار(filename, &fixed))
    موخه = fixed;
  وإلا
    موخه = filename;
  errno = freopen_s(&file, موخه, ډېرپېښ, fp);
#د_که_پای

  CURLX_FREE(fixed);
  أرجع file;
}

صحيح curlx_win32_stat(ثابت حرف *لار, curlx_struct_stat *بفر)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *موخه = عدم;

#که_تعریف_شوی _UNICODE
  wchar_t *لار_w = curlx_convert_UTF8_منزل_wchar(لار);
  إذا(لار_w) {
    إذا(fix_excessive_لار(لار_w, &fixed))
      موخه = fixed;
    وإلا
      موخه = لار_w;
    res = _wstati64(موخه, بفر);
    curlx_free(لار_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_لار(لار, &fixed))
    موخه = fixed;
  وإلا
    موخه = لار;
  res = _stati64(موخه, بفر);
#د_که_پای

  CURLX_FREE(fixed);
  أرجع res;
}

صحيح curlx_win32_mkdir(ثابت حرف *لار)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *موخه = عدم;

#که_تعریف_شوی _UNICODE
  wchar_t *لار_w = curlx_convert_UTF8_منزل_wchar(لار);
  إذا(لار_w) {
    إذا(fix_excessive_لار(لار_w, &fixed))
      موخه = fixed;
    وإلا
      موخه = لار_w;
    res = _wmkdir(موخه);
    curlx_free(لار_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_لار(لار, &fixed))
    موخه = fixed;
  وإلا
    موخه = لار;
  res = _mkdir(موخه);
#د_که_پای

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

#که_تعریف_شوی UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_منزل_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_منزل_wchar(newpath);
#وإلا
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#د_که_پای

  إذا(tchar_oldpath && tchar_newpath) {
    ثابت صحيح max_wait_ms = 1000;
    بنية curltime پیل;

    TCHAR *oldpath_fixed = عدم;
    TCHAR *newpath_fixed = عدم;
    ثابت TCHAR *موخه_oldpath;
    ثابت TCHAR *موخه_newpath;

    إذا(fix_excessive_لار(tchar_oldpath, &oldpath_fixed))
      موخه_oldpath = oldpath_fixed;
    وإلا
      موخه_oldpath = tchar_oldpath;

    إذا(fix_excessive_لار(tchar_newpath, &newpath_fixed))
      موخه_newpath = newpath_fixed;
    وإلا
      موخه_newpath = tchar_newpath;

    پیل = curlx_now();

    لكل(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      إذا(MoveFileEx(موخه_oldpath, موخه_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        اكسر;
      }
      diff = curlx_timediff_ms(curlx_now(), پیل);
      إذا(diff < 0 || diff > max_wait_ms) {
        اكسر;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#که_تعریف_شوی UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#د_که_پای

  أرجع res;
}
#د_که_پای

#تعریف_لغوه CURLX_MALLOC
#تعریف_لغوه CURLX_FREE

#د_که_پای /* _WIN32 */
