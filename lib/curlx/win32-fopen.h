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
#شامل "curl_setup.h"

#شامل "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#شامل <fcntl.h>  /* for open() and attributes */
#endif

صحیح curlx_fseek(تهی *stream, curl_off_t offset, صحیح whence);

#ifdef _WIN32
#شامل <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ثابت نویسه *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ثابت نویسه *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       ساختار _stati64
FILE *curlx_win32_fopen(ثابت نویسه *filename, ثابت نویسه *mode);
FILE *curlx_win32_freopen(ثابت نویسه *filename, ثابت نویسه *mode, FILE *fp);
صحیح curlx_win32_stat(ثابت نویسه *path, curlx_struct_stat *buffer);
صحیح curlx_win32_open(ثابت نویسه *filename, صحیح oflag, ...);
صحیح curlx_win32_rename(ثابت نویسه *oldpath, ثابت نویسه *newpath);
صحیح curlx_win32_mkdir(ثابت نویسه *path);
#define CURLX_FOPEN_کم         curlx_win32_fopen
#define CURLX_FREOPEN_کم       curlx_win32_freopen
#define CURLX_FDOPEN_کم        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#وگرنه
#define curlx_fstat             fstat
#define curlx_struct_stat       ساختار stat
#define CURLX_FOPEN_کم         fopen
#define CURLX_FREOPEN_کم       freopen
#define CURLX_FDOPEN_کم        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#اگر defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#وگرنه
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
#وگرنه
#define curlx_fopen             CURLX_FOPEN_کم
#define curlx_freopen           CURLX_FREOPEN_کم
#define curlx_fdopen            CURLX_FDOPEN_کم
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
