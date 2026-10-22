/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

#ಅನ್ತರ್ಭಾವ "curlx/version_win32.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
ರಚನೆ OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_ಗಾತ್ರ;
  ULONG  dw_Major_ಆವೃತ್ತಿ;
  ULONG  dw_Minor_ಆವೃತ್ತಿ;
  ULONG  dw_Build_ಸಂಖ್ಯೆ;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_ಮುಖವಾಡ;
  UCHAR  w_ಗುಣಲಬ್ಧ_Type;
  UCHAR  wReserved;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (ರಚನೆ OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
ಸ್ಥಿರ RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info;

ಶೂನ್ಯ curlx_verify_windows_init(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
  s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_ವಿಳಾಸ(GetModuleHandle(ಪಠ್ಯ_2("ntdll")), "RtlVerifyVersionInfo"));
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_WINDOWS_UWP */

/*
 * curlx_verify_windows_version()
 *
 * This is used to verify if we are running on a specific Windows version.
 *
 * Parameters:
 *
 * majorVersion [in] - The major version number.
 * minorVersion [in] - The minor version number.
 * buildVersion [in] - The build version number. If 0, this parameter is
 *                     ignored.
 * platform     [in] - The optional platform identifier.
 * condition    [in] - The test condition used to specifier whether we are
 *                     checking a version less than, equal to or greater than
 *                     what is specified in the major and minor version
 *                     numbers.
 *
 * Returns TRUE if matched; otherwise FALSE.
 */
bool curlx_verify_windows_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ build_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ PlatformIdentifier platform,
                                  ಸ್ಥಿರಾಂಕ ಆವೃತ್ತಿ_Condition condition)
{
  bool matched = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ಸ್ಥಿರಾಂಕ ಶಬ್ದ_2 ತುಂಬಿದ_ಆವೃತ್ತಿ = MAKEWORD(minor_ಆವೃತ್ತಿ, major_ಆವೃತ್ತಿ);
  ಸ್ಥಿರಾಂಕ ಶಬ್ದ_2 ಗುರಿ_ಆವೃತ್ತಿ = (ಶಬ್ದ_2)_WIN32_WINNT;

  (ಶೂನ್ಯ)build_ಆವೃತ್ತಿ;

  ಆಯ್ಕೆ(condition) {
  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_LESS_THAN:
    matched = ಗುರಿ_ಆವೃತ್ತಿ < ತುಂಬಿದ_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_LESS_THAN_EQUAL:
    matched = ಗುರಿ_ಆವೃತ್ತಿ <= ತುಂಬಿದ_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_EQUAL:
    matched = ಗುರಿ_ಆವೃತ್ತಿ == ತುಂಬಿದ_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_GREATER_THAN_EQUAL:
    matched = ಗುರಿ_ಆವೃತ್ತಿ >= ತುಂಬಿದ_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_GREATER_THAN:
    matched = ಗುರಿ_ಆವೃತ್ತಿ > ತುಂಬಿದ_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ULONGLONG cm = 0;
  ರಚನೆ OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_ಮುಖವಾಡ = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  ಆಯ್ಕೆ(condition) {
  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಆವೃತ್ತಿ_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ FALSE;
  }

  memset(&osver, 0, ಗಾತ್ರ(osver));
  osver.dw_OSVersion_Info_ಗಾತ್ರ = ಗಾತ್ರ(osver);
  osver.dw_Major_ಆವೃತ್ತಿ = major_ಆವೃತ್ತಿ;
  osver.dw_Minor_ಆವೃತ್ತಿ = minor_ಆವೃತ್ತಿ;
  osver.dw_Build_ಸಂಖ್ಯೆ = build_ಆವೃತ್ತಿ;
  ಆದರೆ(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  ಆದರೆ(platform != PLATFORM_DONT_CARE) {
    cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_ಮುಖವಾಡ |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  ಆದರೆ(s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info)
    matched = !s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info(&osver, dw_Type_ಮುಖವಾಡ, cm);
  ಇಲ್ಲದಿದ್ದರೆ
    matched = !!Verify_ಆವೃತ್ತಿ_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_ಮುಖವಾಡ, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  ಆದರೆ(matched && build_ಆವೃತ್ತಿ &&
     (condition == ಆವೃತ್ತಿ_EQUAL ||
      ((condition == ಆವೃತ್ತಿ_GREATER_THAN_EQUAL ||
        condition == ಆವೃತ್ತಿ_LESS_THAN_EQUAL) &&
        curlx_verify_windows_ಆವೃತ್ತಿ(major_ಆವೃತ್ತಿ, minor_ಆವೃತ್ತಿ, 0,
                                     platform, ಆವೃತ್ತಿ_EQUAL)))) {

    cm = Ver_ಗಣ_Condition_ಮುಖವಾಡ(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_ಮುಖವಾಡ = VER_BUILDNUMBER;
    ಆದರೆ(s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info)
      matched = !s_p_Rtl_Verify_ಆವೃತ್ತಿ_Info(&osver, dw_Type_ಮುಖವಾಡ, cm);
    ಇಲ್ಲದಿದ್ದರೆ
      matched = !!Verify_ಆವೃತ್ತಿ_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_ಮುಖವಾಡ, cm);
  }

#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ matched;
}

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
