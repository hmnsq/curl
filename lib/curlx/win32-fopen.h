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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#அந்தர்பாவ <fcntl.h>  /* for open() and attributes */
#endif

முழு curlx_fseek(வெறுமை *stream, curl_off_t offset, முழு whence);

#ifdef _WIN32
#அந்தர்பாவ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(மாறிலி எழுத்து *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(மாறிலி எழுத்து *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       கட்டமைப்பு _stati64
FILE *curlx_win32_fopen(மாறிலி எழுத்து *filename, மாறிலி எழுத்து *mode);
FILE *curlx_win32_freopen(மாறிலி எழுத்து *filename, மாறிலி எழுத்து *mode, FILE *fp);
முழு curlx_win32_stat(மாறிலி எழுத்து *path, curlx_struct_stat *buffer);
முழு curlx_win32_open(மாறிலி எழுத்து *filename, முழு oflag, ...);
முழு curlx_win32_rename(மாறிலி எழுத்து *oldpath, மாறிலி எழுத்து *newpath);
முழு curlx_win32_mkdir(மாறிலி எழுத்து *path);
#define CURLX_FOPEN_கீழ்         curlx_win32_fopen
#define CURLX_FREOPEN_கீழ்       curlx_win32_freopen
#define CURLX_FDOPEN_கீழ்        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#இல்லையெனில்
#define curlx_fstat             fstat
#define curlx_struct_stat       கட்டமைப்பு stat
#define CURLX_FOPEN_கீழ்         fopen
#define CURLX_FREOPEN_கீழ்       freopen
#define CURLX_FDOPEN_கீழ்        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#எனில் defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#இல்லையெனில்
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
#இல்லையெனில்
#define curlx_fopen             CURLX_FOPEN_கீழ்
#define curlx_freopen           CURLX_FREOPEN_கீழ்
#define curlx_fdopen            CURLX_FDOPEN_கீழ்
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
