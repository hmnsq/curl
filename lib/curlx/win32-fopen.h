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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#അന്തര്ഭാവ <fcntl.h>  /* for open() and attributes */
#endif

പൂർണ്ണസംഖ്യ curlx_fseek(ശൂന്യം *stream, curl_off_t offset, പൂർണ്ണസംഖ്യ whence);

#ifdef _WIN32
#അന്തര്ഭാവ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(സ്ഥിരം അക്ഷരം *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(സ്ഥിരം അക്ഷരം *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       ഘടന _stati64
FILE *curlx_win32_fopen(സ്ഥിരം അക്ഷരം *filename, സ്ഥിരം അക്ഷരം *mode);
FILE *curlx_win32_freopen(സ്ഥിരം അക്ഷരം *filename, സ്ഥിരം അക്ഷരം *mode, FILE *fp);
പൂർണ്ണസംഖ്യ curlx_win32_stat(സ്ഥിരം അക്ഷരം *path, curlx_struct_stat *buffer);
പൂർണ്ണസംഖ്യ curlx_win32_open(സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ oflag, ...);
പൂർണ്ണസംഖ്യ curlx_win32_rename(സ്ഥിരം അക്ഷരം *oldpath, സ്ഥിരം അക്ഷരം *newpath);
പൂർണ്ണസംഖ്യ curlx_win32_mkdir(സ്ഥിരം അക്ഷരം *path);
#define CURLX_FOPEN_താഴെയുള്ള         curlx_win32_fopen
#define CURLX_FREOPEN_താഴെയുള്ള       curlx_win32_freopen
#define CURLX_FDOPEN_താഴെയുള്ള        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#അല്ലെങ്കിൽ
#define curlx_fstat             fstat
#define curlx_struct_stat       ഘടന stat
#define CURLX_FOPEN_താഴെയുള്ള         fopen
#define CURLX_FREOPEN_താഴെയുള്ള       freopen
#define CURLX_FDOPEN_താഴെയുള്ള        fdopen
#define curlx_stat              stat
#define curlx_open              open
#define curlx_close             close
#define curlx_rename            rename
#എങ്കിൽ defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       mkdir(x)
#അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
#define curlx_fopen             CURLX_FOPEN_താഴെയുള്ള
#define curlx_freopen           CURLX_FREOPEN_താഴെയുള്ള
#define curlx_fdopen            CURLX_FDOPEN_താഴെയുള്ള
#define curlx_fclose            fclose
#endif

#endif /* HEADER_CURLX_FOPEN_H */
