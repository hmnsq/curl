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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/win32-fopen.h"

పూర్ణాంకం curlx_fseek(శూన్య *ప్రవాహం, curl_off_t స్థానభ్రంశం, పూర్ణాంకం whence)
{
#నిర్వచితమైతే _WIN32
  తిరిగి _fseeki64(ప్రవాహం, (__int64)స్థానభ్రంశం, whence);
#లేదావేళ defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  తిరిగి fseeko(ప్రవాహం, (off_t)స్థానభ్రంశం, whence);
#లేకపోతే
  ఒకవేళ(స్థానభ్రంశం > LONG_MAX)
    తిరిగి -1;
  తిరిగి fseek(ప్రవాహం, (దీర్ఘ)స్థానభ్రంశం, whence);
#అయితే_ముగింపు
}

#నిర్వచితమైతే _WIN32

#అన్తర్భావ <direct.h>  /* for _mkdir(), _wmkdir() */
#అన్తర్భావ <share.h>  /* for _SH_DENYNO */

#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "curlx/timeval.h"

#నిర్వచితమైతే CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#నిర్వచించు CURLX_MALLOC(x) జ్ఞాపకం_ఇవ్వు(x)
#నిర్వచించు CURLX_FREE(x)   విడుదల(x)
#లేకపోతే
#నిర్వచించు CURLX_MALLOC(x) curlx_malloc(x)
#నిర్వచించు CURLX_FREE(x)   curlx_free(x)
#అయితే_ముగింపు

