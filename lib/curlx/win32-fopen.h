#यदि_अपरिभाषितम् शीर्षकम्_CURLX_FOPEN_H
#परिभाषय शीर्षकम्_CURLX_FOPEN_H
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

#यदि_परिभाषितम् HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>  /* for open() and attributes */
#यदि_अन्तः

पूर्णाङ्क curlx_fseek(शून्य *प्रवाहः, curl_off_t विस्थापनम्, पूर्णाङ्क whence);

#यदि_परिभाषितम् _WIN32
#अन्तर्भाव <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#यदि_अपरिभाषितम् CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ध्रुव वर्ण *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_बहुलकम्,
                        LPSECURITY_गुणाः lp_Security_गुणाः,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_गुणाः,
                        HANDLE hTemplateFile);
HANDLE curlx_FindFirstFile(ध्रुव वर्ण *filename,
                           WIN32_FIND_दत्त *find_दत्त);
#यदि_अन्तः /* !CURL_WINDOWS_UWP */
#परिभाषय curlx_fstat             _fstati64
#परिभाषय curlx_struct_stat       संरचना _stati64
FILE *curlx_win32_fopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *बहुलकम्);
FILE *curlx_win32_freopen(ध्रुव वर्ण *filename, ध्रुव वर्ण *बहुलकम्, FILE *fp);
पूर्णाङ्क curlx_win32_stat(ध्रुव वर्ण *मार्गः, curlx_struct_stat *बफर);
पूर्णाङ्क curlx_win32_open(ध्रुव वर्ण *filename, पूर्णाङ्क oflag, ...);
पूर्णाङ्क curlx_win32_rename(ध्रुव वर्ण *oldpath, ध्रुव वर्ण *newpath);
पूर्णाङ्क curlx_win32_mkdir(ध्रुव वर्ण *मार्गः);
#परिभाषय CURLX_FOPEN_नीचः         curlx_win32_fopen
#परिभाषय CURLX_FREOPEN_नीचः       curlx_win32_freopen
#परिभाषय CURLX_FDOPEN_नीचः        _fdopen
#परिभाषय curlx_stat              curlx_win32_stat
#परिभाषय curlx_open              curlx_win32_open
#परिभाषय curlx_close             _close
#परिभाषय curlx_rename            curlx_win32_rename
#परिभाषय curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#अन्यथा
#परिभाषय curlx_fstat             fstat
#परिभाषय curlx_struct_stat       संरचना stat
#परिभाषय CURLX_FOPEN_नीचः         fopen
#परिभाषय CURLX_FREOPEN_नीचः       freopen
#परिभाषय CURLX_FDOPEN_नीचः        fdopen
#परिभाषय curlx_stat              stat
#परिभाषय curlx_open              उद्घाटय
#परिभाषय curlx_close             close
#परिभाषय curlx_rename            rename
#यदि defined(MSDOS) && !defined(__DJGPP__)
#परिभाषय curlx_mkdir(x, y)       mkdir(x)
#अन्यथा
#परिभाषय curlx_mkdir             mkdir
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् CURL_MEMDEBUG
#परिभाषय curlx_fopen(file, बहुलकम्) curl_dbg_fopen(file, बहुलकम्, __LINE__, __FILE__)
#परिभाषय curlx_freopen(file, बहुलकम्, fh) \
  curl_dbg_freopen(file, बहुलकम्, fh, __LINE__, __FILE__)
#परिभाषय curlx_fdopen(file, बहुलकम्) \
  curl_dbg_fdopen(file, बहुलकम्, __LINE__, __FILE__)
#परिभाषय curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#अन्यथा
#परिभाषय curlx_fopen             CURLX_FOPEN_नीचः
#परिभाषय curlx_freopen           CURLX_FREOPEN_नीचः
#परिभाषय curlx_fdopen            CURLX_FDOPEN_नीचः
#परिभाषय curlx_fclose            fclose
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURLX_FOPEN_H */
