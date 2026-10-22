#جيڪڏهن_وصف_نه_ٿيل سرو_CURLX_FOPEN_H
#وصف_ڪريو سرو_CURLX_FOPEN_H
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

#جيڪڏهن_وصف_ٿيل HAVE_FCNTL_H
#ضمن <fcntl.h>  /* for open() and attributes */
#جيڪڏهن_پورو

صحيح curlx_fseek(فراغ *وهڪرو, curl_off_t وٿي, صحيح whence);

#جيڪڏهن_وصف_ٿيل _WIN32
#ضمن <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#جيڪڏهن_وصف_نه_ٿيل CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_گهڻائي,
                        LPSECURITY_خاصيتون lp_Security_خاصيتون,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_خاصيتون,
                        HANDLE hTemplateFile);
HANDLE curlx_ڳوليو_پهريون_File(ثابت حرف *filename,
                           WIN32_ڳوليو_معلومات *ڳوليو_معلومات);
#جيڪڏهن_پورو /* !CURL_WINDOWS_UWP */
#وصف_ڪريو curlx_fstat             _fstati64
#وصف_ڪريو curlx_struct_stat       بنية _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *گهڻائي);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *گهڻائي, FILE *fp);
صحيح curlx_win32_stat(ثابت حرف *رستو, curlx_struct_stat *بفر);
صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...);
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحيح curlx_win32_mkdir(ثابت حرف *رستو);
#وصف_ڪريو CURLX_FOPEN_هيٺيون         curlx_win32_fopen
#وصف_ڪريو CURLX_FREOPEN_هيٺيون       curlx_win32_freopen
#وصف_ڪريو CURLX_FDOPEN_هيٺيون        _fdopen
#وصف_ڪريو curlx_stat              curlx_win32_stat
#وصف_ڪريو curlx_open              curlx_win32_open
#وصف_ڪريو curlx_close             _close
#وصف_ڪريو curlx_rename            curlx_win32_rename
#وصف_ڪريو curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وإلا
#وصف_ڪريو curlx_fstat             fstat
#وصف_ڪريو curlx_struct_stat       بنية stat
#وصف_ڪريو CURLX_FOPEN_هيٺيون         fopen
#وصف_ڪريو CURLX_FREOPEN_هيٺيون       freopen
#وصف_ڪريو CURLX_FDOPEN_هيٺيون        fdopen
#وصف_ڪريو curlx_stat              stat
#وصف_ڪريو curlx_open              open
#وصف_ڪريو curlx_close             close
#وصف_ڪريو curlx_rename            rename
#إذا defined(MSDOS) && !defined(__DJGPP__)
#وصف_ڪريو curlx_mkdir(x, y)       mkdir(x)
#وإلا
#وصف_ڪريو curlx_mkdir             mkdir
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURL_MEMDEBUG
#وصف_ڪريو curlx_fopen(file, گهڻائي) curl_dbg_fopen(file, گهڻائي, __LINE__, __FILE__)
#وصف_ڪريو curlx_freopen(file, گهڻائي, fh) \
  curl_dbg_freopen(file, گهڻائي, fh, __LINE__, __FILE__)
#وصف_ڪريو curlx_fdopen(file, گهڻائي) \
  curl_dbg_fdopen(file, گهڻائي, __LINE__, __FILE__)
#وصف_ڪريو curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#وإلا
#وصف_ڪريو curlx_fopen             CURLX_FOPEN_هيٺيون
#وصف_ڪريو curlx_freopen           CURLX_FREOPEN_هيٺيون
#وصف_ڪريو curlx_fdopen            CURLX_FDOPEN_هيٺيون
#وصف_ڪريو curlx_fclose            fclose
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURLX_FOPEN_H */
