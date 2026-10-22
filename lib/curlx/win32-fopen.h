#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURLX_FOPEN_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURLX_FOPEN_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>  /* for open() and attributes */
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ curlx_fseek(ಶೂನ್ಯ *ಹರಿವು, curl_off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ <sys/stat.h>  /* for _fstati64(), struct _stati64 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_WINDOWS_UWP
HANDLE curlx_CreateFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                        DWORD dwDesiredAccess,
                        DWORD dw_Share_ಬಹುಲಕ,
                        LPSECURITY_ಗುಣಲಕ್ಷಣಗಳು lp_Security_ಗುಣಲಕ್ಷಣಗಳು,
                        DWORD dwCreationDisposition,
                        DWORD dw_Flags_And_ಗುಣಲಕ್ಷಣಗಳು,
                        HANDLE hTemplateFile);
HANDLE curlx_ಕಂಡುಹಿಡಿ_ಮೊದಲ_File(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                           WIN32_ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ *ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_WINDOWS_UWP */
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fstat             _fstati64
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_struct_stat       ರಚನೆ _stati64
FILE *curlx_win32_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ);
FILE *curlx_win32_freopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE *fp);
ಪೂರ್ಣಾಂಕ curlx_win32_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, curlx_struct_stat *ಬಫರ್);
ಪೂರ್ಣಾಂಕ curlx_win32_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ oflag, ...);
ಪೂರ್ಣಾಂಕ curlx_win32_rename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath);
ಪೂರ್ಣಾಂಕ curlx_win32_mkdir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FOPEN_ಕೆಳಗಿನ         curlx_win32_fopen
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FREOPEN_ಕೆಳಗಿನ       curlx_win32_freopen
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FDOPEN_ಕೆಳಗಿನ        _fdopen
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_stat              curlx_win32_stat
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_open              curlx_win32_open
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_close             _close
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_rename            curlx_win32_rename
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_mkdir(x, y)       curlx_win32_mkdir(x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fstat             fstat
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_struct_stat       ರಚನೆ stat
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FOPEN_ಕೆಳಗಿನ         fopen
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FREOPEN_ಕೆಳಗಿನ       freopen
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_FDOPEN_ಕೆಳಗಿನ        fdopen
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_stat              stat
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_open              open
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_close             close
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_rename            rename
#ಆದರೆ defined(MSDOS) && !defined(__DJGPP__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_mkdir(x, y)       mkdir(x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_mkdir             mkdir
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MEMDEBUG
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fopen(file, ಬಹುಲಕ) curl_dbg_fopen(file, ಬಹುಲಕ, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_freopen(file, ಬಹುಲಕ, fh) \
  curl_dbg_freopen(file, ಬಹುಲಕ, fh, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fdopen(file, ಬಹುಲಕ) \
  curl_dbg_fdopen(file, ಬಹುಲಕ, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fclose(file)      curl_dbg_fclose(file, __LINE__, __FILE__)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fopen             CURLX_FOPEN_ಕೆಳಗಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_freopen           CURLX_FREOPEN_ಕೆಳಗಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fdopen            CURLX_FDOPEN_ಕೆಳಗಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_fclose            fclose
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURLX_FOPEN_H */
