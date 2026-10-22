#اگر_تعریف_نشده سرآیند_CURLX_FOPEN_H
#تعریف_کن سرآیند_CURLX_FOPEN_H
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

#ضمن "curlx/multibyte.h"

#اگر_تعریف_شده HAVE_FCNTL_H
#ضمن <fcntl.h>  /* for open() and attributes */
#پایان_اگر

صحيح curlx_fseek(فراغ *جریان, curl_off_t جابه_جایی, صحيح whence);

#اگر_تعریف_شده _WIN32
#ضمن <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#اگر_تعریف_نشده CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_نما,
                        LPSECURITY_صفات lp_Security_صفات,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_صفات,
                        HANDLE hTemplateFile);
HANDLE curlx_پیدا_کن_اولی_File(ثابت حرف *filename,
                           WIN32_پیدا_کن_داده *پیدا_کن_داده);
#پایان_اگر /* !CURL_WINDOWS_UWP */
#تعریف_کن curlx_fstat             _fstati64
#تعریف_کن curlx_struct_stat       بنية _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *نما);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *نما, FILE *fp);
صحيح curlx_win32_stat(ثابت حرف *مسیر, curlx_struct_stat *میانگیر);
صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...);
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحيح curlx_win32_mkdir(ثابت حرف *مسیر);
#تعریف_کن CURLX_FOPEN_کم         curlx_win32_fopen
#تعریف_کن CURLX_FREOPEN_کم       curlx_win32_freopen
#تعریف_کن CURLX_FDOPEN_کم        _fdopen
#تعریف_کن curlx_stat              curlx_win32_stat
#تعریف_کن curlx_open              curlx_win32_open
#تعریف_کن curlx_close             _close
#تعریف_کن curlx_rename            curlx_win32_rename
#تعریف_کن curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وإلا
#تعریف_کن curlx_fstat             fstat
#تعریف_کن curlx_struct_stat       بنية stat
#تعریف_کن CURLX_FOPEN_کم         fopen
#تعریف_کن CURLX_FREOPEN_کم       freopen
#تعریف_کن CURLX_FDOPEN_کم        fdopen
#تعریف_کن curlx_stat              stat
#تعریف_کن curlx_open              open
#تعریف_کن curlx_close             close
#تعریف_کن curlx_rename            rename
#إذا defined(MSDOS) && !defined(__DJGPP__)
#تعریف_کن curlx_mkdir(x, y)       mkdir(x)
#وإلا
#تعریف_کن curlx_mkdir             mkdir
#پایان_اگر
#پایان_اگر

#اگر_تعریف_شده CURL_MEMDEBUG
#تعریف_کن curlx_fopen(file, نما) curl_dbg_fopen(file, نما, __LINE__, __FILE__)
#تعریف_کن curlx_freopen(file, نما, fh) \
  curl_dbg_freopen(file, نما, fh, __LINE__, __FILE__)
#تعریف_کن curlx_fdopen(file, نما) \
  curl_dbg_fdopen(file, نما, __LINE__, __FILE__)
#تعریف_کن curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#وإلا
#تعریف_کن curlx_fopen             CURLX_FOPEN_کم
#تعریف_کن curlx_freopen           CURLX_FREOPEN_کم
#تعریف_کن curlx_fdopen            CURLX_FDOPEN_کم
#تعریف_کن curlx_fclose            fclose
#پایان_اگر

#پایان_اگر /* HEADER_CURLX_FOPEN_H */
