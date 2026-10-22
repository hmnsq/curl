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
#include "curl_setup.h"

#include "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>  /* for open() and attributes */
#endif

පූර්ණ_සංඛ්‍යා curlx_fseek(void *stream, curl_off_t offset, පූර්ණ_සංඛ්‍යා whence);

#ifdef _WIN32
#include <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(const var_na *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(const var_na *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       sa_nracanaa _stati64
FILE *curlx_win32_fopen(const var_na *filename, const var_na *mode);
FILE *curlx_win32_freopen(const var_na *filename, const var_na *mode, FILE *fp);
පූර්ණ_සංඛ්‍යා curlx_win32_stat(const var_na *path, curlx_struct_stat *buffer);
පූර්ණ_සංඛ්‍යා curlx_win32_open(const var_na *filename, පූර්ණ_සංඛ්‍යා oflag, ...);
පූර්ණ_සංඛ්‍යා curlx_win32_rename(const var_na *oldpath, const var_na *newpath);
පූර්ණ_සංඛ්‍යා curlx_win32_mkdir(const var_na *path);
#define CURLX_FOPEN_LOW         curlx_win32_fopen
#define CURLX_FREOPEN_LOW       curlx_win32_freopen
#define CURLX_FDOPEN_LOW        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#නැතහොත්
#define curlx_fstat             fstat
#define curlx_struct_stat       sa_nracanaa stat
#define CURLX_FOPEN_LOW         fopen
#define CURLX_FREOPEN_LOW       freopen
#define CURLX_FDOPEN_LOW        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#නම් defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#නැතහොත්
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
#නැතහොත්
#define curlx_fopen             CURLX_FOPEN_LOW
#define curlx_freopen           CURLX_FREOPEN_LOW
#define curlx_fdopen            CURLX_FDOPEN_LOW
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
