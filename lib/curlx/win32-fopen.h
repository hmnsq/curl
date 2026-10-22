#جے_بنا_تعریف سرخی_CURLX_FOPEN_H
#تعریف_کرو سرخی_CURLX_FOPEN_H
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

#جے_تعریف_شدہ HAVE_FCNTL_H
#ضمن <fcntl.h>  /* for open() and attributes */
#جے_انت

صحيح curlx_fseek(فراغ *دھارا, curl_off_t وتھ, صحيح whence);

#جے_تعریف_شدہ _WIN32
#ضمن <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#جے_بنا_تعریف CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_منوال,
                        LPSECURITY_گن lp_Security_گن,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_گن,
                        HANDLE hTemplateFile);
HANDLE curlx_لبھو_پہلا_File(ثابت حرف *filename,
                           WIN32_لبھو_انکڑے *لبھو_انکڑے);
#جے_انت /* !CURL_WINDOWS_UWP */
#تعریف_کرو curlx_fstat             _fstati64
#تعریف_کرو curlx_struct_stat       بنية _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *منوال);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *منوال, FILE *fp);
صحيح curlx_win32_stat(ثابت حرف *رستہ, curlx_struct_stat *بفر);
صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...);
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحيح curlx_win32_mkdir(ثابت حرف *رستہ);
#تعریف_کرو CURLX_FOPEN_کم         curlx_win32_fopen
#تعریف_کرو CURLX_FREOPEN_کم       curlx_win32_freopen
#تعریف_کرو CURLX_FDOPEN_کم        _fdopen
#تعریف_کرو curlx_stat              curlx_win32_stat
#تعریف_کرو curlx_open              curlx_win32_open
#تعریف_کرو curlx_close             _close
#تعریف_کرو curlx_rename            curlx_win32_rename
#تعریف_کرو curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وإلا
#تعریف_کرو curlx_fstat             fstat
#تعریف_کرو curlx_struct_stat       بنية stat
#تعریف_کرو CURLX_FOPEN_کم         fopen
#تعریف_کرو CURLX_FREOPEN_کم       freopen
#تعریف_کرو CURLX_FDOPEN_کم        fdopen
#تعریف_کرو curlx_stat              stat
#تعریف_کرو curlx_open              open
#تعریف_کرو curlx_close             close
#تعریف_کرو curlx_rename            rename
#إذا defined(MSDOS) && !defined(__DJGPP__)
#تعریف_کرو curlx_mkdir(x, y)       mkdir(x)
#وإلا
#تعریف_کرو curlx_mkdir             mkdir
#جے_انت
#جے_انت

#جے_تعریف_شدہ CURL_MEMDEBUG
#تعریف_کرو curlx_fopen(file, منوال) curl_dbg_fopen(file, منوال, __LINE__, __FILE__)
#تعریف_کرو curlx_freopen(file, منوال, fh) \
  curl_dbg_freopen(file, منوال, fh, __LINE__, __FILE__)
#تعریف_کرو curlx_fdopen(file, منوال) \
  curl_dbg_fdopen(file, منوال, __LINE__, __FILE__)
#تعریف_کرو curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#وإلا
#تعریف_کرو curlx_fopen             CURLX_FOPEN_کم
#تعریف_کرو curlx_freopen           CURLX_FREOPEN_کم
#تعریف_کرو curlx_fdopen            CURLX_FDOPEN_کم
#تعریف_کرو curlx_fclose            fclose
#جے_انت

#جے_انت /* HEADER_CURLX_FOPEN_H */
