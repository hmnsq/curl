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

صحيح curlx_fseek(فراغ *وهڪرو, curl_off_t وٿي, صحيح whence)
{
#جيڪڏهن_وصف_ٿيل _WIN32
  أرجع _fseeki64(وهڪرو, (__int64)وٿي, whence);
#وإلاإذا defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  أرجع fseeko(وهڪرو, (off_t)وٿي, whence);
#وإلا
  إذا(وٿي > LONG_MAX)
    أرجع -1;
  أرجع fseek(وهڪرو, (ڊگهو)وٿي, whence);
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل _WIN32

#ضمن <direct.h>  /* for _mkdir(), _wmkdir() */
#ضمن <share.h>  /* for _SH_DENYNO */

#ضمن "curlx/multibyte.h"
#ضمن "curlx/timeval.h"

#جيڪڏهن_وصف_ٿيل CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#وصف_ڪريو CURLX_MALLOC(x) يادگيري_ڏيو(x)
#وصف_ڪريو CURLX_FREE(x)   حرر(x)
#وإلا
#وصف_ڪريو CURLX_MALLOC(x) curlx_malloc(x)
#وصف_ڪريو CURLX_FREE(x)   curlx_free(x)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _UNICODE
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
#جيڪڏهن_پورو

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#إذا defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_ڀريل_رستو_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#جيڪڏهن_پورو

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
ساكن bool fix_excessive_رستو(ثابت TCHAR *اندريون, TCHAR **ٻاهريون)
{
  size_t needed, ڳڻپ;
  ثابت wchar_t *اندريون_w;
  wchar_t *fbuf = عدم;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_رستو_len = 32767;

#جيڪڏهن_وصف_نه_ٿيل _UNICODE
  wchar_t *ibuf = عدم;
  حرف *obuf = عدم;
#جيڪڏهن_پورو

  *ٻاهريون = عدم;

  /* skip paths already normalized */
  إذا(!_tcsncmp(اندريون, متن("\\\\?\\"), 4))
    اذهب cleanup;

#جيڪڏهن_وصف_نه_ٿيل _UNICODE
  /* convert multibyte input to unicode */
  إذا(mbstowcs_s(&needed, عدم, 0, اندريون, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_رستو_len)
    اذهب cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!ibuf)
    اذهب cleanup;
  إذا(mbstowcs_s(&ڳڻپ, ibuf, needed, اندريون, needed - 1))
    اذهب cleanup;
  إذا(ڳڻپ != needed)
    اذهب cleanup;
  اندريون_w = ibuf;
#وإلا
  اندريون_w = اندريون;
#جيڪڏهن_پورو

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_ڀريل_رستو_Name_W(اندريون_w, 0, عدم, عدم);
  إذا(!needed || needed > max_رستو_len)
    اذهب cleanup;
  /* skip paths that are not excessive and do not need modification */
  إذا(needed <= MAX_رستو)
    اذهب cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!fbuf)
    اذهب cleanup;
  ڳڻپ = (size_t)Get_ڀريل_رستو_Name_W(اندريون_w, (DWORD)needed, fbuf, عدم);
  إذا(!ڳڻپ || ڳڻپ >= needed)
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
    wchar_t *عارضي;

    إذا(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ڳڻپ - 2) + 1;
      إذا(needed > max_رستو_len)
        اذهب cleanup;

      عارضي = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!عارضي)
        اذهب cleanup;

      إذا(wcsncpy_s(عارضي, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(عارضي);
        اذهب cleanup;
      }
      إذا(wcscpy_s(عارضي + 8, needed, fbuf + 2)) {
        CURLX_FREE(عارضي);
        اذهب cleanup;
      }
    }
    وإلا {
      /* "\\?\" + full path + null */
      needed = 4 + ڳڻپ + 1;
      إذا(needed > max_رستو_len)
        اذهب cleanup;

      عارضي = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!عارضي)
        اذهب cleanup;

      إذا(wcsncpy_s(عارضي, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(عارضي);
        اذهب cleanup;
      }
      إذا(wcscpy_s(عارضي + 4, needed, fbuf)) {
        CURLX_FREE(عارضي);
        اذهب cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = عارضي;
  }

#جيڪڏهن_وصف_نه_ٿيل _UNICODE
  /* convert unicode full path to multibyte output */
  إذا(wcstombs_s(&needed, عدم, 0, fbuf, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_رستو_len)
    اذهب cleanup;
  obuf = CURLX_MALLOC(needed);
  إذا(!obuf)
    اذهب cleanup;
  إذا(wcstombs_s(&ڳڻپ, obuf, needed, fbuf, needed - 1))
    اذهب cleanup;
  إذا(ڳڻپ != needed)
    اذهب cleanup;
  *ٻاهريون = obuf;
  obuf = عدم;
#وإلا
  *ٻاهريون = fbuf;
  fbuf = عدم;
#جيڪڏهن_پورو

cleanup:
  CURLX_FREE(fbuf);
#جيڪڏهن_وصف_نه_ٿيل _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#جيڪڏهن_پورو
  أرجع !!*ٻاهريون;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_گهڻائي,
                        LPSECURITY_خاصيتون lp_Security_خاصيتون,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_خاصيتون,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#جيڪڏهن_وصف_ٿيل UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#جيڪڏهن_پورو

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *هدف = عدم;

    إذا(fix_excessive_رستو(filename_t, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(هدف,
                        dwDesiredAccess,
                        dw_Share_گهڻائي,
                        lp_Security_خاصيتون,
                        dwCreationDisposition,
                        dw_Flags_And_خاصيتون,
                        hTemplateFile);
    CURLX_FREE(fixed);
#جيڪڏهن_وصف_ٿيل UNICODE
    curlx_free(filename_t);
#جيڪڏهن_پورو
  }

  أرجع handle;
}

HANDLE curlx_ڳوليو_پهريون_File(ثابت حرف *filename,
                           WIN32_ڳوليو_معلومات *ڳوليو_معلومات)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#جيڪڏهن_وصف_ٿيل UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_منزل_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#جيڪڏهن_پورو

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *هدف;

    إذا(fix_excessive_رستو(filename_t, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_t;

    handle = ڳوليو_پهريون_File(هدف, ڳوليو_معلومات);
    CURLX_FREE(fixed);

#جيڪڏهن_وصف_ٿيل UNICODE
    curlx_free(filename_t);
#جيڪڏهن_پورو
  }

  أرجع handle;
}
#جيڪڏهن_پورو /* !CURL_WINDOWS_UWP */

صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...)
{
  صحيح pmode = 0;
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#جيڪڏهن_وصف_ٿيل _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
#جيڪڏهن_پورو

  va_list param;
  va_start(param, oflag);
  إذا(oflag & O_CREAT)
    pmode = va_arg(param, صحيح);
  va_end(param);

#جيڪڏهن_وصف_ٿيل _UNICODE
  إذا(filename_w) {
    إذا(fix_excessive_رستو(filename_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_w;
    errno = _wsopen_s(&res, هدف, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستو(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  errno = _sopen_s(&res, هدف, oflag, _SH_DENYNO, pmode);
#جيڪڏهن_پورو

  CURLX_FREE(fixed);
  أرجع res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *گهڻائي)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#جيڪڏهن_وصف_ٿيل _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *گهڻائي_w = fn_convert_UTF8_منزل_wchar(گهڻائي);
  إذا(filename_w && گهڻائي_w) {
    إذا(fix_excessive_رستو(filename_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_w;
    file = _wfsopen(هدف, گهڻائي_w, _SH_DENYNO);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(گهڻائي_w);
#وإلا
  إذا(fix_excessive_رستو(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  file = _fsopen(هدف, گهڻائي, _SH_DENYNO);
#جيڪڏهن_پورو

  CURLX_FREE(fixed);
  أرجع file;
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *گهڻائي, FILE *وهڪرو);
#جيڪڏهن_پورو

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *گهڻائي, FILE *fp)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#جيڪڏهن_وصف_ٿيل _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_منزل_wchar(filename);
  wchar_t *گهڻائي_w = fn_convert_UTF8_منزل_wchar(گهڻائي);
  إذا(filename_w && گهڻائي_w) {
    إذا(fix_excessive_رستو(filename_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_w;
    errno = _wfreopen_s(&file, هدف, گهڻائي_w, fp);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(گهڻائي_w);
#وإلا
  إذا(fix_excessive_رستو(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  errno = freopen_s(&file, هدف, گهڻائي, fp);
#جيڪڏهن_پورو

  CURLX_FREE(fixed);
  أرجع file;
}

صحيح curlx_win32_stat(ثابت حرف *رستو, curlx_struct_stat *بفر)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#جيڪڏهن_وصف_ٿيل _UNICODE
  wchar_t *رستو_w = curlx_convert_UTF8_منزل_wchar(رستو);
  إذا(رستو_w) {
    إذا(fix_excessive_رستو(رستو_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = رستو_w;
    res = _wstati64(هدف, بفر);
    curlx_free(رستو_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستو(رستو, &fixed))
    هدف = fixed;
  وإلا
    هدف = رستو;
  res = _stati64(هدف, بفر);
#جيڪڏهن_پورو

  CURLX_FREE(fixed);
  أرجع res;
}

صحيح curlx_win32_mkdir(ثابت حرف *رستو)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#جيڪڏهن_وصف_ٿيل _UNICODE
  wchar_t *رستو_w = curlx_convert_UTF8_منزل_wchar(رستو);
  إذا(رستو_w) {
    إذا(fix_excessive_رستو(رستو_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = رستو_w;
    res = _wmkdir(هدف);
    curlx_free(رستو_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_رستو(رستو, &fixed))
    هدف = fixed;
  وإلا
    هدف = رستو;
  res = _mkdir(هدف);
#جيڪڏهن_پورو

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

#جيڪڏهن_وصف_ٿيل UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_منزل_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_منزل_wchar(newpath);
#وإلا
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#جيڪڏهن_پورو

  إذا(tchar_oldpath && tchar_newpath) {
    ثابت صحيح max_wait_ms = 1000;
    بنية curltime شروعات;

    TCHAR *oldpath_fixed = عدم;
    TCHAR *newpath_fixed = عدم;
    ثابت TCHAR *هدف_oldpath;
    ثابت TCHAR *هدف_newpath;

    إذا(fix_excessive_رستو(tchar_oldpath, &oldpath_fixed))
      هدف_oldpath = oldpath_fixed;
    وإلا
      هدف_oldpath = tchar_oldpath;

    إذا(fix_excessive_رستو(tchar_newpath, &newpath_fixed))
      هدف_newpath = newpath_fixed;
    وإلا
      هدف_newpath = tchar_newpath;

    شروعات = curlx_now();

    لكل(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      إذا(MoveFileEx(هدف_oldpath, هدف_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        اكسر;
      }
      diff = curlx_timediff_ms(curlx_now(), شروعات);
      إذا(diff < 0 || diff > max_wait_ms) {
        اكسر;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#جيڪڏهن_وصف_ٿيل UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#جيڪڏهن_پورو

  أرجع res;
}
#جيڪڏهن_پورو

#وصف_رد CURLX_MALLOC
#وصف_رد CURLX_FREE

#جيڪڏهن_پورو /* _WIN32 */