#నిర్వచితమైతే _UNICODE
స్థిర wchar_t *fn_convert_UTF8_గమ్యం_wchar(స్థిరరాశి అక్షరం *str_utf8)
{
  wchar_t *str_w = ఖాళీ;

  ఒకవేళ(str_utf8) {
    పూర్ణాంకం str_w_len = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ఖాళీ, 0);
    ఒకవేళ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * పరిమాణం(wchar_t));
      ఒకవేళ(str_w) {
        ఒకవేళ(Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          తిరిగి ఖాళీ;
        }
      }
    }
  }
  తిరిగి str_w;
}
#అయితే_ముగింపు

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ఒకవేళ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_నిండిన_మార్గం_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#అయితే_ముగింపు

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
స్థిర bool fix_excessive_మార్గం(స్థిరరాశి TCHAR *లోపల, TCHAR **బహిర్గతం)
{
  size_t needed, లెక్క;
  స్థిరరాశి wchar_t *లోపల_w;
  wchar_t *fbuf = ఖాళీ;

  /* MS-documented "approximate" limit for the maximum path length */
  స్థిరరాశి size_t max_మార్గం_len = 32767;

#నిర్వచితం_కాకపోతే _UNICODE
  wchar_t *ibuf = ఖాళీ;
  అక్షరం *obuf = ఖాళీ;
#అయితే_ముగింపు

  *బహిర్గతం = ఖాళీ;

  /* skip paths already normalized */
  ఒకవేళ(!_tcsncmp(లోపల, పాఠ్యం("\\\\?\\"), 4))
    వెళ్ళు cleanup;

#నిర్వచితం_కాకపోతే _UNICODE
  /* convert multibyte input to unicode */
  ఒకవేళ(mbstowcs_s(&needed, ఖాళీ, 0, లోపల, 0))
    వెళ్ళు cleanup;
  ఒకవేళ(!needed || needed >= max_మార్గం_len)
    వెళ్ళు cleanup;
  ibuf = CURLX_MALLOC(needed * పరిమాణం(wchar_t));
  ఒకవేళ(!ibuf)
    వెళ్ళు cleanup;
  ఒకవేళ(mbstowcs_s(&లెక్క, ibuf, needed, లోపల, needed - 1))
    వెళ్ళు cleanup;
  ఒకవేళ(లెక్క != needed)
    వెళ్ళు cleanup;
  లోపల_w = ibuf;
#లేకపోతే
  లోపల_w = లోపల;
#అయితే_ముగింపు

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_నిండిన_మార్గం_Name_W(లోపల_w, 0, ఖాళీ, ఖాళీ);
  ఒకవేళ(!needed || needed > max_మార్గం_len)
    వెళ్ళు cleanup;
  /* skip paths that are not excessive and do not need modification */
  ఒకవేళ(needed <= MAX_మార్గం)
    వెళ్ళు cleanup;
  fbuf = CURLX_MALLOC(needed * పరిమాణం(wchar_t));
  ఒకవేళ(!fbuf)
    వెళ్ళు cleanup;
  లెక్క = (size_t)Get_నిండిన_మార్గం_Name_W(లోపల_w, (DWORD)needed, fbuf, ఖాళీ);
  ఒకవేళ(!లెక్క || లెక్క >= needed)
    వెళ్ళు cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ఒకవేళ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  లేకపోతే ఒకవేళ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  లేకపోతే ఒకవేళ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    వెళ్ళు cleanup;
  }
  లేకపోతే {
    wchar_t *తాత్కాలిక;

    ఒకవేళ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (లెక్క - 2) + 1;
      ఒకవేళ(needed > max_మార్గం_len)
        వెళ్ళు cleanup;

      తాత్కాలిక = CURLX_MALLOC(needed * పరిమాణం(wchar_t));
      ఒకవేళ(!తాత్కాలిక)
        వెళ్ళు cleanup;

      ఒకవేళ(wcsncpy_s(తాత్కాలిక, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(తాత్కాలిక);
        వెళ్ళు cleanup;
      }
      ఒకవేళ(wcscpy_s(తాత్కాలిక + 8, needed, fbuf + 2)) {
        CURLX_FREE(తాత్కాలిక);
        వెళ్ళు cleanup;
      }
    }
    లేకపోతే {
      /* "\\?\" + full path + null */
      needed = 4 + లెక్క + 1;
      ఒకవేళ(needed > max_మార్గం_len)
        వెళ్ళు cleanup;

      తాత్కాలిక = CURLX_MALLOC(needed * పరిమాణం(wchar_t));
      ఒకవేళ(!తాత్కాలిక)
        వెళ్ళు cleanup;

      ఒకవేళ(wcsncpy_s(తాత్కాలిక, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(తాత్కాలిక);
        వెళ్ళు cleanup;
      }
      ఒకవేళ(wcscpy_s(తాత్కాలిక + 4, needed, fbuf)) {
        CURLX_FREE(తాత్కాలిక);
        వెళ్ళు cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = తాత్కాలిక;
  }

#నిర్వచితం_కాకపోతే _UNICODE
  /* convert unicode full path to multibyte output */
  ఒకవేళ(wcstombs_s(&needed, ఖాళీ, 0, fbuf, 0))
    వెళ్ళు cleanup;
  ఒకవేళ(!needed || needed >= max_మార్గం_len)
    వెళ్ళు cleanup;
  obuf = CURLX_MALLOC(needed);
  ఒకవేళ(!obuf)
    వెళ్ళు cleanup;
  ఒకవేళ(wcstombs_s(&లెక్క, obuf, needed, fbuf, needed - 1))
    వెళ్ళు cleanup;
  ఒకవేళ(లెక్క != needed)
    వెళ్ళు cleanup;
  *బహిర్గతం = obuf;
  obuf = ఖాళీ;
#లేకపోతే
  *బహిర్గతం = fbuf;
  fbuf = ఖాళీ;
#అయితే_ముగింపు

cleanup:
  CURLX_FREE(fbuf);
#నిర్వచితం_కాకపోతే _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#అయితే_ముగింపు
  తిరిగి !!*బహిర్గతం;
}

#నిర్వచితం_కాకపోతే CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(స్థిరరాశి అక్షరం *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_బహుళకం,
                        LPSECURITY_లక్షణాలు lp_Security_లక్షణాలు,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_లక్షణాలు,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_విలువ;

#నిర్వచితమైతే UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_గమ్యం_wchar(filename);
#లేకపోతే
  స్థిరరాశి TCHAR *filename_t = filename;
#అయితే_ముగింపు

  ఒకవేళ(filename_t) {
    TCHAR *fixed = ఖాళీ;
    స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

    ఒకవేళ(fix_excessive_మార్గం(filename_t, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(లక్ష్యం,
                        dwDesiredAccess,
                        dw_Share_బహుళకం,
                        lp_Security_లక్షణాలు,
                        dwCreationDisposition,
                        dw_Flags_And_లక్షణాలు,
                        hTemplateFile);
    CURLX_FREE(fixed);
#నిర్వచితమైతే UNICODE
    curlx_free(filename_t);
#అయితే_ముగింపు
  }

  తిరిగి handle;
}

HANDLE curlx_కనుగొను_మొదటి_File(స్థిరరాశి అక్షరం *filename,
                           WIN32_కనుగొను_దత్తాంశం *కనుగొను_దత్తాంశం)
{
  HANDLE handle = INVALID_HANDLE_విలువ;

#నిర్వచితమైతే UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_గమ్యం_wchar(filename);
#లేకపోతే
  స్థిరరాశి TCHAR *filename_t = filename;
#అయితే_ముగింపు

  ఒకవేళ(filename_t) {
    TCHAR *fixed = ఖాళీ;
    స్థిరరాశి TCHAR *లక్ష్యం;

    ఒకవేళ(fix_excessive_మార్గం(filename_t, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = filename_t;

    handle = కనుగొను_మొదటి_File(లక్ష్యం, కనుగొను_దత్తాంశం);
    CURLX_FREE(fixed);

#నిర్వచితమైతే UNICODE
    curlx_free(filename_t);
#అయితే_ముగింపు
  }

  తిరిగి handle;
}
#అయితే_ముగింపు /* !CURL_WINDOWS_UWP */

పూర్ణాంకం curlx_win32_open(స్థిరరాశి అక్షరం *filename, పూర్ణాంకం oflag, ...)
{
  పూర్ణాంకం pmode = 0;
  పూర్ణాంకం res = -1;
  TCHAR *fixed = ఖాళీ;
  స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

#నిర్వచితమైతే _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_గమ్యం_wchar(filename);
#అయితే_ముగింపు

  va_list param;
  va_start(param, oflag);
  ఒకవేళ(oflag & O_CREAT)
    pmode = va_arg(param, పూర్ణాంకం);
  va_end(param);

#నిర్వచితమైతే _UNICODE
  ఒకవేళ(filename_w) {
    ఒకవేళ(fix_excessive_మార్గం(filename_w, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = filename_w;
    errno = _wsopen_s(&res, లక్ష్యం, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  లేకపోతే
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#లేకపోతే
  ఒకవేళ(fix_excessive_మార్గం(filename, &fixed))
    లక్ష్యం = fixed;
  లేకపోతే
    లక్ష్యం = filename;
  errno = _sopen_s(&res, లక్ష్యం, oflag, _SH_DENYNO, pmode);
#అయితే_ముగింపు

  CURLX_FREE(fixed);
  తిరిగి res;
}

FILE *curlx_win32_fopen(స్థిరరాశి అక్షరం *filename, స్థిరరాశి అక్షరం *బహుళకం)
{
  FILE *file = ఖాళీ;
  TCHAR *fixed = ఖాళీ;
  స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

#నిర్వచితమైతే _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_గమ్యం_wchar(filename);
  wchar_t *బహుళకం_w = fn_convert_UTF8_గమ్యం_wchar(బహుళకం);
  ఒకవేళ(filename_w && బహుళకం_w) {
    ఒకవేళ(fix_excessive_మార్గం(filename_w, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = filename_w;
    file = _wfsopen(లక్ష్యం, బహుళకం_w, _SH_DENYNO);
  }
  లేకపోతే
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(బహుళకం_w);
#లేకపోతే
  ఒకవేళ(fix_excessive_మార్గం(filename, &fixed))
    లక్ష్యం = fixed;
  లేకపోతే
    లక్ష్యం = filename;
  file = _fsopen(లక్ష్యం, బహుళకం, _SH_DENYNO);
#అయితే_ముగింపు

  CURLX_FREE(fixed);
  తిరిగి file;
}

#ఒకవేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, స్థిరరాశి అక్షరం *filename,
                                  స్థిరరాశి అక్షరం *బహుళకం, FILE *ప్రవాహం);
#అయితే_ముగింపు

FILE *curlx_win32_freopen(స్థిరరాశి అక్షరం *filename, స్థిరరాశి అక్షరం *బహుళకం, FILE *fp)
{
  FILE *file = ఖాళీ;
  TCHAR *fixed = ఖాళీ;
  స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

#నిర్వచితమైతే _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_గమ్యం_wchar(filename);
  wchar_t *బహుళకం_w = fn_convert_UTF8_గమ్యం_wchar(బహుళకం);
  ఒకవేళ(filename_w && బహుళకం_w) {
    ఒకవేళ(fix_excessive_మార్గం(filename_w, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = filename_w;
    errno = _wfreopen_s(&file, లక్ష్యం, బహుళకం_w, fp);
  }
  లేకపోతే
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(బహుళకం_w);
#లేకపోతే
  ఒకవేళ(fix_excessive_మార్గం(filename, &fixed))
    లక్ష్యం = fixed;
  లేకపోతే
    లక్ష్యం = filename;
  errno = freopen_s(&file, లక్ష్యం, బహుళకం, fp);
#అయితే_ముగింపు

  CURLX_FREE(fixed);
  తిరిగి file;
}

పూర్ణాంకం curlx_win32_stat(స్థిరరాశి అక్షరం *మార్గం, curlx_struct_stat *బఫరు)
{
  పూర్ణాంకం res = -1;
  TCHAR *fixed = ఖాళీ;
  స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

#నిర్వచితమైతే _UNICODE
  wchar_t *మార్గం_w = curlx_convert_UTF8_గమ్యం_wchar(మార్గం);
  ఒకవేళ(మార్గం_w) {
    ఒకవేళ(fix_excessive_మార్గం(మార్గం_w, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = మార్గం_w;
    res = _wstati64(లక్ష్యం, బఫరు);
    curlx_free(మార్గం_w);
  }
  లేకపోతే
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#లేకపోతే
  ఒకవేళ(fix_excessive_మార్గం(మార్గం, &fixed))
    లక్ష్యం = fixed;
  లేకపోతే
    లక్ష్యం = మార్గం;
  res = _stati64(లక్ష్యం, బఫరు);
#అయితే_ముగింపు

  CURLX_FREE(fixed);
  తిరిగి res;
}

పూర్ణాంకం curlx_win32_mkdir(స్థిరరాశి అక్షరం *మార్గం)
{
  పూర్ణాంకం res = -1;
  TCHAR *fixed = ఖాళీ;
  స్థిరరాశి TCHAR *లక్ష్యం = ఖాళీ;

#నిర్వచితమైతే _UNICODE
  wchar_t *మార్గం_w = curlx_convert_UTF8_గమ్యం_wchar(మార్గం);
  ఒకవేళ(మార్గం_w) {
    ఒకవేళ(fix_excessive_మార్గం(మార్గం_w, &fixed))
      లక్ష్యం = fixed;
    లేకపోతే
      లక్ష్యం = మార్గం_w;
    res = _wmkdir(లక్ష్యం);
    curlx_free(మార్గం_w);
  }
  లేకపోతే
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#లేకపోతే
  ఒకవేళ(fix_excessive_మార్గం(మార్గం, &fixed))
    లక్ష్యం = fixed;
  లేకపోతే
    లక్ష్యం = మార్గం;
  res = _mkdir(లక్ష్యం);
#అయితే_ముగింపు

  CURLX_FREE(fixed);
  తిరిగి res;
}

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
పూర్ణాంకం curlx_win32_rename(స్థిరరాశి అక్షరం *oldpath, స్థిరరాశి అక్షరం *newpath)
{
  పూర్ణాంకం res = -1; /* fail */

#నిర్వచితమైతే UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_గమ్యం_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_గమ్యం_wchar(newpath);
#లేకపోతే
  స్థిరరాశి TCHAR *tchar_oldpath = oldpath;
  స్థిరరాశి TCHAR *tchar_newpath = newpath;
#అయితే_ముగింపు

  ఒకవేళ(tchar_oldpath && tchar_newpath) {
    స్థిరరాశి పూర్ణాంకం max_wait_ms = 1000;
    నిర్మాణం curltime ప్రారంభం;

    TCHAR *oldpath_fixed = ఖాళీ;
    TCHAR *newpath_fixed = ఖాళీ;
    స్థిరరాశి TCHAR *లక్ష్యం_oldpath;
    స్థిరరాశి TCHAR *లక్ష్యం_newpath;

    ఒకవేళ(fix_excessive_మార్గం(tchar_oldpath, &oldpath_fixed))
      లక్ష్యం_oldpath = oldpath_fixed;
    లేకపోతే
      లక్ష్యం_oldpath = tchar_oldpath;

    ఒకవేళ(fix_excessive_మార్గం(tchar_newpath, &newpath_fixed))
      లక్ష్యం_newpath = newpath_fixed;
    లేకపోతే
      లక్ష్యం_newpath = tchar_newpath;

    ప్రారంభం = curlx_now();

    కొరకు(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ఒకవేళ(MoveFileEx(లక్ష్యం_oldpath, లక్ష్యం_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ఆపు;
      }
      diff = curlx_timediff_ms(curlx_now(), ప్రారంభం);
      ఒకవేళ(diff < 0 || diff > max_wait_ms) {
        ఆపు;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#నిర్వచితమైతే UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#అయితే_ముగింపు

  తిరిగి res;
}
#అయితే_ముగింపు

#నిర్వచనం_రద్దు CURLX_MALLOC
#నిర్వచనం_రద్దు CURLX_FREE

#అయితే_ముగింపు /* _WIN32 */
