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

पूर्णाङ्क curlx_fseek(शून्य *प्रवाहः, curl_off_t विस्थापनम्, पूर्णाङ्क whence)
{
#यदि_परिभाषितम् _WIN32
  प्रत्यागम _fseeki64(प्रवाहः, (__int64)विस्थापनम्, whence);
#नोचेद्यदि defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  प्रत्यागम fseeko(प्रवाहः, (off_t)विस्थापनम्, whence);
#अन्यथा
  यदि(विस्थापनम् > LONG_MAX)
    प्रत्यागम -1;
  प्रत्यागम fseek(प्रवाहः, (दीर्घ)विस्थापनम्, whence);
#यदि_अन्तः
}

#यदि_परिभाषितम् _WIN32

#अन्तर्भाव <direct.h>  /* for _mkdir(), _wmkdir() */
#अन्तर्भाव <share.h>  /* for _SH_DENYNO */

#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "curlx/timeval.h"

#यदि_परिभाषितम् CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#परिभाषय CURLX_MALLOC(x) स्मृतिं_देहि(x)
#परिभाषय CURLX_FREE(x)   मुक्त(x)
#अन्यथा
#परिभाषय CURLX_MALLOC(x) curlx_malloc(x)
#परिभाषय CURLX_FREE(x)   curlx_free(x)
#यदि_अन्तः

