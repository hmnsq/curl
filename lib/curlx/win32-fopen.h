#నిర్వచితం_కాకపోతే శీర్షిక_CURLX_FOPEN_H
#నిర్వచించు శీర్షిక_CURLX_FOPEN_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/multibyte.h"

#నిర్వచితమైతే HAVE_FCNTL_H
#అన్తర్భావ <fcntl.h>  /* for open() and attributes */
#అయితే_ముగింపు

పూర్ణాంకం curlx_fseek(శూన్య *ప్రవాహం, curl_off_t స్థానభ్రంశం, పూర్ణాంకం whence);

#నిర్వచితమైతే _WIN32
#అన్తర్భావ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#నిర్వచితం_కాకపోతే CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(స్థిరరాశి అక్షరం *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_బహుళకం,
                        LPSECURITY_లక్షణాలు lp_Security_లక్షణాలు,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_లక్షణాలు,
                        HANDLE hTemplateFile);
HANDLE curlx_కనుగొను_మొదటి_File(స్థిరరాశి అక్షరం *filename,
                           WIN32_కనుగొను_దత్తాంశం *కనుగొను_దత్తాంశం);
#అయితే_ముగింపు /* !CURL_WINDOWS_UWP */
#నిర్వచించు curlx_fstat             _fstati64
#నిర్వచించు curlx_struct_stat       నిర్మాణం _stati64
FILE *curlx_win32_fopen(స్థిరరాశి అక్షరం *filename, స్థిరరాశి అక్షరం *బహుళకం);
FILE *curlx_win32_freopen(స్థిరరాశి అక్షరం *filename, స్థిరరాశి అక్షరం *బహుళకం, FILE *fp);
పూర్ణాంకం curlx_win32_stat(స్థిరరాశి అక్షరం *మార్గం, curlx_struct_stat *బఫరు);
పూర్ణాంకం curlx_win32_open(స్థిరరాశి అక్షరం *filename, పూర్ణాంకం oflag, ...);
పూర్ణాంకం curlx_win32_rename(స్థిరరాశి అక్షరం *oldpath, స్థిరరాశి అక్షరం *newpath);
పూర్ణాంకం curlx_win32_mkdir(స్థిరరాశి అక్షరం *మార్గం);
#నిర్వచించు CURLX_FOPEN_దిగువ         curlx_win32_fopen
#నిర్వచించు CURLX_FREOPEN_దిగువ       curlx_win32_freopen
#నిర్వచించు CURLX_FDOPEN_దిగువ        _fdopen
#నిర్వచించు curlx_stat              curlx_win32_stat
#నిర్వచించు curlx_open              curlx_win32_open
#నిర్వచించు curlx_close             _close
#నిర్వచించు curlx_rename            curlx_win32_rename
#నిర్వచించు curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#లేకపోతే
#నిర్వచించు curlx_fstat             fstat
#నిర్వచించు curlx_struct_stat       నిర్మాణం stat
#నిర్వచించు CURLX_FOPEN_దిగువ         fopen
#నిర్వచించు CURLX_FREOPEN_దిగువ       freopen
#నిర్వచించు CURLX_FDOPEN_దిగువ        fdopen
#నిర్వచించు curlx_stat              stat
#నిర్వచించు curlx_open              open
#నిర్వచించు curlx_close             close
#నిర్వచించు curlx_rename            rename
#ఒకవేళ defined(MSDOS) && !defined(__DJGPP__)
#నిర్వచించు curlx_mkdir(x, y)       mkdir(x)
#లేకపోతే
#నిర్వచించు curlx_mkdir             mkdir
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే CURL_MEMDEBUG
#నిర్వచించు curlx_fopen(file, బహుళకం) curl_dbg_fopen(file, బహుళకం, __LINE__, __FILE__)
#నిర్వచించు curlx_freopen(file, బహుళకం, fh) \
  curl_dbg_freopen(file, బహుళకం, fh, __LINE__, __FILE__)
#నిర్వచించు curlx_fdopen(file, బహుళకం) \
  curl_dbg_fdopen(file, బహుళకం, __LINE__, __FILE__)
#నిర్వచించు curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#లేకపోతే
#నిర్వచించు curlx_fopen             CURLX_FOPEN_దిగువ
#నిర్వచించు curlx_freopen           CURLX_FREOPEN_దిగువ
#నిర్వచించు curlx_fdopen            CURLX_FDOPEN_దిగువ
#నిర్వచించు curlx_fclose            fclose
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURLX_FOPEN_H */
