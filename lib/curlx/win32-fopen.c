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

صحیح curlx_fseek(خالی *دھارا, curl_off_t انحراف, صحیح whence)
{
#اگر_تعریف_شدہ _WIN32
  واپس _fseeki64(دھارا, (__int64)انحراف, whence);
#وگرنہ_اگر defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  واپس fseeko(دھارا, (off_t)انحراف, whence);
#ورنہ
  اگر(انحراف > LONG_MAX)
    واپس -1;
  واپس fseek(دھارا, (طویل)انحراف, whence);
#اگر_ختم
}

#اگر_تعریف_شدہ _WIN32

#شامل <direct.h>  /* for _mkdir(), _wmkdir() */
#شامل <share.h>  /* for _SH_DENYNO */

#شامل "curlx/multibyte.h"
#شامل "curlx/timeval.h"

#اگر_تعریف_شدہ CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#تعریف_کرو CURLX_MALLOC(x) حافظہ_دو(x)
#تعریف_کرو CURLX_FREE(x)   آزاد(x)
#ورنہ
#تعریف_کرو CURLX_MALLOC(x) curlx_malloc(x)
#تعریف_کرو CURLX_FREE(x)   curlx_free(x)
#اگر_ختم

#اگر_تعریف_شدہ _UNICODE
ساکن wchar_t *fn_convert_UTF8_مقصد_wchar(ثابت حرف *str_utf8)
{
  wchar_t *str_w = تہی;

  اگر(str_utf8) {
    صحیح str_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, تہی, 0);
    اگر(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * حجم(wchar_t));
      اگر(str_w) {
        اگر(Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          واپس تہی;
        }
      }
    }
  }
  واپس str_w;
}
#اگر_ختم

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#اگر defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_بھرا_راستہ_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#اگر_ختم

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
ساکن bool fix_excessive_راستہ(ثابت TCHAR *اندر, TCHAR **برآمد)
{
  size_t needed, گنتی;
  ثابت wchar_t *اندر_w;
  wchar_t *fbuf = تہی;

  /* MS-documented "approximate" limit for the maximum path length */
  ثابت size_t max_راستہ_len = 32767;

#اگر_غیر_تعریف_شدہ _UNICODE
  wchar_t *ibuf = تہی;
  حرف *obuf = تہی;
#اگر_ختم

  *برآمد = تہی;

  /* skip paths already normalized */
  اگر(!_tcsncmp(اندر, متن("\\\\?\\"), 4))
    جاؤ cleanup;

#اگر_غیر_تعریف_شدہ _UNICODE
  /* convert multibyte input to unicode */
  اگر(mbstowcs_s(&needed, تہی, 0, اندر, 0))
    جاؤ cleanup;
  اگر(!needed || needed >= max_راستہ_len)
    جاؤ cleanup;
  ibuf = CURLX_MALLOC(needed * حجم(wchar_t));
  اگر(!ibuf)
    جاؤ cleanup;
  اگر(mbstowcs_s(&گنتی, ibuf, needed, اندر, needed - 1))
    جاؤ cleanup;
  اگر(گنتی != needed)
    جاؤ cleanup;
  اندر_w = ibuf;
#ورنہ
  اندر_w = اندر;
#اگر_ختم

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_بھرا_راستہ_Name_W(اندر_w, 0, تہی, تہی);
  اگر(!needed || needed > max_راستہ_len)
    جاؤ cleanup;
  /* skip paths that are not excessive and do not need modification */
  اگر(needed <= MAX_راستہ)
    جاؤ cleanup;
  fbuf = CURLX_MALLOC(needed * حجم(wchar_t));
  اگر(!fbuf)
    جاؤ cleanup;
  گنتی = (size_t)Get_بھرا_راستہ_Name_W(اندر_w, (DWORD)needed, fbuf, تہی);
  اگر(!گنتی || گنتی >= needed)
    جاؤ cleanup;

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
  ورنہ اگر(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ورنہ اگر(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    جاؤ cleanup;
  }
  ورنہ {
    wchar_t *عارضی;

    اگر(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (گنتی - 2) + 1;
      اگر(needed > max_راستہ_len)
        جاؤ cleanup;

      عارضی = CURLX_MALLOC(needed * حجم(wchar_t));
      اگر(!عارضی)
        جاؤ cleanup;

      اگر(wcsncpy_s(عارضی, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(عارضی);
        جاؤ cleanup;
      }
      اگر(wcscpy_s(عارضی + 8, needed, fbuf + 2)) {
        CURLX_FREE(عارضی);
        جاؤ cleanup;
      }
    }
    ورنہ {
      /* "\\?\" + full path + null */
      needed = 4 + گنتی + 1;
      اگر(needed > max_راستہ_len)
        جاؤ cleanup;

      عارضی = CURLX_MALLOC(needed * حجم(wchar_t));
      اگر(!عارضی)
        جاؤ cleanup;

      اگر(wcsncpy_s(عارضی, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(عارضی);
        جاؤ cleanup;
      }
      اگر(wcscpy_s(عارضی + 4, needed, fbuf)) {
        CURLX_FREE(عارضی);
        جاؤ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = عارضی;
  }

#اگر_غیر_تعریف_شدہ _UNICODE
  /* convert unicode full path to multibyte output */
  اگر(wcstombs_s(&needed, تہی, 0, fbuf, 0))
    جاؤ cleanup;
  اگر(!needed || needed >= max_راستہ_len)
    جاؤ cleanup;
  obuf = CURLX_MALLOC(needed);
  اگر(!obuf)
    جاؤ cleanup;
  اگر(wcstombs_s(&گنتی, obuf, needed, fbuf, needed - 1))
    جاؤ cleanup;
  اگر(گنتی != needed)
    جاؤ cleanup;
  *برآمد = obuf;
  obuf = تہی;
#ورنہ
  *برآمد = fbuf;
  fbuf = تہی;
#اگر_ختم

cleanup:
  CURLX_FREE(fbuf);
#اگر_غیر_تعریف_شدہ _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#اگر_ختم
  واپس !!*برآمد;
}

#اگر_غیر_تعریف_شدہ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_منوال,
                        LPSECURITY_اوصاف lp_Security_اوصاف,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_اوصاف,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#اگر_تعریف_شدہ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_مقصد_wchar(filename);
#ورنہ
  ثابت TCHAR *filename_t = filename;
#اگر_ختم

  اگر(filename_t) {
    TCHAR *fixed = تہی;
    ثابت TCHAR *ہدف = تہی;

    اگر(fix_excessive_راستہ(filename_t, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(ہدف,
                        dwDesiredAccess,
                        dw_Share_منوال,
                        lp_Security_اوصاف,
                        dwCreationDisposition,
                        dw_Flags_And_اوصاف,
                        hTemplateFile);
    CURLX_FREE(fixed);
#اگر_تعریف_شدہ UNICODE
    curlx_free(filename_t);
#اگر_ختم
  }

  واپس handle;
}

HANDLE curlx_ڈھونڈو_پہلا_File(ثابت حرف *filename,
                           WIN32_ڈھونڈو_ڈیٹا *ڈھونڈو_ڈیٹا)
{
  HANDLE handle = INVALID_HANDLE_قدر;

#اگر_تعریف_شدہ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_مقصد_wchar(filename);
#ورنہ
  ثابت TCHAR *filename_t = filename;
#اگر_ختم

  اگر(filename_t) {
    TCHAR *fixed = تہی;
    ثابت TCHAR *ہدف;

    اگر(fix_excessive_راستہ(filename_t, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = filename_t;

    handle = ڈھونڈو_پہلا_File(ہدف, ڈھونڈو_ڈیٹا);
    CURLX_FREE(fixed);

#اگر_تعریف_شدہ UNICODE
    curlx_free(filename_t);
#اگر_ختم
  }

  واپس handle;
}
#اگر_ختم /* !CURL_WINDOWS_UWP */

صحیح curlx_win32_open(ثابت حرف *filename, صحیح oflag, ...)
{
  صحیح pmode = 0;
  صحیح res = -1;
  TCHAR *fixed = تہی;
  ثابت TCHAR *ہدف = تہی;

#اگر_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
#اگر_ختم

  va_list param;
  va_start(param, oflag);
  اگر(oflag & O_CREAT)
    pmode = va_arg(param, صحیح);
  va_end(param);

#اگر_تعریف_شدہ _UNICODE
  اگر(filename_w) {
    اگر(fix_excessive_راستہ(filename_w, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = filename_w;
    errno = _wsopen_s(&res, ہدف, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ورنہ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ورنہ
  اگر(fix_excessive_راستہ(filename, &fixed))
    ہدف = fixed;
  ورنہ
    ہدف = filename;
  errno = _sopen_s(&res, ہدف, oflag, _SH_DENYNO, pmode);
#اگر_ختم

  CURLX_FREE(fixed);
  واپس res;
}

FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *منوال)
{
  FILE *file = تہی;
  TCHAR *fixed = تہی;
  ثابت TCHAR *ہدف = تہی;

#اگر_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
  wchar_t *منوال_w = fn_convert_UTF8_مقصد_wchar(منوال);
  اگر(filename_w && منوال_w) {
    اگر(fix_excessive_راستہ(filename_w, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = filename_w;
    file = _wfsopen(ہدف, منوال_w, _SH_DENYNO);
  }
  ورنہ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(منوال_w);
#ورنہ
  اگر(fix_excessive_راستہ(filename, &fixed))
    ہدف = fixed;
  ورنہ
    ہدف = filename;
  file = _fsopen(ہدف, منوال, _SH_DENYNO);
#اگر_ختم

  CURLX_FREE(fixed);
  واپس file;
}

#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ثابت حرف *filename,
                                  ثابت حرف *منوال, FILE *دھارا);
#اگر_ختم

FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *منوال, FILE *fp)
{
  FILE *file = تہی;
  TCHAR *fixed = تہی;
  ثابت TCHAR *ہدف = تہی;

#اگر_تعریف_شدہ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_مقصد_wchar(filename);
  wchar_t *منوال_w = fn_convert_UTF8_مقصد_wchar(منوال);
  اگر(filename_w && منوال_w) {
    اگر(fix_excessive_راستہ(filename_w, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = filename_w;
    errno = _wfreopen_s(&file, ہدف, منوال_w, fp);
  }
  ورنہ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(منوال_w);
#ورنہ
  اگر(fix_excessive_راستہ(filename, &fixed))
    ہدف = fixed;
  ورنہ
    ہدف = filename;
  errno = freopen_s(&file, ہدف, منوال, fp);
#اگر_ختم

  CURLX_FREE(fixed);
  واپس file;
}

صحیح curlx_win32_stat(ثابت حرف *راستہ, curlx_struct_stat *بفر)
{
  صحیح res = -1;
  TCHAR *fixed = تہی;
  ثابت TCHAR *ہدف = تہی;

#اگر_تعریف_شدہ _UNICODE
  wchar_t *راستہ_w = curlx_convert_UTF8_مقصد_wchar(راستہ);
  اگر(راستہ_w) {
    اگر(fix_excessive_راستہ(راستہ_w, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = راستہ_w;
    res = _wstati64(ہدف, بفر);
    curlx_free(راستہ_w);
  }
  ورنہ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ورنہ
  اگر(fix_excessive_راستہ(راستہ, &fixed))
    ہدف = fixed;
  ورنہ
    ہدف = راستہ;
  res = _stati64(ہدف, بفر);
#اگر_ختم

  CURLX_FREE(fixed);
  واپس res;
}

صحیح curlx_win32_mkdir(ثابت حرف *راستہ)
{
  صحیح res = -1;
  TCHAR *fixed = تہی;
  ثابت TCHAR *ہدف = تہی;

#اگر_تعریف_شدہ _UNICODE
  wchar_t *راستہ_w = curlx_convert_UTF8_مقصد_wchar(راستہ);
  اگر(راستہ_w) {
    اگر(fix_excessive_راستہ(راستہ_w, &fixed))
      ہدف = fixed;
    ورنہ
      ہدف = راستہ_w;
    res = _wmkdir(ہدف);
    curlx_free(راستہ_w);
  }
  ورنہ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ورنہ
  اگر(fix_excessive_راستہ(راستہ, &fixed))
    ہدف = fixed;
  ورنہ
    ہدف = راستہ;
  res = _mkdir(ہدف);
#اگر_ختم

  CURLX_FREE(fixed);
  واپس res;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
صحیح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath)
{
  صحیح res = -1; /* fail */

#اگر_تعریف_شدہ UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_مقصد_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_مقصد_wchar(newpath);
#ورنہ
  ثابت TCHAR *tchar_oldpath = oldpath;
  ثابت TCHAR *tchar_newpath = newpath;
#اگر_ختم

  اگر(tchar_oldpath && tchar_newpath) {
    ثابت صحیح max_wait_ms = 1000;
    ساخت curltime آغاز;

    TCHAR *oldpath_fixed = تہی;
    TCHAR *newpath_fixed = تہی;
    ثابت TCHAR *ہدف_oldpath;
    ثابت TCHAR *ہدف_newpath;

    اگر(fix_excessive_راستہ(tchar_oldpath, &oldpath_fixed))
      ہدف_oldpath = oldpath_fixed;
    ورنہ
      ہدف_oldpath = tchar_oldpath;

    اگر(fix_excessive_راستہ(tchar_newpath, &newpath_fixed))
      ہدف_newpath = newpath_fixed;
    ورنہ
      ہدف_newpath = tchar_newpath;

    آغاز = curlx_now();

    برائے(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      اگر(MoveFileEx(ہدف_oldpath, ہدف_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        توڑو;
      }
      diff = curlx_timediff_ms(curlx_now(), آغاز);
      اگر(diff < 0 || diff > max_wait_ms) {
        توڑو;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#اگر_تعریف_شدہ UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#اگر_ختم

  واپس res;
}
#اگر_ختم

#تعریف_منسوخ CURLX_MALLOC
#تعریف_منسوخ CURLX_FREE

#اگر_ختم /* _WIN32 */
