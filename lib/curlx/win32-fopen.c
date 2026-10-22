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

صحيح curlx_fseek(فراغ *دھارا, curl_off_t وتھ, صحيح whence)
{
#جے_تعریف_شدہ _WIN32
  أرجع _fseeki64(دھارا, (__int64)وتھ, whence);
#وإلاإذا defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  أرجع fseeko(دھارا, (off_t)وتھ, whence);
#وإلا
  إذا(وتھ > LONG_MAX)
    أرجع -1;
  أرجع fseek(دھارا, (لما)وتھ, whence);
#جے_انت
}

#جے_تعریف_شدہ _WIN32

#ضمن <direct.h>  /* for _mkdir(), _wmkdir() */
#ضمن <share.h>  /* for _SH_DENYNO */

#ضمن "curlx/multibyte.h"
#ضمن "curlx/timeval.h"

#جے_تعریف_شدہ CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#تعریف_کرو CURLX_MALLOC(x) حافظہ_دیو(x)
#تعریف_کرو CURLX_FREE(x)   حرر(x)
#وإلا
#تعریف_کرو CURLX_MALLOC(x) curlx_malloc(x)
#تعریف_کرو CURLX_FREE(x)   curlx_free(x)
#جے_انت

#جے_تعریف_شدہ _UNICODE
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
#جے_انت

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#إذا defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_بھریا_رستہ_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#جے_انت

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
ساكن bool fix_excessive_رستہ(ثابت TCHAR *وچ, TCHAR **باہرلا)
{
  size_t needed, گنتی;
  ثابت wchar_t *وچ_w;
  wchar_t *fbuf = عدم;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_رستہ_len = 32767;

#جے_بنا_تعریف _UNICODE
  wchar_t *ibuf = عدم;
  حرف *obuf = عدم;
#جے_انت

  *باہرلا = عدم;

  /* skip paths already normalized */
  إذا(!_tcsncmp(وچ, متن("\\\\?\\"), 4))
    اذهب cleanup;

#جے_بنا_تعریف _UNICODE
  /* convert multibyte input to unicode */
  إذا(mbstowcs_s(&needed, عدم, 0, وچ, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_رستہ_len)
    اذهب cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!ibuf)
    اذهب cleanup;
  إذا(mbstowcs_s(&گنتی, ibuf, needed, وچ, needed - 1))
    اذهب cleanup;
  إذا(گنتی != needed)
    اذهب cleanup;
  وچ_w = ibuf;
#وإلا
  وچ_w = وچ;
#جے_انت

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_بھریا_رستہ_Name_W(وچ_w, 0, عدم, عدم);
  إذا(!needed || needed > max_رستہ_len)
    اذهب cleanup;
  /* skip paths that are not excessive and do not need modification */
  إذا(needed <= MAX_رستہ)
    اذهب cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!fbuf)
    اذهب cleanup;
  گنتی = (size_t)Get_بھریا_رستہ_Name_W(وچ_w, (DWORD)needed, fbuf, عدم);
  إذا(!گنتی || گنتی >= needed)
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
    wchar_t *عارضی;

    إذا(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (گنتی - 2) + 1;
      إذا(needed > max_رستہ_len)
        اذهب cleanup;

      عارضی = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!عارضی)
        اذهب cleanup;

      إذا(wcsncpy_s(عارضی, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(عارضی);
        اذهب cleanup;
      }
      إذا(wcscpy_s(عارضی + 8, needed, fbuf + 2)) {
        CURLX_FREE(عارضی);
        اذهب cleanup;
      }
    }
    وإلا {
      /* "\\?\" + full path + null */
      needed = 4 + گنتی + 1;
      إذا(needed > max_رستہ_len)
        اذهب cleanup;

      عارضی = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!عارضی)
        اذهب cleanup;

      إذا(wcsncpy_s(عارضی, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(عارضی);
        اذهب cleanup;
      }
      إذا(wcscpy_s(عارضی + 4, needed, fbuf)) {
        CURLX_FREE(عارضی);
        اذهب cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = عارضی;
  }

#جے_بنا_تعریف _UNICODE
  /* convert unicode full path to multibyte output */
  إذا(wcstombs_s(&needed, عدم, 0, fbuf, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_رستہ_len)
    اذهب cleanup;
  obuf = CURLX_MALLOC(needed);
  إذا(!obuf)
    اذهب cleanup;
  إذا(wcstombs_s(&گنتی, obuf, needed, fbuf, needed - 1))
    اذهب cleanup;
  إذا(گنتی != needed)
    اذهب cleanup;
  *باہرلا = obuf;
  obuf = عدم;
#وإلا
  *باہرلا = fbuf;
  fbuf = عدم;
#جے_انت

cleanup:
  CURLX_FREE(fbuf);
#جے_بنا_تعریف _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#جے_انت
  أرجع !!*باہرلا;
}

#جے_بنا_تعریف CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_منوال,
                        LPSECURITY_گن lp_Security_گن,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_گن,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#جے_تعریف_شدہ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#جے_انت

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *نشانہ = عدم;

    إذا(fix_excessive_رستہ(filename_t, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(نشانہ,
                        dwDesiredAccess,
                        dw_Share_منوال,
                        lp_Security_گن,
                        dwCreationDisposition,
                        dw_Flags_And_گن,
                        hTemplateFile);
    CURLX_FREE(fixed);
#جے_تعریف_شدہ UNICODE
    curlx_free(filename_t);
#جے_انت
  }

  أرجع handle;
}

HANDLE curlx_لبھو_پہلا_File(ثابت حرف *filename,
                           WIN32_لبھو_انکڑے *لبھو_انکڑے)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#جے_تعریف_شدہ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#جے_انت

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *نشانہ;

    إذا(fix_excessive_رستہ(filename_t, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = filename_t;

    handle = لبھو_پہلا_File(نشانہ, لبھو_انکڑے);
    CURLX_FREE(fixed);

#جے_تعریف_شدہ UNICODE
    curlx_free(filename_t);
#جے_انت
  }

  أرجع handle;
}
#جے_انت /* !CURL_WINDOWS_UWP */

صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...)
{
  صحيح pmode = 0;
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *نشانہ = عدم;

#جے_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
#جے_انت

  va_list param;
  va_start(param, oflag);
  إذا(oflag & O_CREAT)
    pmode = va_arg(param, صحيح);
  va_end(param);

#جے_تعریف_شدہ _UNICODE
  إذا(filename_w) {
    إذا(fix_excessive_رستہ(filename_w, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = filename_w;
    errno = _wsopen_s(&res, نشانہ, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستہ(filename, &fixed))
    نشانہ = fixed;
  وإلا
    نشانہ = filename;
  errno = _sopen_s(&res, نشانہ, oflag, _SH_DENYNO, pmode);
#جے_انت

  CURLX_FREE(fixed);
  أرجع res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *منوال)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *نشانہ = عدم;

#جے_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *منوال_w = fn_convert_UTF8_منزل_wchar(منوال);
  إذا(filename_w && منوال_w) {
    إذا(fix_excessive_رستہ(filename_w, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = filename_w;
    file = _wfsopen(نشانہ, منوال_w, _SH_DENYNO);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(منوال_w);
#وإلا
  إذا(fix_excessive_رستہ(filename, &fixed))
    نشانہ = fixed;
  وإلا
    نشانہ = filename;
  file = _fsopen(نشانہ, منوال, _SH_DENYNO);
#جے_انت

  CURLX_FREE(fixed);
  أرجع file;
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *منوال, FILE *دھارا);
#جے_انت

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *منوال, FILE *fp)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *نشانہ = عدم;

#جے_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *منوال_w = fn_convert_UTF8_منزل_wchar(منوال);
  إذا(filename_w && منوال_w) {
    إذا(fix_excessive_رستہ(filename_w, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = filename_w;
    errno = _wfreopen_s(&file, نشانہ, منوال_w, fp);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(منوال_w);
#وإلا
  إذا(fix_excessive_رستہ(filename, &fixed))
    نشانہ = fixed;
  وإلا
    نشانہ = filename;
  errno = freopen_s(&file, نشانہ, منوال, fp);
#جے_انت

  CURLX_FREE(fixed);
  أرجع file;
}

صحيح curlx_win32_stat(ثابت حرف *رستہ, curlx_struct_stat *بفر)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *نشانہ = عدم;

#جے_تعریف_شدہ _UNICODE
  wchar_t *رستہ_w = curlx_convert_UTF8_منزل_wchar(رستہ);
  إذا(رستہ_w) {
    إذا(fix_excessive_رستہ(رستہ_w, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = رستہ_w;
    res = _wstati64(نشانہ, بفر);
    curlx_free(رستہ_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستہ(رستہ, &fixed))
    نشانہ = fixed;
  وإلا
    نشانہ = رستہ;
  res = _stati64(نشانہ, بفر);
#جے_انت

  CURLX_FREE(fixed);
  أرجع res;
}

صحيح curlx_win32_mkdir(ثابت حرف *رستہ)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *نشانہ = عدم;

#جے_تعریف_شدہ _UNICODE
  wchar_t *رستہ_w = curlx_convert_UTF8_منزل_wchar(رستہ);
  إذا(رستہ_w) {
    إذا(fix_excessive_رستہ(رستہ_w, &fixed))
      نشانہ = fixed;
    وإلا
      نشانہ = رستہ_w;
    res = _wmkdir(نشانہ);
    curlx_free(رستہ_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستہ(رستہ, &fixed))
    نشانہ = fixed;
  وإلا
    نشانہ = رستہ;
  res = _mkdir(نشانہ);
#جے_انت

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

#جے_تعریف_شدہ UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_منزل_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_منزل_wchar(newpath);
#وإلا
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#جے_انت

  إذا(tchar_oldpath && tchar_newpath) {
    ثابت صحيح max_wait_ms = 1000;
    بنية curltime شروع;

    TCHAR *oldpath_fixed = عدم;
    TCHAR *newpath_fixed = عدم;
    ثابت TCHAR *نشانہ_oldpath;
    ثابت TCHAR *نشانہ_newpath;

    إذا(fix_excessive_رستہ(tchar_oldpath, &oldpath_fixed))
      نشانہ_oldpath = oldpath_fixed;
    وإلا
      نشانہ_oldpath = tchar_oldpath;

    إذا(fix_excessive_رستہ(tchar_newpath, &newpath_fixed))
      نشانہ_newpath = newpath_fixed;
    وإلا
      نشانہ_newpath = tchar_newpath;

    شروع = curlx_now();

    لكل(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      إذا(MoveFileEx(نشانہ_oldpath, نشانہ_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        اكسر;
      }
      diff = curlx_timediff_ms(curlx_now(), شروع);
      إذا(diff < 0 || diff > max_wait_ms) {
        اكسر;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#جے_تعریف_شدہ UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#جے_انت

  أرجع res;
}
#جے_انت

#تعریف_رد CURLX_MALLOC
#تعریف_رد CURLX_FREE

#جے_انت /* _WIN32 */
