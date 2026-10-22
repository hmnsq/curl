#که_نه_تعریف_شوی سرلیک_CURLX_FOPEN_H
#تعریف_کړه سرلیک_CURLX_FOPEN_H
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

#که_تعریف_شوی HAVE_FCNTL_H
#ضمن <fcntl.h>  /* for open() and attributes */
#د_که_پای

صحيح curlx_fseek(فراغ *بهیر, curl_off_t دننه_واټن, صحيح whence);

#که_تعریف_شوی _WIN32
#ضمن <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#که_نه_تعریف_شوی CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ډېرپېښ,
                        LPSECURITY_ځانګړتیاوې lp_Security_ځانګړتیاوې,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ځانګړتیاوې,
                        HANDLE hTemplateFile);
HANDLE curlx_پیدا_کړه_لومړی_File(ثابت حرف *filename,
                           WIN32_پیدا_کړه_معلومات *پیدا_کړه_معلومات);
#د_که_پای /* !CURL_WINDOWS_UWP */
#تعریف_کړه curlx_fstat             _fstati64
#تعریف_کړه curlx_struct_stat       بنية _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *ډېرپېښ);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *ډېرپېښ, FILE *fp);
صحيح curlx_win32_stat(ثابت حرف *لار, curlx_struct_stat *بفر);
صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...);
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحيح curlx_win32_mkdir(ثابت حرف *لار);
#تعریف_کړه CURLX_FOPEN_ټیټ         curlx_win32_fopen
#تعریف_کړه CURLX_FREOPEN_ټیټ       curlx_win32_freopen
#تعریف_کړه CURLX_FDOPEN_ټیټ        _fdopen
#تعریف_کړه curlx_stat              curlx_win32_stat
#تعریف_کړه curlx_open              curlx_win32_open
#تعریف_کړه curlx_close             _close
#تعریف_کړه curlx_rename            curlx_win32_rename
#تعریف_کړه curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وإلا
#تعریف_کړه curlx_fstat             fstat
#تعریف_کړه curlx_struct_stat       بنية stat
#تعریف_کړه CURLX_FOPEN_ټیټ         fopen
#تعریف_کړه CURLX_FREOPEN_ټیټ       freopen
#تعریف_کړه CURLX_FDOPEN_ټیټ        fdopen
#تعریف_کړه curlx_stat              stat
#تعریف_کړه curlx_open              open
#تعریف_کړه curlx_close             close
#تعریف_کړه curlx_rename            rename
#إذا defined(MSDOS) && !defined(__DJGPP__)
#تعریف_کړه curlx_mkdir(x, y)       mkdir(x)
#وإلا
#تعریف_کړه curlx_mkdir             mkdir
#د_که_پای
#د_که_پای

#که_تعریف_شوی CURL_MEMDEBUG
#تعریف_کړه curlx_fopen(file, ډېرپېښ) curl_dbg_fopen(file, ډېرپېښ, __LINE__, __FILE__)
#تعریف_کړه curlx_freopen(file, ډېرپېښ, fh) \
  curl_dbg_freopen(file, ډېرپېښ, fh, __LINE__, __FILE__)
#تعریف_کړه curlx_fdopen(file, ډېرپېښ) \
  curl_dbg_fdopen(file, ډېرپېښ, __LINE__, __FILE__)
#تعریف_کړه curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#وإلا
#تعریف_کړه curlx_fopen             CURLX_FOPEN_ټیټ
#تعریف_کړه curlx_freopen           CURLX_FREOPEN_ټیټ
#تعریف_کړه curlx_fdopen            CURLX_FDOPEN_ټیټ
#تعریف_کړه curlx_fclose            fclose
#د_که_پای

#د_که_پای /* HEADER_CURLX_FOPEN_H */
