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

صحيح curlx_fseek(فراغ *جریان, curl_off_t جابه_جایی, صحيح whence)
{
#اگر_تعریف_شده _WIN32
  أرجع _fseeki64(جریان, (__int64)جابه_جایی, whence);
#وإلاإذا defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  أرجع fseeko(جریان, (off_t)جابه_جایی, whence);
#وإلا
  إذا(جابه_جایی > LONG_MAX)
    أرجع -1;
  أرجع fseek(جریان, (دراز)جابه_جایی, whence);
#پایان_اگر
}

#اگر_تعریف_شده _WIN32

#ضمن <direct.h>  /* for _mkdir(), _wmkdir() */
#ضمن <share.h>  /* for _SH_DENYNO */

#ضمن "curlx/multibyte.h"
#ضمن "curlx/timeval.h"

#اگر_تعریف_شده CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#تعریف_کن CURLX_MALLOC(x) حافظه_بده(x)
#تعریف_کن CURLX_FREE(x)   حرر(x)
#وإلا
#تعریف_کن CURLX_MALLOC(x) curlx_malloc(x)
#تعریف_کن CURLX_FREE(x)   curlx_free(x)
#پایان_اگر

#اگر_تعریف_شده _UNICODE
ساكن wchar_t *fn_convert_UTF8_مقصد_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = عدم;

  إذا(str_utf8) {
    صحيح str_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, عدم, 0);
    إذا(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * حجم(wchar_t));
      إذا(str_w) {
        إذا(Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          أرجع عدم;
        }
      }
    }
  }
  أرجع str_w;
}
#پایان_اگر

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#إذا defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_پر_مسیر_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#پایان_اگر

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
ساكن bool fix_excessive_مسیر(ثابت TCHAR *درون, TCHAR **بیرون)
{
  size_t needed, شمار;
  ثابت wchar_t *درون_w;
  wchar_t *fbuf = عدم;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_مسیر_len = 32767;

#اگر_تعریف_نشده _UNICODE
  wchar_t *ibuf = عدم;
  حرف *obuf = عدم;
#پایان_اگر

  *بیرون = عدم;

  /* skip paths already normalized */
  إذا(!_tcsncmp(درون, متن("\\\\?\\"), 4))
    اذهب cleanup;

#اگر_تعریف_نشده _UNICODE
  /* convert multibyte input to unicode */
  إذا(mbstowcs_s(&needed, عدم, 0, درون, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_مسیر_len)
    اذهب cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!ibuf)
    اذهب cleanup;
  إذا(mbstowcs_s(&شمار, ibuf, needed, درون, needed - 1))
    اذهب cleanup;
  إذا(شمار != needed)
    اذهب cleanup;
  درون_w = ibuf;
#وإلا
  درون_w = درون;
#پایان_اگر

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_پر_مسیر_Name_W(درون_w, 0, عدم, عدم);
  إذا(!needed || needed > max_مسیر_len)
    اذهب cleanup;
  /* skip paths that are not excessive and do not need modification */
  إذا(needed <= MAX_مسیر)
    اذهب cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  إذا(!fbuf)
    اذهب cleanup;
  شمار = (size_t)Get_پر_مسیر_Name_W(درون_w, (DWORD)needed, fbuf, عدم);
  إذا(!شمار || شمار >= needed)
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
    wchar_t *موقت;

    إذا(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (شمار - 2) + 1;
      إذا(needed > max_مسیر_len)
        اذهب cleanup;

      موقت = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!موقت)
        اذهب cleanup;

      إذا(wcsncpy_s(موقت, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(موقت);
        اذهب cleanup;
      }
      إذا(wcscpy_s(موقت + 8, needed, fbuf + 2)) {
        CURLX_FREE(موقت);
        اذهب cleanup;
      }
    }
    وإلا {
      /* "\\?\" + full path + null */
      needed = 4 + شمار + 1;
      إذا(needed > max_مسیر_len)
        اذهب cleanup;

      موقت = CURLX_MALLOC(needed * حجم(wchar_t));
      إذا(!موقت)
        اذهب cleanup;

      إذا(wcsncpy_s(موقت, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(موقت);
        اذهب cleanup;
      }
      إذا(wcscpy_s(موقت + 4, needed, fbuf)) {
        CURLX_FREE(موقت);
        اذهب cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = موقت;
  }

#اگر_تعریف_نشده _UNICODE
  /* convert unicode full path to multibyte output */
  إذا(wcstombs_s(&needed, عدم, 0, fbuf, 0))
    اذهب cleanup;
  إذا(!needed || needed >= max_مسیر_len)
    اذهب cleanup;
  obuf = CURLX_MALLOC(needed);
  إذا(!obuf)
    اذهب cleanup;
  إذا(wcstombs_s(&شمار, obuf, needed, fbuf, needed - 1))
    اذهب cleanup;
  إذا(شمار != needed)
    اذهب cleanup;
  *بیرون = obuf;
  obuf = عدم;
#وإلا
  *بیرون = fbuf;
  fbuf = عدم;
#پایان_اگر

cleanup:
  CURLX_FREE(fbuf);
#اگر_تعریف_نشده _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#پایان_اگر
  أرجع !!*بیرون;
}

#اگر_تعریف_نشده CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_نما,
                        LPSECURITY_صفات lp_Security_صفات,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_صفات,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_مقدار;

#اگر_تعریف_شده UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_مقصد_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#پایان_اگر

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *هدف = عدم;

    إذا(fix_excessive_مسیر(filename_t, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(هدف,
                        dwDesiredAccess,
                        dw_Share_نما,
                        lp_Security_صفات,
                        dwCreationDisposition,
                        dw_Flags_And_صفات,
                        hTemplateFile);
    CURLX_FREE(fixed);
#اگر_تعریف_شده UNICODE
    curlx_free(filename_t);
#پایان_اگر
  }

  أرجع handle;
}

HANDLE curlx_پیدا_کن_اولی_File(ثابت حرف *filename,
                           WIN32_پیدا_کن_داده *پیدا_کن_داده)
{
  HANDLE handle = INVALID_HANDLE_مقدار;

#اگر_تعریف_شده UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_مقصد_wchar(filename);
#وإلا
  ثابت TCHAR *filename_t = filename;
#پایان_اگر

  إذا(filename_t) {
    TCHAR *fixed = عدم;
    ثابت TCHAR *هدف;

    إذا(fix_excessive_مسیر(filename_t, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_t;

    handle = پیدا_کن_اولی_File(هدف, پیدا_کن_داده);
    CURLX_FREE(fixed);

#اگر_تعریف_شده UNICODE
    curlx_free(filename_t);
#پایان_اگر
  }

  أرجع handle;
}
#پایان_اگر /* !CURL_WINDOWS_UWP */

صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...)
{
  صحيح pmode = 0;
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#اگر_تعریف_شده _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
#پایان_اگر

  va_list param;
  va_start(param, oflag);
  إذا(oflag & O_CREAT)
    pmode = va_arg(param, صحيح);
  va_end(param);

#اگر_تعریف_شده _UNICODE
  إذا(filename_w) {
    إذا(fix_excessive_مسیر(filename_w, &fixed))
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
  إذا(fix_excessive_مسیر(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  errno = _sopen_s(&res, هدف, oflag, _SH_DENYNO, pmode);
#پایان_اگر

  CURLX_FREE(fixed);
  أرجع res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *نما)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#اگر_تعریف_شده _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
  wchar_t *نما_w = fn_convert_UTF8_مقصد_wchar(نما);
  إذا(filename_w && نما_w) {
    إذا(fix_excessive_مسیر(filename_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_w;
    file = _wfsopen(هدف, نما_w, _SH_DENYNO);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(نما_w);
#وإلا
  إذا(fix_excessive_مسیر(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  file = _fsopen(هدف, نما, _SH_DENYNO);
#پایان_اگر

  CURLX_FREE(fixed);
  أرجع file;
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *نما, FILE *جریان);
#پایان_اگر

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *نما, FILE *fp)
{
  FILE *file = عدم;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#اگر_تعریف_شده _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
  wchar_t *نما_w = fn_convert_UTF8_مقصد_wchar(نما);
  إذا(filename_w && نما_w) {
    إذا(fix_excessive_مسیر(filename_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = filename_w;
    errno = _wfreopen_s(&file, هدف, نما_w, fp);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(نما_w);
#وإلا
  إذا(fix_excessive_مسیر(filename, &fixed))
    هدف = fixed;
  وإلا
    هدف = filename;
  errno = freopen_s(&file, هدف, نما, fp);
#پایان_اگر

  CURLX_FREE(fixed);
  أرجع file;
}

صحيح curlx_win32_stat(ثابت حرف *مسیر, curlx_struct_stat *میانگیر)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#اگر_تعریف_شده _UNICODE
  wchar_t *مسیر_w = curlx_convert_UTF8_مقصد_wchar(مسیر);
  إذا(مسیر_w) {
    إذا(fix_excessive_مسیر(مسیر_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = مسیر_w;
    res = _wstati64(هدف, میانگیر);
    curlx_free(مسیر_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_مسیر(مسیر, &fixed))
    هدف = fixed;
  وإلا
    هدف = مسیر;
  res = _stati64(هدف, میانگیر);
#پایان_اگر

  CURLX_FREE(fixed);
  أرجع res;
}

صحيح curlx_win32_mkdir(ثابت حرف *مسیر)
{
  صحيح res = -1;
  TCHAR *fixed = عدم;
  ثابت TCHAR *هدف = عدم;

#اگر_تعریف_شده _UNICODE
  wchar_t *مسیر_w = curlx_convert_UTF8_مقصد_wchar(مسیر);
  إذا(مسیر_w) {
    إذا(fix_excessive_مسیر(مسیر_w, &fixed))
      هدف = fixed;
    وإلا
      هدف = مسیر_w;
    res = _wmkdir(هدف);
    curlx_free(مسیر_w);
  }
  وإلا
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#وإلا
  إذا(fix_excessive_مسیر(مسیر, &fixed))
    هدف = fixed;
  وإلا
    هدف = مسیر;
  res = _mkdir(هدف);
#پایان_اگر

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

#اگر_تعریف_شده UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_مقصد_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_مقصد_wchar(newpath);
#وإلا
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#پایان_اگر

  إذا(tchar_oldpath && tchar_newpath) {
    ثابت صحيح max_wait_ms = 1000;
    بنية curltime شروع;

    TCHAR *oldpath_fixed = عدم;
    TCHAR *newpath_fixed = عدم;
    ثابت TCHAR *هدف_oldpath;
    ثابت TCHAR *هدف_newpath;

    إذا(fix_excessive_مسیر(tchar_oldpath, &oldpath_fixed))
      هدف_oldpath = oldpath_fixed;
    وإلا
      هدف_oldpath = tchar_oldpath;

    إذا(fix_excessive_مسیر(tchar_newpath, &newpath_fixed))
      هدف_newpath = newpath_fixed;
    وإلا
      هدف_newpath = tchar_newpath;

    شروع = curlx_now();

    لكل(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      إذا(MoveFileEx(هدف_oldpath, هدف_newpath,
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

#اگر_تعریف_شده UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#پایان_اگر

  أرجع res;
}
#پایان_اگر

#لغو_تعریف CURLX_MALLOC
#لغو_تعریف CURLX_FREE

#پایان_اگر /* _WIN32 */
