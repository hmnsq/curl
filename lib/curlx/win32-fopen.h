#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURLX_FOPEN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURLX_FOPEN_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>  /* for open() and attributes */
#ਜੇ_ਅੰਤ

ਪੂਰਨ curlx_fseek(ਖਾਲੀ *ਧਾਰਾ, curl_off_t ਵਿੱਥ, ਪੂਰਨ whence);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ਅਚਲ ਅੱਖਰ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ਬਹੁਲਕ,
                        LPSECURITY_ਗੁਣ lp_Security_ਗੁਣ,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ਗੁਣ,
                        HANDLE hTemplateFile);
HANDLE curlx_ਲੱਭੋ_ਪਹਿਲਾ_File(ਅਚਲ ਅੱਖਰ *filename,
                           WIN32_ਲੱਭੋ_ਅੰਕੜੇ *ਲੱਭੋ_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ /* !CURL_WINDOWS_UWP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fstat             _fstati64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_struct_stat       ਬਣਤਰ _stati64
FILE *curlx_win32_fopen(ਅਚਲ ਅੱਖਰ *filename, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ);
FILE *curlx_win32_freopen(ਅਚਲ ਅੱਖਰ *filename, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE *fp);
ਪੂਰਨ curlx_win32_stat(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, curlx_struct_stat *ਬਫਰ);
ਪੂਰਨ curlx_win32_open(ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ oflag, ...);
ਪੂਰਨ curlx_win32_rename(ਅਚਲ ਅੱਖਰ *oldpath, ਅਚਲ ਅੱਖਰ *newpath);
ਪੂਰਨ curlx_win32_mkdir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FOPEN_ਨੀਵਾਂ         curlx_win32_fopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FREOPEN_ਨੀਵਾਂ       curlx_win32_freopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FDOPEN_ਨੀਵਾਂ        _fdopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_stat              curlx_win32_stat
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_open              curlx_win32_open
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_close             _close
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_rename            curlx_win32_rename
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fstat             fstat
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_struct_stat       ਬਣਤਰ stat
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FOPEN_ਨੀਵਾਂ         fopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FREOPEN_ਨੀਵਾਂ       freopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_FDOPEN_ਨੀਵਾਂ        fdopen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_stat              stat
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_open              open
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_close             close
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_rename            rename
#ਜੇ defined(MSDOS) && !defined(__DJGPP__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_mkdir(x, y)       mkdir(x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_mkdir             mkdir
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_MEMDEBUG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fopen(file, ਬਹੁਲਕ) curl_dbg_fopen(file, ਬਹੁਲਕ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_freopen(file, ਬਹੁਲਕ, fh) \
  curl_dbg_freopen(file, ਬਹੁਲਕ, fh, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fdopen(file, ਬਹੁਲਕ) \
  curl_dbg_fdopen(file, ਬਹੁਲਕ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fopen             CURLX_FOPEN_ਨੀਵਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_freopen           CURLX_FREOPEN_ਨੀਵਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fdopen            CURLX_FDOPEN_ਨੀਵਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_fclose            fclose
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURLX_FOPEN_H */