#यदि_परिभाषितम् _UNICODE
स्थिर wchar_t *fn_convert_UTF8_गन्तव्यम्_wchar(ध्रुव वर्ण *str_utf8)
{
  wchar_t *str_w = रिक्त;

  यदि(str_utf8) {
    पूर्णाङ्क str_w_len = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, रिक्त, 0);
    यदि(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * परिमाण(wchar_t));
      यदि(str_w) {
        यदि(Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          प्रत्यागम रिक्त;
        }
      }
    }
  }
  प्रत्यागम str_w;
}
#यदि_अन्तः

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#यदि defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI Get_पूर्णम्_मार्गः_Name_W(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#यदि_अन्तः

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
स्थिर bool fix_excessive_मार्गः(ध्रुव TCHAR *अन्तर्, TCHAR **बहिर्गमनम्)
{
  size_t needed, गणनम्;
  ध्रुव wchar_t *अन्तर्_w;
  wchar_t *fbuf = रिक्त;

  /* MS-documented "approximate" limit for the maximum path length */
  ध्रुव size_t max_मार्गः_len = 32767;

#यदि_अपरिभाषितम् _UNICODE
  wchar_t *ibuf = रिक्त;
  वर्ण *obuf = रिक्त;
#यदि_अन्तः

  *बहिर्गमनम् = रिक्त;

  /* skip paths already normalized */
  यदि(!_tcsncmp(अन्तर्, _TEXT("\\\\?\\"), 4))
    गच्छ cleanup;

#यदि_अपरिभाषितम् _UNICODE
  /* convert multibyte input to unicode */
  यदि(mbstowcs_s(&needed, रिक्त, 0, अन्तर्, 0))
    गच्छ cleanup;
  यदि(!needed || needed >= max_मार्गः_len)
    गच्छ cleanup;
  ibuf = CURLX_MALLOC(needed * परिमाण(wchar_t));
  यदि(!ibuf)
    गच्छ cleanup;
  यदि(mbstowcs_s(&गणनम्, ibuf, needed, अन्तर्, needed - 1))
    गच्छ cleanup;
  यदि(गणनम् != needed)
    गच्छ cleanup;
  अन्तर्_w = ibuf;
#अन्यथा
  अन्तर्_w = अन्तर्;
#यदि_अन्तः

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (size_t)Get_पूर्णम्_मार्गः_Name_W(अन्तर्_w, 0, रिक्त, रिक्त);
  यदि(!needed || needed > max_मार्गः_len)
    गच्छ cleanup;
  /* skip paths that are not excessive and do not need modification */
  यदि(needed <= MAX_मार्गः)
    गच्छ cleanup;
  fbuf = CURLX_MALLOC(needed * परिमाण(wchar_t));
  यदि(!fbuf)
    गच्छ cleanup;
  गणनम् = (size_t)Get_पूर्णम्_मार्गः_Name_W(अन्तर्_w, (DWORD)needed, fbuf, रिक्त);
  यदि(!गणनम् || गणनम् >= needed)
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
  यदि(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  अन्यथा यदि(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  अन्यथा यदि(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    गच्छ cleanup;
  }
  अन्यथा {
    wchar_t *अस्थायी;

    यदि(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (गणनम् - 2) + 1;
      यदि(needed > max_मार्गः_len)
        गच्छ cleanup;

      अस्थायी = CURLX_MALLOC(needed * परिमाण(wchar_t));
      यदि(!अस्थायी)
        गच्छ cleanup;

      यदि(wcsncpy_s(अस्थायी, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(अस्थायी);
        गच्छ cleanup;
      }
      यदि(wcscpy_s(अस्थायी + 8, needed, fbuf + 2)) {
        CURLX_FREE(अस्थायी);
        गच्छ cleanup;
      }
    }
    अन्यथा {
      /* "\\?\" + full path + null */
      needed = 4 + गणनम् + 1;
      यदि(needed > max_मार्गः_len)
        गच्छ cleanup;

      अस्थायी = CURLX_MALLOC(needed * परिमाण(wchar_t));
      यदि(!अस्थायी)
        गच्छ cleanup;

      यदि(wcsncpy_s(अस्थायी, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(अस्थायी);
        गच्छ cleanup;
      }
      यदि(wcscpy_s(अस्थायी + 4, needed, fbuf)) {
        CURLX_FREE(अस्थायी);
        गच्छ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = अस्थायी;
  }

#यदि_अपरिभाषितम् _UNICODE
  /* convert unicode full path to multibyte output */
  यदि(wcstombs_s(&needed, रिक्त, 0, fbuf, 0))
    गच्छ cleanup;
  यदि(!needed || needed >= max_मार्गः_len)
    गच्छ cleanup;
  obuf = CURLX_MALLOC(needed);
  यदि(!obuf)
    गच्छ cleanup;
  यदि(wcstombs_s(&गणनम्, obuf, needed, fbuf, needed - 1))
    गच्छ cleanup;
  यदि(गणनम् != needed)
    गच्छ cleanup;
  *बहिर्गमनम् = obuf;
  obuf = रिक्त;
#अन्यथा
  *बहिर्गमनम् = fbuf;
  fbuf = रिक्त;
#यदि_अन्तः

cleanup:
  CURLX_FREE(fbuf);
#यदि_अपरिभाषितम् _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#यदि_अन्तः
  प्रत्यागम !!*बहिर्गमनम्;
}

#यदि_अपरिभाषितम् CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ध्रुव वर्ण *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_बहुलकम्,
                        LPSECURITY_गुणाः lp_Security_गुणाः,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_गुणाः,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_मानम्;

#यदि_परिभाषितम् UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_गन्तव्यम्_wchar(filename);
#अन्यथा
  ध्रुव TCHAR *filename_t = filename;
#यदि_अन्तः

  यदि(filename_t) {
    TCHAR *fixed = रिक्त;
    ध्रुव TCHAR *target = रिक्त;

    यदि(fix_excessive_मार्गः(filename_t, &fixed))
      target = fixed;
    अन्यथा
      target = filename_t;
    /* !checksrc! disable BANNEDFUNC 1 */
    handle = CreateFile(target,
                        dwDesiredAccess,
                        dw_Share_बहुलकम्,
                        lp_Security_गुणाः,
                        dwCreationDisposition,
                        dw_Flags_And_गुणाः,
                        hTemplateFile);
    CURLX_FREE(fixed);
#यदि_परिभाषितम् UNICODE
    curlx_free(filename_t);
#यदि_अन्तः
  }

  प्रत्यागम handle;
}

HANDLE curlx_FindFirstFile(ध्रुव वर्ण *filename,
                           WIN32_FIND_दत्त *find_दत्त)
{
  HANDLE handle = INVALID_HANDLE_मानम्;

#यदि_परिभाषितम् UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_गन्तव्यम्_wchar(filename);
#अन्यथा
  ध्रुव TCHAR *filename_t = filename;
#यदि_अन्तः

  यदि(filename_t) {
    TCHAR *fixed = रिक्त;
    ध्रुव TCHAR *target;

    यदि(fix_excessive_मार्गः(filename_t, &fixed))
      target = fixed;
    अन्यथा
      target = filename_t;

    handle = FindFirstFile(target, find_दत्त);
    CURLX_FREE(fixed);

#यदि_परिभाषितम् UNICODE
    curlx_free(filename_t);
#यदि_अन्तः
  }

  प्रत्यागम handle;
}
#यदि_अन्तः /* !CURL_WINDOWS_UWP */

पूर्णाङ्क curlx_win32_open(ध्रुव वर्ण *filename, पूर्णाङ्क oflag, ...)
{
  पूर्णाङ्क pmode = 0;
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#यदि_परिभाषितम् _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_गन्तव्यम्_wchar(filename);
#यदि_अन्तः

  va_list param;
  va_start(param, oflag);
  यदि(oflag & O_CREAT)
    pmode = va_arg(param, पूर्णाङ्क);
  va_end(param);

#यदि_परिभाषितम् _UNICODE
  यदि(filename_w) {
    यदि(fix_excessive_मार्गः(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    errno = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अन्यथा
  यदि(fix_excessive_मार्गः(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  errno = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#यदि_अन्तः

  CURLX_FREE(fixed);
  प्रत्यागम res;
}

FILE *curlx_win32_fopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *बहुलकम्)
{
  FILE *file = रिक्त;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#यदि_परिभाषितम् _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_गन्तव्यम्_wchar(filename);
  wchar_t *बहुलकम्_w = fn_convert_UTF8_गन्तव्यम्_wchar(बहुलकम्);
  यदि(filename_w && बहुलकम्_w) {
    यदि(fix_excessive_मार्गः(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    file = _wfsopen(target, बहुलकम्_w, _SH_DENYNO);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(बहुलकम्_w);
#अन्यथा
  यदि(fix_excessive_मार्गः(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  file = _fsopen(target, बहुलकम्, _SH_DENYNO);
#यदि_अन्तः

  CURLX_FREE(fixed);
  प्रत्यागम file;
}

#यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(FILE **file, ध्रुव वर्ण *filename,
                                  ध्रुव वर्ण *बहुलकम्, FILE *प्रवाहः);
#यदि_अन्तः

FILE *curlx_win32_freopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *बहुलकम्, FILE *fp)
{
  FILE *file = रिक्त;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#यदि_परिभाषितम् _UNICODE
  wchar_t *filename_w = fn_convert_UTF8_गन्तव्यम्_wchar(filename);
  wchar_t *बहुलकम्_w = fn_convert_UTF8_गन्तव्यम्_wchar(बहुलकम्);
  यदि(filename_w && बहुलकम्_w) {
    यदि(fix_excessive_मार्गः(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    errno = _wfreopen_s(&file, target, बहुलकम्_w, fp);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(बहुलकम्_w);
#अन्यथा
  यदि(fix_excessive_मार्गः(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  errno = freopen_s(&file, target, बहुलकम्, fp);
#यदि_अन्तः

  CURLX_FREE(fixed);
  प्रत्यागम file;
}

पूर्णाङ्क curlx_win32_stat(ध्रुव वर्ण *मार्गः, curlx_struct_stat *बफर)
{
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#यदि_परिभाषितम् _UNICODE
  wchar_t *मार्गः_w = curlx_convert_UTF8_गन्तव्यम्_wchar(मार्गः);
  यदि(मार्गः_w) {
    यदि(fix_excessive_मार्गः(मार्गः_w, &fixed))
      target = fixed;
    अन्यथा
      target = मार्गः_w;
    res = _wstati64(target, बफर);
    curlx_free(मार्गः_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अन्यथा
  यदि(fix_excessive_मार्गः(मार्गः, &fixed))
    target = fixed;
  अन्यथा
    target = मार्गः;
  res = _stati64(target, बफर);
#यदि_अन्तः

  CURLX_FREE(fixed);
  प्रत्यागम res;
}

पूर्णाङ्क curlx_win32_mkdir(ध्रुव वर्ण *मार्गः)
{
  पूर्णाङ्क res = -1;
  TCHAR *fixed = रिक्त;
  ध्रुव TCHAR *target = रिक्त;

#यदि_परिभाषितम् _UNICODE
  wchar_t *मार्गः_w = curlx_convert_UTF8_गन्तव्यम्_wchar(मार्गः);
  यदि(मार्गः_w) {
    यदि(fix_excessive_मार्गः(मार्गः_w, &fixed))
      target = fixed;
    अन्यथा
      target = मार्गः_w;
    res = _wmkdir(target);
    curlx_free(मार्गः_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    errno = EINVAL;
#अन्यथा
  यदि(fix_excessive_मार्गः(मार्गः, &fixed))
    target = fixed;
  अन्यथा
    target = मार्गः;
  res = _mkdir(target);
#यदि_अन्तः

  CURLX_FREE(fixed);
  प्रत्यागम res;
}

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
पूर्णाङ्क curlx_win32_rename(ध्रुव वर्ण *oldpath, ध्रुव वर्ण *newpath)
{
  पूर्णाङ्क res = -1; /* fail */

#यदि_परिभाषितम् UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_गन्तव्यम्_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_गन्तव्यम्_wchar(newpath);
#अन्यथा
  ध्रुव TCHAR *tchar_oldpath = oldpath;
  ध्रुव TCHAR *tchar_newpath = newpath;
#यदि_अन्तः

  यदि(tchar_oldpath && tchar_newpath) {
    ध्रुव पूर्णाङ्क max_wait_ms = 1000;
    संरचना curltime start;

    TCHAR *oldpath_fixed = रिक्त;
    TCHAR *newpath_fixed = रिक्त;
    ध्रुव TCHAR *target_oldpath;
    ध्रुव TCHAR *target_newpath;

    यदि(fix_excessive_मार्गः(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    अन्यथा
      target_oldpath = tchar_oldpath;

    यदि(fix_excessive_मार्गः(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    अन्यथा
      target_newpath = tchar_newpath;

    start = curlx_now();

    प्रति(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      यदि(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        भङ्ग;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      यदि(diff < 0 || diff > max_wait_ms) {
        भङ्ग;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#यदि_परिभाषितम् UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#यदि_अन्तः

  प्रत्यागम res;
}
#यदि_अन्तः

#अपरिभाषय CURLX_MALLOC
#अपरिभाषय CURLX_FREE

#यदि_अन्तः /* _WIN32 */
