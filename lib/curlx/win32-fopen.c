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
#समावेश "curl_setup.h"

#समावेश "curlx/win32-fopen.h"

पूर्णांक curlx_fseek(व्योम *stream, curl_off_t offset, पूर्णांक whence)
{
#अगर_घोषित _WIN32
  वापस _fseeki64(stream, (__int64)offset, whence);
#या_अगर defined(HAVE_FSEEKO) && defined(HAVE_DECL_FSEEKO)
  वापस fseeko(stream, (off_t)offset, whence);
#अन्यथा
  अगर(offset > दीर्घ_उच्च)
    वापस -1;
  वापस ख_जाओ(stream, (दीर्घ)offset, whence);
#पूर्ण_अगर
}

#अगर_घोषित _WIN32

#समावेश <direct.h>  /* for _mkdir(), _wmkdir() */
#समावेश <share.h>  /* for _SH_DENYNO */

#समावेश "curlx/multibyte.h"
#समावेश "curlx/timeval.h"

#अगर_घोषित CURL_MEMDEBUG
/*
 * Use system allocators to avoid infinite recursion when called by curl's
 * memory tracker memdebug functions.
 */
#घोषणा CURLX_MALLOC(x) दो_स्मृति(x)
#घोषणा CURLX_FREE(x)   मुक्त(x)
#अन्यथा
#घोषणा CURLX_MALLOC(x) curlx_malloc(x)
#घोषणा CURLX_FREE(x)   curlx_free(x)
#पूर्ण_अगर

#अगर_घोषित _UNICODE
अटल ब_अक्षर_प्रकार *fn_convert_UTF8_to_wchar(स्थिर अक्षर *str_utf8)
{
  ब_अक्षर_प्रकार *str_w = शून्य;

  अगर(str_utf8) {
    पूर्णांक str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                        str_utf8, -1, शून्य, 0);
    अगर(str_w_len > 0) {
      str_w = CURLX_MALLOC(str_w_len * माप(ब_अक्षर_प्रकार));
      अगर(str_w) {
        अगर(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               str_utf8, -1, str_w, str_w_len) == 0) {
          CURLX_FREE(str_w);
          वापस शून्य;
        }
      }
    }
  }
  वापस str_w;
}
#पूर्ण_अगर

/* declare GetFullPathNameW for mingw-w64 UWP builds targeting old Windows */
#अगर defined(CURL_WINDOWS_UWP) && defined(__MINGW32__) && \
  (_WIN32_WINNT < _WIN32_WINNT_WIN10)
