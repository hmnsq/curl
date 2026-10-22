#اگر_غیر_تعریف_شدہ سرخی_CURLX_FOPEN_H
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
#شامل "curl_setup.h"

#شامل "curlx/multibyte.h"

#اگر_تعریف_شدہ HAVE_FCNTL_H
#شامل <fcntl.h>  /* for open() and attributes */
#اگر_ختم

صحیح curlx_fseek(خالی *دھارا, curl_off_t انحراف, صحیح whence);

#اگر_تعریف_شدہ _WIN32
#شامل <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#اگر_غیر_تعریف_شدہ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_منوال,
                        LPSECURITY_اوصاف lp_Security_اوصاف,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_اوصاف,
                        HANDLE hTemplateFile);
HANDLE curlx_ڈھونڈو_پہلا_File(ثابت حرف *filename,
                           WIN32_ڈھونڈو_ڈیٹا *ڈھونڈو_ڈیٹا);
#اگر_ختم /* !CURL_WINDOWS_UWP */
#تعریف_کرو curlx_fstat             _fstati64
#تعریف_کرو curlx_struct_stat       ساخت _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *منوال);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *منوال, FILE *fp);
صحیح curlx_win32_stat(ثابت حرف *راستہ, curlx_struct_stat *بفر);
صحیح curlx_win32_open(ثابت حرف *filename, صحیح oflag, ...);
صحیح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحیح curlx_win32_mkdir(ثابت حرف *راستہ);
#تعریف_کرو CURLX_FOPEN_کم         curlx_win32_fopen
#تعریف_کرو CURLX_FREOPEN_کم       curlx_win32_freopen
#تعریف_کرو CURLX_FDOPEN_کم        _fdopen
#تعریف_کرو curlx_stat              curlx_win32_stat
#تعریف_کرو curlx_open              curlx_win32_open
#تعریف_کرو curlx_close             _close
#تعریف_کرو curlx_rename            curlx_win32_rename
#تعریف_کرو curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#ورنہ
#تعریف_کرو curlx_fstat             fstat
#تعریف_کرو curlx_struct_stat       ساخت stat
#تعریف_کرو CURLX_FOPEN_کم         fopen
#تعریف_کرو CURLX_FREOPEN_کم       freopen
#تعریف_کرو CURLX_FDOPEN_کم        fdopen
#تعریف_کرو curlx_stat              stat
#تعریف_کرو curlx_open              open
#تعریف_کرو curlx_close             close
#تعریف_کرو curlx_rename            rename
#اگر defined(MSDOS) && !defined(__DJGPP__)
#تعریف_کرو curlx_mkdir(x, y)       mkdir(x)
#ورنہ
#تعریف_کرو curlx_mkdir             mkdir
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ CURL_MEMDEBUG
#تعریف_کرو curlx_fopen(file, منوال) curl_dbg_fopen(file, منوال, __LINE__, __FILE__)
#تعریف_کرو curlx_freopen(file, منوال, fh) \
  curl_dbg_freopen(file, منوال, fh, __LINE__, __FILE__)
#تعریف_کرو curlx_fdopen(file, منوال) \
  curl_dbg_fdopen(file, منوال, __LINE__, __FILE__)
#تعریف_کرو curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#ورنہ
#تعریف_کرو curlx_fopen             CURLX_FOPEN_کم
#تعریف_کرو curlx_freopen           CURLX_FREOPEN_کم
#تعریف_کرو curlx_fdopen            CURLX_FDOPEN_کم
#تعریف_کرو curlx_fclose            fclose
#اگر_ختم

#اگر_ختم /* HEADER_CURLX_FOPEN_H */
