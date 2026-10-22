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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>  /* for open() and attributes */
#endif

ಪೂರ್ಣಾಙ್ಕ curlx_fseek(ಶೂನ್ಯ *stream, curl_off_t offset, ಪೂರ್ಣಾಙ್ಕ whence);

#ifdef _WIN32
#ಅನ್ತರ್ಭಾವ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ಧ್ರುವ ವರ್ಣ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ಧ್ರುವ ವರ್ಣ *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       ಸಂರಚನಾ _stati64
FILE *curlx_win32_fopen(ಧ್ರುವ ವರ್ಣ *filename, ಧ್ರುವ ವರ್ಣ *mode);
FILE *curlx_win32_freopen(ಧ್ರುವ ವರ್ಣ *filename, ಧ್ರುವ ವರ್ಣ *mode, FILE *fp);
ಪೂರ್ಣಾಙ್ಕ curlx_win32_stat(ಧ್ರುವ ವರ್ಣ *path, curlx_struct_stat *buffer);
ಪೂರ್ಣಾಙ್ಕ curlx_win32_open(ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ oflag, ...);
ಪೂರ್ಣಾಙ್ಕ curlx_win32_rename(ಧ್ರುವ ವರ್ಣ *oldpath, ಧ್ರುವ ವರ್ಣ *newpath);
ಪೂರ್ಣಾಙ್ಕ curlx_win32_mkdir(ಧ್ರುವ ವರ್ಣ *path);
#define CURLX_FOPEN_ಕೆಳಗಿನ         curlx_win32_fopen
#define CURLX_FREOPEN_ಕೆಳಗಿನ       curlx_win32_freopen
#define CURLX_FDOPEN_ಕೆಳಗಿನ        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#ಅನ್ಯಥಾ
#define curlx_fstat             fstat
#define curlx_struct_stat       ಸಂರಚನಾ stat
#define CURLX_FOPEN_ಕೆಳಗಿನ         fopen
#define CURLX_FREOPEN_ಕೆಳಗಿನ       freopen
#define CURLX_FDOPEN_ಕೆಳಗಿನ        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#ಯದಿ defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#ಅನ್ಯಥಾ
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
#ಅನ್ಯಥಾ
#define curlx_fopen             CURLX_FOPEN_ಕೆಳಗಿನ
#define curlx_freopen           CURLX_FREOPEN_ಕೆಳಗಿನ
#define curlx_fdopen            CURLX_FDOPEN_ಕೆಳಗಿನ
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