WINBASEAPI DWORD WINAPI GetFullPathNameW(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#पूर्ण_अगर

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
अटल bool fix_excessive_path(स्थिर TCHAR *in, TCHAR **out)
{
  माप_प्रकार needed, गिनती;
  स्थिर ब_अक्षर_प्रकार *in_w;
  ब_अक्षर_प्रकार *fbuf = शून्य;

  /* MS-documented "approximate" limit for the maximum path length */
  स्थिर माप_प्रकार max_path_len = 32767;

#अगर_अघोषित _UNICODE
  ब_अक्षर_प्रकार *ibuf = शून्य;
  अक्षर *obuf = शून्य;
#पूर्ण_अगर

  *out = शून्य;

  /* skip paths already normalized */
  अगर(!_tcsncmp(in, _TEXT("\\\\?\\"), 4))
    जाओ cleanup;

#अगर_अघोषित _UNICODE
  /* convert multibyte input to unicode */
  अगर(mbstowcs_s(&needed, शून्य, 0, in, 0))
    जाओ cleanup;
  अगर(!needed || needed >= max_path_len)
    जाओ cleanup;
  ibuf = CURLX_MALLOC(needed * माप(ब_अक्षर_प्रकार));
  अगर(!ibuf)
    जाओ cleanup;
  अगर(mbstowcs_s(&गिनती, ibuf, needed, in, needed - 1))
    जाओ cleanup;
  अगर(गिनती != needed)
    जाओ cleanup;
  in_w = ibuf;
#अन्यथा
  in_w = in;
#पूर्ण_अगर

  /* GetFullPathNameW returns the normalized full path in unicode. It converts
     forward slashes to backslashes, processes .. to remove directory segments,
     etc. Unlike GetFullPathNameA it can process paths that exceed MAX_PATH. */
  needed = (माप_प्रकार)GetFullPathNameW(in_w, 0, शून्य, शून्य);
  अगर(!needed || needed > max_path_len)
    जाओ cleanup;
  /* skip paths that are not excessive and do not need modification */
  अगर(needed <= MAX_PATH)
    जाओ cleanup;
  fbuf = CURLX_MALLOC(needed * माप(ब_अक्षर_प्रकार));
  अगर(!fbuf)
    जाओ cleanup;
  गिनती = (माप_प्रकार)GetFullPathNameW(in_w, (DWORD)needed, fbuf, शून्य);
  अगर(!गिनती || गिनती >= needed)
    जाओ cleanup;

  /* prepend \\?\ or \\?\UNC\ to the excessively long path.
   *
   * c:\longpath            --->    \\?\c:\longpath
   * \\.\c:\longpath        --->    \\?\c:\longpath
   * \\?\c:\longpath        --->    \\?\c:\longpath  (unchanged)
   * \\server\c$\longpath   --->    \\?\UNC\server\c$\longpath
   *
   * https://learn.microsoft.com/dotnet/standard/io/file-path-formats
   */
  अगर(!wcsncmp(fbuf, L"\\\\?\\", 4))
    ; /* do nothing */
  अन्यथा अगर(!wcsncmp(fbuf, L"\\\\.\\", 4))
    fbuf[2] = '?';
  अन्यथा अगर(!wcsncmp(fbuf, L"\\\\.", 3) || !wcsncmp(fbuf, L"\\\\?", 3)) {
    /* Unexpected, not UNC. The formatting doc does not allow this AFAICT. */
    जाओ cleanup;
  }
  अन्यथा {
    ब_अक्षर_प्रकार *अस्थायी;

    अगर(!wcsncmp(fbuf, L"\\\\", 2)) {
      /* "\\?\UNC\" + full path without "\\" + null */
      needed = 8 + (गिनती - 2) + 1;
      अगर(needed > max_path_len)
        जाओ cleanup;

      अस्थायी = CURLX_MALLOC(needed * माप(ब_अक्षर_प्रकार));
      अगर(!अस्थायी)
        जाओ cleanup;

      अगर(wcsncpy_s(अस्थायी, needed, L"\\\\?\\UNC\\", 8)) {
        CURLX_FREE(अस्थायी);
        जाओ cleanup;
      }
      अगर(wcscpy_s(अस्थायी + 8, needed, fbuf + 2)) {
        CURLX_FREE(अस्थायी);
        जाओ cleanup;
      }
    }
    अन्यथा {
      /* "\\?\" + full path + null */
      needed = 4 + गिनती + 1;
      अगर(needed > max_path_len)
        जाओ cleanup;

      अस्थायी = CURLX_MALLOC(needed * माप(ब_अक्षर_प्रकार));
      अगर(!अस्थायी)
        जाओ cleanup;

      अगर(wcsncpy_s(अस्थायी, needed, L"\\\\?\\", 4)) {
        CURLX_FREE(अस्थायी);
        जाओ cleanup;
      }
      अगर(wcscpy_s(अस्थायी + 4, needed, fbuf)) {
        CURLX_FREE(अस्थायी);
        जाओ cleanup;
      }
    }

    CURLX_FREE(fbuf);
    fbuf = अस्थायी;
  }

#अगर_अघोषित _UNICODE
  /* convert unicode full path to multibyte output */
  अगर(wcstombs_s(&needed, शून्य, 0, fbuf, 0))
    जाओ cleanup;
  अगर(!needed || needed >= max_path_len)
    जाओ cleanup;
  obuf = CURLX_MALLOC(needed);
  अगर(!obuf)
    जाओ cleanup;
  अगर(wcstombs_s(&गिनती, obuf, needed, fbuf, needed - 1))
    जाओ cleanup;
  अगर(गिनती != needed)
    जाओ cleanup;
  *out = obuf;
  obuf = शून्य;
#अन्यथा
  *out = fbuf;
  fbuf = शून्य;
#पूर्ण_अगर

cleanup:
  CURLX_FREE(fbuf);
#अगर_अघोषित _UNICODE
  CURLX_FREE(ibuf);
  CURLX_FREE(obuf);
#पूर्ण_अगर
  वापस !!*out;
}

#अगर_अघोषित CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(स्थिर अक्षर *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile)
{
  HANDLE handle = INVALID_HANDLE_मूल्य;

#अगर_घोषित UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#अन्यथा
  स्थिर TCHAR *filename_t = filename;
#पूर्ण_अगर

  अगर(filename_t) {
    TCHAR *fixed = शून्य;
    स्थिर TCHAR *target = शून्य;

    अगर(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    अन्यथा
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
#अगर_घोषित UNICODE
    curlx_free(filename_t);
#पूर्ण_अगर
  }

  वापस handle;
}

HANDLE curlx_FindFirstFile(स्थिर अक्षर *filename,
                           WIN32_FIND_तथ्य *find_तथ्य)
{
  HANDLE handle = INVALID_HANDLE_मूल्य;

#अगर_घोषित UNICODE
  TCHAR *filename_t = curlx_convert_UTF8_to_wchar(filename);
#अन्यथा
  स्थिर TCHAR *filename_t = filename;
#पूर्ण_अगर

  अगर(filename_t) {
    TCHAR *fixed = शून्य;
    स्थिर TCHAR *target;

    अगर(fix_excessive_path(filename_t, &fixed))
      target = fixed;
    अन्यथा
      target = filename_t;

    handle = FindFirstFile(target, find_तथ्य);
    CURLX_FREE(fixed);

#अगर_घोषित UNICODE
    curlx_free(filename_t);
#पूर्ण_अगर
  }

  वापस handle;
}
#पूर्ण_अगर /* !CURL_WINDOWS_UWP */

पूर्णांक curlx_win32_open(स्थिर अक्षर *filename, पूर्णांक oflag, ...)
{
  पूर्णांक pmode = 0;
  पूर्णांक res = -1;
  TCHAR *fixed = शून्य;
  स्थिर TCHAR *target = शून्य;

#अगर_घोषित _UNICODE
  ब_अक्षर_प्रकार *filename_w = fn_convert_UTF8_to_wchar(filename);
#पूर्ण_अगर

  बहु_सूची param;
  बहु_शुरू(param, oflag);
  अगर(oflag & O_CREAT)
    pmode = बहु_तर्क(param, पूर्णांक);
  बहु_पूर्ण(param);

#अगर_घोषित _UNICODE
  अगर(filename_w) {
    अगर(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    त्रुटि_सं = _wsopen_s(&res, target, oflag, _SH_DENYNO, pmode);
    CURLX_FREE(filename_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = EINVAL;
#अन्यथा
  अगर(fix_excessive_path(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  त्रुटि_सं = _sopen_s(&res, target, oflag, _SH_DENYNO, pmode);
#पूर्ण_अगर

  CURLX_FREE(fixed);
  वापस res;
}

खाता *curlx_win32_fopen(स्थिर अक्षर *filename, स्थिर अक्षर *mode)
{
  खाता *file = शून्य;
  TCHAR *fixed = शून्य;
  स्थिर TCHAR *target = शून्य;

#अगर_घोषित _UNICODE
  ब_अक्षर_प्रकार *filename_w = fn_convert_UTF8_to_wchar(filename);
  ब_अक्षर_प्रकार *mode_w = fn_convert_UTF8_to_wchar(mode);
  अगर(filename_w && mode_w) {
    अगर(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    file = _wfsopen(target, mode_w, _SH_DENYNO);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#अन्यथा
  अगर(fix_excessive_path(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  file = _fsopen(target, mode, _SH_DENYNO);
#पूर्ण_अगर

  CURLX_FREE(fixed);
  वापस file;
}

#अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR < 5)
_CRTIMP errno_t __cdecl freopen_s(खाता **file, स्थिर अक्षर *filename,
                                  स्थिर अक्षर *mode, खाता *stream);
#पूर्ण_अगर

खाता *curlx_win32_freopen(स्थिर अक्षर *filename, स्थिर अक्षर *mode, खाता *fp)
{
  खाता *file = शून्य;
  TCHAR *fixed = शून्य;
  स्थिर TCHAR *target = शून्य;

#अगर_घोषित _UNICODE
  ब_अक्षर_प्रकार *filename_w = fn_convert_UTF8_to_wchar(filename);
  ब_अक्षर_प्रकार *mode_w = fn_convert_UTF8_to_wchar(mode);
  अगर(filename_w && mode_w) {
    अगर(fix_excessive_path(filename_w, &fixed))
      target = fixed;
    अन्यथा
      target = filename_w;
    त्रुटि_सं = _wfreopen_s(&file, target, mode_w, fp);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = EINVAL;
  CURLX_FREE(filename_w);
  CURLX_FREE(mode_w);
#अन्यथा
  अगर(fix_excessive_path(filename, &fixed))
    target = fixed;
  अन्यथा
    target = filename;
  त्रुटि_सं = freopen_s(&file, target, mode, fp);
#पूर्ण_अगर

  CURLX_FREE(fixed);
  वापस file;
}

पूर्णांक curlx_win32_stat(स्थिर अक्षर *path, curlx_struct_stat *बफर)
{
  पूर्णांक res = -1;
  TCHAR *fixed = शून्य;
  स्थिर TCHAR *target = शून्य;

#अगर_घोषित _UNICODE
  ब_अक्षर_प्रकार *path_w = curlx_convert_UTF8_to_wchar(path);
  अगर(path_w) {
    अगर(fix_excessive_path(path_w, &fixed))
      target = fixed;
    अन्यथा
      target = path_w;
    res = _wstati64(target, बफर);
    curlx_free(path_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = EINVAL;
#अन्यथा
  अगर(fix_excessive_path(path, &fixed))
    target = fixed;
  अन्यथा
    target = path;
  res = _stati64(target, बफर);
#पूर्ण_अगर

  CURLX_FREE(fixed);
  वापस res;
}

पूर्णांक curlx_win32_mkdir(स्थिर अक्षर *path)
{
  पूर्णांक res = -1;
  TCHAR *fixed = शून्य;
  स्थिर TCHAR *target = शून्य;

#अगर_घोषित _UNICODE
  ब_अक्षर_प्रकार *path_w = curlx_convert_UTF8_to_wchar(path);
  अगर(path_w) {
    अगर(fix_excessive_path(path_w, &fixed))
      target = fixed;
    अन्यथा
      target = path_w;
    res = _wmkdir(target);
    curlx_free(path_w);
  }
  अन्यथा
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = EINVAL;
#अन्यथा
  अगर(fix_excessive_path(path, &fixed))
    target = fixed;
  अन्यथा
    target = path;
  res = _mkdir(target);
#पूर्ण_अगर

  CURLX_FREE(fixed);
  वापस res;
}

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_COOKIES) || \
  !defined(CURL_DISABLE_ALTSVC)
/* rename() on Windows does not overwrite, so we cannot use it here.
   MoveFileEx() does overwrite and is usually atomic but fails when there are
   open handles to the file. */
पूर्णांक curlx_win32_rename(स्थिर अक्षर *oldpath, स्थिर अक्षर *newpath)
{
  पूर्णांक res = -1; /* fail */

#अगर_घोषित UNICODE
  TCHAR *tchar_oldpath = curlx_convert_UTF8_to_wchar(oldpath);
  TCHAR *tchar_newpath = curlx_convert_UTF8_to_wchar(newpath);
#अन्यथा
  स्थिर TCHAR *tchar_oldpath = oldpath;
  स्थिर TCHAR *tchar_newpath = newpath;
#पूर्ण_अगर

  अगर(tchar_oldpath && tchar_newpath) {
    स्थिर पूर्णांक max_wait_ms = 1000;
    काष्ठा curltime start;

    TCHAR *oldpath_fixed = शून्य;
    TCHAR *newpath_fixed = शून्य;
    स्थिर TCHAR *target_oldpath;
    स्थिर TCHAR *target_newpath;

    अगर(fix_excessive_path(tchar_oldpath, &oldpath_fixed))
      target_oldpath = oldpath_fixed;
    अन्यथा
      target_oldpath = tchar_oldpath;

    अगर(fix_excessive_path(tchar_newpath, &newpath_fixed))
      target_newpath = newpath_fixed;
    अन्यथा
      target_newpath = tchar_newpath;

    start = curlx_now();

    क्रम(;;) {
      timediff_t diff;
      /* !checksrc! disable BANNEDFUNC 1 */
      अगर(MoveFileEx(target_oldpath, target_newpath,
                    MOVEFILE_REPLACE_EXISTING)) {
        res = 0; /* success */
        अवरोध;
      }
      diff = curlx_timediff_ms(curlx_now(), start);
      अगर(diff < 0 || diff > max_wait_ms) {
        अवरोध;
      }
      Sleep(1);
    }

    CURLX_FREE(oldpath_fixed);
    CURLX_FREE(newpath_fixed);
  }

#अगर_घोषित UNICODE
  curlx_free(tchar_oldpath);
  curlx_free(tchar_newpath);
#पूर्ण_अगर

  वापस res;
}
#पूर्ण_अगर

#अघोषित CURLX_MALLOC
#अघोषित CURLX_FREE

#पूर्ण_अगर /* _WIN32 */
