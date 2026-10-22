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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/multibyte.h"

#ifdef HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>  /* for open() and attributes */
#endif

पूर्णांक curlx_fseek(शून्य *stream, curl_off_t offset, पूर्णांक whence);

#ifdef _WIN32
#अन्तर्भाव <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ifndef CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(स्थिरांक अक्षर *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(स्थिरांक अक्षर *filename,
                           WIN32_FIND_DATA *find_data);
#endif /* !CURL_WINDOWS_UWP */
#define curlx_fstat             _fstati64
#define curlx_struct_stat       संरचना _stati64
खाता *curlx_win32_fopen(स्थिरांक अक्षर *filename, स्थिरांक अक्षर *mode);
खाता *curlx_win32_freopen(स्थिरांक अक्षर *filename, स्थिरांक अक्षर *mode, खाता *fp);
पूर्णांक curlx_win32_stat(स्थिरांक अक्षर *path, curlx_struct_stat *buffer);
पूर्णांक curlx_win32_open(स्थिरांक अक्षर *filename, पूर्णांक oflag, ...);
पूर्णांक curlx_win32_rename(स्थिरांक अक्षर *oldpath, स्थिरांक अक्षर *newpath);
पूर्णांक curlx_win32_mkdir(स्थिरांक अक्षर *path);
#define CURLX_FOPEN_खालचा         curlx_win32_fopen
#define CURLX_FREOPEN_खालचा       curlx_win32_freopen
#define CURLX_FDOPEN_खालचा        _fdopen
#define curlx_stat              curlx_win32_stat
#define curlx_open              curlx_win32_open
#define curlx_close             _close
#define curlx_rename            curlx_win32_rename
#define curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#अन्यथा
#define curlx_fstat             ख_स्थिति
#define curlx_struct_stat       संरचना stat
#define CURLX_FOPEN_खालचा         ख_खोलो
#define CURLX_FREOPEN_खालचा       ख_व_खोलो
#define CURLX_FDOPEN_खालचा        fdopen
#define curlx_stat              stat
#define curlx_open              खोलो
#define curlx_close             बंद
#define curlx_rename            नाम
#जर defined(MSDOS) && !defined(__DJGPP__)
#define curlx_mkdir(x, y)       सूची_गढ़ो(x)
#अन्यथा
#define curlx_mkdir             सूची_गढ़ो
#endif
#endif

#ifdef CURL_MEMDEBUG
#define curlx_fopen(file, mode) curl_dbg_fopen(file, mode, __LINE__, __FILE__)
#define curlx_freopen(file, mode, fh) \
  curl_dbg_freopen(file, mode, fh, __LINE__, __FILE__)
#define curlx_fdopen(file, mode) \
  curl_dbg_fdopen(file, mode, __LINE__, __FILE__)
#define curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#अन्यथा
#define curlx_fopen             CURLX_FOPEN_खालचा
#define curlx_freopen           CURLX_FREOPEN_खालचा
#define curlx_fdopen            CURLX_FDOPEN_खालचा
#define curlx_fclose            ख_बंद
#endif

#endif /* HEADER_CURLX_FOPEN_H */
