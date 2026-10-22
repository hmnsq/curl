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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

ಪೂರ್ಣಾಂಕ curlx_fseek(ಶೂನ್ಯ *ಹರಿವು, curl_off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಹಿಂತಿರುಗಿ _fseeki64(ಹರಿವು, (__int64)ಸ್ಥಾನಾಂತರ, whence);
#ಅಥವಾಆದರೆ defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  ಹಿಂತಿರುಗಿ fseeko(ಹರಿವು, (off_t)ಸ್ಥಾನಾಂತರ, whence);
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(ಸ್ಥಾನಾಂತರ > LONG_MAX)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ fseek(ಹರಿವು, (ದೀರ್ಘ)ಸ್ಥಾನಾಂತರ, whence);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

#ಅನ್ತರ್ಭಾವ <direct.h>  /* for _mkdir(), _wmkdir() */
#ಅನ್ತರ್ಭಾವ <share.h>  /* for _SH_DENYNO */

#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_MALLOC(x) ಸ್ಮರಣೆ_ಕೊಡು(x)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FREE(x)   ಬಿಡುಗಡೆ(x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_MALLOC(x) curlx_malloc(x)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FREE(x)   curlx_free(x)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
ಸ್ಥಿರ wchar_t *fn_convert_UTF8_ಗಮ್ಯ_wchar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str_utf8)
{
  wchar_t *str_w = ಖಾಲಿ;

  ಆದರೆ(str_utf8) {
    ಪೂರ್ಣಾಂಕ str_w_len = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, ಖಾಲಿ, 0);
    ಆದರೆ(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * ಗಾತ್ರ(wchar_t));
      ಆದರೆ(str_w) {
        ಆದರೆ(Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
      }
    }
  }
  ಹಿಂತಿರುಗಿ str_w;
}
#ಆದರೆ_ಅಂತ್ಯ

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#ಆದರೆ defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_ತುಂಬಿದ_ಮಾರ್ಗ_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ bool fix_excessive_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ TCHAR *ಒಳ, TCHAR **ಹೊರಹೋಗು)
{
  size_t needed, ಎಣಿಕೆ_2;
  ಸ್ಥಿರಾಂಕ wchar_t *ಒಳ_w;
  wchar_t *fbuf = ಖಾಲಿ;

  /* MS-documented "approximate" limit for the maximum path length */
  ಸ್ಥಿರಾಂಕ size_t max_ಮಾರ್ಗ_len = 32767;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _UNICODE
  wchar_t *ibuf = ಖಾಲಿ;
  ಅಕ್ಷರ *obuf = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

  *ಹೊರಹೋಗು = ಖಾಲಿ;

  /* skip paths already normalized */
  ಆದರೆ(!_tcsncmp(ಒಳ, ಪಠ್ಯ("\\\\?\\"), 4))
    ಹೋಗು cleanup;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _UNICODE
  /* convert multibyte input to unicode */
  ಆದರೆ(mbstowcs_s(&needed, ಖಾಲಿ, 0, ಒಳ, 0))
    ಹೋಗು cleanup;
  ಆದರೆ(!needed || needed >= max_ಮಾರ್ಗ_len)
    ಹೋಗು cleanup;
  ibuf = CURLX_MALLOC(needed * ಗಾತ್ರ(wchar_t));
  ಆದರೆ(!ibuf)
    ಹೋಗು cleanup;
  ಆದರೆ(mbstowcs_s(&ಎಣಿಕೆ_2, ibuf, needed, ಒಳ, needed - 1))
    ಹೋಗು cleanup;
  ಆದರೆ(ಎಣಿಕೆ_2 != needed)
    ಹೋಗು cleanup;
  ಒಳ_w = ibuf;
#ಇಲ್ಲದಿದ್ದರೆ
  ಒಳ_w = ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_ತುಂಬಿದ_ಮಾರ್ಗ_Name_W(ಒಳ_w, 0, ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(!needed || needed > max_ಮಾರ್ಗ_len)
    ಹೋಗು cleanup;
  /* skip paths that are not excessive and do not need modification */
  ಆದರೆ(needed <= MAX_ಮಾರ್ಗ)
    ಹೋಗು cleanup;
  fbuf = CURLX_MALLOC(needed * ಗಾತ್ರ(wchar_t));
  ಆದರೆ(!fbuf)
    ಹೋಗು cleanup;
  ಎಣಿಕೆ_2 = (size_t)Get_ತುಂಬಿದ_ಮಾರ್ಗ_Name_W(ಒಳ_w, (DWORD)needed, fbuf, ಖಾಲಿ);
  ಆದರೆ(!ಎಣಿಕೆ_2 || ಎಣಿಕೆ_2 >= needed)
    ಹೋಗು cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  ಆದರೆ(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    ಹೋಗು cleanup;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    wchar_t *ತಾತ್ಕಾಲಿಕ;

    ಆದರೆ(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (ಎಣಿಕೆ_2 - 2) + 1;
      ಆದರೆ(needed > max_ಮಾರ್ಗ_len)
        ಹೋಗು cleanup;

      ತಾತ್ಕಾಲಿಕ = CURLX_MALLOC(needed * ಗಾತ್ರ(wchar_t));
      ಆದರೆ(!ತಾತ್ಕಾಲಿಕ)
        ಹೋಗು cleanup;

      ಆದರೆ(wcsncpy_s(ತಾತ್ಕಾಲಿಕ, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(ತಾತ್ಕಾಲಿಕ);
        ಹೋಗು cleanup;
      }
      ಆದರೆ(wcscpy_s(ತಾತ್ಕಾಲಿಕ + 8, needed, fbuf + 2)) {
        CURLX_FREE(ತಾತ್ಕಾಲಿಕ);
        ಹೋಗು cleanup;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* "\\?\" + full path + null */
      needed = 4 + ಎಣಿಕೆ_2 + 1;
      ಆದರೆ(needed > max_ಮಾರ್ಗ_len)
        ಹೋಗು cleanup;

      ತಾತ್ಕಾಲಿಕ = CURLX_MALLOC(needed * ಗಾತ್ರ(wchar_t));
      ಆದರೆ(!ತಾತ್ಕಾಲಿಕ)
        ಹೋಗು cleanup;

      ಆದರೆ(wcsncpy_s(ತಾತ್ಕಾಲಿಕ, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(ತಾತ್ಕಾಲಿಕ);
        ಹೋಗು cleanup;
      }
      ಆದರೆ(wcscpy_s(ತಾತ್ಕಾಲಿಕ + 4, needed, fbuf)) {
        CURLX_FREE(ತಾತ್ಕಾಲಿಕ);
        ಹೋಗು cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = ತಾತ್ಕಾಲಿಕ;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _UNICODE
  /* convert unicode full path to multibyte output */
  ಆದರೆ(wcstombs_s(&needed, ಖಾಲಿ, 0, fbuf, 0))
    ಹೋಗು cleanup;
  ಆದರೆ(!needed || needed >= max_ಮಾರ್ಗ_len)
    ಹೋಗು cleanup;
  obuf = CURLX_MALLOC(needed);
  ಆದರೆ(!obuf)
    ಹೋಗು cleanup;
  ಆದರೆ(wcstombs_s(&ಎಣಿಕೆ_2, obuf, needed, fbuf, needed - 1))
    ಹೋಗು cleanup;
  ಆದರೆ(ಎಣಿಕೆ_2 != needed)
    ಹೋಗು cleanup;
  *ಹೊರಹೋಗು = obuf;
  obuf = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
  *ಹೊರಹೋಗು = fbuf;
  fbuf = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

cleanup:
  CURLX_FREE(fbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ !!*ಹೊರಹೋಗು;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ಬಹುಲಕ,
                        LPSECURITY_ಗುಣಲಕ್ಷಣಗಳು lp_Security_ಗುಣಲಕ್ಷಣಗಳು,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ಗುಣಲಕ್ಷಣಗಳು,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_ಮೌಲ್ಯ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_ಗಮ್ಯ_wchar(filename);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ TCHAR *filename_t = filename;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(filename_t) {
    TCHAR *fixed = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename_t, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(ಗುರಿ,
                        dwDesiredAccess,
                        dw_Share_ಬಹುಲಕ,
                        lp_Security_ಗುಣಲಕ್ಷಣಗಳು,
                        dwCreationDisposition,
                        dw_Flags_And_ಗುಣಲಕ್ಷಣಗಳು,
                        hTemplateFile);
    CURLX_FREE(fixed);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
    curlx_free(filename_t);
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಹಿಂತಿರುಗಿ handle;
}

HANDLE curlx_ಕಂಡುಹಿಡಿ_ಮೊದಲ_File(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                           WIN32_ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ *ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ)
{
  HANDLE handle = INVALID_HANDLE_ಮೌಲ್ಯ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_ಗಮ್ಯ_wchar(filename);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ TCHAR *filename_t = filename;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(filename_t) {
    TCHAR *fixed = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ;

    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename_t, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = filename_t;

    handle = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File(ಗುರಿ, ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ);
    CURLX_FREE(fixed);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
    curlx_free(filename_t);
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಹಿಂತಿರುಗಿ handle;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_WINDOWS_UWP */

ಪೂರ್ಣಾಂಕ curlx_win32_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ oflag, ...)
{
  ಪೂರ್ಣಾಂಕ pmode = 0;
  ಪೂರ್ಣಾಂಕ res = -1;
  TCHAR *fixed = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ಗಮ್ಯ_wchar(filename);
#ಆದರೆ_ಅಂತ್ಯ

  va_list param;
  va_start(param, oflag);
  ಆದರೆ(oflag & O_CREAT)
    pmode = va_arg(param, ಪೂರ್ಣಾಂಕ);
  va_end(param);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  ಆದರೆ(filename_w) {
    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename_w, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = filename_w;
    errno = _wsopen_s(&res, ಗುರಿ, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename, &fixed))
    ಗುರಿ = fixed;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗುರಿ = filename;
  errno = _sopen_s(&res, ಗುರಿ, oflag, _SH_DENYNO, pmode);
#ಆದರೆ_ಅಂತ್ಯ

  CURLX_FREE(fixed);
  ಹಿಂತಿರುಗಿ res;
}

FILE *curlx_win32_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ)
{
  FILE *file = ಖಾಲಿ;
  TCHAR *fixed = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ಗಮ್ಯ_wchar(filename);
  wchar_t *ಬಹುಲಕ_w = fn_convert_UTF8_ಗಮ್ಯ_wchar(ಬಹುಲಕ);
  ಆದರೆ(filename_w && ಬಹುಲಕ_w) {
    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename_w, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = filename_w;
    file = _wfsopen(ಗುರಿ, ಬಹುಲಕ_w, _SH_DENYNO);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ಬಹುಲಕ_w);
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename, &fixed))
    ಗುರಿ = fixed;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗುರಿ = filename;
  file = _fsopen(ಗುರಿ, ಬಹುಲಕ, _SH_DENYNO);
#ಆದರೆ_ಅಂತ್ಯ

  CURLX_FREE(fixed);
  ಹಿಂತಿರುಗಿ file;
}

#ಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE *ಹರಿವು);
#ಆದರೆ_ಅಂತ್ಯ

FILE *curlx_win32_freopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE *fp)
{
  FILE *file = ಖಾಲಿ;
  TCHAR *fixed = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_ಗಮ್ಯ_wchar(filename);
  wchar_t *ಬಹುಲಕ_w = fn_convert_UTF8_ಗಮ್ಯ_wchar(ಬಹುಲಕ);
  ಆದರೆ(filename_w && ಬಹುಲಕ_w) {
    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename_w, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = filename_w;
    errno = _wfreopen_s(&file, ಗುರಿ, ಬಹುಲಕ_w, fp);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(ಬಹುಲಕ_w);
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(fix_excessive_ಮಾರ್ಗ(filename, &fixed))
    ಗುರಿ = fixed;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗುರಿ = filename;
  errno = freopen_s(&file, ಗುರಿ, ಬಹುಲಕ, fp);
#ಆದರೆ_ಅಂತ್ಯ

  CURLX_FREE(fixed);
  ಹಿಂತಿರುಗಿ file;
}

ಪೂರ್ಣಾಂಕ curlx_win32_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, curlx_struct_stat *ಬಫರ್)
{
  ಪೂರ್ಣಾಂಕ res = -1;
  TCHAR *fixed = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  wchar_t *ಮಾರ್ಗ_w = curlx_convert_UTF8_ಗಮ್ಯ_wchar(ಮಾರ್ಗ);
  ಆದರೆ(ಮಾರ್ಗ_w) {
    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(ಮಾರ್ಗ_w, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = ಮಾರ್ಗ_w;
    res = _wstati64(ಗುರಿ, ಬಫರ್);
    curlx_free(ಮಾರ್ಗ_w);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(fix_excessive_ಮಾರ್ಗ(ಮಾರ್ಗ, &fixed))
    ಗುರಿ = fixed;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗುರಿ = ಮಾರ್ಗ;
  res = _stati64(ಗುರಿ, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

  CURLX_FREE(fixed);
  ಹಿಂತಿರುಗಿ res;
}

ಪೂರ್ಣಾಂಕ curlx_win32_mkdir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಪೂರ್ಣಾಂಕ res = -1;
  TCHAR *fixed = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UNICODE
  wchar_t *ಮಾರ್ಗ_w = curlx_convert_UTF8_ಗಮ್ಯ_wchar(ಮಾರ್ಗ);
  ಆದರೆ(ಮಾರ್ಗ_w) {
    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(ಮಾರ್ಗ_w, &fixed))
      ಗುರಿ = fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ = ಮಾರ್ಗ_w;
    res = _wmkdir(ಗುರಿ);
    curlx_free(ಮಾರ್ಗ_w);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(fix_excessive_ಮಾರ್ಗ(ಮಾರ್ಗ, &fixed))
    ಗುರಿ = fixed;
  ಇಲ್ಲದಿದ್ದರೆ
    ಗುರಿ = ಮಾರ್ಗ;
  res = _mkdir(ಗುರಿ);
#ಆದರೆ_ಅಂತ್ಯ

  CURLX_FREE(fixed);
  ಹಿಂತಿರುಗಿ res;
}

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
ಪೂರ್ಣಾಂಕ curlx_win32_rename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath)
{
  ಪೂರ್ಣಾಂಕ res = -1; /* fail */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_ಗಮ್ಯ_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_ಗಮ್ಯ_wchar(newpath);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ TCHAR *tchar_oldpath = oldpath;
  ಸ್ಥಿರಾಂಕ TCHAR *tchar_newpath = newpath;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(tchar_oldpath && tchar_newpath) {
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ max_wait_ms = 1000;
    ರಚನೆ curltime ಆರಂಭ;

    TCHAR *oldpath_fixed = ಖಾಲಿ;
    TCHAR *newpath_fixed = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ_oldpath;
    ಸ್ಥಿರಾಂಕ TCHAR *ಗುರಿ_newpath;

    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(tchar_oldpath, &oldpath_fixed))
      ಗುರಿ_oldpath = oldpath_fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ_oldpath = tchar_oldpath;

    ಆದರೆ(fix_excessive_ಮಾರ್ಗ(tchar_newpath, &newpath_fixed))
      ಗುರಿ_newpath = newpath_fixed;
    ಇಲ್ಲದಿದ್ದರೆ
      ಗುರಿ_newpath = tchar_newpath;

    ಆರಂಭ = curlx_now();

    ಗಾಗಿ(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      ಆದರೆ(MoveFileEx(ಗುರಿ_oldpath, ಗುರಿ_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        ನಿಲ್ಲಿಸು;
      }
      diff = curlx_timediff_ms(curlx_now(), ಆರಂಭ);
      ಆದರೆ(diff < 0 || diff > max_wait_ms) {
        ನಿಲ್ಲಿಸು;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CURLX_MALLOC
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CURLX_FREE

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
