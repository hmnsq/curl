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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/version_win32.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
ਬਣਤਰ OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_ਮਾਪ;
  ULONG  dw_Major_ਸੰਸਕਰਣ;
  ULONG  dw_Minor_ਸੰਸਕਰਣ;
  ULONG  dw_Build_ਸੰਖਿਆ;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_ਨਕਾਬ;
  UCHAR  w_ਗੁਣਨਫਲ_Type;
  UCHAR  wReserved;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (ਬਣਤਰ OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
ਸਥਿਰ RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info;

ਖਾਲੀ curlx_verify_windows_init(ਖਾਲੀ)
{
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
  s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_ਪਤਾ(GetModuleHandle(ਪਾਠ_2("ntdll")), "RtlVerifyVersionInfo"));
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ /* !CURL_WINDOWS_UWP */

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
bool curlx_verify_windows_ਸੰਸਕਰਣ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major_ਸੰਸਕਰਣ,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor_ਸੰਸਕਰਣ,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ build_ਸੰਸਕਰਣ,
                                  ਅਚਲ PlatformIdentifier platform,
                                  ਅਚਲ ਸੰਸਕਰਣ_Condition condition)
{
  bool matched = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ਅਚਲ ਸ਼ਬਦ_3 ਭਰਿਆ_ਸੰਸਕਰਣ = MAKEWORD(minor_ਸੰਸਕਰਣ, major_ਸੰਸਕਰਣ);
  ਅਚਲ ਸ਼ਬਦ_3 ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ = (ਸ਼ਬਦ_3)_WIN32_WINNT;

  (ਖਾਲੀ)build_ਸੰਸਕਰਣ;

  ਚੋਣ(condition) {
  ਹਾਲਤ ਸੰਸਕਰਣ_LESS_THAN:
    matched = ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ < ਭਰਿਆ_ਸੰਸਕਰਣ;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_LESS_THAN_EQUAL:
    matched = ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ <= ਭਰਿਆ_ਸੰਸਕਰਣ;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_EQUAL:
    matched = ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ == ਭਰਿਆ_ਸੰਸਕਰਣ;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_GREATER_THAN_EQUAL:
    matched = ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ >= ਭਰਿਆ_ਸੰਸਕਰਣ;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_GREATER_THAN:
    matched = ਨਿਸ਼ਾਨਾ_ਸੰਸਕਰਣ > ਭਰਿਆ_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  }

  ਜੇ(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#ਨਹੀਂਤਾਂ
  ULONGLONG cm = 0;
  ਬਣਤਰ OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_ਨਕਾਬ = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  ਚੋਣ(condition) {
  ਹਾਲਤ ਸੰਸਕਰਣ_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ਰੋਕੋ;

  ਹਾਲਤ ਸੰਸਕਰਣ_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ਰੋਕੋ;

  ਮੂਲ:
    ਵਾਪਸ FALSE;
  }

  memset(&osver, 0, ਆਕਾਰ(osver));
  osver.dw_OSVersion_Info_ਮਾਪ = ਆਕਾਰ(osver);
  osver.dw_Major_ਸੰਸਕਰਣ = major_ਸੰਸਕਰਣ;
  osver.dw_Minor_ਸੰਸਕਰਣ = minor_ਸੰਸਕਰਣ;
  osver.dw_Build_ਸੰਖਿਆ = build_ਸੰਸਕਰਣ;
  ਜੇ(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  ਨਹੀਂਤਾਂ ਜੇ(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  ਜੇ(platform != PLATFORM_DONT_CARE) {
    cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_ਨਕਾਬ |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  ਜੇ(s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info)
    matched = !s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info(&osver, dw_Type_ਨਕਾਬ, cm);
  ਨਹੀਂਤਾਂ
    matched = !!Verify_ਸੰਸਕਰਣ_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_ਨਕਾਬ, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  ਜੇ(matched && build_ਸੰਸਕਰਣ &&
     (condition == ਸੰਸਕਰਣ_EQUAL ||
      ((condition == ਸੰਸਕਰਣ_GREATER_THAN_EQUAL ||
        condition == ਸੰਸਕਰਣ_LESS_THAN_EQUAL) &&
        curlx_verify_windows_ਸੰਸਕਰਣ(major_ਸੰਸਕਰਣ, minor_ਸੰਸਕਰਣ, 0,
                                     platform, ਸੰਸਕਰਣ_EQUAL)))) {

    cm = Ver_ਸੈੱਟ_Condition_ਨਕਾਬ(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_ਨਕਾਬ = VER_BUILDNUMBER;
    ਜੇ(s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info)
      matched = !s_p_Rtl_Verify_ਸੰਸਕਰਣ_Info(&osver, dw_Type_ਨਕਾਬ, cm);
    ਨਹੀਂਤਾਂ
      matched = !!Verify_ਸੰਸਕਰਣ_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_ਨਕਾਬ, cm);
  }

#ਜੇ_ਅੰਤ

  ਵਾਪਸ matched;
}

#ਜੇ_ਅੰਤ /* _WIN32 */
