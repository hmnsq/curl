#ifndef HEADER_CURLX_FOPEN_H
#define HEADER_CURLX_FOPEN_H
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

#ifdef HAVE_FCNTL_H
#ضمن <fcntl.h>  /* for open() and attributes */
#endif

صحيح curlx_fseek(فراغ *stream, curl_off_t offset, صحيح whence);

#ifdef _WIN32
#ضمن <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت حرف *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ثابت حرف *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       بنية _stati64
FILE *curlx_win32_fopen(ثابت حرف *filename, ثابت حرف *mode);
FILE *curlx_win32_freopen(ثابت حرف *filename, ثابت حرف *mode, FILE *fp);
صحيح curlx_win32_stat(ثابت حرف *path, curlx_struct_stat *buffer);
صحيح curlx_win32_open(ثابت حرف *filename, صحيح oflag, ...);
صحيح curlx_win32_rename(ثابت حرف *oldpath, ثابت حرف *newpath);
صحيح curlx_win32_mkdir(ثابت حرف *path);
#define CURLX_FOPEN_تۆۋەن         curlx_win32_fopen
#define CURLX_FREOPEN_تۆۋەن       curlx_win32_freopen
#define CURLX_FDOPEN_تۆۋەن        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وإلا
#define curlx_fstat             fstat
#define curlx_struct_stat       بنية stat
#define CURLX_FOPEN_تۆۋەن         fopen
#define CURLX_FREOPEN_تۆۋەن       freopen
#define CURLX_FDOPEN_تۆۋەن        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#إذا defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#وإلا
#define curlx_mkdir             mkdir
#endif
#endif

#ifdef CURL_MEMDEBUG
#define curlx_fopen(file, mode) curl_dbg_fopen(file, mode, __LINE__, __FILE__)
#define curlx_freopen(file, mode, fh) \
  curl_dbg_freopen(file, mode, fh, __LINE__, __FILE__)
#define curlx_fdopen(file, mode) \
  curl_dbg_fdopen(file, mode, __LINE__, __FILE__)
#define curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#وإلا
#define curlx_fopen             CURLX_FOPEN_تۆۋەن
#define curlx_freopen           CURLX_FREOPEN_تۆۋەن
#define curlx_fdopen            CURLX_FDOPEN_تۆۋەن
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
