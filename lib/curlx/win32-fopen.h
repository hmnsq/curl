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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#અન્તર્ભાવ <fcntl.h>  /* for open() and attributes */
#endif

પૂર્ણાંક curlx_fseek(શૂન્ય *stream, curl_off_t offset, પૂર્ણાંક whence);

#ifdef _WIN32
#અન્તર્ભાવ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(અચલ અક્ષર *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(અચલ અક્ષર *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       સંરચના _stati64
FILE *curlx_win32_fopen(અચલ અક્ષર *filename, અચલ અક્ષર *mode);
FILE *curlx_win32_freopen(અચલ અક્ષર *filename, અચલ અક્ષર *mode, FILE *fp);
પૂર્ણાંક curlx_win32_stat(અચલ અક્ષર *path, curlx_struct_stat *buffer);
પૂર્ણાંક curlx_win32_open(અચલ અક્ષર *filename, પૂર્ણાંક oflag, ...);
પૂર્ણાંક curlx_win32_rename(અચલ અક્ષર *oldpath, અચલ અક્ષર *newpath);
પૂર્ણાંક curlx_win32_mkdir(અચલ અક્ષર *path);
#define CURLX_FOPEN_નીચલું         curlx_win32_fopen
#define CURLX_FREOPEN_નીચલું       curlx_win32_freopen
#define CURLX_FDOPEN_નીચલું        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#નહીંતર
#define curlx_fstat             fstat
#define curlx_struct_stat       સંરચના stat
#define CURLX_FOPEN_નીચલું         fopen
#define CURLX_FREOPEN_નીચલું       freopen
#define CURLX_FDOPEN_નીચલું        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#જો defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#નહીંતર
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
#નહીંતર
#define curlx_fopen             CURLX_FOPEN_નીચલું
#define curlx_freopen           CURLX_FREOPEN_નીચલું
#define curlx_fdopen            CURLX_FDOPEN_નીચલું
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
