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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#অন্তর্ভাৱ <fcntl.h>  /* for open() and attributes */
#endif

পূর্ণাঙ্ক curlx_fseek(শূন্য *stream, curl_off_t offset, পূর্ণাঙ্ক whence);

#ifdef _WIN32
#অন্তর্ভাৱ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ধ্রুৱ ৱর্ণ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ধ্রুৱ ৱর্ণ *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       সংরচনা _stati64
FILE *curlx_win32_fopen(ধ্রুৱ ৱর্ণ *filename, ধ্রুৱ ৱর্ণ *mode);
FILE *curlx_win32_freopen(ধ্রুৱ ৱর্ণ *filename, ধ্রুৱ ৱর্ণ *mode, FILE *fp);
পূর্ণাঙ্ক curlx_win32_stat(ধ্রুৱ ৱর্ণ *path, curlx_struct_stat *buffer);
পূর্ণাঙ্ক curlx_win32_open(ধ্রুৱ ৱর্ণ *filename, পূর্ণাঙ্ক oflag, ...);
পূর্ণাঙ্ক curlx_win32_rename(ধ্রুৱ ৱর্ণ *oldpath, ধ্রুৱ ৱর্ণ *newpath);
পূর্ণাঙ্ক curlx_win32_mkdir(ধ্রুৱ ৱর্ণ *path);
#define CURLX_FOPEN_নিচেৰ         curlx_win32_fopen
#define CURLX_FREOPEN_নিচেৰ       curlx_win32_freopen
#define CURLX_FDOPEN_নিচেৰ        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#অন্যথা
#define curlx_fstat             fstat
#define curlx_struct_stat       সংরচনা stat
#define CURLX_FOPEN_নিচেৰ         fopen
#define CURLX_FREOPEN_নিচেৰ       freopen
#define CURLX_FDOPEN_নিচেৰ        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#যদি defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#অন্যথা
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
#অন্যথা
#define curlx_fopen             CURLX_FOPEN_নিচেৰ
#define curlx_freopen           CURLX_FREOPEN_নিচেৰ
#define curlx_fdopen            CURLX_FDOPEN_নিচেৰ
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
