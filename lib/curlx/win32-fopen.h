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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#ଅନ୍ତର୍ଭାଵ <fcntl.h>  /* for open() and attributes */
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_fseek(ଶୂନ୍ୟ *stream, curl_off_t offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା whence);

#ifdef _WIN32
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ଅଚଳ ଅକ୍ଷର *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ଅଚଳ ଅକ୍ଷର *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       ସଂରଚନା _stati64
FILE *curlx_win32_fopen(ଅଚଳ ଅକ୍ଷର *filename, ଅଚଳ ଅକ୍ଷର *mode);
FILE *curlx_win32_freopen(ଅଚଳ ଅକ୍ଷର *filename, ଅଚଳ ଅକ୍ଷର *mode, FILE *fp);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_stat(ଅଚଳ ଅକ୍ଷର *path, curlx_struct_stat *buffer);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_open(ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା oflag, ...);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_rename(ଅଚଳ ଅକ୍ଷର *oldpath, ଅଚଳ ଅକ୍ଷର *newpath);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_win32_mkdir(ଅଚଳ ଅକ୍ଷର *path);
#define CURLX_FOPEN_ତଳ         curlx_win32_fopen
#define CURLX_FREOPEN_ତଳ       curlx_win32_freopen
#define CURLX_FDOPEN_ତଳ        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#ଅନ୍ୟଥା
#define curlx_fstat             fstat
#define curlx_struct_stat       ସଂରଚନା stat
#define CURLX_FOPEN_ତଳ         fopen
#define CURLX_FREOPEN_ତଳ       freopen
#define CURLX_FDOPEN_ତଳ        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#ଯଦି defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
#define curlx_fopen             CURLX_FOPEN_ତଳ
#define curlx_freopen           CURLX_FREOPEN_ତଳ
#define curlx_fdopen            CURLX_FDOPEN_ତଳ
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
