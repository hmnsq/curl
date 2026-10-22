#अगर_अघोषित HEADER_CURLX_FOPEN_H
#घोषणा HEADER_CURLX_FOPEN_H
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
#समावेश "curl_setup.h"

#समावेश "curlx/multibyte.h"

#अगर_घोषित HAVE_FCNTL_H
#समावेश <fcntl.h>  /* for open() and attributes */
#पूर्ण_अगर

पूर्णांक curlx_fseek(व्योम *stream, curl_off_t offset, पूर्णांक whence);

#अगर_घोषित _WIN32
#समावेश <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#अगर_अघोषित CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(स्थिर अक्षर *filename,
                        DWORD dwDesiredAccess,
                        DWORD dwShareMode,
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        DWORD dwCreationDisposition,
                        DWORD dwFlagsAndAttributes,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(स्थिर अक्षर *filename,
                           WIN32_FIND_तथ्य *find_तथ्य);
#पूर्ण_अगर /* !CURL_WINDOWS_UWP */
#घोषणा curlx_fstat             _fstati64
#घोषणा curlx_struct_stat       काष्ठा _stati64
खाता *curlx_win32_fopen(स्थिर अक्षर *filename, स्थिर अक्षर *mode);
खाता *curlx_win32_freopen(स्थिर अक्षर *filename, स्थिर अक्षर *mode, खाता *fp);
पूर्णांक curlx_win32_stat(स्थिर अक्षर *path, curlx_struct_stat *बफर);
पूर्णांक curlx_win32_open(स्थिर अक्षर *filename, पूर्णांक oflag, ...);
पूर्णांक curlx_win32_rename(स्थिर अक्षर *oldpath, स्थिर अक्षर *newpath);
पूर्णांक curlx_win32_mkdir(स्थिर अक्षर *path);
#घोषणा CURLX_FOPEN_निचला         curlx_win32_fopen
#घोषणा CURLX_FREOPEN_निचला       curlx_win32_freopen
#घोषणा CURLX_FDOPEN_निचला        _fdopen
#घोषणा curlx_stat              curlx_win32_stat
#घोषणा curlx_open              curlx_win32_open
#घोषणा curlx_close             _close
#घोषणा curlx_rename            curlx_win32_rename
#घोषणा curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#अन्यथा
#घोषणा curlx_fstat             ख_स्थिति
#घोषणा curlx_struct_stat       काष्ठा stat
#घोषणा CURLX_FOPEN_निचला         ख_खोलो
#घोषणा CURLX_FREOPEN_निचला       ख_व_खोलो
#घोषणा CURLX_FDOPEN_निचला        fdopen
#घोषणा curlx_stat              stat
#घोषणा curlx_open              खोलो
#घोषणा curlx_close             बंद
#घोषणा curlx_rename            नाम
#अगर defined(MSDOS) && !defined(__DJGPP__)
#घोषणा curlx_mkdir(x, y)       सूची_गढ़ो(x)
#अन्यथा
#घोषणा curlx_mkdir             सूची_गढ़ो
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित CURL_MEMDEBUG
#घोषणा curlx_fopen(file, mode) curl_dbg_fopen(file, mode, __LINE__, __खाता__)
#घोषणा curlx_freopen(file, mode, fh) \
  curl_dbg_freopen(file, mode, fh, __LINE__, __खाता__)
#घोषणा curlx_fdopen(file, mode) \
  curl_dbg_fdopen(file, mode, __LINE__, __खाता__)
#घोषणा curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __खाता__)
#अन्यथा
#घोषणा curlx_fopen             CURLX_FOPEN_निचला
#घोषणा curlx_freopen           CURLX_FREOPEN_निचला
#घोषणा curlx_fdopen            CURLX_FDOPEN_निचला
#घोषणा curlx_fclose            ख_बंद
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURLX_FOPEN_H */
